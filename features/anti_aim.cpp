// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "anti_aim.h"
#include "penetration.h"
#include "prediction.h"
#include "movement.h"
#include "features.h"
#include "local_animations.h"
#include <cmath>

void AntiAim::run(crypt_ptr <CUserCmd> cmd)
{
	if (!config->anti_aim.enable)
		return;

	if (!ctx->weapon())
		return;

	if (!ctx->animation_state())
		return;

	if (ctx->freeze_period)
		return;

	if (ctx->local()->m_fFlags() & FL_FROZEN) //-V807
		return;

	if (ctx->local()->m_moveType() == MOVETYPE_NOCLIP)
		return;

	if (ctx->local()->m_moveType() == MOVETYPE_LADDER)
		return;

	if (ctx->weapon()->executed(cmd, true))
		return;

	if (cmd->buttons & IN_USE)
	{
		if (ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_C4)
		{
			auto bomb = crypt_ptr <Bomb>((Bomb*)ctx->weapon().get());

			if (!bomb->m_bStartedArming())
				return;
		}
		else
			return;
	}

	edge_yaw = false;

	cmd->viewangles.x = get_pitch(cmd);
	cmd->viewangles.y = get_yaw(cmd);

	if (config->anti_aim.desync_type == 3 && !force_disable_desync && globals->realtime - flick_time > TICKS_TO_TIME(17) && *ctx->send_packet.get())
	{
		flicked = true;
		flick_time = globals->realtime;

		cmd->viewangles.y += inverted_desync ? -90.0f : 90.0f;
	}

	auto valve_server = gamerules && gamerules->m_bIsValveDS();

	if (valve_server && config->misc.anti_untrusted)
		return;

	cmd->viewangles.z = get_roll(cmd);
}

float AntiAim::get_roll(crypt_ptr <CUserCmd> cmd)
{
	if (!config->anti_aim.roll)
		return 0.0f;

	if (!config->anti_aim.pitch)
		return 0.0f;

	if (!*ctx->send_packet.get())
		return 0.0f;

	if (config->anti_aim.disable_on_high_velocity && engine_prediction->velocity.Length2D() > ctx->max_speed * 0.5f && engine_prediction->flags & FL_ONGROUND && ctx->local()->m_fFlags() & FL_ONGROUND)
		return 0.0f;

	auto roll = (float)config->anti_aim.roll_range;

	if (inverted_desync)
		roll = -roll;

	if (roll_opposite)
		roll = -roll;

	return roll;
}

float AntiAim::get_pitch(crypt_ptr <CUserCmd> cmd)
{
	if (!config->anti_aim.pitch)
		return cmd->viewangles.x;

	return clamp(config->anti_aim.pitch == 1 ? ctx->animation_state()->pitch_max : ctx->animation_state()->pitch_min, -89.0f, 89.0f);
}

float AntiAim::get_yaw(crypt_ptr <CUserCmd> cmd) //-V813
{
	auto target_yaw = get_target_yaw(cmd);
	auto yaw_modifier = 0.0f;
	auto yaw_modifier_range = edge_yaw && config->anti_aim.freestanding && config->anti_aim.freestanding_key.state && config->anti_aim.freestanding_yaw_modifier ? (float)config->anti_aim.freestanding_yaw_modifier_range * 0.5f : (float)config->anti_aim.yaw_modifier_range * 0.5f;

	if (edge_yaw && config->anti_aim.freestanding && config->anti_aim.freestanding_key.state && config->anti_aim.freestanding_yaw_modifier ? config->anti_aim.freestanding_yaw_modifier == 2 : config->anti_aim.yaw_modifier == 1)
	{
		yaw_modifier = yaw_modifier_range;

		if (inverted)
			yaw_modifier = -yaw_modifier;
	}
	else if (edge_yaw && config->anti_aim.freestanding && config->anti_aim.freestanding_key.state && config->anti_aim.freestanding_yaw_modifier ? config->anti_aim.freestanding_yaw_modifier == 3 : config->anti_aim.yaw_modifier == 2)
	{
		if (spin_offset <= -yaw_modifier_range || spin_offset >= yaw_modifier_range)
			switch_spin = !switch_spin;

		if (switch_spin)
			spin_offset -= yaw_modifier_range * 0.25f;
		else
			spin_offset += yaw_modifier_range * 0.25f;

		spin_offset = clamp(spin_offset, -yaw_modifier_range, yaw_modifier_range);
		yaw_modifier = spin_offset;
	}
	else if (edge_yaw && config->anti_aim.freestanding && config->anti_aim.freestanding_key.state && config->anti_aim.freestanding_yaw_modifier ? config->anti_aim.freestanding_yaw_modifier == 4 : config->anti_aim.yaw_modifier == 3)
	{
		if (desync_random_side % 3 == 0)
			yaw_modifier = -yaw_modifier_range;
		else if (desync_random_side % 3 == 2)
			yaw_modifier = yaw_modifier_range;
	}

	if (*ctx->send_packet.get())
		++desync_random_side;

	auto offset = (float)config->anti_aim.yaw_offset;

	if (config->anti_aim.manual_forward_key.state)
		offset = 180.0f;
	else if (config->anti_aim.manual_back_key.state)
		offset = 0.0f;
	else if (config->anti_aim.manual_left_key.state)
		offset = -90.0f;
	else if (config->anti_aim.manual_right_key.state)
		offset = 90.0f;

	auto yaw = target_yaw + offset + yaw_modifier;
	auto yaw_delta = abs(math::normalize_yaw(target_yaw - yaw));

	roll_opposite = yaw_delta > 45.0f && yaw_delta < 135.0f;

	if (!config->anti_aim.desync_type || force_disable_desync)
		return yaw;

	auto original_desync_delta = ctx->animation_state()->get_desync_delta();
	auto desync_delta = inverted_desync ? min(original_desync_delta, (float)config->anti_aim.desync_range_inverted) : min(original_desync_delta, (float)config->anti_aim.desync_range);

	if (*ctx->send_packet.get())
	{
		inverted = !inverted;
		inverted_choke_cycle = false;
	}
	else if (!inverted_choke_cycle)
	{
		auto overlap_delta = abs(math::normalize_yaw(ctx->animation_state()->goal_feet_yaw - local_animations->fake_animstate.goal_feet_yaw));
		auto should_invert = false;

		if (config->anti_aim.desync_type == 1)
		{
			if (config->anti_aim.invert_desync_key.valid())
				inverted_desync = config->anti_aim.invert_desync_key.state;
			else if (overlap_delta < desync_delta * 0.9f)
				should_invert = true;
		}
		else if (config->anti_aim.desync_type == 2)
		{
			if (edge_yaw && config->anti_aim.freestanding && config->anti_aim.freestanding_key.state && config->anti_aim.freestanding_yaw_modifier ? config->anti_aim.freestanding_yaw_modifier != 2 : config->anti_aim.yaw_modifier != 1)
				should_invert = true;
			else if (overlap_delta > desync_delta * 0.9f)
				should_invert = true;
		}
		else if (config->anti_aim.desync_type == 3 && config->anti_aim.invert_desync_key.valid())
			inverted_desync = config->anti_aim.invert_desync_key.state;

		if (should_invert)
		{
			inverted_desync = !inverted_desync;
			inverted_choke_cycle = true;
		}
	}

	auto inv_body_lean = desync_delta;
	auto view_yaw = yaw;
	float leaned_yaw = 0.f;
	float abs_lean = 0.f;
	float j = 0.f;
	float k = 0.f;

	float v8, chto_blyad, i;
	bool v12;

	if (!inverted_desync)
		inv_body_lean *= -1.0f;

	for (leaned_yaw = inv_body_lean + yaw; leaned_yaw > 180.0; leaned_yaw = leaned_yaw - 360.0)
		;
	for (; leaned_yaw < -180.0; leaned_yaw = leaned_yaw + 360.0)
		;
	v8 = ctx->animation_state()->goal_feet_yaw;
	if (v8 > 180.0)
	{
		do
			v8 = v8 - 360.0;
		while (v8 > 180.0);
	}
	for (; v8 < -180.0; v8 = v8 + 360.0)
		;
	for (abs_lean = v8 - leaned_yaw; abs_lean > 180.0; abs_lean = abs_lean - 360.0)
		;
	for (; abs_lean < -180.0; abs_lean = abs_lean + 360.0)
		;
	chto_blyad = *(float*)(uintptr_t(globals.get()) + 0x20) * 102.0f;
	if (abs(abs_lean) <= chto_blyad && desync_delta < original_desync_delta)
		goto LABEL_57;
	if (abs(abs_lean) <= (180.0f - (original_desync_delta + chto_blyad)))
	{
		if (abs_lean <= 0.0)
			view_yaw = original_desync_delta + leaned_yaw;
		else
			view_yaw = leaned_yaw - original_desync_delta;
	}
	else
	{
		view_yaw = leaned_yaw;
	}
	v12 = 1;
	i = ctx->animation_state()->goal_feet_yaw;
	if (desync_delta >= original_desync_delta)
	{
		for (j = yaw; j > 180.0; j = j - 360.0)
			;
		for (; j < -180.0; j = j + 360.0)
			;
		for (; i > 180.0; i = i - 360.0)
			;
		for (; i < -180.0; i = i + 360.0)
			;
		for (k = i - j; k > 180.0; k = k - 360.0)
			;
		for (; k < -180.0; k = k + 360.0)
			;
		if (inverted_desync)
		{
			if (k < desync_delta || k >= 179.0)
				goto LABEL_46;
		}
		else if ((desync_delta * -1.f) < k || k <= -179.0f)
		{
			goto LABEL_50;
		}
		v12 = 0;
		if (*ctx->send_packet.get())
			goto LABEL_59;

	LABEL_46:
		if (inverted_desync)
		{
			if (k > 0.0 && k < 179.0)
				view_yaw = yaw + 120.0f;
			goto LABEL_53;
		}

	LABEL_50:
		if (k < 0.0 && k > -179.0)
			view_yaw = yaw - 120.0f;

	LABEL_53:
		if (!v12)
			goto LABEL_57;
	}

LABEL_57:
	if (!*ctx->send_packet.get())
		goto LABEL_63;

LABEL_59:
	view_yaw = yaw;

LABEL_63:
	yaw = view_yaw;
	return yaw;
}

float AntiAim::get_target_yaw(crypt_ptr <CUserCmd> cmd) //-V813
{
	auto target_angle = cmd->viewangles.y;

	if (config->anti_aim.manual_forward_key.state || config->anti_aim.manual_back_key.state || config->anti_aim.manual_left_key.state || config->anti_aim.manual_right_key.state)
		return target_angle + 180.0f;

	auto edge_yaw = get_edge_yaw(cmd);

	if (edge_yaw != FLT_MAX)
		return edge_yaw;

	if (!config->anti_aim.target_yaw)
		return target_angle + 180.0f;

	auto best_field_of_view = FLT_MAX;

	for (auto i = 1; i <= globals->maxclients; ++i)
	{
		if (config->player_list.player_settings[i].ignore)
			continue;

		auto player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(i));

		if (player.get() == ctx->local().get())
			continue;

		if (!player->valid(!ctx->friendly_fire))
			continue;

		Vector angles;
		engine->GetViewAngles(angles);

		auto current_target_angle = math::calculate_angle(ctx->shoot_position, player->m_vecOrigin());
		auto field_of_view = math::get_field_of_view(angles, current_target_angle);

		if (field_of_view < best_field_of_view)
		{
			target_angle = current_target_angle.y;
			best_field_of_view = field_of_view;
		}
	}

	return target_angle + 180.0f;
}

float AntiAim::get_edge_yaw(crypt_ptr <CUserCmd> cmd)
{
	if (!config->anti_aim.freestanding || !config->anti_aim.freestanding_key.state)
		return FLT_MAX;

	std::vector <HeadPosition> head_positions;
	std::vector <crypt_ptr <Player>> players;

	for (auto i = 1; i <= globals->maxclients; ++i)
	{
		if (config->player_list.player_settings[i].ignore)
			continue;

		auto player = crypt_ptr <Player> ((Player*)entitylist->GetClientEntity(i));

		if (player.get() == ctx->local().get())
			continue;

		if (!player->valid(!ctx->friendly_fire))
			continue;

		players.emplace_back(player);
	}

	if (players.empty())
		return FLT_MAX;

	Vector angles;
	engine->GetViewAngles(angles);

	std::sort(players.begin(), players.end(),
		[&](crypt_ptr <Player>& first, crypt_ptr <Player>& second)
		{
			auto first_angle = math::calculate_angle(ctx->shoot_position, first->get_shoot_position());
			auto first_field_of_view = math::get_field_of_view(angles, first_angle);

			auto second_angle = math::calculate_angle(ctx->shoot_position, second->get_shoot_position());
			auto second_field_of_view = math::get_field_of_view(angles, second_angle);

			return first_field_of_view < second_field_of_view;
		}
	);

	for (auto& player : players)
	{
		auto player_shoot_position = player->get_shoot_position();
		auto player_angle = math::calculate_angle(ctx->shoot_position, player_shoot_position);

		auto angle = player_angle.y + 90.0f;
		auto view_yaw_radians = DirectX::XMConvertToRadians(angle);

		auto sin_angle = 0.0f;
		auto cos_angle = 0.0f;

		DirectX::XMScalarSinCosEst(&sin_angle, &cos_angle, view_yaw_radians);
		head_positions.emplace_back(angle, 0.0f, Vector(20.0f * cos_angle + ctx->shoot_position.x, 20.0f * sin_angle + ctx->shoot_position.y, ctx->shoot_position.z), Vector());

		angle = player_angle.y - 90.0f;
		view_yaw_radians = DirectX::XMConvertToRadians(angle);

		DirectX::XMScalarSinCosEst(&sin_angle, &cos_angle, view_yaw_radians);
		head_positions.emplace_back(angle, 0.0f, Vector(20.0f * cos_angle + ctx->shoot_position.x, 20.0f * sin_angle + ctx->shoot_position.y, ctx->shoot_position.z), Vector());

		for (auto& head_position : head_positions)
		{
			Ray_t ray;
			ray.Init(head_position.position, player_shoot_position);

			CTraceFilterWorldAndPropsOnly filter;
			CGameTrace trace;

			enginetrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);

			head_position.fraction = trace.fraction;
			head_position.end_position = trace.endpos;
		}

		if (ctx->shoot_position.DistTo(head_positions.at(0).end_position) < 100.0f || ctx->shoot_position.DistTo(head_positions.at(1).end_position) < 100.0f)
		{
			if (head_positions.at(0).fraction + 0.5f < head_positions.at(1).fraction)
			{
				edge_yaw = true;
				return head_positions.at(0).angle;
			}
			else if (head_positions.at(1).fraction + 0.5f < head_positions.at(0).fraction)
			{
				edge_yaw = true;
				return head_positions.at(1).angle;
			}
		}
	}

	return FLT_MAX;
}

void AntiAim::micro_movements(crypt_ptr <CUserCmd> cmd)
{
	if (!config->anti_aim.enable)
		return;

	if (!config->anti_aim.desync_type || force_disable_desync)
		return;

	if (ctx->freeze_period)
		return;

	if (ctx->local()->m_bGunGameImmunity() || ctx->local()->m_fFlags() & FL_FROZEN)
		return;

	if (ctx->weapon_data() && ctx->weapon()->is_grenade() && (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2))
		return;

	auto movement_speed = sqrt(cmd->forwardmove * cmd->forwardmove + cmd->sidemove * cmd->sidemove);

	if (movement_speed > 5.0f)
		return;

	if (ctx->local()->m_vecVelocity().Length() > 5.0f)
		return;

	cmd->sidemove += switch_move ? 1.01f : -1.01f;

	if (ctx->local()->m_flDuckAmount() > 0.0f)
		cmd->sidemove /= 0.34f;

	switch_move = !switch_move;
}