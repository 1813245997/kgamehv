#pragma once
namespace utils
{
	namespace user_call
	{


		extern unsigned long long g_d3dcompile_fun;
		extern unsigned long long g_gettickcount64_fun;
		extern unsigned long long g_getsystemmetrics_fun;

		NTSTATUS initialize_user_call_utils();

		NTSTATUS initialize_all_user_functions();

		NTSTATUS get_stack_offset();

		NTSTATUS   initialize_ki_call_user_mode2(OUT unsigned long long* ki_call_user_mode2);


		
		unsigned long long call(
			unsigned long long func_ptr,
			unsigned long long arg1,
			unsigned long long arg2,
			unsigned long long arg3,
			unsigned long long arg4);

		unsigned long long call4(
			unsigned long long func_ptr,
			unsigned long long arg1,
			unsigned long long arg2,
			unsigned long long arg3,
			unsigned long long arg4
			 
		);


		unsigned long long call5(
			unsigned long long func_ptr,
			unsigned long long arg1,
			unsigned long long arg2,
			unsigned long long arg3,
			unsigned long long arg4,
			unsigned long long arg5 
			 );

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

		NTSTATUS initialize_ki_call_user_mode2_callbacks();

		NTSTATUS find_precall_address(
			IN PEPROCESS process,
			IN ULONG_PTR ntdll_base,
			OUT ULONG_PTR* precall_addr_out);

		NTSTATUS find_postcall_address(
			IN PEPROCESS process,
			IN ULONG_PTR user32_base,
			OUT ULONG_PTR* postcall_addr_out);

 
	 }

}