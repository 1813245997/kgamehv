#pragma once
#include "kcsgo2_CPlayer.h"
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
		extern  POINT  g_game_size;
		extern kcsgo2struct::CPlayer g_player_array[60] ;
		extern int g_player_count  ;
		extern HANDLE  g_game_handle ;
 
		bool initialize_game_process(OUT PEPROCESS *outprocess, OUT LARGE_INTEGER* process_create_time);

		bool is_game_process(_In_ PEPROCESS process);

		bool is_game_module(_In_ PEPROCESS process, _In_ PWCHAR module_name);

		bool is_game_process_initialized();

		bool initialize_game_data();

		bool get_cs2_window_info(HANDLE hwnd, POINT* screen_size);

		NTSTATUS cleanup_game_process(_In_ PEPROCESS process);

		NTSTATUS hook_get_cosg_hp(_In_ PEPROCESS process);

		HANDLE find_cs2_window();

	


   }
}