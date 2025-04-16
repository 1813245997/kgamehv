#include "hv.h"
#include "hookutils.h"
#include "hook_manager.h"
#include "hook_functions.h"
#include "utils/scanner_fun.h"
namespace hook_manager
{
	NTSTATUS InitializeAllHooks()
	{
	 
		unsigned long long ki_preprocess_fault_addr = utils::scanner_fun::find_ki_preprocess_fault();

		DbgPrint("[hv] Failed to initialize ntoskrnl info. %p\n", ki_preprocess_fault_addr);
		  hyper::hook((void*)ki_preprocess_fault_addr, hook_functions::Hook_KiPreprocessFault, (void**)&hook_functions::OriginalKiPreprocessFault);
	  	
		return STATUS_SUCCESS;
	}
}







