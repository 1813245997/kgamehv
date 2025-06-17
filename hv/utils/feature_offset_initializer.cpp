#include "global_defs.h"
#include "feature_offset_initializer.h"

 
namespace utils
{
	namespace feature_offset
	{
		unsigned long long g_process_exit_time_offset;

	    unsigned long long g_dxgprocess_offset;

		unsigned long long g_process_wow64_process_offset;

		unsigned long long g_process_peb_offset;

		unsigned long long g_previous_modeoffset;

		NTSTATUS initialize()
		{
			 
			 
			g_process_exit_time_offset = scanner_offset::find_process_exit_time_offset();
			g_dxgprocess_offset = scanner_offset::find_dxgprocess_offset();
			g_process_wow64_process_offset = scanner_offset::find_process_wow64_process_offset();
			g_process_peb_offset = scanner_offset::find_process_peb_offset();
			 
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] process_exit_time_offset       = 0x%llx\n", g_process_exit_time_offset);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] dxgprocess_offset              = 0x%llx\n", g_dxgprocess_offset);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] process_wow64_process_offset  = 0x%llx\n", g_process_wow64_process_offset);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "[hv] process_peb_offset            = 0x%llx\n", g_process_peb_offset);

		 
			if (!g_process_exit_time_offset ||
				!g_dxgprocess_offset ||
				!g_process_wow64_process_offset ||
				!g_process_peb_offset)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to initialize one or more required process offsets\n");
				return STATUS_UNSUCCESSFUL;
			}

			return STATUS_SUCCESS;
		}
	}
}