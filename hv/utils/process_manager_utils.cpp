#include "global_defs.h"
#include "process_manager_utils.h"



namespace utils
{
	namespace process_utils
	{
		 	LIST_ENTRY g_process_list_head;
		 	KSPIN_LOCK g_process_list_lock;

			NTSTATUS init_process_manager()
			{
				InitializeListHead(&g_process_list_head);
				KeInitializeSpinLock(&g_process_list_lock);

				return  STATUS_SUCCESS;
			}

			VOID add_process_entry(HANDLE process_id )
			{
				KIRQL old_irql{};
				NTSTATUS status;
				PEPROCESS process = nullptr;
				PUNICODE_STRING full_image_path = nullptr;
				PUNICODE_STRING process_name = nullptr;
				status = utils::internal_functions::pfn_ps_lookup_process_by_process_id (process_id, &process);
				if (!NT_SUCCESS(status) || !process)
				{
					return;
				}

				PPROCESS_ENTRY entry = reinterpret_cast<PPROCESS_ENTRY>(
					  utils::memory::allocate_independent_pages(sizeof(PROCESS_ENTRY), PAGE_READWRITE));
				if (!entry)
				{
				 
					utils::internal_functions::pfn_ob_dereference_object(process);
					return;
				}

				if (!utils::process_utils::get_process_full_name(process, &full_image_path))
				{
					utils::internal_functions::pfn_ob_dereference_object(process);
					return;
				}

				if (!utils::file_utils::get_file_name_from_full_path(full_image_path,&process_name))
				{
					utils::internal_functions::pfn_ob_dereference_object(process);
					return;
				}

	

				 

				entry->process_id = process_id;
				entry->process = process;   
				entry->image_path = full_image_path;
				entry->process_name = process_name;
				KeQuerySystemTime(&entry->create_time);

				KeAcquireSpinLock(&g_process_list_lock, &old_irql);
				InsertTailList(&g_process_list_head, &entry->list_entry);
				KeReleaseSpinLock(&g_process_list_lock, old_irql);
			}

			VOID remove_process_entry(HANDLE process_id)
			{
				KIRQL old_irql{};
				PLIST_ENTRY current{};

				KeAcquireSpinLock(&g_process_list_lock, &old_irql);
				current = g_process_list_head.Flink;

				while (current != &g_process_list_head)
				{
					PPROCESS_ENTRY entry = CONTAINING_RECORD(current, PROCESS_ENTRY, list_entry);
					if (entry->process_id == process_id)
					{
						RemoveEntryList(&entry->list_entry);

						if (entry->image_path)
						{
							 
							if (entry->image_path->Buffer)
								utils::memory::free_independent_pages(entry->image_path->Buffer, entry->image_path->Length);
							utils::memory::free_independent_pages(entry->image_path, sizeof(UNICODE_STRING));
						}

						if (entry->process_name)
						{
							if (entry->process_name->Buffer)
								utils::memory::free_independent_pages(entry->process_name->Buffer, entry->process_name->Length);
							utils::memory::free_independent_pages(entry->process_name, sizeof(UNICODE_STRING));
						}
						utils::internal_functions::pfn_ob_dereference_object(entry->process);
						entry->process = nullptr;
						 

						utils::memory::free_independent_pages(entry, sizeof(PROCESS_ENTRY));
						break;
					}
					current = current->Flink;
				}

				KeReleaseSpinLock(&g_process_list_lock, old_irql);
			}


			NTSTATUS get_process_entry_by_name(
				_In_ PCUNICODE_STRING process_name,
				_Out_ PPROCESS_ENTRY* process_entry_out)
			{

				if (!process_name || !process_entry_out)
				{
					return STATUS_INVALID_PARAMETER;

				}

				KIRQL old_irql{};
				PLIST_ENTRY current = nullptr;
				NTSTATUS status = STATUS_NOT_FOUND;

				*process_entry_out = nullptr;

				KeAcquireSpinLock(&g_process_list_lock, &old_irql);
				current = g_process_list_head.Flink;



				while (current != &g_process_list_head)
				{
					PPROCESS_ENTRY entry = CONTAINING_RECORD(current, PROCESS_ENTRY, list_entry);

					 
					if (!entry->process_name ||	!RtlEqualUnicodeString(process_name, entry->process_name, TRUE))
					{
						current = current->Flink;
						continue;
					}

					 
					PPROCESS_ENTRY copy_entry = reinterpret_cast<PPROCESS_ENTRY>(
						utils::memory::allocate_independent_pages(sizeof(PROCESS_ENTRY), PAGE_READWRITE));
					if (!copy_entry)
					{
						status = STATUS_INSUFFICIENT_RESOURCES;
						break;
					}

				 
					copy_entry->process_id = entry->process_id;
					copy_entry->create_time = entry->create_time;
					copy_entry->process = entry->process;  

					 
					if (entry->image_path)
					{
						copy_entry->image_path = reinterpret_cast<PUNICODE_STRING>(
							utils::memory::allocate_independent_pages(sizeof(UNICODE_STRING), PAGE_READWRITE));
						if (!copy_entry->image_path)
						{
							utils::memory::free_independent_pages(copy_entry, sizeof(PROCESS_ENTRY));
							status = STATUS_INSUFFICIENT_RESOURCES;
							break;
						}

						copy_entry->image_path->Length = entry->image_path->Length;
						copy_entry->image_path->MaximumLength = entry->image_path->MaximumLength;
						copy_entry->image_path->Buffer = reinterpret_cast<PWCH>(
							utils::memory::allocate_independent_pages(copy_entry->image_path->MaximumLength, PAGE_READWRITE));
						if (!copy_entry->image_path->Buffer)
						{
							utils::memory::free_independent_pages(copy_entry->image_path, sizeof(UNICODE_STRING));
							utils::memory::free_independent_pages(copy_entry, sizeof(PROCESS_ENTRY));
							status = STATUS_INSUFFICIENT_RESOURCES;
							break;
						}
						RtlCopyMemory(copy_entry->image_path->Buffer, entry->image_path->Buffer, entry->image_path->Length);
					}
					else
					{
						copy_entry->image_path = nullptr;
					}

					 
					if (entry->process_name)
					{
						copy_entry->process_name = reinterpret_cast<PUNICODE_STRING>(
							utils::memory::allocate_independent_pages(sizeof(UNICODE_STRING), PAGE_READWRITE));
						if (!copy_entry->process_name)
						{
							if (copy_entry->image_path)
							{
								if (copy_entry->image_path->Buffer)
									utils::memory::free_independent_pages(copy_entry->image_path->Buffer, copy_entry->image_path->Length);
								utils::memory::free_independent_pages(copy_entry->image_path, sizeof(UNICODE_STRING));
							}
							utils::memory::free_independent_pages(copy_entry, sizeof(PROCESS_ENTRY));
							status = STATUS_INSUFFICIENT_RESOURCES;
							break;
						}

						copy_entry->process_name->Length = entry->process_name->Length;
						copy_entry->process_name->MaximumLength = entry->process_name->MaximumLength;
						copy_entry->process_name->Buffer = reinterpret_cast<PWCH>(
							utils::memory::allocate_independent_pages(copy_entry->process_name->MaximumLength, PAGE_READWRITE));
						if (!copy_entry->process_name->Buffer)
						{
							utils::memory::free_independent_pages(copy_entry->process_name, sizeof(UNICODE_STRING));
							if (copy_entry->image_path)
							{
								if (copy_entry->image_path->Buffer)
									utils::memory::free_independent_pages(copy_entry->image_path->Buffer, copy_entry->image_path->Length);
								utils::memory::free_independent_pages(copy_entry->image_path, sizeof(UNICODE_STRING));
							}
							utils::memory::free_independent_pages(copy_entry, sizeof(PROCESS_ENTRY));
							status = STATUS_INSUFFICIENT_RESOURCES;
							break;
						}
						RtlCopyMemory(copy_entry->process_name->Buffer, entry->process_name->Buffer, entry->process_name->Length);
					}
					else
					{
						copy_entry->process_name = nullptr;
					}

					*process_entry_out = copy_entry;
					status = STATUS_SUCCESS;
					break;
				}
				KeReleaseSpinLock(&g_process_list_lock, old_irql);

				return status;
				 
			}

			void  free_process_entry(_In_ PPROCESS_ENTRY entry)
			{
				if (!entry)
					return;

				if (entry->image_path)
				{
					if (entry->image_path->Buffer)
						utils::memory::free_independent_pages(entry->image_path->Buffer, entry->image_path->Length);
					utils::memory::free_independent_pages(entry->image_path, sizeof(UNICODE_STRING));
				}

				if (entry->process_name)
				{
					if (entry->process_name->Buffer)
						utils::memory::free_independent_pages(entry->process_name->Buffer, entry->process_name->Length);
					utils::memory::free_independent_pages(entry->process_name, sizeof(UNICODE_STRING));
				}

				if (entry->process)
				{
					utils::internal_functions::pfn_ob_dereference_object(entry->process);
					entry->process = nullptr;
				}

				utils::memory::free_independent_pages(entry, sizeof(PROCESS_ENTRY));
			}

	}

}

