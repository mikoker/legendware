#include "legit_aim.h"


void LegitAim::run(crypt_ptr <CUserCmd> cmd)
{
	attack = false;

	final_target.reset();
	targets.clear();
	backup.clear();

	if (!ctx->weapon()->can_fire())
		return;

	if (ctx->local()->m_flFlashDuration() > 0.0f)
	{
		auto flash_value = (ctx->local()->m_flFlashBangTime() - globals->curtime) / ctx->local()->m_flFlashDuration();

		if (config->legit.checks[CHECKS_FLASH] && flash_value > 0.5f)
			return;
	}

	original_viewangles = cmd->viewangles;

	recoil_control_system(cmd);
	prepare();
	scan();
	fire(cmd);
	automatic_fire(cmd);

	auto previous_attack = crypt_ptr <CUserCmd> (input->GetUserCmd(cmd->command_number - 1));

	if (!final_target.data || previous_attack->buttons & IN_ATTACK && !ctx->local()->m_iShotsFired())
		first_shot = false;

	for (auto& data : backup)
		data.apply(MATRIX_MAIN, true);
}

void LegitAim::prepare()
{
	auto best_field_of_view = FLT_MAX;

	for (auto i = 1; i <= globals->maxclients; ++i)
	{
		auto player = crypt_ptr <Player> ((Player*)entitylist->GetClientEntity(i));

		if (!player->valid(!ctx->friendly_fire))
			continue;

		if (animations->animation_data[i].empty())
			continue;

		if (config->player_list.player_settings[i].ignore)
			continue;

		targets.emplace_back(LegitPreparedTarget(i, player));
	}
}

void LegitAim::scan()
{
	for (auto& target : targets)
	{
		auto found_backup = false;

		for (auto& backup_data : backup)
		{
			if (backup_data.i == target.index)
			{
				found_backup = true;
				break;
			}
		}

		if (!found_backup)
			backup.emplace_back(AnimationData(target.player));

		auto first = true;

		for (auto i = 0; i < target.data->size(); ++i)
		{
			if (!config->legit.backtrack_limit && !first)
				break;

			first = false;
			auto data = crypt_ptr <AnimationData> (&target.data->at(i));

			if (!data->valid(true, config->legit.backtrack_limit ? (float)config->legit.backtrack_limit * 0.001f : 0.2f))
				continue;

			data->apply();
			scan_hitboxes(target.player, data);
		}
	}
}

void LegitAim::fire(crypt_ptr <CUserCmd> cmd)
{
	if (!final_target.data)
		return;

	if (config->legit.weapon[ctx->weapon_config].target_switch_delay && final_target.data->i != previous_target)
	{
		previous_target = final_target.data->i;
		target_switch_delay = globals->realtime + (float)config->legit.weapon[ctx->weapon_config].target_switch_delay * 0.001f;
	}

	if (target_switch_delay > globals->realtime)
		return;

	auto fire_delay = config->legit.weapon[ctx->weapon_config].automatic_attack ? (float)config->legit.weapon[ctx->weapon_config].fire_delay * 0.001f : 0.0f;

	if (!ctx->local()->can_shoot(true, fire_delay))
		return;

	if (!config->legit.enable_key.state)
		return;

	Vector punch_angle;

	if (config->legit.weapon[ctx->weapon_config].pitch_recoil)
		punch_angle.x = ctx->local()->m_aimPunchAngle().x * convars_manager->convars[CONVAR_WEAPON_RECOIL_SCALE]->GetFloat() * 0.01f * (float)config->legit.weapon[ctx->weapon_config].pitch_recoil;

	if (config->legit.weapon[ctx->weapon_config].yaw_recoil)
		punch_angle.y = ctx->local()->m_aimPunchAngle().y * convars_manager->convars[CONVAR_WEAPON_RECOIL_SCALE]->GetFloat() * 0.01f * (float)config->legit.weapon[ctx->weapon_config].yaw_recoil;

	auto angle = math::normalize_angles(math::calculate_angle(ctx->shoot_position, final_target.point.point) - original_viewangles - punch_angle);
	auto smooth_value = get_smooth_value();

	if (smooth_value > 1.0f)
		angle /= smooth_value;
	else
		attack = true;

	cmd->viewangles = math::normalize_angles(original_viewangles + angle);
	cmd->tickcount = TIME_TO_TICKS(final_target.data->simulation_time + ctx->interpolation);

	engine->SetViewAngles(cmd->viewangles);

	if (config->legit.weapon[ctx->weapon_config].first_shot_delay && !first_shot && cmd->buttons & IN_ATTACK)
	{
		first_shot = true;
		first_shot_delay = globals->realtime + (float)config->legit.weapon[ctx->weapon_config].first_shot_delay * 0.001f;
	}

	if (first_shot_delay > globals->realtime)
		cmd->buttons &= ~IN_ATTACK;
}

void LegitAim::automatic_fire(crypt_ptr <CUserCmd> cmd)
{
	if (!config->legit.weapon[ctx->weapon_config].automatic_fire)
		return;

	if (!config->legit.automatic_fire_key.state)
		return;

	if (cmd->buttons & IN_ATTACK)
		return;

	auto automatic_stop = false;
	auto simulation_time = 0.0f;

	if (!attack)
	{
		for (auto i = 1; i <= globals->maxclients; ++i)
		{
			auto player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(i));

			if (!player->valid(!ctx->friendly_fire))
				continue;

			if (animations->animation_data[i].empty())
				continue;

			if (config->player_list.player_settings[i].ignore)
				continue;

			auto found_backup = false;

			for (auto& backup_data : backup)
			{
				if (backup_data.i == i)
				{
					found_backup = true;
					break;
				}
			}

			if (!found_backup)
				backup.emplace_back(AnimationData(player));

			auto first = true;
			auto minimum_damage = config->legit.weapon[ctx->weapon_config].minimum_damage;

			if (minimum_damage > 100)
				minimum_damage = clamp(minimum_damage, 1, player->m_iHealth() + minimum_damage - 100);
			else
				minimum_damage = clamp(minimum_damage, 1, player->m_iHealth());

			for (auto& data : animations->animation_data[i])
			{
				if (!config->legit.backtrack_limit && !first)
					break;

				first = false;

				if (!data.valid(true, config->legit.backtrack_limit ? (float)config->legit.backtrack_limit * 0.001f : 0.2f))
					continue;

				data.apply();

				Vector view_angles;
				engine->GetViewAngles(view_angles);

				Vector punch_angle;

				if (config->legit.weapon[ctx->weapon_config].pitch_recoil)
					punch_angle.x = ctx->local()->m_aimPunchAngle().x * convars_manager->convars[CONVAR_WEAPON_RECOIL_SCALE]->GetFloat() * 0.01f * (float)config->legit.weapon[ctx->weapon_config].pitch_recoil;

				if (config->legit.weapon[ctx->weapon_config].yaw_recoil)
					punch_angle.y = ctx->local()->m_aimPunchAngle().y * convars_manager->convars[CONVAR_WEAPON_RECOIL_SCALE]->GetFloat() * 0.01f * (float)config->legit.weapon[ctx->weapon_config].yaw_recoil;

				view_angles = math::normalize_angles(view_angles + punch_angle);

				Vector direction;
				math::angle_vectors(view_angles, &direction, nullptr, nullptr);

				CGameTrace trace;

				Ray_t ray;
				ray.Init(ctx->shoot_position, ctx->shoot_position + direction * ctx->weapon_data()->range);

				enginetrace->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, player.get(), &trace);

				if (trace.hit_entity != player.get())
					continue;

				if (config->legit.checks[CHECKS_SMOKE] && ((bool(*)(Vector, Vector))signatures_manager->signatures[SIGNATURE_LINE_GOES_THROUGH_SMOKE])(ctx->shoot_position, trace.endpos))
					continue;

				auto penetration_info = penetration->run(ctx->shoot_position, trace.endpos, player, config->legit.weapon[ctx->weapon_config].automatic_wall && config->legit.automatic_wall_key.state);

				if (penetration_info.damage < minimum_damage)
					continue;

				if (!is_hit_chanced((float)config->legit.weapon[ctx->weapon_config].hit_chance, view_angles, MATRIX_MAIN, trace.hitbox, player, &data))
				{
					automatic_stop = true;
					continue;
				}

				attack = true;
				simulation_time = data.simulation_time;
				break;
			}

			if (attack)
				break;
		}
	}

	if (config->legit.weapon[ctx->weapon_config].automatic_stop && automatic_stop && engine_prediction->flags & FL_ONGROUND && !ctx->weapon()->m_bIsReloading() && TICKS_TO_TIME(ctx->tickbase) >= ctx->local()->m_flNextAttack())
	{
		auto speed = ctx->local()->m_vecVelocity().Length2D();

		if (speed + 1.0f > ctx->max_speed * 0.33f)
		{
			cmd->forwardmove = 0.0f;
			cmd->sidemove = 0.0f;
		}
	}

	if (!attack)
	{
		if (!ctx->local()->m_iShotsFired())
			delay = globals->realtime + (float)config->legit.weapon[ctx->weapon_config].delay * 0.001f;

		return;
	}

	if (delay > globals->realtime)
		return;

	cmd->buttons |= IN_ATTACK;

	if (simulation_time)
		cmd->tickcount = TIME_TO_TICKS(simulation_time + ctx->interpolation);
}

void LegitAim::recoil_control_system(crypt_ptr <CUserCmd> cmd)
{
	if (ctx->local()->m_iShotsFired() <= 1)
		return;

	ctx->used_recoil_control_system = true;

	if (config->legit.weapon[ctx->weapon_config].pitch_recoil)
	{
		auto value = ctx->local()->m_aimPunchAngle().x - previous_aim_punch_angle.x;
		auto modifier = convars_manager->convars[CONVAR_WEAPON_RECOIL_SCALE]->GetFloat() * 0.01f * (float)config->legit.weapon[ctx->weapon_config].pitch_recoil;

		cmd->viewangles.x = clamp(cmd->viewangles.x - value * modifier, -89.0f, 89.0f);
	}

	if (config->legit.weapon[ctx->weapon_config].yaw_recoil)
	{
		auto value = ctx->local()->m_aimPunchAngle().y - previous_aim_punch_angle.y;
		auto modifier = convars_manager->convars[CONVAR_WEAPON_RECOIL_SCALE]->GetFloat() * 0.01f * (float)config->legit.weapon[ctx->weapon_config].yaw_recoil;

		cmd->viewangles.y = clamp(cmd->viewangles.y - value * modifier, -180.0f, 180.0f);
	}

	previous_aim_punch_angle = ctx->local()->m_aimPunchAngle();
	engine->SetViewAngles(cmd->viewangles);
}

void LegitAim::scan_hitboxes(crypt_ptr <Player> player, crypt_ptr <AnimationData> data)
{
	vector <LegitHitbox> hitboxes; //-V827

	if (config->legit.weapon[ctx->weapon_config].hitboxes[0])
		hitboxes.emplace_back(LegitHitbox(HITBOX_HEAD, config->legit.weapon[ctx->weapon_config].hitboxes_priority[0]));

	if (config->legit.weapon[ctx->weapon_config].hitboxes[1])
	{
		hitboxes.emplace_back(LegitHitbox(HITBOX_LOWER_CHEST, config->legit.weapon[ctx->weapon_config].hitboxes_priority[1]));
		hitboxes.emplace_back(LegitHitbox(HITBOX_UPPER_CHEST, config->legit.weapon[ctx->weapon_config].hitboxes_priority[1]));
	}

	if (config->legit.weapon[ctx->weapon_config].hitboxes[2])
		hitboxes.emplace_back(LegitHitbox(HITBOX_STOMACH, config->legit.weapon[ctx->weapon_config].hitboxes_priority[2]));

	if (config->legit.weapon[ctx->weapon_config].hitboxes[3])
	{
		hitboxes.emplace_back(LegitHitbox(HITBOX_RIGHT_THIGH, config->legit.weapon[ctx->weapon_config].hitboxes_priority[3]));
		hitboxes.emplace_back(LegitHitbox(HITBOX_LEFT_THIGH, config->legit.weapon[ctx->weapon_config].hitboxes_priority[3]));
		hitboxes.emplace_back(LegitHitbox(HITBOX_RIGHT_CALF, config->legit.weapon[ctx->weapon_config].hitboxes_priority[3]));
		hitboxes.emplace_back(LegitHitbox(HITBOX_LEFT_CALF, config->legit.weapon[ctx->weapon_config].hitboxes_priority[3]));
	}

	if (config->legit.weapon[ctx->weapon_config].hitboxes[4])
	{
		hitboxes.emplace_back(LegitHitbox(HITBOX_RIGHT_FOOT, config->legit.weapon[ctx->weapon_config].hitboxes_priority[4]));
		hitboxes.emplace_back(LegitHitbox(HITBOX_LEFT_FOOT, config->legit.weapon[ctx->weapon_config].hitboxes_priority[4]));
	}

	if (config->legit.weapon[ctx->weapon_config].hitboxes[5])
	{
		hitboxes.emplace_back(LegitHitbox(HITBOX_RIGHT_UPPER_ARM, config->legit.weapon[ctx->weapon_config].hitboxes_priority[5]));
		hitboxes.emplace_back(LegitHitbox(HITBOX_LEFT_UPPER_ARM, config->legit.weapon[ctx->weapon_config].hitboxes_priority[5]));
		hitboxes.emplace_back(LegitHitbox(HITBOX_RIGHT_FOREARM, config->legit.weapon[ctx->weapon_config].hitboxes_priority[5]));
		hitboxes.emplace_back(LegitHitbox(HITBOX_LEFT_FOREARM, config->legit.weapon[ctx->weapon_config].hitboxes_priority[5]));
	}

	auto max_priority = -1;

	for (auto& hitbox : hitboxes)
	{
		if (hitbox.priority < max_priority)
			continue;

		Vector view_angles;
		engine->GetViewAngles(view_angles);

		Vector punch_angle;

		if (config->legit.weapon[ctx->weapon_config].pitch_recoil)
			punch_angle.x = ctx->local()->m_aimPunchAngle().x * convars_manager->convars[CONVAR_WEAPON_RECOIL_SCALE]->GetFloat() * 0.01f * (float)config->legit.weapon[ctx->weapon_config].pitch_recoil;

		if (config->legit.weapon[ctx->weapon_config].yaw_recoil)
			punch_angle.y = ctx->local()->m_aimPunchAngle().y * convars_manager->convars[CONVAR_WEAPON_RECOIL_SCALE]->GetFloat() * 0.01f * (float)config->legit.weapon[ctx->weapon_config].yaw_recoil;

		view_angles = math::normalize_angles(view_angles + punch_angle);

		auto point = player->get_hitbox_position(hitbox.hitbox, data->matrix[MATRIX_MAIN]);
		auto angle = math::calculate_angle(ctx->shoot_position, point);
		auto field_of_view = math::get_field_of_view(view_angles, angle);

		if (field_of_view > (float)config->legit.weapon[ctx->weapon_config].field_of_view)
			continue;

		if (field_of_view > final_target.field_of_view)
			continue;

		if (config->legit.checks[CHECKS_SMOKE] && ((bool(*)(Vector, Vector))signatures_manager->signatures[SIGNATURE_LINE_GOES_THROUGH_SMOKE])(ctx->shoot_position, point))
			continue;

		CGameTrace trace;

		CTraceFilter filter;
		filter.pSkip = ctx->local().get();

		Ray_t ray;
		ray.Init(ctx->shoot_position, point);

		enginetrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

		if (trace.fraction < 0.95f)
		{
			if (config->legit.weapon[ctx->weapon_config].automatic_wall && config->legit.automatic_wall_key.state)
			{
				auto penetration_info = penetration->run(ctx->shoot_position, point, player, true);

				if (penetration_info.damage < 1)
					continue;
			}
			else if (trace.hit_entity != player.get())
				continue;
		}

		final_target.visible = trace.fraction >= 0.95f;
		final_target.hitbox = hitbox.hitbox;
		final_target.hitgroup = trace.hitgroup;
		final_target.field_of_view = field_of_view;
		final_target.point = point;
		final_target.player = player;
		final_target.data = data;

		max_priority = hitbox.priority;
	}
}

float LegitAim::get_smooth_value()
{
	if (!config->legit.weapon[ctx->weapon_config].smooth)
		return 0.0f;

	auto smooth_value = (float)config->legit.weapon[ctx->weapon_config].smooth_value * 3.0f;
	auto field_of_view = clamp(final_target.field_of_view, 0.0f, 30.0f);

	if (config->legit.weapon[ctx->weapon_config].smooth == 1)
		smooth_value *= field_of_view * 0.1f;
	else
		smooth_value *= pow(field_of_view, 2.0f) * 0.01f;

	return smooth_value;
}

bool LegitAim::is_hit_chanced(float hit_chance, const Vector& angle, int matrix, int hitbox_index, crypt_ptr <Player> player, crypt_ptr <AnimationData> data)
{
	if (convars_manager->convars[CONVAR_WEAPON_ACCURACY_NOSPREAD]->GetBool())
		return true;

	auto hits = 0;

	Vector forward, right, up;
	math::angle_vectors(angle, &forward, &right, &up);

	auto needed_hits = (int)(hit_chance * 2.56f);
	auto allowed_misses = 256 - needed_hits;

	for (auto i = 0; i < 256; ++i)
	{
		auto spread = get_spread(i);
		auto direction = (forward + right * spread.x + up * spread.y).Normalized();
		auto end = ctx->shoot_position + direction * ctx->weapon_data()->range;

		if (hitbox_intersection(hitbox_index, matrix, player, data, end))
			++hits;

		if (hits >= needed_hits)
			return true;

		if (i - hits > allowed_misses)
			return false;
	}

	return false;
}

Vector LegitAim::get_spread(int seed)
{
	static auto initialized = false;

	static float r[256][4];
	static float s[256][2];
	static float c[256][2];

	if (!initialized)
	{
		for (auto i = 0; i < 256; ++i)
		{
			math::random_seed(i + 1);

			r[i][0] = math::random_float(0.0f, 1.0f);
			r[i][1] = math::random_float(0.0f, DirectX::XM_2PI);
			r[i][2] = math::random_float(0.0f, 1.0f);
			r[i][3] = math::random_float(0.0f, DirectX::XM_2PI);

			DirectX::XMScalarSinCosEst(&s[i][0], &c[i][0], r[i][1]);
			DirectX::XMScalarSinCosEst(&s[i][1], &c[i][1], r[i][3]);
		}

		initialized = true;
	}

	auto r1 = r[seed][0];
	auto r2 = r[seed][2];

	if (ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_NEGEV && ctx->weapon()->m_flRecoilIndex() < 3.0f)
	{
		for (auto i = 3; (float)i > ctx->weapon()->m_flRecoilIndex(); --i)
		{
			r1 *= r1;
			r2 *= r2;
		}

		r1 = 1.0f - r1;
		r2 = 1.0f - r2;
	}

	return Vector(c[seed][0] * r1 * ctx->inaccuracy + c[seed][1] * r2 * ctx->spread, s[seed][0] * r1 * ctx->inaccuracy + s[seed][1] * r2 * ctx->spread, 0.0f);
}

#pragma optimize("", off)
bool LegitAim::clip_ray_to_hitbox(const Ray_t& ray, crypt_ptr <mstudiobbox_t> hitbox, matrix3x4_t& matrix, CGameTrace& trace)
{
	trace.fraction = 1.0f;
	trace.startsolid = false;

	return reinterpret_cast <int(__fastcall*)(const Ray_t&, mstudiobbox_t*, matrix3x4_t&, CGameTrace&)> (signatures_manager->signatures[SIGNATURE_CLIP_RAY_TO_HITBOX])(ray, hitbox.get(), matrix, trace) >= 0;
}
#pragma optimize("", on)

bool LegitAim::hitbox_intersection(int hitbox_index, int matrix, crypt_ptr <Player> player, crypt_ptr <AnimationData> data, const Vector& end) //-V813
{
	auto hitbox = player->get_hitbox(hitbox_index);

	if (!hitbox)
		return false;

	if (hitbox->radius > 0.0f)
	{
		Vector min;
		math::vector_transform(hitbox->bbmin, data->matrix[matrix][hitbox->bone], min);

		Vector max;
		math::vector_transform(hitbox->bbmax, data->matrix[matrix][hitbox->bone], max);

		return math::segment_to_segment(ctx->shoot_position, end, min, max) <= hitbox->radius;
	}
	else
	{
		CGameTrace trace;

		Ray_t ray;
		ray.Init(ctx->shoot_position, end);

		return clip_ray_to_hitbox(ray, hitbox, data->matrix[matrix][hitbox->bone], trace);
	}
}