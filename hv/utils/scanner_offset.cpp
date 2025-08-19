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
		

		 unsigned long long find_thread_previous_mode_offset()
		 {
			 unsigned long long previous_mode_offset{};
			 unsigned char* func_ptr = reinterpret_cast<unsigned char*>(ExGetPreviousMode);

			 // 遍历函数，找到最后一个 C3（RET）指令
			 unsigned char* ret_ptr = func_ptr;
			 while (*ret_ptr != 0xC3) {
				 ++ret_ptr;
			 }

			 // 在 RET 前 4 个字节读取偏移
			 previous_mode_offset = *reinterpret_cast<ULONG*>(ret_ptr - 4);

			 return previous_mode_offset;
		 }
		 unsigned long long find_start_address_offset()
		 {
			 ULONG start_address_offset = 0;
			 UNICODE_STRING func_name = { 0 };
			 RtlInitUnicodeString(&func_name, L"PsGetThreadId");
			 PUCHAR func_addr = (PUCHAR)MmGetSystemRoutineAddress(&func_name);
			 ULONG thread_id_offset = 0;

			 for (int i = 0; i < 100; i++)
			 {
				 if (func_addr[i] == 0xc3 &&
					 (func_addr[i + 1] == 0xcc || func_addr[i + 1] == 0x90) &&
					 (func_addr[i + 2] == 0xcc || func_addr[i + 2] == 0x90))
				 {
					 thread_id_offset = *(PULONG)(func_addr + i - 4);
					 break;
				 }
			 }

			 if (!thread_id_offset)
				 return 0;

			 start_address_offset = thread_id_offset - 0x30;
			 return start_address_offset;

			 
		 }
		 unsigned long long find_win32_start_address_offset()
		 {
			 ULONG win32_start_address_offset = 0;

		 
			 WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());
			 UNICODE_STRING func_name = { 0 };
			 RtlInitUnicodeString(&func_name, L"PsGetThreadId");
			 PUCHAR func_addr = (PUCHAR)MmGetSystemRoutineAddress(&func_name);
			 ULONG thread_id_offset = 0;

			 for (int i = 0; i < 100; i++)
			 {
				 if (func_addr[i] == 0xc3 &&
					 (func_addr[i + 1] == 0xcc || func_addr[i + 1] == 0x90) &&
					 (func_addr[i + 2] == 0xcc || func_addr[i + 2] == 0x90))
				 {
					 thread_id_offset = *(PULONG)(func_addr + i - 4);
					 break;
				 }
			 }

			 if (!thread_id_offset)
				 return 0;

			 if (Version == WINDOWS_7 || Version == WINDOWS_7_SP1)
			 {
				 win32_start_address_offset = thread_id_offset + 0x58;
			 }
			 else
			 {
				 win32_start_address_offset = thread_id_offset + 0x50;
			 }

			 return win32_start_address_offset;
		 }
	}
}
