#include "global_defs.h"
#include "window_utils.h"

namespace utils
{
	namespace window_utils
	{

		HANDLE find_window_w_ex(
			_In_opt_ PWCHAR class_name,
			_In_opt_ PWCHAR window_name)
		{
			 

			 

			if (!class_name)
			{
				return nullptr;
			}

			if (!window_name)
			{
				return nullptr;
			}
			PVOID buffer_local = nullptr;
			NTSTATUS status = memory::allocate_user_memory(&buffer_local, 0x1000, PAGE_READWRITE, true, true);
			if (!NT_SUCCESS(status))
			{
				return nullptr;
			}

            
			SIZE_T offset = 0;
			SIZE_T class_name_len = (wcslen(class_name) + 1) * sizeof(WCHAR);   
			SIZE_T window_name_len = (wcslen(window_name) + 1) * sizeof(WCHAR);

			// 分布内存结构 
			PVOID class_name_ptr = reinterpret_cast<PBYTE>(buffer_local) + offset;
			offset += class_name_len;

			PVOID window_name_ptr = reinterpret_cast<PBYTE>(buffer_local) + offset;

			RtlCopyMemory(class_name_ptr, class_name, class_name_len);
			RtlCopyMemory(window_name_ptr, window_name, window_name_len);

			// 构造参数结构（假设调用时传这两个字符串指针即可）
			 unsigned long long   result_ptr = utils::user_call::call(
				utils::dwm_draw::g_find_windoww_fun,
				reinterpret_cast<unsigned long long> ( class_name_ptr),
				reinterpret_cast<unsigned long long> (window_name_ptr),
				0,
				0
			);
			 if (!result_ptr)
			 {
				 return nullptr;
			 }
			HANDLE  handle = (HANDLE) *   reinterpret_cast<PULONG64>(result_ptr);
			HANDLE  process_id  =  utils::internal_functions::pfn_ps_get_current_process_id();
			 utils::memory::free_user_memory(process_id, buffer_local, 0X1000);

			return handle;
		}
	}
}