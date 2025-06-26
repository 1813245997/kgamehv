#pragma once

#include "..\hookutils.h"

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

		extern PHANDLE_TABLE_ENTRY(NTAPI* pfn_exp_lookup_handle_table_entry)(
			_In_ PHANDLE_TABLE HandleTable,
			_In_ EXHANDLE Handle
			);

		extern PVOID(NTAPI* pfn_mm_create_kernel_stack)(
			unsigned long, short, PETHREAD
			);

		extern VOID(NTAPI* pfn_mm_delete_kernel_stack)(
			_In_ PVOID PointerKernelStack,
			_In_ BOOLEAN LargeStack
			);

		extern LARGE_INTEGER(NTAPI* pfn_ps_get_process_exit_time)();


		extern NTSTATUS(NTAPI* pfn_se_locate_process_image_name)(
			_In_  PEPROCESS       process,
			_Out_ PUNICODE_STRING *image_name
			);

		extern unsigned long long (NTAPI* pfn_ps_get_process_dxgprocess)(
			_In_  PEPROCESS   process);

		extern PVOID(NTAPI* pfn_ps_get_process_wow64_process)(
			IN PEPROCESS process);

		extern PVOID(NTAPI* pfn_ps_get_process_peb)(
			IN PEPROCESS process);

		extern NTSTATUS(NTAPI* pfn_mm_copy_virtual_memory)(
			_In_ PEPROCESS from_process,
			_In_ CONST PVOID from_address,
			_In_ PEPROCESS to_process,
			_Out_ PVOID to_address,
			_In_ SIZE_T buffer_size,
			_In_ KPROCESSOR_MODE previous_mode,
			_Out_ PSIZE_T number_of_bytes_copied
			);

		extern BOOLEAN(NTAPI* pfn_rtl_equal_unicode_string)(
			_In_ PCUNICODE_STRING string1,
			_In_ PCUNICODE_STRING string2,
			_In_ BOOLEAN case_insensitive
			);

		extern LONG(NTAPI* pfn_rtl_compare_unicode_string)(
			_In_ PCUNICODE_STRING string1,
			_In_ PCUNICODE_STRING string2,
			_In_ BOOLEAN case_insensitive
			);


		extern NTSTATUS(NTAPI* pfn_ps_lookup_process_by_process_id)(
			_In_ HANDLE process_id,
			_Outptr_ PEPROCESS* out_process
			);


		 

		extern void (NTAPI* pfn_ob_dereference_object)(
			_In_ PVOID object
			);

		extern VOID(NTAPI* pfn_ke_stack_attach_process)(
			_Inout_ PRKPROCESS process,
			_Out_ PVOID apc_state
			);

		extern VOID(NTAPI* pfn_ke_unstack_detach_process)(
			_In_ PVOID apc_state
			);

		extern HANDLE(NTAPI* pfn_ps_get_current_process_id)(
			VOID
			);

		extern NTSTATUS(NTAPI* pfn_zw_allocate_virtual_memory)(
			_In_ HANDLE process_handle,
			_Inout_ PVOID* base_address,
			_In_ ULONG_PTR zero_bits,
			_Inout_ PSIZE_T region_size,
			_In_ ULONG allocation_type,
			_In_ ULONG protect
			);

		extern NTSTATUS(NTAPI* pfn_zw_free_virtual_memory)(
			_In_ HANDLE process_handle,
			_Inout_ PVOID* base_address,
			_Inout_ PSIZE_T region_size,
			_In_ ULONG free_type
			);

		extern PVOID(NTAPI* pfn_ps_get_process_section_base_address)(
			_In_ PEPROCESS process
			);


		extern PMDL(NTAPI* pfn_io_allocate_mdl)(
			_In_opt_ __drv_aliasesMem PVOID VirtualAddress,
			_In_ ULONG Length,
			_In_ BOOLEAN SecondaryBuffer,
			_In_ BOOLEAN ChargeQuota,
			_Inout_opt_ PIRP Irp
			);

		extern VOID(NTAPI* pfn_mm_probe_and_lock_pages)(
			_Inout_ PMDL MemoryDescriptorList,
			_In_ KPROCESSOR_MODE AccessMode,
			_In_ LOCK_OPERATION Operation
			);

		extern VOID(NTAPI* pfn_mm_unlock_pages)(
			_Inout_ PMDL MemoryDescriptorList
			);


		extern VOID(NTAPI* pfn_io_free_mdl)(
			_In_ PMDL Mdl
			);

		extern KIRQL(NTAPI* pfn_ke_get_current_irql)(
			VOID
			);

		extern KPROCESSOR_MODE(NTAPI* pfn_ex_get_previous_mode)(VOID);

		extern PEPROCESS(NTAPI* pfn_ps_get_current_process)(VOID);

		extern PETHREAD(NTAPI* pfn_ps_get_current_thread)(VOID);

		extern PKTHREAD(NTAPI* pfn_ke_get_current_thread)(VOID);

		extern PVOID
		(__stdcall* pfn_mm_allocate_independent_pages)(
			IN SIZE_T number_of_bytes,
			IN ULONG node_number
			);

		extern VOID
		(__stdcall* pfn_mm_free_independent_pages)(
			IN PVOID virtual_address,
			IN SIZE_T number_of_bytes
			);


		extern NTSTATUS
		(NTAPI* pfn_zw_open_file)(
			_Out_ PHANDLE file_handle,
			_In_ ACCESS_MASK desired_access,
			_In_ POBJECT_ATTRIBUTES object_attributes,
			_Out_ PIO_STATUS_BLOCK io_status_block,
			_In_ ULONG share_access,
			_In_ ULONG open_options
			);

		extern NTSTATUS
		(NTAPI* pfn_zw_create_section)(
			_Out_ PHANDLE section_handle,
			_In_ ACCESS_MASK desired_access,
			_In_opt_ POBJECT_ATTRIBUTES object_attributes,
			_In_opt_ PLARGE_INTEGER maximum_size,
			_In_ ULONG section_page_protection,
			_In_ ULONG allocation_attributes,
			_In_opt_ HANDLE file_handle
			);

		extern NTSTATUS
		(NTAPI* pfn_zw_map_view_of_section)(
			_In_ HANDLE section_handle,
			_In_ HANDLE process_handle,
			_Outptr_result_bytebuffer_(*view_size) PVOID* base_address,
			_In_ ULONG_PTR zero_bits,
			_In_ SIZE_T commit_size,
			_Inout_opt_ PLARGE_INTEGER section_offset,
			_Inout_ PSIZE_T view_size,
			_In_ SECTION_INHERIT inherit_disposition,
			_In_ ULONG allocation_type,
			_In_ ULONG win32_protect
			);

		extern NTSTATUS
		(NTAPI* pfn_zw_close)(
			_In_ HANDLE handle
			);

 
		extern NTSTATUS
		(NTAPI* pfn_zw_unmap_view_of_section)(
			_In_ HANDLE process_handle,
			_In_opt_ PVOID base_address
			);


		extern NTSTATUS
		(NTAPI* pfn_nt_query_virtual_memory)(
			_In_ HANDLE ProcessHandle,
			_In_opt_ PVOID BaseAddress,
			_In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
			_Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
			_In_ SIZE_T MemoryInformationLength,
			_Out_opt_ PSIZE_T ReturnLength
			);

		extern NTSTATUS
		(NTAPI* pfn_nt_read_virtual_memory)(
			_In_ HANDLE ProcessHandle,
			_In_opt_ PVOID BaseAddress,
			_Out_writes_bytes_(NumberOfBytesToRead) PVOID Buffer,
			_In_ SIZE_T NumberOfBytesToRead,
			_Out_opt_ PSIZE_T NumberOfBytesRead
			);

		extern NTSTATUS
		(NTAPI* pfn_zw_query_information_process)(
			_In_ HANDLE ProcessHandle,
			_In_ PROCESSINFOCLASS ProcessInformationClass,
			_Out_writes_bytes_(ProcessInformationLength) PVOID ProcessInformation,
			_In_ ULONG ProcessInformationLength,
			_Out_opt_ PULONG ReturnLength
			);

		extern NTSTATUS(NTAPI* pfn_nt_protect_virtual_memory)(
			_In_ HANDLE ProcessHandle,
			_Inout_ PVOID* BaseAddress,
			_Inout_ PSIZE_T NumberOfBytesToProtect,
			_In_ ULONG NewAccessProtection,
			_Out_ PULONG OldAccessProtection
			);
	} 
}
