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

		NTSTATUS get_driver_object(const wchar_t* driver_name, PDRIVER_OBJECT* driver_object);

		// Screen resolution helper function
		NTSTATUS get_screen_resolution(ULONG* width, ULONG* height);

		void send_mouse_input(ULONG flags, LONG x, LONG y);
	}
}