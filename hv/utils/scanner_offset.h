#pragma once
namespace utils
{ 
	namespace scanner_offset
	{
		unsigned long long  find_process_exit_time_offset();

		unsigned long long find_dxgprocess_offset();

		unsigned long long find_process_wow64_process_offset();

		unsigned long long find_process_peb_offset();

	}
}