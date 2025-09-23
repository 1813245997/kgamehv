#pragma once
namespace utils
{ 
	namespace scanner_offset
	{
		unsigned long long  find_process_exit_time_offset();

		unsigned long long find_dxgprocess_offset();

		unsigned long long find_process_wow64_process_offset();

		unsigned long long find_process_peb_offset();

		unsigned long long find_thread_previous_mode_offset();

		unsigned long long find_start_address_offset();

		unsigned long long find_win32_start_address_offset();

		unsigned long long find_eprocess_unique_process_id_offset();
		
	}
}