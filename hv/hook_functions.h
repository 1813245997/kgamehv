#pragma once

namespace hook_functions
{
    /**
 * @brief Hooks the NtOpenProcess function.
 *
 * This function hooks the NtOpenProcess function and intercepts its calls.
 *
 * @param ProcessHandle Pointer to a handle that receives the opened process object handle.
 * @param DesiredAccess Specifies the access rights for the handle to the process object.
 * @param ObjectAttributes Pointer to the object attributes structure.
 * @param ClientId Pointer to the client identifier structure.
 * @return PVOID pointing to the allocated memory, or nullptr if allocation fails.
 */
       NTSTATUS NTAPI Hook_NtOpenProcess(
           _Out_ PHANDLE ProcessHandle,
        _In_ ACCESS_MASK DesiredAccess,
        _In_ POBJECT_ATTRIBUTES ObjectAttributes,
        _In_opt_ PCLIENT_ID ClientId);


   extern 	NTSTATUS(NTAPI* OriginalNtOpenProcess)(
       _Out_ PHANDLE ProcessHandle,
        _In_ ACCESS_MASK DesiredAccess,
        _In_ POBJECT_ATTRIBUTES ObjectAttributes,
        _In_opt_ PCLIENT_ID ClientId);


   extern 	BOOLEAN(__fastcall* OriginalKiPreprocessFault)(
       IN OUT PEXCEPTION_RECORD ExceptionRecord,
	   IN OUT PCONTEXT ContextRecord,
	   IN KPROCESSOR_MODE PreviousMode);

   BOOLEAN __fastcall  Hook_KiPreprocessFault(
       IN OUT PEXCEPTION_RECORD ExceptionRecord,
       IN OUT PCONTEXT ContextRecord,
       IN KPROCESSOR_MODE PreviousMode);

}