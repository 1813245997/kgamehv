#include "global_defs.h"
#include "hook_functions.h"

namespace hook_functions
{

 
#define STACK_CAPTURE_SIZE 32


 	BOOLEAN(__fastcall* original_ki_preprocess_fault)(
		IN OUT PEXCEPTION_RECORD ExceptionRecord,
		IN OUT PCONTEXT ContextRecord,
		IN KPROCESSOR_MODE PreviousMode);

	BOOLEAN __fastcall hook_ki_preprocess_fault(
		_Inout_ PEXCEPTION_RECORD ExceptionRecord,
		_Inout_ PCONTEXT ContextRecord,
		_In_ KPROCESSOR_MODE PreviousMode)
	{
		

		BOOLEAN is_succeed = FALSE;

 

		switch (ExceptionRecord->ExceptionCode)
		{

		case   STATUS_GUARD_PAGE_VIOLATION:
		{
			is_succeed = utils::falt_utils::handle_page_fault_exception(ExceptionRecord, ContextRecord, PreviousMode);

			break;
		}
		case STATUS_BREAKPOINT:
		{
			is_succeed = utils::falt_utils::handle_int3_exception(ExceptionRecord, ContextRecord, PreviousMode);
			break;
		}

		case   STATUS_SINGLE_STEP:
		{
			is_succeed = utils::falt_utils::handle_single_step_exception(ExceptionRecord, ContextRecord, PreviousMode);
			break;
		}


		default:
			break;
		}

		if (!is_succeed)
		{
			return original_ki_preprocess_fault(ExceptionRecord, ContextRecord, PreviousMode);
		}
		
	
		return is_succeed;
		
		 

		 
	}
	 
	   void(__fastcall* original_create_process_notify_routine_t)(
		   _In_ HANDLE ParentId,
		   _In_ HANDLE ProcessId,
		   _In_ BOOLEAN Create
		   ) = nullptr;


	   void __fastcall  new_create_process_notify_routine_t(
		   _In_ HANDLE ParentId,
		   _In_ HANDLE ProcessId,
		   _In_ BOOLEAN Create
	   )
	   {
		    
		   if (Create)
		   {
			    
			   utils::process_utils ::add_process_entry(ProcessId);
		   }
		   else
		   {
			   
			   utils::process_utils::remove_process_entry(ProcessId);
		   }



		   return original_create_process_notify_routine_t(ParentId, ProcessId, Create);
	   }

	 
  
  

	  VOID(__fastcall* original_load_image_notify_routine)(
		  _In_opt_ PUNICODE_STRING FullImageName,
		  _In_ HANDLE ProcessId,
		  _In_ PIMAGE_INFO ImageInfo
		  )=nullptr;

	  VOID  __fastcall  new_load_image_notify_routine(
		  _In_opt_ PUNICODE_STRING full_image_name,
		  _In_ HANDLE process_id,
		  _In_ PIMAGE_INFO image_info
	  )

	  {

		 
		  if (process_id != 0)
		  {
			  return original_load_image_notify_routine(full_image_name, process_id, image_info);
		  }
		 // IDA: "48 89 5C 24 10 48 89 6C 24 20 4C 89 44 24 18"
		 // "\x48\x89\x5C\x24\x10\x48\x89\x6C\x24\x20\x4C\x89\x44\x24\x18", "xxxxxxxxxxxxxxx"
		  if (!full_image_name ||!full_image_name->Buffer)
		  {
			  return original_load_image_notify_routine(full_image_name, process_id, image_info);

		  }

		  /*	  if (!wcsstr(full_image_name->Buffer, L"ACE-BASE.sys"))
				{
					return original_load_image_notify_routine(full_image_name, process_id, image_info);
				}

				unsigned  long long ace_hook_fun = utils::signature_scanner::find_pattern_image(
					reinterpret_cast<unsigned  long long> (image_info->ImageBase),
					"\x48\x89\x5C\x24\x10\x48\x89\x6C\x24\x20\x4C\x89\x44\x24\x18",
					"xxxxxxxxxxxxxxx"
					".text"
				);

				if (!ace_hook_fun)
				{
					LogError("ACE-BASE.sys loaded but target function not found, bugcheck now!");
					KeBugCheckEx(MANUALLY_INITIATED_CRASH, 0, 0, 0, 0);
				}

				utils::hook_utils::hook_kernel_function(reinterpret_cast<void*>(ace_hook_fun),
					hook_functions::new_ace_create_hook_internal,
					reinterpret_cast<void**>(&hook_functions::original_ace_create_hook_internal));*/


		  return original_load_image_notify_routine(full_image_name, process_id, image_info);

	  }

	  NTSTATUS(__fastcall* original_ace_create_hook_internal)(
		  __int64** a1,
		  void* TargetAddress,
		  void* DetourAddress,
		  void** origin_function) = nullptr;

	  NTSTATUS __fastcall new_ace_create_hook_internal(
		  __int64** a1,
		  void* TargetAddress,
		  void* DetourAddress,
		  void** origin_function)
	  {
		  UNICODE_STRING* process_name = nullptr;
		  PEPROCESS CurrentProcess = IoGetCurrentProcess();

		  
		  NTSTATUS status = utils::process_utils::get_process_name(CurrentProcess, &process_name);
		  if (NT_SUCCESS(status) && process_name && process_name->Buffer)
		  {
			  LogInfo("[Hook] TargetAddress %p Function %p [PID: %u, Name: %wZ]\n",
				  TargetAddress,
				  DetourAddress,
				  (ULONG)PsGetProcessId(CurrentProcess),
				  process_name);

			 
			  utils::internal_functions::pfn_ex_free_pool_with_tag(process_name->Buffer, 0);
			  utils::internal_functions::pfn_ex_free_pool_with_tag(process_name, 0);
		  }
		  else
		  {
			  LogInfo("[Hook] TargetAddress %p Function %p [PID: %u, Name: <Unknown>]\n",
				  TargetAddress,
				  DetourAddress,
				  (ULONG)PsGetProcessId(CurrentProcess));
		  }
		  return original_ace_create_hook_internal(a1, TargetAddress, DetourAddress, origin_function);
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

	   BOOLEAN __fastcall new_present_dwm	
	   (
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ kernel_hook_function_info* matched_hook_info)
	   {

		   UNREFERENCED_PARAMETER(ExceptionRecord);

		 
		 

		   //if (!utils::dwm_draw::g_pswap_chain)
		   //{
			  // utils::dwm_draw::g_pswap_chain = ContextRecord->Rcx;
			  // utils::dwm_draw::g_dwm_render_thread = utils::internal_functions::pfn_ps_get_current_thread();
		   //}


		   //if (utils::dwm_draw::g_pswap_chain)
		   //{
			  // if (utils::dwm_draw::g_dwm_render_thread == utils::internal_functions::pfn_ps_get_current_thread())
			  // {
				 //  /*   if (utils::strong_dx::initialize_d3d_resources())
				 //  {
					//   utils::strong_dx::draw_utils();
				 //  }*/
			  // }
		   //}

		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);
		    
		   return TRUE;
	   }

	   BOOLEAN  __fastcall new_present_multiplane_overlay(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ kernel_hook_function_info* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);

		
		 
		  
		 
		   if (!utils::dwm_draw::g_pswap_chain)
		   {
			   utils::dwm_draw::g_pswap_chain = ContextRecord->Rcx;
			   LogInfo("g_pswap_chain RCX value: 0x%llX", ContextRecord->Rcx);
			   utils::dwm_draw::g_dwm_render_thread = utils::internal_functions::pfn_ps_get_current_thread();
			  
		   }

		   if (utils::dwm_draw::g_pswap_chain)
		   {

			
			   if (utils::dwm_draw::g_dwm_render_thread == utils::internal_functions::pfn_ps_get_current_thread())
			   {
				 
				   if (InterlockedCompareExchange(&utils::strong_dx::g_dwm_render_lock, 1, 0) == 0)
				   {
					   if (utils::strong_dx::initialize_d3d_resources(utils::dwm_draw::g_pswap_chain))
					   {
						  
						   utils::strong_dx::draw_utils();
					   }
					   InterlockedExchange(&utils::strong_dx::g_dwm_render_lock, 0);
				   }
			   }
			  
		   }

		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);
		 
		   return TRUE;
	   }

	   BOOLEAN  __fastcall new_cdxgi_swap_chain_dwm_legacy_present_dwm(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ kernel_hook_function_info* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);

		   
		
		   if (!utils::dwm_draw::g_pswap_chain)
		   {
			   utils::dwm_draw::g_pswap_chain = ContextRecord->Rcx;
			   utils::dwm_draw::g_dwm_render_thread = utils::internal_functions::pfn_ps_get_current_thread();

		   }


		   if (utils::dwm_draw::g_pswap_chain)
		   {


			   if (utils::dwm_draw::g_dwm_render_thread == utils::internal_functions::pfn_ps_get_current_thread())
			   {
				 
				   if (InterlockedCompareExchange(&utils::strong_dx::g_dwm_render_lock, 1, 0) == 0)
				   {
					   if (utils::strong_dx::initialize_d3d_resources(utils::dwm_draw::g_pswap_chain))
					   {
						   utils::strong_dx::draw_utils();
					   }
					   InterlockedExchange(&utils::strong_dx::g_dwm_render_lock, 0);
				   }
					   
				  
			   }

		   }
		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);
		   
		   return TRUE;
	   }

	   BOOLEAN  __fastcall new_cddisplay_render_target_present(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ kernel_hook_function_info* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);

		  

		 
		   static bool initialized = false;
		   if (!initialized)
		   {
			    if (ContextRecord->Rcx)
				{
					utils::dwm_draw::g_prender_target = ContextRecord->Rcx;
					utils::dwm_draw::g_pswap_chain = *(uint64_t*)(utils::dwm_draw::g_prender_target + 0xD0) + 0x18;
					utils::dwm_draw::g_dwm_render_thread = utils::internal_functions::pfn_ps_get_current_thread();
					initialized = true;
			    }


		   }
		    

		  
		   if (initialized)
		   {


			   if (utils::dwm_draw::g_dwm_render_thread == utils::internal_functions::pfn_ps_get_current_thread())
			   {
				  
				   if (InterlockedCompareExchange(&utils::strong_dx::g_dwm_render_lock, 1, 0) == 0)
				   {
					/*   if (utils::strong_dx::initialize_d3d11_resources_win1124h2(utils::dwm_draw::g_pswap_chain))
					   {
						   utils::strong_dx::draw_utils();
					   }*/
					   InterlockedExchange(&utils::strong_dx::g_dwm_render_lock, 0);
				   }


			   }

		   }
		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);
		  
		   return TRUE;
	   }


	   BOOLEAN __fastcall new_cocclusion_context_pre_sub_graph(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ kernel_hook_function_info* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);
		  
		 
		  
		   ULONG64 original_return_address = *(ULONG64*)ContextRecord->Rsp;
		   unsigned long long cocclusion_context_pre_sub_graph_fun =
			   reinterpret_cast<unsigned long long>(matched_hook_info->trampoline_va); 

		   unsigned long long usercall_retval_ptr =
			   utils::user_call::call(
				   cocclusion_context_pre_sub_graph_fun,
				   ContextRecord->Rcx,
				   ContextRecord->Rdx,
				   ContextRecord->R8, 0);

		   if (usercall_retval_ptr)
		   {
			   
			   HRESULT hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
			 

			   ContextRecord->Rax = (ContextRecord->Rax & 0xFFFFFFFF00000000) | (hr & 0xFFFFFFFF);
			   bool* a3 = reinterpret_cast<bool*>  (ContextRecord->R8);
			   *a3 = true;
			    

			 
			   ContextRecord->Rip = original_return_address;
			   ContextRecord->Rsp += sizeof(ULONG64);
		   }
	 
		  
		   return TRUE;

	   }
	   BOOLEAN __fastcall new_cocclusion_context_post_sub_graph(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_  kernel_hook_function_info* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);
		  
		 
		   ULONG64 original_return_address = *(ULONG64*)ContextRecord->Rsp;
		   unsigned long long cocclusion_context_post_sub_graph_fun =
			   reinterpret_cast<unsigned long long>(matched_hook_info->trampoline_va);
		   unsigned long long usercall_retval_ptr =
			   utils::user_call::call(
				   cocclusion_context_post_sub_graph_fun,
				   ContextRecord->Rcx,
				   ContextRecord->Rdx,
				   ContextRecord->R8, 0);

		 if (usercall_retval_ptr)
		 {
			
			 HRESULT hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
			 
			 ContextRecord->Rax = (ContextRecord->Rax & 0xFFFFFFFF00000000) | (hr & 0xFFFFFFFF);
			 
			 bool* a3 = reinterpret_cast<bool*>  (ContextRecord->R8);
			  *a3 = false;
			  

		 
			  
			 ContextRecord->Rip = original_return_address;
			 ContextRecord->Rsp += sizeof(ULONG64);
			 }
		  
		   return TRUE;
	   }

	      INT64(__fastcall* original_cdxgi_swap_chain_dwm_legacy_present_dwm)(
		   void* pthis,
		   PVOID pDxgiSwapChain,
		   unsigned int a3,
		   unsigned int a4,
		   const PVOID a5, unsigned int a6, PVOID64 a7, unsigned int a8,
		   PVOID a9, unsigned int a10) = nullptr;


	 

	   BOOLEAN __fastcall  new_get_buffer(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_  kernel_hook_function_info* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);
		   static volatile LONG g_screen_capture_count = 0;

		 
		   utils::strong_dx::g_should_hide_overlay = true;

		  
		   ULONG count = InterlockedIncrement(&g_screen_capture_count);

		 
		  
		   LARGE_INTEGER system_time_before = {};
		   KeQuerySystemTime(&system_time_before);

		   TIME_FIELDS time_fields_before;
		   LARGE_INTEGER local_time_before;
		   ExSystemTimeToLocalTime(&system_time_before, &local_time_before);
		   RtlTimeToTimeFields(&local_time_before, &time_fields_before);

		 LogInfo( "[DWM-HOOK] Before Sleep Time: %04d-%02d-%02d %02d:%02d:%02d.%03d\n",
			   time_fields_before.Year,
			   time_fields_before.Month,
			   time_fields_before.Day,
			   time_fields_before.Hour,
			   time_fields_before.Minute,
			   time_fields_before.Second,
			   time_fields_before.Milliseconds);


		   utils::thread_utils::sleep(1);

		   
		   unsigned long long get_buffer_fun = reinterpret_cast<unsigned long long>(matched_hook_info->trampoline_va);
		   unsigned long long usercall_retval_ptr = utils::user_call::call(
			   get_buffer_fun, ContextRecord->Rcx, ContextRecord->Rdx, ContextRecord->R8, ContextRecord->R9);

		   HRESULT hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
		   ContextRecord->Rax = hr;
		   ContextRecord->Rip = *(ULONG64*)ContextRecord->Rsp;
		   ContextRecord->Rsp += sizeof(ULONG64);

		    
		   LARGE_INTEGER system_time_after = {};
		   KeQuerySystemTime(&system_time_after);

		   TIME_FIELDS time_fields_after;
		   LARGE_INTEGER local_time_after;
		   ExSystemTimeToLocalTime(&system_time_after, &local_time_after);
		   RtlTimeToTimeFields(&local_time_after, &time_fields_after);

		   LogInfo(  "[DWM-HOOK] After Sleep Time:  %04d-%02d-%02d %02d:%02d:%02d.%03d\n",
			   time_fields_after.Year,
			   time_fields_after.Month,
			   time_fields_after.Day,
			   time_fields_after.Hour,
			   time_fields_after.Minute,
			   time_fields_after.Second,
			   time_fields_after.Milliseconds);

		   LogInfo( "[DWM-HOOK] Screen capture attempt detected. Count=%lu RSP=0x%llX\n",
			   count, ContextRecord->Rsp);
		  

		   utils::strong_dx::g_should_hide_overlay =false;
		 
	 
		   return TRUE;
	   }


	   BOOLEAN  __fastcall new_nvfbc_create_ex(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_  kernel_hook_function_info* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);

		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);
		   LogInfo( 
			   "[NVFBC_HOOK_EX] RCX=0x%llx, RDX=0x%llx, R8=0x%llx, R9=0x%llx\n",
			   ContextRecord->Rcx,
			   ContextRecord->Rdx,
			   ContextRecord->R8,
			   ContextRecord->R9);
		   return TRUE;

		  
	   }

	   BOOLEAN  __fastcall new_nvfbc_create(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_ kernel_hook_function_info* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);
		   ContextRecord->Rip = reinterpret_cast<unsigned long long> (matched_hook_info->trampoline_va);
		   LogInfo( 
			   "[NVFBC_HOOK] RCX=0x%llx, RDX=0x%llx, R8=0x%llx, R9=0x%llx\n",
			   ContextRecord->Rcx,
			   ContextRecord->Rdx,
			   ContextRecord->R8,
			   ContextRecord->R9);
		   return TRUE;
		  
	   }


	   __int64(__fastcall* original_dxgk_get_device_state)(
		   _Inout_ PVOID unnamedParam1)
		   = nullptr;



	   __int64 __fastcall hook_dxgk_get_device_state(_Inout_ PVOID unnamedParam1)
	   {
		   static bool has_hooked_get_buffer = false;
			if (utils::internal_functions:: pfn_ke_get_current_irql() >= DISPATCH_LEVEL)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (utils::internal_functions::pfn_ex_get_previous_mode() != MODE::UserMode)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}
			
			if (!unnamedParam1)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (utils::dwm_draw::g_dwm_process == nullptr)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (utils::internal_functions::pfn_ps_get_current_process()!=utils :: dwm_draw::g_dwm_process)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (!utils::strong_dx::is_user_stack_in_dxgi_range())
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (!utils::dwm_draw::g_pswap_chain)
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}

			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<PVOID> (utils::dwm_draw::g_pswap_chain)))
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}
			
			 

			if (utils::dwm_draw::g_dwm_render_thread != utils::internal_functions::pfn_ps_get_current_thread())
			{
				return  original_dxgk_get_device_state(unnamedParam1);
			}
			
		 
			  
			utils::strong_dx:: draw_utils();
		
		
		  
		   return  original_dxgk_get_device_state(unnamedParam1);
	   }

	  



 
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
		   ) = nullptr;

	   NTSTATUS NTAPI  new_nt_gdi_ddddi_open_resource
	   (	ULONG64 a1,
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

	   )
	   {
		//   HANDLE pid = utils::internal_functions::pfn_ps_get_current_process_id();

	 
	 //

		//   if (utils::dwm_draw::g_dwm_process != utils::internal_functions::pfn_ps_get_current_process())
		//   {
		//	   return original_nt_gdi_ddddi_open_resource(
		//		   a1, a2, a3, a4, a5,
		//		   a6, a7, a8, a9, a10,
		//		   a11, a12, a13, a14, a15
		//	   );
		//   }

		// 
	 //
		//   utils::strong_dx::g_should_hide_overlay = true;
		// 
		//   static volatile LONG g_screen_capture_count = 0;
		//   ULONG count = InterlockedIncrement(&g_screen_capture_count);

		//  
		// //  DbgPrintEx(77, 0, "[DWM-HOOK] D3DKMTOpenResource intercepted - possible screen/resource capture attempt. Count=%lu, a1=0x%llx  PID:%p\n", count, a1 , pid);
		// 
		//   utils::thread_utils::sleep_ms(500);
		   NTSTATUS status = original_nt_gdi_ddddi_open_resource(
			   a1, a2, a3, a4, a5,
			   a6, a7, a8, a9, a10,
			   a11, a12, a13, a14, a15
		   );
		////   utils::thread_utils::sleep_ms(200);
		//   utils::strong_dx::g_should_hide_overlay = false;
		 
		   return status;
		  
		  

		  
	   }

	   BOOLEAN  __fastcall new_get_csgo_hp(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_  kernel_hook_function_info* matched_hook_info)
	   {
		 
		   UNREFERENCED_PARAMETER(ExceptionRecord);
		 
		 

		   uint64_t rsi = ContextRecord->Rsi;
		   uint64_t R14 = ContextRecord->R14;

		   uint64_t target_value = *reinterpret_cast<uint64_t*>(rsi + 0xD0);
		    
		   // ���û���� ZF���� 6 λ��
		   if (target_value == R14) {
			   ContextRecord->EFlags |= (1 << 6);
			  
		   }
		   else {
			   ContextRecord->EFlags &= ~(1 << 6);
			   
		   }

		   ULONG64 new_rip =  ContextRecord->Rip  + matched_hook_info->hook_size;
		    
		   ContextRecord->Rip = new_rip;

		   
		 
		  // game::kcsgo2::g_game->loop();
	 
		   
			 
		   return TRUE;
	   }

	   BOOLEAN  __fastcall new_dxgk_get_device_state(
		   _Inout_ PEXCEPTION_RECORD ExceptionRecord,
		   _Inout_ PCONTEXT ContextRecord,
		   _Inout_  kernel_hook_function_info* matched_hook_info)
	   {
		   UNREFERENCED_PARAMETER(ExceptionRecord);
		  
		 //  ULONG64 original_return_address = *(ULONG64*)ContextRecord->Rsp;


		    
		   unsigned long long dxgk_get_device_state_fun =
			   reinterpret_cast<unsigned long long>(matched_hook_info->trampoline_va);

		   ContextRecord->Rip = dxgk_get_device_state_fun;

		   if (utils::dwm_draw::g_dwm_process == nullptr)
		   {
			   return TRUE;
		   }

		   if (utils::internal_functions::pfn_ps_get_current_process() != utils::dwm_draw::g_dwm_process)
		   {
			   return TRUE;
		   }

	 
		   if (!utils::dwm_draw::g_pswap_chain)
		   {
			   return TRUE;
		   }

		   if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<PVOID> (utils::dwm_draw::g_pswap_chain)))
		   {
			   return TRUE;
		   }
		    
		   if (utils::dwm_draw::g_dwm_render_thread != utils::internal_functions::pfn_ps_get_current_thread())
		   {
			   return TRUE;
		   }

		   
		    utils::strong_dx::draw_utils();

		   return TRUE;
	   }


 

}
