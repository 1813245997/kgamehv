
#include "global_defs.h"
#include "hook_functions.h"


namespace hook_functions
{

	NTSTATUS(NTAPI* original_nt_open_process)(_Out_ PHANDLE ProcessHandle,
		_In_ ACCESS_MASK DesiredAccess,
		_In_ POBJECT_ATTRIBUTES ObjectAttributes,
		_In_opt_ PCLIENT_ID ClientId);
	NTSTATUS  hook_nt_open_process(_Out_ PHANDLE ProcessHandle, _In_ ACCESS_MASK DesiredAccess, _In_ POBJECT_ATTRIBUTES ObjectAttributes, _In_opt_ PCLIENT_ID ClientId)
	{

		////DbgBreakPoint();
		if (ClientId != NULL)
		{

			DbgPrintEx(77, 0, "Opening process with PID: %u\n", HandleToUlong(ClientId->UniqueProcess));
		}
		else
		{
			DbgPrintEx(77, 0, "ClientId is NULL, cannot retrieve PID.\n");
		}

		return original_nt_open_process(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
	}



 	BOOLEAN(__fastcall* original_ki_preprocess_fault)(
		IN OUT PEXCEPTION_RECORD ExceptionRecord,
		IN OUT PCONTEXT ContextRecord,
		IN KPROCESSOR_MODE PreviousMode);

	BOOLEAN __fastcall hook_ki_preprocess_fault(
		_Inout_ PEXCEPTION_RECORD ExceptionRecord,
		_Inout_ PCONTEXT ContextRecord,
		_In_ KPROCESSOR_MODE PreviousMode)
	{
		//不可以dprinftf 输出 因为是触发异常输出的
		hyper::EptHookInfo* matched_hook_info = nullptr;
		 
		if (ExceptionRecord->ExceptionCode != STATUS_BREAKPOINT)
		{
			return original_ki_preprocess_fault(ExceptionRecord, ContextRecord, PreviousMode);
		}

		if (!find_hook_info_by_rip(&g_UsermodeHookPageListHead,  ExceptionRecord->ExceptionAddress , &matched_hook_info))
		{
			return original_ki_preprocess_fault(ExceptionRecord, ContextRecord, PreviousMode);
		}

		using handler_fn_t = bool(*)(PEXCEPTION_RECORD, PCONTEXT);
		auto handler = reinterpret_cast<handler_fn_t>(matched_hook_info->handler_va);

		if (handler(ExceptionRecord, ContextRecord))
		{
			return TRUE;  // 已处理
		}

		return original_ki_preprocess_fault(ExceptionRecord, ContextRecord, PreviousMode);
		 
		 
	}


	 BOOLEAN(__fastcall* original_mm_is_address_valid)(
		_In_ PVOID VirtualAddress
		);

	BOOLEAN  __fastcall hook_mm_is_address_valid(
		_In_ PVOID VirtualAddress
	)
	{


		return original_mm_is_address_valid(VirtualAddress);
	}


	  NTSTATUS(__fastcall* original_mm_copy_memory)(
		PVOID TargetAddress,
		MM_COPY_ADDRESS SourceAddress,
		SIZE_T NumberOfBytes,
		ULONG Flags,
		PSIZE_T NumberOfBytesTransferred
		);


     NTSTATUS __fastcall hook_mm_copy_memory(
		 _In_ PVOID TargetAddress,
		 _In_ MM_COPY_ADDRESS SourceAddress,
		 _In_ SIZE_T NumberOfBytes,
		 _In_ ULONG Flags,
		 _Out_ PSIZE_T NumberOfBytesTransferred
	)
	{


		 return original_mm_copy_memory(TargetAddress, SourceAddress, NumberOfBytes, Flags, NumberOfBytesTransferred);
	}
	  

	 ULONG(__fastcall* original_rtl_walk_frame_chain)(
		 _Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
		 _In_ ULONG count,
		 _In_ ULONG flags
		 );

	 ULONG __fastcall hook_rtl_walk_frame_chain(
		 _Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
		 _In_ ULONG count,
		 _In_ ULONG flags
	 )
	 {

		 return original_rtl_walk_frame_chain(callers, count, flags);
	 }


	 PRUNTIME_FUNCTION(NTAPI* original_rtl_lookup_function_entry)(
		 _In_ DWORD64 control_pc,
		 _Out_ PDWORD64 image_base,
		 _Inout_opt_ PUNWIND_HISTORY_TABLE history_table
		 );

	 PRUNTIME_FUNCTION NTAPI hook_rtl_lookup_function_entry(
		 _In_ DWORD64 control_pc,
		 _Out_ PDWORD64 image_base,
		 _Inout_opt_ PUNWIND_HISTORY_TABLE history_table
	 )
	 {
		  

		 return original_rtl_lookup_function_entry(control_pc, image_base, history_table);
	 }
}
