#pragma once

#include "kunordered_map.h"

// Breakpoint function info - removed linked list nodes
typedef struct _breakpoint_function_info
{
	void* original_va;
	void* breakpoint_va;
	void* handler_va;
	unsigned long long original_pa;
	unsigned long long handler_pa;
	unsigned __int8 original_byte;
	size_t instruction_size;
	unsigned long long hit_count;
	bool is_active;
 
	// Add page reference
	void* page_info_ptr;  // Points to the owning page info

} breakpoint_function_info;

// Breakpoint page info - removed linked list nodes, use hash table to manage breakpoints
typedef struct _breakpoint_page_info
{
	unsigned __int64 orig_page_pfn;        
	unsigned __int64 exec_page_pfn;        
	unsigned __int8* page_contents;        
	unsigned int breakpoint_count;		  
	
	// Use hash table to manage all breakpoints on this page
	utils::kunordered_map<void*, breakpoint_function_info*> breakpoint_map;

} breakpoint_page_info;

// Process breakpoint info - manages all pages for a specific process
typedef struct _process_breakpoint_info
{
	HANDLE process_id;
	
	// Use hash table to manage all pages for this process
	utils::kunordered_map<unsigned __int64, breakpoint_page_info*> page_map;  // PFN -> PageInfo

} process_breakpoint_info;

namespace utils {

	namespace shadowbreakpoint
	{
		// Global hash table - manages processes by PID
		extern utils::kunordered_map<HANDLE, process_breakpoint_info*> g_process_breakpoint_map;  // PID -> ProcessInfo
		extern FAST_MUTEX g_breakpoint_list_lock;

		NTSTATUS shadowbp_initialize();

		bool shadowbp_install(_In_opt_  HANDLE process_id, _In_ void* target_address, _In_ void* handler);

		bool shadowbp_install_user(_In_opt_  HANDLE process_id, _In_ void* target_address, _In_ void* handler);

		bool shadowbp_install_kernel(_In_opt_  HANDLE process_id, _In_ void* target_address, _In_ void* handler);

		bool shadowbp_remove_all(HANDLE process_id);

		bool shadowbp_remove(HANDLE process_id, void* target_address);

		bool shadowbp_set_active(HANDLE process_id, void* target_address, bool enable);

		bool shadowbp_find_address(
			HANDLE process_id,
			void* target_address,
			breakpoint_function_info** out_bp_info);

		bool shadowbp_find_tf_address(
			HANDLE process_id,
			void* target_address,
			breakpoint_function_info** out_bp_info);
  
		 
	}
}
