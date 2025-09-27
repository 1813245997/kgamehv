#pragma once
namespace utils
{
	namespace hook_mem_protecion
	{
		NTSTATUS initialize_mem_protections();

		NTSTATUS __fastcall hook_mm_copy_memory(
			_In_ PVOID target_address,
			_In_ MM_COPY_ADDRESS source_address,
			_In_ SIZE_T number_of_bytes,
			_In_ ULONG flags,
			_Out_ PSIZE_T number_of_bytes_transferred
		);

		BOOLEAN  __fastcall hook_mm_is_address_valid(
			_In_ PVOID VirtualAddress
		);

		ULONG NTAPI hook_rtl_walk_frame_chain(
			_Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
			_In_ ULONG count,
			_In_ ULONG flags
		);

		PRUNTIME_FUNCTION NTAPI hook_rtl_lookup_function_entry(
			_In_ DWORD64 control_pc,
			_Out_ PDWORD64 image_base,
			_Inout_opt_ PUNWIND_HISTORY_TABLE history_table
		);

		NTSTATUS NTAPI  hook_nt_query_virtual_memory(
			_In_ HANDLE ProcessHandle,
			_In_opt_ PVOID BaseAddress,
			_In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
			_Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
			_In_ SIZE_T MemoryInformationLength,
			_Out_opt_ PSIZE_T ReturnLength
		);

		NTSTATUS NTAPI  hook_nt_read_virtual_memory(
			_In_ HANDLE ProcessHandle,
			_In_opt_ PVOID BaseAddress,
			_Out_writes_bytes_(NumberOfBytesToRead) PVOID Buffer,
			_In_ SIZE_T NumberOfBytesToRead,
			_Out_opt_ PSIZE_T NumberOfBytesRead
		);
    }
}