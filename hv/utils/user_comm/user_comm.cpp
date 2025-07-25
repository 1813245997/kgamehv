#include "../global_defs.h"
#include "user_comm.h"
 

#define USER_COMM_KEY1 0X88889922
#define USER_COMM_KEY2 0X88889922
namespace utils
{
	namespace user_comm
	{
		  
		 
		 
		bool handle_user_comm_request(user_comm_request* request)
		{
		
			bool is_succeed = false;
			 
		 
		 

			switch (request->op_code)
			{
			case user_comm_test:
			{
				is_succeed = handle_test(request);

				break;
			}
				

			case user_comm_validate_license_expire_time:
			{
				 is_succeed = handle_validate_license_expire_time(request);
				 break;
			}

			case  user_comm_file_delete_force:
			{
				is_succeed = handle_force_delete_file(request);
			}

			default:

				request->status = static_cast<uint64_t>(-1);  // 未知操作
				 
				 
			}

			return is_succeed;
		}

		bool is_valid_comm(user_comm_request* request)
		{
			if (!request)
			{
				return false;
			}

			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(request))
			{
				return false;
			}

			if (request->key1!=USER_COMM_KEY1 ||request->key2!=USER_COMM_KEY2)
			{
				return false;
			}

			return true;
		}

		bool handle_test(user_comm_request* request)
		{
			request->status = 0;  // 表示成功
			return true;
		}

		bool handle_validate_license_expire_time(user_comm_request* request)
		{

			if (!request->input_buffer || request->input_size < sizeof(uint64_t))
			{
				request->status = STATUS_INVALID_PARAMETER;  // 输入无效
				return false;
			}

			uint64_t expire_time = *reinterpret_cast<uint64_t*>(request->input_buffer);
			auth::set_license_expire_timestamp(expire_time);
			
			request->status = STATUS_SUCCESS;
			return true;
		}

		bool handle_force_delete_file(user_comm_request* request)
		{

			if (!request->input_buffer || request->input_size < sizeof(uint64_t))
			{
				request->status = STATUS_INVALID_PARAMETER;
				return false;
			}

			NTSTATUS status = STATUS_UNSUCCESSFUL;
			PWCHAR file_path = *reinterpret_cast<PWCHAR*>(request->input_buffer);

			if (!file_path || !file_path[0])
			{
				request->status = STATUS_INVALID_PARAMETER;
				return false;
			}

			const WCHAR* prefix = L"\\??\\";
			size_t prefix_len = wcslen(prefix);
			size_t path_len = wcslen(file_path);
			size_t total_len = prefix_len + path_len + 1; // +1 for null terminator

			if (total_len > 0xFFFF) // 过长，防止异常
			{
				request->status = STATUS_NAME_TOO_LONG;
				return false;
			}

		 
			PWCHAR full_path =  reinterpret_cast<PWCHAR> (utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, total_len * sizeof(WCHAR), 'dlfF'));
			if (!full_path)
			{
				request->status = STATUS_INSUFFICIENT_RESOURCES;
				return false;
			}

		 
			RtlZeroMemory(full_path, total_len * sizeof(WCHAR));
			wcscpy(full_path, prefix);
			wcscat(full_path, file_path);
		 
			UNICODE_STRING uni_path;
			RtlInitUnicodeString(&uni_path, full_path);

		 
			status = utils::file_utils::force_delete_file(&uni_path);

			// 释放内存
			ExFreePool(full_path);

			if (!NT_SUCCESS(status))
			{
				request->status = status;
				 
				return false;
			}

			request->status = STATUS_SUCCESS;
		 
		 
			return true;
		}
	}
}
