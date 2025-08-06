#pragma once
#include "kcsgo2_CPlayer.h"

#define  MAX_PLAYER_NUM  60 

namespace game
{
	namespace kcsgo2
	{


 
		// 最大玩家数量
	

	// 全局游戏状态变量
	 

		// 玩家数据
		extern kcsgo2struct::CPlayer g_player_array[MAX_PLAYER_NUM];
		extern int g_player_count;

		// 初始化与清理
	 
		bool initialize_game_data();
		NTSTATUS cleanup_game_process();

		// 状态判断
		bool is_game_process(_In_ PEPROCESS process);
		bool is_initialize_game();

		// 窗口相关
		HANDLE find_cs2_window();
		bool get_cs2_window_info(HANDLE hwnd, POINT* screen_size);

		// 玩家数据访问
		bool get_player_data(kcsgo2struct::CPlayer* out_array);
		void set_player_data(const kcsgo2struct::CPlayer* player_array, int count);
		void reset_player_data();
		void initialize_player_data_lock();


		class  CGame
		{
		public:
			bool init(_In_ PEPROCESS process);
			void clear();
			void loop();

		public:

			PEPROCESS m_game_process{};
			HANDLE  m_game_pid{};
			HANDLE  m_game_handle{};
			POINT  m_game_size{};
			ProcessModule m_base_client{};
			ProcessModule m_base_engine{};
			uintptr_t m_buildNumber{};
			bool m_is_initialized{};
			bool isC4Planted;
			int localTeam;
			Vector3 localOrigin;
		private:
			uintptr_t entity_list;
			uintptr_t localPlayer;
			uint32_t localPlayerPawn;
			uintptr_t localpCSPlayerPawn;
			uintptr_t localList_entry2;

			matrix4x4_t  view_matrix;
		};
		inline CGame g_game;

   }
}