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

			// 滑动窗口搜索
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

			// 临时指针指向缓冲区末尾，留一个字节给 '\0'
			char* p = buffer + buffer_size - 1;
			*p = '\0';

			bool is_negative = value < 0;
			unsigned int abs_value = is_negative ? -value : value;

			// 从后往前写数字
			do {
				if (p == buffer)  // 防止缓冲区溢出
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

			// p 指向数字字符串起始位置，buffer是缓冲区起点
			// 返回 p，使用者从这里开始就是有效数字字符串
			return p;
		}

		bool int_to_string(int value, char* buffer, size_t buffer_size)
		{
			if (!buffer || buffer_size == 0)
				return false;

			// 先清空缓冲区
			RtlZeroMemory(buffer, buffer_size);

			// 临时缓冲区，足够存下 int 最大长度
			// int 最多10位，带符号+1，留足32字节
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
			if (len + 1 > buffer_size) // +1是结尾'\0'
				return false;

			// 复制结果到用户缓冲区
			RtlCopyMemory(buffer, p, len + 1);

			return true;
		}
		char* int_to_string_alloc(int value)
		{
			// 最多存放 32 字节足够 int 转字符串了
			const size_t buf_size = 32;
			char* buffer = reinterpret_cast<char*> (utils::internal_functions::pfn_ex_allocate_pool_with_tag (NonPagedPool, buf_size, 0));
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

			// 把字符串移到 buffer 起始位置
			size_t len = buffer + buf_size - p - 1;
			RtlMoveMemory(buffer, p, len + 1);  // +1 拷贝 '\0'

			return buffer;  
		}

		char* concat_strings_no_alloc(char* str1, const char* str2, size_t buffer_size)
		{
			if (!str1 || !str2 || buffer_size == 0)
				return nullptr;

			size_t len1 = strlen(str1);
			size_t len2 = strlen(str2);

			// 判断目标缓冲区是否有足够空间存放拼接后的字符串（包括终止符）
			if (len1 + len2 + 1 > buffer_size)
				return nullptr;

			// 拷贝 str2 到 str1 尾部
			RtlCopyMemory(str1 + len1, str2, len2);

			// 添加字符串结束符
			str1[len1 + len2] = '\0';

			return str1;
		}
		char* concat_strings_alloc(const char* str1, const char* str2)
		{
			if (!str1 && !str2)
				return nullptr;

			size_t len1 = strlen(str1);
			size_t len2 = strlen(str2);

			// 分配内存，注意多一个字节存终止符
			char* result = (char*)utils::internal_functions::pfn_ex_allocate_pool_with_tag(
				NonPagedPool,
				(len1 + len2 + 1) * sizeof(char),
				0);  

			if (!result)
				return nullptr;

			// 拷贝第一个字符串
			if (str1)
				RtlCopyMemory(result, str1, len1);

			// 拷贝第二个字符串
			if (str2)
				RtlCopyMemory(result + len1, str2, len2);

			// 添加结尾0
			result[len1 + len2] = '\0';

			return result;
		}


		// 从 PWCHAR 构造一个 UNICODE_STRING（带内存分配）
		PUNICODE_STRING create_unicode_string_from_wchar(_In_ PCWCHAR src)
		{
			if (!src)
				return nullptr;

			// 计算长度
			size_t len = 0;
			while (src[len] != L'\0')
				len++;

			USHORT byte_len = (USHORT)(len * sizeof(WCHAR));

			// 分配 UNICODE_STRING 结构体
			PUNICODE_STRING uni_str = reinterpret_cast<PUNICODE_STRING>(
				utils::internal_functions::pfn_ex_allocate_pool_with_tag(
					NonPagedPool,
					sizeof(UNICODE_STRING),
					'gsuU' // tag: Uusg (utils string unicode)
				));

			if (!uni_str)
				return nullptr;

			RtlZeroMemory(uni_str, sizeof(UNICODE_STRING));

			// 分配字符串 Buffer
			uni_str->Buffer = reinterpret_cast<PWCH>(
				utils::internal_functions::pfn_ex_allocate_pool_with_tag(
					NonPagedPool,
					byte_len + sizeof(WCHAR), // +1 WCHAR for null-terminator
					'gsuB' // tag: Busg (utils string buffer)
				));

			if (!uni_str->Buffer)
			{
				utils::internal_functions::pfn_ex_free_pool_with_tag(uni_str, 'gsuU');
				return nullptr;
			}

			// 拷贝内容
			RtlCopyMemory(uni_str->Buffer, src, byte_len);
			uni_str->Buffer[len] = L'\0';

			uni_str->Length = byte_len;
			uni_str->MaximumLength = (USHORT)(byte_len + sizeof(WCHAR));
			
			return uni_str;
		}

		// 释放由 create_unicode_string_from_wchar 创建的 UNICODE_STRING
		void free_unicode_string(_In_opt_ PUNICODE_STRING uni_str)
		{
			if (!uni_str)
				return;

			if (uni_str->Buffer)
			{
				utils::internal_functions::pfn_ex_free_pool_with_tag(uni_str->Buffer, 'gsuB');
				uni_str->Buffer = nullptr;
			}

			utils::internal_functions::pfn_ex_free_pool_with_tag(uni_str, 'gsuU');
		}


	}
}