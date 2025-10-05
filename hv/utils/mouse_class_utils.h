#pragma once


namespace utils
{
	namespace mouse_class
	{
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
	}
}