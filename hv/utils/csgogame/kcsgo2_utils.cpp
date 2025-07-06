#include "../global_defs.h"
#include "../dx_draw/math.h"
#include "kcsgo2_utils.h"

namespace game
{
	namespace kcsgo2
	{
		PEPROCESS g_game_process{};
		unsigned long long g_client_base{};
		unsigned long long g_client_size{};
		unsigned long long g_engine2_base{};
		unsigned long long g_engine2_size{};
		kcsgo2struct::CPlayer g_player_array[60] = {};
	    int g_player_count = 0;
		bool g_is_initialized{};
		LARGE_INTEGER g_game_time{};
		PVOID g_user_buffer{};
		HANDLE  g_game_handle{};
		POINT  g_game_size{};

	 
		NTSTATUS cleanup_game_process(_In_ PEPROCESS process)
		{ 
			g_game_process = nullptr;
			g_is_initialized = false;

		 	RtlZeroMemory(g_player_array, sizeof(g_player_array));
			g_player_count = 0;

			g_client_base = 0;
			g_client_size = 0;
			g_engine2_base = 0;
			g_engine2_size = 0;
			g_game_time.QuadPart = 0;
			return STATUS_SUCCESS;
		}

		bool is_game_module(_In_ PEPROCESS process, _In_ PWCHAR module_name)
		{
		 

			if (!module_name)
			{
				return false;
			}

			if (!process)
			{
				return false;
			}
			 

		 if (!utils::process_utils::is_process_name_match_wstr(process, L"cs2.exe", TRUE))
		 {
				  return false;
		  }

		   
		  
			return true;
		}

		bool is_game_process(_In_ PEPROCESS process)
		{
			 if (!process)
			 {
				 return false;
			 }

			 if (process != g_game_process)
			 {
				 return false;
			 }

			 return true;

		}
		bool is_game_process_initialized()
		{ 
			if (!g_is_initialized)
			{
				return false;
			}

			LARGE_INTEGER current_time = {};
			KeQuerySystemTime(&current_time);  // 兼容 Win7

			// 判断是否超过 30 秒（单位：100 纳秒 -> 1 秒 = 10,000,000）
			const LONGLONG time_diff = current_time.QuadPart - g_game_time.QuadPart;
			if (time_diff > 10 * 10 * 1000 * 1000) // 30秒
			{
				return true;
			}

			return false;
		}

		bool get_cs2_window_info(HANDLE hwnd, POINT* screen_size)
		{
			BOOL is_success = FALSE;
			unsigned long long get_window_rect_fun = utils::dwm_draw::g_get_window_rect_fun;

			if (!g_user_buffer)
			{
				if (!NT_SUCCESS(utils::memory::allocate_user_memory(&g_user_buffer, 0x1000, PAGE_READWRITE, true, true)))
				{
					return false;
				}
			}

			if (!screen_size)
			{
				return false;
			}

			// 假设 RECT 在用户空间的偏移地址
			unsigned long long rect_ptr = reinterpret_cast<unsigned long long>(g_user_buffer) + 0x500;

			unsigned long long result_ptr = utils::user_call::call(
				get_window_rect_fun,
				reinterpret_cast<unsigned long long>(hwnd),
				rect_ptr,
				0,
				0
			);

			if (result_ptr)
			{
				// 1. 强制转换为返回值类型指针
				BOOL* result_val_ptr = reinterpret_cast<BOOL*>(result_ptr);
				
				// 2. 解引用后赋值
				is_success = *result_val_ptr;

				// 3. 如果成功了，解析 RECT 数据写入 screen_size
				if (is_success  )
				{
				
					
					RECT* rect = reinterpret_cast<RECT*>(rect_ptr);
					screen_size->x = rect->right - rect->left;
					screen_size->y = rect->bottom - rect->top;
					utils::memory::free_user_memory(utils::internal_functions::pfn_ps_get_current_process_id(), g_user_buffer, 0x1000, true);
					g_user_buffer = nullptr;
				}
			}

			return is_success ? true : false;

		}
		bool initialize_game_process(OUT PEPROCESS *outprocess ,OUT LARGE_INTEGER *process_create_time)
		{
			if (g_is_initialized && g_game_process != nullptr)
			{
				return true;
			}

		 
			HANDLE  game_handle = find_cs2_window();
			if (!game_handle)
			{

				return false;

			}

			g_game_handle = game_handle;
			


			// 获取进程ID
			HANDLE process_id = utils::internal_functions::pfn_nt_user_query_window(game_handle, WindowProcess);
			if (!process_id)
			{
				return false;

			}

			// 尝试获取进程对象
			PEPROCESS process = nullptr;
			NTSTATUS status = utils::internal_functions::pfn_ps_lookup_process_by_process_id(process_id, &process);
			if (!NT_SUCCESS(status))
			{
				return false;
			}
			utils::internal_functions::pfn_ob_dereference_object(process);
			// 设置状态
			*outprocess = process;
			g_is_initialized = true;
		 
			KeQuerySystemTime(&process_create_time);
	 
			return true;
		}

		HANDLE find_cs2_window()
		{
			static PVOID g_user_buffer = nullptr;
			HANDLE handle{};
			if (!g_user_buffer)
			{
				if (!NT_SUCCESS(utils::memory::allocate_user_memory(&g_user_buffer, 0x1000, PAGE_READWRITE, true, true)))
				{
					return nullptr;
				}
			}

			// 固定偏移，确保不重叠
			PVOID class_name_ptr = reinterpret_cast<PBYTE>(g_user_buffer) + 0x100;
			PVOID window_name_ptr = reinterpret_cast<PBYTE>(g_user_buffer) + 0x300;

			// 第一次尝试："Counter-Strike 2"
			const wchar_t* k_class_name = L"SDL_app";
			const wchar_t* k_window_name_1 = L"Counter-Strike 2";

			SIZE_T class_name_len = (wcslen(k_class_name) + 1) * sizeof(WCHAR);
			SIZE_T window_name_len_1 = (wcslen(k_window_name_1) + 1) * sizeof(WCHAR);

			RtlCopyMemory(class_name_ptr, k_class_name, class_name_len);
			reinterpret_cast<wchar_t*>(class_name_ptr)[(class_name_len / sizeof(WCHAR)) - 1] = L'\0';

			RtlCopyMemory(window_name_ptr, k_window_name_1, window_name_len_1);
			reinterpret_cast<wchar_t*>(window_name_ptr)[(window_name_len_1 / sizeof(WCHAR)) - 1] = L'\0';

			unsigned long long result_ptr = utils::user_call::call(
				utils::dwm_draw::g_find_windoww_fun,
				reinterpret_cast<unsigned long long>(class_name_ptr),
				reinterpret_cast<unsigned long long>(window_name_ptr),
				0, 0);

			if (result_ptr)
			{
			     
				 handle =   reinterpret_cast<HANDLE>(*reinterpret_cast<PULONG64>(result_ptr));
				 if (handle)
				 {
					 utils::memory::free_user_memory(utils::internal_functions::pfn_ps_get_current_process_id(), g_user_buffer, 0x1000, true);
					 g_user_buffer = nullptr;
					 return handle;
				 }

			}

			// 第二次尝试："反恐精英：全球攻势"
			const wchar_t* k_window_name_2 = L"反恐精英：全球攻势";
			SIZE_T window_name_len_2 = (wcslen(k_window_name_2) + 1) * sizeof(WCHAR);

			RtlCopyMemory(window_name_ptr, k_window_name_2, window_name_len_2);
			reinterpret_cast<wchar_t*>(window_name_ptr)[(window_name_len_2 / sizeof(WCHAR)) - 1] = L'\0';

			result_ptr = utils::user_call::call(
				utils::dwm_draw::g_find_windoww_fun,
				reinterpret_cast<unsigned long long>(class_name_ptr),
				reinterpret_cast<unsigned long long>(window_name_ptr),
				0, 0);

			if (result_ptr)
			{
				handle = reinterpret_cast<HANDLE>(*reinterpret_cast<PULONG64>(result_ptr));
				if (handle)
				{
					utils::memory::free_user_memory(utils::internal_functions::pfn_ps_get_current_process_id(), g_user_buffer, 0x1000, true);
					g_user_buffer = nullptr;
					return handle;
				}
			}

			return nullptr;
		}
		bool initialize_game_data()
		{
		
			 

			unsigned long long client_base{};
			unsigned long long client_size{};
			unsigned long long engine2_base{};
			unsigned long long engine2_size{};

			 
			if (!initialize_game_process(&g_game_process,&g_game_time))
			{
				return false;
			}
			
			if (!is_game_process_initialized())
			{
				return false;
			}
			if (!get_cs2_window_info(g_game_handle, &g_game_size))
			{
				return false;
			}
			 

			KAPC_STATE apc_state{};
			utils:: internal_functions::pfn_ke_stack_attach_process(g_game_process, &apc_state);

		 
			RtlZeroMemory(g_player_array, sizeof(g_player_array));
			g_player_count = 0;
		  
		 
			 if (!g_client_base)
			 {
				 if (!NT_SUCCESS(utils::module_info::get_process_module_info(g_game_process, L"client.dll", &client_base, &client_size)) )
				 {
					 utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);

					 return false;
				 }
				 g_client_base = client_base;
				 g_client_size = client_size;
			 }

			 if (!g_engine2_base)
			 {
				 if (!NT_SUCCESS (utils::module_info::get_process_module_info(g_game_process, L"engine2.dll", &engine2_base, &engine2_size)))
				 {
					 utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);

					 return false;
				 }
				 g_engine2_base = engine2_base;
				 g_engine2_size = engine2_size;
				 
			 }
		

			
			

			uintptr_t local_pawn_addr = g_client_base + cs2SDK::offsets::dwLocalPlayerPawn;
			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(local_pawn_addr)))
			{
				utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);

				return false;
			}
			memcpy(&kcsgo2data::g_local_pcsplayer_pawn, reinterpret_cast<void*>(local_pawn_addr), sizeof(kcsgo2data::g_local_pcsplayer_pawn));

			if (!kcsgo2data::g_local_pcsplayer_pawn)
			{
				utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);

				return false;
			}

			kcsgo2data::g_entity_list = g_client_base + cs2SDK::offsets::m_offestPlayerArray;
			if (!kcsgo2data::g_entity_list)
			{
				utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);

				return false;
			}


			int health = 0;
			if (utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_iHealth)))
			{
				memcpy(&health, reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_iHealth), sizeof(int));
			}

			if (health > 0 && health <= 100)
			{
				int team = 0;
				void* team_addr = reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_offsettema);
				if (utils::internal_functions::pfn_mm_is_address_valid_ex(team_addr))
				{
					memcpy(&kcsgo2data::g_local_team, team_addr, sizeof(int));
				}
			}



			for (size_t i = 0; i < 60; i++)
			{
				 
				void* origin_addr = reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_vLastSlopeCheckPos);
				if (utils::internal_functions::pfn_mm_is_address_valid_ex(origin_addr))
				{
					memcpy(&kcsgo2data::g_local_origin, origin_addr, sizeof(Vector3));
				}

				uintptr_t player_pawn = 0;
				void* pawn_ptr = reinterpret_cast<void*>(kcsgo2data::g_entity_list + i * sizeof(uintptr_t));
				if (utils::internal_functions::pfn_mm_is_address_valid_ex(pawn_ptr))
				{
					memcpy(&player_pawn, pawn_ptr, sizeof(uintptr_t));
				}

				if (!player_pawn || player_pawn == kcsgo2data::g_local_pcsplayer_pawn)
				{
					continue;
				}

				int team = 0;
				void* team_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_offsettema);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(team_addr))
				{
					continue;
				}

				memcpy(&team, team_addr, sizeof(int));
				if (team == kcsgo2data::g_local_team || team < 2)
				{
					continue;
				}

				int health = 0;
				void* health_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_iHealth);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(health_addr))
				{
					continue;
				}

				memcpy(&health, health_addr, sizeof(int));
				

				if (health <= 0 || health > 100)
				{
					continue;
				}

				int is_valid = 0;
				void* valid_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_offestbIsPlayerExists);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(valid_addr))
				{
					continue;

				}
				memcpy(&is_valid, valid_addr, sizeof(int));
				if (!is_valid)
				{
					continue;
				}


				kcsgo2struct::CPlayer& player = g_player_array[g_player_count];

				player.pCSPlayerPawn = player_pawn;
				player.team = team;
				player.health = health;
				player.bIsPlayerExists = is_valid;
				player.is_local_player = false;

				// 读 origin
				void* tagorigin_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_vLastSlopeCheckPos);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(origin_addr))
				{
					continue;
				}
				memcpy(&player.origin, tagorigin_addr, sizeof(Vector3));
				player.head = { player.origin.x, player.origin.y, player.origin.z + 70.f };

				// 读 scene node
				void* scene_node_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_pGameSceneNode);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(scene_node_addr))
				{
					continue;
				}
				memcpy(&player.gameSceneNode, scene_node_addr, sizeof(uintptr_t));

				player.is_local_player = false;

			    void * ViewMatrix_addr = reinterpret_cast<void*>(g_client_base + cs2SDK::offsets::dwViewMatrix);
			    if ( utils::internal_functions::pfn_mm_is_address_valid_ex(ViewMatrix_addr))
				{
					memcpy(&kcsgo2data::g_view_matrix, ViewMatrix_addr, sizeof(matrix4x4_t));
				}

				/*DbgPrintEx(
					DPFLTR_IHVDRIVER_ID, 0,
					"[CSGO2] Player[%d] pCSPlayerPawnAddr=0x%llX Team=%d HP=%d Pos=(%ld, %ld, %ld) Exists=%d\n",
					g_player_count,
					player.pCSPlayerPawn,
					player.team,
					player.health,
					(LONG)(player.origin.x * 10),
					(LONG)(player.origin.y * 10),
					(LONG)(player.origin.z * 10),
					player.bIsPlayerExists
				);*/
				g_player_count++;
			}

			utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			return true;
		}

		NTSTATUS hook_get_cosg_hp(_In_ PEPROCESS process)
		{

			if (!process)
			{
				return STATUS_INVALID_PARAMETER;
			}

			HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);

			//client.dll +85DA80
			//获取人物血量 利用这个钩子来获取要绘制的人物数据
			unsigned long long client_base{};
			unsigned  long long client_size{};
			unsigned long long get_csgo_hp_fun  =  client_base + 0x85DA80;
			if (!NT_SUCCESS(utils::module_info::get_process_module_info(process, L"client.dll", &client_base, &client_size)))
			{
				return  STATUS_UNSUCCESSFUL;
			}

			bool hook_result = hyper::ept_hook_break_point_int3(
				process_id,
				reinterpret_cast<PVOID>(get_csgo_hp_fun),
				hook_functions::new_get_csgo_hp,
				nullptr
			);

			return hook_result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
		}

	}
}




