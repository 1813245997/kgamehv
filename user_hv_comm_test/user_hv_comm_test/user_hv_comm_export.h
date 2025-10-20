#pragma once

#ifdef USER_HV_COMM_EXPORTS
#define USER_HV_COMM_API __declspec(dllexport)
#else
#define USER_HV_COMM_API __declspec(dllimport)
#endif

// Export C interface to ensure compatibility with both C++ and C
#ifdef __cplusplus
extern "C" {
#endif

// Basic communication test
USER_HV_COMM_API int hv_ping_call_export();

// License management
USER_HV_COMM_API int send_license_expiry_to_kernel_export(unsigned long long expiry_timestamp);

// File operations
USER_HV_COMM_API int force_delete_file_export(const wchar_t* file_path);

// Module information retrieval
USER_HV_COMM_API int get_module_info_export(
    unsigned long process_id,
    const wchar_t* module_name,
    unsigned long long* base_address,
    unsigned long long* module_size
);

// API Hook
USER_HV_COMM_API int hook_user_api_export(
    unsigned long process_id,
    void* target_api,
    void* new_api,
    void** origin_function
);

// Remote injection
USER_HV_COMM_API int handle_remote_inject_export(
    unsigned long process_id,
    void* module_buffer,
    size_t module_size
);

// Virtual memory operations
USER_HV_COMM_API int handle_read_virt_mem_export(
    unsigned long process_id,
    void* src_address,
    void* dst_buffer,
    size_t size,
    size_t* bytes_read_out
);

USER_HV_COMM_API int handle_write_virt_mem_export(
    unsigned long process_id,
    void* dst_address,
    void* src_buffer,
    size_t size,
    size_t* bytes_written_out
);

// Driver management
USER_HV_COMM_API int handle_clear_unloaded_drivers_export(const wchar_t* driver_name);

#ifdef __cplusplus
}
#endif
