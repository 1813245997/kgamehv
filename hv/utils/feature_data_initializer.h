#pragma once

namespace utils {
	namespace feature_data {  
		

		extern PMM_UNLOADED_DRIVER  mm_unloaded_drivers;

		extern PULONG mm_last_unloaded_driver;

		extern PHANDLE_TABLE g_psp_cid_table;



		NTSTATUS initialize();
	}
}