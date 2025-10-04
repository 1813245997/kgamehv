#pragma once

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
		float get_font_size();
		float get_recommended_font_size(int screen_width, int screen_height);
		NTSTATUS set_smart_font_size(int screen_width, int screen_height);
		
		// 交换链变化检测和重新初始化
		bool check_swapchain_changed(PVOID p_dxgi_swapchain);
		NTSTATUS cleanup_dx_resources();
		NTSTATUS reinitialize_dx_resources(PVOID p_dxgi_swapchain);
		
		NTSTATUS initialize_dx_resources(PVOID p_dxgi_swapchain);
		NTSTATUS render_every_thing(PVOID p_dxgi_swapchain);
		NTSTATUS draw_overlay_elements(int width, int height, void* data);
	 }
}