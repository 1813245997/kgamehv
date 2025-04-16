#include "hv.h"

#include <ntddk.h>
#include <ia32.hpp>
#include "hook_manager.h"
#include "utils/module_info.h"

// simple hypercall wrappers
static uint64_t ping() {
  hv::hypercall_input input;
  input.code = hv::hypercall_ping;
  input.key  = hv::hypercall_key;
  return hv::vmx_vmcall(input);
}

void driver_unload(PDRIVER_OBJECT) {
  hv::stop();

  DbgPrint("[hv] Devirtualized the system.\n");
  DbgPrint("[hv] Driver unloaded.\n");
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT const driver, PUNICODE_STRING) {


	DbgPrint("[hv] Driver loaded.\n");

	if (driver)
		driver->DriverUnload = driver_unload;

	if (!hv::start()) {
		DbgPrint("[hv] Failed to virtualize system.\n");
		return STATUS_HV_OPERATION_FAILED;
	}

	if (ping() == hv::hypervisor_signature)
		DbgPrint("[client] Hypervisor signature matches.\n");
	else
		DbgPrint("[client] Failed to ping hypervisor!\n");

	if (!utils::module_info::init_ntoskrnl_info()) {
		DbgPrint("[hv] Failed to initialize ntoskrnl info.\n");
		return STATUS_UNSUCCESSFUL;
	}
	hook_manager::InitializeAllHooks();


	return STATUS_SUCCESS;
}

