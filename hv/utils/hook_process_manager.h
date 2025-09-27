#pragma once

namespace utils
{
	namespace hook_process_manager
	{

		NTSTATUS initialize_hook_process_manager();


		NTSTATUS NTAPI hook_nt_create_section(
			_Out_ PHANDLE section_handle,
			_In_ ACCESS_MASK desired_access,
			_In_opt_ POBJECT_ATTRIBUTES object_attributes,
			_In_opt_ PLARGE_INTEGER maximum_size,
			_In_ ULONG section_page_protection,
			_In_ ULONG allocation_attributes,
			_In_opt_ HANDLE file_handle
		);

		void __fastcall hook_psp_exit_process(
			IN BOOLEAN trim_address_space,
			IN PEPROCESS process
		);

	}
}