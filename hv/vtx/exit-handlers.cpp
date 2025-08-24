#include "exit-handlers.h"
#include "guest-context.h"
#include "exception-routines.h"
#include "introspection.h"
#include "hypercalls.h"
#include "vcpu.h"
#include "vmx.h"
#include "logger.h"
#include "hv.h"
#include "../utils/hook_utils.h"
#include "hypervisor_routines.h"
#include "interrupt.h"
#include "../utils/macros.h"
#include "vmcs_encodings.h"
#include "msr_struct.h"
#include "compatibility_checks.h"
#include "vt_global.h"
#include "bit_wise.h"
namespace hv {

void emulate_cpuid(vcpu* const cpu) {
  auto const ctx = cpu->ctx;

  int regs[4];
  __cpuidex(regs, ctx->eax, ctx->ecx);

  ctx->rax = regs[0];
  ctx->rbx = regs[1];
  ctx->rcx = regs[2];
  ctx->rdx = regs[3];

  cpu->hide_vm_exit_overhead = true;
  skip_instruction();
}

void emulate_rdmsr(vcpu* const cpu) {

	if (cpu->ctx->ecx == IA32_FEATURE_CONTROL) {
		// return the fake guest FEATURE_CONTROL MSR
		cpu->ctx->rax = cpu->cached.guest_feature_control.flags & 0xFFFF'FFFF;
		cpu->ctx->rdx = cpu->cached.guest_feature_control.flags >> 32;

		cpu->hide_vm_exit_overhead = true;
		skip_instruction();
		return;
	}

	host_exception_info e;

	// the guest could be reading from MSRs that are outside of the MSR bitmap
	// range. refer to https://www.unknowncheats.me/forum/3425463-post15.html
	auto const msr_value = rdmsr_safe(e, cpu->ctx->ecx);

	if (e.exception_occurred) {
		// reflect the exception back into the guest
		inject_hw_exception(general_protection, 0);
		return;
	}

	cpu->ctx->rax = msr_value & 0xFFFF'FFFF;
	cpu->ctx->rdx = msr_value >> 32;

	cpu->hide_vm_exit_overhead = true;
	skip_instruction();
 




 // if (cpu->ctx->ecx == IA32_FEATURE_CONTROL) {

 //   
 //   ia32_feature_control_register feature_control{};
 //  
 //   feature_control.flags = hv::vmread(IA32_FEATURE_CONTROL);
 //   feature_control.enable_vmx_outside_smx = false;
 //   // return the fake guest FEATURE_CONTROL MSR
 //  /* cpu->ctx->rax = cpu->cached.guest_feature_control.flags & 0xFFFF'FFFF;
 //   cpu->ctx->rdx = cpu->cached.guest_feature_control.flags >> 32;*/
	//cpu->ctx->rax = feature_control.flags & 0xFFFF'FFFF;
	//cpu->ctx->rdx = feature_control.flags >> 32;
 //   cpu->hide_vm_exit_overhead = true;
 //   skip_instruction();
 //   return;
 // }

 //
 // unsigned long long   msr_value = 0;
 // unsigned long  msr_index = cpu->ctx->ecx;

 //  if ((msr_index <= 0x00001FFF) || ((0xC0000000 <= msr_index) && (msr_index <= 0xC0001FFF)) ||
	//  (msr_index >= RESERVED_MSR_RANGE_LOW && (msr_index <= RESERVED_MSR_RANGE_HI)))
 // {

 //      

	//  switch (msr_index)
	//  {
	//  case IA32_SYSENTER_CS:
 //         msr_value = hv::vmread(VMCS_GUEST_SYSENTER_CS);
	//	  break;

	//  case IA32_SYSENTER_ESP:
 //         msr_value = hv::vmread(VMCS_GUEST_SYSENTER_ESP);
	//	  break;

	//  case IA32_SYSENTER_EIP:
 //         msr_value = hv::vmread(VMCS_GUEST_SYSENTER_EIP);
	//	  break;

	//  case IA32_GS_BASE:
 //         msr_value = hv::vmread(VMCS_GUEST_GS_BASE);
	//	  break;

	//  case IA32_FS_BASE:
 //         msr_value = hv::vmread(VMCS_GUEST_FS_BASE);
	//	  break;

	//  case IA32_INTERRUPT_SSP_TABLE_ADDR:
 //         msr_value = hv::vmread(GUEST_INTERRUPT_SSP_TABLE_ADDR);
	//	  break;


	//  case IA32_S_CET:
 //         msr_value = hv::vmread(GUEST_S_CET);
	//	  break;

	//  case IA32_PERF_GLOBAL_CTRL:
 //         msr_value = hv::vmread(GUEST_PERF_GLOBAL_CONTROL);
	//	  break;

	//  case IA32_PKRS:
 //         msr_value = hv::vmread(GUEST_PKRS);
	//	  break;

	//  case IA32_RTIT_CTL:
 //         msr_value = hv::vmread(GUEST_RTIT_CTL);
	//	  break;

	//  case IA32_BNDCFGS:
 //         msr_value = hv::vmread(GUEST_BNDCFGS);
	//	  break;

	//  case IA32_PAT:
 //         msr_value = hv::vmread(GUEST_PAT);
	//	  break;

	//  case IA32_EFER:
 //         msr_value = hv::vmread(GUEST_EFER);
	//	  break;

	//  default:

	//  {

	//	  if (msr_index <= 0xfff && TestBit(msr_index, (unsigned long*)g_invalid_msr_bitmap) != NULL64_ZERO)
	//	  {
 //             inject_hw_exception(general_protection, 0);
	//		  return;
	//	  }

	//	  if ((msr_index >= 0x40000000 && msr_index <= 0x400000FF) &&
	//		  TestBit(msr_index - 0x40000000, (unsigned long*)g_invalid_synthetic_msr_bitmap) != NULL64_ZERO)
	//	  {
 //             inject_hw_exception(general_protection, 0);
	//		  return;
	//	  }

 //         msr_value = __readmsr(msr_index);


	//  }
	//  break;
	//  }

	//  cpu->ctx->rax = msr_value & 0xFFFF'FFFF;
	//  cpu->ctx->rdx = msr_value >> 32;
	//  
	//  cpu->hide_vm_exit_overhead = true;
	//  skip_instruction();

 // }
 // else
 // {
 //     inject_hw_exception(general_protection, 0);
	//  return;
 // }
   

}

void emulate_wrmsr(vcpu* const cpu) {

	auto const msr = cpu->ctx->ecx;
	auto const value = (cpu->ctx->rdx << 32) | cpu->ctx->eax;

	// let the guest write to the MSRs
	host_exception_info e;
	wrmsr_safe(e, msr, value);

	if (e.exception_occurred) {
		inject_hw_exception(general_protection, 0);
		return;
	}

	// we need to make sure to update EPT memory types if the guest
	// modifies any of the MTRR registers
	if (msr == IA32_MTRR_DEF_TYPE || msr == IA32_MTRR_FIX64K_00000 ||
		msr == IA32_MTRR_FIX16K_80000 || msr == IA32_MTRR_FIX16K_A0000 ||
		(msr >= IA32_MTRR_FIX4K_C0000 && msr <= IA32_MTRR_FIX4K_F8000) ||
		(msr >= IA32_MTRR_PHYSBASE0 && msr <= IA32_MTRR_PHYSBASE0 + 511)) {
		// update EPT memory types
		if (!read_effective_guest_cr0().cache_disable)
			update_ept_memory_type(cpu->ept);

		vmx_invept(invept_all_context, {});
	}

	cpu->hide_vm_exit_overhead = true;
	skip_instruction();
	return;



 //   unsigned long  msr_index = cpu->ctx->ecx;

	//BOOLEAN     unused_kernel_flag{};
	//auto const value = (cpu->ctx->rdx << 32) | cpu->ctx->eax;

 //   __msr msr;
 //   msr.all = value;
 //

	//if ((msr_index <= 0x00001FFF) || ((0xC0000000 <= msr_index) && (msr_index <= 0xC0001FFF)) ||
	//	(msr_index >= RESERVED_MSR_RANGE_LOW && (msr_index <= RESERVED_MSR_RANGE_HI)))
	//{
	//	switch (msr_index)
	//	{
	//	case IA32_DS_AREA:
	//	case IA32_FS_BASE:
	//	case IA32_GS_BASE:
	//	case IA32_KERNEL_GS_BASE:
	//	case IA32_LSTAR:
	//	case IA32_SYSENTER_EIP:
	//	case IA32_SYSENTER_ESP:

	//		if (!check_address_canonicality(msr.all, &unused_kernel_flag))
	//		{
	//			//
	//			// Address is not canonical, inject #GP
	//			//

 //               inject_hw_exception(general_protection, 0);

	//			return;
	//		}

	//		break;
	//	}

	//	switch (msr_index)
	//	{


	//	case IA32_INTERRUPT_SSP_TABLE_ADDR:
	//		hv::vmwrite(GUEST_INTERRUPT_SSP_TABLE_ADDR, msr.all);
	//		break;

	//	case IA32_SYSENTER_CS:
	//		hv::vmwrite(GUEST_SYSENTER_CS, msr.all);
	//		break;

	//	case IA32_SYSENTER_EIP:
	//		hv::vmwrite(GUEST_SYSENTER_EIP, msr.all);
	//		break;

	//	case IA32_SYSENTER_ESP:
	//		hv::vmwrite(GUEST_SYSENTER_ESP, msr.all);
	//		break;

	//	case IA32_S_CET:
	//		hv::vmwrite(GUEST_S_CET, msr.all);
	//		break;

	//	case IA32_PERF_GLOBAL_CTRL:
	//		hv::vmwrite(GUEST_PERF_GLOBAL_CONTROL, msr.all);
	//		break;

	//	case IA32_PKRS:
	//		hv::vmwrite(GUEST_PKRS, msr.all);
	//		break;

	//	case IA32_RTIT_CTL:
	//		hv::vmwrite(GUEST_RTIT_CTL, msr.all);
	//		break;

	//	case IA32_BNDCFGS:
	//		hv::vmwrite(GUEST_BNDCFGS, msr.all);
	//		break;

	//	case IA32_PAT:
	//		hv::vmwrite(GUEST_PAT, msr.all);
	//		break;

	//	case IA32_EFER:
	//		hv::vmwrite(GUEST_EFER, msr.all);
	//		break;

	//	case IA32_GS_BASE:
	//		hv::vmwrite(GUEST_GS_BASE, msr.all);
	//		break;

	//	case IA32_FS_BASE:
	//		hv::vmwrite(GUEST_FS_BASE, msr.all);
	//		break;

	//	default:

	//	{
	//		__writemsr(msr_index, msr.all);

	//		// we need to make sure to update EPT memory types if the guest
 //         // modifies any of the MTRR registers
	//		if (msr_index == IA32_MTRR_DEF_TYPE || msr_index == IA32_MTRR_FIX64K_00000 ||
 //               msr_index == IA32_MTRR_FIX16K_80000 || msr_index == IA32_MTRR_FIX16K_A0000 ||
	//			(msr_index >= IA32_MTRR_FIX4K_C0000 && msr_index <= IA32_MTRR_FIX4K_F8000) ||
	//			(msr_index >= IA32_MTRR_PHYSBASE0 && msr_index <= IA32_MTRR_PHYSBASE0 + 511)) {
	//			// update EPT memory types
	//			if (!read_effective_guest_cr0().cache_disable)
	//				update_ept_memory_type(cpu->ept);

	//			vmx_invept(invept_all_context, {});
	//		}

	//		break;
	//	}



	//	}


	//	 cpu->hide_vm_exit_overhead = true;
 //        skip_instruction();
	//}
	//else
	//{
 //       inject_hw_exception(general_protection, 0);
	//}
}

void emulate_getsec(vcpu*) {
  // inject a #GP(0) since SMX is disabled in the IA32_FEATURE_CONTROL MSR
  inject_hw_exception(general_protection, 0);
}

void emulate_invd(vcpu*) {
  // TODO: properly implement INVD (can probably make a very small stub
  //       that flushes specific cacheline entries prior to executing INVD)
  inject_hw_exception(general_protection, 0);
}

void emulate_xsetbv(vcpu* const cpu) {
  // 3.2.6

  // CR4.OSXSAVE must be 1
  if (!read_effective_guest_cr4().os_xsave) {
    inject_hw_exception(invalid_opcode);
    return;
  }

  xcr0 new_xcr0;
  new_xcr0.flags = (cpu->ctx->rdx << 32) | cpu->ctx->eax;

 
  ULONG64 RCXANDRDX = cpu->ctx->rdx << 32 | cpu->ctx->rax;
  if (cpu->ctx->ecx ==0 &&RCXANDRDX ==0)
  {
	  inject_hw_exception(general_protection, 0);
	  return;
  }
  // only XCR0 is supported
  if (cpu->ctx->ecx != 0) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if trying to set an unsupported bit
  if (new_xcr0.flags & cpu->cached.xcr0_unsupported_mask) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if clearing XCR0.X87
  if (!new_xcr0.x87) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if XCR0.AVX is 1 while XCRO.SSE is cleared
  if (new_xcr0.avx && !new_xcr0.sse) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if XCR0.AVX is clear and XCR0.opmask, XCR0.ZMM_Hi256, or XCR0.Hi16_ZMM is set
  if (!new_xcr0.avx && (new_xcr0.opmask || new_xcr0.zmm_hi256 || new_xcr0.zmm_hi16)) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if setting XCR0.BNDREG or XCR0.BNDCSR while not setting the other
  if (new_xcr0.bndreg != new_xcr0.bndcsr) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if setting XCR0.opmask, XCR0.ZMM_Hi256, or XCR0.Hi16_ZMM while not setting all of them
  if (new_xcr0.opmask != new_xcr0.zmm_hi256 || new_xcr0.zmm_hi256 != new_xcr0.zmm_hi16) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  host_exception_info e;
  xsetbv_safe(e, cpu->ctx->ecx, new_xcr0.flags);

  if (e.exception_occurred) {
    // TODO: assert that it was a #GP(0) that occurred, although I really
    //       doubt that any other exception could happen (according to manual).
    inject_hw_exception(general_protection, 0);
    return;
  }

  HV_LOG_VERBOSE("Wrote %p to XCR0.", new_xcr0.flags);

  cpu->hide_vm_exit_overhead = true;
  skip_instruction();
}

void emulate_vmxon(vcpu*) {
  // usually a #UD doesn't trigger a vm-exit, but in this case it is possible
  // that CR4.VMXE is 1 while guest shadow CR4.VMXE is 0.
  if (!read_effective_guest_cr4().vmx_enable) {
    inject_hw_exception(invalid_opcode);
    return;
  }

  // we are spoofing the value of the IA32_FEATURE_CONTROL MSR in
  // order to convince the guest that VMX has been disabled by BIOS.
  inject_hw_exception(general_protection, 0);
}

void emulate_vmcall(vcpu* const cpu) {
  auto const code = cpu->ctx->rax & 0xFF;
  auto const key  = cpu->ctx->rax >> 8;

  if (hv::get_guest_cpl() != 0)
  {
	  hv::inject_interruption(EXCEPTION_VECTOR_GENERAL_PROTECTION_FAULT, 3, 0, 1);
	  return;
  }
  // validate the hypercall key
  if (key != hypercall_key  ) {
    HV_LOG_VERBOSE("Invalid VMCALL key. RIP=%p.", vmx_vmread(VMCS_GUEST_RIP));
    inject_hw_exception(invalid_opcode);
    return;
  }





  // handle the hypercall
  switch (code) {
  case hypercall_ping:                 hc::ping(cpu);                 return;
  case hypercall_test:                 hc::test(cpu);                 return;
  case hypercall_unload:               hc::unload(cpu);               return;
  case hypercall_read_phys_mem:        hc::read_phys_mem(cpu);        return;
  case hypercall_write_phys_mem:       hc::write_phys_mem(cpu);       return;
  case hypercall_read_virt_mem:        hc::read_virt_mem(cpu);        return;
  case hypercall_write_virt_mem:       hc::write_virt_mem(cpu);       return;
  case hypercall_query_process_cr3:    hc::query_process_cr3(cpu);    return;
  case hypercall_install_ept_hook:     hc::install_ept_hook(cpu);     return;
  case hypercall_remove_ept_hook:      hc::remove_ept_hook(cpu);      return;
  case hypercall_flush_logs:           hc::flush_logs(cpu);           return;
  case hypercall_get_physical_address: hc::get_physical_address(cpu); return;
  case hypercall_hide_physical_page:   hc::hide_physical_page(cpu);   return;
  case hypercall_unhide_physical_page: hc::unhide_physical_page(cpu); return;
  case hypercall_get_hv_base:          hc::get_hv_base(cpu);          return;
  case hypercall_install_mmr:          hc::install_mmr(cpu);          return;
  case hypercall_remove_mmr:           hc::remove_mmr(cpu);           return;
  case hypercall_remove_all_mmrs:      hc::remove_all_mmrs(cpu);      return;
  case hypercall_set_msr_fault_bitmaps:hc::set_invalid_msr_bitmaps(cpu); return;
  

  }

  HV_LOG_VERBOSE("Unhandled VMCALL. RIP=%p.", vmx_vmread(VMCS_GUEST_RIP));

  inject_hw_exception(invalid_opcode);
}

void handle_vmx_preemption(vcpu*) {
  // do nothing.
}

void emulate_mov_to_cr0(vcpu* const cpu, uint64_t const gpr) {
  // 2.4.3
  // 3.2.5
  // 3.4.10.1
  // 3.26.3.2.1

  cr0 new_cr0;
  new_cr0.flags = read_guest_gpr(cpu->ctx, gpr);

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

  // #GP(0) if setting any reserved bits in CR0[63:32]
  if (new_cr0.reserved4) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if setting CR0.PG while CR0.PE is clear
  if (new_cr0.paging_enable && !new_cr0.protection_enable) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if invalid bit combination
  if (!new_cr0.cache_disable && new_cr0.not_write_through) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if an attempt is made to clear CR0.PG
  if (!new_cr0.paging_enable) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if an attempt is made to clear CR0.WP while CR4.CET is set
  if (!new_cr0.write_protect && curr_cr4.control_flow_enforcement_enable) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // the guest tried to modify CR0.CD or CR0.NW, which must be updated manually
  if (new_cr0.cache_disable     != curr_cr0.cache_disable ||
      new_cr0.not_write_through != curr_cr0.not_write_through) {
    // TODO: should we care about NW?
    if (new_cr0.cache_disable)
      set_ept_memory_type(cpu->ept, MEMORY_TYPE_UNCACHEABLE);
    else
      update_ept_memory_type(cpu->ept);

    vmx_invept(invept_all_context, {});
  }

  HV_LOG_VERBOSE("Writing %p to CR0.", new_cr0.flags);

  vmx_vmwrite(VMCS_CTRL_CR0_READ_SHADOW, new_cr0.flags);

  // make sure to account for VMX reserved bits when setting the real CR0
  new_cr0.flags |= cpu->cached.vmx_cr0_fixed0;
  new_cr0.flags &= cpu->cached.vmx_cr0_fixed1;

  vmx_vmwrite(VMCS_GUEST_CR0, new_cr0.flags);

  cpu->hide_vm_exit_overhead = true;
  skip_instruction();
}

void emulate_mov_to_cr3(vcpu* const cpu, uint64_t const gpr) {
  cr3 new_cr3;
  new_cr3.flags = read_guest_gpr(cpu->ctx, gpr);

  auto const curr_cr4 = read_effective_guest_cr4();

  bool invalidate_tlb = true;

  // 3.4.10.4.1
  if (curr_cr4.pcid_enable && (new_cr3.flags & (1ull << 63))) {
    invalidate_tlb = false;
    new_cr3.flags &= ~(1ull << 63);
  }

  // a mask where bits [63:MAXPHYSADDR] are set to 1
  auto const reserved_mask = ~((1ull << cpu->cached.max_phys_addr) - 1);

  // 3.2.5
  if (new_cr3.flags & reserved_mask) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // 3.28.4.3.3
  if (invalidate_tlb) {
    invvpid_descriptor desc;
    desc.linear_address = 0;
    desc.reserved1      = 0;
    desc.reserved2      = 0;
    desc.vpid           = guest_vpid;
    vmx_invvpid(invvpid_single_context_retaining_globals, desc);
  }

  // it is now safe to write the new guest cr3
  vmx_vmwrite(VMCS_GUEST_CR3, new_cr3.flags);

  cpu->hide_vm_exit_overhead = true;
  skip_instruction();
}

void emulate_mov_to_cr4(vcpu* const cpu, uint64_t const gpr) {
  // 2.4.3
  // 2.6.2.1
  // 3.2.5
  // 3.4.10.1
  // 3.4.10.4.1

  cr4 new_cr4;
  new_cr4.flags = read_guest_gpr(cpu->ctx, gpr);

  cr3 curr_cr3;
  curr_cr3.flags = vmx_vmread(VMCS_GUEST_CR3);

  auto const curr_cr0 = read_effective_guest_cr0();
  auto const curr_cr4 = read_effective_guest_cr4();

  // #GP(0) if an attempt is made to set CR4.SMXE when SMX is not supported
  if (!cpu->cached.cpuid_01.cpuid_feature_information_ecx.safer_mode_extensions
      && new_cr4.smx_enable) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if an attempt is made to write a 1 to any reserved bits
  if (new_cr4.reserved1 || new_cr4.reserved2) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if an attempt is made to change CR4.PCIDE from 0 to 1 while CR3[11:0] != 000H
  if ((new_cr4.pcid_enable && !curr_cr4.pcid_enable) && (curr_cr3.flags & 0xFFF)) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if CR4.PAE is cleared
  if (!new_cr4.physical_address_extension) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if CR4.LA57 is enabled
  if (new_cr4.linear_addresses_57_bit) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // #GP(0) if CR4.CET == 1 and CR0.WP == 0
  if (new_cr4.control_flow_enforcement_enable && !curr_cr0.write_protect) {
    inject_hw_exception(general_protection, 0);
    return;
  }

  // invalidate TLB entries if required
  if (new_cr4.page_global_enable != curr_cr4.page_global_enable ||
      !new_cr4.pcid_enable && curr_cr4.pcid_enable ||
      new_cr4.smep_enable && !curr_cr4.smep_enable) {
    invvpid_descriptor desc;
    desc.linear_address = 0;
    desc.reserved1      = 0;
    desc.reserved2      = 0;
    desc.vpid           = guest_vpid;
    vmx_invvpid(invvpid_single_context, desc);
  }
  
  HV_LOG_VERBOSE("Writing %p to CR4.", new_cr4.flags);

  vmx_vmwrite(VMCS_CTRL_CR4_READ_SHADOW, new_cr4.flags);

  // make sure to account for VMX reserved bits when setting the real CR4
  new_cr4.flags |= cpu->cached.vmx_cr4_fixed0;
  new_cr4.flags &= cpu->cached.vmx_cr4_fixed1;

  vmx_vmwrite(VMCS_GUEST_CR4, new_cr4.flags);

  cpu->hide_vm_exit_overhead = true;
  skip_instruction();
}

void emulate_mov_from_cr3(vcpu* const cpu, uint64_t const gpr) {
  write_guest_gpr(cpu->ctx, gpr, vmx_vmread(VMCS_GUEST_CR3));

  cpu->hide_vm_exit_overhead = true;
  skip_instruction();
}

void emulate_clts(vcpu* const cpu) {
  // clear CR0.TS in the read shadow
  vmx_vmwrite(VMCS_CTRL_CR0_READ_SHADOW, vmx_vmread(VMCS_CTRL_CR0_READ_SHADOW) & ~CR0_TASK_SWITCHED_FLAG);

  // clear CR0.TS in the real CR0 register
  vmx_vmwrite(VMCS_GUEST_CR0,
    vmx_vmread(VMCS_GUEST_CR0) & ~CR0_TASK_SWITCHED_FLAG);

  cpu->hide_vm_exit_overhead = true;
  skip_instruction();
}

void emulate_lmsw(vcpu* const cpu, uint16_t const value) {
  // 3.25.1.3

  cr0 new_cr0;
  new_cr0.flags = value;

  // update the guest CR0 read shadow
  cr0 shadow_cr0;
  shadow_cr0.flags = vmx_vmread(VMCS_CTRL_CR0_READ_SHADOW);
  shadow_cr0.protection_enable   = new_cr0.protection_enable;
  shadow_cr0.monitor_coprocessor = new_cr0.monitor_coprocessor;
  shadow_cr0.emulate_fpu         = new_cr0.emulate_fpu;
  shadow_cr0.task_switched       = new_cr0.task_switched;
  vmx_vmwrite(VMCS_CTRL_CR0_READ_SHADOW, shadow_cr0.flags);

  // update the real guest CR0.
  // we don't have to worry about VMX reserved bits since CR0.PE (the only
  // reserved bit) can't be cleared to 0 by the LMSW instruction while in
  // protected mode.
  cr0 real_cr0;
  real_cr0.flags = vmx_vmread(VMCS_GUEST_CR0);
  real_cr0.protection_enable   = new_cr0.protection_enable;
  real_cr0.monitor_coprocessor = new_cr0.monitor_coprocessor;
  real_cr0.emulate_fpu         = new_cr0.emulate_fpu;
  real_cr0.task_switched       = new_cr0.task_switched;
  vmx_vmwrite(VMCS_GUEST_CR0, real_cr0.flags);

  cpu->hide_vm_exit_overhead = true;
  skip_instruction();
}

void handle_mov_cr(vcpu* const cpu) {
  vmx_exit_qualification_mov_cr qualification;
  qualification.flags = vmx_vmread(VMCS_EXIT_QUALIFICATION);

  switch (qualification.access_type) {
  // MOV CRn, XXX
  case VMX_EXIT_QUALIFICATION_ACCESS_MOV_TO_CR:
    switch (qualification.control_register) {
    case VMX_EXIT_QUALIFICATION_REGISTER_CR0:
      emulate_mov_to_cr0(cpu, qualification.general_purpose_register);
      break;
    case VMX_EXIT_QUALIFICATION_REGISTER_CR3:
      emulate_mov_to_cr3(cpu, qualification.general_purpose_register);
      break;
    case VMX_EXIT_QUALIFICATION_REGISTER_CR4:
      emulate_mov_to_cr4(cpu, qualification.general_purpose_register);
      break;
    }
    break;
  // MOV XXX, CRn
  case VMX_EXIT_QUALIFICATION_ACCESS_MOV_FROM_CR:
    // TODO: assert that we're accessing CR3 (and not CR8)
    emulate_mov_from_cr3(cpu, qualification.general_purpose_register);
    break;
  // CLTS
  case VMX_EXIT_QUALIFICATION_ACCESS_CLTS:
    emulate_clts(cpu);
    break;
  // LMSW XXX
  case VMX_EXIT_QUALIFICATION_ACCESS_LMSW:
    emulate_lmsw(cpu, qualification.lmsw_source_data);
    break;
  }
}

void handle_nmi_window(vcpu* const cpu) {
  --cpu->queued_nmis;

  // inject the NMI into the guest
  inject_nmi();

  if (cpu->queued_nmis == 0) {
    // disable NMI-window exiting since we have no more NMIs to inject
    auto ctrl = read_ctrl_proc_based();
    ctrl.nmi_window_exiting = 0;
    write_ctrl_proc_based(ctrl);
  }
  
  // there is the possibility that a host NMI occurred right before we
  // disabled NMI-window exiting. make sure to re-enable it if this is the case.
  if (cpu->queued_nmis > 0) {
    auto ctrl = read_ctrl_proc_based();
    ctrl.nmi_window_exiting = 1;
    write_ctrl_proc_based(ctrl);
  }
}

void handle_exception_or_nmi(vcpu* const cpu) {

    UNREFERENCED_PARAMETER(cpu);
  // enqueue an NMI to be injected into the guest later on
	const auto interrupt_flags = static_cast<uint32_t>(vmx_vmread(VMCS_VMEXIT_INTERRUPTION_INFORMATION));
	const uint32_t error_code = static_cast<uint32_t>  (hv::vmread(VMCS_VMEXIT_INTERRUPTION_ERROR_CODE));
	vmexit_interrupt_information interrupt_info{};
	interrupt_info.flags = interrupt_flags;
    rflags guest_rflags{ vmx_vmread(VMCS_GUEST_RFLAGS) };
	const auto guest_rip = vmx_vmread(VMCS_GUEST_RIP);

	// === Page Fault ===
	if (interrupt_info.vector == EXCEPTION_VECTOR_PAGE_FAULT)
	{
		const auto qualification = hv::vmread(VMCS_EXIT_QUALIFICATION);
		__writecr2(qualification);
	}
	// === Single Step (#DB) 且 Trap Flag 未开启，判断是否为 Hook 原始地址命中 ===
	else if (interrupt_info.vector == EXCEPTION_VECTOR_SINGLE_STEP&& guest_rflags.trap_flag == false)
	{
	 
     
	 
          hooked_function_info matched_hook_info{};
		 if (utils::hook_utils:: find_hook_info_by_rip( reinterpret_cast<void*>(guest_rip), &matched_hook_info))
			{
				vmx_vmwrite(VMCS_GUEST_RIP, reinterpret_cast<ULONG_PTR>(matched_hook_info.new_handler_va));
				return;
			}
             
		 
	}
	else if (interrupt_info.vector == EXCEPTION_VECTOR_SINGLE_STEP  && guest_rflags.trap_flag == true)
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

void handle_vmx_instruction(vcpu*) {
  // inject #UD for every VMX instruction since we
  // don't allow the guest to ever enter VMX operation.
  inject_hw_exception(invalid_opcode);
}

void handle_ept_violation(vcpu* const cpu) {
  vmx_exit_qualification_ept_violation qualification;
  qualification.flags = vmx_vmread(VMCS_EXIT_QUALIFICATION);

  // guest physical address that caused the ept-violation
  auto const physical_address = vmx_vmread(qualification.caused_by_translation ?
    VMCS_GUEST_PHYSICAL_ADDRESS : VMCS_EXIT_GUEST_LINEAR_ADDRESS);

  auto const pte = get_ept_pte(cpu->ept, physical_address);

  for (auto const& entry : cpu->ept.mmr) {
    // ignore pages that aren't being monitored
    if (physical_address < (entry.start & ~0xFFFull))
      continue;
    if (physical_address >= ((entry.start + entry.size + 0xFFF) & ~0xFFFull))
      continue;

    pte->read_access    = 1;
    pte->write_access   = 1;
    pte->execute_access = 1;

    auto const is_relevant_mode =
         (qualification.read_access    && (entry.mode & mmr_memory_mode_r))
      || (qualification.write_access   && (entry.mode & mmr_memory_mode_w))
      || (qualification.execute_access && (entry.mode & mmr_memory_mode_x));

    if (is_relevant_mode &&
        physical_address >= entry.start &&
        physical_address < (entry.start + entry.size)) {

      char name[16] = {};
      current_guest_image_file_name(name);

      char mode[4] = "---";
      if (qualification.read_access)
        mode[0] = 'r';
      if (qualification.write_access)
        mode[1] = 'w';
      if (qualification.execute_access)
        mode[2] = 'x';

      HV_LOG_MMR_ACCESS("[%s] accessed memory at physical address <%p>:", name, physical_address);
      HV_LOG_MMR_ACCESS("    MODE: %s", mode);
      HV_LOG_MMR_ACCESS("    PID:  %p", current_guest_pid());
      HV_LOG_MMR_ACCESS("    CPL:  %i", current_guest_cpl());
      HV_LOG_MMR_ACCESS("    RIP:  %p", vmx_vmread(VMCS_GUEST_RIP));
      HV_LOG_MMR_ACCESS("    RSP:  %p", vmx_vmread(VMCS_GUEST_RSP));
      HV_LOG_MMR_ACCESS("    RAX:  %p", cpu->ctx->rax);
      HV_LOG_MMR_ACCESS("    RCX:  %p", cpu->ctx->rcx);
      HV_LOG_MMR_ACCESS("    RDX:  %p", cpu->ctx->rdx);
      HV_LOG_MMR_ACCESS("    RBX:  %p", cpu->ctx->rbx);
      HV_LOG_MMR_ACCESS("    RBP:  %p", cpu->ctx->rbp);
      HV_LOG_MMR_ACCESS("    RSI:  %p", cpu->ctx->rsi);
      HV_LOG_MMR_ACCESS("    RDI:  %p", cpu->ctx->rdi);
      HV_LOG_MMR_ACCESS("    R8:   %p", cpu->ctx->r8);
      HV_LOG_MMR_ACCESS("    R9:   %p", cpu->ctx->r9);
      HV_LOG_MMR_ACCESS("    R10:  %p", cpu->ctx->r10);
      HV_LOG_MMR_ACCESS("    R11:  %p", cpu->ctx->r11);
      HV_LOG_MMR_ACCESS("    R12:  %p", cpu->ctx->r12);
      HV_LOG_MMR_ACCESS("    R13:  %p", cpu->ctx->r13);
      HV_LOG_MMR_ACCESS("    R14:  %p", cpu->ctx->r14);
      HV_LOG_MMR_ACCESS("    R15:  %p", cpu->ctx->r15);
    }

    cpu->ept.mmr_mtf_pte  = pte;
    cpu->ept.mmr_mtf_mode = entry.mode;

    enable_monitor_trap_flag();

    return;
  }

  if (qualification.execute_access &&
     (qualification.write_access || qualification.read_access)) {
    HV_LOG_ERROR("Invalid EPT access combination. PhysAddr = %p.", physical_address);
    inject_hw_exception(general_protection, 0);
    return;
  }

  auto const hook = find_ept_hook(cpu->ept, physical_address >> 12);

  if (!hook) {
    HV_LOG_ERROR("Failed to find EPT hook. PhysAddr = %p.", physical_address);
    inject_hw_exception(general_protection, 0);
    return;
  }

  if (qualification.execute_access) {
    pte->read_access       = 0;
    pte->write_access      = 0;
    pte->execute_access    = 1;
    pte->page_frame_number = hook->exec_pfn;
  } else {
    pte->read_access       = 1;
    pte->write_access      = 1;
    pte->execute_access    = 0;
    pte->page_frame_number = hook->orig_pfn;
  }
}

void handle_vmx_rfalgs(vcpu* cpu)
{
	rflags guest_rflags{  };
	guest_rflags.flags = vmx_vmread(VMCS_GUEST_RFLAGS);
	guest_rflags.zero_flag=true;
	__vmx_vmwrite(VMCS_GUEST_RFLAGS, guest_rflags.flags);
	skip_instruction();
}

void emulate_rdtsc(vcpu* const cpu) {
  auto const tsc = __rdtsc();

  // return current TSC
  cpu->ctx->rax = tsc & 0xFFFFFFFF;
  cpu->ctx->rdx = (tsc >> 32) & 0xFFFFFFFF;

  skip_instruction();
}

void emulate_rdtscp(vcpu* const cpu) {
  unsigned int aux = 0;
  auto const tsc = __rdtscp(&aux);

  // return current TSC
  cpu->ctx->rax = tsc & 0xFFFFFFFF;
  cpu->ctx->rdx = (tsc >> 32) & 0xFFFFFFFF;
  cpu->ctx->rcx = aux;

  skip_instruction();
}

void handle_monitor_trap_flag(vcpu* const cpu) {
  auto& pte       = cpu->ept.mmr_mtf_pte;
  auto const mode = cpu->ept.mmr_mtf_mode;

  // restore MMR mode
  if (pte) {
    pte->read_access    = !(mode & mmr_memory_mode_r);
    pte->write_access   = !(mode & mmr_memory_mode_w);
    pte->execute_access = !(mode & mmr_memory_mode_x);

    // write access but no read access will generate an EPT misconfiguration
    if (pte->write_access && !pte->read_access)
      pte->write_access = 0;

    pte = nullptr;

    vmx_invept(invept_all_context, {});
  }

  disable_monitor_trap_flag();
}

void handle_ept_misconfiguration(vcpu*) {
  auto const phys = vmx_vmread(VMCS_GUEST_PHYSICAL_ADDRESS);
  HV_LOG_ERROR("Unhandled EPT Misconfiguration: %p", phys);
}

} // namespace hv

