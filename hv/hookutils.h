#pragma once
 

namespace hyper
{
	//
	// Structure representing a hooked function within a page.
	//
	struct EptHookInfo
	{
		LIST_ENTRY list_entry;

		void* original_va;
		void* fake_va;
		void* handler_va;

		uint64_t original_pa;
		uint64_t fake_pa;

		uint8_t* trampoline_va;      
		uint8_t* fake_page_contents;       

		

		uint64_t hook_size;
	};

	//
	// Structure representing a hooked page context and all its related function hooks.
	//
	struct EptHookedPageContext
	{
		// This entry is part of the global hooked page list
		LIST_ENTRY PageEntry;

		// List head for all HookInfo entries in this page
		LIST_ENTRY hooked_info_list;

		// Page Frame Number of the original hooked page
		uint64_t HookedPageFrameNumber;

		// Page Frame Number of the fake page with hooks
		uint64_t FakePageFrameNumber;

		//
		// Page with our hooked functions
		//
		unsigned __int8* fake_page_contents;

		uint32_t ref_count;
	};


	NTSTATUS HookManagerInitialize();
 
	bool hook(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function);

	void unhook(_In_ void* target_api);

	static 	bool hook_instruction_memory(EptHookInfo* hooked_function_info, void* target_function, unsigned __int64 page_offset);
	static	void hook_write_absolute_jump(unsigned __int8* target_buffer, unsigned __int64 destination_address);
}

 
extern LIST_ENTRY g_HookedPageListHead;