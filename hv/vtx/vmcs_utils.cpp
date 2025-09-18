#include "../utils/global_defs.h"
#include "vmcs_utils.h"

// get the value of CR0 that the guest believes is active.
// this is a mixture of the guest CR0 and the CR0 read shadow.
cr0 read_effective_guest_cr0() {
	// TODO: cache this value
	auto const mask = hv::vmread(VMCS_CTRL_CR0_GUEST_HOST_MASK);

	// bits set to 1 in the mask are read from CR0, otherwise from the shadow
	cr0 cr0;
	cr0.flags = (hv::vmread(VMCS_CTRL_CR0_READ_SHADOW) & mask)
		| (hv::vmread(VMCS_GUEST_CR0) & ~mask);

	return cr0;
}

// get the value of CR4 that the guest believes is active.
// this is a mixture of the guest CR4 and the CR4 read shadow.
cr4 read_effective_guest_cr4() {
	// TODO: cache this value
	auto const mask = hv::vmread(VMCS_CTRL_CR4_GUEST_HOST_MASK);

	// bits set to 1 in the mask are read from CR4, otherwise from the shadow
	cr4 cr4;
	cr4.flags = (hv::vmread(VMCS_CTRL_CR4_READ_SHADOW) & mask)
		| (hv::vmread(VMCS_GUEST_CR4) & ~mask);

	return cr4;
}

uint16_t current_guest_cpl() {
	vmx_segment_access_rights ss;
	ss.flags = static_cast<uint32_t>(hv::vmread(VMCS_GUEST_SS_ACCESS_RIGHTS));
	return ss.descriptor_privilege_level;
}

ia32_vmx_procbased_ctls_register read_ctrl_proc_based() {
	ia32_vmx_procbased_ctls_register value;
	value.flags = hv::vmread(VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS);
	return value;
}

void write_ctrl_proc_based(ia32_vmx_procbased_ctls_register const value) {
	hv::vmwrite(VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, value.flags);
}



bool vmx_get_current_execution_mode()
{
	if (!g_vmm_context)
	{
		return VmxExecutionModeNonRoot;
	}
	if (!g_vmm_context->initialized)
	{
		return VmxExecutionModeNonRoot;
	}

	ULONG index = KeGetCurrentProcessorNumberEx(nullptr);
	__vcpu* current_vm_state = g_vmm_context->vcpu_table[index];

	return current_vm_state->is_on_vmx_root_mode ? VmxExecutionModeRoot : VmxExecutionModeNonRoot;
}



unsigned __int32 ajdust_controls(unsigned __int32 ctl, unsigned __int32 msr)
{
	__msr msr_value = { 0 };
	msr_value.all = __readmsr(msr);
	ctl &= msr_value.high;
	ctl |= msr_value.low;
	return ctl;
}

void cache_cpu_data(vcpu_cached_data& cached) {




	cpuid_eax_80000008 cpuid_80000008;
	__cpuid(reinterpret_cast<int*>(&cpuid_80000008), 0x80000008);

	cached.max_phys_addr = cpuid_80000008.eax.number_of_physical_address_bits;

	cached.vmx_cr0_fixed0 = __readmsr(IA32_VMX_CR0_FIXED0);
	cached.vmx_cr0_fixed1 = __readmsr(IA32_VMX_CR0_FIXED1);
	cached.vmx_cr4_fixed0 = __readmsr(IA32_VMX_CR4_FIXED0);
	cached.vmx_cr4_fixed1 = __readmsr(IA32_VMX_CR4_FIXED1);

	cpuid_eax_0d_ecx_00 cpuid_0d;
	__cpuidex(reinterpret_cast<int*>(&cpuid_0d), 0x0D, 0x00);

	// features in XCR0 that are supported
	cached.xcr0_unsupported_mask = ~((static_cast<uint64_t>(
		cpuid_0d.edx.flags) << 32) | cpuid_0d.eax.flags);

	cached.feature_control.flags = __readmsr(IA32_FEATURE_CONTROL);
	cached.vmx_misc.flags = __readmsr(IA32_VMX_MISC);

	// create a fake guest FEATURE_CONTROL MSR that has VMX and SMX disabled
	cached.guest_feature_control = cached.feature_control;
	cached.guest_feature_control.lock_bit = 1;
	cached.guest_feature_control.enable_vmx_inside_smx = 0;
	cached.guest_feature_control.enable_vmx_outside_smx = 0;
	cached.guest_feature_control.senter_local_function_enables = 0;
	cached.guest_feature_control.senter_global_enable = 0;
}