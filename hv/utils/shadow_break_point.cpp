#include "global_defs.h"
#include "shadow_break_point.h"

 

namespace utils {
	namespace shadowbreakpoint {
		 
		LIST_ENTRY g_breakpoint_list{};
		FAST_MUTEX g_breakpoint_list_lock{};

	     NTSTATUS shadowbp_initialize()
		{
			 NTSTATUS status = STATUS_SUCCESS;
			InitializeListHead(&g_breakpoint_list);
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
					 func_info->instruction_size = LDE(target_address, 64);
					 func_info->is_active = true;
					 RtlCopyMemory(&func_info->original_byte, target_address, 1);
					 *reinterpret_cast<uint8_t*>(func_info->breakpoint_va) = 0xCC;// INT3
					 ++page_info->breakpoint_count;

					 InsertHeadList(&page_info->breakpoint_list, &func_info->list_entry);
					 succeed = true;
					 goto clear;
				 }
			 }



			 auto* page_info = pool_manager::request_pool<breakpoint_page_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(breakpoint_page_info));
			 if (!page_info)
			 {
				 goto clear;
			 }

			 page_info->page_contents = pool_manager::request_pool<uint8_t*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, TRUE, PAGE_SIZE);
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


			 auto* func_info = pool_manager::request_pool<breakpoint_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(breakpoint_function_info));
			 if (!func_info)
			 {
				 pool_manager::release_pool(page_info->page_contents);
				 pool_manager::release_pool(page_info);
				 goto clear;
			 }

			 void* aligned_target = PAGE_ALIGN(target_address);
			 RtlCopyMemory(page_info->page_contents, aligned_target, PAGE_SIZE);

			 auto  exec_page = utils::internal_functions::pfn_mm_get_physical_address(page_info->page_contents).QuadPart;

			 page_info->process_id = process_id;
			 page_info->orig_page_pfn = GET_PFN(target_pa);
			 page_info->exec_page_pfn = GET_PFN(exec_page);
			 func_info->handler_va = handler;
			 func_info->original_va = target_address;
			 func_info->original_pa = target_pa;
			 func_info->breakpoint_va = &page_info->page_contents[page_offset];
			 func_info->instruction_size = LDE(target_address, 64);
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
						 goto clear;

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
					 succeed = true;
					 goto clear;
				 }
			 }



			 auto* page_info = pool_manager::request_pool<breakpoint_page_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(breakpoint_page_info));
			 if (!page_info)
			 {
				 goto clear;
			 }

			 page_info->page_contents = pool_manager::request_pool<uint8_t*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, TRUE, PAGE_SIZE);
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


			 auto* func_info = pool_manager::request_pool<breakpoint_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(breakpoint_function_info));
			 if (!func_info)
			 {
				 pool_manager::release_pool(page_info->page_contents);
				 pool_manager::release_pool(page_info);
				 goto clear;
			 }

			 void* aligned_target = PAGE_ALIGN(target_address);
			 RtlCopyMemory(page_info->page_contents, aligned_target, PAGE_SIZE);

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
			 breakpoint_function_info** out_bp_info)
		 {
			 

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

					 if (reinterpret_cast<void*>(target_address) == bp_info->original_va &&bp_info->is_active)
					 {
						 *out_bp_info = bp_info;   
					 
						 return true;
					 }
				 }
			 }

		 
			 return false;
		 }

		 bool shadowbp_find_tf_address(
			 HANDLE process_id,
			 void* target_address,
			 breakpoint_function_info** out_bp_info)
		 {
			 


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
                    
					  void* tf_address = reinterpret_cast<void*>(reinterpret_cast<uint64_t>(target_address) - bp_info->instruction_size);
					 if (reinterpret_cast<void*>(tf_address) == bp_info->original_va &&bp_info->is_active)
					 {
						 *out_bp_info = bp_info;   
					 
						 return true;
					 }
				 }
			 }

			 
			 return false;
		 }



 

	}
}
