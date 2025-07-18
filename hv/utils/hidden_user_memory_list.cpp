#include "global_defs.h"
#include "hidden_user_memory_list.h"

namespace utils
{
	namespace   hidden_user_memory
	{
		LIST_ENTRY g_hidden_process_list_head{};
		ERESOURCE g_hidden_process_resource{};

		void initialize_hidden_user_memory()
		{
			InitializeListHead(&g_hidden_process_list_head);
			ExInitializeResourceLite(&g_hidden_process_resource);
		}


		NTSTATUS insert_hidden_address_for_pid(HANDLE process_id, unsigned long long start_address, unsigned long long end_address)
		{
			if (start_address >= end_address)
				return STATUS_INVALID_PARAMETER;

			NTSTATUS status = STATUS_SUCCESS;
			BOOLEAN need_insert_proc = TRUE;

			ExAcquireResourceExclusiveLite(&g_hidden_process_resource, TRUE);

			// 遍历链表找是否已存在进程节点
			PLIST_ENTRY pEntry = g_hidden_process_list_head.Flink;
			PHIDDEN_PROCESS_ENTRY pProcEntry = nullptr;

			while (pEntry != &g_hidden_process_list_head)
			{
				pProcEntry = CONTAINING_RECORD(pEntry, HIDDEN_PROCESS_ENTRY, list_entry);
				if (pProcEntry->process_pid == process_id)
				{
					need_insert_proc = FALSE;
					break;
				}
				pEntry = pEntry->Flink;
			}

			if (need_insert_proc)
			{
				 
				pProcEntry = (PHIDDEN_PROCESS_ENTRY)utils::internal_functions::pfn_mm_allocate_independent_pages(sizeof(HIDDEN_PROCESS_ENTRY),0) ;
				if (!pProcEntry)
				{
					ExReleaseResourceLite(&g_hidden_process_resource);
					return STATUS_INSUFFICIENT_RESOURCES;
				}
				RtlZeroMemory(pProcEntry, sizeof(HIDDEN_PROCESS_ENTRY));
				pProcEntry->process_pid = process_id;
				InitializeListHead(&pProcEntry->address_list_head);
				InsertTailList(&g_hidden_process_list_head, &pProcEntry->list_entry);
			}

			// 检查地址区间是否已存在
			pEntry = pProcEntry->address_list_head.Flink;
			while (pEntry != &pProcEntry->address_list_head)
			{
				PHIDDEN_ADDRESS_ENTRY pAddr = CONTAINING_RECORD(pEntry, HIDDEN_ADDRESS_ENTRY, list_entry);
				if (pAddr->start_address == start_address && pAddr->end_address == end_address)
				{
					// 已存在相同区间，直接返回成功
					ExReleaseResourceLite(&g_hidden_process_resource);
					return STATUS_SUCCESS;
				}
				pEntry = pEntry->Flink;
			}

			// 新地址节点
			PHIDDEN_ADDRESS_ENTRY pNewAddr = (PHIDDEN_ADDRESS_ENTRY)utils::internal_functions::pfn_mm_allocate_independent_pages(  sizeof(HIDDEN_ADDRESS_ENTRY),0);
			if (!pNewAddr)
			{
				ExReleaseResourceLite(&g_hidden_process_resource);
				return STATUS_INSUFFICIENT_RESOURCES;
			}

			pNewAddr->start_address = start_address;
			pNewAddr->end_address = end_address;
			InsertTailList(&pProcEntry->address_list_head, &pNewAddr->list_entry);

			ExReleaseResourceLite(&g_hidden_process_resource);
			return status;
		}


		bool is_address_hidden_for_pid(HANDLE process_id, unsigned long long address)
		{
			BOOLEAN found = FALSE;

			ExAcquireResourceSharedLite(&g_hidden_process_resource, TRUE);

			PLIST_ENTRY pEntry = g_hidden_process_list_head.Flink;
			while (pEntry != &g_hidden_process_list_head)
			{
				PHIDDEN_PROCESS_ENTRY pProcEntry = CONTAINING_RECORD(pEntry, HIDDEN_PROCESS_ENTRY, list_entry);

				if (pProcEntry->process_pid != process_id)
				{
					pEntry = pEntry->Flink;
					continue;  
				}

			 
				PLIST_ENTRY addrEntry = pProcEntry->address_list_head.Flink;
				while (addrEntry != &pProcEntry->address_list_head)
				{
					PHIDDEN_ADDRESS_ENTRY pAddr = CONTAINING_RECORD(addrEntry, HIDDEN_ADDRESS_ENTRY, list_entry);

					if (address >= pAddr->start_address && address <= pAddr->end_address)
					{
						found = TRUE;
						break;
					}
					addrEntry = addrEntry->Flink;
				}

				break; // PID已找到，无需继续查找其他进程节点
			}

			ExReleaseResourceLite(&g_hidden_process_resource);
			return found == TRUE;

		 
		}


		bool remove_hidden_addresses_for_pid(HANDLE process_id)
		{
			ExAcquireResourceExclusiveLite(&g_hidden_process_resource, TRUE);

			PLIST_ENTRY pEntry = g_hidden_process_list_head.Flink;
			while (pEntry != &g_hidden_process_list_head)
			{
				PHIDDEN_PROCESS_ENTRY pProcEntry = CONTAINING_RECORD(pEntry, HIDDEN_PROCESS_ENTRY, list_entry);
				PLIST_ENTRY nextEntry = pEntry->Flink;

				if (pProcEntry->process_pid != process_id)
				{
					pEntry = nextEntry;
					continue;
				}
			 
				// 释放所有地址节点
				PLIST_ENTRY addrEntry = pProcEntry->address_list_head.Flink;
				while (addrEntry != &pProcEntry->address_list_head)
				{
					PHIDDEN_ADDRESS_ENTRY pAddr = CONTAINING_RECORD(addrEntry, HIDDEN_ADDRESS_ENTRY, list_entry);
					addrEntry = addrEntry->Flink;

					RemoveEntryList(&pAddr->list_entry);
					utils::internal_functions::pfn_mm_free_independent_pages(pAddr, sizeof(HIDDEN_ADDRESS_ENTRY));
				}

				// 释放进程节点
				RemoveEntryList(&pProcEntry->list_entry);
				utils::internal_functions::pfn_mm_free_independent_pages(pProcEntry, sizeof(HIDDEN_PROCESS_ENTRY));

				ExReleaseResourceLite(&g_hidden_process_resource);
				return true;
			}

			ExReleaseResourceLite(&g_hidden_process_resource);
			return false;
		}




		bool remove_hidden_address_for_pid(
			HANDLE process_id,
			unsigned long long start_address,
			unsigned long long end_address
		)
		{
			if (start_address >= end_address)
				return false;

			bool removed = false;
			ExAcquireResourceExclusiveLite(&g_hidden_process_resource, TRUE);

			PLIST_ENTRY pEntry = g_hidden_process_list_head.Flink;
			while (pEntry != &g_hidden_process_list_head)
			{
				PHIDDEN_PROCESS_ENTRY pProcEntry = CONTAINING_RECORD(pEntry, HIDDEN_PROCESS_ENTRY, list_entry);
				PLIST_ENTRY next_proc = pEntry->Flink;

				if (pProcEntry->process_pid != process_id)
				{
					pEntry = next_proc;
					continue;
				}

				PLIST_ENTRY addrEntry = pProcEntry->address_list_head.Flink;
				while (addrEntry != &pProcEntry->address_list_head)
				{
					PHIDDEN_ADDRESS_ENTRY pAddr = CONTAINING_RECORD(addrEntry, HIDDEN_ADDRESS_ENTRY, list_entry);
					PLIST_ENTRY next_addr = addrEntry->Flink;

					if (pAddr->start_address == start_address && pAddr->end_address == end_address)
					{
						RemoveEntryList(&pAddr->list_entry);
						utils::internal_functions::pfn_mm_free_independent_pages(pAddr, sizeof(HIDDEN_ADDRESS_ENTRY));
						removed = true;
						break;
					}

					addrEntry = next_addr;
				}

				// 如果地址已移除并且当前进程无剩余地址，清理进程节点
				if (removed && IsListEmpty(&pProcEntry->address_list_head))
				{
					RemoveEntryList(&pProcEntry->list_entry);
					utils::internal_functions::pfn_mm_free_independent_pages(pProcEntry, sizeof(HIDDEN_PROCESS_ENTRY));
				}

				break;
			}

			ExReleaseResourceLite(&g_hidden_process_resource);
			return removed;
		}


	}


}