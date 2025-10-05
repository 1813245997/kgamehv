#include "global_defs.h"
#include "render.h"
 
 
namespace utils
{
	namespace render
	{
        PVOID g_pSwapChain{};
        PVOID g_pd3dDevice{};
        PVOID g_pD3DXDeviceCtx{};
        PVOID g_pSurface{};
        PVOID g_pRenderTargetView{};
        PVOID g_pRasterizerState{};
        Font* g_Font = nullptr;
        
       
        static PVOID g_last_swapchain = nullptr;
        static bool g_need_reinit = false;
		HRESULT(__fastcall* pfn_get_dxgi_device)(PVOID p_dxgi_swapchain, _In_  REFIID riid, _Out_ void** ppDevice);
		void(__fastcall* pfn_get_immediate_context)(PVOID p_dxgi_device, _Out_ void** ppContext);
		HRESULT(__fastcall* pfn_get_buffer)(PVOID p_dxgi_swapchain, _In_  REFIID riid, _Out_ void** ppBuffer);

        NTSTATUS initialize_font()
        {
            g_Font = (Font*)utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, sizeof(Font), 'FONT');
            if (!g_Font)
            {
                return STATUS_UNSUCCESSFUL;
             }
            RtlZeroMemory(g_Font, sizeof(Font));
            if (!g_Font->InitFont(fontdata::data, sizeof(fontdata::data)))
            {
                return STATUS_UNSUCCESSFUL;
            }
            
           
            g_Font->SetScale(1.0f);
            
            
            return STATUS_SUCCESS;
        }

      
        NTSTATUS set_font_size(float scale)
        {
            if (!g_Font)
            {
                return STATUS_UNSUCCESSFUL;
            }
            
            g_Font->SetScale(scale);
            LogInfo("Font scale set to: %.2f", scale);
            return STATUS_SUCCESS;
        }

         
        float get_font_size()
        {
            if (!g_Font)
            {
                return 1.0f;
            }
            
            return g_Font->GetScale();
        }

      
        float get_recommended_font_size(int screen_width, int screen_height)
        {
            
            if (screen_width >= 3840) {
                return 2.0f;   
            } else if (screen_width >= 2560) {
                return 1.5f;  
            } else if (screen_width >= 1920) {
                return 1.2f;  
            } else if (screen_width >= 1366) {
                return 1.0f;  
            } else if (screen_width >= 1024) {
                return 0.8f;   
            } else {
                return 0.6f;  
            }
        }

        
        NTSTATUS set_smart_font_size(int screen_width, int screen_height)
        {
            if (!g_Font)
            {
                return STATUS_UNSUCCESSFUL;
            }
            
            float recommended_size = get_recommended_font_size(screen_width, screen_height);
            g_Font->SetScale(recommended_size);
            LogInfo("Smart font size set to: %.2f for resolution: %dx%d", recommended_size, screen_width, screen_height);
            return STATUS_SUCCESS;
        }

        
        bool check_swapchain_changed(PVOID p_dxgi_swapchain)
        {
            if (p_dxgi_swapchain != g_last_swapchain)
            {
                
                g_last_swapchain = p_dxgi_swapchain;
                g_need_reinit = true;
                return true;
            }
            return false;
        }

        
        NTSTATUS cleanup_dx_resources()
        {
            LogInfo("Cleaning up DirectX resources...");
            
            
            if (g_pRenderTargetView)
            {
                utils::vfun_utils::release(g_pRenderTargetView);
                g_pRenderTargetView = nullptr;
            }
            
             
            if (g_pSurface)
            {
                utils::vfun_utils::release(g_pSurface);
                g_pSurface = nullptr;
            }
            
           
            g_pD3DXDeviceCtx = nullptr;
            g_pd3dDevice = nullptr;
            g_pSwapChain = nullptr;
            
            LogInfo("DirectX resources cleaned up");
            return STATUS_SUCCESS;
        }

        
        NTSTATUS reinitialize_dx_resources(PVOID p_dxgi_swapchain)
        {
            if (!g_need_reinit)
            {
                return STATUS_SUCCESS;
            }
            
            LogInfo("Reinitializing DirectX resources for new SwapChain...");
            
           
            cleanup_dx_resources();
            
           
            NTSTATUS status = initialize_dx_resources(p_dxgi_swapchain);
            if (NT_SUCCESS(status))
            {
                g_need_reinit = false;
                LogInfo("DirectX resources reinitialized successfully");
            }
            else
            {
                LogError("Failed to reinitialize DirectX resources");
            }
            
            return status;
        }

        NTSTATUS initialize_dx_resources(PVOID p_dxgi_swapchain)
        {
            const GUID ID3D11DeviceVar = { 0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40 };
            const GUID ID3D11Texture2DVar = { 0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c };

            static unsigned long long g_user_buffer = 0;
            if (!g_user_buffer)
            {
                // Optimized buffer size: GUID(16)*2 + PVOID(8)*4 + padding = ~0x600 bytes  
                NTSTATUS status = utils::memory::allocate_user_memory(reinterpret_cast<PVOID*>(&g_user_buffer), 0x10000, PAGE_READWRITE, true, false);
                if (!NT_SUCCESS(status))
                {
                    return STATUS_UNSUCCESSFUL;
                }
            }

            g_pSwapChain = p_dxgi_swapchain;


            // Use offset 0x200 for first GUID
            PVOID guid_buffer_1 = reinterpret_cast<PVOID>(g_user_buffer + 0x200);
            memory::mem_copy(guid_buffer_1, (PVOID)&ID3D11DeviceVar, sizeof(ID3D11DeviceVar));
            PVOID device_buffer = reinterpret_cast<PVOID>(g_user_buffer + 0x250);
            *reinterpret_cast<PVOID*>(device_buffer) = g_pd3dDevice;

            pfn_get_dxgi_device =  reinterpret_cast<HRESULT (__fastcall *)(PVOID, REFIID, void **)>(utils::vfun_utils::get_vfunc(g_pSwapChain, 7));
          
          
             unsigned long long return_ptr = user_call::call(
                reinterpret_cast<unsigned long long>(pfn_get_dxgi_device),
                reinterpret_cast<unsigned long long>(g_pSwapChain),
                reinterpret_cast<unsigned long long>(guid_buffer_1),
                reinterpret_cast<unsigned long long>(device_buffer),
                0);

                HRESULT hr = *reinterpret_cast<HRESULT*>(return_ptr);
                if (FAILED(hr))
                {
                    return STATUS_UNSUCCESSFUL;
                }

                g_pd3dDevice = *(PVOID*)(device_buffer);

             

                // Use offset 0x300 for immediate context buffer
                PVOID context_buffer = reinterpret_cast<PVOID>(g_user_buffer + 0x300);
                pfn_get_immediate_context =  reinterpret_cast<void (__fastcall*)(PVOID, void **)>(utils::vfun_utils::get_vfunc(g_pd3dDevice, 40));
                user_call::call(
                    reinterpret_cast<unsigned long long>(pfn_get_immediate_context),
                    reinterpret_cast<unsigned long long>(g_pd3dDevice),
                    reinterpret_cast<unsigned long long>(context_buffer),
                    0,
                    0);

                g_pD3DXDeviceCtx = *(PVOID*)(context_buffer);

                // Use offset 0x400 for second GUID
                PVOID guid_buffer_2 = reinterpret_cast<PVOID>(g_user_buffer + 0x400);
                memory::mem_copy(guid_buffer_2, (PVOID)&ID3D11Texture2DVar, sizeof(ID3D11Texture2DVar));

                PVOID surface_buffer = reinterpret_cast<PVOID>(g_user_buffer + 0x450);
                *reinterpret_cast<PVOID*>(surface_buffer) = g_pSurface;
                pfn_get_buffer =  reinterpret_cast<HRESULT (__fastcall*)(PVOID, REFIID, void **)>(utils::vfun_utils::get_vfunc(g_pSwapChain, 9));
                  return_ptr = user_call::call(
                    reinterpret_cast<unsigned long long>(pfn_get_buffer),
                    reinterpret_cast<unsigned long long>(g_pSwapChain),
                    0,
                    reinterpret_cast<unsigned long long>(guid_buffer_2),
                    reinterpret_cast<unsigned long long>(surface_buffer));


                hr = *reinterpret_cast<HRESULT*>(return_ptr);
                if (FAILED(hr))
                {
                    return STATUS_UNSUCCESSFUL;
                }

                g_pSurface = *(PVOID*)(surface_buffer);

 


               // utils::vfun_utils::release(g_pSurface);

              
         
               return STATUS_SUCCESS;
        }

        NTSTATUS render_every_thing(PVOID p_dxgi_swapchain)
        {
             
            if (check_swapchain_changed(p_dxgi_swapchain))
            {
                
                NTSTATUS status = reinitialize_dx_resources(p_dxgi_swapchain);
                if (!NT_SUCCESS(status))
                {
                    LogError("Failed to reinitialize DirectX resources");
                    return STATUS_UNSUCCESSFUL;
                }
            }

            HRESULT hr{};
            D3D11_TEXTURE2D_DESC SDesc{};
            D3D11_MAPPED_SUBRESOURCE MapRes{};
            unsigned long long retval_ptr{};
            RtlZeroMemory(&SDesc, sizeof(SDesc));
            RtlZeroMemory(&MapRes, sizeof(MapRes));

            static unsigned long long g_user_buffer = 0;
            if (!g_user_buffer)
            {
                // Optimized buffer size: SDesc(32) + texture_ptr(8) + MapRes(32) + padding = ~0x1000 bytes 
                NTSTATUS status = utils::memory::allocate_user_memory(reinterpret_cast<PVOID*>(&g_user_buffer), 0x10000, PAGE_READWRITE, true, false);
                if (!NT_SUCCESS(status))
                {
                    return STATUS_UNSUCCESSFUL;
                }
            }
            auto get_desc_fun =  utils::vfun_utils::get_vfunc( g_pSurface, 10) ;


             unsigned long long   sdescbuffer = g_user_buffer ;

            retval_ptr = user_call::call(
                 get_desc_fun ,
                reinterpret_cast<unsigned long long>( g_pSurface),
                sdescbuffer ,
                0,
                0);
            
                SDesc = *(D3D11_TEXTURE2D_DESC*)sdescbuffer;
                SDesc.BindFlags = 0;
                SDesc.MiscFlags = 0;
				SDesc.Usage = D3D11_USAGE_STAGING;
				SDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
                memory::mem_copy( reinterpret_cast<PVOID>(sdescbuffer), &SDesc, sizeof(SDesc));
                

                auto create_texture2D_fun = utils::vfun_utils::get_vfunc(g_pd3dDevice, 5);

                // Allocate buffer for texture pointer
                PVOID texture_ptr_buffer = reinterpret_cast<PVOID>(g_user_buffer + 0x100);
                *reinterpret_cast<unsigned long long*>(texture_ptr_buffer) = 0; // Initialize to NULL

                // Call CreateTexture2D
                retval_ptr  = user_call::call(
                    create_texture2D_fun,
                    reinterpret_cast<unsigned long long>(g_pd3dDevice),
                    sdescbuffer,  // &textureDesc
                    0,            // NULL
                    reinterpret_cast<unsigned long long>(texture_ptr_buffer)  // &pTexture
                );

                // Get the created texture pointer
                PVOID pTexture = *reinterpret_cast<PVOID*>(texture_ptr_buffer);

                hr = *reinterpret_cast<HRESULT*>(retval_ptr);

                if (FAILED(hr)) {
                     
                    return STATUS_UNSUCCESSFUL;
                }

                auto copy_resource_fun =   utils::vfun_utils::get_vfunc(g_pD3DXDeviceCtx, 47) ;

                user_call::call(
                    copy_resource_fun,
                    reinterpret_cast<unsigned long long>(g_pD3DXDeviceCtx),
                    reinterpret_cast<unsigned long long>(pTexture),
                    reinterpret_cast<unsigned long long>(g_pSurface),
                    0
                );

                auto map_fun =   utils::vfun_utils::get_vfunc(g_pD3DXDeviceCtx, 14) ;

                unsigned long long map_buffer = g_user_buffer+ 0x1000;
                retval_ptr = user_call::call6(
                    map_fun,
                    reinterpret_cast<unsigned long long>(g_pD3DXDeviceCtx),
                    reinterpret_cast<unsigned long long>(pTexture),
                    0,
                    D3D11_MAP_READ_WRITE,
                    0,
                    map_buffer
                );

                hr = *reinterpret_cast<HRESULT*>(retval_ptr);

                if (FAILED(hr)) {
                     
                    utils::vfun_utils::release(pTexture);
                    return STATUS_UNSUCCESSFUL;
                }


               memory::mem_copy(&MapRes, reinterpret_cast<PVOID>(map_buffer), sizeof(MapRes));

                if (SDesc.Width && SDesc.Height && MapRes.pData)
                {
                      draw_overlay_elements(SDesc.Width, SDesc.Height, MapRes.pData);
                }

                auto unmap_fun =   utils::vfun_utils::get_vfunc(g_pD3DXDeviceCtx, 15) ;
                user_call::call(
                    unmap_fun,
                    reinterpret_cast<unsigned long long>(g_pD3DXDeviceCtx),
                    reinterpret_cast<unsigned long long>(pTexture),
                    0,
                    0
                );

                user_call::call(
                    copy_resource_fun,
                    reinterpret_cast<unsigned long long>(g_pD3DXDeviceCtx),
                    reinterpret_cast<unsigned long long>(g_pSurface),
                    reinterpret_cast<unsigned long long>(pTexture),
                    0
                );

                utils::vfun_utils::release(pTexture);
               
            
                return STATUS_SUCCESS;
            
        }

        NTSTATUS draw_overlay_elements(int width, int height, void* data)
        {
			memset(g_pagehit, 0, sizeof(g_pagehit));
			memset(g_pagevaild, 0, sizeof(g_pagevaild));


            return game::game_cheat::game_draw_every_thing(width, height, data);
        }
	}
}