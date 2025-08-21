#include "hv.h"
#include "prevmcall.h"
#include "processor_affinity.h"
namespace prevmcall
{
	  size_t read_phys_mem(void* const dst,
		uint64_t const src, size_t const size) {
		hv::hypercall_input input;
		input.code = hv::hypercall_read_phys_mem;
		input.key = hv::hypercall_key;
		input.args[0] = reinterpret_cast<uint64_t>(dst);
		input.args[1] = src;
		input.args[2] = size;
		return hv::vmx_vmcall(input);
	}

	  size_t write_phys_mem(uint64_t const dst,
		  void const* const src, size_t const size) {
		  hv::hypercall_input input;
		  input.code = hv::hypercall_write_phys_mem;
		  input.key = hv::hypercall_key;
		  input.args[0] = dst;
		  input.args[1] = reinterpret_cast<uint64_t>(src);
		  input.args[2] = size;
		  return hv::vmx_vmcall(input);
	  }

	  size_t read_virt_mem(cr3 const cr3, void* const dst,
		  void const* const src, size_t const size) {
		  hv::hypercall_input input;
		  input.code = hv::hypercall_read_virt_mem;
		  input.key = hv::hypercall_key;
		  input.args[0] = cr3.flags;
		  input.args[1] = reinterpret_cast<uint64_t>(dst);
		  input.args[2] = reinterpret_cast<uint64_t>(src);
		  input.args[3] = size;
		  return hv::vmx_vmcall(input);
	  }
	   size_t write_virt_mem(cr3 const cr3, void* const dst,
		  void const* const src, size_t const size) {
		  hv::hypercall_input input;
		  input.code = hv::hypercall_write_virt_mem;
		  input.key = hv::hypercall_key;
		  input.args[0] = cr3.flags;
		  input.args[1] = reinterpret_cast<uint64_t>(dst);
		  input.args[2] = reinterpret_cast<uint64_t>(src);
		  input.args[3] = size;
		  return hv::vmx_vmcall(input);
	  }


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