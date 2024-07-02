// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

void __fastcall hooked_standardblendingrules(Player* player, void* edx, CStudioHdr* hdr, Vector* position, Quaternion* q, float current_time, int bone_mask)
{
	if (!player->valid())
		return ((StandardBlendingRules)original_standardblendingrules)(player, hdr, position, q, current_time, bone_mask);

	((StandardBlendingRules)original_standardblendingrules)(player, hdr, position, q, current_time, bone_mask & ~0xC0000);

	if (player->m_fEffects() & 8)
		player->m_fEffects() &= ~8;
}