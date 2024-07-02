// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

bool __fastcall hooked_shouldcollide(void* ecx, void* edx, int collision_group, int contents_mask)
{
	auto physics_solid_mask_for_entity = [](Player* entity) -> unsigned int
	{
		if (!entity)
			return MASK_PLAYERSOLID;

		if (entity->m_lifeState() == LIFE_DYING)
			return MASK_PLAYERSOLID_BRUSHONLY;

		auto solid_teammates = convars_manager->convars[CONVAR_MP_SOLID_TEAMMATES]->GetInt();

		if (solid_teammates && solid_teammates != 2)
			return MASK_PLAYERSOLID;

		switch (entity->m_iTeamNum())
		{
		case 2:
			return MASK_PLAYERSOLID | CONTENTS_TEAM1;
		case 3:
			return MASK_PLAYERSOLID | CONTENTS_TEAM2;
		}

		return MASK_PLAYERSOLID;
	};

	if (collision_group == COLLISION_GROUP_PLAYER_MOVEMENT)
	{
		auto team_mask = physics_solid_mask_for_entity((Player*)ecx) & (CONTENTS_TEAM1 | CONTENTS_TEAM2);
		auto other_team_mask = contents_mask & (CONTENTS_TEAM1 | CONTENTS_TEAM2);

		if (team_mask && team_mask == other_team_mask)
			return false;
	}

	return ((ShouldCollide)original_shouldcollide)(ecx, collision_group, contents_mask);
}