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

		BOOLEAN contains_substring_wchar(PWCHAR main_str, PWCHAR search_str, BOOLEAN case_insensitive)
		{
			if (!main_str || !search_str)
				return FALSE;

			SIZE_T main_len = 0;
			while (main_str[main_len] != L'\0')
				main_len++;

			SIZE_T search_len = 0;
			while (search_str[search_len] != L'\0')
				search_len++;

			if (main_len < search_len || search_len == 0)
				return FALSE;

			UNICODE_STRING target;
			target.Buffer = search_str;
			target.Length = (USHORT)(search_len * sizeof(WCHAR));
			target.MaximumLength = target.Length;

			 
			for (SIZE_T i = 0; i <= main_len - search_len; ++i)
			{
				UNICODE_STRING slice;
				slice.Buffer = &main_str[i];
				slice.Length = (USHORT)(search_len * sizeof(WCHAR));
				slice.MaximumLength = slice.Length;
				if (internal_functions::pfn_rtl_equal_unicode_string(&slice, &target, case_insensitive))
				{
					return TRUE;
				}
			}

			return FALSE;
		}


		BOOLEAN contains_unicode_substring(PUNICODE_STRING main_str, PUNICODE_STRING search_str, BOOLEAN case_insensitive)
		{
			if (!main_str || !search_str || main_str->Length < search_str->Length)
			{
				return FALSE;

			}

			USHORT max_offset = main_str->Length - search_str->Length;

			UNICODE_STRING slice = {  };
			slice.Length = search_str->Length;
			slice.MaximumLength = search_str->Length;

			for (USHORT i = 0; i <= max_offset; i += sizeof(WCHAR))
			{
				slice.Buffer = (PWCH)((PUCHAR)main_str->Buffer + i);

				if (internal_functions::pfn_rtl_equal_unicode_string(&slice, search_str, case_insensitive))
				{
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
			for (int i = 0;  utf8[i] != '\0'; i++) {
				  v = utf8[i];
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

		size_t strlen(const char* str)
		{
			size_t len = 0;
			while (str && str[len] != '\0') ++len;
			return len;
		}

		char* int_to_string_no_alloc(int value, char* buffer, size_t buffer_size)
		{
			if (!buffer || buffer_size == 0)
				return nullptr;

			 
			char* p = buffer + buffer_size - 1;
			*p = '\0';

			bool is_negative = value < 0;
			unsigned int abs_value = is_negative ? -value : value;

			 
			do {
				if (p == buffer)  
					return nullptr;
				*--p = '0' + (abs_value % 10);
				abs_value /= 10;
			} while (abs_value);

			if (is_negative)
			{
				if (p == buffer)
					return nullptr;
				*--p = '-';
			}

		 
			return p;
		}

		bool int_to_string(int value, char* buffer, size_t buffer_size)
		{
			if (!buffer || buffer_size == 0)
				return false;

			 
			RtlZeroMemory(buffer, buffer_size);

			 
			char temp[32] = { 0 };
			char* p = temp + sizeof(temp) - 1;
			*p = '\0';

			bool is_negative = (value < 0);
			unsigned int abs_value = is_negative ? (unsigned int)(-value) : (unsigned int)value;

			do {
				*--p = '0' + (abs_value % 10);
				abs_value /= 10;
			} while (abs_value);

			if (is_negative)
				*--p = '-';

			size_t len = strlen(p);
			if (len + 1 > buffer_size)  
				return false;

		 
			RtlCopyMemory(buffer, p, len + 1);

			return true;
		}
		char* int_to_string_alloc(int value)
		{
			 
			const size_t buf_size = 32;
			char* buffer = reinterpret_cast<char*> (utils::memory::allocate_independent_pages(buf_size, PAGE_READWRITE));
			if (!buffer)
				return nullptr;

			char* p = buffer + buf_size - 1;
			*p = '\0';

			bool is_negative = value < 0;
			unsigned int abs_value = is_negative ? -value : value;

			do {
				*--p = '0' + (abs_value % 10);
				abs_value /= 10;
			} while (abs_value);

			if (is_negative)
				*--p = '-';

			 
			size_t len = buffer + buf_size - p - 1;
			RtlMoveMemory(buffer, p, len + 1);   

			return buffer;  
		}

		char* concat_strings_no_alloc(char* str1, const char* str2, size_t buffer_size)
		{
			if (!str1 || !str2 || buffer_size == 0)
				return nullptr;

			size_t len1 = strlen(str1);
			size_t len2 = strlen(str2);

			 
			if (len1 + len2 + 1 > buffer_size)
				return nullptr;

			 
			RtlCopyMemory(str1 + len1, str2, len2);

			 
			str1[len1 + len2] = '\0';

			return str1;
		}
		char* concat_strings_alloc(const char* str1, const char* str2)
		{
			if (!str1 && !str2)
				return nullptr;

			size_t len1 = strlen(str1);
			size_t len2 = strlen(str2);

			 
			char* result = (char*)utils::memory::allocate_independent_pages(
				 
				(len1 + len2 + 1) * sizeof(char),
				0);  

			if (!result)
				return nullptr;

			 
			if (str1)
				RtlCopyMemory(result, str1, len1);

			 
			if (str2)
				RtlCopyMemory(result + len1, str2, len2);

			 
			result[len1 + len2] = '\0';

			return result;
		}


		 
		PUNICODE_STRING create_unicode_string_from_wchar(_In_ PCWCHAR src)
		{
			if (!src)
				return nullptr;

			 
			size_t len = 0;
			while (src[len] != L'\0')
				len++;

			USHORT byte_len = (USHORT)(len * sizeof(WCHAR));

			 
			PUNICODE_STRING uni_str = reinterpret_cast<PUNICODE_STRING>(
				utils::memory::allocate_independent_pages(
				 
					sizeof(UNICODE_STRING),
					  PAGE_READWRITE
				));

			if (!uni_str)
				return nullptr;

			RtlZeroMemory(uni_str, sizeof(UNICODE_STRING));

			 
			uni_str->Buffer = reinterpret_cast<PWCH>(utils::memory::allocate_independent_pages(byte_len + sizeof(WCHAR), PAGE_READWRITE));

			if (!uni_str->Buffer)
			{
				utils::memory::free_independent_pages(uni_str, sizeof(UNICODE_STRING));
				return nullptr;
			}

			 
			RtlCopyMemory(uni_str->Buffer, src, byte_len);
			uni_str->Buffer[len] = L'\0';

			uni_str->Length = byte_len;
			uni_str->MaximumLength = (USHORT)(byte_len + sizeof(WCHAR));
			
			return uni_str;
		}

		 
		void free_unicode_string(_In_opt_ PUNICODE_STRING uni_str)
		{
			if (!uni_str)
				return;

			if (uni_str->Buffer)
			{
				utils::memory::free_independent_pages(uni_str->Buffer, uni_str->Length);
				uni_str->Buffer = nullptr;
			}

			utils::memory::free_independent_pages(uni_str, sizeof(UNICODE_STRING));
		}

		// ANSI to Unicode conversion functions
		bool ansi_to_unicode(_In_ PCHAR ansi_str, _Inout_ PWCHAR unicode_buffer, _In_ SIZE_T buffer_size)
		{
			if (!ansi_str || !unicode_buffer || buffer_size == 0)
			{
				return false;
			}

			// Get ANSI string length
			size_t ansi_length = strlen(ansi_str);
			if (ansi_length == 0)
			{
				unicode_buffer[0] = L'\0';
				return true;
			}

			// Check if buffer is large enough (each ANSI char becomes 1 WCHAR)
			if (ansi_length + 1 > buffer_size)
			{
				return false;
			}

			// Convert each ANSI character to Unicode
			for (size_t i = 0; i < ansi_length; i++)
			{
				unicode_buffer[i] = static_cast<WCHAR>(static_cast<unsigned char>(ansi_str[i]));
			}

			// Null terminate
			unicode_buffer[ansi_length] = L'\0';

			return true;
		}

		PUNICODE_STRING ansi_to_unicode_string(_In_ PCHAR ansi_str)
		{
			if (!ansi_str)
			{
				return nullptr;
			}

			// Get ANSI string length
			size_t ansi_length = strlen(ansi_str);
			if (ansi_length == 0)
			{
				// Return empty UNICODE_STRING
				PUNICODE_STRING empty_str = static_cast<PUNICODE_STRING>(
					utils::memory::allocate_independent_pages(sizeof(UNICODE_STRING), PAGE_READWRITE));
					
				 

				if (empty_str)
				{
					empty_str->Length = 0;
					empty_str->MaximumLength = sizeof(WCHAR);
					empty_str->Buffer = static_cast<PWCH>(
							utils::memory::allocate_independent_pages(sizeof(WCHAR), PAGE_READWRITE));
						
					 

					if (empty_str->Buffer)
					{
						empty_str->Buffer[0] = L'\0';
					}
				}

				return empty_str;
			}

			// Allocate UNICODE_STRING structure
			PUNICODE_STRING unicode_string = static_cast<PUNICODE_STRING>(
				 utils::memory::allocate_independent_pages(sizeof(UNICODE_STRING), PAGE_READWRITE));
				
		 

			if (!unicode_string)
			{
				return nullptr;
			}

			// Allocate buffer for Unicode string
			USHORT buffer_size = static_cast<USHORT>((ansi_length + 1) * sizeof(WCHAR));
			PWCH buffer = static_cast<PWCH>(
				utils::memory::allocate_independent_pages(buffer_size, PAGE_READWRITE));
				
			 

			if (!buffer)
			{
				utils::memory::free_independent_pages(unicode_string, sizeof(UNICODE_STRING));
				return nullptr;
			}

			// Convert ANSI to Unicode
			for (size_t i = 0; i < ansi_length; i++)
			{
				buffer[i] = static_cast<WCHAR>(static_cast<unsigned char>(ansi_str[i]));
			}

			// Null terminate
			buffer[ansi_length] = L'\0';

			// Initialize UNICODE_STRING
			unicode_string->Buffer = buffer;
			unicode_string->Length = static_cast<USHORT>(ansi_length * sizeof(WCHAR));
			unicode_string->MaximumLength = buffer_size;

			return unicode_string;
		}


	}
}