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
		 
		void utf8_to_unicode(LPCSTR utf8, LPWSTR uni, SIZE_T max_count)
		{
			unsigned char v;
			int unidx = 0;
			for (int i = 0; v = utf8[i]; i++) {
				if ((v & 0b10000000) == 0) {
					uni[unidx] = v;
				}
				else if ((v & 0b11100000) == 0b11000000) {
					unsigned short c = (unsigned short)((v & 0b00011111)) << 6;
					v = utf8[++i];
					if ((v & 0b11000000) == 0b10000000) {
						c |= (v & 0b00111111);
						uni[unidx] = c;
					}
					else
						continue;
				}
				else if ((v & 0b11110000) == 0b11100000) {
					unsigned short c = (unsigned short)((v & 0b00001111)) << 12;
					v = utf8[++i];
					if ((v & 0b11000000) == 0b10000000) {
						c |= (unsigned short)((v & 0b00111111)) << 6;
						v = utf8[++i];
						if ((v & 0b11000000) == 0b10000000) {
							c |= (unsigned short)((v & 0b00111111));
							uni[unidx] = c;
						}
						else
							continue;
					}
					else
						continue;
				}
				else
					continue;
				unidx++;
				if (unidx >= max_count)
					break;
			}
			uni[unidx] = 0;
			return;
		}


	}
}