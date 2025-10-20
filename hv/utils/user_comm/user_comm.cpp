#include "../global_defs.h"
#include "user_comm.h"
 
 

#define USER_COMM_KEY1 0X88889922
#define USER_COMM_KEY2 0X88889922

// Maximum size for virtual memory read operations (1MB)
#define MAX_READ_SIZE (1024 * 1024)
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

	 
			case  user_comm_remote_inject:
			{
				is_succeed = handle_remote_inject(request);
				break;
			}

			case  user_comm_clear_unloaded_drivers:
			{
				is_succeed = handle_clear_unloaded_drivers(request);
				break;
			}
			case user_comm_read_virt_mem:
			{
				 
				is_succeed = handle_read_virt_mem(request);
				break;
			}
			case  user_comm_write_virt_mem:
			{	  
				is_succeed = handle_write_virt_mem(request);
				break;
			}
 

			default:

				request->status = static_cast<uint64_t>(-1);  
				 
				 
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
			request->status = 0;   
			return true;
		}

		bool handle_validate_license_expire_time(user_comm_request* request)
		{

			if (!request->input_buffer || request->input_size < sizeof(uint64_t))
			{
				request->status = STATUS_INVALID_PARAMETER;   
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

			 
			size_t path_len = wcslen(file_path) + 1;

			 
			PWCHAR safe_path = reinterpret_cast<PWCHAR>(utils::memory::allocate_independent_pages(path_len * sizeof(WCHAR), PAGE_READWRITE));
				 
			 

			if (!safe_path)
			{
				request->status = STATUS_INSUFFICIENT_RESOURCES;
				return false;
			}

			 
			RtlCopyMemory(safe_path, file_path, path_len * sizeof(WCHAR));

			 
			UNICODE_STRING uni_path;
			RtlInitUnicodeString(&uni_path, safe_path);

			 
			status = utils::file_utils::force_delete_file(&uni_path);

			 
			 utils::memory::free_independent_pages (safe_path, path_len * sizeof(WCHAR));

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
			PWCHAR module_name = reinterpret_cast<PWCHAR> (params->module_name);    
			PULONG64 base_address = reinterpret_cast<PULONG64> (params->base_address); 
			PULONG64 module_size = reinterpret_cast<PULONG64> (params->module_size);   

			if (wcslen(module_name) == 0)
			{
				
				request->status = STATUS_INVALID_PARAMETER;
				return false;
			}

			*base_address = 0;
			*module_size = 0;


			PEPROCESS process = nullptr;
			NTSTATUS status = utils::internal_functions::pfn_ps_lookup_process_by_process_id (process_id, &process);
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

	    
			size_t name_len = (wcslen(module_name) + 1) * sizeof(WCHAR);
			PWCHAR kernel_module_name = reinterpret_cast<PWCHAR>(utils::memory::allocate_independent_pages(name_len, PAGE_READWRITE));
			if (!kernel_module_name)
			{
				utils::internal_functions::pfn_ob_dereference_object(process);
				request->status = STATUS_INSUFFICIENT_RESOURCES;
				return false;
			}

			RtlZeroMemory(kernel_module_name, name_len);
			RtlCopyMemory(kernel_module_name, module_name, name_len);
 
			status = utils::module_info:: get_process_module_info(
				process,
				kernel_module_name,
				base_address,
				reinterpret_cast<SIZE_T*>(module_size)
			);

	 
			utils::memory::free_independent_pages(kernel_module_name, name_len);
			utils::internal_functions::pfn_ob_dereference_object(process);
			request->status = status;
			return true;


			 
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

		bool handle_read_virt_mem(user_comm_request* request)
		{
			if (!request || !request->input_buffer)
			{
				return false;
			}

			p_user_comm_read_virt_mem_params params =
				reinterpret_cast<p_user_comm_read_virt_mem_params>(request->input_buffer);

			if (!params || !params->process_id || !params->src_address || !params->dst_buffer || !params->size)
			{
				request->status = STATUS_INVALID_PARAMETER;
				return false;
			}

			// Initialize bytes_read to 0
			*reinterpret_cast<size_t*>(params->bytes_read) = 0;

			// Validate size parameter
			if (params->size == 0 || params->size > MAX_READ_SIZE)
			{
				request->status = STATUS_INVALID_PARAMETER;
				return false;
			}

			// Get target process CR3
			cr3 target_cr3 = hv::prevmcall::query_process_cr3(params->process_id);
			if (!target_cr3.flags)
			{
				request->status = STATUS_NOT_FOUND;
				return false;
			}

			// Allocate kernel buffer for reading virtual memory
			void* kernel_buffer = utils::memory::allocate_independent_pages(params->size, PAGE_READWRITE);
			if (!kernel_buffer)
			{
				request->status = STATUS_INSUFFICIENT_RESOURCES;
				return false;
			}

			size_t bytes_read = 0;
			bool success = false;

			// Read virtual memory into kernel buffer
			bytes_read = hv::prevmcall::read_virt_mem(
				target_cr3,
				kernel_buffer,
				reinterpret_cast<void const*>(params->src_address),
				static_cast<size_t>(params->size)
			);

			if (bytes_read > 0)
			{
				// Copy data from kernel buffer to user buffer
				RtlCopyMemory(
					reinterpret_cast<void*>(params->dst_buffer),
					kernel_buffer,
					bytes_read
				);

				success = true;
			}

			// Free kernel buffer
			utils::memory::free_independent_pages(kernel_buffer, params->size);

			// Set result
			*reinterpret_cast<size_t*>(params->bytes_read) = bytes_read;
			request->status = success ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

			return success;
		}

		bool handle_write_virt_mem(user_comm_request* request)
		{
			if (!request || !request->input_buffer)
			{
				return false;
			}

			p_user_comm_write_virt_mem_params params =
				reinterpret_cast<p_user_comm_write_virt_mem_params>(request->input_buffer);

			if (!params || !params->process_id || !params->dst_address || !params->src_buffer || !params->size)
			{
				request->status = STATUS_INVALID_PARAMETER;
				return false;
			}

			// Initialize bytes_written to 0
			*reinterpret_cast<size_t*>(params->bytes_written) = 0;

			// Validate size parameter
			if (params->size == 0 || params->size > MAX_READ_SIZE)
			{
				request->status = STATUS_INVALID_PARAMETER;
				return false;
			}

			// Get target process CR3
			cr3 target_cr3 = hv::prevmcall::query_process_cr3(params->process_id);
			if (!target_cr3.flags)
			{
				request->status = STATUS_NOT_FOUND;
				return false;
			}

			// Allocate kernel buffer for writing virtual memory
			void* kernel_buffer = utils::memory::allocate_independent_pages(params->size, PAGE_READWRITE);
			if (!kernel_buffer)
			{
				request->status = STATUS_INSUFFICIENT_RESOURCES;
				return false;
			}

			size_t bytes_written = 0;
			bool success = false;

			// Copy data from user buffer to kernel buffer
			RtlCopyMemory(
				kernel_buffer,
				reinterpret_cast<void const*>(params->src_buffer),
				static_cast<size_t>(params->size)
			);

			// Write virtual memory from kernel buffer
			bytes_written = hv::prevmcall::write_virt_mem(
				target_cr3,
				reinterpret_cast<void*>(params->dst_address),
				kernel_buffer,
				static_cast<size_t>(params->size)
			);

			if (bytes_written > 0)
			{
				success = true;
			}

			// Free kernel buffer
			utils::memory::free_independent_pages(kernel_buffer, params->size);

			// Set result
			*reinterpret_cast<size_t*>(params->bytes_written) = bytes_written;
			request->status = success ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

			return success;
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

			bool cleared_unloaded = utils::kernel_hide::clear_mm_unloaded_drivers_instr(driver_name);
			bool cleared_ci_cache = utils::kernel_hide::clear_ci_ea_cache_lookaside_list();

			bool result = cleared_unloaded && cleared_ci_cache;

			request->status = result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
			return result;
		}



	}
}
