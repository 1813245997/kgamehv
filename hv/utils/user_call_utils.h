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

		unsigned long long call2(
			unsigned long long ptr, 
			char* user_rsp, 
			void** call_data);
		  
 
	 }

}