// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

bool __fastcall hooked_setupbones(void* ecx, void* edx, matrix3x4_t* bone_world_out, int max_bones, int bone_mask, float current_time)
{
	auto result = true;
	auto r_jiggle_bones_backup = convars_manager->convars[CONVAR_R_JIGGLE_BONES]->GetInt();

	convars_manager->convars[CONVAR_R_JIGGLE_BONES]->SetValue(0);

	if (ecx)
	{
		auto player = crypt_ptr <Player> ((Player*)((uintptr_t)ecx - 0x4));

		if (player->valid())
		{
			auto animstate = player->get_animation_state();
			auto previous_weapon = animstate ? crypt_ptr <Weapon> (animstate->weapon_last_bone_setup) : crypt_ptr <Weapon>();

			if (previous_weapon)
				animstate->weapon_last_bone_setup = animstate->weapon;

			if (ctx->setuping_bones)
				result = ((SetupBones)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);
			else if (bone_world_out)
				memcpy(bone_world_out, player->m_CachedBoneData().Base(), player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));

			if (previous_weapon)
				animstate->weapon_last_bone_setup = previous_weapon.get();
		}
		else
			result = ((SetupBones)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);
	}
	else
		result = ((SetupBones)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);

	convars_manager->convars[CONVAR_R_JIGGLE_BONES]->SetValue(r_jiggle_bones_backup);
	return result;
}