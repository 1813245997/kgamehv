#pragma once
#include "kcsgo2_CPlayer.h"

#define  MAX_PLAYER_NUM  60 

namespace game
{
	namespace kcsgo2
	{




		// 最大玩家数量


	// 全局游戏状态变量
		extern PEPROCESS g_game_process;
		extern  HANDLE  g_game_pid;
		extern HANDLE g_game_handle;
		extern bool g_is_initialized;
		extern POINT g_game_size;

		// 玩家数据
		extern kcsgo2struct::CPlayer g_player_array[MAX_PLAYER_NUM];
		extern int g_player_count;

		// 初始化与清理
		bool initialize_game_process(_In_ PEPROCESS process);
		bool initialize_game_data();
		NTSTATUS cleanup_game_process();

		// 状态判断
		bool is_game_process(_In_ PEPROCESS process);
		bool is_initialize_game();

		// 窗口相关
		HANDLE find_cs2_window();
		bool get_cs2_window_info(HANDLE hwnd, POINT* screen_size);

		// 玩家数据访问
		bool get_player_data(kcsgo2struct::CPlayer* out_array, int* out_actual_count);
		void set_player_data(const kcsgo2struct::CPlayer* player_array, int count);
		void reset_player_data();
		void initialize_player_data_lock();

   }
}