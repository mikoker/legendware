// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "penetration.h"

PenetrationInfo Penetration::run(const Vector& shoot_position, const Vector& point, crypt_ptr <Player> entity, bool penetrate_walls) //-V813
{
	auto direction = (point - shoot_position).Normalized();

	auto visible = true;
	auto damage = -1.0f;
	auto hitbox = -1;
	auto hitgroup = -1;
	auto penetration_count = 4;

	if (fire_bullet(direction, visible, damage, hitbox, hitgroup, penetration_count, entity, shoot_position, penetrate_walls))
		return PenetrationInfo(visible, (int)damage, hitbox, hitgroup, penetration_count); //-V2003

	return PenetrationInfo(true, -1, -1, -1, 4);
}

bool Penetration::is_breakable_entity(crypt_ptr <IClientEntity> entity)
{
	if (!entity)
		return false;

	if (!entity->EntIndex())
		return false;

	auto client_class = crypt_ptr <ClientClass>(entity->GetClientClass());

	if (!client_class)
		return false;

	auto take_damage = *(uintptr_t*)((uintptr_t)signatures_manager->signatures[SIGNATURE_IS_BREAKABLE] + 0x26);
	auto backup = *(uint8_t*)((uintptr_t)entity.get() + take_damage);

	if (crypt_hash_r(client_class->m_pNetworkName) == crypt_hash("CBreakableSurface"))
		*(uint8_t*)((uintptr_t)entity.get() + take_damage) = DAMAGE_YES;
	else if (crypt_hash_r(client_class->m_pNetworkName) == crypt_hash("CBaseDoor") || crypt_hash_r(client_class->m_pNetworkName) == crypt_hash("CDynamicProp"))
		*(uint8_t*)((uintptr_t)entity.get() + take_damage) = DAMAGE_NO;

	auto result = reinterpret_cast <bool(__thiscall*)(IClientEntity*)> (signatures_manager->signatures[SIGNATURE_IS_BREAKABLE])(entity.get());
	*(uint8_t*)((uintptr_t)entity.get() + take_damage) = backup;

	return result;
}

void Penetration::scale_damage(crypt_ptr <Player> player, CGameTrace& enter_trace, float& current_damage) //-V813
{
	auto is_armored = [&]() -> bool
	{
		auto has_helmet = player->m_bHasHelmet();
		auto armor_value = player->m_ArmorValue();

		if (armor_value > 0)
		{
			switch (enter_trace.hitgroup)
			{
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
			case HITGROUP_GEAR:
				return true;
			case HITGROUP_HEAD:
				return has_helmet || player->m_bHasHeavyArmor();
			}

			return player->m_bHasHeavyArmor();
		}

		return false;
	};

	auto head_scale = player->m_iTeamNum() == 3 ? convars_manager->convars[CONVAR_MP_DAMAGE_SCALE_CT_HEAD]->GetFloat() : convars_manager->convars[CONVAR_MP_DAMAGE_SCALE_T_HEAD]->GetFloat();
	auto body_scale = player->m_iTeamNum() == 3 ? convars_manager->convars[CONVAR_MP_DAMAGE_SCALE_CT_BODY]->GetFloat() : convars_manager->convars[CONVAR_MP_DAMAGE_SCALE_T_BODY]->GetFloat();

	auto armor_heavy = player->m_bHasHeavyArmor();

	if (armor_heavy)
		head_scale *= 0.5f;

	switch (enter_trace.hitgroup)
	{
	case HITGROUP_HEAD:
		current_damage *= 4.0f * head_scale;
		break;
	case HITGROUP_CHEST:
		current_damage *= body_scale; //-V1037
		break;
	case HITGROUP_GEAR:
		current_damage *= body_scale;
		break;
	case HITGROUP_STOMACH:
		current_damage *= 1.25f * body_scale;
		break;
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
		current_damage *= body_scale;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		current_damage *= 0.75f * body_scale;
		break;
	default:
		break;
	}

	if (is_armored())
	{
		auto armor_scale = 1.0f;
		auto armor_ratio = ctx->weapon_data()->armor_ratio * 0.5f;
		auto armor_bonus_ratio = 0.5f;

		if (armor_heavy)
		{
			armor_ratio *= 0.2f;
			armor_bonus_ratio = 0.33f;
			armor_scale = 0.25f;
		}

		auto new_damage = current_damage * armor_ratio;
		auto estiminated_damage = (current_damage - current_damage * armor_ratio) * armor_scale * armor_bonus_ratio;
		auto armor_value = (float)player->m_ArmorValue();

		if (estiminated_damage > armor_value)
			new_damage = current_damage - armor_value / armor_bonus_ratio;

		current_damage = new_damage;
	}
}

bool Penetration::trace_to_exit(CGameTrace& enter_trace, CGameTrace& exit_trace, Vector start_position, const Vector& direction) //-V813
{
	auto enter_point_contents = 0;
	auto point_contents = 0;

	auto is_window = false;
	auto flag = false;

	auto distance = 0.0f;

	Vector start; //-V688
	Vector end; //-V688

	do
	{
		distance += 4.0f;

		end = start_position + direction * distance;
		start = end - direction * 4.0f;

		if (!enter_point_contents)
		{
			enter_point_contents = enginetrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX);
			point_contents = enter_point_contents;
		}
		else
			point_contents = enginetrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX);

		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX) || enter_point_contents == point_contents))
			continue;

		uint32_t filter[4] =
		{
			*(uint32_t*)(signatures_manager->signatures[SIGNATURE_TRACE_FILTER_SIMPLE] + 0x3D),
			(uint32_t)ctx->local().get(),
			0,
			0
		};

		Ray_t ray;
		ray.Init(end, start);

		enginetrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, (CTraceFilter*)filter, &exit_trace); //-V641

		if (exit_trace.startsolid && exit_trace.surface.flags & SURF_HITBOX)
		{
			ray.Init(end, start_position);
			filter[1] = (uint32_t)exit_trace.hit_entity;

			enginetrace->TraceRay(ray, MASK_SHOT_HULL, (CTraceFilter*)filter, &exit_trace); //-V641

			if (exit_trace.DidHit() && !exit_trace.startsolid)
				return true;

			continue;
		}

		auto surface_name = (int*)enter_trace.surface.name; //-V206

		if (surface_name)
		{
			if (*surface_name == 1936744813 && surface_name[1] == 1601397551 && surface_name[2] == 1768318575 && surface_name[3] == 1731159395 && surface_name[4] == 1936941420 && surface_name[5] == 1651668271 && surface_name[6] == 1734307425 && surface_name[7] == 1936941420)
				is_window = true;
			else
			{
				is_window = false;

				if (*surface_name != 1936744813)
					goto LABEL_34;
			}

			if (surface_name[1] == 1600480303 && surface_name[2] == 1701536108 && surface_name[3] == 1634494255 && surface_name[4] == 1731162995 && surface_name[5] == 1936941420)
			{
				flag = true;

			LABEL_35:
				if (is_window || flag)
				{
					exit_trace = enter_trace;
					exit_trace.endpos = end + direction;
					return true;
				}

				goto LABEL_37;
			}

		LABEL_34:
			flag = false;
			goto LABEL_35;
		}

	LABEL_37:
		if (!exit_trace.DidHit() || exit_trace.startsolid)
		{
			if (enter_trace.hit_entity && enter_trace.hit_entity->EntIndex() && is_breakable_entity(enter_trace.hit_entity))
			{
				exit_trace = enter_trace;
				exit_trace.endpos = start_position + direction;
				return true;
			}

			continue;
		}

		if (exit_trace.surface.flags & SURF_NODRAW)
		{
			if (is_breakable_entity(exit_trace.hit_entity) && is_breakable_entity(enter_trace.hit_entity))
				return true;

			if (!(enter_trace.surface.flags & SURF_NODRAW))
				continue;
		}

		if (exit_trace.plane.normal.Dot(direction) <= 1.0f)
			return true;
	}
	while (distance <= 90.0f);

	return false;
}

bool Penetration::handle_bullet_penetration(CGameTrace& enter_trace, Vector& shoot_position, const Vector& direction, int& penetration_count, float& current_damage)
{
	if (ctx->weapon_data()->penetration <= 0.0f)
		return false;

	if (penetration_count <= 0)
		return false;

	auto contents_grate = enter_trace.contents & CONTENTS_GRATE;
	auto surf_nodraw = enter_trace.surface.flags & SURF_NODRAW;

	auto enter_surface_data = physicssurface->GetSurfaceData(enter_trace.surface.surfaceProps);
	auto enter_material = enter_surface_data->game.material;

	auto is_solid_surf = enter_trace.contents >> 3 & CONTENTS_SOLID;
	auto is_light_surf = enter_trace.surface.flags >> 7 & SURF_LIGHT;

	CGameTrace exit_trace;

	if (!trace_to_exit(enter_trace, exit_trace, enter_trace.endpos, direction) && !(enginetrace->GetPointContents(enter_trace.endpos, MASK_SHOT_HULL) & MASK_SHOT_HULL))
		return false;

	auto enter_penetration_modifier = enter_surface_data->game.flPenetrationModifier;
	auto exit_surface_data = physicssurface->GetSurfaceData(exit_trace.surface.surfaceProps);

	if (!exit_surface_data)
		return false;

	auto exit_material = exit_surface_data->game.material;
	auto exit_penetration_modifier = exit_surface_data->game.flPenetrationModifier;

	auto combined_damage_modifier = 0.16f;
	auto combined_penetration_modifier = (enter_penetration_modifier + exit_penetration_modifier) * 0.5f;

	if (enter_material == CHAR_TEX_GLASS || enter_material == CHAR_TEX_GRATE)
	{
		combined_penetration_modifier = 3.0f;
		combined_damage_modifier = 0.05f;
	}
	else if (contents_grate || surf_nodraw)
		combined_penetration_modifier = 1.0f;
	else if (enter_material == CHAR_TEX_FLESH && ((Player*)enter_trace.hit_entity)->m_iTeamNum() == ctx->local()->m_iTeamNum() && !convars_manager->convars[CONVAR_FF_DAMAGE_REDUCTION_BULLETS]->GetFloat())
	{
		if (!convars_manager->convars[CONVAR_FF_DAMAGE_BULLET_PENETRATION]->GetFloat()) //-V550
			return false;

		combined_penetration_modifier = convars_manager->convars[CONVAR_FF_DAMAGE_BULLET_PENETRATION]->GetFloat();
		combined_damage_modifier = 0.16f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == CHAR_TEX_WOOD || exit_material == CHAR_TEX_CARDBOARD)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == CHAR_TEX_PLASTIC)
			combined_penetration_modifier = 2.0f;
	}

	auto penetration_modifier = max(0.0f, 1.0f / combined_penetration_modifier);
	auto penetration_distance = (exit_trace.endpos - enter_trace.endpos).Length();

	penetration_distance = penetration_distance * penetration_distance * penetration_modifier * 0.041666668f;

	auto damage_modifier = max(0.0f, 3.0f / ctx->weapon_data()->penetration * 1.25f) * penetration_modifier * 3.0f + current_damage * combined_damage_modifier + penetration_distance;
	auto damage_lost = max(0.0f, damage_modifier);

	if (damage_lost > current_damage)
		return false;

	current_damage -= damage_lost;

	if (current_damage < 1.0f)
		return false;

	shoot_position = exit_trace.endpos;
	--penetration_count;

	return true;
}

void Penetration::clip_trace_to_player(crypt_ptr <Player> player, const Vector& start, const Vector& end, unsigned int mask, crypt_ptr <CTraceFilter> filter, crypt_ptr <CGameTrace> trace)
{
	auto collideable = crypt_ptr <ICollideable> (player->GetCollideable());

	if (!collideable)
		return;

	auto mins = collideable->OBBMins();
	auto maxs = collideable->OBBMaxs();

	auto direction = end - start;
	direction.Normalize();

	auto center = (maxs + mins) / 0.5f;
	auto position = center + player->m_vecOrigin();

	auto to = position - start;
	auto range_along = direction.Dot(to);

	auto range = 0.0f;

	if (range_along < 0.0f)
		range = -to.Length();
	else if (range_along > direction.Length())
		range = -(position - end).Length();
	else
	{
		auto ray = position - (direction * range_along + start);
		range = ray.Length();
	}

	if (range <= 60.0f)
	{
		CGameTrace current_trace;

		Ray_t ray;
		ray.Init(start, end);

		enginetrace->ClipRayToEntity(ray, mask, player.get(), &current_trace);

		if (trace->fraction > current_trace.fraction)
			*trace.get() = current_trace;
	}
}

bool Penetration::fire_bullet(Vector& direction, bool& visible, float& current_damage, int& hitbox, int& hitgroup, int& penetration_count, crypt_ptr <Player> player, const Vector& shoot_position, bool penetrate_walls)
{
	CGameTrace enter_trace;
	CTraceFilter filter;

	filter.pSkip = ctx->local().get();
	current_damage = ctx->weapon_data()->damage;

	auto current_shoot_position = shoot_position;
	auto current_distance = 0.0f;
	auto max_range = ctx->weapon_data()->range;

	while (current_damage >= 1.0f)
	{
		max_range -= current_distance;
		auto end = current_shoot_position + direction * max_range;

		Ray_t ray;
		ray.Init(current_shoot_position, end);

		CTraceFilter filter;
		filter.pSkip = ctx->local().get();

		enginetrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enter_trace);

		if (player)
			clip_trace_to_player(player, shoot_position, end + direction * 40.0f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enter_trace);

		if (enter_trace.fraction == 1.0f) //-V550
			break;

		current_distance += enter_trace.fraction * max_range;
		current_damage *= pow(ctx->weapon_data()->range_modifier, current_distance * 0.002f);

		auto enter_surface_data = physicssurface->GetSurfaceData(enter_trace.surface.surfaceProps);

		if (current_distance > 3000.0f && ctx->weapon_data()->penetration || enter_surface_data->game.flPenetrationModifier < 0.1f) //-V1051
			break;

		if (enter_trace.hit_entity && enter_trace.hitgroup <= HITGROUP_RIGHTLEG && enter_trace.hit_entity == player.get())
		{
			if (ctx->weapon() && ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_TASER)
				current_damage *= 0.92f;
			else
				scale_damage((Player*)enter_trace.hit_entity, enter_trace, current_damage);

			hitbox = enter_trace.hitbox;
			hitgroup = enter_trace.hitgroup;
			return true;
		}

		if (!penetration_count)
			break;

		auto penetrate = penetrate_walls;

		if (!penetrate_walls)
			penetrate = enter_trace.contents & CONTENTS_WINDOW;

		if (!penetrate || !handle_bullet_penetration(enter_trace, current_shoot_position, direction, penetration_count, current_damage))
			break;

		visible = false;

		if (!player)
			return true;
	}

	return false;
}
