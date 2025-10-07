#include "global_defs.h"
#include "user_call_utils.h"

namespace utils
{
	namespace user_call
	{

		unsigned long long g_offset_stack{};
		bool g_kvashadow{};
		unsigned long long g_ki_call_user_mode2{};
		unsigned long long g_precall_addr{};
		unsigned long long g_postcall_addr{};

		unsigned long long g_d3dcompile_fun{};
		unsigned long long g_gettickcount64_fun{};
		unsigned long long g_getsystemmetrics_fun{};
		NTSTATUS initialize_user_call_utils()
		{
			NTSTATUS status =STATUS_SUCCESS;
			status = get_stack_offset();
			if (!NT_SUCCESS(status))
			{
				LogError("get_stack_offset failed with status: 0x%X", status);
				return status;
			}
			LogInfo("get_stack_offset SUCCESS");
			status = initialize_ki_call_user_mode2(&g_ki_call_user_mode2);
			if (!NT_SUCCESS(status))
			{
				LogError("get_stack_offset failed with status: 0x%X", status);
				return status;
			}
			LogInfo("initialize_ki_call_user_mode2 SUCCESS");

			status = initialize_ki_call_user_mode2_callbacks();
			if (!NT_SUCCESS(status))
			{
				LogError("initialize_ki_call_user_mode2_callbacks failed with status: 0x%X", status);
				return status;
			}
			LogInfo("initialize_ki_call_user_mode2_callbacks SUCCESS");

			status = initialize_all_user_functions();
			if (!NT_SUCCESS(status))
			{
				LogError("initialize_all_user_functions failed with status: 0x%X", status);
				return status;
			}
			LogInfo("initialize_all_user_functions SUCCESS");

			LogInfo("initialize_user_call_utils SUCCESS");

		
			return STATUS_SUCCESS;
		}

		NTSTATUS initialize_all_user_functions()
		{
			NTSTATUS status = STATUS_SUCCESS;
			
			PEPROCESS process{};
			if (!process_utils::get_process_by_name(L"dwm.exe", &process))
			{
				LogError("Failed to find dwm.exe process");
				return STATUS_NOT_FOUND;
			}

			KAPC_STATE apc_state{};
			
			utils::internal_functions::pfn_ke_stack_attach_process(process, &apc_state);


			unsigned long long addr =scanner_fun::find_module_export_by_name(
				reinterpret_cast<void*>(module_info::d3dcompiler_47_base),
				"D3DCompile"
			);

			if (!addr)
			{
				LogError("Failed to find D3DCompile function");
				goto exit;
			}

			g_d3dcompile_fun = addr;
			LogInfo("D3DCompile function found successfully.0x%llX", addr);

			addr =scanner_fun::find_module_export_by_name(
				reinterpret_cast<void*>(module_info::kernel32_base),
				"GetTickCount64"
			);
			if (!addr)
			{
				LogError("Failed to find GetTickCount64 function");
				goto exit;
			}

			g_gettickcount64_fun = addr;
			LogInfo("GetTickCount64 function found successfully.0x%llX", addr);

			addr =scanner_fun::find_module_export_by_name(
				reinterpret_cast<void*>(module_info::user32_base),
				"GetSystemMetrics"
			);
			if (!addr)
			{
				LogError("Failed to find GetSystemMetrics function");
				goto exit;
			}
			g_getsystemmetrics_fun = addr;
			LogInfo("GetSystemMetrics function found successfully.0x%llX", addr);

	 

			exit:
			utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			utils::internal_functions::pfn_ob_dereference_object(process);


			 
			return status;
		}
  
		NTSTATUS get_stack_offset()
		{
			// Get system call entry address (KiSystemCall64) via MSR
			const unsigned long long syscall_entry_addr = __readmsr(IA32_LSTAR);
			if (!syscall_entry_addr)
			{
				return STATUS_NOT_FOUND;
			}

			// IA32_LSTAR+8 stores stack offset (KVA shadow related)
			const ULONG raw_offset = *(ULONG*)(syscall_entry_addr + 8);

			// Check if lowest byte is 0x10, which is the expected pattern
			if ((raw_offset & 0xFF) != 0x10)
			{
				return STATUS_NOT_FOUND;
			}

			// Mask out low byte, keep only high byte part as offset base value
			const ULONG kva_offset = raw_offset & 0xFF00;

			// Calculate final stack offset
			g_offset_stack = kva_offset + 8;

			// Determine if KVA Shadow (Meltdown patch) is enabled
			g_kvashadow = (kva_offset != 0);

			return STATUS_SUCCESS;
		}
  
		NTSTATUS   initialize_ki_call_user_mode2(OUT unsigned long long* ki_call_user_mode2)
		{
			 char *  shellcode_addr = reinterpret_cast<char*> (utils::memory::allocate_independent_pages(0x1000, PAGE_EXECUTE_READWRITE));
			 if (!shellcode_addr)
			 {
				 return STATUS_INSUFFICIENT_RESOURCES;
			 }
			 /*	 00000000: 90                      nop
					  00000001 : EB 08                   jmp     short 0x0B; Jump to real code start

					  ; Modified code segment (original was 90 90 90 90)
					  00000003: [g_offset_stack] dd ? ; Dynamically written value
					  00000007: [g_kvashadow] dd ? ; Dynamically written value

					  ; === Real code start ===
					  0000000B: 48 C7 C0 E4 0E 00 00    mov     rax, 0EE4h
					  00000012 : 48 35 DC 0F 00 00 xor rax, 0FDCh
					  00000018 : 48 2B E0                sub     rsp, rax
					  0000001B : 48 83 C4 10             add     rsp, 10h
					  0000001F : 74 06                   je      short 00000027h; ZF = 1 jump
					  00000021: 75 04                   jne     short 00000027h; ZF = 0 jump
					  00000023: 90                      nop; Placeholder
					  00000024: 90                      nop
					  00000025 : 90                      nop
					  00000026 : 90                      nop
					  00000027 : 48 83 EC 10             sub     rsp, 10h
					  0000002B : 44 0F 29 54 24 70       movaps[rsp + 70h], xmm10
					  00000031 : 0F 29 7C 24 40          movaps[rsp + 40h], xmm7
					  00000036 : 48 8D 84 24 00 01 00 00 lea     rax, [rsp + 100h]
					  0000003E : 44 0F 29 58 80          movaps[rax - 80h], xmm11
					  00000043 : 44 0F 29 70 B0          movaps[rax - 50h], xmm14
					  00000048 : 0F 29 74 24 30          movaps[rsp + 30h], xmm6
					  0000004D : 44 0F 29 68 A0          movaps[rax - 60h], xmm13
					  00000052 : 44 0F 29 44 24 50       movaps[rsp + 50h], xmm8
					  00000058 : 44 0F 29 78 C0          movaps[rax - 40h], xmm15
					  0000005D : 44 0F 29 4C 24 60       movaps[rsp + 60h], xmm9
					  00000063 : 44 0F 29 60 90          movaps[rax - 70h], xmm12
					  00000068 : 48 89 68 F8             mov[rax - 8], rbp
					  0000006C : 48 8B EC                mov     rbp, rsp
					  0000006F : 48 89 18                mov[rax], rbx
					  00000072 : 48 89 78 08             mov[rax + 8], rdi
					  00000076: 48 89 70 10             mov[rax + 10h], rsi
					  0000007A : 4C 89 68 20             mov[rax + 20h], r13
					  0000007E : 4C 89 78 30             mov[rax + 30h], r15
					  00000082 : 4C 89 60 18             mov[rax + 18h], r12
					  00000086 : 4C 89 70 28             mov[rax + 28h], r14
					  0000008A : 48 89 8D D8 00 00 00    mov[rbp + 0D8h], rcx
					  00000091 : 49 8D 40 D0             lea     rax, [r8 - 30h]
					  00000095 : 48 89 85 E0 00 00 00    mov[rbp + 0E0h], rax
					  0000009C : 68 01 09 00 00          push    901h
					  000000A1: 48 81 34 24 89 08 00 00 xor qword ptr[rsp], 889h
					  000000A9: 58                      pop     rax
					  000000AA : 65 48 8B 18             mov     rbx, gs : [rax] ; GS segment access
					  000000AE: 49 89 60 20             mov[r8 + 20h], rsp
					  000000B2 : 48 8B B3 90 00 00 00    mov     rsi, [rbx + 90h]
					  000000B9 : 48 89 B5 D0 00 00 00    mov[rbp + 0D0h], rsi
					  000000C0 : FA                      cli; Disable interrupts
					  000000C1: 4C 89 43 28             mov[rbx + 28h], r8
					  000000C5 : 4D 8D 48 50             lea     r9, [r8 + 50h]
					  000000C9 : 4C 89 4B 38             mov[rbx + 38h], r9
					  000000CD : E8 00 00 00 00          call    near 000000D2h; Relative call
					  000000D2: 58                      pop     rax; Get return address
					  000000D3: 48 2D CB 00 00 00       sub     rax, 0CBh
					  000000D9 : 83 38 00                cmp     dword ptr[rax], 0; Check memory value
					  000000DC: 74 15                   je      short 000000F3h
					  000000DE : E8 00 00 00 00          call    near 000000E3h
					  000000E3 : 58                      pop     rax
					  000000E4 : 48 2D E0 00 00 00       sub     rax, 0E0h
					  000000EA : 8B 00                   mov     eax, [rax]
					  000000EC : 67 65 4C 89 00          mov     gs : [eax] , r8; GS segment modification
					  000000F1: EB 0D                   jmp     short 00000100h
					  000000F3 : 65 48 8B 3C 25 08 00 00 00 mov   rdi, gs: [8]
					  000000FC : 4C 89 47 04             mov[rdi + 4], r8
					  00000100 : B9 00 60 00 00          mov     ecx, 6000h
					  00000105 : 4C 2B C9                sub     r9, rcx
					  00000108 : 65 4C 89 04 25 A8 01 00 00 mov   gs : [1A8h] , r8; Modify GS segment
					  00000111: 4C 89 4B 30             mov[rbx + 30h], r9
					  00000115 : 49 8D A0 70 FE FF FF    lea     rsp, [r8 - 190h]
					  0000011C : 48 8B FC                mov     rdi, rsp
					  0000011F : B9 32 00 00 00          mov     ecx, 32h
					  00000124 : F3 48 A5                rep movsq; Memory block copy
					  00000127: 48 8D AE F0 FE FF FF    lea     rbp, [rsi - 110h]
					  0000012E : 0F AE 55 AC             ldmxcsr[rbp - 54h]; Load MXCSR
					  00000132: 4C 8B 9D F8 00 00 00    mov     r11, [rbp + 0F8h]
					  00000139 : 48 8B AD D8 00 00 00    mov     rbp, [rbp + 0D8h]
					  00000140 : 48 8B 42 10             mov     rax, [rdx + 10h]
					  00000144 : 48 8B 62 08             mov     rsp, [rdx + 8]
					  00000148: 48 8B 0A                mov     rcx, [rdx]
					  0000014B : F3 0F 10 42 18          movss   xmm0, dword ptr[rdx + 18h]
					  00000150 : F3 0F 10 52 28          movss   xmm2, dword ptr[rdx + 28h]
					  00000155 : F3 0F 10 4A 20          movss   xmm1, dword ptr[rdx + 20h]
					  0000015A : F3 0F 10 5A 30          movss   xmm3, dword ptr[rdx + 30h]
					  0000015F : 48 83 7A 38 00          cmp     qword ptr[rdx + 38h], 0
					  00000164 : 74 04                   je      short 0000016Ah
					  00000166 : 4C 8B 6A 38             mov     r13, [rdx + 38h]
					  0000016A : E8 00 00 00 00          call    near 0000016Fh
					  0000016F : 48 83 04 24 07          add     qword ptr[rsp], 7
					  00000174 : C3                      ret
					  00000175 : 01 0F                   add[rdi], ecx
					  00000177 : 01 F8                   add     eax, edi
					  00000179 : 48 0F 07                sysretq; Key: return to user space
					  0000017C: 59                      pop     rcx
					  0000017D : 66 CF                   iretq; 32-bit interrupt return
					  0000017F: EF                      out     dx, eax*/
			 memcpy(shellcode_addr, "\x90\xEB\x08\x90\x90\x90\x90\x90\x90\x90\x90\x48\xC7\xC0\xE4\x0E\x00\x00\x48\x35\xDC\x0F\x00\x00\x48\x2B\xE0\x48\x83\xC4\x10\x74\x06\x75\x04\x90\x90\x90\x90\x48\x83\xEC\x10\x44\x0F\x29\x54\x24\x70\x0F\x29\x7C\x24\x40\x48\x8D\x84\x24\x00\x01\x00\x00\x44\x0F\x29\x58\x80\x44\x0F\x29\x70\xB0\x0F\x29\x74\x24\x30\x44\x0F\x29\x68\xA0\x44\x0F\x29\x44\x24\x50\x44\x0F\x29\x78\xC0\x44\x0F\x29\x4C\x24\x60\x44\x0F\x29\x60\x90\x48\x89\x68\xF8\x48\x8B\xEC\x48\x89\x18\x48\x89\x78\x08\x48\x89\x70\x10\x4C\x89\x68\x20\x4C\x89\x78\x30\x4C\x89\x60\x18\x4C\x89\x70\x28\x48\x89\x8D\xD8\x00\x00\x00\x49\x8D\x40\xD0\x48\x89\x85\xE0\x00\x00\x00\x68\x01\x09\x00\x00\x48\x81\x34\x24\x89\x08\x00\x00\x58\x65\x48\x8B\x18\x49\x89\x60\x20\x48\x8B\xB3\x90\x00\x00\x00\x48\x89\xB5\xD0\x00\x00\x00\xFA\x4C\x89\x43\x28\x4D\x8D\x48\x50\x4C\x89\x4B\x38\xE8\x00\x00\x00\x00\x58\x48\x2D\xCB\x00\x00\x00\x83\x38\x00\x74\x15\xE8\x00\x00\x00\x00\x58\x48\x2D\xE0\x00\x00\x00\x8B\x00\x67\x65\x4C\x89\x00\xEB\x0D\x65\x48\x8B\x3C\x25\x08\x00\x00\x00\x4C\x89\x47\x04\xB9\x00\x60\x00\x00\x4C\x2B\xC9\x65\x4C\x89\x04\x25\xA8\x01\x00\x00\x4C\x89\x4B\x30\x49\x8D\xA0\x70\xFE\xFF\xFF\x48\x8B\xFC\xB9\x32\x00\x00\x00\xF3\x48\xA5\x48\x8D\xAE\xF0\xFE\xFF\xFF\x0F\xAE\x55\xAC\x4C\x8B\x9D\xF8\x00\x00\x00\x48\x8B\xAD\xD8\x00\x00\x00\x48\x8B\x42\x10\x48\x8B\x62\x08\x48\x8B\x0A\xF3\x0F\x10\x42\x18\xF3\x0F\x10\x52\x28\xF3\x0F\x10\x4A\x20\xF3\x0F\x10\x5A\x30\x48\x83\x7A\x38\x00\x74\x04\x4C\x8B\x6A\x38\xE8\x00\x00\x00\x00\x48\x83\x04\x24\x07\xC3\x01\x0F\x01\xF8\x48\x0F\x07\x59\x66\xCF\xEF", 385);

			 *(unsigned long *)(shellcode_addr + 3) = g_offset_stack;
			 *(unsigned long*)(shellcode_addr + 7) = g_kvashadow;
		 
			 *ki_call_user_mode2 = reinterpret_cast<unsigned  long long> (shellcode_addr);
			 return STATUS_SUCCESS;
		 
		}


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

		unsigned long long call4(unsigned long long func_ptr, unsigned long long arg1, unsigned long long arg2, unsigned long long arg3, unsigned long long arg4)
		{
 

			void* cdata[9]{};
			auto new_user_rsp = thread_utils::get_user_stack_ptr() - 0x98 & 0xFFFFFFFFFFFFFFF0;

			*(unsigned long long*)(new_user_rsp + 0x20 + (0 * 8)) = arg1;
			*(unsigned long long*)(new_user_rsp + 0x20 + (1 * 8)) = arg2;
			*(unsigned long long*)(new_user_rsp + 0x20 + (2 * 8)) = arg3;
			*(unsigned long long*)(new_user_rsp + 0x20 + (3 * 8)) = arg4;

			unsigned long long  ret = call2(func_ptr, reinterpret_cast<char*> (new_user_rsp), cdata);

 
			return ret;
		}

		unsigned long long call5(
			unsigned long long func_ptr,
			unsigned long long arg1,
			unsigned long long arg2,
			unsigned long long arg3,
			unsigned long long arg4,
			unsigned long long arg5
		)
		{
 


			void* cdata[9]{};
			auto new_user_rsp = thread_utils::get_user_stack_ptr() - 0x98 & 0xFFFFFFFFFFFFFFF0;

			*(unsigned long long*)(new_user_rsp + 0x20 + (0 * 8)) = arg1;
			*(unsigned long long*)(new_user_rsp + 0x20 + (1 * 8)) = arg2;
			*(unsigned long long*)(new_user_rsp + 0x20 + (2 * 8)) = arg3;
			*(unsigned long long*)(new_user_rsp + 0x20 + (3 * 8)) = arg4;
			*(unsigned long long*)(new_user_rsp + 0x70 + (0 * 8)) = arg5;
			 

			unsigned long long  ret = call2(func_ptr, reinterpret_cast<char*> (new_user_rsp), cdata);

		 
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


		unsigned long long call7(
			unsigned long long func_ptr,
			unsigned long long arg1,
			unsigned long long arg2,
			unsigned long long arg3,
			unsigned long long arg4,
			unsigned long long arg5,
			unsigned long long arg6,
		unsigned long long arg7)
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
			*(unsigned long long*)(new_user_rsp + 0x70 + (2 * 8)) = arg7;

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

		 
			*(unsigned long long*)(new_user_rsp + 0x20 + (0 * 8)) = arg1;
			*(unsigned long long*)(new_user_rsp + 0x20 + (1 * 8)) = arg2;
			*(unsigned long long*)(new_user_rsp + 0x20 + (2 * 8)) = arg3;
			*(unsigned long long*)(new_user_rsp + 0x20 + (3 * 8)) = arg4;

			 
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
			 
			   
			   info.StackFlags =5;  
			   info.StackType = 0;      

			 
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
			*(unsigned long long*)(user_rsp + 0x48) = g_postcall_addr;
			call_data[2] = reinterpret_cast<void *> (ptr);
			call_data[1] = user_rsp;
			call_data[0] = reinterpret_cast<void*> (g_precall_addr);


			//get apc state
			auto apcIndex = *(unsigned char*)(current_thread + 0x24A);
			auto v1 = *(unsigned short*)(current_thread + 0x1E4);
			auto v2 = *(unsigned short*)(current_thread + 0x1E6);

			//fix apc state
			*(unsigned short*)(current_thread + 0x1E4) = 0;
			*(unsigned short*)(current_thread + 0x1E6) = 0;
			*(unsigned char*)(current_thread + 0x24A) = 0;

			typedef VOID(*FnKiCallUserMode2)(unsigned long long* OutVarPtr, unsigned long long CallCtx, unsigned long long KStackControl);
			auto pFnKiCallUserMode2 = (FnKiCallUserMode2) g_ki_call_user_mode2;

		 
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
			/*if (!dll_name || !user_buffer || !utils::dwm_draw::g_load_librarya_fun)
				return nullptr;

			 
			PVOID dll_name_ptr = reinterpret_cast<PBYTE>(user_buffer) + 0x100;

		 
			SIZE_T dll_name_len = (strlen(dll_name) + 1) * sizeof(char);
			RtlCopyMemory(dll_name_ptr, dll_name, dll_name_len);
			reinterpret_cast<char*>(dll_name_ptr)[dll_name_len - 1] = '\0';

			 
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
			}*/

			return nullptr;
		}
		

		FARPROC get_proc_address(
			HMODULE module,
			const char* func_name,
			PVOID user_buffer)
		{
			/*if (!module || !func_name || !user_buffer || !utils::dwm_draw::g_get_proc_address_fun)
				return nullptr;

			 
			PVOID func_name_ptr = reinterpret_cast<PBYTE>(user_buffer) + 0x100;

			 
			SIZE_T func_name_len = (strlen(func_name) + 1) * sizeof(char);
			RtlCopyMemory(func_name_ptr, func_name, func_name_len);
			reinterpret_cast<char*>(func_name_ptr)[func_name_len - 1] = '\0';

		 
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
			}*/

			return nullptr;
		}

		NTSTATUS initialize_ki_call_user_mode2_callbacks()
		{
			PEPROCESS process{};
			NTSTATUS status{};
			
			if (!process_utils::get_process_by_name(L"dwm.exe", &process))
			{
				LogError("Failed to find dwm.exe process");
				return STATUS_NOT_FOUND;
			}

			status = find_precall_address(process, reinterpret_cast<ULONG_PTR>(utils::module_info::ntdll_base), &g_precall_addr);
			if (!NT_SUCCESS(status))
			{
				LogError("find_precall_address failed with status: 0x%X", status);
				internal_functions::pfn_ob_dereference_object(process);
				return status;
			}

			LogInfo("find_precall_address SUCCESS");
			
			status = find_postcall_address(process, reinterpret_cast<ULONG_PTR>(utils::module_info::user32_base), &g_postcall_addr);
			if (!NT_SUCCESS(status))
			{
				LogError("find_postcall_address failed with status: 0x%X", status);
				internal_functions::pfn_ob_dereference_object(process);
				return status;
			}
			
			LogInfo("find_postcall_address SUCCESS");

			internal_functions::pfn_ob_dereference_object(process);
			LogInfo("initialize_ki_call_user_mode2_callbacks SUCCESS");
			
			return STATUS_SUCCESS;
		}

		NTSTATUS find_precall_address(
			IN PEPROCESS process,
			IN ULONG_PTR ntdll_base,
			OUT ULONG_PTR* precall_addr_out)
		{
			if (!process || !ntdll_base || !precall_addr_out)
				return STATUS_INVALID_PARAMETER;

			*precall_addr_out = 0;

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			ULONG_PTR match_addr = signature_scanner::find_pattern_image(
				ntdll_base,
				"\x48\x8B\x4C\x24\xCC\x48\x8B\x54\x24\xCC\x4C",
				"xxxx?xxxx?x");

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (!match_addr)
			{
				LogError(
					"Failed to locate PreCall pattern in ntdll.dll.\n");
				return STATUS_NOT_FOUND;
			}

			*precall_addr_out = match_addr;

			LogInfo(
				"PreCall found at: 0x%llX\n", *precall_addr_out);

			return STATUS_SUCCESS;
		}

		NTSTATUS find_postcall_address(
			IN PEPROCESS process,
			IN ULONG_PTR user32_base,
			OUT ULONG_PTR* postcall_addr_out)
		{
			if (!process || !user32_base || !postcall_addr_out)
				return STATUS_INVALID_PARAMETER;

			*postcall_addr_out = 0;

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			ULONG_PTR match_addr = signature_scanner::find_pattern_image(
				user32_base,
				"\x45\x33\xC0\x48\x89\x44\x24\x20\x48\x8D\x4C\x24\x20", 
				"xxxxxxxxxxxxx");

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (!match_addr)
			{
				LogError(
					"Failed to locate PostCall pattern in user32.dll.\n");
				return STATUS_NOT_FOUND;
			}

			*postcall_addr_out = match_addr;

			LogInfo(
				"PostCall found at: 0x%llX\n", *postcall_addr_out);

			return STATUS_SUCCESS;
		}

	}


}