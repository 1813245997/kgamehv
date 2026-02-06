#include "global_defs.h"
#include "hook_manager.h"
namespace hook_manager
{
	NTSTATUS initialize_all_hooks()
	{
		 
		NTSTATUS status = STATUS_SUCCESS;
		status = utils::hook_utils::initialize_hook_page_lists();
		if (!NT_SUCCESS(status))
		{
			LogError("Failed to initialize hook page lists (0x%X).", status);
			return status;
		}

		status = utils::shadowbreakpoint::shadowbp_initialize();
		if (!NT_SUCCESS(status))
		{
			LogError("Failed to initialize shadow break point (0x%X).", status);
			return status;
		}
		 
		status = utils::hook_mem_protection::initialize_mem_protections();
		if (!NT_SUCCESS(status))
		{

			LogError("Failed to initialize mem prot (0x%X).", status);
			return status;
		}
		status = utils::hook_anti::initialize_hook_anti();
		if (!NT_SUCCESS(status))
		{

			LogError("Failed to initialize hook_anti (0x%X).", status);
			return status;
		}

		//会导致游戏不正常进入
		//status = utils::hook_process_manager::initialize_hook_process_manager();
		//if (!NT_SUCCESS(status))
		//{
		//	LogError("Failed to initialize hook process manager (0x%X).", status);
		//	return status;
		//}

		status = utils::hook_user_comm::initialize_hook_user_comm();
		if (!NT_SUCCESS(status))
		{
			LogError("Failed to initialize hook user comm (0x%X).", status);
			return status;
		}



		 

 






		return STATUS_SUCCESS;
	}

	

}







