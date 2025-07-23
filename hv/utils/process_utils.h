#pragma once

namespace utils

{
	namespace process_utils
	{
		bool get_process_by_name(_In_ LPCWSTR target_name, _Out_ PEPROCESS* out_process);

		bool get_process_by_pid(_In_ HANDLE pid, _Out_ PEPROCESS* out_process);

		bool get_process_full_name(_In_ PEPROCESS process, _Out_ PUNICODE_STRING* process_name);

		bool get_process_name_by_pid(_In_ HANDLE pid, _Inout_ PUNICODE_STRING* process_name);

		bool get_process_name(_In_ PEPROCESS process, _Inout_  PUNICODE_STRING* process_name);

		bool is_process_exited(_In_ PEPROCESS process);

		bool is_process_64_bit(_In_ PEPROCESS process);

		bool is_process_name_match(_In_ PEPROCESS process, _In_ PUNICODE_STRING target_name, _In_ BOOLEAN case_insensitive);

		bool is_process_name_match_wstr(_In_ PEPROCESS process, _In_ PWCHAR target_name_wstr, _In_ BOOLEAN case_insensitive);

		bool is_process_name_match_wstr_by_pid(_In_ HANDLE pid, _In_ PWCHAR target_name_wstr, _In_ BOOLEAN case_insensitive);

		PVOID get_process_peb32_process(_In_ PEPROCESS process);
		
		PVOID get_process_peb64_process(_In_ PEPROCESS process);


		PHANDLE_TABLE_ENTRY exp_lookup_handle_table_entry(_In_ PHANDLE_TABLE handle_table, _In_ HANDLE pid);

	}
}