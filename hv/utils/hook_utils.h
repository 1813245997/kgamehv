#pragma once


 
 

typedef struct _kernel_hook_function_info 
{
	 
	LIST_ENTRY list_entry;
	unsigned __int64 hook_size;
	void* original_va;
	void* fake_va;
	void* handler_va;
	unsigned __int8* trampoline_va;
	unsigned long long original_pa;
	unsigned long long handler_pa;
	unsigned __int8*   original_code_backup;

	 
}kernel_hook_function_info;

typedef struct _kernel_hook_page_info
{

	LIST_ENTRY page_list;
	LIST_ENTRY function_list;
	unsigned __int64 orig_page_pfn;
	unsigned __int64 exec_page_pfn;
 	unsigned __int8* fake_page_contents;
	unsigned int hook_count;

} kernel_hook_page_info;

 
namespace utils
{
	namespace hook_utils
	{
		extern LIST_ENTRY g_kernel_hook_page_list_head ;



		NTSTATUS initialize_hook_page_lists();

		bool hook_kernel_function(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function);

		bool find_kernel_hook_info_by_rip(
			void* rip,
			kernel_hook_function_info* out_hook_info);
		 


 
	}
}