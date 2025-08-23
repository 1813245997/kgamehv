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
				LogError("driver_name is null.");
				return false;
			}

			if (!feature_data::mm_unloaded_drivers) {
				LogError("mm_unloaded_drivers is null.");
				return false;
			}

			if (!feature_data::mm_last_unloaded_driver) {
				LogError("mm_last_unloaded_driver is null.");
				return false;
			}

			unloaders_count = *feature_data::mm_last_unloaded_driver;
			if (unloaders_count == 0) {
				LogInfo("unloaders_count == 0, nothing to clear.");
				return false;
			}

			static ERESOURCE PsLoadedModuleResource;
			if (!ExAcquireResourceExclusiveLite(&PsLoadedModuleResource, TRUE)) {
				LogError("Failed to acquire PsLoadedModuleResource.");
				return false;
			}

			PUNICODE_STRING target_name = utils::string_utils::create_unicode_string_from_wchar(driver_name);
			if (!target_name) {
				ExReleaseResourceLite(&PsLoadedModuleResource);
				LogError("Failed to create UNICODE_STRING from driver_name: %ws", driver_name);
				return false;
			}

			PMM_UNLOADED_DRIVER  unloaders = *(PMM_UNLOADED_DRIVER*) feature_data::mm_unloaded_drivers;
			 
		 

			for (unsigned long i = 0; i < unloaders_count && i < max_unloaded_driver_count; ++i)
			{
				PMM_UNLOADED_DRIVER entry = &unloaders[i];
				if (!entry)
				{
					continue;
				}
				if (!entry->Name.Buffer)
				{
					continue;
				}
				if (  wcsstr(entry->Name.Buffer, target_name->Buffer))
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

					LogInfo("Successfully cleared unloaded driver: %ws (new count=%lu)", driver_name, *feature_data::mm_last_unloaded_driver);
					return true;
				}
			}

			utils::string_utils::free_unicode_string(target_name);
			ExReleaseResourceLite(&PsLoadedModuleResource);

			LogInfo("No matching unloaded driver entry found for: %ws", driver_name);
			return false;
		}

		bool clear_ci_ea_cache_lookaside_list()
		{
			BOOLEAN result = FALSE;

			PLOOKASIDE_LIST_EX ci_ea_cache_lookaside_list = feature_data::g_ciea_cache_lookaside_list;
			if (!ci_ea_cache_lookaside_list)
			{
				LogError("g_ciea_cache_lookaside_list is NULL");
				return FALSE;
			}

			ULONG list_entry_size = ci_ea_cache_lookaside_list->L.Size;

			// 删除原来的 lookaside list
			ExDeleteLookasideListEx(ci_ea_cache_lookaside_list);

			// 重新初始化
			NTSTATUS status = ExInitializeLookasideListEx(
				ci_ea_cache_lookaside_list,
				NULL,
				NULL,
				PagedPool,
				0,
				list_entry_size,
				'csIC',
				0
			);

			if (NT_SUCCESS(status))
			{
				LogInfo("Successfully cleared and reinitialized g_CiEaCacheLookasideList (Size = %lu)", list_entry_size);
				result = TRUE;
			}
			else
			{
				LogError("Failed to reinitialize g_CiEaCacheLookasideList, status = 0x%X", status);
				result = FALSE;
			}

			return result;
		}



	}

}