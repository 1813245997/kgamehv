#pragma once
namespace utils
{
	namespace scanner_data
	{

		unsigned long long find_mm_unloaded_drivers_instr();
		unsigned long long find_mm_last_unloaded_driver_instr();
		unsigned long long find_psp_cid_table();
	}

}