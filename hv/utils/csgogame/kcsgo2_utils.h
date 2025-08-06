#pragma once
#include "kcsgo2_CPlayer.h"

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
			bool init(_In_ PEPROCESS process);
			void clear();
			void loop();

			bool CGame::get_player_data(utils::kvector<kcsgo2struct::CPlayer>* out_list);
		//	bool world_to_screen(const _In_ Vector3* v, _In_opt_ Vector3* out);
			Vector3 world_to_screen(Vector3* v);//ÐÂµÄ
			bool is_in_bounds(const Vector3& pos );
		private:
			HANDLE find_cs2_window();
			bool get_cs2_window_info(HANDLE hwnd, POINT* screen_size);
			void set_player_data(utils::kvector<kcsgo2struct::CPlayer>& list);
			void reset_player_data();

		public:

			PEPROCESS m_game_process{};
			HANDLE  m_game_pid{};
			HANDLE  m_game_handle{};
			POINT  m_game_size{};
			RECT   m_game_bounds{};
			PVOID m_user_buffer{};
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