#include "global_defs.h"
#include "shadow_break_point.h"

namespace utils {
	namespace shadowbreakpoint {
		
		// Global hash table - manages processes by PID
		utils::kunordered_map<HANDLE, process_breakpoint_info*> g_process_breakpoint_map;  // PID -> ProcessInfo
		FAST_MUTEX g_breakpoint_list_lock{};

		NTSTATUS shadowbp_initialize()
		{
			NTSTATUS status = STATUS_SUCCESS;
			
			// Initialize hash table
			g_process_breakpoint_map.init();
			ExInitializeFastMutex(&g_breakpoint_list_lock);

			if (!internal_functions::pfn_ki_preprocess_fault)
			{
				status = STATUS_NOT_FOUND;
				LogError("KiPreprocessFault is not found");
				goto clear;
			}

			if (!hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(internal_functions::pfn_ki_preprocess_fault),
				hook_functions::hook_ki_preprocess_fault,
				reinterpret_cast<void**>(&hook_functions::original_ki_preprocess_fault)
			))
			{
				LogError("Failed to hook KiPreprocessFault");
				status = STATUS_UNSUCCESSFUL;
				goto clear;
			}

		clear:
			return STATUS_SUCCESS;
		}

		bool shadowbp_install(_In_opt_  HANDLE process_id, _In_ void* target_address, _In_ void* handler)
		{
			if (utils::memory::is_user_address(target_address))
			{
				return shadowbp_install_user(process_id, target_address, handler);
			}

			return shadowbp_install_kernel(process_id, target_address, handler);
		}

		bool shadowbp_install_user(_In_opt_  HANDLE process_id, _In_ void* target_address, _In_ void* handler)
		{
			PEPROCESS process = nullptr;

			if (!target_address)
			{
				return false;
			}

			if (!handler)
			{
				return false;
			}

			NTSTATUS status = utils::internal_functions::pfn_ps_lookup_process_by_process_id(process_id, &process);
			if (!NT_SUCCESS(status))
			{
				return false;
			}

			if (utils::internal_functions::pfn_ps_get_process_exit_status(process) != STATUS_PENDING)
			{
				utils::internal_functions::pfn_ob_dereference_object(process);
				return false;
			}

			const ULONGLONG target_cr3 = utils::process_utils::get_process_cr3(process);
			if (target_cr3 == 0)
			{
				utils::internal_functions::pfn_ob_dereference_object(process);
				return false;
			}

			KAPC_STATE apc_state{};
			uint64_t target_pa{};
			
			bool succeed = false;
			  
			utils::internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			void* aligned_target = PAGE_ALIGN(target_address);
			target_pa = utils::internal_functions::pfn_mm_get_physical_address(aligned_target).QuadPart;
			if (target_pa == 0)
			{
				goto clear;
			}
			unsigned __int64 page_pfn = GET_PFN(target_pa);
			unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET(target_address);

			ExAcquireFastMutex(&g_breakpoint_list_lock);

		 

		  	bool is_new_page = false;
			// O(1) find process
			auto process_it = g_process_breakpoint_map.find(process_id);
			process_breakpoint_info* process_info = nullptr;
			
			if (process_it != g_process_breakpoint_map.end())
			{
				process_info = process_it->second;
			}
			else
			{
				// Create new process info
				process_info = pool_manager::request_pool<process_breakpoint_info*>(
					pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(process_breakpoint_info));
				if (!process_info)
				{
					goto clear;
				}
				
				process_info->process_id = process_id;
				process_info->page_map.init();
				
				// Add to process hash table
				g_process_breakpoint_map[process_id] = process_info;
			}

			// O(1) find page within process
			auto page_it = process_info->page_map.find(page_pfn);
			breakpoint_page_info* page_info = nullptr;
			
			if (page_it != process_info->page_map.end())
			{
				page_info = page_it->second;
			}
			else
			{
				is_new_page = true;
				// Create new page
				page_info = pool_manager::request_pool<breakpoint_page_info*>(
					pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(breakpoint_page_info));
				if (!page_info)
				{
					goto clear;
				}

				page_info->page_contents = pool_manager::request_pool<uint8_t*>(
					pool_manager::INTENTION_FAKE_PAGE_CONTENTS, TRUE, PAGE_SIZE);
				if (!page_info->page_contents)
				{
					pool_manager::release_pool(page_info);
					goto clear;
				}

				uint64_t fake_pa = utils::internal_functions::pfn_mm_get_physical_address(page_info->page_contents).QuadPart;
				if (fake_pa == 0)
				{
					pool_manager::release_pool(page_info->page_contents);
					pool_manager::release_pool(page_info);
					goto clear;
				}

				 
				RtlCopyMemory(page_info->page_contents, aligned_target, PAGE_SIZE);

				auto exec_page = utils::internal_functions::pfn_mm_get_physical_address(page_info->page_contents).QuadPart;

				page_info->orig_page_pfn = page_pfn;
				page_info->exec_page_pfn = GET_PFN(exec_page);
				page_info->breakpoint_count = 0;
				page_info->breakpoint_map.init();

				// Add to process page map
				process_info->page_map[page_pfn] = page_info;
				
				
			}

			// Create breakpoint info
			auto* func_info = pool_manager::request_pool<breakpoint_function_info*>(
				pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(breakpoint_function_info));
			if (!func_info)
			{
				goto clear;
			}
			
			func_info->handler_va = handler;
			func_info->original_va = target_address;
			func_info->original_pa = target_pa;
			func_info->breakpoint_va = &page_info->page_contents[page_offset];
			func_info->instruction_size = LDE(target_address, 64);
			func_info->is_active = true;
			func_info->page_info_ptr = page_info;
			RtlCopyMemory(&func_info->original_byte, target_address, 1);
			*reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = 0xCC; // INT3
			++page_info->breakpoint_count;

			// O(1) insert - only insert to page hash table
			page_info->breakpoint_map[target_address] = func_info;
			succeed = true;

			if (is_new_page)
			{
				hv::prevmcall::install_ept_hook(page_info->orig_page_pfn, page_info->exec_page_pfn);
			}

		clear:
			ExReleaseFastMutex(&g_breakpoint_list_lock);
			utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			utils::internal_functions::pfn_ob_dereference_object(process);

			if (succeed)
			{
				LogInfo(
					"[shadowbp] Install breakpoint SUCCESS - target: %p, handler: %p\n",
					target_address,
					handler);
			}
			else
			{
				LogError(
					"[shadowbp] Install breakpoint FAILED - target: %p, handler: %p\n",
					target_address,
					handler);
			}

			return succeed;
		}

		bool shadowbp_install_kernel(_In_opt_  HANDLE process_id, _In_ void* target_address, _In_ void* handler)
		{
			if (!target_address)
			{
				return false;
			}
			  
			if (!handler)
			{
				return false;
			}
			 
			uint64_t target_pa{};
			
			bool succeed = false;
			  
			

			bool is_new_page = false;
			void* aligned_target = PAGE_ALIGN(target_address);
			target_pa = utils::internal_functions::pfn_mm_get_physical_address(aligned_target).QuadPart;
			if (target_pa == 0)
			{
				goto clear;
			}
			unsigned __int64 page_pfn = GET_PFN(target_pa);
			unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET(target_address);

			ExAcquireFastMutex(&g_breakpoint_list_lock);



			
			// O(1) find process
			auto process_it = g_process_breakpoint_map.find(process_id);
			process_breakpoint_info* process_info = nullptr;
			
			if (process_it != g_process_breakpoint_map.end())
			{
				process_info = process_it->second;
			}
			else
			{
				// Create new process info
				process_info = pool_manager::request_pool<process_breakpoint_info*>(
					pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(process_breakpoint_info));
				if (!process_info)
				{
					goto clear;
				}
				
				process_info->process_id = process_id;
				process_info->page_map.init();
				
				// Add to process hash table
				g_process_breakpoint_map[process_id] = process_info;
			}

			// O(1) find page within process
			auto page_it = process_info->page_map.find(page_pfn);
			breakpoint_page_info* page_info = nullptr;
			
			if (page_it != process_info->page_map.end())
			{
				page_info = page_it->second;
			}
			else
			{
				is_new_page = true;
				// Create new page
				page_info = pool_manager::request_pool<breakpoint_page_info*>(
					pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(breakpoint_page_info));
				if (!page_info)
				{
					goto clear;
				}

				page_info->page_contents = pool_manager::request_pool<uint8_t*>(
					pool_manager::INTENTION_FAKE_PAGE_CONTENTS, TRUE, PAGE_SIZE);
				if (!page_info->page_contents)
				{
					pool_manager::release_pool(page_info);
					goto clear;
				}

				uint64_t fake_pa = utils::internal_functions::pfn_mm_get_physical_address(page_info->page_contents).QuadPart;
				if (fake_pa == 0)
				{
					pool_manager::release_pool(page_info->page_contents);
					pool_manager::release_pool(page_info);
					goto clear;
				}

				
				RtlCopyMemory(page_info->page_contents, aligned_target, PAGE_SIZE);

				auto exec_page = utils::internal_functions::pfn_mm_get_physical_address(page_info->page_contents).QuadPart;

				page_info->orig_page_pfn = page_pfn;
				page_info->exec_page_pfn = GET_PFN(exec_page);
				page_info->breakpoint_count = 0;
				page_info->breakpoint_map.init();

				// Add to process page map
				process_info->page_map[page_pfn] = page_info;
					
				
			}

			// Create breakpoint info
			auto* func_info = pool_manager::request_pool<breakpoint_function_info*>(
				pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(breakpoint_function_info));
			if (!func_info)
			{
				goto clear;
			}

			func_info->handler_va = handler;
			func_info->original_va = target_address;
			func_info->original_pa = target_pa;
			func_info->breakpoint_va = &page_info->page_contents[page_offset];
			func_info->is_active = true;
			func_info->page_info_ptr = page_info;
				  
			RtlCopyMemory(&func_info->original_byte, target_address, 1);
			*reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = 0xCC; // INT3
			++page_info->breakpoint_count;

			// O(1) insert - only insert to page hash table
			page_info->breakpoint_map[target_address] = func_info;
			succeed = true;
			if (is_new_page)
			{
				hv::prevmcall::install_ept_hook(page_info->orig_page_pfn, page_info->exec_page_pfn);
			}

		clear:
			ExReleaseFastMutex(&g_breakpoint_list_lock);

			if (succeed)
			{
				LogInfo(
					"[shadowbp] Install breakpoint SUCCESS - target: %p, handler: %p\n",
					target_address,
					handler);
			}
			else
			{
				LogError(
					"[shadowbp] Install breakpoint FAILED - target: %p, handler: %p\n",
					target_address,
					handler);
			}

			return succeed;
		}

		bool shadowbp_remove_all(HANDLE process_id)
		{
			bool removed_any = false;

			ExAcquireFastMutex(&g_breakpoint_list_lock);

			// O(1) find process
			auto process_it = g_process_breakpoint_map.find(process_id);
			if (process_it != g_process_breakpoint_map.end())
			{
				process_breakpoint_info* process_info = process_it->second;
				
				// Clean all pages in this process
				for (auto& page_pair : process_info->page_map)
				{
					breakpoint_page_info* page_info = page_pair.second;
					
					// Clean all breakpoints in this page
					for (auto& bp_pair : page_info->breakpoint_map)
					{
						breakpoint_function_info* func_info = bp_pair.second;
						if (func_info)
						{
							*reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = func_info->original_byte;
							func_info->is_active = false;
							pool_manager::release_pool(func_info);
						}
					}

					// Remove EPT hook
					hv::prevmcall::remove_ept_hook(page_info->orig_page_pfn);

					// Clean page
					pool_manager::release_pool(page_info->page_contents);
					page_info->breakpoint_map.clear();
					pool_manager::release_pool(page_info);
				}

				// Clean process
				process_info->page_map.clear();
				pool_manager::release_pool(process_info);
				
				// Remove from global hash table
				g_process_breakpoint_map.erase(process_id);
				removed_any = true;
			}

			ExReleaseFastMutex(&g_breakpoint_list_lock);

			if (removed_any)
			{
				LogInfo("[shadowbp] Removed all breakpoints for process %p", process_id);
			}
			else
			{
				LogError("[shadowbp] No breakpoints found for process %p", process_id);
			}

			return removed_any;
		}

		bool shadowbp_remove(HANDLE process_id, void* target_address)
		{
			bool removed = false;

			ExAcquireFastMutex(&g_breakpoint_list_lock);

			void* aligned_target = PAGE_ALIGN(target_address);
			// Calculate page PFN
			uint64_t target_pa = utils::internal_functions::pfn_mm_get_physical_address(aligned_target).QuadPart;
			if (target_pa == 0)
			{
				goto done;
			}
			unsigned __int64 page_pfn = GET_PFN(target_pa);

			// O(1) find process
			auto process_it = g_process_breakpoint_map.find(process_id);
			if (process_it == g_process_breakpoint_map.end())
			{
				goto done;  // Process doesn't exist
			}

			// O(1) find page within process
			process_breakpoint_info* process_info = process_it->second;
			auto page_it = process_info->page_map.find(page_pfn);
			if (page_it == process_info->page_map.end())
			{
				goto done;  // Page doesn't exist
			}

			// O(1) find breakpoint within page
			breakpoint_page_info* page_info = page_it->second;
			auto bp_it = page_info->breakpoint_map.find(target_address);
			if (bp_it != page_info->breakpoint_map.end())
			{
				breakpoint_function_info* func_info = bp_it->second;
				
				*reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = func_info->original_byte;
				func_info->is_active = false;

				// Remove from page hash table using target_address as key
				page_info->breakpoint_map.erase(target_address);
				--page_info->breakpoint_count;

				if (page_info->breakpoint_count == 0)
				{
					// Remove EPT hook
					hv::prevmcall::remove_ept_hook(page_info->orig_page_pfn);
					
					// Clean page
					pool_manager::release_pool(page_info->page_contents);
					page_info->breakpoint_map.clear();
					pool_manager::release_pool(page_info);
					
					// Remove from process page map using PFN as key
					process_info->page_map.erase(page_pfn);
					
					// If process has no pages left, remove process
					if (process_info->page_map.empty())
					{
						process_info->page_map.clear();
						pool_manager::release_pool(process_info);
						g_process_breakpoint_map.erase(process_id);
					}
				}

				pool_manager::release_pool(func_info);
				removed = true;
			}

		done:
			ExReleaseFastMutex(&g_breakpoint_list_lock);

			if (removed)
			{
				LogInfo("[shadowbp] Removed breakpoint at %p for process %p", target_address, process_id);
			}
			else
			{
				LogError("[shadowbp] Failed to find breakpoint at %p for process %p", target_address, process_id);
			}

			return removed;
		}

		bool shadowbp_set_active(HANDLE process_id, void* target_address, bool enable)
		{
			bool updated = false;

			ExAcquireFastMutex(&g_breakpoint_list_lock);

			void* aligned_target = PAGE_ALIGN(target_address);
			// Calculate page PFN
			uint64_t target_pa = utils::internal_functions::pfn_mm_get_physical_address(aligned_target).QuadPart;
			if (target_pa == 0)
			{
				goto done;
			}
			unsigned __int64 page_pfn = GET_PFN(target_pa);

			// O(1) find process
			auto process_it = g_process_breakpoint_map.find(process_id);
			if (process_it == g_process_breakpoint_map.end())
			{
				goto done;  // Process doesn't exist
			}

			// O(1) find page within process
			process_breakpoint_info* process_info = process_it->second;
			auto page_it = process_info->page_map.find(page_pfn);
			if (page_it == process_info->page_map.end())
			{
				goto done;  // Page doesn't exist
			}

			// O(1) find breakpoint within page
			breakpoint_page_info* page_info = page_it->second;
			auto bp_it = page_info->breakpoint_map.find(target_address);
			if (bp_it != page_info->breakpoint_map.end())
			{
				breakpoint_function_info* func_info = bp_it->second;

				if (enable && !func_info->is_active)
				{
					*reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = 0xCC;
					func_info->is_active = true;
					updated = true;
				}
				else if (!enable && func_info->is_active)
				{
					*reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = func_info->original_byte;
					func_info->is_active = false;
					updated = true;
				}
			}

		done:
			ExReleaseFastMutex(&g_breakpoint_list_lock);

			if (updated)
			{
				LogInfo("[shadowbp] Set breakpoint at %p for process %p -> %s",
					target_address, process_id, enable ? "ENABLED" : "DISABLED");
			}
			else
			{
				LogError("[shadowbp] Failed to update breakpoint at %p for process %p",
					target_address, process_id);
			}

			return updated;
		}

		bool shadowbp_find_address(
			HANDLE process_id,
			void* target_address,
			breakpoint_function_info** out_bp_info)
		{
			if (!target_address || !out_bp_info)
			{
				return false;
			}

			void* aligned_target = PAGE_ALIGN(target_address);
			// Calculate page PFN
			uint64_t target_pa = utils::internal_functions::pfn_mm_get_physical_address(aligned_target).QuadPart;
			if (target_pa == 0)
			{
				return false;
			}
			unsigned __int64 page_pfn = GET_PFN(target_pa);

			// O(1) find process
			auto process_it = g_process_breakpoint_map.find(process_id);
			if (process_it == g_process_breakpoint_map.end())
			{
				return false;  // Process doesn't exist
			}

			// O(1) find page within process
			process_breakpoint_info* process_info = process_it->second;
			auto page_it = process_info->page_map.find(page_pfn);
			if (page_it == process_info->page_map.end())
			{
				return false;  // Page doesn't exist
			}

			// O(1) find breakpoint within page
			breakpoint_page_info* page_info = page_it->second;
			auto bp_it = page_info->breakpoint_map.find(target_address);
			if (bp_it != page_info->breakpoint_map.end())
			{
				breakpoint_function_info* bp_info = bp_it->second;
				if (bp_info->is_active)
				{
					*out_bp_info = bp_info;
					return true;
				}
			}

			return false;
		}

		bool shadowbp_find_tf_address(
			HANDLE process_id,
			void* target_address,
			breakpoint_function_info** out_bp_info)
		{
			if (!target_address || !out_bp_info)
			{
				return false;
			}

			// O(1) find process
			auto process_it = g_process_breakpoint_map.find(process_id);
			if (process_it == g_process_breakpoint_map.end())
			{
				return false;  // Process doesn't exist
			}

			// Iterate through all pages in this process to find TF match
			process_breakpoint_info* process_info = process_it->second;
			for (auto& page_pair : process_info->page_map)
			{
				breakpoint_page_info* page_info = page_pair.second;
				
				// Iterate through all breakpoints in this page to find TF match
				for (auto& bp_pair : page_info->breakpoint_map)
				{
					breakpoint_function_info* bp_info = bp_pair.second;
					if (bp_info && bp_info->is_active)
					{
						// Calculate TF address: target_address - instruction_size
						void* tf_address = reinterpret_cast<void*>(
							reinterpret_cast<uint64_t>(target_address) - bp_info->instruction_size);
						
						// Check if TF address matches the original VA
						if (reinterpret_cast<void*>(tf_address) == bp_info->original_va)
						{
							*out_bp_info = bp_info;
							return true;
						}
					}
				}
			}

			return false;
		}

 
	}
}