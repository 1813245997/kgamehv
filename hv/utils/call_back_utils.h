#pragma once
namespace utils
{
	namespace call_back_utils
	{
		unsigned  long long get_create_process_callback_address_by_index(int index);

		unsigned long long get_load_image_callback_address_by_index(unsigned int index);

		NTSTATUS get_create_process_callback_by_index(int index, void** callback_address);

		NTSTATUS get_load_image_callback_by_index(unsigned int index, PVOID* callback_address);

		PVOID find_psp_create_process_notify_routine();
		  
		PVOID find_psp_load_image_notify_routine();

		
	}
}
