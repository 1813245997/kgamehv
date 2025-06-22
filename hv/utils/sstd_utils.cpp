#include "global_defs.h"
#include "sstd_utils.h"
namespace utils
{
	namespace ssdt
	{
		  PSSDT g_ssdt_table = nullptr;
		  PSSDT g_win32k_table = nullptr;
		  NTSTATUS initialize_ssdt_tables()
		  {
			  if (g_ssdt_table && g_win32k_table)
			  {
				  return STATUS_ALREADY_REGISTERED;  // 已初始化
			  }

			  PSSDT tmp_nt_table = nullptr;
			  PSSDT tmp_win32k_table = nullptr;

			  if (!get_ke_service_descriptor_table_addr(module_info::ntoskrnl_base, &tmp_nt_table, &tmp_win32k_table))
			  {
				  return STATUS_NOT_FOUND;
			  }

			  if (!tmp_nt_table || !tmp_win32k_table)
			  {
				  return STATUS_INVALID_ADDRESS;
			  }

			  g_ssdt_table = tmp_nt_table;
			  g_win32k_table = tmp_win32k_table;

			  return STATUS_SUCCESS;
		  }
		NTSTATUS get_syscall_number(IN const char* fun_name, OUT unsigned int* syscall_number)
		{
			if (!fun_name  )
			{
				return STATUS_INVALID_PARAMETER;
			}
			if (!syscall_number)
			{
				return STATUS_INVALID_PARAMETER;
			}
			unsigned long long ntdll_base = 0;

			NTSTATUS status = module_info::map_user_module_to_kernel(L"\\SystemRoot\\System32\\ntdll.dll", &ntdll_base);
			if (!NT_SUCCESS(status) || !ntdll_base)
			{
				return status;
			}

			PUCHAR fun_addr = reinterpret_cast <PUCHAR>  (scanner_fun::find_module_export_by_name(reinterpret_cast<void*> (ntdll_base), fun_name));
			
			if (!fun_addr)
			{

				module_info::free_map_module(ntdll_base);
				return STATUS_PROCEDURE_NOT_FOUND;
			}

			 
#ifdef _X86_
			// 32位下，mov eax, imm32 -> B8 xx xx xx xx
			if (fun_addr[0] == 0xB8)
			{
				*syscall_number = *(unsigned int*)(fun_addr + 1);
				module_info::free_map_module(ntdll_base);
				return STATUS_SUCCESS;
		}
#elif defined(_AMD64_)
			// 64位下，mov eax, imm32 -> 4C 8B D1 B8 xx xx xx xx
			if (*(fun_addr + 3) == 0xB8)
			{
				*syscall_number = *(unsigned int*)(fun_addr + 4);
				module_info::free_map_module(ntdll_base);
				return STATUS_SUCCESS;
			}
#endif 
			module_info::free_map_module(ntdll_base);
			return STATUS_NOT_SUPPORTED;
		}

		bool get_ke_service_descriptor_table_addr(void* module_base, PSSDT* nt_table_out, PSSDT* win32k_table_out)
		{ 
			if (!module_base)
			{
				return false;
			}

			if (!nt_table_out)
			{
				return false;
			}
			if (!win32k_table_out)
			{
				return false;
			}

			unsigned long long ke_sdt_shadow_addr = signature_scanner::find_pattern_image(
				reinterpret_cast<unsigned long long>(module_base),
				"\x4C\x8D\x15\x00\x00\x00\x00\x4C\x8D\x1D\x00\x00\x00\x00\xF7",
				"xxx????xxx????x",
				".text");

			if (!ke_sdt_shadow_addr)
				return false;

			ULONG rel_offset = *(ULONG*)(ke_sdt_shadow_addr + 10);
			PSSDT nt_table = reinterpret_cast<PSSDT>(rel_offset + ke_sdt_shadow_addr + 14);
			PSSDT win32k_table = nt_table + 1;

			*nt_table_out = nt_table;
			*win32k_table_out = win32k_table;
			return true;
		}


		unsigned int get_syscall_number_simple(const char* fun_name)
		{
			unsigned int syscall_number = 0;
			if (!fun_name)
			{	return 0;

			}

			NTSTATUS status = get_syscall_number(fun_name, &syscall_number);
			if (!NT_SUCCESS(status))
			{
				return 0;

			}

			return syscall_number;
		}

		unsigned long long get_ssdt_fun_addr(void* module_base, unsigned long syscall_number)
		{
			 
			if (!g_ssdt_table)
			{
				PSSDT tmp_nt_table = nullptr;
				PSSDT tmp_win32k_table = nullptr;

				if (!get_ke_service_descriptor_table_addr(module_base ,&tmp_nt_table, &tmp_win32k_table) )
				{
					return 0;
				}

				 
				g_ssdt_table = tmp_nt_table;
				g_win32k_table = tmp_win32k_table;
			}

		 
			PULONG service_table_base = reinterpret_cast<PULONG>(g_ssdt_table->ServiceTable);
			if (!service_table_base)
			{
				return 0;

			}

			 
			ULONG service_table_entry = service_table_base[syscall_number];
			service_table_entry >>= 4;
			unsigned long long function_address = reinterpret_cast<unsigned long long>(service_table_base) + service_table_entry;

			return function_address;
		}

		unsigned long long get_syscall_fun_addr(void* module_base, const char* fun_name)
		{
			unsigned int syscall_number = get_syscall_number_simple(fun_name);
			 if (!syscall_number)
			 {
				 return 0;
			 }

			unsigned long long fun_addr =  get_ssdt_fun_addr(module_base,syscall_number);
			return fun_addr;
		}



	}


}