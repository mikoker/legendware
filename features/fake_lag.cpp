// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "fake_lag.h"
#include "anti_aim.h"
#include "exploits.h"
#include "prediction.h"
#include "movement.h"
#include "aim.h"
#include "dormant.h"

void FakeLag::run(crypt_ptr <CUserCmd> cmd) //-V813
{
	if (config->anti_aim.fake_duck_key.state && engine_prediction->flags & FL_ONGROUND && ctx->local()->m_fFlags() & FL_ONGROUND && gamerules)
	{
		*ctx->send_packet.get() = gamerules->m_bIsValveDS() ? ctx->send_packet_fake_duck : clientstate->m_nChokedCommands >= 14;
		return;
	}

	if (!config->anti_aim.enable_fake_lag || ctx->freeze_period)
	{
		if (config->anti_aim.enable && config->anti_aim.desync_type)
			*ctx->send_packet.get() = clientstate->m_nChokedCommands;

		return;
	}

	auto speed = engine_prediction->velocity.Length2D();
	lag_limit = config->anti_aim.fake_lag_limit;

	if (speed > 0.0f)
		dynamic_choke = (int)ceil(64.0f / (speed * globals->intervalpertick));
	else
		dynamic_choke = INT_MAX;

	if (config->anti_aim.fake_lag_type == 1)
	{
		if (!(ctx->tickbase % 32))
			force_trigger_lag = true;

		if (dynamic_choke < lag_limit)
			lag_limit = dynamic_choke;
	}

	if (exploits->ticks_allowed)
		lag_limit = min(lag_limit, max(12 - exploits->ticks_allowed, 1));

	if (gamerules && gamerules->m_bIsValveDS())
		lag_limit = exploits->ticks_allowed ? 1 : min(lag_limit, 6);

	if (trigger_peek(cmd))
	{
		choke_trigger = true;
		peek = true;
		*ctx->send_packet.get() = true;
		return;
	}
	else if (!peek && lag_limit > 1 && !clientstate->m_nChokedCommands)
	{
		choke_trigger = trigger_lag();
		force_trigger_lag = false;
	}

	if (!choke_trigger)
		lag_limit = 1;

	*ctx->send_packet.get() = clientstate->m_nChokedCommands >= lag_limit;

	if (*ctx->send_packet.get())
	{
		choke_trigger = false;
		peek = false;
	}
}

void FakeLag::jitter_velocity(crypt_ptr <CUserCmd> cmd) //-V813
{
	if (!config->anti_aim.enable)
		return;

	velocity = FLT_MAX;

	if (ctx->local()->m_vecVelocity().Length2D() < 5.0f)
		return;

	auto value = (float)(cmd->command_number % 20000) * globals->intervalpertick * 5.0f;
	auto value_int = (uint32_t)value;

	auto final_value = abs((float)(value - (float)value_int) - 0.5f);

	velocity = (final_value + final_value) * 10.0f;
	movement_system->slow_walk(cmd, (ctx->max_speed - velocity) / ctx->max_speed);
}

bool FakeLag::trigger_lag()
{
	if (!config->anti_aim.fake_lag_type)
		return true;
	else if (force_trigger_lag)
		return true;
	else if (dynamic_choke <= config->anti_aim.fake_lag_limit)
		return true;
	else if ((ctx->local()->m_fFlags() & FL_ONGROUND) != (engine_prediction->flags & FL_ONGROUND))
		return true;
	else if (abs(ctx->local()->m_flDuckAmount() - ctx->animation_state()->duck_amount) > 0.0f)
		return true;
	else if (abs(ctx->local()->m_vecVelocity().Length() - engine_prediction->velocity.Length()) > 5.0f)
		return true;
	else if (config->anti_aim.fake_lag_type == 2 && velocity < 2.5f)
		return true;

	return false;
}

bool FakeLag::trigger_peek(crypt_ptr <CUserCmd> cmd)
{
	if (peek)
		return false;

	if (lag_limit <= 1)
		return false;

	if (aim->ticks_to_stop >= lag_limit)
		return false;

	auto predicted_shoot_position = ctx->shoot_position + ctx->local()->m_vecVelocity() * globals->intervalpertick * (float)(lag_limit - aim->ticks_to_stop);

	for (auto i = 1; i <= globals->maxclients; ++i)
	{
		auto player = crypt_ptr <Player> ((Player*)entitylist->GetClientEntity(i));

		if (player.get() == ctx->local().get())
			continue;

		if (!player->valid(!ctx->friendly_fire))
			continue;

		if (player->IsDormant())
		{
			auto backup_abs_origin = player->GetAbsOrigin();

			if (dormant->adjust_sound(player))
			{
				CGameTrace trace;
				CGameTrace trace_prediction;

				Ray_t ray;
				CTraceFilterWorldAndPropsOnly filter;

				ray.Init(ctx->shoot_position, player->GetAbsOrigin() + player->m_vecViewOffset());
				enginetrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);

				ray.Init(predicted_shoot_position, player->GetAbsOrigin() + player->m_vecViewOffset());
				enginetrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace_prediction);

				if (trace_prediction.fraction - trace.fraction > 0.5f)
					return true;
			}

			player->set_abs_origin(backup_abs_origin);
		}
		else
		{
			auto player_shoot_position = player->get_shoot_position();
			auto penetration_info = penetration->run(ctx->shoot_position, player_shoot_position, player, true);

			if (penetration_info.damage >= 1)
				continue;

			auto predicted_penetration_info = penetration->run(predicted_shoot_position, player_shoot_position, player, true);

			if (predicted_penetration_info.damage < 1)
				continue;

			return true;
		}
	}

	Vector view_angles;
	engine->GetViewAngles(view_angles);

	view_angles.x = 0.0f;

	Vector forward;
	math::angle_vectors(view_angles, &forward, nullptr, nullptr);

	forward.z = 0.0f;

	auto end = ctx->shoot_position + forward * 200.0f;
	auto end_prediction = predicted_shoot_position + forward * 200.0f;

	CGameTrace trace;
	CGameTrace trace_prediction;

	Ray_t ray;
	CTraceFilterWorldAndPropsOnly filter;

	ray.Init(ctx->shoot_position, end);
	enginetrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);

	ray.Init(predicted_shoot_position, end_prediction);
	enginetrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace_prediction);

	return trace_prediction.fraction - trace.fraction > 0.5f;
}