#include "global_defs.h"
#include "user_call_utils.h"

namespace utils
{
	namespace user_call
	{
		unsigned long long call(unsigned long long func_ptr, unsigned long long arg1, unsigned long long arg2, unsigned long long arg3, unsigned long long arg4)
		{
				unsigned long long current_irql = asm_read_cr8();
				bool i_enable = asm_read_rflags() & 0x200;

				if (current_irql > PASSIVE_LEVEL) {
					asm_write_cr8(PASSIVE_LEVEL);
				}

				 
			void* cdata[9]{};
			auto new_user_rsp =  thread_utils::get_user_stack_ptr() - 0x98 & 0xFFFFFFFFFFFFFFF0;
			 
			*(unsigned long long*)(new_user_rsp + 0x20 + (0 * 8)) = arg1;
			*(unsigned long long*)(new_user_rsp + 0x20 + (1 * 8)) = arg2;
			*(unsigned long long*)(new_user_rsp + 0x20 + (2 * 8)) = arg3;
			*(unsigned long long*)(new_user_rsp + 0x20 + (3 * 8)) = arg4;

			unsigned long long  ret = call2(func_ptr, reinterpret_cast<char*> (new_user_rsp), cdata);

			if (current_irql > PASSIVE_LEVEL) {
				asm_write_cr8(current_irql);
			}

			if (!i_enable)
			{
				asm_cli();
			}
			return ret;
		}


		HRESULT call_ret_hr(unsigned long long func_ptr, unsigned long long arg1, unsigned long long arg2, unsigned long long arg3, unsigned long long arg4)
		{
			unsigned long long current_irql = asm_read_cr8();
			bool i_enable = asm_read_rflags() & 0x200;

			if (current_irql > PASSIVE_LEVEL) {
				asm_write_cr8(PASSIVE_LEVEL);
			}


			void* cdata[9]{};
			auto new_user_rsp = thread_utils::get_user_stack_ptr() - 0x98 & 0xFFFFFFFFFFFFFFF0;

			*(unsigned long long*)(new_user_rsp + 0x20 + (0 * 8)) = arg1;
			*(unsigned long long*)(new_user_rsp + 0x20 + (1 * 8)) = arg2;
			*(unsigned long long*)(new_user_rsp + 0x20 + (2 * 8)) = arg3;
			*(unsigned long long*)(new_user_rsp + 0x20 + (3 * 8)) = arg4;

			unsigned long long  ret = call2(func_ptr, reinterpret_cast<char*> (new_user_rsp), cdata);

			if (current_irql > PASSIVE_LEVEL) {
				asm_write_cr8(current_irql);
			}

			if (!i_enable)
			{
				asm_cli();
			}
			if (ret)
			{
				HRESULT 	hr = *reinterpret_cast<PULONG>(ret);
				return  hr;
			}
			return S_FALSE;
		}


		unsigned long long call6(
			unsigned long long func_ptr, 
			unsigned long long arg1,
			unsigned long long arg2, 
			unsigned long long arg3,
			unsigned long long arg4 , 
			unsigned long long arg5,
			unsigned long long arg6)
		{
		 
			unsigned long long current_irql = asm_read_cr8();
			bool i_enable = asm_read_rflags() & 0x200;

			if (current_irql > PASSIVE_LEVEL) {
				asm_write_cr8(PASSIVE_LEVEL);
			}


			void* cdata[9]{};
			auto new_user_rsp = thread_utils::get_user_stack_ptr() - 0x98 & 0xFFFFFFFFFFFFFFF0;

			*(unsigned long long*)(new_user_rsp + 0x20 + (0 * 8)) = arg1;
			*(unsigned long long*)(new_user_rsp + 0x20 + (1 * 8)) = arg2;
			*(unsigned long long*)(new_user_rsp + 0x20 + (2 * 8)) = arg3;
			*(unsigned long long*)(new_user_rsp + 0x20 + (3 * 8)) = arg4;
			*(unsigned long long*)(new_user_rsp + 0x70 + (0 * 8)) = arg5;
			*(unsigned long long*)(new_user_rsp + 0x70 + (1 * 8)) = arg6;

			unsigned long long  ret = call2(func_ptr, reinterpret_cast<char*> (new_user_rsp), cdata);

			if (current_irql > PASSIVE_LEVEL) {
				asm_write_cr8(current_irql);
			}

			if (!i_enable)
			{
				asm_cli();
			}
 
			return ret;
		}

		unsigned long long call2(
			unsigned long long ptr,
			char* user_rsp,
			void** call_data)
		{

			
			//alloc new kernel stack
			unsigned long long  ret_val_ptr{};
			auto current_thread = reinterpret_cast<unsigned long long>  (internal_functions::pfn_ke_get_current_thread());

			auto kernel_stack = reinterpret_cast<unsigned long long > (internal_functions::pfn_mm_create_kernel_stack (false, 0, 0));
			if (!kernel_stack)
			{
				return 0;
			}

			//fill kernel stack control
			auto kernel_stack_control = kernel_stack - 0x50;
			*(unsigned long long *)(kernel_stack - 0x50) = kernel_stack;
			*(unsigned long long *)(kernel_stack - 0x48) = kernel_stack - 0x6000;
			*(unsigned long long *)(kernel_stack - 0x40) = *(unsigned long long*)(current_thread + 0x38);
			*(unsigned long long *)(kernel_stack - 0x38) = *(unsigned long long*)(current_thread + 0x30);
			*(unsigned long long *)(kernel_stack - 0x28) = *(unsigned long long*)(current_thread + 0x28);


			//fill kernel shadow stack control
			*(unsigned long long*)(kernel_stack - 0x20) = 0;
			*(unsigned long long*)(kernel_stack - 0x18) = 0;
			*(unsigned long long*)(kernel_stack - 0x10) = 0;
			*(unsigned long long*)(kernel_stack - 0x08) = 0;


			//push ret to kernel addr
			*(unsigned long long*)(user_rsp + 0x48) = dwm_draw::g_postcall_addr;
			call_data[2] = reinterpret_cast<void *> (ptr);
			call_data[1] = user_rsp;
			call_data[0] = reinterpret_cast<void*> (dwm_draw::g_precall_addr);


			//get apc state
			auto apcIndex = *(unsigned char*)(current_thread + 0x24A);
			auto v1 = *(unsigned short*)(current_thread + 0x1E4);
			auto v2 = *(unsigned short*)(current_thread + 0x1E6);

			//fix apc state
			*(unsigned short*)(current_thread + 0x1E4) = 0;
			*(unsigned short*)(current_thread + 0x1E6) = 0;
			*(unsigned char*)(current_thread + 0x24A) = 0;

			typedef VOID(*FnKiCallUserMode2)(unsigned long long* OutVarPtr, unsigned long long CallCtx, unsigned long long KStackControl);
			auto pFnKiCallUserMode2 = (FnKiCallUserMode2)dwm_draw::g_ki_call_user_mode2;

		 
			pFnKiCallUserMode2( &ret_val_ptr, reinterpret_cast<unsigned long long> (call_data), kernel_stack_control);

			 
			//restore apc state
			*(unsigned short*)(current_thread + 0x1E4) = v1;
			*(unsigned short*)(current_thread + 0x1E6) = v2;
			*(unsigned char*)(current_thread + 0x24A) = apcIndex;

			internal_functions::pfn_mm_delete_kernel_stack(reinterpret_cast<PVOID> (kernel_stack), false);
			return ret_val_ptr;

		}
		
	}


}