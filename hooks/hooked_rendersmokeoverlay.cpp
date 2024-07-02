// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

void __fastcall hooked_rendersmokeoverlay(void* ecx, void* edx, bool pre_view_model)
{
	if (config->visuals.world.removals[REMOVALS_SMOKE])
		return;

	return ((RenderSmokeOverlay)original_rendersmokeoverlay)(ecx, pre_view_model);
}