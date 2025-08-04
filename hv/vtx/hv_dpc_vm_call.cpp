#include "../utils/global_defs.h"
#include "hv_dpc_vm_call.h"
#include "vmcall_reason.h"
#include "../asm/vm_intrin.h"

namespace hvgt
{
	void broadcast_vmoff(KDPC*, PVOID, PVOID SystemArgument1, PVOID SystemArgument2)
	{
		__vm_call(VMCALL_VMXOFF, 0, 0, 0);
		KeSignalCallDpcSynchronize(SystemArgument2);
		KeSignalCallDpcDone(SystemArgument1);
	}





	void broadcast_hook_function(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
	{
		const auto args = reinterpret_cast<HookFunctionArgs*>(DeferredContext);

		if (__vm_call_ex(VMCALL_EPT_HOOK_FUNCTION,
			args->current_cr3,
			 args->pfn_of_hooked_page ,
			 args->pfn_of_fake_page_contents ,
			0,
			0,
			0,
			0,
			0,
			0))
		{
			InterlockedIncrement16(&args->statuses);
		}

		KeSignalCallDpcSynchronize(SystemArgument2);
		KeSignalCallDpcDone(SystemArgument1);
	}



	void broadcast_unhook_function(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
	{
		const auto args = reinterpret_cast<UnHookFunctionArgs*>(DeferredContext);

		if (__vm_call(VMCALL_EPT_UNHOOK_FUNCTION,
			args->current_cr3,
			args->pfn_of_hooked_page,
			0
			
		))
		{
			InterlockedIncrement16(&args->statuses);
		}

		KeSignalCallDpcSynchronize(SystemArgument2);
		KeSignalCallDpcDone(SystemArgument1);
	}

	void broadcast_break_point_int3(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
	{
		const auto args = reinterpret_cast<SetBreakpointArgs*>(DeferredContext);

		// 调用 VMCALL 设置 INT3 断点
		if (__vm_call_ex(
			VMCALL_EPT_SET_BREAK_POINT,              // 虚拟机调用号（假设这个用于设置断点）
			args->current_cr3,                   // 目标 CR3（页表）
			reinterpret_cast<unsigned __int64>(args->breakpoint_address), // 目标地址
			reinterpret_cast<unsigned __int64>(args->breakpoint_handler), // 命中断点后调用的处理函数
			reinterpret_cast<unsigned __int64>(args->original_byte),
			0,
			0,
			0,
			0,
			0

		))
		{
			// 设置成功，标记状态
			InterlockedIncrement16(&args->statuses);
		}

		// 同步 DPC 完成
		KeSignalCallDpcSynchronize(SystemArgument2);
		KeSignalCallDpcDone(SystemArgument1);
	}



	void broadcast_remove_break_point_int3(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
	{
		const auto args = reinterpret_cast<RemoveBreakpointArgs*>(DeferredContext);

		// 调用 VMCALL 卸载 INT3 断点
		if (__vm_call(
			VMCALL_EPT_REMOVE_BREAK_POINT,               // 虚拟机调用号（假设这是用于卸载断点）
			args->current_cr3,                           // 目标页表
			static_cast<unsigned __int64>(args->remove_all_breakpoints), // true/false 标志
			reinterpret_cast<unsigned __int64>(args->breakpoint_address) // 断点地址（或忽略）
		))
		{
			InterlockedIncrement16(&args->statuses);     // 设置成功
		}

		KeSignalCallDpcSynchronize(SystemArgument2);     // 同步信号
		KeSignalCallDpcDone(SystemArgument1);            // 通知 DPC 完成
	}


	void broadcast_test_vmcall(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
	{
		const auto statuses = reinterpret_cast<volatile SHORT*>(DeferredContext);

		if (__vm_call(VMCALL_TEST, 0, 0, 0))
		{
			InterlockedIncrement16(statuses);
		}

		KeSignalCallDpcSynchronize(SystemArgument2);
		KeSignalCallDpcDone(SystemArgument1);
	}
 
	void vmoff()
	{
		KeGenericCallDpc(broadcast_vmoff, NULL);
	}

	void broadcast_user_exception_int3(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
	{
		const auto args = reinterpret_cast<SetExceptionBreakpointArgs*>(DeferredContext);

		// 调用 VMCALL 设置 INT3 断点
		if (__vm_call_ex(
			VMCALL_EPT_SET_BREAK_POINT_EXCEPTION,              // 虚拟机调用号（假设这个用于设置断点）
			reinterpret_cast<unsigned __int64>(args->process_id),
			args->current_cr3,                   // 目标 CR3（页表）
			reinterpret_cast<unsigned __int64>(args->breakpoint_address), // 目标地址
			reinterpret_cast<unsigned __int64>(args->breakpoint_handler), // 命中断点后调用的处理函数
			reinterpret_cast<unsigned __int64>(args->trampoline_va),
			0,
			0,
			0,
			0 

		))
		{
			// 设置成功，标记状态
			InterlockedIncrement16(&args->statuses);
		}

		// 同步 DPC 完成
		KeSignalCallDpcSynchronize(SystemArgument2);
		KeSignalCallDpcDone(SystemArgument1);

	}

	void broadcast_remove_user_exception_int3(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
	{
		const auto args = reinterpret_cast<RemoveExceptionBreakpointArgs*>(DeferredContext);

		// 调用 VMCALL 卸载 INT3 断点
		if (__vm_call_ex(
			VMCALL_EPT_REMOVE_BREAK_POINT_EXCEPTION,      // 虚拟机调用号（假设这是用于卸载断点）
		   reinterpret_cast<unsigned long long>	(args->process_id),
			args->current_cr3,                           // 目标页表
			reinterpret_cast<unsigned __int64>(args->breakpoint_address), // 断点地址（或忽略）
			static_cast<unsigned __int64>(args->remove_all_breakpoints), // true/false 标志,
			0,
			0,
			0,
			0,
			0
		))
		{
			InterlockedIncrement16(&args->statuses);     // 设置成功
		}

		KeSignalCallDpcSynchronize(SystemArgument2);     // 同步信号
		KeSignalCallDpcDone(SystemArgument1);            // 通知 DPC 完成
	}
 

 
}