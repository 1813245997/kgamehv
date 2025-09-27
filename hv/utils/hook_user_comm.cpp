#include "global_defs.h"
#include "hook_user_comm.h"
namespace utils
{
	namespace  hook_user_comm
	{
        
		NTSTATUS(NTAPI* original_nt_write_virtual_memory)(
			HANDLE ProcessHandle,
			PVOID BaseAddress,
			PVOID Buffer,
			SIZE_T NumberOfBytesToWrite,
			PSIZE_T NumberOfBytesWritten
			) = nullptr;


		NTSTATUS initialize_hook_user_comm()
		{
			NTSTATUS status = STATUS_SUCCESS;
			
			if (!reinterpret_cast<void*>(utils::internal_functions::pfn_nt_write_virtual_memory))
			{
				LogError("pfn_nt_write_virtual_memory is not found");
				status = STATUS_NOT_FOUND;
				goto clear;
			}
			
			if (!utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(utils::internal_functions::pfn_nt_write_virtual_memory),
				hook_nt_write_virtual_memory,
				reinterpret_cast<void**>(&original_nt_write_virtual_memory)
			))
			{
				LogError("Failed to hook pfn_nt_write_virtual_memory");
				status = STATUS_UNSUCCESSFUL;
				goto clear;
			}
			LogInfo("Hook pfn_nt_write_virtual_memory SUCCESS");

		 clear:
			return status;
		}



 
		NTSTATUS NTAPI hook_nt_write_virtual_memory(
			HANDLE ProcessHandle,
			PVOID BaseAddress,
			PVOID Buffer,
			SIZE_T NumberOfBytesToWrite,
			PSIZE_T NumberOfBytesWritten
		)
		{
			if (!Buffer)
			{
 
				return original_nt_write_virtual_memory(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten);
			}
 
		   
 
			if (!utils::user_comm::is_valid_comm(reinterpret_cast<user_comm_request *>(Buffer)))
			{
				return original_nt_write_virtual_memory(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten);
			}
 
			if (NumberOfBytesToWrite != sizeof(user_comm_request))
			{
				return original_nt_write_virtual_memory(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten);
			}
 
 
 
 
			utils::user_comm::handle_user_comm_request(reinterpret_cast<user_comm_request*>(Buffer));
			return STATUS_SUCCESS;
 
 
			 
 
		}
	}
}