#pragma once
 
namespace hook_functions
{

    NTSTATUS NTAPI hook_nt_open_process(
        _Out_ PHANDLE ProcessHandle,
        _In_ ACCESS_MASK DesiredAccess,
        _In_ POBJECT_ATTRIBUTES ObjectAttributes,
        _In_opt_ PCLIENT_ID ClientId
    );

    extern NTSTATUS(NTAPI* original_nt_open_process)(
        _Out_ PHANDLE ProcessHandle,
        _In_ ACCESS_MASK DesiredAccess,
        _In_ POBJECT_ATTRIBUTES ObjectAttributes,
        _In_opt_ PCLIENT_ID ClientId
        );

    /**
     * @brief Hook for KiPreprocessFault.
     */
    BOOLEAN __fastcall hook_ki_preprocess_fault(
        _Inout_ PEXCEPTION_RECORD ExceptionRecord,
        _Inout_ PCONTEXT ContextRecord,
        _In_ KPROCESSOR_MODE PreviousMode
    );

    extern BOOLEAN(__fastcall* original_ki_preprocess_fault)(
        _Inout_ PEXCEPTION_RECORD ExceptionRecord,
        _Inout_ PCONTEXT ContextRecord,
        _In_ KPROCESSOR_MODE PreviousMode
        );

    /**
     * @brief Hook for MmIsAddressValid.
     */
    BOOLEAN __fastcall hook_mm_is_address_valid(
        _In_ PVOID VirtualAddress
    );

    extern BOOLEAN(__fastcall* original_mm_is_address_valid)(
        _In_ PVOID VirtualAddress
        );

    /**
     * @brief Hook for MmCopyMemory.
     */
    NTSTATUS __fastcall hook_mm_copy_memory(
        _In_ PVOID TargetAddress,
        _In_ MM_COPY_ADDRESS SourceAddress,
        _In_ SIZE_T NumberOfBytes,
        _In_ ULONG Flags,
        _Out_ PSIZE_T NumberOfBytesTransferred
    );

    extern NTSTATUS(__fastcall* original_mm_copy_memory)(
        _In_ PVOID TargetAddress,
        _In_ MM_COPY_ADDRESS SourceAddress,
        _In_ SIZE_T NumberOfBytes,
        _In_ ULONG Flags,
        _Out_ PSIZE_T NumberOfBytesTransferred
        );


	ULONG __fastcall hook_rtl_walk_frame_chain(
		_Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
		_In_ ULONG count,
		_In_ ULONG flags
	);

	extern ULONG(__fastcall* original_rtl_walk_frame_chain)(
		_Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
		_In_ ULONG count,
		_In_ ULONG flags
		);

    /**
     * @brief Hook for RtlLookupFunctionEntry.
     */
 //   PRUNTIME_FUNCTION NTAPI hook_rtl_lookup_function_entry(
 //       _In_ DWORD64 control_pc,
 //       _Out_ PDWORD64 image_base,
 //       _Inout_opt_ PUNWIND_HISTORY_TABLE history_table
 //   );

	//extern	PRUNTIME_FUNCTION(NTAPI* original_rtl_lookup_function_entry)(
	//		_In_ DWORD64 control_pc,
	//		_Out_ PDWORD64 image_base,
	//		_Inout_opt_ PUNWIND_HISTORY_TABLE history_table
	//		);
}
