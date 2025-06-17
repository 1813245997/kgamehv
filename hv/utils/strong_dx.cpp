#include "global_defs.h"
#include "strong_dx.h"
#include "../ia32-doc/ia32.hpp"
#include "dx_draw/LegacyRender.h"
#include "dx11.h"


#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)

namespace utils
{
	namespace  strong_dx
	{    

			
			bool g_initialized{};
			PVOID g_desc_buffer{};
			PVOID g_entity_buffer{};
			PVOID g_swap_chain{};
			PVOID  g_pdevice{};
			PVOID g_pContext{};
			PVOID g_pRenderTargetView;
			PVOID g_Surface{};
		 
		NTSTATUS initialize(unsigned long long pswap_chain)
		{
			NTSTATUS status{};
			PVOID get_device_fun{};
			PVOID get_immediate_context_fun{};
			PVOID get_buffer_fun{};
			PVOID desc_buffer_local{};
			PVOID entity_buffer_local{};
			 
			HRESULT  hr{};

			const GUID ID3D11DeviceVar = { 0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40 };
			const GUID ID3D11Texture2DVar = { 0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c };
			if (g_initialized)
			{
				return STATUS_SUCCESS;
			}
			g_swap_chain = reinterpret_cast<PVOID> (pswap_chain);



			status = memory::allocate_user_memory(&desc_buffer_local, 0x1000);
			if (!NT_SUCCESS(status))
			{
				return status;
			}
			g_desc_buffer = desc_buffer_local;
			status = memory::allocate_user_memory(&entity_buffer_local, 0x1000);
			if (!NT_SUCCESS(status))
			{
				return status;
			}
			g_entity_buffer = desc_buffer_local;

		 
			memory::mem_copy(desc_buffer_local, (PVOID)&ID3D11DeviceVar, sizeof(ID3D11DeviceVar));

		     get_device_fun =   utils::vfun_utils::get_vfunc(g_swap_chain, 7);

				 hr = user_call::call(
				reinterpret_cast<unsigned long long> (get_device_fun),
				pswap_chain,
				reinterpret_cast<unsigned long long> (desc_buffer_local),
				reinterpret_cast<unsigned long long> (entity_buffer_local),
				0);

			if (FAILED(hr))
			{
				return STATUS_DEVICE_DOES_NOT_EXIST;
				
			}

			g_pdevice = *reinterpret_cast<PVOID*>(entity_buffer_local);
			//vfun_utils::release(g_pdevice);
			get_immediate_context_fun = utils::vfun_utils::get_vfunc(g_pdevice, 40);

			memory::mem_zero(entity_buffer_local, 0X1000);

		    user_call::call(
				reinterpret_cast<unsigned long long> (get_immediate_context_fun),
				reinterpret_cast<unsigned long long> (g_pdevice),
				reinterpret_cast<unsigned long long>(entity_buffer_local),
				0,
				0);

			g_pContext = *(PVOID*)entity_buffer_local;
			//vfun_utils::release(g_pContext);
			memory::mem_zero(desc_buffer_local, 0X1000);

			memory::mem_zero(entity_buffer_local, 0X1000);

			memory::mem_copy(desc_buffer_local, (PVOID)&ID3D11Texture2DVar, sizeof(ID3D11Texture2DVar));

			get_buffer_fun = utils::vfun_utils::get_vfunc(g_swap_chain, 9);
			
			hr = user_call::call(
				reinterpret_cast<unsigned long long> (get_buffer_fun),
				reinterpret_cast<unsigned long long> (g_swap_chain),
				reinterpret_cast<unsigned long long> (desc_buffer_local),
				reinterpret_cast<unsigned long long> (entity_buffer_local),
				0);
			if (FAILED(hr))
			{
					return STATUS_DEVICE_DOES_NOT_EXIST;

			}
			g_Surface = *(PVOID*)entity_buffer_local;
			 
			//vfun_utils::release(renderTargetTexture);
			 
		 
			g_initialized = true;
			

			return STATUS_SUCCESS;
		}

		void draw_utils()
		{
			 
			if (!NT_SUCCESS(initialize(dwm_draw::g_pswap_chain)))
			{
				return;
			}

			HRESULT  hr{};
			D3D11_TEXTURE2D_DESC SDesc{};
			D3D11_MAPPED_SUBRESOURCE MapRes{};
			unsigned  long long  get_desc_fun{};
			unsigned  long long copy_resource_fun{};
			unsigned  long long create_texture2D_fun{};
			unsigned  long long map_fun{};
			unsigned  long long umap_fun{};
		
			    get_desc_fun =	reinterpret_cast<unsigned long long > (utils::vfun_utils::get_vfunc(g_Surface,10));
			memory::mem_zero(g_desc_buffer, 0X1000);
			user_call::call(
				get_desc_fun,
				reinterpret_cast<unsigned long long>(g_Surface),
				reinterpret_cast<unsigned long long>(g_desc_buffer),
				0,
				0
			);

			SDesc = *(D3D11_TEXTURE2D_DESC*)g_desc_buffer;

			SDesc.BindFlags = 0;
			SDesc.MiscFlags = 0;
			SDesc.Usage = D3D11_USAGE_STAGING;
			SDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

			memory::mem_zero(g_desc_buffer, 0X1000);
			memory::mem_copy(g_desc_buffer, &SDesc, sizeof(SDesc));


			   create_texture2D_fun = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(g_pdevice, 5));

			hr =	user_call::call(
				create_texture2D_fun,
				reinterpret_cast<unsigned long long>(g_pdevice),
				reinterpret_cast<unsigned long long>(g_desc_buffer),
				0,
				reinterpret_cast<unsigned long long>(g_entity_buffer)
			);

			if (FAILED(hr))
			{
				return;
			}
			 
			PVOID pTexture = *(PVOID*)g_entity_buffer;

			  copy_resource_fun  = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(g_pContext, 47));

			user_call::call(
				copy_resource_fun,
				reinterpret_cast<unsigned long long>(g_pContext),
				reinterpret_cast<unsigned long long>(pTexture),
				reinterpret_cast<unsigned long long>(g_Surface)
				,0
			);

			memory::mem_zero(g_desc_buffer, 0X1000);
			map_fun = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(g_pContext, 14));

			user_call::call6(
				map_fun,
				reinterpret_cast<unsigned long long>(g_pContext),
				reinterpret_cast<unsigned long long>(pTexture),
				0,
				D3D11_MAP_READ_WRITE,
				0,
				reinterpret_cast<unsigned long long>(g_desc_buffer)
			);

			MapRes = *(D3D11_MAPPED_SUBRESOURCE*)g_desc_buffer;
			memset(pagehit, 0, sizeof(pagehit));
			memset(pagevaild, 0, sizeof(pagevaild));

			cr4 cr4vlaue{ __readcr4() };
			bool smap = cr4vlaue.smap_enable;
			if (smap == true) {
				cr4vlaue.smap_enable = false;
				__writecr4(cr4vlaue.flags);
			}

			  
			ByteRender rend;
			rend.Setup(SDesc.Width, SDesc.Height, MapRes.Data);
			rend.Line({ 100, 200 }, { 500, 200 }, FColor(__rdtsc()), 1);
			//rend.String(&g_Font, { 100, 200 }, L"https://github.com/cs1ime", PM_XRGB(255, 0, 0));
			if (smap == true) {

				cr4vlaue.smap_enable = true;
				__writecr4(cr4vlaue.flags);
			}

			umap_fun = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(g_pContext, 15));
			user_call::call(
				umap_fun,
				reinterpret_cast<unsigned long long>(g_pContext),
				reinterpret_cast<unsigned long long>(pTexture),
				0,
				0
			);

			user_call::call(
				copy_resource_fun,
				reinterpret_cast<unsigned long long>(g_pContext),
				reinterpret_cast<unsigned long long>(g_Surface),
				reinterpret_cast<unsigned long long>(pTexture)
				, 0
			);
			vfun_utils::release(pTexture);
			vfun_utils::release(g_Surface);
		}

	}
}

