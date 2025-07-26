#include "../global_defs.h"
#include "kcsgo2_CPlayer.h"

void game::kcsgo2struct::CPlayer::ReadBones(_In_  matrix4x4_t* view_matrix, _In_ POINT& screen_size)
{
	for (int i = 0; i < BONE_COUNT; ++i)
	{
		const BONE_ENTRY& entry = g_bone_list[i];
		size_t bone_index = entry.index;

		Vector3 bone_position{};
		uintptr_t bone_address = boneArray + bone_index * 0x20;

		memcpy(&bone_position, (void*)bone_address, sizeof(Vector3));

		Vector3 screen_position{};
		if (world_to_screen(&bone_position, &screen_position, view_matrix, screen_size)) {
			bones.bone_positions[entry.bone_enum] = screen_position;
		}
		else {
			bones.bone_positions[entry.bone_enum] = { 0.f, 0.f, 0.f };
		}
	}
}

void game::kcsgo2struct::CPlayer::ReadBones(_In_ unsigned long long directory_table_base,  _In_  matrix4x4_t* view_matrix, _In_ POINT& screen_size)
{
	for (int i = 0; i < BONE_COUNT; ++i)
	{
		const BONE_ENTRY& entry = g_bone_list[i];
		size_t bone_index = entry.index;

		Vector3 bone_position{};
		uintptr_t bone_address = boneArray + bone_index * 0x20;

	     utils::memory::read_virtual_object<Vector3>(directory_table_base, reinterpret_cast<void*> (bone_address), bone_position);
	 

		Vector3 screen_position{};
		if (world_to_screen(&bone_position, &screen_position, view_matrix, screen_size)) {
			bones.bone_positions[entry.bone_enum] = screen_position;
		}
		else {
			bones.bone_positions[entry.bone_enum] = { 0.f, 0.f, 0.f };
		}
	}
}
