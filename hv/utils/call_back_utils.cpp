#include "global_defs.h"
#include "call_back_utils.h"

namespace utils
{
	namespace call_back_utils
	{
		unsigned  long long get_create_process_callback_address_by_index(int index)
		{
			void* callback_va = nullptr;
			if (!NT_SUCCESS(get_create_process_callback_by_index(index, &callback_va)) || callback_va == nullptr)
			{
				LogError("Failed to get callback address at index %d", index);
				return 0;
			}
			return reinterpret_cast<ULONG64>(callback_va);
		}

		unsigned long long get_load_image_callback_address_by_index(unsigned int index)
		{
			PVOID callback_address = nullptr;

			NTSTATUS status = get_load_image_callback_by_index(index, &callback_address);
			if (!NT_SUCCESS(status) || callback_address == nullptr)
			{
				LogInfo("Failed to get load image callback at index %u", index);
				return 0;
			}

			return reinterpret_cast<unsigned long long>(callback_address);
		}


		NTSTATUS get_create_process_callback_by_index(int index, void** callback_address)
		{
			// Ensure the index is within valid range (0-63)
			if (index < 0 || index >= 64)
			{
				LogError("Invalid index: %d", index);
				return STATUS_INVALID_PARAMETER;
			}

			// Get the address of the PspCreateProcessNotifyRoutine array
			PULONG64 PspCreateProcessNotifyRoutineAddr = reinterpret_cast<PULONG64>(
				find_psp_create_process_notify_routine()
				);

			// Check if the routine address was found
			if (PspCreateProcessNotifyRoutineAddr == nullptr)
			{
				LogError("Failed to find PspCreateProcessNotifyRoutine address.");
				return STATUS_UNSUCCESSFUL;
			}

			LogInfo("PspCreateProcessNotifyRoutine Address: %p", PspCreateProcessNotifyRoutineAddr);

			// Get the specific callback address by index
			PVOID pCreateProcessNotifyRoutineAddress = *(PVOID*)((PUCHAR)PspCreateProcessNotifyRoutineAddr + sizeof(PVOID) * index);
			pCreateProcessNotifyRoutineAddress = (PVOID)((ULONG64)pCreateProcessNotifyRoutineAddress & 0xfffffffffffffff8);

			// Check if the address is valid
			if (!MmIsAddressValid(pCreateProcessNotifyRoutineAddress))
			{
				LogError("Invalid callback address at index %d", index);
				return STATUS_INVALID_ADDRESS;
			}

			// Dereference to get the actual callback function address
			pCreateProcessNotifyRoutineAddress = *(PVOID*)pCreateProcessNotifyRoutineAddress;

			// Return the callback address through the output parameter
			*callback_address = pCreateProcessNotifyRoutineAddress;
			LogInfo("Callback Index: %d | Callback Address: 0x%p", index, pCreateProcessNotifyRoutineAddress);

			return STATUS_SUCCESS;
		}

		NTSTATUS get_load_image_callback_by_index(unsigned int index, PVOID* callback_address)
		{
			// Ensure the index is within valid range (0-63)
			if (index >= 64)
			{
				LogInfo("Invalid index: %u", index);
				return STATUS_INVALID_PARAMETER;
			}

			// Get the address of the PspLoadImageNotifyRoutine array
			PULONG64 p_psp_load_image_notify_routine = reinterpret_cast<PULONG64>(
				find_psp_load_image_notify_routine()
				);

			// Log the address of the routine
			LogInfo("PspLoadImageNotifyRoutine Address: %p", p_psp_load_image_notify_routine);

			// Check if the routine address was found
			if (p_psp_load_image_notify_routine == nullptr)
			{
				LogInfo("Failed to find PspLoadImageNotifyRoutine.");
				return STATUS_UNSUCCESSFUL;
			}

			// Get the specific callback address by index
			PVOID notify_routine_entry = *(PVOID*)((PUCHAR)p_psp_load_image_notify_routine + sizeof(PVOID) * index);
			notify_routine_entry = (PVOID)((ULONG64)notify_routine_entry & 0xfffffffffffffff8);

			// Check if the address is valid
			if (!MmIsAddressValid(notify_routine_entry))
			{
				LogInfo("Invalid callback address at index %u", index);
				return STATUS_INVALID_ADDRESS;
			}

			// Dereference to get the actual callback function address
			PVOID actual_callback_address = *(PVOID*)notify_routine_entry;

			// Return the callback address through the output parameter
			*callback_address = actual_callback_address;

			LogInfo("Callback Index: %u | Callback Address: 0x%p", index, actual_callback_address);

			return STATUS_SUCCESS;
		}

	 

		PVOID find_psp_create_process_notify_routine()
		{
			UNICODE_STRING unstrFunc{};
			static PULONG64  PspCreateProcessNotifyRoutine = 0;
			unsigned long long PspCreateProcessNotifyRoutineAddr = 0;
			unsigned long long PspSetCreateProcessNotifyRoutine = 0;
			PVOID pPsSetCreateProcessNotifyRoutine = NULL;
			WindowsVersion Version = static_cast<WindowsVersion>(utils::os_info::get_build_number());
			RtlInitUnicodeString(&unstrFunc, L"PsSetCreateProcessNotifyRoutine");
			pPsSetCreateProcessNotifyRoutine = MmGetSystemRoutineAddress(&unstrFunc);
			switch (Version)
			{
			case WINDOWS_7:
			{

				// Parse the PsSetCreateProcessNotifyRoutine function to find the jmp instruction
				unsigned char* pFunc = (unsigned char*)reinterpret_cast<ULONG64> (pPsSetCreateProcessNotifyRoutine) + 3;
				// Look for the jmp instruction (opcode 0xE9)
				if (pFunc[0] == 0xE9)
				{
					// Read the 4-byte relative offset from the jmp instruction
					ULONG relativeOffset = *reinterpret_cast<ULONG*>(pFunc + 1);

					// Calculate the target address of the jmp (PspSetCreateProcessNotifyRoutine)
					PspSetCreateProcessNotifyRoutine = reinterpret_cast<unsigned long long>(pFunc) + 5 + relativeOffset;
					LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);
					PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(PspSetCreateProcessNotifyRoutine, 0X1000, "\x4C\x8D\x35\xCC\xCC\xCC\xCC\x41\x8B\xC4", "xxx????xxx");
					PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspCreateProcessNotifyRoutineAddr), 3, 7));
				}

			}
			break;
			case WINDOWS_7_SP1:
			{
				// Get the address of PsSetCreateProcessNotifyRoutine using MmGetSystemRoutineAddress
				pPsSetCreateProcessNotifyRoutine = MmGetSystemRoutineAddress(&unstrFunc);
				if (!pPsSetCreateProcessNotifyRoutine)
				{
					LogError("Failed to get address of PsSetCreateProcessNotifyRoutine");
					return nullptr;
				}
				LogInfo("Address of PsSetCreateProcessNotifyRoutine: %p", pPsSetCreateProcessNotifyRoutine);
				// Parse the PsSetCreateProcessNotifyRoutine function to find the jmp instruction
				unsigned char* pFunc = (unsigned char*)reinterpret_cast<ULONG64> (pPsSetCreateProcessNotifyRoutine) + 3;
				// Look for the jmp instruction (opcode 0xE9)
				if (pFunc[0] == 0xE9)
				{
					// Read the 4-byte relative offset from the jmp instruction
					ULONG relativeOffset = *reinterpret_cast<ULONG*>(pFunc + 1);

					// Calculate the target address of the jmp (PspSetCreateProcessNotifyRoutine)
					PspSetCreateProcessNotifyRoutine = reinterpret_cast<unsigned long long>(pFunc) + 5 + relativeOffset;
					LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);
					PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(PspSetCreateProcessNotifyRoutine, 0X1000, "\x4C\x8D\x35\xCC\xCC\xCC\xCC\x41\x8B\xC4", "xxx????xxx");
					PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspCreateProcessNotifyRoutineAddr), 3, 7));
				}

			}
			break;
			case WINDOWS_8:
			{
				// Get the address of PsSetCreateProcessNotifyRoutine using MmGetSystemRoutineAddress


				// Parse the PsSetCreateProcessNotifyRoutine function to find the jmp instruction
				unsigned char* pFunc = (unsigned char*)reinterpret_cast<ULONG64> (pPsSetCreateProcessNotifyRoutine) + 3;
				// Look for the jmp instruction (opcode 0xE9)
				if (pFunc[0] == 0xEB)
				{
					// Read the 4-byte relative offset from the jmp instruction
					ULONG relativeOffset = *reinterpret_cast<ULONG*>(pFunc + 1);

					// Calculate the target address of the jmp (PspSetCreateProcessNotifyRoutine)
					PspSetCreateProcessNotifyRoutine = reinterpret_cast<unsigned long long>(pFunc) + 2 + relativeOffset;
					LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);
					PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(PspSetCreateProcessNotifyRoutine, 0X1000, "\x4C\x8D\x3D\xCC\xCC\xCC\xCC\x8B\xC3\x45\x33\xC0", "xxx????xxxxx");
					PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspCreateProcessNotifyRoutineAddr), 3, 7));
				}

			}
			break;
			case WINDOWS_8_1:
			{


				// Parse the PsSetCreateProcessNotifyRoutine function to find the jmp instruction
				unsigned char* pFunc = (unsigned char*)reinterpret_cast<ULONG64> (pPsSetCreateProcessNotifyRoutine) + 3;
				// Look for the jmp instruction (opcode 0xE9)
				if (pFunc[0] == 0xE9)
				{
					// Read the 4-byte relative offset from the jmp instruction
					ULONG relativeOffset = *reinterpret_cast<ULONG*>(pFunc + 1);

					// Calculate the target address of the jmp (PspSetCreateProcessNotifyRoutine)
					PspSetCreateProcessNotifyRoutine = reinterpret_cast<unsigned long long>(pFunc) + 5 + relativeOffset;
					LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);
					PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(PspSetCreateProcessNotifyRoutine, 0X1000, "\x4C\x8D\x3D\xCC\xCC\xCC\xCC\x8B\xC3\x45\x33\xC0", "xxx????xxxxx");
					PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspCreateProcessNotifyRoutineAddr), 3, 7));
				}

			}
			break;
			case WINDOWS_10_VERSION_1507:
			{

				// Parse the PsSetCreateProcessNotifyRoutine function to find the jmp instruction
				unsigned char* pFunc = (unsigned char*)reinterpret_cast<ULONG64> (pPsSetCreateProcessNotifyRoutine) + 3;
				// Look for the jmp instruction (opcode 0xE9)
				if (pFunc[0] == 0xE9)
				{
					// Read the 4-byte relative offset from the jmp instruction
					ULONG relativeOffset = *reinterpret_cast<ULONG*>(pFunc + 1);

					// Calculate the target address of the jmp (PspSetCreateProcessNotifyRoutine)
					PspSetCreateProcessNotifyRoutine = reinterpret_cast<unsigned long long>(pFunc) + 5 + relativeOffset;
					LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);
					PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(PspSetCreateProcessNotifyRoutine, 0X1000, "\x4C\x8D\x3D\xCC\xCC\xCC\xCC\x8B\xC3\x45\x33\xC0", "xxx????xxxxx");
					PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspCreateProcessNotifyRoutineAddr), 3, 7));
				}

			}
			break;
			case WINDOWS_10_VERSION_1511:
			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE9)
					{

						ULONG relativeOffset = *reinterpret_cast<ULONG*>(pFunc + i + 1);


						PspSetCreateProcessNotifyRoutine = reinterpret_cast<unsigned long long>(pFunc) + i + 5 + relativeOffset;
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x25\xCC\xCC\xCC\xCC\x4D\x8D\x2C\xEC", "xxx????xxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}


			}

			break;
			case WINDOWS_10_VERSION_1607:
			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE9)
					{

						ULONG relativeOffset = *reinterpret_cast<ULONG*>(pFunc + i + 1);


						PspSetCreateProcessNotifyRoutine = reinterpret_cast<unsigned long long>(pFunc) + i + 5 + relativeOffset;
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x25\xCC\xCC\xCC\xCC\x49\x8D\x0C\xFC", "xxx????xxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}


			}
			break;
			case WINDOWS_10_VERSION_1703:
			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE9)
					{

						ULONG relativeOffset = *reinterpret_cast<ULONG*>(pFunc + i + 1);


						PspSetCreateProcessNotifyRoutine = reinterpret_cast<unsigned long long>(pFunc) + i + 5 + relativeOffset;
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x25\xCC\xCC\xCC\xCC\x49\x8D\x0C\xDC", "xxx????xxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}

			break;
			case WINDOWS_10_VERSION_1709:
			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}

			break;
			case WINDOWS_10_VERSION_1803:
			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}

			break;
			case WINDOWS_10_VERSION_1809:
			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}

			break;
			case WINDOWS_10_VERSION_19H1:
			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			case WINDOWS_10_VERSION_19H2:
			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			case WINDOWS_10_VERSION_20H1:
			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			case WINDOWS_10_VERSION_20H2:

			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			case WINDOWS_10_VERSION_21H1:

			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			case WINDOWS_10_VERSION_21H2:

			{



				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			case WINDOWS_10_VERSION_22H2:

			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			case WINDOWS_11_VERSION_21H2:
			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			case WINDOWS_11_VERSION_22H2:
			{


				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			case WINDOWS_11_VERSION_23H2:

			{




				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			case WINDOWS_11_VERSION_24H2:
			{



				unsigned char* pFunc = static_cast<unsigned char*>(pPsSetCreateProcessNotifyRoutine);


				for (size_t i = 0; i < 0x100; ++i)
				{

					if (pFunc[i] == 0xE8)
					{

						PspSetCreateProcessNotifyRoutine = utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (reinterpret_cast<unsigned long long>(pFunc) + i), 1, 5);
						LogInfo("PspSetCreateProcessNotifyRoutine address: %llx", PspSetCreateProcessNotifyRoutine);


						PspCreateProcessNotifyRoutineAddr = utils::signature_scanner::find_pattern(
							PspSetCreateProcessNotifyRoutine,
							0x1000,
							"\x4C\x8D\x2D\xCC\xCC\xCC\xCC\x48\x8D\x0C\xDD\x00\x00\x00\x00", "xxx????xxxxxxxx"
						);

						PspCreateProcessNotifyRoutine = reinterpret_cast<PULONG64>(
							utils::signature_scanner::resolve_relative_address(
								reinterpret_cast<PVOID>(PspCreateProcessNotifyRoutineAddr), 3, 7
							)
							);

						break;
					}
				}

			}
			break;
			default:
				break;
			}

			LogInfo("  PspCreateProcessNotifyRoutine: %p", PspCreateProcessNotifyRoutine);
			if (!PspCreateProcessNotifyRoutine)
			{
				return nullptr;
			}
			return PspCreateProcessNotifyRoutine;
		}
		PVOID find_psp_load_image_notify_routine()
		{
			WindowsVersion Version = static_cast<WindowsVersion>(utils::os_info::get_build_number());
			static PULONG64  PspLoadImageNotifyRoutine = 0;
			unsigned long long PspLoadImageNotifyRoutineAddr = 0;
			PVOID pPsSetLoadImageNotifyRoutineEx = NULL;
			UNICODE_STRING ustrFuncName;
			RtlInitUnicodeString(&ustrFuncName, L"PsSetLoadImageNotifyRoutineEx");
			switch (Version)
			{
			case WINDOWS_7:
			{
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(PsSetLoadImageNotifyRoutine), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8B\xD7\x48\x8D\x0C\xD9\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x11\xFF\xC3\x83\xFB\x08", "xxx????xxxxxxxxxxx????xxxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}


			break;
			case WINDOWS_7_SP1:
			{
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(PsSetLoadImageNotifyRoutine), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8B\xD7\x48\x8D\x0C\xD9\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x11\xFF\xC3\x83\xFB\x40\x72\xDF\x48\x8B\xCF\xE8\xCC\xCC\xCC\xCC\xEB\xCC\xF0\x83\x05\xCC\xCC\xCC\xCC\xCC\x8B\x05\xCC\xCC\xCC\xCC\xA8\x01", "xxx????xxxxxxxxxxx????xxxxxxxxxxxxxxx????xxxxx?????xx????xx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_8:
			{
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(PsSetLoadImageNotifyRoutine), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8B\xD7\x48\x8D\x0C\xD9\xE8\xCC\xCC\xCC\xCC\x84\xC0\x74\x27\xF0\xFF\x05\xCC\xCC\xCC\xCC\x8B\x05\xCC\xCC\xCC\xCC\xA8\x01", "xxx????xxxxxxxxxxx????xxxxxxx????xx????xx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));

			}
			break;
			case WINDOWS_8_1:
			{
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(PsSetLoadImageNotifyRoutine), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8B\xD7\x48\x8D\x0C\xD9\xE8\xCC\xCC\xCC\xCC\x84\xC0\x74\x27\xF0\xFF\x05\xCC\xCC\xCC\xCC\x8B\x05\xCC\xCC\xCC\xCC\xA8\x01", "xxx????xxxxxxxxxxx????xxxxxxx????xx????xx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));

			}
			break;
			case WINDOWS_10_VERSION_1507:
			{
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(PsSetLoadImageNotifyRoutine), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\x9F\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}

			break;
			case WINDOWS_10_VERSION_1511:
			{
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(PsSetLoadImageNotifyRoutine), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\x9F\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_1607:
			{
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(PsSetLoadImageNotifyRoutine), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\x9F\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_1703:
			{

				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(PsSetLoadImageNotifyRoutine), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\x9F\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_1709:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\x9F\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_1803:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\x9F\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_1809:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\x9F\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_19H1:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\x9F\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_19H2:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\x9F\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_20H1:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\xA0\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_20H2:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\xA0\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_21H1:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\xA0\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_21H2:
			{

				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\xA0\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_10_VERSION_22H2:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\xA0\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_11_VERSION_21H2:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\xA0\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_11_VERSION_22H2:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\xA0\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_11_VERSION_23H2:
			{
				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\xA0\x00\x00\x00", "xxx????xxxxxxxxxxx????xxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));
			}
			break;
			case WINDOWS_11_VERSION_24H2:
			{


				pPsSetLoadImageNotifyRoutineEx = MmGetSystemRoutineAddress(&ustrFuncName);
				LogInfo("Address of PsSetLoadImageNotifyRoutineEx: %p", pPsSetLoadImageNotifyRoutineEx);
				PspLoadImageNotifyRoutineAddr = utils::signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(pPsSetLoadImageNotifyRoutineEx), 0X1000, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x45\x33\xC0\x48\x8D\x0C\xD9\x48\x8B\xD7\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x0C\xFF\xC3\x83\xFB\x40\x72\xDF\xE9\x01\xAF\x18\x00", "xxx????xxxxxxxxxxx????xxxxxxxxxxxxxxxx");
				PspLoadImageNotifyRoutine = reinterpret_cast<PULONG64> (utils::signature_scanner::resolve_relative_address(reinterpret_cast<PVOID> (PspLoadImageNotifyRoutineAddr), 3, 7));

			}
			break;
			default:
				break;
			}

			LogInfo("  PspLoadImageNotifyRoutine: %p", PspLoadImageNotifyRoutine);
			if (!PspLoadImageNotifyRoutine)
			{
				return nullptr;
			}
			return PspLoadImageNotifyRoutine;
		}
	}
}