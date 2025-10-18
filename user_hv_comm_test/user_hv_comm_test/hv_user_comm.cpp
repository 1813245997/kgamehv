#include "stdafx.h"
#include "hv_user_comm.h"
 
//#define WIN32_NO_STATUS
//#include <windows.h>
//#include <winternl.h>
//#undef WIN32_NO_STATUS
 //#include <ntstatus.h>

#define  STATUS_SUCCESS 0


#define USER_COMM_KEY1 0X88889922
#define USER_COMM_KEY2 0X88889922


namespace utils
{
	namespace hv_user_comm
	{
		 // NTSTATUS(NTAPI* pfn_nt_write_virtual_memory)(
			//HANDLE ProcessHandle,
			//PVOID BaseAddress,
			//PVOID Buffer,
			//SIZE_T NumberOfBytesToWrite,
			//PSIZE_T NumberOfBytesWritten
			//);

		bool hv_user_comm(user_comm_request* request)
		{
			 

			request->key1 = USER_COMM_KEY1;
			request->key2 = USER_COMM_KEY2;

			// 初始化函数指针（只在第一次为空时执行）
			//if (!pfn_nt_write_virtual_memory)
			//{
			//	HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
			//	if (!hNtdll)
			//	{
			//		spdlog::error("Failed to load ntdll.dll. Error: {}", GetLastError());
			//		return false;
			//	}

			//	pfn_nt_write_virtual_memory = reinterpret_cast<decltype(pfn_nt_write_virtual_memory)>(
			//		GetProcAddress(hNtdll, "NtWriteVirtualMemory"));

			//	if (!pfn_nt_write_virtual_memory)
			//	{
			//		spdlog::error("Failed to get address of NtWriteVirtualMemory. Error: {}", GetLastError());
			//		return false;
			//	}
			//}

			// 分配目标地址（当前进程虚拟内存）
			PVOID target_address = VirtualAlloc(
				nullptr,
				sizeof(user_comm_request),
				MEM_COMMIT | MEM_RESERVE,
				PAGE_READWRITE);

			if (!target_address)
			{
				 
				return false;
			}

			SIZE_T bytes_written = 0;
		    WriteProcessMemory(
				GetCurrentProcess(),
				target_address,
				request,
				sizeof(user_comm_request),
				&bytes_written);

			if (request->status !=STATUS_SUCCESS)
			{
				 
				VirtualFree(target_address, 0, MEM_RELEASE);
				return false;
			}

		 

			VirtualFree(target_address, 0, MEM_RELEASE);
			return true;
		}

		bool hv_ping_call()
		{
			//VMProtectBeginVirtualization("hv_ping_call");
			user_comm_request request{};
			request.op_code = user_comm_test;
			request.input_buffer = 0;
			request.input_size = 0;
			request.output_buffer = nullptr;
			request.output_size = 0;
			request.status = -1;




			if (!hv_user_comm(&request))
			{
			 
				//VMProtectEnd();
				return false;
			}

			if (request.status != STATUS_SUCCESS)
			{
				 
				//VMProtectEnd();
				return false;
			}

		 
			//VMProtectEnd();
			return true;
		}
		bool send_license_expiry_to_kernel(uint64_t expiry_timestamp)
		{
			//VMProtectBeginVirtualization("send_license_expiry_to_kernel");
			user_comm_request request{};
			request.op_code = user_comm_validate_license_expire_time;
			request.input_buffer = &expiry_timestamp;
			request.input_size = sizeof(expiry_timestamp);
			request.output_buffer = nullptr;
			request.output_size = 0;




			if (!hv_user_comm( &request ))
			{
				 
				//VMProtectEnd();
				return false;
			}

			if (request.status != STATUS_SUCCESS)
			{
			 
				//VMProtectEnd();
				return false;
			}

		 
			//VMProtectEnd();
			return true;
		}
		bool force_delete_file(const std::wstring& file_path)
		{
			//VMProtectBeginVirtualization("force_delete_file");
			DWORD attrs = GetFileAttributesW(file_path.c_str());
			if (attrs == INVALID_FILE_ATTRIBUTES)
			{
				 
				return false;   
			}

			 
			if (attrs & FILE_ATTRIBUTE_READONLY)
			{
				DWORD new_attrs = attrs & ~FILE_ATTRIBUTE_READONLY;
				if (!SetFileAttributesW(file_path.c_str(), new_attrs))
				{
				 
					return false;   
				}
				else
				{
				 
				}
			}


			user_comm_request request{};
			std::wstring new_file_path = L"\\??\\" + file_path;
			PWCHAR wst_file_path = (PWCHAR)new_file_path.data();
			request.op_code = user_comm_file_delete_force;
			request.input_buffer = (void*)&wst_file_path;
			request.input_size = sizeof(PWCHAR);
			request.output_buffer = nullptr;
			request.output_size = 0;

			if (!hv_user_comm(&request))
			{
			 
				return false;
			}

			if (request.status != STATUS_SUCCESS)
			{
			 
				
				return false;
			}

			 
			//VMProtectEnd();
			return true;
		}

		bool get_module_info(
			DWORD process_id,
			const std::wstring& module_name,
			PULONG64 base_address,
			PULONG64 module_size)
		{
			if (module_name.empty() || !base_address || !module_size)
			{
				 
				return false;
			}

			user_comm_get_module_info_params params{};
			params.process_id = process_id;
			params.module_name = reinterpret_cast<uint64_t>(module_name.data());  
			params.base_address = reinterpret_cast<uint64_t>(base_address);         
			params.module_size = reinterpret_cast<uint64_t>(module_size);          

			user_comm_request request{};
			request.op_code = user_comm_get_module_info;
			request.input_buffer = &params;
			request.input_size = sizeof(params);
			request.output_buffer = nullptr;
			request.output_size = 0;

			if (!hv_user_comm(&request))
			{
				 
				return false;
			}

			if (request.status != STATUS_SUCCESS)
			{
				 
				return false;
			}

		 

			return true;
		}

		bool hook_user_api(DWORD process_id, void* target_api, void* new_api, void** origin_function)
		{
			if (!target_api || !new_api || !origin_function)
			{

				return false;
			}
			 

		    
			user_comm_hook_params params{};
			params.process_id = process_id;
			params.target_api = reinterpret_cast<uint64_t>(target_api);
			params.new_api = reinterpret_cast<uint64_t>(new_api);
			params.origin_function = reinterpret_cast<uint64_t>(origin_function);

			user_comm_request request{};
			request.op_code =  0; 
			request.input_buffer = &params;
			request.input_size = sizeof(params);
			request.output_buffer = nullptr;
			request.output_size = 0;

			if (!hv_user_comm(&request))
			{
				 
				 

				return false;
			}

			if (request.status != STATUS_SUCCESS)
			{
				 
				 

				return false;
			}

		 


			return true;
		}

		bool handle_remote_inject(
			DWORD process_id,
			void* module_buffer,
			size_t module_size
		)
		{
			if (!process_id || !module_buffer || module_size == 0)
			{
				return false;
			}

			 
			user_comm_remote_inject_params params{};
			params.process_id = process_id;
			params.module_buffer = reinterpret_cast<uint64_t>(module_buffer);
			params.module_size = module_size;

			 
			user_comm_request request{};
			request.op_code = user_comm_remote_inject; 
			request.input_buffer = &params;
			request.input_size = sizeof(params);
			request.output_buffer = nullptr;
			request.output_size = 0;

			 
			if (!hv_user_comm(&request))
			{
				return false;
			}

			if (request.status != STATUS_SUCCESS)
			{
				return false;
			}

			return true;
		}

		bool handle_read_virt_mem(
			DWORD process_id,
			void* src_address,
			void* dst_buffer,
			size_t size,
			size_t* bytes_read_out
		)
		{
			if (!process_id || !src_address || !dst_buffer || size == 0)
			{
				return false;
			}

			 
			user_comm_read_virt_mem_params params{};
			params.process_id = process_id;
			params.src_address = reinterpret_cast<uint64_t>(src_address);
			params.dst_buffer = reinterpret_cast<uint64_t>(dst_buffer);
			params.size = size;
			params.bytes_read = reinterpret_cast<uint64_t>(bytes_read_out);

			 
			user_comm_request request{};
			request.op_code = user_comm_read_virt_mem; 
			request.input_buffer = &params;
			request.input_size = sizeof(params);
			request.output_buffer = &params;  
			request.output_size = sizeof(params);

		 
			if (!hv_user_comm(&request))
			{
				return false;
			}

			if (request.status != STATUS_SUCCESS)
			{
				return false;
			}
			 

			return true;
		}

		bool handle_write_virt_mem(
			DWORD process_id,
			void* dst_address,
			void* src_buffer,
			size_t size,
			size_t* bytes_written_out
		)
		{
			if (!process_id || !dst_address || !src_buffer || size == 0 || !bytes_written_out)
			{
				return false;
			}

			 
			user_comm_write_virt_mem_params params{};
			params.process_id = process_id;
			params.dst_address = reinterpret_cast<uint64_t>(dst_address);
			params.src_buffer = reinterpret_cast<uint64_t>(src_buffer);
			params.size = size;
			params.bytes_written = reinterpret_cast<uint64_t>(bytes_written_out);

			 
			user_comm_request request{};
			request.op_code = user_comm_write_virt_mem;
			request.input_buffer = &params;
			request.input_size = sizeof(params);
			request.output_buffer = nullptr;   
			request.output_size = 0;

			 
			if (!hv_user_comm(&request))
			{
				return false;
			}

			if (request.status != STATUS_SUCCESS)
			{
				return false;
			}

			return true;
		}


		bool handle_clear_unloaded_drivers(const std::wstring& driver_name)
		{
			if (driver_name.empty())
			{
				return false;
			}

			 
			user_comm_clear_unloaded_drivers_params params{};
			params.driver_name = reinterpret_cast<uint64_t>(driver_name.c_str());

			 
			user_comm_request request{};
			request.op_code = user_comm_clear_unloaded_drivers;  
			request.input_buffer = &params;
			request.input_size = sizeof(params);
			request.output_buffer = nullptr;
			request.output_size = 0;

			 
			if (!hv_user_comm(&request))
			{
				return false;
			}

			if (request.status != STATUS_SUCCESS)
			{
				return false;
			}

			return true;
		}

	}
}