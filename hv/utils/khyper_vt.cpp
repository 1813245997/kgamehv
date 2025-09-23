#include "global_defs.h"
#include "khyper_vt.h"
 
 
namespace utils
{
	namespace  khyper_vt
	{
		
		NTSTATUS initialize_khyper_vt()
		{
			NTSTATUS  status{};
			CpuVendor  cpu_type{};
			status = initialize_cpu_vendor(&cpu_type);
			if (!NT_SUCCESS(status))
			{
				LogError("Failed to initialize CPU vendor information. Status: 0x%X", status);
				return status;
			}

			LogInfo("CPU vendor information initialized successfully.");

			status = is_cpu_virtualization_supported(cpu_type);
			if (!NT_SUCCESS(status)) {
				LogError("CPU does not support virtualization. Status: 0x%X", status);
				return status;  // Return the specific error code if virtualization is not supported
			}
			LogInfo("CPU supports virtualization.");
			status = is_ept_or_rvi_supported(cpu_type);
			if (!NT_SUCCESS(status)) {
				LogError("EPT or RVI support check failed. Status: 0x%X", status);
				return status;  // Return the specific error code if EPT or RVI is not supported
			}
			LogInfo("EPT or RVI support is confirmed.");

			status = is_virtualization_enabled_in_bios(cpu_type);
				if (!NT_SUCCESS(status)) {
					LogError("Virtualization is not enabled in BIOS. Status: 0x%X", status);
					return status;  // Return the specific error code if virtualization is not enabled
				}
			LogInfo("Virtualization is enabled in BIOS.");
			status = initialize_khyper_game_vm(cpu_type);
			if (!NT_SUCCESS(status)) {
				LogError("Failed to initialize virtualization functionality. Status: 0x%X", status);
				return status;
			}
			LogInfo("Virtualization functionality initialized successfully.");


			return status;

		}

		NTSTATUS  initialize_cpu_vendor(CpuVendor * cpu_type)
		{
			int cpu_info[4] = { 0 };
			__cpuid(cpu_info, 0);  // Get vendor ID

			char vendor[13]{};
			memcpy(vendor, &cpu_info[1], 4);
			memcpy(vendor + 4, &cpu_info[3], 4);
			memcpy(vendor + 8, &cpu_info[2], 4);

			if (strcmp(vendor, "GenuineIntel") == 0) {
				*cpu_type = CpuVendor::intel;
				LogInfo("CPU Vendor: Intel");
			}
			else if (strcmp(vendor, "AuthenticAMD") == 0) {
				*cpu_type = CpuVendor::amd;
				LogInfo("CPU Vendor: AMD");
				return STATUS_NOT_SUPPORTED;
			}
			else {
				*cpu_type = CpuVendor::unknown;
				LogInfo("CPU Vendor: Unknown");
				return STATUS_NOT_SUPPORTED;
			}

			return STATUS_SUCCESS;
		}

		NTSTATUS is_cpu_virtualization_supported(CpuVendor cpu_type)
		{
			int cpu_info[4] = { 0 };
			BOOLEAN is_supported = FALSE;

			// Check if the CPU supports virtualization
			if (cpu_type == CpuVendor::intel) {
				// Check if VT-x is supported on Intel CPUs
				__cpuid(cpu_info, 1);
				is_supported = (cpu_info[2] & (1 << 5)) != 0;  // ECX bit 5
			}
			else if (cpu_type == CpuVendor::amd) {
				// Check if SVM is supported on AMD CPUs
				__cpuid(cpu_info, 0x80000001);
				is_supported = (cpu_info[2] & (1 << 2)) != 0;  // ECX bit 2
			}
			else {
				// Unknown CPU vendor
				return STATUS_NOT_SUPPORTED;
			}

			return is_supported ? STATUS_SUCCESS : STATUS_NOT_SUPPORTED;
		}


		NTSTATUS is_ept_or_rvi_supported(CpuVendor cpu_type)
		{
			if (cpu_type == CpuVendor::intel)
			{
				ia32_vmx_ept_vpid_cap_register VpidRegister;
				ia32_mtrr_def_type_register    MTRRDefType;

				VpidRegister.flags = __readmsr(IA32_VMX_EPT_VPID_CAP);
				MTRRDefType.flags = __readmsr(IA32_MTRR_DEF_TYPE);

				if (!VpidRegister.page_walk_length_4 || !VpidRegister.memory_type_write_back || !VpidRegister.pde_2mb_pages)
				{
					LogError("Processor does not support required EPT features\n");
					return STATUS_NOT_SUPPORTED;
				}

				if (!VpidRegister.advanced_vmexit_ept_violations_information)
				{
					LogError("Processor does not support advanced VM-exit EPT violation information\n");
					return STATUS_NOT_SUPPORTED;
				}

				if (!VpidRegister.execute_only_pages)
				{
					LogError("Processor does not support execute-only pages; hooks may not work as this feature is used in our design\n");
					return STATUS_NOT_SUPPORTED;
				}

				if (!MTRRDefType.mtrr_enable)
				{
					LogError("Dynamic MTRR range is not supported\n");
					return STATUS_NOT_SUPPORTED;
				}

				LogInfo(" *** All required EPT features are present *** \n");

				return STATUS_SUCCESS;
			}
			else if (cpu_type == CpuVendor::amd)
			{
				LogError("Support check for AMD processors is not yet implemented\n");
				return STATUS_NOT_SUPPORTED;
			}

			LogError("Unknown CPU vendor\n");
			return STATUS_NOT_SUPPORTED;
		}

		NTSTATUS is_virtualization_enabled_in_bios(CpuVendor cpu_type)
		{
			if (cpu_type == CpuVendor::intel) {
				ia32_feature_control_register featureControlMsr{};
				// Check if VT-x is enabled on Intel CPUs by reading the MSR_IA32_FEATURE_CONTROL register
				featureControlMsr.flags = __readmsr(IA32_FEATURE_CONTROL);

				// Check if VT-x is enabled by evaluating the appropriate bit
				if (featureControlMsr.enable_vmx_outside_smx) {
					return STATUS_SUCCESS;
				}
				return STATUS_NOT_SUPPORTED;
			}
			else if (cpu_type == CpuVendor::amd) {
				//__svm_msr_vm_cr svmMsr{};
				//// Read the SVM_MSR_VM_CR MSR register
				//svmMsr.all = __readmsr(SVM_MSR_VM_CR);

				//// Check if SVM is enabled by evaluating the appropriate bit
				//if (BooleanFlagOn(svmMsr.all, SVM_VM_CR_SVMDIS)) {
				//	return STATUS_NOT_SUPPORTED;
				//}
				//return STATUS_SUCCESS;
				return STATUS_NOT_SUPPORTED;
			}
			else {
				// Unknown CPU vendor
				return STATUS_NOT_SUPPORTED;
			}
		}

		NTSTATUS initialize_khyper_game_vm(CpuVendor cpu_type)
		{
			NTSTATUS status = STATUS_SUCCESS;

			switch (cpu_type)
			{
			case CpuVendor::unknown:
				LogError("CPU vendor is unknown. Cannot initialize virtualization.");
				status = STATUS_NOT_SUPPORTED;
				break;

			case CpuVendor::intel:
				status = initialize_intel_vtx();
				LogInfo("Intel VT-x 0x%X", status);
				break;

			case CpuVendor::amd:
				status = initialize_amd_svm();
				LogInfo("AMD SVM 0x%X", status);
				break;

			default:
				LogError("Unsupported CPU vendor.");
				status = STATUS_NOT_SUPPORTED;
				break;
			}


			return status;
		}

		 
		NTSTATUS initialize_intel_vtx()
		{
		 
			if (vmm_init() == false)
			{
				hv::disable_vmx_operation();
				free_vmm_context();
				LogError("Vmm initialization failed");
				return STATUS_FAILED_DRIVER_ENTRY;
			}
			 
			if (hv ::prevmcall:: test_vmcall())
			{
				LogInfo("Successfully pinged the hypervisor.\n");
			}


			for (unsigned int iter = 0; iter < g_vmm_context->processor_count; iter++)
			{
				hv::measure_vm_exit_overheads(g_vmm_context->vcpu_table[iter]);
			}

			// Additional initialization code for Intel VT-x can be added here
			// Placeholder for additional initialization code
			LogInfo("Initializing Intel VT-x specific settings...");
			// If additional code is added, log success or failure of each step as needed

			LogInfo("Intel VT-x virtualization initialization succeeded.");
			return STATUS_SUCCESS;  // Return STATUS_SUCCESS if all operations succeeded
		}

		NTSTATUS initialize_amd_svm()
		{
			return NTSTATUS();
		}
		 
		bool is_intel_cpu()
		{
			int cpu_info[4] = { 0 };
			__cpuid(cpu_info, 0);  // ªÒ»° Vendor ID

			char vendor[13]{};
			memcpy(vendor, &cpu_info[1], 4);
			memcpy(vendor + 4, &cpu_info[3], 4);
			memcpy(vendor + 8, &cpu_info[2], 4);

			if (strcmp(vendor, "GenuineIntel") == 0) {
			 
				return true;
			}
			else if (strcmp(vendor, "AuthenticAMD") == 0) {
			 
			}
			else {
				LogInfo("CPU Vendor: Unknown");
			}

			return false;
		}
	}
}

