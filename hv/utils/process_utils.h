#pragma once

namespace utils

{
	namespace process_utils
	{
		bool get_process_by_name(_In_ LPCWSTR process_name, _Out_ PEPROCESS* out_process);

		bool is_process_exited(PEPROCESS process);

		bool get_process_by_pid(_In_ HANDLE pid, _Out_ PEPROCESS* out_process);


		PHANDLE_TABLE_ENTRY exp_lookup_handle_table_entry(_In_ PHANDLE_TABLE handle_table, _In_ HANDLE pid);

	}
}