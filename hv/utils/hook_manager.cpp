#include "global_defs.h"
#include "hook_manager.h"
namespace hook_manager
{
	NTSTATUS initialize_all_hooks()
	{
		using namespace utils::internal_functions;

		 
		utils::hook_utils:: initialize_hook_page_lists();
		if (pfn_ki_preprocess_fault)
		{
			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_ki_preprocess_fault),
				hook_functions::hook_ki_preprocess_fault,
				reinterpret_cast<void**>(&hook_functions::original_ki_preprocess_fault)
			);
		}

		if (pfn_psp_exit_process)
		{
			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_psp_exit_process),
				hook_functions::hook_psp_exit_process,
				reinterpret_cast<void**>(&hook_functions::original_psp_exit_process)
			);
		}


		/*	if (pfn_create_process_notify_routine_t)
			{
				utils::hook_utils::hook_kernel_function(
					reinterpret_cast<void*>(pfn_create_process_notify_routine_t),
					hook_functions::new_create_process_notify_routine_t,
					reinterpret_cast<void**>(&hook_functions::original_create_process_notify_routine_t)
				);*/
		//}

		if (pfn_mm_copy_memory)
		{
			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_mm_copy_memory),
				hook_functions::hook_mm_copy_memory,
				reinterpret_cast<void**>(&hook_functions::original_mm_copy_memory)
			);
		}

		if (pfn_mm_is_address_valid)
		{
			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_mm_is_address_valid),
				hook_functions::hook_mm_is_address_valid,
				reinterpret_cast<void**>(&hook_functions::original_mm_is_address_valid)
			);
		}

		if (pfn_rtl_walk_frame_chain)
		{
			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_rtl_walk_frame_chain),
				hook_functions::hook_rtl_walk_frame_chain,
				reinterpret_cast<void**>(&hook_functions::original_rtl_walk_frame_chain)
			);
		}
		if (pfn_rtl_lookup_function_entry)
		{
			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_rtl_lookup_function_entry),
				hook_functions::hook_rtl_lookup_function_entry,
				reinterpret_cast<void**>(&hook_functions::original_rtl_lookup_function_entry)
			);
		}

		if (pfn_nt_write_virtual_memory)
		{

			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_nt_write_virtual_memory),
				hook_functions::new_nt_write_virtual_memory,
				reinterpret_cast<void**>(&hook_functions::original_nt_write_virtual_memory)
			);
		}

		if (pfn_nt_query_virtual_memory)
		{
			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_nt_query_virtual_memory),
				hook_functions::new_nt_query_virtual_memory,
				reinterpret_cast<void**>(&hook_functions::original_nt_query_virtual_memory)
			);
		}
		if (pfn_nt_read_virtual_memory)
		{
			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_nt_read_virtual_memory),
				hook_functions::new_nt_read_virtual_memory,
				reinterpret_cast<void**>(&hook_functions::original_nt_read_virtual_memory)
			);
		}

		if (pfn_nt_create_section)
		{
			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_nt_create_section),
				hook_functions::hook_nt_create_section,
				reinterpret_cast<void**>(&hook_functions::original_nt_create_section)
			);
		}

		if (pfn_nt_create_user_process)
		{
			utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(pfn_nt_create_user_process),
				hook_functions::new_nt_create_user_process,
				reinterpret_cast<void**>(&hook_functions::original_nt_create_user_process)
			);
		}






		return STATUS_SUCCESS;
	}

}







