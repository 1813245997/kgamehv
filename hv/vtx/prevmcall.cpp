#include "hv.h"
#include "prevmcall.h"
#include "processor_affinity.h"
namespace prevmcall
{
	void install_ept_hook(uint64_t original_pfn, uint64_t fake_pfn)
	{
		hv::hypercall_input input{};
		input.key = hv::hypercall_key;
		input.code = hv::hypercall_code::hypercall_install_ept_hook;
		input.args[0] = original_pfn;
		input.args[1] = fake_pfn;

		hv::ForEachLogicalProcessor([](void* input) {
			hv::vmx_vmcall(*reinterpret_cast<hv::hypercall_input*>(input));
			}, &input);
		 
	}

	void remove_ept_hook(uint64_t pfn)
	{
		hv::hypercall_input input{};
		input.key = hv::hypercall_key;
		input.code = hv::hypercall_code::hypercall_remove_ept_hook;
		input.args[0] =  pfn;

		hv::ForEachLogicalProcessor(
			[](void* input) {
				hv::vmx_vmcall(*reinterpret_cast<hv::hypercall_input*>(input));
			}, &input
		);
	}

	cr3 query_process_cr3(uint64_t const pid)
	{
		hv::hypercall_input input;
		input.code = hv::hypercall_query_process_cr3;
		input.key = hv::hypercall_key;
		input.args[0] = pid;

		cr3 cr3;
		cr3.flags = hv::vmx_vmcall(input);
		return cr3;
	}

}