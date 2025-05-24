#pragma once

namespace utils {
	namespace feature_init {  
		

		extern PMM_UNLOADED_DRIVER  mm_unloaded_drivers;

		extern PULONG mm_last_unloaded_driver;

		NTSTATUS initialize();
	}
}