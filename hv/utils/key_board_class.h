#pragma once

namespace utils
{
	 
	namespace key_board_class
	{
		#pragma pack(push,4)
		typedef struct _KEYBOARD_INPUT_DATA {

			USHORT UnitId;
		
			USHORT MakeCode;
		
			USHORT Flags;
		
			USHORT Reserved;
		
			ULONG ExtraInformation;
		
		} KEYBOARD_INPUT_DATA, *PKEYBOARD_INPUT_DATA;
		#pragma pack()

		NTSTATUS initialize_key_board_class();

		NTSTATUS find_key_board_class_service_callback();

		NTSTATUS search_service_from_kdb_ext(PDRIVER_OBJECT KbdDriverObject, PDEVICE_OBJECT UsingDeviceObject);

		void hook_key_board_class_service_callback(
			_In_ PDEVICE_OBJECT  device_object,
			_In_ PKEYBOARD_INPUT_DATA  input_data_start,
			_In_ PKEYBOARD_INPUT_DATA  input_data_end,
			_Out_ PULONG  data_consumed_count
		);
	}
	   
}