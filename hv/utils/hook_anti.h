#pragma once
#include "kvector.h"
#include "string_utils.h"

namespace utils
{
	namespace hook_anti
	{
		NTSTATUS initialize_hook_anti();

		PVOID get_driver_entry_point(PVOID  image_base);

		NTSTATUS init_driver_name_list();

		BOOLEAN is_target_driver(_In_ PUNICODE_STRING FullImageName);
		VOID hook_load_image_notify_routine(
		_In_opt_ PUNICODE_STRING FullImageName,
		_In_ HANDLE ProcessId,
		_In_ PIMAGE_INFO ImageInfo
		);

		NTSTATUS hook_driver_entry_point(PDRIVER_OBJECT const driver, PUNICODE_STRING RegistryPath);

		VOID hook_driver_unload(PDRIVER_OBJECT driver);

		
	}
}
