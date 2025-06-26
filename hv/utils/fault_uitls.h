#pragma once

namespace utils
{
	namespace falt_utils
	{
		BOOLEAN  handle_int3_exception(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode);

		// 处理页面异常（如 STATUS_GUARD_PAGE_VIOLATION 等）
		BOOLEAN  handle_page_fault_exception(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode);

		BOOLEAN 	handle_single_step_exception(
			_Inout_ PEXCEPTION_RECORD ExceptionRecord,
			_Inout_ PCONTEXT ContextRecord,
			_In_ KPROCESSOR_MODE PreviousMode);
	}
}
