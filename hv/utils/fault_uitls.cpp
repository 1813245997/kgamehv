#include "global_defs.h"
#include "fault_uitls.h"
 
namespace utils
{
	namespace falt_utils
	{
		 
		BOOLEAN   handle_page_fault_exception(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode)
		{
			UNREFERENCED_PARAMETER(ExceptionRecord);
			UNREFERENCED_PARAMETER(ContextRecord);
		 
	         //https://github.com/lainswork/dwm-screen-shot  
		 
		/*	if (PreviousMode != MODE::UserMode)
			{

				return FALSE;
			}

			if (dwm_draw::g_dwm_process != internal_functions::pfn_ps_get_current_process())
			{
				return FALSE;
			}



			utils::strong_dx::g_should_hide_overlay = true;*/

			//PVOID page_base = PAGE_ALIGN(ExceptionRecord->ExceptionAddress);
		 //

			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
			//	"[PAGE_FAULT_FIX] Address: 0x%p  PageBase: 0x%p  \n",
			//	ExceptionRecord->ExceptionAddress, page_base );

			 

			return FALSE;
			 

		}

		 
		BOOLEAN handle_int3_exception(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode)
		{
			breakpoint_function_info* bp_info = nullptr;
			HANDLE process_id = utils::internal_functions::pfn_ps_get_current_process_id();

			if (PreviousMode != MODE::UserMode)
			{
				return FALSE;
			}


			

			BOOLEAN found = utils::shadowbreakpoint::shadowbp_find_address(
				process_id, ExceptionRecord->ExceptionAddress, &bp_info);

			if (!found)
			{
				
				return FALSE;
			}

			if (!bp_info)
			{
				
				return FALSE;
			}
			 
			bp_info->hit_count++;

			using handler_fn_t = NTSTATUS(__fastcall*)(_Inout_ PCONTEXT ContextRecord);
			auto handler_func = reinterpret_cast<handler_fn_t>(bp_info->handler_va);

			NTSTATUS status = handler_func(ContextRecord);
			if (!NT_SUCCESS(status))
			{
				//LogError("Handler failed with status: 0x%X", status);
			}

			// Restore original byte
			*(PUINT8)bp_info->breakpoint_va = bp_info->original_byte;

			// Enable single step for next instruction
			ContextRecord->EFlags |= 0x100;
			
			return TRUE;
		}

	
			 

		BOOLEAN handle_single_step_exception(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode)
		{
			 
			breakpoint_function_info* bp_info = nullptr;
			HANDLE process_id = utils::internal_functions::pfn_ps_get_current_process_id();

			if (PreviousMode != MODE::UserMode)
			{
				return FALSE;
			}

			
			
			
			// Find breakpoint info for current RIP (single step happens at next instruction)
			BOOLEAN found = utils::shadowbreakpoint::shadowbp_find_tf_address(
				process_id, reinterpret_cast<PVOID>(ExceptionRecord->ExceptionAddress), &bp_info);

			if (!found)
			{
				return FALSE;
			}

			if (!bp_info)
			{
				return FALSE;
			}
		 
			// Restore INT3 instruction at the breakpoint address
			*(PUINT8)bp_info->breakpoint_va = 0xCC;

			// Clear single step flag
			ContextRecord->EFlags &= ~0x100;
		
			return TRUE;
		}
	 }
}