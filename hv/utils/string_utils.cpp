#include "global_defs.h"
#include "string_utils.h"

namespace utils
{
	namespace string_utils
	{
		BOOLEAN contains_substring(PSTRING main_str, PSTRING search_str, BOOLEAN case_insensitive)
		{
			 
			if (main_str == nullptr) {
				return FALSE;
			}

			if (search_str == nullptr) {
				return FALSE;
			}

			 
			if (main_str->Length < search_str->Length) {
				return FALSE;
			}

			USHORT length_diff = main_str->Length - search_str->Length;

			STRING slice;
			slice.Length = search_str->Length;
			slice.MaximumLength = search_str->Length;

			for (USHORT i = 0; i <= length_diff; ++i) {
				slice.Buffer = main_str->Buffer + i;

				if (RtlEqualString(&slice, search_str, case_insensitive)) {
					return TRUE;
				}
			}

			return FALSE;
		}
	}
}