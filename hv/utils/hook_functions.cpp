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
  

	  
	    

 

}
