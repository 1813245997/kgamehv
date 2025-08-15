#pragma once
namespace utils
{
	namespace user_call
	{
		unsigned long long call(
			unsigned long long func_ptr,
			unsigned long long arg1,
			unsigned long long arg2,
			unsigned long long arg3,
			unsigned long long arg4);

		unsigned long long call6(
			unsigned long long func_ptr,
			unsigned long long arg1,
			unsigned long long arg2,
			unsigned long long arg3,
			unsigned long long arg4,
			unsigned long long arg5,
			unsigned long long arg6);

		unsigned long long call7(
			unsigned long long func_ptr,
			unsigned long long arg1,
			unsigned long long arg2,
			unsigned long long arg3,
			unsigned long long arg4,
			unsigned long long arg5,
			unsigned long long arg6,
			unsigned long long arg7);
		unsigned long long call11(
			unsigned long long func_ptr,
			unsigned long long arg1,
			unsigned long long arg2,
			unsigned long long arg3,
			unsigned long long arg4,
			unsigned long long arg5,
			unsigned long long arg6,
			unsigned long long arg7,
			unsigned long long arg8,
			unsigned long long arg9,
			unsigned long long arg10,
			unsigned long long arg11
		);
		unsigned long long call2(
			unsigned long long ptr, 
			char* user_rsp, 
			void** call_data);
		  
		HMODULE load_librarya(
			const char* dll_name,
			PVOID user_buffer);

		FARPROC get_proc_address(
			HMODULE module,
			const char* func_name,
			PVOID user_buffer);
	 }

}