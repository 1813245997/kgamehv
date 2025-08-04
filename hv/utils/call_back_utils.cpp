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
	}
}