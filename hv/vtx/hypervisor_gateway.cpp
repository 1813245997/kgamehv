#pragma warning( disable : 4201)
#include "../utils/global_defs.h"

#include <ntddk.h>
#include <intrin.h>
 
#include "../asm\vm_intrin.h"
#include "vmcall_reason.h"
#include "hv_dpc_vm_call.h"
 

#define IOCTL_POOL_MANAGER_ALLOCATE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

namespace hvgt
{


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

 
	bool hook_kernel_function(_In_ void* target_address, _In_ void* hook_function, _Inout_ void** origin_function)
	{
		HookFunctionArgs args{ __readcr3(), target_address, hook_function, origin_function, 0 };
		KeGenericCallDpc(broadcast_hook_function, &args);

		return static_cast<ULONG>(args.statuses) == KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	}

	bool hook_break_point_int3(_In_ unsigned long long target_cr3, _In_  void* target_function, _In_  void* breakpoint_handler, _Inout_  unsigned char* original_byte)
	{
		SetBreakpointArgs args{ target_cr3, target_function, breakpoint_handler,original_byte, 0 };
		KeGenericCallDpc(broadcast_break_point_int3, &args);

		return static_cast<ULONG>(args.statuses) == KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	}
	 
	bool hook_user_exception_int3(_In_ HANDLE  process_id,  _In_ unsigned long long target_cr3,  _In_  void* target_function, _In_  void* breakpoint_handler, _Inout_ unsigned char* trampoline_va)
	{
		SetExceptionBreakpointArgs args{ process_id, target_cr3, target_function, breakpoint_handler,trampoline_va, 0 };
		KeGenericCallDpc(broadcast_user_exception_int3, &args);

		return static_cast<ULONG>(args.statuses) == KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	}

	bool unhook_break_point_int3(
		unsigned long long target_cr3,        
		void* breakpoint_address,             
		bool remove_all                       
	)
	{
		RemoveBreakpointArgs args{	target_cr3,breakpoint_address,remove_all,	0
		};

	 
		KeGenericCallDpc(broadcast_remove_break_point_int3, &args);

	 
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

	void hypervisor_visible(bool value)
	{
		if (value == true)
			__vm_call(VMCALL_UNHIDE_HV_PRESENCE, 0, 0, 0);
		else
			__vm_call(VMCALL_HIDE_HV_PRESENCE, 0, 0, 0);
	}


 
}