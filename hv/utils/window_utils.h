#pragma once
namespace utils
{
	namespace window_utils
	{
		HANDLE find_window_by_class_and_title(
			const wchar_t* class_name,
			const wchar_t* window_name,
			PVOID user_buffer);


		bool  get_window_rect_r0(
			HANDLE hwnd,
			RECT* rect_out,
			PVOID user_buffer);
	}
}