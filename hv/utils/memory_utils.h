#pragma once
namespace utils
{
	namespace memory
	{
		extern unsigned long long g_pte_base;
		extern unsigned long long g_pde_base;
		extern unsigned long long g_ppe_base;
		extern unsigned long long g_pxe_base;

		NTSTATUS initialize_all_paging_base();

		NTSTATUS initialize_pte_base();

		unsigned long long get_pte_base();

		unsigned long long get_pml4(unsigned long long virtual_address);

		unsigned long long get_pdpte(unsigned long long virtual_address);

		unsigned long long get_pde(unsigned long long virtual_address);

		unsigned long long get_pte(unsigned long long virtual_address);

		unsigned long long get_physical_address(unsigned long long virtual_address);

		bool is_virtual_address_valid(unsigned long long virtual_address);

		void set_execute_page(unsigned long long virtual_address, SIZE_T length);


	}
}