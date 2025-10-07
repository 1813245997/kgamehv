#include "../utils/global_defs.h"
#include "delta_force.h"

namespace game
{
	namespace delta_force
	{
	     


		NTSTATUS initialize_cheat_game( PUNICODE_STRING module_name )
		{
			return STATUS_SUCCESS;
		}
		
		NTSTATUS clean_up_cheat_game(PEPROCESS process)
		{
			return STATUS_SUCCESS;
		}

		NTSTATUS draw_every_thing( int width, int height, void* data )
		{
			initialize_menu(width, height, data);
			return STATUS_SUCCESS;
		}

		void initialize_config()
	  {
		
			    game::cheat_config::g_cheat_features.show_menu = true;
				// Initialize F1 - Player Skeleton
				game::cheat_config::g_cheat_features.player_skeleton.enabled = true;
				game::cheat_config::g_cheat_features.player_skeleton.color[0] = 255.0f;  // Red
				game::cheat_config::g_cheat_features.player_skeleton.color[1] = 0.0f;  // Green
				game::cheat_config::g_cheat_features.player_skeleton.color[2] = 0.0f;  // Blue
				game::cheat_config::g_cheat_features.player_skeleton.rainbow = false;
				game::cheat_config::g_cheat_features.player_skeleton.rainbowSpeed = 0.6f;
	

				// Initialize F2 - ESP				 
				game::cheat_config::g_cheat_features.esp.enabled = true;
				game::cheat_config::g_cheat_features.esp.color[0] = 0.0f;  // Red
				game::cheat_config::g_cheat_features.esp.color[1] = 0.0f;  // Green
				game::cheat_config::g_cheat_features.esp.color[2] = 1.0f;  // Blue
				game::cheat_config::g_cheat_features.esp.rainbow = false;
				game::cheat_config::g_cheat_features.esp.rainbowSpeed = 0.6f;

				// Initialize F3 - Aimbot
				game::cheat_config::g_cheat_features.aimbot.enabled = false;
				game::cheat_config::g_cheat_features.aimbot.color[0] = 0.0f;  // Red
				game::cheat_config::g_cheat_features.aimbot.color[1] = 1.0f;  // Green
				game::cheat_config::g_cheat_features.aimbot.color[2] = 0.0f;  // Blue
				game::cheat_config::g_cheat_features.aimbot.rainbow = false;
				game::cheat_config::g_cheat_features.aimbot.rainbowSpeed = 0.6f;
	


	  }

		NTSTATUS initialize_menu( int width, int height, void* data )
		{
			// Check if menu should be displayed
			if (!game::cheat_config::is_menu_visible())
			{
				return STATUS_SUCCESS;  // Don't draw menu if show_menu is false
			}

			ByteRender rend;
			rend.Setup(width, height, data);
			
			// Menu configuration
			int menu_width = 300;
			int menu_height = 200;
			int menu_x = width - menu_width - 20;
			int menu_y = 20;
			
			// Draw main menu box (border)
			rend.Rectangle(
				menu_x, menu_y, menu_width, menu_height,
				PM_XRGB(0, 255, 0),  // Green border
				2  // Border thickness
			);
			
			// Draw title
			rend.String(
				utils::render::g_Font,
				{ static_cast<float>(menu_x + 10), static_cast<float>(menu_y + 10) },
				L"DeltaForce Cheat",
				PM_XRGB(255, 255, 0)  // Yellow title
			);
			
			// Draw menu control instruction
			rend.String(
				utils::render::g_Font,
				{ static_cast<float>(menu_x + 10), static_cast<float>(menu_y + 35) },
				L"HOME - Toggle Menu",
				PM_XRGB(0, 255, 255)  // Cyan instruction
			);
			
			// Menu items with F-keys and status
			int item_y = menu_y + 60;
			int line_height = 25;
			
			// F1 - Player Skeleton
			rend.String(
				utils::render::g_Font,
				{ static_cast<float>(menu_x + 15), static_cast<float>(item_y) },
				L"F1 - Player Skeleton",
				PM_XRGB(255, 255, 255)  // White text
			);
			
			// Display status based on configuration
			if (game::cheat_config::is_player_skeleton_enabled())
			{
				rend.String(
					utils::render::g_Font,
					{ static_cast<float>(menu_x + 240), static_cast<float>(item_y) },
					L"[ON]",
					PM_XRGB(0, 255, 0)  // Green ON
				);
			}
			else
			{
				rend.String(
					utils::render::g_Font,
					{ static_cast<float>(menu_x + 240), static_cast<float>(item_y) },
					L"[OFF]",
					PM_XRGB(255, 0, 0)  // Red OFF
				);
			}
			

			
			// F2 - ESP
			item_y += line_height;
			rend.String(
				utils::render::g_Font,
				{ static_cast<float>(menu_x + 15), static_cast<float>(item_y) },
				L"F2 - ESP",
				PM_XRGB(255, 255, 255)  // White text
			);
			
			// Display status based on configuration
			if (game::cheat_config::is_esp_enabled())
			{
				rend.String(
					utils::render::g_Font,
					{ static_cast<float>(menu_x + 240), static_cast<float>(item_y) },
					L"[ON]",
					PM_XRGB(0, 255, 0)  // Green ON
				);
			}
			else
			{
				rend.String(
					utils::render::g_Font,
					{ static_cast<float>(menu_x + 240), static_cast<float>(item_y) },
					L"[OFF]",
					PM_XRGB(255, 0, 0)  // Red OFF
				);
			}

			// F3 - Aimbot
			item_y += line_height;
			rend.String(
				utils::render::g_Font,
				{ static_cast<float>(menu_x + 15), static_cast<float>(item_y) },
				L"F3 - Aimbot",
				PM_XRGB(255, 255, 255)  // White text
			);
			
			// Display status based on configuration
			if (game::cheat_config::is_aimbot_enabled())
			{
				rend.String(
					utils::render::g_Font,
					{ static_cast<float>(menu_x + 240), static_cast<float>(item_y) },
					L"[ON]",
					PM_XRGB(0, 255, 0)  // Green ON
				);
			}
			else
			{
				rend.String(
					utils::render::g_Font,
					{ static_cast<float>(menu_x + 240), static_cast<float>(item_y) },
					L"[OFF]",
					PM_XRGB(255, 0, 0)  // Red OFF
				);
			}
			
			// Draw separator line
			rend.Line(
				{ static_cast<float>(menu_x + 10), static_cast<float>(menu_y + menu_height - 30) },
				{ static_cast<float>(menu_x + menu_width - 10), static_cast<float>(menu_y + menu_height - 30) },
				PM_XRGB(0, 255, 0),  // Green line
				1
			);
			
			// Draw instructions
			rend.String(
				utils::render::g_Font,
				{ static_cast<float>(menu_x + 15), static_cast<float>(menu_y + menu_height - 15) },
				L"Press F-Key to toggle",
				PM_XRGB(0, 255, 255)  // Cyan instructions
			);
			
			return STATUS_SUCCESS;
		}

	}
}