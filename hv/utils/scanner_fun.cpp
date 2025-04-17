#include "global_defs.h"
#include "scanner_fun.h"

namespace utils
{
	namespace scanner_fun
	{
		unsigned long long find_ki_preprocess_fault()
		{
			static unsigned long long ki_preprocess_fault_addr{};
			if (ki_preprocess_fault_addr!=0)
			{
				return ki_preprocess_fault_addr;
			}
			auto ntoskrnl_base = module_info::ntoskrnl_base;
			auto ntoskrnl_size = module_info::ntoskrnl_size;
			unsigned long long temp_addr{};
			WindowsVersion Version = static_cast<WindowsVersion>(os_info::get_build_number());

			switch (Version)
			{
			case utils::WINDOWS_7:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x31\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
				break;
			case utils::WINDOWS_7_SP1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x0F\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
				break;
			case utils::WINDOWS_8:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x7E\x03\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
				break;
			case utils::WINDOWS_8_1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\xF3\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
				break;
			case utils::WINDOWS_10_VERSION_1507:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\xF1\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1511:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\xC2\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1607:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\xB3\x03\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1703:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x8C\x03\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1709:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x87\x03\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1803:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x6C\x03\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_1809:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x45\x45\x84\xFF", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x45\x45\x84\xFF", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_19H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x45\x45\x84\xFF", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x49\x45\x84\xE4", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_20H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x49\x45\x84\xE4", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H1:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x49\x45\x84\xE4", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x49\x45\x84\xE4", "x????xxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_10_VERSION_22H2:
			{
				 
				  temp_addr =  signature_scanner:: find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x49\x45\x84\xE4", "x????xxxxxxx"
				);

				 
				  ki_preprocess_fault_addr =
					  signature_scanner::resolve_relative_address(
						  reinterpret_cast<PVOID>(temp_addr), 1, 5);
				 
			}
			break;
			case utils::WINDOWS_11_VERSION_21H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x4D\x44\x8A\x45\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_22H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x85\x75\x04\x00\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_23H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
					"\xE8\xCC\xCC\xCC\xCC\x84\xC0\x75\x4D\x44\x8A\x45\x00", "x????xxxxxxxx"
				);


				ki_preprocess_fault_addr =
					signature_scanner::resolve_relative_address(
						reinterpret_cast<PVOID>(temp_addr), 1, 5);

			}
			break;
			case utils::WINDOWS_11_VERSION_24H2:
			{

				temp_addr = signature_scanner::find_pattern(
					reinterpret_cast<ULONG_PTR>(ntoskrnl_base), ntoskrnl_size,
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
			static unsigned long long mm_copy_memory_addr{};

			if (mm_copy_memory_addr != 0)
				return mm_copy_memory_addr;

			UNICODE_STRING func_name;
			RtlInitUnicodeString(&func_name, L"MmCopyMemory");

			void* func_ptr = MmGetSystemRoutineAddress(&func_name);

			if (func_ptr)
				mm_copy_memory_addr = reinterpret_cast<unsigned long long>(func_ptr);

			return mm_copy_memory_addr;
		 
			 


		}
		unsigned long long find_mm_is_address_valid()
		{
			static unsigned long long mm_is_address_valid_addr = 0;   

			if (mm_is_address_valid_addr != 0)
				return mm_is_address_valid_addr;

			UNICODE_STRING func_name;
			RtlInitUnicodeString(&func_name, L"MmIsAddressValid");

			void* func_ptr = MmGetSystemRoutineAddress(&func_name);

			if (func_ptr)
				mm_is_address_valid_addr = reinterpret_cast<unsigned long long>(func_ptr);

			return mm_is_address_valid_addr;
		}

		unsigned long long find_mm_is_address_valid_ex()
		{
			// Static variable to hold the cached address once found
			static unsigned long long mm_is_address_valid_ex_addr = 0;

			// If the address has already been resolved, return the cached value
			if (mm_is_address_valid_ex_addr != 0)
			{
				return mm_is_address_valid_ex_addr;
			}

			// Start by getting the address of MmIsAddressValid
			unsigned long long mm_is_address_valid_addr = (unsigned long long)MmIsAddressValid;

			// Traverse the memory from MmIsAddressValid to find the call instruction (opcode E8)
			for (unsigned long long curr_addr = mm_is_address_valid_addr; curr_addr < mm_is_address_valid_addr + 0x1000; curr_addr++)
			{
				unsigned char opcode = *((unsigned char*)curr_addr);

				if (opcode == 0xE8) // 0xE8 is the opcode for a "call" instruction
				{
					// Use resolve_relative_address to get the address being called by the call instruction
					mm_is_address_valid_ex_addr = signature_scanner::resolve_relative_address((PVOID)curr_addr, 1, 5);  // 1-byte offset, 5-byte instruction size
					return mm_is_address_valid_ex_addr;
				}
			}

			// Return 0 if the function is not found
			return 0;
		}

		unsigned long long find_rtl_walk_frame_chain()
		{
			static unsigned long long rtl_walk_frame_chain_addr = 0;

			if (rtl_walk_frame_chain_addr != 0)
				return rtl_walk_frame_chain_addr;

			UNICODE_STRING func_name;
			RtlInitUnicodeString(&func_name, L"RtlWalkFrameChain");

			void* func_ptr = MmGetSystemRoutineAddress(&func_name);

			if (func_ptr)
				rtl_walk_frame_chain_addr = reinterpret_cast<unsigned long long>(func_ptr);

			return rtl_walk_frame_chain_addr;
		}

		unsigned long long find_rtl_lookup_function_entry()
		{
			static unsigned long long rtl_lookup_function_entry_addr = 0;

			if (rtl_lookup_function_entry_addr != 0)
				return rtl_lookup_function_entry_addr;

			UNICODE_STRING func_name;
			RtlInitUnicodeString(&func_name, L"RtlLookupFunctionEntry");

			void* func_ptr = MmGetSystemRoutineAddress(&func_name);

			if (func_ptr)
				rtl_lookup_function_entry_addr = reinterpret_cast<unsigned long long>(func_ptr);

			return rtl_lookup_function_entry_addr;
		}



	}
}
