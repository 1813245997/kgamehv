#pragma once



typedef enum _hook_type
{
	hook_type_unknown = 0,                 // 未知类型
	hook_kernel_function_redirect,               //内核 函数跳转重定向 Hook
	hook_jmp_fake_page,
	hook_user_exception_break_point_int3,           // 用户层异常断点 Hook（如 INT3）
	hook_dbg_break_point_int1,             // IN1  断点 Hook
	hook_dbg_break_point_int3,             // INT3 断点 Hook
 
} hook_type;


typedef struct _hooked_function_info 
{
	 
	LIST_ENTRY hooked_function_list;
	   
	unsigned __int64 hook_size;

	void* original_va;

	void* fake_va;

	void* new_handler_va;

	unsigned long long original_pa;

	unsigned long long fake_pa;

	unsigned long long new_handler_pa;
	 

	unsigned __int8* trampoline_va;

	unsigned __int8* fake_page_contents;

	unsigned __int8* original_instructions_backup;

	hook_type type;

}hooked_function_info;

typedef struct _kernel_ept_hook_info
{
	//
	// 链表节点：用于管理所有已 Hook 的内核页
	//
	LIST_ENTRY hooked_page_list;

	//
	// 此页上被 Hook 的函数信息列表（例如目标地址、跳板等）
	//
	LIST_ENTRY hooked_functions_list;

	HANDLE process_id;

	//
	// 被 Hook 的物理页帧号（PFN）
	//
	unsigned __int64 pfn_of_hooked_page;

	//
	// 假页内容的物理页帧号（由外部模块分配并管理）
	//
	unsigned __int64 pfn_of_fake_page_contents;

	//假页面分配的内容
 	unsigned __int8* fake_page_contents;

	//
	// 引用计数： 
	//
	unsigned int ref_count;

	
} kernel_hook_info;


namespace utils
{
	namespace hook_utils
	{
		extern LIST_ENTRY g_kernel_hook_page_list_head;

		extern  LIST_ENTRY g_user_hook_page_list_head;


		void initialize_hook_page_lists();

		bool hook_kernel_function(_In_ void* target_api, _In_ void* new_api, _Out_ void** origin_function);

		bool hook_user_exception_handler(_In_ PEPROCESS process,_In_ void* target_api,_In_ void* exception_handler, _In_ bool allocate_trampoline_page);

		//bool hook_break_point_int3(_In_ PEPROCESS process, _In_ void* target_api, _In_ void* new_api, _Inout_ unsigned char * original_byte);

		//bool unhook_user_all_exception_int3(_In_ PEPROCESS process);


		 bool find_hook_info_by_rip(
			 void* rip,
			 hooked_function_info** out_hook_info);

		 bool find_user_exception_info_by_rip(
			 HANDLE process_id,
			 void* rip,
			 hooked_function_info** out_hook_info);

		 void write_int3(uint8_t* address);

		 void write_int1(uint8_t* address);
	 
		 bool hook_instruction_memory_int3(_Inout_ hooked_function_info* hooked_function_info, void* target_function, unsigned __int64 page_offset);

		 void  hook_write_absolute_jump_int3(unsigned __int8* target_buffer, unsigned __int64 destination_address);

		 bool hook_instruction_memory_int1(_Inout_ hooked_function_info* hooked_function_info, void* target_function, unsigned __int64 page_offset);

		 void hook_write_absolute_jump(unsigned __int8* target_buffer, unsigned __int64 destination_address);
	}
}