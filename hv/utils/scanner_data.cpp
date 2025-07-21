#include "global_defs.h"
#include "scanner_data.h"
namespace utils
{
	namespace scanner_data
	{
		unsigned long long find_mm_unloaded_drivers_instr()
		{

			 	unsigned long long mm_unloaded_drivers_instr_addr{};
				unsigned long long temp_addr{};
				 
			
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x05\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_7_SP1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x05\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_8:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_8_1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x15\x00\x00\x00\x00\x4C\x8B\xC9",
					"xxx????xxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x4C\x8B\x0D\xCC\xCC\xCC\xCC\x4C\x8B\xC1\x4D\x85\xC9",
					"xxx????xxxxxx");
				mm_unloaded_drivers_instr_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			default:
				break;
			}

			return mm_unloaded_drivers_instr_addr;
		}
		unsigned long long find_mm_last_unloaded_driver_instr()
		{
			 unsigned long long  mm_last_unloaded_driver_instr{};
			unsigned long long temp_addr{};
			

			 
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr),2, 6);
			}
			break;
			case utils::WINDOWS_7_SP1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_8:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_8_1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr) ,2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr),2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr),2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2,6);
			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr),2, 6);
			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\x05\x00\x00\x00\x00\x83\xF8\x32",
					"xx????xxx");
				mm_last_unloaded_driver_instr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 2, 6);
			}
			break;
			default:
				break;
			}

			return mm_last_unloaded_driver_instr;
		}
		unsigned long long find_psp_cid_table()
		{
			 unsigned long long  psp_cid_table_addr{};
			unsigned long long temp_addr{};
			 


			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{
				//PsLookupThreadByThreadId
				//PAGE:00000001403380FA 48 8B D1                                                        mov     rdx, rcx
				//PAGE : 00000001403380FD 48 8B 0D C4 6A EE FF                                            mov     rcx, cs:PspCidTable

				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\xD1\x48\x8B\x0D\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x44\x8D\x6B\x01", 
					"xxxxxx????x????xxxx",
					"PAGE");
				temp_addr += 3;
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_7_SP1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\xD1\x48\x8B\x0D\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x44\x8D\x6B\x01",
					"xxxxxx????x????xxxx",
					"PAGE");
				temp_addr += 3;
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_8:
			{
				 //PspExitThread
			//PAGE:0000000140470474 48 8B 3D 0D 7D EE FF                                            mov     rdi, cs : PspCidTable
			//	PAGE : 000000014047047B F7 C2 FC 03 00 00                                               test    edx, 3FCh
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x3D\xCC\xCC\xCC\xCC\xF7\xC2\xFC\x03\x00\x00",
					"xxx????xxxxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_8_1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x0D\xCC\xCC\xCC\xCC\xF0\x48\x0F\xC1\x2E",
					"xxx????xxxxx", 
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xF9",
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x0D\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x48\x8B\xCB",
					"xxx????x????xxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x0D\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x48\x8B\xCB",
					"xxx????x????xxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9",
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9", 
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9", 
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9", 
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9",
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9",
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9", 
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9", 
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9", 
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9",
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\x41\x0F\xB7\xD9",
					"xxx????xxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x0D\xCC\xCC\xCC\xCC\x49\xD1\xE9",
					"xxx????xxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\xF0\x48\x0F\xC1\x08", 
					"xxx????xxxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{
				//PspClearProcessThreadCidRefs
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x15\xCC\xCC\xCC\xCC\xF0\x48\x0F\xC1\x08",
					"xxx????xxxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{
				temp_addr = signature_scanner::find_pattern_image(reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x48\x8B\x2D\xCC\xCC\xCC\xCC\x0F\x0D\x08\x48\x8B\x00", 
					"xxx????xxxxxx",
					"PAGE");
				psp_cid_table_addr = signature_scanner::resolve_relative_address(reinterpret_cast<PVOID>(temp_addr), 3, 7);
			}
			break;
			default:
				break;
			}

			return psp_cid_table_addr;
		}
	}

}