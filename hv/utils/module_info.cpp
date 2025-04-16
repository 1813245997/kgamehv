#include <ntifs.h>
#include "module_info.h"
#include "ntos_struct_def.h"
#include "native_api.h"
#include "string_utils.h"
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
	}
}