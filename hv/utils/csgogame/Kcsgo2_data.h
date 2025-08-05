#pragma once
#include "../dx_draw/math.h"
namespace game
{
	 
	namespace kcsgo2data
	{
	 

		// 本地玩家 Pawn 指针
		extern uintptr_t g_local_pcsplayer_pawn;

		// 实体列表基址
		extern uintptr_t g_entity_list;

		 
		
		//本地玩家坐标
		extern  Vector3  g_local_origin;

		 extern matrix4x4_t g_view_matrix;
		 
		
	 }

}