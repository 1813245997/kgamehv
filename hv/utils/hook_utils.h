#pragma once

#include "kunordered_map.h"

// Hook function info - removed linked list nodes
typedef struct _kernel_hook_function_info 
{
	unsigned __int64 hook_size;
	void* original_va;
	void* fake_va;
	void* handler_va;
	unsigned __int8* trampoline_va;
	unsigned long long original_pa;
	unsigned long long handler_pa;
	unsigned __int8* original_code_backup;
	
	// Add page reference
	void* page_info_ptr;  // Points to the owning page info

} kernel_hook_function_info;

// Hook page info - removed linked list nodes, use hash table to manage functions
typedef struct _kernel_hook_page_info
{
	unsigned __int64 orig_page_pfn;
	unsigned __int64 exec_page_pfn;
	unsigned __int8* fake_page_contents;
	unsigned int hook_count;
	
	// Use hash table to manage all functions on this page
	utils::kunordered_map<void*, kernel_hook_function_info*> function_map;

} kernel_hook_page_info;

namespace utils
{
	namespace hook_utils
	{
		// Global hash table - only manages pages, functions are managed by their respective pages
		extern utils::kunordered_map<unsigned __int64, kernel_hook_page_info*> g_hook_page_map;  // PFN -> PageInfo

		NTSTATUS initialize_hook_page_lists();

		bool hook_kernel_function(_In_ void* target_api, _In_ void* new_api, _Inout_ void** origin_function);

		bool is_address_already_hooked(_In_ void* target_api);

		bool find_kernel_hook_info_by_rip(
			void* rip,
			kernel_hook_function_info* out_hook_info);
		 
 

	}
}