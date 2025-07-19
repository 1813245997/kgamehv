#include "global_defs.h"
#include "fault_uitls.h"
 
namespace utils
{
	namespace falt_utils
	{
		// 处理页面异常（如 STATUS_GUARD_PAGE_VIOLATION 等）
		BOOLEAN   handle_page_fault_exception(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode)
		{
			UNREFERENCED_PARAMETER(ContextRecord);
			////还要处理页面异常  处理注入DWM绘制截图相关事宜
	  //      //https://github.com/lainswork/dwm-screen-shot 是通过页面异常hook  游戏反作弊也极有可能通过VEH这类hook   要处理几个可能会被拿到swap指针的函数
			////不可以dprinftf 输出 因为是触发异常输出的
			if (PreviousMode != MODE::UserMode)
			{

				return FALSE;
			}

			if (dwm_draw::g_dwm_process != internal_functions::pfn_ps_get_current_process())
			{
				return FALSE;
			}



			utils::strong_dx::g_should_hide_overlay = true;

			//PVOID page_base = PAGE_ALIGN(ExceptionRecord->ExceptionAddress);
		 //

			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
			//	"[PAGE_FAULT_FIX] Address: 0x%p  PageBase: 0x%p  \n",
			//	ExceptionRecord->ExceptionAddress, page_base );

			 

			return FALSE;
			 

		}

		 
		BOOLEAN  handle_int3_exception(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode)
		{
		

			 //不可以dprinftf 输出 因为是触发异常输出的
			hyper::EptHookInfo* matched_hook_info = nullptr;
 
			if (PreviousMode != MODE::UserMode) 
			{
				return FALSE;
			}

 

			if (!hyper::find_hook_break_point_int3(ExceptionRecord->ExceptionAddress, &matched_hook_info))
			{
				return FALSE;
			}

			using handler_fn_t = BOOLEAN(__fastcall*)(PEXCEPTION_RECORD, PCONTEXT, hyper::EptHookInfo*);
			auto handler = reinterpret_cast<handler_fn_t>(matched_hook_info->handler_va);

			if (handler(ExceptionRecord, ContextRecord, matched_hook_info))
			{
				return TRUE;  // 已处理
			}


			return FALSE;


		}

	
			 

		BOOLEAN 	handle_single_step_exception(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode)
		{
			UNREFERENCED_PARAMETER(ContextRecord);
			if (PreviousMode != MODE::UserMode)
			{
				return FALSE;
			}

			if (dwm_draw::g_dwm_process != internal_functions::pfn_ps_get_current_process())
			{

				return FALSE;
			}


			utils::strong_dx::g_should_hide_overlay = true;

			//PVOID page_base = PAGE_ALIGN(ExceptionRecord->ExceptionAddress);
			////SIZE_T region_size = PAGE_SIZE;
		 //

			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
			//	"[SINGLE_STEP_HOOK] Address: 0x%p  PageBase: 0x%p  \n",
			//	ExceptionRecord->ExceptionAddress, page_base );

			// 可选恢复页权限逻辑...

			return FALSE;
		}
	 }
}