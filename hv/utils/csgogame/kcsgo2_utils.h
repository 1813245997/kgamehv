#pragma once
#include "kcsgo2_CPlayer.h"

#define  MAX_PLAYER_NUM  60 

namespace game
{
	namespace kcsgo2
	{




		extern PEPROCESS g_game_process;
		extern unsigned long long g_client_base;
		extern unsigned long long g_client_size;
		extern unsigned long long g_engine2_base;
		extern unsigned long long g_engine2_size;
		extern bool g_is_initialized;
		extern HANDLE  g_game_handle;
		extern  POINT  g_game_size;
		extern kcsgo2struct::CPlayer g_player_array[60] ;
		extern int g_player_count  ;
		
		extern   LARGE_INTEGER g_process_time;
		bool initialize_game_process(_In_ PEPROCESS process);
		bool initialize_game_process2();
	 
		 
		bool is_game_process(_In_ PEPROCESS process);


		bool is_create_time();

		bool is_initialize_game();

		bool initialize_game_data();
		bool initialize_game_data2();

		bool get_cs2_window_info(HANDLE hwnd, POINT* screen_size);

		

		NTSTATUS cleanup_game_process();

		 

		HANDLE find_cs2_window();

		bool get_player_data(kcsgo2struct::CPlayer* out_array, int max_count, int* out_actual_count);

		void initialize_player_data_lock();
		void clear_all_player_info();
		void set_player_data(const kcsgo2struct::CPlayer* player_array, int count);

   }
}