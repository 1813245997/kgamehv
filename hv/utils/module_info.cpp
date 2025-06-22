#include "global_defs.h"
#include "module_info.h"

namespace utils
{
	namespace module_info
	{
		PVOID ntoskrnl_base{};
		ULONG64 ntoskrnl_size{};

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
				return TRUE;
			}

			return FALSE;
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





	 
	}
}