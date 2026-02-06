#pragma once
namespace utils
{
	namespace driver_features
	{
		NTSTATUS initialize_all_features(IN PVOID context);

		bool get_module_info_from_context(PVOID context, PVOID& module_base, SIZE_T& image_size);

	    

	}
}