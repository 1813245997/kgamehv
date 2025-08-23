 

 
 
#include "utils/global_defs.h"
 



void driver_unload(PDRIVER_OBJECT) {
   

  LogInfo("Devirtualized the system.\n");
  LogInfo("Driver unloaded.\n");
}
//snake_case
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT const driver, PUNICODE_STRING) {

	 
	return utils::driver_features::initialize_all_features(driver);
}

