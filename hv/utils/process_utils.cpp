#include "global_defs.h"
#include "process_utils.h"

namespace utils

{
	namespace process_utils
	{

		

		bool get_process_by_name(_In_ LPCWSTR process_name, _Out_ PEPROCESS* out_process)
		{
			if (!out_process)
			{
				return false;
			}

			ULONG64 max_handle = feature_data::g_psp_cid_table->NextHandleNeedingPool;
			for (ULONG64 raw_handle = 4; raw_handle < max_handle; raw_handle += 4)
			{
				HANDLE handle = reinterpret_cast<HANDLE>(raw_handle);
				PEPROCESS process = nullptr;

				if (!get_process_by_pid(handle, &process))
					continue;

				//获取 EPROCESS.SeAuditProcessCreationInfo.ImageFileName 判断进程名
				//// 找到名字匹配的进程就返回
				//if (utils::unicode_string_equals(process_name, process->ImageFileName))
				//{
				//	*out_process = process;
				//	return true;
				//}
			}

			return false;
		}

		bool get_process_by_pid(_In_ HANDLE pid, _Out_ PEPROCESS* out_process)
		{
			  
			if (!out_process)
			{
				return false;
			}

			auto handle_entry = exp_lookup_handle_table_entry(
				feature_data::g_psp_cid_table,
				pid
			);
			if (!handle_entry)
			{
				return false;
			}

			auto entry_va = reinterpret_cast<uintptr_t>(handle_entry);
			if (!memory::is_virtual_address_valid(entry_va))
			{
				return false;
			}

			uint64_t raw_value = handle_entry->Value;
			uint64_t process_address = 0;

			DWORD build = os_info::get_build_number();
			if (build < WINDOWS_10_VERSION_1507)
			{
				// 老版本：低 3 位为引用计数，屏蔽之后即为 EPROCESS 地址
				raw_value &= ~0x7ui64;
				process_address = raw_value;
			}
			else
			{
				// 新版本：高 48 位存地址，低 16 位为引用计数
				raw_value >>= 16;
				if (raw_value == 0)
				{
					return false;
				}
				process_address = raw_value | 0xFFFF'0000'0000'0000ui64;
			}

			if (!memory::is_virtual_address_valid(process_address))
			{
				return false;
			}

			auto process = reinterpret_cast<PEPROCESS>(process_address);
			if (is_process_exited(process))
			{
				return false;
			}

			 
			*out_process = process;
			return true;
		}

		bool is_process_exited(PEPROCESS process)
		{
			if (!process)
			{
				return false;
			}

			 
			if (!memory::is_virtual_address_valid(reinterpret_cast<uintptr_t>(process)))
			{
				return false;
			}

			const uintptr_t exit_time_addr = reinterpret_cast<uintptr_t>(process) + feature_offset::g_process_exit_time_offset;
			const auto exit_time_value = *reinterpret_cast<const uint64_t*>(exit_time_addr);

			return exit_time_value != 0;
		}

		PHANDLE_TABLE_ENTRY exp_lookup_handle_table_entry(PHANDLE_TABLE handle_table, HANDLE pid)
		{
			 
		 

			uint64_t handle_value = reinterpret_cast<uint64_t>(pid) & 0xFFFFFFFFFFFFFFFCui64;
			if (handle_value >= handle_table->NextHandleNeedingPool)
			{
				return nullptr;
			}


			uint64_t table_code = handle_table->TableCode;
			uint64_t table_level = table_code & 0x3;

		 
			 //二级句柄表
			if (table_level == 1)
			{
				uint64_t handle_array = *reinterpret_cast<uint64_t*>(table_code + 8 * (handle_value >> 10) - 1);
				return reinterpret_cast<PHANDLE_TABLE_ENTRY>(handle_array + 4 * (handle_value & 0x3FF));
			}

			else if (table_level ==2)
			{
				// 三级句柄表：两次解引用
				uint64_t first_level = *reinterpret_cast<uint64_t*>(table_code + 8 * (handle_value >> 19) - 2);
				uint64_t handle_array = *reinterpret_cast<uint64_t*>(first_level + 8 * ((handle_value >> 10) & 0x1FF));
				return reinterpret_cast<PHANDLE_TABLE_ENTRY>(handle_array + 4 * (handle_value & 0x3FF));
			}
			 

			//一级句柄表
			return reinterpret_cast<PHANDLE_TABLE_ENTRY>(table_code + 4 * handle_value);
		}

	}
}