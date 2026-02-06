#include "global_defs.h"
#include "hook_anti.h"
 
namespace utils
{
	namespace hook_anti
	{

		utils::kvector<PUNICODE_STRING> g_target_driver_names  ;

		VOID(__fastcall* original_load_image_notify_routine)(
			_In_opt_ PUNICODE_STRING FullImageName, 
			_In_ HANDLE ProcessId,
			_In_ PIMAGE_INFO ImageInfo
			)=nullptr;


		NTSTATUS(__fastcall* original_driver_entry_point)(
			PDRIVER_OBJECT const driver, 
			PUNICODE_STRING RegistryPath)=nullptr;

		NTSTATUS init_driver_name_list()
		{
			g_target_driver_names.init();

			const wchar_t* driver_names[] = {
				L"ACE-BASE.sys"
			};

			for (size_t i = 0; i < sizeof(driver_names) / sizeof(driver_names[0]); i++)
			{
				size_t string_length = wcslen(driver_names[i]) * sizeof(WCHAR);
				size_t buffer_size = string_length + sizeof(WCHAR);
				
				 
				auto* pUnicodeString = reinterpret_cast<PUNICODE_STRING>(
					utils::memory::allocate_independent_pages(sizeof(UNICODE_STRING), PAGE_READWRITE)
				);
				if (!pUnicodeString)
				{
					LogError("Failed to allocate memory for UNICODE_STRING");
					return STATUS_UNSUCCESSFUL;
				}
				RtlZeroMemory(pUnicodeString, sizeof(UNICODE_STRING));

				 
				auto* buffer = reinterpret_cast<PWCHAR>(
					utils::memory::allocate_independent_pages(buffer_size, PAGE_READWRITE)
				);
				if (!buffer)
				{
					LogError("Failed to allocate memory for driver name buffer");
					utils::memory::free_independent_pages(pUnicodeString, sizeof(UNICODE_STRING));
					return STATUS_UNSUCCESSFUL;
				}
				RtlZeroMemory(buffer, buffer_size);
				
			 
				RtlCopyMemory(buffer, driver_names[i], string_length);
				buffer[string_length / sizeof(WCHAR)] = L'\0';

				 
				pUnicodeString->Length = static_cast<USHORT>(string_length);
				pUnicodeString->MaximumLength = static_cast<USHORT>(buffer_size);
				pUnicodeString->Buffer = buffer;
				
				if (!g_target_driver_names.push_back(pUnicodeString))
				{
					LogError("Failed to add driver name to list");
					utils::memory::free_independent_pages(pUnicodeString, sizeof(UNICODE_STRING));
					utils::memory::free_independent_pages(buffer, buffer_size);
					return STATUS_UNSUCCESSFUL;
				}
			}

			LogInfo("Driver name list initialized with %d drivers", g_target_driver_names.size());
			return STATUS_SUCCESS;
		}

		BOOLEAN is_target_driver(_In_ PUNICODE_STRING FullImageName)
		{
			if (!FullImageName || !FullImageName->Buffer)
			{
				return FALSE;
			}

			for (size_t i = 0; i < g_target_driver_names.size(); i++)
			{
				PUNICODE_STRING target_name = g_target_driver_names[i];
				if (target_name && target_name->Buffer)
				{
					if (utils::string_utils::contains_substring_wchar(FullImageName->Buffer, target_name->Buffer, TRUE))
					{
						return TRUE;
					}
				}
			}

			return FALSE;
		}

		NTSTATUS initialize_hook_anti()
		{
			NTSTATUS status = init_driver_name_list();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize driver name list");
				return status;
			}

			if (!utils::internal_functions:: pfn_load_image_notify_routine)
			{
				LogError("pfn_load_image_notify_routine is not found");
				return STATUS_NOT_FOUND;
			}

			if (!utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(utils::internal_functions::pfn_load_image_notify_routine),
				hook_load_image_notify_routine,
				reinterpret_cast<void**>(& original_load_image_notify_routine)
			))
			{
				LogError("Failed to hook pfn_load_image_notify_routine");
				return STATUS_UNSUCCESSFUL;
			}
		LogInfo("Hook pfn_load_image_notify_routine SUCCESS");

		return STATUS_SUCCESS;
	}

		PVOID get_driver_entry_point(PVOID  image_base)
		{
			auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(image_base);
			auto nt_header = reinterpret_cast<PIMAGE_NT_HEADERS64>(
				reinterpret_cast<ULONG64>(image_base) + dos_header->e_lfanew
			);
		
			auto entry_point = reinterpret_cast<PVOID>(
				reinterpret_cast<ULONG64>(image_base) + nt_header->OptionalHeader.AddressOfEntryPoint
			);
		
			return entry_point;
	 
		}


		BOOL is_driver_image(_In_opt_ PUNICODE_STRING FullImageName,_In_ HANDLE ProcessId,_In_ PIMAGE_INFO ImageInfo)
		{
		
	   

			if(ProcessId !=0)
			{
				return FALSE;
			}

			if(!ImageInfo->SystemModeImage)
			{
				return FALSE;
			}
			return TRUE;
		}

		VOID hook_load_image_notify_routine(_In_opt_ PUNICODE_STRING FullImageName,_In_ HANDLE ProcessId,_In_ PIMAGE_INFO ImageInfo)
		{

			if(FullImageName == nullptr)
			{
				return original_load_image_notify_routine(FullImageName, ProcessId, ImageInfo);
			}
			
			if(ImageInfo == nullptr)
			{
				return original_load_image_notify_routine(FullImageName, ProcessId, ImageInfo);
			}

			if (!is_driver_image(FullImageName , ProcessId, ImageInfo))
			{
				return original_load_image_notify_routine(FullImageName, ProcessId, ImageInfo);
			}

			if (is_target_driver(FullImageName))
			{
				 
				LogInfo("Target driver loaded: %wZ", FullImageName);
				PVOID driver_entry_point = get_driver_entry_point(ImageInfo->ImageBase);
				if(driver_entry_point == nullptr)
				{
					LogError("Failed to get driver entry point");
					return original_load_image_notify_routine(FullImageName, ProcessId, ImageInfo);
				}
				utils::hook_utils::hook_kernel_function(driver_entry_point, hook_driver_entry_point, nullptr);
				LogInfo("Driver entry point hooked");
				return;
			}

			LogInfo("Driver image loaded: %wZ", FullImageName);

			return original_load_image_notify_routine(FullImageName, ProcessId, ImageInfo);
		}

		VOID hook_driver_unload(PDRIVER_OBJECT driver)
		{
			LogInfo("Driver unload: %wZ", driver->DriverName);

			UNICODE_STRING symlink_name;
			PDEVICE_OBJECT device_object = driver->DeviceObject;

			 
			RtlInitUnicodeString(&symlink_name, L"\\DosDevices\\Global\\ACE-BASE");
			IoDeleteSymbolicLink(&symlink_name);
			IoDeleteDevice(device_object);

			 
		}
		NTSTATUS hook_driver_entry_point(PDRIVER_OBJECT const driver, PUNICODE_STRING RegistryPath)
		{
			driver->DriverUnload = hook_driver_unload;
			LogInfo("Driver entry point hooked");
			LogInfo("Driver name: %wZ", RegistryPath);

			UNICODE_STRING device_name;
			UNICODE_STRING symlink_name;
			PDEVICE_OBJECT device_object = nullptr;
			NTSTATUS status;

			RtlInitUnicodeString(&device_name, L"\\Device\\ACE-BASE");
			RtlInitUnicodeString(&symlink_name, L"\\DosDevices\\Global\\ACE-BASE");
			status = IoCreateDevice(
				driver,      // Driver object
				0,                // Device extension size
				&device_name,         // Device name
				FILE_DEVICE_UNKNOWN, // Device type
				FILE_DEVICE_SECURE_OPEN, // Device characteristics
				FALSE,            // Exclusive device
				&device_object     // Output device object
			);
			if (!NT_SUCCESS(status))
			{
				return status;
			}

		 
			status = IoCreateSymbolicLink(&symlink_name, &device_name);
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to create symbolic link: 0x%X", status);
				IoDeleteDevice(device_object);
				return status;
			}

			LogInfo("Device and symbolic link created successfully");

		 
			 return status;
		}


	}

	
}
