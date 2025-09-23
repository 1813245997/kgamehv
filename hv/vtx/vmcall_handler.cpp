#pragma warning( disable : 4201 4244 4805 4189)
#include "../utils/global_defs.h"
#include "vmcall_handler.h"

namespace hv
{
	void vmexit_vmcall_handler(__vcpu* vcpu)
	{

		auto const code = vcpu->vmexit_info.guest_registers->rax & 0xFF;
		auto const key = vcpu->vmexit_info.guest_registers->rax >> 8;

		if (key != hypercall_key) {
			inject_interruption(EXCEPTION_VECTOR_UNDEFINED_OPCODE, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, false);
			return;
		}
	  
		switch (code)
		{
		case   hypercall_test:			   hc::test(vcpu);				return;
		case   hypercall_vmxoff:		   hc::vmoff(vcpu);				return;
		case   hypercall_install_ept_hook: hc::install_ept_hook(vcpu);	return;
		case   hypercall_remove_ept_hook:  hc::remove_ept_hook(vcpu);   return;
		case   hypercall_dump_pool_manager:hc::dump_pools_info(vcpu);	return;
		case   hypercall_dump_vmcs_state:  hc::dump_vmcs(vcpu);			return;
		case   hypercall_read_phys_mem:    hc::read_phys_mem(vcpu);		return;
		case   hypercall_write_phys_mem:   hc::write_phys_mem(vcpu);	return;
		case   hypercall_read_virt_mem:    hc::read_virt_mem(vcpu);		return;
		case   hypercall_write_virt_mem:   hc::write_virt_mem(vcpu);	return;
		case   hypercall_query_process_cr3:hc::query_process_cr3(vcpu);	return;

		}

		inject_interruption(EXCEPTION_VECTOR_UNDEFINED_OPCODE, INTERRUPT_TYPE_HARDWARE_EXCEPTION, 0, false);
	}

 }


 