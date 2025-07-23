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

		unsigned long long new_get_pte_base();

		NTSTATUS allocate_user_hidden_exec_memory(_In_  PEPROCESS process ,OUT PVOID* base_address, _In_   SIZE_T size, bool load = true, bool hide = true);

		NTSTATUS allocate_user_memory(OUT PVOID* base_address, _In_   SIZE_T size, ULONG protect = PAGE_READWRITE, bool load = true ,bool hide =true);

		NTSTATUS free_user_memory(_In_ HANDLE process_id, _In_  PVOID base_address, _In_ SIZE_T size,  bool hide = true);

		NTSTATUS lock_memory(unsigned long long  address, ULONG size, OUT PMDL* out_mdl);

		void unlock_memory(PMDL mdl);

		unsigned long long get_pte_base();

		unsigned long long get_pde_base();

		unsigned long long get_ppe_base();

		unsigned long long get_pxe_base();



		unsigned long long get_pml4(unsigned long long virtual_address);

		unsigned long long get_pdpte(unsigned long long virtual_address);

		unsigned long long get_pde(unsigned long long virtual_address);

		unsigned long long get_pte(unsigned long long virtual_address);

		unsigned long long get_physical_address(unsigned long long virtual_address);

		bool is_virtual_address_valid(unsigned long long virtual_address);

		void set_execute_page(unsigned long long virtual_address, SIZE_T length);

		void mem_zero(PVOID ptr, SIZE_T size, UCHAR filling = 0);

		void mem_copy(PVOID dst, PVOID src, ULONG size);

		bool is_user_address(void* virtual_address);

		bool is_executable_address(unsigned long long virtual_address);

		//新获取PTE 等算法
		long long mm_get_pte_address(long long virtual_address);

		long long mm_get_pde_address(long long virtual_address);

		long long mm_get_ppe_address(long long virtual_address);

		long long mm_get_pxe_address(long long virtual_address);

	}
}