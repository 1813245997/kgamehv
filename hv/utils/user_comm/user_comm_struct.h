#pragma once
enum user_comm_op : uint64_t {
	user_comm_test = 0,
	user_comm_validate_license_expire_time,
	user_comm_file_delete_force ,
	user_comm_get_module_info,
	user_comm_user_hook,
	user_comm_remote_inject
	// 可拓展
};

struct user_comm_request {

	uint64_t key1;
	uint64_t key2;
	uint64_t op_code;
	
	void* input_buffer;    // 可选输入数据
	uint64_t input_size;

	void* output_buffer;   // 可选输出数据
	uint64_t output_size;

	uint64_t status;       // 用于填充处理结果
};



typedef struct user_comm_get_module_info_params {
	uint64_t process_id;       /**< PID of the target process */
	uint64_t module_name;      /**< Address of the module name string (UTF-16) */
	uint64_t base_address;     /**< Pointer to receive the base address of the module */
	uint64_t module_size;      /**< Pointer to receive the size of the module */
} user_comm_get_module_info_params, * p_user_comm_get_module_info_params;


typedef struct user_comm_hook_params {
	uint64_t process_id;        /**< PID of the target process */
	uint64_t target_api;        /**< Address of the target API to hook */
	uint64_t new_api;           /**< Address of the new API (hook handler) */
	uint64_t origin_function;   /**< Pointer to receive the original function address */
} user_comm_hook_params, * p_user_comm_hook_params;

/** New: Remote injection parameters */
typedef struct user_comm_remote_inject_params {
	uint64_t process_id;     /**< PID of the target process */
	uint64_t module_buffer;  /**< Memory address of the module buffer */
	uint64_t module_size;    /**< Size of the module in bytes */
} user_comm_remote_inject_params, * p_user_comm_remote_inject_params;