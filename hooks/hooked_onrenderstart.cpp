// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

void __fastcall hooked_onrenderstart(void* ecx, void* edx)
{
	auto backup_zoom_sensitivity_ratio_mouse = convars_manager->convars[CONVAR_ZOOM_SENSITIVITY_RATIO_MOUSE]->GetFloat();

	if (config->visuals.world.removals[REMOVALS_ZOOM])
		convars_manager->convars[CONVAR_ZOOM_SENSITIVITY_RATIO_MOUSE]->SetValue(0.0f);

	((OnRenderStart)original_onrenderstart)(ecx);

	if (config->visuals.world.removals[REMOVALS_ZOOM])
		convars_manager->convars[CONVAR_ZOOM_SENSITIVITY_RATIO_MOUSE]->SetValue(backup_zoom_sensitivity_ratio_mouse);
}