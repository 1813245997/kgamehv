#pragma once
namespace utils
{
	namespace module_info
	{

		BOOLEAN  get_driver_module_info(const char* module_name, ULONG64& module_size, PVOID& module_base_address);
		BOOLEAN init_ntoskrnl_info();


		extern PVOID ntoskrnl_base;
		extern ULONG64 ntoskrnl_size;
	}
}