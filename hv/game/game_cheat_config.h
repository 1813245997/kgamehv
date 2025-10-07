#pragma once

namespace game
{
	namespace cheat_config
	{
		// Color structure for RGB values
		struct Color_
		{
			float color[3]{ 1.0f, 1.0f, 1.0f };  // RGB values (0.0f - 1.0f)
			bool rainbow{ false };                // Enable rainbow effect
			float rainbowSpeed{ 0.6f };           // Rainbow animation speed
		};

		// Color with toggle functionality
		struct ColorToggle : public Color_
		{
			bool enabled{ false };                // Feature enabled/disabled
		};

		// Cheat feature configurations
		struct CheatFeatures
		{
			// Menu display control
			bool show_menu{ false };
			
			// F1 - Player Skeleton
			ColorToggle player_skeleton;
			
			// F2 - ESP
			ColorToggle esp;

			// F3 - Aimbot
			ColorToggle aimbot;
			

		};

		// Global cheat configuration instance
		extern CheatFeatures g_cheat_features;

 

		// Toggle functions for each feature
		void toggle_player_skeleton();
		void toggle_esp();
		void toggle_aimbot();


		// Menu control functions
		void toggle_menu_display();
		bool is_menu_visible();

		// Get status functions
		bool is_player_skeleton_enabled();
		bool is_esp_enabled();
		bool is_aimbot_enabled();


		// Get color functions
		ColorToggle get_player_skeleton_config();
		ColorToggle get_esp_config();
		ColorToggle get_aimbot_config();

	}
}