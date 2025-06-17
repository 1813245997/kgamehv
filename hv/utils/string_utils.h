#pragma once
namespace utils
{
	namespace string_utils
	{
		BOOLEAN contains_substring(PSTRING main_str, PSTRING search_str, BOOLEAN case_insensitive);
		BOOLEAN compare_unicode_strings(_In_ PUNICODE_STRING str1, _In_ PUNICODE_STRING str2, _In_ BOOLEAN case_insensitive);
		size_t get_wide_string_length(_In_ PCWCHAR str);
		void utf8_to_unicode(LPCSTR utf8, LPWSTR uni, SIZE_T max_count);

	}
}