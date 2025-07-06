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


		uint8_t* original_instructions_backup;
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

		HANDLE  process_id;
		 
		uint32_t  ref_count;
	};


	NTSTATUS HookManagerInitialize();
 
	bool hook(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function);

	bool find_hook_info_by_rip(
		LIST_ENTRY* hook_page_list,
		void* rip,
		hyper::EptHookInfo** out_hook_info);
	
	void unhook(_In_ void* target_api);

	 
	void hook_write_absolute_jump(unsigned __int8* target_buffer, unsigned __int64 destination_address);



	//DBG
	bool ept_hook_break_point_int3(_In_ HANDLE process_id, _In_ void* target_api, _In_ void* new_api, _Inout_ void** origin_function, _Inout_ bool allocate_trampoline =true);

	bool find_hook_break_point_int3(
		_In_ void* rip,
		_Out_ hyper::EptHookInfo** out_hook_info);

	bool hook_instruction_memory(EptHookInfo* hooked_function_info, void* target_function, unsigned __int64 page_offset);

	void hook_write_absolute_jump_r3(unsigned __int8* target_buffer, unsigned __int64 destination_address, bool is64);

	bool unhook_all_ept_hooks_for_pid(_In_ HANDLE process_id);



	void write_int1(uint8_t* address);

	void write_int3(uint8_t* address);

}

 
extern LIST_ENTRY g_HookedPageListHead;

extern LIST_ENTRY g_ept_breakpoint_hook_list;