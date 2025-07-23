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
		kcsgo2struct::CPlayer g_player_array[MAX_PLAYER_NUM] = {};
	    int g_player_count = 0;
		bool g_is_initialized{};
		 
		PVOID g_user_buffer{};
		HANDLE  g_game_handle{};
		POINT  g_game_size{};

		FAST_MUTEX g_player_data_lock;
		LARGE_INTEGER g_process_time = {   };

		bool initialize_game_process(_In_ PEPROCESS process)
		{

			if (g_is_initialized && g_game_process != nullptr) {
				return true;
			}


			if (!utils::process_utils::is_process_name_match_wstr(process, L"cs2.exe", TRUE))
			{
				return false;
			}

			if (PsGetProcessExitStatus(process) != STATUS_PENDING)
			{ 
				
				utils::internal_functions::pfn_ob_dereference_object(process);
				return false;
			}

			utils::internal_functions::pfn_ob_dereference_object(process);

			unsigned long long client_base = 0;
			unsigned long long client_size = 0;
			unsigned long long engine2_base = 0;
			unsigned long long engine2_size = 0;

			NTSTATUS status1 = utils::module_info::get_process_module_info(process, L"client.dll", &client_base, &client_size);
			NTSTATUS status2 = utils::module_info::get_process_module_info(process, L"engine2.dll", &engine2_base, &engine2_size);


			if (!NT_SUCCESS(status1) || !NT_SUCCESS(status2)) {

				return false;
			}

			if (client_base == 0 || client_size == 0)
			{
				return false;
			}

			if (engine2_base == 0 || engine2_size == 0)
			{
				return false;
			}




			HANDLE process_id = utils::internal_functions::pfn_ps_get_process_id(process);
			unsigned  long long get_hp_fun = client_base + cs2SDK::offsets::m_hook_offset;
			hyper::ept_hook_break_point_int3(
				process_id,
				reinterpret_cast<PVOID>(get_hp_fun),
				hook_functions::new_get_csgo_hp,
				nullptr,
				false
			);



			KeQuerySystemTime(&g_process_time);
			g_game_process = process;
			g_client_base = client_base;
			g_client_size = client_size;
			g_engine2_base = engine2_base;
			g_engine2_size = engine2_size;
			g_is_initialized = true;


			return true;


		}


		bool initialize_game_process2( )
		{

			PEPROCESS process = nullptr;

			if (g_is_initialized)
			{
				return true;
			}
			 
			if (!utils::process_utils::get_process_by_name(L"cs2.exe", &process))
			{
				return false;
			}

			if (PsGetProcessExitStatus(process) != STATUS_PENDING)
			{
				utils:: internal_functions::pfn_ob_dereference_object(process);
				return false;
			}

			utils::internal_functions::pfn_ob_dereference_object(process);
			unsigned long long client_base = 0;
			unsigned long long client_size = 0;
			unsigned long long engine2_base = 0;
			unsigned long long engine2_size = 0;

			NTSTATUS status1 = utils::module_info::get_process_module_info(process, L"client.dll", &client_base, &client_size);
			NTSTATUS status2 = utils::module_info::get_process_module_info(process, L"engine2.dll", &engine2_base, &engine2_size);


			if (!NT_SUCCESS(status1) || !NT_SUCCESS(status2)) {

				return false;
			}

			if (client_base == 0 || client_size == 0)
			{
				return false;
			}

			if (engine2_base == 0 || engine2_size == 0)
			{
				return false;
			}
			 
			KeQuerySystemTime(&g_process_time);
			g_game_process = process;

			g_client_base = client_base;
			g_client_size = client_size;
			g_engine2_base = engine2_base;
			g_engine2_size = engine2_size;
			g_is_initialized = true;


			return true;


		}
		bool initialize_game_process3(HANDLE process_id)
		{
			PEPROCESS process = nullptr;
			NTSTATUS status = utils::internal_functions::pfn_ps_lookup_process_by_process_id (process_id, &process);
			if (!NT_SUCCESS(status) || process == nullptr)
			{
				return false;
			}

			if (PsGetProcessExitStatus(process) != STATUS_PENDING)
			{
				utils::internal_functions::pfn_ob_dereference_object(process);
				return false;
			}
			utils::internal_functions::pfn_ob_dereference_object(process);
			KeQuerySystemTime(&g_process_time);
			g_game_process = process;
			g_is_initialized = true;

			return true;
		}


		NTSTATUS cleanup_game_process( )
		{ 
			 
			g_game_process = nullptr;
			g_is_initialized = false;

			clear_all_player_info();

			g_client_base = 0;
			g_client_size = 0;
			g_engine2_base = 0;
			g_engine2_size = 0;
			g_game_handle = 0;
			g_process_time.QuadPart = 0;
			return STATUS_SUCCESS;
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
		 

		bool get_cs2_window_info(HANDLE hwnd, POINT* screen_size)
		{
			BOOL is_success = FALSE;
			unsigned long long get_window_rect_fun = utils::dwm_draw::g_get_window_rect_fun;

			if (!g_user_buffer)
			{
				if (!NT_SUCCESS(utils::memory::allocate_user_memory(&g_user_buffer, 0x1000, PAGE_READWRITE, true, false)))
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
					//utils::memory::free_user_memory(utils::internal_functions::pfn_ps_get_current_process_id(), g_user_buffer, 0x1000, false);
					//g_user_buffer = nullptr;
				}
			}

			return is_success ? true : false;

		}
		bool is_initialize_game()
		{
			return g_is_initialized;
		}

		bool is_create_time()
		{
			 
			LARGE_INTEGER current_time;
			KeQuerySystemTime(&current_time);
			if ((current_time.QuadPart - g_process_time.QuadPart) < 10 * 10000000LL) {
				return false; // 未超过10秒，跳过
			}

			return true;
		}

	
		 

		HANDLE find_cs2_window()
		{
			 
			HANDLE handle{};
			if (!g_user_buffer)
			{
				if (!NT_SUCCESS(utils::memory::allocate_user_memory(&g_user_buffer, 0x1000, PAGE_READWRITE, true, false)))
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
					 RtlZeroMemory(g_user_buffer, 0X1000);
					 // utils::memory::free_user_memory(utils::internal_functions::pfn_ps_get_current_process_id(), g_user_buffer, 0x1000, false);
					  //g_user_buffer = nullptr;
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
					RtlZeroMemory(g_user_buffer, 0X1000);
					//utils::memory::free_user_memory(utils::internal_functions::pfn_ps_get_current_process_id(), g_user_buffer, 0x1000, false);
					//g_user_buffer = nullptr;
					return handle;
				}
			}

			return nullptr;
		}

		void initialize_player_data_lock()
		{
		 
				ExInitializeFastMutex(&g_player_data_lock);
				 
			 
		}

		void set_player_data(const kcsgo2struct::CPlayer* player_array, int count)
		{
			if (!player_array || count < 0 || count > MAX_PLAYER_NUM)
			{
				return;

			}

			if (count == 0)
			{
				ExAcquireFastMutex(&g_player_data_lock);
				RtlZeroMemory(g_player_array, sizeof(g_player_array));
				g_player_count = 0;
				ExReleaseFastMutex(&g_player_data_lock);
			}
			else
			{
				ExAcquireFastMutex(&g_player_data_lock);

				RtlZeroMemory(g_player_array, sizeof(g_player_array));
				memcpy(g_player_array, player_array, sizeof(kcsgo2struct::CPlayer) * count);
				g_player_count = count;
				ExReleaseFastMutex(&g_player_data_lock);
			}

			

		}

		bool get_player_data(kcsgo2struct::CPlayer* out_array,  int* out_actual_count)
		{
		 

			ExAcquireFastMutex(&g_player_data_lock);

			int copy_count = g_player_count;
			if (copy_count !=0)
			{
				memcpy(out_array, g_player_array, sizeof(kcsgo2struct::CPlayer) * copy_count);
			}
			 
			
			 
			ExReleaseFastMutex(&g_player_data_lock);

			*out_actual_count = copy_count;
			return true;  // 成功返回0
		}
		void clear_all_player_info()
		{
			ExAcquireFastMutex(&g_player_data_lock);

			RtlZeroMemory(g_player_array, sizeof(g_player_array));
			g_player_count = 0;

			ExReleaseFastMutex(&g_player_data_lock);
		}
		bool initialize_game_data()
		{
			if (!g_game_handle)
			{
				g_game_handle = find_cs2_window();

				if (!g_game_handle)
					return false;

				if (!get_cs2_window_info(g_game_handle, &g_game_size))
					return false;
			}
			
			uintptr_t local_pawn_addr = g_client_base + cs2SDK::offsets::dwLocalPlayerPawn;
			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(local_pawn_addr)))
			{
				return false;
			}

			memcpy(&kcsgo2data::g_local_pcsplayer_pawn, reinterpret_cast<void*>(local_pawn_addr), sizeof(kcsgo2data::g_local_pcsplayer_pawn));
			if (!kcsgo2data::g_local_pcsplayer_pawn)
			{
				return false;
			}

			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<PVOID> (kcsgo2data::g_local_pcsplayer_pawn)))
			{
				return false;
			}
			kcsgo2data::g_entity_list = g_client_base + cs2SDK::offsets::m_offestPlayerArray;
			if (!kcsgo2data::g_entity_list)
			{
				return false;


			}

			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<PVOID> (kcsgo2data::g_entity_list)))
			{
				return false;
			}


			int health = 0;
			if (utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_iHealth)))
			{

				memcpy(&health, reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_iHealth), sizeof(int));
			}


			// 获取本地玩家血量与队伍
			if (health > 0 && health <= 100)
			{
				void* team_addr = reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_offsettema);
				if (utils::internal_functions::pfn_mm_is_address_valid_ex(team_addr))
				{
					memcpy(&kcsgo2data::g_local_team, team_addr, sizeof(int));
				}
			}

			// 更新 local 原点坐标
			void* origin_addr = reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_vLastSlopeCheckPos);
			if (utils::internal_functions::pfn_mm_is_address_valid_ex(origin_addr))
			{
				memcpy(&kcsgo2data::g_local_origin, origin_addr, sizeof(Vector3));
			}

			// 采集玩家数据临时数组
			kcsgo2struct::CPlayer temp_array[60]{};
			int temp_count = 0;

			for (size_t i = 0; i < 60; ++i)
			{
				uintptr_t player_pawn = 0;
				void* pawn_ptr = reinterpret_cast<void*>(kcsgo2data::g_entity_list + i * sizeof(uintptr_t));



				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(pawn_ptr))
				{
					continue;
				}

				memcpy(&player_pawn, pawn_ptr, sizeof(uintptr_t));

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

			   
				  
					kcsgo2struct::CPlayer& player = temp_array[temp_count];
					player.pCSPlayerPawn = player_pawn;
					player.team = team;
					player.health = health;
					
					player.is_local_player = false;

					// 读取 origin
					void* tagorigin_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_vLastSlopeCheckPos);
					if (!utils::internal_functions::pfn_mm_is_address_valid_ex(tagorigin_addr))
					{
						continue;
					}
					memcpy(&player.origin, tagorigin_addr, sizeof(Vector3));
					player.head = { player.origin.x, player.origin.y, player.origin.z + 70.f };

					// 读取 scene node
					void* scene_node_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_pGameSceneNode);
					if (!utils::internal_functions::pfn_mm_is_address_valid_ex(scene_node_addr))
					{
						continue;
					}
					memcpy(&player.gameSceneNode, scene_node_addr, sizeof(uintptr_t));


					// 显示玩家骨骼
					if (KMenuConfig::ShowBone.enabled)
					{
						void* bone_array_addr_ptr = reinterpret_cast<void*>(player.gameSceneNode + cs2SDK::offsets::m_OoffsetBone);
						if (utils::internal_functions::pfn_mm_is_address_valid_ex(bone_array_addr_ptr))
						{
							memcpy(&player.boneArray, bone_array_addr_ptr, sizeof(uint64_t));
							player.ReadBones( &game::kcsgo2data::g_view_matrix, game::kcsgo2::g_game_size);
						}
						 
						
					}


					// 读取 view matrix（每次都读，不太必要）
					void* view_matrix_addr = reinterpret_cast<void*>(g_client_base + cs2SDK::offsets::dwViewMatrix);
					if (utils::internal_functions::pfn_mm_is_address_valid_ex(view_matrix_addr))
					{
						memcpy(&kcsgo2data::g_view_matrix, view_matrix_addr, sizeof(matrix4x4_t));
					}

					 

					temp_count++;
			}

			// 提交玩家数据
			set_player_data(temp_array, temp_count);

			return true;

		}


		bool initialize_game_data2()
		{
			if (!g_game_handle) 
			{
				g_game_handle = find_cs2_window();

				if (!g_game_handle)
					return false;

				if (!get_cs2_window_info(g_game_handle, &g_game_size))
					return false;
			}

			if (!g_game_process)
			{
				return false;
			}
			KAPC_STATE apc_state{};
			 

			utils::internal_functions::pfn_ke_stack_attach_process(g_game_process, &apc_state);

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

			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn)))
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

			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(kcsgo2data::g_entity_list)))
			{
				utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
				return false;
			}
			int health = 0;
			if (utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_iHealth)))
			{
				memcpy(&health, reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_iHealth), sizeof(int));
			}
			// 获取本地玩家血量与队伍
			if (health > 0 && health <= 100)
			{
				void* team_addr = reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_offsettema);
				if (utils::internal_functions::pfn_mm_is_address_valid_ex(team_addr))
				{
					memcpy(&kcsgo2data::g_local_team, team_addr, sizeof(int));
				}
			}

			// 更新 local 原点坐标
			void* origin_addr = reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_vLastSlopeCheckPos);
			if (utils::internal_functions::pfn_mm_is_address_valid_ex(origin_addr))
			{
				memcpy(&kcsgo2data::g_local_origin, origin_addr, sizeof(Vector3));
			}

			// 采集玩家数据临时数组
			kcsgo2struct::CPlayer temp_array[60]{};
			int temp_count = 0;

			for (size_t i = 0; i < 60; ++i)
			{
				uintptr_t player_pawn = 0;
				void* pawn_ptr = reinterpret_cast<void*>(kcsgo2data::g_entity_list + i * sizeof(uintptr_t));
				if (utils::internal_functions::pfn_mm_is_address_valid_ex(pawn_ptr))
				{
					memcpy(&player_pawn, pawn_ptr, sizeof(uintptr_t));
				}

				if (!player_pawn || player_pawn == kcsgo2data::g_local_pcsplayer_pawn)
					continue;

				int team = 0;
				void* team_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_offsettema);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(team_addr))
					continue;
				memcpy(&team, team_addr, sizeof(int));
				if (team == kcsgo2data::g_local_team || team < 2)
					continue;

				void* health_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_iHealth);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(health_addr))
					continue;
				memcpy(&health, health_addr, sizeof(int));
				if (health <= 0 || health > 100)
					continue;

			 



				kcsgo2struct::CPlayer& player = temp_array[temp_count];
				player.pCSPlayerPawn = player_pawn;
				player.team = team;
				player.health = health;
				 
				player.is_local_player = false;

				// 读取 origin
				void* tagorigin_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_vLastSlopeCheckPos);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(tagorigin_addr))
					continue;
				memcpy(&player.origin, tagorigin_addr, sizeof(Vector3));
				player.head = { player.origin.x, player.origin.y, player.origin.z + 70.f };

				// 读取 scene node
				void* scene_node_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_pGameSceneNode);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(scene_node_addr))
					continue;
				memcpy(&player.gameSceneNode, scene_node_addr, sizeof(uintptr_t));


				// 显示玩家骨骼
				if (KMenuConfig::ShowBone.enabled)
				{
					void* bone_array_addr_ptr = reinterpret_cast<void*>(player.gameSceneNode + cs2SDK::offsets::m_OoffsetBone);
					if (utils::internal_functions::pfn_mm_is_address_valid_ex(bone_array_addr_ptr))
					{
						memcpy(&player.boneArray, bone_array_addr_ptr, sizeof(uint64_t));
						player.ReadBones(&game::kcsgo2data::g_view_matrix, game::kcsgo2::g_game_size);
					}


				}


				// 读取 view matrix（每次都读，不太必要）
				void* view_matrix_addr = reinterpret_cast<void*>(g_client_base + cs2SDK::offsets::dwViewMatrix);
				if (utils::internal_functions::pfn_mm_is_address_valid_ex(view_matrix_addr))
				{
					memcpy(&kcsgo2data::g_view_matrix, view_matrix_addr, sizeof(matrix4x4_t));
				}



				temp_count++;
			}
			utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			// 提交玩家数据
			set_player_data(temp_array, temp_count);

			return true;

		}
	 

		bool initialize_game_data3()
		{
			if (!g_game_handle)
			{
				g_game_handle = find_cs2_window();

				if (!g_game_handle)
					return false;

				if (!get_cs2_window_info(g_game_handle, &g_game_size))
					return false;
			}

			if (!g_game_process)
			{
				return false;
			}
			unsigned long long client_base = 0;
			unsigned long long client_size = 0;
			unsigned long long engine2_base = 0;
			unsigned long long engine2_size = 0;

			NTSTATUS status1 = utils::module_info::get_process_module_info(g_game_process, L"client.dll", &client_base, &client_size);
			NTSTATUS status2 = utils::module_info::get_process_module_info(g_game_process, L"engine2.dll", &engine2_base, &engine2_size);


			if (!NT_SUCCESS(status1) || !NT_SUCCESS(status2)) {

				return false;
			}

			if (client_base == 0 || client_size == 0)
			{
				return false;
			}

			if (engine2_base == 0 || engine2_size == 0)
			{
				return false;
			}
			 
			g_client_base = client_base;
			g_client_size = client_size;
			g_engine2_base = engine2_base;
			g_engine2_size = engine2_size;
			KAPC_STATE apc_state{};


			utils::internal_functions::pfn_ke_stack_attach_process(g_game_process, &apc_state);

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

			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn)))
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

			if (!utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(kcsgo2data::g_entity_list)))
			{
				utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
				return false;
			}
			int health = 0;
			if (utils::internal_functions::pfn_mm_is_address_valid_ex(reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_iHealth)))
			{
				memcpy(&health, reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_iHealth), sizeof(int));
			}
			// 获取本地玩家血量与队伍
			if (health > 0 && health <= 100)
			{
				void* team_addr = reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_offsettema);
				if (utils::internal_functions::pfn_mm_is_address_valid_ex(team_addr))
				{
					memcpy(&kcsgo2data::g_local_team, team_addr, sizeof(int));
				}
			}

			// 更新 local 原点坐标
			void* origin_addr = reinterpret_cast<void*>(kcsgo2data::g_local_pcsplayer_pawn + cs2SDK::offsets::m_vLastSlopeCheckPos);
			if (utils::internal_functions::pfn_mm_is_address_valid_ex(origin_addr))
			{
				memcpy(&kcsgo2data::g_local_origin, origin_addr, sizeof(Vector3));
			}

			// 采集玩家数据临时数组
			kcsgo2struct::CPlayer temp_array[60]{};
			int temp_count = 0;

			for (size_t i = 0; i < 60; ++i)
			{
				uintptr_t player_pawn = 0;
				void* pawn_ptr = reinterpret_cast<void*>(kcsgo2data::g_entity_list + i * sizeof(uintptr_t));
				if (utils::internal_functions::pfn_mm_is_address_valid_ex(pawn_ptr))
				{
					memcpy(&player_pawn, pawn_ptr, sizeof(uintptr_t));
				}

				if (!player_pawn || player_pawn == kcsgo2data::g_local_pcsplayer_pawn)
					continue;

				int team = 0;
				void* team_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_offsettema);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(team_addr))
					continue;
				memcpy(&team, team_addr, sizeof(int));
				if (team == kcsgo2data::g_local_team || team < 2)
					continue;

				void* health_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_iHealth);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(health_addr))
					continue;
				memcpy(&health, health_addr, sizeof(int));
				if (health <= 0 || health > 100)
					continue;
 



				kcsgo2struct::CPlayer& player = temp_array[temp_count];
				player.pCSPlayerPawn = player_pawn;
				player.team = team;
				player.health = health;
			 
				player.is_local_player = false;

				// 读取 origin
				void* tagorigin_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_vLastSlopeCheckPos);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(tagorigin_addr))
					continue;
				memcpy(&player.origin, tagorigin_addr, sizeof(Vector3));
				player.head = { player.origin.x, player.origin.y, player.origin.z + 70.f };

				// 读取 scene node
				void* scene_node_addr = reinterpret_cast<void*>(player_pawn + cs2SDK::offsets::m_pGameSceneNode);
				if (!utils::internal_functions::pfn_mm_is_address_valid_ex(scene_node_addr))
					continue;
				memcpy(&player.gameSceneNode, scene_node_addr, sizeof(uintptr_t));


				// 显示玩家骨骼
				if (KMenuConfig::ShowBone.enabled)
				{
					void* bone_array_addr_ptr = reinterpret_cast<void*>(player.gameSceneNode + cs2SDK::offsets::m_OoffsetBone);
					if (utils::internal_functions::pfn_mm_is_address_valid_ex(bone_array_addr_ptr))
					{
						memcpy(&player.boneArray, bone_array_addr_ptr, sizeof(uint64_t));
						player.ReadBones(&game::kcsgo2data::g_view_matrix, game::kcsgo2::g_game_size);
					}


				}


				// 读取 view matrix（每次都读，不太必要）
				void* view_matrix_addr = reinterpret_cast<void*>(g_client_base + cs2SDK::offsets::dwViewMatrix);
				if (utils::internal_functions::pfn_mm_is_address_valid_ex(view_matrix_addr))
				{
					memcpy(&kcsgo2data::g_view_matrix, view_matrix_addr, sizeof(matrix4x4_t));
				}



				temp_count++;
			}
			utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);
			// 提交玩家数据
			set_player_data(temp_array, temp_count);

			return true;
		}

	}
}




