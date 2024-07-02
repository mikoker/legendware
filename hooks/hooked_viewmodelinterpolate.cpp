// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\features.h"
#include "..\features\prediction.h"

bool __fastcall hooked_viewmodelinterpolate(Viewmodel* viewmodel, void* edx, float time)
{
	if (!ctx->local()->valid())
		return ((ViewmodelInterpolate)original_viewmodelinterpolate)(viewmodel, time);

	auto local_viewmodel = crypt_ptr <Viewmodel> (ctx->local()->m_hViewModel().Get());

	if (viewmodel != local_viewmodel.get())
		return ((ViewmodelInterpolate)original_viewmodelinterpolate)(viewmodel, time);

	engine_prediction->fix_interpolation(false, true);
	auto result = ((ViewmodelInterpolate)original_viewmodelinterpolate)(viewmodel, time);

	engine_prediction->fix_interpolation(true, true);
	return result;
}