#include "global_defs.h"
#include "hook_process_manager.h"


namespace utils
{
	namespace hook_process_manager
	{
		NTSTATUS(NTAPI* original_nt_create_section)(
			_Out_ PHANDLE section_handle,
			_In_ ACCESS_MASK desired_access,
			_In_opt_ POBJECT_ATTRIBUTES object_attributes,
			_In_opt_ PLARGE_INTEGER maximum_size,
			_In_ ULONG section_page_protection,
			_In_ ULONG allocation_attributes,
			_In_opt_ HANDLE file_handle
			);

		void(__fastcall* original_psp_exit_process)(
			IN BOOLEAN trim_address_space,
			IN PEPROCESS process
			);

		NTSTATUS initialize_hook_process_manager()
		{
			NTSTATUS status = STATUS_SUCCESS;

			if (!reinterpret_cast<void*>(utils::internal_functions::pfn_nt_create_section))
			{
				LogError("pfn_nt_create_section is not found");
				status = STATUS_NOT_FOUND;
				goto clear;
			}

			
			if (!utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(utils::internal_functions::pfn_nt_create_section),
				hook_nt_create_section,
				reinterpret_cast<void**>(&original_nt_create_section)
			))
			{
				LogError("Failed to hook pfn_nt_create_section");
				status = STATUS_UNSUCCESSFUL;
				goto clear;
			}
			LogInfo("Hook pfn_nt_create_section SUCCESS");

			if (!reinterpret_cast<void*>(utils::internal_functions::pfn_psp_exit_process))
			{
				LogError("pfn_psp_exit_process is not found");
				status = STATUS_NOT_FOUND;
				goto clear;
			}
			if (!utils::hook_utils::hook_kernel_function(
				reinterpret_cast<void*>(utils::internal_functions::pfn_psp_exit_process),
				hook_psp_exit_process,
				reinterpret_cast<void**>(&original_psp_exit_process)
			))
			{
				LogError("Failed to hook pfn_psp_exit_process");
				status = STATUS_UNSUCCESSFUL;
				goto clear;
			}
			LogInfo("Hook pfn_psp_exit_process SUCCESS");

		clear:
			return status;
		}


		NTSTATUS NTAPI hook_nt_create_section(
			_Out_ PHANDLE section_handle,
			_In_ ACCESS_MASK desired_access,
			_In_opt_ POBJECT_ATTRIBUTES object_attributes,
			_In_opt_ PLARGE_INTEGER maximum_size,
			_In_ ULONG section_page_protection,
			_In_ ULONG allocation_attributes,
			_In_opt_ HANDLE file_handle
		)
		{
			NTSTATUS status{};
			PFILE_OBJECT pfile_object{};
			PEPROCESS process{};
			POBJECT_NAME_INFORMATION ObjectNameInformation{};
			HANDLE process_id{};

			process = utils::internal_functions::pfn_ps_get_current_process();
			process_id = utils::internal_functions::pfn_ps_get_process_id(process);

			if (!file_handle)
			{
				return original_nt_create_section(
					section_handle,
					desired_access,
					object_attributes,
					maximum_size,
					section_page_protection,
					allocation_attributes,
					file_handle
				);
			}

			if (allocation_attributes != 0x1000000)
			{
				return original_nt_create_section(
					section_handle,
					desired_access,
					object_attributes,
					maximum_size,
					section_page_protection,
					allocation_attributes,
					file_handle
				);
			}

			if (section_page_protection != PAGE_EXECUTE)
			{
				return original_nt_create_section(
					section_handle,
					desired_access,
					object_attributes,
					maximum_size,
					section_page_protection,
					allocation_attributes,
					file_handle
				);
			}

			status = utils::internal_functions::pfn_ob_reference_object_by_handle(
				file_handle,
				0,
				0,
				KernelMode,
				(PVOID*)&pfile_object,
				NULL
			);

			if (!NT_SUCCESS(status))
			{
				return original_nt_create_section(
					section_handle,
					desired_access,
					object_attributes,
					maximum_size,
					section_page_protection,
					allocation_attributes,
					file_handle
				);
			}

			status = utils::internal_functions::pfn_io_query_file_dos_device_name(pfile_object, &ObjectNameInformation);
			if (!NT_SUCCESS(status))
			{
				utils::internal_functions::pfn_ob_dereference_object(pfile_object);
				return original_nt_create_section(
					section_handle,
					desired_access,
					object_attributes,
					maximum_size,
					section_page_protection,
					allocation_attributes,
					file_handle
				);
			}

			utils::internal_functions::pfn_ob_dereference_object(pfile_object);

			if (!ObjectNameInformation)
			{
				return original_nt_create_section(
					section_handle,
					desired_access,
					object_attributes,
					maximum_size,
					section_page_protection,
					allocation_attributes,
					file_handle
				);
			}

			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(ObjectNameInformation->Name.Buffer))
			{
				return original_nt_create_section(
					section_handle,
					desired_access,
					object_attributes,
					maximum_size,
					section_page_protection,
					allocation_attributes,
					file_handle
				);
			}

			// TODO: Add your logic here to check the file name and decide whether to block

			return original_nt_create_section(
				section_handle,
				desired_access,
				object_attributes,
				maximum_size,
				section_page_protection,
				allocation_attributes,
				file_handle
			);
		}



		void __fastcall hook_psp_exit_process(
			IN BOOLEAN trim_address_space,
			IN PEPROCESS process
		)
		{
			 
			if (trim_address_space && process)
			{
				// TODO: Add cleanup logic here
			}

			return original_psp_exit_process(trim_address_space, process);
		}
	}
}

