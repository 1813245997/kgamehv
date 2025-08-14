#include "global_defs.h"
#include "user_call_utils.h"

namespace utils
{
	namespace user_call
	{
		unsigned long long call(unsigned long long func_ptr, unsigned long long arg1, unsigned long long arg2, unsigned long long arg3, unsigned long long arg4)
		{
			/*	unsigned long long current_irql = asm_read_cr8();
				bool i_enable = asm_read_rflags() & 0x200;

				if (current_irql > PASSIVE_LEVEL) {
					asm_write_cr8(PASSIVE_LEVEL);
				}*/

				 
			void* cdata[9]{};
			auto new_user_rsp =  thread_utils::get_user_stack_ptr() - 0x98 & 0xFFFFFFFFFFFFFFF0;
			 
			*(unsigned long long*)(new_user_rsp + 0x20 + (0 * 8)) = arg1;
			*(unsigned long long*)(new_user_rsp + 0x20 + (1 * 8)) = arg2;
			*(unsigned long long*)(new_user_rsp + 0x20 + (2 * 8)) = arg3;
			*(unsigned long long*)(new_user_rsp + 0x20 + (3 * 8)) = arg4;

			unsigned long long  ret = call2(func_ptr, reinterpret_cast<char*> (new_user_rsp), cdata);

		/*	if (current_irql > PASSIVE_LEVEL) {
				asm_write_cr8(current_irql);
			}

			if (!i_enable)
			{
				asm_cli();
			}*/
			return ret;
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
		 
		/*	unsigned long long current_irql = asm_read_cr8();
			bool i_enable = asm_read_rflags() & 0x200;

			if (current_irql > PASSIVE_LEVEL) {
				asm_write_cr8(PASSIVE_LEVEL);
			}*/


			void* cdata[9]{};
			auto new_user_rsp = thread_utils::get_user_stack_ptr() - 0x98 & 0xFFFFFFFFFFFFFFF0;

			*(unsigned long long*)(new_user_rsp + 0x20 + (0 * 8)) = arg1;
			*(unsigned long long*)(new_user_rsp + 0x20 + (1 * 8)) = arg2;
			*(unsigned long long*)(new_user_rsp + 0x20 + (2 * 8)) = arg3;
			*(unsigned long long*)(new_user_rsp + 0x20 + (3 * 8)) = arg4;
			*(unsigned long long*)(new_user_rsp + 0x70 + (0 * 8)) = arg5;
			*(unsigned long long*)(new_user_rsp + 0x70 + (1 * 8)) = arg6;

			unsigned long long  ret = call2(func_ptr, reinterpret_cast<char*> (new_user_rsp), cdata);

		/*	if (current_irql > PASSIVE_LEVEL) {
				asm_write_cr8(current_irql);
			}

			if (!i_enable)
			{
				asm_cli();
			}*/
 
			return ret;
		}

		unsigned long long call11(
			unsigned long long func_ptr,
			unsigned long long arg1,
			unsigned long long arg2,
			unsigned long long arg3,
			unsigned long long arg4,
			unsigned long long arg5,
			unsigned long long arg6,
			unsigned long long arg7,
			unsigned long long arg8,
			unsigned long long arg9,
			unsigned long long arg10,
			unsigned long long arg11
	 
		)
		{
			void* cdata[9]{};
			auto new_user_rsp = (thread_utils::get_user_stack_ptr() - 0x200) & 0xFFFFFFFFFFFFFFF0ULL;

			// 前4个参数放寄存器影子区
			*(unsigned long long*)(new_user_rsp + 0x20 + (0 * 8)) = arg1;
			*(unsigned long long*)(new_user_rsp + 0x20 + (1 * 8)) = arg2;
			*(unsigned long long*)(new_user_rsp + 0x20 + (2 * 8)) = arg3;
			*(unsigned long long*)(new_user_rsp + 0x20 + (3 * 8)) = arg4;

			// 剩下的参数依次压到栈
			unsigned long long extra_args[] = {
				arg5, arg6, arg7, arg8,
				arg9, arg10, arg11 
			};

			for (int i = 0; i < 12; ++i)
			{
				*(unsigned long long*)(new_user_rsp + 0x70 + (i * 8)) = extra_args[i];
			}

			unsigned long long ret = call2(func_ptr, reinterpret_cast<char*>(new_user_rsp), cdata);
			return ret;
		}

		unsigned long long call2(
			unsigned long long ptr,
			char* user_rsp,
			void** call_data)
		{

			
			//alloc new kernel stack
			 
		
			unsigned long long  ret_val_ptr{};
			unsigned long long kernel_stack = 0;
			typedef NTSTATUS(__fastcall* PMM_CREATE_KERNEL_STACK_WIN11)(PMM_KERNEL_STACK_CONTEXT Info);

			typedef VOID(__fastcall* PMM_DELETE_KERNEL_STACK_WIN11)(PMM_KERNEL_STACK_CONTEXT Info);

			auto pfn_mm_create_kernel_stack_win11 = reinterpret_cast<PMM_CREATE_KERNEL_STACK_WIN11>(internal_functions::pfn_mm_create_kernel_stack);

			auto pfn_mm_delete_kernel_stack_win11 = reinterpret_cast<PMM_DELETE_KERNEL_STACK_WIN11>(internal_functions::pfn_mm_delete_kernel_stack);

			auto current_thread = reinterpret_cast<unsigned long long>  (internal_functions::pfn_ke_get_current_thread());

			MM_KERNEL_STACK_CONTEXT info{};
		   if (utils::os_info::get_build_number()>=WINDOWS_11_VERSION_24H2)
		   {
			 
			   // 反编译里的 0x500000010 拆解
			   info.StackFlags =5;  // 原始 IDA 里低 32 位的 0x10
			   info.StackType = 0;     // 高 32 位的 0x5 (0x500000000 >> 32)

			 
			   NTSTATUS status = pfn_mm_create_kernel_stack_win11(&info);
		
			   if (!NT_SUCCESS(status) || info.StackBase == 0) {
				   return 0;
			   }
			   kernel_stack = (unsigned long long)info.StackBase;
		   }
		   else
		   {
			   kernel_stack = reinterpret_cast<unsigned long long> (internal_functions::pfn_mm_create_kernel_stack(false, 0, 0));
			   if (!kernel_stack)
			   {
				   return 0;
			   }
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
			if (utils::os_info::get_build_number() >= WINDOWS_11_VERSION_24H2)
			{
				pfn_mm_delete_kernel_stack_win11(&info);
			}
			else
			{
				internal_functions::pfn_mm_delete_kernel_stack(reinterpret_cast<PVOID> (kernel_stack), false);
			}
			
			return ret_val_ptr;

		}

		HMODULE load_librarya(const char* dll_name, PVOID user_buffer)
		{
			if (!dll_name || !user_buffer || !utils::dwm_draw::g_load_librarya_fun)
				return nullptr;

			// 确保偏移固定，避免缓冲区冲突
			PVOID dll_name_ptr = reinterpret_cast<PBYTE>(user_buffer) + 0x100;

			// 拷贝 DLL 名
			SIZE_T dll_name_len = (strlen(dll_name) + 1) * sizeof(char);
			RtlCopyMemory(dll_name_ptr, dll_name, dll_name_len);
			reinterpret_cast<char*>(dll_name_ptr)[dll_name_len - 1] = '\0';

			// 调用用户态 LoadLibraryA
			unsigned long long result_ptr = utils::user_call::call(
				utils::dwm_draw::g_load_librarya_fun,
				reinterpret_cast<unsigned long long>(dll_name_ptr),
				0, 0, 0);

			if (result_ptr)
			{
				HMODULE module = reinterpret_cast<HMODULE>(
					*reinterpret_cast<PULONG64>(result_ptr));

				RtlZeroMemory(user_buffer, 0x1000);
				if (module)
				{
					return module;
				}
			}

			return nullptr;
		}
		

		FARPROC get_proc_address(
			HMODULE module,
			const char* func_name,
			PVOID user_buffer)
		{
			if (!module || !func_name || !user_buffer || !utils::dwm_draw::g_get_proc_address_fun)
				return nullptr;

			// 确保偏移固定，避免缓冲区冲突
			PVOID func_name_ptr = reinterpret_cast<PBYTE>(user_buffer) + 0x100;

			// 拷贝函数名
			SIZE_T func_name_len = (strlen(func_name) + 1) * sizeof(char);
			RtlCopyMemory(func_name_ptr, func_name, func_name_len);
			reinterpret_cast<char*>(func_name_ptr)[func_name_len - 1] = '\0';

			// 调用用户态 GetProcAddress
			unsigned long long result_ptr = utils::user_call::call(
				utils::dwm_draw::g_get_proc_address_fun,
				reinterpret_cast<unsigned long long>(module),
				reinterpret_cast<unsigned long long>(func_name_ptr),
				0, 0 );

			if (result_ptr)
			{
				FARPROC func_ptr = reinterpret_cast<FARPROC>(
					*reinterpret_cast<PULONG64>(result_ptr));

				RtlZeroMemory(user_buffer, 0x1000);
				if (func_ptr)
				{
					return func_ptr;
				}
			}

			return nullptr;
		}

	}


}