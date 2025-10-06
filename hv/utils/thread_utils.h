#pragma once
#include "../ia32/ia32.h"
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

		TASK_STATE_SEGMENT_64* get_task_state_segment(_In_ UINT64 Kpcr);

		PMACHINE_FRAME get_isr_machine_frame(_In_ TASK_STATE_SEGMENT_64* task_state_segment);


	}
}