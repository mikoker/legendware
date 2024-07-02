// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

void __fastcall hooked_rendermodels(void* ecx, void* edx, void* info, int model_type_count, ModelListByType_t* model_list, int total_model_count, int render_mode, bool shadow_depth_include_translucent_materials)
{
	if (!config->visuals.world.world_color_modulate.world)
		return ((RenderModels)original_rendermodels)(ecx, info, model_type_count, model_list, total_model_count, render_mode, shadow_depth_include_translucent_materials);

	for (auto i = 0; i < model_type_count; ++i)
	{
		auto& list = model_list[i];

		for (auto j = 0; j < list.m_nCount; ++j)
		{
			auto& info = list.m_pRenderModels[j];

			info.m_DiffuseModulation.x *= config->visuals.world.world_color_modulate.props_clr[0];
			info.m_DiffuseModulation.y *= config->visuals.world.world_color_modulate.props_clr[1];
			info.m_DiffuseModulation.z *= config->visuals.world.world_color_modulate.props_clr[2];
			info.m_DiffuseModulation.w *= config->visuals.world.world_color_modulate.props_clr[3];
		}
	}

	return ((RenderModels)original_rendermodels)(ecx, info, model_type_count, model_list, total_model_count, render_mode, shadow_depth_include_translucent_materials);
}