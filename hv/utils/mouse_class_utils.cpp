#include "global_defs.h"
#include "mouse_class_utils.h"

namespace utils
{
	namespace mouse_class
	{
		void(__fastcall* g_mouse_class_service_callback)(PDEVICE_OBJECT  device_object,
			PMOUSE_INPUT_DATA  input_data_start,
			PMOUSE_INPUT_DATA  input_data_end,
			PULONG  input_data_consumed);


        PDEVICE_OBJECT g_mouse_device_object;

		NTSTATUS initialize_mouse_class()
		{
			NTSTATUS status = STATUS_SUCCESS;
			 
			
			LogInfo("Initializing mouse class");
			
			status = find_mouse_class_service_callback();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to find mouse class service callback");
				return status;
			}
			
			LogInfo("Mouse class initialized successfully");
			return STATUS_SUCCESS;
		}

		// Helper function: Get driver object
		NTSTATUS get_driver_object(const wchar_t* driver_name, PDRIVER_OBJECT* driver_object)
		{
			UNICODE_STRING uni_name;
			utils::internal_functions::pfn_rtl_init_unicode_string(&uni_name, driver_name);
			
			return ObReferenceObjectByName(
				&uni_name,
				OBJ_CASE_INSENSITIVE,
				NULL,
				0,
				*IoDriverObjectType,
				KernelMode,
				NULL,
				reinterpret_cast<PVOID*>(driver_object)
			);
		}

		NTSTATUS find_mouse_class_service_callback()
		{
			NTSTATUS status = STATUS_SUCCESS;
			PDRIVER_OBJECT mouse_class_driver = NULL;
			PDRIVER_OBJECT usb_mouse_driver = NULL;
			PDRIVER_OBJECT ps2_mouse_driver = NULL;
			PDRIVER_OBJECT selected_port_driver = NULL;
			PDEVICE_OBJECT current_device = NULL;

 

			// Step 1: Try to get USB mouse port driver (\Driver\mouhid)
			status = get_driver_object(L"\\Driver\\mouhid", &usb_mouse_driver);
			if (NT_SUCCESS(status))
			{
				LogInfo("Found USB mouse driver (mouhid)");
				ObDereferenceObject(usb_mouse_driver);
			}
			else
			{
				LogError("USB mouse driver (mouhid) not found");
			}

			// Step 2: Try to get PS/2 mouse port driver (\Driver\i8042prt)
			status = get_driver_object(L"\\Driver\\i8042prt", &ps2_mouse_driver);
			if (NT_SUCCESS(status))
			{
				LogInfo("Found PS/2 mouse driver (i8042prt)");
				ObDereferenceObject(ps2_mouse_driver);
			}
			else
			{
				LogError("PS/2 mouse driver (i8042prt) not found");
			}

			// Step 3: Check if any mouse port drivers are available
			if (!usb_mouse_driver && !ps2_mouse_driver)
			{
				LogError("No mouse port drivers found");
				return STATUS_SUCCESS;
			}

			// Step 4: Select port driver (prefer USB over PS/2)
			selected_port_driver = usb_mouse_driver ? usb_mouse_driver : ps2_mouse_driver;
			LogInfo("Selected port driver: %s", usb_mouse_driver ? "USB" : "PS/2");

			// Step 5: Get mouse class driver (\Driver\mouclass)
			status = get_driver_object(L"\\Driver\\mouclass", &mouse_class_driver);
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to get mouse class driver (mouclass)");
				return STATUS_UNSUCCESSFUL;
			}
			ObDereferenceObject(mouse_class_driver);

			// Step 6: Traverse port driver device list to find MouseClassServiceCallback
			current_device = selected_port_driver->DeviceObject;
			while (current_device)
			{
				status = search_service_from_mou_ext(mouse_class_driver, current_device);
				if (status == STATUS_SUCCESS)
				{
					break;
				}
				current_device = current_device->NextDevice;
			}

			// Step 7: Verify if callback function was successfully found
			if (g_mouse_device_object && g_mouse_class_service_callback)
			{
				LogInfo("Successfully found MouseClassServiceCallback");
				
				  
			}
			else
			{
				LogError("Failed to find MouseClassServiceCallback");
				status = STATUS_UNSUCCESSFUL;
			}

			return status;
		}
		

		NTSTATUS search_service_from_mou_ext(PDRIVER_OBJECT mouse_class_driver, PDEVICE_OBJECT port_device)
		{
			PDEVICE_OBJECT target_class_device = NULL;
			PDEVICE_OBJECT current_device = port_device;
			UCHAR* device_extension_ptr = NULL;
			PVOID driver_start_addr = NULL;
			ULONG driver_size = 0;
			UNICODE_STRING mouse_class_name;
			NTSTATUS status = STATUS_UNSUCCESSFUL;

			// Initialize variables
			driver_start_addr = mouse_class_driver->DriverStart;
			driver_size = mouse_class_driver->DriverSize;
			utils::internal_functions::pfn_rtl_init_unicode_string(&mouse_class_name, L"\\Driver\\mouclass");

			// If mouse class device not found, return failure
			if (current_device->AttachedDevice == NULL)
			{
				LogError("Mouse class device not found in device chain");
				return status;
			}

			// Step 1: Find mouse class driver device in device chain
			while (current_device->AttachedDevice != NULL)
			{
				LogDebug("Checking attached device: 0x%x", current_device->AttachedDevice);
				LogDebug("Driver name: %wZ", &current_device->AttachedDevice->DriverObject->DriverName);

				if (utils::internal_functions::pfn_rtl_compare_unicode_string(
					&current_device->AttachedDevice->DriverObject->DriverName,
					&mouse_class_name,
					TRUE) == 0)
				{
					LogDebug("Found mouse class device");
					break;
				}
				current_device = current_device->AttachedDevice;
			}



			// Step 2: Find corresponding device object in mouse class driver
			target_class_device = mouse_class_driver->DeviceObject;
			while (target_class_device)
			{
				if (current_device->AttachedDevice != target_class_device)
				{
					target_class_device = target_class_device->NextDevice;
					continue;
				}

				// Step 3: Search for callback function pointer in device extension
				device_extension_ptr = reinterpret_cast<UCHAR*>(current_device->DeviceExtension);
				g_mouse_device_object = NULL;

				for (int i = 0; i < 4096; i++, device_extension_ptr++)
				{
					PVOID potential_callback = NULL;

					// Check memory address validity
					if (!internal_functions::pfn_mm_is_address_valid(device_extension_ptr))
					{
						break;
					}

					// If target already found, exit early
					if (g_mouse_device_object && g_mouse_class_service_callback)
					{
						status = STATUS_SUCCESS;
						break;
					}

					potential_callback = *reinterpret_cast<PVOID*>(device_extension_ptr);

					// Check if it's class driver device object
					if (potential_callback == target_class_device)
					{
						g_mouse_device_object = target_class_device;
						LogDebug("Found MouseClassDeviceObject at 0x%x", g_mouse_device_object);
						continue;
					}

					// Check if it's mouse class service callback function
					if ((potential_callback > driver_start_addr) &&
						(potential_callback < reinterpret_cast<UCHAR*>(driver_start_addr) + driver_size) &&
						internal_functions::pfn_mm_is_address_valid(potential_callback))
					{
						g_mouse_class_service_callback = reinterpret_cast<void(__fastcall*)(PDEVICE_OBJECT, PMOUSE_INPUT_DATA, PMOUSE_INPUT_DATA, PULONG)>(potential_callback);
						LogDebug("Found MouseClassServiceCallback at 0x%x", potential_callback);
						status = STATUS_SUCCESS;
					}
				}

				if (status == STATUS_SUCCESS)
				{
					break;
				}

				// Continue checking next device object
				target_class_device = target_class_device->NextDevice;
			}

			return status;
		}
		

		// Helper function: Get screen resolution using GetSystemMetrics
		NTSTATUS get_screen_resolution(ULONG* width, ULONG* height)
		{
		 
			unsigned long long  result{};
			// Initialize with default values in case of failure
			 


			// Get screen width using GetSystemMetrics(SM_CXSCREEN)
			   result = utils::user_call::call(
			 	utils::user_call::g_getsystemmetrics_fun, 
			 	SM_CXSCREEN, 
			 	0, 0, 0
			 );

			 *width = *reinterpret_cast<ULONG*>(result);
			 // Get screen height using GetSystemMetrics(SM_CYSCREEN)
			    result = utils::user_call::call(
			 	utils::user_call::g_getsystemmetrics_fun, 
			 	SM_CYSCREEN, 
			 	0, 0, 0
			 );

			 // Extract the actual values from the results
			 *height = *reinterpret_cast<ULONG*>(result);
			  
			 
			return STATUS_SUCCESS;
		}

		void send_mouse_input(ULONG flags, LONG x, LONG y)
		{
			PMOUSE_INPUT_DATA mouse_input_data_start = NULL;
			PMOUSE_INPUT_DATA mouse_input_data_end = NULL;
			ULONG input_data_consumed = 1;
			KIRQL old_irql;
			ULONG screen_width  =0;  // Default resolution
			ULONG screen_height =0; // Default resolution

			// Check if callback function and device object are available
			if (!g_mouse_class_service_callback || !g_mouse_device_object)
			{
				LogError("Mouse class service callback or device object not available");
				return;
			}

			 
			// Get screen resolution for absolute coordinate calculation
			NTSTATUS resolution_status = get_screen_resolution(&screen_width, &screen_height);
			if (!NT_SUCCESS(resolution_status))
			{
				LogError("Failed to get screen resolution, using defaults: %dx%d", screen_width, screen_height);
			}
			else
			{
				LogDebug("Using screen resolution: %dx%d", screen_width, screen_height);
			}

			// Allocate memory for mouse input data
			mouse_input_data_start = static_cast<PMOUSE_INPUT_DATA>(
				utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, sizeof(MOUSE_INPUT_DATA) * 2, 'MOUS')
			);
			
			if (!mouse_input_data_start)
			{
				LogError("Failed to allocate memory for mouse input data");
				return;
			}

			// Initialize mouse input data
			RtlZeroMemory(mouse_input_data_start, sizeof(MOUSE_INPUT_DATA) * 2);
			mouse_input_data_end = mouse_input_data_start + 1;

			// Set up mouse input data
			mouse_input_data_start->Flags = flags;
			mouse_input_data_start->UnitId = 0;
			mouse_input_data_start->RawButtons = 0;
			mouse_input_data_start->ExtraInformation = 0;

			if (flags == MOUSE_MOVE_RELATIVE)
			{
				mouse_input_data_start->LastX = x;
				mouse_input_data_start->LastY = y;
				 
			}
			else if (flags == MOUSE_MOVE_ABSOLUTE)
			{
				// Convert absolute coordinates to normalized coordinates (0-65535)
				// Formula: coordinate * 0xffff / screen_resolution + 1
				mouse_input_data_start->LastX = (x * 0xffff) / screen_width + 1;
				mouse_input_data_start->LastY = (y * 0xffff) / screen_height + 1;
				 
			}
			else
			{
				 
				ExFreePool(mouse_input_data_start);
				return;
			}

			// Enter guarded region and raise IRQL for safe execution
			KeEnterGuardedRegion();
			KeRaiseIrql(DISPATCH_LEVEL, &old_irql);

			// Validate device extension address before accessing
			if (internal_functions::pfn_mm_is_address_valid(g_mouse_device_object->DeviceExtension))
			{
				// Check spinlock address validity (offset 0x90 in device extension)
				PULONG64 spinlock_ptr = reinterpret_cast<PULONG64>(
					reinterpret_cast<PUCHAR>(g_mouse_device_object->DeviceExtension) + 0x90
				);

				if (internal_functions::pfn_mm_is_address_valid(spinlock_ptr))
				{
					// Call the mouse class service callback
					g_mouse_class_service_callback(
						g_mouse_device_object,
						mouse_input_data_start,
						mouse_input_data_end,
						&input_data_consumed
					);
					
					 
				}
				 
			}
			 

			// Restore IRQL and leave guarded region
			KeLowerIrql(old_irql);
			KeLeaveGuardedRegion();

			// Free allocated memory
			ExFreePool(mouse_input_data_start);
		}
  }
}


