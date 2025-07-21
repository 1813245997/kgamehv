#include "global_defs.h"
#include "global_initializer.h"

namespace utils
{
	namespace  global
	{
		bool initialize_all_globals()
		{
			utils::os_info::dw_build_number = 0;

			utils::ssdt::g_ssdt_table =0;
			utils::ssdt::g_win32k_table=0;

			 utils::dwm_draw:: g_dwm_process=0;
			 utils::dwm_draw::g_dwm_render_thread = 0;
			 utils::dwm_draw::g_precall_addr = 0;
			 utils::dwm_draw::g_postcall_addr = 0;
			 utils::dwm_draw::g_context_offset = 0;
			 utils::dwm_draw::g_gdi32_base = 0;
			 utils::dwm_draw::g_gdi32_size = 0;
			 utils::dwm_draw::g_ntdll_base = 0;
			 utils::dwm_draw::g_ntdll_size = 0;
			 utils::dwm_draw::g_user32_base = 0;
			 utils::dwm_draw::g_user32_size = 0;
			 utils::dwm_draw::g_dwmcore_base = 0;
			 utils::dwm_draw::g_dwmcore_size = 0;
			 utils::dwm_draw::g_dxgi_base = 0;
			 utils::dwm_draw::g_dxgi_size = 0;
			 utils::dwm_draw::g_offset_stack = 0;
			 utils::dwm_draw::g_ccomposition_present = 0;
			 utils::dwm_draw::g_cocclusion_context_pre_sub_graph = 0;
			 utils::dwm_draw::g_cocclusion_context_post_sub_graph = 0;
			 utils::dwm_draw::g_cdxgi_swapchain_present_multiplane_overlay = 0;
			 utils::dwm_draw::g_cdxgi_swapchain_present_dwm = 0;
			 utils::dwm_draw::g_cdxgi_swap_chain_dwm_legacy_present_dwm = 0;
			 utils::dwm_draw::g_pswap_chain = 0;
			 utils::dwm_draw::g_dxgk_get_device_state = 0;
			 utils::dwm_draw::g_dxgk_open_resource = 0;
			 utils::dwm_draw::g_ki_call_user_mode2 = 0;
			 utils::dwm_draw::g_kvashadow = 0;
			 utils::dwm_draw::g_client_to_screen_fun = 0;
			 utils::dwm_draw::g_get_client_rect_fun = 0;
			 utils::dwm_draw::g_get_foreground_window_fun = 0;
			 utils::dwm_draw::g_find_windoww_fun = 0;
			 utils::dwm_draw::g_get_window_rect_fun = 0;

			 utils::strong_dx::g_initialized = 0;
			 utils::strong_dx::  g_user_buffer = 0;
			 utils::strong_dx::  g_texture_buffer = 0;
			 utils::strong_dx::  g_swap_chain = 0;
			 utils::strong_dx::  g_pdevice = 0;
			 utils::strong_dx::  g_pContext = 0;
			 utils::strong_dx::  g_dwm_render_lock = 0;
			 utils::strong_dx::g_should_hide_overlay = 0;


			 game::kcsgo2::g_game_process = 0;;
			 game::kcsgo2::g_client_base = 0;;
			 game::kcsgo2::g_client_size = 0;;
			 game::kcsgo2::g_engine2_base = 0;;
			 game::kcsgo2::g_engine2_size = 0;;
			 game::kcsgo2::g_is_initialized = 0;;
			 game::kcsgo2::g_game_handle = 0;;
		  
			 utils::auth::g_license_expire_timestamp = 0;
			 return true;
		}
	}
}