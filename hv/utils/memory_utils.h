#pragma once
namespace utils
{
	namespace memory
	{
		struct physical_page_info
		{
			unsigned  long long base_address;
			unsigned  long long pte_address;
			SIZE_T size;
		};

		struct PAGE_TABLE_INFO
		{
			ULONG64 Pxe;
			ULONG64 Ppe;
			ULONG64 Pde;
			ULONG64 Pte;
			ULONG PageType;
		};


		extern unsigned long long g_pte_base;
		extern unsigned long long g_pde_base;
		extern unsigned long long g_ppe_base;
		extern unsigned long long g_pxe_base;

		NTSTATUS initialize_all_paging_base();

		NTSTATUS initialize_pte_base();

		unsigned long long new_get_pte_base();
		 
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

		void set_pte_bits(unsigned long long virtual_address, SIZE_T length, ULONG64 bits_to_set);

		void mem_zero(PVOID ptr, SIZE_T size, UCHAR filling = 0);

		void mem_copy(PVOID dst, PVOID src, ULONG size);

		bool is_user_address(void* virtual_address);

		bool is_executable_address(unsigned long long virtual_address);

		bool is_va_physical_address_valid(unsigned long long virtual_address);

		bool is_phys_page_in_range(unsigned  long long phys_page_base, size_t size);

	 
		long long mm_get_pte_address(long long virtual_address);

		long long mm_get_pde_address(long long virtual_address);

		long long mm_get_ppe_address(long long virtual_address);

		long long mm_get_pxe_address(long long virtual_address);

        PVOID allocate_independent_pages(__in SIZE_T size,__in ULONG new_protect);

		VOID free_independent_pages(__in PVOID base_address, __in SIZE_T size);

		NTSTATUS allocate_user_hidden_exec_memory(_In_  PEPROCESS process, OUT PVOID* base_address, _In_   SIZE_T size, bool load = true, bool hide = true);

		NTSTATUS allocate_user_memory(OUT PVOID* base_address, _In_   SIZE_T size, ULONG protect = PAGE_READWRITE, bool load = true, bool hide = true);

		NTSTATUS free_user_memory(_In_ HANDLE process_id, _In_  PVOID base_address, _In_ SIZE_T size, bool hide = true);

		NTSTATUS  free_user_memory(
			_In_ PVOID base_address,
			_In_ SIZE_T size,
			bool hide = true);
		NTSTATUS lock_memory(unsigned long long  address, ULONG size, OUT PMDL* out_mdl);

		void unlock_memory(PMDL mdl);

		NTSTATUS read_virtual_memory(
			unsigned long long directory_table_base,
			void* target_address,
			void* out_buffer,
			PULONG pSizeRead);

		NTSTATUS   allocate_physical_page(
			physical_page_info* physical_page_info,
			SIZE_T size);

		NTSTATUS get_phys_page_address(
			physical_page_info* transfer_page_info,
			unsigned long long target_cr3,
			PVOID page_va,
			PULONG64 physical_address_out);

		NTSTATUS   get_phys_page_size(
			physical_page_info* transfer_page_info,
			unsigned long long page_address,
			size_t * page_size,
			unsigned long long target_cr3);

		NTSTATUS get_page_table_info(
			physical_page_info* transfer_page_info,
			ULONG64 cr3,
			ULONG64 page_address,
			PAGE_TABLE_INFO* page_table_info_out);


		NTSTATUS read_physical_page(
			physical_page_info* transfer_page_info,
			ULONG64 phys_page_base,
			PVOID buffer,
			SIZE_T size);

		void __fastcall free_physical_page(
			physical_page_info* page_info);
		 
		template<typename T>
		NTSTATUS read_virtual_object(ULONG64 cr3, PVOID address, T& out_value)
		{
			ULONG size = sizeof(T);
			return read_virtual_memory(cr3, address, &out_value, &size);
		}


		template<typename T>
		T read(uintptr_t address, bool safe_check = true) {
			if (safe_check && !utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(address))) {
				return T{};
			}

			T buffer{};
			 memcpy(&buffer, reinterpret_cast<void*>(address), sizeof(T));
			return buffer;
		}

	     bool read_raw(uintptr_t address, void* buffer, size_t size, bool safe_check = true);
		
		 
	}
}