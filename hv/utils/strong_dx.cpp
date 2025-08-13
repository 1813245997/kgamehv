#include "global_defs.h"
#include "strong_dx.h"
#include "../vtx/msr.h"
#include "dx_draw/LegacyRender.h"
#include "dx11.h"
#include "dx_draw/fontdata.h"
 
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)

namespace utils
{
	namespace  strong_dx
	{    
			
			bool g_initialized{};
			unsigned long long g_user_buffer{};
			unsigned long long  g_texture_buffer{};
			unsigned long long g_pOverlaySwapChain{};
			PVOID g_swap_chain{};

			PVOID g_pdevice{};
			PVOID g_pContext{};
			PVOID g_Surface{};
			Font* g_Font = nullptr;
			bool g_should_hide_overlay = false;

			
		   volatile LONG g_dwm_render_lock = 0;
		   
		   bool initialize_font()
		   {
			    
			   g_Font = (Font*)utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, sizeof(Font), 'FONT');
			   if (!g_Font)
			   {
				   LogError("Font allocation failed.");
				   return false;
			   }

			   RtlZeroMemory(g_Font, sizeof(Font));

			   if (!g_Font->InitFont(fontdata::data, sizeof(fontdata::data)))
			   {
				   LogError("Failed to initialize font.");
				   return false;
			   }

			   LogDebug("Font initialized successfully.");
			   return true;

			    
		   }
		   
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

		bool initialize_d3d11_resources_win1124h2(unsigned long long pOverlaySwapChain)
		{
			if (g_initialized)
				return true;


			if (!pOverlaySwapChain)
			{
				return false;
			}

			g_pOverlaySwapChain = pOverlaySwapChain;




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

			uint64_t cd3dDeviceAddr = *(uint64_t*)(g_pOverlaySwapChain + 0x28);
			g_pdevice = *(PVOID*)(cd3dDeviceAddr + 0x228);


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

			 


			return true;
		}

		void render_overlay_frame_win1124h2(void (*draw_callback)(int width, int height, void* data))
		{

			//if (!g_pdevice || !g_pContext || !g_user_buffer || !g_texture_buffer  )
			//	return;



			//HRESULT hr{};
			//D3D11_TEXTURE2D_DESC SDesc{};
			//D3D11_MAPPED_SUBRESOURCE MapRes{};
			//unsigned long long usercall_retval_ptr{};

			//uint32_t Index = *(uint32_t*)(g_pOverlaySwapChain + 0x1EC);
			//uint64_t DeviceTextureTarget = *(uint64_t*)(*(uint64_t*)(*(uint64_t*)(g_pOverlaySwapChain + 0x1B0) + (Index * 8)) + 0xD8);
			//g_Surface =(PVOID) DeviceTextureTarget;
	 

			//// 获取 Surface 描述
			//auto get_desc_fun = reinterpret_cast<unsigned long long>(
			//	utils::vfun_utils::get_vfunc(g_Surface, 10));

			//user_call::call(
			//	get_desc_fun,
			//	reinterpret_cast<unsigned long long>(g_Surface),
			//	g_user_buffer,
			//	0,
			//	0
			//);

			//SDesc = *(D3D11_TEXTURE2D_DESC*)g_user_buffer;
			//SDesc.BindFlags = 0;
			//SDesc.MiscFlags = 0;
			//SDesc.Usage = D3D11_USAGE_STAGING;
			//SDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

			//memory::mem_copy((PVOID)g_user_buffer, &SDesc, sizeof(SDesc));

			//// 创建 staging texture
			//auto create_texture2D_fun = reinterpret_cast<unsigned long long>(
			//	utils::vfun_utils::get_vfunc(g_pdevice, 5));

			//memory::mem_zero((PVOID)g_texture_buffer, 0x4000, 0);
			//usercall_retval_ptr = user_call::call(
			//	create_texture2D_fun,
			//	reinterpret_cast<unsigned long long>(g_pdevice),
			//	g_user_buffer,
			//	0,
			//	g_texture_buffer
			//);

			//hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
			//if (FAILED(hr))
			//	return;

			//PVOID pTexture = *(PVOID*)g_texture_buffer;
			//if (!pTexture)
			//{
			//	//vfun_utils::release(g_Surface);
			//	//g_Surface = nullptr;
			//	return;
			//}

			//// 拷贝屏幕资源到 staging texture
			//auto copy_resource_fun = reinterpret_cast<unsigned long long>(
			//	utils::vfun_utils::get_vfunc(g_pContext, 47));

			//user_call::call(
			//	copy_resource_fun,
			//	reinterpret_cast<unsigned long long>(g_pContext),
			//	reinterpret_cast<unsigned long long>(pTexture),
			//	reinterpret_cast<unsigned long long>(g_Surface),
			//	0
			//);

			//// Map staging texture 读写
			//auto map_fun = reinterpret_cast<unsigned long long>(
			//	utils::vfun_utils::get_vfunc(g_pContext, 14));

			//usercall_retval_ptr = user_call::call6(
			//	map_fun,
			//	reinterpret_cast<unsigned long long>(g_pContext),
			//	reinterpret_cast<unsigned long long>(pTexture),
			//	0,
			//	D3D11_MAP_READ_WRITE,
			//	0,
			//	g_user_buffer
			//);

			//hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
			///*	if (FAILED(hr))
			//	{
			//		vfun_utils::release(pTexture);
			//		return;
			//	}*/
			//memcpy(&MapRes, reinterpret_cast<PVOID>(g_user_buffer), sizeof(D3D11_MAPPED_SUBRESOURCE));

			//// 调用用户绘制回调
			//if (SDesc.Width && SDesc.Height && MapRes.pData)
			//{
			//	draw_callback(SDesc.Width, SDesc.Height, MapRes.pData);
			//}

			//// Unmap
			//auto unmap_fun = reinterpret_cast<unsigned long long>(
			//	utils::vfun_utils::get_vfunc(g_pContext, 15));

			//user_call::call(
			//	unmap_fun,
			//	reinterpret_cast<unsigned long long>(g_pContext),
			//	reinterpret_cast<unsigned long long>(pTexture),
			//	0,
			//	0
			//);

			//// 将修改后的内容写回 Surface
			//user_call::call(
			//	copy_resource_fun,
			//	reinterpret_cast<unsigned long long>(g_pContext),
			//	reinterpret_cast<unsigned long long>(g_Surface),
			//	reinterpret_cast<unsigned long long>(pTexture),
			//	0
			//);

			//// 释放 staging texture
			//vfun_utils::release(pTexture);
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

		void draw_update_required_notice(ByteRender& rend)
		{
			if (!game::kcsgo2::g_game || !game::kcsgo2::g_game->m_cheat_update)
				return;

			const int screen_width = rend.GetScreenWidth();
			const int screen_height = rend.GetScreenHeight();

			rend.StringA(
				g_Font,
				screen_width / 2 - 80, // 居中偏移
				screen_height / 2,
				"Cheat update required!",
				FColor(255, 0, 0)
			);
		}
		void draw_overlay_elements(int width, int height, void* data)
		{

			//if (utils::auth::is_license_expired())
			//{
			//	return;
			//}
			memset(g_pagehit, 0, sizeof(g_pagehit));
			memset(g_pagevaild, 0, sizeof(g_pagevaild));

			
			
			ByteRender rend;
			rend.Setup(width, height, data);
			int margin =30;
			rend.String(
				g_Font,
				{ static_cast<float>(width - 150), static_cast<float>(margin) }, // 右上角位置
				L"KGameBoxCS2",
				PM_XRGB(255, 0, 0)
			);

		  
 
			//=== 绘制是否更新辅助===
			draw_update_required_notice(rend);
			// === 绘制 ESP 相关 ===
			draw_players_esp(rend);
			 
			 

			// ToDo: draw other overlay elements like menu, C4, weapon, etc.

		 
		}
 
		void draw_players_esp(ByteRender& rend)
		{
			 

			utils::kvector< game::kcsgo2struct::CPlayer>list{};



			if (!game::kcsgo2::g_game->update_game_windows(utils::dwm_draw::g_game_utils_buffer))
			{
				return;
			}
			 
			if (game::kcsgo2::g_game->isC4Planted)
			{
			 
				const Vector3 c4ScreenPos = game::kcsgo2::g_game->world_to_screen(&game::kcsgo2::g_game->c4Origin);
				if (c4ScreenPos.z >= 0.01f) {
					float c4Distance = game::kcsgo2::g_game->localOrigin.calculate_distance(game::kcsgo2::g_game->c4Origin);
					float c4RoundedDistance = utils::math:: round(c4Distance / 500.f);

					float height = 10 - c4RoundedDistance;
					float width = height * 1.4f;

					rend.Rectangle(

						c4ScreenPos.x - (width / 2),
						c4ScreenPos.y - (height / 2),
						width,
						height,
						FColor(config::esp_box_color_enemy.r, config::esp_box_color_enemy.g, config::esp_box_color_enemy.b)
						
					);

					rend.StringA(
						g_Font,
						c4ScreenPos.x + (width / 2 + 5),
						c4ScreenPos.y,
						"C4",
						FColor(config::show_name.color[0], config::show_name.color[1], config::show_name.color[2])
						 
					);
				}
			}
		 
	


			if (!game::kcsgo2:: g_game->get_player_data (&list))
			{
				return;
			}



			 
			 
			for (auto it = list.begin(); it != list.end(); ++it)
			{
				  auto& player = *it;
			 
			  
				const Vector3 screenPos = game::kcsgo2::g_game->world_to_screen(&player.origin);
				const Vector3 screenHead = game::kcsgo2::g_game->world_to_screen(&player.head);

				if (screenPos.z < 0.01f ||!game::kcsgo2::g_game->is_in_bounds(screenPos))
				{
					continue;

				}


				const float height = screenPos.y - screenHead.y;
				const float width = height / 2.4f;
				float distance = game::kcsgo2::g_game->localOrigin.calculate_distance(player.origin);
				int roundedDistance = utils::math::round (distance / 10.f);

				int box_height = static_cast<int>(screenPos.y - screenHead.y);
				int box_width = box_height / 2;
				int box_x = static_cast<int>(screenHead.x - box_width / 2);
				int box_y = static_cast<int>(screenHead.y);

				// ==== Head Tracker (Circle) ====
				if (config::show_head_tracker.enabled)
				{
					const auto& head_bone = player.bones.bone_positions[game::kcsgo2struct::BONE_HEAD];
					
						 
						const float radius = width / 5.0f;

						FColor color(config::show_head_tracker.color[0], config::show_head_tracker.color[1], config::show_head_tracker.color[2]);

						rend.DrawCircle(
							head_bone.x,
							head_bone.y - (width / 12.0f),
							radius,
							color,
							1.0f
						);
				

				 
			
				}

				// ==== Bone ESP ====
				if (config::show_skeleton_esp.enabled)
				{

					FColor color(config::show_skeleton_esp.color[0], config::show_skeleton_esp.color[1], config::show_skeleton_esp.color[2]);
					//if (game::kcsgo2::g_game->isC4Planted)
					//{
					// 
					//	  
					//	float distance_to_c4 = player.origin.calculate_distance(game::kcsgo2::g_game->c4Origin);

					//	if (distance_to_c4 < 5.0f) // 根据需要调小或调大此阈值
					//	{
					//		color = FColor(255, 0, 0); // 红色
					//	}
					//}

				 

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


				// ==== Box ESP ====
				if (config::show_box_esp.enabled)
				{
				

					FColor color(config::show_box_esp.color[0], config::show_box_esp.color[1], config::show_box_esp.color[2]);
					rend.Rectangle(box_x, box_y, static_cast<float>(box_width), static_cast<float>(box_height), color, 1);
				}

				//rend.Rectangle(
				//	screenHead.x - (width / 2 + 10),
				//	screenHead.y + (height * (100 - player.armor) / 100),
				//	2,
				//	height - (height * (100 - player.armor) / 100),
				//	FColor(255, 185, 0)
				//);
				rend.Rectangle(
					screenHead.x - (width / 2 + 10 + 6), // 左移6像素，放到血条左边
					screenHead.y + (height * (100 - player.armor) / 100),
					2,
					height - (height * (100 - player.armor) / 100),
					FColor(255, 185, 0)
				);

				if (config::show_health_bar.enabled)
				{
					// ==== 血条绘制 ====
					const int bar_width = 4;
					const int bar_height = height;

					// ===== 背景（灰） =====
					rend.Rectangle(
						screenHead.x - (width / 2 + 4 + bar_width),  // 偏移与护甲相对对称
						screenHead.y,
						bar_width,
						bar_height,
						FColor(50, 50, 50), // 灰色背景
						1.0f
					);

					// ===== 颜色渐变（红->绿） =====
					FColor health_color(
						static_cast<uint8_t>(255 * (1.0f - player.health / 100.0f)),  // 红色
						static_cast<uint8_t>(255 * (player.health / 100.0f)),         // 绿色
						0
					);

					rend.Rectangle(
						screenHead.x - (width / 2 + 4 + bar_width),
						screenHead.y + bar_height * (1.0f - player.health / 100.0f),
						bar_width,
						bar_height * (player.health / 100.0f),
						health_color,
						1.0f
					);
				}
				//rend.Rectangle(
				//	 
				//	screenHead.x - (width / 2 + 5),
				//	screenHead.y + (height * (100 - player.health) / 100),
				//	2,
				//	height - (height * (100 - player.health) / 100),
				//	FColor(
				//		75,
				//		(55 + player.health * 2),
				//		(255 - player.health)
				//	)
				//);

				//rend.StringA (
				//	g_Font,
				//	screenHead.x + (width / 2 + 5),
				//	screenHead.y,
				//	player.name,
				//	FColor  (config::ShowName.color[0], config::ShowName.color[1], config::ShowName.color[2])
				//	 //10
				//);

				/**
				* I know is not the best way but a simple way to not saturate the screen with a ton of information
				*/
				if (roundedDistance > config::flag_render_distance)
					continue;


			

				/*	char health_str[32] = { 0 };
					if (!utils::string_utils::int_to_string(player.health, health_str, sizeof(health_str)))
					{
						return;
					}


						rend.StringA(
							g_Font,
							screenHead.x + (width / 2 + 5),
							screenHead.y + 10,
							utils::string_utils::concat_strings_no_alloc(health_str, "hp", sizeof(health_str)),
							FColor(
								(255 - player.health),
								(55 + player.health * 2),
								75)
						);



					char armor_str[32] = { 0 };

					if (!utils::string_utils::int_to_string(player.armor, armor_str, sizeof(armor_str)))
					{
						return;
					}

						rend.StringA(
							g_Font,
							screenHead.x + (width / 2 + 5),
							screenHead.y + 20,
							utils::string_utils::concat_strings_no_alloc(armor_str, "armor", sizeof(armor_str)),
							FColor(
								75,
								(55 + player.armor * 2),
								(255 - player.armor)
							)
						);*/
					 

				if (config::show_extra_flags.enabled)
				{


					rend.StringA(
						g_Font,
						screenHead.x + (width / 2 + 5),
						screenHead.y + 30,
						player.weapon,
						FColor(config::show_extra_flags.color[0], config::show_extra_flags.color[1], config::show_extra_flags.color[2])
						 //10
						 
					);


					char  dist_str[32] = { 0 };

					if (!utils::string_utils::int_to_string(roundedDistance, dist_str, sizeof(dist_str)))
					{
						return;
					}
					 

				 
						rend.StringA(
							g_Font,
							screenHead.x + (width / 2 + 5),
							screenHead.y + 40,
							utils::string_utils::concat_strings_no_alloc(dist_str, "m away", sizeof(dist_str)),
							FColor(
								config::show_extra_flags.color[0],
								config::show_extra_flags.color[1],
								config::show_extra_flags.color[2]
							)
						);
 


					char* money_str = utils::string_utils::int_to_string_alloc(player.money);
					if (!money_str) return;

					char* money_display_str = utils::string_utils::concat_strings_alloc("$", money_str);

					if (money_display_str)
					{
						rend.StringA(
							g_Font,
							screenHead.x + (width / 2 + 5),
							screenHead.y + 50,
							money_display_str,
							FColor(0, 125, 0)
						);

						utils::internal_functions::pfn_ex_free_pool_with_tag(money_display_str, 0);
					}

					utils::internal_functions::pfn_ex_free_pool_with_tag(money_str, 0);


					if (player.flashAlpha > 100)
					{
						rend.StringA(
							g_Font,
							screenHead.x + (width / 2 + 5),
							screenHead.y + 60,
							"Player is flashed",
							FColor(
								config::show_extra_flags.color[0],
								config::show_extra_flags.color[1],
								config::show_extra_flags.color[2]
							)
							//10
						);
					}

					if (player.is_defusing)
					{
						const char * defuText = "Player is defusing";
						rend.StringA(
							g_Font,
							screenHead.x + (width / 2 + 5),
							screenHead.y + 60,
							defuText,
							FColor(
								config::show_extra_flags.color[0],
								config::show_extra_flags.color[1],
								config::show_extra_flags.color[2]
							)
							//10
						);
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
			 

			if (utils::os_info::get_build_number() >= WINDOWS_11_VERSION_24H2)
			{
				render_overlay_frame_win1124h2(draw_overlay_elements);
			}
			else
			{
				render_overlay_frame(draw_overlay_elements);
			}
			
		


		 
			

		 
		 
		
		}

		 

		 

	}
}

