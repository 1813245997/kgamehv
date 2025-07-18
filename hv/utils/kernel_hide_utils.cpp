#include "global_defs.h"
#include "kernel_hide_utils.h"
#include "..\arch.h"

namespace utils
{
	namespace kernel_hide
	{
		bool clear_mm_unloaded_drivers_instr(_In_ PWCHAR driver_name)
		{
			UNREFERENCED_PARAMETER(driver_name);
			//constexpr unsigned int max_unloaded_driver_count = 50;
			//unsigned long unloaders_count{};
		 //
			//if (!driver_name) {
			//	 
			//	return false;
			//}

			//if (!feature_data::mm_unloaded_drivers) {
			//	 
			//	return false;
			//}

			//if (!feature_data::mm_last_unloaded_driver) {
			// 
			//	return false;
			//}

			//unloaders_count = *feature_data::mm_last_unloaded_driver;
			//if (unloaders_count == 0) {
			//	 
			//	return false;
			//}

			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] PsLoadedModuleResource address: 0x%p\n", &PsLoadedModuleResource);
			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] PsLoadedModuleResource contents: 0x%p\n", PsLoadedModuleResource);

			//if (!ExAcquireResourceExclusiveLite(&PsLoadedModuleResource, TRUE)) {

			// 
			//	return false;
			//}

			////enable_user_access();
			//UNICODE_STRING target_name{};
			//SIZE_T name_len = wcslen(driver_name) * sizeof(WCHAR);
			//target_name.Buffer = static_cast<PWCH>(ExAllocatePoolWithTag(NonPagedPool, name_len + sizeof(WCHAR), 'mmcl'));
			//if (!target_name.Buffer)
			//{
			//	//disable_user_access();
			//	return false;
			//}
			//
			//RtlZeroMemory(target_name.Buffer, name_len + sizeof(WCHAR));
			//RtlCopyMemory(target_name.Buffer, driver_name, name_len);
			////disable_user_access();
			//target_name.Length = static_cast<USHORT>(name_len);
			//target_name.MaximumLength = static_cast<USHORT>(name_len + sizeof(WCHAR));


			//
		 //
			//for (unsigned long i = 0; i < unloaders_count && i < max_unloaded_driver_count; ++i)
			//{
			//	PMM_UNLOADED_DRIVER entry = &feature_data::mm_unloaded_drivers[i];
			//	if (entry->Name.Buffer && wcsstr(entry->Name.Buffer, target_name.Buffer))
			//	{
			//		  

			//		ExFreePool(entry->Name.Buffer);
			//		entry->Name.Buffer = nullptr;

			//		RtlZeroMemory(entry, sizeof(MM_UNLOADED_DRIVER));

			//	 
			//		for (unsigned long j = i; j < unloaders_count - 1; ++j)
			//		{
			//			feature_data::mm_unloaded_drivers[j] = feature_data::mm_unloaded_drivers[j + 1];
			//		}

			//		 
			//		RtlZeroMemory(&feature_data::mm_unloaded_drivers[unloaders_count - 1], sizeof(MM_UNLOADED_DRIVER));

			//		 
			//		(*feature_data::mm_last_unloaded_driver)--;

			//		 
			//		ExFreePool(target_name.Buffer);

			//		return true;
			//	}
			//}
		 //
			//ExReleaseResourceLite(&PsLoadedModuleResource);
			//ExFreePool(target_name.Buffer);
			
			return false;
		}


	}

}