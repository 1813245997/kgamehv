#pragma once

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
       
    }
}
