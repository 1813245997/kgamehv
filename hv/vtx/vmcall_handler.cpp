#pragma warning( disable : 4201 4244 4805 4189)

#include <intrin.h>
#include "common.h"
#include "vmcall_handler.h"
#include "../asm\vm_intrin.h"
#include "vmcall_reason.h"
#include "vmcs_encodings.h"
#include "msr.h"
#include "cr.h"
#include "cpuid.h"
#include "vmexit_handler.h"
#include "hypervisor_routines.h"
#include "interrupt.h"
#include "../asm\vm_context.h"
#include "vt_global.h"
#include "hypervisor_gateway.h"

void restore_segment_registers()
{
	__writemsr(IA32_FS_BASE, hv::vmread(GUEST_FS_BASE));
	__writemsr(IA32_GS_BASE, hv::vmread(GUEST_GS_BASE));
	__reload_gdtr(hv::vmread(GUEST_GDTR_BASE), hv::vmread(GUEST_GDTR_LIMIT));
	__reload_idtr(hv::vmread(GUEST_IDTR_BASE), hv::vmread(GUEST_IDTR_LIMIT));
}

void call_vmxoff(__vcpu* vcpu)
{
	__writecr3(hv::vmread(GUEST_CR3));

	vcpu->vmx_off_state.guest_rip = vcpu->vmexit_info.guest_rip + vcpu->vmexit_info.instruction_length;
	vcpu->vmx_off_state.guest_rsp = vcpu->vmexit_info.guest_registers->rsp;
	vcpu->vmx_off_state.vmx_off_executed = true;

	restore_segment_registers();

	__vmx_off();

	__writecr4(__readcr4() & (~0x2000));
}

void vmexit_vmcall_handler(__vcpu* vcpu) 
{
	bool status = true;
	unsigned __int64 vmcall_reason = vcpu->vmexit_info.guest_registers->rcx;
	unsigned __int64 vmcall_parameter1 = vcpu->vmexit_info.guest_registers->rdx;
	unsigned __int64 vmcall_parameter2 = vcpu->vmexit_info.guest_registers->r8;
	unsigned __int64 vmcall_parameter3 = vcpu->vmexit_info.guest_registers->r9;

	//
	// These only if __vmcall_ex was called
	//
	unsigned __int64 vmcall_parameter4 = vcpu->vmexit_info.guest_registers->r10;
	unsigned __int64 vmcall_parameter5 = vcpu->vmexit_info.guest_registers->r11;
	unsigned __int64 vmcall_parameter6 = vcpu->vmexit_info.guest_registers->r12;
	unsigned __int64 vmcall_parameter7 = vcpu->vmexit_info.guest_registers->r13;
	unsigned __int64 vmcall_parameter8 = vcpu->vmexit_info.guest_registers->r14;
	 

	  
	if (vcpu->vmexit_info.guest_registers->rax != VMCALL_IDENTIFIER  )
	{
		hv::inject_interruption(EXCEPTION_VECTOR_UNDEFINED_OPCODE, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, false);
		return;
	}

	switch (vmcall_reason)
	{
		case VMCALL_TEST:
		{
			vcpu->vm_exit_last_tsc = __rdtsc();
			adjust_rip(vcpu);
			break;
		}

		case VMCALL_VMXOFF:
		{
			call_vmxoff(vcpu);
			break;
		}

		case VMCALL_EPT_HOOK_FUNCTION:
		{
			unsigned __int64 old_cr3 = hv::swap_context(vmcall_parameter1);

			status = ept::hook_kernel_function (
				*vcpu->ept_state, 
				reinterpret_cast<void *>  (vmcall_parameter2),
				reinterpret_cast<void*>  (vmcall_parameter3), 
				reinterpret_cast<void**>  (vmcall_parameter4));

			hv::restore_context(old_cr3);

			adjust_rip(vcpu);
			break;
		}

		case VMCALL_EPT_UNHOOK_FUNCTION:
		{
			unsigned __int64 old_cr3 = hv::swap_context(vmcall_parameter1);

			EptUnhookType unhook_type = static_cast<EptUnhookType>(vmcall_parameter2); // 使用枚举类型

			switch (unhook_type)
			{
			case UNHOOK_ALL:
				ept::unhook_all_functions(*vcpu->ept_state);
				break;

			case UNHOOK_SINGLE:
				 
				status = ept::unhook_function(*vcpu->ept_state, vmcall_parameter3);
				break;

			case UNHOOK_BY_PID:
				 
				status = ept::unhook_by_pid(*vcpu->ept_state, reinterpret_cast<HANDLE>(vmcall_parameter4));
				break;

			default:
				status = false; // 无效的操作类型
				break;
			}

			hv::restore_context(old_cr3);

			adjust_rip(vcpu);
			break;
		}

		case VMCALL_EPT_SET_BREAK_POINT:
		{
			unsigned __int64 old_cr3 = hv::swap_context(vmcall_parameter1);

			status = ept::hook_break_ponint_int3(
				*vcpu->ept_state,
				reinterpret_cast<void*>  (vmcall_parameter2),
				reinterpret_cast<void*>  (vmcall_parameter3),
				reinterpret_cast<unsigned char*>  (vmcall_parameter4));

			hv::restore_context(old_cr3);

			adjust_rip(vcpu);
			break;
		}
		
		case VMCALL_EPT_REMOVE_BREAK_POINT:
		{
			unsigned __int64 old_cr3 = hv::swap_context(vmcall_parameter1);

			 
			 
			hv::restore_context(old_cr3);
			adjust_rip(vcpu);
			break;
			 
		}

		case  VMCALL_EPT_SET_BREAK_POINT_EXCEPTION:
		{

			unsigned __int64 old_cr3 = hv::swap_context(vmcall_parameter2);

			status = ept::hook_user_exception_int3(
				*vcpu->ept_state,
				reinterpret_cast<HANDLE>  (vmcall_parameter1),
				reinterpret_cast<void*>  (vmcall_parameter3),
				reinterpret_cast<void*>  (vmcall_parameter4),
				reinterpret_cast<unsigned char*>  (vmcall_parameter5));

			hv::restore_context(old_cr3);
			adjust_rip(vcpu);
			break;
		}
		case VMCALL_EPT_REMOVE_BREAK_POINT_EXCEPTION:
		{
			unsigned __int64 old_cr3 = hv::swap_context(vmcall_parameter1);



			hv::restore_context(old_cr3);
			adjust_rip(vcpu);
			break;
		}


		case VMCALL_DUMP_POOL_MANAGER:
		{
			pool_manager::dump_pools_info();
			adjust_rip(vcpu);
			break;
		}

		case VMCALL_DUMP_VMCS_STATE:
		{
			hv::dump_vmcs();
			adjust_rip(vcpu);
			break;
		}

		case VMCALL_HIDE_HV_PRESENCE:
		{
			g_vmm_context->hv_presence = false;
			adjust_rip(vcpu);
			break;
		}

		case VMCALL_UNHIDE_HV_PRESENCE:
		{
			g_vmm_context->hv_presence = true;
			adjust_rip(vcpu);
			break;
		}

		case  VMCALL_SET_MSR_FAULT_BITMAPS:
		{
		 	set_invalid_msr_bitmaps(vmcall_parameter1, vmcall_parameter2);
			adjust_rip(vcpu);
			break;
		}

		 
		 

		default:
		{
			adjust_rip(vcpu);
			break;
		}
	}

	vcpu->vmexit_info.guest_registers->rax = status;
}



void set_invalid_msr_bitmaps(UINT64 msr_bitmap, UINT64 synthetic_msr_bitmap)
{
	g_invalid_msr_bitmap = reinterpret_cast<unsigned  long long*>(msr_bitmap);
	g_invalid_synthetic_msr_bitmap = reinterpret_cast<unsigned  long long*>(synthetic_msr_bitmap);
}
