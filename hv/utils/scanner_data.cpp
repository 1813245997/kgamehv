#include "global_defs.h"
#include "scanner_data.h"
namespace utils
{
	namespace scanner_data
	{
		unsigned long long find_mm_unloaded_drivers_instr()
		{

			static	unsigned long long mm_unloaded_drivers_instr_addr{};
			unsigned long long temp_addr{};
			if (mm_unloaded_drivers_instr_addr != 0)
			{
				return mm_unloaded_drivers_instr_addr;
			}
			
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
			static	unsigned long long  mm_last_unloaded_driver_instr{};
			unsigned long long temp_addr{};
			if (mm_last_unloaded_driver_instr != 0)
			{
				return  mm_last_unloaded_driver_instr;
			}

			 
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
	}

}