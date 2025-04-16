#include "hv.h"
#include "hook_functions.h"

namespace hook_functions
{

	NTSTATUS(NTAPI* OriginalNtOpenProcess)(_Out_ PHANDLE ProcessHandle,
		_In_ ACCESS_MASK DesiredAccess,
		_In_ POBJECT_ATTRIBUTES ObjectAttributes,
		_In_opt_ PCLIENT_ID ClientId);
	NTSTATUS  Hook_NtOpenProcess(_Out_ PHANDLE ProcessHandle, _In_ ACCESS_MASK DesiredAccess, _In_ POBJECT_ATTRIBUTES ObjectAttributes, _In_opt_ PCLIENT_ID ClientId)
	{

		////DbgBreakPoint();
		//if (ClientId != NULL)
		//{

		//	DbgPrintEx(77, 0, "Opening process with PID: %u\n", HandleToUlong(ClientId->UniqueProcess));
		//}
		//else
		//{
		//	DbgPrintEx(77, 0, "ClientId is NULL, cannot retrieve PID.\n");
		//}

		return OriginalNtOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
	}

 	BOOLEAN(__fastcall* OriginalKiPreprocessFault)(
		IN OUT PEXCEPTION_RECORD ExceptionRecord,
		IN OUT PCONTEXT ContextRecord,
		IN KPROCESSOR_MODE PreviousMode);

	BOOLEAN __fastcall Hook_KiPreprocessFault(IN OUT PEXCEPTION_RECORD ExceptionRecord, IN OUT PCONTEXT ContextRecord, IN KPROCESSOR_MODE PreviousMode)
	{

	 
		return OriginalKiPreprocessFault(ExceptionRecord,ContextRecord,PreviousMode);
	}

}
