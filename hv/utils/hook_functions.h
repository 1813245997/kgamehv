#pragma once

namespace hook_functions
{

    
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


	ULONG NTAPI hook_rtl_walk_frame_chain(
		_Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
		_In_ ULONG count,
		_In_ ULONG flags
	);

	extern ULONG(NTAPI* original_rtl_walk_frame_chain)(
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

	__int64 __fastcall hook_dxgk_get_device_state(_Inout_ PVOID unnamedParam1);

	extern __int64(__fastcall* original_dxgk_get_device_state)(_Inout_ PVOID unnamedParam1);


	BOOLEAN __fastcall new_present_dwm(
		_Inout_ PEXCEPTION_RECORD ExceptionRecord,
		_Inout_ PCONTEXT ContextRecord,
		_Inout_ hyper::EptHookInfo* matched_hook_info);


	BOOLEAN  __fastcall new_present_multiplane_overlay(
		_Inout_ PEXCEPTION_RECORD ExceptionRecord,
		_Inout_ PCONTEXT ContextRecord,
		_Inout_ hyper::EptHookInfo* matched_hook_info);

	extern  INT64(__fastcall* original_present_multiplane_overlay)(
		void* thisptr,
		PVOID dxgi_swap_chain,
		unsigned int a3,
		unsigned int a4,
		int a5,
		const void* a6,
		PVOID64 a7,
		unsigned int a8
		);

	BOOLEAN  __fastcall new_cdxgi_swap_chain_dwm_legacy_present_dwm(
		_Inout_ PEXCEPTION_RECORD ExceptionRecord,
		_Inout_ PCONTEXT ContextRecord,
		_Inout_ hyper::EptHookInfo* matched_hook_info);

	extern  INT64(__fastcall* original_cdxgi_swap_chain_dwm_legacy_present_dwm)(
		void* pthis,
		PVOID pDxgiSwapChain,
		unsigned int a3,
		unsigned int a4,
		const PVOID a5, unsigned int a6, PVOID64 a7, unsigned int a8,
		PVOID a9, unsigned int a10);


	BOOLEAN __fastcall  new_get_buffer(
		_Inout_ PEXCEPTION_RECORD ExceptionRecord,
		_Inout_ PCONTEXT ContextRecord,
		_Inout_ hyper::EptHookInfo* matched_hook_info);



	  NTSTATUS NTAPI  new_nt_query_virtual_memory (
		_In_ HANDLE ProcessHandle,
		_In_opt_ PVOID BaseAddress,
		_In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
		_Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
		_In_ SIZE_T MemoryInformationLength,
		_Out_opt_ PSIZE_T ReturnLength
		);
	   

	  extern   NTSTATUS(NTAPI* original_nt_query_virtual_memory) (
		_In_ HANDLE ProcessHandle,
		_In_opt_ PVOID BaseAddress,
		_In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
		_Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
		_In_ SIZE_T MemoryInformationLength,
		_Out_opt_ PSIZE_T ReturnLength
		);


	  NTSTATUS NTAPI new_nt_read_virtual_memory (
		  _In_ HANDLE ProcessHandle,
		  _In_opt_ PVOID BaseAddress,
		  _Out_writes_bytes_(NumberOfBytesToRead) PVOID Buffer,
		  _In_ SIZE_T NumberOfBytesToRead,
		  _Out_opt_ PSIZE_T NumberOfBytesRead
		  );


	  extern NTSTATUS(NTAPI* original_nt_read_virtual_memory)(
		  _In_ HANDLE ProcessHandle,
		  _In_opt_ PVOID BaseAddress,
		  _Out_writes_bytes_(NumberOfBytesToRead) PVOID Buffer,
		  _In_ SIZE_T NumberOfBytesToRead,
		  _Out_opt_ PSIZE_T NumberOfBytesRead
	  );

	  NTSTATUS NTAPI  new_nt_protect_virtual_memory (
		  _In_ HANDLE ProcessHandle,
		  _Inout_ PVOID* BaseAddress,
		  _Inout_ PSIZE_T NumberOfBytesToProtect,
		  _In_ ULONG NewAccessProtection,
		  _Out_ PULONG OldAccessProtection
		  );

	  extern  NTSTATUS(NTAPI* original_nt_protect_virtual_memory)(
		  _In_ HANDLE ProcessHandle,
		  _Inout_ PVOID* BaseAddress,
		  _Inout_ PSIZE_T NumberOfBytesToProtect,
		  _In_ ULONG NewAccessProtection,
		  _Out_ PULONG OldAccessProtection
		  );
}
