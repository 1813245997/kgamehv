#include "hv.h"
#include "memory_pool.h"
#include "hookutils.h"
#include "LDE64.h"
#include "prevmcall.h"
  
LIST_ENTRY g_HookedPageListHead{};


LIST_ENTRY  g_UsermodeHookPageListHead{};


namespace hyper
{
#define MASK_EPT_PML1_OFFSET(_VAR_) ((unsigned __int64)_VAR_ & 0xFFFULL)
#define GET_PFN(_VAR_) (_VAR_ >> PAGE_SHIFT)


	NTSTATUS HookManagerInitialize()
	{
		InitializeListHead(&g_HookedPageListHead);
		InitializeListHead(&g_UsermodeHookPageListHead);

		return STATUS_SUCCESS;
	}

 
	bool hook_instruction_memory(EptHookInfo* hooked_function_info, void* target_function , unsigned __int64 page_offset)
	{
		auto write_int1 = [](uint8_t* address)
		{
			*address = 0xF1; // INT1 (ICEBP)
		};

		auto write_int3 = [](uint8_t* address)
		{
			*address = 0xCC; // INT3
		};


		 

		//R3
		if ((unsigned __int64)target_function <= (unsigned __int64)MM_HIGHEST_USER_ADDRESS)
		{
			//R3的trampoline_va不可以直接跳回原函数 只是备份了内容 如果直接跳回原函数从R0跳回R3 可能会出错
			unsigned __int64 hooked_instructions_size = LDE(target_function, 64);

			hooked_function_info->hook_size = hooked_instructions_size;

			RtlCopyMemory(hooked_function_info->trampoline_va, target_function, hooked_instructions_size);

			hook_write_absolute_jump(&hooked_function_info->trampoline_va[hooked_instructions_size], (unsigned __int64)target_function + hooked_instructions_size);

			write_int3(&hooked_function_info->fake_page_contents[page_offset]);
		}

		//R0 
		else
		{
			unsigned __int64 hooked_instructions_size = LDE(target_function, 64);

			hooked_function_info->hook_size = hooked_instructions_size;

			// Copy overwritten instructions to trampoline buffer
			RtlCopyMemory(hooked_function_info->trampoline_va, target_function, hooked_instructions_size);

			// Add the absolute jump back to the original function.
			hook_write_absolute_jump(&hooked_function_info->trampoline_va[hooked_instructions_size], (unsigned __int64)target_function + hooked_instructions_size);

		
			// Write icebp instruction to our shadow page memory to trigger vmexit.
			 write_int1(&hooked_function_info->fake_page_contents[page_offset]);
		}
		return true;
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

	 

	bool hook(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function)
	{
		 
		 
		if (!target_api || !new_api)
			return false;

		if (origin_function)
			*origin_function = nullptr;

		uint64_t target_pa = MmGetPhysicalAddress(target_api).QuadPart;
		if (target_pa == 0)
			return false;

	
		unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET((unsigned __int64)target_api);



		for (PLIST_ENTRY entry = g_HookedPageListHead.Flink;
			entry != &g_HookedPageListHead;
			entry = entry->Flink)
		{
			EptHookedPageContext* page_ctx = CONTAINING_RECORD(entry, EptHookedPageContext, PageEntry);

			if (page_ctx->HookedPageFrameNumber == GET_PFN(target_pa))
			{
				auto* hook_info = reinterpret_cast<EptHookInfo*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(EptHookInfo), POOL_TAG));
				if (!hook_info)
					return false;

				hook_info->trampoline_va = reinterpret_cast<uint8_t*>(ExAllocatePoolWithTag(NonPagedPool, 100, POOL_TAG));
				if (!hook_info->trampoline_va)
				{
					ExFreePool(hook_info);
					return false;
				}
				hook_info->handler_va = new_api;
				hook_info->original_va = target_api;
				hook_info->original_pa = target_pa;
				hook_info->fake_va = &page_ctx->fake_page_contents[page_offset];
				hook_info->fake_pa = MmGetPhysicalAddress(&page_ctx->fake_page_contents[page_offset]).QuadPart;
				hook_info->fake_page_contents = page_ctx->fake_page_contents;
				if (origin_function)
					*origin_function = hook_info->trampoline_va;
				++page_ctx->ref_count;
				hook_instruction_memory(hook_info, target_api,page_offset);
				InsertHeadList(&page_ctx->hooked_info_list, &hook_info->list_entry);

				return true;
			}
		}
		 

		 
		auto* page_ctx = reinterpret_cast<EptHookedPageContext*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(EptHookedPageContext), POOL_TAG));
		if (!page_ctx)
			return false;

		page_ctx->fake_page_contents = reinterpret_cast<uint8_t*>(ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE, POOL_TAG));
		if (!page_ctx->fake_page_contents)
		{
			ExFreePool(page_ctx);
			return false;
		}
		uint64_t fake_pa = MmGetPhysicalAddress(page_ctx->fake_page_contents).QuadPart;
		if (fake_pa == 0)
		{
			ExFreePool(page_ctx->fake_page_contents);
			ExFreePool(page_ctx);
		
			return false;

		}
		InitializeListHead(&page_ctx->hooked_info_list);

		auto* hook_info = reinterpret_cast<EptHookInfo*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(EptHookInfo), POOL_TAG));
		if (!hook_info)
		{
			ExFreePool(page_ctx->fake_page_contents);
			ExFreePool(page_ctx);
			return false;
		}

		hook_info->trampoline_va = reinterpret_cast<uint8_t*>(ExAllocatePoolWithTag(NonPagedPool, 100, POOL_TAG));
		if (!hook_info->trampoline_va)
		{
			ExFreePool(page_ctx->fake_page_contents);
			ExFreePool(page_ctx);
			ExFreePool(hook_info);
			return false;
		}
		RtlCopyMemory(page_ctx->fake_page_contents, PAGE_ALIGN(target_api), PAGE_SIZE);
	
		page_ctx->HookedPageFrameNumber = GET_PFN(target_pa);
		page_ctx->FakePageFrameNumber = GET_PFN(MmGetPhysicalAddress(page_ctx->fake_page_contents).QuadPart);
		hook_info->handler_va = new_api;
		hook_info->original_va = target_api;
		hook_info->fake_va = &page_ctx->fake_page_contents[page_offset];
		hook_info->original_pa = target_pa;
		hook_info->fake_pa = MmGetPhysicalAddress( &page_ctx->fake_page_contents[page_offset]).QuadPart ;
		hook_info->fake_page_contents = page_ctx->fake_page_contents;
		++page_ctx->ref_count;
		if (origin_function)
			*origin_function = hook_info->trampoline_va;

		hook_instruction_memory(hook_info, target_api,page_offset);
		InsertHeadList(&page_ctx->hooked_info_list, &hook_info->list_entry);
		InsertHeadList(&g_HookedPageListHead, &page_ctx->PageEntry);

		prevmcall::install_ept_hook(hook_info->original_pa, hook_info->fake_pa);

		 
		 return true;

	}

	void unhook(_In_ void* target_api)
	{
		if (!target_api)
			return;

		const uint64_t target_pa = MmGetPhysicalAddress(target_api).QuadPart;
		if (target_pa == 0)
			return;

		for (PLIST_ENTRY entry = g_HookedPageListHead.Flink;
			entry != &g_HookedPageListHead;
			entry = entry->Flink)
		{
			auto* page_ctx = CONTAINING_RECORD(entry, EptHookedPageContext, PageEntry);

			if (page_ctx->HookedPageFrameNumber != GET_PFN(target_pa))
				continue;

			for (PLIST_ENTRY func_entry = page_ctx->hooked_info_list.Flink;
				func_entry != &page_ctx->hooked_info_list;
				func_entry = func_entry->Flink)
			{
				auto* hook_info = CONTAINING_RECORD(func_entry, EptHookInfo, list_entry);

				if (hook_info->original_va != target_api)
					continue;

				if (page_ctx->ref_count<=1)
				{
					// 找到了目标 Hook，移除并清理
					prevmcall::remove_ept_hook(hook_info->original_pa);

				}
				else
				{
					RtlCopyMemory(hook_info->fake_va, hook_info->original_va, hook_info->hook_size);
				}
			

				RemoveEntryList(&hook_info->list_entry);

				if (hook_info->trampoline_va)
					ExFreePool(hook_info->trampoline_va);

				ExFreePool(hook_info);

				// 如果该页面没有剩余 hook，连带清理页面上下文
				if (IsListEmpty(&page_ctx->hooked_info_list))
				{
					RemoveEntryList(&page_ctx->PageEntry);

					if (page_ctx->fake_page_contents)
						ExFreePool(page_ctx->fake_page_contents);

					ExFreePool(page_ctx);
				}

				return;
			}
		}
	}



	bool hookr3( _In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function)
	{


		if (!target_api || !new_api)
			return false;

		if (origin_function)
			*origin_function = nullptr;

		uint64_t target_pa = MmGetPhysicalAddress(target_api).QuadPart;
		if (target_pa == 0)
			return false;


		unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET((unsigned __int64)target_api);



		for (PLIST_ENTRY entry = g_UsermodeHookPageListHead.Flink;
			entry != &g_UsermodeHookPageListHead;
			entry = entry->Flink)
		{
			EptHookedPageContext* page_ctx = CONTAINING_RECORD(entry, EptHookedPageContext, PageEntry);

			if (page_ctx->HookedPageFrameNumber == GET_PFN(target_pa))
			{
				auto* hook_info = reinterpret_cast<EptHookInfo*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(EptHookInfo), POOL_TAG));
				if (!hook_info)
					return false;

				hook_info->trampoline_va = reinterpret_cast<uint8_t*>(ExAllocatePoolWithTag(NonPagedPool, 100, POOL_TAG));
				if (!hook_info->trampoline_va)
				{
					ExFreePool(hook_info);
					return false;
				}
				hook_info->handler_va = new_api;
				hook_info->original_va = target_api;
				hook_info->original_pa = target_pa;
				hook_info->fake_va = &page_ctx->fake_page_contents[page_offset];
				hook_info->fake_pa = MmGetPhysicalAddress(&page_ctx->fake_page_contents[page_offset]).QuadPart;
				hook_info->fake_page_contents = page_ctx->fake_page_contents;
				if (origin_function)
					*origin_function = hook_info->trampoline_va;
				++page_ctx->ref_count;
				hook_instruction_memory(hook_info, target_api, page_offset);
				InsertHeadList(&page_ctx->hooked_info_list, &hook_info->list_entry);

				return true;
			}
		}



		auto* page_ctx = reinterpret_cast<EptHookedPageContext*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(EptHookedPageContext), POOL_TAG));
		if (!page_ctx)
			return false;

		page_ctx->fake_page_contents = reinterpret_cast<uint8_t*>(ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE, POOL_TAG));
		if (!page_ctx->fake_page_contents)
		{
			ExFreePool(page_ctx);
			return false;
		}
		uint64_t fake_pa = MmGetPhysicalAddress(page_ctx->fake_page_contents).QuadPart;
		if (fake_pa == 0)
		{
			ExFreePool(page_ctx->fake_page_contents);
			ExFreePool(page_ctx);

			return false;

		}
		InitializeListHead(&page_ctx->hooked_info_list);

		auto* hook_info = reinterpret_cast<EptHookInfo*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(EptHookInfo), POOL_TAG));
		if (!hook_info)
		{
			ExFreePool(page_ctx->fake_page_contents);
			ExFreePool(page_ctx);
			return false;
		}

		hook_info->trampoline_va = reinterpret_cast<uint8_t*>(ExAllocatePoolWithTag(NonPagedPool, 100, POOL_TAG));
		if (!hook_info->trampoline_va)
		{
			ExFreePool(page_ctx->fake_page_contents);
			ExFreePool(page_ctx);
			ExFreePool(hook_info);
			return false;
		}
		RtlCopyMemory(page_ctx->fake_page_contents, PAGE_ALIGN(target_api), PAGE_SIZE);

		page_ctx->HookedPageFrameNumber = GET_PFN(target_pa);
		page_ctx->FakePageFrameNumber = GET_PFN(MmGetPhysicalAddress(page_ctx->fake_page_contents).QuadPart);
		hook_info->handler_va = new_api;
		hook_info->original_va = target_api;
		hook_info->fake_va = &page_ctx->fake_page_contents[page_offset];
		hook_info->original_pa = target_pa;
		hook_info->fake_pa = MmGetPhysicalAddress(&page_ctx->fake_page_contents[page_offset]).QuadPart;
		hook_info->fake_page_contents = page_ctx->fake_page_contents;
		++page_ctx->ref_count;
		if (origin_function)
			*origin_function = hook_info->trampoline_va;

		hook_instruction_memory(hook_info, target_api, page_offset);
		InsertHeadList(&page_ctx->hooked_info_list, &hook_info->list_entry);
		InsertHeadList(&g_UsermodeHookPageListHead, &page_ctx->PageEntry);

		prevmcall::install_ept_hook(hook_info->original_pa, hook_info->fake_pa);


		return true;

	}

	 
	void unhookr3(_In_ void* target_api)
	{
		if (!target_api)
			return;

		const uint64_t target_pa = MmGetPhysicalAddress(target_api).QuadPart;
		if (target_pa == 0)
			return;

		for (PLIST_ENTRY entry = g_UsermodeHookPageListHead.Flink;
			entry != &g_UsermodeHookPageListHead;
			entry = entry->Flink)
		{
			auto* page_ctx = CONTAINING_RECORD(entry, EptHookedPageContext, PageEntry);

			if (page_ctx->HookedPageFrameNumber != GET_PFN(target_pa))
				continue;

			for (PLIST_ENTRY func_entry = page_ctx->hooked_info_list.Flink;
				func_entry != &page_ctx->hooked_info_list;
				func_entry = func_entry->Flink)
			{
				auto* hook_info = CONTAINING_RECORD(func_entry, EptHookInfo, list_entry);

				if (hook_info->original_va != target_api)
					continue;

				if (page_ctx->ref_count <= 1)
				{
					// 找到了目标 Hook，移除并清理
					prevmcall::remove_ept_hook(hook_info->original_pa);

				}
				else
				{
					RtlCopyMemory(hook_info->fake_va, hook_info->original_va, hook_info->hook_size);
				}


				RemoveEntryList(&hook_info->list_entry);

				if (hook_info->trampoline_va)
					ExFreePool(hook_info->trampoline_va);

				ExFreePool(hook_info);

				// 如果该页面没有剩余 hook，连带清理页面上下文
				if (IsListEmpty(&page_ctx->hooked_info_list))
				{
					RemoveEntryList(&page_ctx->PageEntry);

					if (page_ctx->fake_page_contents)
						ExFreePool(page_ctx->fake_page_contents);

					ExFreePool(page_ctx);
				}

				return;
			}
		}
	}


	bool find_hook_info_by_rip(
		LIST_ENTRY* hook_page_list,
		void* rip,
		hyper::EptHookInfo** out_hook_info)
	{
		for (PLIST_ENTRY page_entry = hook_page_list->Flink;
			page_entry != hook_page_list;
			page_entry = page_entry->Flink)
		{
			auto* hooked_page = CONTAINING_RECORD(page_entry, hyper::EptHookedPageContext, PageEntry);

			for (PLIST_ENTRY func_entry = hooked_page->hooked_info_list.Flink;
				func_entry != &hooked_page->hooked_info_list;
				func_entry = func_entry->Flink)
			{
				auto* hook_info = CONTAINING_RECORD(func_entry, hyper::EptHookInfo, list_entry);

				if (reinterpret_cast<void*>(rip) == hook_info->original_va)
				{
					*out_hook_info = hook_info;
					return true;
				}
			}
		}

		return false;
	}

}