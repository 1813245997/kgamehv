#include "stdafx.h"
#include "user_hv_comm_export.h"
#include "hv_user_comm.h"

// Export function implementations, wrapping C++ interfaces as C interfaces

extern "C" {

USER_HV_COMM_API int hv_ping_call_export()
{
    try {
        bool result = utils::hv_user_comm::hv_ping_call();
        return result ? 1 : 0;
    }
    catch (...) {
        return 0;
    }
}

USER_HV_COMM_API int send_license_expiry_to_kernel_export(unsigned long long expiry_timestamp)
{
    try {
        bool result = utils::hv_user_comm::send_license_expiry_to_kernel(expiry_timestamp);
        return result ? 1 : 0;
    }
    catch (...) {
        return 0;
    }
}

USER_HV_COMM_API int force_delete_file_export(const wchar_t* file_path)
{
    try {
        if (!file_path) {
            return 0;
        }
        
        std::wstring wstr_file_path(file_path);
        bool result = utils::hv_user_comm::force_delete_file(wstr_file_path);
        return result ? 1 : 0;
    }
    catch (...) {
        return 0;
    }
}

USER_HV_COMM_API int get_module_info_export(
    unsigned long process_id,
    const wchar_t* module_name,
    unsigned long long* base_address,
    unsigned long long* module_size
)
{
    try {
        if (!module_name || !base_address || !module_size) {
            return 0;
        }
        
        std::wstring wstr_module_name(module_name);
        ULONG64 base_addr = 0;
        ULONG64 mod_size = 0;
        
        bool result = utils::hv_user_comm::get_module_info(
            process_id,
            wstr_module_name,
            &base_addr,
            &mod_size
        );
        
        if (result) {
            *base_address = base_addr;
            *module_size = mod_size;
            return 1;
        }
        
        return 0;
    }
    catch (...) {
        return 0;
    }
}

USER_HV_COMM_API int hook_user_api_export(
    unsigned long process_id,
    void* target_api,
    void* new_api,
    void** origin_function
)
{
    try {
        if (!target_api || !new_api || !origin_function) {
            return 0;
        }
        
        bool result = utils::hv_user_comm::hook_user_api(
            process_id,
            target_api,
            new_api,
            origin_function
        );
        
        return result ? 1 : 0;
    }
    catch (...) {
        return 0;
    }
}

USER_HV_COMM_API int handle_remote_inject_export(
    unsigned long process_id,
    void* module_buffer,
    size_t module_size
)
{
    try {
        if (!process_id || !module_buffer || module_size == 0) {
            return 0;
        }
        
        bool result = utils::hv_user_comm::handle_remote_inject(
            process_id,
            module_buffer,
            module_size
        );
        
        return result ? 1 : 0;
    }
    catch (...) {
        return 0;
    }
}

USER_HV_COMM_API int handle_read_virt_mem_export(
    unsigned long process_id,
    void* src_address,
    void* dst_buffer,
    size_t size,
    size_t* bytes_read_out
)
{
    try {
        if (!process_id || !src_address || !dst_buffer || size == 0 || !bytes_read_out) {
            return 0;
        }
        
        bool result = utils::hv_user_comm::handle_read_virt_mem(
            process_id,
            src_address,
            dst_buffer,
            size,
            bytes_read_out
        );
        
        return result ? 1 : 0;
    }
    catch (...) {
        return 0;
    }
}

USER_HV_COMM_API int handle_write_virt_mem_export(
    unsigned long process_id,
    void* dst_address,
    void* src_buffer,
    size_t size,
    size_t* bytes_written_out
)
{
    try {
        if (!process_id || !dst_address || !src_buffer || size == 0 || !bytes_written_out) {
            return 0;
        }
        
        bool result = utils::hv_user_comm::handle_write_virt_mem(
            process_id,
            dst_address,
            src_buffer,
            size,
            bytes_written_out
        );
        
        return result ? 1 : 0;
    }
    catch (...) {
        return 0;
    }
}

USER_HV_COMM_API int handle_clear_unloaded_drivers_export(const wchar_t* driver_name)
{
    try {
        if (!driver_name) {
            return 0;
        }
        
        std::wstring wstr_driver_name(driver_name);
        bool result = utils::hv_user_comm::handle_clear_unloaded_drivers(wstr_driver_name);
        return result ? 1 : 0;
    }
    catch (...) {
        return 0;
    }
}

} // extern "C"
