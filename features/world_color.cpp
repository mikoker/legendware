// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "world_color.h"
#include "..\config\config.h"

void WorldColor::modulate(crypt_ptr <IMaterial> material)
{
	auto material_group_name = crypt_hash_r(material->GetTextureGroupName());

	if (material_group_name != crypt_hash("StaticProp textures") && material_group_name != crypt_hash(TEXTURE_GROUP_OTHER) && material_group_name != crypt_hash(TEXTURE_GROUP_CLIENT_EFFECTS) && material_group_name != crypt_hash(TEXTURE_GROUP_PRECACHED) && material_group_name != crypt_hash(TEXTURE_GROUP_SKYBOX) && material_group_name != crypt_hash(TEXTURE_GROUP_VGUI) && material_group_name != crypt_hash(TEXTURE_GROUP_MODEL))
	{
		if (config->visuals.world.world_color_modulate.world)
		{
			material->AlphaModulate(config->visuals.world.world_color_modulate.world_clr[3]);
			material->ColorModulate(config->visuals.world.world_color_modulate.world_clr[0], config->visuals.world.world_color_modulate.world_clr[1], config->visuals.world.world_color_modulate.world_clr[2]);
		}
		else
		{
			material->AlphaModulate(1.0f);
			material->ColorModulate(1.0f, 1.0f, 1.0f);
		}
	}
}

void WorldColor::apply()
{
	static auto night_mode_enable = false;
	static auto night_mode_clr = Color::White;

	if (!force_update && night_mode_enable == config->visuals.world.world_color_modulate.world && night_mode_clr == Color(config->visuals.world.world_color_modulate.world_clr))
		return;

	force_update = false;

	night_mode_enable = config->visuals.world.world_color_modulate.world;
	night_mode_clr = Color(config->visuals.world.world_color_modulate.world_clr);

	for (auto i = materialsystem->FirstMaterial(); i != materialsystem->InvalidMaterial(); i = materialsystem->NextMaterial(i))
	{
		auto material = crypt_ptr <IMaterial> (materialsystem->GetMaterial(i));

		if (!material)
			continue;

		if (material->IsErrorMaterial())
			continue;

		modulate(material);
	}
}