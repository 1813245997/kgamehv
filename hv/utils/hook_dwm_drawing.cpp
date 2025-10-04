#include "../utils/global_defs.h"
#include "hook_dwm_drawing.h"

// Global variables
bool g_dwm_hooks_initialized = false;
bool g_kernel_dxresources_initialized = false;

PEPROCESS g_dwm_process{};

unsigned long long g_cocclusion_context_post_sub_graph{};
unsigned long long g_cdxgi_swapchain_present_multiplane_overlay{};
unsigned long long g_cdxgi_swapchain_dwm_legacy_present_dwm{};
unsigned long long g_cddisplay_render_target_present{};

 
 

namespace utils
{
    namespace hook_dwm_drawing
    {
     



   



        NTSTATUS initialize_dwm_drawing()
        {
            NTSTATUS status = STATUS_SUCCESS;

         
            // TODO: Implement DWM function symbol resolution
            // Need to dynamically resolve function addresses in dwmcore.dll
            // Since these are internal functions, need to use pattern scanning or other methods

            LogInfo("DWM hooks initialization started");
        
            status = utils::render::initialize_font();
            if (!NT_SUCCESS(status))
            {
                LogError("Failed to initialize font (0x%X).", status);
                return status;
            }
            LogInfo("Font initialized successfully.");

            status = get_dwm_process(&g_dwm_process);
            if (!NT_SUCCESS(status))
            {
                LogError("Failed to get dwm process (0x%X).", status);
                return status;
            }
            LogInfo("DWM process found successfully.");
            
            

             status = find_hook_dwm_drawing();
            if (!NT_SUCCESS(status))
            {
                LogError("Failed to find hook dwm drawing (0x%X).", status);
                return status;
            }
            LogInfo("Hook dwm drawing found successfully.");

            status = initialize_hook_dwm_drawing();
            if (!NT_SUCCESS(status))
            {
                LogError("Failed to initialize hook dwm drawing (0x%X).", status);
                return status;
            }
            LogInfo("Hook dwm drawing initialized successfully.");


            return STATUS_SUCCESS;
        }

 

        NTSTATUS find_hook_dwm_drawing()
        {

            NTSTATUS status = STATUS_SUCCESS;


            KAPC_STATE apc_state{};
			utils::internal_functions::pfn_ke_stack_attach_process(g_dwm_process, &apc_state);
           

            unsigned long long addr = scanner_fun::find_cocclusion_context_post_sub_graph(  reinterpret_cast<unsigned long long>(utils::module_info::dwmcore_base) );

            if (addr == 0)
            {
                LogError("Failed to find cocclusion context post subgraph (0x%X).", STATUS_NOT_FOUND);
                status = STATUS_NOT_FOUND;
                goto clean_up;
            }
            
            g_cocclusion_context_post_sub_graph = addr;
            LogInfo("Cocclusion context post subgraph found successfully.0x%llX", addr);

            addr = scanner_fun::find_cdxgi_swapchain_present_multiplane_overlay(reinterpret_cast<unsigned long long>(utils::module_info::dxgi_base));
            if (addr == 0)
            {
                LogError("Failed to find cdxgi swapchain present multiplane overlay (0x%X).", STATUS_NOT_FOUND);
                status = STATUS_NOT_FOUND;
                goto clean_up;
            }
            g_cdxgi_swapchain_present_multiplane_overlay = addr;
            LogInfo("Cdxgi swapchain present multiplane overlay found successfully.0x%llX", addr);


            addr = scanner_fun::find_cdxgi_swapchain_dwm_legacy_present_dwm(reinterpret_cast<unsigned long long>(utils::module_info::dxgi_base));
            if (addr == 0)
            {
                LogError("Failed to find cdxgi swapchain dwm legacy present dwm (0x%X).", STATUS_NOT_FOUND);
                status = STATUS_NOT_FOUND;
                goto clean_up;
            }
            g_cdxgi_swapchain_dwm_legacy_present_dwm = addr;
            LogInfo("Cdxgi swapchain dwm legacy present dwm found successfully.0x%llX", addr);

            if (utils::os_info::get_build_number() >= WINDOWS_11_VERSION_24H2)
            {
                addr = scanner_fun::find_cddisplay_render_target_present(reinterpret_cast<unsigned long long>(utils::module_info::dxgi_base));
                if (addr == 0)
                {
                    LogError("Failed to find cddisplay render target present (0x%X).", STATUS_NOT_FOUND);
                    status = STATUS_NOT_FOUND;
                    goto clean_up;
                }
                g_cddisplay_render_target_present = addr;
                LogInfo("Cddisplay render target present found successfully.0x%llX", addr);
            }

         clean_up:
            utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
            return status;
        }

        NTSTATUS initialize_hook_dwm_drawing()
        {
            NTSTATUS status = STATUS_SUCCESS;
            HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(g_dwm_process);
            LogInfo("Initializing hook dwm drawing...");
            
            
            LogInfo("Installing shadow break point for cocclusion context post subgraph...");
			
			if (!utils::shadowbreakpoint::shadowbp_install(process_id, reinterpret_cast<void*>(g_cocclusion_context_post_sub_graph), reinterpret_cast<void*>(handle_cocclusion_context_post_sub_graph)))
			{
				LogError("Failed to install shadow break point for cocclusion context post subgraph.");
				status = STATUS_UNSUCCESSFUL;
				goto clean_up;
			}
			LogInfo("Installed shadow break point for cocclusion context post subgraph.");

            LogInfo("Installing shadow break point for cdxgi swapchain present multiplane overlay...");
            if (!utils::shadowbreakpoint::shadowbp_install(process_id, reinterpret_cast<void*>(g_cdxgi_swapchain_present_multiplane_overlay), reinterpret_cast<void*>(handle_cdxgi_swapchain_present_multiplane_overlay)))
            {
                LogError("Failed to install shadow break point for cdxgi swapchain present multiplane overlay.");
                status = STATUS_UNSUCCESSFUL;
                goto clean_up;
            }
            LogInfo("Installed shadow break point for cdxgi swapchain present multiplane overlay.");

            LogInfo("Installing shadow break point for cdxgi swapchain dwm legacy present dwm...");
            if (!utils::shadowbreakpoint::shadowbp_install(process_id, reinterpret_cast<void*>(g_cdxgi_swapchain_dwm_legacy_present_dwm), reinterpret_cast<void*>(handle_cdxgi_swapchain_dwm_legacy_present_dwm)))
            {
                LogError("Failed to install shadow break point for cdxgi swapchain dwm legacy present dwm.");
                status = STATUS_UNSUCCESSFUL;
                goto clean_up;
            }
            LogInfo("Installed shadow break point for cdxgi swapchain dwm legacy present dwm.");

            if (utils::os_info::get_build_number() >= WINDOWS_11_VERSION_24H2)
            {
                LogInfo("Installing shadow break point for cddisplay render target present...");
                if (!utils::shadowbreakpoint::shadowbp_install(process_id, reinterpret_cast<void*>(g_cddisplay_render_target_present), reinterpret_cast<void*>(handle_cddisplay_render_target_present)))
                {
                    LogError("Failed to install shadow break point for cddisplay render target present.");
                    status = STATUS_UNSUCCESSFUL;
                    goto clean_up;
                }
                LogInfo("Installed shadow break point for cddisplay render target present.");
            }
            
        clean_up:
            return status;

          
            
        }
 

        NTSTATUS get_dwm_process(_Out_ PEPROCESS * out_process)
		{
			PEPROCESS process{};

			if (!out_process)
			{
				return STATUS_INVALID_PARAMETER;
			}

			if (!process_utils::get_process_by_name(L"dwm.exe", &process))
			{
				return STATUS_NOT_FOUND;
			}
			utils::internal_functions::pfn_ob_dereference_object(process);
			*out_process = process;
			return STATUS_SUCCESS;
		}


        NTSTATUS handle_cocclusion_context_post_sub_graph(_Inout_ PCONTEXT ContextRecord)
        {

           // DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "handle_cocclusion_context_post_sub_graph\n");
            return STATUS_SUCCESS;
        }

        NTSTATUS handle_cdxgi_swapchain_present_multiplane_overlay(_Inout_ PCONTEXT ContextRecord)
        {
           
           // draw_every_thing(reinterpret_cast<PVOID>(ContextRecord->Rcx));
           // DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "handle_cdxgi_swapchain_present_multiplane_overlay\n");
            return STATUS_SUCCESS;
        }   

        NTSTATUS handle_cdxgi_swapchain_dwm_legacy_present_dwm(_Inout_ PCONTEXT ContextRecord)
        {
            
             draw_every_thing(reinterpret_cast<PVOID>(ContextRecord->Rcx));
           // DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "handle_cdxgi_swapchain_dwm_legacy_present_dwm\n");
            return STATUS_SUCCESS;
        }

        NTSTATUS handle_cddisplay_render_target_present(_Inout_ PCONTEXT ContextRecord)
        {
            
            //draw_every_thing(reinterpret_cast<PVOID>(ContextRecord->Rcx));
            //DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "handle_cddisplay_render_target_present\n");
            return STATUS_SUCCESS;
        }
     


        NTSTATUS draw_every_thing(PVOID p_dxgi_swapchain  )
        {
           //存在切换分辨率失效的问题

            if (!p_dxgi_swapchain)
            {
                return STATUS_INVALID_PARAMETER;
            }
            
            
            if (!g_kernel_dxresources_initialized)
            {
               
            
                if (!NT_SUCCESS(utils::render::initialize_dx_resources(p_dxgi_swapchain)))
                {
                    return STATUS_UNSUCCESSFUL;
                }
                g_kernel_dxresources_initialized = true;
            }
            else
            {
                utils::render::render_every_thing(p_dxgi_swapchain);

            }

            
            return STATUS_SUCCESS;
        }


    }
}
