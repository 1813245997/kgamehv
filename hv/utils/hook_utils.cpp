#include "global_defs.h"
#include "hook_utils.h"
#include "fault_uitls.h"
#include "../LDE64.h"
#include "../vtx/prevmcall.h"
#include "../vtx/poolmanager.h"
namespace utils
{
	namespace hook_utils
	{
#define GET_PFN(_VAR_) (_VAR_ >> PAGE_SHIFT)
#define MASK_EPT_PML1_OFFSET(_VAR_) ((unsigned __int64)_VAR_ & 0xFFFULL)
		LIST_ENTRY g_kernel_hook_page_list_head{};
		LIST_ENTRY g_user_hook_list{};
		LIST_ENTRY g_ept_breakpoint_hook_list{};
		FAST_MUTEX g_user_hook_page_list_lock;
		FAST_MUTEX g_ept_breakpoint_hook_list_lock;
		  void initialize_hook_page_lists()
		  {
			  InitializeListHead(&g_kernel_hook_page_list_head);

			  InitializeListHead(&g_ept_breakpoint_hook_list);

			  InitializeListHead(&g_user_hook_list);

			  ExInitializeFastMutex(&g_ept_breakpoint_hook_list_lock);

			  ExInitializeFastMutex(&g_user_hook_page_list_lock);
			 
			  // 如果后续还有其他链表，也可以在此统一添加初始化逻辑
			  // InitializeListHead(&g_another_list_head);
		  }
		bool hook_kernel_function(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function)
		{

		 
			if (!target_api || !new_api)
				return false;

			if (origin_function)
				*origin_function = nullptr;

			uint64_t target_pa = utils::internal_functions::pfn_mm_get_physical_address(target_api).QuadPart;
			if (target_pa == 0)
				return false;



			unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET((unsigned __int64)target_api);



			for (PLIST_ENTRY entry = g_kernel_hook_page_list_head.Flink;
				entry != &g_kernel_hook_page_list_head;
				entry = entry->Flink)
			{
				kernel_hook_info* hooked_page_info = CONTAINING_RECORD(entry, kernel_hook_info, hooked_page_list);

				if (hooked_page_info->pfn_of_hooked_page == GET_PFN(target_pa))
				{

					auto* hook_info = pool_manager::request_pool<hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(hooked_function_info));   
					if (!hook_info)
					{
						return false;

					}


					hook_info->trampoline_va = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_EXEC_TRAMPOLINE, TRUE, 100);   
					if (!hook_info->trampoline_va)
					{
						pool_manager::release_pool(hook_info);
						return false;
					}

					hook_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_ORIGINAL_INSTRUCTIONS, TRUE, 100);
					if (hook_info->original_instructions_backup == nullptr)
					{
						pool_manager::release_pool(hook_info->trampoline_va);
						pool_manager::release_pool(hook_info);
						LogError("There is no pre-allocated pool for trampoline");
						return false;
					}

					RtlZeroMemory(hook_info->trampoline_va, 100);
					RtlZeroMemory(hook_info->original_instructions_backup, 100);


					hook_info->new_handler_va = new_api;
					hook_info->original_va = target_api;
					hook_info->original_pa = target_pa;
					hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
					hook_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
					hook_info->fake_page_contents = hooked_page_info->fake_page_contents;
					if (origin_function)
						*origin_function = hook_info->trampoline_va;
					++hooked_page_info->ref_count;
					hook_instruction_memory_int1(hook_info, target_api, page_offset);
				 
					InsertHeadList(&hooked_page_info->hooked_functions_list, &hook_info->hooked_function_list);

					return true;
				}
			}



			auto* hooked_page_info = pool_manager::request_pool<kernel_hook_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(kernel_hook_info));   
			if (!hooked_page_info)
			{
				return false;
			}

			hooked_page_info->fake_page_contents = pool_manager::request_pool<uint8_t*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, TRUE,PAGE_SIZE);    
			if (!hooked_page_info->fake_page_contents)
			{
				pool_manager::release_pool(hooked_page_info);
				return false;
			}
			uint64_t fake_pa = utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart;
			if (fake_pa == 0)
			{
				pool_manager::release_pool(hooked_page_info->fake_page_contents);
				pool_manager::release_pool(hooked_page_info);

				return false;

			}
		

			auto* hook_info = pool_manager::request_pool<hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(hooked_function_info));
			if (!hook_info)
			{
				pool_manager::release_pool(hooked_page_info->fake_page_contents);
				pool_manager::release_pool(hooked_page_info);
				return false;
			}


			hook_info->trampoline_va = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_EXEC_TRAMPOLINE, TRUE, 100);

			if (!hook_info->trampoline_va)
			{
				pool_manager::release_pool(hook_info);
				return false;

			}
			hook_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_ORIGINAL_INSTRUCTIONS, TRUE, 100);
			if (hook_info->original_instructions_backup == nullptr)
			{
				pool_manager::release_pool(hook_info->trampoline_va);
				pool_manager::release_pool(hook_info);
				 
				LogError("There is no pre-allocated pool for trampoline");
				return false;
			}
 

			RtlCopyMemory(hooked_page_info->fake_page_contents, PAGE_ALIGN(target_api), PAGE_SIZE);



			hooked_page_info->process_id = utils::internal_functions::pfn_ps_get_current_process_id();
			hooked_page_info->pfn_of_hooked_page = GET_PFN(target_pa);
			hooked_page_info->pfn_of_fake_page_contents = GET_PFN(utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart);
			hooked_page_info->is_user = false;
			hook_info->new_handler_va = new_api;
			hook_info->original_va = target_api;
			hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
			hook_info->original_pa = target_pa;
			hook_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
			hook_info->fake_page_contents = hooked_page_info->fake_page_contents;
			++hooked_page_info->ref_count;
			if (origin_function)
				*origin_function = hook_info->trampoline_va;

			hook_instruction_memory_int1(hook_info, target_api, page_offset);

			InitializeListHead(&hooked_page_info->hooked_functions_list);
			InsertHeadList(&hooked_page_info->hooked_functions_list, &hook_info->hooked_function_list);
			InsertHeadList(&g_kernel_hook_page_list_head, &hooked_page_info->hooked_page_list);
		 

			prevmcall::install_ept_hook(hooked_page_info->pfn_of_hooked_page, hooked_page_info->pfn_of_fake_page_contents);
			 
			LogInfo("hook: Successfully hooked target_api: %p -> new_api: %p, fake_page: %p, trampoline: %p",
				target_api, new_api, hook_info->fake_va, *origin_function);
		 
			return true;
		 
			 
		}



		bool hook_user_hook_handler(
			_In_ PEPROCESS process,
			_In_ void* target_api,
			_In_ void* new_api,
			_Out_ void** origin_function)
		{



			if (!process || !target_api  )
				return false;

			const bool is_intel = utils::khyper_vt::is_intel_cpu();
			const HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);
			const ULONGLONG target_cr3 = utils::process_utils::get_process_cr3(process);
			if (target_cr3 == 0)
			{
				return false;
			}


			KAPC_STATE apc_state{};
			uint64_t target_pa{};
			bool is_64_bit = false;
			bool succeed = false;
			uintptr_t trampoline_va = {};
			is_64_bit = utils::process_utils::is_process_64_bit(process);

			utils::internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET((unsigned __int64)target_api);

			target_pa = utils::internal_functions::pfn_mm_get_physical_address(target_api).QuadPart;
			if (target_pa == 0)
			{
				goto clear;

			}

			 
			ExAcquireFastMutex(&g_user_hook_page_list_lock);

			for (PLIST_ENTRY entry = g_user_hook_list.Flink;
				entry != &g_user_hook_list;
				entry = entry->Flink)
			{
				kernel_hook_info* hooked_page_info = CONTAINING_RECORD(entry, kernel_hook_info, hooked_page_list);

				if (hooked_page_info->pfn_of_hooked_page == GET_PFN(target_pa) && hooked_page_info->process_id == process_id)
				{

					auto* hook_info = pool_manager::request_pool<hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(hooked_function_info));
					if (!hook_info)
					{
						return false;

					}

					if (!NT_SUCCESS(utils::memory::allocate_user_hidden_exec_memory(process, reinterpret_cast<PVOID*> (&hook_info->trampoline_va), 100)))
					{

						pool_manager::release_pool(hook_info);
						goto clear;

					}
					trampoline_va = reinterpret_cast<uint64_t> (hook_info->trampoline_va);
					 
					 

					hook_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_ORIGINAL_INSTRUCTIONS, TRUE, 100);
					if (hook_info->original_instructions_backup == nullptr)
					{
						 
						pool_manager::release_pool(hook_info);

						LogError("There is no pre-allocated pool for trampoline");
						goto clear;
					}
					 

					hook_info->new_handler_va = new_api;
					hook_info->original_va = target_api;
					hook_info->original_pa = target_pa;
					hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
					hook_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
					hook_info->fake_page_contents = hooked_page_info->fake_page_contents;
					*origin_function = hook_info->trampoline_va;
					++hooked_page_info->ref_count;
					hook_instruction_memory_int1(hook_info, target_api, page_offset);

				 
					InsertHeadList(&hooked_page_info->hooked_functions_list, &hook_info->hooked_function_list);


					goto clear;
				}
			}



			auto* hooked_page_info = pool_manager::request_pool<kernel_hook_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(kernel_hook_info));
			if (!hooked_page_info)
			{
				return false;
			}

			hooked_page_info->fake_page_contents = pool_manager::request_pool<uint8_t*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, TRUE, PAGE_SIZE);
			if (!hooked_page_info->fake_page_contents)
			{
				pool_manager::release_pool(hooked_page_info);
				return false;
			}
			uint64_t fake_pa = utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart;
			if (fake_pa == 0)
			{
				pool_manager::release_pool(hooked_page_info->fake_page_contents);
				pool_manager::release_pool(hooked_page_info);

				return false;

			}
			 


			auto* hook_info = pool_manager::request_pool<hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(hooked_function_info));
			if (!hook_info)
			{
				pool_manager::release_pool(hooked_page_info->fake_page_contents);
				pool_manager::release_pool(hooked_page_info);
				return false;
			}
			 
			if (!NT_SUCCESS(utils::memory::allocate_user_hidden_exec_memory(process, reinterpret_cast<PVOID*> (&hook_info->trampoline_va), 100)))
			{
					pool_manager::release_pool(hooked_page_info->fake_page_contents);
					pool_manager::release_pool(hooked_page_info);
					pool_manager::release_pool(hook_info);
					goto clear;

			}
		 
		 


			hook_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_ORIGINAL_INSTRUCTIONS, TRUE, 100);
			if (hook_info->original_instructions_backup == nullptr)
			{
				pool_manager::release_pool(hook_info->trampoline_va);
				pool_manager::release_pool(hook_info);

				LogError("There is no pre-allocated pool for trampoline");
				return false;
			}



			RtlCopyMemory(hooked_page_info->fake_page_contents, PAGE_ALIGN(target_api), PAGE_SIZE);



			hooked_page_info->process_id = process_id;
			hooked_page_info->pfn_of_hooked_page = GET_PFN(target_pa);
			hooked_page_info->pfn_of_fake_page_contents = GET_PFN(utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart);
			hooked_page_info->is_user = true;
			hook_info->new_handler_va = new_api;
			hook_info->original_va = target_api;
			hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
			hook_info->original_pa = target_pa;
			hook_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
			hook_info->fake_page_contents = hooked_page_info->fake_page_contents;
			trampoline_va = reinterpret_cast<uint64_t> (hook_info->trampoline_va);
			*origin_function = hook_info->trampoline_va;
			++hooked_page_info->ref_count;




			if (!hook_instruction_memory_int1(hook_info, target_api, page_offset))
			{
				pool_manager::release_pool(hooked_page_info->fake_page_contents);
				pool_manager::release_pool(hooked_page_info);
				pool_manager::release_pool(hook_info->original_instructions_backup);
				utils::memory::free_user_memory(process_id, hook_info->trampoline_va, 100);
				pool_manager::release_pool(hook_info);
				goto clear;
			}

			prevmcall::install_ept_hook(hooked_page_info->pfn_of_hooked_page, hooked_page_info->pfn_of_fake_page_contents);


			InitializeListHead(&hooked_page_info->hooked_functions_list);
			InsertHeadList(&hooked_page_info->hooked_functions_list, &hook_info->hooked_function_list);
			InsertHeadList(&g_user_hook_list, &hooked_page_info->hooked_page_list);


			succeed = true;
		clear:
			ExReleaseFastMutex(&g_user_hook_page_list_lock);
			utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
		 

			if (succeed) {
				LogInfo("Hook info: target VA=%p, new handler VA=%p, trampoline VA=%p",
					target_api,
					new_api,
					trampoline_va);

			}
			else {
				LogInfo("Hook info not allocated. target VA: %p, handler VA: %p", target_api, new_api);
			}


			return succeed;

		}

		bool hook_user_exception_handler(
			_In_ PEPROCESS process,
			_In_ void* target_api,
			_In_ void* exception_handler,
			_In_ bool allocate_trampoline_page)
		{
			 

			if (!process || !target_api || !exception_handler)
				return false;

			const bool is_intel = utils::khyper_vt::is_intel_cpu();
			const HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);
			const ULONGLONG target_cr3 = utils::process_utils::get_process_cr3(process);
			if (target_cr3 == 0)
			{
				return false;
			}

		 
			KAPC_STATE apc_state{};
			uint64_t target_pa{};
			bool is_64_bit = false;
			bool succeed = false;
			uintptr_t trampoline_va = {};
		
			is_64_bit = utils::process_utils::is_process_64_bit(process);

			utils::internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET((unsigned __int64)target_api);

			target_pa = utils::internal_functions::pfn_mm_get_physical_address(target_api).QuadPart;
			if (target_pa == 0)
			{
				goto clear;

			}

			ExAcquireFastMutex(&g_ept_breakpoint_hook_list_lock);

			for (PLIST_ENTRY entry = g_ept_breakpoint_hook_list.Flink;
				entry != &g_ept_breakpoint_hook_list;
				entry = entry->Flink)
			{
				kernel_hook_info* hooked_page_info = CONTAINING_RECORD(entry, kernel_hook_info, hooked_page_list);

				if (hooked_page_info->pfn_of_hooked_page == GET_PFN(target_pa) && hooked_page_info->process_id == process_id)
				{

					auto* hook_info = pool_manager::request_pool<hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(hooked_function_info));
					if (!hook_info)
					{
						return false;

					}

					if (!NT_SUCCESS(utils::memory::allocate_user_hidden_exec_memory(process, reinterpret_cast<PVOID*> (&hook_info->trampoline_va), 100)))
					{

						pool_manager::release_pool(hook_info);
						goto clear;

					}
					trampoline_va = reinterpret_cast<uint64_t> (hook_info->trampoline_va);



					hook_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_ORIGINAL_INSTRUCTIONS, TRUE, 100);
					if (hook_info->original_instructions_backup == nullptr)
					{

						pool_manager::release_pool(hook_info);

						LogError("There is no pre-allocated pool for trampoline");
						goto clear;
					}


					hook_info->new_handler_va = exception_handler;
					hook_info->original_va = target_api;
					hook_info->original_pa = target_pa;
					hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
					hook_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
					hook_info->fake_page_contents = hooked_page_info->fake_page_contents;
				 
					++hooked_page_info->ref_count;
					hook_instruction_memory_int3(hook_info, target_api, page_offset );

				
					InsertHeadList(&hooked_page_info->hooked_functions_list, &hook_info->hooked_function_list);
					 

					goto clear;
				}
			}



			auto* hooked_page_info = pool_manager::request_pool<kernel_hook_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(kernel_hook_info));
			if (!hooked_page_info)
			{
				return false;
			}

			hooked_page_info->fake_page_contents = pool_manager::request_pool<uint8_t*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, TRUE, PAGE_SIZE);
			if (!hooked_page_info->fake_page_contents)
			{
				pool_manager::release_pool(hooked_page_info);
				return false;
			}
			uint64_t fake_pa = utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart;
			if (fake_pa == 0)
			{
				pool_manager::release_pool(hooked_page_info->fake_page_contents);
				pool_manager::release_pool(hooked_page_info);

				return false;

			}



			auto* hook_info = pool_manager::request_pool<hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(hooked_function_info));
			if (!hook_info)
			{
				pool_manager::release_pool(hooked_page_info->fake_page_contents);
				pool_manager::release_pool(hooked_page_info);
				return false;
			}

			if (!NT_SUCCESS(utils::memory::allocate_user_hidden_exec_memory(process, reinterpret_cast<PVOID*> (&hook_info->trampoline_va), 100)))
			{
				pool_manager::release_pool(hooked_page_info->fake_page_contents);
				pool_manager::release_pool(hooked_page_info);
				pool_manager::release_pool(hook_info);
				goto clear;

			}




			hook_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_ORIGINAL_INSTRUCTIONS, TRUE, 100);
			if (hook_info->original_instructions_backup == nullptr)
			{
				pool_manager::release_pool(hook_info->trampoline_va);
				pool_manager::release_pool(hook_info);

				LogError("There is no pre-allocated pool for trampoline");
				return false;
			}
			 


			RtlCopyMemory(hooked_page_info->fake_page_contents, PAGE_ALIGN(target_api), PAGE_SIZE);
			 
			hooked_page_info->process_id = process_id;
			hooked_page_info->pfn_of_hooked_page = GET_PFN(target_pa);
			hooked_page_info->pfn_of_fake_page_contents = GET_PFN(utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart);
			hooked_page_info->is_user = true;
			hook_info->new_handler_va = exception_handler;
			hook_info->original_va = target_api;
			hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
			hook_info->original_pa = target_pa;
			hook_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
			hook_info->fake_page_contents = hooked_page_info->fake_page_contents;
			trampoline_va = reinterpret_cast<uintptr_t>(hook_info->trampoline_va);
			++hooked_page_info->ref_count;
			 



			if (!hook_instruction_memory_int3(hook_info, target_api, page_offset ))
			{
				pool_manager::release_pool(hooked_page_info->fake_page_contents);
				pool_manager::release_pool(hooked_page_info);
				pool_manager::release_pool(hook_info->original_instructions_backup);
				utils::memory::free_user_memory(process_id, hook_info->trampoline_va, 100);
				pool_manager::release_pool(hook_info);
				goto clear;
			}

			prevmcall::install_ept_hook(hooked_page_info->pfn_of_hooked_page, hooked_page_info->pfn_of_fake_page_contents);


			InitializeListHead(&hooked_page_info->hooked_functions_list);
			InsertHeadList(&hooked_page_info->hooked_functions_list, &hook_info->hooked_function_list);
			InsertHeadList(&g_ept_breakpoint_hook_list, &hooked_page_info->hooked_page_list);
			 
			 
			succeed = true;
		clear:
			ExReleaseFastMutex(&g_ept_breakpoint_hook_list_lock);
			utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
 
			bool is_int3_hook = true; // 如果你使用的是 INT3 断点 hook

			if (succeed) {
				LogInfo(
					"Breakpoint hook installed: target VA=%p, handler VA=%p, trampoline VA=%p, INT3=%s",
					target_api,
					exception_handler,
					(PVOID)trampoline_va,
					is_int3_hook ? "yes" : "no"
				);
			}
			else {
				LogInfo(
					"Hook installation failed: target VA=%p, handler VA=%p",
					target_api,
					exception_handler
				);
			}


			return succeed;

			 
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
		 hooked_function_info * out_hook_info)
	 {

		 if (utils::memory::is_user_address(rip))
		 {
			 //这个地方不可以上APC锁 因为是DPC级别
		 
			 LIST_ENTRY* hook_page_list = &g_user_hook_list;
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
						 *out_hook_info = *hook_info;
						 
				 
						 return true;
					 }
				 }
			 }
		 
			 return false;

		 }
		 else
		 {
			 LIST_ENTRY* hook_page_list = &g_kernel_hook_page_list_head;
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
						 *out_hook_info = *hook_info;
						 return true;
					 }
				 }
			 }

			 return false;
		 }

		
	 }
	  
	 bool find_user_exception_info_by_rip(
		 HANDLE process_id,
		 void* rip,
		 hooked_function_info * out_hook_info)
	 {
		 ExAcquireFastMutex(&g_ept_breakpoint_hook_list_lock);
		 
		 LIST_ENTRY* hook_page_list = &g_ept_breakpoint_hook_list;
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
					 ExReleaseFastMutex(&g_ept_breakpoint_hook_list_lock);
					 return true;
				 }
			 }
		 }
		 ExReleaseFastMutex(&g_ept_breakpoint_hook_list_lock);
		 return false;
		 
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

		 ExAcquireFastMutex(&g_ept_breakpoint_hook_list_lock);
		 PLIST_ENTRY current_page = g_ept_breakpoint_hook_list.Flink;

		 while (current_page != &g_ept_breakpoint_hook_list)
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

			 

				 if (hooked_function_info->original_instructions_backup != nullptr)
				 {
					 pool_manager::release_pool(hooked_function_info->original_instructions_backup);
				 }

				 pool_manager::release_pool(hooked_function_info);

				 
				 RemoveEntryList(current_func);
				 page_changed = true;
				 current_func = next_func;
			 }

			  
			  
			 prevmcall::remove_ept_hook(hooked_page_info->pfn_of_hooked_page);
				 
			 pool_manager::release_pool (hooked_page_info->fake_page_contents);
			 RemoveEntryList(current_page);
			 pool_manager::release_pool(hooked_page_info);
			
			 current_page = next_page;
		 }
		 ExReleaseFastMutex(&g_ept_breakpoint_hook_list_lock);
		 return unhooked_any;
		  
	 }

	   
	 bool remove_user_hook_handler(_In_ PEPROCESS process)
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

		 
		 PLIST_ENTRY current_page = g_user_hook_list.Flink;

		 while (current_page != &g_user_hook_list)
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
		 

			 while (current_func != &hooked_page_info->hooked_functions_list)
			 {
				 PLIST_ENTRY next_func = current_func->Flink;
				 _hooked_function_info* hooked_function_info = CONTAINING_RECORD(current_func, _hooked_function_info, hooked_function_list);

				 if (hooked_function_info->original_instructions_backup != nullptr)
				 {
					 pool_manager::release_pool(hooked_function_info->original_instructions_backup);
				 }
				 pool_manager::release_pool(hooked_function_info);
				 RemoveEntryList(current_func);
				 current_func = next_func;
			 }



			 prevmcall::remove_ept_hook(hooked_page_info->pfn_of_hooked_page);

			 pool_manager::release_pool(hooked_page_info->fake_page_contents);
			 RemoveEntryList(current_page);
			 pool_manager::release_pool(hooked_page_info);

			 current_page = next_page;
		 }
		  
		 //return unhooked_any;

		 return unhooked_any;

	 }

		 
	 }
}