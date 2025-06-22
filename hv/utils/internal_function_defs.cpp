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
			   unsigned long, short, PETHREAD
			   ) = nullptr;

		    VOID(NTAPI* pfn_mm_delete_kernel_stack)(
			   _In_ PVOID PointerKernelStack,
			   _In_ BOOLEAN LargeStack
			   ) = nullptr;

			  LARGE_INTEGER(NTAPI* pfn_ps_get_process_exit_time)() = nullptr;

			NTSTATUS(NTAPI* pfn_se_locate_process_image_name)(
				  _In_  PEPROCESS       process,
				  _Out_ PUNICODE_STRING *image_name
				  ) = nullptr;

			unsigned long long (NTAPI* pfn_ps_get_process_dxgprocess)(
				_In_  PEPROCESS   process) = nullptr;

			 PVOID(NTAPI* pfn_ps_get_process_wow64_process)(
				IN PEPROCESS process) = nullptr;

		     PVOID(NTAPI* pfn_ps_get_process_peb)(
				IN PEPROCESS process) = nullptr;

			 NTSTATUS(NTAPI* pfn_mm_copy_virtual_memory)(
				 _In_ PEPROCESS from_process,
				 _In_ CONST PVOID from_address,
				 _In_ PEPROCESS to_process,
				 _Out_ PVOID to_address,
				 _In_ SIZE_T buffer_size,
				 _In_ KPROCESSOR_MODE previous_mode,
				 _Out_ PSIZE_T number_of_bytes_copied
				 ) = nullptr;

			  BOOLEAN(NTAPI* pfn_rtl_equal_unicode_string)(
				 _In_ PCUNICODE_STRING string1,
				 _In_ PCUNICODE_STRING string2,
				 _In_ BOOLEAN case_insensitive
				 ) = nullptr;

			   LONG(NTAPI* pfn_rtl_compare_unicode_string)(
				  _In_ PCUNICODE_STRING string1,
				  _In_ PCUNICODE_STRING string2,
				  _In_ BOOLEAN case_insensitive
				  ) = nullptr;

			   NTSTATUS(NTAPI* pfn_ps_lookup_process_by_process_id)(
				   _In_ HANDLE process_id,
				   _Outptr_ PEPROCESS* out_process
				   ) = nullptr;

			   void (NTAPI* pfn_ob_dereference_object)(
				   _In_ PVOID object
				   ) = nullptr;

			   VOID(NTAPI* pfn_ke_stack_attach_process)(
				   _Inout_ PRKPROCESS process,
				   _Out_ PVOID apc_state
				   ) = nullptr;

			    VOID(NTAPI* pfn_ke_unstack_detach_process)(
				   _In_ PVOID apc_state
				   ) = nullptr;

			HANDLE(NTAPI* pfn_ps_get_current_process_id)(
					VOID
					)=nullptr;

			  NTSTATUS(NTAPI* pfn_zw_allocate_virtual_memory)(
				_In_ HANDLE process_handle,
				_Inout_ PVOID* base_address,
				_In_ ULONG_PTR zero_bits,
				_Inout_ PSIZE_T region_size,
				_In_ ULONG allocation_type,
				_In_ ULONG protect
				) = nullptr;

			    NTSTATUS(NTAPI* pfn_zw_free_virtual_memory)(
				  _In_ HANDLE process_handle,
				  _Inout_ PVOID* base_address,
				  _Inout_ PSIZE_T region_size,
				  _In_ ULONG free_type
				  ) = nullptr;

			   PVOID(NTAPI* pfn_ps_get_process_section_base_address)(
				  _In_ PEPROCESS process
				  ) = nullptr;

			    PMDL(NTAPI* pfn_io_allocate_mdl)(
				   _In_opt_ __drv_aliasesMem PVOID VirtualAddress,
				   _In_ ULONG Length,
				   _In_ BOOLEAN SecondaryBuffer,
				   _In_ BOOLEAN ChargeQuota,
				   _Inout_opt_ PIRP Irp
				   ) = nullptr;

				VOID(NTAPI* pfn_mm_probe_and_lock_pages)(
					_Inout_ PMDL MemoryDescriptorList,
					_In_ KPROCESSOR_MODE AccessMode,
					_In_ LOCK_OPERATION Operation
					) = nullptr;

				 VOID(NTAPI* pfn_mm_unlock_pages)(
					_Inout_ PMDL MemoryDescriptorList
					) = nullptr;


				 VOID(NTAPI* pfn_io_free_mdl)(
					_In_ PMDL Mdl
					) = nullptr;

				  KIRQL(NTAPI* pfn_ke_get_current_irql)(
					 VOID
					 ) = nullptr;

				  KPROCESSOR_MODE(NTAPI* pfn_ex_get_previous_mode)
					  (VOID) = nullptr;

				   PEPROCESS(NTAPI* pfn_ps_get_current_process)
					  (VOID)= nullptr;

				   PETHREAD(NTAPI* pfn_ps_get_current_thread)
					   (VOID) = nullptr;

				   PETHREAD(NTAPI* pfn_ke_get_current_thread)
					   (VOID) = nullptr;

				   PVOID
				   (__stdcall* pfn_mm_allocate_independent_pages)(
					   IN SIZE_T number_of_bytes,
					   IN ULONG node_number
					   ) = nullptr;

				   VOID
				   (__stdcall* pfn_mm_free_independent_pages)(
					   IN PVOID virtual_address,
					   IN SIZE_T number_of_bytes
					   ) = nullptr;

				    NTSTATUS
				   (NTAPI* pfn_zw_open_file)(
					   _Out_ PHANDLE file_handle,
					   _In_ ACCESS_MASK desired_access,
					   _In_ POBJECT_ATTRIBUTES object_attributes,
					   _Out_ PIO_STATUS_BLOCK io_status_block,
					   _In_ ULONG share_access,
					   _In_ ULONG open_options
					   ) = nullptr;

					  NTSTATUS
					(NTAPI* pfn_zw_create_section)(
						_Out_ PHANDLE section_handle,
						_In_ ACCESS_MASK desired_access,
						_In_opt_ POBJECT_ATTRIBUTES object_attributes,
						_In_opt_ PLARGE_INTEGER maximum_size,
						_In_ ULONG section_page_protection,
						_In_ ULONG allocation_attributes,
						_In_opt_ HANDLE file_handle
						) = nullptr;

					  NTSTATUS
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
						  ) = nullptr;


					    NTSTATUS
					  (NTAPI* pfn_zw_close)(
						  _In_ HANDLE handle
						  ) = nullptr;

					 NTSTATUS
						(NTAPI* pfn_zw_unmap_view_of_section)(
							_In_ HANDLE process_handle,
							_In_opt_ PVOID base_address
							) = nullptr;



				    NTSTATUS
				   (NTAPI* pfn_nt_query_virtual_memory)(
					   _In_ HANDLE ProcessHandle,
					   _In_opt_ PVOID BaseAddress,
					   _In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
					   _Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
					   _In_ SIZE_T MemoryInformationLength,
					   _Out_opt_ PSIZE_T ReturnLength
					   ) = nullptr;

				  NTSTATUS
					(NTAPI* pfn_nt_read_virtual_memory)(
						_In_ HANDLE ProcessHandle,
						_In_opt_ PVOID BaseAddress,
						_Out_writes_bytes_(NumberOfBytesToRead) PVOID Buffer,
						_In_ SIZE_T NumberOfBytesToRead,
						_Out_opt_ PSIZE_T NumberOfBytesRead
						) = nullptr;

				    NTSTATUS
				  (NTAPI* pfn_zw_query_information_process)(
					  _In_ HANDLE ProcessHandle,
					  _In_ PROCESSINFOCLASS ProcessInformationClass,
					  _Out_writes_bytes_(ProcessInformationLength) PVOID ProcessInformation,
					  _In_ ULONG ProcessInformationLength,
					  _Out_opt_ PULONG ReturnLength
					  ) = nullptr;


		NTSTATUS initialize_internal_functions()
		{
			 
			 
			auto ntoskrnl_base = module_info::ntoskrnl_base;
		 
		 
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
			unsigned long long ps_get_process_exit_time_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetProcessExitTime");
		    unsigned long long se_locate_process_image_name_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "SeLocateProcessImageName");
			unsigned long long ps_get_process_dxgprocess_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetProcessDxgProcess");
			unsigned long long ps_get_process_wow64_process_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetProcessWow64Process");
			unsigned long long ps_get_process_peb_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetProcessPeb");
			unsigned long long mm_copy_virtual_memory_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "MmCopyVirtualMemory");
			unsigned long long rtl_equal_unicode_string_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "RtlEqualUnicodeString");
			unsigned long long rtl_compare_unicode_string_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "RtlCompareUnicodeString");
			unsigned long long ps_lookup_process_by_process_id_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsLookupProcessByProcessId");
			unsigned long long ob_dereference_object_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ObDereferenceObject");
			unsigned long long ke_stack_attach_process_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeStackAttachProcess");
			unsigned long long ke_unstack_detach_process_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeUnstackDetachProcess");
			unsigned long long ps_get_current_process_id_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetCurrentProcessId");
			unsigned long long zw_allocate_virtual_memory_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ZwAllocateVirtualMemory");
			unsigned long long zw_free_virtual_memory_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ZwFreeVirtualMemory");
			unsigned long long ps_get_process_section_base_address_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetProcessSectionBaseAddress");
			unsigned long long io_allocate_mdl_addr =  scanner_fun::find_module_export_by_name(ntoskrnl_base, "IoAllocateMdl");
			unsigned long long mm_probe_and_lock_pages_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "MmProbeAndLockPages");
			unsigned long long mm_unlock_pages_addr =  scanner_fun::find_module_export_by_name(ntoskrnl_base, "MmUnlockPages");
			unsigned long long io_free_mdl_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "IoFreeMdl");
			unsigned long long ke_get_current_irql_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeGetCurrentIrql");
			unsigned long long ex_get_previous_mode_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ExGetPreviousMode");
			unsigned long long ps_get_current_process_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetCurrentProcess");
			unsigned long long ps_get_current_thread_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "PsGetCurrentProcess");
			unsigned long long ke_get_current_thread_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "KeGetCurrentThread");
			unsigned long long zw_open_file_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ZwOpenFile");
			unsigned long long zw_create_section_addr  = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ZwCreateSection");
			unsigned long long zw_map_view_of_section_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ZwMapViewOfSection");
			unsigned long long zw_close_addr =  scanner_fun::find_module_export_by_name(ntoskrnl_base, "ZwClose");
			unsigned long long zw_unmap_view_of_section_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ZwUnmapViewOfSection");
			unsigned long long zw_query_information_process_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ZwQueryInformationProcess");
			//unsigned long long zw_query_virtual_memory_addr = scanner_fun::find_module_export_by_name(ntoskrnl_base, "ZwQueryVirtualMemory");
			 

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
			INIT_FUNC_PTR(pfn_ps_get_process_exit_time, ps_get_process_exit_time_addr);
			INIT_FUNC_PTR(pfn_se_locate_process_image_name, se_locate_process_image_name_addr);
			INIT_FUNC_PTR(pfn_ps_get_process_dxgprocess,ps_get_process_dxgprocess_addr);
			INIT_FUNC_PTR(pfn_ps_get_process_wow64_process, ps_get_process_wow64_process_addr);
			INIT_FUNC_PTR(pfn_ps_get_process_peb, ps_get_process_peb_addr);
			INIT_FUNC_PTR(pfn_mm_copy_virtual_memory, mm_copy_virtual_memory_addr);
			INIT_FUNC_PTR(pfn_rtl_equal_unicode_string, rtl_equal_unicode_string_addr);
			INIT_FUNC_PTR(pfn_rtl_compare_unicode_string, rtl_compare_unicode_string_addr);
			INIT_FUNC_PTR(pfn_ps_lookup_process_by_process_id, ps_lookup_process_by_process_id_addr);
			INIT_FUNC_PTR(pfn_ob_dereference_object, ob_dereference_object_addr);
			INIT_FUNC_PTR(pfn_ke_stack_attach_process, ke_stack_attach_process_addr);
			INIT_FUNC_PTR(pfn_ke_unstack_detach_process, ke_unstack_detach_process_addr);
			INIT_FUNC_PTR(pfn_ps_get_current_process_id, ps_get_current_process_id_addr);
			INIT_FUNC_PTR(pfn_zw_allocate_virtual_memory, zw_allocate_virtual_memory_addr);
			INIT_FUNC_PTR(pfn_ps_get_process_section_base_address, ps_get_process_section_base_address_addr);
			INIT_FUNC_PTR(pfn_io_allocate_mdl, io_allocate_mdl_addr);
			INIT_FUNC_PTR(pfn_mm_probe_and_lock_pages, mm_probe_and_lock_pages_addr);
			INIT_FUNC_PTR(pfn_mm_unlock_pages, mm_unlock_pages_addr);
			INIT_FUNC_PTR(pfn_io_free_mdl, io_free_mdl_addr);
			INIT_FUNC_PTR(pfn_ke_get_current_irql, ke_get_current_irql_addr);
			INIT_FUNC_PTR(pfn_ex_get_previous_mode, ex_get_previous_mode_addr);
			INIT_FUNC_PTR(pfn_ps_get_current_process, ps_get_current_process_addr);
			INIT_FUNC_PTR(pfn_ps_get_current_thread , ps_get_current_thread_addr);
			INIT_FUNC_PTR(pfn_ke_get_current_thread, ke_get_current_thread_addr);
			INIT_FUNC_PTR(pfn_zw_open_file, zw_open_file_addr);
			INIT_FUNC_PTR(pfn_zw_create_section, zw_create_section_addr);
			INIT_FUNC_PTR(pfn_zw_map_view_of_section, zw_map_view_of_section_addr);
			INIT_FUNC_PTR(pfn_zw_close, zw_close_addr);
			INIT_FUNC_PTR(pfn_zw_unmap_view_of_section, zw_unmap_view_of_section_addr);
			INIT_FUNC_PTR(pfn_zw_query_information_process, zw_query_information_process_addr);
			INIT_FUNC_PTR(pfn_zw_free_virtual_memory, zw_free_virtual_memory_addr);
			//rtl_compare_unicode_string_addr
 
			//These three search feature codes will cause errors. Find a way to solve it.
			unsigned long long ki_preprocess_fault_addr = scanner_fun::find_ki_preprocess_fault();
			unsigned long long psp_exit_process_addr = scanner_fun::find_psp_exit_process();
			unsigned long long mm_is_address_valid_ex_addr = scanner_fun::find_mm_is_address_valid_ex();
			unsigned long long exp_lookup_handle_table_entry_addr = scanner_fun::find_exp_lookup_handle_table_entry();
			unsigned long long mm_create_kernel_stack_addr = scanner_fun::find_mm_create_kernel_stack();
			unsigned long long mm_delete_kernel_stack_addr = scanner_fun::find_mm_delete_kernel_stack();
			unsigned long long mm_allocate_independent_pages_addr = scanner_fun::find_mm_allocate_independent_pages();
			unsigned long long mm_free_independent_pages_addr = scanner_fun::find_mm_free_independent_pages();
			 
			unsigned long long nt_query_virtual_memory_addr = ssdt::get_syscall_fun_addr(ntoskrnl_base, "NtQueryVirtualMemory");
			unsigned long long nt_read_virtual_memory_addr = ssdt::get_syscall_fun_addr(ntoskrnl_base, "NtReadVirtualMemory");

		 
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
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ps_get_process_exit_time_addr      = %p\n", reinterpret_cast<PVOID>(ps_get_process_exit_time_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] se_locate_process_image_name_addr      = %p\n", reinterpret_cast<PVOID>(se_locate_process_image_name_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ps_get_process_dxgprocess_addr      = %p\n", reinterpret_cast<PVOID>(ps_get_process_dxgprocess_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ps_get_process_wow64_process_addr      = %p\n", reinterpret_cast<PVOID>(ps_get_process_wow64_process_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ps_get_process_peb_addr      = %p\n", reinterpret_cast<PVOID>(ps_get_process_peb_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] mm_copy_virtual_memory_addr      = %p\n", reinterpret_cast<PVOID>(mm_copy_virtual_memory_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] rtl_equal_unicode_string_addr      = %p\n", reinterpret_cast<PVOID>(rtl_equal_unicode_string_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] rtl_compare_unicode_string_addr      = %p\n", reinterpret_cast<PVOID>(rtl_compare_unicode_string_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ps_lookup_process_by_process_id_addr      = %p\n", reinterpret_cast<PVOID>(ps_lookup_process_by_process_id_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ob_dereference_object_addr      = %p\n", reinterpret_cast<PVOID>(ob_dereference_object_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ke_stack_attach_process_addr      = %p\n", reinterpret_cast<PVOID>(ke_stack_attach_process_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ke_unstack_detach_process_addr      = %p\n", reinterpret_cast<PVOID>(ke_unstack_detach_process_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ps_get_current_process_id_addr      = %p\n", reinterpret_cast<PVOID>(ps_get_current_process_id_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] zw_allocate_virtual_memory_addr      = %p\n", reinterpret_cast<PVOID>(zw_allocate_virtual_memory_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ps_get_process_section_base_address_addr      = %p\n", reinterpret_cast<PVOID>(ps_get_process_section_base_address_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] io_allocate_mdl_addr      = %p\n", reinterpret_cast<PVOID>(io_allocate_mdl_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] mm_probe_and_lock_pages_addr      = %p\n", reinterpret_cast<PVOID>(mm_probe_and_lock_pages_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] mm_unlock_pages_addr      = %p\n", reinterpret_cast<PVOID>(mm_unlock_pages_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] io_free_mdl_add      = %p\n", reinterpret_cast<PVOID>(io_free_mdl_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ke_get_current_irql_addr      = %p\n", reinterpret_cast<PVOID>(ke_get_current_irql_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ex_get_previous_mode_addr      = %p\n", reinterpret_cast<PVOID>(ex_get_previous_mode_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ps_get_current_process_addr      = %p\n", reinterpret_cast<PVOID>(ps_get_current_process_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ps_get_current_thread_addr      = %p\n", reinterpret_cast<PVOID>(ps_get_current_thread_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] ke_get_current_thread_addr      = %p\n", reinterpret_cast<PVOID>(ke_get_current_thread_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] mm_allocate_independent_pages_addr      = %p\n", reinterpret_cast<PVOID>(mm_allocate_independent_pages_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] mm_free_independent_pages_addr      = %p\n", reinterpret_cast<PVOID>(mm_free_independent_pages_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] zw_open_file_addr      = %p\n", reinterpret_cast<PVOID>(zw_open_file_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] zw_create_section_addr      = %p\n", reinterpret_cast<PVOID>(zw_create_section_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] zw_map_view_of_section_addr      = %p\n", reinterpret_cast<PVOID>(zw_map_view_of_section_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] zw_close_addr      = %p\n", reinterpret_cast<PVOID>(zw_close_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] zw_unmap_view_of_section_addr      = %p\n", reinterpret_cast<PVOID>(zw_unmap_view_of_section_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] nt_query_virtual_memory_addr      = %p\n", reinterpret_cast<PVOID>(nt_query_virtual_memory_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] nt_read_virtual_memory_addr       = %p\n", reinterpret_cast<PVOID>(nt_read_virtual_memory_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] zw_query_information_process_addr       = %p\n", reinterpret_cast<PVOID>(zw_query_information_process_addr));
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] zw_free_virtual_memory_addr       = %p\n", reinterpret_cast<PVOID>(zw_free_virtual_memory_addr));

			//zw_free_virtual_memory_addr
			//zw_query_information_process_addr
			//nt_read_virtual_memory_addr 
			//nt_query_virtual_memory_addr

			//zw_unmap_view_of_section_addr
			//zw_close_addr
			//zw_map_view_of_section_addr
			//zw_create_section_addr
			//zw_open_file_addr
			//ps_get_current_thread_addr
			//ps_get_current_process_addr
			//ex_get_previous_mode_addr
			 //mm_probe_and_lock_pages_addr
			

		 
		 

			 
		 
 

			INIT_FUNC_PTR(pfn_ki_preprocess_fault, ki_preprocess_fault_addr);
			INIT_FUNC_PTR(pfn_psp_exit_process, psp_exit_process_addr);
			INIT_FUNC_PTR(pfn_mm_is_address_valid_ex, mm_is_address_valid_ex_addr);
			INIT_FUNC_PTR(pfn_exp_lookup_handle_table_entry, exp_lookup_handle_table_entry_addr);
			INIT_FUNC_PTR(pfn_mm_create_kernel_stack , mm_create_kernel_stack_addr);
			INIT_FUNC_PTR(pfn_mm_delete_kernel_stack, mm_delete_kernel_stack_addr);
			INIT_FUNC_PTR(pfn_mm_allocate_independent_pages, mm_allocate_independent_pages_addr);
			INIT_FUNC_PTR(pfn_mm_free_independent_pages, mm_free_independent_pages_addr);
			INIT_FUNC_PTR(pfn_nt_query_virtual_memory, nt_query_virtual_memory_addr);
			INIT_FUNC_PTR(pfn_nt_read_virtual_memory, nt_read_virtual_memory_addr);

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
			if (!ps_get_process_exit_time_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_process_exit_time_addr is null.\n");
			if (!se_locate_process_image_name_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] se_locate_process_image_name_addr is null.\n");
			if (!ps_get_process_dxgprocess_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_process_dxgprocess_addr is null.\n");
			if (!ps_get_process_wow64_process_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_process_wow64_process_addr is null.\n");
			if (!ps_get_process_peb_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_process_peb_addr is null.\n");
			if (!mm_copy_virtual_memory_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_copy_virtual_memory_addr is null.\n");
			if (!rtl_equal_unicode_string_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] rtl_equal_unicode_string_addr is null.\n");
			if (!rtl_compare_unicode_string_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] rtl_compare_unicode_string_addr is null.\n");
			if(!ps_lookup_process_by_process_id_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_lookup_process_by_process_id_addr is null.\n");
			if(!ob_dereference_object_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ob_dereference_object_addr is null.\n");
			if (!ke_stack_attach_process_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_stack_attach_process_addr is null.\n");
			if (!ke_unstack_detach_process_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_unstack_detach_process_addr is null.\n");
			if (!ps_get_current_process_id_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_current_process_id_addr is null.\n");
			if(!zw_allocate_virtual_memory_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] zw_allocate_virtual_memory_addr is null.\n");
			if (!ps_get_process_section_base_address_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_process_section_base_address_addr is null.\n");
			if (!io_allocate_mdl_addr)
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] io_allocate_mdl_addr is null.\n");
			 if(!mm_probe_and_lock_pages_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_probe_and_lock_pages_addr is null.\n");
			 if (!mm_unlock_pages_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_unlock_pages_addr is null.\n");
			 if (!io_free_mdl_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] io_free_mdl_addr is null.\n");
			 if (!ke_get_current_irql_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_get_current_irql_addr is null.\n");
			 if (!ex_get_previous_mode_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ex_get_previous_mode_addr is null.\n");
			 if (!ps_get_current_process_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_current_process_addr is null.\n");
			 if (!ps_get_current_thread_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ps_get_current_thread_addr is null.\n");
			 if (!ke_get_current_thread_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] ke_get_current_thread_addr is null.\n");
			 if (!mm_allocate_independent_pages_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_allocate_independent_pages_addr is null.\n");
			 if (!mm_free_independent_pages_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] mm_free_independent_pages_addr is null.\n");
			 if (!zw_open_file_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] zw_open_file_addr is null.\n");
			 if(!zw_create_section_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] zw_create_section_addr is null.\n");
			 if(!zw_map_view_of_section_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] zw_map_view_of_section_addr is null.\n");
			 if (!zw_close_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] zw_close_addr is null.\n");
			 if (!zw_unmap_view_of_section_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] zw_unmap_view_of_section_addr is null.\n");
			 if(!nt_query_virtual_memory_addr)
				 DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] nt_query_virtual_memory_add is null.\n");
			  if (!nt_read_virtual_memory_addr)
				  DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] nt_read_virtual_memory_addr is null.\n");
			  if(!zw_query_information_process_addr)
				  DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] zw_query_information_process_addr is null.\n");
			  if (!zw_free_virtual_memory_addr)
				  DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[hv] zw_free_virtual_memory_addr is null.\n");

				  //nt_read_virtual_memory_addr 
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
				!mm_delete_kernel_stack_addr||
				!ps_get_process_exit_time_addr||
				!se_locate_process_image_name_addr||
				!ps_get_process_dxgprocess_addr||
				!ps_get_process_wow64_process_addr||
				!ps_get_process_peb_addr||
				!mm_copy_virtual_memory_addr||
				!rtl_equal_unicode_string_addr||
				!rtl_compare_unicode_string_addr||
				!ps_lookup_process_by_process_id_addr||
				!ob_dereference_object_addr||
				!ke_stack_attach_process_addr||
				!ke_unstack_detach_process_addr||
				!ps_get_current_process_id_addr||
				!zw_allocate_virtual_memory_addr||
				!ps_get_process_section_base_address_addr||
				!io_allocate_mdl_addr||
				!mm_probe_and_lock_pages_addr||
				!mm_unlock_pages_addr||
				!io_free_mdl_addr||
				!ke_get_current_irql_addr||
				!ex_get_previous_mode_addr||
				!ps_get_current_process_addr||
				!ps_get_current_thread_addr||
				!ke_get_current_thread_addr||
				!mm_allocate_independent_pages_addr||
				!mm_free_independent_pages_addr||
				!zw_open_file_addr||
				!zw_create_section_addr||
				!zw_map_view_of_section_addr||
				!zw_close_addr||
				!zw_unmap_view_of_section_addr||
				!nt_query_virtual_memory_addr||
				!nt_read_virtual_memory_addr||
				!zw_query_information_process_addr||
				!zw_free_virtual_memory_addr)
			{
				return STATUS_UNSUCCESSFUL;
			}

			 

		 


			return STATUS_SUCCESS;
		}

	}
}
