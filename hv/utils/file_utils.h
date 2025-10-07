#pragma once
namespace utils
{
	namespace file_utils
	{
		NTSTATUS force_delete_file(PUNICODE_STRING full_file_path);

		bool get_file_name_from_full_path(_In_ PUNICODE_STRING full_path, _Inout_ PUNICODE_STRING* file_name);
	}
}