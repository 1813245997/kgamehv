#pragma once


namespace utils
{
	namespace mouse_class
	{

#define MOUSE_MOVE_RELATIVE         0
#define MOUSE_MOVE_ABSOLUTE         1
#pragma pack(push,4)
		typedef struct _MOUSE_INPUT_DATA {

			USHORT UnitId;

			USHORT Flags;

			union {
				ULONG Buttons;
				struct {
					USHORT  ButtonFlags;
					USHORT  ButtonData;
				};
			};

			ULONG RawButtons;

			LONG LastX;

			LONG LastY;

			ULONG ExtraInformation;

		} MOUSE_INPUT_DATA, * PMOUSE_INPUT_DATA;
#pragma pack()


		NTSTATUS initialize_mouse_class();

		NTSTATUS find_mouse_class_service_callback();

		NTSTATUS search_service_from_mou_ext(PDRIVER_OBJECT mou_driver_object, PDEVICE_OBJECT port_device);

		NTSTATUS get_screen_resolution(ULONG* width, ULONG* height);

		void send_mouse_input(ULONG flags, LONG x, LONG y);

		void  hook_mouse_class_service_callback(
			_In_ PDEVICE_OBJECT  device_object,
			_In_ PMOUSE_INPUT_DATA  input_data_start,
			_In_ PMOUSE_INPUT_DATA  input_data_end,
			_Out_ PULONG  data_consumed_count);
	}
}