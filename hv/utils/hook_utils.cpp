#include "global_defs.h"
#include "hook_utils.h"
namespace utils
{
	namespace hook_utils
	{
		bool hook_kernel_function(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function)
		{
			if (!target_api || !new_api || !origin_function)
			{
				LogError("hook: Invalid parameters. target_api: %p, new_api: %p, origin_function: %p",
					target_api, new_api, origin_function);
				return false;
			}

			*origin_function = nullptr;

			const bool is_intel = utils::khyper_vt::is_intel_cpu();
			LogDebug("hook: Detected CPU vendor: %s", is_intel ? "Intel" : "AMD");

			if (is_intel)
			{
				bool result = hvgt::hook_kernel_function(target_api, new_api, origin_function);
				if (result)
				{
					LogInfo("hook: Successfully hooked. target_api: %p, new_api: %p, origin_function: %p",
						target_api, new_api, *origin_function);
				}
				else
				{
					LogError("hook: Hook failed. target_api: %p, new_api: %p", target_api, new_api);
				}
				return result;
			}
			else
			{
				LogError("hook: Unsupported CPU vendor (AMD). target_api: %p, new_api: %p", target_api, new_api);
				return false;
			}
		}


		bool hook_break_point_int3(_In_ HANDLE process_id, _In_ void* target_api, _In_ void* new_api, unsigned char* original_byte)
		{
			const bool is_intel = utils::khyper_vt::is_intel_cpu();
			const bool is_user = utils::memory::is_user_address(target_api);

			if (is_intel)
			{
				if (is_user)
				{
					return ept_hook_user_break_point_int3( target_api, new_api, original_byte);
				}
				else
				{
					return ept_hook_kernel_break_point_int3(target_api, new_api, original_byte);
				}
			}
			else // AMD
			{
				if (is_user)
				{
					return npt_hook_user_break_point_int3(process_id, target_api, new_api, original_byte);
				}
				else
				{
					return npt_hook_kernel_break_point_int3(target_api, new_api, original_byte);
				}
			}
	

			

		}
	 }
}