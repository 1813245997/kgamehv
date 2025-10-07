#pragma once
enum class GameType
{
	Unknown,
	DeltaForce,
};

namespace game
{
	namespace game_cheat
	{
		extern GameType g_game_type;

		NTSTATUS initialize_game_cheat( GameType game_type );
		
		NTSTATUS game_draw_every_thing( int width, int height, void* data );

		NTSTATUS initialize_cheat_game( PUNICODE_STRING module_name );

		NTSTATUS clean_up_cheat_game(PEPROCESS process);

        GameType get_game_type();

        void  initialize_config();

	}
}