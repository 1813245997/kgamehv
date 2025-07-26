#pragma once
#include "../dx_draw/math.h"


#define BONE_CONNECTION_COUNT 15
namespace game
{

	namespace kcsgo2struct
	{
		enum BONE_INDEX {
			BONE_HEAD,
			BONE_NECK,
			BONE_SPINE_1,
			BONE_SPINE_2,
			BONE_PELVIS,
			BONE_ARM_UPPER_L,
			BONE_ARM_LOWER_L,
			BONE_HAND_L,
			BONE_ARM_UPPER_R,
			BONE_ARM_LOWER_R,
			BONE_HAND_R,
			BONE_LEG_UPPER_L,
			BONE_LEG_LOWER_L,
			BONE_ANKLE_L,
			BONE_LEG_UPPER_R,
			BONE_LEG_LOWER_R,
			BONE_ANKLE_R,
			BONE_COUNT // 自动计算总数
		};

		struct BONE_ENTRY {
			BONE_INDEX bone_enum;
			int index; // 真实骨骼在骨骼数组中的索引
		};

		inline BONE_ENTRY g_bone_list[BONE_COUNT] = {
			{ BONE_HEAD, 6 },
			{ BONE_NECK, 5 },
			{ BONE_SPINE_1, 4 },
			{ BONE_SPINE_2, 2 },
			{ BONE_PELVIS, 0 },
			{ BONE_ARM_UPPER_L, 8 },
			{ BONE_ARM_LOWER_L, 9 },
			{ BONE_HAND_L, 10 },
			{ BONE_ARM_UPPER_R, 13 },
			{ BONE_ARM_LOWER_R, 14 },
			{ BONE_HAND_R, 15 },
			{ BONE_LEG_UPPER_L, 22 },
			{ BONE_LEG_LOWER_L, 23 },
			{ BONE_ANKLE_L, 24 },
			{ BONE_LEG_UPPER_R, 25 },
			{ BONE_LEG_LOWER_R, 26 },
			{ BONE_ANKLE_R, 27 }
		};
		 


		typedef struct _BONE_CONNECTION {
			BONE_INDEX from;
			BONE_INDEX to;
		} BONE_CONNECTION;
		static const BONE_CONNECTION g_bone_connections[BONE_CONNECTION_COUNT] = {
			{ BONE_NECK,        BONE_SPINE_1 },
			{ BONE_SPINE_1,     BONE_SPINE_2 },
			{ BONE_SPINE_2,     BONE_PELVIS },
			{ BONE_SPINE_1,     BONE_ARM_UPPER_L },
			{ BONE_ARM_UPPER_L, BONE_ARM_LOWER_L },
			{ BONE_ARM_LOWER_L, BONE_HAND_L },
			{ BONE_SPINE_1,     BONE_ARM_UPPER_R },
			{ BONE_ARM_UPPER_R, BONE_ARM_LOWER_R },
			{ BONE_ARM_LOWER_R, BONE_HAND_R },
			{ BONE_PELVIS,      BONE_LEG_UPPER_L },
			{ BONE_LEG_UPPER_L, BONE_LEG_LOWER_L },
			{ BONE_LEG_LOWER_L, BONE_ANKLE_L },
			{ BONE_PELVIS,      BONE_LEG_UPPER_R },
			{ BONE_LEG_UPPER_R, BONE_LEG_LOWER_R },
			{ BONE_LEG_LOWER_R, BONE_ANKLE_R }
		};

		typedef struct _CBONES {
			Vector3 bone_positions[BONE_COUNT];  // index 与 enum 一一对应
		} CBONES;
		class CPlayer
		{

		public:
			void ReadBones(_In_ unsigned long long directory_table_base,_In_  matrix4x4_t* view_matrix, _In_ POINT& screen_size);
			void ReadBones(_In_  matrix4x4_t* view_matrix, _In_ POINT& screen_size);
		public:
			LIST_ENTRY list_entry;  
			uintptr_t entity;
			uintptr_t pCSPlayerPawn;
			uintptr_t gameSceneNode;
			uintptr_t boneArray;
			Vector3 origin;
			Vector3 head;
			CBONES bones;
			int team;
			int health;
			float flashAlpha;
			 
			bool is_local_player;
		 
		

		};
		 
		
	 

	}

}