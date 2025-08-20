#include "global_defs.h"
#include "kernel_hide_utils.h"
 

namespace utils
{
	namespace kernel_hide
	{
		bool clear_mm_unloaded_drivers_instr(_In_ PWCHAR driver_name)
		{
		 
			 
			constexpr unsigned int max_unloaded_driver_count = 50;
			unsigned long unloaders_count{};

			if (!driver_name) {
				return false;
			}

			if (!feature_data::mm_unloaded_drivers) {
				return false;
			}

			if (!feature_data::mm_last_unloaded_driver) {
				return false;
			}

			unloaders_count = *feature_data::mm_last_unloaded_driver;
			if (unloaders_count == 0) {
				return false;
			}

			if (!ExAcquireResourceExclusiveLite(&PsLoadedModuleResource, TRUE)) {
				return false;
			}

			// 用 utils::string_utils 来构造 UNICODE_STRING
			PUNICODE_STRING target_name = utils::string_utils::create_unicode_string_from_wchar(driver_name);
			if (!target_name) {
				ExReleaseResourceLite(&PsLoadedModuleResource);
				return false;
			}

			for (unsigned long i = 0; i < unloaders_count && i < max_unloaded_driver_count; ++i)
			{
				PMM_UNLOADED_DRIVER entry = &feature_data::mm_unloaded_drivers[i];
				if (entry->Name.Buffer && wcsstr(entry->Name.Buffer, target_name->Buffer))
				{
					ExFreePool(entry->Name.Buffer);
					entry->Name.Buffer = nullptr;

					RtlZeroMemory(entry, sizeof(MM_UNLOADED_DRIVER));

					// 向前移动
					for (unsigned long j = i; j < unloaders_count - 1; ++j)
					{
						feature_data::mm_unloaded_drivers[j] = feature_data::mm_unloaded_drivers[j + 1];
					}

					// 清掉最后一项
					RtlZeroMemory(&feature_data::mm_unloaded_drivers[unloaders_count - 1], sizeof(MM_UNLOADED_DRIVER));

					// 递减计数
					(*feature_data::mm_last_unloaded_driver)--;

					utils::string_utils::free_unicode_string(target_name);
					ExReleaseResourceLite(&PsLoadedModuleResource);

					return true;
				}
			}

			utils::string_utils::free_unicode_string(target_name);
			ExReleaseResourceLite(&PsLoadedModuleResource);

			return false;
		}


	}

}