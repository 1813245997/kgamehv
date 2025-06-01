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
			 
				if ( RtlEqualString(&slice, search_str, case_insensitive)) {
					return TRUE;
				}
			}

			return FALSE;
		}

		BOOLEAN compare_unicode_strings(_In_ PUNICODE_STRING str1, _In_ PUNICODE_STRING str2, _In_ BOOLEAN case_insensitive)
		{
			if (!str1 || !str2 || !str1->Buffer || !str2->Buffer)
			{
				return FALSE;
			}

			return  internal_functions::pfn_rtl_equal_unicode_string(str1, str2, case_insensitive);
		}

		size_t get_wide_string_length(_In_ PCWCHAR str)
		{
			if (!str)
			{
				return 0;
			}

			size_t length = 0;
			while (str[length] != L'\0')
			{
				length++;
			}

			return length;
		}
	}
}