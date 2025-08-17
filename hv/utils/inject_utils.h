#pragma once
namespace utils
{
	namespace inject_utils
	{
		NTSTATUS hijack_thread_inject_dll_x64(HANDLE process_id, PVOID dll_shellcode, ULONG dll_size);

		NTSTATUS remote_thread_inject_x64_dll(HANDLE process_id, PVOID dll_shellcode, ULONG dll_size);

		VOID ex_free_memory_work_item(_In_ PVOID Parameter);
	 }
}