#include "global_defs.h"
#include "scanner_offset.h"

namespace utils
{
	namespace scanner_offset
	{
	 
		unsigned long long  find_process_exit_time_offset()
		 {

			 unsigned long long process_exit_time_offset{};
			 unsigned long long temp_addr{};
			 

			 WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			 switch (Version)
			 {
			 case utils::WINDOWS_7:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);

			 }
			 break;
			 case utils::WINDOWS_7_SP1:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_8:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_8_1:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_1507:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_1511:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_1607:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_1703:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_1709:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_1803:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_1809:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_19H1:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_19H2:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_20H1:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_20H2:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_21H1:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_21H2:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_10_VERSION_22H2:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_11_VERSION_21H2:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_11_VERSION_22H2:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_11_VERSION_23H2:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 case utils::WINDOWS_11_VERSION_24H2:
			 {
				 temp_addr = signature_scanner::find_pattern(reinterpret_cast<ULONG_PTR>(internal_functions::pfn_ps_get_process_exit_time),
					 0x100,
					 "\x48\x8B\x80\x00\x00\x00\x00\xC3",
					 "xxx????x"
				 );
				 temp_addr += 3;
				 process_exit_time_offset = *reinterpret_cast<ULONG*>(temp_addr);
			 }
			 break;
			 default:
				 break;
			 }

			 return process_exit_time_offset;
		 }

		 unsigned long long find_dxgprocess_offset()
		 {
			 unsigned long long dxgprocess_offset{};
			 unsigned long long temp_addr{};
			 
			 WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());
			 if (Version<WINDOWS_10_VERSION_1507)
			 {
				 return 0;
			 }

			 temp_addr = reinterpret_cast<unsigned long long>(internal_functions::pfn_ps_get_process_dxgprocess);
			 temp_addr += 3;
			 dxgprocess_offset = *reinterpret_cast<ULONG*>(temp_addr);

			 return dxgprocess_offset;
		 }
		 unsigned long long find_process_wow64_process_offset()
		 {
			 unsigned long long process_wow64_process_offset{};
			 unsigned long long temp_addr{};

			 WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());
			 if (Version < WINDOWS_10_VERSION_1507)
			 {
				 return 0;
			 }

			 temp_addr = reinterpret_cast<unsigned long long>(internal_functions::pfn_ps_get_process_wow64_process);
			 temp_addr += 3;
			 process_wow64_process_offset = *reinterpret_cast<ULONG*>(temp_addr);

			 return process_wow64_process_offset;
		 }
		 unsigned long long find_process_peb_offset()
		 {
			 unsigned long long process_peb_offset{};
			 unsigned long long temp_addr{};

			 WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());
			 if (Version < WINDOWS_10_VERSION_1507)
			 {
				 return 0;
			 }

			 temp_addr = reinterpret_cast<unsigned long long>(internal_functions::pfn_ps_get_process_peb);
			 temp_addr += 3;
			 process_peb_offset = *reinterpret_cast<ULONG*>(temp_addr);

			 return process_peb_offset;
		 }
	}
}
