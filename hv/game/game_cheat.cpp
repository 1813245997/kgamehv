#include "../utils/global_defs.h"
#include "game_cheat.h"


namespace game
{
	namespace game_cheat
	{
        GameType g_game_type{};
        
		NTSTATUS initialize_game_cheat( GameType game_type )
		{
            g_game_type = game_type;
            initialize_config();
			return STATUS_SUCCESS;
		}


        NTSTATUS game_draw_every_thing(int width, int height, void* data)
        {
            switch (g_game_type)
            {
                case GameType::DeltaForce:
                {
                    return game::delta_force::draw_every_thing(width, height, data);
                    break;
                }
        
                default:
                {
                    break;
                }
            }
        
            return STATUS_SUCCESS;
        }

        
        NTSTATUS  initialize_cheat_game( PUNICODE_STRING module_name )
        {
            switch (g_game_type)
            {
                case GameType::DeltaForce:
                {
                    return game::delta_force::initialize_cheat_game(module_name);
                }

                default:
                {
                    break;
                }
            }


            return STATUS_SUCCESS;
        }

        NTSTATUS clean_up_cheat_game(PEPROCESS process)
        {
            switch (g_game_type)
            {
                case GameType::DeltaForce:
                {
                    return game::delta_force::clean_up_cheat_game(process);
                }

                default:
                {
                    break;
                }
            }   

            return STATUS_SUCCESS;
        }


        GameType get_game_type()
        {
          
            return g_game_type;
        }


        void  initialize_config()
        {
            switch (g_game_type)
            {
                case GameType::DeltaForce:
                {
                    return game::delta_force::initialize_config();
                }
                default:
                {
                    break;
                }
            }

        }

	}
}