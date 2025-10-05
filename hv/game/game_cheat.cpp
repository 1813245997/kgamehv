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
			return STATUS_SUCCESS;
		}


        NTSTATUS game_draw_every_thing(int width, int height, void* data)
        {
		
            ByteRender rend;
			rend.Setup(width, height, data);
			int margin = 30;
			rend.String(
				utils::render::g_Font,
				{ static_cast<float>(width - 150), static_cast<float>(margin) },  
				L"DeltaForce",
				PM_XRGB(255, 0, 0)
			);
            switch (g_game_type)
            {
                case GameType::DeltaForce:

                
                    break;
                default:
                    break;
            }
            return STATUS_SUCCESS;
        }

        GameType get_game_type()
        {
            return g_game_type;
        }
	}
}