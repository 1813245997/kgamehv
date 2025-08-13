#pragma warning( disable : 4201 4244)
#include "../utils/global_defs.h"
#include <intrin.h>
#include "segment.h"
#include "common.h"
#include "vmcs.h"
#include "vmcs_encodings.h"
#include "msr.h"
#include "../asm\vm_intrin.h"
#include "../asm\vm_context.h"
#include "cr.h"
 
#include "hypervisor_routines.h"



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
/// <summary>
/// Get segment base
/// </summary>
/// <param name="selector"></param>
/// <param name="gdt_base"></param>
/// <returns></returns>
unsigned __int64 get_segment_base(unsigned __int16 selector, unsigned __int8* gdt_base)
{
	__segment_descriptor* segment_descriptor;

	segment_descriptor = (__segment_descriptor*)(gdt_base + (selector & ~0x7));

	unsigned __int64 segment_base = segment_descriptor->base_low | segment_descriptor->base_middle << 16 | segment_descriptor->base_high << 24;

	if (segment_descriptor->descriptor_type == false)
		segment_base = (segment_base & MASK_32BITS) | (unsigned __int64)segment_descriptor->base_upper << 32;

	return segment_base;
}

/// <summary>
/// Fill the guest's selector data
/// </summary>
/// <param name="gdt_base"></param>
/// <param name="segment_register"></param>
/// <param name="selector"></param>
void fill_guest_selector_data(void* gdt_base, unsigned __int32 segment_register, unsigned __int16 selector)
{
	__segment_access_rights segment_access_rights;
	__segment_descriptor* segment_descriptor;

	if (selector & 0x4)
		return;

	segment_descriptor = (__segment_descriptor*)((unsigned __int8*)gdt_base + (selector & ~0x7));

	unsigned __int64 segment_base = segment_descriptor->base_low | segment_descriptor->base_middle << 16 | segment_descriptor->base_high << 24;

	unsigned __int32 segment_limit = segment_descriptor->limit_low | (segment_descriptor->segment_limit_high << 16);

	//
	// Load ar get access rights of descriptor specified by selector
	// Lower 8 bits are zeroed so we have to bit shift it to right by 8
	//
	segment_access_rights.all = __load_ar(selector) >> 8;
	segment_access_rights.unusable = 0;
	segment_access_rights.reserved0 = 0;
	segment_access_rights.reserved1 = 0;

	// This is a TSS or callgate etc, save the base high part
	if (segment_descriptor->descriptor_type == false)
		segment_base = (segment_base & MASK_32BITS) | (unsigned __int64)segment_descriptor->base_upper << 32;

	if (segment_descriptor->granularity == true)
		segment_limit = (segment_limit << 12) + 0xfff;

	if (selector == 0)
		segment_access_rights.all |= 0x10000;

	hv::vmwrite<unsigned __int64>(GUEST_ES_SELECTOR + segment_register * 2, selector);
	hv::vmwrite<unsigned __int64>(GUEST_ES_LIMIT + segment_register * 2, segment_limit);
	hv::vmwrite<unsigned __int64>(GUEST_ES_BASE + segment_register * 2, segment_base);
	hv::vmwrite<unsigned __int64>(GUEST_ES_ACCESS_RIGHTS + segment_register * 2, segment_access_rights.all);
}

unsigned __int32 ajdust_controls(unsigned __int32 ctl, unsigned __int32 msr)
{
	__msr msr_value = { 0 };
	msr_value.all = __readmsr(msr);
	ctl &= msr_value.high;
	ctl |= msr_value.low;
	return ctl;
}

 
 
void prepare_external_structures(__vcpu* const vcpu)
{
	 
	memset(&vcpu->host_tss, 0, sizeof(vcpu->host_tss));

	hv:: prepare_host_idt(vcpu->host_idt);
	hv::prepare_host_gdt(vcpu->host_gdt, &vcpu->host_tss);
}
 
 

/// <summary>
/// Set the vmcs structure
/// </summary>
/// <param name="vcpu"></param>
/// <param name="guest_rsp"></param>
void fill_vmcs(__vcpu* vcpu, void* guest_rsp)
{
	__pseudo_descriptor64 gdtr = { 0 };
	__pseudo_descriptor64 idtr = { 0 };

	__exception_bitmap exception_bitmap = { 0 };
	__vmx_basic_msr vmx_basic = { 0 };
	__vmx_entry_control entry_controls = { 0 };
	__vmx_exit_control exit_controls = { 0 };
	__vmx_pinbased_control_msr pinbased_controls = { 0 };
	__vmx_primary_processor_based_control primary_controls = { 0 };
	__vmx_secondary_processor_based_control secondary_controls = { 0 };
	
	const unsigned __int8 selector_mask = 7;

	vmx_basic.all = __readmsr(IA32_VMX_BASIC);


	//pinbased_controls.virtual_nmis = true;
	//pinbased_controls.nmi_exiting = true;
	//pinbased_controls.vmx_preemption_timer = true;

	//primary_controls.cr3_load_exiting = true;
 
	primary_controls.use_msr_bitmaps = true;
	primary_controls.active_secondary_controls = true;
	primary_controls.mov_dr_exiting = true;
	primary_controls.use_tsc_offsetting = true;
	

	secondary_controls.descriptor_table_exiting = true;
	secondary_controls.wbinvd_exiting = true;
	secondary_controls.enable_ept = true;
	secondary_controls.enable_vpid = true;
	secondary_controls.enable_rdtscp = true;
	secondary_controls.enable_xsave_xrstor = true;
	secondary_controls.enable_invpcid = true;
	secondary_controls.conceal_vmx_from_pt = true;
	secondary_controls.rdrand_exiting = true;
	secondary_controls.rdseed_exiting = true;

	//开启的场景（true）：
	//	对抗分析 / 反调试 / 虚拟化隐藏：

	//	防止 guest 使用 PT 检测是否运行在虚拟化环境中。

	//	常见于 VM 虚拟机 rootkit、反虚拟化工具、恶意代码隐藏 VMX 痕迹等
	secondary_controls.conceal_vmx_from_pt = true;   
	secondary_controls.enable_user_wait_and_pause = true;//新加


	exit_controls.all = 0;
	exit_controls.save_dbg_controls = true;
	exit_controls.host_address_space_size = true;


	//个别机器会卡死
	//exit_controls.save_ia32_pat = true;
	//exit_controls.load_ia32_pat = true;
	//exit_controls.load_ia32_perf_global_control = true;
	 exit_controls.conceal_vmx_from_pt = true;
 

	entry_controls.all = 0;
	entry_controls.load_dbg_controls = true;
	entry_controls.ia32e_mode_guest = true;


	//个别机器会卡死
	//entry_controls.load_ia32_pat = true;
	//entry_controls.load_ia32_perf_global_control = true;
	entry_controls.conceal_vmx_from_pt = true;
	 
 

	exception_bitmap.debug = true;                   // #DB - Debug Exception (set)
	exception_bitmap.nmi_interrupt = true;           // NMI - Non-Maskable Interrupt (set)
	exception_bitmap.breakpoint = true;              // #BP - Breakpoint Exception (set)
 
	vcpu->queued_nmis = 0;
	vcpu->tsc_offset = 0;
	vcpu->preemption_timer = 0;
	vcpu->vm_exit_tsc_overhead = 0;
	vcpu->vm_exit_mperf_overhead = 0;
	vcpu->vm_exit_ref_tsc_overhead = 0;
	 

	//
   // ******* Time-stamp counter offset *******
   //

 
	hv::vmwrite(VMCS_CTRL_TSC_OFFSET, 0);
	hv::vmwrite(VMCS_CTRL_PAGEFAULT_ERROR_CODE_MASK, 0);
	hv::vmwrite(VMCS_CTRL_PAGEFAULT_ERROR_CODE_MATCH, 0);
	 
 
 


	 
	hv::vmwrite(VMCS_CTRL_VMEXIT_MSR_STORE_COUNT, 0);
	hv::vmwrite(VMCS_CTRL_VMEXIT_MSR_LOAD_COUNT, 0);
	 

	hv::vmwrite(VMCS_CTRL_VMENTRY_MSR_LOAD_COUNT, 0);
  
	hv::vmwrite(VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD, 0);
	hv::vmwrite(VMCS_CTRL_VMENTRY_EXCEPTION_ERROR_CODE, 0);
	hv::vmwrite(VMCS_CTRL_VMENTRY_INSTRUCTION_LENGTH, 0);
	 

	hv::vmwrite <unsigned __int64>(VMCS_CTRL_TPR_THRESHOLD, 0);
 
 
#ifndef _MINIMAL
	//memset(vcpu->vcpu_bitmaps.msr_bitmap, 0xff, PAGE_SIZE);
#endif

	//
	// Msr bitmap controls which operation on which msr
	// in range of 0x00000000 to 0x00001FFF or 
	// in range of 0xC0000000 to 0xC0001FFF cause a vmexit

	//
	// Set single msr
     hv::set_msr_bitmap(IA32_FEATURE_CONTROL, vcpu, true, false, true);

	//
	// Only if your upper hypervisor is vmware
	// Because Vmware tools use ports 0x5655,0x5656,0x5657,0x5658,0x5659,0x565a,0x565b,0x1090,0x1094 as I/O backdoor
	//hv::set_io_bitmap(0x5655, vcpu, false);
	//hv::set_io_bitmap(0x5656, vcpu, false);
	//hv::set_io_bitmap(0x5657, vcpu, false);
	//hv::set_io_bitmap(0x5658, vcpu, false);
	//hv::set_io_bitmap(0x5659, vcpu, false);
	//hv::set_io_bitmap(0x565a, vcpu, false);
	//hv::set_io_bitmap(0x565b, vcpu, false);
	//hv::set_io_bitmap(0x1094, vcpu, false);
	//hv::set_io_bitmap(0x1090, vcpu, false);

	__vmx_vmclear((unsigned __int64*)&vcpu->vmcs_physical);
	__vmx_vmptrld((unsigned __int64*)&vcpu->vmcs_physical);

	__sgdt(&gdtr);
	__sidt(&idtr);

	// Global descriptor table and local one
	hv::vmwrite<unsigned __int64>(GUEST_GDTR_LIMIT, gdtr.limit);
	hv::vmwrite<unsigned __int64>(GUEST_IDTR_LIMIT, idtr.limit);
	hv::vmwrite<unsigned __int64>(GUEST_GDTR_BASE, gdtr.base_address);
	hv::vmwrite<unsigned __int64>(GUEST_IDTR_BASE, idtr.base_address);


	// Hypervisor features
	hv::vmwrite<unsigned __int64>(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, ajdust_controls(pinbased_controls.all, vmx_basic.true_controls ? IA32_VMX_TRUE_PINBASED_CTLS : IA32_VMX_PINBASED_CTLS));
	hv::vmwrite<unsigned __int64>(CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, ajdust_controls(primary_controls.all, vmx_basic.true_controls ? IA32_VMX_TRUE_PROCBASED_CTLS : IA32_VMX_PROCBASED_CTLS));
	hv::vmwrite<unsigned __int64>(CONTROL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, ajdust_controls(secondary_controls.all, IA32_VMX_PROCBASED_CTLS2));
	hv::vmwrite<unsigned __int64>(CONTROL_VM_EXIT_CONTROLS, ajdust_controls(exit_controls.all, vmx_basic.true_controls ? IA32_VMX_TRUE_EXIT_CTLS : IA32_VMX_EXIT_CTLS));
	hv::vmwrite<unsigned __int64>(CONTROL_VM_ENTRY_CONTROLS, ajdust_controls(entry_controls.all, vmx_basic.true_controls ? IA32_VMX_TRUE_ENTRY_CTLS : IA32_VMX_ENTRY_CTLS));

	// Segments
	fill_guest_selector_data((void*)gdtr.base_address, ES, __read_es());
	fill_guest_selector_data((void*)gdtr.base_address, CS, __read_cs());
	fill_guest_selector_data((void*)gdtr.base_address, SS, __read_ss());
	fill_guest_selector_data((void*)gdtr.base_address, DS, __read_ds());
	fill_guest_selector_data((void*)gdtr.base_address, FS, __read_fs());
	fill_guest_selector_data((void*)gdtr.base_address, GS, __read_gs());
	fill_guest_selector_data((void*)gdtr.base_address, LDTR, __read_ldtr());
	fill_guest_selector_data((void*)gdtr.base_address, TR, __read_tr());
	hv::vmwrite<unsigned __int64>(GUEST_FS_BASE, __readmsr(IA32_FS_BASE));
	hv::vmwrite<unsigned __int64>(GUEST_GS_BASE, __readmsr(IA32_GS_BASE));


	// only vm-exit when guest tries to change a reserved bit
	hv::vmwrite(VMCS_CTRL_CR0_GUEST_HOST_MASK,
		vcpu->cached.vmx_cr0_fixed0 | ~vcpu->cached.vmx_cr0_fixed1 |
		CR0_CACHE_DISABLE_FLAG | CR0_WRITE_PROTECT_FLAG);


	hv::vmwrite(VMCS_CTRL_CR4_GUEST_HOST_MASK,
		vcpu->cached.vmx_cr4_fixed0 | ~vcpu->cached.vmx_cr4_fixed1);

 
	hv::vmwrite(VMCS_CTRL_CR0_READ_SHADOW, __readcr0());
	hv::vmwrite(VMCS_CTRL_CR4_READ_SHADOW, __readcr4() & ~CR4_VMX_ENABLE_FLAG);


	hv::vmwrite<unsigned __int64>(VMCS_CTRL_CR3_TARGET_COUNT,0);
	//hv::vmwrite(VMCS_CTRL_CR3_TARGET_VALUE_0, hv::get_system_directory_table_base());

	hv::vmwrite<unsigned __int64>(GUEST_CR0, __readcr0());
	hv::vmwrite<unsigned __int64>(GUEST_CR3, __readcr3());
	hv::vmwrite<unsigned __int64>(GUEST_CR4, __readcr4());





 
	// Cr registers
	 
	hv::vmwrite<unsigned __int64>(HOST_CS_SELECTOR, __read_cs() & ~selector_mask);
	hv::vmwrite<unsigned __int64>(HOST_SS_SELECTOR, __read_ss() & ~selector_mask);
	hv::vmwrite<unsigned __int64>(HOST_DS_SELECTOR, __read_ds() & ~selector_mask);
	hv::vmwrite<unsigned __int64>(HOST_ES_SELECTOR, __read_es() & ~selector_mask);
	hv::vmwrite<unsigned __int64>(HOST_FS_SELECTOR, __read_fs() & ~selector_mask);
	hv::vmwrite<unsigned __int64>(HOST_GS_SELECTOR, __read_gs() & ~selector_mask);
	hv::vmwrite<unsigned __int64>(HOST_TR_SELECTOR, __read_tr() & ~selector_mask);
	 
 
	hv::vmwrite<unsigned __int64>(HOST_FS_BASE, __readmsr(IA32_FS_BASE));
	hv::vmwrite<unsigned __int64>(HOST_GS_BASE, __readmsr(IA32_GS_BASE));
	hv::vmwrite<unsigned __int64>(HOST_TR_BASE, get_segment_base(__read_tr(),(unsigned char*)gdtr.base_address));
	hv::vmwrite<unsigned __int64>(HOST_GDTR_BASE, gdtr.base_address);
	hv::vmwrite<unsigned __int64>(HOST_IDTR_BASE, idtr.base_address);
	 
	hv::vmwrite<unsigned __int64>(HOST_CR0, __readcr0());
	hv::vmwrite<unsigned __int64>(HOST_CR3, hv::get_system_directory_table_base());
	hv::vmwrite<unsigned __int64>(HOST_CR4, __readcr4());

	  

	// Debug register
	hv::vmwrite<unsigned __int64>(GUEST_DR7, __readdr(7));

	// RFLAGS
	hv::vmwrite<unsigned __int64>(GUEST_RFLAGS, __readeflags());

	// RSP and RIP
	hv::vmwrite<void*>(GUEST_RSP, guest_rsp);
	hv::vmwrite<void*>(GUEST_RIP, vmx_restore_state);
	hv::vmwrite<unsigned __int64>(HOST_RSP, (unsigned __int64)vcpu->vmm_stack + VMM_STACK_SIZE);
	hv::vmwrite<void*>(HOST_RIP, vmm_entrypoint);

	// MSRS Guest
	hv::vmwrite<unsigned __int64>(GUEST_DEBUG_CONTROL, __readmsr(IA32_DEBUGCTL));
	hv::vmwrite<unsigned __int64>(GUEST_SYSENTER_CS, __readmsr(IA32_SYSENTER_CS));
	hv::vmwrite<unsigned __int64>(GUEST_SYSENTER_ESP, __readmsr(IA32_SYSENTER_ESP));
	hv::vmwrite<unsigned __int64>(GUEST_SYSENTER_EIP, __readmsr(IA32_SYSENTER_EIP));
	 
	//hv::vmwrite(VMCS_GUEST_PAT, __readmsr(IA32_PAT));
//	hv::vmwrite(VMCS_GUEST_PERF_GLOBAL_CTRL, __readmsr(IA32_PERF_GLOBAL_CTRL));
	

	// MSRS Host
	hv::vmwrite<unsigned __int64>(HOST_SYSENTER_CS, __readmsr(IA32_SYSENTER_CS));
	hv::vmwrite<unsigned __int64>(HOST_SYSENTER_ESP, __readmsr(IA32_SYSENTER_ESP));
	hv::vmwrite<unsigned __int64>(HOST_SYSENTER_EIP, __readmsr(IA32_SYSENTER_EIP));
	  
	//个别机器会卡死
	/*ia32_pat_register host_pat;
	host_pat.flags = 0;
	host_pat.pa0 = MEMORY_TYPE_WRITE_BACK;
	host_pat.pa1 = MEMORY_TYPE_WRITE_THROUGH;
	host_pat.pa2 = MEMORY_TYPE_UNCACHEABLE_MINUS;
	host_pat.pa3 = MEMORY_TYPE_UNCACHEABLE;
	host_pat.pa4 = MEMORY_TYPE_WRITE_BACK;
	host_pat.pa5 = MEMORY_TYPE_WRITE_THROUGH;
	host_pat.pa6 = MEMORY_TYPE_UNCACHEABLE_MINUS;
	host_pat.pa7 = MEMORY_TYPE_UNCACHEABLE;
	hv::vmwrite(VMCS_HOST_PAT, host_pat.flags);*/
	//hv::vmwrite(VMCS_HOST_PERF_GLOBAL_CTRL, 0);


	// Features
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_VMCS_LINK_POINTER, ~0ULL);

	hv::vmwrite<unsigned __int64>(VMCS_CTRL_EXCEPTION_BITMAP, exception_bitmap.all);

	if (primary_controls.use_msr_bitmaps == true)
		hv::vmwrite<unsigned __int64>(VMCS_CTRL_MSR_BITMAP_ADDRESS, vcpu->vcpu_bitmaps.msr_bitmap_physical);

	if (primary_controls.use_io_bitmaps == true)
	{
		hv::vmwrite<unsigned __int64>(CONTROL_BITMAP_IO_A_ADDRESS, vcpu->vcpu_bitmaps.io_bitmap_a_physical);
		hv::vmwrite<unsigned __int64>(CONTROL_BITMAP_IO_B_ADDRESS, vcpu->vcpu_bitmaps.io_bitmap_b_physical);
	}

	if (secondary_controls.enable_vpid == true)
		hv::vmwrite<unsigned __int64>(CONTROL_VIRTUAL_PROCESSOR_IDENTIFIER, KeGetCurrentProcessorNumberEx(NULL) + 1);

	if (secondary_controls.enable_ept == true && secondary_controls.enable_vpid == true)
		hv::vmwrite<unsigned __int64>(CONTROL_EPT_POINTER, vcpu->ept_state->ept_pointer->all);

	//hv::vmwrite(VMCS_GUEST_ACTIVITY_STATE, vmx_active);
	hv::vmwrite(VMCS_GUEST_VMX_PREEMPTION_TIMER_VALUE, MAXULONG64);
}