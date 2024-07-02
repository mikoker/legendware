// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

void __fastcall hooked_performscreenoverlay(void* ecx, void* edx, int x, int y, int w, int h)
{
	if (config->misc.adblock)
		return;

	return ((PerformScreenOverlay)original_performscreenoverlay)(ecx, x, y, w, h);
}