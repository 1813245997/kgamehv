#include "hv.h"

 
#include <ia32.hpp>
#include "utils/driver_feature_init.h"

 

void driver_unload(PDRIVER_OBJECT) {
  hv::stop();

  DbgPrint("[hv] Devirtualized the system.\n");
  DbgPrint("[hv] Driver unloaded.\n");
}
//snake_case
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT const driver, PUNICODE_STRING) {


	return utils::driver_features::initialize_all_features(driver);
}

