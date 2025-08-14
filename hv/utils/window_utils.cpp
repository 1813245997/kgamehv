#include "global_defs.h"
#include "window_utils.h"

namespace utils
{
	namespace window_utils
	{
		HANDLE find_window_by_class_and_title(
			const wchar_t* class_name,
			const wchar_t* window_name,
			PVOID user_buffer)
		{
			if (!class_name || !window_name || !user_buffer)
				return nullptr;

			// 确保偏移固定，避免冲突
			PVOID class_name_ptr = reinterpret_cast<PBYTE>(user_buffer) + 0x100;
			PVOID window_name_ptr = reinterpret_cast<PBYTE>(user_buffer) + 0x300;

			// 拷贝类名
			SIZE_T class_name_len = (wcslen(class_name) + 1) * sizeof(WCHAR);
			RtlCopyMemory(class_name_ptr, class_name, class_name_len);
			reinterpret_cast<wchar_t*>(class_name_ptr)[(class_name_len / sizeof(WCHAR)) - 1] = L'\0';

			// 拷贝窗口名
			SIZE_T window_name_len = (wcslen(window_name) + 1) * sizeof(WCHAR);
			RtlCopyMemory(window_name_ptr, window_name, window_name_len);
			reinterpret_cast<wchar_t*>(window_name_ptr)[(window_name_len / sizeof(WCHAR)) - 1] = L'\0';

			// 调用用户态 FindWindowW
			unsigned long long result_ptr = utils::user_call::call(
				utils::dwm_draw::g_find_windoww_fun,
				reinterpret_cast<unsigned long long>(class_name_ptr),
				reinterpret_cast<unsigned long long>(window_name_ptr),
				0, 0);

			if (result_ptr)
			{
				HANDLE handle = reinterpret_cast<HANDLE>(
					*reinterpret_cast<PULONG64>(result_ptr));

				RtlZeroMemory(user_buffer, 0x1000);
				if (handle)
				{
					
					return handle;
				}
			}
			 

			return nullptr;
		}

	}
}