#include "../global_defs.h"
#include "KMenuConfig.h"

void KMenuConfig::  initialize_menu()
{ 
}

void KMenuConfig::initialize_visual_config_once()
{
	 

	KMenuConfig::ShowBoxEsp2d.enabled = true;
	KMenuConfig::ShowBone.enabled = true;
	KMenuConfig::ShowDistance.enabled = true;
	KMenuConfig::ShowName.enabled = true;
	KMenuConfig::ShowXianshiWuQi.enabled = true;
	KMenuConfig::ShowC4ESP.enabled = true;
	KMenuConfig::Showheadtracker.enabled = true;
	KMenuConfig::ShowHealthBar.enabled = true;

	// 距离颜色 (青色)
	KMenuConfig::ShowDistance.color[0] = 0.0f;
	KMenuConfig::ShowDistance.color[1] = 255.0f;
	KMenuConfig::ShowDistance.color[2] = 255.0f;

	// 方框颜色 (绿色)
	KMenuConfig::ShowBoxEsp2d.color[0] = 0.0f;
	KMenuConfig::ShowBoxEsp2d.color[1] = 255.0f;
	KMenuConfig::ShowBoxEsp2d.color[2] = 0.0f;

	// 骨骼颜色 (黄色)
	KMenuConfig::ShowBone.color[0] = 0.0f;
	KMenuConfig::ShowBone.color[1] = 255.0f;
	KMenuConfig::ShowBone.color[2] = 255.0f;

	// 名字颜色 (红色)
	KMenuConfig::ShowName.color[0] = 255.0f;
	KMenuConfig::ShowName.color[1] = 0.0f;
	KMenuConfig::ShowName.color[2] = 0.0f;

	// C4颜色 (深红)
	KMenuConfig::ShowC4ESP.color[0] = 175.0f;
	KMenuConfig::ShowC4ESP.color[1] = 75.0f;
	KMenuConfig::ShowC4ESP.color[2] = 75.0f;

	// 武器颜色 (红)
	KMenuConfig::ShowXianshiWuQi.color[0] = 1.0f;
	KMenuConfig::ShowXianshiWuQi.color[1] = 0.0f;
	KMenuConfig::ShowXianshiWuQi.color[2] = 0.0f;

	// 头部颜色 (红)
	KMenuConfig::Showheadtracker.color[0] = 51.0f;   // 蓝色
	KMenuConfig::Showheadtracker.color[1] = 51.0f;   // 绿色
	KMenuConfig::Showheadtracker.color[2] = 255.0f;  // 红色

	// 血量颜色 (红)
	KMenuConfig::ShowHealthBar.color[0] = 255.0f;
	KMenuConfig::ShowHealthBar.color[1] = 0.0f;
	KMenuConfig::ShowHealthBar.color[2] = 0.0f;
}
