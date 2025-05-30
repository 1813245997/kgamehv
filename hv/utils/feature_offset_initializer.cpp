#include "global_defs.h"
#include "feature_offset_initializer.h"

 
namespace utils
{
	namespace feature_offset
	{
		unsigned long long g_process_exit_time_offset;

		NTSTATUS initialize()
		{
			g_process_exit_time_offset = scanner_offset::find_process_exit_time_offset();

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
				"[hv] process_exit_time_offset = 0x%llx\n", g_process_exit_time_offset);

			if (!g_process_exit_time_offset)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
					"[hv] Failed to locate process_exit_time_offset\n");
				return STATUS_UNSUCCESSFUL;
			}

			return STATUS_SUCCESS;
		}
	}
}