#include "global_defs.h"
#include "thread_utils.h"
#include "../ia32/ia32.h"
namespace utils
{
	namespace thread_utils
	{
		unsigned long long get_user_stack_ptr()
		{
			auto current_thread = reinterpret_cast<unsigned long long> (internal_functions::pfn_ke_get_current_thread());
			auto trap_frame = *(KTRAP_FRAME**)(current_thread + 0x90);
			return *(unsigned long long*)&trap_frame->Rsp;
		}
		void sleep(ULONG seconds)
		{
			LARGE_INTEGER larTime = {};
			larTime.QuadPart = -10 * 1000 * 1000;
			larTime.QuadPart *= seconds;
			KeDelayExecutionThread(KernelMode, FALSE, &larTime);
		}

		void sleep_ms(ULONG64 milliseconds)
		{
		 

			LARGE_INTEGER larTime;
			larTime.QuadPart = milliseconds * -10000;
			KeDelayExecutionThread(KernelMode, FALSE, &larTime);
		}

		PETHREAD nt_get_process_main_thread(PEPROCESS process)
		{


			PETHREAD ethread = NULL;

			KAPC_STATE apc_state = { 0 };

			utils::internal_functions::pfn_ke_stack_attach_process(process, &apc_state);

			HANDLE thread_handle = NULL;

			NTSTATUS status = utils::internal_functions::pfn_nt_get_next_thread(
				NtCurrentProcess(),
				NULL,
				THREAD_ALL_ACCESS,
				OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
				0,
				&thread_handle
			);

			if (NT_SUCCESS(status))
			{
				status = ObReferenceObjectByHandle(
					thread_handle,
					THREAD_ALL_ACCESS,
					*PsThreadType,
					KernelMode,
					(PVOID*)&ethread,
					NULL
				);

				NtClose(thread_handle);

				if (!NT_SUCCESS(status))
				{
					ethread = NULL;
				}
			}

			utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			return ethread;
		}


		KPROCESSOR_MODE ke_set_previous_mode(KPROCESSOR_MODE mode)
		{
			KPROCESSOR_MODE old = ExGetPreviousMode();
			*(KPROCESSOR_MODE*)((PBYTE)KeGetCurrentThread() + utils::scanner_offset::find_thread_previous_mode_offset()) = mode;
			return old;
		}


		NTSTATUS my_create_thread(
			PEPROCESS process,
			PVOID shellcode,
			PVOID context,
			PETHREAD* thread
		) {
		 
			KAPC_STATE kApcState = { 0 };
			utils::internal_functions::pfn_ke_stack_attach_process(process, &kApcState);

			 
			MODE previous_mode = (MODE)ke_set_previous_mode(KernelMode);

			HANDLE hThread = NULL;
			NTSTATUS status = STATUS_UNSUCCESSFUL;

			 
			status = utils::internal_functions::pfn_nt_create_thread_ex(
				&hThread,                
				THREAD_ALL_ACCESS,       
				NULL,                    
				NtCurrentProcess(),      
				shellcode,               
				context,                 
				0,                       
				0,                      
				0,                       
				0,                      
				NULL                    
			);

			if (NT_SUCCESS(status) && thread) {
				 
				status = ObReferenceObjectByHandle(
					hThread,
					THREAD_ALL_ACCESS,
					*PsThreadType,
					KernelMode,
					(PVOID*)thread,
					NULL
				);
			}

		 
			if (hThread) {
				NtClose(hThread);
			}

			 
			ke_set_previous_mode(previous_mode);
			utils::internal_functions::pfn_ke_unstack_detach_process(&kApcState);

			return status;
		}

		TASK_STATE_SEGMENT_64* get_task_state_segment(_In_ UINT64 Kpcr)
		{
			return *(TASK_STATE_SEGMENT_64**)(Kpcr + KPCR_TSS_BASE_OFFSET);
		}

		PMACHINE_FRAME get_isr_machine_frame(_In_ TASK_STATE_SEGMENT_64* task_state_segment)
		{
			return reinterpret_cast<PMACHINE_FRAME>(task_state_segment->Ist3 - sizeof(MACHINE_FRAME));
		}

	}
}