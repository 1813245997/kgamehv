#pragma once
namespace utils
{
	namespace thread_utils
	{
		unsigned long long get_user_stack_ptr();

		void sleep(ULONG seconds);

		void sleep_ms(ULONG64 milliseconds);

		PETHREAD nt_get_process_main_thread(PEPROCESS process);

		KPROCESSOR_MODE ke_set_previous_mode(KPROCESSOR_MODE mode);

		NTSTATUS my_create_thread(PEPROCESS process, PVOID shellcode, PVOID context, PETHREAD* thread);
	}
}