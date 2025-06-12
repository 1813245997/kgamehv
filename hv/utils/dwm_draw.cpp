#include "global_defs.h"
#include "dwm_draw.h"
#include <ia32.hpp>

namespace utils
{
	namespace dwm_draw
	{
	
		PEPROCESS g_dwm_process{};

		unsigned long long g_precall_addr{};
		unsigned long long g_postcall_addr{};
		unsigned long long g_context_offset{};
		unsigned long long g_ntdll_base{};
		unsigned long long g_ntdll_size{};
		unsigned long long g_user32_base{};
		unsigned long long g_user32_size{};
		unsigned long long g_dwmcore_base{};
		unsigned long long g_dwmcore_size{};
		unsigned long long g_dxgi_base{};
		unsigned long long g_dxgi_size{};
		unsigned long long g_offset_stack{};
		unsigned long long g_ccomposition_present{};
		unsigned long long g_cocclusion_context_post_sub_graph;
		unsigned long long g_cdxgi_swapchain_present_multiplane_overlay{};
		unsigned long long g_cdxgi_swap_chain_dwm_legacy_present_dwm{};
		bool g_kvashadow{};





		NTSTATUS initialize()
		{
			NTSTATUS status{};
			

			status = get_stack_offset();
			if (!NT_SUCCESS(status))
			{
				return status;
			}

			status = get_dwm_process(&g_dwm_process);
			if (!NT_SUCCESS(status))
			{
				return status;
			}

			status = initialize_user_modules(g_dwm_process);
			if (!NT_SUCCESS(status))
			{
				return status;
			}
 

			status = find_precall_address(g_dwm_process,g_ntdll_base,&g_precall_addr);
			if (!NT_SUCCESS(status))
			{
				return status;
			}

			status = find_postcall_address(g_dwm_process, g_user32_base, &g_postcall_addr);
			if (!NT_SUCCESS(status))
			{
				return status;
			}


			status = find_context_offset(g_dwm_process,g_ntdll_base,&g_context_offset);
			if (!NT_SUCCESS(status))
			{
				return status;
			}

			status = find_ccomposition_present(g_dwm_process, g_dwmcore_base, &g_ccomposition_present);
			if (!NT_SUCCESS(status))
			{
				return status;
			}

			status = find_cocclusion_context_post_sub_graph(g_dwm_process, g_dwmcore_base, &g_cocclusion_context_post_sub_graph);
			if (!NT_SUCCESS(status))
			{
				return status;
			}
		 
			status = find_cdxgi_swapchain_present_multiplane_overlay(g_dwm_process, g_dxgi_base, &g_cdxgi_swapchain_present_multiplane_overlay);
			if (!NT_SUCCESS(status))
			{
				return status;
			}
			
			status = find_cdxgi_swapchain_dwm_legacy_present_dwm(g_dwm_process, g_dxgi_base, &g_cdxgi_swap_chain_dwm_legacy_present_dwm);
			if (!NT_SUCCESS(status))
			{
				return status;
			}


			status = hook_present_multiplane_overlay(g_dwm_process );
			if (!NT_SUCCESS(status))
			{
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

		NTSTATUS initialize_user_modules(_In_ PEPROCESS process)
		{
			NTSTATUS status{};
			unsigned long long ntdll_base = 0;
			SIZE_T ntdll_size = 0;
			unsigned long long user32_base = 0;
			SIZE_T user32_size = 0;
			unsigned long long dwmcore_base = 0;
			SIZE_T dwmcore_size = 0;
			unsigned long long dxgi_base = 0;
			SIZE_T dxgi_size = 0;

			// ntdll.dll
			status = module_info::get_process_module_info(process, L"ntdll.dll", &ntdll_base, &ntdll_size);
			if (!NT_SUCCESS(status)) {
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to get ntdll.dll module info (0x%X)\n", status);
				return status;
			}

			// user32.dll
			status = module_info::get_process_module_info(process, L"user32.dll", &user32_base, &user32_size);
			if (!NT_SUCCESS(status)) {
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to get user32.dll module info (0x%X)\n", status);
				return status;
			}

			// dwmcore.dll
			status = module_info::get_process_module_info(process, L"dwmcore.dll", &dwmcore_base, &dwmcore_size);
			if (!NT_SUCCESS(status)) {
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to get dwmcore.dll module info (0x%X)\n", status);
				return status;
			}
			status = module_info::get_process_module_info(process, L"dxgi.dll", &dxgi_base, &dxgi_size);
			if (!NT_SUCCESS(status)) {
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to get dxgi.dll module info (0x%X)\n", status);
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

			DbgPrintEx(DPFLTR_IHVDRIVER_ID,0,
				"[hv] ntdll.dll  -> Base: 0x%llX, Size: 0x%llX\n", g_ntdll_base, g_ntdll_size);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID,0,
				"[hv] user32.dll -> Base: 0x%llX, Size: 0x%llX\n", g_user32_base, g_user32_size);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID,0,
				"[hv] dwmcore.dll -> Base: 0x%llX, Size: 0x%llX\n", g_dwmcore_base, g_dwmcore_size);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] dxgi.dll    -> Base: 0x%llX, Size: 0x%llX\n", g_dxgi_base, g_dxgi_size);

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
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to locate PreCall pattern in ntdll.dll.\n");
				return STATUS_NOT_FOUND;
			}

			*precall_addr_out = match_addr;

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] PreCall found at: 0x%llX\n", *precall_addr_out);

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
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to locate PostCall pattern in user32.dll.\n");
				return STATUS_NOT_FOUND;
			}

			*postcall_addr_out = match_addr;

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] PostCall found at: 0x%llX\n", *postcall_addr_out);

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
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to locate context pattern (all signatures failed).\n");
				return STATUS_NOT_FOUND;
			}

			// 解算相对地址：lea rcx, [rip+rel32]

			ULONG_PTR  abs_addr = signature_scanner::resolve_relative_address(
				reinterpret_cast<PVOID>(match_addr), 3, 7);


			*context_offset_out = abs_addr - ntdll_base;

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] Found context offset = 0x%llX\n", *context_offset_out);


		 

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
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to locate CComposition::Present function.\n");
				return STATUS_NOT_FOUND;
			}

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] Found CComposition::Present at 0x%llX\n", resolved_addr);
			*ccomposition_present_addr_out = resolved_addr;
			 

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

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] Found COcclusionContext::PostSubgraph at 0x%llX\n", addr);
		 
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

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] Found CDXGISwapChain::PresentMultiplaneOverlay at 0x%llX\n", addr);

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

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] Found CDXGISwapChainDWMLegacy::PresentDWM at 0x%llX\n", addr);

			return STATUS_SUCCESS;
		}

		NTSTATUS hook_present_multiplane_overlay(IN PEPROCESS process)
		{ 
			 
			if (!process)
			{
				return STATUS_INVALID_PARAMETER;
			}

			 
			HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);
		   
			bool hook_result =  hyper:: ept_hook_break_point_int3(
				process_id,
				reinterpret_cast<PVOID>(g_cdxgi_swapchain_present_multiplane_overlay),
				new_present_multiplane_overlay,
				reinterpret_cast<void**>(&original_present_multiplane_overlay)
			);

			  
			return hook_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

			 
		}


		INT64(NTAPI* original_present_multiplane_overlay)(
			void* thisptr,
			PVOID dxgi_swap_chain,
			unsigned int a3,
			unsigned int a4,
			int a5,
			const void* a6,
			PVOID64 a7,
			unsigned int a8
			) = nullptr;

	  bool NTAPI  new_present_multiplane_overlay(
		  void* thisptr,
		  PVOID dxgi_swap_chain,
		  unsigned int a3,
		  unsigned int a4,
		  int a5,
		  const void* a6,
		  PVOID64 a7,
		  unsigned int a8)
		{
			
		  
		  //DbgPrintEx(DPFLTR_IHVDRIVER_ID,0,
			 // "[OverlayHook] Exception Context:\n"
			 // "RIP = 0x%llx\n"
			 // "RSP = 0x%llx\n"
			 // "RBP = 0x%llx\n"
			 // "RAX = 0x%llx\n"
			 // "RBX = 0x%llx\n"
			 // "RCX = 0x%llx\n"
			 // "RDX = 0x%llx\n"
			 // "RSI = 0x%llx\n"
			 // "RDI = 0x%llx\n"
			 // "R8  = 0x%llx\n"
			 // "R9  = 0x%llx\n"
			 // "R10 = 0x%llx\n"
			 // "R11 = 0x%llx\n"
			 // "R12 = 0x%llx\n"
			 // "R13 = 0x%llx\n"
			 // "R14 = 0x%llx\n"
			 // "R15 = 0x%llx\n",
			 // ContextRecord->Rip,
			 // ContextRecord->Rsp,
			 // ContextRecord->Rbp,
			 // ContextRecord->Rax,
			 // ContextRecord->Rbx,
			 // ContextRecord->Rcx,
			 // ContextRecord->Rdx,
			 // ContextRecord->Rsi,
			 // ContextRecord->Rdi,
			 // ContextRecord->R8,
			 // ContextRecord->R9,
			 // ContextRecord->R10,
			 // ContextRecord->R11,
			 // ContextRecord->R12,
			 // ContextRecord->R13,
			 // ContextRecord->R14,
			 // ContextRecord->R15
		  //);
		//	ContextRecord->Rip = (unsigned long long) (original_present_multiplane_overlay);


			 
		  return original_present_multiplane_overlay(  thisptr,
			   dxgi_swap_chain,
			  a3,
			  a4,
			   a5,
			   a6,
			   a7,
			   a8);
		}

	  NTSTATUS test_hook_r3_ept_hook_break_point_int3( )
	  {

		  PEPROCESS process{};

		  DbgBreakPoint();

		  if (!process_utils::get_process_by_name(L"TestEptR3HOOK.exe", &process))
		  {
			  return STATUS_NOT_FOUND;
		  }

		
		  KAPC_STATE apc_state{};
		  HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);

		 utils::internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

	     unsigned long long  MessageBoxWaddr =   scanner_fun::find_module_export_by_name((void*)g_user32_base, "MessageBoxW");

		 utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);

		 internal_functions::pfn_ob_dereference_object(process);
		 


		  bool hook_result = hyper::ept_hook_break_point_int3(
			  process_id,
			  reinterpret_cast<PVOID>(MessageBoxWaddr),
			  new_present_multiplane_overlay,
			  reinterpret_cast<void**>(&original_present_multiplane_overlay)
		  );

		
		  return STATUS_SUCCESS;
	  }
		 
	}

}