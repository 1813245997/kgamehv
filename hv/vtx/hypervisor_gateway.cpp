#pragma warning( disable : 4201)
#include "../utils/global_defs.h"
#include "hypervisor_gateway.h"
   
 

 

#define IOCTL_POOL_MANAGER_ALLOCATE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

namespace hvgt
{


 

	bool hook_function(_In_ unsigned __int64 current_cr3 , _In_ unsigned __int64 pfn_of_hooked_page, _In_ unsigned __int64 pfn_of_fake_page_contents)
	{
		HookFunctionArgs args{ current_cr3, pfn_of_hooked_page, pfn_of_fake_page_contents,  0 };
		KeGenericCallDpc(broadcast_hook_function, &args);

		return static_cast<ULONG>(args.statuses) == KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	}

	bool remove_hook(_In_ unsigned __int64 current_cr3, _In_ unsigned __int64 pfn_of_hooked_page)
	{
		UnHookFunctionArgs args{ current_cr3, pfn_of_hooked_page ,0 };
		KeGenericCallDpc(broadcast_unhook_function, &args);

		return static_cast<ULONG>(args.statuses) == KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	}

	 

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