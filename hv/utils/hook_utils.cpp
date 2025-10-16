#include "global_defs.h"
#include "hook_utils.h"
#include "fault_uitls.h"
#include "../LDE64.h"
#include "../vtx/poolmanager.h"

namespace utils
{
	namespace hook_utils
	{
		// Global hash table - only manages pages, functions are managed by their respective pages
		utils::kunordered_map<unsigned __int64, kernel_hook_page_info*> g_hook_page_map;  // PFN -> PageInfo

		bool hook_instruction_memory_int1(_Inout_ kernel_hook_function_info* hooked_function_info, void* target_function);
		void hook_write_absolute_jump(unsigned __int8* target_buffer, unsigned __int64 destination_address);

		NTSTATUS initialize_hook_page_lists()
		{
			NTSTATUS status = STATUS_SUCCESS;
			
			// Initialize hash table
			g_hook_page_map.init();
			
			return status;
		}

		bool is_address_already_hooked(_In_ void* target_api)
		{
			if (!target_api)
			{
				return false;
			}

			void* aligned_target = PAGE_ALIGN(target_api);
			uint64_t target_pa = utils::internal_functions::pfn_mm_get_physical_address(aligned_target).QuadPart;
			if (target_pa == 0)
			{
				return false;
			}
			unsigned __int64 page_pfn = GET_PFN(target_pa);

			// O(1) find page
			auto page_it = g_hook_page_map.find(page_pfn);
			if (page_it == g_hook_page_map.end())
			{
				return false;  // Page doesn't exist, function is definitely not hooked
			}

			// O(1) find function within page
			kernel_hook_page_info* page_info = page_it->second;
			auto func_it = page_info->function_map.find(target_api);
			return (func_it != page_info->function_map.end());
		}

		bool hook_kernel_function(_In_ void* target_api, _In_ void* new_api, _Inout_ void** origin_function)
		{
			 
			if (!target_api || !new_api)
			{
				return false;
			}

			// Check if already hooked
			if (is_address_already_hooked(target_api))
			{

				return true;  // Already hooked, return success
			}

			if (origin_function)
			{
				*origin_function = nullptr;
			}

			void* aligned_target = PAGE_ALIGN(target_api);
			uint64_t target_pa = utils::internal_functions::pfn_mm_get_physical_address(aligned_target).QuadPart;
			if (target_pa == 0)
			{
				return false;
			}
			unsigned __int64 page_pfn = GET_PFN(target_pa);
			unsigned __int64 page_offset = MASK_EPT_PML1_OFFSET(target_api);
			kernel_hook_function_info* hook_info = nullptr;
			kernel_hook_page_info* hooked_page_info = nullptr;
			bool succeed = false;
			bool is_new_page = false;

			// O(1) find page - using hash table
			auto page_it = g_hook_page_map.find(page_pfn);
			if (page_it != g_hook_page_map.end())
			{
				hooked_page_info = page_it->second;
			}
			else
			{
				is_new_page = true;
				// Create new page
				hooked_page_info = pool_manager::request_pool<kernel_hook_page_info*>(
					pool_manager::INTENTION_TRACK_HOOKED_PAGES, TRUE, sizeof(kernel_hook_page_info));
				if (!hooked_page_info)
				{
					goto clear;
				}

				hooked_page_info->fake_page_contents = pool_manager::request_pool<uint8_t*>(
					pool_manager::INTENTION_FAKE_PAGE_CONTENTS, TRUE, PAGE_SIZE);
				if (!hooked_page_info->fake_page_contents)
				{
					pool_manager::release_pool(hooked_page_info);
					return false;
				}

				uint64_t fake_pa = utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart;
				if (fake_pa == 0)
				{
					pool_manager::release_pool(hooked_page_info->fake_page_contents);
					pool_manager::release_pool(hooked_page_info);
					goto clear;
				}

				
				RtlCopyMemory(hooked_page_info->fake_page_contents, aligned_target, PAGE_SIZE);

				auto exec_page = utils::internal_functions::pfn_mm_get_physical_address(hooked_page_info->fake_page_contents).QuadPart;

				hooked_page_info->orig_page_pfn = page_pfn;
				hooked_page_info->exec_page_pfn = GET_PFN(exec_page);
				hooked_page_info->hook_count = 0;
				hooked_page_info->function_map.init();

				// Add to page hash table first, but don't install EPT hook yet
				g_hook_page_map[page_pfn] = hooked_page_info;
			}

			// Create function info
			hook_info = pool_manager::request_pool<kernel_hook_function_info*>(
				pool_manager::INTENTION_TRACK_HOOKED_FUNCTIONS, TRUE, sizeof(kernel_hook_function_info));
			if (!hook_info)
			{
				goto clear;
			}

			hook_info->trampoline_va = pool_manager::request_pool<unsigned __int8*>(
				pool_manager::INTENTION_EXEC_TRAMPOLINE, TRUE, 100);
			if (!hook_info->trampoline_va)
			{
				pool_manager::release_pool(hook_info);
				goto clear;
			}

			hook_info->original_code_backup = pool_manager::request_pool<unsigned __int8*>(
				pool_manager::INTENTION_BACKUP_INSTRUCTION, TRUE, 100);
			if (!hook_info->original_code_backup)
			{
				pool_manager::release_pool(hook_info->trampoline_va);
				pool_manager::release_pool(hook_info);
				LogError("There is no pre-allocated pool for trampoline");
				goto clear;
			}

			// Set function info
			hook_info->handler_va = new_api;
			hook_info->original_va = target_api;
			hook_info->original_pa = target_pa;
			hook_info->fake_va = &hooked_page_info->fake_page_contents[page_offset];
			hook_info->page_info_ptr = hooked_page_info;

			if (origin_function)
			{
				*origin_function = hook_info->trampoline_va;
			}

			++hooked_page_info->hook_count;
			hook_instruction_memory_int1(hook_info, target_api);

			// O(1) insert - only insert to page hash table
			hooked_page_info->function_map[target_api] = hook_info;

			// If it's a newly created page, install EPT hook now
			if (is_new_page)
			{
				hv::prevmcall::install_ept_hook(hooked_page_info->orig_page_pfn, hooked_page_info->exec_page_pfn);
			}

			succeed = true;

		clear:
			if (succeed && hook_info)
			{
				LogInfo("hook: Successfully hooked target_api: %p -> new_api: %p, fake_page: %p, trampoline: %p",
					target_api, new_api, hook_info->fake_va, origin_function ? *origin_function : nullptr);
			}
			else
			{
				LogError("hook: FAILED to hook target_api: %p -> new_api: %p",
					target_api, new_api);
			}

			return succeed;
		}

		bool find_kernel_hook_info_by_rip(
			void* rip,
			kernel_hook_function_info* out_hook_info)
		{
			if (!rip || !out_hook_info)
			{
				return false;
			}

			// Calculate page PFN
			void* aligned_rip = PAGE_ALIGN(rip);
			uint64_t target_pa = utils::internal_functions::pfn_mm_get_physical_address(aligned_rip).QuadPart;
			if (target_pa == 0)
			{
				return false;
			}
			unsigned __int64 page_pfn = GET_PFN(target_pa);

			// O(1) find page
			auto page_it = g_hook_page_map.find(page_pfn);
			if (page_it == g_hook_page_map.end())
			{
				return false;  // Page doesn't exist
			}

			// O(1) find function within page
			kernel_hook_page_info* page_info = page_it->second;
			auto func_it = page_info->function_map.find(rip);
			if (func_it != page_info->function_map.end())
			{
				*out_hook_info = *(func_it->second);
				return true;
			}

			return false;
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

		bool hook_instruction_memory_int1(_Inout_ kernel_hook_function_info* hooked_function_info, void* target_function)
		{
			unsigned __int64 hooked_instructions_size = LDE(target_function, 64);

			hooked_function_info->hook_size = hooked_instructions_size;

			RtlCopyMemory(hooked_function_info->trampoline_va, target_function, hooked_instructions_size);
			RtlCopyMemory(hooked_function_info->original_code_backup, target_function, hooked_instructions_size);

			hook_write_absolute_jump(&hooked_function_info->trampoline_va[hooked_instructions_size], 
				(unsigned __int64)target_function + hooked_instructions_size);

			*reinterpret_cast<uint8_t*>(hooked_function_info->fake_va) = 0xF1; // INT1 (ICEBP)

			return true;
		}
	}
}