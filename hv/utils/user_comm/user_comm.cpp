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
				break;
			}

			case  user_comm_get_module_info:
			{
				 
				is_succeed = handle_get_module_info(request);
				break;
				 
			}

			case  user_comm_user_hook:
			{
			 
				is_succeed = handle_hook_user_api(request);
				break;
			}

			case  user_comm_remote_inject:
			{
				is_succeed = handle_remote_inject(request);
				break;
			}

			case  user_comm_clear_unloaded_drivers:
			{

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

			// 计算路径长度（字符数 + 1）
			size_t path_len = wcslen(file_path) + 1;

			// 申请非分页内存
			PWCHAR safe_path = (PWCHAR)utils::internal_functions::pfn_ex_allocate_pool_with_tag(
				NonPagedPool,
				path_len * sizeof(WCHAR),
				'dlfF' // 自定义TAG，保持你代码风格
			);

			if (!safe_path)
			{
				request->status = STATUS_INSUFFICIENT_RESOURCES;
				return false;
			}

			// 拷贝路径字符串到安全内存
			RtlCopyMemory(safe_path, file_path, path_len * sizeof(WCHAR));

			// 初始化UNICODE_STRING
			UNICODE_STRING uni_path;
			RtlInitUnicodeString(&uni_path, safe_path);

			// 调用删除文件
			status = utils::file_utils::force_delete_file(&uni_path);

			// 释放池内存
			ExFreePool(safe_path);

			if (!NT_SUCCESS(status))
			{
				request->status = status;
				return false;
			}

			request->status = STATUS_SUCCESS;
			return true;

		 
		   
		}


		bool handle_get_module_info(user_comm_request* request)
		{
			if (!request)
			{
				return false;
			}

			if (!request->input_buffer)
			{
				return false;
			}

			p_user_comm_get_module_info_params params =reinterpret_cast<p_user_comm_get_module_info_params>(request->input_buffer);

			HANDLE process_id = reinterpret_cast<HANDLE> (params->process_id);
			PWCHAR module_name = reinterpret_cast<PWCHAR> (params->module_name);   // UTF-16 字符串地址
			PULONG64 base_address = reinterpret_cast<PULONG64> (params->base_address);  // 输出参数指针
			PULONG64 module_size = reinterpret_cast<PULONG64> (params->module_size);   // 输出参数指针

			if (wcslen(module_name) == 0)
			{
				return STATUS_INVALID_PARAMETER;
			}

			*base_address = 0;
			*module_size = 0;


			PEPROCESS process = nullptr;
			NTSTATUS status = utils::internal_functions::pfn_ps_lookup_process_by_process_id (process_id, &process);
			if (!NT_SUCCESS(status))
			{
				return status; // Return the status indicating failure to lookup the process
			}

			if (utils::internal_functions::pfn_ps_get_process_exit_status(process) != STATUS_PENDING)
			{
				utils::internal_functions::pfn_ob_dereference_object(process);
				return STATUS_PROCESS_IS_TERMINATING; // Return status indicating the process is terminating
			}

	   
			// --------------------------
			// 拷贝字符串到新内存
			// --------------------------
			size_t name_len = (wcslen(module_name) + 1) * sizeof(WCHAR);
			PWCHAR kernel_module_name = (PWCHAR)ExAllocatePoolWithTag(NonPagedPool, name_len, 'modN');
			if (!kernel_module_name)
			{
				utils::internal_functions::pfn_ob_dereference_object(process);
				return STATUS_INSUFFICIENT_RESOURCES;
			}

			RtlZeroMemory(kernel_module_name, name_len);
			RtlCopyMemory(kernel_module_name, module_name, name_len);

			// --------------------------
			// 调用 get_process_module_info
			// --------------------------
			status = utils::module_info:: get_process_module_info(
				process,
				kernel_module_name,
				base_address,
				reinterpret_cast<SIZE_T*>(module_size)
			);

			// --------------------------
			// 释放内存
			// --------------------------
			ExFreePoolWithTag(kernel_module_name, 'modN');
			utils::internal_functions::pfn_ob_dereference_object(process);
			return status;


			 
		}

		bool handle_hook_user_api(user_comm_request* request)
		{
			if (!request || !request->input_buffer)
			{
				return false;
			}
		 
			p_user_comm_hook_params params =
				reinterpret_cast<p_user_comm_hook_params>(request->input_buffer);

			HANDLE process_id = reinterpret_cast<HANDLE>(params->process_id);
			void* target_api = reinterpret_cast<void*>(params->target_api);
			void* new_api = reinterpret_cast<void*>(params->new_api);
			void** origin_function = reinterpret_cast<void**>(params->origin_function);

			PEPROCESS process = nullptr;
			NTSTATUS status = utils::internal_functions::pfn_ps_lookup_process_by_process_id(process_id, &process);
			if (!NT_SUCCESS(status))
			{
				
				request->status = status;
				return false;
			}

			if (utils::internal_functions::pfn_ps_get_process_exit_status(process) != STATUS_PENDING)
			{
				utils::internal_functions::pfn_ob_dereference_object(process);
				request->status = STATUS_PROCESS_IS_TERMINATING;
				return false;
			}

			bool hook_result = utils::hook_utils::hook_user_hook_handler(
				process,
				target_api,
				new_api,
				origin_function
			);
			utils::internal_functions::pfn_ob_dereference_object(process);
			request->status = hook_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
			return hook_result;


		}

		bool handle_remote_inject(user_comm_request* request)
		{

			if (!request || !request->input_buffer)
			{
				return false;
			}

			p_user_comm_remote_inject_params params =
				reinterpret_cast<p_user_comm_remote_inject_params>(request->input_buffer);

			HANDLE process_id = reinterpret_cast<HANDLE>(params->process_id);
			void* module_buffer = reinterpret_cast<void*>(params->module_buffer);
			SIZE_T module_size = static_cast<SIZE_T>(params->module_size);

			PEPROCESS process = nullptr;
			NTSTATUS status = utils::internal_functions::pfn_ps_lookup_process_by_process_id(process_id, &process);
			if (!NT_SUCCESS(status))
			{
				request->status = status;
				return false;
			}

			if (utils::internal_functions::pfn_ps_get_process_exit_status(process) != STATUS_PENDING)
			{
				utils::internal_functions::pfn_ob_dereference_object(process);
				request->status = STATUS_PROCESS_IS_TERMINATING;
				return false;
			}
			
			bool inject_result = utils::inject_utils::remote_thread_inject_x64_dll(
				process_id,
				module_buffer,
				module_size
			);

			utils::internal_functions::pfn_ob_dereference_object(process);
			request->status = inject_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

			return inject_result;

		}

		bool handle_clear_unloaded_drivers(user_comm_request* request)
		{
			if (!request || !request->input_buffer)
			{
				return false;
			}

	 
			p_user_comm_clear_unloaded_drivers_params params =
				reinterpret_cast<p_user_comm_clear_unloaded_drivers_params>(request->input_buffer);

			if (!params || !params->driver_name)
			{
				request->status = STATUS_INVALID_PARAMETER;
				return false;
			}

			 
			PWCHAR driver_name = reinterpret_cast<PWCHAR>(params->driver_name);
 
			bool result = utils::kernel_hide::clear_mm_unloaded_drivers_instr(driver_name);

			request->status = result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
			return result;
		}

	}
}
