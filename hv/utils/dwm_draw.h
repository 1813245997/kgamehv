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
		extern unsigned long long g_ntdll_base ;
		extern unsigned long long g_ntdll_size ;
		extern unsigned long long g_user32_base ;
		extern unsigned long long g_user32_size ;
		extern unsigned long long g_dwmcore_base ;
		extern unsigned long long g_dwmcore_size ;
		extern unsigned long long g_dxgi_base ;
		extern unsigned long long g_dxgi_size ;
		extern unsigned long long g_offset_stack ;
		extern unsigned long long g_ccomposition_present ;
		extern unsigned long long g_cocclusion_context_post_sub_graph;
		extern unsigned long long g_cdxgi_swapchain_present_multiplane_overlay ;
		extern unsigned long long g_cdxgi_swap_chain_dwm_legacy_present_dwm ;
		extern unsigned long long g_pswap_chain ;

		extern unsigned long long g_dxgk_get_device_state ;

		extern unsigned long long g_ki_call_user_mode2 ;

		extern bool g_kvashadow ;

	
		
		NTSTATUS initialize();

		NTSTATUS  initialize_dwm_utils_modules(_In_ PEPROCESS process);

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

		NTSTATUS find_cocclusion_context_post_sub_graph(
			IN PEPROCESS process,
			IN unsigned long long dwmcore_base,
			OUT unsigned long long* cocclusion_context_post_sub_graph_addr_out);

		NTSTATUS  find_cdxgi_swapchain_present_multiplane_overlay(
			IN PEPROCESS process,
			IN unsigned long long dxgi_base,
			OUT unsigned long long*  cdxgi_swapchain_present_multiplane_overlay_out);

		NTSTATUS  find_cdxgi_swapchain_dwm_legacy_present_dwm(
			IN PEPROCESS process,
			IN unsigned long long dxgi_base,
			OUT unsigned long long* cdxgi_swap_chain_dwm_legacy_present_dwm_out);

		NTSTATUS find_dxgk_get_device_state(
			IN PEPROCESS process, 
			IN unsigned long long dxgkrnl_base,
			OUT unsigned long long* dxgk_get_device_state);




		 NTSTATUS hook_present_multiplane_overlay(IN PEPROCESS process);

		 NTSTATUS hook_cdxgi_swapchain_dwm_legacy_present_dwm(IN PEPROCESS process);

		 NTSTATUS  hook_get_buffer(IN PEPROCESS process);
 
		 NTSTATUS  hook_dxgk_get_device_state(IN PEPROCESS process);
	 
		 
 	}

}