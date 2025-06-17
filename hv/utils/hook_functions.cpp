
#include "global_defs.h"

#include "hook_functions.h"
#include "dwm_draw.h"

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

		if (PreviousMode == MODE::UserMode)
		{
			if (!find_hook_break_point_int3(ExceptionRecord->ExceptionAddress, &matched_hook_info))
			{
				return original_ki_preprocess_fault(ExceptionRecord, ContextRecord, PreviousMode);
			}

			using handler_fn_t =BOOLEAN(__fastcall*)(PEXCEPTION_RECORD, PCONTEXT, hyper::EptHookInfo*);
			auto handler = reinterpret_cast<handler_fn_t>(matched_hook_info->handler_va);

			if (handler(ExceptionRecord, ContextRecord, matched_hook_info))
			{
				return TRUE;  // 已处理
			}
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

		
	   if (utils::hidden_modules::is_address_hidden(VirtualAddress))
	   {
		   return FALSE;
	   }


		return utils::internal_functions::pfn_mm_is_address_valid_ex (VirtualAddress);
	}


	  NTSTATUS(__fastcall* original_mm_copy_memory)(
		  _In_ PVOID target_address,
		  _In_ MM_COPY_ADDRESS source_address,
		  _In_ SIZE_T number_of_bytes,
		  _In_ ULONG flags,
		  _Out_ PSIZE_T number_of_bytes_transferred
		);


	  NTSTATUS __fastcall hook_mm_copy_memory(
		  _In_ PVOID target_address,
		  _In_ MM_COPY_ADDRESS source_address,
		  _In_ SIZE_T number_of_bytes,
		  _In_ ULONG flags,
		  _Out_ PSIZE_T number_of_bytes_transferred
	  )
	  {
		  
		  if (target_address == nullptr )
		  {
			  return original_mm_copy_memory(
				  target_address,
				  source_address,
				  number_of_bytes,
				  flags,
				  number_of_bytes_transferred
			  );
		  }

		  if (number_of_bytes == 0)
		  {
			  return original_mm_copy_memory(
				  target_address,
				  source_address,
				  number_of_bytes,
				  flags,
				  number_of_bytes_transferred
			  );
		  }

		  PVOID64 virtual_address = nullptr;

		  if (flags == MM_COPY_MEMORY_PHYSICAL)
		  {
			  virtual_address = utils::internal_functions::pfn_mm_get_virtual_for_physical (source_address.PhysicalAddress);
		  }
		  else
		  {
			  virtual_address = source_address.VirtualAddress;
		  }

		  
		  if (utils::hidden_modules::is_address_hidden(virtual_address))
		  {
			  *number_of_bytes_transferred = 0;
			  return STATUS_CONFLICTING_ADDRESSES;
		  }

		 
		  return original_mm_copy_memory(
			  target_address,
			  source_address,
			  number_of_bytes,
			  flags,
			  number_of_bytes_transferred
		  );
	  }

	  

	 ULONG(NTAPI* original_rtl_walk_frame_chain)(
		 _Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
		 _In_ ULONG count,
		 _In_ ULONG flags
		 );

	 ULONG NTAPI hook_rtl_walk_frame_chain(
		 _Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
		 _In_ ULONG count,
		 _In_ ULONG flags
	 )
	 {
		 ULONG frames_captured = original_rtl_walk_frame_chain(callers, count, flags);

		 if (frames_captured == 0)
		 {
			 return frames_captured;
		 }

		 if (!callers)
		 {
			 return frames_captured;
		 }


		 if (!utils::internal_functions::pfn_mm_is_address_valid_ex(callers))
		 {
			 return frames_captured;
		 }

		 for (ULONG i = 0; i < frames_captured; ++i)
		 {
			 

			 if (!utils::internal_functions::pfn_mm_is_address_valid_ex(callers[i]))
				 continue;

			 PVOID addr = callers[i];
			 if (utils::hidden_modules::is_address_hidden(addr))
			 {
				 callers[i] = nullptr;
				 return i;
			 }
		 }

		 return frames_captured;
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


	   void(__fastcall* original_psp_exit_process)(
		 IN BOOLEAN trim_address_space,
		 IN PEPROCESS process
		 );

	   void __fastcall hook_psp_exit_process(
		   IN BOOLEAN trim_address_space,
		   IN PEPROCESS process
	   )
	   {



		   return original_psp_exit_process(trim_address_space, process);

	 }


	   NTSTATUS(NTAPI* original_nt_create_section)(
		   _Out_ PHANDLE section_handle,
		   _In_ ACCESS_MASK desired_access,
		   _In_opt_ POBJECT_ATTRIBUTES object_attributes,
		   _In_opt_ PLARGE_INTEGER maximum_size,
		   _In_ ULONG section_page_protection,
		   _In_ ULONG allocation_attributes,
		   _In_opt_ HANDLE file_handle
		   );


	   NTSTATUS NTAPI hook_nt_create_section(
		   _Out_ PHANDLE section_handle,
		   _In_ ACCESS_MASK desired_access,
		   _In_opt_ POBJECT_ATTRIBUTES object_attributes,
		   _In_opt_ PLARGE_INTEGER maximum_size,
		   _In_ ULONG section_page_protection,
		   _In_ ULONG allocation_attributes,
		   _In_opt_ HANDLE file_handle
	   )
	   {
		   return original_nt_create_section(
			   section_handle,
			   desired_access,
			   object_attributes,
			   maximum_size,
			   section_page_protection,
			   allocation_attributes,
			   file_handle
		   );
	   }


	   INT64(__fastcall* original_present_multiplane_overlay)(
		   void* thisptr,
		   PVOID dxgi_swap_chain,
		   unsigned int a3,
		   unsigned int a4,
		   int a5,
		   const void* a6,
		   PVOID64 a7,
		   unsigned int a8
		   ) = nullptr;

	   BOOLEAN  __fastcall new_present_multiplane_overlay(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ hyper::EptHookInfo* matched_hook_info)
	   {

		   if (!utils:: dwm_draw::g_pswap_chain)
		   {
			   utils::dwm_draw::g_pswap_chain = ContextRecord->Rdx;
		   }

		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);

		   return TRUE;
	   }


	      INT64(__fastcall* original_cdxgi_swap_chain_dwm_legacy_present_dwm)(
		   void* pthis,
		   PVOID pDxgiSwapChain,
		   unsigned int a3,
		   unsigned int a4,
		   const PVOID a5, unsigned int a6, PVOID64 a7, unsigned int a8,
		   PVOID a9, unsigned int a10) = nullptr;


	   BOOLEAN  __fastcall new_cdxgi_swap_chain_dwm_legacy_present_dwm(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ hyper::EptHookInfo* matched_hook_info)
	   {
		   if (!utils::dwm_draw::g_pswap_chain)
		   {
			   utils::dwm_draw::g_pswap_chain = ContextRecord->Rdx;
		   }
		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);

		   return TRUE;
	   }

	   __int64(__fastcall* original_dxgk_get_device_state)(
		   _Inout_ PVOID unnamedParam1)
		   = nullptr;


	   __int64 __fastcall hook_dxgk_get_device_state(_Inout_ PVOID unnamedParam1)
	   {
		  
			if (utils::internal_functions:: pfn_ke_get_current_irql() >= DISPATCH_LEVEL)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (utils::internal_functions::pfn_ex_get_previous_mode() != MODE::UserMode)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (utils::dwm_draw::g_dwm_process==nullptr)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (utils::internal_functions::pfn_ps_get_current_process()!=utils :: dwm_draw::g_dwm_process)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}
			//utils::dwm_draw::g_dwm_render_thread= utils::internal_functions::
	/*		if ()
			{
			}*/

			 


		   return  original_dxgk_get_device_state(unnamedParam1);
	   }
}
