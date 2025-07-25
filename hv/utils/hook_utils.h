#pragma once
namespace utils
{
	namespace hook_utils
	{
		bool hook_kernel_function(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function);

		bool hook_break_point_int3(_In_ HANDLE process_id, _In_ void* target_api, _In_ void* new_api, _Inout_ unsigned char * original_byte);
	}
}