#include "../utils/global_defs.h"
#include "hook_mem_protection.h"
namespace utils
{
	namespace hook_mem_protecion
	{



		NTSTATUS(__fastcall* original_mm_copy_memory)(
			_In_ PVOID target_address,
			_In_ MM_COPY_ADDRESS source_address,
			_In_ SIZE_T number_of_bytes,
			_In_ ULONG flags,
			_Out_ PSIZE_T number_of_bytes_transferred
			);

		BOOLEAN(__fastcall* original_mm_is_address_valid)(
			_In_ PVOID VirtualAddress
			);

		ULONG(NTAPI* original_rtl_walk_frame_chain)(
			_In_ PVOID* callers,
			_In_ ULONG count,
			_In_ ULONG flags
			);


		PRUNTIME_FUNCTION(NTAPI* original_rtl_lookup_function_entry)(
			_In_ DWORD64 control_pc,
			_Out_ PDWORD64 image_base,
			_Inout_opt_ PUNWIND_HISTORY_TABLE history_table
			);

		NTSTATUS(NTAPI* original_nt_query_virtual_memory) (
				_In_ HANDLE ProcessHandle,
				_In_opt_ PVOID BaseAddress,
				_In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
				_Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
				_In_ SIZE_T MemoryInformationLength,
				_Out_opt_ PSIZE_T ReturnLength
				) = nullptr;
	
		NTSTATUS(NTAPI* original_nt_read_virtual_memory)(
					_In_ HANDLE ProcessHandle,
					_In_opt_ PVOID BaseAddress,
					_Out_writes_bytes_(NumberOfBytesToRead) PVOID Buffer,
					_In_ SIZE_T NumberOfBytesToRead,
					_Out_opt_ PSIZE_T NumberOfBytesRead
				) = nullptr;

		NTSTATUS initialize_mem_protections()
		{

			NTSTATUS status = STATUS_SUCCESS;
			if (!reinterpret_cast<void*>(utils::internal_functions::pfn_mm_copy_memory))
			{
				LogError("pfn_mm_copy_memory is not found");
				status = STATUS_NOT_FOUND;
				goto clear;
			}

			if (!utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(utils::internal_functions::pfn_mm_copy_memory),
				hook_mm_copy_memory,
				reinterpret_cast<void**>(&original_mm_copy_memory)
			))
			{
				LogError("Failed to hook pfn_mm_copy_memory");
				status = STATUS_UNSUCCESSFUL;
				goto clear;
			}


			LogInfo("Hook pfn_mm_copy_memory SUCCESS");

			if (!reinterpret_cast<void*>(utils::internal_functions::pfn_mm_is_address_valid))
			{
				LogError("pfn_mm_is_address_valid is not found");
				status = STATUS_NOT_FOUND;
				goto clear;
			}

			if (!utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(utils::internal_functions::pfn_mm_is_address_valid),
				hook_mm_is_address_valid,
				reinterpret_cast<void**>(&original_mm_is_address_valid)
			))
			{
				LogError("Failed to hook pfn_mm_is_address_valid");
				status = STATUS_UNSUCCESSFUL;
				goto clear;
			}
			LogInfo("Hook pfn_mm_is_address_valid SUCCESS");

			if (!reinterpret_cast<void*>(utils::internal_functions::pfn_rtl_walk_frame_chain))
			{
				LogError("pfn_rtl_walk_frame_chain is not found");
				status = STATUS_NOT_FOUND;
				goto clear;
			}

			if (!utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(utils::internal_functions::pfn_rtl_walk_frame_chain),
				hook_rtl_walk_frame_chain,
				reinterpret_cast<void**>(&original_rtl_walk_frame_chain)
			))
			{
				LogError("Failed to hook pfn_rtl_walk_frame_chain");
				status = STATUS_UNSUCCESSFUL;
				goto clear;
			}
			LogInfo("Hook pfn_rtl_walk_frame_chain SUCCESS");

		if (!reinterpret_cast<void*>(utils::internal_functions::pfn_rtl_lookup_function_entry))
		{
			LogError("pfn_rtl_lookup_function_entry is not found");
			status = STATUS_NOT_FOUND;
			goto clear;
		}
		
		if (!utils::hook_utils::hook_kernel_function(
			reinterpret_cast<void*>(utils::internal_functions::pfn_rtl_lookup_function_entry),
			hook_rtl_lookup_function_entry,
			reinterpret_cast<void**>(&original_rtl_lookup_function_entry)
		))
		{
			LogError("Failed to hook pfn_rtl_lookup_function_entry");
			status = STATUS_UNSUCCESSFUL;
			goto clear;
		}
		LogInfo("Hook pfn_rtl_lookup_function_entry SUCCESS");

		if (!reinterpret_cast<void*>(utils::internal_functions::pfn_nt_query_virtual_memory))
		{
			LogError("pfn_nt_query_virtual_memory is not found");
			status = STATUS_NOT_FOUND;
			goto clear;
		}

		if (!utils::hook_utils::hook_kernel_function(
			reinterpret_cast<void*>(utils::internal_functions::pfn_nt_query_virtual_memory),
			hook_nt_query_virtual_memory,
			reinterpret_cast<void**>(&original_nt_query_virtual_memory)
		))
		{
			LogError("Failed to hook pfn_nt_query_virtual_memory");
			status = STATUS_UNSUCCESSFUL;
			goto clear;
		}
		LogInfo("Hook pfn_nt_query_virtual_memory SUCCESS");

		if (!reinterpret_cast<void*>(utils::internal_functions::pfn_nt_read_virtual_memory))
		{
			LogError("pfn_nt_read_virtual_memory is not found");
			status = STATUS_NOT_FOUND;
			goto clear;
		}

		if (!utils::hook_utils::hook_kernel_function(
			reinterpret_cast<void*>(utils::internal_functions::pfn_nt_read_virtual_memory),
			hook_nt_read_virtual_memory,
			reinterpret_cast<void**>(&original_nt_read_virtual_memory)
		))
		{
			LogError("Failed to hook pfn_nt_read_virtual_memory");
			status = STATUS_UNSUCCESSFUL;
			goto clear;
		}
		LogInfo("Hook pfn_nt_read_virtual_memory SUCCESS");

		clear:
			return status;

		}

		static bool is_address_hidden(PVOID virtual_address, HANDLE process_id)
		{
			if (utils::hidden_modules::is_address_hidden(virtual_address))
			{
				return true;
			}

			if (utils::hidden_user_memory::is_pid_hidden(process_id))
			{
				if (utils::hidden_user_memory::is_address_hidden_for_pid(process_id, reinterpret_cast<unsigned long long>(virtual_address)))
				{
					return true;
				}
			}

			return false;
		}

		NTSTATUS __fastcall hook_mm_copy_memory(
			_In_ PVOID target_address,
			_In_ MM_COPY_ADDRESS source_address,
			_In_ SIZE_T number_of_bytes,
			_In_ ULONG flags,
			_Out_ PSIZE_T number_of_bytes_transferred
		)
		{





			PVOID64 virtual_address = nullptr;
			HANDLE process_id = utils::internal_functions::pfn_ps_get_current_process_id();
			if (flags == MM_COPY_MEMORY_PHYSICAL)
			{
				virtual_address = utils::internal_functions::pfn_mm_get_virtual_for_physical(source_address.PhysicalAddress);
			}
			else
			{
				virtual_address = source_address.VirtualAddress;
			}


			if (is_address_hidden(virtual_address, process_id))
			{
				*number_of_bytes_transferred = 0;
				return STATUS_CONFLICTING_ADDRESSES;
			}



			return original_mm_copy_memory(
				target_address,
				source_address,
				number_of_bytes,
				flags,
				number_of_bytes_transferred
			);
		}



		BOOLEAN  __fastcall hook_mm_is_address_valid(
			_In_ PVOID VirtualAddress
		)
		{

			HANDLE process_id = utils::internal_functions::pfn_ps_get_current_process_id();


			if (is_address_hidden(VirtualAddress, process_id))
			{
				return FALSE;
			}


			return   original_mm_is_address_valid(VirtualAddress);
		}


		ULONG NTAPI hook_rtl_walk_frame_chain(
			_Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
			_In_ ULONG count,
			_In_ ULONG flags
		)
		{
			// Validate input parameters
			if (!callers || count == 0 )
			{
				 
				return 0;
			}

			// Check if callers buffer is valid
			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(callers))
			{
				 
				return 0;
			}

			HANDLE process_id = utils::internal_functions::pfn_ps_get_current_process_id();

			// Call original function
			ULONG frames_captured = original_rtl_walk_frame_chain(callers, count, flags);

		 

			// Process captured frames safely
			for (ULONG i = 0; i < frames_captured; ++i)
			{
				// Check if we can safely access callers[i]
				PVOID* caller_ptr = &callers[i];
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(caller_ptr))
				{
					 
					continue;
				}

				PVOID addr = callers[i];

				// Check if the address is valid before processing
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(addr))
				{
					continue;
				}

				if (is_address_hidden(addr, process_id))
				{
					// Remove hidden address by shifting remaining frames
					for (ULONG j = i + 1; j < frames_captured; ++j)
					{
						// Check if we can safely access callers[j]
						PVOID* src_ptr = &callers[j];
						PVOID* dst_ptr = &callers[j - 1];
						
						if (utils::internal_functions::pfn_mm_is_address_valid_ex(src_ptr) &&
							utils::internal_functions::pfn_mm_is_address_valid_ex(dst_ptr))
						{
							callers[j - 1] = callers[j];
						}
						else
						{
							 
							break;
						}
					}

					frames_captured--;
					i--;
				}


			}

			return frames_captured;
		}


		PRUNTIME_FUNCTION NTAPI hook_rtl_lookup_function_entry(
			_In_ DWORD64 control_pc,
			_Out_ PDWORD64 image_base,
			_Inout_opt_ PUNWIND_HISTORY_TABLE history_table
		)
		{


			HANDLE process_id = utils::internal_functions::pfn_ps_get_current_process_id();

 
			if (is_address_hidden(reinterpret_cast<PVOID>(control_pc), process_id))
			{
				if (image_base)
				{
					*image_base = 0;
				}
				return nullptr;
			}

			 


			return original_rtl_lookup_function_entry(control_pc, image_base, history_table);
		}




 
		NTSTATUS NTAPI  hook_nt_query_virtual_memory(
			_In_ HANDLE ProcessHandle,
			_In_opt_ PVOID BaseAddress,
			_In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
			_Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
			_In_ SIZE_T MemoryInformationLength,
			_Out_opt_ PSIZE_T ReturnLength
		)
		{
 
			if (MemoryInformationClass != MemoryBasicInformation &&
				MemoryInformationClass != 2)
			{
				return original_nt_query_virtual_memory(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
			}
 
			PROCESS_BASIC_INFORMATION ProcessBasicInfo;
			NTSTATUS QueryStatus = utils::internal_functions::pfn_zw_query_information_process(
				ProcessHandle,
				ProcessBasicInformation,
				(PVOID)&ProcessBasicInfo,
				sizeof(ProcessBasicInfo),
				NULL
			);
 
			if (!NT_SUCCESS(QueryStatus)) {
 
				return original_nt_query_virtual_memory(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
			}
 
 
			if (!utils::hidden_user_memory::is_address_hidden_for_pid((HANDLE)ProcessBasicInfo.UniqueProcessId, reinterpret_cast<unsigned long long> (BaseAddress))) {
				return original_nt_query_virtual_memory(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
			}
 
 
 
			NTSTATUS  Status = original_nt_query_virtual_memory(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
			if (!NT_SUCCESS(Status))
			{
				return Status;
			}
			PMEMORY_BASIC_INFORMATION pMBI = static_cast<PMEMORY_BASIC_INFORMATION>(
				MemoryInformation);
 
			PVOID NextBase = reinterpret_cast<PUCHAR>(pMBI->BaseAddress) +
				pMBI->RegionSize;
 
			MEMORY_BASIC_INFORMATION NextBlock;
			SIZE_T NextLength = 0;
			Status = original_nt_query_virtual_memory(ProcessHandle, NextBase,
				MemoryInformationClass, &NextBlock, sizeof(NextBlock), &NextLength);
 
 
			pMBI->AllocationBase = 0;
			pMBI->AllocationProtect = 0;
			pMBI->State = MEM_FREE;
			pMBI->Protect = PAGE_NOACCESS;
			pMBI->Type = 0;
 
			// If next block is free too, add it to the region size
			if (NextBlock.State == MEM_FREE)
				pMBI->RegionSize += NextBlock.RegionSize;
 
 
			// Clear the output buffer if the address is hidden
 
 
			return Status;
 
 
		}
 

 
 
		NTSTATUS NTAPI hook_nt_read_virtual_memory(
			_In_ HANDLE ProcessHandle,
			_In_opt_ PVOID BaseAddress,
			_Out_writes_bytes_(NumberOfBytesToRead) PVOID Buffer,
			_In_ SIZE_T NumberOfBytesToRead,
			_Out_opt_ PSIZE_T NumberOfBytesRead
		)
		{
			PROCESS_BASIC_INFORMATION ProcessBasicInfo;
			NTSTATUS QueryStatus = utils::internal_functions::pfn_zw_query_information_process(
				ProcessHandle,
				ProcessBasicInformation,
				(PVOID)&ProcessBasicInfo,
				sizeof(ProcessBasicInfo),
				NULL
			);
 
			if (!NT_SUCCESS(QueryStatus)) {
 
				return original_nt_read_virtual_memory(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToRead, NumberOfBytesRead);
			}
 
			if (utils::hidden_user_memory::is_address_hidden_for_pid((HANDLE)ProcessBasicInfo.UniqueProcessId, reinterpret_cast<unsigned long long> (BaseAddress))) {
 
 
				return STATUS_ACCESS_VIOLATION;
			}
 
 
			return original_nt_read_virtual_memory(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToRead, NumberOfBytesRead);
		}




	}

}