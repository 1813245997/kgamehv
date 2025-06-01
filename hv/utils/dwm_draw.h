#pragma once
namespace utils
{
	namespace dwm_draw
	{
		extern PEPROCESS g_dwm_process;
		extern unsigned long long  g_precall_addr ;
		extern unsigned long long  g_postcall_addr ;
		extern unsigned long long g_ntdll_base ;
		extern unsigned long long g_ntdll_size ;
		extern unsigned long long g_user32_base ;
		extern unsigned long long g_user32_size ;
		extern unsigned long long g_dwmcore_base ;
		extern unsigned long long g_dwmcore_size ;
		extern unsigned long long g_offset_stack ;
		extern unsigned long long g_ccomposition_present ;
		extern bool g_kvashadow ;
		
		NTSTATUS initialize();

		NTSTATUS initialize_user_modules(_In_ PEPROCESS process);

		NTSTATUS get_dwm_process(_Out_ PEPROCESS* process);

		NTSTATUS get_stack_offset();

		NTSTATUS find_precall_and_postcall(IN PEPROCESS process);

		NTSTATUS find_ccomposition_present(IN PEPROCESS process );

		
	}

}