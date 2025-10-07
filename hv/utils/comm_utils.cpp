#include "global_defs.h"
#include "comm_utils.h"
namespace utils
{
	namespace comm
	{
		 
		NTSTATUS get_driver_object(const wchar_t* driver_name, PDRIVER_OBJECT* driver_object)
		{
			UNICODE_STRING uni_name;
			utils::internal_functions::pfn_rtl_init_unicode_string(&uni_name, driver_name);

			return ObReferenceObjectByName(
				&uni_name,
				OBJ_CASE_INSENSITIVE,
				NULL,
				0,
				*IoDriverObjectType,
				KernelMode,
				NULL,
				reinterpret_cast<PVOID*>(driver_object)
			);
		}
	 }
}