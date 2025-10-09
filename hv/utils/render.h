#pragma once
#include "font_utils.h" // Include font_utils.h for Font class

namespace utils
{
	namespace render
	{

		 
		

		extern  PVOID g_pSwapChain ;
		extern  PVOID g_pd3dDevice ;
		extern  PVOID g_pD3DXDeviceCtx ;
		extern  PVOID g_pSurface ;
		extern  PVOID g_pRenderTargetView ;
		extern  PVOID g_pRasterizerState;

		extern Font* g_Font ;
 
		NTSTATUS initialize_font();

		NTSTATUS set_font_size(float scale);

		NTSTATUS set_smart_font_size(int screen_width, int screen_height);

		NTSTATUS cleanup_dx_resources();

		NTSTATUS reinitialize_dx_resources(PVOID p_dxgi_swapchain);

		NTSTATUS initialize_dx_resources(PVOID p_dxgi_swapchain);

		NTSTATUS initialize_older_dx_resources(PVOID p_dxgi_swapchain);

		NTSTATUS initialize_last_dx_resources(PVOID p_render_target);

		NTSTATUS render_every_thing(PVOID p_dxgi_swapchain);



		NTSTATUS draw_overlay_elements(int width, int height, void* data);

		float get_font_size();

		float get_recommended_font_size(int screen_width, int screen_height);

		bool check_swapchain_changed(PVOID p_dxgi_swapchain);

		PVOID get_last_render_target_view();


		
	 }
}