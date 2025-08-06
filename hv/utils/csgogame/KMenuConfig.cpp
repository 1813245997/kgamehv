#include "../global_defs.h"
#include "KMenuConfig.h"

void config::  initialize_menu()
{ 
}

void config::initialize_visual_config_once()
{
	 

	config::show_box_esp.enabled = false;
	config::show_skeleton_esp.enabled = true;
	config::show_team_esp.enabled = false;
	config::ShowDistance.enabled = true;
	config::ShowName.enabled = true;
	config::ShowXianshiWuQi.enabled = true;
	config::ShowC4ESP.enabled = true;
	config::show_head_tracker.enabled = true;
	config::show_health_bar.enabled = true;

	config::show_extra_flags.enabled =false;

	// 距离颜色 (青色)
	config::ShowDistance.color[0] = 0.0f;
	config::ShowDistance.color[1] = 255.0f;
	config::ShowDistance.color[2] = 255.0f;

	// 方框颜色 (绿色)
	config::show_box_esp.color[0] = 0.0f;
	config::show_box_esp.color[1] = 255.0f;
	config::show_box_esp.color[2] = 0.0f;

	// 骨骼颜色 (黄色)
	config::show_skeleton_esp.color[0] = 255.0f;
	config::show_skeleton_esp.color[1] = 255.0f;
	config::show_skeleton_esp.color[2] = 0.0f;
	

	// 名字颜色 (白色)
	config::ShowName.color[0] = 255.0f;
	config::ShowName.color[1] = 255.0f;
	config::ShowName.color[2] = 255.0f;

	// C4颜色 (深红)
	config::ShowC4ESP.color[0] = 175.0f;
	config::ShowC4ESP.color[1] = 75.0f;
	config::ShowC4ESP.color[2] = 75.0f;

	// 武器颜色 (红)
	config::ShowXianshiWuQi.color[0] = 1.0f;
	config::ShowXianshiWuQi.color[1] = 0.0f;
	config::ShowXianshiWuQi.color[2] = 0.0f;

	// 头部颜色 (红)
	config::show_head_tracker.color[0] = 255.0f;  // 红色
	config::show_head_tracker.color[1] = 51.0f;   // 绿色
	config::show_head_tracker.color[2] = 51.0f;   // 蓝色

	// 血量颜色 (红)
	config::show_health_bar.color[0] = 255.0f;
	config::show_health_bar.color[1] = 0.0f;
	config::show_health_bar.color[2] = 0.0f;

	config::show_extra_flags.color[0] =175.0f;
	config::show_extra_flags.color[1] = 75.0f;
	config::show_extra_flags.color[2] = 75.0f;
}
