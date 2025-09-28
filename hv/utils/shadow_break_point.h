#pragma once
 

typedef struct _breakpoint_function_info
{
	LIST_ENTRY list_entry;
	void* original_va;
	void* breakpoint_va;
	void* handler_va;
	unsigned long long original_pa;
	unsigned long long handler_pa;
	unsigned __int8    original_byte;
	unsigned __int8    instruction_size;
	unsigned long long hit_count;
	bool is_active;
 

} breakpoint_function_info;

typedef struct _breakpoint_page_info
{
	LIST_ENTRY page_list;                  
	LIST_ENTRY breakpoint_list;            
	HANDLE process_id;                     
	unsigned __int64 orig_page_pfn;        
	unsigned __int64 exec_page_pfn;        
	unsigned __int8* page_contents;        
	unsigned int breakpoint_count;		  
} breakpoint_page_info;

namespace utils {

	namespace shadowbreakpoint
	{
		extern LIST_ENTRY  g_breakpoint_list;
		extern FAST_MUTEX  g_breakpoint_list_lock;

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
