#include "global_defs.h"
#include "hidden_module_list.h"

namespace utils
{
	namespace hidden_modules
	{
		LIST_ENTRY g_hidden_module_list_head;

		unsigned long long g_dirver_base{};

		unsigned long long g_driver_size{};

		unsigned long long g_physical_base{};

		 

		void initialize_hidden_module_list()
		{
			InitializeListHead(&g_hidden_module_list_head);
		}

		unsigned long long get_driver_base()
		{
			return g_dirver_base;
		}

		unsigned long long get_driver_size()
		{
			return g_driver_size;
		}

		  void set_driver_info(unsigned long long base, unsigned long long size)
		{
			g_dirver_base = base;
			g_driver_size = size;
			g_physical_base = MmGetPhysicalAddress(reinterpret_cast<PVOID>(base)).QuadPart;
		}

		void add_hidden_module(PVOID base, SIZE_T size, PWCHAR module_name_ptr)
		{
			auto entry = static_cast<PHIDDEN_MODULE_ENTRY>(
				utils::memory::allocate_independent_pages(sizeof(HIDDEN_MODULE_ENTRY), PAGE_READWRITE));
			if (!entry)
				return;

			g_dirver_base = reinterpret_cast<unsigned long long>(base);
			g_driver_size = size;
			entry->module_base = base;
			entry->module_end = reinterpret_cast<PVOID>(reinterpret_cast<UINT_PTR>(base) + size);

			if (module_name_ptr)
			{
				 
				UNICODE_STRING tmp_name;
				utils::internal_functions::pfn_rtl_init_unicode_string(&tmp_name, module_name_ptr);

				if (!NT_SUCCESS(utils::internal_functions::pfn_rtl_duplicate_unicode_string(
					RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE, &tmp_name, &entry->module_name)))
				{

					entry->module_name.Length = 0;
					entry->module_name.MaximumLength = 0;
					entry->module_name.Buffer = nullptr;
				}
			}
			else
			{

				entry->module_name.Length = 0;
				entry->module_name.MaximumLength = 0;
				entry->module_name.Buffer = nullptr;
			}

			InsertTailList(&g_hidden_module_list_head, &entry->list_entry);
		}


		void remove_hidden_module(PVOID base)
		{
			if (!base)
				return;

			for (PLIST_ENTRY entry = g_hidden_module_list_head.Flink;
				entry != &g_hidden_module_list_head;
				entry = entry->Flink)
			{
				auto module_entry = CONTAINING_RECORD(entry, HIDDEN_MODULE_ENTRY, list_entry);
				if (module_entry->module_base == base)
				{
					RemoveEntryList(entry);
					utils::memory::free_independent_pages(module_entry, sizeof(HIDDEN_MODULE_ENTRY));
					break;
				}
			}
		}

		void clear_all_hidden_modules()
		{
			while (!IsListEmpty(&g_hidden_module_list_head))
			{
				PLIST_ENTRY entry = RemoveHeadList(&g_hidden_module_list_head);
				auto module_entry = CONTAINING_RECORD(entry, HIDDEN_MODULE_ENTRY, list_entry);
				utils::memory::free_independent_pages(module_entry, sizeof(HIDDEN_MODULE_ENTRY));
			}
		}

		bool is_address_hidden(PVOID address)
		{
			if (!address)
				return false;

			for (PLIST_ENTRY entry = g_hidden_module_list_head.Flink;
				entry != &g_hidden_module_list_head;
				entry = entry->Flink)
			{
				auto module_entry = CONTAINING_RECORD(entry, HIDDEN_MODULE_ENTRY, list_entry);
				if (reinterpret_cast<UINT_PTR>(address) >= reinterpret_cast<UINT_PTR>(module_entry->module_base) &&
					reinterpret_cast<UINT_PTR>(address) <= reinterpret_cast<UINT_PTR>(module_entry->module_end))
				{
					return true;
				}
			}

			return false;

		}

		bool is_address_hidden_physical(unsigned long long address)
		{
			if (!address)
			{
					return false;
			}
			if (address >= g_physical_base && address <= g_physical_base + g_driver_size)
			{
				return true;
			}

			return false;
		}


   }


}