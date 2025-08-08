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

		bone_position = game::kcsgo2::g_game->read<Vector3>(bone_address);
 

		Vector3 screen_position{};
		if (world_to_screen(&bone_position, &screen_position, view_matrix, screen_size)) {
			bones.bone_positions[entry.bone_enum] = screen_position;
		}
		else {
			bones.bone_positions[entry.bone_enum] = { 0.f, 0.f, 0.f };
		}
	}
}

void game::kcsgo2struct::CPlayer::CalcScreenPositions(matrix4x4_t* view_matrix, const POINT& screen_size)
{
	screen_pos_valid = false;

	if (!world_to_screen(&origin, &screen_foot, view_matrix, screen_size))
		return;

	Vector3 head_pos = origin;
	head_pos.z += 70.0f;

	if (!world_to_screen(&head_pos, &screen_head, view_matrix, screen_size))
		return;

	screen_pos_valid = true;
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

