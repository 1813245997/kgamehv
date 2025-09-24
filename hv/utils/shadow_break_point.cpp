#include "global_defs.h"
#include "shadow_break_point.h"

 

namespace utils {
	namespace shadowbreakpoint {
		 
		LIST_ENTRY g_breakpoint_list{};
		FAST_MUTEX g_breakpoint_list_lock{};

	     void shadowbp_initialize()
		{
			InitializeListHead(&g_breakpoint_list);
			ExInitializeFastMutex(&g_breakpoint_list_lock);
		}

		 bool shadowbp_install(HANDLE process_id, void* target_address, void* handler)
		 {
			 PEPROCESS process = nullptr;
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
			 bool is_64_bit = false;
			 bool succeed = false;
			 

			 is_64_bit = utils::process_utils::is_process_64_bit(process);

			 utils::internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			 unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET(target_address);

			 target_pa = utils::internal_functions::pfn_mm_get_physical_address(target_address).QuadPart;
			 if (target_pa == 0)
			 {
				 goto clear;

			 }

			 ExAcquireFastMutex(&g_breakpoint_list_lock);

			 for (PLIST_ENTRY entry = g_breakpoint_list.Flink;
				 entry != &g_breakpoint_list;
				 entry = entry->Flink)
			 {
				 breakpoint_page_info* page_info = CONTAINING_RECORD(entry, breakpoint_page_info, page_list);

				 if (page_info->orig_page_pfn == GET_PFN(target_pa) && page_info->process_id == process_id)
				 {

					 auto* func_info = pool_manager::request_pool<breakpoint_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(breakpoint_function_info));
					 if (!func_info)
					 {
						 return false;

					 }

					 func_info->handler_va = handler;
					 func_info->original_va = target_address;
					 func_info->original_pa = target_pa;
					 func_info->breakpoint_va = &page_info->page_contents[page_offset];
					 func_info->is_active = true;
					 RtlCopyMemory(&func_info->original_byte, target_address, 1);
					 *reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = 0xCC;// INT3
					 ++page_info->breakpoint_count;

					 InsertHeadList(&page_info->breakpoint_list, &func_info->list_entry);
					 goto clear;
				 }
			 }



			 auto* page_info = pool_manager::request_pool<breakpoint_page_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(breakpoint_page_info));
			 if (!page_info)
			 {
				 return false;
			 }

			 page_info->page_contents = pool_manager::request_pool<uint8_t*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, TRUE, PAGE_SIZE);
			 if (!page_info->page_contents)
			 {
				 pool_manager::release_pool(page_info);
				 return false;
			 }

			 uint64_t fake_pa = utils::internal_functions::pfn_mm_get_physical_address(page_info->page_contents).QuadPart;
			 if (fake_pa == 0)
			 {
				 pool_manager::release_pool(page_info->page_contents);
				 pool_manager::release_pool(page_info);

				 return false;

			 }

			   
			 auto* func_info = pool_manager::request_pool<breakpoint_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(breakpoint_function_info));
			 if (!func_info)
			 {
				 pool_manager::release_pool(page_info->page_contents);
				 pool_manager::release_pool(page_info);
				 return false;
			 }

			 void* aligned_target = PAGE_ALIGN(target_address);
			 RtlCopyMemory(page_info->page_contents,aligned_target, PAGE_SIZE);

			 auto  exec_page = utils::internal_functions::pfn_mm_get_physical_address(page_info->page_contents).QuadPart;

			 page_info->process_id = process_id;
			 page_info->orig_page_pfn = GET_PFN(target_pa);
			 page_info->exec_page_pfn = GET_PFN(exec_page);
			 func_info->handler_va = handler;
			 func_info->original_va = target_address;
			 func_info->original_pa = target_pa;
			 func_info->breakpoint_va = &page_info->page_contents[page_offset];
			 func_info->is_active = true;
			 RtlCopyMemory(&func_info->original_byte, target_address, 1);
			 *reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = 0xCC;// INT3
			 ++page_info->breakpoint_count;
			 
			 InitializeListHead(&page_info->breakpoint_list);
			 InsertHeadList(&page_info->breakpoint_list, &func_info->list_entry);
			 InsertHeadList(&g_breakpoint_list, &page_info->page_list);
			 hv::prevmcall::install_ept_hook(page_info->orig_page_pfn, page_info->exec_page_pfn);
			 succeed = true;
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

		 bool shadowbp_remove_all(HANDLE process_id)
		 {
			 bool removed_any = false;

			 ExAcquireFastMutex(&g_breakpoint_list_lock);

			 for (PLIST_ENTRY page_entry = g_breakpoint_list.Flink;
				 page_entry != &g_breakpoint_list; )
			 {
				 auto* page_info = CONTAINING_RECORD(page_entry, breakpoint_page_info, page_list);
				 page_entry = page_entry->Flink;  

				 if (page_info->process_id != process_id)
				 {
					 continue;
				 }

				 
				 for (PLIST_ENTRY func_entry = page_info->breakpoint_list.Flink;
					 func_entry != &page_info->breakpoint_list; )
				 {
					 auto* func_info = CONTAINING_RECORD(func_entry, breakpoint_function_info, list_entry);
					 func_entry = func_entry->Flink;

					 *reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = func_info->original_byte;
					 func_info->is_active = false;

					 RemoveEntryList(&func_info->list_entry);
					 pool_manager::release_pool(func_info);
				 }

				 
				 RemoveEntryList(&page_info->page_list);
				 hv::prevmcall::remove_ept_hook(page_info->orig_page_pfn);

				 pool_manager::release_pool(page_info->page_contents);
				 pool_manager::release_pool(page_info);

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

			 for (PLIST_ENTRY page_entry = g_breakpoint_list.Flink;
				 page_entry != &g_breakpoint_list;
				 page_entry = page_entry->Flink)
			 {
				 auto* page_info = CONTAINING_RECORD(page_entry, breakpoint_page_info, page_list);

				 if (page_info->process_id != process_id)
				 {
					 continue;
				 }

				 for (PLIST_ENTRY func_entry = page_info->breakpoint_list.Flink;
					 func_entry != &page_info->breakpoint_list;
					 func_entry = func_entry->Flink)
				 {
					 auto* func_info = CONTAINING_RECORD(func_entry, breakpoint_function_info, list_entry);

					 if (func_info->original_va == target_address)
					 {
						  
						 *reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = func_info->original_byte;
						 func_info->is_active = false;

						 RemoveEntryList(&func_info->list_entry);
						 --page_info->breakpoint_count;

						 if (page_info->breakpoint_count == 0)
						 {
							 RemoveEntryList(&page_info->page_list);
							 hv::prevmcall::remove_ept_hook(page_info->orig_page_pfn);
							 pool_manager::release_pool(page_info->page_contents);
							 pool_manager::release_pool(page_info);
						 }

						 pool_manager::release_pool(func_info);


						 removed = true;
						 goto done;
					 }
				 }
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

			 for (PLIST_ENTRY page_entry = g_breakpoint_list.Flink;
				 page_entry != &g_breakpoint_list;
				 page_entry = page_entry->Flink)
			 {
				 auto* page_info = CONTAINING_RECORD(page_entry, breakpoint_page_info, page_list);

				 if (page_info->process_id != process_id)
				 {
					 continue;
				 }

				 for (PLIST_ENTRY func_entry = page_info->breakpoint_list.Flink;
					 func_entry != &page_info->breakpoint_list;
					 func_entry = func_entry->Flink)
				 {
					 auto* func_info = CONTAINING_RECORD(func_entry, breakpoint_function_info, list_entry);

					 if (func_info->original_va == target_address)
					 {
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

						 goto done;
					 }
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
			 void* target_address ,
			 breakpoint_function_info* out_bp_info)
		 {
			 ExAcquireFastMutex(&g_breakpoint_list_lock);

			 LIST_ENTRY* page_list = &g_breakpoint_list;
			 for (PLIST_ENTRY page_entry = page_list->Flink;
				 page_entry != page_list;
				 page_entry = page_entry->Flink)
			 {
				 auto* bp_page = CONTAINING_RECORD(page_entry, breakpoint_page_info, page_list);

				 if (bp_page->process_id != process_id)
				 {
					 continue;
				 }

				 for (PLIST_ENTRY func_entry = bp_page->breakpoint_list.Flink;
					 func_entry != &bp_page->breakpoint_list;
					 func_entry = func_entry->Flink)
				 {
					 auto* bp_info = CONTAINING_RECORD(func_entry, breakpoint_function_info, list_entry);

					 if (reinterpret_cast<void*>(target_address) == bp_info->original_va)
					 {
						 *out_bp_info = *bp_info;   
						 ExReleaseFastMutex(&g_breakpoint_list_lock);
						 return true;
					 }
				 }
			 }

			 ExReleaseFastMutex(&g_breakpoint_list_lock);
			 return false;
		 }

 


 

	}
}
