#pragma once
namespace utils
{
	namespace strong_dx
	{
		extern  bool g_initialized;
		extern PVOID g_desc_buffer;
		extern PVOID g_entity_buffer;
		extern PVOID g_swap_chain;
		extern PVOID g_pdevice ;
		extern PVOID g_pContext;
		NTSTATUS initialize(unsigned long long pswap_chain);
		void draw_utils();

	}
 
}