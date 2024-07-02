// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "knife_aim.h"
#include "logs.h"
#include "exploits.h"

void KnifeAim::run(crypt_ptr <CUserCmd> cmd)
{
	if (ctx->weapon_config != WEAPON_CONFIG_KNIFE)
		return;

	backup.clear();
	targets.clear();
	final_target.reset();

	prepare();
	scan();
	fire(cmd);

	for (auto& data : backup)
		data.apply(MATRIX_MAIN, true);
}

void KnifeAim::prepare()
{
	for (auto i = 1; i <= globals->maxclients; ++i)
	{
		auto player = crypt_ptr <Player> ((Player*)entitylist->GetClientEntity(i));

		if (player.get() == ctx->local().get())
			continue;

		if (!player->valid(!ctx->friendly_fire))
			continue;

		if (animations->animation_data[i].empty())
			continue;

		if (config->player_list.player_settings[i].ignore)
			continue;

		targets.emplace_back(PreparedTarget(i, player));
	}
}

void KnifeAim::scan()
{
	auto best_distance = FLT_MAX;

	for (auto& target : targets)
	{
		crypt_ptr <AnimationData> data;

		for (auto i = 0; i < target.data->size(); ++i)
		{
			auto current_data = crypt_ptr <AnimationData> (&target.data->at(i));

			if (!current_data->valid())
				continue;

			if (current_data->old_simulation_time <= 0.0f)
				continue;

			if (current_data->old_simulation_time > current_data->simulation_time)
				continue;

			auto distance = current_data->origin.DistTo(ctx->shoot_position);

			if (distance < best_distance)
			{
				best_distance = distance;
				data = current_data;
			}
		}

		if (!data)
			continue;

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

		final_target.player = target.player;
		final_target.data = data;
	}
}

void KnifeAim::fire(crypt_ptr <CUserCmd> cmd)
{
	if (!final_target.data)
		return;

	if (!ctx->local()->can_shoot())
		return;

	if (!config->rage.automatic_fire && !(cmd->buttons & IN_ATTACK) && !(cmd->buttons & IN_ATTACK2))
		return;

	final_target.data->apply();

	auto mins = final_target.data->origin + final_target.data->mins;
	auto maxs = final_target.data->origin + final_target.data->maxs;

	auto shoot_position = final_target.player->get_shoot_position();

	if (mins < shoot_position)
		mins = shoot_position;

	if (mins > maxs)
		mins = maxs;

	auto delta = mins - ctx->shoot_position;

	if (delta.Length() > 75.0f)
		return;

	auto fired = false;
	auto angle_delta = abs(math::normalize_yaw(final_target.data->angles.y - math::calculate_angle(shoot_position, ctx->shoot_position).y));

	if (angle_delta < 120.0f && (delta.Length() > 60.0f || final_target.player->m_iHealth() > 46))
	{
		if (!(cmd->buttons & IN_ATTACK) && !(cmd->buttons & IN_ATTACK2))
			cmd->buttons |= IN_ATTACK;

		cmd->viewangles = delta.ToEulerAngles();
		cmd->tickcount = TIME_TO_TICKS(final_target.data->simulation_time + ctx->interpolation);

		if (!config->rage.silent)
			engine->SetViewAngles(cmd->viewangles);

		fired = true;
	}
	else if (delta.Length() < 60.0f)
	{
		auto end = ctx->shoot_position + delta * 32.0f;

		CTraceFilter filter;
		filter.pSkip = ctx->local().get();

		CGameTrace trace;
		Ray_t ray;

		ray.Init(ctx->shoot_position, end, Vector(-16.0f, -16.0f, -18.0f), Vector(16.0f, 16.0f, 18.0f));
		enginetrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

		if (trace.hit_entity == final_target.player.get())
		{
			if (!(cmd->buttons & IN_ATTACK) && !(cmd->buttons & IN_ATTACK2))
				cmd->buttons |= IN_ATTACK2;

			cmd->viewangles = delta.ToEulerAngles();
			cmd->tickcount = TIME_TO_TICKS(final_target.data->simulation_time + ctx->interpolation);

			if (!config->rage.silent)
				engine->SetViewAngles(cmd->viewangles);

			fired = true;
		}
	}

#if BETA
	if (fired)
	{
		auto original_tickbase = ctx->tickbase;

		if (exploits->hide_shots || exploits->double_tap)
			original_tickbase += exploits->target_tickbase_shift;

		auto correct = clamp(TICKS_TO_TIME(engine_prediction->latency) + ctx->interpolation, 0.0f, convars_manager->convars[CONVAR_SV_MAXUNLAG]->GetFloat());
		auto delta_time = correct - (TICKS_TO_TIME(original_tickbase) - final_target.data->simulation_time);
		auto backtrack_ticks = TIME_TO_TICKS(abs(delta_time));

		string log;

		player_info_t player_info;
		engine->GetPlayerInfo(final_target.data->i, &player_info);

		log += crypt_str("Fired shot at ") + (string)player_info.szName;
		log += crypt_str(", backtrack: ") + to_string(backtrack_ticks);
		log += crypt_str(", choke: ") + to_string(TIME_TO_TICKS(final_target.data->simulation_time - final_target.data->old_simulation_time));

		logs->add(log, Color::LightBlue, crypt_str("[ SHOT ] "));
	}
#endif
}