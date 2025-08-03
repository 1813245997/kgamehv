#include "global_defs.h"
#include "dwm_draw.h"
#include "hook_utils.h"
#include "../ia32/ia32.hpp"

namespace utils
{
	namespace dwm_draw
	{
	
		PEPROCESS g_dwm_process{};
		PETHREAD g_dwm_render_thread{};
		unsigned long long g_precall_addr{};
		unsigned long long g_postcall_addr{};
		unsigned long long g_context_offset{};
		unsigned long long g_gdi32_base{};
		unsigned long long g_gdi32_size{};
		unsigned long long g_ntdll_base{};
		unsigned long long g_ntdll_size{};
		unsigned long long g_user32_base{};
		unsigned long long g_user32_size{};
		unsigned long long g_dwmcore_base{};
		unsigned long long g_dwmcore_size{};
		unsigned long long g_dxgi_base{};
		unsigned long long g_dxgi_size{};
		unsigned long long g_dxgkrnl_base{};
		unsigned long long g_dxgkrnl_size{};
		unsigned long long g_offset_stack{};
		unsigned long long g_ccomposition_present{};
		unsigned long long g_cocclusion_context_pre_sub_graph{};
		unsigned long long g_cocclusion_context_post_sub_graph{};
		unsigned long long g_cdxgi_swap_chain_dwm_legacy_present_dwm{}; //CDXGISwapChain::PresentImplCore
		unsigned long long g_cdxgi_swapchain_present_dwm{}; //CDXGISwapChain::PresentDWM
		unsigned long long g_cdxgi_swapchain_present_multiplane_overlay{}; //CDXGISwapChain::PresentMultiplaneOverlay
	    unsigned long long g_dxgk_open_resource{};

		unsigned long long g_pswap_chain{};

		unsigned long long g_dxgk_get_device_state{};

		unsigned long long g_ki_call_user_mode2{};

		bool g_kvashadow{};

		



		//CDXGISwapChain::PresentImplCore


		//要用到的一部分窗口函数
		unsigned long long g_client_to_screen_fun{};
		unsigned long long g_get_client_rect_fun{};
		unsigned long long g_get_foreground_window_fun{};
		unsigned long long g_find_windoww_fun{};
		unsigned long long g_get_window_rect_fun{};

		NTSTATUS initialize()
		{
			NTSTATUS status{};
			
		
			
			status = get_stack_offset();
			if (!NT_SUCCESS(status))
			{
				LogError("get_stack_offset failed with status: 0x%X", status);
				return status;
			}
			status = initialize_ki_call_user_mode2(&g_ki_call_user_mode2);
			if (!NT_SUCCESS(status))
			{
				LogError("initialize_ki_call_user_mode2 failed with status: 0x%X", status);
				return status;
			}

			status = get_dwm_process(&g_dwm_process);
			if (!NT_SUCCESS(status))
			{
				LogError("get_dwm_process failed with status: 0x%X", status);
				return status;
			}

			status = initialize_dwm_utils_modules(g_dwm_process);
			if (!NT_SUCCESS(status))
			{
				LogError("initialize_dwm_utils_modules failed with status: 0x%X", status);
				return status;
			}
			
		

			status = find_precall_address(g_dwm_process,g_ntdll_base,&g_precall_addr);
			if (!NT_SUCCESS(status))
			{
				LogError("find_precall_address failed with status: 0x%X", status);
				return status;
			}

			status = find_postcall_address(g_dwm_process, g_user32_base, &g_postcall_addr);
			if (!NT_SUCCESS(status))
			{
				LogError("find_postcall_address failed with status: 0x%X", status);
				return status;
			}


			status = find_ccomposition_present(g_dwm_process, g_dwmcore_base, &g_ccomposition_present);
			if (!NT_SUCCESS(status))
			{
				LogError("find_ccomposition_present failed with status: 0x%X", status);
				return status;
			}

			status = find_cocclusion_context_pre_sub_graph(g_dwm_process, g_dwmcore_base, &g_cocclusion_context_pre_sub_graph);
			if (!NT_SUCCESS(status))
			{
				LogError("find_cocclusion_context_pre_sub_graph failed with status: 0x%X", status);
				return status;
			}

			status = find_cocclusion_context_post_sub_graph(g_dwm_process, g_dwmcore_base, &g_cocclusion_context_post_sub_graph);
			if (!NT_SUCCESS(status))
			{
				LogError("find_cocclusion_context_post_sub_graph failed with status: 0x%X", status);
				return status;
			}

			status = find_cdxgi_swapchain_present_dwm(g_dwm_process, g_dxgi_base, &g_cdxgi_swapchain_present_dwm);
			if (!NT_SUCCESS(status))
			{
				LogError("find_cdxgi_swapchain_present_dwm failed with status: 0x%X", status);
				return status;
			}

			status = find_cdxgi_swapchain_present_multiplane_overlay(g_dwm_process, g_dxgi_base, &g_cdxgi_swapchain_present_multiplane_overlay);
			if (!NT_SUCCESS(status))
			{
				LogError("find_cdxgi_swapchain_present_multiplane_overlay failed with status: 0x%X", status);
				return status;
			}
			
			status = find_cdxgi_swapchain_dwm_legacy_present_dwm(g_dwm_process, g_dxgi_base, &g_cdxgi_swap_chain_dwm_legacy_present_dwm);
			if (!NT_SUCCESS(status))
			{
				LogError("find_cdxgi_swapchain_dwm_legacy_present_dwm failed with status: 0x%X", status);
				return status;
			}

			status = find_find_windoww(g_dwm_process, g_user32_base, &g_find_windoww_fun);
			if (!NT_SUCCESS(status))
			{
				LogError("find_find_windoww failed with status: 0x%X", status);
				return status;
			}
			
			status = find_client_to_screen(g_dwm_process, g_user32_base, &g_client_to_screen_fun);
			if (!NT_SUCCESS(status))
			{
				LogError("find_client_to_screen failed with status: 0x%X", status);
				return status;
			}
			
			status = find_get_client_rect(g_dwm_process, g_user32_base, &g_get_client_rect_fun);
			if (!NT_SUCCESS(status))
			{
				LogError("find_get_client_rect failed with status: 0x%X", status);
				return status;
			}

			status = find_get_foreground_window(g_dwm_process, g_user32_base, &g_get_foreground_window_fun);
			if (!NT_SUCCESS(status))
			{
				LogError("find_get_foreground_window failed with status: 0x%X", status);
				return status;
			}

			status = find_get_window_rect(g_dwm_process, g_user32_base, &g_get_window_rect_fun);
			if (!NT_SUCCESS(status))
			{
				LogError("find_get_window_rect failed with status: 0x%X", status);
				return status;
			}
			 

			status = hook_swapchain_present_dwm(g_dwm_process);
			if (!NT_SUCCESS(status))
			{
				LogError("hook_swapchain_present_dwm failed with status: 0x%X", status);
				return status;
			}
			//物理机走这个
			status = hook_present_multiplane_overlay(g_dwm_process);
			if (!NT_SUCCESS(status))
			{
				LogError("hook_present_multiplane_overlay failed with status: 0x%X", status);
				return status;
			}

			//虚拟机走这个
			status = hook_cdxgi_swapchain_dwm_legacy_present_dwm(g_dwm_process);
			if (!NT_SUCCESS(status))
			{
				LogError("hook_cdxgi_swapchain_dwm_legacy_present_dwm failed with status: 0x%X", status);
				return status;
			}


			status = hook_cocclusion_context_post_sub_graph(g_dwm_process);
			if (!NT_SUCCESS(status))
			{
				LogError("hook_cocclusion_context_post_sub_graph failed with status: 0x%X", status);
				return status;
			 
			}

			status = hook_cocclusion_context_pre_sub_graph(g_dwm_process);
			if (!NT_SUCCESS(status))
			{
				LogError("hook_cocclusion_context_pre_sub_graph failed with status: 0x%X", status);
				return status;
			}


		 
			 

			return STATUS_SUCCESS;
		}

		NTSTATUS get_dwm_process(_Out_ PEPROCESS * out_process)
		{
			PEPROCESS process{};

			if (!out_process)
			{
				return STATUS_INVALID_PARAMETER;
			}

			if (!process_utils::get_process_by_name(L"dwm.exe", &process))
			{
				return STATUS_NOT_FOUND;
			}
			internal_functions::pfn_ob_dereference_object(process);
			*out_process = process;
			return STATUS_SUCCESS;
		}

		NTSTATUS initialize_dwm_utils_modules(_In_ PEPROCESS process)
		{
			NTSTATUS status{};
			unsigned long long ntdll_base{};
			SIZE_T ntdll_size{};
			unsigned long long user32_base{};
			SIZE_T user32_size{};
			unsigned long long dwmcore_base{};
			SIZE_T dwmcore_size{};
			unsigned long long dxgi_base{};
			SIZE_T dxgi_size{};
			unsigned long long  dxgkrnl_base{};
			SIZE_T dxgkrnl_size{};
			unsigned long long gdi32_base{};
			SIZE_T gdi32_size{};
			

			// ntdll.dll
			status = module_info::get_process_module_info(process, L"ntdll.dll", &ntdll_base, &ntdll_size);
			if (!NT_SUCCESS(status)) {
				LogDebug(
					"Failed to get ntdll.dll module info (0x%X)\n", status);
				return status;
			}

			// user32.dll
			status = module_info::get_process_module_info(process, L"user32.dll", &user32_base, &user32_size);
			if (!NT_SUCCESS(status)) {
				LogDebug(
					"Failed to get user32.dll module info (0x%X)\n", status);
				return status;
			}

			// dwmcore.dll
			status = module_info::get_process_module_info(process, L"dwmcore.dll", &dwmcore_base, &dwmcore_size);
			if (!NT_SUCCESS(status)) {
				LogDebug( 
					"Failed to get dwmcore.dll module info (0x%X)\n", status);
				return status;
			}
			status = module_info::get_process_module_info(process, L"dxgi.dll", &dxgi_base, &dxgi_size);
			if (!NT_SUCCESS(status)) {
				LogDebug(
					"Failed to get dxgi.dll module info (0x%X)\n", status);
				return status;
			}

			if (!module_info::get_driver_module_info("dxgkrnl.sys", dxgkrnl_size, (PVOID&)dxgkrnl_base)) {
				LogDebug(
					"Failed to get dxgkrnl.sys module info (0x%X)\n", status);
				return STATUS_INVALID_PARAMETER;
			}

			status = module_info::get_process_module_info(process, L"gdi32.dll", &gdi32_base, &gdi32_size);
			if (!NT_SUCCESS(status)) {
				LogDebug( 
					"Failed to get gdi32.dll module info (0x%X)\n", status);
				return status;
			}

			 
			g_ntdll_base = ntdll_base;
			g_ntdll_size = ntdll_size;
			g_user32_base = user32_base;
			g_user32_size = user32_size;
			g_dwmcore_base = dwmcore_base;
			g_dwmcore_size = dwmcore_size;
			g_dxgi_base = dxgi_base;
			g_dxgi_size = dxgi_size;
			g_gdi32_base = gdi32_base;
			g_gdi32_size = gdi32_size;
			g_dxgkrnl_base = dxgkrnl_base;
			g_dxgkrnl_size = dxgkrnl_size;


			

			LogDebug(
				"ntdll.dll  -> Base: 0x%llX, Size: 0x%llX\n", g_ntdll_base, g_ntdll_size);
			LogDebug( 
				"user32.dll -> Base: 0x%llX, Size: 0x%llX\n", g_user32_base, g_user32_size);
			LogDebug( 
				"dwmcore.dll -> Base: 0x%llX, Size: 0x%llX\n", g_dwmcore_base, g_dwmcore_size);
			LogDebug(  
				"dxgi.dll    -> Base: 0x%llX, Size: 0x%llX\n", g_dxgi_base, g_dxgi_size);
			LogDebug( 
				"gdi32.dll   -> Base: 0x%llX, Size: 0x%llX\n", g_gdi32_base, g_gdi32_size);
			LogDebug( 
				"dxgkrnl.sys    -> Base: 0x%llX, Size: 0x%llX\n", g_dxgkrnl_base, g_dxgkrnl_size);
	

			return STATUS_SUCCESS;
		}


		NTSTATUS get_stack_offset()
		{
			// 获取系统调用入口地址（KiSystemCall64）通过 MSR
			const unsigned long long syscall_entry_addr = __readmsr(IA32_LSTAR);
			if (!syscall_entry_addr)
			{
				return STATUS_NOT_FOUND;
			}

			// IA32_LSTAR+8 存的是栈偏移（KVA shadow 相关）
			const ULONG raw_offset = *(ULONG*)(syscall_entry_addr + 8);

			// 检查最低字节是否为 0x10，这是预期模式
			if ((raw_offset & 0xFF) != 0x10)
			{
				return STATUS_NOT_FOUND;
			}

			// 屏蔽掉低字节，只保留高字节部分作为偏移基础值
			const ULONG kva_offset = raw_offset & 0xFF00;

			// 计算最终栈偏移
			g_offset_stack = kva_offset + 8;

			// 判断是否启用了 KVA Shadow（Meltdown 补丁）
			g_kvashadow = (kva_offset != 0);

			return STATUS_SUCCESS;
		}

		NTSTATUS   initialize_ki_call_user_mode2(OUT unsigned long long* ki_call_user_mode2)
		{
			 char *  shellcode_addr = reinterpret_cast<char*> (internal_functions::pfn_mm_allocate_independent_pages(0x1000, 0));
			 if (!shellcode_addr)
			 {
				 return STATUS_INSUFFICIENT_RESOURCES;
			 }
			 /*	 00000000: 90                      nop
					  00000001 : EB 08                   jmp     short 0x0B; 跳转到真实代码开始处

					  ; 被修改的代码段（原始为 90 90 90 90）
					  00000003: [g_offset_stack] dd ? ; 动态写入值
					  00000007: [g_kvashadow] dd ? ; 动态写入值

					  ; == = 真实代码开始 == =
					  0000000B: 48 C7 C0 E4 0E 00 00    mov     rax, 0EE4h
					  00000012 : 48 35 DC 0F 00 00 xor rax, 0FDCh
					  00000018 : 48 2B E0                sub     rsp, rax
					  0000001B : 48 83 C4 10             add     rsp, 10h
					  0000001F : 74 06                   je      short 00000027h; ZF = 1 跳转
					  00000021: 75 04                   jne     short 00000027h; ZF = 0 跳转
					  00000023: 90                      nop; 占位符
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
					  000000AA : 65 48 8B 18             mov     rbx, gs : [rax] ; GS 段访问
					  000000AE: 49 89 60 20             mov[r8 + 20h], rsp
					  000000B2 : 48 8B B3 90 00 00 00    mov     rsi, [rbx + 90h]
					  000000B9 : 48 89 B5 D0 00 00 00    mov[rbp + 0D0h], rsi
					  000000C0 : FA                      cli; 禁用中断
					  000000C1: 4C 89 43 28             mov[rbx + 28h], r8
					  000000C5 : 4D 8D 48 50             lea     r9, [r8 + 50h]
					  000000C9 : 4C 89 4B 38             mov[rbx + 38h], r9
					  000000CD : E8 00 00 00 00          call    near 000000D2h; 相对调用
					  000000D2: 58                      pop     rax; 获取返回地址
					  000000D3: 48 2D CB 00 00 00       sub     rax, 0CBh
					  000000D9 : 83 38 00                cmp     dword ptr[rax], 0; 检查内存值
					  000000DC: 74 15                   je      short 000000F3h
					  000000DE : E8 00 00 00 00          call    near 000000E3h
					  000000E3 : 58                      pop     rax
					  000000E4 : 48 2D E0 00 00 00       sub     rax, 0E0h
					  000000EA : 8B 00                   mov     eax, [rax]
					  000000EC : 67 65 4C 89 00          mov     gs : [eax] , r8; GS 段修改
					  000000F1: EB 0D                   jmp     short 00000100h
					  000000F3 : 65 48 8B 3C 25 08 00 00 00 mov   rdi, gs: [8]
					  000000FC : 4C 89 47 04             mov[rdi + 4], r8
					  00000100 : B9 00 60 00 00          mov     ecx, 6000h
					  00000105 : 4C 2B C9                sub     r9, rcx
					  00000108 : 65 4C 89 04 25 A8 01 00 00 mov   gs : [1A8h] , r8; 修改 GS 段
					  00000111: 4C 89 4B 30             mov[rbx + 30h], r9
					  00000115 : 49 8D A0 70 FE FF FF    lea     rsp, [r8 - 190h]
					  0000011C : 48 8B FC                mov     rdi, rsp
					  0000011F : B9 32 00 00 00          mov     ecx, 32h
					  00000124 : F3 48 A5                rep movsq; 内存块复制
					  00000127: 48 8D AE F0 FE FF FF    lea     rbp, [rsi - 110h]
					  0000012E : 0F AE 55 AC             ldmxcsr[rbp - 54h]; 加载 MXCSR
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
					  00000179 : 48 0F 07                sysretq; 关键：返回用户空间
					  0000017C: 59                      pop     rcx
					  0000017D : 66 CF                   iretq; 32位中断返回
					  0000017F: EF                      out     dx, eax*/
			 memcpy(shellcode_addr, "\x90\xEB\x08\x90\x90\x90\x90\x90\x90\x90\x90\x48\xC7\xC0\xE4\x0E\x00\x00\x48\x35\xDC\x0F\x00\x00\x48\x2B\xE0\x48\x83\xC4\x10\x74\x06\x75\x04\x90\x90\x90\x90\x48\x83\xEC\x10\x44\x0F\x29\x54\x24\x70\x0F\x29\x7C\x24\x40\x48\x8D\x84\x24\x00\x01\x00\x00\x44\x0F\x29\x58\x80\x44\x0F\x29\x70\xB0\x0F\x29\x74\x24\x30\x44\x0F\x29\x68\xA0\x44\x0F\x29\x44\x24\x50\x44\x0F\x29\x78\xC0\x44\x0F\x29\x4C\x24\x60\x44\x0F\x29\x60\x90\x48\x89\x68\xF8\x48\x8B\xEC\x48\x89\x18\x48\x89\x78\x08\x48\x89\x70\x10\x4C\x89\x68\x20\x4C\x89\x78\x30\x4C\x89\x60\x18\x4C\x89\x70\x28\x48\x89\x8D\xD8\x00\x00\x00\x49\x8D\x40\xD0\x48\x89\x85\xE0\x00\x00\x00\x68\x01\x09\x00\x00\x48\x81\x34\x24\x89\x08\x00\x00\x58\x65\x48\x8B\x18\x49\x89\x60\x20\x48\x8B\xB3\x90\x00\x00\x00\x48\x89\xB5\xD0\x00\x00\x00\xFA\x4C\x89\x43\x28\x4D\x8D\x48\x50\x4C\x89\x4B\x38\xE8\x00\x00\x00\x00\x58\x48\x2D\xCB\x00\x00\x00\x83\x38\x00\x74\x15\xE8\x00\x00\x00\x00\x58\x48\x2D\xE0\x00\x00\x00\x8B\x00\x67\x65\x4C\x89\x00\xEB\x0D\x65\x48\x8B\x3C\x25\x08\x00\x00\x00\x4C\x89\x47\x04\xB9\x00\x60\x00\x00\x4C\x2B\xC9\x65\x4C\x89\x04\x25\xA8\x01\x00\x00\x4C\x89\x4B\x30\x49\x8D\xA0\x70\xFE\xFF\xFF\x48\x8B\xFC\xB9\x32\x00\x00\x00\xF3\x48\xA5\x48\x8D\xAE\xF0\xFE\xFF\xFF\x0F\xAE\x55\xAC\x4C\x8B\x9D\xF8\x00\x00\x00\x48\x8B\xAD\xD8\x00\x00\x00\x48\x8B\x42\x10\x48\x8B\x62\x08\x48\x8B\x0A\xF3\x0F\x10\x42\x18\xF3\x0F\x10\x52\x28\xF3\x0F\x10\x4A\x20\xF3\x0F\x10\x5A\x30\x48\x83\x7A\x38\x00\x74\x04\x4C\x8B\x6A\x38\xE8\x00\x00\x00\x00\x48\x83\x04\x24\x07\xC3\x01\x0F\x01\xF8\x48\x0F\x07\x59\x66\xCF\xEF", 385);

			 *(unsigned long *)(shellcode_addr + 3) = g_offset_stack;
			 *(unsigned long*)(shellcode_addr + 7) = g_kvashadow;
			 memory::set_execute_page(reinterpret_cast<unsigned long long> (shellcode_addr), 0x1000);
			 utils::internal_functions::pfn_mm_set_page_protection(shellcode_addr, 0x1000, PAGE_EXECUTE_READWRITE);

		 

			 if (!memory::is_executable_address(reinterpret_cast<unsigned  long long> (shellcode_addr)))
			 {
				 memory::set_execute_page(reinterpret_cast<unsigned long long> (shellcode_addr), 0x1000);
			 }
			 *ki_call_user_mode2 = reinterpret_cast<unsigned  long long> (shellcode_addr);
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

			LogDebug(
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

			LogDebug(
				"PostCall found at: 0x%llX\n", *postcall_addr_out);

			return STATUS_SUCCESS;
		}


		 
	
		NTSTATUS find_context_offset(IN PEPROCESS process, 
			IN unsigned long long ntdll_base,
			OUT unsigned long long* context_offset_out)
		{
			KAPC_STATE apc_state{};
		 
			ULONG_PTR match_addr = 0;
			*context_offset_out = 0;
			// 特征码1
			const char* pattern1 =
				"\x48\x8D\x0D\xCC\xCC\xCC\xCC\xFF\x15\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x89\xCC\xCC\xCC\x45\x33\xC0";
			const char* mask1 =
				"xxx????xx????xxx????xx???xxx";

			// 特征码2
			const char* pattern2 =
				"\x48\x8D\x0D\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x89\xCC\xCC\xCC\x45\x33\xC0";
			const char* mask2 =
				"xxx????x????xxx????xx???xxx";

			// 特征码3
			const char* pattern3 =
				"\x48\x8D\x0D\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC"
				"\x48\x8B\x44\x24\xCC\x48\x89\x05";
			const char* mask3 =
				"xxx????x????xxxx?xxx";

			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			match_addr = signature_scanner::find_pattern_image(
				ntdll_base, pattern1, mask1, ".text");

			if (!match_addr)
			{
				match_addr = signature_scanner::find_pattern_image(
					ntdll_base, pattern2, mask2, ".text");
			}

			if (!match_addr)
			{
				match_addr = signature_scanner::find_pattern_image(
					ntdll_base, pattern3, mask3, ".text");
			}

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (!match_addr)
			{
				LogError(
					"Failed to locate context pattern (all signatures failed).\n");
				return STATUS_NOT_FOUND;
			}

			// 解算相对地址：lea rcx, [rip+rel32]

			ULONG_PTR  abs_addr = signature_scanner::resolve_relative_address(
				reinterpret_cast<PVOID>(match_addr), 3, 7);


			*context_offset_out = abs_addr - ntdll_base;

			LogDebug(
				"Found context offset = 0x%llX\n", *context_offset_out);


		 

			return STATUS_SUCCESS;
		}



		NTSTATUS find_ccomposition_present(
			IN PEPROCESS process ,
			IN unsigned long long dwmcore_base,
			OUT unsigned long long* ccomposition_present_addr_out)
		{
			 
			KAPC_STATE apc_state{};
			unsigned long long temp_addr = 0;
			unsigned long long resolved_addr = 0;

			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);
			temp_addr = signature_scanner::find_pattern_image(
				dwmcore_base,
				"\x89\xCC\x24\xB8\x01\x00\x00\xE8\xCC\xCC\xCC\xCC\x85\xC0",
				"x?xxxxxx????xx", ".text");

			if (temp_addr)
			{
				temp_addr += 7; // 指向CALL的E8偏移
				resolved_addr = signature_scanner::resolve_relative_address(
					reinterpret_cast<PVOID>(temp_addr), 1, 5);
			}
			else
			{
				// 特征码2: 通常是 LEA + CALL，通过函数头部回溯定位
				temp_addr = signature_scanner::find_pattern_image(
					dwmcore_base,
					"\x49\x8D\x4B\xD8\x48\xFF\x15",
					"xxxxxxx", ".text");

				if (temp_addr)
				{
					resolved_addr = signature_scanner::find_function_begin(
						dwmcore_base, temp_addr);
				}
				else
				{
					// 特征码3: 如失败，尝试使用 fallback 的 MOV + CALL 模式
					temp_addr = signature_scanner::find_pattern_image(
						dwmcore_base,
						"\x40\xB7\x01\xFF\x15",
						"xxxxx", ".text");

					if (temp_addr)
					{
						resolved_addr = signature_scanner::find_function_begin(
							dwmcore_base, temp_addr);
					}
				}
			}
			internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			if (resolved_addr == 0)
			{
				LogError( 
					"Failed to locate CComposition::Present function.\n");
				return STATUS_NOT_FOUND;
			}

			LogDebug(
				"Found CComposition::Present at 0x%llX\n", resolved_addr);
			*ccomposition_present_addr_out = resolved_addr;
			 

			return STATUS_SUCCESS;
		}
		NTSTATUS find_cocclusion_context_pre_sub_graph(
			IN PEPROCESS process,
			IN unsigned long long dwmcore_base,
			OUT unsigned long long* cocclusion_context_pre_sub_graph_addr_out)
		{   
			if (!cocclusion_context_pre_sub_graph_addr_out)
				return STATUS_INVALID_PARAMETER;

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_cocclusion_context_pre_sub_graph(dwmcore_base);

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*cocclusion_context_pre_sub_graph_addr_out = addr;

			LogDebug(
				"Found COcclusionContext::PreSubgraph at 0x%llX\n", addr);

			return STATUS_SUCCESS;
			   
		}

		NTSTATUS find_cocclusion_context_post_sub_graph(
			IN PEPROCESS process,
			IN unsigned long long dwmcore_base,
			OUT unsigned long long* cocclusion_context_post_sub_graph_addr_out)
		{
			if (!cocclusion_context_post_sub_graph_addr_out)
				return STATUS_INVALID_PARAMETER;

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_cocclusion_context_post_sub_graph(  dwmcore_base );

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*cocclusion_context_post_sub_graph_addr_out = addr;

			LogDebug( 
				"Found COcclusionContext::PostSubgraph at 0x%llX\n", addr);
		 
			return STATUS_SUCCESS;
		}


		NTSTATUS find_cdxgi_swapchain_present_dwm(
			IN PEPROCESS process,
			IN unsigned long long dxgi_base,
			OUT unsigned long long* cdxgi_swapchain_present_dwm_out)
		{
			if (!cdxgi_swapchain_present_dwm_out)
				return STATUS_INVALID_PARAMETER;

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_cdxgi_swapchain_present_dwm(dxgi_base);

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*cdxgi_swapchain_present_dwm_out = addr;

			LogDebug( 
				"Found CDXGISwapChain::PresentDWM at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}
		NTSTATUS  find_cdxgi_swapchain_present_multiplane_overlay(
			IN PEPROCESS process,
			IN unsigned long long dxgi_base,
			OUT unsigned long long* cdxgi_swapchain_present_multiplane_overlay_out)
		{
			if (!cdxgi_swapchain_present_multiplane_overlay_out)
				return STATUS_INVALID_PARAMETER;

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_cdxgi_swapchain_present_multiplane_overlay(dxgi_base);

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*cdxgi_swapchain_present_multiplane_overlay_out = addr;

			LogDebug( 
				"Found CDXGISwapChain::PresentMultiplaneOverlay at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}
	  

		NTSTATUS  find_cdxgi_swapchain_dwm_legacy_present_dwm(
			IN PEPROCESS process,
			IN unsigned long long dxgi_base,
			OUT unsigned long long* cdxgi_swap_chain_dwm_legacy_present_dwm_out)
		{
			if (!cdxgi_swap_chain_dwm_legacy_present_dwm_out)
				return STATUS_INVALID_PARAMETER;

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_cdxgi_swapchain_dwm_legacy_present_dwm(dxgi_base);

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*cdxgi_swap_chain_dwm_legacy_present_dwm_out = addr;

			LogDebug( 
				"Found CDXGISwapChainDWMLegacy::PresentDWM at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}

		NTSTATUS find_find_windoww(
			IN PEPROCESS process,
			IN unsigned long long user32_base,
			OUT unsigned long long* find_windoww_addr)
		{

			if (!find_windoww_addr)
			{
				return STATUS_INVALID_PARAMETER;
			}

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_module_export_by_name(reinterpret_cast<void*> (user32_base),"FindWindowW");

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*find_windoww_addr = addr;

			LogDebug( 
				"Found FindWindowW at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}

		NTSTATUS find_client_to_screen(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* client_to_screen_addr)
		{
			if (!client_to_screen_addr)
			{
				return STATUS_INVALID_PARAMETER;
			}

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_module_export_by_name(
				reinterpret_cast<void*>(user32_base),
				"ClientToScreen"
			);

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*client_to_screen_addr = addr;

			LogDebug( 
				"Found ClientToScreen at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}


		NTSTATUS find_get_client_rect(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* get_client_rect_addr)
		{
			if (!get_client_rect_addr)
			{
				return STATUS_INVALID_PARAMETER;
			}

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_module_export_by_name(
				reinterpret_cast<void*>(user32_base),
				"GetClientRect"
			);

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*get_client_rect_addr = addr;

			LogDebug( 
				"Found GetClientRect at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}


		NTSTATUS find_get_foreground_window(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* get_foreground_window_addr)
		{
			if (!get_foreground_window_addr)
			{
				return STATUS_INVALID_PARAMETER;
			}

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_module_export_by_name(
				reinterpret_cast<void*>(user32_base),
				"GetForegroundWindow"
			);

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*get_foreground_window_addr = addr;

			LogDebug(
				"Found GetForegroundWindow at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}


		NTSTATUS find_get_window_rect(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* get_window_rect_addr)
		{
			if (!get_window_rect_addr)
			{
				return STATUS_INVALID_PARAMETER;
			}

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_module_export_by_name(
				reinterpret_cast<void*>(user32_base),
				"GetWindowRect"
			);

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*get_window_rect_addr = addr;

			LogDebug( 
				"Found GetWindowRect at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}
	 

		NTSTATUS find_dxgk_get_device_state(
			IN PEPROCESS process,
			IN unsigned long long dxgkrnl_base,
			OUT unsigned long long* dxgk_get_device_state)
		{
			if (!dxgk_get_device_state)
				return STATUS_INVALID_PARAMETER;

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_module_export_by_name(reinterpret_cast<void*> (dxgkrnl_base),"DxgkGetDeviceState");
			if (!addr)
			{
				addr = scanner_fun::find_module_export_by_name(reinterpret_cast<void*> (dxgkrnl_base), "NtGdiDdDDIGetDeviceState");
			}

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*dxgk_get_device_state = addr;

			LogDebug(
				"Found DxgkGetDeviceState at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}
		NTSTATUS   find_open_resource(
			IN PEPROCESS process,
			IN unsigned long long dxgkrnl_base,
			OUT unsigned long long* dxgk_open_resource)
		{
			if (!dxgk_open_resource)
				return STATUS_INVALID_PARAMETER;

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned long long addr = scanner_fun::find_module_export_by_name(reinterpret_cast<void*> (dxgkrnl_base), "DxgkOpenResource");
			if (!addr)
			{
				addr = scanner_fun::find_module_export_by_name(reinterpret_cast<void*> (dxgkrnl_base), "NtGdiDdDDIOpenResource");
			}

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			if (addr == 0)
				return STATUS_NOT_FOUND;

			*dxgk_open_resource = addr;

			LogDebug(
				"Found DxgkOpenResource at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}


		NTSTATUS  hook_swapchain_present_dwm(IN PEPROCESS process)
		{

			if (!process)
			{
				return STATUS_INVALID_PARAMETER;
			}

		 
			
			bool hook_result = hook_utils::hook_user_exception_handler(
				process,
				reinterpret_cast<PVOID>(g_cdxgi_swapchain_present_dwm),
				hook_functions::new_present_dwm,
				true
				 
			);

			return hook_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

		}

		NTSTATUS hook_present_multiplane_overlay(IN PEPROCESS process)
		{ 
			 
			if (!process)
			{
				return STATUS_INVALID_PARAMETER;
			}
			 
		 
		   
			bool hook_result = hook_utils::hook_user_exception_handler(
				process,
				reinterpret_cast<PVOID>(g_cdxgi_swapchain_present_multiplane_overlay),
				hook_functions::new_present_multiplane_overlay,
				true
			 
			);
			 
			return hook_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

			 
		}


		NTSTATUS hook_cocclusion_context_pre_sub_graph(IN PEPROCESS process)
		{
			if (!process)
			{
				return STATUS_INVALID_PARAMETER;
			}

		 

			bool hook_result = hook_utils::hook_user_exception_handler(
			    process,
				reinterpret_cast<PVOID>(g_cocclusion_context_pre_sub_graph),
				hook_functions::new_cocclusion_context_pre_sub_graph,
				true
				 
			);

			return hook_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
		}
		NTSTATUS  hook_cocclusion_context_post_sub_graph(IN PEPROCESS process)
		{
			if (!process)
			{
				return STATUS_INVALID_PARAMETER;
			}
 

			bool hook_result = hook_utils::hook_user_exception_handler(
			    process,
				reinterpret_cast<PVOID>(g_cocclusion_context_post_sub_graph),
				hook_functions::new_cocclusion_context_post_sub_graph ,
				true
			    
			);

			return hook_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
		}

		NTSTATUS hook_cdxgi_swapchain_dwm_legacy_present_dwm(IN PEPROCESS process)
		{
			 
			if (!process)
			{
				return STATUS_INVALID_PARAMETER;
			}

		 

			bool hook_result = hook_utils::hook_user_exception_handler(
				process,
				reinterpret_cast<PVOID>(g_cdxgi_swap_chain_dwm_legacy_present_dwm),
				hook_functions::new_cdxgi_swap_chain_dwm_legacy_present_dwm ,
				true
				 
			);

			return hook_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
		}

		NTSTATUS hook_dxgk_get_device_state(IN PEPROCESS process)
		{
			if (!process)
			{
				return STATUS_INVALID_PARAMETER;
			}

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);
			bool hook_result = false;
			//bool hook_result = hook_utils::hook_user_exception_handler(
			//	NULL,
			//	reinterpret_cast<PVOID>(g_dxgk_get_device_state),
			//	hook_functions::new_dxgk_get_device_state 
			//   
			//);
		 

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			return hook_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

			 
		}

		NTSTATUS hook_get_buffer(IN PEPROCESS process)
		{
			if (!process)
			{
				return STATUS_INVALID_PARAMETER;
			}
			
		 

			unsigned  long long get_buffer_fun = reinterpret_cast<unsigned long long>  (utils::vfun_utils::get_vfunc(reinterpret_cast<PVOID>(utils::dwm_draw::g_pswap_chain), 9));
		 

			bool hook_result = hook_utils::hook_user_exception_handler(
				process,
				reinterpret_cast<PVOID>(get_buffer_fun),
				hook_functions::new_get_buffer,
				true
			 
			);

			return hook_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
		
			 
		 
		}

 

	 
	 
		 
	}

}