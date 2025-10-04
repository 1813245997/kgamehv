#pragma once

 
extern bool g_dwm_hooks_initialized  ;
extern  bool g_kernel_dxresources_initialized ;

extern  PEPROCESS g_dwm_process ;

extern  unsigned long long g_cocclusion_context_post_sub_graph ;
extern  unsigned long long g_cdxgi_swapchain_present_multiplane_overlay ;
extern  unsigned long long g_cdxgi_swapchain_dwm_legacy_present_dwm ;
extern  unsigned long long g_cddisplay_render_target_present ;





namespace utils
{
    namespace hook_dwm_drawing
    {
     


       
        // Initialization functions
        NTSTATUS initialize_dwm_drawing();
 
        NTSTATUS find_hook_dwm_drawing();

        NTSTATUS initialize_hook_dwm_drawing();
     
        NTSTATUS get_dwm_process(_Out_ PEPROCESS * out_process);
  
        NTSTATUS handle_cocclusion_context_post_sub_graph(_Inout_ PCONTEXT ContextRecord);

        NTSTATUS handle_cdxgi_swapchain_present_multiplane_overlay(_Inout_ PCONTEXT ContextRecord);

        NTSTATUS handle_cdxgi_swapchain_dwm_legacy_present_dwm(_Inout_ PCONTEXT ContextRecord);

        NTSTATUS handle_cddisplay_render_target_present(_Inout_ PCONTEXT ContextRecord);
        
        NTSTATUS  draw_every_thing(PVOID p_dxgi_swapchain);


      

        
    }
}
