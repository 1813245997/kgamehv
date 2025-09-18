#pragma once
#include "LegacyRender.h"
namespace utils
{
	namespace strong_dx
	{
		extern  bool g_initialized;
		extern unsigned long long g_user_buffer;
		extern unsigned long long g_texture_buffer;
		extern unsigned long long g_old_user_buffer;
		extern   unsigned long long g_pOverlaySwapChain;
		extern  	unsigned long long g_RenderTargetView;
		extern PVOID g_swap_chain;
		extern PVOID g_pd3dDevice;
		extern PVOID g_pd3dDeviceContext;
		extern PVOID g_Surface;
	
		extern volatile LONG g_dwm_render_lock ;
		extern bool g_should_hide_overlay;
	 
		 
		bool initialize_font();
		bool initialize_d3d_resources(unsigned long long  pswap_chain);
		bool initialize_d3d11_resources_win1124h2(unsigned long long p_swap_chain);
		void release_d3d_resources();
		void render_overlay_frame(void (*draw_callback)(int width, int height, void* data));
		void render_overlay_frame_win1124h2(void (*draw_callback)(int width, int height, void* data));
		void draw_overlay_elements(int width, int height, void* data);
		void draw_utils( );
		PULONG64 get_user_rsp_ptr();
		bool is_user_stack_in_dxgi_range();
		bool is_user_rsp_ptr();
		bool is_user_stack_in_dxgi_range();
		void draw_update_required_notice(ByteRender& rend);
		void draw_players_esp(ByteRender& rend);


	}
 
}