#include "global_defs.h"
#include "ark_utils.h"


namespace utils
{
	namespace ark_utils
	{

		NTSTATUS get_user_module_offset_string(
		 
			_In_ unsigned long long  address,
			_Out_ PUNICODE_STRING out_module_offset_info
		)
		{
								
			PEPROCESS process = internal_functions::pfn_ps_get_current_process();

			if (!process)
			{
				return STATUS_INVALID_PARAMETER;
			}


			if (address == 0)
			{
				return STATUS_INVALID_PARAMETER;  
			}

			if (!internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<PVOID>(address)))
			{
				return STATUS_ACCESS_VIOLATION;  
			}

									
			return STATUS_SUCCESS;
		}

	}
}