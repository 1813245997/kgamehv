#include "../global_defs.h"
#include "../dx_draw/math.h"
#include "kcsgo2_utils.h"


namespace game
{
	namespace kcsgo2
	{
	 
		 
	
		 
	

		bool CGame::init(_In_ PEPROCESS process)
		{

		 

			if (m_is_initialized)
			{
				return true;
			}


		 
			m_base_client = utils::module_info::get_module(process,  L"client.dll");
			m_base_engine = utils::module_info::get_module(process,   L"engine2.dll" );

			if (m_base_client.base == 0 || m_base_engine.base == 0)
			{
				return false;
			}



			m_buildNumber = utils::memory::read<uintptr_t>(m_base_engine.base + updater::offsets::dwBuildNumber);

			if (g_game->m_buildNumber != updater::offsets::build_number)
			{
				m_cheat_update = true;
				return false;
			}

			m_game_pid = utils::internal_functions::pfn_ps_get_process_id(process);
			m_game_process = process;
			unsigned  long long get_hp_fun = m_base_client.base + updater::offsets::m_hook_offset;
			utils::hook_utils::hook_user_exception_handler(
				process,
				reinterpret_cast<PVOID>(get_hp_fun),
				hook_functions::new_get_csgo_hp,
				false


			);
			m_is_initialized = true;

			return true;
		}

		bool CGame::is_init2()
		{
			return m_is_initialized;
		}
		bool  CGame::init2(_In_ PEPROCESS process)
		{
			if (m_is_initialized)
			{
				return true;
			}

		 
		 
			 

			m_base_client = utils::module_info::get_module(process,   L"client.dll");
			m_base_engine = utils::module_info::get_module(process, L"engine2.dll");

			if (m_base_client.base == 0 || m_base_engine.base == 0)
			{
				return false;
			}

		     
		
			 m_game_cr3 = utils::process_utils::get_process_cr3(process);

			 m_game_process = process;
			 m_buildNumber =  read<uintptr_t>(m_base_engine.base + updater::offsets::dwBuildNumber);

			 /* if (g_game->m_buildNumber != updater::offsets::build_number)
			  {
				  m_cheat_update = true;
				  return false;
			  }*/
			   
		
		
			m_game_pid = utils::internal_functions::pfn_ps_get_process_id(process);
			// 标记已初始化
			m_is_initialized = true;

			return true;



		}
		void initialize_player_data_lock()
		{
			 
			game::kcsgo2::g_game =(game::kcsgo2::CGame*) utils::internal_functions::pfn_ex_allocate_pool_with_tag(NonPagedPool, sizeof(game::kcsgo2::CGame), 0);
			ExInitializeFastMutex(&g_game->m_player_data_lock);
			ExInitializeFastMutex(&g_game->m_g_data_lock);
			g_game->players.init();
			g_game->clear();
		 
			


		}


		void CGame::clear()
		{
			m_is_initialized = false;

			// 清空指针和句柄
			m_game_process = nullptr;
			m_game_pid = nullptr;
			m_game_handle = nullptr;
			m_game_cr3 = 0;

			// 清空基础结构体
			m_game_size = { 0, 0 };
			m_game_bounds = { 0, 0, 0, 0 };
			m_base_client = {};
			m_base_engine = {};
			m_buildNumber = 0;

			// 清空状态标志和临时数据
			isC4Planted = false;
			localTeam = 0;
			localOrigin = {};
			c4Origin={};
			// 清空地址和矩阵数据
			entity_list = 0;
			localPlayer = 0;
			localPlayerPawn = 0;
			localpCSPlayerPawn = 0;
			localList_entry2 = 0;
			view_matrix = {};
			m_cheat_update = false;
			reset_player_data();

		}

		bool CGame::get_cs2_window_info(HANDLE hwnd, POINT* screen_size, PVOID user_buffer)
		{
			BOOL is_success = FALSE;
			unsigned long long get_window_rect_fun = utils::dwm_draw::g_get_window_rect_fun;

			 
			if (!screen_size)
			{
				return false;
			}
			
			// 假设 RECT 在用户空间的偏移地址
			unsigned long long rect_ptr = reinterpret_cast<unsigned long long>(user_buffer) + 0x500;

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
					m_game_bounds = *rect;
					//utils::memory::free_user_memory(utils::internal_functions::pfn_ps_get_current_process_id(), g_user_buffer, 0x1000, false);
					//g_user_buffer = nullptr;
				}
			}

			return is_success ? true : false;

		}
		 

	  

		bool CGame::read_raw(uintptr_t address, void* buffer, size_t size)
		{
			if (buffer == nullptr || size == 0)
			{
				return false;
			}
			bool is_succeed = false;
			KAPC_STATE apc_state{};

			if (!m_game_process)
			{
				return  is_succeed;
			}

			if (utils::internal_functions::pfn_ps_get_process_exit_status(m_game_process) != STATUS_PENDING)
			{

				return  is_succeed;
			}
			utils::internal_functions::pfn_ke_stack_attach_process(m_game_process, &apc_state);
			is_succeed = utils::memory::read_raw(address, &buffer, size);
			utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);

			return  is_succeed;

		/*	if (buffer == nullptr || size == 0)
			{
				return false;
			}
			ULONG size2 = size;
			NTSTATUS status = utils::memory::read_virtual_memory(m_game_cr3, reinterpret_cast<void*> (address), &buffer, &size2);
			if (!NT_SUCCESS(status))
			{

				return false;
			}

			 
			return true;*/
		}

		HANDLE CGame::find_cs2_window(PVOID user_buffer)
		{
			 
			HANDLE handle{};
		 

			// 固定偏移，确保不重叠
			PVOID class_name_ptr = reinterpret_cast<PBYTE>(user_buffer) + 0x100;
			PVOID window_name_ptr = reinterpret_cast<PBYTE>(user_buffer) + 0x300;

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
					 RtlZeroMemory(user_buffer, 0X1000);
					 // utils::memory::free_user_memory(utils::internal_functions::pfn_ps_get_current_process_id(), g_user_buffer, 0x1000, false);
					  //g_user_buffer = nullptr;
					 return handle;
				 }

			}

			// 第二次尝试："反恐精英：全球攻势"
			const wchar_t* k_window_name_2 =   L"反恐精英：全球攻势" ;
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
					RtlZeroMemory(user_buffer, 0X1000);
					//utils::memory::free_user_memory(utils::internal_functions::pfn_ps_get_current_process_id(), g_user_buffer, 0x1000, false);
					//g_user_buffer = nullptr;
					return handle;
				}
			}

			return nullptr;
		}

	
		void  CGame::set_player_data(utils::kvector<kcsgo2struct::CPlayer>& list)
		{
			ExAcquireFastMutex(&m_player_data_lock);

			players.assign(list.begin(), list.end());
			ExReleaseFastMutex(&m_player_data_lock);
		}

		void CGame::reset_player_data()
		{
			ExAcquireFastMutex(&m_player_data_lock);
			players.clear();
			ExReleaseFastMutex(&m_player_data_lock);
		}


		bool CGame::get_player_data(utils::kvector<kcsgo2struct::CPlayer>* out_list)
		{
			if (!out_list)
				return false;

			ExAcquireFastMutex(&m_player_data_lock);

			if (players.empty())
			{
				ExReleaseFastMutex(&m_player_data_lock);
				return false;
			}

			out_list->assign(players.begin(), players.end());

			ExReleaseFastMutex(&m_player_data_lock);
			return true;
		}

		//bool CGame::world_to_screen(const Vector3* v, Vector3* out)
		//{
		//	matrix4x4_t  Pview = view_matrix;
		//	Vector3  pos{};
		//	pos.x = v->x;
		//	pos.y = v->y;
		//	pos.z = v->z;
		//	Vector4 clipCoords;

		//	// 计算裁剪坐标
		//	clipCoords.x = pos.x * Pview[0] + pos.y * Pview[1] + pos.z * Pview[2] + Pview[3];
		//	clipCoords.y = pos.x * Pview[4] + pos.y * Pview[5] + pos.z * Pview[6] + Pview[7];
		//	clipCoords.z = pos.x * Pview[8] + pos.y * Pview[9] + pos.z * Pview[10] + Pview[11];
		//	clipCoords.w = pos.x * Pview[12] + pos.y * Pview[13] + pos.z * Pview[14] + Pview[15];

		//	// 如果 w 小于一个很小的值，返回 false
		//	if (clipCoords.w < 0.01f)
		//		return false;

		//	// 计算 NDC（规范化设备坐标）
		//	Vector3  NDC;
		//	NDC.x = clipCoords.x / clipCoords.w;
		//	NDC.y = clipCoords.y / clipCoords.w;
		//	NDC.z = clipCoords.z / clipCoords.w;

		//	// 计算最终屏幕坐标
		//	out->x = (m_game_size.x / 2 * NDC.x) + (NDC.x + m_game_size.x / 2);
		//	out->y = -(m_game_size.y / 2 * NDC.y) + (NDC.y + m_game_size.y / 2);
		//	out->z = v->z;  // 保持原 z 坐标

		//	if (out->x < 0 || out->x > m_game_size.x || out->y < 0 || out->y > m_game_size.y)
		//		return false;

		//	return true;
		// 
		//}

		uintptr_t CC4::get_planted() {
			return  g_game->read<uintptr_t>(g_game->read<uintptr_t>(game::kcsgo2::g_game->m_base_client.base + updater::offsets::dwPlantedC4));
		}

		uintptr_t CC4::get_node() {
			return g_game->read<uintptr_t>(get_planted() + updater::offsets::m_pGameSceneNode);
		}

		Vector3 CC4::get_origin() {
			return g_game->  read<Vector3>(get_node() + updater::offsets::m_vecAbsOrigin);
		}


		Vector3 CGame::world_to_screen(Vector3* v)
		{
			float _x = view_matrix[0][0] * v->x + view_matrix[0][1] * v->y + view_matrix[0][2] * v->z + view_matrix[0][3];
			float _y = view_matrix[1][0] * v->x + view_matrix[1][1] * v->y + view_matrix[1][2] * v->z + view_matrix[1][3];

			float w = view_matrix[3][0] * v->x + view_matrix[3][1] * v->y + view_matrix[3][2] * v->z + view_matrix[3][3];

			float inv_w = 1.f / w;
			_x *= inv_w;
			_y *= inv_w;

			float x = m_game_bounds.right * .5f;
			float y = m_game_bounds.bottom * .5f;

			x += 0.5f * _x * m_game_bounds.right + 0.5f;
			y -= 0.5f * _y * m_game_bounds.bottom + 0.5f;

			return { x, y, w };
		}

		bool CGame::is_in_bounds(const Vector3& pos)
		{
			int width = m_game_bounds.right;
			int height = m_game_bounds.bottom;
		 
			return pos.x >= 0 && pos.x <= width && pos.y >= 0 && pos.y <= height;
		}
		 
	 
		 


		void CGame::loop()
		{

			 
			if (!m_game_handle)
			{
				m_game_handle = find_cs2_window(0);

				if (!m_game_handle)
				{
					return  ;
				}

				if (!get_cs2_window_info(m_game_handle, &m_game_size, 0))

				{
					return  ;
				}
			}

		 
			isC4Planted = false;

			localPlayer =  utils::memory:: read<uintptr_t>(m_base_client.base + updater::offsets::dwLocalPlayerController);
			if (!localPlayer)
			{
				return;
			}

			localPlayerPawn = utils::memory::read< uint32_t>(localPlayer + updater::offsets::m_hPlayerPawn);
			if (!localPlayerPawn)
			{
				return;
			}

			entity_list = utils::memory::read<uintptr_t>(m_base_client.base + updater::offsets::dwEntityList);
			if (!entity_list)
			{
				return;
			}
			

			localList_entry2 = utils::memory::read<uintptr_t>(entity_list + 0x8 * ((localPlayerPawn & 0x7FFF) >> 9) + 16);
			localpCSPlayerPawn = utils::memory::read<uintptr_t>(localList_entry2 + 120 * (localPlayerPawn & 0x1FF));
			if (!localpCSPlayerPawn)
			{
				return;
			}
			view_matrix = utils::memory::read<view_matrix_t>(m_base_client.base + updater::offsets::dwViewMatrix);


			localTeam = utils::memory::read<int>(localPlayer + updater::offsets::m_iTeamNum);
			localOrigin = utils::memory::read<Vector3>(localpCSPlayerPawn + updater::offsets::m_vOldOrigin);
			isC4Planted = utils::memory::read<bool>(m_base_client.base + updater::offsets::dwPlantedC4 - 0x8);


			int playerIndex = 0;

			utils::kvector<kcsgo2struct::CPlayer>list;
			kcsgo2struct::CPlayer player{};
			uintptr_t list_entry,list_entry2, playerPawn, playerMoneyServices, clippingWeapon, weaponData ;

			 
			if (isC4Planted)
			{
				c4Origin = game::kcsgo2::g_game->c4.get_origin();

			}
		 

			while (true)
			{
				playerIndex++;
				list_entry = utils::memory::read<uintptr_t>(entity_list + (8 * (playerIndex & 0x7FFF) >> 9) + 16);
				if (!list_entry) break;

				player.entity = utils::memory::read<uintptr_t>(list_entry + 120 * (playerIndex & 0x1FF));
				if (!player.entity) continue;

				/**
				* Skip rendering your own character and teammates
				*
				* If you really want you can exclude your own character from the check but
				* since you are in the same team as yourself it will be excluded anyway
				**/

				player.team = utils::memory::read<int>(player.entity + updater::offsets::m_iTeamNum);
				//if (config::team_esp && (player.team == localTeam)) continue;
				if ( player.team == localTeam ) continue;

				playerPawn = utils::memory::read<  uint32_t>(player.entity + updater::offsets::m_hPlayerPawn);

				list_entry2 = utils::memory::read<uintptr_t>(entity_list + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
				if (!list_entry2) continue;


				player.pCSPlayerPawn = utils::memory::read<uintptr_t>(list_entry2 + 120 * (playerPawn & 0x1FF));
				if (!player.pCSPlayerPawn) continue;


				player.health = utils::memory::read<int>(player.pCSPlayerPawn + updater::offsets::m_iHealth);
				player.armor = utils::memory::read<int>(player.pCSPlayerPawn + updater::offsets::m_ArmorValue);
				if (player.health <= 0 || player.health > 100) continue;


				//if (config::team_esp && (player.pCSPlayerPawn == localPlayer)) continue;
				if (player.pCSPlayerPawn == localPlayer) continue;


				/*
				* Unused for now, but for a vis check
				*
				* player.spottedState = process->read<uintptr_t>(player.pCSPlayerPawn + 0x1630);
				* player.is_spotted = process->read<DWORD_PTR>(player.spottedState + 0xC); // bSpottedByMask
				* player.is_spotted = process->read<bool>(player.spottedState + 0x8); // bSpotted
				*/

				// Read entity controller from the player pawn
				uintptr_t handle = utils::memory::read< uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_hController);
				int index = handle & 0x7FFF;
				int segment = index >> 9;
				int entry = index & 0x1FF;

				uintptr_t controllerListSegment = utils::memory::read<uintptr_t>(entity_list + 0x8 * segment + 0x10);
				uintptr_t controller = utils::memory::read<uintptr_t>(controllerListSegment + 120 * entry);

				if (!controller)
					continue;

				// Read player name from the controller
				utils::memory::read_raw(controller + updater::offsets::m_iszPlayerName, player.name, sizeof(player.name) - 1);
				player.name[sizeof(player.name) - 1] = '\0';

				player.gameSceneNode = utils::memory::read<uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_pGameSceneNode);
				player.origin = utils::memory::read<Vector3>(player.pCSPlayerPawn + updater::offsets::m_vOldOrigin);
				player.head = { player.origin.x, player.origin.y, player.origin.z + 75.f };

				if (player.origin.x == localOrigin.x && player.origin.y == localOrigin.y && player.origin.z == localOrigin.z)
					continue;

			//	if (config::render_distance != -1 && (localOrigin - player.origin).length2d() > config::render_distance) continue;
				if (player.origin.x == 0 && player.origin.y == 0) continue;


				if (config::show_skeleton_esp.enabled)
				{
					player.gameSceneNode = utils::memory::read<uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_pGameSceneNode);
					player.boneArray = utils::memory::read<uintptr_t>(player.gameSceneNode + 0x1F0);
					player.ReadBones((matrix4x4_t*) & view_matrix, m_game_size);
				}
			
				//if (config::show_head_tracker && !config::show_skeleton_esp) {
				//	player.gameSceneNode = utils::memory::read<uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_pGameSceneNode);
				//	player.boneArray = utils::memory::read<uintptr_t>(player.gameSceneNode + 0x1F0);
				//	player.ReadHead();
				//}


				if (config::show_extra_flags.enabled) {
					/*
					* Reading values for extra flags is now separated from the other reads
					* This removes unnecessary memory reads, improving performance when not showing extra flags
					*/
					player.is_defusing = utils::memory::read<bool>(player.pCSPlayerPawn + updater::offsets::m_bIsDefusing);

					playerMoneyServices = utils::memory::read<uintptr_t>(player.entity + updater::offsets::m_pInGameMoneyServices);
					player.money = utils::memory::read<int>(playerMoneyServices + updater::offsets::m_iAccount);

					player.flashAlpha = utils::memory::read<float>(player.pCSPlayerPawn + updater::offsets::m_flFlashOverlayAlpha);

					clippingWeapon = utils::memory::read< uint64_t>(player.pCSPlayerPawn + updater::offsets::m_pClippingWeapon);
					 uint64_t firstLevel = utils::memory::read< uint64_t>(clippingWeapon + 0x10); // First offset
					weaponData = utils::memory::read< uint64_t>(firstLevel + 0x20); // Final offset
					/*weaponData = process->read<std::uint64_t>(clippingWeapon + 0x10);
					weaponData = process->read<std::uint64_t>(weaponData + updater::offsets::m_szName);*/

					CHAR buffer[MAX_PATH] = {  };
					CHAR prefix[] = "weapon_";

					utils::memory::read_raw(weaponData, buffer, sizeof(buffer));
					 
						// 判断前缀是否为 "weapon_"
					if (RtlCompareMemory(buffer, prefix, sizeof("weapon_") - 1) == sizeof("weapon_") - 1)
						// 拷贝去掉前缀后的内容到 player.weapon
						RtlCopyMemory(player.weapon, buffer + sizeof("weapon_") - 1, sizeof(player.weapon) - 1);
				
					else
					    RtlCopyMemory(player.weapon, "Invalid Weapon Name", sizeof("Invalid Weapon Name"));
					
					 
 
				}

				list.push_back(player);
			 
			}


			players.clear();

			players.assign(list.begin(), list.end());


		}
		 
		void CGame::loop2(PVOID user_buffer)
		{

			if (!user_buffer)
			{
				return;
			}

			if (!m_game_handle)
			{
				m_game_handle = find_cs2_window(user_buffer);

				if (!m_game_handle)
				{
					return;
				}

				if (!get_cs2_window_info(m_game_handle, &m_game_size, user_buffer))

				{
					return;
				}
			}

			 
			isC4Planted = false;

			localPlayer =  read<uintptr_t>(m_base_client.base + updater::offsets::dwLocalPlayerController);
			if (!localPlayer)
			{
				return;
			}

			localPlayerPawn =  read< uint32_t>(localPlayer + updater::offsets::m_hPlayerPawn);
			if (!localPlayerPawn)
			{
				return;
			}

			entity_list = read<uintptr_t>(m_base_client.base + updater::offsets::dwEntityList);
			if (!entity_list)
			{
				return;
			}


			localList_entry2 =  read<uintptr_t>(entity_list + 0x8 * ((localPlayerPawn & 0x7FFF) >> 9) + 16);
			localpCSPlayerPawn =  read<uintptr_t>(localList_entry2 + 120 * (localPlayerPawn & 0x1FF));
			if (!localpCSPlayerPawn)
			{
				return;
			}
			view_matrix =  read<view_matrix_t>(m_base_client.base + updater::offsets::dwViewMatrix);


			localTeam =  read<int>(localPlayer + updater::offsets::m_iTeamNum);
			localOrigin =  read<Vector3>(localpCSPlayerPawn + updater::offsets::m_vOldOrigin);
			isC4Planted =  read<bool>(m_base_client.base + updater::offsets::dwPlantedC4 - 0x8);


			int playerIndex = 0;

			utils::kvector<kcsgo2struct::CPlayer>list;
			kcsgo2struct::CPlayer player{};
			uintptr_t list_entry, list_entry2, playerPawn, playerMoneyServices, clippingWeapon, weaponData;


			if (isC4Planted)
			{
				c4Origin = game::kcsgo2::g_game->c4.get_origin();

			}


			while (true)
			{
				playerIndex++;
				list_entry =  read<uintptr_t>(entity_list + (8 * (playerIndex & 0x7FFF) >> 9) + 16);
				if (!list_entry) break;

				player.entity =  read<uintptr_t>(list_entry + 120 * (playerIndex & 0x1FF));
				if (!player.entity) continue;

				/**
				* Skip rendering your own character and teammates
				*
				* If you really want you can exclude your own character from the check but
				* since you are in the same team as yourself it will be excluded anyway
				**/

				player.team =  read<int>(player.entity + updater::offsets::m_iTeamNum);
				//if (config::team_esp && (player.team == localTeam)) continue;
				if (player.team == localTeam) continue;

				playerPawn =  read<  uint32_t>(player.entity + updater::offsets::m_hPlayerPawn);

				list_entry2 =  read<uintptr_t>(entity_list + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
				if (!list_entry2) continue;


				player.pCSPlayerPawn =  read<uintptr_t>(list_entry2 + 120 * (playerPawn & 0x1FF));
				if (!player.pCSPlayerPawn) continue;


				player.health =  read<int>(player.pCSPlayerPawn + updater::offsets::m_iHealth);
				player.armor =  read<int>(player.pCSPlayerPawn + updater::offsets::m_ArmorValue);
				if (player.health <= 0 || player.health > 100) continue;


				//if (config::team_esp && (player.pCSPlayerPawn == localPlayer)) continue;
				if (player.pCSPlayerPawn == localPlayer) continue;


				/*
				* Unused for now, but for a vis check
				*
				* player.spottedState = process->read<uintptr_t>(player.pCSPlayerPawn + 0x1630);
				* player.is_spotted = process->read<DWORD_PTR>(player.spottedState + 0xC); // bSpottedByMask
				* player.is_spotted = process->read<bool>(player.spottedState + 0x8); // bSpotted
				*/

				// Read entity controller from the player pawn
				uintptr_t handle =  read< uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_hController);
				int index = handle & 0x7FFF;
				int segment = index >> 9;
				int entry = index & 0x1FF;

				uintptr_t controllerListSegment = read<uintptr_t>(entity_list + 0x8 * segment + 0x10);
				uintptr_t controller =  read<uintptr_t>(controllerListSegment + 120 * entry);

				if (!controller)
					continue;

				// Read player name from the controller
				 read_raw(controller + updater::offsets::m_iszPlayerName, player.name, sizeof(player.name) - 1);
				player.name[sizeof(player.name) - 1] = '\0';

				player.gameSceneNode =  read<uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_pGameSceneNode);
				player.origin =  read<Vector3>(player.pCSPlayerPawn + updater::offsets::m_vOldOrigin);
				player.head = { player.origin.x, player.origin.y, player.origin.z + 75.f };

				if (player.origin.x == localOrigin.x && player.origin.y == localOrigin.y && player.origin.z == localOrigin.z)
					continue;

				//	if (config::render_distance != -1 && (localOrigin - player.origin).length2d() > config::render_distance) continue;
				if (player.origin.x == 0 && player.origin.y == 0) continue;


				if (config::show_skeleton_esp.enabled)
				{
					player.gameSceneNode =  read<uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_pGameSceneNode);
					player.boneArray =  read<uintptr_t>(player.gameSceneNode + 0x1F0);
					player.ReadBones((matrix4x4_t*)&view_matrix, m_game_size);
				}

				//if (config::show_head_tracker && !config::show_skeleton_esp) {
				//	player.gameSceneNode = utils::memory::read<uintptr_t>(player.pCSPlayerPawn + updater::offsets::m_pGameSceneNode);
				//	player.boneArray = utils::memory::read<uintptr_t>(player.gameSceneNode + 0x1F0);
				//	player.ReadHead();
				//}


				if (config::show_extra_flags.enabled) {
					/*
					* Reading values for extra flags is now separated from the other reads
					* This removes unnecessary memory reads, improving performance when not showing extra flags
					*/
					player.is_defusing =  read<bool>(player.pCSPlayerPawn + updater::offsets::m_bIsDefusing);

					playerMoneyServices =  read<uintptr_t>(player.entity + updater::offsets::m_pInGameMoneyServices);
					player.money =  read<int>(playerMoneyServices + updater::offsets::m_iAccount);

					player.flashAlpha =  read<float>(player.pCSPlayerPawn + updater::offsets::m_flFlashOverlayAlpha);

					clippingWeapon =  read< uint64_t>(player.pCSPlayerPawn + updater::offsets::m_pClippingWeapon);
					uint64_t firstLevel =  read< uint64_t>(clippingWeapon + 0x10); // First offset
					weaponData =  read< uint64_t>(firstLevel + 0x20); // Final offset
					/*weaponData = process->read<std::uint64_t>(clippingWeapon + 0x10);
					weaponData = process->read<std::uint64_t>(weaponData + updater::offsets::m_szName);*/

					CHAR buffer[MAX_PATH] = {  };
					CHAR prefix[] = "weapon_";

					 read_raw(weaponData, buffer, sizeof(buffer));

					// 判断前缀是否为 "weapon_"
					if (RtlCompareMemory(buffer, prefix, sizeof("weapon_") - 1) == sizeof("weapon_") - 1)
						// 拷贝去掉前缀后的内容到 player.weapon
						RtlCopyMemory(player.weapon, buffer + sizeof("weapon_") - 1, sizeof(player.weapon) - 1);

					else
						RtlCopyMemory(player.weapon, "Invalid Weapon Name", sizeof("Invalid Weapon Name"));



				}

				list.push_back(player);

			}


			players.clear();

			players.assign(list.begin(), list.end());
		}




}
}




