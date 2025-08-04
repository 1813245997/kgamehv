#pragma once
// 用于请求设置函数 Hook 的参数结构体
struct HookFunctionArgs
{
	unsigned __int64 current_cr3;   // 当前进程的 CR3（页表基址），用于在正确的地址空间中访问目标函数
	unsigned __int64 pfn_of_hooked_page; 
	unsigned __int64 pfn_of_fake_page_contents;
	volatile SHORT statuses;        // 状态同步标志位，用于指示操作结果或状态（例如：处理中、完成、失败等）
};

// 用于请求解除函数 Hook 的参数结构体
struct UnHookFunctionArgs
{
	unsigned __int64 current_cr3;       // 当前进程的 CR3（页表基址），用于恢复目标函数地址空间
	unsigned __int64 pfn_of_hooked_page;
	volatile SHORT statuses;            // 状态同步标志位，用于指示操作结果或状态（例如：处理中、完成、失败等）
};

struct SetBreakpointArgs
{
	unsigned __int64 current_cr3;         // 当前 CR3，用于进程上下文
	void* breakpoint_address;             // 要设置断点的地址
	void* breakpoint_handler;             // 断点命中后的处理函数
	unsigned char* original_byte;          // 原始字节（用于恢复）
	volatile SHORT statuses;              // 状态同步标记
};

struct RemoveBreakpointArgs
{
	unsigned __int64 current_cr3;         // 当前 CR3，用于进程上下文
	void* breakpoint_address;             // 要移除断点的地址
	bool remove_all_breakpoints;          // 是否移除所有断点（true 时忽略地址）
	volatile SHORT statuses;              // 状态同步标记
};



struct SetExceptionBreakpointArgs
{
	HANDLE process_id;                    // 目标进程 ID
	unsigned __int64 current_cr3;         // 当前 CR3，用于进程上下文
	void* breakpoint_address;             // 要设置断点的地址
	void* breakpoint_handler;             // 断点命中后的处理函数
	unsigned char* trampoline_va;          // 跳回原函数
	volatile SHORT statuses;              // 状态同步标记
};

struct RemoveExceptionBreakpointArgs
{
	HANDLE process_id;                    // 目标进程 ID
	unsigned __int64 current_cr3;         // 当前 CR3，用于进程上下文
	void* breakpoint_address;             // 要移除断点的地址
	bool remove_all_breakpoints;          // 是否移除所有断点（true 时忽略地址）
	volatile SHORT statuses;              // 状态同步标记
};


struct FindHookInfoArgs
{
	HANDLE process_id;              // 要查询的进程 PID
	hook_type type;
	void* target_address;           // 要检查是否被 Hook 的地址
	void** hook_info_result;        // 输出：如果存在 Hook，则返回相关信息指针（可为结构体指针）
	
	volatile SHORT statuses;        // 状态同步标记 
};
