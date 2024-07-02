// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\features.h"
#include "..\features\prediction.h"

int __fastcall hooked_entityinterpolate(Entity* entity, void* edx, float& time, Vector& old_origin, Vector& old_angles, int& no_more_changes)
{
	if (!ctx->local()->valid())
		return ((EntityInterpolate)original_entityinterpolate)(entity, time, old_origin, old_angles, no_more_changes);

	if ((Entity*)entity != ctx->local().get())
		return ((EntityInterpolate)original_entityinterpolate)(entity, time, old_origin, old_angles, no_more_changes);

	engine_prediction->fix_interpolation(false);
	auto result = ((EntityInterpolate)original_entityinterpolate)(entity, time, old_origin, old_angles, no_more_changes);

	engine_prediction->fix_interpolation(true);
	return result;
}