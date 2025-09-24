#pragma once
 

typedef struct _breakpoint_function_info
{
	LIST_ENTRY list_entry;                // 链表节点（挂在 breakpoint_list 上）
	void* original_va;                    // 原始函数地址
	void* breakpoint_va;                  // 断点地址
	void* handler_va;                     // 断点处理函数地址 
	unsigned long long original_pa;       // 原始物理地址
	unsigned long long handler_pa;        // 处理函数物理地址
	unsigned __int8    original_byte;     // 保存原始指令
	unsigned long long hit_count;         // 命中次数
	bool is_active;                       // 是否启用
} breakpoint_function_info;

typedef struct _breakpoint_page_info
{
	LIST_ENTRY page_list;                 // 所有断点页的链表
	LIST_ENTRY breakpoint_list;           // 当前页上的断点集合
	HANDLE process_id;                    // 关联进程
	unsigned __int64 orig_page_pfn;       // 原始页 PFN
	unsigned __int64 exec_page_pfn;       // 可执行页 PFN
	unsigned __int8* page_contents;       // 页内容副本（带断点）
	unsigned int breakpoint_count;		  // 当前页上的断点数量
} breakpoint_page_info;

namespace utils {

	namespace shadowbreakpoint
	{
		extern LIST_ENTRY  g_breakpoint_list;
		extern FAST_MUTEX  g_breakpoint_list_lock;

		void shadowbp_initialize();

		bool shadowbp_install(_In_ HANDLE process_id, _In_ void* target_address, _In_ void* handler);

		bool shadowbp_remove_all(HANDLE process_id);

		bool shadowbp_remove(HANDLE process_id, void* target_address);

		bool shadowbp_set_active(HANDLE process_id, void* target_address, bool enable);

		bool shadowbp_find_address(
			HANDLE process_id,
			void* target_address,
			breakpoint_function_info* out_bp_info);



		 
	}
}
