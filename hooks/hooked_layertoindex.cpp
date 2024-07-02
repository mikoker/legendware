// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

bool __fastcall hooked_layertoindex(AnimationState* animation_state, void* edx, AnimationLayer* layer, int& index)
{
	return false;
}