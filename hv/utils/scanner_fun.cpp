#include "global_defs.h"
#include "scanner_fun.h"

namespace utils
{
	namespace scanner_fun
	{
		unsigned long long find_module_export_by_name(void* module_base, const char* export_name)
		{
			if (!module_base || !export_name)
				return 0;


			auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module_base);
			if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
				return 0;

			auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS64>(
				reinterpret_cast<unsigned char*>(module_base) + dos_header->e_lfanew);


			if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
				return 0;


			auto export_directory_rva = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
			if (!export_directory_rva)
				return 0;

	 

			auto export_directory = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
				reinterpret_cast<unsigned char*>(module_base) + export_directory_rva);

			auto names = reinterpret_cast<DWORD*>(reinterpret_cast<unsigned char*>(module_base) + export_directory->AddressOfNames);
			auto functions = reinterpret_cast<DWORD*>(reinterpret_cast<unsigned char*>(module_base) + export_directory->AddressOfFunctions);
			auto ordinals = reinterpret_cast<WORD*>(reinterpret_cast<unsigned char*>(module_base) + export_directory->AddressOfNameOrdinals);

			for (size_t i = 0; i < export_directory->NumberOfNames; ++i)
			{
				auto name = reinterpret_cast<const char*>(reinterpret_cast<unsigned char*>(module_base) + names[i]);
				if (_stricmp(name, export_name) == 0)
				{
					auto ordinal = ordinals[i];
					auto function_rva = functions[ordinal];
					return reinterpret_cast<unsigned long long>(reinterpret_cast<unsigned char*>(module_base) + function_rva);
				}
			}

			return 0;


			 
		}

		unsigned long long find_win32k_exprot_by_name(const char* export_name)
		{
			if (!export_name)
			{
				return 0;
			}

			PEPROCESS process = nullptr;
			if (!process_utils::get_process_by_name(L"csrss.exe", &process))
			{
			 
				return 0;
			}

			KAPC_STATE apc_state{};
			internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			PVOID win32kfull_base = nullptr;
			size_t win32k_size = 0;

			// 获取 win32kfull.sys 模块基址和大小
			if (!module_info::get_driver_module_info("win32kfull.sys", win32k_size, win32kfull_base))
			{
			  
				internal_functions::pfn_ke_unstack_detach_process(&apc_state);
				internal_functions::pfn_ob_dereference_object(process);
				return 0;
			}

			// 查找导出函数地址
			auto export_addr = scanner_fun::find_module_export_by_name(
			    win32kfull_base,
				export_name
			);

			internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			internal_functions::pfn_ob_dereference_object(process);

			return export_addr;
		}
		unsigned long long find_ki_preprocess_fault()
		{
			unsigned long long ki_preprocess_fault_addr{};
			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{
				 
				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x31\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
				break;
			case utils::WINDOWS_7_SP1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x0F\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
				break;
			case utils::WINDOWS_8:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x7E\x03\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
				break;
			case utils::WINDOWS_8_1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\xF3\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
				break;
			case utils::WINDOWS_10_VERSION_1507:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\xF1\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\xC2\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\xB3\x03\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x8C\x03\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x87\x03\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x6C\x03\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x45\x45\x84\xFF", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base) ,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x45\x45\x84\xFF", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x45\x45\x84\xFF", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x49\x45\x84\xE4", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x49\x45\x84\xE4", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x49\x45\x84\xE4", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{
			 
				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x49\x45\x84\xE4", "x????xxxxxxx"
				);

				

				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{
			 
				  temp_addr =  signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x49\x45\x84\xE4", "x????xxxxxxx"
				);

				  
				  ki_preprocess_fault_addr =
					  signature_scanner::resolve_relative_address(
						  reinterpret_cast<PVOID>(temp_addr), 1, 5);
				  LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);
				 
			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x4D\x44\x8A\x45\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x75\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x4D\x44\x8A\x45\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x4D\x44\x8A\x45\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);
				LogDebug("Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);

			}
			break;
			default:
				LogDebug("default  Windows_X: temp_addr = 0x%llX, ki_preprocess_fault_addr = 0x%llX", temp_addr, ki_preprocess_fault_addr);
				break;
			}
			return ki_preprocess_fault_addr;
		}
		unsigned long long find_mm_copy_memory()
		{

			const auto mm_copy_memory_addr = scanner_fun::find_module_export_by_name(module_info::ntoskrnl_base, "MmCopyMemory");
			return mm_copy_memory_addr;
		}
		unsigned long long find_mm_is_address_valid()
		{
		 
			unsigned long long mm_is_address_valid_addr = scanner_fun::find_module_export_by_name(module_info::ntoskrnl_base, "MmIsAddressValid");
			return mm_is_address_valid_addr;
		}

		unsigned long long find_mm_is_address_valid_ex()
		{
			// Static variable to hold the cached address once found
		   unsigned long long mm_is_address_valid_ex_addr = 0;
			// Start by getting the address of MmIsAddressValid
			unsigned long long mm_is_address_valid_addr = find_mm_is_address_valid();
			unsigned long long curr_addr = mm_is_address_valid_addr + 4;
		 
			// Use resolve_relative_address to get the address being called by the call instruction
			mm_is_address_valid_ex_addr = signature_scanner::resolve_relative_address((PVOID)curr_addr, 1, 5);  // 1-byte offset, 5-byte instruction size
			return mm_is_address_valid_ex_addr;
		 
 
		}

		unsigned long long find_rtl_walk_frame_chain()
		{


			const auto rtl_walk_frame_chain_addr = scanner_fun::find_module_export_by_name(module_info::ntoskrnl_base, "RtlWalkFrameChain");
			 

			return rtl_walk_frame_chain_addr;
		}

		unsigned long long find_rtl_lookup_function_entry()
		{
		 
			 unsigned long long rtl_lookup_function_entry_addr = scanner_fun::find_module_export_by_name(module_info::ntoskrnl_base, "RtlLookupFunctionEntry");
		 

			return rtl_lookup_function_entry_addr;
		}

		unsigned long long find_psp_exit_process()
		{
			//static unsigned long long psp_exit_process_addr = 0;
			unsigned long long psp_exit_process_addr = 0;
			 
			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\xE8\xCC\xCC\xCC\xCC\x49\x8B\xCC\xE8\xCC\xCC\xCC\xCC\x48\x8B\xD8", "x????xxxx????xxx"
				);


				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_7_SP1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\xE8\xCC\xCC\xCC\xCC\x49\x8B\xCC\xE8\xCC\xCC\xCC\xCC\x48\x8B\xD8", "x????xxxx????xxx"
				);


				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x48\x8D\x4C\x24\x20\xE8\xCC\xCC\xCC\xCC\x48\x8B\xCB", "xxx????xxxxxx????xxx"
				);
				temp_addr += 2;

				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8_1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x48\x8D\x4D\xC0", "xxx????xxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4D\xC0", "xxx????xxxxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4D\xC8", "xxx????xxxxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4D\xC8", "xxx????xxxxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4C\x24\x30", "xxx????xxxxxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4C\x24\x30", "xxx????xxxxxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4C\x24\x30", "xxx????xxxxxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4D\xC8", "xxx????xxxxxx"
				);
				temp_addr += 2;

				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x48\x8D\x4C\x24\x30\xE8\xCC\xCC\xCC\xCC\x48\x8B\xCB", "xxx????xxxxxx????xxx"
				);
				temp_addr += 2;

				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x48\x8D\x4C\x24\x30\xE8\xCC\xCC\xCC\xCC\x48\x8B\xCB", "xxx????xxxxxx????xxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x48\x8D\x4D\xC0", "xxx????xxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x48\x8D\x4D\xC0", "xxx????xxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x48\x8D\x4D\xC0", "xxx????xxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x48\x8D\x4D\xC0", "xxx????xxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x48\x8D\x4D\xC0", "xxx????xxxx"
				);
				temp_addr += 2;

				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base), 
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4C\x24\x38", "xxx????xxxxxxx"
				);
				temp_addr += 2;

				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4C\x24\x30", "xxx????xxxxxxx"
				);

				temp_addr += 2;
				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4C\x24\x30", "xxx????xxxxxxx"
				);
				temp_addr += 2;

				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),  
					"\xB1\x01\xE8\xCC\xCC\xCC\xCC\xBA\x50\x73\x54\x65", "xxx????xxxxx"
				);
				temp_addr += 2;

				psp_exit_process_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			default:
				break;
			}

			return psp_exit_process_addr;
		}

		unsigned long long find_exp_lookup_handle_table_entry()
		{
			unsigned long long exp_lookup_handle_table_entry_addr = 0;

			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{
				//ExEnumHandleTable
				//PAGE:0000000140306AFC 49 8B CD                                                        mov     rcx, r13
				//PAGE : 0000000140306AFF E8 AC 1B FF FF                                                  call    ExpLookupHandleTableEntry
				//PAGE : 0000000140306B04 49 3B C2                                                        cmp     rax, r10
				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xCD\xE8\xCC\xCC\xCC\xCC\x49\x3B\xC2", "xxxx????xxx","PAGE"
				);
				temp_addr += 3;

				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_7_SP1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xCD\xE8\xCC\xCC\xCC\xCC\x49\x3B\xC2", "xxxx????xxx", "PAGE"
				);
				temp_addr += 3;

				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xCD\xE8\xCC\xCC\xCC\xCC\x49\x3B\xC2", "xxxx????xxx", "PAGE"
				);
				temp_addr += 3;

				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8_1:
			{
				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xCD\xE8\xCC\xCC\xCC\xCC\x49\x3B\xC2", "xxxx????xxx", "PAGE"
				);
				temp_addr += 3;

				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);
				temp_addr += 3;

				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);
				temp_addr += 3;

				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3 ;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);
				temp_addr += 3;

				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF8", "xxxx????xxx", "PAGE"
				);
				temp_addr += 3;

				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x49\x8B\xD1\xE8\xCC\xCC\xCC\xCC\x41\xB8\xFF\x00\x00\x00", "xxxx????xxxxxx", "PAGE"
				);

				temp_addr += 3;
				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4C\x24\x30", "xxx????xxxxxxx", "PAGE"
				);
				temp_addr += 2;

				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xB1\x01\xE8\xCC\xCC\xCC\xCC\xBA\x50\x73\x54\x65", "xxx????xxxxx", "PAGE"
				);
				temp_addr += 2;

				exp_lookup_handle_table_entry_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			default:
				break;
			}

			return exp_lookup_handle_table_entry_addr;
		}

		unsigned long long find_mm_create_kernel_stack()
		{
			unsigned long long mm_create_kernel_stack_addr = 0;

			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{
				 
				//KeAllocateCalloutStackEx
				//PAGE:000000014046EE4D E8 3E 69 C1 FF                                                  call    MmCreateKernelStack
				//PAGE : 000000014046EE52 48 85 C0                                                        test    rax, rax
				//PAGE : 000000014046EE55 74 5D                                                           jz      short loc_14046EEB4

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x85\xC0\x74\x5D\x49\x89\x06", "x????xxxxxxxx", "PAGE"
				);
			 

				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_7_SP1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x85\xC0\x74\x5D\x49\x89\x06", "x????xxxxxxxx", "PAGE"
				);
				 

				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x85\xC0\x0F\x84\xCD\x82\x0F\x00", "x????xxxxxxxxx", "PAGE"
				);
				 

				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8_1:
			{
				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x85\xC0\x0F\x84\xAA\x3F\x0C\x00", "x????xxxxxxxxx", "PAGE"
				);
				 

				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x85\xC0\x0F\x84\x31\x37\x0D\x00", "x????xxxxxxxxx", "PAGE"
				);

			 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x85\xC0\x0F\x84\x55\x56\x09\x00", "x????xxxxxxxxx", "PAGE"
				);

			 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x8B\xE8\x48\x85\xC0\x0F\x85\xBE\x00\x00\x00", "x????xxxxxxxxxxxx", ".text"
				);

			 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF0\x48\x85\xC0\x0F\x85\xD0\x00\x00\x00", "x????xxxxxxxxxxxx", ".text"
				);

				 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x8B\xF0\x48\x85\xC0\x0F\x84\x04\x01\x00\x00", "x????xxxxxxxxxxxx", ".text"
				);

				 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x4C\x8B\xF0\x48\x85\xC0\x0F\x84\xA8\x99\x17\x00", "x????xxxxxxxxxxxx", ".text"
				);
				 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x41\x83\xCF\x04", "x????xxxx", ".text"
				);
			 

				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x41\x83\xCF\x04", "x????xxxx", ".text"
				);
			 

				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x41\x83\xCF\x04", "x????xxxx", ".text"
				);

				 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x41\x83\xCF\x04", "x????xxxx",  ".text"
				);

				 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x41\x83\xCF\x04", "x????xxxx", ".text"
				);

				 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x41\x83\xCF\x04", "x????xxxx", ".text"
				);

				 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x41\x83\xCF\x04", "x????xxxx",  ".text"
				);

			 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x41\x83\xCF\x04", "x????xxxx", ".text"
				);
			 

				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x8B\xE8\x48\x85\xC0\x0F\x84\x34\x01\x00\x00", "x????xxxxxxxxxxxx", ".text"
				);
				 

				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x8B\xE8\x48\x85\xC0\x0F\x84\x38\x01\x00\x00", "x????xxxxxxxxxxxx", ".text"
				);

				 
				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x8B\xE8\x48\x85\xC0\x0F\x84\x38\x01\x00\x00", "x????xxxxxxxxxxxx", ".text"
				);
			 

				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x4C\x8B\x7D\x9F", "x????xxxx", ".text"
				);
				 

				mm_create_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			default:
				break;
			}

			return mm_create_kernel_stack_addr;
		}

		unsigned long long find_mm_delete_kernel_stack()
		{
			unsigned long long mm_delete_kernel_stack_addr = 0;

			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{

				//KeFreeCalloutStack
				//PAGE:000000014046ED59 8B D5                                                           mov     edx, ebp
				//PAGE:000000014046EE4D E8 3E 69 C1 FF                                                  call    MmCreateKernelStack
				//PAGE : 000000014046EE52 48 85 C0                                                        test    rax, rax
				//PAGE : 000000014046EE55 74 5D                                                           jz      short loc_14046EEB4

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x0F\xB6\x43\x05", "xxx????xxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_7_SP1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x0F\xB6\x43\x05", "xxx????xxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x0F\xB6\x43\x05", "xxx????xxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8_1:
			{
				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD6\xE8\xCC\xCC\xCC\xCC\x0F\xB6\x47\x05", "xxx????xxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05", "xxx????xxx", "PAGE"
				);


				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05", "xxx????xxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05", "xxx????xxx", "PAGE"
				);
				temp_addr += 2;

				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05", "xxx????xxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05", "xxx????xxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);
				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);
				temp_addr += 2;

				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);
				temp_addr += 2;

				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);
				temp_addr += 2;

				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\x8B\xD5\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "xxx????xxx????xxxxxx", "PAGE"
				);

				temp_addr += 2;
				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{

				temp_addr = signature_scanner::find_pattern_image(
					reinterpret_cast<ULONG_PTR>(module_info::ntoskrnl_base),
					"\xE8\xCC\xCC\xCC\xCC\x48\x8B\x05\xCC\xCC\xCC\xCC\x48\x05\x00\x10\x00\x00", "x????xxx????xxxxxx", "PAGE"
				);


				mm_delete_kernel_stack_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			default:
				break;
			}

			return mm_delete_kernel_stack_addr;
		}

		unsigned long long find_cocclusion_context_pre_sub_graph(unsigned long long image_base)
		{
			unsigned long long cocclusion_context_pre_sub_graph_addr = 0;

			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{
			//text:0000000180085120 ? PreSubgraph@COcclusionContext@@QEAAJPEBVCVisualTree@@PEA_N@Z proc near
			//	.text:0000000180085120; CODE XREF : CVisualTreeIterator::WalkSubtree<COcclusionContext>(CVisual const*, COcclusionContext*, WalkReason) + E8↑p
			//	.text : 0000000180085120; DATA XREF : .rdata : 000000018032EA08↓o ...
			//	.text : 0000000180085120
			//	.text:0000000180085120                                                 var_260 = dword ptr - 260h
			//	.text:0000000180085120                                                 var_258 = qword ptr - 258h
			//	.text:0000000180085120                                                 var_250 = qword ptr - 250h
			//	.text:0000000180085120                                                 var_240 = byte ptr - 240h
			//	.text:0000000180085120                                                 var_23F = byte ptr - 23Fh
			//	.text:0000000180085120                                                 var_23E = byte ptr - 23Eh
			//	.text:0000000180085120                                                 var_23D = byte ptr - 23Dh
			//	.text:0000000180085120                                                 var_23C = dword ptr - 23Ch
			//	.text:0000000180085120                                                 var_238 = byte ptr - 238h
			//	.text:0000000180085120                                                 var_230 = qword ptr - 230h
			//	.text:0000000180085120                                                 var_220 = qword ptr - 220h
			//	.text:0000000180085120                                                 var_210 = xmmword ptr - 210h
			//	.text:0000000180085120                                                 var_200 = xmmword ptr - 200h
			//	.text:0000000180085120                                                 var_1F0 = xmmword ptr - 1F0h
			//	.text:0000000180085120                                                 var_1E0 = xmmword ptr - 1E0h
			//	.text:0000000180085120                                                 var_1D0 = dword ptr - 1D0h
			//	.text:0000000180085120                                                 var_1C0 = byte ptr - 1C0h
			//	.text:0000000180085120                                                 var_180 = dword ptr - 180h
			//	.text:0000000180085120                                                 var_170 = byte ptr - 170h
			//	.text:0000000180085120                                                 var_130 = dword ptr - 130h
			//	.text:0000000180085120                                                 var_120 = byte ptr - 120h
			//	.text:0000000180085120                                                 var_E0 = dword ptr - 0E0h
			//	.text:0000000180085120                                                 lpMem = qword ptr - 0D0h
			//	.text:0000000180085120                                                 var_C0 = xmmword ptr - 0C0h
			//	.text:0000000180085120                                                 var_B0 = xmmword ptr - 0B0h
			//	.text:0000000180085120                                                 var_A0 = qword ptr - 0A0h
			//	.text:0000000180085120                                                 var_98 = qword ptr - 98h
			//	.text:0000000180085120                                                 var_90 = xmmword ptr - 90h
			//	.text:0000000180085120                                                 var_80 = xmmword ptr - 80h
			//	.text:0000000180085120                                                 var_70 = xmmword ptr - 70h
			//	.text:0000000180085120                                                 var_60 = xmmword ptr - 60h
			//	.text:0000000180085120                                                 var_50 = xmmword ptr - 50h
			//	.text:0000000180085120                                                 var_40 = xmmword ptr - 40h
			//	.text:0000000180085120                                                 arg_18 = qword ptr  28h
			//	.text:0000000180085120
			//	.text:0000000180085120; __unwind{ // __GSHandlerCheck
			//	.text:0000000180085120 48 89 5C 24 20                                                  mov[rsp - 8 + arg_18], rbx
			//	.text:0000000180085125 55                                                              push    rbp
			//	.text:0000000180085126 56                                                              push    rsi
			//	.text:0000000180085127 57                                                              push    rdi
			//	.text:0000000180085128 41 54                                                           push    r12
			//	.text:000000018008512A 41 55                                                           push    r13
			//	.text:000000018008512C 41 56                                                           push    r14
			//	.text:000000018008512E 41 57                                                           push    r15

				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x4C\x8B\xDC\x49\x89\x5B\x10\x49\x89\x6B\x18\x56\x57\x41\x54\x48\x81\xEC\x80\x00\x00\x00", 
					"xxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_7_SP1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x4C\x8B\xDC\x49\x89\x5B\x10\x49\x89\x6B\x18\x56\x57\x41\x54\x48\x81\xEC\x80\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_8:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\xFF\xF5\x53\x56\x57\x41\x54\x41\x55\x41\x56\x48\x8D\x6C\x24\xC0",
					"xxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_8_1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\xFF\xF5\x53\x56\x57\x41\x54\x41\x55\x41\x56\x48\x8D\x6C\x24\xC0", 
					"xxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x48\x8D\xAC\x24\xC8\xFC\xFF\xFF",
					"xxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x48\x89\x58\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x18\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\xA8\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x40\xFC\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xB0\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xD0\xFD\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xB0\xFE\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xA0\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xA0\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xC0\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xC0\xFE\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xC0\xFE\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xB0\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xB0\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x55\x53\x56\x57\x41\x54\x41\x56\x41\x57\x48\x8D\xAC\x24\xD0\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x55\x53\x56\x57\x41\x54\x41\x56\x41\x57\x48\x8D\xAC\x24\xD0\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x55\x53\x56\x57\x41\x54\x41\x56\x41\x57\x48\x8D\xAC\x24\xD0\xFE\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x4C\x89\x44\x24\x18\x48\x89\x54\x24\x10\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\xE1", 
					"xxxxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_pre_sub_graph_addr = temp_addr;

			}
			break;
			default:
				break;
			}

			return cocclusion_context_pre_sub_graph_addr;
		}

		unsigned long long find_cocclusion_context_post_sub_graph(unsigned long long image_base)
		{
			unsigned long long cocclusion_context_post_sub_graph_addr = 0;

			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{
					//.text:000000018010CA90 ? PostSubgraph@COcclusionContext@@UEAAJPEBVCVisualTree@@PEA_N@Z proc near
					//.text:000000018010CA90; DATA XREF : .text : 000000018007E344↑o
					//.text : 000000018010CA90; .text:0000000180085318↑o ...
					//.text:000000018010CA90
					//.text:000000018010CA90                                                 var_28 = dword ptr - 28h
					//.text:000000018010CA90                                                 arg_0 = qword ptr  8
					//.text:000000018010CA90                                                 arg_8 = qword ptr  10h
					//.text:000000018010CA90                                                 arg_10 = qword ptr  18h
					//.text:000000018010CA90
					//.text:000000018010CA90 48 89 5C 24 08                                                  mov[rsp + arg_0], rbx
					//.text:000000018010CA95 48 89 6C 24 10                                                  mov[rsp + arg_8], rbp
					//.text:000000018010CA9A 48 89 74 24 18                                                  mov[rsp + arg_10], rsi
					//.text:000000018010CA9F 57                                                              push    rdi
					//.text:000000018010CAA0 41 56                                                           push    r14
					//.text:000000018010CAA2 41 57                                                           push    r15
				 
				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x41\x30", 
					"xxxxxxxxxxxxxxxxxxx",
					".text"
				);

			 
				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_7_SP1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x41\x30",
					"xxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_8:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x41\x56\x41\x57\x48\x83\xEC\x30\x41\xC6\x00\x01",
					"xxxxxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_8_1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x41\x56\x41\x57\x48\x83\xEC\x30\x41\xC6\x00\x01", 
					"xxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x41\x56\x41\x57\x48\x83\xEC\x30\x41\xC6\x00\x01", 
					"xxxxxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x41\x56\x41\x57\x48\x83\xEC\x30\x41\xC6\x00\x01",
					"xxxxxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x55\x53\x56\x41\x54",
					"xxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\xE8\xFE\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x40\xFF\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x08\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxxx", 
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{


				temp_addr = signature_scanner::find_pattern_image(
					 image_base ,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\xC8\xFD\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxxx", 
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{

				
				temp_addr = signature_scanner::find_pattern_image(
					 image_base ,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\xF8\xFE\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					 image_base ,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\xF8\xFE\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{
			 

				temp_addr = signature_scanner::find_pattern_image(
					 image_base ,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x08\xFF\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					 image_base ,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x08\xFF\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					 image_base ,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x08\xFF\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					 image_base ,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x08\xFF\xFF\xFF", 
					"xxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					 image_base ,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x08\xFF\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);
				

				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
				 image_base ,
					"\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x41\x30", "xxxxxxxxxxxxxxxxxxx", ".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
				    image_base ,
					"\x48\x89\x5C\x24\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\xF0\xFE\xFF\xFF",
					"xxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base ,
					"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\xD8\xFE\xFF\xFF\x48\x81\xEC\xF0\x01\x00\x00\x0F\x29\x70\xB8\x0F\x29\x78\xA8\x44\x0F\x29\x40\x98\x44\x0F\x29\x48\x88\x48\x8B\x05",
					"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					 image_base ,
					"\x4C\x8B\xDC\x55\x53\x56\x57\x49\x8D\x6B\xC8", 
					"xxxxxxxxxxx", 
					".text"
				);


				cocclusion_context_post_sub_graph_addr = temp_addr;

			}
			break;
			default:
				break;
			}

			return cocclusion_context_post_sub_graph_addr;
		}


		unsigned long long find_cdxgi_swapchain_present_dwm(unsigned long long image_base)
		{

			unsigned long long cdxgi_swapchain_present_dwm_addr = 0;

			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{


				/*		temp_addr = signature_scanner::find_pattern_image(
							image_base,
							"\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x41\x30",
							"xxxxxxxxxxxxxxxxxxx",
							".text"
						);*/


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_7_SP1:
			{


				/*	temp_addr = signature_scanner::find_pattern_image(
						image_base,
						"\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x41\x30",
						"xxxxxxxxxxxxxxxxxxx",
						".text"
					);*/


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_8:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x18\x89\x54\x24\x10",
					"xxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_8_1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x18\x89\x54\x24\x10",
					"xxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x18\x89\x54\x24\x10",
					"xxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x18\x89\x54\x24\x10",
					"xxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x44\x89\x44\x24\x18",
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x44\x89\x44\x24\x18",
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\xA0\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\xA0\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{
				//OK

				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x80\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\xA0",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_dwm_addr = temp_addr;

			}
			break;
			default:
				break;
			}

			return cdxgi_swapchain_present_dwm_addr;
		}

		unsigned long long find_cdxgi_swapchain_present_multiplane_overlay(unsigned long long image_base)
		{
			unsigned long long cdxgi_swapchain_present_multiplane_overlay_addr = 0;

			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{
	 

		/*		temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x41\x30",
					"xxxxxxxxxxxxxxxxxxx",
					".text"
				);*/


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_7_SP1:
			{


				/*	temp_addr = signature_scanner::find_pattern_image(
						image_base,
						"\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x41\x30",
						"xxxxxxxxxxxxxxxxxxx",
						".text"
					);*/


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_8:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x18\x89\x54\x24\x10", 
					"xxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_8_1:
			{

				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x18\x89\x54\x24\x10",
					"xxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x18\x89\x54\x24\x10", 
					"xxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x18\x89\x54\x24\x10",
					"xxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x44\x89\x44\x24\x18", 
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x44\x89\x44\x24\x18", 
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\xA0\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\xA0\x00\x00\x00", 
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00", 
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00", 
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00", 
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x8B\xC4\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00", 
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00",
					"xxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x90\x00\x00\x00", 
					"xxxxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\xA0", 
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_present_multiplane_overlay_addr = temp_addr;

			}
			break;
			default:
				break;
			}

			return cdxgi_swapchain_present_multiplane_overlay_addr;
		}

		unsigned long long find_cdxgi_swapchain_dwm_legacy_present_dwm(unsigned long long image_base)
		{
			unsigned long long cdxgi_swapchain_dwm_legacy_present_dwm_addr = 0;

			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{


				/*		temp_addr = signature_scanner::find_pattern_image(
							image_base,
							"\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x41\x30",
							"xxxxxxxxxxxxxxxxxxx",
							".text"
						);*/


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_7_SP1:
			{


				/*	temp_addr = signature_scanner::find_pattern_image(
						image_base,
						"\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x41\x30",
						"xxxxxxxxxxxxxxxxxxx",
						".text"
					);*/


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_8:
			{


				/*	temp_addr = signature_scanner::find_pattern_image(
						image_base,
						"\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x41\x56\x41\x57\x48\x83\xEC\x30\x41\xC6\x00\x01",
						"xxxxxxxxxxxxxxxxxxxxxxxxxxxx",
						".text"
					);*/


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_8_1:
			{

				/*	temp_addr = signature_scanner::find_pattern_image(
						image_base,
						"\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x41\x56\x41\x57\x48\x83\xEC\x30\x41\xC6\x00\x01",
						"xxxxxxxxxxxxxxxxxxxxxxxxxxxx",
						".text"
					);*/


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x44\x89\x4C\x24\x20\x89\x54\x24\x10",
					"xxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\xF9",
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8B\xEC",
					"xxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8B\xEC", 
					"xxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\xD0", 
					"xxxxxxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\xB0", 
					"xxxxxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x53\x48\x83\xEC\x50\x48\x8B\x49\x40",
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x53\x48\x83\xEC\x50\x48\x8B\x49\x40", 
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x53\x48\x83\xEC\x50\x48\x8B\x49\x40", 
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x53\x48\x83\xEC\x50\x48\x8B\x49\x40", 
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x53\x48\x83\xEC\x50\x48\x8B\x49\x40",
					"xxxxxxxxxx"
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x53\x48\x83\xEC\x50\x48\x8B\x49\x40", 
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x53\x48\x83\xEC\x50\x48\x8B\x49\x40",
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x53\x48\x83\xEC\x50\x48\x8B\x49\x40", 
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x50\x48\x8B\x49\x40",
					"xxxxxxxxxxxxxx"  ,
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x50\x48\x8B\x49\x40",
					"xxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x50\x48\x8B\x49\x40", 
					"xxxxxxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{


				temp_addr = signature_scanner::find_pattern_image(
					image_base,
					"\x40\x53\x48\x83\xEC\x50\x48\x8B\x49\x40",
					"xxxxxxxxxx",
					".text"
				);


				cdxgi_swapchain_dwm_legacy_present_dwm_addr = temp_addr;

			}
			break;
			default:
				break;
			}

			return cdxgi_swapchain_dwm_legacy_present_dwm_addr;
		}

	  
		unsigned long long find_mm_allocate_independent_pages()
		{
			static unsigned long long mm_allocate_independent_pages_addr{};
			if (mm_allocate_independent_pages_addr != 0)
			{
				return mm_allocate_independent_pages_addr;
			}
			auto ntoskrnl_base = module_info::ntoskrnl_base;
			auto ntoskrnl_size = module_info::ntoskrnl_size;
			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			//.text:  00000001403755D0 41 8B D6                                                        mov     edx, r14d
			//.text : 00000001403755D3 B9 00 10 00 00                                                  mov     ecx, 1000h
			//.text : 00000001403755D8 E8 83 01 3E 00                                                  call    MmAllocateIndependentPages; Call
			switch (Version)
			{
			case utils::WINDOWS_7:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xB9\x00\x50\x00\x00\x0F\xB7\x50\x54", "xxxxxxxxx"
				);

				temp_addr += 9;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_7_SP1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xB9\x00\x50\x00\x00\x0F\xB7\x50\x54", "xxxxxxxxx"
				);
				temp_addr += 9;

				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xB9\x00\x40\x00\x00\x0F\xB7\x50\x44", "xxxxxxxxx"
				);

				temp_addr += 9;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8_1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x0F\xB7\xD2\xB9\x00\x70\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x83\xCA\xFF\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x83\xCA\xFF\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x83\xCA\xFF\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x83\xCA\xFF\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x83\xCA\xFF\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x83\xCA\xFF\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x83\xCA\xFF\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x83\xCA\xFF\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x83\xCA\xFF\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x41\x8B\xD6\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x41\x8B\xD6\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x41\x8B\xD6\xB9\x00\x10\x00\x00",
					"xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x41\x8B\xD6\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x41\x8B\xD6\xB9\x00\x10\x00\x00",
					"xxxxxxxx"
				);

				temp_addr += 8;


				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x41\x8B\xD6\xB9\x00\x10\x00\x00\xE8",
					"xxxxxxxxx"
				);

				temp_addr += 8;

				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x41\x8B\xD6\xB9\x00\x10\x00\x00", "xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x41\x8B\xD6\xB9\x00\x10\x00\x00",
					"xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x41\x8B\xD6\xB9\x00\x10\x00\x00",
					"xxxxxxxx"
				);

				temp_addr += 8;
				mm_allocate_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			default:
				break;
			}
			return mm_allocate_independent_pages_addr;
		}
		unsigned long long find_mm_free_independent_pages()
		{
			static unsigned long long mm_free_independent_pages_addr{};
			if (mm_free_independent_pages_addr != 0)
			{
				return mm_free_independent_pages_addr;
			}
			auto ntoskrnl_base = module_info::ntoskrnl_base;
			auto ntoskrnl_size = module_info::ntoskrnl_size;
			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			//PAGE:000000014088EAF0 74 0A                                                           jz      short loc_14088EAFC; Jump if Zero(ZF = 1)
			//PAGE:000000014088EAF2 BA 00 50 00 00                                                  mov     edx, 5000h
			//PAGE :000000014088EAF7 E8 34 91 EC FF                                                call    MmFreeIndependentPages; Call Procedure

			switch (Version)
			{
			case utils::WINDOWS_7:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_7_SP1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);
				temp_addr += 7;

				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_8_1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1507:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00", "xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;


				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00", "xxxxxxx"
				);

				temp_addr += 7;

				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00", "xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\x74\x0A\xBA\x00\x50\x00\x00",
					"xxxxxxx"
				);

				temp_addr += 7;
				mm_free_independent_pages_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			default:
				break;
			}
			return mm_free_independent_pages_addr;
		}

		unsigned long long find_open_resource()
		{
			return 0;
		}


	}
}
