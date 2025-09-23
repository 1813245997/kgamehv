#pragma warning( disable : 4201 4244 4065)
 
#include "../utils/global_defs.h"
#include "vmcall_handler.h"
 

  
  
void vmexit_ept_violation_handler(__vcpu* vcpu);
void vmexit_unimplemented(__vcpu* vcpu);
void vmexit_exception_handler(__vcpu* vcpu);
void vmexit_cr_handler(__vcpu* vcpu);
void vmexit_vm_instruction(__vcpu* vcpu);
void vmexit_triple_fault_handler(__vcpu* vcpu);
void vmexit_failed(__vcpu* vcpu);
void vmexit_invd_handler(__vcpu* vcpu);
void vmexit_rdtscp_handler(__vcpu* vcpu);
void vmexit_xsetbv_handler(__vcpu* vcpu);
void vmexit_rdtsc_handler(__vcpu* vcpu);
void vmexit_rdrand_handler(__vcpu* vcpu);
void vmexit_rdseed_handler(__vcpu* vcpu);
void vmexit_io_handler(__vcpu* vcpu);
void vmexit_mov_dr_handler(__vcpu* vcpu);
void vmexit_cpuid_handler(__vcpu* vcpu);
void vmexit_msr_read_handler(__vcpu* vcpu);
void vmexit_msr_write_handler(__vcpu* vcpu);
void vmexit_invpcid_handler(__vcpu* vcpu);
void vmexit_invlpg_handler(__vcpu* vcpu);
void vmexit_ldtr_access_handler(__vcpu* vcpu);
void vmexit_gdtr_access_handler(__vcpu* vcpu);
void vmexit_monitor_trap_flag_handler(__vcpu* vcpu);
void vmexit_ept_misconfiguration_handler(__vcpu* vcpu);
void vmexit_vm_entry_failure_mce_handler(__vcpu* vcpu);
void vmexit_invalid_guest_state_handler(__vcpu* vcpu);
void vmexit_msr_loading_handler(__vcpu* vcpu);
void vmexit_vmx_preemption(__vcpu* vcpu);
void vmexit_nmi_window(__vcpu* vcpu);
void vmexit_vmxon_handler(__vcpu* vcpu);
void vmexit_getsec_handler(__vcpu* vcpu);
void (*exit_handlers[EXIT_REASON_LAST])(__vcpu* guest_registers) =
{
	vmexit_exception_handler,						// 00 EXIT_REASON_EXCEPTION_NMI
	vmexit_unimplemented,							// 01 EXIT_REASON_EXTERNAL_INTERRUPT
	vmexit_triple_fault_handler,					// 02 EXIT_REASON_TRIPLE_FAULT
	vmexit_unimplemented,							// 03 EXIT_REASON_INIT_SIGNAL
	vmexit_unimplemented,							// 04 EXIT_REASON_SIPI
	vmexit_unimplemented,							// 05 EXIT_REASON_IO_SMI
	vmexit_unimplemented,							// 06 EXIT_REASON_OTHER_SMI
	vmexit_unimplemented,							// 07 EXIT_REASON_PENDING_INTERRUPT
	vmexit_nmi_window,							    // 08 EXIT_REASON_NMI_WINDOW
	vmexit_unimplemented,							// 09 EXIT_REASON_TASK_SWITCH
	vmexit_cpuid_handler,							// 10 EXIT_REASON_CPUID
	vmexit_getsec_handler,							// 11 EXIT_REASON_GETSEC
	vmexit_unimplemented,							// 12 EXIT_REASON_HLT
	vmexit_invd_handler,							// 13 EXIT_REASON_INVD
	vmexit_invlpg_handler,							// 14 EXIT_REASON_INVLPG
	vmexit_unimplemented,							// 15 EXIT_REASON_RDPMC
	vmexit_rdtsc_handler,							// 16 EXIT_REASON_RDTSC
	vmexit_unimplemented,							// 17 EXIT_REASON_RSM
	hv::vmexit_vmcall_handler,							// 18 EXIT_REASON_VMCALL
	vmexit_vm_instruction,							// 19 EXIT_REASON_VMCLEAR
	vmexit_vm_instruction,							// 20 EXIT_REASON_VMLAUNCH
	vmexit_vm_instruction,							// 21 EXIT_REASON_VMPTRLD
	vmexit_vm_instruction,							// 22 EXIT_REASON_VMPTRST
	vmexit_vm_instruction,							// 23 EXIT_REASON_VMREAD
	vmexit_vm_instruction,							// 24 EXIT_REASON_VMRESUME
	vmexit_vm_instruction,							// 25 EXIT_REASON_VMWRITE
	vmexit_vm_instruction,							// 26 EXIT_REASON_VMXOFF
	vmexit_vmxon_handler,							// 27 EXIT_REASON_VMXON
	vmexit_cr_handler,								// 28 EXIT_REASON_CR_ACCESSES
	vmexit_mov_dr_handler,							// 29 EXIT_REASON_MOV_DR
	vmexit_io_handler,								// 30 EXIT_REASON_IO_INSTRUCTION
	vmexit_msr_read_handler,						// 31 EXIT_REASON_MSR_READ
	vmexit_msr_write_handler,						// 32 EXIT_REASON_MSR_WRITE
	vmexit_invalid_guest_state_handler,				// 33 EXIT_REASON_INVALID_GUEST_STATE
	vmexit_msr_loading_handler,					    // 34 EXIT_REASON_MSR_LOADING
	vmexit_unimplemented,							// 35 EXIT_REASON_RESERVED1
	vmexit_unimplemented,							// 36 EXIT_REASON_MWAIT
	vmexit_monitor_trap_flag_handler,				// 37 EXIT_REASON_MONITOR_TRAP_FLAG
	vmexit_unimplemented,							// 38 EXIT_REASON_RESERVED2
	vmexit_unimplemented,							// 39 EXIT_REASON_MONITOR
	vmexit_unimplemented,							// 40 EXIT_REASON_PAUSE
	vmexit_vm_entry_failure_mce_handler,			// 41 EXIT_REASON_VM_ENTRY_FAILURE_MACHINE_CHECK_EVENT
	vmexit_unimplemented,							// 42 EXIT_REASON_RESERVED3
	vmexit_unimplemented,							// 43 EXIT_REASON_TPR_BELOW_THRESHOLD
	vmexit_unimplemented,							// 44 EXIT_REASON_APIC_ACCESS 
	vmexit_unimplemented,							// 45 EXIT_REASON_VIRTUALIZED_EIO
	vmexit_gdtr_access_handler,						// 46 EXIT_REASON_ACCESS_TO_GDTR_OR_IDTR
	vmexit_ldtr_access_handler,						// 47 EXIT_REASON_ACCESS_TO_LDTR_OR_TR
	vmexit_ept_violation_handler,					// 48 EXIT_REASON_EPT_VIOLATION
	vmexit_ept_misconfiguration_handler,			// 49 EXIT_REASON_EPT_MISCONFIGURATION
	vmexit_vm_instruction,							// 50 EXIT_REASON_INVEPT
	vmexit_rdtscp_handler,							// 51 EXIT_REASON_RDTSCP
	vmexit_vmx_preemption,							// 52 EXIT_REASON_VMX_PREEMPTION_TIMER_EXPIRED
	vmexit_vm_instruction,							// 53 EXIT_REASON_INVVPID
	vmexit_invd_handler,							// 54 EXIT_REASON_WBINVD
	vmexit_xsetbv_handler,							// 55 EXIT_REASON_XSETBV
	vmexit_unimplemented,							// 56 EXIT_REASON_APIC_WRITE
	vmexit_rdrand_handler,							// 57 EXIT_REASON_RDRAND
	vmexit_invpcid_handler,							// 58 EXIT_REASON_INVPCID
	vmexit_vm_instruction,							// 59 EXIT_REASON_VMFUNC
	vmexit_unimplemented,							// 60 EXIT_REASON_ENCLS
	vmexit_rdseed_handler,							// 61 EXIT_REASON_RDSEED
	vmexit_unimplemented,							// 62 EXIT_REASON_PAGE_MODIFICATION_LOG_FULL
	vmexit_unimplemented,							// 63 EXIT_REASON_XSAVES
	vmexit_unimplemented,							// 64 EXIT_REASON_XRSTORS
	vmexit_unimplemented,							// 65 EXIT_REASON_RESERVED4
	vmexit_unimplemented,							// 66 EXIT_REASON_SPP_RELATED_EVENT
	vmexit_unimplemented,							// 67 EXIT_REASON_UMWAIT
	vmexit_unimplemented							// 68 EXIT_REASON_TPAUSE
};

void vmexit_monitor_trap_flag_handler(__vcpu* vcpu)
{
	hv::set_mtf(false);
	const auto hooked_entry = vcpu->ept_state->page_to_change;
	if (hooked_entry)
	{
		hooked_entry->original_entry.execute = false;
		vcpu->ept_state->page_to_change = nullptr;
		ept::swap_pml1_and_invalidate_tlb(*vcpu->ept_state, hooked_entry->entry_address, hooked_entry->original_entry, invept_type::invept_single_context);
	}
}

void vmexit_ept_misconfiguration_handler(__vcpu* vcpu)
{
	ASSERT(FALSE);
	hv::dump_vmcs();
	LogInfo(" vmexit_ept_misconfiguration_handler");
	KeBugCheckEx(HYPERVISOR_ERROR, 1, vcpu->vmexit_info.reason, vcpu->vmexit_info.qualification, 0);
}

void vmexit_vm_entry_failure_mce_handler(__vcpu* vcpu)
{
	ASSERT(FALSE);
	hv::dump_vmcs();
	LogInfo(" vmexit_vm_entry_failure_mce_handler");
	KeBugCheckEx(HYPERVISOR_ERROR, 1, vcpu->vmexit_info.reason, vcpu->vmexit_info.qualification, 0);
}

void vmexit_invalid_guest_state_handler(__vcpu* vcpu)
{
	//ASSERT(FALSE);
	//hv::dump_vmcs();
	//LogInfo(" vmexit_invalid_guest_state_handler");
	//KeBugCheckEx(HYPERVISOR_ERROR, 1, vcpu->vmexit_info.reason, vcpu->vmexit_info.qualification, 0);
}

void vmexit_msr_loading_handler(__vcpu* vcpu)
{
	ASSERT(FALSE);
	hv::dump_vmcs();
	LogInfo(" vmexit_msr_loading_handler");
	KeBugCheckEx(HYPERVISOR_ERROR, 1, vcpu->vmexit_info.reason, vcpu->vmexit_info.qualification, 0);
}

/// <summary>
/// sgdt,sidt,lgdt,lidt handler
/// </summary>
/// <param name="guest_regs"></param>
void vmexit_gdtr_access_handler(__vcpu* vcpu)
{
	__vmexit_instruction_information3 instruction_information = { vcpu->vmexit_info.instruction_information };

	union __tmp_desc
	{
		__pseudo_descriptor64 desc64;
		__pseudo_descriptor32 desc32;
	};

	__tmp_desc* tmp_desc = (__tmp_desc*)hv::get_guest_address(vcpu);

	unsigned __int64 old_cr3 = hv::swap_context();

	switch (instruction_information.instruction_identity)
	{
		// SGDT
		case 0:
		{
			__segment_selector selector;
			selector.all = hv::vmread(GUEST_CS_SELECTOR);

			__segment_descriptor* segment_desc = (__segment_descriptor*)(hv::vmread(GUEST_GDTR_BASE) + selector.index * 8);

			if (segment_desc->long_mode == 1)
			{
				tmp_desc->desc64.base_address = hv::vmread(GUEST_GDTR_BASE);
				tmp_desc->desc64.limit = hv::vmread(GUEST_GDTR_LIMIT);
			}

			else 
			{
				tmp_desc->desc32.base_address = hv::vmread(GUEST_GDTR_BASE);
				tmp_desc->desc32.limit = hv::vmread(GUEST_GDTR_LIMIT);
			}

			break;
		}

		// SIDT
		case 1:
		{
			__segment_selector selector;
			selector.all = hv::vmread(GUEST_CS_SELECTOR);

			__segment_descriptor* segment_desc = (__segment_descriptor*)(hv::vmread(GUEST_GDTR_BASE) + selector.index * 8);

			if (segment_desc->long_mode == 1)
			{
				tmp_desc->desc64.base_address = hv::vmread(GUEST_IDTR_BASE);
				tmp_desc->desc64.limit = hv::vmread(GUEST_IDTR_LIMIT);
			}

			else
			{
				tmp_desc->desc32.base_address = hv::vmread(GUEST_IDTR_BASE);
				tmp_desc->desc32.limit = hv::vmread(GUEST_IDTR_LIMIT);
			}

			break;
		}

		// LGDT
		case 2:
		{
			hv::vmwrite(GUEST_GDTR_BASE, tmp_desc->desc64.base_address);
			hv::vmwrite(GUEST_GDTR_LIMIT, tmp_desc->desc64.limit);

			break;
		}

		// LIDT
		case 3:
		{
			hv::vmwrite(GUEST_IDTR_BASE, tmp_desc->desc64.base_address);
			hv::vmwrite(GUEST_IDTR_LIMIT, tmp_desc->desc64.limit);

			break;
		}
	}

	hv::restore_context(old_cr3);

	adjust_rip(vcpu);
}

/// <summary>
/// sldt,str,lldt,ltr handler
/// </summary>
/// <param name="guest_regs"></param>
void vmexit_ldtr_access_handler(__vcpu* vcpu)
{
	__vmexit_instruction_information4 instruction_information = { vcpu->vmexit_info.instruction_information };
	unsigned __int64* linear_address = 
		instruction_information.mem_reg ? 
		&vcpu->vmexit_info.guest_registers->rax - instruction_information.reg1 : 
		(unsigned __int64*)hv::get_guest_address(vcpu);

	unsigned __int64 old_cr3 = hv::swap_context();

	switch (instruction_information.instruction_identity)
	{
		// SLDT
		case 0:
		{
			*linear_address = hv::vmread(GUEST_LDTR_SELECTOR);

			break;
		}

		// STR
		case 1:
		{
			 
			*linear_address = hv::vmread(GUEST_TR_SELECTOR);

			break;
		}

		// LLDT
		case 2:
		{
			hv::vmwrite(GUEST_LDTR_SELECTOR, *linear_address);

			break;
		}

		// LTR
		case 3:
		{
			hv::vmwrite(GUEST_TR_SELECTOR, *linear_address);

			__segment_selector selector;
			selector.all = *linear_address;
			__segment_descriptor* segment_desc = (__segment_descriptor*)(hv::vmread(GUEST_GDTR_BASE) + selector.index * 8);
			segment_desc->type |= 2;

			break;
		}
	}

	hv::restore_context(old_cr3);

	adjust_rip(vcpu);
}

/// <summary>
/// Msr read handler
/// </summary>
/// <param name="guest_regs"></param>
void vmexit_msr_read_handler(__vcpu* vcpu)
{

  

	
	 
	UINT32      msr_index  = vcpu->vmexit_info.guest_registers->rcx & 0xffffffff;
	 
	if (msr_index == IA32_FEATURE_CONTROL)
	{
		 
		vcpu->vmexit_info.guest_registers->rax = vcpu->cached.guest_feature_control.flags & 0xFFFF'FFFF;
		vcpu->vmexit_info.guest_registers->rdx = vcpu->cached.guest_feature_control.flags >> 32;
		
		vcpu->hide_vm_exit_overhead = true;
		adjust_rip(vcpu);
		return;
	}

	__msr msr{};
	hv::host_exception_info e{};

	// the guest could be reading from MSRs that are outside of the MSR bitmap
	// range. refer to https://www.unknowncheats.me/forum/3425463-post15.html
	msr.all = rdmsr_safe(e, msr_index);

	if (e.exception_occurred) {
		// reflect the exception back into the guest
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	vcpu->vmexit_info.guest_registers->rdx = msr.high;
	vcpu->vmexit_info.guest_registers->rax = msr.low;
	vcpu->hide_vm_exit_overhead = true;
	adjust_rip(vcpu);


	 
	
	  
}

/// <summary>
/// Msr write handler
/// </summary>
/// <param name="guest_regs"></param>
void vmexit_msr_write_handler(__vcpu* vcpu)
{
	UINT32 msr_index = vcpu->vmexit_info.guest_registers->rcx & 0xffffffff;
	BOOLEAN     unused_kernel_flag{};
	__msr msr;
	msr.high = vcpu->vmexit_info.guest_registers->rdx;
	msr.low = vcpu->vmexit_info.guest_registers->rax;

	if ((msr_index <= 0x00001FFF) || ((0xC0000000 <= msr_index) && (msr_index <= 0xC0001FFF)) ||
		(msr_index >= RESERVED_MSR_RANGE_LOW && (msr_index <= RESERVED_MSR_RANGE_HI)))
	{
		switch (msr_index)
		{
		case IA32_DS_AREA:
		case IA32_FS_BASE:
		case IA32_GS_BASE:
		case IA32_KERNEL_GS_BASE:
		case IA32_LSTAR:
		case IA32_SYSENTER_EIP:
		case IA32_SYSENTER_ESP:

			if (!check_address_canonicality(msr.all, &unused_kernel_flag))
			{
				//
				// Address is not canonical, inject #GP
				//
				hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);

				return;
			}

			break;
		}

		switch (msr_index)
		{


		case IA32_INTERRUPT_SSP_TABLE_ADDR:
			hv::vmwrite(GUEST_INTERRUPT_SSP_TABLE_ADDR, msr.all);
			break;

		case IA32_SYSENTER_CS:
			hv::vmwrite(GUEST_SYSENTER_CS, msr.all);
			break;

		case IA32_SYSENTER_EIP:
			hv::vmwrite(GUEST_SYSENTER_EIP, msr.all);
			break;

		case IA32_SYSENTER_ESP:
			hv::vmwrite(GUEST_SYSENTER_ESP, msr.all);
			break;

		case IA32_S_CET:
			hv::vmwrite(GUEST_S_CET, msr.all);
			break;

		case IA32_PERF_GLOBAL_CTRL:
			hv::vmwrite(GUEST_PERF_GLOBAL_CONTROL, msr.all);
			break;

		case IA32_PKRS:
			hv::vmwrite(GUEST_PKRS, msr.all);
			break;

		case IA32_RTIT_CTL:
			hv::vmwrite(GUEST_RTIT_CTL, msr.all);
			break;

		case IA32_BNDCFGS:
			hv::vmwrite(GUEST_BNDCFGS, msr.all);
			break;

		case IA32_PAT:
			hv::vmwrite(GUEST_PAT, msr.all);
			break;

		case IA32_EFER:
			hv::vmwrite(GUEST_EFER, msr.all);
			break;

		case IA32_GS_BASE:
			hv::vmwrite(GUEST_GS_BASE, msr.all);
			break;

		case IA32_FS_BASE:
			hv::vmwrite(GUEST_FS_BASE, msr.all);
			break;

		default:
			 
		{
			__writemsr(msr_index, msr.all);


			break;
		}
				
			  
			
		}


		vcpu->hide_vm_exit_overhead = true;
		adjust_rip(vcpu);
	}
	else
	{
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
	}
	
	
}

/// <summary>
/// Ept violation handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_ept_violation_handler(__vcpu* vcpu)
{
	__ept_violation ept_violation;

	ept_violation.all = vcpu->vmexit_info.qualification;
	unsigned __int64 guest_physical_adddress = hv::vmread(GUEST_PHYSICAL_ADDRESS);

	PLIST_ENTRY current = &vcpu->ept_state->hooked_page_list;
	while (&vcpu->ept_state->hooked_page_list != current->Flink)
	{
		current = current->Flink;
		__ept_hooked_page_info* hooked_entry = CONTAINING_RECORD(current, __ept_hooked_page_info, hooked_page_list);
		if (hooked_entry->pfn_of_hooked_page == GET_PFN(guest_physical_adddress))
		{
			if ((ept_violation.read_access || ept_violation.write_access) && (!ept_violation.ept_readable || !ept_violation.ept_writeable))
			{
				const auto old_cr3 = hv::swap_context();
				const auto rip_physical_address = MmGetPhysicalAddress((PVOID)vcpu->vmexit_info.guest_rip).QuadPart;
				const auto rip_physical_pnf = GET_PFN(rip_physical_address);
				hv::restore_context(old_cr3);
				if (static_cast<unsigned __int64>(rip_physical_pnf) == hooked_entry->pfn_of_hooked_page)
				{
					hv::set_mtf(true);
					hooked_entry->original_entry.execute = true;
					vcpu->ept_state->page_to_change = hooked_entry;
					ept::swap_pml1_and_invalidate_tlb(*vcpu->ept_state, hooked_entry->entry_address, hooked_entry->original_entry, invept_type::invept_single_context);
				}
				else
				{
					ept::swap_pml1_and_invalidate_tlb(*vcpu->ept_state, hooked_entry->entry_address, hooked_entry->original_entry, invept_type::invept_single_context);
				}
			}

			else if (ept_violation.execute_access && (ept_violation.ept_readable || ept_violation.ept_writeable))
			{
				ept::swap_pml1_and_invalidate_tlb(*vcpu->ept_state, hooked_entry->entry_address, hooked_entry->changed_entry, invept_type::invept_single_context);
			}

			break;
		}
	}
}

/// <summary>
/// Exception handler
/// </summary>
/// <param name="guest_reg"></param>
void vmexit_exception_handler(__vcpu* vcpu)
{
	__vmexit_interrupt_info interrupt_info;
	interrupt_info.all = hv::vmread(VM_EXIT_INTERRUPTION_INFORMATION);
	
	unsigned __int32 error_code = hv::vmread(VM_EXIT_INTERRUPTION_ERROR_CODE);
	//find_hook_info_by_rip
	// Exit Qualification contain the linear address which caused page fault
	if (interrupt_info.vector == EXCEPTION_VECTOR_PAGE_FAULT)
	{
		__writecr2(vcpu->vmexit_info.qualification);
	}
	else if (interrupt_info.vector == EXCEPTION_VECTOR_SINGLE_STEP && vcpu->vmexit_info.guest_rflags.trap_flag == false)
	{
		hooked_function_info  matched_hook_info  ;
		if (utils::hook_utils:: find_hook_info_by_rip(   reinterpret_cast<void*>(vcpu->vmexit_info.guest_rip), &matched_hook_info))
		{
			hv::vmwrite(VMCS_GUEST_RIP, matched_hook_info.new_handler_va);
			return;
		}
		 
	}
	else if(interrupt_info.vector == EXCEPTION_VECTOR_SINGLE_STEP &&interrupt_info.interruption_type== privileged_software_exception && vcpu->vmexit_info.guest_rflags.trap_flag == true)
	{
		//asm_pg_KiErrata361Present proc
		//	mov ax, ss
		//	pushfq
		//	or qword ptr[rsp], 100h
		//	popfq
		//	mov ss, ax
		//	db 0f1h; icebp
		//	pushfq
		//	and qword ptr[rsp], 0FFFFFEFFh
		//	popfq
		//	ret
		//	asm_pg_KiErrata361Present endp
		dr6 dr6;
		dr6.flags = __readdr(6);
		dr6.single_instruction = 1;
		__writedr(6, dr6.flags);
	}
 

	hv::inject_interruption(interrupt_info.vector, interrupt_info.interruption_type, error_code, interrupt_info.error_code_valid);
}

/// <summary>
/// Cpuid handler
/// </summary>
/// <param name="guest_regs"></param>
void vmexit_cpuid_handler(__vcpu* vcpu)
{
	 

	int regs[4];
	auto const ctx = vcpu->vmexit_info.guest_registers;
	__cpuidex(regs, ctx->rax, ctx->rcx);
 
	ctx->rax = regs[0];
	ctx->rbx = regs[1];
	ctx->rcx = regs[2];
	ctx->rdx = regs[3];
 
	vcpu->hide_vm_exit_overhead = true;
 	adjust_rip(vcpu);
}

/// <summary>
/// Invpcid handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_invpcid_handler(__vcpu* vcpu) 
{
	__vmexit_instruction_information2 instruction_information = { vcpu->vmexit_info.instruction_information };

	unsigned __int64* type = &vcpu->vmexit_info.guest_registers->rax - instruction_information.reg2;

	if (*type > invpcid_all_context)
	{
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	unsigned __int64 guest_address = hv::get_guest_address(vcpu);

	unsigned __int64 old_cr3 = hv::swap_context();

	if (MmGetPhysicalAddress((void*)guest_address).QuadPart == 0) 
	{
		__writecr2(guest_address);
		hv::inject_interruption(EXCEPTION_VECTOR_PAGE_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		hv::restore_context(old_cr3);
		return;
	}

	__invpcid_descriptor descriptor;
	memcpy(&descriptor, (void*)guest_address, sizeof(descriptor));

	hv::restore_context(old_cr3);

	if(descriptor.reserved != 0)
	{
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	cr4 cr4 ;
	cr4.flags = __readcr4();

	if ((*type == invpcid_individual_address || *type == invpcid_single_context) && descriptor.pcid != 0 && cr4.pcid_enable == false)
	{
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	if (*type == invpcid_individual_address && !hv::is_address_canonical(descriptor.linear_address))
	{
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	if (*type == invpcid_individual_address)
		invvpid_invidual_address(descriptor.linear_address, 1);

	else if (*type == invpcid_single_context)
		invvpid_single_context_address(1);

	else if (*type == invvpid_all_context)
		invvpid_single_context_address(1);

	else if (*type == invvpid_single_context_retaining_globals)
		invvpid_single_context_except_global_translations(1);

	adjust_rip(vcpu);
}

void vmexit_invlpg_handler(__vcpu* vcpu)
{
	invvpid_invidual_address(vcpu->vmexit_info.qualification, 1);

	adjust_rip(vcpu);
}

/// <summary>
/// Mov dr handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_mov_dr_handler(__vcpu* vcpu) 
{
	//
	// Moves the contents of a debug register (DR0, DR1, DR2, DR3, DR4, DR5, DR6, or DR7) to a general-purpose register or vice versa.
	// The operand size for these instructions is always 32 bits in non-64-bit modes, regardless of the operand-size attribute. 
	// (See Section 17.2, �Debug Registers? of the Intel?64 and IA-32 Architectures Software Developer�s Manual, Volume 3A, 
	// for a detailed description of the flags and fields in the debug registers.)
	//

	//
	// Accessing dr registers from non ring 0 is forbidden
	// and cause #GP exception
	//
	if (hv::get_guest_cpl() != 0) 
	{
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	__exit_qualification_dr operation;
	operation.all = vcpu->vmexit_info.qualification;
	unsigned __int64* gp_register = &vcpu->vmexit_info.guest_registers->rax - operation.gp_register;

	//
	// Accessing dr register 4 or 5 when debugging extension in cr4 is on cause #UD exception
	// When debug extension is off then it's algined to dr 6 and 7
	//
	if (operation.debug_register_number == 4 || operation.debug_register_number == 5) 
	{
		cr4 cr4;
		cr4.flags = hv::vmread(GUEST_CR4);

		if (cr4.debugging_extensions == true) 
		{
			hv::inject_interruption(EXCEPTION_VECTOR_UNDEFINED_OPCODE, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, false);
			return;
		}

		operation.debug_register_number += 2;
	}

	//
	// Trying to write to 32 upper bits of dr6 or dr7 cause a #GP exception 
	//
	if ((operation.debug_register_number == 6 || operation.debug_register_number == 7) && operation.access_direction == 0 && (*gp_register >> 32) != 0) 
	{
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	//
	// While dr7 bit general detect is set any access to any dr register cause #DB exception
	//
	dr7 dr7;
	dr7.flags = hv::vmread(GUEST_DR7);
	if (dr7.general_detect == 1) 
	{
		dr6 dr6;
		dr6.flags = __readdr(6);
		dr6.breakpoint_condition = 0;
		dr6.debug_register_access_detected = 1;

		__writedr(6, dr6.flags);

		dr7.general_detect = 0;

		hv::vmwrite<unsigned __int64>(GUEST_DR7, dr7.flags);

		hv::inject_interruption(EXCEPTION_VECTOR_SINGLE_STEP, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, false);
		return;
	}

	//
	// Mov to dr
	//
	if (operation.access_direction == 0) 
	{
		switch (operation.debug_register_number)
		{
			case 0:
				__writedr(0, *gp_register);
				break;

			case 1:
				__writedr(1, *gp_register);
				break;

			case 2:
				__writedr(2, *gp_register);
				break;

			case 3:
				__writedr(3, *gp_register);
				break;

			case 6:
				__writedr(6, *gp_register);
				break;

			case 7:
				hv::vmwrite<unsigned __int64>(GUEST_DR7, *gp_register);
				break;
		}
	}

	//
	// Mov from dr
	//
	else 
	{
		switch (operation.debug_register_number)
		{
			case 0:
				*gp_register = __readdr(0);
				break;

			case 1:
				*gp_register = __readdr(1);
				break;

			case 2:
				*gp_register = __readdr(2);
				break;

			case 3:
				*gp_register = __readdr(3);
				break;

			case 6:
				*gp_register = __readdr(6);
				break;

			case 7:
				*gp_register = hv::vmread(GUEST_DR7);
				break;
		}
	}

	adjust_rip(vcpu);
}

/// <summary>
/// IO access handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_io(__vcpu* vcpu)
{
	__exit_qualification_io io_information;
	 rflags rflags = vcpu->vmexit_info.guest_rflags;

	io_information.all = vcpu->vmexit_info.qualification;

	union
	{
		unsigned __int8* byte_ptr;
		unsigned __int16* word_ptr;
		unsigned long* dword_ptr;
		unsigned __int64* qword_ptr;

		unsigned __int64 qword;
	}port_value;

	if (io_information.string_instruction == 0)
		port_value.qword_ptr = &vcpu->vmexit_info.guest_registers->rax;

	//
	// If it's ins/outs instruction we have to check if passed buffer address exists and if not inject #PF
	//
	else 
	{
		port_value.qword = io_information.direction == 0 ? vcpu->vmexit_info.guest_registers->rsi : vcpu->vmexit_info.guest_registers->rdi;
		unsigned __int64 physcial_address = MmGetPhysicalAddress((void*)port_value.qword).QuadPart;

		if (physcial_address == 0)
		{
			__writecr2(port_value.qword);
			hv::inject_interruption(EXCEPTION_VECTOR_PAGE_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
			return;
		}
	}

	unsigned __int32 count = io_information.rep == 0 ? 1 : MASK_GET_LOWER_32BITS(vcpu->vmexit_info.guest_registers->rax);

	// OUT
	if (io_information.direction == 0)
	{
		// Not string
		if (io_information.string_instruction == 0)
		{
			switch (io_information.access_size)
			{
				// 1 Byte size
			case 0:
				__outbyte(io_information.port_number, *port_value.byte_ptr);
				break;

				// 2 Byte size
			case 1:
				__outword(io_information.port_number, *port_value.word_ptr);
				break;

				// 4 Byte size
			case 3:
				__outdword(io_information.port_number, *port_value.dword_ptr);
				break;
			}
		}

		// String
		else
		{
			switch (io_information.access_size)
			{
				// 1 Byte size
			case 0:
				__outbytestring(io_information.port_number, port_value.byte_ptr, count);
				break;

				// 2 Byte size
			case 1:
				__outwordstring(io_information.port_number, port_value.word_ptr, count);
				break;

				// 4 Byte size
			case 3:
				__outdwordstring(io_information.port_number, port_value.dword_ptr, count);
				break;
			}
		}
	}

	// IN
	else
	{
		// Not string
		if (io_information.string_instruction == 0)
		{
			switch (io_information.access_size)
			{
				// 1 Byte size
			case 0:
				*port_value.byte_ptr = __inbyte(io_information.port_number);
				break;

				// 2 Byte size
			case 1:
				*port_value.word_ptr = __inword(io_information.port_number);
				break;

				// 4 Byte size
			case 3:
				*port_value.dword_ptr = __indword(io_information.port_number);
				break;
			}
		}

		// String
		else
		{
			switch (io_information.access_size)
			{
				// 1 Byte size
			case 0:
				__inbytestring(io_information.port_number, port_value.byte_ptr, count);
				break;

				// 2 Byte size
			case 1:
				__inwordstring(io_information.port_number, port_value.word_ptr, count);
				break;

				// 4 Byte size
			case 3:
				__indwordstring(io_information.port_number, port_value.dword_ptr, count);
				break;
			}
		}
	}

	if (io_information.string_instruction == 1)
	{
		if (rflags.direction_flag == 1)
			*port_value.qword_ptr -= count * io_information.access_size;

		else
			*port_value.qword_ptr += count * io_information.access_size;

		if (io_information.rep == 1)
			vcpu->vmexit_info.guest_registers->rcx = 0;
	}

	adjust_rip(vcpu);
}

/// <summary>
/// IO access handler wraper
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_io_handler(__vcpu* vcpu) 
{
	unsigned __int64 old_cr3 = hv::swap_context();
	vmexit_io(vcpu);
	hv::restore_context(old_cr3);
}

/// <summary>
/// Rdrand handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_rdrand_handler(__vcpu* vcpu)
{
	 rflags rflags = vcpu->vmexit_info.guest_rflags;
	__vmexit_instruction_information5 instruction_information;

	instruction_information.all = vcpu->vmexit_info.instruction_information;

	unsigned __int64* register_pointer = &vcpu->vmexit_info.guest_registers->rax - instruction_information.operand_register;

	//
	// Loads a hardware generated random value and store it in the destination register.
	// The size of the random value is determined by the destination register size and operating mode.
	// The Carry Flag indicates whether a random value is available at the time the instruction is executed.
	// CF=1 indicates that the data in the destination is valid. Otherwise CF=0 and the data in the destination operand 
	// will be returned as zeros for the specified width. All other flags are forced to 0 in either situation. 
	// Software must check the state of CF=1 for determining if a valid random value has been returned, 
	// otherwise it is expected to loop and retry execution of RDRAND 
	// (see Intel?64 and IA-32 Architectures Software Developer�s Manual, Volume 1, Section 7.3.17, �Random Number Generator Instructions?.
	// This instruction is available at all privilege levels.
	//
	switch (instruction_information.operand_size)
	{
		case 0:
		{
			rflags.carry_flag = _rdrand16_step((unsigned __int16*)register_pointer);
			break;
		}

		case 1:
		{
			rflags.carry_flag = _rdrand32_step((unsigned __int32*)register_pointer);
			break;
		}

		case 2:
		{
			rflags.carry_flag = _rdrand64_step(register_pointer);
			break;
		}
	}

	hv::vmwrite<unsigned __int64>(GUEST_RFLAGS, rflags.flags);

	adjust_rip(vcpu);
}

/// <summary>
/// Rdseed handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_rdseed_handler(__vcpu* vcpu)
{
	rflags rflags = vcpu->vmexit_info.guest_rflags;
	__vmexit_instruction_information5 instruction_information;
	
	instruction_information.all = vcpu->vmexit_info.instruction_information;

	unsigned __int64*  register_pointer = &vcpu->vmexit_info.guest_registers->rax - instruction_information.operand_register;

	//
	// Loads a hardware generated random value and store it in the destination register.
	// The random value is generated from an Enhanced NRBG (Non Deterministic Random Bit Generator)
	// that is compliant to NIST SP800-90B and NIST SP800-90C in the XOR construction mode.
	// The size of the random value is determined by the destination register size and operating mode.
	// The Carry Flag indicates whether a random value is available at the time the instruction is executed. 
	// CF=1 indicates that the data in the destination is valid. Otherwise CF=0 and the data in the destination operand 
	// will be returned as zeros for the specified width. All other flags are forced to 0 in either situation.
	// Software must check the state of CF=1 for determining if a valid random seed value has been returned, 
	// otherwise it is expected to loop and retry execution of RDSEED (see Section 1.2).
	// The RDSEED instruction is available at all privilege levels.The RDSEED instruction executes normally either
	// inside or outside a transaction region.
	//
	switch (instruction_information.operand_size)
	{
		case 0:
		{
			rflags.carry_flag = _rdseed16_step((unsigned __int16*)register_pointer);
			break;
		}

		case 1:
		{
			rflags.carry_flag = _rdseed32_step((unsigned __int32*)register_pointer);
			break;
		}

		case 2:
		{
			rflags.carry_flag = _rdseed64_step(register_pointer);
			break;
		}
	}

	hv::vmwrite<unsigned __int64>(GUEST_RFLAGS, rflags.flags);

	adjust_rip(vcpu);
}

/// <summary>
/// Handler for failed vmexit
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_failed(__vcpu* vcpu)
{
	ASSERT(FALSE);
	hv::dump_vmcs();
	KeBugCheckEx(HYPERVISOR_ERROR, 1, vcpu->vmexit_info.reason, vcpu->vmexit_info.qualification, 0);
}

/// <summary>
/// Xsetbv handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_xsetbv_handler(__vcpu* vcpu)
{
	
	 

	// CR4.OSXSAVE must be 1
	if (!read_effective_guest_cr4().os_xsave) {
		hv::inject_interruption(EXCEPTION_VECTOR_UNDEFINED_OPCODE, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}


	xcr0 new_xcr0;
	new_xcr0.flags =  vcpu->vmexit_info.guest_registers->rdx << 32 | MASK_GET_LOWER_32BITS(vcpu->vmexit_info.guest_registers->rax);
	UINT32 xcr_numbe = vcpu->vmexit_info.guest_registers->rcx & 0xffffffff;
	// only XCR0 is supported
	if (xcr_numbe!=0) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if trying to set an unsupported bit
	if (new_xcr0.flags & vcpu->cached.xcr0_unsupported_mask) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if clearing XCR0.X87
	if (!new_xcr0.x87) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if XCR0.AVX is 1 while XCRO.SSE is cleared
	if (new_xcr0.avx && !new_xcr0.sse) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if XCR0.AVX is clear and XCR0.opmask, XCR0.ZMM_Hi256, or XCR0.Hi16_ZMM is set
	if (!new_xcr0.avx && (new_xcr0.opmask || new_xcr0.zmm_hi256 || new_xcr0.zmm_hi16)) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if setting XCR0.BNDREG or XCR0.BNDCSR while not setting the other
	if (new_xcr0.bndreg != new_xcr0.bndcsr) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if setting XCR0.opmask, XCR0.ZMM_Hi256, or XCR0.Hi16_ZMM while not setting all of them
	if (new_xcr0.opmask != new_xcr0.zmm_hi256 || new_xcr0.zmm_hi256 != new_xcr0.zmm_hi16) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	 

	 
	_xsetbv(xcr_numbe, new_xcr0.flags);
	vcpu->hide_vm_exit_overhead = true;
	adjust_rip(vcpu);
}

/// <summary>
/// Invd handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_invd_handler(__vcpu* vcpu)
{
	//
	// Invalidates (flushes) the processor�s internal caches and issues a special-function bus cycle that directs 
	// external caches to also flush themselves. Data held in internal caches is not written back to main memory.
	// After executing this instruction, the processor does not wait for the external caches to complete their flushing operation before 
	// proceeding with instruction execution.It is the responsibility of hardware to respond to the cache flush signal.
	// The INVD instruction is a privileged instruction.When the processor is running in protected mode,
	// the CPL of a program or procedure must be 0 to execute this instruction.
	// The INVD instruction may be used when the cache is used as temporary memory and the cache contents
	// need to be invalidated rather than written back to memory.When the cache is used as temporary memory,
	// no external device should be actively writing data to main memory.
	// Use this instruction with care.Data cached internally and not written back to main memory will be lost.
	// Note that any data from an external device to main memory(for example, via a PCIWrite) can be temporarily stored in the caches;
	// these data can be lost when an INVD instruction is executed.Unless there is a specific requirement or 
	// benefit to flushing caches without writing back modified cache lines(for example, temporary memory, 
	// testing, or fault recovery where cache coherency with main memory is not a concern), software should instead use the WBINVD instruction.
	//
	// tldr: We use wbinvd cause it's safer
	//
	__wbinvd();
	adjust_rip(vcpu);
}

/// <summary>
/// Rdtscp handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_rdtscp_handler(__vcpu* vcpu)
{
	//
	// Reads the current value of the processor�s time-stamp counter (a 64-bit MSR) into the EDX:EAX registers
	// and also reads the value of the IA32_TSC_AUX MSR (address C0000103H) into the ECX register.
	// The EDX register is loaded with the high-order 32 bits of the IA32_TSC MSR; 
	// the EAX register is loaded with the low-order 32 bits of the IA32_TSC MSR; 
	// and the ECX register is loaded with the low-order 32-bits of IA32_TSC_AUX MSR.
	// On processors that support the Intel 64 architecture, the high-order 32 bits of each of RAX, RDX, and RCX are cleared.
	//

	unsigned __int32 processor_id;
	unsigned __int64 tsc = __rdtscp(&processor_id);
	vcpu->vmexit_info.guest_registers->rcx = processor_id;
	vcpu->vmexit_info.guest_registers->rdx = MASK_GET_HIGHER_32BITS(tsc) >> 32;
	vcpu->vmexit_info.guest_registers->rax = MASK_GET_LOWER_32BITS(tsc);

	adjust_rip(vcpu);
}

/// <summary>
/// Handler for unimplemented cases
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_unimplemented(__vcpu* vcpu)
{
	LogError("Not implemented vmexit reason %llu, qualificaton %llu, guest rip 0x%llX", vcpu->vmexit_info.reason, vcpu->vmexit_info.qualification, vcpu->vmexit_info.guest_rip);
}

/// <summary>
/// VT-x instructions handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_vm_instruction(__vcpu* vcpu)
{
	UNREFERENCED_PARAMETER(vcpu);
	hv::inject_interruption(EXCEPTION_VECTOR_UNDEFINED_OPCODE, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, false);
}

/// <summary>
/// Triple fault handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_triple_fault_handler(__vcpu* vcpu) 
{
	//
	// Dump whole vmcs state before hard reset
	//
	UNREFERENCED_PARAMETER(vcpu);
	hv:: dump_vmcs();
	ASSERT(FALSE);
	hv::hard_reset();
}

/// <summary>
/// Rdtsc handler
/// </summary>
/// <param name="guest_registers"></param>
void vmexit_rdtsc_handler(__vcpu* vcpu) 
{
	//
	// Loads the current value of the processor's time-stamp counter into the EDX:EAX registers.
	// The time-stamp counter is contained in a 64-bit MSR.
	// The high-order 32 bits of the MSR are loaded into the EDX register, and the low-order 32 bits are loaded into the EAX register.
	// The processor monotonically increments the time-stamp counter MSR every clock cycle and resets it to 0 whenever the processor is reset.
	// See "Time Stamp Counter" in Chapter 15 of the IA-32 Intel Architecture Software Developer's Manual, 
	// Volume 3 for specific details of the time stamp counter behavior.
	//

	unsigned __int64 tsc = __rdtsc();

	vcpu->vmexit_info.guest_registers->rdx = MASK_GET_HIGHER_32BITS(tsc) >> 32;
	vcpu->vmexit_info.guest_registers->rax = MASK_GET_LOWER_32BITS(tsc);

	adjust_rip(vcpu);
}

/// <summary>
/// Get rsp for leaving vmx operation
/// </summary>
/// <returns></returns>
unsigned __int64 return_rsp_for_vmxoff()
{
	return g_vmm_context->vcpu_table[KeGetCurrentProcessorNumberEx(NULL)]->vmx_off_state.guest_rsp;
}

/// <summary>
/// Get rip for leaving vmx operation
/// </summary>
/// <returns></returns>
unsigned __int64 return_rip_for_vmxoff()
{
	return g_vmm_context->vcpu_table[KeGetCurrentProcessorNumberEx(NULL)]->vmx_off_state.guest_rip;
}

void emulate_mov_to_cr0(__vcpu* const vcpu, unsigned  long long vlaue)
{
	cr0 new_cr0{};
	new_cr0.flags = vlaue;
	auto const curr_cr0 = read_effective_guest_cr0();
	auto const curr_cr4 = read_effective_guest_cr4();

	// CR0[15:6] is always 0
	new_cr0.reserved1 = 0;

	// CR0[17] is always 0
	new_cr0.reserved2 = 0;

	// CR0[28:19] is always 0
	new_cr0.reserved3 = 0;

	// CR0.ET is always 1
	new_cr0.extension_type = 1;

	if (new_cr0.reserved4) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	if (new_cr0.paging_enable && !new_cr0.protection_enable) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	if (!new_cr0.cache_disable && new_cr0.not_write_through) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if an attempt is made to clear CR0.PG
	if (!new_cr0.paging_enable) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	if (!new_cr0.write_protect && curr_cr4.control_flow_enforcement_enable) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}
	hv::vmwrite<unsigned __int64>(CONTROL_CR0_READ_SHADOW, new_cr0.flags);

	new_cr0.flags |= vcpu->cached.vmx_cr0_fixed0;
	new_cr0.flags &= vcpu->cached.vmx_cr0_fixed1;

	hv::vmwrite<unsigned __int64>(GUEST_CR0, new_cr0.flags);
	vcpu->hide_vm_exit_overhead = true;
	adjust_rip(vcpu);
}

void emulate_mov_to_cr3(__vcpu* vcpu, unsigned  long long vlaue)
{
   
	cr3 new_cr3;
	new_cr3.flags = vlaue;

	auto const curr_cr4 = read_effective_guest_cr4();

	bool invalidate_tlb = true;

	// 3.4.10.4.1
	if (curr_cr4.pcid_enable && (new_cr3.flags & (1ull << 63))) {
		invalidate_tlb = false;
		new_cr3.flags &= ~(1ull << 63);
	}

	// a mask where bits [63:MAXPHYSADDR] are set to 1
	auto const reserved_mask = ~((1ull << vcpu->cached.max_phys_addr) - 1);

	// 3.2.5
	if (new_cr3.flags & reserved_mask) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// 3.28.4.3.3
	if (invalidate_tlb) {
	 
		invvpid_single_context_except_global_translations(1);
		 
	}

	// it is now safe to write the new guest cr3
	hv::vmwrite(VMCS_GUEST_CR3, new_cr3.flags);

	vcpu->hide_vm_exit_overhead = true;
	adjust_rip(vcpu);
}

void emulate_mov_to_cr4(__vcpu* vcpu, unsigned  long long vlaue)
{
 
	// 2.4.3
	// 2.6.2.1
	// 3.2.5
	// 3.4.10.1
	// 3.4.10.4.1

	cr4 new_cr4;
	new_cr4.flags = vlaue;

	cr3 curr_cr3;
	curr_cr3.flags = hv::vmread(VMCS_GUEST_CR3);

	auto const curr_cr0 = read_effective_guest_cr0();
	auto const curr_cr4 = read_effective_guest_cr4();

	// #GP(0) if an attempt is made to set CR4.SMXE when SMX is not supported
	if (!vcpu->cached.cpuid_01.cpuid_feature_information_ecx.safer_mode_extensions
		&& new_cr4.smx_enable) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if an attempt is made to write a 1 to any reserved bits
	if (new_cr4.reserved1 || new_cr4.reserved2) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if an attempt is made to change CR4.PCIDE from 0 to 1 while CR3[11:0] != 000H
	if ((new_cr4.pcid_enable && !curr_cr4.pcid_enable) && (curr_cr3.flags & 0xFFF)) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if CR4.PAE is cleared
	if (!new_cr4.physical_address_extension) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if CR4.LA57 is enabled
	if (new_cr4.linear_addresses_57_bit) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// #GP(0) if CR4.CET == 1 and CR0.WP == 0
	if (new_cr4.control_flow_enforcement_enable && !curr_cr0.write_protect) {
		hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
		return;
	}

	// invalidate TLB entries if required
	if (new_cr4.page_global_enable != curr_cr4.page_global_enable ||
		!new_cr4.pcid_enable && curr_cr4.pcid_enable ||
		new_cr4.smep_enable && !curr_cr4.smep_enable) {
 
		invvpid_single_context_address(guest_vpid);
	}

	 

	hv::vmwrite(VMCS_CTRL_CR4_READ_SHADOW, new_cr4.flags);

	// make sure to account for VMX reserved bits when setting the real CR4
	new_cr4.flags |= vcpu->cached.vmx_cr4_fixed0;
	new_cr4.flags &= vcpu->cached.vmx_cr4_fixed1;

	hv::vmwrite(VMCS_GUEST_CR4, new_cr4.flags);

	vcpu->hide_vm_exit_overhead = true;
	adjust_rip(vcpu);
}
void emulate_mov_from_cr3(__vcpu* vcpu, uint64_t const gpr)
{
	 
	vmx_exit_qualification_mov_cr qualification;
	qualification.flags = vcpu->vmexit_info.qualification;

	unsigned __int64* register_pointer = &vcpu->vmexit_info.guest_registers->rax - qualification.general_purpose_register;

 
 
		switch (qualification.control_register)
		{
		case 0:
		{
			*register_pointer = hv::vmread(GUEST_CR0);
			break;
		}

		case 3:
		{
			*register_pointer = hv::vmread(GUEST_CR3);
			break;
		}

		case 4:
		{
			*register_pointer = hv::vmread(GUEST_CR4);
			break;
		}


	 
		}

		vcpu->hide_vm_exit_overhead = true;
		adjust_rip(vcpu);

}

void emulate_clts(__vcpu* const vcpu)
{
	// clear CR0.TS in the read shadow
	hv::vmwrite(VMCS_CTRL_CR0_READ_SHADOW, hv::vmread(VMCS_CTRL_CR0_READ_SHADOW) & ~CR0_TASK_SWITCHED_FLAG);

	// clear CR0.TS in the real CR0 register
	hv::vmwrite(VMCS_GUEST_CR0,
		hv::vmread(VMCS_GUEST_CR0) & ~CR0_TASK_SWITCHED_FLAG);
	vcpu->hide_vm_exit_overhead = true;
 
	adjust_rip(vcpu);
}
void emulate_lmsw(__vcpu* const vcpu, uint16_t const value)
{
	cr0 new_cr0;
	new_cr0.flags = value;

	// update the guest CR0 read shadow
	cr0 shadow_cr0;
	shadow_cr0.flags = hv::vmread(VMCS_CTRL_CR0_READ_SHADOW);
	shadow_cr0.protection_enable = new_cr0.protection_enable;
	shadow_cr0.monitor_coprocessor = new_cr0.monitor_coprocessor;
	shadow_cr0.emulate_fpu = new_cr0.emulate_fpu;
	shadow_cr0.task_switched = new_cr0.task_switched;
	hv::vmwrite(VMCS_CTRL_CR0_READ_SHADOW, shadow_cr0.flags);

	// update the real guest CR0.
	// we don't have to worry about VMX reserved bits since CR0.PE (the only
	// reserved bit) can't be cleared to 0 by the LMSW instruction while in
	// protected mode.
	cr0 real_cr0;
	real_cr0.flags = hv::vmread(VMCS_GUEST_CR0);
	real_cr0.protection_enable = new_cr0.protection_enable;
	real_cr0.monitor_coprocessor = new_cr0.monitor_coprocessor;
	real_cr0.emulate_fpu = new_cr0.emulate_fpu;
	real_cr0.task_switched = new_cr0.task_switched;
	hv::vmwrite(VMCS_GUEST_CR0, real_cr0.flags);
	vcpu->hide_vm_exit_overhead = true;
	adjust_rip(vcpu);
}

unsigned __int64* get_register_pointer(__vmexit_guest_registers* regs, UINT32 reg)
{
	switch (reg)
	{
	case 0: return &regs->rax;
	case 1: return &regs->rcx;
	case 2: return &regs->rdx;
	case 3: return &regs->rbx;
	case 4: return &regs->rsp;
	case 5: return &regs->rbp;
	case 6: return &regs->rsi;
	case 7: return &regs->rdi;
	case 8: return &regs->r8;
	case 9: return &regs->r9;
	case 10: return &regs->r10;
	case 11: return &regs->r11;
	case 12: return &regs->r12;
	case 13: return &regs->r13;
	case 14: return &regs->r14;
	case 15: return &regs->r15;
	default:
		return nullptr;
	}
}
 
void vmexit_cr_handler(__vcpu* vcpu)
{
	 
	vmx_exit_qualification_mov_cr qualification;
	qualification.flags = vcpu->vmexit_info.qualification;
 
	 
	unsigned __int64* register_pointer = get_register_pointer( vcpu->vmexit_info.guest_registers, qualification.general_purpose_register);
	 
  
	switch (qualification.access_type) {
		 
	 
	case VMX_EXIT_QUALIFICATION_ACCESS_MOV_TO_CR:
		switch (qualification.control_register) {
		case VMX_EXIT_QUALIFICATION_REGISTER_CR0:
			emulate_mov_to_cr0(vcpu, *register_pointer);
			break;
		case VMX_EXIT_QUALIFICATION_REGISTER_CR3:
			emulate_mov_to_cr3(vcpu, *register_pointer);
			break;
		case VMX_EXIT_QUALIFICATION_REGISTER_CR4:
			emulate_mov_to_cr4(vcpu, *register_pointer);
			break;
		}
		break;
		 

 
	case VMX_EXIT_QUALIFICATION_ACCESS_MOV_FROM_CR:
		emulate_mov_from_cr3(vcpu, qualification.control_register);
		break;
		 
	case VMX_EXIT_QUALIFICATION_ACCESS_CLTS:
		emulate_clts(vcpu);
		break;
		 
	case VMX_EXIT_QUALIFICATION_ACCESS_LMSW:
		emulate_lmsw(vcpu, qualification.lmsw_source_data);
		break;
	}


 
	
}

/// <summary>
/// Vm exit dispatcher
/// </summary>
/// <param name="guest_registers"></param>
/// <returns> status </returns>
bool vmexit_handler(__vmexit_guest_registers* guest_registers)
{
	__vcpu* vcpu = g_vmm_context->vcpu_table[KeGetCurrentProcessorNumberEx(NULL)];

	guest_registers->rsp = hv::vmread(GUEST_RSP);

	vcpu->vmexit_info.reason = hv::vmread(EXIT_REASON) & 0xffff;
	vcpu->vmexit_info.qualification = hv::vmread(EXIT_QUALIFICATION);
	vcpu->vmexit_info.guest_rflags.flags = hv::vmread(GUEST_RFLAGS);
	vcpu->vmexit_info.guest_rip = hv::vmread(GUEST_RIP);
	vcpu->vmexit_info.instruction_length = hv::vmread(VM_EXIT_INSTRUCTION_LENGTH);
	vcpu->vmexit_info.instruction_information = hv::vmread(VM_EXIT_INSTRUCTION_INFORMATION);
	vcpu->vmexit_info.guest_registers = guest_registers;
	vcpu->hide_vm_exit_overhead = false;
	vcpu->is_on_vmx_root_mode = true;
 
	//
	//  Instructions That Cause VM Exits Unconditionally
	//  The following instructions cause VM exits when they are executed in VMX non - root operation : CPUID, GETSEC,
	//  INVD, and XSETBV.This is also true of instructions introduced with VMX, which include : INVEPT, INVVPID,
	//  VMCALL, VMCLEAR, VMLAUNCH, VMPTRLD, VMPTRST, VMRESUME, VMXOFF, and VMXON.
	//
	exit_handlers[vcpu->vmexit_info.reason](vcpu);

	if (vcpu->vmx_off_state.vmx_off_executed == 1)
	{
		vcpu->vcpu_status.vmm_launched = 0;
		return false;
	}

	 hide_vm_exit_overhead(vcpu);
	 hv::vmwrite(VMCS_CTRL_TSC_OFFSET, vcpu->tsc_offset);
	 hv::vmwrite(VMCS_GUEST_VMX_PREEMPTION_TIMER_VALUE, vcpu->preemption_timer);

	vcpu->is_on_vmx_root_mode = false;
	return true;
}

/// <summary>
/// Add to guest rip size of instruction which he executed
/// </summary>
void adjust_rip(__vcpu* vcpu)
{
	hv::vmwrite(GUEST_RIP, vcpu->vmexit_info.guest_rip + vcpu->vmexit_info.instruction_length);
	if (vcpu->vmexit_info.guest_rflags.trap_flag)
	{
		__vmx_pending_debug_exceptions pending_debug = { hv::vmread(GUEST_PENDING_DEBUG_EXCEPTION) };
		__vmx_interruptibility_state interruptibility = { hv::vmread(GUEST_INTERRUPTIBILITY_STATE) };

		pending_debug.bs = true;
		hv::vmwrite(GUEST_PENDING_DEBUG_EXCEPTION, pending_debug.all);

		interruptibility.blocking_by_sti = false;
		interruptibility.blocking_by_mov_ss = false;
		hv::vmwrite(GUEST_INTERRUPTIBILITY_STATE, interruptibility.all);
	}
}

void vmexit_vmx_preemption(__vcpu* vcpu)
{

}


void hide_vm_exit_overhead(__vcpu* vcpu)
{
	//ia32_perf_global_ctrl_register perf_global_ctrl;
	//perf_global_ctrl.flags = vcpu->msr_exit_store.perf_global_ctrl.msr_data;

	//// make sure the CPU loads the previously stored guest state on vm-entry
	//vcpu->msr_entry_load.aperf.msr_data =vcpu->msr_exit_store.aperf.msr_data;
	//vcpu->msr_entry_load.mperf.msr_data =vcpu->msr_exit_store.mperf.msr_data;
	//hv::vmwrite(VMCS_GUEST_PERF_GLOBAL_CTRL, perf_global_ctrl.flags);

	//// account for the constant overhead associated with loading/storing MSRs
	//vcpu->msr_entry_load.aperf.msr_data -= vcpu->vm_exit_mperf_overhead;
	//vcpu->msr_entry_load.mperf.msr_data -= vcpu->vm_exit_mperf_overhead;

	//// account for the constant overhead associated with loading/storing MSRs
	//if (perf_global_ctrl.en_fixed_ctrn & (1ull << 2)) {
	//	auto const cpl = current_guest_cpl();

	//	ia32_fixed_ctr_ctrl_register fixed_ctr_ctrl;
	//	fixed_ctr_ctrl.flags = __readmsr(IA32_FIXED_CTR_CTRL);

	//	// this also needs to be done for many other PMCs, but whatever
	//	if ((cpl == 0 && fixed_ctr_ctrl.en2_os) || (cpl == 3 && fixed_ctr_ctrl.en2_usr))
	//		__writemsr(IA32_FIXED_CTR2, __readmsr(IA32_FIXED_CTR2) - vcpu->vm_exit_ref_tsc_overhead);
	//}

	// this usually occurs for vm-exits that are unlikely to be reliably timed,
	// such as when an exception occurs or if the preemption timer fired
	if (!vcpu->hide_vm_exit_overhead || vcpu->vm_exit_tsc_overhead > 10000) {
		// this is our chance to resync the TSC
		vcpu->tsc_offset = 0;

		// soft disable the VMX preemption timer
	 	vcpu->preemption_timer = ~0ull;

		return;
	}

	// set the preemption timer to cause an exit after 10000 guest TSC ticks have passed
	vcpu->preemption_timer = max(2,
		10000 >> vcpu->cached.vmx_misc.preemption_timer_tsc_relationship);

	// use TSC offsetting to hide from timing attacks that use the TSC
	vcpu->tsc_offset -= vcpu->vm_exit_tsc_overhead;

}

void vmexit_nmi_window(__vcpu* vcpu)
{
	--vcpu->queued_nmis;

	// inject the NMI into the guest
	inject_nmi();

	if (vcpu->queued_nmis == 0) {
		// disable NMI-window exiting since we have no more NMIs to inject
		auto ctrl = read_ctrl_proc_based();
		ctrl.nmi_window_exiting = 0;
		write_ctrl_proc_based(ctrl);
	}

	// there is the possibility that a host NMI occurred right before we
	// disabled NMI-window exiting. make sure to re-enable it if this is the case.
	if (vcpu->queued_nmis > 0) {
		auto ctrl = read_ctrl_proc_based();
		ctrl.nmi_window_exiting = 1;
		write_ctrl_proc_based(ctrl);
	}
}

void inject_nmi()
{
	vmentry_interrupt_information interrupt_info;
	interrupt_info.flags = 0;
	interrupt_info.vector = nmi;
	interrupt_info.interruption_type = non_maskable_interrupt;
	interrupt_info.deliver_error_code = 0;
	interrupt_info.valid = 1;
	hv::vmwrite(VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD, interrupt_info.flags);
}

void vmexit_vmxon_handler(__vcpu* vcpu)
{
	// usually a #UD doesn't trigger a vm-exit, but in this case it is possible
    // that CR4.VMXE is 1 while guest shadow CR4.VMXE is 0.
	if (!read_effective_guest_cr4().vmx_enable) {
		hv::inject_interruption(EXCEPTION_VECTOR_UNDEFINED_OPCODE, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, false);
		return;
	}


	hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
	 
}

void vmexit_getsec_handler(__vcpu* vcpu)
{
	hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, true);
}

namespace hv
{

	// called for every host interrupt
	void handle_host_interrupt(trap_frame* const frame) {

		// we only care about NMIs and exceptions	
		ULONG  processor_number = KeGetCurrentProcessorNumberEx(NULL);
		__vcpu* vcpu = g_vmm_context->vcpu_table[processor_number];
		
		switch (frame->vector) {
			// host NMIs
		case nmi: {
			auto ctrl = read_ctrl_proc_based();
			ctrl.nmi_window_exiting = 1;
			write_ctrl_proc_based(ctrl);

			 
			++vcpu->queued_nmis;

			break;
		}
				// host exceptions
		default: {
			// no registered exception handler
			if (!frame->r10 || !frame->r11) {
				/*	HV_LOG_ERROR("Unhandled exception. RIP=hv.sys+%p. Vector=%u.",
						frame->rip - reinterpret_cast<UINT64>(&__ImageBase), frame->vector);*/

				// ensure a triple-fault
				segment_descriptor_register_64 idtr;
				idtr.base_address = frame->rsp;
				idtr.limit = 0xFFF;
				__lidt(&idtr);

				break;
			}

			/*		HV_LOG_HOST_EXCEPTION("Handling host exception. RIP=hv.sys+%p. Vector=%u",
						frame->rip - reinterpret_cast<UINT64>(&__ImageBase), frame->vector);*/

			// jump to the exception handler
			frame->rip = frame->r10;

			auto const e = reinterpret_cast<host_exception_info*>(frame->r11);

			e->exception_occurred = true;
			e->vector = frame->vector;
			e->error = frame->error;

			// slightly helps prevent infinite exceptions
			frame->r10 = 0;
			frame->r11 = 0;
		}
		}
	}
}