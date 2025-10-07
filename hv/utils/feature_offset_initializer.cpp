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

		unsigned long long  g_start_address_offset;

		unsigned long long g_win32_start_address_offset;

		unsigned long long g_eprocess_unique_process_id_offset;

		unsigned long long g_eprocess_image_file_name_offset;


		NTSTATUS initialize()
		{
			 
			 
			g_process_exit_time_offset = scanner_offset::find_process_exit_time_offset();
			g_dxgprocess_offset = scanner_offset::find_dxgprocess_offset();
			g_process_wow64_process_offset = scanner_offset::find_process_wow64_process_offset();
			g_process_peb_offset = scanner_offset::find_process_peb_offset();
			g_start_address_offset = scanner_offset::find_start_address_offset();
			g_win32_start_address_offset = scanner_offset::find_win32_start_address_offset();
			g_eprocess_unique_process_id_offset = scanner_offset::find_eprocess_unique_process_id_offset();
			g_eprocess_image_file_name_offset = scanner_offset::find_eprocess_image_file_name_offset();
			LogInfo("process_exit_time_offset       = 0x%llx\n", g_process_exit_time_offset);
			LogInfo("dxgprocess_offset              = 0x%llx\n", g_dxgprocess_offset);
			LogInfo("process_wow64_process_offset  = 0x%llx\n", g_process_wow64_process_offset);
			LogInfo("process_peb_offset            = 0x%llx\n", g_process_peb_offset);
			LogInfo("start_address_offset          = 0x%llx\n", g_start_address_offset);
			LogInfo("win32_start_address_offset    = 0x%llx\n", g_win32_start_address_offset);
			LogInfo("eprocess_unique_process_id_offset = 0x%llx\n", g_eprocess_unique_process_id_offset);
			LogInfo("eprocess_image_file_name_offset = 0x%llx\n", g_eprocess_image_file_name_offset);

			if (!g_process_exit_time_offset ||
				!g_dxgprocess_offset ||
				!g_process_wow64_process_offset ||
				!g_process_peb_offset||
				!g_start_address_offset||
				!g_win32_start_address_offset||
				!g_eprocess_unique_process_id_offset||
				!g_eprocess_image_file_name_offset)
			{
				LogError(
					"Failed to initialize one or more required process offsets\n");
				return STATUS_UNSUCCESSFUL;
			}

			return STATUS_SUCCESS;
		}
	}
}