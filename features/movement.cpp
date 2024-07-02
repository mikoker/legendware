// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "movement.h"
#include "features.h"
#include "prediction.h"
#include "..\config\config.h"
#include "aim.h"
#include "knife_aim.h"
#include "anti_aim.h"
#include "local_animations.h"
#include "exploits.h"
#include "fake_lag.h"

void Movement::run(crypt_ptr <CUserCmd> cmd)
{
	ctx->original_forwardmove = cmd->forwardmove;
	ctx->original_sidemove = cmd->sidemove;

	wish_angle = math::normalize_angles(cmd->viewangles);
	strafe_angle = wish_angle;

	if (!ctx->local()->valid())
		return;

	infinite_duck(cmd);
	fake_duck(cmd);

	static auto previous_flags = engine_prediction->flags;

	if (engine_prediction->flags & FL_ONGROUND)
	{
		auto automatic_peeking = ctx->automatic_peek && movement_system->holding_automatic_peek;

		if (config->anti_aim.slow_walk_key.state && !automatic_peeking)
			slow_walk(cmd);
		else if (config->misc.fast_stop)
			fast_stop(cmd);

		if (config->anti_aim.enable && engine_prediction->move_type == MOVETYPE_WALK)
		{
			fake_lag->jitter_velocity(cmd);
			anti_aim->micro_movements(cmd);
		}
	}
	else
	{
		if (engine_prediction->move_type == MOVETYPE_NOCLIP || engine_prediction->move_type == MOVETYPE_LADDER)
			return;

		automatic_jump(cmd);
	}

	if (!(ctx->local()->m_fFlags() & FL_ONGROUND))
		automatic_strafe(cmd);

	automatic_peek(cmd, previous_flags & FL_ONGROUND);
	previous_flags = engine_prediction->flags;
}

void Movement::automatic_jump(crypt_ptr <CUserCmd> cmd)
{
	if (!config->misc.automatic_jump)
		return;

	cmd->buttons &= ~IN_JUMP;
}

void Movement::automatic_strafe(crypt_ptr <CUserCmd> cmd)
{
	if (!config->misc.automatic_strafe || cmd->buttons & IN_SPEED)
		return;

	static float strafe_side = 1;
	auto min_estktard = [](float a1, float a2) -> float
	{
		if (a1 < 0.0f == a2 < 0.0f)
			return fminf(a1, a2);

		float result = a1;
		if (a1 >= 0.0f) result = a2;
		return result;
	};

	auto velocity = ctx->local()->m_vecVelocity();
	velocity.z = 0;

	const auto speed = velocity.Length2D();
	const auto ideal_strafe = min_estktard(90.f, 845.f / (speed + FLT_EPSILON));

	const auto on_ground = ctx->local()->m_fFlags() & FL_ONGROUND;
	const auto side_speed = convars_manager->convars[CONVAR_CL_SIDESPEED]->GetFloat();
	auto forwardmove = cmd->forwardmove;
	auto sidemove = cmd->sidemove;

	if (abs(forwardmove) < 2)
		forwardmove = 0;

	if (abs(sidemove) < 2)
		sidemove = 0;

	auto using_wasd = false;

	strafe_angle = wish_angle;

	Vector right;
	Vector forward;
	math::angle_vectors(strafe_angle, &forward, &right, nullptr);

	if (forward[2] != 0)
	{
		forward[2] = 0;
		forward.Normalize();
	}

	if (right[2] != 0)
	{
		right[2] = 0;
		right.Normalize();
	}

	Vector wishdir;
	for (int i = 0; i < 2; i++)       // Determine x and y parts of velocity
		wishdir[i] = forward[i] * forwardmove + right[i] * sidemove;

	wishdir.Normalize();

	auto wishang = wishdir.ToEulerAngles();
	float velocity_yaw = DirectX::XMConvertToDegrees(atan2(velocity.y, velocity.x));

	velocity_yaw = math::normalize_yaw(velocity_yaw);

	if ((forwardmove != 0.0f || sidemove != 0.0f) && config->misc.automatic_strafe == 2)
	{
		cmd->forwardmove = 0.0f;
		cmd->sidemove = 0.0f;
		strafe_angle.y = math::normalize_yaw(wishang.y);
  		using_wasd = true;
	}
	else if (forwardmove > 0.f)
		cmd->forwardmove = 0.f;

	if (speed < 2.0f)
		cmd->forwardmove = side_speed;

	const auto velocity_delta = math::normalize_yaw(strafe_angle.y - velocity_yaw);
	const auto retrack = float(100 - config->misc.smoothing) * 0.02f * (ideal_strafe + ideal_strafe);

	//smooth strafe on high delta
	if (using_wasd && abs(velocity_delta) > 170.f && speed > 80.f || velocity_delta > retrack && speed > 80.f)
	{
		strafe_angle.y = retrack + velocity_yaw;
		cmd->sidemove = -side_speed;
	}
	else
	{
		if (-retrack <= velocity_delta || speed <= 80.0f)
		{
			strafe_angle.y += (ideal_strafe * strafe_side);
			cmd->sidemove = side_speed * strafe_side;
		}
		else
		{
			strafe_angle.y = velocity_yaw - retrack;
			cmd->sidemove = side_speed;
		}
	}

	strafe_side *= -1;

	fix_movement(cmd, strafe_angle);
}

void Movement::infinite_duck(crypt_ptr <CUserCmd> cmd)
{
	if (!config->misc.infinite_duck)
		return;

	if (gamerules && gamerules->m_bIsValveDS() && config->misc.anti_untrusted)
		return;

	cmd->buttons |= IN_BULLRUSH;
}

void Movement::edge_jump(crypt_ptr <CUserCmd> cmd)
{
	if (!config->misc.edge_jump_key.state)
		return;

	if (!(engine_prediction->flags & FL_ONGROUND))
		return;

	if (ctx->local()->m_fFlags() & FL_ONGROUND)
		return;

	cmd->buttons |= IN_JUMP;
}

void Movement::fake_duck(crypt_ptr <CUserCmd> cmd)
{
	if (!ctx->fake_ducking && (ctx->local()->m_flDuckAmount() <= 0.0f || ctx->local()->m_flDuckAmount() >= 1.0f))
		ctx->visual_fake_ducking = false;

	if (!config->anti_aim.fake_duck_key.state)
	{
		ctx->fake_ducking = false;
		return;
	}

	if (!(engine_prediction->flags & FL_ONGROUND))
	{
		ctx->fake_ducking = false;
		return;
	}

	if (gamerules && gamerules->m_bIsValveDS())
	{
		if (config->misc.anti_untrusted)
		{
			ctx->send_packet_fake_duck = true;
			return;
		}

		if (!ctx->fake_ducking && clientstate->m_nChokedCommands)
		{
			ctx->send_packet_fake_duck = true;
			return;
		}

		static auto switch_duck = true;

		if (switch_duck)
		{
			cmd->buttons |= IN_DUCK;

			if (ctx->local()->m_flDuckAmount() > 0.42f)
			{
				switch_duck = false;
				ctx->send_packet_fake_duck = true;
			}
		}
		else
		{
			cmd->buttons &= ~IN_DUCK;

			if (ctx->local()->m_flDuckAmount() < 0.36f)
				switch_duck = true;
		}
	}
	else
	{
		if (!ctx->fake_ducking && clientstate->m_nChokedCommands < 7)
			return;

		auto duck = clientstate->m_nChokedCommands >= 7;

		if (duck)
			cmd->buttons |= IN_DUCK;
		else
			cmd->buttons &= ~IN_DUCK;
	}

	cmd->buttons |= IN_BULLRUSH;

	ctx->fake_ducking = true;
	ctx->visual_fake_ducking = true;
}

void Movement::slow_walk(crypt_ptr <CUserCmd> cmd, float speed)
{
	if (cmd->buttons & IN_JUMP)
		return;

	if (!(cmd->buttons & IN_FORWARD) && !(cmd->buttons & IN_MOVELEFT) && !(cmd->buttons & IN_BACK) && !(cmd->buttons & IN_MOVERIGHT))
		return;

	Vector forward, right;
	math::angle_vectors(cmd->viewangles, &forward, &right, nullptr);

	forward.NormalizeInPlace();
	right.NormalizeInPlace();

	Vector wish_velocity;

	for (auto i = 0; i < 2; ++i)
		wish_velocity[i] = forward[i] * cmd->forwardmove + right[i] * cmd->sidemove;

	wish_velocity.z = 0.0f;

	auto wish_direction = wish_velocity;
	auto wish_speed = wish_direction.Normalize();

	auto max_unclamped_speed = min(ctx->max_speed, ctx->local()->m_flMaxspeed());

	if (wish_speed > 0.0f && wish_speed > max_unclamped_speed)
	{
		wish_velocity *= max_unclamped_speed / wish_speed;
		wish_speed = max_unclamped_speed;
	}

	auto predicted_velocity = engine_prediction->velocity;

	predicted_velocity.z = 0.0f;
	accelerate(cmd, &predicted_velocity, wish_speed, wish_direction, max_unclamped_speed);

	auto cmd_speed = sqrt(cmd->sidemove * cmd->sidemove + cmd->forwardmove * cmd->forwardmove);

	auto forward_scale = cmd->forwardmove / cmd_speed;
	auto side_scale = cmd->sidemove / cmd_speed;

	auto predicted_speed = predicted_velocity.Length2D();
	auto max_speed = ctx->max_speed * speed;

	if (predicted_speed > max_speed)
	{
		cmd->forwardmove = forward_scale * max_speed;
		cmd->sidemove = side_scale * max_speed;
	}
}

void Movement::automatic_peek(crypt_ptr <CUserCmd> cmd, bool previous_on_ground)
{
	if (!config->misc.automatic_peek_key.valid())
		return;

	if (config->misc.automatic_peek_key.state && !ctx->automatic_peek_position.IsZero())
	{
		if (!ctx->automatic_peek)
			ctx->automatic_peek_radius = 0.0f;
	
		ctx->automatic_peek = true;
	}
	else
	{
		holding_automatic_peek = false;
		automatic_peek_shot = false;

		ctx->automatic_peek = false;
	}

	if (ctx->automatic_peek_radius == 0.0f)
		ctx->automatic_peek_position = ctx->local()->m_vecOrigin();

	if (!(engine_prediction->flags & FL_ONGROUND))
	{
		Ray_t ray;
		CTraceFilterWorldAndPropsOnly filter;
		CGameTrace trace;

		ray.Init(ctx->automatic_peek_position, ctx->automatic_peek_position - Vector(0.0f, 0.0f, 8192.0f));
		enginetrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

		if (trace.fraction < 1.0f)
			ctx->automatic_peek_position = trace.endpos + Vector(0.0f, 0.0f, 2.0f);
	}

	if (!ctx->automatic_peek)
		return;

	if (!holding_automatic_peek)
		return;

	if (!previous_on_ground)
		return;

	if (!(engine_prediction->flags & FL_ONGROUND))
		return;

	if (!automatic_peek_shot && config->misc.automatic_peek[AUTOMATIC_PEEK_KEY] && (cmd->buttons & IN_FORWARD || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVERIGHT || cmd->buttons & IN_JUMP))
	{
		holding_automatic_peek = false;
		return;
	}

	auto delta = ctx->automatic_peek_position - ctx->local()->m_vecOrigin();

	if (delta.Length2D() < 10.0f)
	{
		holding_automatic_peek = false;
		automatic_peek_shot = false;
		return;
	}

	wish_angle = delta.ToEulerAngles();

	cmd->forwardmove = convars_manager->convars[CONVAR_CL_FORWARDSPEED]->GetFloat();
	cmd->sidemove = 0.0f;
}

void Movement::fast_stop(crypt_ptr <CUserCmd> cmd)
{
	if (!(engine_prediction->flags & FL_ONGROUND))
		return;

	if (cmd->buttons & IN_FORWARD || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVERIGHT || cmd->buttons & IN_JUMP)
		return;

	if (engine_prediction->velocity.Length2D() < 20.0f)
		return;

	Vector angle;
	math::vector_angles(engine_prediction->velocity * -1.0f, angle);

	angle.y = math::normalize_yaw(movement_system->wish_angle.y - angle.y);

	Vector direction;
	math::angle_vectors(angle, &direction, nullptr, nullptr);

	auto stop = direction * engine_prediction->velocity.Length2D(); //-V688

	cmd->forwardmove = stop.x;
	cmd->sidemove = stop.y;
}

void Movement::fix_movement(crypt_ptr <CUserCmd> cmd, const Vector& wish_angle, bool roll) //-V688
{
	cmd->viewangles = math::normalize_angles(cmd->viewangles);

	if (wish_angle == cmd->viewangles)
		return;

	if (!cmd->forwardmove && !cmd->sidemove)
		return;

	Vector pure_forward, pure_right, pure_up;
	math::angle_vectors(wish_angle, &pure_forward, &pure_right, &pure_up);

	Vector current_forward, current_right, current_up;
	math::angle_vectors(cmd->viewangles, &current_forward, &current_right, &current_up);

	pure_forward.z = 0.0f;
	pure_right.z = 0.0f;

	current_forward.z = 0.0f;
	current_right.z = 0.0f;

	pure_forward.Normalize();
	pure_right.Normalize();

	current_forward.Normalize();
	current_right.Normalize();

	Vector pure_wish_direction;

	for (auto i = 0; i < 2; ++i)
		pure_wish_direction[i] = pure_forward[i] * cmd->forwardmove + pure_right[i] * cmd->sidemove;

	Vector current_wish_direction;

	for (auto i = 0; i < 2; ++i)
		current_wish_direction[i] = current_forward[i] * cmd->forwardmove + current_right[i] * cmd->sidemove;

	if (pure_wish_direction != current_wish_direction)
	{
		cmd->forwardmove = (pure_wish_direction.x * current_right.y - current_right.x * pure_wish_direction.y) / (current_right.y * current_forward.x - current_right.x * current_forward.y);
		cmd->sidemove = (pure_wish_direction.y * current_forward.x - current_forward.y * pure_wish_direction.x) / (current_right.y * current_forward.x - current_right.x * current_forward.y);
	}

	if (config->anti_aim.leg_movement && ctx->local()->m_moveType() != MOVETYPE_LADDER)
	{
		cmd->buttons &= ~IN_FORWARD;
		cmd->buttons &= ~IN_BACK;
		cmd->buttons &= ~IN_MOVELEFT;
		cmd->buttons &= ~IN_MOVERIGHT;

		if (config->anti_aim.leg_movement == 1)
		{
			if (cmd->forwardmove)
				cmd->buttons |= cmd->forwardmove > 0.0f ? IN_FORWARD : IN_BACK;

			if (cmd->sidemove)
				cmd->buttons |= cmd->sidemove > 0.0f ? IN_MOVERIGHT : IN_MOVELEFT;
		}
		else
		{
			if (cmd->forwardmove)
				cmd->buttons |= cmd->forwardmove > 0.0f ? IN_BACK : IN_FORWARD;

			if (cmd->sidemove)
				cmd->buttons |= cmd->sidemove > 0.0f ? IN_MOVELEFT : IN_MOVERIGHT;
		}
	}
}

void Movement::friction(crypt_ptr <Vector> velocity)
{
	auto speed = velocity->Length2D();

	if (speed <= 0.1f)
		return;

	auto drop = 0.0f;

	if (ctx->local()->m_hGroundEntity())
	{
		auto friction = convars_manager->convars[CONVAR_SV_FRICTION]->GetFloat() * ctx->local()->m_surfaceFriction();
		auto control = max(convars_manager->convars[CONVAR_SV_STOPSPEED]->GetFloat(), speed);

		drop = friction * control * globals->intervalpertick;
	}

	auto new_speed = max(speed - drop, 0.0f);

	if (new_speed == speed) //-V550
		return;

	*velocity.get() *= new_speed / speed;
}

void Movement::accelerate(crypt_ptr <CUserCmd> cmd, crypt_ptr <Vector> velocity, float wish_speed, Vector wish_direction, float max_unclamped_speed)
{
	auto speed = velocity->Length2D();

	if (speed <= 0.1f)
		velocity->Zero();
	else
		friction(velocity);

	auto current_speed = velocity->Dot(wish_direction);
	auto current_speed_clamped = max(current_speed, 0.0f);

	auto add_speed = wish_speed - current_speed;

	if (add_speed > 0.0f)
	{
		auto is_running = true;
		auto is_ducked = cmd->buttons & IN_DUCK || ctx->local()->m_fFlags() & FL_DUCKING;

		if (!(cmd->buttons & IN_SPEED) || is_ducked)
			is_running = 0;

		auto in_zoom = false;

		auto wish_speed_clamped = std::fmaxf(wish_speed, 250.0f);
		auto wish_speed_ratio = wish_speed_clamped;
		auto wish_speed_final = wish_speed_ratio;

		if (convars_manager->convars[CONVAR_SV_ACCELERATE_USE_WEAPON_SPEED]->GetBool())
		{
			if (ctx->weapon()->m_zoomLevel() > 0)
				in_zoom = ctx->max_speed * 0.52f < 110.f;

			auto ratio = ctx->max_speed / 250.0f;

			if (ratio < 1.0f)
				wish_speed_clamped *= ratio;

			if ((!is_ducked && !is_running || in_zoom) && ratio <= 1.0f)
			{
				wish_speed_ratio *= ratio;
				wish_speed_final = wish_speed_ratio;
			}
		}

		auto wish_speed_ducked = wish_speed_clamped;

		if (is_ducked)
		{
			if (!in_zoom)
			{
				wish_speed_ratio *= 0.52f;
				wish_speed_final = wish_speed_ratio;
			}

			wish_speed_ducked = wish_speed_clamped * 0.52f;
		}

		auto sv_accelerate = convars_manager->convars[CONVAR_SV_ACCELERATE]->GetFloat();

		if (is_running)
		{
			if (!in_zoom)
				wish_speed_ratio = wish_speed_final * 0.52f;

			wish_speed_ratio = wish_speed_ducked * 0.52f;

			if (current_speed_clamped > wish_speed_ratio - 5.0f && current_speed_clamped - wish_speed_ratio >= -5.0f)
				sv_accelerate *= max(min((wish_speed_ratio - current_speed_clamped - 5.0f) * 0.2f + 1.0f, 1.0f), 0.0f);
		}

		*velocity.get() += wish_direction * min(add_speed, globals->intervalpertick * sv_accelerate * wish_speed_ratio);
	}

	auto final_speed = velocity->Length2D();

	if (final_speed > max_unclamped_speed)
		*velocity.get() = velocity->Normalized() * max_unclamped_speed;
}