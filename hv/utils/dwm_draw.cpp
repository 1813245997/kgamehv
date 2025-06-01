#include "global_defs.h"
#include "dwm_draw.h"
#include <ia32.hpp>

namespace utils
{
	namespace dwm_draw
	{
	
		PEPROCESS g_dwm_process{};

		unsigned long long  g_precall_addr{};
		unsigned long long  g_postcall_addr{};
		unsigned long long g_ntdll_base{};
		unsigned long long g_ntdll_size{};
		unsigned long long g_user32_base{};
		unsigned long long g_user32_size{};
		unsigned long long g_dwmcore_base{};
		unsigned long long g_dwmcore_size{};
		unsigned long long g_offset_stack{};
		unsigned long long g_ccomposition_present{};
		bool g_kvashadow{};


		NTSTATUS initialize()
		{
			NTSTATUS status{};
			DbgBreakPoint();

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
			
			status = find_precall_and_postcall(g_dwm_process);
			if (!NT_SUCCESS(status))
			{
				return status;
			}

			status = find_ccomposition_present(g_dwm_process);
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

			 
			g_ntdll_base = ntdll_base;
			g_ntdll_size = ntdll_size;
			g_user32_base = user32_base;
			g_user32_size = user32_size;
			g_dwmcore_base = dwmcore_base;
			g_dwmcore_size = dwmcore_size;

			DbgPrintEx(DPFLTR_IHVDRIVER_ID,0,
				"[hv] ntdll.dll  -> Base: 0x%llX, Size: 0x%llX\n", g_ntdll_base, g_ntdll_size);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID,0,
				"[hv] user32.dll -> Base: 0x%llX, Size: 0x%llX\n", g_user32_base, g_user32_size);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID,0,
				"[hv] dwmcore.dll -> Base: 0x%llX, Size: 0x%llX\n", g_dwmcore_base, g_dwmcore_size);

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



		NTSTATUS find_precall_and_postcall(IN PEPROCESS process)
		{
			NTSTATUS status{};
			KAPC_STATE apc_state{};
 
			unsigned long long  precall_addr{};
			unsigned long long  postcall_addr{};
 
			
			 
  

			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);
			precall_addr = signature_scanner::find_pattern_image(
				g_ntdll_base, "\x48\x8B\x4C\x24\xCC\x48\x8B\x54\x24\xCC\x4C", "xxxx?xxxx?x");
			if (!precall_addr)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to locate PreCall pattern in ntdll.dll.\n");
				internal_functions::pfn_ke_unstack_detach_process(&apc_state);
				 
				
				return STATUS_NOT_FOUND;
			}

			postcall_addr = signature_scanner::find_pattern_image(
				g_user32_base, "\x45\x33\xC0\x48\x89\x44\x24\x20\x48\x8D\x4C\x24\x20", "xxxxxxxxxxxxx");
			if (!postcall_addr)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to locate PostCall pattern in user32.dll.\n");
				internal_functions::pfn_ke_unstack_detach_process(&apc_state);
				 
				
				return STATUS_NOT_FOUND;
			}

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			 
			g_precall_addr = precall_addr;
			g_postcall_addr = postcall_addr;

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] PreCall found at: 0x%llX, PostCall found at: 0x%llX\n",
				g_precall_addr, g_postcall_addr);

		
			 
			return status;
		}
	
		NTSTATUS find_ccomposition_present(IN PEPROCESS process )
		{
			 
			KAPC_STATE apc_state{};
			unsigned long long temp_addr = 0;
			unsigned long long ccomposition_present_addr = 0;

			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);
			temp_addr = signature_scanner::find_pattern_image(
				g_dwmcore_base,
				"\x89\xCC\x24\xB8\x01\x00\x00\xE8\xCC\xCC\xCC\xCC\x85\xC0",
				"x?xxxxxx????xx");

			if (temp_addr)
			{
				temp_addr += 7; // 指向CALL的E8偏移
				ccomposition_present_addr = signature_scanner::resolve_relative_address(
					reinterpret_cast<PVOID>(temp_addr), 1, 5);
			}
			else
			{
				// 特征码2: 通常是 LEA + CALL，通过函数头部回溯定位
				temp_addr = signature_scanner::find_pattern_image(
					g_dwmcore_base,
					"\x49\x8D\x4B\xD8\x48\xFF\x15",
					"xxxxxxx");

				if (temp_addr)
				{
					ccomposition_present_addr = signature_scanner::find_function_begin(
						g_dwmcore_base, temp_addr);
				}
				else
				{
					// 特征码3: 如失败，尝试使用 fallback 的 MOV + CALL 模式
					temp_addr = signature_scanner::find_pattern_image(
						g_dwmcore_base,
						"\x40\xB7\x01\xFF\x15",
						"xxxxx");

					if (temp_addr)
					{
						ccomposition_present_addr = signature_scanner::find_function_begin(
							g_dwmcore_base, temp_addr);
					}
				}
			}
			internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			if (ccomposition_present_addr == 0)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to locate CComposition::Present function.\n");
				return STATUS_NOT_FOUND;
			}

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] Found CComposition::Present at 0x%llX\n", ccomposition_present_addr);

			g_ccomposition_present = ccomposition_present_addr;

			return STATUS_SUCCESS;
		}

	}

}