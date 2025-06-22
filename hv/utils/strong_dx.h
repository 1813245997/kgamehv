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
		extern volatile LONG g_dwm_render_lock ;
		NTSTATUS initialize(unsigned long long pswap_chain);
		 
		void render_overlay_frame(void (*draw_callback)(int width, int height, void* data));
		void draw_overlay_elements(int width, int height, void* data);
		PULONG64 get_user_rsp_ptr();
		bool is_user_stack_in_dxgi_range();
		bool is_user_rsp_ptr();
		bool is_user_stack_in_dxgi_range();
	}
 
}