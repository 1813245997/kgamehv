#include "global_defs.h"
#include "strong_dx.h"
#include "../ia32-doc/ia32.hpp"
#include "dx_draw/LegacyRender.h"
#include "dx11.h"
#include "../arch.h"

#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)

namespace utils
{
	namespace  strong_dx
	{    

			
			bool g_initialized{};
			unsigned long long g_user_buffer{};
			unsigned long long  g_texture_buffer{};
			PVOID g_swap_chain{};
			PVOID  g_pdevice{};
			PVOID g_pContext{};
			PVOID g_pRenderTargetView;
			PVOID g_Surface{};
			
			bool g_should_hide_overlay = false;
		   volatile LONG g_dwm_render_lock = 0;

		   static ULONG_PTR g_slient_start_time = 0;
		   static ULONG_PTR g_PreviousRenderTime = 0;

		 
	 
		void render_overlay_frame(void (*draw_callback)(int width, int height, void* data))
		{

			NTSTATUS status{};
			PVOID get_device_fun{};
			PVOID get_immediate_context_fun{};
			PVOID get_buffer_fun{};
			PVOID desc_buffer_local{};
			PVOID texture_buffer{};
			unsigned  long long  get_desc_fun{};
			unsigned  long long copy_resource_fun{};
			unsigned  long long create_texture2D_fun{};
			unsigned  long long map_fun{};
			unsigned  long long umap_fun{};
			unsigned long long   usercall_retval_ptr{};
			D3D11_TEXTURE2D_DESC SDesc{};
			D3D11_MAPPED_SUBRESOURCE MapRes{};




			const GUID ID3D11DeviceVar = { 0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40 };
			const GUID ID3D11Texture2DVar = { 0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c };
			HRESULT  hr{};


			 
				if (!utils::dwm_draw::g_pswap_chain)
				{
					return;
				}
				g_swap_chain = reinterpret_cast<PVOID> (utils::dwm_draw::g_pswap_chain);


				if (!g_user_buffer)
				{
					status = memory::allocate_user_memory(&desc_buffer_local, 0x1000, PAGE_READWRITE, true, true);
					if (!NT_SUCCESS(status))
					{
						return;
					}
					g_user_buffer = reinterpret_cast<unsigned long long> (desc_buffer_local);
				}

				if (!g_texture_buffer)
				{

					status = memory::allocate_user_memory(&texture_buffer, 0x4000, PAGE_READWRITE, true,true);
					if (!NT_SUCCESS(status))
					{
						return;
					}
					g_texture_buffer = reinterpret_cast<unsigned long long> (texture_buffer);
				}

				if (!g_pdevice)
				{
					memory::mem_copy(reinterpret_cast<PVOID>  (g_user_buffer), (PVOID)&ID3D11DeviceVar, sizeof(ID3D11DeviceVar));

					get_device_fun = utils::vfun_utils::get_vfunc(g_swap_chain, 7);

					usercall_retval_ptr = user_call::call(
						reinterpret_cast<unsigned long long> (get_device_fun),
						reinterpret_cast<unsigned long long> (g_swap_chain),
						g_user_buffer,
						g_user_buffer + sizeof(ID3D11DeviceVar),
						0);

					hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
					if (FAILED(hr))
					{
						return;

					}

					g_pdevice = *(PVOID*)(g_user_buffer + sizeof(ID3D11DeviceVar));
				}
				  
				
					//vfun_utils::release(g_pdevice);
				 

				if (!g_pContext)
				{
					get_immediate_context_fun = utils::vfun_utils::get_vfunc(g_pdevice, 40);
					user_call::call(
						reinterpret_cast<unsigned long long> (get_immediate_context_fun),
						reinterpret_cast<unsigned long long> (g_pdevice),
						g_user_buffer,
						0,
						0);

					g_pContext = *(PVOID*)g_user_buffer;
					//vfun_utils::release(g_pContext);

				}
			 
				 
				if (!g_Surface)
				{
					memory::mem_copy((PVOID)g_user_buffer, (PVOID)&ID3D11Texture2DVar, sizeof(ID3D11Texture2DVar));

					get_buffer_fun = utils::vfun_utils::get_vfunc(g_swap_chain, 9);

					usercall_retval_ptr = user_call::call(
						reinterpret_cast<unsigned long long> (get_buffer_fun),
						reinterpret_cast<unsigned long long> (g_swap_chain),
						0,
						g_user_buffer,
						g_user_buffer + sizeof(ID3D11Texture2DVar));
					hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
					if (FAILED(hr))
					{
						return;

					}
					g_Surface = *(PVOID*)(g_user_buffer + sizeof(ID3D11Texture2DVar));
				}



				if (!g_pdevice || !g_pContext || !g_Surface)
				{
					return;
				}


			 
				/*	if (g_slient_start_time) {
						if (utils::time_utils::get_real_time() - g_slient_start_time > 3000) {
							g_slient_start_time = 0;
						}
						g_PreviousRenderTime = utils::time_utils::get_real_time();
						return;
					}
					if (utils::time_utils::get_real_time() - g_PreviousRenderTime > 150) {
						g_PreviousRenderTime = utils::time_utils::get_real_time();
						g_slient_start_time = utils::time_utils::get_real_time();
						return;
					}

					g_PreviousRenderTime = utils::time_utils::get_real_time();*/


				get_desc_fun = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(g_Surface, 10));


				user_call::call(
					get_desc_fun,
					reinterpret_cast<unsigned long long>(g_Surface),
					g_user_buffer,
					0,
					0

				);


				SDesc = *(D3D11_TEXTURE2D_DESC*)g_user_buffer;
				SDesc.BindFlags = 0;
				SDesc.MiscFlags = 0;
				SDesc.Usage = D3D11_USAGE_STAGING;
				SDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

				memory::mem_copy((PVOID)g_user_buffer, &SDesc, sizeof(SDesc));

				  


				create_texture2D_fun = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(g_pdevice, 5));


				memory::mem_zero((PVOID)g_texture_buffer, 0x4000, 0);
				usercall_retval_ptr = user_call::call(
					create_texture2D_fun,
					reinterpret_cast<unsigned long long>(g_pdevice),
					g_user_buffer,
					0,
					g_texture_buffer
				);

				hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
				if (FAILED(hr))
				{
					return;
				}

				PVOID pTexture = *(PVOID*)g_texture_buffer;
				if (!pTexture)
				{
					return;
				}


				copy_resource_fun = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(g_pContext, 47));

				user_call::call(
					copy_resource_fun,
					reinterpret_cast<unsigned long long>(g_pContext),
					reinterpret_cast<unsigned long long>(pTexture),
					reinterpret_cast<unsigned long long>(g_Surface)
					, 0
				);


				map_fun = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(g_pContext, 14));

				usercall_retval_ptr = user_call::call6(
					map_fun,
					reinterpret_cast<unsigned long long>(g_pContext),
					reinterpret_cast<unsigned long long>(pTexture),
					0,
					D3D11_MAP_READ_WRITE,
					0,
					g_user_buffer
				);
				hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
			 
				memcpy(&MapRes, reinterpret_cast<PVOID>(g_user_buffer), sizeof(D3D11_MAPPED_SUBRESOURCE));
				 
				if (SDesc.Width && SDesc.Height && MapRes.Data)
				{
					draw_callback(SDesc.Width, SDesc.Height, MapRes.Data);

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
				 
		}

		void draw_overlay_elements(int width, int height, void* data)
		{
			memset(pagehit, 0, sizeof(pagehit));
			memset(pagevaild, 0, sizeof(pagevaild));

			ByteRender rend;
			rend.Setup(width, height, data);
			rend.Line({ 100, 200 }, { 500, 200 }, FColor(__rdtsc()), 1);


			//// 先安全获取一份玩家数据副本
			//game:: kcsgo2struct::CPlayer players_copy[MAX_PLAYER_NUM] = {};
			//int player_count = 0;

			//if (!game::kcsgo2::get_player_data(players_copy, MAX_PLAYER_NUM, &player_count))
			//{
			//	return; // 获取数据失败，直接返回
			//}

			//for (int i = 0; i < player_count; ++i)
			//{
			//	const auto& player = players_copy[i];
			//	if (!player.bIsPlayerExists)
			//		continue;

			//	Vector3 ov{}, ov2{};
			//	if (!world_to_screen( &player.origin, &ov, &game::kcsgo2data::g_view_matrix, game::kcsgo2::g_game_size))
			//		continue;

			//	Vector3 head_pos = player.origin;
			//	head_pos.z += 70;

			//	if (!world_to_screen(&head_pos, &ov2, &game::kcsgo2data::g_view_matrix, game::kcsgo2::g_game_size))
			//		continue;

			//	int box_height = static_cast<int>(ov.y - ov2.y);
			//	int box_width = box_height / 2;
			//	int box_x = static_cast<int>(ov2.x - box_width / 2);
			//	int box_y = static_cast<int>(ov2.y);

			//	rend.Rectangle(box_x, box_y, static_cast<float>(box_width), static_cast<float>(box_height), FColor(0, 255, 0), 1);
			//}
		}

		PULONG64 get_user_rsp_ptr()
		{
			const ULONG64 syscall_address = __readmsr(IA32_LSTAR);
			if (syscall_address == 0)
				return nullptr;

			const auto offset_ptr = reinterpret_cast<const ULONG*>(syscall_address + 8);
			if (!offset_ptr)
				return nullptr;

			const ULONG offset = *offset_ptr;
			const auto rsp_ptr = reinterpret_cast<PULONG64>(__readgsqword(offset));
			return rsp_ptr;
		}

		bool is_user_rsp_ptr()
		{
			PULONG64 user_rsp_ptr = get_user_rsp_ptr();
			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(user_rsp_ptr))
			{
				return false;
			}
			
			unsigned long long  user_rsp = *user_rsp_ptr;

			if (user_rsp>MmUserProbeAddress)
			{
				return false;
			}
			return true;
			
		}

		bool is_user_stack_in_dxgi_range()
		{
			PULONG64 user_rsp_ptr = get_user_rsp_ptr();
			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(user_rsp_ptr))
			{
				return false;
			}

			unsigned long long user_rsp = *user_rsp_ptr;

			// 判断 user_rsp 是否在 g_dxgi_base 范围内
			if (utils::dwm_draw::g_dxgi_base && utils::dwm_draw::g_dxgi_size)
			{
				unsigned long long base =  utils::dwm_draw::g_dxgi_base ;
				unsigned long long limit = base + utils::dwm_draw::g_dxgi_size;

				if (user_rsp >= base && user_rsp < limit)
				{
					return true;
				}
			}

			return false;
		}

		void draw_utils( )
		{
			static bool  has_hooked_get_buffer = false;
		 
			if (g_should_hide_overlay)
			{
				 
				return;
			}

			// 防止并发调用
			if (InterlockedCompareExchange(&g_dwm_render_lock, 1, 0) != 0)
				return;



		/*	if (!has_hooked_get_buffer)
			{
				utils::dwm_draw::hook_get_buffer(utils::dwm_draw::g_dwm_process);
				has_hooked_get_buffer = true;
			}*/
			 

			if (!game::kcsgo2::is_initialize_game())
			{
				InterlockedExchange(&g_dwm_render_lock, 0);
				return;
			}

			if (!game::kcsgo2::is_create_time())
			{
				InterlockedExchange(&g_dwm_render_lock, 0);
				return;
			}
			render_overlay_frame(draw_overlay_elements);


		 
			InterlockedExchange(&g_dwm_render_lock, 0);

		 
		 
		
		}

		 

		 

	}
}

