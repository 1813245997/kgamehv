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

		bool get_file_name_from_full_path(_In_ PUNICODE_STRING full_path, _Out_ PUNICODE_STRING* file_name)
		{
			if (!full_path || !full_path->Buffer || full_path->Length == 0)
			{
				return false;
			}
			if (!file_name)
			{
				return false;
			}

			// 查找最后一个反斜杠位置
			USHORT length = full_path->Length / sizeof(WCHAR);
			WCHAR* buffer = full_path->Buffer;
			USHORT last_backslash_index = 0;

			for (USHORT i = 0; i < length; ++i)
			{
				if (buffer[i] == L'\\')
				{
					last_backslash_index = i + 1;
				}
			}

			USHORT name_length = (length - last_backslash_index) * sizeof(WCHAR);
			if (name_length == 0)
			{
				// 如果没有反斜杠或最后一个反斜杠后面没内容，直接返回失败
				return false;
			}

			// 分配缓冲区
			UNICODE_STRING name_only{};
			name_only.Length = name_length;
			name_only.MaximumLength = static_cast<USHORT>(name_length + sizeof(WCHAR));
			name_only.Buffer = static_cast<PWSTR>(
				internal_functions::pfn_ex_allocate_pool_with_tag(PagedPool, name_only.MaximumLength, 'prcN'));
			if (!name_only.Buffer)
			{
				return false;
			}

			// 复制字符串
			RtlCopyMemory(name_only.Buffer, &buffer[last_backslash_index], name_length);
			// 添加结尾空字符
			name_only.Buffer[name_only.Length / sizeof(WCHAR)] = L'\0';

			// 分配UNICODE_STRING结构体
			*file_name = static_cast<PUNICODE_STRING>(
				internal_functions::pfn_ex_allocate_pool_with_tag(PagedPool, sizeof(UNICODE_STRING), 'prcS'));
			if (!*file_name)
			{
				internal_functions::pfn_ex_free_pool_with_tag(name_only.Buffer, 0);
				return false;
			}

			**file_name = name_only;

			return true;
		}
	}
}