//#include "../global_defs.h"
//#include "imgui_utils.h"
//#include "../dx11.h"
//namespace utils
//{
//	namespace imgui_uitls
//	{
//#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
//#define FAILED(hr) (((HRESULT)(hr)) < 0)
//
//		static unsigned long long g_pd3dDevice{};
//		static unsigned long long g_pd3dDeviceContext{};
//		static unsigned long long g_pSwapChain{};
//		static unsigned long long g_mainRenderTargetView{};
//		static unsigned long long g_user_buffer{};
//		static unsigned long long g_texture_buffer{};
//
//
//		bool ImGuiInit( unsigned  long long * pSwapChain)
//		{
//			NTSTATUS status{};
//			PVOID get_device_fun{};
//			PVOID get_immediate_context_fun{};
//			PVOID get_buffer_fun{};
//			PVOID desc_buffer_local{};
//			PVOID texture_buffer{};
//			unsigned  long long  get_desc_fun{};
//			unsigned  long long copy_resource_fun{};
//			unsigned  long long create_texture2D_fun{};
//			unsigned  long long map_fun{};
//			unsigned  long long umap_fun{};
//			unsigned long long CreateRenderTargetView_fun{};
//			unsigned long long   usercall_retval_ptr{};
//			
//			D3D11_TEXTURE2D_DESC SDesc{};
//			D3D11_MAPPED_SUBRESOURCE MapRes{};
//
//			unsigned long long  RenderTargetTexture{};
//
//
//			const GUID ID3D11DeviceVar = { 0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40 };
//			const GUID ID3D11Texture2DVar = { 0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c };
//			HRESULT  hr{};
//
//
//			if (!pSwapChain)
//			{
//				return false;
//			}
//
//			g_pSwapChain = reinterpret_cast<unsigned long long> (pSwapChain);
//
//			if (!g_user_buffer)
//			{
//				status = memory::allocate_user_memory(&desc_buffer_local, 0x1000, PAGE_READWRITE, true, true);
//				if (!NT_SUCCESS(status))
//				{
//					return false;
//				}
//				g_user_buffer = reinterpret_cast<unsigned long long> (desc_buffer_local);
//			}
//
//			if (!g_texture_buffer)
//			{
//
//				status = memory::allocate_user_memory(&texture_buffer, 0x4000, PAGE_READWRITE, true, true);
//				if (!NT_SUCCESS(status))
//				{
//					return false;
//				}
//				g_texture_buffer = reinterpret_cast<unsigned long long> (texture_buffer);
//			}
//
// 
//				memory::mem_copy(reinterpret_cast<PVOID>  (g_user_buffer), (PVOID)&ID3D11DeviceVar, sizeof(ID3D11DeviceVar));
//
//				get_device_fun = utils::vfun_utils::get_vfunc(reinterpret_cast<PVOID> (g_pSwapChain), 7);
//
//				hr = user_call::call_ret_hr(
//					reinterpret_cast<unsigned long long> (get_device_fun),
//					g_pSwapChain,
//					g_user_buffer,
//					g_user_buffer + sizeof(ID3D11DeviceVar),
//					0);
//
//				 
//				if (FAILED(hr))
//				{
//					return false;
//					
//				}
//
//			
//				g_pd3dDevice = *(unsigned long long*)(g_user_buffer + sizeof(ID3D11DeviceVar));
// 
//				get_immediate_context_fun = utils::vfun_utils::get_vfunc(reinterpret_cast<PVOID> (g_pd3dDevice), 40);
//				user_call::call(
//					reinterpret_cast<unsigned long long> (get_immediate_context_fun),
//					g_pd3dDevice,
//					g_user_buffer,
//					0,
//					0);
//
//				g_pd3dDeviceContext = *reinterpret_cast<unsigned long long*> (g_user_buffer);
//				 
//
//		  
//			 
//				memory::mem_copy((PVOID)g_user_buffer, (PVOID)&ID3D11Texture2DVar, sizeof(ID3D11Texture2DVar));
//
//				get_buffer_fun = utils::vfun_utils::get_vfunc(reinterpret_cast<PVOID> (g_pSwapChain), 9);
//
//				hr = user_call::call_ret_hr(
//					reinterpret_cast<unsigned long long> (get_buffer_fun),
//					g_pSwapChain,
//					0,
//					g_user_buffer,
//					g_user_buffer + sizeof(ID3D11Texture2DVar));
//				 
//				if (SUCCEEDED(hr))
//				{
//					RenderTargetTexture = *reinterpret_cast<unsigned long long*>(g_user_buffer + sizeof(ID3D11Texture2DVar));
//
//					CreateRenderTargetView_fun = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(reinterpret_cast<PVOID> (g_pd3dDevice), 9));
//					user_call::call(
//					 CreateRenderTargetView_fun ,
//						g_pd3dDevice,
//						RenderTargetTexture,
//						0,
//						g_user_buffer);
//
//					g_mainRenderTargetView = *reinterpret_cast<unsigned long long*> (g_user_buffer);
//					if (!g_mainRenderTargetView)
//					{
//						return false;
//					}
//
//					vfun_utils::release( reinterpret_cast<PVOID> (RenderTargetTexture));
//				}
//				
//			 
//
//
//
//
//
//			return false;
//		}
//	 }
//}
//
//
