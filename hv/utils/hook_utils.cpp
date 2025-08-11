#include "global_defs.h"
#include "hook_utils.h"
#include "fault_uitls.h"
#include "../LDE64.h"
namespace utils
{
	namespace hook_utils
	{
#define GET_PFN(_VAR_) (_VAR_ >> PAGE_SHIFT)
#define MASK_EPT_PML1_OFFSET(_VAR_) ((unsigned __int64)_VAR_ & 0xFFFULL)
		  LIST_ENTRY g_kernel_hook_page_list_head;
		  LIST_ENTRY g_user_hook_page_list_head;

		  FAST_MUTEX g_user_hook_page_list_lock;
		  
		  void initialize_hook_page_lists()
		  {
			  InitializeListHead(&g_kernel_hook_page_list_head);
			  InitializeListHead(&g_user_hook_page_list_head);

			  ExInitializeFastMutex(&g_user_hook_page_list_lock);
			 
			  // 如果后续还有其他链表，也可以在此统一添加初始化逻辑
			  // InitializeListHead(&g_another_list_head);
		  }
		bool hook_kernel_function(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function)
		{

		 
			if (!target_api || !new_api || !origin_function)
			{
				LogError("hook: Invalid parameters. target_api: %p, new_api: %p, origin_function: %p",
					target_api, new_api, origin_function);
				return false;
			}
			if (origin_function)
			{
				*origin_function = nullptr;
			}

			uint64_t target_pa{};
			uint64_t target_page_pfn{};
			unsigned __int64 page_offset{};
			const bool is_intel = utils::khyper_vt::is_intel_cpu();


			 page_offset = MASK_EPT_PML1_OFFSET((unsigned __int64)target_api);
			 void* target_api_page_base = PAGE_ALIGN(target_api);

			target_pa = utils::internal_functions::pfn_mm_get_physical_address(target_api_page_base).QuadPart;
			if (target_pa == 0)
			{
				return false;

			}
			target_page_pfn = GET_PFN(target_pa);

			PLIST_ENTRY current = &g_kernel_hook_page_list_head;
			while (&g_kernel_hook_page_list_head != current->Flink)
			{
				current = current->Flink;
				kernel_hook_info* hooked_page_info = CONTAINING_RECORD(current, kernel_hook_info, hooked_page_list);
				if (hooked_page_info->pfn_of_hooked_page == target_page_pfn)
				{
					LogInfo("Page already hooked");

					 
					hooked_function_info* hook_info = reinterpret_cast<hooked_function_info*>(utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, sizeof(hooked_function_info), POOL_TAG));
					if (hook_info == nullptr)
					{
						LogError("There is no pre-allocated pool for hooked function struct");
						return false;
					}
					 
					RtlZeroMemory(hook_info, sizeof(hooked_function_info));
					hook_info->trampoline_va = reinterpret_cast<uint8_t*>(utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, 100, POOL_TAG));

					if (!hook_info->trampoline_va)
					{
						ExFreePool(hook_info);
						return false;

					}
					hook_info->original_instructions_backup = reinterpret_cast<uint8_t*>(utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, 100, POOL_TAG));
					if (hook_info->original_instructions_backup == nullptr)
					{
						ExFreePool(hook_info->trampoline_va);
						ExFreePool(hook_info);
						LogError("There is no pre-allocated pool for trampoline");
						return false;
					}

					RtlZeroMemory(hook_info->trampoline_va, 100);
					RtlZeroMemory(hook_info->original_instructions_backup, 100);

					hook_info->new_handler_va = new_api;
					hook_info->original_va = target_api;
					hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
				 
			  
					hook_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(new_api).QuadPart;
					hook_info->original_pa = target_pa;
					hook_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
					hook_info->fake_page_contents = hooked_page_info->fake_page_contents;
				 
					hook_info->type = hook_jmp_fake_page;



					if (hook_instruction_memory_int1(hook_info, target_api, page_offset) == false)
					{

						ExFreePool(hook_info->trampoline_va);
						ExFreePool(hook_info->original_instructions_backup);
						ExFreePool(hook_info);
						LogError("Hook failed");
						return false;
					}
					++hooked_page_info->ref_count;
					if (origin_function)
						*origin_function = hook_info->trampoline_va;
					// Track all hooked functions within page
					InsertHeadList(&hooked_page_info->hooked_functions_list, &hook_info->hooked_function_list);

					return true;
				}
			}
			 

			kernel_hook_info* hooked_page_info = reinterpret_cast<kernel_hook_info*>(utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, sizeof(kernel_hook_info), POOL_TAG));
			if (hooked_page_info == nullptr)
			{
				LogError("There is no preallocated pool for hooked page info");
				return false;
			}
			
			RtlZeroMemory(hooked_page_info, sizeof(kernel_hook_info));
		
			hooked_page_info->fake_page_contents = reinterpret_cast<uint8_t*>(utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, PAGE_SIZE, POOL_TAG));
			if (hooked_page_info->fake_page_contents == nullptr)
			{
				ExFreePool(hooked_page_info);
				LogError("There is no preallocated pool for fake page contents");
				return false;
			}


			uint64_t fake_pa = utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart;
			if (fake_pa == 0)
			{

				ExFreePool(hooked_page_info->fake_page_contents);
				ExFreePool(hooked_page_info);
				return false;


			}
		
			 
			

			hooked_function_info* hook_info = reinterpret_cast<hooked_function_info*>(utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, sizeof(hooked_function_info), POOL_TAG));
			if (hook_info == nullptr)
			{
				LogError("There is no pre-allocated pool for hooked function struct");
				return false;
			}
			RtlZeroMemory(hook_info, sizeof(hooked_function_info));

			hook_info->trampoline_va = reinterpret_cast<uint8_t*>(utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, 100, POOL_TAG));

			if (!hook_info->trampoline_va)
			{
				ExFreePool(hook_info);
				return false;

			}
			hook_info->original_instructions_backup = reinterpret_cast<uint8_t*>(utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, 100, POOL_TAG));
			if (hook_info->original_instructions_backup == nullptr)
			{
				ExFreePool(hook_info->trampoline_va);
				ExFreePool(hook_info);
				LogError("There is no pre-allocated pool for trampoline");
				return false;
			}

			RtlZeroMemory(hook_info->trampoline_va, 100);
			RtlZeroMemory(hook_info->original_instructions_backup, 100);
	
			uint64_t  fake_page_pa = utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart;
		    uint64_t  fake_page_pfn = GET_PFN(fake_page_pa);
		 
			hooked_page_info->pfn_of_hooked_page = target_page_pfn;
			hooked_page_info->pfn_of_fake_page_contents = fake_page_pfn;

			hook_info->new_handler_va = new_api;
			hook_info->original_va = target_api;
			hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];


			hook_info->original_pa = target_pa;
			hook_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(new_api).QuadPart;
			hook_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
			hook_info->fake_page_contents = hooked_page_info->fake_page_contents;

			RtlCopyMemory(hooked_page_info->fake_page_contents, PAGE_ALIGN(target_api), PAGE_SIZE);

			hook_info->type = hook_jmp_fake_page;

		 

			if (hook_instruction_memory_int1(hook_info, target_api, page_offset) == false)
			{

				ExFreePool(hook_info->trampoline_va);
				ExFreePool(hook_info->original_instructions_backup);
				ExFreePool(hook_info);
				LogError("Hook failed");
				return false;
			}
			++hooked_page_info->ref_count;
			if (origin_function)
				*origin_function = hook_info->trampoline_va;
		 

			InitializeListHead(&hooked_page_info->hooked_functions_list);


			InsertHeadList(&hooked_page_info->hooked_functions_list, &hook_info->hooked_function_list);
			InsertHeadList(&g_kernel_hook_page_list_head, &hooked_page_info->hooked_page_list);
			
			if (!hvgt::hook_function(__readcr3(), hooked_page_info->pfn_of_hooked_page, hooked_page_info->pfn_of_fake_page_contents))
			{
				LogError("hook: Failed to hook target_api: %p -> new_api: %p, fake_page: %p, trampoline: %p",
					target_api, new_api, hook_info->fake_va, *origin_function);
				return false;
				
			}
			 
			LogError("hook: Successfully hooked target_api: %p -> new_api: %p, fake_page: %p, trampoline: %p",
				target_api, new_api, hook_info->fake_va, *origin_function);
		 
			return true;
		 
			 
		}


		bool hook_user_exception_handler(
			_In_ PEPROCESS process,
			_In_ void* target_api,
			_In_ void* exception_handler,
			_In_ bool allocate_trampoline_page)
		{
			 

			if (!process || !target_api || !exception_handler)
				return false;

			const HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);
			const ULONGLONG target_cr3 = utils::process_utils::get_process_cr3(process);
			if (target_cr3 == 0)
			{
				return false;
			}

			const bool is_intel = utils::khyper_vt::is_intel_cpu();
			unsigned char* trampoline_va = nullptr;
			PMDL mdl = nullptr;
			KAPC_STATE apc_state{};
			bool apc_attached = false;
			bool result = false;

			// Attach 到目标进程
			utils::internal_functions::pfn_ke_stack_attach_process(process, &apc_state);
			apc_attached = true;
			void* target_api_page_base = PAGE_ALIGN(target_api);
		 

			// 锁页确保页面存在
			//if (!NT_SUCCESS(utils::memory::lock_memory( reinterpret_cast<unsigned  long long> (target_api_page_base), PAGE_SIZE, &mdl))) {
			//	goto CLEANUP;
			//}

			// 分配 trampoline（可选）
			if (allocate_trampoline_page) {
				if (!NT_SUCCESS(utils::memory::allocate_user_hidden_exec_memory(
					process, reinterpret_cast<PVOID*>(&trampoline_va), PAGE_SIZE))) {
					goto CLEANUP;
				}
			}

		 
		 

			// 获取 target API 物理页信息
			uint64_t target_pa = utils::internal_functions::pfn_mm_get_physical_address(target_api_page_base).QuadPart;
			if (!target_pa)
			{
				goto CLEANUP;
			}

			uint64_t page_offset = MASK_EPT_PML1_OFFSET((uintptr_t)target_api);
			uint64_t target_page_pfn = GET_PFN(target_pa);

			ExAcquireFastMutex(&g_user_hook_page_list_lock);
			// 搜索是否已 hook 页面
			PLIST_ENTRY current = g_user_hook_page_list_head.Flink;
			while (current != &g_user_hook_page_list_head) {
				kernel_hook_info* hooked_page_info = CONTAINING_RECORD(current, kernel_hook_info, hooked_page_list);
				if (hooked_page_info->pfn_of_hooked_page == target_page_pfn) {

					LogInfo("Page already hooked");
					hooked_function_info* hook_info = reinterpret_cast<hooked_function_info*>(
						utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, sizeof(hooked_function_info), POOL_TAG));
					if (!hook_info) {
						LogError("No memory for hooked_function_info");
						goto CLEANUP;
					}
					RtlZeroMemory(hook_info, sizeof(hooked_function_info));

					hook_info->trampoline_va = trampoline_va;
					if (trampoline_va)
					{
						RtlZeroMemory(trampoline_va, 100);
					}

					hook_info->original_instructions_backup = reinterpret_cast<uint8_t*>(
						utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, 100, POOL_TAG));
					if (!hook_info->original_instructions_backup) {
						ExFreePool(hook_info);
						goto CLEANUP;
					}
					RtlZeroMemory(hook_info->original_instructions_backup, 100);

					hook_info->new_handler_va = exception_handler;
					hook_info->original_va = target_api;
					hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
					hook_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(exception_handler).QuadPart;
					hook_info->original_pa = target_pa;
					hook_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(hook_info->fake_va).QuadPart;
					hook_info->fake_page_contents = hooked_page_info->fake_page_contents;
					hook_info->type = hook_user_exception_break_point_int3;

					if (!hook_instruction_memory_int3(hook_info, target_api, page_offset)) {
						ExFreePool(hook_info->original_instructions_backup);
						ExFreePool(hook_info);
						goto CLEANUP;
					}

					++hooked_page_info->ref_count;
					InsertHeadList(&hooked_page_info->hooked_functions_list, &hook_info->hooked_function_list);

					 
					result = true;
					goto CLEANUP;
				}
				current = current->Flink;
			}

			
			kernel_hook_info* hooked_page_info = reinterpret_cast<kernel_hook_info*>(
				utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, sizeof(kernel_hook_info), POOL_TAG));
			if (!hooked_page_info)
			{
				goto CLEANUP;

			}
			RtlZeroMemory(hooked_page_info, sizeof(kernel_hook_info));

			hooked_page_info->fake_page_contents = reinterpret_cast<uint8_t*>(
				utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, PAGE_SIZE, POOL_TAG));
			if (!hooked_page_info->fake_page_contents) {
				ExFreePool(hooked_page_info);
				goto CLEANUP;
			}

			uint64_t fake_pa = utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart;
			if (!fake_pa) {
				ExFreePool(hooked_page_info->fake_page_contents);
				ExFreePool(hooked_page_info);
				goto CLEANUP;
			}

			hooked_function_info* hook_info = reinterpret_cast<hooked_function_info*>(
				utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, sizeof(hooked_function_info), POOL_TAG));
			if (!hook_info) {
				ExFreePool(hooked_page_info->fake_page_contents);
				ExFreePool(hooked_page_info);
				goto CLEANUP;
			}
			RtlZeroMemory(hook_info, sizeof(hooked_function_info));

			hook_info->trampoline_va = trampoline_va;
			if (trampoline_va) 
			{
				RtlZeroMemory(trampoline_va, 100);
			}

			hook_info->original_instructions_backup = reinterpret_cast<uint8_t*>(
				utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, 100, POOL_TAG));
			if (!hook_info->original_instructions_backup) {
				ExFreePool(hook_info);
				ExFreePool(hooked_page_info->fake_page_contents);
				ExFreePool(hooked_page_info);
				goto CLEANUP;
			}
			RtlZeroMemory(hook_info->original_instructions_backup, 100);

			RtlCopyMemory(hooked_page_info->fake_page_contents, PAGE_ALIGN(target_api), PAGE_SIZE);

			hooked_page_info->pfn_of_hooked_page = target_page_pfn;
			hooked_page_info->pfn_of_fake_page_contents = GET_PFN(fake_pa);
			hooked_page_info->process_id = process_id;

			hook_info->new_handler_va = exception_handler;
			hook_info->original_va = target_api;
			hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
			hook_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(exception_handler).QuadPart;
			hook_info->original_pa = target_pa;
			hook_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(hook_info->fake_va).QuadPart;
			hook_info->fake_page_contents = hooked_page_info->fake_page_contents;
			hook_info->type = hook_user_exception_break_point_int3;

			if (!hook_instruction_memory_int3(hook_info, target_api, page_offset)) {
				ExFreePool(hook_info->original_instructions_backup);
				ExFreePool(hook_info);
				ExFreePool(hooked_page_info->fake_page_contents);
				ExFreePool(hooked_page_info);
				goto CLEANUP;
			}

			++hooked_page_info->ref_count;
			InitializeListHead(&hooked_page_info->hooked_functions_list);
			InsertHeadList(&hooked_page_info->hooked_functions_list, &hook_info->hooked_function_list);
			InsertHeadList(&g_user_hook_page_list_head, &hooked_page_info->hooked_page_list);
			
			result = hvgt::hook_function(target_cr3, target_page_pfn, hooked_page_info->pfn_of_fake_page_contents);
			if (!result) {
				LogError("hook: Failed to hook target_api: %p", target_api);
			}
			else {
				LogInfo("hook: Success target_api: %p -> handler: %p", target_api, exception_handler);
			}

		CLEANUP:
			ExReleaseFastMutex(&g_user_hook_page_list_lock);
	/*		if (mdl) {
				utils::memory::unlock_memory(mdl);
			}*/

			if (!result && trampoline_va) {
				utils::memory::free_user_memory(process_id, trampoline_va, PAGE_SIZE);
			}

		 

			if (apc_attached) {
				utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			}

			return result;
		}

 
void write_int3(uint8_t* address)
{
	*address = 0xCC; // INT3
}
	 void write_int1(uint8_t* address)
	{
		*address = 0xF1; // INT1 (ICEBP)
	}

	 	void hook_write_absolute_jump(unsigned __int8* target_buffer, unsigned __int64 destination_address)
	{
		// push lower 32 bits of destination address	
		target_buffer[0] = 0x68;
		*((unsigned __int32*)&target_buffer[1]) = (unsigned __int32)destination_address;

		// mov dword ptr [rsp + 4]
		target_buffer[5] = 0xc7;
		target_buffer[6] = 0x44;
		target_buffer[7] = 0x24;
		target_buffer[8] = 0x04;

		// higher 32 bits of destination address	
		*((unsigned __int32*)&target_buffer[9]) = (unsigned __int32)(destination_address >> 32);

		// ret
		target_buffer[13] = 0xc3;

	}

			void  hook_write_absolute_jump_int3(unsigned __int8* target_buffer, unsigned __int64 destination_address)
	{
		PEPROCESS process = utils::internal_functions::pfn_ps_get_current_process();
		bool is_64_bit = utils::process_utils::is_process_64_bit(process);
		if (is_64_bit)
		{
			// push lower 32 bits of destination address	
			target_buffer[0] = 0x68;
			*((unsigned __int32*)&target_buffer[1]) = (unsigned __int32)destination_address;

			// mov dword ptr [rsp + 4]
			target_buffer[5] = 0xc7;
			target_buffer[6] = 0x44;
			target_buffer[7] = 0x24;
			target_buffer[8] = 0x04;

			// higher 32 bits of destination address	
			*((unsigned __int32*)&target_buffer[9]) = (unsigned __int32)(destination_address >> 32);

			// ret
			target_buffer[13] = 0xc3;
		}
		else
		{
			// push imm32
			target_buffer[0] = 0x68;
			*((unsigned __int32*)&target_buffer[1]) = (unsigned __int32)destination_address;

			// ret
			target_buffer[5] = 0xC3;
		}
	}

	 bool hook_instruction_memory_int1(_Inout_ hooked_function_info* hooked_function_info, void* target_function, unsigned __int64 page_offset)
	 {
		 unsigned __int64 hooked_instructions_size = LDE(target_function, 64);

		 hooked_function_info->hook_size = hooked_instructions_size;

		 // Copy overwritten instructions to trampoline buffer
		 RtlCopyMemory(hooked_function_info->trampoline_va, target_function, hooked_instructions_size);
		 RtlCopyMemory(hooked_function_info->original_instructions_backup, target_function, hooked_instructions_size);
		 // Add the absolute jump back to the original function.
		 hook_write_absolute_jump(&hooked_function_info->trampoline_va[hooked_instructions_size], (unsigned __int64)target_function + hooked_instructions_size);

		 // Write icebp instruction to our shadow page memory to trigger vmexit.
		 write_int1(&hooked_function_info->fake_page_contents[page_offset]);



		 return true;
	 }
	 
	 	bool hook_instruction_memory_int3(_Inout_ hooked_function_info* hooked_function_info, void* target_function,  unsigned __int64 page_offset)
	{
		 

	 

		unsigned __int64 hooked_instructions_size = LDE(target_function, 64);
		hooked_function_info->hook_size = hooked_instructions_size;

		RtlCopyMemory(hooked_function_info->original_instructions_backup, target_function, hooked_instructions_size);

		if (hooked_function_info->trampoline_va)
		{
			RtlCopyMemory(hooked_function_info->trampoline_va, target_function, hooked_instructions_size);

			hook_write_absolute_jump_int3(
				&hooked_function_info->trampoline_va[hooked_instructions_size],
				(unsigned __int64)target_function + hooked_instructions_size
			);
		}

		 

		write_int3(&hooked_function_info->fake_page_contents[page_offset]);

		return true;
	}



	 bool find_hook_info_by_rip(
		 void* rip,
		 hooked_function_info** out_hook_info)
	 {
		 LIST_ENTRY* hook_page_list =&g_kernel_hook_page_list_head;
		 for (PLIST_ENTRY page_entry = hook_page_list->Flink;
			 page_entry != hook_page_list;
			 page_entry = page_entry->Flink)
		 {
			 auto* hooked_page = CONTAINING_RECORD(page_entry, kernel_hook_info, hooked_page_list);


			 for (PLIST_ENTRY func_entry = hooked_page->hooked_functions_list.Flink;
				 func_entry != &hooked_page->hooked_functions_list;
				 func_entry = func_entry->Flink)
			 {

				 auto* hook_info = CONTAINING_RECORD(func_entry, hooked_function_info, hooked_function_list);

				 if (reinterpret_cast<void*>(rip) == hook_info->original_va)
				 {
					 *out_hook_info = hook_info;
					 return true;
				 }
			 }
		 }

		 return false;
	 }
	  
	 bool find_user_exception_info_by_rip(
		 HANDLE process_id,
		 void* rip,
		 hooked_function_info * out_hook_info)
	 {

		 ExAcquireFastMutex(&g_user_hook_page_list_lock);
		 LIST_ENTRY* hook_page_list = &g_user_hook_page_list_head;
		 for (PLIST_ENTRY page_entry = hook_page_list->Flink;
			 page_entry != hook_page_list;
			 page_entry = page_entry->Flink)
		 {


			 auto* hooked_page = CONTAINING_RECORD(page_entry, kernel_hook_info, hooked_page_list);

			 if (hooked_page->process_id != process_id)
			 {
				 continue;
			 }
			 for (PLIST_ENTRY func_entry = hooked_page->hooked_functions_list.Flink;
				 func_entry != &hooked_page->hooked_functions_list;
				 func_entry = func_entry->Flink)
			 {

				 auto* hook_info = CONTAINING_RECORD(func_entry, hooked_function_info, hooked_function_list);

				 if (reinterpret_cast<void*>(rip) == hook_info->original_va)
				 {
					 *out_hook_info = *hook_info;
					 ExReleaseFastMutex(&g_user_hook_page_list_lock);
					 return true;
				 }
			 }
		 }
		 ExReleaseFastMutex(&g_user_hook_page_list_lock);
		 return false;
		  
	 }

	 void safe_wait_for_zero_call_count(volatile LONG* call_count_ptr)
	 {
		 

		 while (InterlockedCompareExchange(call_count_ptr, 0, 0) > 0)
		 {
			 // 等待调用计数为0，防止卸载时函数正在执行
			 LogDebug("[HOOK] Waiting for call count to reach zero, current: %ld\n", *call_count_ptr);
			 utils::thread_utils::sleep_ms(10);
			 // 或者用 Sleep(100); 需要适合内核模式的等待函数
		 }
	 }
	 bool remove_user_exception_handler(_In_ PEPROCESS process)
	 {
		 if (!process)
		 {
			 return false;
		 }

		 const HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);
		 const ULONGLONG target_cr3 = utils::process_utils::get_process_cr3(process);

		 if (target_cr3 == 0 || process_id == nullptr)
		 {
			 return false;
		 }

		 bool unhooked_any = false;

		 ExAcquireFastMutex(&g_user_hook_page_list_lock);
		 PLIST_ENTRY current_page = g_user_hook_page_list_head.Flink;

		 while (current_page != &g_user_hook_page_list_head)
		 {
			 PLIST_ENTRY next_page = current_page->Flink;
			 kernel_hook_info* hooked_page_info = CONTAINING_RECORD(current_page, kernel_hook_info, hooked_page_list);

			 if (hooked_page_info->process_id != process_id)
			 {
				 current_page = next_page;
				 continue;
			 }
			
			 unhooked_any = true;

			 // 遍历该页所有hook的函数
			 PLIST_ENTRY current_func = hooked_page_info->hooked_functions_list.Flink;
			 bool page_changed = false;

			 while (current_func != &hooked_page_info->hooked_functions_list)
			 {
				 PLIST_ENTRY next_func = current_func->Flink;
				 _hooked_function_info* hooked_function_info = CONTAINING_RECORD(current_func, _hooked_function_info, hooked_function_list);

				 // 等待调用计数归零，确保无正在调用的hook
				 
				 //进程退出的情况 他引用计数可能没减完
				 // safe_wait_for_zero_call_count(&hooked_function_info->call_count);
				 // 卸载该函数hook
				/*  unsigned __int64 function_page_offset = MASK_EPT_PML1_OFFSET(hooked_function_info->original_va);
				  RtlCopyMemory(&hooked_function_info->fake_page_contents[function_page_offset], hooked_function_info->original_va, hooked_function_info->hook_size);



				  if (hooked_function_info->trampoline_va != nullptr)
				  {
					  utils::memory::free_user_memory(process_id, hooked_function_info->trampoline_va, PAGE_SIZE);
				  }*/

				 if (hooked_function_info->original_instructions_backup != nullptr)
				 {
					 ExFreePool(hooked_function_info->original_instructions_backup);
				 }

				 ExFreePool(hooked_function_info);

				 // 页面hook计数减1
				 if (hooked_page_info->ref_count > 0)
				 {
					 InterlockedDecrement((volatile LONG*)&hooked_page_info->ref_count);
				 }
				 RemoveEntryList(current_func);
				 page_changed = true;
				 current_func = next_func;
			 }

			 // 页面hook计数为0时，调用hvgt::remove_hook恢复原始页面映射
			 if (page_changed && hooked_page_info->ref_count == 0)
			 {
				 hvgt::remove_hook(target_cr3, hooked_page_info->pfn_of_hooked_page);

				 ExFreePool(hooked_page_info->fake_page_contents);
				 RemoveEntryList(current_page);
				 ExFreePool(hooked_page_info);
				 LogInfo("Successfully unhooked and restored page for process (pid: %Iu).", (ULONG_PTR)process_id);
		  }

			 current_page = next_page;
		 }
		 ExReleaseFastMutex(&g_user_hook_page_list_lock);
		 return unhooked_any;

		 
		  
	 }
		 
	 }
}