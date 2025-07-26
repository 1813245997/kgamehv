#pragma warning( disable : 4201 4244)
#include "../utils/global_defs.h"
#include "../ASM\vm_context.h"
#include <ntddk.h>
#include <intrin.h>
#include <stdlib.h>
#include "common.h"
#include "msr.h"
#include "vmcs_encodings.h"
#include "ept.h"
 
#include "hypervisor_routines.h"
#include "mtrr.h"
#include "allocators.h"
#include "spinlock.h"


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

	void write_int3(uint8_t* address)
	{
		*address = 0xCC; // INT3
	}

	bool hook_instruction_memory_int1(__ept_hooked_function_info* hooked_function_info, void* target_function, unsigned __int64 page_offset)
	{
		unsigned __int64 hooked_instructions_size = LDE(target_function, 64);

		hooked_function_info->hook_size = hooked_instructions_size;

		// Copy overwritten instructions to trampoline buffer
		RtlCopyMemory(hooked_function_info->trampoline_va, target_function, hooked_instructions_size);
		RtlCopyMemory(hooked_function_info->original_instructions_backup, target_function, hooked_instructions_size);
		// Add the absolute jump back to the original function.
		hook_write_absolute_jump(&hooked_function_info->trampoline_va[hooked_instructions_size], (unsigned __int64)target_function + hooked_instructions_size);

		// Write icebp instruction to our shadow page memory to trigger vmexit.
		write_int1(&hooked_function_info->fake_page_contents[page_offset]);



		return true;
	}

	bool hook_instruction_memory_int3(__ept_hooked_function_info* hooked_function_info, void* target_function,  unsigned __int64 page_offset)
	{
		bool is_user = utils::memory::is_user_address(target_function);

		bool is_ac = false;
		if (is_user)
		{
			is_ac = is_ac_flag_set();
			if (!is_ac)
				_setac();  // 设置 AC 标志
		}

		unsigned __int64 hooked_instructions_size = LDE(target_function, 64);
		hooked_function_info->hook_size = hooked_instructions_size;

		RtlCopyMemory(hooked_function_info->original_instructions_backup, target_function, hooked_instructions_size);

		if (hooked_function_info->trampoline_va)
		{
			RtlCopyMemory(hooked_function_info->trampoline_va, target_function, hooked_instructions_size);

			hook_write_absolute_jump_int3(
				&hooked_function_info->trampoline_va[hooked_instructions_size],
				(unsigned __int64)target_function + hooked_instructions_size
			);
		}

		if (is_user && !is_ac)
			_clearac();  // 恢复原始状态

		write_int3(&hooked_function_info->fake_page_contents[page_offset]);

		return true;
	}

	void hook_write_absolute_jump(unsigned __int8* target_buffer, unsigned __int64 destination_address)
	{
		// push lower 32 bits of destination address	
		target_buffer[0] = 0x68;
		*((unsigned __int32*)&target_buffer[1]) = (unsigned __int32)destination_address;

		// mov dword ptr [rsp + 4]
		target_buffer[5] = 0xc7;
		target_buffer[6] = 0x44;
		target_buffer[7] = 0x24;
		target_buffer[8] = 0x04;

		// higher 32 bits of destination address	
		*((unsigned __int32*)&target_buffer[9]) = (unsigned __int32)(destination_address >> 32);

		// ret
		target_buffer[13] = 0xc3;

	}
 
	void  hook_write_absolute_jump_int3(unsigned __int8* target_buffer, unsigned __int64 destination_address)
	{
		PEPROCESS process = utils::internal_functions::pfn_ps_get_current_process();
		bool is_64_bit = utils::process_utils::is_process_64_bit(process);
		if (is_64_bit)
		{
			// push lower 32 bits of destination address	
			target_buffer[0] = 0x68;
			*((unsigned __int32*)&target_buffer[1]) = (unsigned __int32)destination_address;

			// mov dword ptr [rsp + 4]
			target_buffer[5] = 0xc7;
			target_buffer[6] = 0x44;
			target_buffer[7] = 0x24;
			target_buffer[8] = 0x04;

			// higher 32 bits of destination address	
			*((unsigned __int32*)&target_buffer[9]) = (unsigned __int32)(destination_address >> 32);

			// ret
			target_buffer[13] = 0xc3;
		}
		else
		{
			// push imm32
			target_buffer[0] = 0x68;
			*((unsigned __int32*)&target_buffer[1]) = (unsigned __int32)destination_address;

			// ret
			target_buffer[5] = 0xC3;
		}
	}

	 


	bool is_page_splitted(__ept_state& ept_state, unsigned __int64 physical_address)
	{
		__ept_pde* entry = get_pml2_entry(ept_state, physical_address);
		return !entry->page_directory_entry.large_page;
	}

	/// <summary>
	/// Perfrom a hook
	/// </summary>
	/// <param name="target_address" > Address of function which we want to hook </param>
	/// <param name="hook_function"> Address of hooked version of function which we are hooking </param>
	/// <param name="origin_function"> Address used to call original function </param>
	/// <returns></returns>
	bool hook_kernel_function(_In_  __ept_state& ept_state, _In_  void* target_function, _In_  void* new_function, _Out_   void** origin_function)
	{
		 
		if (!target_function)
		{
			return false;
		}

		if (!new_function)
		{
			return false;
		}

		if (!origin_function)
		{
			return false;
		}

		*origin_function = nullptr;


		unsigned __int64 physical_address = utils::internal_functions::pfn_mm_get_physical_address(target_function).QuadPart;

		//
		// Check if function exist in physical memory
		//
		if (physical_address == NULL)
		{
			LogError("Requested virtual memory doesn't exist in physical one");
			return false;
		}


		unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET((unsigned __int64)target_function);

		//
		// Check if page isn't already hooked
		//
		PLIST_ENTRY current = &ept_state.hooked_page_list;
		while (&ept_state.hooked_page_list != current->Flink)
		{
			current = current->Flink;
			__ept_hooked_page_info* hooked_page_info = CONTAINING_RECORD(current, __ept_hooked_page_info, hooked_page_list);

			if (hooked_page_info->pfn_of_hooked_page == GET_PFN(physical_address))
			{
				LogInfo("Page already hooked");

				__ept_hooked_function_info* hooked_function_info = pool_manager::request_pool<__ept_hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(__ept_hooked_function_info));
				if (hooked_function_info == nullptr)
				{
					LogError("There is no pre-allocated pool for hooked function struct");
					return false;
				}

				hooked_function_info->trampoline_va = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_EXEC_TRAMPOLINE, TRUE, 100);
				if (hooked_function_info->trampoline_va == nullptr)
				{
					pool_manager::release_pool(hooked_function_info);
					LogError("There is no pre-allocated pool for trampoline");
					return false;
				}
				hooked_function_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_INSTRUCTION, TRUE, 100);
				if (hooked_function_info->original_instructions_backup == nullptr)
				{
					pool_manager::release_pool(hooked_function_info->trampoline_va);
					pool_manager::release_pool(hooked_function_info);
					LogError("There is no pre-allocated pool for trampoline");
					return false;
				}

				hooked_function_info->new_handler_va = new_function;
				hooked_function_info->original_va = target_function;
				hooked_function_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
				
				hooked_function_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(new_function).QuadPart;
				hooked_function_info->original_pa = physical_address;
				hooked_function_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
				hooked_function_info->fake_page_contents = hooked_page_info->fake_page_contents;
				hooked_function_info->type = hook_type::hook_kernel_function_redirect;
				 
				if (hook_instruction_memory_int1(hooked_function_info, target_function, page_offset) == false)
				{
					if (hooked_function_info->trampoline_va != nullptr)
					{
						pool_manager::release_pool(hooked_function_info->trampoline_va);
					}
					if (hooked_function_info->original_instructions_backup != nullptr)
					{
						pool_manager::release_pool(hooked_function_info->original_instructions_backup);
					}

					pool_manager::release_pool(hooked_function_info);
					LogError("Hook failed");
					return false;
				}
				++hooked_page_info->ref_count;
				if (origin_function)
					*origin_function = hooked_function_info->trampoline_va;
				// Track all hooked functions within page
				InsertHeadList(&hooked_page_info->hooked_functions_list, &hooked_function_info->hooked_function_list);

				return true;
			}
		}

		if (is_page_splitted(ept_state, physical_address) == false)
		{
			void* split_buffer = pool_manager::request_pool<void*>(pool_manager::INTENTION_SPLIT_PML2, true, sizeof(__ept_dynamic_split));
			if (split_buffer == nullptr)
			{
				LogError("There is no preallocated pool for split");
				return false;
			}

			if (split_pml2(ept_state, split_buffer, physical_address) == false)
			{
				pool_manager::release_pool(split_buffer);
				LogError("Split failed");
				return false;
			}
		}

		__ept_pte* target_page = get_pml1_entry(ept_state, physical_address);
		if (target_page == nullptr)
		{
			LogError("Failed to get PML1 entry of the target address");
			return false;
		}

		__ept_hooked_page_info* hooked_page_info = pool_manager::request_pool<__ept_hooked_page_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, true, sizeof(__ept_hooked_page_info));
		if (hooked_page_info == nullptr)
		{
			LogError("There is no preallocated pool for hooked page info");
			return false;
		}

		hooked_page_info->fake_page_contents = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, true, PAGE_SIZE);
		if (hooked_page_info->fake_page_contents == nullptr)
		{
			pool_manager::release_pool(hooked_page_info);
			LogError("There is no preallocated pool for fake page contents");
			return false;
		}

		InitializeListHead(&hooked_page_info->hooked_functions_list);

		__ept_hooked_function_info* hooked_function_info = pool_manager::request_pool<__ept_hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, true, sizeof(__ept_hooked_function_info));
		if (hooked_function_info == nullptr)
		{
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			LogError("There is no preallocated pool for hooked function info");
			return false;
		}

		hooked_function_info->trampoline_va = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_EXEC_TRAMPOLINE, TRUE, 100);
		if (hooked_function_info->trampoline_va == nullptr)
		{
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			pool_manager::release_pool(hooked_function_info);
			LogError("There is no pre-allocated pool for trampoline");
			return false;
		}

		hooked_function_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_INSTRUCTION, TRUE, 100);
		if (hooked_function_info->original_instructions_backup == nullptr)
		{
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_function_info->trampoline_va);
			pool_manager::release_pool(hooked_page_info);
			pool_manager::release_pool(hooked_function_info);
			LogError("There is no pre-allocated pool for trampoline");
			return false;
		}

		hooked_page_info->process_id = utils::internal_functions::pfn_ps_get_current_process_id();
		hooked_page_info->pfn_of_hooked_page = GET_PFN(physical_address);
		hooked_page_info->pfn_of_fake_page_contents = GET_PFN(MmGetPhysicalAddress(hooked_page_info->fake_page_contents).QuadPart);
		hooked_page_info->entry_address = target_page;

		hooked_page_info->entry_address->execute = 0;
		hooked_page_info->entry_address->read = 1;
		hooked_page_info->entry_address->write = 1;

		hooked_page_info->original_entry = *target_page;
		hooked_page_info->changed_entry = *target_page;

		hooked_page_info->changed_entry.read = 0;
		hooked_page_info->changed_entry.write = 0;
		hooked_page_info->changed_entry.execute = 1;

		hooked_page_info->changed_entry.physical_address = hooked_page_info->pfn_of_fake_page_contents;

		
		
		RtlCopyMemory(hooked_page_info->fake_page_contents, PAGE_ALIGN(target_function), PAGE_SIZE);
		hooked_page_info->is_user_mode = false;
		hooked_page_info->process_id = PsGetCurrentProcessId();

		hooked_function_info->new_handler_va = new_function;
		hooked_function_info->original_va = target_function;
		hooked_function_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];


		hooked_function_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(new_function).QuadPart;
		hooked_function_info->original_pa = physical_address;
		hooked_function_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
		hooked_function_info->fake_page_contents = hooked_page_info->fake_page_contents;
		hooked_function_info->type = hook_type::hook_kernel_function_redirect;
		 
	
		if(hook_instruction_memory_int1(hooked_function_info, target_function,page_offset) == false)
		{
			if (hooked_function_info->trampoline_va != nullptr)
				pool_manager::release_pool(hooked_function_info->trampoline_va);
			pool_manager::release_pool(hooked_function_info);
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			LogError("Hook failed");
			return false;
		}
		++hooked_page_info->ref_count;
		if (origin_function)
			*origin_function = hooked_function_info->trampoline_va;
		// Track all hooked functions
		InsertHeadList(&hooked_page_info->hooked_functions_list, &hooked_function_info->hooked_function_list);

		// Track all hooked pages
		InsertHeadList(&ept_state.hooked_page_list, &hooked_page_info->hooked_page_list);

		invept_single_context_address(ept_state.ept_pointer->all);

		return true;
	}

	bool hook_user_exception_int3(_In_  __ept_state& ept_state, _In_ HANDLE process_id, _In_  void* target_function, _In_  void* breakpoint_handler, _In_ uint8_t* trampoline_va)
	{
		if (!target_function)
		{
			return false;
		}

		if (!breakpoint_handler)
		{
			return false;
		}

		 

		unsigned __int64 physical_address = utils::internal_functions::pfn_mm_get_physical_address(target_function).QuadPart;

		//
		// Check if function exist in physical memory
		//
		if (physical_address == NULL)
		{
			LogError("Requested virtual memory doesn't exist in physical one");
			return false;
		}


		unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET((unsigned __int64)target_function);

	 

		 

		//
		// Check if page isn't already hooked
		//
		PLIST_ENTRY current = &ept_state.hooked_page_list;
		while (&ept_state.hooked_page_list != current->Flink)
		{
			current = current->Flink;
			__ept_hooked_page_info* hooked_page_info = CONTAINING_RECORD(current, __ept_hooked_page_info, hooked_page_list);

			if (hooked_page_info->pfn_of_hooked_page == GET_PFN(physical_address) && hooked_page_info->process_id == process_id)
			{
				LogInfo("Page already hooked");

				__ept_hooked_function_info* hooked_function_info = pool_manager::request_pool<__ept_hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(__ept_hooked_function_info));
				if (hooked_function_info == nullptr)
				{

					LogError("There is no pre-allocated pool for hooked function struct");

					return false;
				}


		 
				hooked_function_info->trampoline_va = trampoline_va;
				hooked_function_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_INSTRUCTION, TRUE, 100);
				if (hooked_function_info->original_instructions_backup == nullptr)
				{

					pool_manager::release_pool(hooked_function_info->trampoline_va);
					pool_manager::release_pool(hooked_function_info);
					LogError("There is no pre-allocated pool for trampoline");
					return false;
				}

				hooked_function_info->new_handler_va = breakpoint_handler;
				hooked_function_info->original_va = target_function;
				hooked_function_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];

				hooked_function_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(breakpoint_handler).QuadPart;
				hooked_function_info->original_pa = physical_address;
				hooked_function_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
				hooked_function_info->fake_page_contents = hooked_page_info->fake_page_contents;
				hooked_function_info->type = hook_type::hook_user_exception_break_point_int3;

				if (hook_instruction_memory_int3(hooked_function_info, target_function, page_offset) == false)
				{

					if (hooked_function_info->original_instructions_backup != nullptr)
					{
						pool_manager::release_pool(hooked_function_info->original_instructions_backup);
					}

					pool_manager::release_pool(hooked_function_info);

					LogError("Hook failed");

					return false;
				}
				 
				++hooked_page_info->ref_count;

				// 尝试在 g_ept_breakpoint_hook_list 中查找是否已存在页级断点节点
				ept_debugged_page_info* debugged_page = nullptr;

				for (auto entry = g_ept_breakpoint_hook_list.Flink;
					entry != &g_ept_breakpoint_hook_list;
					entry = entry->Flink)
				{
					debugged_page = CONTAINING_RECORD(entry, ept_debugged_page_info, debugged_page_list_entry);

					if (debugged_page->process_id == process_id&& debugged_page->pfn_of_hooked_page== GET_PFN(physical_address))
					{
						break; // 找到了
					}
					 
				}

				if (debugged_page)
				{
					// 分配断点节点
					ept_breakpoint_info* breakpoint_info = pool_manager::request_pool<ept_breakpoint_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, true, sizeof(ept_breakpoint_info));
					if (!breakpoint_info)
					{
						LogError("Failed to allocate breakpoint_info.");
						return true; // 不影响主逻辑
					}

					breakpoint_info->type = hook_type::hook_user_exception_break_point_int3;
					breakpoint_info->original_va = hooked_function_info->original_va;
					breakpoint_info->fake_va = hooked_function_info->fake_va;
					breakpoint_info->breakpoint_handler_va = hooked_function_info->new_handler_va;
					breakpoint_info->trampoline_va = hooked_function_info->trampoline_va;
					breakpoint_info->original_instruction_backup = hooked_function_info->original_instructions_backup;
					breakpoint_info->instruction_size = hooked_function_info->hook_size;
					InsertHeadList(&debugged_page->breakpoints_list_head, &breakpoint_info->breakpoint_list_entry);
				}
			
				// Track all hooked functions within page
				InsertHeadList(&hooked_page_info->hooked_functions_list, &hooked_function_info->hooked_function_list);

				return true;
			}
		}

		if (is_page_splitted(ept_state, physical_address) == false)
		{
			void* split_buffer = pool_manager::request_pool<void*>(pool_manager::INTENTION_SPLIT_PML2, true, sizeof(__ept_dynamic_split));
			if (split_buffer == nullptr)
			{

				LogError("There is no preallocated pool for split");
				return false;
			}

			if (split_pml2(ept_state, split_buffer, physical_address) == false)
			{

				pool_manager::release_pool(split_buffer);
				LogError("Split failed");
				return false;
			}
		}

		__ept_pte* target_page = get_pml1_entry(ept_state, physical_address);
		if (target_page == nullptr)
		{

			LogError("Failed to get PML1 entry of the target address");
			return false;
		}

		__ept_hooked_page_info* hooked_page_info = pool_manager::request_pool<__ept_hooked_page_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, true, sizeof(__ept_hooked_page_info));
		if (hooked_page_info == nullptr)
		{

			LogError("There is no preallocated pool for hooked page info");
			return false;
		}

		hooked_page_info->fake_page_contents = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, true, PAGE_SIZE);
		if (hooked_page_info->fake_page_contents == nullptr)
		{

			pool_manager::release_pool(hooked_page_info);
			LogError("There is no preallocated pool for fake page contents");
			return false;
		}

		InitializeListHead(&hooked_page_info->hooked_functions_list);

		__ept_hooked_function_info* hooked_function_info = pool_manager::request_pool<__ept_hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, true, sizeof(__ept_hooked_function_info));
		if (hooked_function_info == nullptr)
		{

			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			LogError("There is no preallocated pool for hooked function info");
			return false;
		}
		hooked_function_info->trampoline_va = trampoline_va;
		hooked_function_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_INSTRUCTION, TRUE, 100);
		if (hooked_function_info->original_instructions_backup == nullptr)
		{

			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			pool_manager::release_pool(hooked_function_info);
			LogError("There is no pre-allocated pool for trampoline");
			return false;
		}

		hooked_page_info->process_id = process_id;
		hooked_page_info->pfn_of_hooked_page = GET_PFN(physical_address);
		hooked_page_info->pfn_of_fake_page_contents = GET_PFN(MmGetPhysicalAddress(hooked_page_info->fake_page_contents).QuadPart);
		hooked_page_info->entry_address = target_page;

		hooked_page_info->entry_address->execute = 0;
		hooked_page_info->entry_address->read = 1;
		hooked_page_info->entry_address->write = 1;

		hooked_page_info->original_entry = *target_page;
		hooked_page_info->changed_entry = *target_page;

		hooked_page_info->changed_entry.read = 0;
		hooked_page_info->changed_entry.write = 0;
		hooked_page_info->changed_entry.execute = 1;

		hooked_page_info->changed_entry.physical_address = hooked_page_info->pfn_of_fake_page_contents;

	   bool is_ac =  is_ac_flag_set();
		if (!is_ac)
		{
			_setac();
		}
		 
		RtlCopyMemory(hooked_page_info->fake_page_contents, PAGE_ALIGN(target_function), PAGE_SIZE);
		if (!is_ac)
		{
			_clearac();
		}
	
		hooked_page_info->process_id = PsGetCurrentProcessId();
		hooked_page_info->is_user_mode = true;

		hooked_function_info->new_handler_va = breakpoint_handler;
		hooked_function_info->original_va = target_function;
		hooked_function_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];


		hooked_function_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(breakpoint_handler).QuadPart;
		hooked_function_info->original_pa = physical_address;
		hooked_function_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
		hooked_function_info->fake_page_contents = hooked_page_info->fake_page_contents;
		hooked_function_info->type = hook_type::hook_user_exception_break_point_int3;


		if (hook_instruction_memory_int3(hooked_function_info, target_function, page_offset) == false)
		{


			pool_manager::release_pool(hooked_function_info->original_instructions_backup);
			pool_manager::release_pool(hooked_function_info);
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			LogError("Hook failed");
			return false;
		}
		 

		++hooked_page_info->ref_count;
		// 尝试在 g_ept_breakpoint_hook_list 中查找是否已存在页级断点节点


		   ept_debugged_page_info* debugged_page = nullptr;
		     
		// 如果未找到，则新建一个
		 
			debugged_page = pool_manager::request_pool<ept_debugged_page_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, true, sizeof(ept_debugged_page_info));
			if (!debugged_page)
			{
				LogError("Failed to allocate debugged_page node.");
				return true; // 不影响主流程
			}
			debugged_page->pfn_of_hooked_page = hooked_page_info->pfn_of_hooked_page;
			debugged_page->process_id = process_id;
			debugged_page->fake_page_contents = hooked_page_info->fake_page_contents;
			debugged_page->ref_count = 1;
			InitializeListHead(&debugged_page->breakpoints_list_head);
			InsertHeadList(&g_ept_breakpoint_hook_list, &debugged_page->debugged_page_list_entry);
		 

		// 分配断点节点
		ept_breakpoint_info* breakpoint_info = pool_manager::request_pool<ept_breakpoint_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, true, sizeof(ept_breakpoint_info));
		if (!breakpoint_info)
		{
			LogError("Failed to allocate breakpoint_info.");
			return true; // 不影响主逻辑
		}
		breakpoint_info->type = hook_type::hook_user_exception_break_point_int3;
		breakpoint_info->original_va = hooked_function_info->original_va;
		breakpoint_info->fake_va = hooked_function_info->fake_va;
		breakpoint_info->breakpoint_handler_va = hooked_function_info->new_handler_va;
		breakpoint_info->trampoline_va = hooked_function_info->trampoline_va;
		breakpoint_info->original_instruction_backup = hooked_function_info->original_instructions_backup;
		breakpoint_info->instruction_size = hooked_function_info->hook_size;

		// 插入链表
		InsertHeadList(&debugged_page->breakpoints_list_head, &breakpoint_info->breakpoint_list_entry);

		// Track all hooked functions
		InsertHeadList(&hooked_page_info->hooked_functions_list, &hooked_function_info->hooked_function_list);

		// Track all hooked pages
		InsertHeadList(&ept_state.hooked_page_list, &hooked_page_info->hooked_page_list);

		invept_single_context_address(ept_state.ept_pointer->all);

		return true;
	}

	bool hook_break_ponint_int3(_In_  __ept_state& ept_state, _In_  void* target_function, _In_  void* breakpoint_handler, _Out_ unsigned char* original_byte)
	{
		// 判断目标函数地址是否位于用户态地址空间
		const bool is_user_mode_address = utils::memory::is_user_address(target_function);

		if (is_user_mode_address)
		{
			// 用户态地址，调用用户态断点安装逻辑
			return hook_user_break_point_int3(ept_state, target_function, breakpoint_handler, original_byte);
		}
		else
		{
			// 内核态地址，调用内核断点安装逻辑
			return hook_kernel_break_point_int3(ept_state, target_function, breakpoint_handler, original_byte);
		}
	}

	bool hook_kernel_break_point_int3(__ept_state& ept_state, void* target_function, void* breakpoint_handler, _Out_ unsigned char* original_byte)
	{
		if (!target_function)
		{
			return false;
		}

		if (!breakpoint_handler)
		{
			return false;
		}

		if (original_byte )
		{
			*original_byte = 0;
		}
		
		  

		unsigned __int64 physical_address = utils::internal_functions::pfn_mm_get_physical_address(target_function).QuadPart;

		//
		// Check if function exist in physical memory
		//
		if (physical_address == NULL)
		{
			LogError("Requested virtual memory doesn't exist in physical one");
			return false;
		}

		unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET((unsigned __int64)target_function);

		//
// Check if page isn't already hooked
//
		PLIST_ENTRY current = &ept_state.hooked_page_list;
		while (&ept_state.hooked_page_list != current->Flink)
		{
			current = current->Flink;
			__ept_hooked_page_info* hooked_page_info = CONTAINING_RECORD(current, __ept_hooked_page_info, hooked_page_list);

			if (hooked_page_info->pfn_of_hooked_page == GET_PFN(physical_address))
			{
				LogInfo("Page already hooked");

				__ept_hooked_function_info* hooked_function_info = pool_manager::request_pool<__ept_hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(__ept_hooked_function_info));
				if (hooked_function_info == nullptr)
				{
					LogError("There is no pre-allocated pool for hooked function struct");
					return false;
				}

			 
				hooked_function_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_INSTRUCTION, TRUE, 100);
				if (hooked_function_info->original_instructions_backup == nullptr)
				{
					pool_manager::release_pool(hooked_function_info->trampoline_va);
					pool_manager::release_pool(hooked_function_info);
					LogError("There is no pre-allocated pool for trampoline");
					return false;
				}

				hooked_function_info->new_handler_va = breakpoint_handler;
				hooked_function_info->original_va = target_function;
				hooked_function_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];

				hooked_function_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(breakpoint_handler).QuadPart;
				hooked_function_info->original_pa = physical_address;
				hooked_function_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
				hooked_function_info->fake_page_contents = hooked_page_info->fake_page_contents;
				hooked_function_info->type = hook_type::hook_dbg_break_point_int3;
				 
				if (hook_instruction_memory_int1(hooked_function_info, target_function, page_offset) == false)
				{
					
					if (hooked_function_info->original_instructions_backup != nullptr)
					{
						pool_manager::release_pool(hooked_function_info->original_instructions_backup);
					}

					pool_manager::release_pool(hooked_function_info);
					LogError("Hook failed");
					return false;
				}
				if (original_byte)
				{
					*original_byte = *reinterpret_cast<PUCHAR> (hooked_function_info->original_instructions_backup);
				}

				++hooked_page_info->ref_count;

				 
				// Track all hooked functions within page
				InsertHeadList(&hooked_page_info->hooked_functions_list, &hooked_function_info->hooked_function_list);

				return true;
			}
		}

		if (is_page_splitted(ept_state, physical_address) == false)
		{
			void* split_buffer = pool_manager::request_pool<void*>(pool_manager::INTENTION_SPLIT_PML2, true, sizeof(__ept_dynamic_split));
			if (split_buffer == nullptr)
			{
				LogError("There is no preallocated pool for split");
				return false;
			}

			if (split_pml2(ept_state, split_buffer, physical_address) == false)
			{
				pool_manager::release_pool(split_buffer);
				LogError("Split failed");
				return false;
			}
		}

		__ept_pte* target_page = get_pml1_entry(ept_state, physical_address);
		if (target_page == nullptr)
		{
			LogError("Failed to get PML1 entry of the target address");
			return false;
		}

		__ept_hooked_page_info* hooked_page_info = pool_manager::request_pool<__ept_hooked_page_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, true, sizeof(__ept_hooked_page_info));
		if (hooked_page_info == nullptr)
		{
			LogError("There is no preallocated pool for hooked page info");
			return false;
		}

		hooked_page_info->fake_page_contents = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, true, PAGE_SIZE);
		if (hooked_page_info->fake_page_contents == nullptr)
		{
			pool_manager::release_pool(hooked_page_info);
			LogError("There is no preallocated pool for fake page contents");
			return false;
		}

		InitializeListHead(&hooked_page_info->hooked_functions_list);

		__ept_hooked_function_info* hooked_function_info = pool_manager::request_pool<__ept_hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, true, sizeof(__ept_hooked_function_info));
		if (hooked_function_info == nullptr)
		{
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			LogError("There is no preallocated pool for hooked function info");
			return false;
		}


		hooked_function_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_INSTRUCTION, TRUE, 100);
		if (hooked_function_info->original_instructions_backup == nullptr)
		{
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			pool_manager::release_pool(hooked_function_info);
			LogError("There is no pre-allocated pool for trampoline");
			return false;
		}

		hooked_page_info->process_id = utils::internal_functions::pfn_ps_get_current_process_id();
		hooked_page_info->pfn_of_hooked_page = GET_PFN(physical_address);
		hooked_page_info->pfn_of_fake_page_contents = GET_PFN(MmGetPhysicalAddress(hooked_page_info->fake_page_contents).QuadPart);
		hooked_page_info->entry_address = target_page;

		hooked_page_info->entry_address->execute = 0;
		hooked_page_info->entry_address->read = 1;
		hooked_page_info->entry_address->write = 1;

		hooked_page_info->original_entry = *target_page;
		hooked_page_info->changed_entry = *target_page;

		hooked_page_info->changed_entry.read = 0;
		hooked_page_info->changed_entry.write = 0;
		hooked_page_info->changed_entry.execute = 1;

		hooked_page_info->changed_entry.physical_address = hooked_page_info->pfn_of_fake_page_contents;



		RtlCopyMemory(hooked_page_info->fake_page_contents, PAGE_ALIGN(target_function), PAGE_SIZE);

		hooked_page_info->process_id = PsGetCurrentProcessId();
		hooked_page_info->is_user_mode =false;
		hooked_function_info->new_handler_va = breakpoint_handler;
		hooked_function_info->original_va = target_function;
		hooked_function_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];


		hooked_function_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(breakpoint_handler).QuadPart;
		hooked_function_info->original_pa = physical_address;
		hooked_function_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
		hooked_function_info->fake_page_contents = hooked_page_info->fake_page_contents;
		hooked_function_info->type = hook_type::hook_dbg_break_point_int3;
		 

		if (hook_instruction_memory_int3(hooked_function_info, target_function, page_offset) == false)
		{
			pool_manager::release_pool(hooked_function_info->original_instructions_backup);
			pool_manager::release_pool(hooked_function_info);
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			LogError("Hook failed");
			return false;
		}

		if (original_byte)
		{
			*original_byte = *reinterpret_cast<PUCHAR> (hooked_function_info->original_instructions_backup);
		}
		++hooked_page_info->ref_count;
	 
		// Track all hooked functions
		InsertHeadList(&hooked_page_info->hooked_functions_list, &hooked_function_info->hooked_function_list);

		// Track all hooked pages
		InsertHeadList(&ept_state.hooked_page_list, &hooked_page_info->hooked_page_list);

		invept_single_context_address(ept_state.ept_pointer->all);

		return true;
		 
	}
 
	bool hook_user_break_point_int3(_In_  __ept_state& ept_state, _In_  void* target_function, _In_  void* breakpoint_handler, _Out_ unsigned char* original_byte)
	{
		if (!target_function)
		{
			return false;
		}

		if (!breakpoint_handler)
		{
			return false;
		}
		  
		unsigned __int64 physical_address = utils::internal_functions::pfn_mm_get_physical_address(target_function).QuadPart;

		//
		// Check if function exist in physical memory
		//
		if (physical_address == NULL)
		{
			LogError("Requested virtual memory doesn't exist in physical one");
			return false;
		}


		unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET((unsigned __int64)target_function);

		PEPROCESS process = utils::internal_functions::pfn_ps_get_current_process();
		HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);
	 
		 


		//提前锁页 防止异常
	/*	PMDL mdl{};
		if (!NT_SUCCESS(utils::memory::lock_memory(page_offset, 0x1000, &mdl)))
		{
			return false;

		}*/


		//
		// Check if page isn't already hooked
		//
		PLIST_ENTRY current = &ept_state.hooked_page_list;
		while (&ept_state.hooked_page_list != current->Flink)
		{
			current = current->Flink;
			__ept_hooked_page_info* hooked_page_info = CONTAINING_RECORD(current, __ept_hooked_page_info, hooked_page_list);
			 
			if (hooked_page_info->pfn_of_hooked_page == GET_PFN(physical_address)&&hooked_page_info->process_id == process_id)
			{
				LogInfo("Page already hooked");

				__ept_hooked_function_info* hooked_function_info = pool_manager::request_pool<__ept_hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(__ept_hooked_function_info));
				if (hooked_function_info == nullptr)
				{
					 
					LogError("There is no pre-allocated pool for hooked function struct");
					
					return false;
				}

				hooked_function_info->trampoline_va = nullptr;
				hooked_function_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_INSTRUCTION, TRUE, 100);
				if (hooked_function_info->original_instructions_backup == nullptr)
				{
					 
					pool_manager::release_pool(hooked_function_info->trampoline_va);
					pool_manager::release_pool(hooked_function_info);
					LogError("There is no pre-allocated pool for trampoline");
					return false;
				}

				hooked_function_info->new_handler_va = breakpoint_handler;
				hooked_function_info->original_va = target_function;
				hooked_function_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];

				hooked_function_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(breakpoint_handler).QuadPart;
				hooked_function_info->original_pa = physical_address;
				hooked_function_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
				hooked_function_info->fake_page_contents = hooked_page_info->fake_page_contents;
				hooked_function_info->type = hook_type::hook_dbg_break_point_int3;
			 
				if (hook_instruction_memory_int3(hooked_function_info, target_function, page_offset) == false)
				{
					 
					if (hooked_function_info->original_instructions_backup != nullptr)
					{
						pool_manager::release_pool(hooked_function_info->original_instructions_backup);
					}

					pool_manager::release_pool(hooked_function_info);
					 
					LogError("Hook failed");
					
					return false;
				}
				if (original_byte)
				{
					*original_byte = *reinterpret_cast<PUCHAR> (hooked_function_info->original_instructions_backup);
				}
				++hooked_page_info->ref_count;
				
				// Track all hooked functions within page
				InsertHeadList(&hooked_page_info->hooked_functions_list, &hooked_function_info->hooked_function_list);
				 
				return true;
			}
		}

		if (is_page_splitted(ept_state, physical_address) == false)
		{
			void* split_buffer = pool_manager::request_pool<void*>(pool_manager::INTENTION_SPLIT_PML2, true, sizeof(__ept_dynamic_split));
			if (split_buffer == nullptr)
			{
				 
				LogError("There is no preallocated pool for split");
				return false;
			}

			if (split_pml2(ept_state, split_buffer, physical_address) == false)
			{
				 
				pool_manager::release_pool(split_buffer);
				LogError("Split failed");
				return false;
			}
		}

		__ept_pte* target_page = get_pml1_entry(ept_state, physical_address);
		if (target_page == nullptr)
		{
			 
			LogError("Failed to get PML1 entry of the target address");
			return false;
		}

		__ept_hooked_page_info* hooked_page_info = pool_manager::request_pool<__ept_hooked_page_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, true, sizeof(__ept_hooked_page_info));
		if (hooked_page_info == nullptr)
		{
			 
			LogError("There is no preallocated pool for hooked page info");
			return false;
		}

		hooked_page_info->fake_page_contents = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, true, PAGE_SIZE);
		if (hooked_page_info->fake_page_contents == nullptr)
		{
			 
			pool_manager::release_pool(hooked_page_info);
			LogError("There is no preallocated pool for fake page contents");
			return false;
		}

		InitializeListHead(&hooked_page_info->hooked_functions_list);

		__ept_hooked_function_info* hooked_function_info = pool_manager::request_pool<__ept_hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, true, sizeof(__ept_hooked_function_info));
		if (hooked_function_info == nullptr)
		{
			 
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			LogError("There is no preallocated pool for hooked function info");
			return false;
		}
		hooked_function_info->trampoline_va = nullptr;
		hooked_function_info->original_instructions_backup = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_BACKUP_INSTRUCTION, TRUE, 100);
		if (hooked_function_info->original_instructions_backup == nullptr)
		{
			 
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			pool_manager::release_pool(hooked_function_info);
			LogError("There is no pre-allocated pool for trampoline");
			return false;
		}

		hooked_page_info->process_id = process_id;
		hooked_page_info->pfn_of_hooked_page = GET_PFN(physical_address);
		hooked_page_info->pfn_of_fake_page_contents = GET_PFN(MmGetPhysicalAddress(hooked_page_info->fake_page_contents).QuadPart);
		hooked_page_info->entry_address = target_page;

		hooked_page_info->entry_address->execute = 0;
		hooked_page_info->entry_address->read = 1;
		hooked_page_info->entry_address->write = 1;

		hooked_page_info->original_entry = *target_page;
		hooked_page_info->changed_entry = *target_page;

		hooked_page_info->changed_entry.read = 0;
		hooked_page_info->changed_entry.write = 0;
		hooked_page_info->changed_entry.execute = 1;

		hooked_page_info->changed_entry.physical_address = hooked_page_info->pfn_of_fake_page_contents;



		RtlCopyMemory(hooked_page_info->fake_page_contents, PAGE_ALIGN(target_function), PAGE_SIZE);
		
		hooked_page_info->process_id = PsGetCurrentProcessId();
		hooked_page_info->is_user_mode = true;

		hooked_function_info->new_handler_va = breakpoint_handler;
		hooked_function_info->original_va = target_function;
		hooked_function_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];


		hooked_function_info->new_handler_pa = utils::internal_functions::pfn_mm_get_physical_address(breakpoint_handler).QuadPart;
		hooked_function_info->original_pa = physical_address;
		hooked_function_info->fake_pa = utils::internal_functions::pfn_mm_get_physical_address(&hooked_page_info->fake_page_contents[page_offset]).QuadPart;
		hooked_function_info->fake_page_contents = hooked_page_info->fake_page_contents;
		hooked_function_info->type = hook_type::hook_dbg_break_point_int3;
		

		if (hook_instruction_memory_int3(hooked_function_info, target_function, page_offset) == false)
		{
			 
		 
			pool_manager::release_pool(hooked_function_info->original_instructions_backup);
			pool_manager::release_pool(hooked_function_info);
			pool_manager::release_pool(hooked_page_info->fake_page_contents);
			pool_manager::release_pool(hooked_page_info);
			LogError("Hook failed");
			return false;
		}
		if (original_byte)
		{
			*original_byte = *reinterpret_cast<PUCHAR> (hooked_function_info->original_instructions_backup);
		}
		++hooked_page_info->ref_count;

		 
		// Track all hooked functions
		InsertHeadList(&hooked_page_info->hooked_functions_list, &hooked_function_info->hooked_function_list);

		// Track all hooked pages
		InsertHeadList(&ept_state.hooked_page_list, &hooked_page_info->hooked_page_list);

		invept_single_context_address(ept_state.ept_pointer->all);
		 
		return true;
	}
  


	bool find_break_point_info(
	 
		_In_ HANDLE process_id,
		_In_ hook_type type,
		_In_ void* target_function,
	
		_Out_ ept_breakpoint_info** out_hook_info
	)
	{
		if (!out_hook_info)
		{
			return false;
		}
		*out_hook_info = nullptr;
	 
		LIST_ENTRY* hook_page_list = &g_ept_breakpoint_hook_list;

		for (PLIST_ENTRY page_entry = hook_page_list->Flink;
			page_entry != hook_page_list;
			page_entry = page_entry->Flink)
		{
			auto* hooked_page = CONTAINING_RECORD(page_entry, ept_debugged_page_info, debugged_page_list_entry);

		 
			if (hooked_page->process_id != process_id)
				continue;

			for (PLIST_ENTRY func_entry = hooked_page->breakpoints_list_head.Flink;
				func_entry != &hooked_page->breakpoints_list_head;
				func_entry = func_entry->Flink)
			{
				auto* hook_info = CONTAINING_RECORD(func_entry, ept_breakpoint_info, breakpoint_list_entry);

				if (type != hook_info->type)
				{ 
					continue;
					
				}

				if (reinterpret_cast<void*>(target_function) == hook_info->original_va)
				{
					*out_hook_info = hook_info;
					return true;
				}
				
			}
		}
		return false;
	}
	//bool InstallUserHook_function(__ept_state& ept_state, void* original_function, void* hooked_function, void** origin_function  )
	//{
	// 
	//	//DbgBreakPoint();
	//	unsigned __int64 physical_address = MmGetPhysicalAddress(original_function).QuadPart;

	//	//
	//	// Check if function exist in physical memory
	//	//
	//	if (physical_address == NULL)
	//	{
	//		LogError("Requested virtual memory doesn't exist in physical one");
	//		return false;
	//	}

	//	//
	//	// Check if page isn't already hooked
	//	//
	//	PLIST_ENTRY current = &ept_state.hooked_page_list;
	//	while (&ept_state.hooked_page_list != current->Flink)
	//	{
	//		current = current->Flink;
	//		__ept_hooked_page_info* hooked_page_info = CONTAINING_RECORD(current, __ept_hooked_page_info, hooked_page_list);

	//		if (hooked_page_info->pfn_of_hooked_page == GET_PFN(physical_address))
	//		{
	//			LogInfo("Page already hooked");

	//			__ept_hooked_function_info* hooked_function_info = pool_manager::request_pool<__ept_hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(__ept_hooked_function_info));
	//			if (hooked_function_info == nullptr)
	//			{
	//				LogError("There is no pre-allocated pool for hooked function struct");
	//				return false;
	//			}

	//			//hooked_function_info->trampoline_address =(unsigned __int8*) trampoline_address;
	//			//if (hooked_function_info->trampoline_address == nullptr)
	//			//{
	//			//	pool_manager::release_pool(hooked_function_info);
	//			//	LogError("There is no pre-allocated pool for trampoline");
	//			//	return false;
	//			//}

	//			hooked_function_info->original_va = original_function;

	//			hooked_function_info->fake_va = hooked_function;

	//			hooked_function_info->fake_page_contents = hooked_page_info->fake_page_contents;

	//			if (hook_instruction_memory(hooked_function_info, original_function, origin_function) == false)
	//			{
	//				/*if (hooked_function_info->trampoline_address != nullptr)
	//					pool_manager::release_pool(hooked_function_info->trampoline_address);*/
	//				pool_manager::release_pool(hooked_function_info);
	//				LogError("Hook failed");
	//				return false;
	//			}

	//			// Track all hooked functions within page
	//			InsertHeadList(&hooked_page_info->hooked_functions_list, &hooked_function_info->hooked_function_list);

	//			return true;
	//		}
	//	}

	//	if (is_page_splitted(ept_state, physical_address) == false)
	//	{
	//		void* split_buffer = pool_manager::request_pool<void*>(pool_manager::INTENTION_SPLIT_PML2, true, sizeof(__ept_dynamic_split));
	//		if (split_buffer == nullptr)
	//		{
	//			LogError("There is no preallocated pool for split");
	//			return false;
	//		}

	//		if (split_pml2(ept_state, split_buffer, physical_address) == false)
	//		{
	//			pool_manager::release_pool(split_buffer);
	//			LogError("Split failed");
	//			return false;
	//		}
	//	}

	//	__ept_pte* target_page = get_pml1_entry(ept_state, physical_address);
	//	if (target_page == nullptr)
	//	{
	//		LogError("Failed to get PML1 entry of the target address");
	//		return false;
	//	}

	//	__ept_hooked_page_info* hooked_page_info = pool_manager::request_pool<__ept_hooked_page_info*>(pool_manager::INTENTION_TRACK_HOOKED_PAGES, true, sizeof(__ept_hooked_page_info));
	//	if (hooked_page_info == nullptr)
	//	{
	//		LogError("There is no preallocated pool for hooked page info");
	//		return false;
	//	}

	//	hooked_page_info->fake_page_contents = pool_manager::request_pool<unsigned __int8*>(pool_manager::INTENTION_FAKE_PAGE_CONTENTS, true, PAGE_SIZE);
	//	if (hooked_page_info->fake_page_contents == nullptr)
	//	{
	//		pool_manager::release_pool(hooked_page_info);
	//		LogError("There is no preallocated pool for fake page contents");
	//		return false;
	//	}

	//	InitializeListHead(&hooked_page_info->hooked_functions_list);

	//	__ept_hooked_function_info* hooked_function_info = pool_manager::request_pool<__ept_hooked_function_info*>(pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, true, sizeof(__ept_hooked_function_info));
	//	if (hooked_function_info == nullptr)
	//	{
	//		pool_manager::release_pool(hooked_page_info->fake_page_contents);
	//		pool_manager::release_pool(hooked_page_info);
	//		LogError("There is no preallocated pool for hooked function info");
	//		return false;
	//	}

	//	//hooked_function_info->trampoline_address = (unsigned __int8*)trampoline_address;
	//	//if (hooked_function_info->trampoline_address == nullptr)
	//	//{
	//	//	pool_manager::release_pool(hooked_page_info->fake_page_contents);
	//	//	pool_manager::release_pool(hooked_page_info);
	//	//	pool_manager::release_pool(hooked_function_info);
	//	//	LogError("There is no pre-allocated pool for trampoline");
	//	//	return false;
	//	//}

	//	hooked_page_info->pfn_of_hooked_page = GET_PFN(physical_address);
	//	hooked_page_info->pfn_of_fake_page_contents = GET_PFN(MmGetPhysicalAddress(hooked_page_info->fake_page_contents).QuadPart);
	//	hooked_page_info->entry_address = target_page;

	//	hooked_page_info->entry_address->execute = 0;
	//	hooked_page_info->entry_address->read = 1;
	//	hooked_page_info->entry_address->write = 1;

	//	hooked_page_info->original_entry = *target_page;
	//	hooked_page_info->changed_entry = *target_page;

	//	hooked_page_info->changed_entry.read = 0;
	//	hooked_page_info->changed_entry.write = 0;
	//	hooked_page_info->changed_entry.execute = 1;

	//	hooked_page_info->changed_entry.physical_address = hooked_page_info->pfn_of_fake_page_contents;

	//	hooked_page_info->process_id = PsGetCurrentProcessId();
	//	_setac();
	//	 
	//	RtlCopyMemory(hooked_page_info->fake_page_contents, PAGE_ALIGN(original_function), PAGE_SIZE);
	//	_clearac();
	//	

	//	hooked_function_info->original_va = original_function;

	//	hooked_function_info->fake_va = hooked_function;

	//	hooked_function_info->fake_page_contents = hooked_page_info->fake_page_contents;

	//	if (hook_instruction_memory(hooked_function_info, original_function, origin_function) == false)
	//	{
	//		/*if (hooked_function_info->trampoline_address != nullptr)
	//			pool_manager::release_pool(hooked_function_info->trampoline_address);*/
	//		pool_manager::release_pool(hooked_function_info);
	//		pool_manager::release_pool(hooked_page_info->fake_page_contents);
	//		pool_manager::release_pool(hooked_page_info);
	//		LogError("Hook failed");
	//		return false;
	//	}
	//
	//	// Track all hooked functions
	//	InsertHeadList(&hooked_page_info->hooked_functions_list, &hooked_function_info->hooked_function_list);

	//	// Track all hooked pages
	//	InsertHeadList(&ept_state.hooked_page_list, &hooked_page_info->hooked_page_list);

	//	invept_single_context(ept_state.ept_pointer->all);

	//	  
	//	return true;
	//}

	/// <summary>
	/// Unhook single function
	/// </summary>
	/// <param name="virtual_address"></param>
	/// <returns></returns>
	bool unhook_function(__ept_state& ept_state, unsigned __int64 virtual_address)
	{
		//
		// Check if function which we want to unhook exist in physical memory
		unsigned __int64 physical_address = MmGetPhysicalAddress((void*)virtual_address).QuadPart;
		if (physical_address == 0)
			return false;

		PLIST_ENTRY current_hooked_page = &ept_state.hooked_page_list;
		while (&ept_state.hooked_page_list != current_hooked_page->Flink)
		{
			current_hooked_page = current_hooked_page->Flink;
			__ept_hooked_page_info* hooked_page_info = CONTAINING_RECORD(current_hooked_page, __ept_hooked_page_info, hooked_page_list);

			//
			// Check if function pfn is equal to pfn saved in hooked page info
			if (hooked_page_info->pfn_of_hooked_page == GET_PFN(physical_address))
			{
				PLIST_ENTRY current_hooked_function;
				current_hooked_function = &hooked_page_info->hooked_functions_list;

				while (&hooked_page_info->hooked_functions_list != current_hooked_function->Flink)
				{
					current_hooked_function = current_hooked_function->Flink;
					__ept_hooked_function_info* hooked_function_info = CONTAINING_RECORD(current_hooked_function, __ept_hooked_function_info, hooked_function_list);
					
					unsigned __int64 function_page_offset = MASK_EPT_PML1_OFFSET(virtual_address);

					//
					// Check if the address of function which we want to unhook is 
					// the same as address of function in hooked function info struct
					//
					if (function_page_offset == MASK_EPT_PML1_OFFSET(hooked_function_info->original_va))
					{
						// Restore overwritten data
						RtlCopyMemory(&hooked_function_info->fake_page_contents[function_page_offset], hooked_function_info->original_va, hooked_function_info->hook_size);
						
						RemoveEntryList(current_hooked_function);

						if(hooked_function_info->trampoline_va != nullptr)
							pool_manager::release_pool(hooked_function_info->trampoline_va);
						pool_manager::release_pool(hooked_function_info);

						//
						// If there is no more function hooks free hooked page info struct
						if (hooked_page_info->hooked_functions_list.Flink == hooked_page_info->hooked_functions_list.Blink)
						{
							hooked_page_info->original_entry.execute = 1;
							swap_pml1_and_invalidate_tlb(ept_state, hooked_page_info->entry_address, hooked_page_info->original_entry, invept_type::invept_single_context);

							RemoveEntryList(current_hooked_page);
							pool_manager::release_pool(hooked_page_info->fake_page_contents);
							pool_manager::release_pool(hooked_page_info);
							return true;
						}

						invept_all_contexts();
						return true;
					}
				}
			}
		}
		return false;
	}

	bool unhook_by_pid(__ept_state& ept_state, HANDLE processId)
	{
	  
		bool unhooked_any = false;
		PLIST_ENTRY current_hooked_page = &ept_state.hooked_page_list;
		while (&ept_state.hooked_page_list != current_hooked_page->Flink)
		{
			current_hooked_page = current_hooked_page->Flink;
			__ept_hooked_page_info* hooked_page_info = CONTAINING_RECORD(current_hooked_page, __ept_hooked_page_info, hooked_page_list);

			// Check if the current hooked page belongs to the specified process ID
			if (hooked_page_info->process_id == processId)
			{
				unhooked_any = true;
				// Unhook all functions related to this page
				PLIST_ENTRY current_hooked_function = &hooked_page_info->hooked_functions_list;

				while (&hooked_page_info->hooked_functions_list != current_hooked_function->Flink)
				{
					current_hooked_function = current_hooked_function->Flink;
					__ept_hooked_function_info* hooked_function_info = CONTAINING_RECORD(current_hooked_function, __ept_hooked_function_info, hooked_function_list);

					// Restore overwritten data for each function

					unsigned __int64 function_page_offset = MASK_EPT_PML1_OFFSET(hooked_function_info->original_va);
					RtlCopyMemory(&hooked_function_info->fake_page_contents[function_page_offset], hooked_function_info->original_va, hooked_function_info->hook_size);
					RemoveEntryList(current_hooked_function);

					if (hooked_function_info->trampoline_va != nullptr)
						pool_manager::release_pool(hooked_function_info->trampoline_va);

					pool_manager::release_pool(hooked_function_info);
				}

				// After unhooking all functions, restore the original page entry and remove the page
				hooked_page_info->original_entry.execute = 1;
				swap_pml1_and_invalidate_tlb(ept_state, hooked_page_info->entry_address, hooked_page_info->original_entry, invept_type::invept_single_context);

				RemoveEntryList(current_hooked_page);
				pool_manager::release_pool(hooked_page_info->fake_page_contents);
				pool_manager::release_pool(hooked_page_info);
			}
		}
		if (unhooked_any)
		{
			invept_all_contexts();  // Invalidate EPT for all contexts
		}
		
		return true;
	}

	/// <summary>
	/// Unhook all functions and invalidate tlb
	/// </summary>
	void unhook_all_functions(__ept_state& ept_state)
	{
		PLIST_ENTRY current_hooked_page = ept_state.hooked_page_list.Flink;
		while (&ept_state.hooked_page_list != current_hooked_page)
		{
			__ept_hooked_page_info* hooked_entry = CONTAINING_RECORD(current_hooked_page, __ept_hooked_page_info, hooked_page_list);

			PLIST_ENTRY current_hooked_function;

			current_hooked_function = hooked_entry->hooked_functions_list.Flink;
			while (&hooked_entry->hooked_functions_list != current_hooked_function)
			{
				__ept_hooked_function_info* hooked_function_info = CONTAINING_RECORD(current_hooked_function, __ept_hooked_function_info, hooked_function_list);
				
				// If hook uses two trampolines unhook second one
				if (hooked_function_info->trampoline_va != nullptr)
					pool_manager::release_pool(hooked_function_info->trampoline_va);

				RemoveEntryList(current_hooked_function);

				current_hooked_function = current_hooked_function->Flink;

				pool_manager::release_pool(hooked_function_info);
			}

			// Restore original pte value
			hooked_entry->original_entry.execute = 1;
			swap_pml1_and_invalidate_tlb(ept_state, hooked_entry->entry_address, hooked_entry->original_entry, invept_type::invept_single_context);

			RemoveEntryList(current_hooked_page);

			current_hooked_page = current_hooked_page->Flink;

			pool_manager::release_pool(hooked_entry->fake_page_contents);
			pool_manager::release_pool(hooked_entry);
		}
	}
}