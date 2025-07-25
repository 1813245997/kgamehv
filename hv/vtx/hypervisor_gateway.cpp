#pragma warning( disable : 4201)
#include "../utils/global_defs.h"

#include <ntddk.h>
#include <intrin.h>
 
#include "../asm\vm_intrin.h"
#include "vmcall_reason.h"
 

#define IOCTL_POOL_MANAGER_ALLOCATE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

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
			args->current_cr3 ,
			reinterpret_cast<unsigned long long> (args->target_address),
			reinterpret_cast<unsigned long long> (args->hook_function),
			reinterpret_cast<unsigned long long> (args->origin_function), 
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
			args->unhook_all_functions,
			(unsigned __int64)args->function_to_unhook
			))
		{
			InterlockedIncrement16(&args->statuses);
		}

		KeSignalCallDpcSynchronize(SystemArgument2);
		KeSignalCallDpcDone(SystemArgument1);
	}

	void broadcast_break_point_in3(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
	{
		const auto args = reinterpret_cast<SetBreakpointArgs*>(DeferredContext);

		// 调用 VMCALL 设置 INT3 断点
		if (__vm_call_ex(
			VMCALL_EPT_SET_BREAK_POINT,              // 虚拟机调用号（假设这个用于设置断点）
			args->current_cr3,                   // 目标 CR3（页表）
			reinterpret_cast<unsigned __int64>(args->breakpoint_address), // 目标地址
			reinterpret_cast<unsigned __int64>(args->breakpoint_handler) , // 命中断点后调用的处理函数
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

	void broadcast_remove_break_point_in3(KDPC*, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
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

	/// <summary>
	/// Turn off virtual machine
	/// </summary>
	void vmoff()
	{
		KeGenericCallDpc(broadcast_vmoff, NULL);
	}

	/// <summary>
	/// Set/Unset presence of hypervisor
	/// </summary>
	/// <param name="value"> If false, hypervisor is not visible via cpuid interface, If true, it become visible</param>
	void hypervisor_visible(bool value)
	{
		if (value == true)
			__vm_call(VMCALL_UNHIDE_HV_PRESENCE, 0, 0, 0);
		else
			__vm_call(VMCALL_HIDE_HV_PRESENCE, 0, 0, 0);
	}

	/// <summary>
	/// Unhook all functions and invalidate tlb
	/// </summary>
	/// <returns> status </returns>
	bool unhook_all_functions()
	{
		UnHookFunctionArgs args{ __readcr3(),EptUnhookType::UNHOOK_ALL, nullptr, 0 };
		KeGenericCallDpc(broadcast_unhook_function, &args);

		return static_cast<ULONG>(args.statuses) == KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	}

	/// <summary>
	/// Unhook single function and invalidate tlb
	/// </summary>
	/// <param name="function_address"></param>
	/// <returns> status </returns>
	bool unhook_function(void* function_address)
	{
		UnHookFunctionArgs args{ __readcr3(),EptUnhookType::UNHOOK_SINGLE, function_address, 0 };
		KeGenericCallDpc(broadcast_unhook_function, &args);

		return static_cast<ULONG>(args.statuses) == KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	}

	/// <summary>
	/// Unhook functions by process PID and invalidate TLB
	/// </summary>
	/// <param name="process_id">The PID of the process whose hooks should be removed</param>
	/// <returns>status</returns>
	//bool unhook_by_pid(HANDLE process_id)
	//{
	//	UnHookFunctionArgs args{ EptUnhookType::UNHOOK_BY_PID, nullptr, __readcr3(), process_id,0 };
	//	KeGenericCallDpc(broadcast_unhook_function, &args);

	//	return static_cast<ULONG>(args.statuses) == KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	//}
	/// <summary>
	/// Hook function via ept and invalidates mappings
	/// </summary>
	/// <param name="target_address">Address of function which we want to hook</param>
	/// <param name="hook_function">Address of function which is used to call original function</param>
	/// <param name="origin_function">Address of function which is used to call original function</param>
	/// <returns> status </returns>
	bool hook_kernel_function(_In_ void* target_address, _In_ void* hook_function, _Inout_ void** origin_function)
	{
		HookFunctionArgs args{ __readcr3(), target_address, hook_function, origin_function, 0 };
		KeGenericCallDpc(broadcast_hook_function, &args);

		return static_cast<ULONG>(args.statuses) == KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	}
	 
	//bool 

	/// <summary>
	/// Dump info about allocated pools (Use Dbgview to see information)
	/// </summary>
	void dump_pool_manager()
	{
		__vm_call(VMCALL_DUMP_POOL_MANAGER, 0, 0, 0);
	}

	/// <summary>
	/// Check if we can communicate with hypervisor
	/// </summary>
	/// <returns> status </returns>
	bool test_vmcall()
	{
		volatile SHORT statuses{};
		KeGenericCallDpc(broadcast_test_vmcall, (PVOID)&statuses);

		return static_cast<ULONG>(statuses) == KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	}


}