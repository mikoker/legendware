// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\config\config.h"
#include "..\features\features.h"


enum
{
	ENEMY,
	LOCAL,
	TEAM
};

void __fastcall hooked_postscreeneffects(void* ecx, void* edx, CViewSetup* setup)
{
	static auto original = hooks_manager->hooks[HOOK_CLIENTMODE]->get_func_address <DoPostScreenEffects>(crypt_hash_n(44));

	if (!ctx->local())
		return original(ecx, setup);

	static std::vector<std::pair<int, int>> customGlowEntities;

	for (const auto& [entityIndex, glowObjectIndex] : customGlowEntities)
		glow->UnregisterGlowObject(glowObjectIndex);

	customGlowEntities.clear();

	for (auto i = 0; i < glow->m_GlowObjectDefinitions.Count(); i++)
	{
		auto object = crypt_ptr <CGlowObjectManager::GlowObjectDefinition_t>(&glow->m_GlowObjectDefinitions[i]);

		if (!object)
			continue;

		if (object->IsUnused())
			continue;

		auto entity = crypt_ptr <Entity>(object->GetEnt());

		if (!entity)
			continue;
		if (entity->IsDormant())
			continue;

		auto client_class = crypt_ptr <ClientClass>(entity->GetClientClass());

		if (!client_class)
			continue;

		switch (client_class->m_ClassID) {
		case ClassID::CEconEntity:
		case ClassID::CPlantedC4:
			if (!glow->HasGlowEffect(entity.get())) {
				if (auto index{ glow->RegisterGlowObject(entity.get()) }; index != -1)
					customGlowEntities.emplace_back(i, index);
			}
			break;
		case ClassID::CBaseCSGrenadeProjectile:
		case ClassID::CBreachChargeProjectile:
		case ClassID::CBumpMineProjectile:
		case ClassID::CDecoyProjectile:
		case ClassID::CMolotovProjectile:
		case ClassID::CSensorGrenadeProjectile:
		case ClassID::CSmokeGrenadeProjectile:
		case ClassID::CSnowballProjectile:
			if (config->visuals.world.nade.glow) {
				if (!glow->HasGlowEffect(entity.get())) {
					if (auto index{ glow->RegisterGlowObject(entity.get()) }; index != -1)
						customGlowEntities.emplace_back(i, index);
				}
				object->Set(config->visuals.world.nade.glow_clr[0], config->visuals.world.nade.glow_clr[1], config->visuals.world.nade.glow_clr[2], config->visuals.world.nade.glow_clr[3], 1, config->visuals.world.nade.glow_type);
			}

		}
		if (entity->is_weapon() && config->visuals.world.weapon.glow)
		{
			if (!entity->m_hOwnerEntity().IsValid() &&
				entity->m_vecOrigin() != Vector(0, 0, 0)) {
				object->Set(config->visuals.world.weapon.glow_clr[0], config->visuals.world.weapon.glow_clr[1], config->visuals.world.weapon.glow_clr[2], config->visuals.world.weapon.glow_clr[3], 1, config->visuals.world.weapon.glow_type);
			}
		}
		if (client_class->m_ClassID == CPlantedC4)
		{
;
			if (config->visuals.world.c4.glow)
				object->Set(config->visuals.world.c4.glow_clr[0], config->visuals.world.c4.glow_clr[1], config->visuals.world.c4.glow_clr[2], config->visuals.world.c4.glow_clr[3], 1, config->visuals.world.c4.glow_type);
		}
		if (client_class->m_ClassID == CCSPlayer)
		{

			auto player = crypt_ptr <Player>((Player*)entity.get());

			if (player->m_lifeState() != LIFE_ALIVE)
				continue;

			int type = LOCAL;
			if (player.get() != ctx->local().get()) {
				if (player->m_iTeamNum() != ctx->local()->m_iTeamNum() || ctx->friendly_fire)
					type = ENEMY;
				else if (player->m_iTeamNum() == ctx->local()->m_iTeamNum())
					type = TEAM;
			}

			if (config->visuals.enemy.glow_enable && type == ENEMY) {
				object->Set(config->visuals.enemy.glow_clr[0], config->visuals.enemy.glow_clr[1], config->visuals.enemy.glow_clr[2], config->visuals.enemy.glow_clr[3], 1, config->visuals.enemy.glow_type);
			}
			else if (config->visuals.local.glow_enable && type == LOCAL)
			{
				object->Set(config->visuals.local.glow_clr[0], config->visuals.local.glow_clr[1], config->visuals.local.glow_clr[2], config->visuals.local.glow_clr[3], 1, config->visuals.local.glow_type);
			}
			else if (config->visuals.team.glow_enable && type == TEAM)
			{
				object->Set(config->visuals.team.glow_clr[0], config->visuals.team.glow_clr[1], config->visuals.team.glow_clr[2], config->visuals.team.glow_clr[3], 1, config->visuals.team.glow_type);
			}
		}
	}

	return original(ecx, setup);
}