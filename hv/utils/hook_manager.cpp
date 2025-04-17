#include "global_defs.h"
#include "hook_manager.h"
namespace hook_manager
{
	NTSTATUS InitializeAllHooks()
	{
		using namespace utils::scanner_fun;

		const auto ki_preprocess_fault_addr = find_ki_preprocess_fault();
		const auto mm_copy_memory_addr = find_mm_copy_memory();
		const auto mm_is_address_valid_addr = find_mm_is_address_valid();
		const auto rtl_walk_frame_chain_addr = find_rtl_walk_frame_chain();
		const auto rtl_lookup_function_entry_addr = find_rtl_lookup_function_entry();

		if (ki_preprocess_fault_addr)
		{
			hyper::hook(
				reinterpret_cast<void*>(ki_preprocess_fault_addr),
				hook_functions::hook_ki_preprocess_fault,
				reinterpret_cast<void**>(&hook_functions::original_ki_preprocess_fault)
			);
		}

		if (mm_copy_memory_addr)
		{
			hyper::hook(
				reinterpret_cast<void*>(mm_copy_memory_addr),
				hook_functions::hook_mm_copy_memory,
				reinterpret_cast<void**>(&hook_functions::original_mm_copy_memory)
			);
		}

		if (mm_is_address_valid_addr)
		{
			hyper::hook(
				reinterpret_cast<void*>(mm_is_address_valid_addr),
				hook_functions::hook_mm_is_address_valid,
				reinterpret_cast<void**>(&hook_functions::original_mm_is_address_valid)
			);
		}

		if (rtl_walk_frame_chain_addr)
		{
			hyper::hook(
				reinterpret_cast<void*>(rtl_walk_frame_chain_addr),
				hook_functions::hook_rtl_walk_frame_chain,
				reinterpret_cast<void**>(&hook_functions::original_rtl_walk_frame_chain)
			);
		}

		if (rtl_lookup_function_entry_addr)
		{
		/*	hyper::hook(
				reinterpret_cast<void*>(rtl_lookup_function_entry_addr),
				hook_functions::hook_rtl_lookup_function_entry,
				reinterpret_cast<void**>(&hook_functions::original_rtl_lookup_function_entry)
			);*/
		}

		return STATUS_SUCCESS;
	}

}







