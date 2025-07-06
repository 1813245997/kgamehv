#pragma once
#include "../dx_draw/math.h"
namespace game
{

	namespace kcsgo2struct
	{

		 
		struct CPlayer
		{

			LIST_ENTRY list_entry;  
			uintptr_t entity;
			uintptr_t pCSPlayerPawn;
			uintptr_t gameSceneNode;
			uintptr_t boneArray;
			Vector3 origin;
			Vector3 head;
			int team;
			int health;
			float flashAlpha;
			int bIsPlayerExists;
			bool is_local_player;

		};
		 
		
	 

	}

}