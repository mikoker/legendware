// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\menu\menu.h"
#include "..\features\features.h"
#include "..\features\third_person.h"


void __stdcall hooked_overrideview(CViewSetup* setup)
{
	static auto original = hooks_manager->hooks[HOOK_CLIENTMODE]->get_func_address <OverrideView> (crypt_hash_n(18));

	if (!engine->IsInGame())
		return original(setup);

	if (ctx->local()->valid() && ctx->weapon())
	{
		if (config->visuals.world.removals[REMOVALS_ZOOM])
		{
			if (!ctx->local()->m_bIsScoped() || ctx->weapon()->m_zoomLevel() == 1)
				setup->fov = (float)config->misc.field_of_view;
			else
				setup->fov = (float)config->misc.field_of_view * 0.5f;
		}
		else
			setup->fov += (float)config->misc.field_of_view - 90.0f;

		auto valid_weapon = true;

		if (ctx->local()->m_bIsScoped() && !input->m_fCameraInThirdPerson && (ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_AUG || ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_SG553))
			valid_weapon = false;

		if (config->misc.viewmodel_changer && config->misc.viewmodel_roll && valid_weapon)
		{
			auto viewmodel = crypt_ptr <Viewmodel> (ctx->local()->m_hViewModel().Get());

			if (viewmodel)
				viewmodel->set_abs_angles(Vector(setup->angles.x, setup->angles.y, setup->angles.z + (float)config->misc.viewmodel_roll));
		}
	}
	else if (config->visuals.world.removals[REMOVALS_ZOOM])
		setup->fov = (float)config->misc.field_of_view;

	ctx->field_of_view = setup->fov;

	third_person->run();
	original(setup);

	if (ctx->visual_fake_ducking)
	{
		setup->origin = ctx->local()->GetAbsOrigin() + Vector(0.0f, 0.0f, movement->GetPlayerViewOffset(false).z);

		if (input->m_fCameraInThirdPerson)
		{
			Vector camera_forward;

			math::angle_vectors(Vector(input->m_vecCameraOffset.x, input->m_vecCameraOffset.y, 0.0f), &camera_forward, nullptr, nullptr);
			math::vector_ma(setup->origin, -input->m_vecCameraOffset.z, camera_forward, setup->origin);
		}
	}
}