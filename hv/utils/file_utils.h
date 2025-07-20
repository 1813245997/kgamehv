#pragma once
namespace utils
{
	namespace file_utils
	{
		NTSTATUS force_delete_file(PUNICODE_STRING full_file_path);
	}
}