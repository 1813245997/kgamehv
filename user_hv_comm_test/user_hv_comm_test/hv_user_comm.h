#pragma once
namespace utils
{
	namespace hv_user_comm
	{
	 

		bool hv_user_comm(user_comm_request* request);

		bool hv_ping_call();

		bool send_license_expiry_to_kernel(uint64_t expiry_timestamp);

		bool force_delete_file(const std::wstring& file_path);

		bool get_module_info(
			DWORD process_id,
			const std::wstring& module_name,
			PULONG64 base_address,
			PULONG64 module_size);

		bool hook_user_api(DWORD process_id, void* target_api, void* new_api, void** origin_function);

		bool handle_remote_inject(
			DWORD process_id,
			void* module_buffer,
			size_t module_size
		);

		bool handle_read_virt_mem(
			DWORD process_id,
			void* src_address,
			void* dst_buffer,
			size_t size,
			size_t* bytes_read_out
		);

		bool handle_write_virt_mem(
			DWORD process_id,
			void* dst_address,
			void* src_buffer,
			size_t size,
			size_t* bytes_written_out
		);

		bool handle_clear_unloaded_drivers(const std::wstring& driver_name);
		
	 }
}