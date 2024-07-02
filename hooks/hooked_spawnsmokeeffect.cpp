// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

void __fastcall hooked_spawnsmokeeffect(void* ecx, void* edx)
{
	if (config->visuals.world.removals[REMOVALS_SMOKE])
		return;

	return ((SpawnSmokeEffect)original_spawnsmokeeffect)(ecx);
}