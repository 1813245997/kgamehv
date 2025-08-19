#pragma once
namespace utils
{
	namespace  feature_offset
	{

		 

		extern unsigned long long g_process_exit_time_offset;

		extern unsigned long long g_dxgprocess_offset;

		extern unsigned long long g_process_wow64_process_offset;

		extern unsigned long long g_process_peb_offset;
		 
		extern unsigned long long g_previous_modeoffset;

		extern unsigned long long g_start_address_offset;

		extern unsigned long long g_win32_start_address_offset;

		NTSTATUS initialize();
	}
}