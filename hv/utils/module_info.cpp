#include "global_defs.h"
#include "module_info.h"

namespace utils
{
	namespace module_info
	{
		PVOID ntoskrnl_base{};
		ULONG64 ntoskrnl_size{};

		PVOID g_ci_base{};
		ULONG64 g_ci_size{};

		// Common user module global variable definitions
		PVOID ntdll_base{};
		ULONG64 ntdll_size{};
		
		PVOID kernel32_base{};
		ULONG64 kernel32_size{};
		
		PVOID user32_base{};
		ULONG64 user32_size{};
		
		PVOID gdi32_base{};
		ULONG64 gdi32_size{};
		
		PVOID dwmcore_base{};
		ULONG64 dwmcore_size{};
		
		PVOID dwmapi_base{};
		ULONG64 dwmapi_size{};
		
		PVOID dxgi_base{};
		ULONG64 dxgi_size{};
		
		PVOID d3dcompiler_47_base{};
		ULONG64 d3dcompiler_47_size{};

		BOOLEAN  get_driver_module_info(const char* module_name, ULONG64& module_size, PVOID& module_base_address)
		{
			ULONG required_bytes{};
			NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, nullptr, 0, &required_bytes);

			 
			PSYSTEM_MODULE_INFORMATION module_info = (PSYSTEM_MODULE_INFORMATION)ExAllocatePool(NonPagedPool, required_bytes);
			if (module_info == nullptr) {
				return FALSE;
			}
			RtlSecureZeroMemory(module_info, required_bytes);

		 
			status = ZwQuerySystemInformation(SystemModuleInformation, module_info, required_bytes, &required_bytes);
			if (!NT_SUCCESS(status)) {
				ExFreePool(module_info);  
				return FALSE;
			}

			 
			STRING target_module_name{};
			RtlInitString(&target_module_name, module_name);

			 
			for (ULONG i = 0; i < module_info->ModulesCount; i++) {
				STRING current_module_name;
				RtlInitString(&current_module_name, (PCSZ)module_info->Modules[i].FullPathName);

				 
				if (utils::string_utils::contains_substring(&current_module_name, &target_module_name, TRUE)) {
					 
					if (module_info->Modules[i].ImageSize != NULL) {
						module_size = module_info->Modules[i].ImageSize;
						module_base_address = module_info->Modules[i].ImageBase;
						ExFreePool(module_info);  
						return TRUE;
					}
				}
			}

			ExFreePool(module_info);   
			return FALSE;
		}
	 
		BOOLEAN init_ntoskrnl_info()
		{
			PVOID base_address = nullptr;
			ULONG64 size = 0;
 
			if (get_driver_module_info("ntoskrnl.exe", size, base_address)) {
				ntoskrnl_base = base_address;
				ntoskrnl_size = size;
				
			}
			else
			{
				return FALSE;
			}
			// ��ȡ ci.dll
			if (get_driver_module_info("ci.dll", size, base_address)) {
				g_ci_base = base_address;
				g_ci_size = size;
			}
			else {
				 
				return FALSE;
			}
			return TRUE;
		}

		NTSTATUS get_process_module_info(
			_In_ PEPROCESS process,
			_In_ const wchar_t* module_name,
			_Out_ unsigned long long* base_address,
			_Out_ SIZE_T* module_size
		)
		{
			 
			if (!process)
				return STATUS_INVALID_PARAMETER;

			if (!module_name || utils::string_utils::get_wide_string_length(const_cast<PWCHAR>(module_name)) == 0)
				return STATUS_INVALID_PARAMETER;

			if (!base_address || !module_size)
				return STATUS_INVALID_PARAMETER;

			 
			if (utils::process_utils::is_process_exited(process))
				return STATUS_PROCESS_IS_TERMINATING;

		 
			*base_address = 0;
			*module_size = 0;
			NTSTATUS status{};
			SIZE_T bytes_read{};
			ULONG64 KbaseAddress{};
			ULONG64 KmoduleSize{};
			UNICODE_STRING target_module_name{};
			KAPC_STATE apc_state{};
			internal_functions::pfn_rtl_init_unicode_string(&target_module_name, module_name);

			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);
		    if (process_utils::is_process_64_bit(process))
		    {

				PPEB peb64 = reinterpret_cast<PPEB>(utils::process_utils::get_process_peb64_process(process));
				PPEB_LDR_DATA pebldr{};
				internal_functions::pfn_mm_copy_virtual_memory(process, peb64, process, peb64, 1, UserMode, &bytes_read);
				if (!peb64  )
				{
					internal_functions::pfn_ke_unstack_detach_process(&apc_state);
					return STATUS_NOT_FOUND;

				}
				pebldr = reinterpret_cast<PPEB_LDR_DATA>(peb64->Ldr);
				if ( !pebldr)
				{
					internal_functions::pfn_ke_unstack_detach_process(&apc_state);
					return STATUS_NOT_FOUND;
				}

				  
				PLIST_ENTRY pList = reinterpret_cast<PLIST_ENTRY>(&pebldr->InLoadOrderModuleList);
				PLDR_DATA_TABLE_ENTRY plistNext = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(pList->Flink);
				while (reinterpret_cast<unsigned long long> (pList)!= reinterpret_cast<unsigned long long>(plistNext))
				{
					PWCH baseDllName = (PWCH)plistNext->BaseDllName.Buffer;
					UNICODE_STRING uBaseName{};
					internal_functions::pfn_rtl_init_unicode_string(&uBaseName, baseDllName);
					if (internal_functions::pfn_rtl_compare_unicode_string  (&uBaseName,&target_module_name, TRUE) == 0)
					{
						KbaseAddress = reinterpret_cast<unsigned long long  > (plistNext->DllBase);
						KmoduleSize = plistNext->SizeOfImage;
						status = STATUS_SUCCESS;
						break;
					}
					plistNext = (PLDR_DATA_TABLE_ENTRY)plistNext->InLoadOrderLinks.Flink;
				}
		    }
			else
			{
				 
				PEB32* peb32 = reinterpret_cast<PEB32*>(process_utils::get_process_peb32_process(process));
				PPEB_LDR_DATA32 pebldr{};
				internal_functions::pfn_mm_copy_virtual_memory(process, peb32, process, peb32, 1, UserMode, &bytes_read);
				if (!peb32)
				{
					internal_functions::pfn_ke_unstack_detach_process(&apc_state);
					return STATUS_NOT_FOUND;

				}

				 pebldr = reinterpret_cast<PPEB_LDR_DATA32>(UlongToPtr(peb32->Ldr));
				 if (!pebldr)
				 {
					 internal_functions::pfn_ke_unstack_detach_process(&apc_state);
					 return STATUS_NOT_FOUND;
				 }

				PLIST_ENTRY32 pList = reinterpret_cast<PLIST_ENTRY32>(&pebldr->InLoadOrderModuleList);
				PLDR_DATA_TABLE_ENTRY32 plistNext = reinterpret_cast<PLDR_DATA_TABLE_ENTRY32>(UlongToPtr(pList->Flink));
				while (reinterpret_cast<unsigned long long>(pList) != reinterpret_cast<unsigned long long>(plistNext))
				{
					PWCH baseDllName = (PWCH)plistNext->BaseDllName.Buffer;
					UNICODE_STRING uBaseName{};
					internal_functions::pfn_rtl_init_unicode_string(&uBaseName, baseDllName);
					if (internal_functions::pfn_rtl_compare_unicode_string(&uBaseName, &target_module_name, TRUE) == 0)
					{

						KbaseAddress = static_cast<ULONG64>(plistNext->DllBase);
						KmoduleSize = plistNext->SizeOfImage;
						status = STATUS_SUCCESS;
						break;
					}
					plistNext = (PLDR_DATA_TABLE_ENTRY32)UlongToPtr(plistNext->InLoadOrderLinks.Flink);
				}
			}
			internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			*base_address =  KbaseAddress ;
			*module_size = KmoduleSize;
			
			return status;
		}

		NTSTATUS map_user_module_to_kernel(const wchar_t* module_full_path, unsigned long long* base_address_out)
		{
			if (!module_full_path  )	
			{
				return STATUS_INVALID_PARAMETER;
			}

			if (!base_address_out)
			{
				return STATUS_INVALID_PARAMETER;
			}

			HANDLE h_section = nullptr;
			HANDLE h_file = nullptr;
			UNICODE_STRING unicode_module_name;
			PVOID base_address = nullptr;
			SIZE_T view_size = 0;
			IO_STATUS_BLOCK io_status_block{};
			OBJECT_ATTRIBUTES object_attributes = {
				sizeof(OBJECT_ATTRIBUTES),
				nullptr,
				&unicode_module_name,
				OBJ_CASE_INSENSITIVE
			};

			utils::internal_functions::pfn_rtl_init_unicode_string(&unicode_module_name, module_full_path);

			NTSTATUS status = utils::internal_functions::pfn_zw_open_file(
				&h_file,
				FILE_EXECUTE | SYNCHRONIZE,
				&object_attributes,
				&io_status_block,
				FILE_SHARE_READ,
				FILE_SYNCHRONOUS_IO_NONALERT
			);
			if (!NT_SUCCESS(status)) {
				return status;
			}

			object_attributes.ObjectName = nullptr;

			status = utils::internal_functions::pfn_zw_create_section(
				&h_section,
				SECTION_ALL_ACCESS,
				&object_attributes,
				nullptr,
				PAGE_EXECUTE,
				SEC_IMAGE,
				h_file
			);
			if (!NT_SUCCESS(status)) {
				ZwClose(h_file);
				return status;
			}

			status = utils::internal_functions::pfn_zw_map_view_of_section(
				h_section,
				NtCurrentProcess(),
				&base_address,
				0,
				0,
				nullptr,
				&view_size,
				ViewShare,
				0,
				PAGE_READWRITE
			);

			utils::internal_functions::pfn_zw_close (h_section);
			utils::internal_functions::pfn_zw_close(h_file);

			if (!NT_SUCCESS(status)) {
				return status;
			}

			*base_address_out = reinterpret_cast<unsigned long long>(base_address);
			return STATUS_SUCCESS;
		}


		NTSTATUS free_map_module(unsigned long long base_address)
		{

			 
			return utils::internal_functions::pfn_zw_unmap_view_of_section(NtCurrentProcess(), reinterpret_cast<PVOID> (base_address));
		}



		ProcessModule get_module(_In_ PEPROCESS process, _In_ const wchar_t* module_name)
		{
			ProcessModule result{};
			get_process_module_info(process, module_name , &result.base, &result.size);
			return result;
		}


		NTSTATUS initialize_all_user_modules()
		{
			PEPROCESS process{};
			NTSTATUS status = STATUS_SUCCESS;
			
			// Try to get module information from dwm.exe process
			if (!process_utils::get_process_by_name(L"dwm.exe", &process))
			{
				LogError("Failed to find dwm.exe process");
				return STATUS_NOT_FOUND;
			}

			LogInfo("Initializing common user modules from process PID: %d", 
				PsGetProcessId(process));

			// Initialize ntdll.dll
			status = get_process_module_info(process, L"ntdll.dll", 
				reinterpret_cast<unsigned long long*>(&ntdll_base), &ntdll_size);
			if (NT_SUCCESS(status)) {
				LogInfo("ntdll.dll: Base=0x%p, Size=0x%llX", ntdll_base, ntdll_size);
			} else {
				LogError("Failed to get ntdll.dll module info (0x%X)", status);
			}

			// Initialize kernel32.dll
			status = get_process_module_info(process, L"kernel32.dll", 
				reinterpret_cast<unsigned long long*>(&kernel32_base), &kernel32_size);
			if (NT_SUCCESS(status)) {
				LogInfo("kernel32.dll: Base=0x%p, Size=0x%llX", kernel32_base, kernel32_size);
			} else {
				LogError("Failed to get kernel32.dll module info (0x%X)", status);
			}

			// Initialize user32.dll
			status = get_process_module_info(process, L"user32.dll", 
				reinterpret_cast<unsigned long long*>(&user32_base), &user32_size);
			if (NT_SUCCESS(status)) {
				LogInfo("user32.dll: Base=0x%p, Size=0x%llX", user32_base, user32_size);
			} else {
				LogError("Failed to get user32.dll module info (0x%X)", status);
			}

			// Initialize gdi32.dll
			status = get_process_module_info(process, L"gdi32.dll", 
				reinterpret_cast<unsigned long long*>(&gdi32_base), &gdi32_size);
			if (NT_SUCCESS(status)) {
				LogInfo("gdi32.dll: Base=0x%p, Size=0x%llX", gdi32_base, gdi32_size);
			} else {
				LogError("Failed to get gdi32.dll module info (0x%X)", status);
			}

			// Initialize dwmcore.dll (DWM core module)
			status = get_process_module_info(process, L"dwmcore.dll", 
				reinterpret_cast<unsigned long long*>(&dwmcore_base), &dwmcore_size);
			if (NT_SUCCESS(status)) {
				LogInfo("dwmcore.dll: Base=0x%p, Size=0x%llX", dwmcore_base, dwmcore_size);
			} else {
				LogError("Failed to get dwmcore.dll module info (0x%X)", status);
			}

			// Initialize dwmapi.dll
			status = get_process_module_info(process, L"dwmapi.dll", 
				reinterpret_cast<unsigned long long*>(&dwmapi_base), &dwmapi_size);
			if (NT_SUCCESS(status)) {
				LogInfo("dwmapi.dll: Base=0x%p, Size=0x%llX", dwmapi_base, dwmapi_size);
			} else {
				LogError("Failed to get dwmapi.dll module info (0x%X)", status);
			}

			// Initialize dxgi.dll
			status = get_process_module_info(process, L"dxgi.dll", 
				reinterpret_cast<unsigned long long*>(&dxgi_base), &dxgi_size);
			if (NT_SUCCESS(status)) {
				LogInfo("dxgi.dll: Base=0x%p, Size=0x%llX", dxgi_base, dxgi_size);
			} else {
				LogError("Failed to get dxgi.dll module info (0x%X)", status);
			}

 
			// Initialize D3DCompiler_47.dll
			status = get_process_module_info(process, L"d3dcompiler_47.dll", 
				reinterpret_cast<unsigned long long*>(&d3dcompiler_47_base), &d3dcompiler_47_size);
			if (NT_SUCCESS(status)) {
				LogInfo("d3dcompiler_47.dll: Base=0x%p, Size=0x%llX", d3dcompiler_47_base, d3dcompiler_47_size);
			} else {
				LogError("Failed to get d3dcompiler_47.dll module info (0x%X)", status);
			}

			// Clean up process reference
			if (process) {
				internal_functions::pfn_ob_dereference_object(process);
			}

			LogInfo("User modules initialization completed");
			return STATUS_SUCCESS;
		}
	}
}