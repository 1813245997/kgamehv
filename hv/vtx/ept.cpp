#pragma warning( disable : 4201 4244)
#include "../utils/global_defs.h"
#include "ept.h"


namespace ept
{
	/// <summary>
	/// Build mtrr map to track physical memory type
	/// </summary>
	void build_mtrr_map()
	{
		__mtrr_cap_reg mtrr_cap = { 0 };
		__mtrr_physbase_reg current_phys_base = { 0 };
		__mtrr_physmask_reg current_phys_mask = { 0 };
		__mtrr_def_type mtrr_def_type = { 0 };
		__mtrr_range_descriptor* descriptor;

		//
		// The memory type range registers (MTRRs) provide a mechanism for associating the memory types (see Section
		// 11.3, 揗ethods of Caching Available? with physical - address ranges in system memory.They allow the processor to
		// optimize operations for different types of memory such as RAM, ROM, frame - buffer memory, and memory - mapped
		// I/O devices.They also simplify system hardware design by eliminating the memory control pins used for this func -
		// tion on earlier IA - 32 processors and the external logic needed to drive them.
		//

		mtrr_cap.all = __readmsr(IA32_MTRRCAP);
		mtrr_def_type.all = __readmsr(IA32_MTRR_DEF_TYPE);

		if (mtrr_def_type.mtrr_enabled == false)
		{
			g_vmm_context->mtrr_info.default_memory_type = MEMORY_TYPE_UNCACHEABLE;
			return;
		}

		g_vmm_context->mtrr_info.default_memory_type = mtrr_def_type.memory_type;

		if (mtrr_cap.smrr_support == true)
		{
			current_phys_base.all = __readmsr(IA32_SMRR_PHYSBASE);
			current_phys_mask.all = __readmsr(IA32_SMRR_PHYSMASK);

			if (current_phys_mask.valid && current_phys_base.type != mtrr_def_type.memory_type)
			{
				descriptor = &g_vmm_context->mtrr_info.memory_range[g_vmm_context->mtrr_info.enabled_memory_ranges++];
				descriptor->physcial_base_address = current_phys_base.physbase << PAGE_SHIFT;

				unsigned long bits_in_mask = 0;
				_BitScanForward64(&bits_in_mask, current_phys_mask.physmask << PAGE_SHIFT);

				descriptor->physcial_end_address = descriptor->physcial_base_address + ((1ULL << bits_in_mask) - 1ULL);
				descriptor->memory_type = (unsigned __int8)current_phys_base.type;
				descriptor->fixed_range = false;
			}
		}

		if (mtrr_cap.fixed_range_support == true && mtrr_def_type.fixed_range_mtrr_enabled)
		{
			constexpr auto k64_base = 0x0;
			constexpr auto k64_size = 0x10000;
			constexpr auto k16_base = 0x80000;
			constexpr auto k16_size = 0x4000;
			constexpr auto k4_base = 0xC0000;
			constexpr auto k4_size = 0x1000;

			__mtrr_fixed_range_type k64_types = { __readmsr(IA32_MTRR_FIX64K_00000) };

			for (unsigned int i = 0; i < 8; i++)
			{
				descriptor = &g_vmm_context->mtrr_info.memory_range[g_vmm_context->mtrr_info.enabled_memory_ranges++];
				descriptor->memory_type = k64_types.types[i];
				descriptor->physcial_base_address = k64_base + (k64_size * i);
				descriptor->physcial_end_address = k64_base + (k64_size * i) + (k64_size - 1);
				descriptor->fixed_range = true;
			}

			for (unsigned int i = 0; i < 2; i++)
			{
				__mtrr_fixed_range_type k16_types = { __readmsr(IA32_MTRR_FIX16K_80000 + i) };

				for (unsigned int j = 0; j < 8; j++)
				{
					descriptor = &g_vmm_context->mtrr_info.memory_range[g_vmm_context->mtrr_info.enabled_memory_ranges++];
					descriptor->memory_type = k16_types.types[j];
					descriptor->physcial_base_address = (k16_base + (i * k16_size * 8)) + (k16_size * j);
					descriptor->physcial_end_address = (k16_base + (i * k16_size * 8)) + (k16_size * j) + (k16_size - 1);
					descriptor->fixed_range = true;
				}
			}

			for (unsigned int i = 0; i < 8; i++)
			{
				__mtrr_fixed_range_type k4_types = { __readmsr(IA32_MTRR_FIX4K_C0000 + i) };

				for (unsigned int j = 0; j < 8; j++)
				{
					descriptor = &g_vmm_context->mtrr_info.memory_range[g_vmm_context->mtrr_info.enabled_memory_ranges++];
					descriptor->memory_type = k4_types.types[j];
					descriptor->physcial_base_address = (k4_base + (i * k4_size * 8)) + (k4_size * j);
					descriptor->physcial_end_address = (k4_base + (i * k4_size * 8)) + (k4_size * j) + (k4_size - 1);
					descriptor->fixed_range = true;
				}
			}
		}

		//
		// Indicates the number of variable ranges
		// implemented on the processor.
		for (int i = 0; i < mtrr_cap.range_register_number; i++)
		{
			//
			// The first entry in each pair (IA32_MTRR_PHYSBASEn) defines the base address and memory type for the range;
			// the second entry(IA32_MTRR_PHYSMASKn) contains a mask used to determine the address range.The 搉?suffix
			// is in the range 0 through m? and identifies a specific register pair.
			//
			current_phys_base.all = __readmsr(IA32_MTRR_PHYSBASE0 + (i * 2));
			current_phys_mask.all = __readmsr(IA32_MTRR_PHYSMASK0 + (i * 2));

			//
			// If range is enabled
			if (current_phys_mask.valid && current_phys_base.type != mtrr_def_type.memory_type)
			{
				descriptor = &g_vmm_context->mtrr_info.memory_range[g_vmm_context->mtrr_info.enabled_memory_ranges++];

				//
				// Calculate base address, physbase is truncated by 12 bits so we have to left shift it by 12
				//
				descriptor->physcial_base_address = current_phys_base.physbase << PAGE_SHIFT;

				//
				// Index of first bit set to one determines how much do we have to bit shift to get size of range
				// physmask is truncated by 12 bits so we have to left shift it by 12
				//
				unsigned long bits_in_mask = 0;
				_BitScanForward64(&bits_in_mask, current_phys_mask.physmask << PAGE_SHIFT);

				//
				// Calculate the end of range specified by mtrr
				//
				descriptor->physcial_end_address = descriptor->physcial_base_address + ((1ULL << bits_in_mask) - 1ULL);

				//
				// Get memory type of range
				//
				descriptor->memory_type = (unsigned __int8)current_phys_base.type;
				descriptor->fixed_range = false;
			}
		}
	}

	/// <summary>
	/// Get page cache memory type
	/// </summary>
	/// <param name="pfn"></param>
	/// <param name="is_large_page"></param>
	/// <returns></returns>
	unsigned __int8 get_memory_type(unsigned __int64 pfn, bool is_large_page)
	{
		unsigned __int64 page_start_address = is_large_page == true ? pfn * LARGE_PAGE_SIZE : pfn * PAGE_SIZE;
		unsigned __int64 page_end_address = is_large_page == true ? (pfn * LARGE_PAGE_SIZE) + (LARGE_PAGE_SIZE - 1) : (pfn * PAGE_SIZE) + (PAGE_SIZE - 1);
		unsigned __int8 memory_type = g_vmm_context->mtrr_info.default_memory_type;

		for (unsigned int i = 0; i < g_vmm_context->mtrr_info.enabled_memory_ranges; i++)
		{
			if (page_start_address >= g_vmm_context->mtrr_info.memory_range[i].physcial_base_address &&
				page_end_address <= g_vmm_context->mtrr_info.memory_range[i].physcial_end_address)
			{
				memory_type = g_vmm_context->mtrr_info.memory_range[i].memory_type;

				if (g_vmm_context->mtrr_info.memory_range[i].fixed_range == true)
					break;

				if (memory_type == MEMORY_TYPE_UNCACHEABLE)
					break;
			}
		}

		return memory_type;
	}

	/// <summary>
	/// Check if potential large page doesn't land on two or more different cache memory types
	/// </summary>
	/// <param name="pfn"></param>
	/// <returns></returns>
	bool is_valid_for_large_page(unsigned __int64 pfn)
	{
		unsigned __int64 page_start_address = pfn * LARGE_PAGE_SIZE;
		unsigned __int64 page_end_address = (pfn * LARGE_PAGE_SIZE) + (LARGE_PAGE_SIZE - 1);

		for (unsigned int i = 0; i < g_vmm_context->mtrr_info.enabled_memory_ranges; i++)
		{
			if (page_start_address <= g_vmm_context->mtrr_info.memory_range[i].physcial_end_address &&
				page_end_address > g_vmm_context->mtrr_info.memory_range[i].physcial_end_address)
				return false;

			else if (page_start_address < g_vmm_context->mtrr_info.memory_range[i].physcial_base_address &&
					 page_end_address >= g_vmm_context->mtrr_info.memory_range[i].physcial_base_address)
				return false;
		}

		return true;
	}

	/// <summary> 
	/// Setup page memory type
	/// </summary>
	/// <param name="entry"> Pointer to pml2 entry </param>
	/// <param name="pfn"> Page frame number </param>
	bool setup_pml2_entry(__ept_state& ept_state, __ept_pde& entry, unsigned __int64 pfn)
	{
		entry.page_directory_entry.physical_address = pfn;
		
		if (is_valid_for_large_page(pfn) == true)
		{
			entry.page_directory_entry.memory_type = get_memory_type(pfn, true);
			return true;
		}

		else
		{
			void* split_buffer = pool_manager::request_pool<void*>(pool_manager::INTENTION_SPLIT_PML2, true, sizeof(__ept_dynamic_split));
			if (split_buffer == nullptr)
			{
				LogError("Failed to allocate split buffer");
				return false;
			}

			return split_pml2(ept_state, split_buffer, pfn * LARGE_PAGE_SIZE);
		}
	}

	/// <summary>
	/// Create ept page table
	/// </summary>
	/// <returns> status </returns>
	bool create_ept_page_table(__ept_state& ept_state)
	{
		PHYSICAL_ADDRESS max_size;
		max_size.QuadPart = MAXULONG64;

		ept_state.ept_page_table = allocate_contignous_memory<__vmm_ept_page_table>();
		if (ept_state.ept_page_table == NULL)
		{
			LogError("Failed to allocate memory for PageTable");
			return false;
		}

		__vmm_ept_page_table* page_table = ept_state.ept_page_table;
		RtlSecureZeroMemory(page_table, sizeof(__vmm_ept_page_table));

		//
		// Set all pages as rwx to prevent unwanted ept violation
		//
		page_table->pml4[0].physical_address = GET_PFN(MmGetPhysicalAddress(&page_table->pml3[0]).QuadPart);
		page_table->pml4[0].read = 1;
		page_table->pml4[0].write = 1;
		page_table->pml4[0].execute = 1;

		__ept_pdpte pdpte_template = { 0 };

		pdpte_template.read = 1;
		pdpte_template.write = 1;
		pdpte_template.execute = 1;

		__stosq((unsigned __int64*)&page_table->pml3[0], pdpte_template.all, 512);

		for (int i = 0; i < 512; i++)
			page_table->pml3[i].physical_address = GET_PFN(MmGetPhysicalAddress(&page_table->pml2[i][0]).QuadPart);

		__ept_pde pde_template = { 0 };

		pde_template.page_directory_entry.read = 1;
		pde_template.page_directory_entry.write = 1;
		pde_template.page_directory_entry.execute = 1;

		pde_template.page_directory_entry.large_page = 1;

		__stosq((unsigned __int64*)&page_table->pml2[0], pde_template.all, 512 * 512);

		for (int i = 0; i < 512; i++)
			for (int j = 0; j < 512; j++)
				if(setup_pml2_entry(ept_state, page_table->pml2[i][j], (i * 512) + j) == false)
					return false;

		return true;
	}

	/// <summary>
	/// Initialize ept structure
	/// </summary>
	/// <returns></returns>
	bool initialize(__ept_state& ept_state)
	{
		__eptp* ept_pointer = allocate_pool<__eptp*>(PAGE_SIZE);
		if (ept_pointer == NULL)
			return false;

		RtlSecureZeroMemory(ept_pointer, PAGE_SIZE);

		if (create_ept_page_table(ept_state) == false)
			return false;

		ept_pointer->memory_type = g_vmm_context->mtrr_info.default_memory_type;

		// Indicates 4 level paging
		ept_pointer->page_walk_length = 3;

		ept_pointer->pml4_address = GET_PFN(MmGetPhysicalAddress(&ept_state.ept_page_table->pml4).QuadPart);

		ept_state.ept_pointer = ept_pointer;

		return true;
	}

	/// <summary>
	/// Get pml2 entry
	/// </summary>
	/// <param name="physical_address"></param>
	/// <returns> pointer to pml2 </returns>
	__ept_pde* get_pml2_entry(__ept_state& ept_state, unsigned __int64 physical_address)
	{
		unsigned __int64 pml4_index = MASK_EPT_PML4_INDEX(physical_address);
		unsigned __int64 pml3_index = MASK_EPT_PML3_INDEX(physical_address);
		unsigned __int64 pml2_index = MASK_EPT_PML2_INDEX(physical_address);

		if (pml4_index > 0)
		{
			LogError("Address above 512GB is invalid");
			return nullptr;
		}

		return &ept_state.ept_page_table->pml2[pml3_index][pml2_index];
	}

	/// <summary>
	/// Get pml1 entry
	/// </summary>
	/// <param name="physical_address"></param>
	/// <returns></returns>
	__ept_pte* get_pml1_entry(__ept_state& ept_state, unsigned __int64 physical_address)
	{
		unsigned __int64 pml4_index = MASK_EPT_PML4_INDEX(physical_address);
		unsigned __int64 pml3_index = MASK_EPT_PML3_INDEX(physical_address);
		unsigned __int64 pml2_index = MASK_EPT_PML2_INDEX(physical_address);

		if (pml4_index > 0)
		{
			LogError("Address above 512GB is invalid");
			return nullptr;
		}

		__ept_pde* pml2 = &ept_state.ept_page_table->pml2[pml3_index][pml2_index];
		if (pml2->page_directory_entry.large_page == 1)
		{
			return nullptr;
		}

		PHYSICAL_ADDRESS pfn;
		pfn.QuadPart = pml2->large_page.physical_address << PAGE_SHIFT;
		__ept_pte* pml1 = (__ept_pte*)MmGetVirtualForPhysical(pfn);

		if (pml1 == nullptr)
		{
			return nullptr;
		}

		pml1 = &pml1[MASK_EPT_PML1_INDEX(physical_address)];
		return pml1;
	}

	/// <summary>
	/// Split pml2 into 512 pml1 entries (From one 2MB page to 512 4KB pages)
	/// </summary>
	/// <param name="pre_allocated_buffer"> Pre allocated buffer for split </param>
	/// <param name="physical_address"></param>
	/// <returns> status </returns>
	bool split_pml2(__ept_state& ept_state, void* pre_allocated_buffer, unsigned __int64 physical_address)
	{
		__ept_pde* entry = get_pml2_entry(ept_state, physical_address);
		if (entry == NULL)
		{
			LogError("Invalid address passed");
			return false;
		}

		__ept_dynamic_split* new_split = (__ept_dynamic_split*)pre_allocated_buffer;
		RtlSecureZeroMemory(new_split, sizeof(__ept_dynamic_split));

		//
		// Set all pages as rwx to prevent unwanted ept violation
		//
		new_split->entry = entry;

		__ept_pte entry_template = { 0 };
		entry_template.read = 1;
		entry_template.write = 1;
		entry_template.execute = 1;
		entry_template.ept_memory_type = entry->page_directory_entry.memory_type;
		entry_template.ignore_pat = entry->page_directory_entry.ignore_pat;
		entry_template.suppress_ve = entry->page_directory_entry.suppressve;

		__stosq((unsigned __int64*)&new_split->pml1[0], entry_template.all, 512);
		for (int i = 0; i < 512; i++)
		{
			unsigned __int64 pfn = ((entry->page_directory_entry.physical_address * LARGE_PAGE_SIZE) >> PAGE_SHIFT) + i;
			new_split->pml1[i].physical_address = pfn;
			new_split->pml1[i].ept_memory_type = get_memory_type(pfn, false);
		}

		__ept_pde new_entry = { 0 };
		new_entry.large_page.read = 1;
		new_entry.large_page.write = 1;
		new_entry.large_page.execute = 1;

		new_entry.large_page.physical_address = MmGetPhysicalAddress(&new_split->pml1[0]).QuadPart >> PAGE_SHIFT;

		RtlCopyMemory(entry, &new_entry, sizeof(new_entry));

		return true;
	}

	bool is_page_splitted(__ept_state& ept_state, unsigned __int64 physical_address)
	{
		__ept_pde* entry = get_pml2_entry(ept_state, physical_address);
		return !entry->page_directory_entry.large_page;
	}

	/// <summary>
	/// Swap physcial pages and invalidate tlb
	/// </summary>
	/// <param name="entry_address"> Pointer to page table entry which we want to change </param>
	/// <param name="entry_value"> Pointer to page table entry which we want use to change </param>
	/// <param name="invalidation_type"> Specifiy if we want to invalidate single context or all contexts  </param>
	void swap_pml1_and_invalidate_tlb(__ept_state& ept_state, __ept_pte* entry_address, __ept_pte entry_value, invept_type invalidation_type)
	{
		// Set the value
		entry_address->all = entry_value.all;

		// Invalidate the cache
		if (invalidation_type == invept_type::invept_single_context)
		{
			invept_single_context_address(ept_state.ept_pointer->all);
		}
		else
		{
			invept_all_contexts();
		}
	}

	void write_int1(uint8_t* address)
	{
		*address = 0xF1; // INT1 (ICEBP)
	}



	bool hook_page_pfn_ept(
		__ept_state& ept_state,
		unsigned __int64 orig_page_pfn,
		unsigned __int64 exec_page_pfn
	)
	{
		if (orig_page_pfn == 0 || exec_page_pfn == 0)
		{
			LogError("Invalid PFN parameters");
			return false;
		}

		// 计算目标页面的物理地址
		unsigned __int64 target_physical_address = orig_page_pfn << PAGE_SHIFT;

		// 如果没有 split，则执行 PML2 拆页

		if (is_page_splitted(ept_state, target_physical_address) == false)
		{
			void* split_buffer = pool_manager::request_pool<void*>(
				pool_manager::INTENTION_SPLIT_PML2, true, sizeof(__ept_dynamic_split)
				);
			if (!split_buffer)
			{
				LogError("Failed to allocate split buffer");
				return false;
			}

			if (!split_pml2(ept_state, split_buffer, target_physical_address))
			{
				pool_manager::release_pool(split_buffer);
				LogError("PML2 Split failed");
				return false;
			}
		}

		// 获取 PML1 页表项
		__ept_pte* target_page = get_pml1_entry(ept_state, target_physical_address);
		if (!target_page)
		{
			LogError("Failed to get PML1 entry");
			return false;
		}

		// 创建 Hook 信息结构
		auto* hook_info = pool_manager::request_pool<__ept_hooked_page_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, true, sizeof(__ept_hooked_page_info));
		if (!hook_info)
		{
			LogError("Failed to allocate hooked page info");
			return false;
		}

		// 填充 hook 信息
		hook_info->pfn_of_hooked_page = orig_page_pfn;
		hook_info->pfn_of_fake_page = exec_page_pfn;
		hook_info->entry_address = target_page;
	
	 
		hook_info->entry_address->execute = 0;
		hook_info->entry_address->read = 1;
		hook_info->entry_address->write = 1;

		hook_info->original_entry = *target_page;
		hook_info->changed_entry = *target_page;

	 
		hook_info->changed_entry.read = 0;
		hook_info->changed_entry.write = 0;
		hook_info->changed_entry.execute = 1;
		 
		hook_info->changed_entry.physical_address = exec_page_pfn;
		 
		 
		// 插入到 hook 链表中
		InsertHeadList(&ept_state.hooked_page_list, &hook_info->hooked_page_list);
		invept_single_context_address(ept_state.ept_pointer->all);

		invept_all_contexts();
		return true;
	}
  

	bool unhook_page_pfn_ept(__ept_state& ept_state, unsigned __int64 orig_page_pfn)
	{
		if (orig_page_pfn == 0)
		{
			return false;
		}

		// 遍历 hooked_page_list 找到目标 PFN 对应的 hook 信息
		for (PLIST_ENTRY entry = ept_state.hooked_page_list.Flink; entry != &ept_state.hooked_page_list; entry = entry->Flink)
		{
			auto* hook_info = CONTAINING_RECORD(entry, __ept_hooked_page_info, hooked_page_list);

			if (hook_info->pfn_of_hooked_page != orig_page_pfn)
			{
				continue;

			}
			 
			 hook_info->original_entry.read = 1;
			 hook_info->original_entry.write = 1;
			 hook_info->original_entry.execute = 1;
			 hook_info->entry_address->all = hook_info->original_entry.all;

			 invept_all_contexts();
			 
			RemoveEntryList(entry);
			pool_manager::release_pool(hook_info);
			return true;
			 
		}

		 
		return false;
	}
 
 
 

}