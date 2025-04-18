#pragma once


namespace utils

{

	namespace hidden_modules
	{
		typedef struct _HIDDEN_MODULE_ENTRY
		{
			LIST_ENTRY list_entry;
			PVOID module_base;
			PVOID module_end;
			UNICODE_STRING module_name;
		} HIDDEN_MODULE_ENTRY, * PHIDDEN_MODULE_ENTRY;

		extern LIST_ENTRY g_hidden_module_list_head;

		void initialize_hidden_module_list();

		void add_hidden_module(PVOID base, SIZE_T size, PWCHAR module_name_ptr = nullptr);

		void remove_hidden_module(PVOID base);

		void clear_all_hidden_modules();

		bool is_address_hidden(PVOID address);
	}

}