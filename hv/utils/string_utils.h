#pragma once
namespace utils
{
	namespace string_utils
	{
		BOOLEAN contains_substring(PSTRING main_str, PSTRING search_str, BOOLEAN case_insensitive);

		BOOLEAN contains_unicode_substring(PUNICODE_STRING main_str, PUNICODE_STRING search_str, BOOLEAN case_insensitive);

		BOOLEAN contains_substring_wchar(PWCHAR main_str, PWCHAR search_str, BOOLEAN case_insensitive);

		BOOLEAN compare_unicode_strings(_In_ PUNICODE_STRING str1, _In_ PUNICODE_STRING str2, _In_ BOOLEAN case_insensitive);

		size_t get_wide_string_length(_In_ PCWCHAR str);

		size_t strlen(const char* str);

		void utf8_to_unicode(LPCSTR utf8, LPWSTR uni, SIZE_T max_count);

		bool int_to_string(int value, char* buffer, size_t buffer_size);

		char* int_to_string_no_alloc(int value, char* buffer, size_t buffer_size);

		char* concat_strings_no_alloc(char* str1, const char* str2, size_t buffer_size);

		char* int_to_string_alloc(int value);

		char* concat_strings_alloc(const char* str1, const char* str2);

		PUNICODE_STRING create_unicode_string_from_wchar(_In_ PCWCHAR src);

		void free_unicode_string(_In_opt_ PUNICODE_STRING uni_str);
	
	}
}