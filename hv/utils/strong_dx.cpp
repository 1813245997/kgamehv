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
			PVOID g_desc_buffer{};
			PVOID g_entity_buffer{};
			PVOID g_swap_chain{};
			PVOID  g_pdevice{};
			PVOID g_pContext{};
			PVOID g_pRenderTargetView;
			PVOID g_Surface{};
		 
			bool g_should_hide_overlay = false;
		   volatile LONG g_dwm_render_lock = 0;
		 
		NTSTATUS initialize(unsigned long long pswap_chain)
		{
			NTSTATUS status{};
			PVOID get_device_fun{};
			PVOID get_immediate_context_fun{};
			PVOID get_buffer_fun{};
			PVOID desc_buffer_local{};
			PVOID entity_buffer_local{};
			unsigned long long   usercall_retval_ptr{};
			HRESULT  hr{};
			const GUID ID3D11DeviceVar = { 0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40 };
			const GUID ID3D11Texture2DVar = { 0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c };
			if (g_initialized)
			{
				return STATUS_SUCCESS;
			}

			if (!pswap_chain)
			{
				return STATUS_INVALID_PARAMETER;
			}
			g_swap_chain = reinterpret_cast<PVOID> (pswap_chain);
			 
		 
			status = memory::allocate_user_memory(&desc_buffer_local, 0x1000, PAGE_READWRITE, true, true);
			if (!NT_SUCCESS(status))
			{
				return status;
			}
			g_desc_buffer = desc_buffer_local;
			status = memory::allocate_user_memory(&entity_buffer_local, 0x1000, PAGE_READWRITE, true, true);
			if (!NT_SUCCESS(status))
			{
				return status;
			}
			g_entity_buffer = entity_buffer_local;

		 
			memory::mem_copy(desc_buffer_local, (PVOID)&ID3D11DeviceVar, sizeof(ID3D11DeviceVar));

		     get_device_fun =   utils::vfun_utils::get_vfunc(g_swap_chain, 7);

			 usercall_retval_ptr = user_call::call(
				reinterpret_cast<unsigned long long> (get_device_fun),
				pswap_chain,
				reinterpret_cast<unsigned long long> (desc_buffer_local),
				reinterpret_cast<unsigned long long> (entity_buffer_local),
				0);
			 hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
			if (FAILED(hr))
			{
				return STATUS_DEVICE_DOES_NOT_EXIST;
				
			}

			g_pdevice = *reinterpret_cast<PVOID*>(entity_buffer_local);
			//vfun_utils::release(g_pdevice);
			get_immediate_context_fun = utils::vfun_utils::get_vfunc(g_pdevice, 40);

		

		    user_call::call(
				reinterpret_cast<unsigned long long> (get_immediate_context_fun),
				reinterpret_cast<unsigned long long> (g_pdevice),
				reinterpret_cast<unsigned long long>(entity_buffer_local),
				0,
				0);

			g_pContext = *(PVOID*)entity_buffer_local;
			//vfun_utils::release(g_pContext);
			

			memory::mem_copy(desc_buffer_local, (PVOID)&ID3D11Texture2DVar, sizeof(ID3D11Texture2DVar));

			get_buffer_fun = utils::vfun_utils::get_vfunc(g_swap_chain, 9);
			
			usercall_retval_ptr = user_call::call(
				reinterpret_cast<unsigned long long> (get_buffer_fun),
				reinterpret_cast<unsigned long long> (g_swap_chain),
				0,
				reinterpret_cast<unsigned long long> (desc_buffer_local),
				reinterpret_cast<unsigned long long> (entity_buffer_local) );
			hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
			if (FAILED(hr))
			{
					return STATUS_DEVICE_DOES_NOT_EXIST;

			}
			g_Surface = *(PVOID*)entity_buffer_local;
			 
			//vfun_utils::release(renderTargetTexture);
			 
		 
			g_initialized = true;
			/*	vfun_utils::release(g_pdevice);
				vfun_utils::release(g_pContext);
				vfun_utils::release(g_Surface);*/

			return STATUS_SUCCESS;
		}

	 
		void render_overlay_frame(void (*draw_callback)(int width, int height, void* data))
		{
			HRESULT  hr{};
			PVOID  desc_buffer = g_desc_buffer;
			PVOID entity_buffer = g_entity_buffer;
			D3D11_TEXTURE2D_DESC SDesc{};
			D3D11_MAPPED_SUBRESOURCE MapRes{};

			unsigned  long long  get_desc_fun{};
			unsigned  long long copy_resource_fun{};
			unsigned  long long create_texture2D_fun{};
			unsigned  long long map_fun{};
			unsigned  long long umap_fun{};
			unsigned long long   usercall_retval_ptr{};
			 
			 
			get_desc_fun = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(g_Surface, 10));
		 

			user_call::call(
				get_desc_fun,
				reinterpret_cast<unsigned long long>(g_Surface),
				reinterpret_cast<unsigned long long>(desc_buffer),
				0,
				0
			);

			SDesc = *(D3D11_TEXTURE2D_DESC*)desc_buffer;

			SDesc.BindFlags = 0;
			SDesc.MiscFlags = 0;
			SDesc.Usage = D3D11_USAGE_STAGING;
			SDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

			//memory::mem_zero(desc_buffer, 0X1000);
			memory::mem_copy(desc_buffer, &SDesc, sizeof(SDesc));


			create_texture2D_fun = reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(g_pdevice, 5));

		 
			
			usercall_retval_ptr = user_call::call(
				create_texture2D_fun,
				reinterpret_cast<unsigned long long>(g_pdevice),
				reinterpret_cast<unsigned long long>(desc_buffer),
				0,
				reinterpret_cast<unsigned long long>(entity_buffer)
			);

			hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
			if (FAILED(hr))
			{
				return;
			}

			PVOID pTexture = *(PVOID*)entity_buffer;
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

			user_call::call6(
				map_fun,
				reinterpret_cast<unsigned long long>(g_pContext),
				reinterpret_cast<unsigned long long>(pTexture),
				0,
				D3D11_MAP_READ_WRITE,
				0,
				reinterpret_cast<unsigned long long>(desc_buffer)
			);

			MapRes = *(D3D11_MAPPED_SUBRESOURCE*)desc_buffer;
		 

			/*	if (SDesc.Width && SDesc.Height && MapRes.Data)
				{

				}*/
			draw_callback(SDesc.Width, SDesc.Height, MapRes.Data);
		
			 
		 

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

			if (!game::kcsgo2::initialize_game_data())
			{
				
				return;
			}

		 
			

			for ( size_t i =0;i<game::kcsgo2::g_player_count;i++)
			{

				int box_h{};
				int box_w{};
				int box_x{};
				int box_y{};
				Vector3 ov{}, ov2{};
				game::kcsgo2struct::CPlayer& player =   game::kcsgo2::g_player_array[i];
 
				if (!world_to_screen(&player.origin, &ov, &game::kcsgo2data::g_view_matrix, game::kcsgo2::g_game_size))
				{
					continue;
				}
				player.origin.z += 70;


				if (!world_to_screen(&player.origin, &ov2, &game::kcsgo2data::g_view_matrix, game::kcsgo2::g_game_size))
				{
					continue; // 如果转换失败，跳过该玩家
				}

				// 绘制矩形框
				int box_height = static_cast<int> (ov.y - ov2.y);   // 视角高
				int box_weight = box_height / 2; // 视角宽
				box_h = box_height;
				box_w = box_weight;
				box_x = static_cast<float> (ov2.x - box_weight / 2);
				box_y = ov2.y;

			
				 
				 

			 

				
				/*unsigned long long current_irql = asm_read_cr8();
				asm_write_cr8(DISPATCH_LEVEL);*/
				/*	bool old_state = asm_read_rflags() & 0x200;
					if (old_state)
					{
						asm_cli();
					}*/
					/*	KIRQL old_irql;
						KeRaiseIrql(DISPATCH_LEVEL, &old_irql);
						cr4 cr4vlaue{ __readcr4() };
						bool smap = cr4vlaue.smap_enable;
						if (smap) {
							cr4vlaue.smap_enable = 0;
							__writecr4(cr4vlaue.flags);
						}*/
				////防止其他绘制打断 
			
			
				rend.Rectangle(box_x, box_y, static_cast<float> (box_weight), static_cast<float> (box_height), FColor(0, 255, 0), 1);
				
				//if (smap)
				//{
				//	cr4vlaue.smap_enable = true;
				//	__writecr4(cr4vlaue.flags);
				//}
				///*	if (old_state)
				//	{
				//		asm_sti();
				//	}*/
		  //  	// asm_write_cr8(current_irql);
				//KeLowerIrql(old_irql);
				 
			}

			 
			  

			
			//rend.String(&g_Font, { 100, 200 }, L"https://github.com/cs1ime", PM_XRGB(255, 0, 0));

		    

			
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



			if (!has_hooked_get_buffer)
			{
				utils::dwm_draw::hook_get_buffer(utils::dwm_draw::g_dwm_process);
				has_hooked_get_buffer = true;
			}

			if (!NT_SUCCESS(initialize(utils::dwm_draw::g_pswap_chain)))
			{
				InterlockedExchange(&g_dwm_render_lock, 0);
				return;
			}
			
			render_overlay_frame(draw_overlay_elements);


		 
			InterlockedExchange(&g_dwm_render_lock, 0);

		 
		 
		
		}

		 

		 

	}
}

