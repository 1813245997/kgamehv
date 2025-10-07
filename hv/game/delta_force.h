#pragma once

namespace game
{
	namespace delta_force
	{



		NTSTATUS initialize_cheat_game( PUNICODE_STRING module_name );

		NTSTATUS clean_up_cheat_game(PEPROCESS process);

		NTSTATUS draw_every_thing( int width, int height, void* data );

		NTSTATUS initialize_menu( int width, int height, void* data );

		void initialize_config();

		
	}
}