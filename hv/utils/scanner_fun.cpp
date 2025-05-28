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

			}
			break;
			default:
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
					"\x33\xC9\xE8\xCC\xCC\xCC\xCC\x33\xD2\x48\x8D\x4C\x24\x30", "xxx????xxxxxxx"
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
					"\xB1\x01\xE8\xCC\xCC\xCC\xCC\xBA\x50\x73\x54\x65", "xxx????xxxxx"
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



	}
}
