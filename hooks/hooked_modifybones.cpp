// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "../features/local_animations.h"
#include "../features/features.h"

Vector* __fastcall hooked_get_eye_angles(Player* player, void* edx)
{
	static auto return_to_clampbonesinbbox = (void*)(signatures_manager->signatures[SIGNATURE_RETURN_TO_CLAMPBONESINBBOX]);

	if ((uint32_t)_ReturnAddress() == signatures_manager->signatures[SIGNATURE_RET_EYEPOS_AND_VECTORS]
		&& *(void**)((uint32_t)_AddressOfReturnAddress() + 0x20) == return_to_clampbonesinbbox
		&& engine->IsInGame())
	{
		if (player->EntIndex() == engine->GetLocalPlayer())
			return &local_animations->sent_angle;
		else if (uint32_t(player->EntIndex() - 1) <= 63)
			return &player->m_angEyeAngles();
	}

	return ((GetEyeAngles)original_get_eye_angles)(player);
}

void __fastcall hooked_modifybones(Player* player, void* edx, void* bones, int mask)
{
	if (player->EntIndex() != engine->GetLocalPlayer())
		return ((ModifyBones)original_modifybones)(player, bones, mask);

	if (!ctx->arti_never_wanted_to_fix_this)
		return;
	return ((ModifyBones)original_modifybones)(player, bones, mask);
}

void __fastcall hooked_set_collision_bounds(ICollideable* p, int edx, const Vector& mi, const Vector& ma)
{
	auto daun = (Player*)(uint32_t(p) - 0x320);

	if (!daun || daun->EntIndex() < 0 || daun->EntIndex() > 64 || !daun->is_player())
		return reinterpret_cast <void(__thiscall*)(ICollideable*, const Vector&, const Vector&)>(original_set_collision_bounds)(p, mi, ma);

	//TODO: predict it for local & enemies.
	// dont remove the restore, its needed to fix valves brain.
	//

	reinterpret_cast <void(__thiscall*)(ICollideable*, const Vector&, const Vector&)>(original_set_collision_bounds)(p, mi, ma);
}