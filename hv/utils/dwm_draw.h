#pragma once
 
namespace utils
{
	namespace dwm_draw
	{
		extern PEPROCESS g_dwm_process;
		extern PETHREAD g_dwm_render_thread ;
		extern unsigned long long  g_precall_addr ;
		extern unsigned long long  g_postcall_addr ;
		extern unsigned long long g_context_offset;
		extern unsigned long long g_kernel32_base ;
		extern unsigned long long g_kernel32_size ;
		extern unsigned long long g_gdi32_base;
		extern unsigned long long g_gdi32_size;
		extern unsigned long long g_ntdll_base ;
		extern unsigned long long g_ntdll_size ;
		extern unsigned long long g_user32_base ;
		extern unsigned long long g_user32_size ;
		extern unsigned long long g_dwmcore_base ;
		extern unsigned long long g_dwmcore_size ;
		extern unsigned long long g_dxgi_base ;
		extern unsigned long long g_dxgi_size ;
		extern unsigned long long g_d3dcompiler_47_base;
		extern unsigned long long g_d3dcompiler_47_size;
		extern unsigned long long g_offset_stack ;
		extern unsigned long long g_ccomposition_present ;
		extern unsigned long long g_cocclusion_context_pre_sub_graph;
		extern unsigned long long g_cocclusion_context_post_sub_graph;
		extern unsigned long long g_cdxgi_swapchain_present_multiplane_overlay ;
		extern unsigned long long g_cdxgi_swapchain_present_dwm;
		extern unsigned long long g_cdxgi_swap_chain_dwm_legacy_present_dwm ;
		extern unsigned long long g_cddisplay_render_target_present;
		extern unsigned long long g_pswap_chain ;
		extern unsigned long long g_prender_target ;

		extern unsigned long long g_dxgk_get_device_state ;

		extern unsigned long long g_dxgk_open_resource;

		extern unsigned long long g_ki_call_user_mode2 ;

		extern bool g_kvashadow ;

		extern unsigned long long g_client_to_screen_fun ;
		extern unsigned long long g_get_client_rect_fun ;
		extern unsigned long long g_get_foreground_window_fun;
		extern unsigned long long g_find_windoww_fun;
		extern unsigned long long g_get_window_rect_fun ;
		extern unsigned long long g_get_keyboard_layout_fun ;
		extern unsigned long long g_get_locale_info_a_fun;
		extern unsigned long long  g_load_librarya_fun;
		extern unsigned long long g_get_proc_address_fun;
		extern unsigned long long g_d3dcompile_fun;
		extern unsigned long long g_set_cursor_pos_fun;
		extern unsigned long long g_screen_to_client_fun;
		extern unsigned long long g_get_cursor_pos_fun;
		extern unsigned long long g_get_key_state_fun;
		extern  unsigned long long g_set_cursor_fun ;
		extern unsigned long long g_load_cursora_fun;
		extern unsigned long long g_rtl_copy_memory_fun;
		extern PVOID g_game_utils_buffer;
		extern unsigned long long  g_imgui_buffer;

		
	
		
		NTSTATUS initialize();

		void finalize();

		NTSTATUS  initialize_dwm_utils_modules(_In_ PEPROCESS process);

		NTSTATUS   initialize_user_buffer(PEPROCESS process );

		NTSTATUS   initialize_ki_call_user_mode2(OUT unsigned long long * ki_call_user_mode2);

		

		NTSTATUS get_dwm_process(_Out_ PEPROCESS* process);

		NTSTATUS get_stack_offset();



		NTSTATUS find_precall_address(
			IN PEPROCESS process,
			IN ULONG_PTR ntdll_base,
			OUT ULONG_PTR* precall_addr_out);

		NTSTATUS find_postcall_address(
			IN PEPROCESS process,
			IN ULONG_PTR user32_base,
			OUT ULONG_PTR* postcall_addr_out);

		NTSTATUS find_context_offset(IN PEPROCESS process,
			IN unsigned long long ntdll_base,
			OUT unsigned long long* context_offset_out);

	 

		NTSTATUS find_ccomposition_present(
			IN PEPROCESS process,
			IN unsigned long long dwmcore_base,
			OUT unsigned long long* ccomposition_present_addr_out);


		NTSTATUS find_cocclusion_context_pre_sub_graph(
			IN PEPROCESS process,
			IN unsigned long long dwmcore_base,
			OUT unsigned long long* cocclusion_context_pre_sub_graph_addr_out);

		NTSTATUS find_cocclusion_context_post_sub_graph(
			IN PEPROCESS process,
			IN unsigned long long dwmcore_base,
			OUT unsigned long long* cocclusion_context_post_sub_graph_addr_out);

		NTSTATUS find_cdxgi_swapchain_present_dwm(
			IN PEPROCESS process,
			IN unsigned long long dxgi_base,
			OUT unsigned long long* cdxgi_swapchain_present_dwm_out);

		NTSTATUS  find_cdxgi_swapchain_present_multiplane_overlay(
			IN PEPROCESS process,
			IN unsigned long long dxgi_base,
			OUT unsigned long long*  cdxgi_swapchain_present_multiplane_overlay_out);

		NTSTATUS  find_cdxgi_swapchain_dwm_legacy_present_dwm(
			IN PEPROCESS process,
			IN unsigned long long dxgi_base,
			OUT unsigned long long* cdxgi_swap_chain_dwm_legacy_present_dwm_out);
	 
		NTSTATUS  find_cddisplay_render_target_present(
			IN PEPROCESS process,
			IN unsigned long long dwmcore_base,
			OUT unsigned long long* coverlaycontext_presentmpo_out);

		NTSTATUS find_find_windoww(
			IN PEPROCESS process,
			IN unsigned long long user32_base,
			OUT unsigned long long* find_windoww_addr);

		NTSTATUS find_client_to_screen(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* client_to_screen_addr
		);

		NTSTATUS find_get_client_rect(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* get_client_rect_addr
		);

		NTSTATUS find_get_foreground_window(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* get_foreground_window_addr
		);

		NTSTATUS find_get_window_rect(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* get_window_rect_addr);

		NTSTATUS find_get_keyboard_layout(_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* get_keyboard_layout_addr);

		NTSTATUS find_get_locale_info_a(
			_In_ PEPROCESS process,
			_In_ unsigned long long kernel32_base,
			_Out_ unsigned long long* get_locale_info_addr);

		NTSTATUS find_load_librarya(
			_In_ PEPROCESS process,
			_In_ unsigned long long kernel32_base,
			_Out_ unsigned long long* load_librarya_addr);

		NTSTATUS find_get_proc_address(
			_In_ PEPROCESS process,
			_In_ unsigned long long kernel32_base,
			_Out_ unsigned long long* get_proc_address_addr);

		NTSTATUS find_d3dcompile(
			_In_ PEPROCESS process,
			_In_ unsigned long long d3dcompiler_47_base,
			_Out_ unsigned long long* d3dcompile_addr
		);

		NTSTATUS find_set_cursor_pos(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* set_cursor_pos_addr);


		NTSTATUS find_screen_to_client(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* screen_to_client_addr);

		 
		NTSTATUS find_get_cursor_pos(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* get_cursor_pos_addr);

		NTSTATUS find_get_key_state(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* get_key_state_addr
		);

		NTSTATUS find_set_cursor(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* set_cursor_addr
		);


		NTSTATUS find_load_cursora(
			_In_ PEPROCESS process,
			_In_ unsigned long long user32_base,
			_Out_ unsigned long long* load_cursor_a_addr
		);
		NTSTATUS find_rtl_copy_memory(
			_In_ PEPROCESS process,
			_In_ unsigned long long kernel32_base,
			_Out_ unsigned long long* rtl_copy_memory_addr
		);

		NTSTATUS find_dxgk_get_device_state(
			IN PEPROCESS process, 
			IN unsigned long long dxgkrnl_base,
			OUT unsigned long long* dxgk_get_device_state);



		NTSTATUS   find_open_resource(
			IN PEPROCESS process,
			IN unsigned long long dxgkrnl_base, 
			OUT unsigned long long * dxgk_open_resource);



		NTSTATUS  hook_swapchain_present_dwm(IN PEPROCESS process);

		 NTSTATUS hook_present_multiplane_overlay(IN PEPROCESS process);

		 NTSTATUS hook_cdxgi_swapchain_dwm_legacy_present_dwm(IN PEPROCESS process);

		 NTSTATUS hook_cddisplay_render_target_present(IN PEPROCESS process);
		  
		 NTSTATUS  hook_get_buffer(IN PEPROCESS process);

		 NTSTATUS hook_cocclusion_context_pre_sub_graph(IN PEPROCESS process);

		 NTSTATUS  hook_cocclusion_context_post_sub_graph(IN PEPROCESS process);

		 NTSTATUS  hook_dxgk_get_device_state(IN PEPROCESS process);
	 
	 	 


		 
 	}

}