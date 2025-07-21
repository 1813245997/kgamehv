#include "global_defs.h"
#include "feature_data_initializer.h"

namespace utils {
	namespace feature_data {

		PMM_UNLOADED_DRIVER  mm_unloaded_drivers = nullptr;
		PULONG mm_last_unloaded_driver = nullptr;
		PHANDLE_TABLE g_psp_cid_table = nullptr;

	 

		NTSTATUS initialize()
		{
			 
			const unsigned long long mm_unloaded_drivers_addr = scanner_data::find_mm_unloaded_drivers_instr();
			const unsigned long long mm_last_unloaded_driver_addr = scanner_data::find_mm_last_unloaded_driver_instr();
			const unsigned long long psp_cid_table_addr = scanner_data::find_psp_cid_table();
		

			mm_unloaded_drivers = reinterpret_cast<PMM_UNLOADED_DRIVER >(mm_unloaded_drivers_addr);
			mm_last_unloaded_driver = reinterpret_cast<PULONG>(mm_last_unloaded_driver_addr);
			g_psp_cid_table = reinterpret_cast<PHANDLE_TABLE>(psp_cid_table_addr);
			LogDebug( 
				"mm_unloaded_drivers       = %p\n", mm_unloaded_drivers);
			LogDebug( 
				"mm_last_unloaded_driver   = %p\n", mm_last_unloaded_driver);

			LogDebug(
				"g_psp_cid_table   = %p\n", g_psp_cid_table);
		 
			if (!mm_unloaded_drivers_addr || 
				!mm_last_unloaded_driver_addr||
				!psp_cid_table_addr)
			{
				LogDebug( 
					"Failed to locate MmUnloadedDrivers or MmLastUnloadedDriver\n");
				return STATUS_UNSUCCESSFUL;
			}

			return STATUS_SUCCESS;



		}
	}
}