#pragma warning( disable : 4201 4244)
#include "../utils/global_defs.h"
#include "vmcs.h"
  



/// <summary>
/// Derived from Intel Manuals Voulme 3 Section 24.6.2 Table 24-6. Definitions of Primary Processor-Based VM-Execution Controls
/// </summary>
/// <param name="primary_controls"></param>
void set_primary_controls(__vmx_primary_processor_based_control& primary_controls)
{
	/**
	* If this control is 1, a VM exit occurs at the beginning of any instruction if RFLAGS.IF = 1 and
	* there are no other blocking of interrupts (see Section 24.4.2).
	*/
	primary_controls.interrupt_window_exiting = false;

	/**
	* This control determines whether executions of RDTSC, executions of RDTSCP, and executions
	* of RDMSR that read from the IA32_TIME_STAMP_COUNTER MSR return a value modified by
	* the TSC offset field (see Section 24.6.5 and Section 25.3).
	*/
	primary_controls.use_tsc_offsetting = false;

	/**
	* This control determines whether executions of HLT cause VM exits.
	*/
	primary_controls.hlt_exiting = false;

	/**
	* This determines whether executions of INVLPG cause VM exits.
	*/

#ifdef _MINIMAL
	primary_controls.invldpg_exiting = false;
#else
	primary_controls.invldpg_exiting = true;
#endif

	/**
	* This control determines whether executions of MWAIT cause VM exits.
	*/
	primary_controls.mwait_exiting = false;

	/**
	* This control determines whether executions of RDPMC cause VM exits.
	*/
	primary_controls.rdpmc_exiting = false;

	/**
	* This control determines whether executions of RDTSC and RDTSCP cause VM exits.
	*/
#ifdef _MINIMAL
	primary_controls.rdtsc_exiting = false;
#else
	primary_controls.rdtsc_exiting = true;
#endif

	/**
	* In conjunction with the CR3-target controls (see Section 24.6.7), this control determines
	* whether executions of MOV to CR3 cause VM exits. See Section 25.1.3.
	* The first processors to support the virtual-machine extensions supported only the 1-setting
	* of this control.
	*/
#ifdef _MINIMAL
	primary_controls.cr3_load_exiting = false;
#else
	primary_controls.cr3_load_exiting = true;
#endif

	/**
	* This control determines whether executions of MOV from CR3 cause VM exits.
	* The first processors to support the virtual-machine extensions supported only the 1-setting
	* of this control.
	*/
#ifdef _MINIMAL
	primary_controls.cr3_store_exiting = false;
#else
	primary_controls.cr3_store_exiting = true;
#endif

	/**
	* This control determines whether executions of MOV to CR8 cause VM exits.
	*/
	primary_controls.cr8_load_exiting = false;

	/**
	* This control determines whether executions of MOV from CR8 cause VM exits.
	*/
	primary_controls.cr8_store_exiting = false;

	/**
	* Setting this control to 1 enables TPR virtualization and other APIC-virtualization features. See
	* Chapter 29.
	*/
	primary_controls.use_tpr_shadow = false;

	/**
	* If this control is 1, a VM exit occurs at the beginning of any instruction if there is no virtual-
	* NMI blocking (see Section 24.4.2).
	*/
	primary_controls.nmi_window_exiting = false;

	/**
	* This control determines whether executions of MOV DR cause VM exits.
	*/
#ifdef _MINIMAL
	primary_controls.mov_dr_exiting = false;
#else
	primary_controls.mov_dr_exiting = true;
#endif

	/**
	* This control determines whether executions of I/O instructions (IN, INS/INSB/INSW/INSD, OUT,
	* and OUTS/OUTSB/OUTSW/OUTSD) cause VM exits.
	*/
	primary_controls.unconditional_io_exiting = false;

	/**
	* This control determines whether I/O bitmaps are used to restrict executions of I/O instructions
	(see Section 24.6.4 and Section 25.1.3).
	For this control, ??means 揹o not use I/O bitmaps?and ??means 搖se I/O bitmaps.?If the I/O
	bitmaps are used, the setting of the 搖nconditional I/O exiting?control is ignored
	*/
#ifdef _MINIMAL
	primary_controls.use_io_bitmaps = true;
#else
	primary_controls.use_io_bitmaps = true;
#endif

	/**
	* If this control is 1, the monitor trap flag debugging feature is enabled. See Section 25.5.2.
	*/
	primary_controls.monitor_trap_flag = false;

	/**
	* This control determines whether MSR bitmaps are used to control execution of the RDMSR
	* and WRMSR instructions (see Section 24.6.9 and Section 25.1.3).
	* For this control, ??means 揹o not use MSR bitmaps?and ??means 搖se MSR bitmaps.?If the
	* MSR bitmaps are not used, all executions of the RDMSR and WRMSR instructions cause
	* VM exits.
	*/
	primary_controls.use_msr_bitmaps =true;

	/**
	* This control determines whether executions of MONITOR cause VM exits.
	*/
	primary_controls.monitor_exiting = false;

	/**
	* This control determines whether executions of PAUSE cause VM exits.
	*/
	primary_controls.pause_exiting = false;

	/**
	* This control determines whether the secondary processor-based VM-execution controls are
	* used. If this control is 0, the logical processor operates as if all the secondary processor-based
	* VM-execution controls were also 0.
	*/
	primary_controls.active_secondary_controls = true;
}

/// <summary>
/// Derived from Intel Manuals Voulme 3 Section 24.6.2 Table 24-7. Definitions of Secondary Processor-Based VM-Execution Controls
/// </summary>
/// <param name="secondary_controls"></param>
void set_secondary_controls(__vmx_secondary_processor_based_control& secondary_controls)
{
	/**
	* If this control is 1, the logical processor treats specially accesses to the page with the APIC-
	* access address. See Section 29.4.
	*/
	secondary_controls.virtualize_apic_accesses = false;

	/**
	* If this control is 1, extended page tables (EPT) are enabled. See Section 28.2.
	*/
	secondary_controls.enable_ept = true;

	/**
	* This control determines whether executions of LGDT, LIDT, LLDT, LTR, SGDT, SIDT, SLDT, and
	* STR cause VM exits.
	*/
#ifdef _MINIMAL
	secondary_controls.descriptor_table_exiting = false;
#else
	secondary_controls.descriptor_table_exiting = true;
#endif

	/**
	* If this control is 0, any execution of RDTSCP causes an invalid-opcode exception (#UD).
	*/
	secondary_controls.enable_rdtscp = true;

	/**
	* If this control is 1, the logical processor treats specially RDMSR and WRMSR to APIC MSRs (in
	* the range 800H?FFH). See Section 29.5.
	*/
	secondary_controls.virtualize_x2apic = false;

	/**
	* If this control is 1, cached translations of linear addresses are associated with a virtual-
	* processor identifier (VPID). See Section 28.1.
	*/
	secondary_controls.enable_vpid = true;

	/**
	* This control determines whether executions of WBINVD cause VM exits.
	*/
#ifdef _MINIMAL
	secondary_controls.wbinvd_exiting = false;
#else
	secondary_controls.wbinvd_exiting = true;
#endif

	/**
	* This control determines whether guest software may run in unpaged protected mode or in real-
	* address mode.
	*/
	secondary_controls.unrestricted_guest = false;

	/**
	* If this control is 1, the logical processor virtualizes certain APIC accesses. See Section 29.4 and
	* Section 29.5.
	*/
	secondary_controls.apic_register_virtualization = false;

	/**
	* This controls enables the evaluation and delivery of pending virtual interrupts as well as the
	* emulation of writes to the APIC registers that control interrupt prioritization.
	*/
	secondary_controls.virtual_interrupt_delivery = false;

	/**
	* This control determines whether a series of executions of PAUSE can cause a VM exit (see
	* Section 24.6.13 and Section 25.1.3).
	*/
	secondary_controls.pause_loop_exiting = false;

	/**
	* This control determines whether executions of RDRAND cause VM exits.
	*/
#ifdef _MINIMAL
	secondary_controls.rdrand_exiting = false;
#else
	secondary_controls.rdrand_exiting = true;
#endif

	/**
	* If this control is 0, any execution of INVPCID causes a #UD.
	*/
	secondary_controls.enable_invpcid = true;

	/**
	* Setting this control to 1 enables use of the VMFUNC instruction in VMX non-root operation. See
	* Section 25.5.6.
	*/
	secondary_controls.enable_vmfunc = false;

	/**
	* If this control is 1, executions of VMREAD and VMWRITE in VMX non-root operation may access
	* a shadow VMCS (instead of causing VM exits). See Section 24.10 and Section 30.3.
	*/
	secondary_controls.vmcs_shadowing = false;

	/**
	* If this control is 1, executions of ENCLS consult the ENCLS-exiting bitmap to determine whether
	* the instruction causes a VM exit. See Section 24.6.16 and Section 25.1.3.
	*/
	secondary_controls.enable_encls_exiting = false;

	/**
	* This control determines whether executions of RDSEED cause VM exits.
	*/
#ifdef _MINIMAL
	secondary_controls.rdseed_exiting = false;
#else
	secondary_controls.rdseed_exiting = true;
#endif

	/**
	* If this control is 1, an access to a guest-physical address that sets an EPT dirty bit first adds an
	* entry to the page-modification log. See Section 28.2.6.
	*/
	secondary_controls.enable_pml = false;

	/**
	* If this control is 1, EPT violations may cause virtualization exceptions (#VE) instead of VM exits.
	* See Section 25.5.7.
	*/
	secondary_controls.use_virtualization_exception = false;

	/**
	* If this control is 1, Intel Processor Trace suppresses from PIPs an indication that the processor
	* was in VMX non-root operation and omits a VMCS packet from any PSB+ produced in VMX non-
	* root operation (see Chapter 35).
	*/
	secondary_controls.conceal_vmx_from_pt = true;

	/**
	* If this control is 0, any execution of XSAVES or XRSTORS causes a #UD.
	*/
	secondary_controls.enable_xsave_xrstor = true;

	/**
	* If this control is 1, EPT execute permissions are based on whether the linear address being
	* accessed is supervisor mode or user mode. See Chapter 28.
	*/
	secondary_controls.mode_based_execute_control_ept = false;

	/**
	* This control determines whether executions of RDTSC, executions of RDTSCP, and executions
	* of RDMSR that read from the IA32_TIME_STAMP_COUNTER MSR return a value modified by the
	* TSC multiplier field (see Section 24.6.5 and Section 25.3).
	*/
	secondary_controls.sub_page_write_permission_for_ept = false;

	/**
	* This control determines whether executions of RDTSC, executions of RDTSCP, and executions
	* of RDMSR that read from the IA32_TIME_STAMP_COUNTER MSR return a value modified by the
	* TSC multiplier field (see Section 24.6.5 and Section 25.3).
	*/
	secondary_controls.intel_pt_uses_guest_physical_address = false;

	/**
	* This control determines whether executions of RDTSC, executions of RDTSCP, and executions
	* of RDMSR that read from the IA32_TIME_STAMP_COUNTER MSR return a value modified by the
	* TSC multiplier field (see Section 24.6.5 and Section 25.3).
	*/
	secondary_controls.use_tsc_scaling = false;

	/**
	* If this control is 0, any execution of TPAUSE, UMONITOR, or UMWAIT causes a #UD.
	*/
	secondary_controls.enable_user_wait_and_pause = false;

	/**
	* If this control is 1, executions of ENCLV consult the ENCLV-exiting bitmap to determine whether
	* the instruction causes a VM exit. See Section 24.6.17 and Section 25.1.3.
	*/
	secondary_controls.enable_enclv_exiting = false;
}

/// <summary>
/// Derived from Intel Manuals Voulme 3 Section 24.8.1 Table 24-13. Definitions of VM-Entry Controls
/// </summary>
/// <param name="entry_control"></param>
void set_entry_control(__vmx_entry_control& entry_control)
{
	/**
	* This control determines whether DR7 and the IA32_DEBUGCTL MSR are loaded on VM entry.
	* The first processors to support the virtual-machine extensions supported only the 1-setting of
	* this control.
	*/
	entry_control.load_dbg_controls = false;

	/**
	* On processors that support Intel 64 architecture, this control determines whether the logical
	* processor is in IA-32e mode after VM entry. Its value is loaded into IA32_EFER.LMA as part of
	* VM entry. 1
	* This control must be 0 on processors that do not support Intel 64 architecture.
	*/
	entry_control.ia32e_mode_guest = true;

	/**
	* This control determines whether the logical processor is in system-management mode (SMM)
	* after VM entry. This control must be 0 for any VM entry from outside SMM.
	*/
	entry_control.entry_to_smm = false;

	/**
	* If set to 1, the default treatment of SMIs and SMM is in effect after the VM entry (see Section
	* 34.15.7). This control must be 0 for any VM entry from outside SMM.
	*/
	entry_control.deactivate_dual_monitor_treament = false;

	/**
	* This control determines whether the IA32_PERF_GLOBAL_CTRL MSR is loaded on VM entry.
	*/
	entry_control.load_ia32_perf_global_control = false;

	/**
	* This control determines whether the IA32_PAT MSR is loaded on VM entry.
	*/
	entry_control.load_ia32_pat = false;

	/**
	* This control determines whether the IA32_EFER MSR is loaded on VM entry.
	*/
	entry_control.load_ia32_efer = false;

	/**
	* This control determines whether the IA32_BNDCFGS MSR is loaded on VM entry.
	*/
	entry_control.load_ia32_bndcfgs = false;

	/**
	* If this control is 1, Intel Processor Trace does not produce a paging information packet (PIP) on
	* a VM entry or a VMCS packet on a VM entry that returns from SMM (see Chapter 35).
	*/
	entry_control.conceal_vmx_from_pt = false;

	/**
	* This control determines whether the IA32_RTIT_CTL MSR is loaded on VM entry.
	*/
	entry_control.load_ia32_rtit_ctl = false;

	/**
	* This control determines whether CET-related MSRs and SPP are loaded on VM entry.
	*/
	entry_control.load_cet_state = false;

	/**
	* This control determines whether CET-related MSRs and SPP are loaded on VM entry.
	*/
	entry_control.load_pkrs = false;
}

/// <summary>
/// Derived from Intel Manuals Voulme 3 Section 24.7.1 Table 24-11. Definitions of VM-Exit Controls
/// </summary>
/// <param name="exit_control"></param>
void set_exit_control(__vmx_exit_control& exit_control)
{
	/**
	* This control determines whether DR7 and the IA32_DEBUGCTL MSR are saved on VM exit.
	* The first processors to support the virtual-machine extensions supported only the 1-
	* setting of this control.
	*/
	exit_control.save_dbg_controls = false;

	/**
	* On processors that support Intel 64 architecture, this control determines whether a logical
	* processor is in 64-bit mode after the next VM exit. Its value is loaded into CS.L,
	* IA32_EFER.LME, and IA32_EFER.LMA on every VM exit. 1
	* This control must be 0 on processors that do not support Intel 64 architecture.
	*/
	exit_control.host_address_space_size = true;

	/**
	* This control determines whether the IA32_PERF_GLOBAL_CTRL MSR is loaded on VM exit.
	*/
	exit_control.load_ia32_perf_global_control = false;

	/**
	* This control affects VM exits due to external interrupts:
	* ?If such a VM exit occurs and this control is 1, the logical processor acknowledges the
	*   interrupt controller, acquiring the interrupt抯 vector. The vector is stored in the VM-exit
	*   interruption-information field, which is marked valid.
	* ?If such a VM exit occurs and this control is 0, the interrupt is not acknowledged and the
	*   VM-exit interruption-information field is marked invalid.
	*/
	exit_control.ack_interrupt_on_exit = true;

	/**
	* This control determines whether the IA32_PAT MSR is saved on VM exit.
	*/
	exit_control.save_ia32_pat = false;

	/**
	* This control determines whether the IA32_PAT MSR is loaded on VM exit.
	*/
	exit_control.load_ia32_pat = false;

	/**
	* This control determines whether the IA32_EFER MSR is saved on VM exit.
	*/
	exit_control.save_ia32_efer = false;

	/**
	* This control determines whether the IA32_EFER MSR is loaded on VM exit.
	*/
	exit_control.load_ia32_efer = false;

	/**
	* This control determines whether the value of the VMX-preemption timer is saved on
	* VM exit.
	*/
	exit_control.save_vmx_preemption_timer_value = false;

	/**
	* This control determines whether the IA32_BNDCFGS MSR is cleared on VM exit.
	*/
	exit_control.clear_ia32_bndcfgs = false;

	/**
	* If this control is 1, Intel Processor Trace does not produce a paging information packet (PIP)
	* on a VM exit or a VMCS packet on an SMM VM exit (see Chapter 35).
	*/
	exit_control.conceal_vmx_from_pt = false;

	/**
	* This control determines whether the IA32_RTIT_CTL MSR is cleared on VM exit.
	*/
	exit_control.load_ia32_rtit_ctl = false;

	/**
	* This control determines whether CET-related MSRs and SPP are loaded on VM exit.
	*/
	exit_control.load_cet_state = false;

	/**
	* This control determines whether the IA32_PKRS MSR is loaded on VM exit.
	*/
	exit_control.load_pkrs = false;
}

/// <summary>
/// Derived from Intel Manuals Voulme 3 Section 24.6.1 Table 24-5. Definitions of Pin-Based VM-Execution Controls
/// </summary>
/// <param name="pinbased_controls"></param>
void set_pinbased_control_msr(__vmx_pinbased_control_msr& pinbased_controls)
{
	/**
	* If this control is 1, external interrupts cause VM exits. Otherwise, they are delivered normally
	* through the guest interrupt-descriptor table (IDT). If this control is 1, the value of RFLAGS.IF
	* does not affect interrupt blocking.
	*/
	pinbased_controls.external_interrupt_exiting = false;

	/**
	* If this control is 1, non-maskable interrupts (NMIs) cause VM exits. Otherwise, they are
	* delivered normally using descriptor 2 of the IDT. This control also determines interactions
	* between IRET and blocking by NMI (see Section 25.3).
	*/
	pinbased_controls.nmi_exiting = true;

	/**
	* If this control is 1, NMIs are never blocked and the 揵locking by NMI?bit (bit 3) in the
	* interruptibility-state field indicates 搗irtual-NMI blocking?(see Table 24-3). This control also
	* interacts with the 揘MI-window exiting?VM-execution control (see Section 24.6.2).
	*/
	pinbased_controls.virtual_nmis = true;

	/**
	* If this control is 1, the VMX-preemption timer counts down in VMX non-root operation; see
	* Section 25.5.1. A VM exit occurs when the timer counts down to zero; see Section 25.2.
	*/
	pinbased_controls.vmx_preemption_timer = false;

	/**
	* If this control is 1, the processor treats interrupts with the posted-interrupt notification vector
	* (see Section 24.6.8) specially, updating the virtual-APIC page with posted-interrupt requests
	* (see Section 29.6).
	*/
	pinbased_controls.process_posted_interrupts = false;
}

/// <summary>
/// Set which exception cause vmexit
/// </summary>
/// <param name="exception_bitmap"></param>
void set_exception_bitmap(__exception_bitmap& exception_bitmap)
{
	exception_bitmap.divide_error = false;

	exception_bitmap.debug = true;

	exception_bitmap.nmi_interrupt = true;

	exception_bitmap.breakpoint = false;

	exception_bitmap.overflow = false;

	exception_bitmap.bound = false;

	exception_bitmap.invalid_opcode = false;

	exception_bitmap.coprocessor_segment_overrun = false;

	exception_bitmap.invalid_tss = false;

	exception_bitmap.segment_not_present = false;

	exception_bitmap.stack_segment_fault = false;

	exception_bitmap.general_protection = false;

	exception_bitmap.page_fault = false;

	exception_bitmap.x87_floating_point_error = false;

	exception_bitmap.alignment_check = false;

	exception_bitmap.machine_check = false;

	exception_bitmap.simd_floating_point_error = false;

	exception_bitmap.virtualization_exception = false;
}
 
 
 
  

void fill_host_structures(__vcpu* cpu)
{
	memset(&cpu->host_tss, 0, sizeof(cpu->host_tss));

	hv::prepare_host_idt(cpu->host_idt);
	hv::prepare_host_gdt(cpu->host_gdt, &cpu->host_tss);
}

 
void fill_vmcs(__vcpu* vcpu, void* guest_rsp)
{		
			
	// 1. 清理并加载 VMCS（确保当前 VCPU 使用自己的 VMCS）
	__vmx_vmclear((unsigned __int64*)&vcpu->vmcs_physical);
	__vmx_vmptrld((unsigned __int64*)&vcpu->vmcs_physical);
		
	// 2. 配置控制字段（执行控制、退出控制、进入控制等）
	fill_vmcs_control_fields(vcpu);

	// 3. 配置杂项字段（调试控制、异常位图等）
	fill_vmcs_misc_fields(vcpu);

	// 4. 初始化外部 Host 结构（GDT / IDT / TSS）
	fill_host_structures(vcpu);

	// 5. 填充 Guest 状态字段（寄存器、RSP、段寄存器、CRx）
	fill_vmcs_guest_fields(vcpu, guest_rsp);

	// 6. 填充 Host 状态字段（寄存器、RSP、段寄存器、MSR 基址等）
	fill_vmcs_host_fields(vcpu);
}

void fill_vmcs_control_fields(__vcpu* vcpu)
{
	__exception_bitmap exception_bitmap = { 0 };
	ia32_vmx_basic_register vmx_basic = { 0 };
	__vmx_entry_control entry_controls = { 0 };
	__vmx_exit_control exit_controls = { 0 };
	__vmx_pinbased_control_msr pinbased_controls = { 0 };
	__vmx_primary_processor_based_control primary_controls = { 0 };
	__vmx_secondary_processor_based_control secondary_controls = { 0 };



	vmx_basic.flags = __readmsr(IA32_VMX_BASIC);


	set_entry_control(entry_controls);

	set_exit_control(exit_controls);

	set_primary_controls(primary_controls);

	set_secondary_controls(secondary_controls);

	set_exception_bitmap(exception_bitmap);

	set_pinbased_control_msr(pinbased_controls);


	vcpu->queued_nmis = 0;
	vcpu->tsc_offset = 0;
	vcpu->preemption_timer = 0;
	vcpu->vm_exit_tsc_overhead = 0;
	vcpu->vm_exit_mperf_overhead = 0;
	vcpu->vm_exit_ref_tsc_overhead = 0;


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


	__vmx_vmclear((unsigned __int64*)&vcpu->vmcs_physical);
	__vmx_vmptrld((unsigned __int64*)&vcpu->vmcs_physical);

	// Hypervisor features
	hv::vmwrite<unsigned __int64>(VMCS_CTRL_PIN_BASED_VM_EXECUTION_CONTROLS, ajdust_controls(pinbased_controls.all, vmx_basic.vmx_controls ? IA32_VMX_TRUE_PINBASED_CTLS : IA32_VMX_PINBASED_CTLS));
	hv::vmwrite<unsigned __int64>(VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, ajdust_controls(primary_controls.all, vmx_basic.vmx_controls ? IA32_VMX_TRUE_PROCBASED_CTLS : IA32_VMX_PROCBASED_CTLS));
	hv::vmwrite<unsigned __int64>(VMCS_CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, ajdust_controls(secondary_controls.all, IA32_VMX_PROCBASED_CTLS2));
	hv::vmwrite<unsigned __int64>(VMCS_CTRL_PRIMARY_VMEXIT_CONTROLS, ajdust_controls(exit_controls.all, vmx_basic.vmx_controls ? IA32_VMX_TRUE_EXIT_CTLS : IA32_VMX_EXIT_CTLS));
	hv::vmwrite<unsigned __int64>(VMCS_CTRL_VMENTRY_CONTROLS, ajdust_controls(entry_controls.all, vmx_basic.vmx_controls ? IA32_VMX_TRUE_ENTRY_CTLS : IA32_VMX_ENTRY_CTLS));
	hv::vmwrite<unsigned __int64>(VMCS_CTRL_EXCEPTION_BITMAP, exception_bitmap.all);


	// only vm-exit when guest tries to change a reserved bit
	hv::vmwrite(VMCS_CTRL_CR0_GUEST_HOST_MASK,
		vcpu->cached.vmx_cr0_fixed0 | ~vcpu->cached.vmx_cr0_fixed1 |
		CR0_CACHE_DISABLE_FLAG | CR0_WRITE_PROTECT_FLAG);


	hv::vmwrite(VMCS_CTRL_CR4_GUEST_HOST_MASK,
		vcpu->cached.vmx_cr4_fixed0 | ~vcpu->cached.vmx_cr4_fixed1);


	hv::vmwrite(VMCS_CTRL_CR0_READ_SHADOW, __readcr0());
	hv::vmwrite(VMCS_CTRL_CR4_READ_SHADOW, __readcr4() & ~CR4_VMX_ENABLE_FLAG);


	if (vcpu->cached.vmx_misc.cr3_target_count >= 1)
	{
		hv::vmwrite(VMCS_CTRL_CR3_TARGET_COUNT, 1);
		hv::vmwrite(VMCS_CTRL_CR3_TARGET_VALUE_0, g_vmm_context->system_cr3.flags);
	}
	else
	{
		hv::vmwrite(VMCS_CTRL_CR3_TARGET_COUNT, 0);
	}


	if (primary_controls.use_msr_bitmaps == true)
	{
		hv::vmwrite<unsigned __int64>(VMCS_CTRL_MSR_BITMAP_ADDRESS, vcpu->vcpu_bitmaps.msr_bitmap_physical);

	}

	if (primary_controls.use_io_bitmaps == true)
	{
		hv::vmwrite<unsigned __int64>(CONTROL_BITMAP_IO_A_ADDRESS,0);
		hv::vmwrite<unsigned __int64>(CONTROL_BITMAP_IO_B_ADDRESS,0);
	}


	// EPT and VPID

	if (secondary_controls.enable_vpid == true)
		hv::vmwrite<unsigned __int64>(VMCS_CTRL_VIRTUAL_PROCESSOR_IDENTIFIER, 1);

	if (secondary_controls.enable_ept == true && secondary_controls.enable_vpid == true)
		hv::vmwrite<unsigned __int64>(VMCS_CTRL_EPT_POINTER, vcpu->ept_state->ept_pointer->all);
}

void fill_vmcs_misc_fields(__vcpu* vcpu)
{
	hv::vmwrite(VMCS_CTRL_TSC_OFFSET, 0);

	hv::vmwrite(VMCS_CTRL_PAGEFAULT_ERROR_CODE_MASK, 0);
	hv::vmwrite(VMCS_CTRL_PAGEFAULT_ERROR_CODE_MATCH, 0);

	hv::vmwrite(VMCS_CTRL_VMEXIT_MSR_STORE_COUNT, 0);
	hv::vmwrite(VMCS_CTRL_VMEXIT_MSR_LOAD_COUNT, 0);

	hv::vmwrite(VMCS_CTRL_VMENTRY_MSR_LOAD_COUNT, 0);

	hv::vmwrite(VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD, 0);
	hv::vmwrite(VMCS_CTRL_VMENTRY_EXCEPTION_ERROR_CODE, 0);
	hv::vmwrite(VMCS_CTRL_VMENTRY_INSTRUCTION_LENGTH, 0);

	hv::vmwrite(VMCS_CTRL_TPR_THRESHOLD, 0);
}

void fill_vmcs_guest_fields(__vcpu* vcpu, void* guest_rsp)
{
	segment_descriptor_register_64 gdtr, idtr;

	__sgdt(&gdtr);
	__sidt(&idtr);




	// Control registers
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_CR0, __readcr0());
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_CR3, __readcr3());
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_CR4, __readcr4());

	// Debug register
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_DR7, __readdr(7));

	// RFLAGS
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_RFLAGS, __readeflags());

	// RSP and RIP
	hv::vmwrite<void*>(VMCS_GUEST_RSP, guest_rsp);
	hv::vmwrite<void*>(VMCS_GUEST_RIP, vmx_restore_state);



	// Segment selectors
	hv::vmwrite(VMCS_GUEST_CS_SELECTOR, __read_cs() );
	hv::vmwrite(VMCS_GUEST_SS_SELECTOR, __read_ss() );
	hv::vmwrite(VMCS_GUEST_DS_SELECTOR, __read_ds() );
	hv::vmwrite(VMCS_GUEST_ES_SELECTOR, __read_es() );
	hv::vmwrite(VMCS_GUEST_FS_SELECTOR, __read_fs() );
	hv::vmwrite(VMCS_GUEST_GS_SELECTOR, __read_gs() );
	hv::vmwrite(VMCS_GUEST_TR_SELECTOR, __read_tr() );
	hv::vmwrite(VMCS_GUEST_LDTR_SELECTOR, __read_ldtr() );

	// Base
	hv::vmwrite(VMCS_GUEST_CS_BASE, hv::segment_base(gdtr, __read_cs()));
	hv::vmwrite(VMCS_GUEST_SS_BASE, hv::segment_base(gdtr, __read_ss()));
	hv::vmwrite(VMCS_GUEST_DS_BASE, hv::segment_base(gdtr, __read_ds()));
	hv::vmwrite(VMCS_GUEST_ES_BASE, hv::segment_base(gdtr, __read_es()));
	hv::vmwrite(VMCS_GUEST_FS_BASE, __readmsr(IA32_FS_BASE));
	hv::vmwrite(VMCS_GUEST_GS_BASE, __readmsr(IA32_GS_BASE));
	hv::vmwrite(VMCS_GUEST_TR_BASE, hv::segment_base(gdtr, __read_tr()));
	hv::vmwrite(VMCS_GUEST_LDTR_BASE, hv::segment_base(gdtr, __read_ldtr()));

	// Limit
	hv::vmwrite(VMCS_GUEST_CS_LIMIT, __segmentlimit(__read_cs() ));
	hv::vmwrite(VMCS_GUEST_SS_LIMIT, __segmentlimit(__read_ss() ));
	hv::vmwrite(VMCS_GUEST_DS_LIMIT, __segmentlimit(__read_ds() ));
	hv::vmwrite(VMCS_GUEST_ES_LIMIT, __segmentlimit(__read_es() ));
	hv::vmwrite(VMCS_GUEST_FS_LIMIT, __segmentlimit(__read_fs() ));
	hv::vmwrite(VMCS_GUEST_GS_LIMIT, __segmentlimit(__read_gs() ));
	hv::vmwrite(VMCS_GUEST_TR_LIMIT, __segmentlimit(__read_tr() ));
	hv::vmwrite(VMCS_GUEST_LDTR_LIMIT, __segmentlimit(__read_ldtr() ));
 
	
	// Access rights
	hv::vmwrite(VMCS_GUEST_CS_ACCESS_RIGHTS, hv::segment_access(gdtr, __read_cs()).flags);
	hv::vmwrite(VMCS_GUEST_SS_ACCESS_RIGHTS, hv::segment_access(gdtr, __read_ss()).flags);
	hv::vmwrite(VMCS_GUEST_DS_ACCESS_RIGHTS, hv::segment_access(gdtr, __read_ds()).flags);
	hv::vmwrite(VMCS_GUEST_ES_ACCESS_RIGHTS, hv::segment_access(gdtr, __read_es()).flags);
	hv::vmwrite(VMCS_GUEST_FS_ACCESS_RIGHTS, hv::segment_access(gdtr, __read_fs()).flags);
	hv::vmwrite(VMCS_GUEST_GS_ACCESS_RIGHTS, hv::segment_access(gdtr, __read_gs()).flags);
	hv::vmwrite(VMCS_GUEST_TR_ACCESS_RIGHTS, hv::segment_access(gdtr, __read_tr()).flags);
	hv::vmwrite(VMCS_GUEST_LDTR_ACCESS_RIGHTS, hv::segment_access(gdtr, __read_ldtr()).flags);

	 
	hv::vmwrite(VMCS_GUEST_GDTR_BASE, gdtr.base_address);
	hv::vmwrite(VMCS_GUEST_IDTR_BASE, idtr.base_address);
	hv::vmwrite(VMCS_GUEST_GDTR_LIMIT, gdtr.limit);
	hv::vmwrite(VMCS_GUEST_IDTR_LIMIT, idtr.limit);

	  
	// MSR
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_DEBUGCTL, __readmsr(IA32_DEBUGCTL));
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_SYSENTER_CS, __readmsr(IA32_SYSENTER_CS));
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_SYSENTER_ESP, __readmsr(IA32_SYSENTER_ESP));
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_SYSENTER_EIP, __readmsr(IA32_SYSENTER_EIP));
	hv::vmwrite<unsigned __int64>(VMCS_GUEST_EFER, __readmsr(IA32_EFER));


	hv::vmwrite(VMCS_GUEST_ACTIVITY_STATE, vmx_active);
	 
	hv::vmwrite(VMCS_GUEST_INTERRUPTIBILITY_STATE, 0);
	 
	hv::vmwrite(VMCS_GUEST_PENDING_DEBUG_EXCEPTIONS, 0);
	 
	hv::vmwrite(VMCS_GUEST_VMCS_LINK_POINTER, MAXULONG64);
	 
	hv::vmwrite(VMCS_GUEST_VMX_PREEMPTION_TIMER_VALUE, MAXULONG64);

}

void fill_vmcs_host_fields(__vcpu* vcpu)
{
	 

	// Cr registers

	hv::vmwrite<unsigned __int64>(VMCS_HOST_CR0, __readcr0());

	cr3 host_cr3;
	host_cr3.flags = 0;
	host_cr3.page_level_cache_disable = 0;
	host_cr3.page_level_write_through = 0;
	host_cr3.address_of_page_directory = utils::internal_functions::pfn_mm_get_physical_address(&g_vmm_context->host_page_tables.pml4).QuadPart >> 12;

	hv::vmwrite<unsigned __int64>(VMCS_HOST_CR3, host_cr3.flags);
	hv::vmwrite<unsigned __int64>(VMCS_HOST_CR4, __readcr4());

	hv::vmwrite<unsigned __int64>(VMCS_HOST_RSP, (unsigned __int64)vcpu->vmm_stack + VMM_STACK_SIZE);
	hv::vmwrite<void*>(VMCS_HOST_RIP, vmm_entrypoint);



	//hv::vmwrite(VMCS_GUEST_PAT, __readmsr(IA32_PAT));
//	hv::vmwrite(VMCS_GUEST_PERF_GLOBAL_CTRL, __readmsr(IA32_PERF_GLOBAL_CTRL));


	// MSRS Host
	hv::vmwrite<unsigned __int64>(VMCS_HOST_SYSENTER_CS, __readmsr(IA32_SYSENTER_CS));
	hv::vmwrite<unsigned __int64>(VMCS_HOST_SYSENTER_ESP, __readmsr(IA32_SYSENTER_ESP));
	hv::vmwrite<unsigned __int64>(VMCS_HOST_SYSENTER_EIP, __readmsr(IA32_SYSENTER_EIP));
	hv::vmwrite<unsigned __int64>(VMCS_HOST_EFER, __readmsr(IA32_EFER));


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


	// Segment selectors
	hv::vmwrite<unsigned __int64>(VMCS_HOST_CS_SELECTOR, hv::host_cs_selector.flags);
	hv::vmwrite<unsigned __int64>(VMCS_HOST_SS_SELECTOR, 0);
	hv::vmwrite<unsigned __int64>(VMCS_HOST_DS_SELECTOR, 0);
	hv::vmwrite<unsigned __int64>(VMCS_HOST_ES_SELECTOR, 0);
	hv::vmwrite<unsigned __int64>(VMCS_HOST_FS_SELECTOR, 0);
	hv::vmwrite<unsigned __int64>(VMCS_HOST_GS_SELECTOR, 0);
	hv::vmwrite<unsigned __int64>(VMCS_HOST_TR_SELECTOR, hv::host_tr_selector.flags);
 

	// Segment base
	hv::vmwrite<unsigned __int64>(VMCS_HOST_FS_BASE, __readmsr(IA32_FS_BASE));
	hv::vmwrite<unsigned __int64>(VMCS_HOST_GS_BASE, __readmsr(IA32_GS_BASE));
	hv::vmwrite(VMCS_HOST_TR_BASE, reinterpret_cast<size_t>(&vcpu->host_tss));
	hv::vmwrite(VMCS_HOST_GDTR_BASE, reinterpret_cast<size_t>(&vcpu->host_gdt));
	hv::vmwrite(VMCS_HOST_IDTR_BASE, reinterpret_cast<size_t>(&vcpu->host_idt));

}
