#include "global_defs.h"
#include "internal_function_defs.h"


#define INIT_FUNC_PTR(pfn, addr) pfn = reinterpret_cast<decltype(pfn)>(addr)

namespace utils
{
	namespace internal_functions
	{

		void(__fastcall* pfn_ki_preprocess_fault)(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode
			) = nullptr;

		NTSTATUS(__fastcall* pfn_mm_copy_memory)(
			_In_ PVOID target_address,
			_In_ MM_COPY_ADDRESS source_address,
			_In_ SIZE_T number_of_bytes,
			_In_ ULONG flags,
			_Out_ PSIZE_T number_of_bytes_transferred
			) = nullptr;

		BOOLEAN(__fastcall* pfn_mm_is_address_valid)(
			PVOID virtual_address
			) = nullptr;

	    BOOLEAN(__fastcall* pfn_mm_is_address_valid_ex)(
			PVOID virtual_address
			) = nullptr;

		USHORT(__fastcall* pfn_rtl_walk_frame_chain)(
			_Out_writes_(count - (flags >> RTL_STACK_WALKING_MODE_FRAMES_TO_SKIP_SHIFT)) PVOID* callers,
			_In_ ULONG count,
			_In_ ULONG flags
			) = nullptr;

		PRUNTIME_FUNCTION(__fastcall* pfn_rtl_lookup_function_entry)(
			_In_ DWORD64 control_pc,
			_Out_ PDWORD64 image_base,
			_Inout_opt_ PUNWIND_HISTORY_TABLE history_table
			) = nullptr;

		void(__fastcall* pfn_psp_exit_process)(
			BOOLEAN trim_address_space,
			PEPROCESS process
			) = nullptr;


		PHYSICAL_ADDRESS(__stdcall* pfn_mm_get_physical_address)(
			_In_ PVOID base_address
			) = nullptr;


		PVOID(__stdcall* pfn_ex_allocate_pool_with_tag)(
			_In_ POOL_TYPE pool_type,
			_In_ SIZE_T number_of_bytes,
			_In_ ULONG tag
			) = nullptr;


		VOID(__stdcall* pfn_ex_free_pool_with_tag)(
			_Frees_ptr_ PVOID p,
			_In_ ULONG tag
			) = nullptr;

		void(__stdcall* pfn_rtl_init_unicode_string)(
			_Out_ PUNICODE_STRING destination_string,
			_In_opt_z_ PCWSTR source_string
			) = nullptr;

		VOID(__stdcall* pfn_dbg_print)(
			_In_z_ PCSTR format,
			...
			) = nullptr;

		NTSTATUS(__stdcall* pfn_rtl_duplicate_unicode_string)(
			_In_ ULONG flags,
			_In_ const UNICODE_STRING* source_string,
			_Out_ UNICODE_STRING* destination_string
			) = nullptr;

		PVOID(__stdcall* pfn_mm_get_virtual_for_physical)(
			_In_ PHYSICAL_ADDRESS physical_address
			) = nullptr;

		KAFFINITY(__stdcall* pfn_ke_set_system_affinity_thread_ex)(
			_In_ KAFFINITY affinity
			) = nullptr;

		void(__stdcall* pfn_ke_revert_to_user_affinity_thread_ex)(
			_In_ KAFFINITY affinity
			) = nullptr;

		ULONG(__stdcall* pfn_ke_query_active_processor_count)(
			_In_opt_ PKAFFINITY active_processors
			) = nullptr;

		HANDLE(__stdcall* pfn_ps_get_process_id)(
			_In_ PVOID process
			) = nullptr;

		PVOID(__stdcall* pfn_ps_get_current_thread_process)() = nullptr;;

		 UCHAR* (__stdcall* pfn_ps_get_process_image_file_name)(
			_In_ PVOID process
			) = nullptr;
	 
		// PEPROCESS(__stdcall* pfn_ps_initial_system_process) = nullptr;

		ULONG(__stdcall* pfn_rtl_random_ex)(
			_Inout_ PULONG seed
			) = nullptr;

		VOID(__stdcall* pfn_rtl_init_string)(
			_Out_ PSTRING destination_string,
			_In_opt_z_ PCSZ source_string
			) = nullptr;

		NTSTATUS(__stdcall* pfn_zw_query_system_information)(
			_In_ SYSTEM_INFORMATION_CLASS system_information_class,
			_Out_ PVOID system_information,
			_In_ ULONG system_information_length,
			_Out_opt_ PULONG return_length
			) = nullptr;

		NTSTATUS(__stdcall* pfn_rtl_get_version)(
			_Inout_ PRTL_OSVERSIONINFOW version_information
			) = nullptr;

		INT(__cdecl* pfn__stricmp)(
			_In_z_ const char* string1,
			_In_z_ const char* string2
			) = nullptr;

		PVOID(__stdcall* pfn_mm_get_system_routine_address)(
			_In_ PUNICODE_STRING system_routine_name
			) = nullptr;

		BOOLEAN(__stdcall* pfn_rtl_equal_string)(
			_In_ const STRING* string1,
			_In_ const STRING* string2,
			_In_ BOOLEAN case_insensitive
			) = nullptr;

		VOID(__stdcall* pfn_ke_bug_check_ex)(
			_In_ ULONG bug_check_code,
			_In_ ULONG_PTR param1,
			_In_ ULONG_PTR param2,
			_In_ ULONG_PTR param3,
			_In_ ULONG_PTR param4
			) = nullptr;


		 VOID(__stdcall* pfn_ke_revert_to_user_affinity_thread)(
			 VOID)  = nullptr;

		  USHORT(__stdcall* pfn_ke_query_active_group_count)(
			 VOID) = nullptr;

		  ULONG(__stdcall* pfn_ke_query_active_processor_count_ex)(
			  _In_ USHORT group_number
			  ) = nullptr;

		   VOID(__stdcall* pfn_ke_set_system_group_affinity_thread)(
			  _In_ PGROUP_AFFINITY affinity,
			  _Out_opt_ PGROUP_AFFINITY previous_affinity
			  ) = nullptr;

		   NTSTATUS(NTAPI * pfn_nt_create_section)(
			   _Out_ PHANDLE SectionHandle,
			   _In_ ACCESS_MASK DesiredAccess,
			   _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
			   _In_opt_ PLARGE_INTEGER MaximumSize,
			   _In_ ULONG SectionPageProtection,
			   _In_ ULONG AllocationAttributes,
			   _In_opt_ HANDLE FileHandle
			   ) = nullptr;

		   NTSTATUS(NTAPI* pfn_nt_close)(
			   _In_ _Post_ptr_invalid_ HANDLE handle
			   ) = nullptr;


		   PHANDLE_TABLE_ENTRY(NTAPI* pfn_exp_lookup_handle_table_entry)(
			   _In_ PHANDLE_TABLE HandleTable,
			   _In_ EXHANDLE Handle
			   ) = nullptr;

		    PVOID(NTAPI* pfn_mm_create_kernel_stack)(
			   _In_ BOOLEAN LargeStack,
			   _In_ UCHAR Processor
			   ) = nullptr;

		    VOID(NTAPI* pfn_mm_delete_kernel_stack)(
			   _In_ PVOID PointerKernelStack,
			   _In_ BOOLEAN LargeStack
			   ) = nullptr;

		NTSTATUS initialize_internal_functions()
		{
			auto ntoskrnl_base = module_info::ntoskrnl_base;
			DbgBreakPoint();
		 
			unsigned long long rtl_get_version_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "RtlGetVersion");
			unsigned long long mm_copy_memory_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "MmCopyMemory");
			unsigned long long mm_is_address_valid_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "MmIsAddressValid");
			unsigned long long rtl_walk_frame_chain_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "RtlWalkFrameChain");
			unsigned long long rtl_lookup_function_entry_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "RtlLookupFunctionEntry");
			unsigned long long mm_get_physical_address_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "MmGetPhysicalAddress");
			unsigned long long ex_allocate_pool_with_tag_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ExAllocatePoolWithTag");
			unsigned long long ex_free_pool_with_tag_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ExFreePoolWithTag");
			unsigned long long rtl_init_unicode_string_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "RtlInitUnicodeString");
			unsigned long long dbg_print_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "DbgPrint");
			unsigned long long rtl_duplicate_unicode_string_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "RtlDuplicateUnicodeString");
			unsigned long long mm_get_virtual_for_physical_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "MmGetVirtualForPhysical");
			unsigned long long ke_set_system_affinity_thread_ex_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeSetSystemAffinityThreadEx");
			unsigned long long ke_revert_to_user_affinity_thread_ex_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeRevertToUserAffinityThreadEx");
			unsigned long long ke_query_active_processor_count_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeQueryActiveProcessorCount");
			unsigned long long ps_get_process_id_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetProcessId");
			unsigned long long ps_get_current_thread_process_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetCurrentThreadProcess");
			unsigned long long ps_get_process_image_file_name_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetProcessImageFileName");
			unsigned long long ps_initial_system_process_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsInitialSystemProcess");
			unsigned long long rtl_random_ex_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "RtlRandomEx");
			unsigned long long rtl_init_string_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "RtlInitString");
			unsigned long long zw_query_system_information_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ZwQuerySystemInformation");
			unsigned long long stricmp_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "_stricmp");
			unsigned long long mm_get_system_routine_address_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "MmGetSystemRoutineAddress");
			unsigned long long rtl_equal_string_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "RtlEqualString");
			unsigned long long ke_bug_check_ex_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeBugCheckEx");
			unsigned long long ke_revert_to_user_affinity_thread_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeRevertToUserAffinityThread");
			unsigned long long ke_query_active_group_count_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeQueryActiveGroupCount");
			unsigned long long ke_query_active_processor_count_ex_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeQueryActiveProcessorCountEx");
			unsigned long long ke_set_system_group_affinity_thread_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeSetSystemGroupAffinityThread");
		    unsigned long long nt_create_section_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "NtCreateSection");
			unsigned long long nt_close_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "NtClose");




			INIT_FUNC_PTR(pfn_mm_copy_memory, mm_copy_memory_addr);
			INIT_FUNC_PTR(pfn_mm_is_address_valid, mm_is_address_valid_addr);
			INIT_FUNC_PTR(pfn_rtl_walk_frame_chain, rtl_walk_frame_chain_addr);
			INIT_FUNC_PTR(pfn_rtl_lookup_function_entry, rtl_lookup_function_entry_addr);
			INIT_FUNC_PTR(pfn_mm_get_physical_address, mm_get_physical_address_addr);
			INIT_FUNC_PTR(pfn_ex_allocate_pool_with_tag, ex_allocate_pool_with_tag_addr);
			INIT_FUNC_PTR(pfn_ex_free_pool_with_tag, ex_free_pool_with_tag_addr);
			INIT_FUNC_PTR(pfn_rtl_init_unicode_string, rtl_init_unicode_string_addr);
			INIT_FUNC_PTR(pfn_dbg_print, dbg_print_addr);
			INIT_FUNC_PTR(pfn_rtl_duplicate_unicode_string, rtl_duplicate_unicode_string_addr);
			INIT_FUNC_PTR(pfn_mm_get_virtual_for_physical, mm_get_virtual_for_physical_addr);
			INIT_FUNC_PTR(pfn_ke_set_system_affinity_thread_ex, ke_set_system_affinity_thread_ex_addr);
			INIT_FUNC_PTR(pfn_ke_revert_to_user_affinity_thread_ex, ke_revert_to_user_affinity_thread_ex_addr);
			INIT_FUNC_PTR(pfn_ke_query_active_processor_count, ke_query_active_processor_count_addr);
			INIT_FUNC_PTR(pfn_ps_get_process_id, ps_get_process_id_addr);
			INIT_FUNC_PTR(pfn_ps_get_current_thread_process, ps_get_current_thread_process_addr);
			INIT_FUNC_PTR(pfn_ps_get_process_image_file_name, ps_get_process_image_file_name_addr);
			//INIT_FUNC_PTR(pfn_ps_initial_system_process, ps_initial_system_process_addr);
			INIT_FUNC_PTR(pfn_rtl_random_ex, rtl_random_ex_addr);
			INIT_FUNC_PTR(pfn_rtl_init_string, rtl_init_string_addr);
			INIT_FUNC_PTR(pfn_zw_query_system_information, zw_query_system_information_addr);
			INIT_FUNC_PTR(pfn_rtl_get_version, rtl_get_version_addr);
			INIT_FUNC_PTR(pfn__stricmp, stricmp_addr);
			INIT_FUNC_PTR(pfn_mm_get_system_routine_address, mm_get_system_routine_address_addr);
			INIT_FUNC_PTR(pfn_rtl_equal_string, rtl_equal_string_addr);
			INIT_FUNC_PTR(pfn_ke_bug_check_ex, ke_bug_check_ex_addr);
			INIT_FUNC_PTR(pfn_ke_revert_to_user_affinity_thread, ke_revert_to_user_affinity_thread_addr);
			INIT_FUNC_PTR(pfn_ke_query_active_group_count, ke_query_active_group_count_addr);
			INIT_FUNC_PTR(pfn_ke_query_active_processor_count_ex, ke_query_active_processor_count_ex_addr);
			INIT_FUNC_PTR(pfn_ke_set_system_group_affinity_thread, ke_set_system_group_affinity_thread_addr);
			INIT_FUNC_PTR(pfn_nt_create_section, nt_create_section_addr);
			INIT_FUNC_PTR(pfn_nt_close, nt_close_addr);
			 
			//These three search feature codes will cause errors. Find a way to solve it.
			unsigned long long ki_preprocess_fault_addr = scanner_fun::find_ki_preprocess_fault();
			unsigned long long psp_exit_process_addr = scanner_fun::find_psp_exit_process();
			unsigned long long mm_is_address_valid_ex_addr = scanner_fun::find_mm_is_address_valid_ex();
			unsigned long long exp_lookup_handle_table_entry_addr = scanner_fun::find_exp_lookup_handle_table_entry();
			unsigned long long mm_create_kernel_stack_addr = scanner_fun::find_mm_create_kernel_stack();
			unsigned long long mm_delete_kernel_stack_addr = scanner_fun::find_mm_delete_kernel_stack();

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] RtlGetVersion               = %p\n", reinterpret_cast<PVOID>(rtl_get_version_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] MmCopyMemory               = %p\n", reinterpret_cast<PVOID>(mm_copy_memory_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] MmIsAddressValid           = %p\n", reinterpret_cast<PVOID>(mm_is_address_valid_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] RtlWalkFrameChain          = %p\n", reinterpret_cast<PVOID>(rtl_walk_frame_chain_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] RtlLookupFunctionEntry     = %p\n", reinterpret_cast<PVOID>(rtl_lookup_function_entry_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] MmGetPhysicalAddress       = %p\n", reinterpret_cast<PVOID>(mm_get_physical_address_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ExAllocatePoolWithTag      = %p\n", reinterpret_cast<PVOID>(ex_allocate_pool_with_tag_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ExFreePoolWithTag          = %p\n", reinterpret_cast<PVOID>(ex_free_pool_with_tag_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] RtlInitUnicodeString       = %p\n", reinterpret_cast<PVOID>(rtl_init_unicode_string_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] DbgPrint                   = %p\n", reinterpret_cast<PVOID>(dbg_print_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] RtlDuplicateUnicodeString  = %p\n", reinterpret_cast<PVOID>(rtl_duplicate_unicode_string_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] MmGetVirtualForPhysical    = %p\n", reinterpret_cast<PVOID>(mm_get_virtual_for_physical_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] KeSetSystemAffinityThreadEx       = %p\n", reinterpret_cast<PVOID>(ke_set_system_affinity_thread_ex_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] KeRevertToUserAffinityThreadEx    = %p\n", reinterpret_cast<PVOID>(ke_revert_to_user_affinity_thread_ex_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] KeQueryActiveProcessorCount       = %p\n", reinterpret_cast<PVOID>(ke_query_active_processor_count_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] PsGetProcessId                     = %p\n", reinterpret_cast<PVOID>(ps_get_process_id_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] PsGetCurrentThreadProcess          = %p\n", reinterpret_cast<PVOID>(ps_get_current_thread_process_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] PsGetProcessImageFileName          = %p\n", reinterpret_cast<PVOID>(ps_get_process_image_file_name_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] PsInitialSystemProcess             = %p\n", reinterpret_cast<PVOID>(ps_initial_system_process_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] RtlRandomEx                        = %p\n", reinterpret_cast<PVOID>(rtl_random_ex_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] RtlInitString                      = %p\n", reinterpret_cast<PVOID>(rtl_init_string_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ZwQuerySystemInformation           = %p\n", reinterpret_cast<PVOID>(zw_query_system_information_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] _stricmp                           = %p\n", reinterpret_cast<PVOID>(stricmp_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] MmGetSystemRoutineAddress          = %p\n", reinterpret_cast<PVOID>(mm_get_system_routine_address_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] RtlEqualString                     = %p\n", reinterpret_cast<PVOID>(rtl_equal_string_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] KeBugCheckEx                       = %p\n", reinterpret_cast<PVOID>(ke_bug_check_ex_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] KeRevertToUserAffinityThread       = %p\n", reinterpret_cast<PVOID>(ke_revert_to_user_affinity_thread_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] KeQueryActiveGroupCount            = %p\n", reinterpret_cast<PVOID>(ke_query_active_group_count_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] KeQueryActiveProcessorCountEx      = %p\n", reinterpret_cast<PVOID>(ke_query_active_processor_count_ex_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] KeSetSystemGroupAffinityThread     = %p\n", reinterpret_cast<PVOID>(ke_set_system_group_affinity_thread_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] NtCreateSection                    = %p\n", reinterpret_cast<PVOID>(nt_create_section_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] NtClose                            = %p\n", reinterpret_cast<PVOID>(nt_close_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,"[hv] ki_preprocess_fault_addr     = %p\n",reinterpret_cast<PVOID>(ki_preprocess_fault_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,"[hv] psp_exit_process_addr     = %p\n",reinterpret_cast<PVOID>(psp_exit_process_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,"[hv] mm_is_address_valid_ex_addr     = %p\n",reinterpret_cast<PVOID>(mm_is_address_valid_ex_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] exp_lookup_handle_table_entry_addr     = %p\n", reinterpret_cast<PVOID>(exp_lookup_handle_table_entry_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] mm_create_kernel_stack_addr     = %p\n", reinterpret_cast<PVOID>(mm_create_kernel_stack_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] mm_delete_kernel_stack_addr     = %p\n", reinterpret_cast<PVOID>(mm_delete_kernel_stack_addr));

		 


			INIT_FUNC_PTR(pfn_ki_preprocess_fault, ki_preprocess_fault_addr);
			INIT_FUNC_PTR(pfn_psp_exit_process, psp_exit_process_addr);
			INIT_FUNC_PTR(pfn_mm_is_address_valid_ex, mm_is_address_valid_ex_addr);
			INIT_FUNC_PTR(pfn_exp_lookup_handle_table_entry, exp_lookup_handle_table_entry_addr);
			INIT_FUNC_PTR(pfn_mm_create_kernel_stack , mm_create_kernel_stack_addr);
			INIT_FUNC_PTR(pfn_mm_delete_kernel_stack, mm_delete_kernel_stack_addr);



			if (!ki_preprocess_fault_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ki_preprocess_fault_addr is null.\n");
			if (!psp_exit_process_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] psp_exit_process_addr is null.\n");
			if (!mm_is_address_valid_ex_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_is_address_valid_ex_addr is null.\n");
			if (!mm_copy_memory_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_copy_memory_addr is null.\n");
			if (!mm_is_address_valid_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_is_address_valid_addr is null.\n");
			if (!rtl_walk_frame_chain_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] rtl_walk_frame_chain_addr is null.\n");
			if (!rtl_lookup_function_entry_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] rtl_lookup_function_entry_addr is null.\n");
			if (!mm_get_physical_address_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_get_physical_address_addr is null.\n");
			if (!ex_allocate_pool_with_tag_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ex_allocate_pool_with_tag_addr is null.\n");
			if (!ex_free_pool_with_tag_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ex_free_pool_with_tag_addr is null.\n");
			if (!rtl_init_unicode_string_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] rtl_init_unicode_string_addr is null.\n");
			if (!dbg_print_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] dbg_print_addr is null.\n");
			if (!rtl_duplicate_unicode_string_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] rtl_duplicate_unicode_string_addr is null.\n");
			if (!mm_get_virtual_for_physical_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_get_virtual_for_physical_addr is null.\n");
			if (!ke_set_system_affinity_thread_ex_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_set_system_affinity_thread_ex_addr is null.\n");
			if (!ke_revert_to_user_affinity_thread_ex_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_revert_to_user_affinity_thread_ex_addr is null.\n");
			if (!ke_query_active_processor_count_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_query_active_processor_count_addr is null.\n");
			if (!ps_get_process_id_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_process_id_addr is null.\n");
			if (!ps_get_current_thread_process_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_current_thread_process_addr is null.\n");
			if (!ps_get_process_image_file_name_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_process_image_file_name_addr is null.\n");
			if (!ps_initial_system_process_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_initial_system_process_addr is null.\n");
			if (!rtl_random_ex_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] rtl_random_ex_addr is null.\n");
			if (!rtl_init_string_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] rtl_init_string_addr is null.\n");
			if (!zw_query_system_information_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] zw_query_system_information_addr is null.\n");
			if (!rtl_get_version_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] rtl_get_version_addr is null.\n");
			if (!stricmp_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] stricmp_addr is null.\n");
			if (!mm_get_system_routine_address_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_get_system_routine_address_addr is null.\n");
			if (!rtl_equal_string_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] rtl_equal_string_addr is null.\n");
			if (!ke_bug_check_ex_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_bug_check_ex_addr is null.\n");
			if (!ke_revert_to_user_affinity_thread_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_revert_to_user_affinity_thread_addr is null.\n");
			if (!ke_query_active_group_count_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_query_active_group_count_addr is null.\n");
			if (!ke_query_active_processor_count_ex_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_query_active_processor_count_ex_addr is null.\n");
			if (!ke_set_system_group_affinity_thread_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_set_system_group_affinity_thread_addr is null.\n");
			if (!nt_create_section_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] nt_create_section_addr is null.\n");
			if (!nt_close_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] nt_close_addr is null.\n");
			if (!exp_lookup_handle_table_entry_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] exp_lookup_handle_table_entry_addr is null.\n");
			if (!mm_create_kernel_stack_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_create_kernel_stack_addr is null.\n");
			if (!mm_delete_kernel_stack_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_delete_kernel_stack_addr is null.\n");

		 

			if (!ki_preprocess_fault_addr||
				!psp_exit_process_addr||
				!mm_is_address_valid_ex_addr||
				!mm_copy_memory_addr||
				!mm_is_address_valid_addr||
				!rtl_walk_frame_chain_addr||
				!rtl_lookup_function_entry_addr||
				!mm_get_physical_address_addr||
				!ex_allocate_pool_with_tag_addr||
				!ex_free_pool_with_tag_addr||
				!rtl_init_unicode_string_addr||
				!dbg_print_addr || 
				!rtl_duplicate_unicode_string_addr ||
				!mm_get_virtual_for_physical_addr ||
				!ke_set_system_affinity_thread_ex_addr || 
				!ke_revert_to_user_affinity_thread_ex_addr ||
				!ke_query_active_processor_count_addr ||
				!ps_get_process_id_addr || 
				!ps_get_current_thread_process_addr || 
				!ps_get_process_image_file_name_addr ||
				!ps_initial_system_process_addr ||
				!rtl_random_ex_addr || 
				!rtl_init_string_addr || 
				!zw_query_system_information_addr || 
				!rtl_get_version_addr ||
				!stricmp_addr || 
				!mm_get_system_routine_address_addr || 
				!rtl_equal_string_addr || 
				!ke_bug_check_ex_addr||
				!ke_revert_to_user_affinity_thread_addr||
				!ke_query_active_group_count_addr||
				!ke_query_active_processor_count_ex_addr ||
				!ke_set_system_group_affinity_thread_addr||
				!nt_create_section_addr||
				!nt_close_addr||
				!exp_lookup_handle_table_entry_addr||
				!mm_create_kernel_stack_addr||
				!mm_delete_kernel_stack_addr)
			{
				return STATUS_UNSUCCESSFUL;
			}

			 




			return STATUS_SUCCESS;
		}

	}
}
