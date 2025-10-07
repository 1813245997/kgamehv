#include "../utils/global_defs.h"
#include "game_cheat_config.h"

namespace game
{
	namespace cheat_config
	{
		// Global cheat configuration instance
		CheatFeatures g_cheat_features;

	 

		// Toggle functions for each feature
		void toggle_player_skeleton()
		{
			g_cheat_features.player_skeleton.enabled = !g_cheat_features.player_skeleton.enabled;
		}

		void toggle_esp()
		{
			g_cheat_features.esp.enabled = !g_cheat_features.esp.enabled;
		}

		void toggle_aimbot()
		{
			g_cheat_features.aimbot.enabled = !g_cheat_features.aimbot.enabled;
		}



		// Menu control functions
		void toggle_menu_display()
		{
			g_cheat_features.show_menu = !g_cheat_features.show_menu;
		}

		bool is_menu_visible()
		{
			return g_cheat_features.show_menu;
		}

		// Get status functions
		bool is_player_skeleton_enabled()
		{
			return g_cheat_features.player_skeleton.enabled;
		}

		bool is_esp_enabled()
		{
			return g_cheat_features.esp.enabled;
		}

		bool is_aimbot_enabled()
		{
			return g_cheat_features.aimbot.enabled;
		}



		// Get color functions
		ColorToggle get_player_skeleton_config()
		{
			return g_cheat_features.player_skeleton;
		}

		ColorToggle get_esp_config()
		{
			return g_cheat_features.esp;
		}

		ColorToggle get_aimbot_config()
		{
			return g_cheat_features.aimbot;
		}


	}
}