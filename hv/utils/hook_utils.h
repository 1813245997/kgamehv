#pragma once
namespace utils
{
	namespace hook_utils
	{
		bool hook(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function);
	 
	}
}