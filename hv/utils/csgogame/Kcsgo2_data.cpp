#include "../global_defs.h"

#include  "Kcsgo2_data.h"

namespace game
{

	namespace kcsgo2data
	{
	 

		// 本地玩家 Pawn 指针
		 uintptr_t g_local_pcsplayer_pawn{};

		// 实体列表基址
		  uintptr_t g_entity_list{};
		   
		  Vector3  g_local_origin{};

	     matrix4x4_t g_view_matrix{};

		
	
	}

}