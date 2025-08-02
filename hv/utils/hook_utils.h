#pragma once




namespace utils
{
	namespace hook_utils
	{


		bool hook_kernel_function(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function);

		bool hook_user_exception_handler(_In_ PEPROCESS process,_In_ void* target_api,_In_ void* exception_handler);

		bool hook_break_point_int3(_In_ PEPROCESS process, _In_ void* target_api, _In_ void* new_api, _Inout_ unsigned char * original_byte);

		bool unhook_user_all_exception_int3(_In_ PEPROCESS process);


	}
}