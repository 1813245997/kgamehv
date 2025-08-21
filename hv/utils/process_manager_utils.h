#pragma once


namespace utils
{
	namespace process_utils
	{
		extern	LIST_ENTRY g_process_list_head;
		extern	KSPIN_LOCK g_process_list_lock;



		NTSTATUS init_process_manager();

		VOID add_process_entry(HANDLE process_id );
		VOID remove_process_entry(HANDLE process_id);

		NTSTATUS get_process_entry_by_name(
			_In_ PCUNICODE_STRING process_name,
			_Out_ PPROCESS_ENTRY* process_entry_out);

		void  free_process_entry(_In_ PPROCESS_ENTRY entry);

	}

}

