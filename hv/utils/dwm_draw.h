#pragma once
namespace utils
{
	namespace dwm_draw
	{
		extern PEPROCESS g_dwm_process;
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


		extern bool g_kvashadow ;
		
		NTSTATUS initialize();

		NTSTATUS initialize_user_modules(_In_ PEPROCESS process);

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

		 NTSTATUS hook_present_multiplane_overlay(IN PEPROCESS process);

		 bool  __fastcall new_present_multiplane_overlay(
			 _Inout_ PEXCEPTION_RECORD ExceptionRecord,
			 _Inout_ PCONTEXT ContextRecord);

		 extern  INT64(NTAPI* original_present_multiplane_overlay)(
			 void* thisptr,
			 PVOID dxgi_swap_chain,
			 unsigned int a3,
			 unsigned int a4,
			 int a5,
			 const void* a6,
			 PVOID64 a7,
			 unsigned int a8
			 );

		 NTSTATUS test_hook_r3_ept_hook_break_point_int3();
	 
		
 	}

}