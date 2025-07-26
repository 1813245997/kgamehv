#include "global_defs.h"
#include "process_utils.h"

namespace utils

{
	namespace process_utils
	{

		

		bool get_process_by_name(_In_ LPCWSTR target_name, _Out_ PEPROCESS* out_process)
		{
			if (!target_name)
			{
				return false;
			}

			if (!out_process)
			{
				return false;
			}

			*out_process = nullptr;

			UNICODE_STRING target_unicode_name{};
			internal_functions::pfn_rtl_init_unicode_string(&target_unicode_name, target_name);

		 

			for (ULONG64 pid_value = 4; pid_value < 0x10000; pid_value += 4)
			{
				HANDLE possible_pid = reinterpret_cast<HANDLE>(pid_value);
				PEPROCESS process = nullptr;

				if (!NT_SUCCESS(internal_functions::pfn_ps_lookup_process_by_process_id(possible_pid, &process)))
				{
					continue;
				}
				 

				PUNICODE_STRING process_name = nullptr;
				if (!get_process_name(process, &process_name) )
				{
					internal_functions::pfn_ob_dereference_object(process);
					continue;
				}
				

				if (!utils::string_utils::compare_unicode_strings(process_name, &target_unicode_name, TRUE))
				{
					if (process_name->Buffer)
						internal_functions::pfn_ex_free_pool_with_tag(process_name->Buffer,0);
					internal_functions::pfn_ex_free_pool_with_tag(process_name,0);
					internal_functions::pfn_ob_dereference_object(process);
					continue;
				}

				if (process_name->Buffer)
					internal_functions::pfn_ex_free_pool_with_tag(process_name->Buffer,0);
				internal_functions::pfn_ex_free_pool_with_tag(process_name,0);

				*out_process = process;
				

				return true;
			 
				
			}

			return false;
		}


		bool get_process_by_pid(_In_ HANDLE pid, _Out_ PEPROCESS* out_process)
		{
			*out_process = nullptr;
			if (!out_process)
			{
				return false;
			}
			
			auto handle_entry =   exp_lookup_handle_table_entry(
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

			uint64_t raw_value = *(ULONG64*)handle_entry;
			//uint64_t raw_value = handle_entry->ObjectPointerBits;
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
			 
				raw_value = raw_value >> 0x10;
			 
				if (raw_value == 0)
				{
					return false;
				}
				process_address = raw_value | 0xFFFF000000000000;
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
		bool get_process_full_name(_In_ PEPROCESS process, _Out_ PUNICODE_STRING* process_name)
		{
			if (!process)
			{
				return false;
			}
			if (!process_name)
			{
				return false;
			}

		  
			NTSTATUS status = internal_functions::pfn_se_locate_process_image_name(process, process_name);
			return NT_SUCCESS(status) && *process_name != nullptr;
		}

		bool get_process_name_by_pid(_In_ HANDLE pid, _Inout_ PUNICODE_STRING* process_name)
		{
			if (!pid || !process_name)
			{
				return false;
			}

			PEPROCESS process = nullptr;
			NTSTATUS status = utils::internal_functions::pfn_ps_lookup_process_by_process_id (pid, &process);
			if (!NT_SUCCESS(status) || !process)
			{
				return false;
			}

			bool result = get_process_name(process, process_name);
			utils::internal_functions::pfn_ob_dereference_object(process);
			 
			return result;
		}

		bool get_process_name(_In_ PEPROCESS process,_Inout_ PUNICODE_STRING* process_name)
		{
			if (!process)
			{
				return false;
			}
			if (!process_name)
			{
				return false;
			}

			if (!internal_functions::pfn_se_locate_process_image_name)
			{
				return false;
			}

			PUNICODE_STRING full_image_name = nullptr;
			NTSTATUS status = internal_functions::pfn_se_locate_process_image_name(process, &full_image_name);

			if (!NT_SUCCESS(status))
			{
				return false;
			}
		
			if (!full_image_name)
			{
				return false;
			}

			if (!full_image_name->Buffer)
			{
				return false;
			}
			 


			// 查找最后一个反斜杠
			USHORT length = full_image_name->Length / sizeof(WCHAR);
			WCHAR* buffer = full_image_name->Buffer;
			USHORT last_backslash_index = 0;

			for (USHORT i = 0; i < length; ++i)
			{
				if (buffer[i] == L'\\')
				{
					last_backslash_index = i + 1;
				}
			}

			USHORT name_length = (length - last_backslash_index) * sizeof(WCHAR);
			UNICODE_STRING name_only{};
			name_only.Length = static_cast<USHORT>(name_length);
			name_only.MaximumLength = static_cast<USHORT>(name_length + sizeof(WCHAR));
			name_only.Buffer = static_cast<PWSTR>(internal_functions::pfn_ex_allocate_pool_with_tag(PagedPool, name_only.MaximumLength, 'prcN'));

			if (!name_only.Buffer)
			{
				ExFreePool(full_image_name);
				return false;
			}
			
			RtlCopyMemory(name_only.Buffer, &buffer[last_backslash_index], name_length);
			name_only.Buffer[name_only.Length / sizeof(WCHAR)] = L'\0';

			*process_name = static_cast<PUNICODE_STRING>(internal_functions::pfn_ex_allocate_pool_with_tag (PagedPool, sizeof(UNICODE_STRING), 'prcS'));
			if (!*process_name)
			{
				internal_functions::pfn_ex_free_pool_with_tag(name_only.Buffer,0);
				internal_functions::pfn_ex_free_pool_with_tag(full_image_name,0);
				return false;
			}

			**process_name = name_only;

			internal_functions::pfn_ex_free_pool_with_tag(full_image_name,0);
			return true;
		}

		bool is_process_exited(_In_ PEPROCESS process)
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

		bool is_process_64_bit(_In_ PEPROCESS process)
		{
			if (get_process_peb32_process(process))
			{
				return false;
			}
			return true;
		}

		bool is_process_name_match(_In_ PEPROCESS process, _In_ PUNICODE_STRING target_name, _In_ BOOLEAN case_insensitive)
		{
			bool result = false;

			if (process == nullptr)
			{
				return false;
			}

			if (target_name == nullptr)
			{
				return result;
			}

			if (target_name->Buffer == nullptr)
			{
				return  result;
			}

			if (target_name->Length == 0)
			{
				return  result;
			}

			PUNICODE_STRING process_name = nullptr;

			if (!get_process_name(process, &process_name))
			{
				return false;
			}

			
			if (internal_functions::pfn_rtl_equal_unicode_string)
			{
				result = internal_functions::pfn_rtl_equal_unicode_string(process_name, target_name, case_insensitive);
			}

			// 清理分配的内存
			if (process_name)
			{
				if (process_name->Buffer)
					internal_functions::pfn_ex_free_pool_with_tag(process_name->Buffer, 0);

				internal_functions::pfn_ex_free_pool_with_tag(process_name, 0);
			}

			return result;
		}

		bool is_process_name_match_wstr_by_pid(_In_ HANDLE pid, _In_ PWCHAR target_name_wstr, _In_ BOOLEAN case_insensitive)
		{
			if (target_name_wstr == nullptr)
			{
				return false;
			}

			UNICODE_STRING target_name_unicode;
			RtlInitUnicodeString(&target_name_unicode, target_name_wstr);

			PEPROCESS process = nullptr;
			NTSTATUS status =  utils::internal_functions::pfn_ps_lookup_process_by_process_id (pid, &process);
			if (!NT_SUCCESS(status) || process == nullptr)
			{
				return false;
			}

			BOOLEAN result = is_process_name_match(process, &target_name_unicode, case_insensitive);

			utils::internal_functions::pfn_ob_dereference_object(process);  
			return result;
		}
		bool is_process_name_match_wstr(_In_ PEPROCESS process, _In_ PWCHAR target_name_wstr, _In_ BOOLEAN case_insensitive)
		{
			if (target_name_wstr == nullptr)
			{
				return false;
			}

			UNICODE_STRING target_name_unicode;
			RtlInitUnicodeString(&target_name_unicode, target_name_wstr);

			return is_process_name_match(process, &target_name_unicode, case_insensitive);
		}

		PVOID get_process_peb32_process(_In_ PEPROCESS process)
		{
			if (!process)
			{
				return false;
			}

			const uintptr_t process_wow64_process_addr = reinterpret_cast<uintptr_t>(process) + feature_offset::g_process_wow64_process_offset;
			auto process_wow64_process_value = *reinterpret_cast<const PULONG_PTR*>(process_wow64_process_addr);
			return process_wow64_process_value;
		}

		PVOID get_process_peb64_process(_In_ PEPROCESS process)
		{
			if (!process)
			{
				return false;
			}

			 
			const uintptr_t process_peb64_address =
				reinterpret_cast<uintptr_t>(process) + feature_offset::g_process_peb_offset;

		 
			const ULONG_PTR peb64_value =
				*reinterpret_cast<const ULONG_PTR*>(process_peb64_address);

		 
			return reinterpret_cast<PVOID>(peb64_value);
		}

		PHANDLE_TABLE_ENTRY exp_lookup_handle_table_entry(_In_ PHANDLE_TABLE handle_table, _In_ HANDLE pid)
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

		     if (table_level !=0)
			{
				// 三级句柄表：两次解引用
				uint64_t first_level = *reinterpret_cast<uint64_t*>(table_code + 8 * (handle_value >> 19) - 2);
				uint64_t handle_array = *reinterpret_cast<uint64_t*>(first_level + 8 * ((handle_value >> 10) & 0x1FF));
				return reinterpret_cast<PHANDLE_TABLE_ENTRY>(handle_array + 4 * (handle_value & 0x3FF));
			}
			 

			//一级句柄表
			return reinterpret_cast<PHANDLE_TABLE_ENTRY>(table_code + 4 * handle_value);
		}


		unsigned long long get_process_cr3(_In_ PEPROCESS process)
		{
			if (!process)
				return 0;
			 
			constexpr SIZE_T offset_directory_table_base = 0x28;

			PUCHAR process_base = reinterpret_cast<PUCHAR>(process);
			return *reinterpret_cast<ULONGLONG*>(process_base + offset_directory_table_base);
		}
	}
}