#pragma once
namespace utils
{
	namespace  hook_user_comm
	{
		NTSTATUS initialize_hook_user_comm();	


		NTSTATUS NTAPI hook_nt_write_virtual_memory(
			HANDLE ProcessHandle,
			PVOID BaseAddress,
			PVOID Buffer,
			SIZE_T NumberOfBytesToWrite,
			PSIZE_T NumberOfBytesWritten
		);

	}
}