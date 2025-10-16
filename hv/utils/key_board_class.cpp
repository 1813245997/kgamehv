#include "global_defs.h"
#include "key_board_class.h"
#include "../game/game_cheat_config.h"

namespace utils
{

	namespace key_board_class
	{

		PDEVICE_OBJECT g_key_board_device_object = NULL;

		void(__fastcall* g_key_board_class_service_callback)(
			_In_ PDEVICE_OBJECT  device_object,
			_In_ PKEYBOARD_INPUT_DATA  input_data_start,
			_In_ PKEYBOARD_INPUT_DATA  input_data_end,
			_Out_ PULONG  data_consumed_count
		);
		void(__fastcall* original_key_board_class_service_callback)(
			_In_ PDEVICE_OBJECT  device_object,
			_In_ PKEYBOARD_INPUT_DATA  input_data_start,
			_In_ PKEYBOARD_INPUT_DATA  input_data_end,
			_Out_ PULONG  data_consumed_count
		);

		NTSTATUS initialize_key_board_class()
		{
			 
			NTSTATUS status = STATUS_SUCCESS;
			
			LogInfo("Initializing key board class");
			
			status = find_key_board_class_service_callback();
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to find key board class service callback");
				return status;
			}
			
			if (!utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(g_key_board_class_service_callback),
				hook_key_board_class_service_callback,
				reinterpret_cast<void**>(&original_key_board_class_service_callback)
			))
			{
				LogError("Failed to hook key board class service callback");
				return STATUS_UNSUCCESSFUL;
			}
			LogInfo("Hook key board class service callback SUCCESS");
			
			LogInfo("Key board class initialized successfully");
			
			return status;
		}
 

		NTSTATUS find_key_board_class_service_callback()
		{
			NTSTATUS status = STATUS_SUCCESS;
			 
			
			// Driver object pointers
			PDRIVER_OBJECT usb_keyboard_driver = NULL;      // USB keyboard driver (kbdhid)
			PDRIVER_OBJECT ps2_keyboard_driver = NULL;      // PS/2 keyboard driver (i8042prt)
			PDRIVER_OBJECT keyboard_class_driver = NULL;    // Keyboard class driver (kbdclass)
			PDRIVER_OBJECT target_driver = NULL;            // Target driver object
			PDEVICE_OBJECT target_device = NULL;            // Target device object

			LogInfo("KeyboardClass: Starting keyboard service callback discovery...");

			// Step 1: Get USB keyboard driver object
			status = utils::comm::get_driver_object(L"\\Driver\\kbdhid", &usb_keyboard_driver);
			if (NT_SUCCESS(status))
			{
				LogInfo("KeyboardClass: USB keyboard driver found");
				ObDereferenceObject(usb_keyboard_driver);
			}
			else
			{
				LogInfo("KeyboardClass: USB keyboard driver not found");
			}

			// Step 2: Get PS/2 keyboard driver object
			status = utils::comm::get_driver_object(L"\\Driver\\i8042prt", &ps2_keyboard_driver);
			if (NT_SUCCESS(status))
			{
				LogInfo("KeyboardClass: PS/2 keyboard driver found");
				ObDereferenceObject(ps2_keyboard_driver);
			}
			else
			{
				LogInfo("KeyboardClass: PS/2 keyboard driver not found");
			}

			// Step 3: Select target driver object (prefer PS/2 keyboard)
			if (!ps2_keyboard_driver && !usb_keyboard_driver)
			{
				LogError("KeyboardClass: No keyboard drivers found");
				return STATUS_UNSUCCESSFUL;
			}

			target_driver = ps2_keyboard_driver ? ps2_keyboard_driver : usb_keyboard_driver;
			LogInfo("KeyboardClass: Using %s keyboard driver", 
				ps2_keyboard_driver ? "PS/2" : "USB");

			// Step 4: Get keyboard class driver object
			status = utils::comm::get_driver_object(L"\\Driver\\kbdclass", &keyboard_class_driver);
			if (!NT_SUCCESS(status))
			{
				LogError("KeyboardClass: Failed to get keyboard class driver");
				return STATUS_UNSUCCESSFUL;
			}
			ObDereferenceObject(keyboard_class_driver);

			// Step 5: Traverse device objects to find service callback
			target_device = target_driver->DeviceObject;
			while (target_device)
			{
				status = search_service_from_kdb_ext(keyboard_class_driver, target_device);
				if (NT_SUCCESS(status))
				{
					LogInfo("KeyboardClass: Service callback found on device");
					break;
				}
				target_device = target_device->NextDevice;
			}

			// Step 6: Verify results
			if (g_key_board_device_object && g_key_board_class_service_callback)
			{
				LogInfo("KeyboardClass: Keyboard service callback discovery completed successfully");
				return STATUS_SUCCESS;
			}
			else
			{
				LogError("KeyboardClass: Failed to find keyboard service callback");
				return STATUS_UNSUCCESSFUL;
			}
		}

	NTSTATUS search_service_from_kdb_ext(PDRIVER_OBJECT keyboard_class_driver, PDEVICE_OBJECT port_device)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		PDEVICE_OBJECT target_device_object = NULL;
		UCHAR* device_extension = NULL;
		PVOID driver_start = NULL;
		ULONG driver_size = 0;
		PDEVICE_OBJECT temp_device = NULL;
		UNICODE_STRING keyboard_driver_name;
		int search_index = 0;

		// Initialize parameters
		driver_start = keyboard_class_driver->DriverStart;
		driver_size = keyboard_class_driver->DriverSize;
		utils::internal_functions::pfn_rtl_init_unicode_string(&keyboard_driver_name, L"\\Driver\\kbdclass");

		LogInfo("KeyboardClass: Searching for service callback in device extension...");

		// Step 1: Find device attached to keyboard class driver
		temp_device = port_device;
		while (temp_device->AttachedDevice != NULL)
		{
			LogDebug("KeyboardClass: Checking attached device: 0x%p", temp_device->AttachedDevice);
			LogDebug("KeyboardClass: Driver name: %wZ", &temp_device->AttachedDevice->DriverObject->DriverName);
			
			if (utils::internal_functions::pfn_rtl_compare_unicode_string(&temp_device->AttachedDevice->DriverObject->DriverName,
				&keyboard_driver_name, TRUE) == 0)
			{
				LogInfo("KeyboardClass: Found keyboard class driver attachment");
				break;
			}
			temp_device = temp_device->AttachedDevice;
		}

		if (temp_device->AttachedDevice == NULL)
		{
			LogError("KeyboardClass: No keyboard class driver attachment found");
			return status;
		}

		// Step 2: Traverse keyboard class driver device objects
		target_device_object = keyboard_class_driver->DeviceObject;
		while (target_device_object)
		{
			if (temp_device->AttachedDevice != target_device_object)
			{
				target_device_object = target_device_object->NextDevice;
				continue;
			}

			// Step 3: Search for callback function in device extension
			device_extension = (UCHAR*)temp_device->DeviceExtension;
			g_key_board_device_object = NULL;
			g_key_board_class_service_callback = NULL;

			// Traverse each pointer in device extension
			for (search_index = 0; search_index < 4096; search_index++, device_extension++)
			{
				PVOID pointer_value = NULL;

				// Check address validity
				if (!utils::internal_functions::pfn_mm_is_address_valid(device_extension))
				{
					LogDebug("KeyboardClass: Invalid address at index %d", search_index);
					break;
				}

				// Check if callback function already found
				if (g_key_board_device_object && g_key_board_class_service_callback)
				{
					LogInfo("KeyboardClass: Service callback already found");
					status = STATUS_SUCCESS;
					break;
				}

				// Read pointer value
				pointer_value = *(PVOID*)device_extension;

				// Check if it's the target device object
				if (pointer_value == target_device_object)
				{
					g_key_board_device_object = target_device_object;
					LogDebug("KeyboardClass: Found target device object at index %d", search_index);
					continue;
				}

				// Check if it's a callback function in keyboard class driver
				if ((pointer_value > driver_start) && 
					(pointer_value < (UCHAR*)driver_start + driver_size) &&
					(utils::internal_functions::pfn_mm_is_address_valid(pointer_value)))
				{
					g_key_board_class_service_callback = reinterpret_cast<void(__fastcall*)(PDEVICE_OBJECT, PKEYBOARD_INPUT_DATA, PKEYBOARD_INPUT_DATA, PULONG)>(pointer_value);
					LogInfo("KeyboardClass: Found service callback at index %d: 0x%p", 
						search_index, pointer_value);
				}
			}

			if (status == STATUS_SUCCESS)
			{
				break;
			}

			target_device_object = target_device_object->NextDevice;
		}

		// Step 4: Verify results
		if (g_key_board_device_object && g_key_board_class_service_callback)
		{
			LogInfo("KeyboardClass: Service callback search completed successfully");
			return STATUS_SUCCESS;
		}
		else
		{
			LogError("KeyboardClass: Service callback not found");
			return STATUS_UNSUCCESSFUL;
		}

	 
	  }

	  void hook_key_board_class_service_callback(
		_In_ PDEVICE_OBJECT  device_object,
		_In_ PKEYBOARD_INPUT_DATA  input_data_start,
		_In_ PKEYBOARD_INPUT_DATA  input_data_end,
		_Out_ PULONG  data_consumed_count
	  )
	  {
		  
		  
		// Process keyboard input data
		PKEYBOARD_INPUT_DATA current_input = input_data_start;
		
		while (current_input < input_data_end)
		{
		 
			
			// Check for menu-related hotkeys
			if (current_input->Flags == KEY_UP)  
			{
				switch (current_input->MakeCode)
				{
				 
					case 0x3B: // F1 key (scan code)
					{
						 
						game::cheat_config::toggle_player_skeleton();
						break;
					}
					case 0x3C: // F2 key (scan code)
					{
						 
						game::cheat_config::toggle_esp();
						break;
					}
					case 0x3D: // F3 key (scan code)
					{
						 
						game::cheat_config::toggle_aimbot();
						break;
					}
					default:
					{
						// Other keys - no action needed
						break;
					}
				}
			}
			else if (current_input->Flags == KEY_EXTENDED_FLAGS)
			{
				switch (current_input->MakeCode)
				{
				case 0x47: // HOME key (scan code)
				{

					game::cheat_config::toggle_menu_display();
					break;
				}
				 
				default:
				{
					// Other keys - no action needed
					break;
				}
				}
			}

		
			
			current_input++;
		}

		// Call original callback
		return original_key_board_class_service_callback(device_object, input_data_start, input_data_end, data_consumed_count);
	  }
	}

}