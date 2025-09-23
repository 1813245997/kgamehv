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



	}
}