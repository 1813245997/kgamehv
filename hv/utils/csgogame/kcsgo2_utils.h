#pragma once
#include "kcsgo2_CPlayer.h"
#include "../memory_utils.h"
#define  MAX_PLAYER_NUM  60 

namespace game
{
	namespace kcsgo2
	{

		  
		 
		 
		 
		void initialize_player_data_lock();

		class CC4 {
		public:
			uintptr_t get_planted();
			uintptr_t get_node();
			Vector3 get_origin();
		public:
			
		};
		class  CGame
		{
		public:
			//游戏强检测几乎用不了 
			bool init(_In_ PEPROCESS process);
			//强检测用
			bool init2(_In_ PEPROCESS process);
			bool is_init2();
			void clear();
			void loop();
			void loop2(PVOID user_buffer);

			bool CGame::get_player_data(utils::kvector<kcsgo2struct::CPlayer>* out_list);
		//	bool world_to_screen(const _In_ Vector3* v, _In_opt_ Vector3* out);
			Vector3 world_to_screen(Vector3* v);//新的
			bool is_in_bounds(const Vector3& pos );

			template<typename T>
			T read(uintptr_t address)
			{
				T buffer{};
				KAPC_STATE apc_state{};

				if (!m_game_process)
				{
					return buffer;
				}

				if (utils::internal_functions::pfn_ps_get_process_exit_status(m_game_process) != STATUS_PENDING)
				{

					return buffer;
				}
				utils::internal_functions::pfn_ke_stack_attach_process(m_game_process, &apc_state);
				buffer = utils::memory::read<T>(address);
				utils::internal_functions::pfn_ke_unstack_detach_process(&apc_state);

				return buffer;
				/*	T buffer{};
					ULONG size = sizeof(T);
					NTSTATUS status = utils::memory::read_virtual_memory(m_game_cr3, reinterpret_cast<void*> (address), &buffer, &size);
					if (!NT_SUCCESS(status))
					{

						return T{};
					}
					return buffer;*/
			}

			bool read_raw(uintptr_t address, void* buffer, size_t size);
			

		private:
			HANDLE find_cs2_window(PVOID user_buffer);
			bool get_cs2_window_info(HANDLE hwnd, POINT* screen_size, PVOID user_buffer);
			void set_player_data(utils::kvector<kcsgo2struct::CPlayer>& list);
			void reset_player_data();

		public:

			PEPROCESS m_game_process{};
			unsigned long long m_game_cr3{};
			HANDLE  m_game_pid{};
			HANDLE  m_game_handle{};
			POINT  m_game_size{};
			RECT   m_game_bounds{};
		 
			ProcessModule m_base_client{};
			ProcessModule m_base_engine{};
			uintptr_t m_buildNumber{};
			bool m_is_initialized{};
			bool isC4Planted;
			int localTeam;
			CC4 c4;
			Vector3 c4Origin;
			Vector3 localOrigin;
			utils::kvector< kcsgo2struct::CPlayer> players = {};
			FAST_MUTEX m_player_data_lock;
			FAST_MUTEX m_g_data_lock;
			bool m_cheat_update{};

		private:
			uintptr_t entity_list;
			uintptr_t localPlayer;
			uint32_t localPlayerPawn;
			uintptr_t localpCSPlayerPawn;
			uintptr_t localList_entry2;

			view_matrix_t  view_matrix;
			//matrix4x4_t  view_matrix;
		};
		inline CGame *g_game;

   }
}