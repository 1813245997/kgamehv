#include "global_defs.h"
#include "memory_utils.h"

#define PTE_MASK (0x7FFFFFFFF8)

namespace utils
{
	namespace memory
	{
#define SizeAlign(Size) ((Size + 0xFFF) & 0xFFFFFFFFFFFFF000)


		unsigned long long g_pte_base{};
		unsigned long long g_pde_base{};
		unsigned long long g_ppe_base{};
		unsigned long long g_pxe_base{};


		NTSTATUS initialize_all_paging_base()
		{
			NTSTATUS status = initialize_pte_base();
			if (!NT_SUCCESS(status))
			{
				return status;
			}
			
			g_pde_base = mm_get_pte_address(g_pte_base);
			g_ppe_base = mm_get_pte_address(g_pde_base);
			g_pxe_base = mm_get_pte_address(g_ppe_base);

			LogDebug("Paging Base Values:\n"
				"  PTE Base: 0x%llX\n"
				"  PDE Base: 0x%llX\n"
				"  PPE Base: 0x%llX\n"
				"  PXE Base: 0x%llX",
				g_pte_base, g_pde_base, g_ppe_base, g_pxe_base);
			return STATUS_SUCCESS;
		}
		unsigned long long new_get_pte_base()
		{
			PHYSICAL_ADDRESS physical_address;
			unsigned long long pte_base{};
			unsigned long long index{};
			physical_address.QuadPart = __readcr3() & 0xfffffffffffff000;   // 获取CR3寄存器，清除低12位
			PLONG64 pxe_ptr =  reinterpret_cast<PLONG64>  (MmGetVirtualForPhysical(physical_address));   // 获取其所在的虚拟地址 - 页表自映射
			
			// 遍历比较
			while ((pxe_ptr[index] & 0xfffffffff000) != physical_address.QuadPart) {
				index++;
				if (index >= 512) {
					return 0;
				}
			}
			// 计算pte基址
			pte_base = ((index + 0x1fffe00) << 39);

			return pte_base;
		}
		NTSTATUS initialize_pte_base()
		{
			ULONG dw_build_number = utils::os_info::get_build_number();
			if (dw_build_number < 14393)
			{
				g_pte_base = 0xFFFFF68000000000llu;
			}
			else
			{
				//wchar_t wa_MmGetVirtualForPhysical[] = { 0xE3AE, 0xE38E, 0xE3A4, 0xE386, 0xE397, 0xE3B5, 0xE38A, 0xE391, 0xE397, 0xE396, 0xE382, 0xE38F, 0xE3A5, 0xE38C, 0xE391, 0xE3B3, 0xE38B, 0xE39A, 0xE390, 0xE38A, 0xE380, 0xE382, 0xE38F, 0xE3E3, 0xE3E3 };

				//for (int i = 0; i < 25; i++)
				//{
				//	wa_MmGetVirtualForPhysical[i] ^= 0x6D6D;
				//	wa_MmGetVirtualForPhysical[i] ^= 0x8E8E;
				//};

				//UNICODE_STRING unFuncNameMmGetVirtualForPhysical = { 0 };
				//RtlInitUnicodeString(&unFuncNameMmGetVirtualForPhysical, wa_MmGetVirtualForPhysical);
				//PUCHAR funcMmGetVirtualForPhysical = (PUCHAR)MmGetSystemRoutineAddress(&unFuncNameMmGetVirtualForPhysical);
				//g_pte_base = *(PULONG64)(funcMmGetVirtualForPhysical + 0x22);

				g_pte_base = new_get_pte_base();
			}
			  
			return STATUS_SUCCESS;
		}

		

		unsigned long long get_pte_base()
		{ 
			return g_pte_base;
		}

		unsigned long long get_pde_base()
		{
			return g_pde_base;
		}

		unsigned long long get_ppe_base()
		{
			return g_ppe_base;
		}

		unsigned long long get_pxe_base()
		{
			return g_pxe_base;
		}



		unsigned long long get_pml4(unsigned long long virtual_address)
		{
		 
			return  mm_get_pxe_address(virtual_address);
		}

		unsigned long long get_pdpte(unsigned long long virtual_address)
		{
			 
			return mm_get_ppe_address(virtual_address);
		}

		unsigned long long get_pde(unsigned long long virtual_address)
		{
			 

			return  mm_get_pde_address(virtual_address);
		}

		unsigned long long get_pte(unsigned long long virtual_address)
		{
			
			return  mm_get_pte_address(virtual_address);
		}

		unsigned long long get_physical_address(unsigned long long virtual_address)
		{

			ppte_64 pml4_entry = reinterpret_cast<ppte_64>(get_pml4(virtual_address));
			if (!pml4_entry->valid || pml4_entry->large_page)
			{
				return 0;
			}

			ppte_64 pdpt_entry = reinterpret_cast<ppte_64>(get_pdpte(virtual_address));
			if (!pdpt_entry->valid  )
			{
				return 0;
			}

			if (pdpt_entry->large_page)
			{
				constexpr ULONG64 LARGE_PAGE_OFFSET_MASK = 0x3FFFFFFF;
				ULONG64 offset = virtual_address & LARGE_PAGE_OFFSET_MASK;
				return (pdpt_entry->page_frame_number << 12) + offset;
			}

			ppte_64 pde_entry = reinterpret_cast<ppte_64>(get_pde(virtual_address));
			if (!pde_entry->valid)
			{
				return 0;
			}

			// Handle 2MB large page
			if (pde_entry->large_page)
			{
				constexpr ULONG64 LARGE_PAGE_OFFSET_MASK = 0x1FFFFF;
				ULONG64 offset = virtual_address & LARGE_PAGE_OFFSET_MASK;
				return (pde_entry->page_frame_number << 12) + offset;
			}

			ppte_64 pte_entry = reinterpret_cast<ppte_64>(get_pte(virtual_address));
			if (!pte_entry->valid)
			{
				return 0;
			}

			ULONG64 offset = virtual_address & 0xFFF;
			return (pte_entry->page_frame_number << 12) + offset;
		}

		bool is_virtual_address_valid(unsigned long long virtual_address)
		{
			// 1. PML4E
			auto pml4_entry = reinterpret_cast<ppte_64>(get_pml4(virtual_address));
			if (!pml4_entry->valid)
			{
				return false;
			}

			// 2. PDPTE
			auto pdpt_entry = reinterpret_cast<ppte_64>(get_pdpte(virtual_address));
			if (!pdpt_entry->valid)
			{
				return false;
			}
			if (pdpt_entry->large_page)
			{
				// 1 GB 大页映射，地址有效
				return true;
			}

			// 3. PDE
			auto pde_entry = reinterpret_cast<ppte_64>(get_pde(virtual_address));
			if (!pde_entry->valid)
			{
				return false;
			}
			if (pde_entry->large_page)
			{
				// 2 MB 大页映射，地址有效
				return true;
			}

			// 4. PTE
			auto pte_entry = reinterpret_cast<ppte_64>(get_pte(virtual_address));
			return pte_entry->valid;
		}


		void set_execute_page(unsigned long long virtual_address, SIZE_T length)
		{

			unsigned long long  start_address = virtual_address & (~0xFFF);
			unsigned long long  end_address = (virtual_address + length) & (~0xFFF);

			for (unsigned long long current_address = start_address; current_address <= end_address; current_address += PAGE_SIZE)
			{

				ppte_64 pde = reinterpret_cast<ppte_64>  (get_pde(current_address));


				if (internal_functions::pfn_mm_is_address_valid_ex (pde) && pde->valid == 1)
				{
					pde->no_execute = 0;

				}
				ppte_64 pte = reinterpret_cast<ppte_64>(get_pte(current_address));
				if (internal_functions::pfn_mm_is_address_valid_ex(pte) && pte->valid == 1)
				{
					pte->no_execute = 0;

				}


				__invlpg(reinterpret_cast<PVOID>(current_address));
			}
		}


		void set_pte_bits(unsigned long long virtual_address, SIZE_T length, ULONG64 bits_to_set)
		{
			const ULONG64 safe_bit_mask = 0xFFF0000000000FFFULL;
			ULONG64 masked_bits = bits_to_set & safe_bit_mask;
			unsigned long long start_address = virtual_address & (~0xFFF);
			unsigned long long end_address = (virtual_address + length) & (~0xFFF);


			for (unsigned long long current_address = start_address; current_address <= end_address; current_address += PAGE_SIZE)
			{

				ppte_64 pte = reinterpret_cast<ppte_64>(get_pte(current_address));

				if (MmIsAddressValid(pte) && pte->valid == 1)
				{
					*reinterpret_cast<PULONG64>(pte) |= masked_bits;
					__invlpg(reinterpret_cast<PVOID>(current_address));
				}
			}
		}



		   void mem_zero(PVOID ptr, SIZE_T size, UCHAR filling  ) {
			__stosb((PUCHAR)ptr, filling, size);
		}

		   void mem_copy(PVOID dst, PVOID src, ULONG size) {
			   __movsb((PUCHAR)dst, (const PUCHAR)src, size);
		   }
		   bool is_user_address(void* virtual_address)
		   {
			   return virtual_address <= MM_HIGHEST_USER_ADDRESS;
		   }
		   bool is_executable_address(unsigned long long virtual_address)
		   {
			   PULONG64  pml4 = reinterpret_cast<PULONG64>	(get_pml4(virtual_address));
			   unsigned long long entry = *pml4;
			   if (!MmiCheckFlag(entry, MmiEntryFlag_Present))
			   {
				   return false;
			   }
			   PULONG64  pdpte = reinterpret_cast<PULONG64>	(get_pdpte(virtual_address));
			   entry = *pdpte;
			   if (!MmiCheckFlag(entry, MmiEntryFlag_Present))
			   {
				   return false;
			   }


			   if (MmiCheckFlag(entry, MmiEntryFlag_HugePage))
			   {
				   if (!MmiCheckFlag(entry, MmiEntryFlag_NoExecute))
				   {
					   return true;
				   }
				   return false;
			   }

			   PULONG64 pde = reinterpret_cast<PULONG64>	(get_pde(virtual_address));
			   entry = *pde;

			   if (!MmiCheckFlag(entry, MmiEntryFlag_Present))
			   {
				   return false;
			   }
			   if (MmiCheckFlag(entry, MmiEntryFlag_HugePage))
			   {
				   if (!MmiCheckFlag(entry, MmiEntryFlag_NoExecute))
				   {
					   return true;
				   }
				   return false;
			   }

			   PULONG64 pte = reinterpret_cast<PULONG64>	(get_pte(virtual_address));
			   entry = *pte;
			   if (!MmiCheckFlag(entry, MmiEntryFlag_Present))
			   {
				   return false;
			   }

			   if (!MmiCheckFlag(entry, MmiEntryFlag_NoExecute))
			   {
				   return true;
			   }

			   return false;
		   }
		   

	 
		   long long mm_get_pte_address(long long virtual_address)
		   {
			   return ((((((long long)virtual_address) & 0x0000FFFFFFFFF000) >> 12) << 3) + (g_pte_base));
		   }

		   long long mm_get_pde_address(long long virtual_address)
		   {
			   return ((((((long long)virtual_address) & 0x0000FFFFFFFFF000) >> 21) << 3) + (g_pde_base));
		   }

		   long long mm_get_ppe_address(long long virtual_address)
		   {
			   return ((((((long long)virtual_address) & 0x0000FFFFFFFFF000) >> 30) << 3) + (g_ppe_base));
		   }

		   long long  mm_get_pxe_address(long long virtual_address)
		   {
			   return ((((((long long)virtual_address) & 0x0000FFFFFFFFF000) >> 39) << 3) + (g_pxe_base));
		   }


		   NTSTATUS allocate_user_hidden_exec_memory(_In_  PEPROCESS process, OUT PVOID* base_address, _In_   SIZE_T size, bool load, bool hide)
		   {
			   SIZE_T region_size = size;
			   NTSTATUS status = internal_functions::pfn_zw_allocate_virtual_memory(
				   ZwCurrentProcess(),
				   base_address,
				   0,
				   &region_size,
				   MEM_COMMIT | MEM_RESERVE,
				   PAGE_EXECUTE_READWRITE
			   );

			   if (NT_SUCCESS(status))
			   {
				   HANDLE pid = utils::internal_functions::pfn_ps_get_process_id(process);
				   unsigned long long start_addr = reinterpret_cast<unsigned long long>(*base_address);
				   unsigned long long end_addr = start_addr + region_size;

				   if (load)
				   {
					   mem_zero(*base_address, region_size);
				   }

				   if (hide)
				   {
					   utils::hidden_user_memory::insert_hidden_address_for_pid(pid, start_addr, end_addr);
				   }

				   // 输出调试信息
			   /*	DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
					   "[allocate_user_hidden_exec_memory] PID: %lu, Addr: 0x%llX, Size: 0x%llX bytes\n",
					   pid, start_addr, region_size);*/
			   }
			   //set_execute_page(*reinterpret_cast<PULONG64> ( base_address),  region_size);
			   return status;
		   }
		   NTSTATUS allocate_user_memory(OUT PVOID* base_address, _In_   SIZE_T size, ULONG protect, bool load, bool hide)
		   {
			   SIZE_T region_size = SizeAlign(size);

			   NTSTATUS status = internal_functions::pfn_zw_allocate_virtual_memory(
				   ZwCurrentProcess(),
				   base_address,
				   0,
				   &region_size,
				   MEM_COMMIT,
				   protect
			   );


			   if (NT_SUCCESS(status)) {
				   HANDLE pid = utils::internal_functions::pfn_ps_get_current_process_id();
				   unsigned long long start_addr = reinterpret_cast<unsigned long long>(*base_address);
				   unsigned long long end_addr = start_addr + region_size;

				   // 输出分配信息
			   /*	DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0,
					   "[allocate_user_memory] PID: %lu, Addr: 0x%llX, Size: 0x%llX bytes\n",
					   pid, start_addr, region_size);*/

				   if (load) {
					   mem_zero(*base_address, region_size);
				   }

				   if (hide) {
					   utils::hidden_user_memory::insert_hidden_address_for_pid(pid, start_addr, end_addr);
				   }
			   }
			   return status;

		   }

		   NTSTATUS free_user_memory(
			   _In_ HANDLE process_id,
			   _In_ PVOID base_address,
			   _In_ SIZE_T size,
			   bool hide)
		   {

			   if (!process_id)
			   {
				   return STATUS_INVALID_PARAMETER_1;
			   }

			   if (!base_address)
			   {
				   return STATUS_INVALID_PARAMETER_2;
			   }

			   if (size == 0)
			   {
				   return STATUS_INVALID_PARAMETER_3;
			   }



			   NTSTATUS status = utils::internal_functions::pfn_zw_free_virtual_memory(
				   ZwCurrentProcess(),
				   &base_address,
				   &size,
				   MEM_RELEASE);

			   if (NT_SUCCESS(status) && hide)
			   {
				   unsigned long long start_addr = reinterpret_cast<unsigned long long>(base_address);
				   unsigned long long end_addr = start_addr + size;

				   utils::hidden_user_memory::remove_hidden_address_for_pid(
					   process_id,
					   start_addr,
					   end_addr);
			   }

			   return status;
		   }


		   NTSTATUS lock_memory(unsigned long long  address, ULONG size, OUT PMDL* out_mdl)
		   {
			   PMDL mdl = NULL;

			   mdl = internal_functions::pfn_io_allocate_mdl(reinterpret_cast<PVOID> (address), size, FALSE, FALSE, NULL);
			   if (mdl)
			   {
				   internal_functions::pfn_mm_probe_and_lock_pages(mdl, UserMode, IoReadAccess);
			   }
			   *out_mdl = mdl;
			   return STATUS_SUCCESS;

		   }

		   void unlock_memory(PMDL mdl)
		   {
			   if (!mdl)
			   {
				   return;
			   }

			   internal_functions::pfn_mm_unlock_pages(mdl);
			   internal_functions::pfn_io_free_mdl(mdl);
		   }

		   //NTSTATUS rtl_super_copy_memory(IN VOID UNALIGNED* destination, IN CONST VOID UNALIGNED* source, IN ULONG length)
		   //{
			  // //Change memory properties.
			  // PMDL g_pmdl = IoAllocateMdl(destination, length, 0, 0, NULL);
			  // if (!g_pmdl)
				 //  return STATUS_UNSUCCESSFUL;
			  // MmBuildMdlForNonPagedPool(g_pmdl);
			  // PVOID Mapped = MmMapLockedPages(g_pmdl, KernelMode);
			  // if (!Mapped)
			  // {
				 //  IoFreeMdl(g_pmdl);
				 //  return STATUS_UNSUCCESSFUL;
			  // }
			  // KIRQL kirql = KeRaiseIrqlToDpcLevel();
			  // RtlCopyMemory(Mapped, source, length);
			  // KeLowerIrql(kirql);
			  // //Restore memory properties.
			  // MmUnmapLockedPages((PVOID)Mapped, g_pmdl);
			  // IoFreeMdl(g_pmdl);
			  // return STATUS_SUCCESS;
		   //}

		   //NTSTATUS rtl_super_copy_memory_from_source(
			  // IN VOID UNALIGNED* destination,
			  // IN CONST VOID UNALIGNED* source,
			  // IN ULONG length)
		   //{
			  // // Allocate an MDL for the source address
			  // PMDL sourceMdl = IoAllocateMdl((PVOID)source, length, 0, 0, NULL);
			  // if (!sourceMdl)
				 //  return STATUS_UNSUCCESSFUL;

			  // // Build the MDL for non-paged pool
			  // MmBuildMdlForNonPagedPool(sourceMdl);

			  // // Map the source address into kernel mode
			  // PVOID mappedSource = MmMapLockedPages(sourceMdl, KernelMode);
			  // if (!mappedSource)
			  // {
				 //  IoFreeMdl(sourceMdl);
				 //  return STATUS_UNSUCCESSFUL;
			  // }

			  // // Perform the memory copy
			  // KIRQL kirql = KeRaiseIrqlToDpcLevel();
			  // RtlCopyMemory(destination, mappedSource, length);
			  // KeLowerIrql(kirql);

			  // // Restore memory properties
			  // MmUnmapLockedPages(mappedSource, sourceMdl);
			  // IoFreeMdl(sourceMdl);
			  // return STATUS_SUCCESS;
		   //}

	}
}