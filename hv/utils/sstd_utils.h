#pragma once
namespace utils
{
	namespace ssdt 
	{
		extern  PSSDT g_ssdt_table ;
		extern  PSSDT g_win32k_table ;

		NTSTATUS initialize_ssdt_tables();

		NTSTATUS get_syscall_number(IN const char* fun_name, OUT unsigned int* syscall_number);

		NTSTATUS get_win32_sys_call_number(IN const char* fun_name, OUT unsigned int* syscall_number);

		bool get_ke_service_descriptor_table_addr(void* module_base,  PSSDT* nt_table_out, PSSDT* win32k_table_out);

		unsigned int get_syscall_number_simple(const char* fun_name);

		unsigned int get_win32_syscall_number_simple(const char* fun_name);


		unsigned long long get_ssdt_fun_addr(  unsigned long syscall_number);

		unsigned long long get_syscall_fun_addr( const char* fun_name);

		unsigned long long get_sssdt_fun_addr(void* module_base, unsigned long syscall_number);
		
		unsigned long long get_win32_syscall_fun_addr(void* module_base, const char* fun_name);

    }
}