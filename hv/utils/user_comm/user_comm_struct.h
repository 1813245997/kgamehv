#pragma once
enum user_comm_op : uint64_t {
	user_comm_test = 0,
	user_comm_validate_license_expire_time,
	user_comm_file_delete_force
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