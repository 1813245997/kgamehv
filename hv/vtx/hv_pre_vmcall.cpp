#include "../utils/global_defs.h"
#include "hv_pre_vmcall.h"

namespace hv
{
	namespace prevmcall
	{
		bool test_vmcall()
	   {
		 hypercall_input input{};
		 
		input.key =  hypercall_key;
		input.code = hypercall_code::hypercall_test;

		return  vmx_vmcall(input);
		 
	   }

		void install_ept_hook(uint64_t pfn_of_hooked_page,  uint64_t pfn_of_fake_page)
		{
						
			 hypercall_input input{};
			input.key =  hypercall_key;
			input.code =  hypercall_code::hypercall_install_ept_hook;
			input.args[0] = pfn_of_hooked_page;
			input.args[1] = pfn_of_fake_page;
			 
			 for_each_logical_processor([](void* input) {

				 vmx_vmcall(*reinterpret_cast< hypercall_input*>(input));


				}, &input);

		}

        
        void remove_ept_hook(uint64_t pfn_of_hooked_page )
        {
			 hypercall_input input{};
			input.key =  hypercall_key;
			input.code =  hypercall_code::hypercall_remove_ept_hook;
			input.args[0] = pfn_of_hooked_page;

			 for_each_logical_processor(
				[](void* input) {
					 vmx_vmcall(*reinterpret_cast< hypercall_input*>(input));
				}, &input
			);
        }

		cr3  query_process_cr3(uint64_t pid)
		{
			hypercall_input input{};
			input.key =  hypercall_key;
			input.code =  hypercall_code::hypercall_query_process_cr3;
			input.args[0] = pid;
			cr3 value{};
			value.flags =  vmx_vmcall(input);
			return value;
		}

		size_t read_virt_mem(cr3 cr3, void* dst, void const* src, size_t size)
		{
			hypercall_input input{};
			input.key =  hypercall_key;
			input.code =  hypercall_code::hypercall_read_virt_mem;
			input.args[0] = cr3.flags;
			input.args[1] = reinterpret_cast<uint64_t>(dst);
			input.args[2] = reinterpret_cast<uint64_t>(src);
			input.args[3] = size;
			return vmx_vmcall(input);
		}

		size_t write_virt_mem(cr3 cr3, void* dst, void const* src, size_t size)
		{
			hypercall_input input{};	
			input.key =  hypercall_key;
			input.code =  hypercall_code::hypercall_write_virt_mem;
			input.args[0] = cr3.flags;
			input.args[1] = reinterpret_cast<uint64_t>(dst);
			input.args[2] = reinterpret_cast<uint64_t>(src);
			input.args[3] = size;
			return vmx_vmcall(input);
		}
	}
}