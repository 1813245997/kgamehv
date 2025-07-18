#pragma once
#include "dx_draw/LegacyRender.h"
namespace utils
{
	namespace strong_dx
	{
		extern  bool g_initialized;
		extern unsigned long long g_user_buffer;
		extern unsigned long long g_texture_buffer;
		extern PVOID g_swap_chain;
		extern PVOID g_pdevice ;
		extern PVOID g_pContext;
	
		extern volatile LONG g_dwm_render_lock ;
		extern bool g_should_hide_overlay;
	 
		 
	 
		bool initialize_d3d_resources();
		void render_overlay_frame(void (*draw_callback)(int width, int height, void* data));
		void draw_overlay_elements(int width, int height, void* data);
		void draw_utils( );
		PULONG64 get_user_rsp_ptr();
		bool is_user_stack_in_dxgi_range();
		bool is_user_rsp_ptr();
		bool is_user_stack_in_dxgi_range();
		void draw_players_esp(ByteRender& rend);
	}
 
}