#pragma once
namespace utils
{
	namespace module_info
	{

		extern PVOID ntoskrnl_base;
		extern ULONG64 ntoskrnl_size;

		extern PVOID g_ci_base;
		extern ULONG64 g_ci_size;

		// Common user module global variables
		extern PVOID ntdll_base;
		extern ULONG64 ntdll_size;
		
		extern PVOID kernel32_base;
		extern ULONG64 kernel32_size;
		
		extern PVOID user32_base;
		extern ULONG64 user32_size;
		
		extern PVOID gdi32_base;
		extern ULONG64 gdi32_size;
		
		extern PVOID dwmcore_base;
		extern ULONG64 dwmcore_size;
		
		extern PVOID dwmapi_base;
		extern ULONG64 dwmapi_size;
		
		extern PVOID dxgi_base;
		extern ULONG64 dxgi_size;
		
		extern PVOID d3dcompiler_47_base;
		extern ULONG64 d3dcompiler_47_size;

		BOOLEAN  get_driver_module_info(const char* module_name, ULONG64& module_size, PVOID& module_base_address);

 

		BOOLEAN init_ntoskrnl_info();
		
		NTSTATUS get_process_module_info(
			_In_ PEPROCESS process,
			_In_ const wchar_t* module_name,
			_Out_ unsigned long long* base_address,
			_Out_ SIZE_T* module_size
		);
		
		NTSTATUS map_user_module_to_kernel(const wchar_t* module_full_path, unsigned long long* base_address_out);
		
		NTSTATUS free_map_module(unsigned long long base_address);

		NTSTATUS initialize_all_user_modules();

		ProcessModule get_module(_In_ PEPROCESS process,_In_ const wchar_t* module_name);
	}
}