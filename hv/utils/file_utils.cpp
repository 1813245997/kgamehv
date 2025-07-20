#include "global_defs.h"
#include "file_utils.h"


namespace utils
{
	namespace file_utils
	{
		NTSTATUS force_delete_file(PUNICODE_STRING full_file_path)
		{
			NTSTATUS status = STATUS_UNSUCCESSFUL;
			HANDLE h_file = NULL;
			IO_STATUS_BLOCK io_status = { NULL };
			OBJECT_ATTRIBUTES file_attrib = { NULL };

			InitializeObjectAttributes(&file_attrib, full_file_path, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);

			status = IoCreateFileEx(&h_file, SYNCHRONIZE | DELETE, &file_attrib, &io_status, NULL, FILE_ATTRIBUTE_NORMAL,
				FILE_SHARE_DELETE, FILE_OPEN,
				FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0,
				CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING, NULL);

			if (!NT_SUCCESS(status)) {
				return status;
			}

			PFILE_OBJECT file_object;

			status = ObReferenceObjectByHandleWithTag(h_file, SYNCHRONIZE | DELETE, *IoFileObjectType, KernelMode, 'tlfD', (PVOID*)&file_object, NULL);

			if (!NT_SUCCESS(status)) {
				ObCloseHandle(h_file, KernelMode);
				return status;
			}

			file_object->SectionObjectPointer->ImageSectionObject = NULL;
			file_object->SectionObjectPointer->DataSectionObject = 0;
			MmFlushImageSection(file_object->SectionObjectPointer, MmFlushForDelete);

			status = ZwDeleteFile(&file_attrib);

			ObfDereferenceObject(file_object);
			ObCloseHandle(h_file, KernelMode);

			return status;
		}
	}
}