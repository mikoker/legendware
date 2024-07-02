// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\features.h"
#include "..\features\animations.h"
#include "..\render.h"

void __fastcall hooked_updateclientsideanimation(Player* player, void* edx)
{
	if (!player)
		return ((UpdateClientSideAnimation)original_updateclientsideanimation)(player);

	if (!player->valid())
		return ((UpdateClientSideAnimation)original_updateclientsideanimation)(player);

	if (ctx->updating_animation)
		return ((UpdateClientSideAnimation)original_updateclientsideanimation)(player);

	if (player == ctx->local().get())
		local_animations->render();
	else if (!animations->animation_data[player->EntIndex()].empty())
	{
		auto front = crypt_ptr <AnimationData> (&animations->animation_data[player->EntIndex()].front());

		if (front->dormant)
			return;

		if (front->render_origin == player->GetAbsOrigin())
			return;

		for (auto i = 0; i < front->bone_count; ++i)
		{
			front->matrix[MATRIX_VISUAL_INTERPOLATED][i][0][3] -= front->render_origin.x;
			front->matrix[MATRIX_VISUAL_INTERPOLATED][i][1][3] -= front->render_origin.y;
			front->matrix[MATRIX_VISUAL_INTERPOLATED][i][2][3] -= front->render_origin.z;

			front->matrix[MATRIX_VISUAL_INTERPOLATED][i][0][3] += player->GetAbsOrigin().x;
			front->matrix[MATRIX_VISUAL_INTERPOLATED][i][1][3] += player->GetAbsOrigin().y;
			front->matrix[MATRIX_VISUAL_INTERPOLATED][i][2][3] += player->GetAbsOrigin().z;

			if (player->m_CachedBoneData().Count() > i)
				player->m_CachedBoneData().Base()[i] = front->matrix[MATRIX_VISUAL_INTERPOLATED][i];
		}

		front->render_origin = player->GetAbsOrigin();
		player->attachment_helper();
	}
}