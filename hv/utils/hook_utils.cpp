#include "global_defs.h"
#include "hook_utils.h"
namespace utils
{
	namespace hook_utils
	{
		bool hook(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function)
		{
			if (!target_api || !new_api || !origin_function)
			{
				LogError("hook: Invalid parameters.");
				return false;
			}

			const bool is_intel = utils::khyper_vt::is_intel_cpu();

			LogDebug("hook: Detected CPU vendor: {}", is_intel ? "Intel" : "AMD");

			if (is_intel)
			{
				return hvgt::hook_kernel_function(target_api, new_api, origin_function);
			}
			else
			{
				//return svm::hook_kernel_function(target_api, new_api, origin_function);
				return false;
			}
			
		}
	 }
}