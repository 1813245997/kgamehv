#pragma once
#include "hook_utils.h"
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

 
 

  

	  void __fastcall  new_create_process_notify_routine_t (
		_In_ HANDLE ParentId,
		_In_ HANDLE ProcessId,
		_In_ BOOLEAN Create
		);

	extern void(__fastcall* original_create_process_notify_routine_t)(
		_In_ HANDLE ParentId,
		_In_ HANDLE ProcessId,
		_In_ BOOLEAN Create
		);

 

	__int64 __fastcall hook_dxgk_get_device_state(_Inout_ PVOID unnamedParam1);

	extern __int64(__fastcall* original_dxgk_get_device_state)(_Inout_ PVOID unnamedParam1);


	 





 

 



 

	  NTSTATUS NTAPI  new_nt_gdi_ddddi_open_resource(
		  ULONG64 a1,
		  __int64 a2,
		  __int64 a3,
		  __int64 a4,
		  unsigned int a5,
		  __int64 a6,
		  char a7,
		  __int64 a8,
		  __int64 a9,
		  __int64 a10,
		  __int64 a11,
		  __int64 a12,
		  __int64 a13,
		  __int64 a14,
		  __int64 a15
	  );

	  extern NTSTATUS(NTAPI* original_nt_gdi_ddddi_open_resource)(
		  ULONG64 a1,
		  __int64 a2,
		  __int64 a3,
		  __int64 a4,
		  unsigned int a5,
		  __int64 a6,
		  char a7,
		  __int64 a8,
		  __int64 a9,
		  __int64 a10,
		  __int64 a11,
		  __int64 a12,
		  __int64 a13,
		  __int64 a14,
		  __int64 a15
		  );

 


	  extern  VOID(__fastcall* original_load_image_notify_routine)(
		  _In_opt_ PUNICODE_STRING FullImageName,
		  _In_ HANDLE ProcessId,
		  _In_ PIMAGE_INFO ImageInfo
		  );

	  VOID  __fastcall  new_load_image_notify_routine (
		  _In_opt_ PUNICODE_STRING FullImageName,
		  _In_ HANDLE ProcessId,
		  _In_ PIMAGE_INFO ImageInfo
		  );


	  extern    NTSTATUS(__fastcall* original_ace_create_hook_internal)(
		  __int64** a1,
		  void* TargetAddress,
		  void* DetourAddress,
		  void** origin_function);

	  NTSTATUS __fastcall new_ace_create_hook_internal(
		  __int64** a1,
		  void* TargetAddress,
		  void* DetourAddress,
		  void** origin_function);


	  BOOLEAN __fastcall new_present_dwm(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);


	  BOOLEAN  __fastcall new_present_multiplane_overlay(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);

	  BOOLEAN __fastcall new_cocclusion_context_pre_sub_graph(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);

	  BOOLEAN __fastcall new_cocclusion_context_post_sub_graph(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);


	  BOOLEAN  __fastcall new_cdxgi_swap_chain_dwm_legacy_present_dwm(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);


	  BOOLEAN  __fastcall new_cddisplay_render_target_present(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);

	  BOOLEAN __fastcall  new_get_buffer(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);

	  BOOLEAN  __fastcall new_nvfbc_create_ex(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);


	  BOOLEAN  __fastcall new_nvfbc_create(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);


	  BOOLEAN  __fastcall new_get_csgo_hp(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);

	  BOOLEAN  __fastcall new_dxgk_get_device_state(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);

	  BOOLEAN  __fastcall new_get_csgo_hp(
		  _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		  _Inout_ PCONTEXT ContextRecord,
		  _Inout_ kernel_hook_function_info* matched_hook_info);
	  
}
