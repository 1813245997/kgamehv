#pragma once
namespace utils
{
	namespace user_comm
	{
		

		bool is_valid_comm(user_comm_request* request);

		bool handle_test(user_comm_request* request);

     	bool handle_user_comm_request(user_comm_request* request);

		bool handle_validate_license_expire_time(user_comm_request* request);

		bool handle_force_delete_file(user_comm_request* request);

		bool handle_get_module_info(user_comm_request* request);

		bool handle_hook_user_api(user_comm_request* request);
		
	}
}