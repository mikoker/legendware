#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"
#include "features.h"
#include "prediction.h"
#include "logs.h"

enum
{
	MATRIX_VISUAL,
	MATRIX_VISUAL_INTERPOLATED,
	MATRIX_MAIN,
	MATRIX_ZERO,
	MATRIX_FIRST,
	MATRIX_FIRST_LOW,
	MATRIX_SECOND,
	MATRIX_SECOND_LOW,
	MATRIX_MAX
};

enum
{
	LAYERS_ORIGINAL,
	LAYERS_ZERO,
	LAYERS_FIRST,
	LAYERS_SECOND,
	LAYERS_MAX
};

class AnimationData
{
public:
	int i;

	matrix3x4_t matrix[MATRIX_MAX][MAXSTUDIOBONES];
	AnimationLayer layers[LAYERS_MAX][13];
	ShortAnimationLayer server_layers[13];

	bool invalid;
	bool immune;
	bool dormant;
	bool shot;
	bool backup;
	bool exploit;
	bool strafing;
	bool walking;

	int choke;
	int server_tick;
	int flags;
	int bone_count;
	int resolver_type;
	int resolver_side;
	int velocity_state;

	float curtime;
	float simulation_time;
	float old_simulation_time;
	float shot_time;
	float duck_amount;
	float lower_body_yaw_target;
	float max_speed;
	float roll;

	Vector angles;
	Vector abs_angles;
	Vector velocity;
	Vector origin;
	Vector render_origin;
	Vector mins;
	Vector maxs;

	AnimationState animation_state;

	AnimationData(int i = -1) //-V730
	{
		this->i = i;

		invalid = false;
		immune = false;
		dormant = false;
		backup = false;
		shot = false;
		exploit = false;
		strafing = false;
		walking = false;

		choke = 0;
		server_tick = 0;
		flags = 0;
		bone_count = 0;
		resolver_type = 0;
		resolver_side = 0;
		velocity_state = 0;

		curtime = 0.0f;
		simulation_time = 0.0f;
		shot_time = 0.0f;
		old_simulation_time = 0.0f;
		duck_amount = 0.0f;
		lower_body_yaw_target = 0.0f;
		max_speed = 0.0f;
		roll = 0.0f;

		angles.Zero();
		abs_angles.Zero();
		velocity.Zero();
		origin.Zero();
		render_origin.Zero();
		mins.Zero();
		maxs.Zero();
	}

	AnimationData(crypt_ptr <Player> player)
	{
		shot = false;
		backup = false;
		exploit = false;

		choke = 0;
		server_tick = 0;
		resolver_type = 0;
		resolver_side = 0;
		velocity_state = 0;

		shot_time = 0.0f;
		max_speed = 0.0f;
		roll = 0.0f;

		store(player); //-V1053
	}

	virtual void store(crypt_ptr <Player> player, bool store_extra = true)
	{
		i = player->EntIndex();

		if (store_extra)
		{
			memcpy(matrix[MATRIX_MAIN], player->m_CachedBoneData().Base(), player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));
			memcpy(layers[LAYERS_ORIGINAL], player->get_animation_layer().get(), player->get_animation_layers_count() * sizeof(AnimationLayer));
		}

		invalid = false;
		backup = store_extra;
		immune = player->m_bGunGameImmunity();
		dormant = player->IsDormant();
		strafing = player->m_bStrafing();
		walking = player->m_bIsWalking();

		flags = player->m_fFlags();
		bone_count = player->m_CachedBoneData().Count();

		simulation_time = player->m_flSimulationTime();
		old_simulation_time = player->m_flOldSimulationTime();
		duck_amount = player->m_flDuckAmount();
		lower_body_yaw_target = player->m_flLowerBodyYawTarget();

		angles = player->m_angEyeAngles();
		abs_angles = player->GetAbsAngles();
		velocity = player->m_vecVelocity();
		origin = player->m_vecOrigin();
		render_origin = player->m_vecOrigin();

		auto collideable = crypt_ptr <ICollideable> (player->GetCollideable());

		if (collideable)
		{
			mins = collideable->OBBMins();
			maxs = collideable->OBBMaxs();
		}

		animation_state = *player->get_animation_state().get();
	}

	virtual void apply(int matrix_index = MATRIX_MAIN, bool backup = false)
	{
		auto player = crypt_ptr <Player> ((Player*)entitylist->GetClientEntity(i));

		if (!player->valid())
			return;

		player->m_angEyeAngles() = angles;
		player->set_abs_angles(abs_angles);
		player->m_vecOrigin() = origin;
		player->set_abs_origin(origin);
		player->set_collision_bounds(mins, maxs, true);

		memcpy(player->get_animation_layer().get(), layers[LAYERS_ORIGINAL], player->get_animation_layers_count() * sizeof(AnimationLayer));

		if (matrix_index >= MATRIX_MAIN && matrix_index < MATRIX_MAX)
		{
			player->m_CachedBoneData().m_Size = bone_count;
			memcpy(player->m_CachedBoneData().Base(), matrix[matrix_index], bone_count * sizeof(matrix3x4_t));

			const auto current_backup_curtime = globals->curtime;

			globals->curtime = TICKS_TO_TIME(ctx->local()->m_nTickBase());
			((void(__thiscall*)(void*, void*, int))signatures_manager->signatures[SIGNATURE_MODIFY_BONES])(player.get(), player->m_CachedBoneData().Base(), BONE_USED_BY_HITBOX);
			globals->curtime = current_backup_curtime;
		}

		player->m_iMostRecentModelBoneCounter() = player->m_iModelBoneCounter();
	}

	virtual bool valid(bool extra_checks = true, float limit = 0.2f, bool visual = false)
	{
		auto player = crypt_ptr <Player> ((Player*)entitylist->GetClientEntity(i));

		if (!player->valid())
			return false;

		if (invalid)
			return false;

		if (immune)
			return false;

		if (dormant)
			return false;

		if (!extra_checks)
			return true;

		if (!convars_manager->convars[CONVAR_CL_LAGCOMPENSATION]->GetBool())
			return true;

		auto latency = 0.0f;

		if (visual)
		{
			auto net_channel_info = engine->GetNetChannelInfo();

			if (net_channel_info)
				latency = clamp(net_channel_info->GetLatency(FLOW_OUTGOING) + net_channel_info->GetLatency(FLOW_INCOMING), 0.0f, 1.0f);
		}
		else
			latency = clamp(TICKS_TO_TIME(engine_prediction->latency), 0.0f, 1.0f);

		auto correct = 0.0f;

		if (visual)
		{
			auto update_rate = clamp(convars_manager->convars[CONVAR_CL_UPDATERATE]->GetFloat(), convars_manager->convars[CONVAR_SV_MINUPDATERATE]->GetFloat(), convars_manager->convars[CONVAR_SV_MAXUPDATERATE]->GetFloat());
			auto lerp_ratio = clamp(convars_manager->convars[CONVAR_CL_INTERP_RATIO]->GetFloat(), convars_manager->convars[CONVAR_SV_CLIENT_MIN_INTERP_RATIO]->GetFloat(), convars_manager->convars[CONVAR_SV_CLIENT_MAX_INTERP_RATIO]->GetFloat());
			auto interpolation = clamp(lerp_ratio / update_rate, convars_manager->convars[CONVAR_CL_INTERP]->GetFloat(), 1.0f);

			correct = clamp(latency + interpolation, 0.0f, convars_manager->convars[CONVAR_SV_MAXUNLAG]->GetFloat());
		}
		else
			correct = clamp(latency + ctx->interpolation, 0.0f, convars_manager->convars[CONVAR_SV_MAXUNLAG]->GetFloat());

		auto delta_time = 0.0f;

		if (visual)
			delta_time = correct - (globals->curtime - simulation_time);
		else
			delta_time = correct - (TICKS_TO_TIME(ctx->tickbase) - simulation_time);

		if (abs(delta_time) >= limit)
			return false;

		auto extra_choke = 0;

		if (!visual && ctx->fake_ducking)
			extra_choke = 14 - clientstate->m_nChokedCommands;

		auto server_tickcount = globals->tickcount + latency + extra_choke;
		auto dead_time = (int)(float)((float)((int)((float)((float)server_tickcount * globals->intervalpertick) - 0.2f) / globals->intervalpertick) + 0.5f);

		if (TIME_TO_TICKS(simulation_time + ctx->interpolation) < dead_time)
			return false;

		return true;
	}
};

class PlayerData
{
public:
	float simulation_time;
	float simulation_time_old;
	float max_previous_simtime;
	float last_loop_cycle;
	float last_loop_rate;
	float goal_feet_yaw;

	PlayerData()
	{
		reset();
	}

	void reset()
	{
		simulation_time = 0.0f;
		simulation_time_old = 0.0f;
		max_previous_simtime = 0.0f;
		last_loop_cycle = 0.0f;
		last_loop_rate = 0.0f;
		goal_feet_yaw = 0.0f;
	}
};

class Animations
{
public:
	deque <AnimationData> animation_data[65];
	PlayerData player_data[65];

	virtual void update(crypt_ptr <Player> player, crypt_ptr <AnimationData> data);
	virtual void run();
};