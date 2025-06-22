#pragma once
namespace utils
{
	namespace   hidden_user_memory
	{
		extern LIST_ENTRY g_hidden_process_list_head;
		extern ERESOURCE g_hidden_process_resource;
		// 地址节点：一个进程中一段被隐藏的内存区间
		typedef struct _HIDDEN_ADDRESS_ENTRY
		{
			LIST_ENTRY list_entry;     // 挂入进程的地址列表
			unsigned long long  start_address;       // 内存起始地址
			unsigned long long  end_address;         // 内存结束地址
		} HIDDEN_ADDRESS_ENTRY, * PHIDDEN_ADDRESS_ENTRY;

		// 进程节点：表示一个进程，其下挂多个被隐藏的内存区间
		typedef struct _HIDDEN_PROCESS_ENTRY
		{
			LIST_ENTRY list_entry;         // 挂入全局进程链表
			ULONG process_pid;                     // 进程 PID
			LIST_ENTRY address_list_head;  // 地址节点链表头
		} HIDDEN_PROCESS_ENTRY, * PHIDDEN_PROCESS_ENTRY;


		void initialize_hidden_user_memory();

		NTSTATUS insert_hidden_address_for_pid(ULONG process_id, unsigned long long start_address, unsigned long long end_address);
		bool is_address_hidden_for_pid(ULONG process_id, unsigned long long address);
		bool remove_hidden_address_for_pid(
			ULONG process_id,
			unsigned long long start_address,
			unsigned long long end_address
		);
		bool remove_hidden_addresses_for_pid(ULONG process_id);
	}
}