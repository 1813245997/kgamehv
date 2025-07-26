 

 
 
#include "utils/global_defs.h"
#include "vtx/common.h"



void driver_unload(PDRIVER_OBJECT) {
   

  LogDebug("Devirtualized the system.\n");
  LogDebug("Driver unloaded.\n");
}
//snake_case
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT const driver, PUNICODE_STRING) {


	return utils::driver_features::initialize_all_features(driver);
}

