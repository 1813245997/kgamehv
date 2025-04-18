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
		_In_ PVOID target_address,
		_In_ MM_COPY_ADDRESS source_address,
		_In_ SIZE_T number_of_bytes,
		_In_ ULONG flags,
		_Out_ PSIZE_T number_of_bytes_transferred
    );

    extern NTSTATUS(__fastcall* original_mm_copy_memory)(
		_In_ PVOID target_address,
		_In_ MM_COPY_ADDRESS source_address,
		_In_ SIZE_T number_of_bytes,
		_In_ ULONG flags,
		_Out_ PSIZE_T number_of_bytes_transferred
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
	PRUNTIME_FUNCTION NTAPI hook_rtl_lookup_function_entry(
		_In_ DWORD64 control_pc,
		_Out_ PDWORD64 image_base,
		_Inout_opt_ PUNWIND_HISTORY_TABLE history_table
	);

	extern	PRUNTIME_FUNCTION(NTAPI* original_rtl_lookup_function_entry)(
		_In_ DWORD64 control_pc,
		_Out_ PDWORD64 image_base,
		_Inout_opt_ PUNWIND_HISTORY_TABLE history_table
		);


	void __fastcall hook_psp_exit_process(
		IN BOOLEAN trim_address_space,
		IN PEPROCESS process
	);

	extern void(__fastcall* original_psp_exit_process)(
		IN BOOLEAN trim_address_space,
		IN PEPROCESS process
		);

	NTSTATUS NTAPI hook_nt_create_section(
		_Out_ PHANDLE section_handle,
		_In_ ACCESS_MASK desired_access,
		_In_opt_ POBJECT_ATTRIBUTES object_attributes,
		_In_opt_ PLARGE_INTEGER maximum_size,
		_In_ ULONG section_page_protection,
		_In_ ULONG allocation_attributes,
		_In_opt_ HANDLE file_handle
	);

	extern NTSTATUS(NTAPI* original_nt_create_section)(
		_Out_ PHANDLE section_handle,
		_In_ ACCESS_MASK desired_access,
		_In_opt_ POBJECT_ATTRIBUTES object_attributes,
		_In_opt_ PLARGE_INTEGER maximum_size,
		_In_ ULONG section_page_protection,
		_In_ ULONG allocation_attributes,
		_In_opt_ HANDLE file_handle
		);


}
