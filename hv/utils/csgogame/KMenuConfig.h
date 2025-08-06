#pragma once

namespace config
{
	struct RGB {
		int r;
		int g;
		int b;

		// Conversion function from RGB to COLORREF
		/*operator COLORREF() const {
			return RGB(r, g, b);
		}*/
	};
	struct Color_
	{
		float color[3]{ 1.0f, 1.0f, 1.0f }; // 颜色值（RGB）
		bool rainbow{ false };             // 是否启用彩虹模式
		float rainbowSpeed{ 0.6f };        // 彩虹颜色变化速度
	};

	struct ColorToggle : public Color_
	{
		bool enabled{ false };             // 是否启用该功能
	};


	inline ColorToggle show_box_esp;           // 显示玩家2D方框
	inline ColorToggle show_skeleton_esp;      // 显示玩家骨骼
	inline ColorToggle show_team_esp;          // 显示自己团队的透视
	inline ColorToggle ShowDistance;           // 显示玩家与本地玩家的距离
	inline ColorToggle ShowName;               // 显示玩家名称
	inline ColorToggle ShowXianshiWuQi;        // 显示武器的配置
	inline ColorToggle ShowC4ESP;              // 显示C4相关的信息（新添加的功能）
	inline ColorToggle show_head_tracker;		   // 显示头部跟踪器
	inline ColorToggle show_health_bar;		   //显示血量
	inline ColorToggle show_extra_flags; //显示附加标志


	inline RGB esp_box_color_team = { 75, 175, 75 };
	inline RGB esp_box_color_enemy = { 175, 75, 75 };
	inline RGB esp_skeleton_color_team = { 75, 175, 75 };
	inline RGB esp_skeleton_color_enemy = { 175, 75, 75 };


	inline bool ShowMenu = true;		   // 是否显示菜单界面
	inline bool Showarmor = true;		   // 显示护甲
	inline bool ShowEyeRay = true;         // 是否显示玩家的视线方向（射线）

 
 	inline float render_distance = -1.f;

	inline int flag_render_distance = 200;
	//snake_case 命名风格
	void initialize_menu();

	void initialize_visual_config_once();
}

