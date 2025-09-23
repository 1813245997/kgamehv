#pragma once
#include "../ia32/ia32.hpp"

namespace hv {
	 
	inline constexpr uint64_t hypercall_key = 84320;

	enum hypercall_code : uint64_t
	{
		hypercall_test,
		hypercall_vmxoff,
		hypercall_install_ept_hook,
		hypercall_remove_ept_hook,
		hypercall_dump_pool_manager,
		hypercall_dump_vmcs_state,
		hypercall_read_phys_mem,
		hypercall_write_phys_mem,
		hypercall_read_virt_mem,
		hypercall_write_virt_mem,
		hypercall_query_process_cr3,
	};


	// hypercall input
	struct hypercall_input {
		// rax
		struct {
			hypercall_code code : 8;
			uint64_t       key : 56;
		};

		// rcx, rdx, r8, r9, r10, r11
		uint64_t args[6];
	};


	 
	namespace hc {

		void test(__vcpu* vcpu);

		void vmoff(__vcpu* vcpu);

		void install_ept_hook(__vcpu* vcpu);

		void remove_ept_hook(__vcpu* vcpu);

		void dump_pools_info(__vcpu* vcpu);

		void dump_vmcs(__vcpu* vcpu);

		void read_phys_mem(__vcpu* vcpu);

		void write_phys_mem(__vcpu* vcpu);

		void read_virt_mem(__vcpu* vcpu);

		void write_virt_mem(__vcpu* vcpu);

		void query_process_cr3(__vcpu* vcpu);
	}

 
	uint64_t vmx_vmcall(hypercall_input& input);
 
}