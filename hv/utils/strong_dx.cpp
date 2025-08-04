#include "global_defs.h"
#include "strong_dx.h"
#include "../vtx/msr.h"
#include "dx_draw/LegacyRender.h"
#include "dx11.h"
 
 
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
			PVOID g_pdevice{};
			PVOID g_pContext{};
			PVOID g_Surface{};
			
			bool g_should_hide_overlay = false;
		   volatile LONG g_dwm_render_lock = 0;
		    
		 
		   bool initialize_d3d_resources(unsigned long long  pswap_chain)
		   {
			  
			   if (g_initialized)
				   return true;

			   const GUID ID3D11DeviceVar = { 0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40 };
			   const GUID ID3D11Texture2DVar = { 0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c };

			   if (!pswap_chain)
			   {
				   return false;
			   }

			   g_swap_chain = reinterpret_cast<PVOID>(pswap_chain);

			   if (!g_user_buffer)
			   {
				   PVOID desc_buffer_local{};
				   NTSTATUS status = memory::allocate_user_memory(&desc_buffer_local, 0x1000, PAGE_READWRITE, true, false);
				   if (!NT_SUCCESS(status))
					   return false;

				   g_user_buffer = reinterpret_cast<unsigned long long>(desc_buffer_local);
			   }

			   if (!g_texture_buffer)
			   {
				   PVOID texture_buffer{};
				   NTSTATUS status = memory::allocate_user_memory(&texture_buffer, 0x4000, PAGE_READWRITE, true, false);
				   if (!NT_SUCCESS(status))
					   return false;

				   g_texture_buffer = reinterpret_cast<unsigned long long>(texture_buffer);
			   }

			   if (!g_pdevice)
			   {
				   memory::mem_copy(reinterpret_cast<PVOID>(g_user_buffer), (PVOID)&ID3D11DeviceVar, sizeof(ID3D11DeviceVar));

				   PVOID get_device_fun = utils::vfun_utils::get_vfunc(g_swap_chain, 7);
				   auto usercall_retval_ptr = user_call::call(
					   reinterpret_cast<unsigned long long>(get_device_fun),
					   reinterpret_cast<unsigned long long>(g_swap_chain),
					   g_user_buffer,
					   g_user_buffer + sizeof(ID3D11DeviceVar),
					   0);

				   HRESULT hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
				   if (FAILED(hr))
					   return false;

				   g_pdevice = *(PVOID*)(g_user_buffer + sizeof(ID3D11DeviceVar));
			   }

			   if (!g_pContext)
			   {
				   PVOID get_immediate_context_fun = utils::vfun_utils::get_vfunc(g_pdevice, 40);
				   user_call::call(
					   reinterpret_cast<unsigned long long>(get_immediate_context_fun),
					   reinterpret_cast<unsigned long long>(g_pdevice),
					   g_user_buffer,
					   0,
					   0);

				   g_pContext = *(PVOID*)g_user_buffer;
			   }

			   if (!g_Surface)
			   {
				   memory::mem_copy((PVOID)g_user_buffer, (PVOID)&ID3D11Texture2DVar, sizeof(ID3D11Texture2DVar));

				   PVOID get_buffer_fun = utils::vfun_utils::get_vfunc(g_swap_chain, 9);
				   auto usercall_retval_ptr = user_call::call(
					   reinterpret_cast<unsigned long long>(get_buffer_fun),
					   reinterpret_cast<unsigned long long>(g_swap_chain),
					   0,
					   g_user_buffer,
					   g_user_buffer + sizeof(ID3D11Texture2DVar));

				   HRESULT hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
				   if (FAILED(hr))
					   return false;

				   g_Surface = *(PVOID*)(g_user_buffer + sizeof(ID3D11Texture2DVar));
			   }

			   g_initialized = g_pdevice && g_pContext && g_Surface;
			   return g_initialized;
		   }
		void render_overlay_frame(void (*draw_callback)(int width, int height, void* data))
		{

			if (!g_pdevice || !g_pContext   || !g_user_buffer || !g_texture_buffer||!g_Surface)
				return;

			
			
			HRESULT hr{};
			D3D11_TEXTURE2D_DESC SDesc{};
			D3D11_MAPPED_SUBRESOURCE MapRes{};
			unsigned long long usercall_retval_ptr{};
	/*		const GUID ID3D11Texture2DVar = { 0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c };

		 
				memory::mem_copy((PVOID)g_user_buffer, (PVOID)&ID3D11Texture2DVar, sizeof(ID3D11Texture2DVar));

				PVOID get_buffer_fun = utils::vfun_utils::get_vfunc(g_swap_chain, 9);
				auto usercall_retval_ptr = user_call::call(
					reinterpret_cast<unsigned long long>(get_buffer_fun),
					reinterpret_cast<unsigned long long>(g_swap_chain),
					0,
					g_user_buffer,
					g_user_buffer + sizeof(ID3D11Texture2DVar));

				  hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
				if (FAILED(hr))
					return  ;

				g_Surface = *(PVOID*)(g_user_buffer + sizeof(ID3D11Texture2DVar));*/
			 
			// 获取 Surface 描述
			auto get_desc_fun = reinterpret_cast<unsigned long long>(
				utils::vfun_utils::get_vfunc(g_Surface, 10));

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

			// 创建 staging texture
			auto create_texture2D_fun = reinterpret_cast<unsigned long long>(
				utils::vfun_utils::get_vfunc(g_pdevice, 5));

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
				return;

			PVOID pTexture = *(PVOID*)g_texture_buffer;
			if (!pTexture)
			{
				//vfun_utils::release(g_Surface);
				//g_Surface = nullptr;
				return;
			}

			// 拷贝屏幕资源到 staging texture
			auto copy_resource_fun = reinterpret_cast<unsigned long long>(
				utils::vfun_utils::get_vfunc(g_pContext, 47));

			user_call::call(
				copy_resource_fun,
				reinterpret_cast<unsigned long long>(g_pContext),
				reinterpret_cast<unsigned long long>(pTexture),
				reinterpret_cast<unsigned long long>(g_Surface),
				0
			);

			// Map staging texture 读写
			auto map_fun = reinterpret_cast<unsigned long long>(
				utils::vfun_utils::get_vfunc(g_pContext, 14));

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
			/*	if (FAILED(hr))
				{
					vfun_utils::release(pTexture);
					return;
				}*/
			memcpy(&MapRes, reinterpret_cast<PVOID>(g_user_buffer), sizeof(D3D11_MAPPED_SUBRESOURCE));

			// 调用用户绘制回调
			if (SDesc.Width && SDesc.Height && MapRes.pData)
			{
				draw_callback(SDesc.Width, SDesc.Height, MapRes.pData);
			}

			// Unmap
			auto unmap_fun = reinterpret_cast<unsigned long long>(
				utils::vfun_utils::get_vfunc(g_pContext, 15));

			user_call::call(
				unmap_fun,
				reinterpret_cast<unsigned long long>(g_pContext),
				reinterpret_cast<unsigned long long>(pTexture),
				0,
				0
			);

			// 将修改后的内容写回 Surface
			user_call::call(
				copy_resource_fun,
				reinterpret_cast<unsigned long long>(g_pContext),
				reinterpret_cast<unsigned long long>(g_Surface),
				reinterpret_cast<unsigned long long>(pTexture),
				0
			);

			// 释放 staging texture
			vfun_utils::release(pTexture);
			//vfun_utils::release(g_Surface);
			//g_Surface = nullptr;
		}

		void release_d3d_resources()
		{
			if (!g_initialized)
			{
				return;
			}

			// 释放 Surface
			if (g_Surface)
			{
				utils::vfun_utils::release(g_Surface);
				g_Surface = nullptr;
			}

			// 释放 Context
			if (g_pContext)
			{
				utils::vfun_utils::release(g_pContext);
				g_pContext = nullptr;
			}

			// 释放 Device
			if (g_pdevice)
			{
				utils::vfun_utils::release(g_pdevice);
				g_pdevice = nullptr;
			}

			// 释放 SwapChain (通常不释放窗口的 SwapChain，除非你自己创建的)
			  g_swap_chain = nullptr; // 仅清空指针，非自己创建的 swapchain 不释放

			// 释放用户缓冲区
			if (g_user_buffer)
			{
				memory::free_user_memory(reinterpret_cast<PVOID>(g_user_buffer),0x1000,false);
				g_user_buffer = 0;
			}

			// 释放纹理缓冲区
			if (g_texture_buffer)
			{
				memory::free_user_memory(reinterpret_cast<PVOID>(g_texture_buffer),0x4000, false);
				g_texture_buffer = 0;
			}

			g_initialized = false;
		}

		void draw_overlay_elements(int width, int height, void* data)
		{

			/*if (utils::auth::is_license_expired())
			{
				return;
			}*/
			memset(g_pagehit, 0, sizeof(g_pagehit));
			memset(g_pagevaild, 0, sizeof(g_pagevaild));



			ByteRender rend;
			rend.Setup(width, height, data);
			rend.Line({ 100, 200 }, { 500, 200 }, FColor(__rdtsc()), 1);
			// 按方案类型执行初始化流程
			bool initialized = false;
#if defined(ENABLE_GAME_DRAW_TYPE3)

			

#if ENABLE_GAME_DRAW_TYPE3 == 1
			// 第一套绘制逻辑：通过 DLL 初始化
			initialized = game::kcsgo2::is_initialize_game();

#elif ENABLE_GAME_DRAW_TYPE3 == 2
			// 第二套绘制逻辑：自己遍历进程
			  
		 

		//	 initialized = game::kcsgo2::initialize_game_process2();
#elif ENABLE_GAME_DRAW_TYPE3 == 3
			// 第三套绘制逻辑：自己初始化数据
			initialized = game::kcsgo2::is_initialize_game() &&
				game::kcsgo2::is_create_time() &&
				game::kcsgo2::initialize_game_data3();

#endif // ENABLE_GAME_DRAW_TYPE3
 

#endif // defined(ENABLE_GAME_DRAW_TYPE3)

			if (!initialized)
			{
				return;
			}
			// === 绘制 ESP 相关 ===
			draw_players_esp(rend);
			 
			 

			// ToDo: draw other overlay elements like menu, C4, weapon, etc.

		 
		}
		void draw_players_esp(ByteRender& rend)
		{
			game::kcsgo2struct::CPlayer players_copy[MAX_PLAYER_NUM] = {};
			int player_count = 0;

			if (!game::kcsgo2::get_player_data(players_copy, &player_count))
			{
				return;
			}

			if (player_count==0)
			{
				RtlZeroMemory(players_copy, sizeof(players_copy));
			}
			

			for (size_t i = 0; i < player_count; ++i)
			{
				const auto& player = players_copy[i];
				 
				if (player.health <= 0 || player.health > 100)
				{
					continue;
				}

				Vector3 foot_screen{}, head_screen{};
				if (!world_to_screen(&player.origin, &foot_screen, &game::kcsgo2data::g_view_matrix, game::kcsgo2::g_game_size))
					continue;

				Vector3 head_pos = player.origin;
				head_pos.z += 70.0f;
				if (!world_to_screen(&head_pos, &head_screen, &game::kcsgo2data::g_view_matrix, game::kcsgo2::g_game_size))
					continue;

				// ==== Box ESP ====
				if (KMenuConfig::ShowBoxEsp2d.enabled)
				{
					int box_height = static_cast<int>(foot_screen.y - head_screen.y);
					int box_width = box_height / 2;
					int box_x = static_cast<int>(head_screen.x - box_width / 2);
					int box_y = static_cast<int>(head_screen.y);

					FColor color(KMenuConfig::ShowBoxEsp2d.color[0], KMenuConfig::ShowBoxEsp2d.color[1], KMenuConfig::ShowBoxEsp2d.color[2]);
					rend.Rectangle(box_x, box_y, static_cast<float>(box_width), static_cast<float>(box_height), color, 1);
				}

				// ==== Head Tracker (Circle) ====
				if (KMenuConfig::Showheadtracker.enabled)
				{
					const auto& head_bone = player.bones.bone_positions[game::kcsgo2struct::BONE_HEAD];

					// 确保骨骼位置有效

					if (head_bone.x != 0.0f && head_bone.y!= 0.0f)
					{
						const float height = foot_screen.y - head_screen.y;
						const float width = height / 2.4f;
						const float radius = width / 5.0f;

						FColor color(KMenuConfig::Showheadtracker.color[0], KMenuConfig::Showheadtracker.color[1], KMenuConfig::Showheadtracker.color[2]);




						rend.DrawCircle(
							head_bone.x,
							head_bone.y - (width / 12.0f),
							radius,
							color,
							1.0f
						);
					}

				 
			
				}

				// ==== Bone ESP ====
				if (KMenuConfig::ShowBone.enabled)
				{
					FColor color(KMenuConfig::ShowBone.color[0], KMenuConfig::ShowBone.color[1], KMenuConfig::ShowBone.color[2]);

					for (size_t j = 0; j < BONE_CONNECTION_COUNT; ++j)
					{
						const auto& connection = game::kcsgo2struct::g_bone_connections[j];
					  
						const auto& from_pos = player.bones.bone_positions[connection.from];
						const auto& to_pos = player.bones.bone_positions[connection.to];

						// 骨骼坐标为 (0, 0, 0) 时通常表示无效
						if ((from_pos.x != 0.f && from_pos.y != 0.f) &&
							(to_pos.x != 0.f && to_pos.y != 0.f ))
						{
							Vector2 screen_from = { from_pos.x, from_pos.y };
							Vector2 screen_to = { to_pos.x, to_pos.y };

							rend.Line(screen_from, screen_to, color, 1);
						}

						 
					}
				}
			}
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

		;



		/*	if (!has_hooked_get_buffer)
			{
				utils::dwm_draw::hook_get_buffer(utils::dwm_draw::g_dwm_process);
				has_hooked_get_buffer = true;
			}*/
			 

			
			render_overlay_frame(draw_overlay_elements);


		 
			

		 
		 
		
		}

		 

		 

	}
}

