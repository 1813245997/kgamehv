#pragma once
namespace utils
{
	namespace call_back_utils
	{
		unsigned  long long get_create_process_callback_address_by_index(int index);
		NTSTATUS get_create_process_callback_by_index(int index, void** callback_address);
		PVOID find_psp_create_process_notify_routine();
	}
}
