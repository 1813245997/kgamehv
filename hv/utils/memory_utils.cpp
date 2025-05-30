#include "global_defs.h"
#include "memory_utils.h"

#define PTE_MASK (0x7FFFFFFFF8)

namespace utils
{
	namespace memory
	{



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
			g_pde_base = get_pte(g_pte_base);
			g_ppe_base = get_pte(g_pde_base);
			g_pxe_base = get_pte(g_ppe_base);

			return STATUS_SUCCESS;
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
				wchar_t wa_MmGetVirtualForPhysical[] = { 0xE3AE, 0xE38E, 0xE3A4, 0xE386, 0xE397, 0xE3B5, 0xE38A, 0xE391, 0xE397, 0xE396, 0xE382, 0xE38F, 0xE3A5, 0xE38C, 0xE391, 0xE3B3, 0xE38B, 0xE39A, 0xE390, 0xE38A, 0xE380, 0xE382, 0xE38F, 0xE3E3, 0xE3E3 };

				for (int i = 0; i < 25; i++)
				{
					wa_MmGetVirtualForPhysical[i] ^= 0x6D6D;
					wa_MmGetVirtualForPhysical[i] ^= 0x8E8E;
				};

				UNICODE_STRING unFuncNameMmGetVirtualForPhysical = { 0 };
				RtlInitUnicodeString(&unFuncNameMmGetVirtualForPhysical, wa_MmGetVirtualForPhysical);
				PUCHAR funcMmGetVirtualForPhysical = (PUCHAR)MmGetSystemRoutineAddress(&unFuncNameMmGetVirtualForPhysical);
				g_pte_base = *(PULONG64)(funcMmGetVirtualForPhysical + 0x22);
			}


			return STATUS_SUCCESS;
		}

		unsigned long long get_pte_base()
		{
			if (!g_pte_base)
			{
				initialize_pte_base();
			}

			return g_pte_base;
		}

		unsigned long long get_pml4(unsigned long long virtual_address)
		{
			unsigned long long pdpte = get_pdpte(virtual_address);
			unsigned long long pte_base = get_pte_base();
			return ((pdpte >> 9) & PTE_MASK) + pte_base;
		}

		unsigned long long get_pdpte(unsigned long long virtual_address)
		{
			unsigned long long pde = get_pde(virtual_address);
			unsigned long long pte_base = get_pte_base();
			return ((pde >> 9) & PTE_MASK) + pte_base;
		}

		unsigned long long get_pde(unsigned long long virtual_address)
		{
			ULONG64 pte = get_pte(virtual_address);

			ULONG64 pteBase = get_pte_base();

			return ((pte >> 9) & PTE_MASK) + pteBase;
		}

		unsigned long long get_pte(unsigned long long virtual_address)
		{
			unsigned long long pte_base = get_pte_base();
			return ((virtual_address >> 9) & PTE_MASK) + pte_base;
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





	}
}