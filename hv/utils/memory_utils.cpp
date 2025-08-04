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
		PPHYSICAL_MEMORY_RANGE g_physical_memory_ranges = 0;

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
		   
		   bool is_va_physical_address_valid(unsigned long long   virtual_address)
		   {
			   return utils::internal_functions::pfn_mm_get_physical_address (reinterpret_cast<PVOID> (virtual_address)).QuadPart > 0x1000;
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
		   NTSTATUS  free_user_memory(
			   _In_ PVOID base_address,
			   _In_ SIZE_T size,
			   bool hide)
		   {
			   const HANDLE current_pid = PsGetCurrentProcessId();

			   return free_user_memory(current_pid, base_address, size, hide);
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

		   KIRQL __fastcall raise_irql(KIRQL NewIrql)
		   {
			   KIRQL CurrentIrql;

			   CurrentIrql = KeGetCurrentIrql();
			   __writecr8(NewIrql);
			   return CurrentIrql;
		   }

		   KIRQL __fastcall raise_irql_to_dpc_level()
		   {
			   return raise_irql(2u);
		   }

		   void __fastcall lower_irql(KIRQL Irql)
		   {
			   __writecr8(Irql);
		   }

		   NTSTATUS read_virtual_memory(
			   unsigned long long directory_table_base,
			   void* target_address,
			   void* out_buffer,
			   PULONG pSizeRead)
		   {

			   if (!directory_table_base)
			   {
				   return STATUS_INVALID_PARAMETER;
			   }

			   if (!target_address)
			   {
				   return STATUS_INVALID_PARAMETER;
			   }

			   if (!out_buffer)
			   {
				   return STATUS_INVALID_PARAMETER;
			   }

			   if (!pSizeRead)
			   {
				   return STATUS_INVALID_PARAMETER;
			   }

			   if (!*pSizeRead)
			   {
				   return STATUS_INVALID_PARAMETER;
			   }

			   BOOLEAN IsDoneRead{};
			   NTSTATUS status{};
			   ULONG SizeLeft{};
			   ULONG OffsetInPage{};
			   ULONG SizeRead{};
			   ULONG SizeLeftInPage{};
			   ULONG Size{};
			   ULONG64 PhysicalAddress{};
			   ULONG64 PageAddress{};
			   ULONG64 page_size{};

			   physical_page_info transfer_page_info{};
			    status = allocate_physical_page(&transfer_page_info, PAGE_SIZE);
			   if (!NT_SUCCESS(status))
			   {
				   return status;
			   }

			 
			   status =  get_phys_page_size(&transfer_page_info, (ULONG64)target_address, &page_size, directory_table_base);
			   if (!NT_SUCCESS(status) || page_size > PAGE_SIZE)
			   {
				    free_physical_page(&transfer_page_info);
				   return STATUS_INVALID_PARAMETER;
			   }

		 
			   OffsetInPage = (ULONG64)target_address & 0xFFF;
			   PageAddress = (ULONG64)target_address & 0xFFFFFFFFFFFFF000u;
			   Size = *pSizeRead;
			   SizeLeft = *pSizeRead;
			   do
			   {
				   IsDoneRead = FALSE;
				   if (SizeLeft >= PAGE_SIZE - OffsetInPage)
				   {
					   SizeLeftInPage = PAGE_SIZE - OffsetInPage;
				   }
				   else
				   {
					   SizeLeftInPage = SizeLeft;
				   }
				   
				   PhysicalAddress = 0;
				   status = get_phys_page_address(&transfer_page_info, directory_table_base, (PVOID)PageAddress, &PhysicalAddress);

				   if (NT_SUCCESS(status) && PhysicalAddress)
				   {
					   if (NT_SUCCESS(read_physical_page(
						   &transfer_page_info,
						   PhysicalAddress + OffsetInPage,
						   out_buffer,
						   SizeLeftInPage)))
					   {
						   SizeRead += SizeLeftInPage;
						   IsDoneRead = TRUE;
					   }
				   }
				   
				    
				   if (!IsDoneRead)
				   {
					   memset(out_buffer, 0, SizeLeftInPage);
				   }
				   out_buffer = (PUCHAR)out_buffer + SizeLeftInPage;
				   PageAddress += OffsetInPage + (ULONG64)SizeLeftInPage;
				   OffsetInPage = 0;
				   SizeLeft -= SizeLeftInPage;
			   } while (SizeLeft && SizeLeft < Size);

			   if (SizeRead)
			   {
				   *pSizeRead = SizeRead;
				   status = STATUS_SUCCESS;
			   }
			   else
			   {
				   status = STATUS_CANT_WAIT;
			   }
				  
			   
		  
			   free_physical_page(&transfer_page_info);
			   return status;
 
		   }

		   NTSTATUS   allocate_physical_page(
			   physical_page_info* physical_page_info,
			   SIZE_T size)
		   {
			   if (!physical_page_info)
			   {
				   return STATUS_INVALID_BUFFER_SIZE;
			   }

			   if (size != 0x1000)
			   {
				   return STATUS_INVALID_BUFFER_SIZE;
			   }


			  

			   unsigned long long base_address = reinterpret_cast<unsigned long long > (MmAllocateMappingAddress(0x1000, 'axe'));
			   if (!base_address)
			   {
				   return STATUS_INSUFFICIENT_RESOURCES;

			   }

			   unsigned  long long pte_address =  get_pte(  base_address ) ;
			   if (!pte_address || !is_va_physical_address_valid( pte_address )) {
				   MmFreeMappingAddress(reinterpret_cast<PVOID> (base_address), 'axe');
				   return STATUS_MEMORY_NOT_ALLOCATED;
			   }

			  physical_page_info->base_address = base_address;
			  physical_page_info->size = 0x1000;
			  physical_page_info->pte_address = pte_address;

			   return STATUS_SUCCESS;
		   }



		   NTSTATUS get_phys_page_address(
			   physical_page_info* transfer_page_info,
			   unsigned long long target_cr3,
			   PVOID page_va,
			   PULONG64 physical_address_out)
		   {

			   if (!physical_address_out)
				   return STATUS_INVALID_PARAMETER;

			   *physical_address_out = 0;

			   ULONG64 cr3 = target_cr3 ? target_cr3 : __readcr3();

			   PAGE_TABLE_INFO page_table_info{};
			   NTSTATUS status = get_page_table_info(transfer_page_info, cr3, (ULONG64)page_va, &page_table_info);
			   if (!NT_SUCCESS(status))
				   return status;

			   ULONG64 page_phys = 0;
			   SIZE_T page_size = 0;

			   switch (page_table_info.PageType)
			   {
			   case 5u:
				   page_phys = ( (page_table_info.Pte >> 12) & 0xFFFFFFFFFFi64) << 12;
				   page_size = 0x1000;
				   break;

			   case 6u:
				   page_phys =( (page_table_info.Pde >> 21) & 0x7FFFFFFF) << 21;
				   page_size = 0x200000;
				   break;

			   case 7u:
				   page_phys =( (page_table_info.Ppe >> 30) & 0x3FFFFF) << 30;
				   page_size = 0x40000000;
				   break;

			   default:
				   return STATUS_INVALID_PAGE_PROTECTION;
			   }

			   if (page_phys != 0)
			   {
				   *physical_address_out = (ULONG64)page_va + page_phys - (~(page_size - 1) & (ULONG64)page_va);
				 
				   return STATUS_SUCCESS;
			   }

			   return STATUS_NO_MORE_ENTRIES; // page_phys 为 0

		   }
		   NTSTATUS __fastcall get_phys_page_size(
			   physical_page_info* transfer_page_info,
			   unsigned long long page_address,
			   size_t* page_size,
			   unsigned long long target_cr3)
		   {

			   NTSTATUS status{};
			   ULONG Flag;
			   ULONG v8;
			   ULONG v9;
			   SIZE_T PageSize;
			   PAGE_TABLE_INFO PageTableInfo{};
			   unsigned long long cr3{ target_cr3 };
			    
			   memset(&PageTableInfo, 0, 0x24);
			   
			   if (get_page_table_info(transfer_page_info,cr3, page_address, &PageTableInfo))
			   {
				   status = 2;
				   if ((PageTableInfo.Pxe & 1) != 0)
				   {
					   if ((PageTableInfo.Ppe & 1) != 0 && ((PageTableInfo.Ppe >> 7) & 1) == 0)
					   {
						   if ((PageTableInfo.Pde & 1) != 0 && ((PageTableInfo.Pde >> 7) & 1) == 0)
							   PageSize = 0x1000;
						   else
							   PageSize = 0x200000;
					   }
					   else
					   {
						   PageSize = 0x40000000;
					   }
				   }
				   else
				   {
					   PageSize = 0x8000000000;
				   }
			   }
			   else
			   {
				   switch (PageTableInfo.PageType)
				   {
				   case 5u:
					   PageSize = 0x1000;
					   Flag = 16;
					   if ((PageTableInfo.Ppe & 1) != 0)
					   {
						   if ((PageTableInfo.Ppe & 0x8000000000000000) != 0i64)
							   Flag = 24;
						   if (((PageTableInfo.Ppe >> 1) & 1) == 0)
							   Flag |= 0x80u;
						   if (((PageTableInfo.Ppe >> 2) & 1) == 0)
							   Flag |= 4u;
						   if ((PageTableInfo.Pde & 1) != 0)
						   {
							   if ((PageTableInfo.Pde & 0x8000000000000000) != 0i64)
								   Flag |= 8u;
							   if (((PageTableInfo.Pde >> 1) & 1) == 0)
								   Flag |= 0x80u;
							   if (((PageTableInfo.Pde >> 2) & 1) == 0)
								   Flag |= 4u;
							   if ((PageTableInfo.Pte & 1) != 0)
							   {
								   if ((PageTableInfo.Pte & 0x8000000000000000) != 0i64)
									   Flag |= 8u;
								   if (((PageTableInfo.Pte >> 1) & 1) == 0)
									   Flag |= 0x80u;
								   if (((PageTableInfo.Pte >> 2) & 1) == 0)
									   Flag |= 4u;
								   if (((PageTableInfo.Pte >> 5) & 1) != 0)
									   Flag |= 0x100u;
								   if (((PageTableInfo.Pte >> 6) & 1) != 0)
									   Flag |= 0x200u;
								   status = Flag | 1;
							   }
							   else
							   {
								   status = Flag | 2;
							   }
						   }
						   else
						   {
							   status = Flag | 2;
						   }
					   }
					   else
					   {
						   status = 18;
					   }
					   break;
				   case 6u:
					   PageSize = 0x200000;
					   v8 = 32;
					   if ((PageTableInfo.Ppe & 1) != 0)
					   {
						   if ((PageTableInfo.Ppe & 0x8000000000000000) != 0i64)
							   v8 = 40;
						   if (((PageTableInfo.Ppe >> 1) & 1) == 0)
							   v8 |= 0x80u;
						   if (((PageTableInfo.Ppe >> 2) & 1) == 0)
							   v8 |= 4u;
						   if ((PageTableInfo.Pde & 1) != 0)
						   {
							   if ((PageTableInfo.Pde & 0x8000000000000000) != 0i64)
								   v8 |= 8u;
							   if (((PageTableInfo.Pde >> 1) & 1) == 0)
								   v8 |= 0x80u;
							   if (((PageTableInfo.Pde >> 2) & 1) == 0)
								   v8 |= 4u;
							   if (((PageTableInfo.Pde >> 5) & 1) != 0)
								   v8 |= 0x100u;
							   if (((PageTableInfo.Pde >> 6) & 1) != 0)
								   v8 |= 0x200u;
							   status = v8 | 1;
						   }
						   else
						   {
							   status = v8 | 2;
						   }
					   }
					   else
					   {
						   status = 34;
					   }
					   break;
				   case 7u:
					   PageSize = 0x40000000;
					   v9 = 64;
					   if ((PageTableInfo.Ppe & 1) != 0)
					   {
						   if ((PageTableInfo.Ppe & 0x8000000000000000) != 0i64)
							   v9 = 72;
						   if (((PageTableInfo.Ppe >> 1) & 1) == 0)
							   v9 |= 0x80u;
						   if (((PageTableInfo.Ppe >> 2) & 1) == 0)
							   v9 |= 4u;
						   if (((PageTableInfo.Ppe >> 5) & 1) != 0)
							   v9 |= 0x100u;
						   if (((PageTableInfo.Ppe >> 6) & 1) != 0)
							   v9 |= 0x200u;
						   status = v9 | 1;
					   }
					   else
					   {
						   status = 66;
					   }
					   break;
				   default:
					   PageSize = 0x1000;
					   break;
				   }
			   }
			   if (page_size)
			   {
				   *page_size = PageSize;
			   }

			   return  status;
		   }


		   NTSTATUS get_page_table_info(
			   physical_page_info* transfer_page_info,
			   ULONG64 cr3,
			   ULONG64 page_address,
			   PAGE_TABLE_INFO* page_table_info_out)
		   {
			   if (!transfer_page_info)
			   {
				   return 22;
			   }
			   memset(page_table_info_out, 0 , sizeof(PAGE_TABLE_INFO));

			   if (read_physical_page(
				   transfer_page_info,
				   (((cr3 >> 12) & 0xFFFFFFFFFFi64) << 12) + 8 * ((page_address >> 39) & 0x1FF),
				   page_table_info_out,
				   8)
				   || (page_table_info_out->Pxe & 1) == 0)
			   {
				   return 262;
			   }

			   if (((page_table_info_out->Pxe >> 12) & 0xFFFFFFFFFFi64) == ((cr3 >> 12) & 0xFFFFFFFFFFi64))
			   {
				   return 266;

			   }
			   if (read_physical_page(
				   transfer_page_info,
				   (((page_table_info_out->Pxe >> 12) & 0xFFFFFFFFFFi64) << 12) + 8 * ((page_address >> 30) & 0x1FF),
				   &page_table_info_out->Ppe,
				   8)
				   || (page_table_info_out->Ppe & 1) == 0)
			   {
				   return 263;
			   }
			   if (((page_table_info_out->Ppe >> 7) & 1) != 0)
			   {
				   page_table_info_out->PageType = 7;                // 1GB large page
				   return 0;
			   }

			   else if (!read_physical_page(
				   transfer_page_info,
				   (((page_table_info_out->Ppe >> 12) & 0xFFFFFFFFFFi64) << 12) + 8 * ((page_address >> 21) & 0x1FF),
				   &page_table_info_out->Pde,
				   8)
				   && (page_table_info_out->Pde & 1) != 0)
			   {
				   if (((page_table_info_out->Pde >> 7) & 1) != 0)
				   {
					   page_table_info_out->PageType = 6;              // 2MB large page
					   return 0;
				   }
				   else if (!read_physical_page(
					   transfer_page_info,
					   (((page_table_info_out->Pde >> 12) & 0xFFFFFFFFFFi64) << 12) + 8 * ((page_address >> 12) & 0x1FF),
					   &page_table_info_out->Pte,
					   8)
					   && (page_table_info_out->Pte & 1) != 0)
				   {
					   page_table_info_out->PageType = 5;              // 4KB Page
					   return 0;
				   }
				   else
				   {
					   return 265;
				   }
			   }
			   else
			   {
				   return 264;
			   }
		   }



		   NTSTATUS read_physical_page(
			   physical_page_info* transfer_page_info,
			   ULONG64 phys_page_base,
			   PVOID buffer,
			   SIZE_T size)
		   {
			   if (!phys_page_base || !buffer || !size)
				   return STATUS_INVALID_PARAMETER;

			   if (!transfer_page_info ||
				   !transfer_page_info->base_address ||
				   !transfer_page_info->pte_address)
				   return STATUS_INVALID_PARAMETER_1;

			   if (size > transfer_page_info->size)
				   return STATUS_BUFFER_TOO_SMALL;

			   // 跨页不支持
			   if ((phys_page_base >> 12) != ((phys_page_base + size - 1) >> 12))
				   return STATUS_INVALID_PARAMETER_MIX;

			   if (!is_phys_page_in_range(phys_page_base, size))
				   return STATUS_INVALID_ADDRESS;

			   ULONG old_irql = 0;
			   bool raised_irql = false;

			   // 如果当前IRQL小于DPC_LEVEL，提升到DPC
			   if (KeGetCurrentIrql() < DISPATCH_LEVEL) {
				   old_irql = raise_irql_to_dpc_level();
				   raised_irql = true;
			   }

			  unsigned  long long  pte_addr = transfer_page_info->pte_address;
			   if (!is_va_physical_address_valid( pte_addr )) {
				   if (raised_irql) lower_irql(old_irql);
				   return STATUS_INVALID_ADDRESS;
			   }

			   // 保存原始PTE并挂上目标物理页
			   ULONG64* pte_entry = reinterpret_cast<ULONG64*>(pte_addr);
			   ULONG64 old_pte = *pte_entry;

			   *pte_entry =
				   ((phys_page_base >> 12) & 0xFFFFFFFFFFULL) << 12 |
				   (*pte_entry & 0xFFF0000000000EF8ULL) |
				   0x103ULL; // Present | ReadWrite | NX 清除

			   __invlpg((void*) transfer_page_info->base_address);

			   memmove(
				   reinterpret_cast<char*>(buffer),
				   reinterpret_cast<char*>(transfer_page_info->base_address) + (phys_page_base & 0xFFF),
				   size
			   );

			   // 恢复原始PTE
			   *pte_entry = old_pte;

			   if (raised_irql)
				   lower_irql(old_irql);

			   return STATUS_SUCCESS;
		   }

		   bool is_phys_page_in_range(unsigned  long long phys_page_base, size_t size)
		   {
			   if (!g_physical_memory_ranges)
			   {
				   // 仅在 IRQL = PASSIVE_LEVEL 时调用 MmGetPhysicalMemoryRanges
				   if (KeGetCurrentIrql())
					   return false;

				   g_physical_memory_ranges = MmGetPhysicalMemoryRanges();
			   }

			   if (!g_physical_memory_ranges)
				   return false;

			   const ULONG64 phys_page_end = phys_page_base + size - 1;

			   for (int i = 0;; ++i)
			   {
				   PHYSICAL_MEMORY_RANGE physical_memory_range = g_physical_memory_ranges[i];

				   // 遇到终止项
				   if (physical_memory_range.BaseAddress.QuadPart == 0 ||
					   physical_memory_range.NumberOfBytes.QuadPart == 0)
				   {
					   break;
				   }

			 

				   // 判断地址是否完全落在当前物理内存区间中
				   if (phys_page_base >= (ULONG64)physical_memory_range.BaseAddress.QuadPart
					   && phys_page_base < (ULONG64)physical_memory_range.NumberOfBytes.QuadPart + physical_memory_range.BaseAddress.QuadPart
					   && phys_page_end >= (ULONG64)physical_memory_range.BaseAddress.QuadPart
					   && phys_page_end < (ULONG64)physical_memory_range.NumberOfBytes.QuadPart + physical_memory_range.BaseAddress.QuadPart)
				   {
					   return true;
				   }
			   }

			   return false;
		   }


		   void __fastcall free_physical_page(
			   physical_page_info* page_info)
		   {
			   if (page_info)
			   {
				   if (page_info->base_address)
				   {
					   MmFreeMappingAddress((PVOID)page_info->base_address, 'axe');
					   memset(page_info, 0i64, sizeof(physical_page_info));
				   }
			   }
		   }

	}
}