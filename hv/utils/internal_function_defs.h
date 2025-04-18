#pragma once

 

namespace utils
{
	namespace internal_functions
	{
		NTSTATUS initialize_internal_functions();



		// 显式函数指针定义
		extern void(__fastcall* pfn_ki_preprocess_fault)(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode
			);

		extern NTSTATUS(__fastcall* pfn_mm_copy_memory)(
			_In_ PVOID target_address,
			_In_ MM_COPY_ADDRESS source_address,
			_In_ SIZE_T number_of_bytes,
			_In_ ULONG flags,
			_Out_ PSIZE_T number_of_bytes_transferred
			);

		extern BOOLEAN(__fastcall* pfn_mm_is_address_valid)(
			PVOID virtual_address
			);

		extern BOOLEAN(__fastcall* pfn_mm_is_address_valid_ex)(
			PVOID virtual_address
			);


		extern USHORT(__fastcall* pfn_rtl_walk_frame_chain)(
			_Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
			_In_ ULONG count,
			_In_ ULONG flags
			);

		extern PRUNTIME_FUNCTION(__fastcall* pfn_rtl_lookup_function_entry)(
			_In_ DWORD64 control_pc,
			_Out_ PDWORD64 image_base,
			_Inout_opt_ PUNWIND_HISTORY_TABLE history_table
			);

		extern void(__fastcall* pfn_psp_exit_process)(
			BOOLEAN trim_address_space,
			PEPROCESS process
			);

		extern PHYSICAL_ADDRESS(__stdcall* pfn_mm_get_physical_address)(
			_In_ PVOID base_address
			);

		extern PVOID(__stdcall* pfn_ex_allocate_pool_with_tag)(
			_In_ POOL_TYPE pool_type,
			_In_ SIZE_T number_of_bytes,
			_In_ ULONG tag
			);

		extern VOID(__stdcall* pfn_ex_free_pool_with_tag)(
			_Frees_ptr_ PVOID p,
			_In_ ULONG tag
			);

		extern void(__stdcall* pfn_rtl_init_unicode_string)(
			_Out_ PUNICODE_STRING destination_string,
			_In_opt_z_ PCWSTR source_string
			);

		extern VOID(__stdcall* pfn_dbg_print)(
			_In_z_ PCSTR format,
			...
			);

		extern NTSTATUS(__stdcall* pfn_rtl_duplicate_unicode_string)(
			_In_ ULONG flags,
			_In_ const UNICODE_STRING* source_string,
			_Out_ UNICODE_STRING* destination_string
			);

		extern PVOID(__stdcall* pfn_mm_get_virtual_for_physical)(
			_In_ PHYSICAL_ADDRESS physical_address
			);

		extern KAFFINITY(__stdcall* pfn_ke_set_system_affinity_thread_ex)(
			_In_ KAFFINITY affinity
			);

		extern void(__stdcall* pfn_ke_revert_to_user_affinity_thread_ex)(
			_In_ KAFFINITY affinity
			);

		extern ULONG(__stdcall* pfn_ke_query_active_processor_count)(
			_In_opt_ PKAFFINITY active_processors
			);

		extern HANDLE(__stdcall* pfn_ps_get_process_id)(
			_In_ PVOID process
			);

		extern PVOID(__stdcall* pfn_ps_get_current_thread_process)();

		extern UCHAR*(__stdcall* pfn_ps_get_process_image_file_name)(
			_In_ PVOID process
			);



		extern ULONG(__stdcall* pfn_rtl_random_ex)(
			_Inout_ PULONG seed
			);

		extern VOID(__stdcall* pfn_rtl_init_string)(
			_Out_ PSTRING destination_string,
			_In_opt_z_ PCSZ source_string
			);

		extern NTSTATUS(__stdcall* pfn_zw_query_system_information)(
			_In_ SYSTEM_INFORMATION_CLASS system_information_class,
			_Out_ PVOID system_information,
			_In_ ULONG system_information_length,
			_Out_opt_ PULONG return_length
			);

		extern NTSTATUS(__stdcall* pfn_rtl_get_version)(
			_Inout_ PRTL_OSVERSIONINFOW version_information
			);

		extern INT(__cdecl* pfn__stricmp)(
			_In_z_ const char* string1,
			_In_z_ const char* string2
			);

		extern PVOID(__stdcall* pfn_mm_get_system_routine_address)(
			_In_ PUNICODE_STRING system_routine_name
			);

		extern BOOLEAN(__stdcall* pfn_rtl_equal_string)(
			_In_ const STRING* string1,
			_In_ const STRING* string2,
			_In_ BOOLEAN case_insensitive
			);

		extern VOID(__stdcall* pfn_ke_bug_check_ex)(
			_In_ ULONG bug_check_code,
			_In_ ULONG_PTR param1,
			_In_ ULONG_PTR param2,
			_In_ ULONG_PTR param3,
			_In_ ULONG_PTR param4
			);

		extern VOID(__stdcall* pfn_ke_revert_to_user_affinity_thread)(
			VOID);

		extern USHORT(__stdcall* pfn_ke_query_active_group_count)(VOID);

		extern ULONG(__stdcall* pfn_ke_query_active_processor_count_ex)(
			_In_ USHORT group_number
			);

		extern VOID(__stdcall* pfn_ke_set_system_group_affinity_thread)(
			_In_ PGROUP_AFFINITY affinity,
			_Out_opt_ PGROUP_AFFINITY previous_affinity
			);

		extern NTSTATUS(NTAPI * pfn_nt_create_section)(
			_Out_ PHANDLE SectionHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
			_In_opt_ PLARGE_INTEGER MaximumSize,
			_In_ ULONG SectionPageProtection,
			_In_ ULONG AllocationAttributes,
			_In_opt_ HANDLE FileHandle
			);

		extern NTSTATUS(NTAPI* pfn_nt_close)(
			_In_ _Post_ptr_invalid_ HANDLE handle
			);
	}
}
