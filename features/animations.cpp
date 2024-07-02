// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "animations.h"
#include "features.h"
#include "..\globals.h"

#include "logs.h"

float min_estktard(float a1, float a2)
{
	float v2; // xmm2_4
	float v3; // xmm3_8
	float v5; // [esp+0h] [ebp-10h]
	float v6; // [esp+8h] [ebp-8h]

	v2 = a1;
	v5 = a1;
	v6 = a2;
	if (v5 < 0.0f == v6 < 0.0f)
		return min(a1, a2);
	v3 = a1;
	if (v3 >= 0.0f)
		v2 = a2;
	return v2;
}

void Animations::run()
{
	for (auto i = 1; i <= globals->maxclients; ++i)
	{
		auto player = crypt_ptr <Player> ((Player*)entitylist->GetClientEntity(i));

		if (player.get() == ctx->local().get())
			continue;

		if (!player->valid())
		{
			animation_data[i].clear();
			player_data[i].reset();
			//resolver[i]->reset();
			continue;
		}
		else if (player->IsDormant())
		{
			animation_data[i].clear();
			continue;
		}

		auto layers_updated = false;
		auto alive_loop_layer = player->get_animation_layer(11);

		if (!player->IsDormant())
		{
			auto layers_changed = alive_loop_layer->m_flCycle != player_data[i].last_loop_cycle || alive_loop_layer->m_flPlaybackRate != player_data[i].last_loop_rate;

			if (layers_changed)
			{
				layers_updated = true;

				player_data[i].last_loop_cycle = alive_loop_layer->m_flCycle;
				player_data[i].last_loop_rate = alive_loop_layer->m_flPlaybackRate;
			}
			else
			{
				player->m_flOldSimulationTime() = player_data[i].simulation_time_old;
				player->m_flSimulationTime() = player_data[i].simulation_time;
			}
		}

		if (!layers_updated)
			continue;

		auto previous_simulation_time = player_data[i].simulation_time;
		auto current_simulation_time = player->m_flSimulationTime();

		player_data[i].simulation_time_old = max(previous_simulation_time, player_data[i].simulation_time_old);
		player_data[i].simulation_time = current_simulation_time;

		if (!animation_data[i].empty() && !animation_data[i].front().dormant && (player->m_vecOrigin() - animation_data[i].front().origin).LengthSqr() > 4096.0f)
			for (auto& record : animation_data[i])
				record.invalid = true;

		player->m_bClientSideAnimation() = true;
		update(player, &animation_data[i].emplace_front(AnimationData(i)));

		while (animation_data[i].size() > 64)
			animation_data[i].pop_back();

		if (!convars_manager->convars[CONVAR_CL_LAGCOMPENSATION]->GetBool() && !animation_data[i].empty())
		{
			auto found_valid = false;

			for (auto& data : animation_data[i])
			{
				if (data.valid() && !found_valid)
				{
					found_valid = true;
					continue;
				}

				data.invalid = true;
			}
		}
	}
}

void Animations::update(crypt_ptr <Player> player, crypt_ptr <AnimationData> data)
{
	auto is_float_invalid = [](float value) -> bool
	{
		return isnan(value) || isinf(value);
	};

	auto animation_state = player->get_animation_state();

	if (!animation_state)
	{
		call_virtual <void(__thiscall*)(void*)>(player.get(), INDEX_UPDATE_CLIENTSIDE_ANIMATION)(player.get());
		return;
	}

	crypt_ptr <AnimationData> previous_data = nullptr;

	if (animation_data[data->i].size() >= 2)
		previous_data = &animation_data[data->i].at(1);

	if (previous_data && (previous_data->dormant || TIME_TO_TICKS(player->m_flSimulationTime() - previous_data->simulation_time) > 61))
		previous_data = nullptr;

	memcpy(data->layers[LAYERS_ORIGINAL], player->get_animation_layer().get(), player->get_animation_layers_count() * sizeof(AnimationLayer));

	for (auto i = 0; i < 13; ++i)
	{
		data->server_layers[i].m_flCycle = data->layers[LAYERS_ORIGINAL][i].m_flCycle;
		data->server_layers[i].m_flPlaybackRate = data->layers[LAYERS_ORIGINAL][i].m_flPlaybackRate;
		data->server_layers[i].m_flWeight = data->layers[LAYERS_ORIGINAL][i].m_flWeight;
		data->server_layers[i].m_nOrder = data->layers[LAYERS_ORIGINAL][i].m_nOrder;
		data->server_layers[i].m_nSequence = data->layers[LAYERS_ORIGINAL][i].m_nSequence;
	}

	const auto backup_lower_body_yaw_target = player->m_flLowerBodyYawTarget();
	const auto backup_duck_amount = player->m_flDuckAmount();
	const auto backup_flags = player->m_fFlags();
	const auto backup_strafing = player->m_bStrafing();
	const auto backup_walking = player->m_bIsWalking();

	auto backup_curtime = globals->curtime; //-V807
	auto backup_frametime = globals->frametime;
	auto backup_realtime = globals->realtime;
	auto backup_framecount = globals->framecount;
	auto backup_tickcount = globals->tickcount;
	auto backup_interpolation_amount = globals->interpolation_amount;

	globals->curtime = player->m_flSimulationTime();
	globals->frametime = globals->intervalpertick;

	auto simulation_ticks_old = TIME_TO_TICKS(player->m_flOldSimulationTime());
	auto simulation_ticks_new = TIME_TO_TICKS(player->m_flSimulationTime());

	if (simulation_ticks_new < simulation_ticks_old || abs(globals->tickcount - simulation_ticks_new) > 2)
		data->exploit = true;

	auto simulation_delta = clamp(simulation_ticks_new - simulation_ticks_old, 1, 18);

	if (simulation_ticks_old + simulation_delta != simulation_ticks_new) //-V1051
		simulation_ticks_old = simulation_ticks_new - simulation_delta;

	data->shot_time = 0.0f;
	data->shot = false;

	auto max_speed = 250.0f;
	auto weapon = crypt_ptr <Weapon>(player->m_hActiveWeapon().Get());

	if (weapon)
	{
		auto weapon_data = weapon->get_weapon_data();

		if (weapon_data)
			max_speed = player->m_bIsScoped() ? weapon_data->max_player_speed_alt : weapon_data->max_player_speed;

		data->shot_time = weapon->m_fLastShotTime();

		if (TIME_TO_TICKS(weapon->m_fLastShotTime()) > simulation_ticks_old && TIME_TO_TICKS(weapon->m_fLastShotTime()) <= simulation_ticks_new)
			data->shot = true;
	}

	if (abs(math::normalize_yaw(player->m_flThirdpersonRecoil() + player->m_angEyeAngles().x - 180.0f)) < 0.1f)
		player->m_angEyeAngles().x = 89.0f;

	data->max_speed = max_speed;

	if (previous_data)
	{
		auto current_playback_rate = data->server_layers[11].m_flPlaybackRate;
		auto previous_playback_rate = previous_data->server_layers[11].m_flPlaybackRate;

		auto current_cycle = data->server_layers[11].m_flCycle;
		auto previous_cycle = (int)(previous_data->server_layers[11].m_flCycle / (globals->intervalpertick * previous_playback_rate) + 0.5f);

		auto cycle = 0;

		if (current_playback_rate == previous_playback_rate)
			cycle = (int)(current_cycle / (current_playback_rate * globals->intervalpertick) + 0.5f);
		else
			cycle = (int)(previous_cycle + ((current_cycle / current_playback_rate + (1.0f - previous_data->server_layers[11].m_flCycle) / previous_playback_rate) / globals->intervalpertick + 0.5f));

		auto layer_delta = cycle - previous_cycle;

		if (layer_delta <= 18)
			simulation_delta = max(layer_delta, 1);
	}

	if (simulation_delta >= 18)
		simulation_delta = 1;

	if (simulation_ticks_old + simulation_delta != simulation_ticks_new)
		simulation_ticks_old = simulation_ticks_new - simulation_delta;

	data->choke = simulation_delta;
	data->server_tick = clientstate->m_ClockDriftMgr.m_nServerTick;
	data->velocity_state = 0;

	if (previous_data && data->choke > 1)
	{
		auto animation_speed = 0.0f;

		auto origin_delta = player->m_vecOrigin() - previous_data->origin;
		auto time_difference = player->m_flSimulationTime() - previous_data->simulation_time;

		if (!origin_delta.IsZero())
		{
			auto was_in_ground = backup_flags & FL_ONGROUND && previous_data->flags & FL_ONGROUND;

			if (TIME_TO_TICKS(time_difference) > 1)
			{
				if (TIME_TO_TICKS(time_difference) < 20)
				{
					player->m_vecVelocity() = origin_delta / time_difference;
					data->velocity_state = 1;

					if (animation_data[data->i].size() >= 3)
					{
						auto previous_velocity = 1.0f / TICKS_TO_TIME(previous_data->choke) * (previous_data->origin - animation_data[data->i].at(2).origin);

						if (!previous_velocity.IsZero())
						{
							auto real_velocity = player->m_vecVelocity().Length2D();

							Vector velocity_angle;
							math::vector_angles(player->m_vecVelocity(), velocity_angle);

							Vector previous_velocity_angle;
							math::vector_angles(previous_data->velocity, previous_velocity_angle);

							auto delta = math::normalize_yaw(velocity_angle.y - previous_velocity_angle.y);
							auto velocity_direction = math::normalize_yaw(delta * 0.5f + velocity_angle.y);

							if (abs(delta) < 80.f)
							{
								Vector angle;
								angle.y = velocity_direction;

								Vector direction;
								math::angle_vectors(angle, &direction, nullptr, nullptr);

								player->m_vecVelocity().x = direction.x * real_velocity;
								player->m_vecVelocity().y = direction.y * real_velocity;
							}
						}
					}
				}

				if (backup_flags & FL_ONGROUND)
				{
					if (data->server_layers[11].m_flWeight > 0.0f && data->server_layers[11].m_flWeight < 1.0f && data->server_layers[11].m_flWeight != previous_data->server_layers[11].m_flWeight && data->server_layers[11].m_flPlaybackRate == previous_data->server_layers[11].m_flPlaybackRate)
					{
						auto modifier = 0.35f * (1.0f - data->server_layers[11].m_flWeight);

						if (modifier > 0.0f && modifier < 1.0f)
							animation_speed = max_speed * (modifier + 0.55f);
					}

					if (animation_speed > 0.0f)
					{
						animation_speed /= player->m_vecVelocity().Length2D();

						player->m_vecVelocity().x *= animation_speed;
						player->m_vecVelocity().y *= animation_speed;

						data->velocity_state = 2;
					}
					else
					{
						auto weight_speed = data->server_layers[6].m_flWeight;

						if (weight_speed > 0.0f && weight_speed <= 0.1f)
						{
							if (backup_flags & FL_DUCKING)
								weight_speed = weight_speed * 0.34f;
							else if (player->m_bIsWalking())
								weight_speed = weight_speed * 0.52f;

							if (weight_speed > 0.0f)
							{
								player->m_vecVelocity().x = (player->m_vecVelocity().x / player->m_vecVelocity().Length()) * max_speed * weight_speed;
								player->m_vecVelocity().y = (player->m_vecVelocity().y / player->m_vecVelocity().Length()) * max_speed * weight_speed;

								data->velocity_state = 3;
							}
						}
					}

					player->m_vecVelocity().z = 0.0f;
				}
			}

			if (!was_in_ground && data->server_layers[4].m_flWeight > 0.0f && !convars_manager->convars[CONVAR_SV_ENABLEBUNNYHOPPING]->GetInt())
			{
				auto speed = player->m_vecVelocity().Length();

				if (speed > 320.0f)
					player->m_vecVelocity() *= 320.0f / speed;
			}

			auto cur_cycle = data->server_layers[4].m_flCycle;
			auto prev_cycle = previous_data->server_layers[4].m_flCycle;

			if (backup_flags & FL_ONGROUND)
				player->m_vecVelocity().z = 0.f;
			else
			{
				bool jumped = player->get_sequence_activity(data->server_layers[4].m_nSequence) == ACT_CSGO_JUMP;

				if (cur_cycle == prev_cycle && data->server_layers[4].m_nSequence == previous_data->server_layers[4].m_nSequence
					|| cur_cycle >= prev_cycle)
					jumped = false;

				float new_z = (origin_delta.z / TICKS_TO_TIME(data->choke));

				if (!jumped)
				{
					auto gravity = convars_manager->convars[CONVAR_SV_GRAVITY]->GetFloat();
					gravity = (gravity * -1.f) * TICKS_TO_TIME(data->choke);
					new_z = ((origin_delta.z / TICKS_TO_TIME(data->choke)) - (gravity * 0.5f)) + gravity;

					player->m_vecVelocity().z = min_estktard(new_z, convars_manager->convars[CONVAR_SV_JUMP_IMPULSE]->GetFloat());
				}
				else if ((previous_data->flags & FL_ONGROUND) != 0)
				{
					float time_in_air = max(globals->intervalpertick, data->server_layers[4].m_flCycle / data->server_layers[4].m_flPlaybackRate);

					auto gravity = (convars_manager->convars[CONVAR_SV_GRAVITY]->GetFloat() * -1.f) * time_in_air;
					new_z = ((origin_delta.z / time_in_air) - (gravity * 0.5f)) + gravity;
					player->m_vecVelocity().z = min_estktard(new_z, convars_manager->convars[CONVAR_SV_JUMP_IMPULSE]->GetFloat());
				}
				else
				{
					float time_in_air = max(globals->intervalpertick, data->server_layers[4].m_flCycle / data->server_layers[4].m_flPlaybackRate);
					auto gravity_inverted = (convars_manager->convars[CONVAR_SV_GRAVITY]->GetFloat() * -1.f);

					CGameTrace tr;
					tr.hitgroup = -1;
					tr.hitbox = -1;
					tr.fraction = 1.0f;
					tr.startsolid = 0;
					tr.hit_entity = 0;

					Ray_t ray;
					ray.Init((previous_data->origin + player->m_vecOrigin()) * 0.5f, Vector(player->m_vecOrigin().x, player->m_vecOrigin().y, abs((((gravity_inverted * 0.5f) * time_in_air) * time_in_air) + (convars_manager->convars[CONVAR_SV_JUMP_IMPULSE]->GetFloat() * time_in_air)) - player->m_vecOrigin().z), player->GetCollideable()->OBBMins(), player->GetCollideable()->OBBMaxs());

					CTraceFilter filter;
					filter.pSkip = player.get();

					enginetrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

					if (tr.DidHit() && tr.plane.normal.z >= 0.7f)
					{
						new_z = ((player->m_vecOrigin().z - tr.endpos.z) / time_in_air)
							- ((gravity_inverted * time_in_air) * 0.5f);

						player->m_vecVelocity().z = fminf(new_z, (gravity_inverted * time_in_air) + new_z);
					}
					else
					{
						player->m_vecVelocity().z = (gravity_inverted * time_in_air) + convars_manager->convars[CONVAR_SV_JUMP_IMPULSE]->GetFloat();
					}
				}
			}
		}
	}

	if (backup_flags & FL_ONGROUND && player->m_vecVelocity().Length() > 0.0f && data->server_layers[6].m_flPlaybackRate <= 0.0f)
		player->m_vecVelocity().Zero();

	if (is_float_invalid(player->m_vecVelocity().x) || is_float_invalid(player->m_vecVelocity().y) || is_float_invalid(player->m_vecVelocity().z))
		player->m_vecVelocity().Zero();

	if (backup_flags & FL_ONGROUND
		&& player->m_vecVelocity().Length() < 0.1f
		&& data->server_layers[6].m_flPlaybackRate != 0.f
		&& data->server_layers[6].m_flWeight != 0.f)
		player->m_vecVelocity() = Vector(1.1f, 0, 0);

	auto backup_velocity = player->m_vecVelocity();

	if (previous_data)
	{
		*animation_state.get() = previous_data->animation_state;

		animation_state->primary_cycle = previous_data->server_layers[6].m_flCycle;
		animation_state->move_weight = previous_data->server_layers[6].m_flWeight;
		animation_state->strafe_sequence = previous_data->server_layers[7].m_nSequence;
		animation_state->strafe_change_weight = previous_data->server_layers[7].m_flWeight;
		animation_state->strafe_change_cycle = previous_data->server_layers[7].m_flCycle;
		animation_state->acceleration_weight = previous_data->server_layers[12].m_flWeight;

	}
	else
	{
		auto last_update_time = animation_state->last_update_time;

		if (backup_flags & FL_ONGROUND)
		{
			animation_state->on_ground = true;
			animation_state->landing = false;

			player->m_vecVelocity().z = 0.0f;
		}

		animation_state->last_update_time = player->m_flSimulationTime() - globals->intervalpertick;

		if (player->get_sequence_activity(data->server_layers[4].m_nSequence) == ACT_CSGO_JUMP)
		{
			auto land_time = player->m_flSimulationTime() - (data->server_layers[4].m_flCycle / data->server_layers[4].m_flPlaybackRate);

			if (land_time > last_update_time)
			{
				animation_state->on_ground = false;
				animation_state->duration_in_air = 0.0f;
				animation_state->last_update_time = land_time;

				player->m_flPoseParameters()[6] = 0.0f;
			}
		}
	}

	if (previous_data)
	{
		memcpy(player->get_animation_layer().get(), previous_data->layers, player->get_animation_layers_count() * sizeof(AnimationLayer));

		if (data->choke > 1)
		{
			auto land_tick = -1;
			auto land_type = 0;

			auto on_ground = backup_flags & FL_ONGROUND;

			if (!(backup_flags & FL_ONGROUND && previous_data->flags & FL_ONGROUND))
			{
				if (backup_flags & FL_ONGROUND)
				{
					if (!(previous_data->flags & FL_ONGROUND) && data->server_layers[5].m_flWeight > 0.0f && previous_data->server_layers[5].m_flWeight <= 0.0f)
					{
						auto sequence_activity = player->get_sequence_activity(data->server_layers[5].m_nSequence);

						if (sequence_activity == ACT_CSGO_LAND_LIGHT || sequence_activity == ACT_CSGO_LAND_HEAVY)
						{
							land_tick = TIME_TO_TICKS(player->m_flSimulationTime() - (data->server_layers[5].m_flCycle / data->server_layers[5].m_flPlaybackRate)) + 1;
							land_type = 1;
						}
					}
				}
				else if ((previous_data->server_layers[4].m_flCycle != data->server_layers[4].m_flCycle || previous_data->server_layers[4].m_nSequence != data->server_layers[4].m_nSequence) && previous_data->server_layers[4].m_flCycle > data->server_layers[4].m_flCycle)
				{
					auto sequence_activity = player->get_sequence_activity(data->server_layers[4].m_nSequence);

					if (sequence_activity == ACT_CSGO_JUMP)
					{
						land_tick = TIME_TO_TICKS(player->m_flSimulationTime() - (data->server_layers[4].m_flCycle / data->server_layers[4].m_flPlaybackRate)) + 1;
						land_type = 2;
					}
				}

				on_ground = previous_data->flags & FL_ONGROUND;

				if (!(backup_flags & FL_ONGROUND) && data->shot)
				{
					if (data->server_layers[4].m_flWeight > 0.0f && data->server_layers[4].m_flPlaybackRate > 0.0f)
					{
						if (player->get_sequence_activity(data->server_layers[4].m_nSequence) == ACT_CSGO_JUMP)
						{
							if ((data->server_layers[4].m_flCycle != previous_data->server_layers[4].m_flCycle || previous_data->server_layers[4].m_nSequence != data->server_layers[4].m_nSequence) && previous_data->server_layers[4].m_flCycle > data->server_layers[4].m_flCycle)
							{
								player->m_flPoseParameters()[6] = 0.0f;
								animation_state->duration_in_air = data->server_layers[4].m_flCycle / data->server_layers[4].m_flPlaybackRate;
							}
						}
					}
				}
			}

			auto duck_amount_per_tick = (player->m_flDuckAmount() - previous_data->duck_amount) / (float)data->choke;
			auto velocity_per_tick = (player->m_vecVelocity() - previous_data->velocity) / (float)data->choke;

			player->m_bStrafing() = previous_data->strafing; //used in animfix but not calculated on client
			player->m_bIsWalking() = previous_data->walking; //used in animfix but not calculated on client

			for (auto i = 1; i < data->choke; ++i)
			{
				auto simulated_time = TICKS_TO_TIME(simulation_ticks_old + i);
				auto sim_tick = TIME_TO_TICKS(simulated_time);

				if (duck_amount_per_tick != 0.0f)
					player->m_flDuckAmount() = previous_data->duck_amount + duck_amount_per_tick * (float)i;

				if (!velocity_per_tick.IsZero())
					player->m_vecVelocity() = previous_data->velocity + velocity_per_tick * (float)i;

				if (land_type > 0)
				{
					if (simulated_time < player->m_flSimulationTime())
					{
						if (sim_tick == land_tick && land_type == 1)
						{
							on_ground = true;

							player->get_animation_layer(5)->m_flCycle = 0.0f;
							player->get_animation_layer(5)->m_flPlaybackRate = data->server_layers[5].m_flPlaybackRate;
							player->get_animation_layer(5)->m_nSequence = data->server_layers[5].m_nSequence;
						}
						else if (sim_tick == land_tick - 1 && land_type == 2)
							on_ground = true;
						else if (sim_tick == land_tick && land_type == 2)
						{
							on_ground = false;

							player->get_animation_layer(4)->m_flCycle = 0.0f;
							player->get_animation_layer(4)->m_flPlaybackRate = data->server_layers[4].m_flPlaybackRate;
							player->get_animation_layer(4)->m_nSequence = data->server_layers[4].m_nSequence;

							// z vel fix on jump
							auto gravity = convars_manager->convars[CONVAR_SV_GRAVITY]->GetFloat() * globals->intervalpertick;
							player->m_vecVelocity().z = convars_manager->convars[CONVAR_SV_JUMP_IMPULSE]->GetFloat() - (gravity * 0.5f);
						}
					}

					if (on_ground)
						player->m_fFlags() |= FL_ONGROUND;
					else
						player->m_fFlags() &= ~FL_ONGROUND;
				}

				if (data->shot)
				{
					if (data->shot_time <= simulated_time)
						player->m_flLowerBodyYawTarget() = backup_lower_body_yaw_target;
					else
						player->m_flLowerBodyYawTarget() = previous_data->lower_body_yaw_target;
				}
				else
					player->m_flLowerBodyYawTarget() = backup_lower_body_yaw_target;

				globals->realtime = simulated_time;
				globals->curtime = simulated_time;
				globals->framecount = sim_tick;
				globals->tickcount = sim_tick;
				globals->interpolation_amount = 0.0f;

				player->update_animations();

				globals->realtime = backup_realtime;
				globals->curtime = backup_curtime;
				globals->framecount = backup_framecount;
				globals->tickcount = backup_tickcount;
				globals->interpolation_amount = backup_interpolation_amount;
			}
		}
	}

	player->m_vecVelocity() = backup_velocity;
	player->m_flLowerBodyYawTarget() = backup_lower_body_yaw_target;
	player->m_flDuckAmount() = backup_duck_amount;
	player->m_fFlags() = backup_flags;
	player->m_bStrafing() = backup_strafing;
	player->m_bIsWalking() = backup_walking;

	globals->curtime = player->m_flSimulationTime();

	player->UpdateCollisionBounds();

	auto setup_matrix = [](crypt_ptr <Player> player, crypt_ptr <AnimationData> data, const int& matrix, const float& roll) -> void
	{
		auto backup_abs_origin = player->GetAbsOrigin();

		player->invalidate_physics_recursive(8);
		player->set_abs_origin(player->m_vecOrigin());

		AnimationLayer backup_layers[13];

		memcpy(backup_layers, player->get_animation_layer().get(), player->get_animation_layers_count() * sizeof(AnimationLayer));
		memcpy(player->get_animation_layer().get(), data->layers, player->get_animation_layers_count() * sizeof(AnimationLayer));

		auto success = matrix != MATRIX_VISUAL_INTERPOLATED && player->setup_bones_rebuilded(BONE_USED_BY_HITBOX, data->matrix[matrix]);

		if (!success)
			player->setup_bones(data->matrix[matrix], matrix == MATRIX_VISUAL_INTERPOLATED ? BONE_USED_BY_ANYTHING : BONE_USED_BY_HITBOX);

#if 0
		ctx->fixing_roll = true;

		if (ctx->fixing_roll)
		{
			auto current_backup_curtime = globals->curtime;

			ctx->new_fixed_roll = roll;
			globals->curtime = TICKS_TO_TIME(clientstate->m_ClockDriftMgr.m_nServerTick);

			((void(__thiscall*)(void*, void*, int))signatures_manager->signatures[SIGNATURE_MODIFY_BONES])(player.get(), data->matrix[matrix], (matrix == MATRIX_VISUAL_INTERPOLATED ? BONE_USED_BY_ANYTHING : BONE_USED_BY_HITBOX));
			globals->curtime = current_backup_curtime;
		}

		ctx->fixing_roll = false;
		ctx->new_fixed_roll = 0.0f;
#endif

		player->set_abs_origin(backup_abs_origin);
		memcpy(player->get_animation_layer().get(), backup_layers, player->get_animation_layers_count() * sizeof(AnimationLayer));
	};

	auto roll = 0.0f;

	if (config->player_list.player_settings[data->i].force_roll)
		roll = clamp((float)config->player_list.player_settings[data->i].roll, -50.0f, 50.0f);

	AnimationState backup_animation_state;
	memcpy(&backup_animation_state, animation_state.get(), sizeof(AnimationState));

	AnimationLayer backup_layers[13];
	memcpy(backup_layers, player->get_animation_layer().get(), player->get_animation_layers_count() * sizeof(AnimationLayer));

	animation_state->goal_feet_yaw = player_data[data->i].goal_feet_yaw;
	player->update_animations();

	//resolver[data->i]->goal_feet_yaw[GOAL_FEET_YAW_ORIGINAL] = animation_state->goal_feet_yaw;
	data->animation_state = *animation_state.get();

	memcpy(data->layers[LAYERS_ZERO], player->get_animation_layer().get(), player->get_animation_layers_count() * sizeof(AnimationLayer));

	player_data[data->i].goal_feet_yaw = animation_state->goal_feet_yaw;
	memcpy(player->get_animation_layer().get(), backup_layers, player->get_animation_layers_count() * sizeof(AnimationLayer));

	memcpy(animation_state.get(), &backup_animation_state, sizeof(AnimationState));

	auto valid_team = player->m_iTeamNum() != ctx->local()->m_iTeamNum() || ctx->friendly_fire;

	if (config->rage.enable && valid_team && ctx->local()->valid())
	{
		animation_state->goal_feet_yaw = math::normalize_yaw(player->m_angEyeAngles().y);
		player->update_animations();

		setup_matrix(player, data, MATRIX_ZERO, roll);

		memcpy(player->get_animation_layer().get(), backup_layers, player->get_animation_layers_count() * sizeof(AnimationLayer));
		memcpy(animation_state.get(), &backup_animation_state, sizeof(AnimationState));

		animation_state->goal_feet_yaw = math::normalize_yaw(player->m_angEyeAngles().y + 120.0f);
		player->update_animations();

		//resolver[data->i]->goal_feet_yaw[GOAL_FEET_YAW_FIRST] = animation_state->goal_feet_yaw;

		memcpy(data->layers[LAYERS_FIRST], player->get_animation_layer().get(), player->get_animation_layers_count() * sizeof(AnimationLayer));
		setup_matrix(player, data, MATRIX_FIRST, roll);

		memcpy(player->get_animation_layer().get(), backup_layers, player->get_animation_layers_count() * sizeof(AnimationLayer));
		memcpy(animation_state.get(), &backup_animation_state, sizeof(AnimationState));

		animation_state->goal_feet_yaw = math::normalize_yaw(player->m_angEyeAngles().y + animation_state->get_desync_delta() * 0.5f);
		player->update_animations();

		setup_matrix(player, data, MATRIX_FIRST_LOW, roll);

		memcpy(player->get_animation_layer().get(), backup_layers, player->get_animation_layers_count() * sizeof(AnimationLayer));
		memcpy(animation_state.get(), &backup_animation_state, sizeof(AnimationState));

		animation_state->goal_feet_yaw = math::normalize_yaw(player->m_angEyeAngles().y - 120.0f);
		player->update_animations();

		//resolver[data->i]->goal_feet_yaw[GOAL_FEET_YAW_SECOND] = animation_state->goal_feet_yaw;

		memcpy(data->layers[LAYERS_SECOND], player->get_animation_layer().get(), player->get_animation_layers_count() * sizeof(AnimationLayer));
		setup_matrix(player, data, MATRIX_SECOND, roll);

		memcpy(player->get_animation_layer().get(), backup_layers, player->get_animation_layers_count() * sizeof(AnimationLayer));
		memcpy(animation_state.get(), &backup_animation_state, sizeof(AnimationState));

		animation_state->goal_feet_yaw = math::normalize_yaw(player->m_angEyeAngles().y - animation_state->get_desync_delta() * 0.5f);
		player->update_animations();

		setup_matrix(player, data, MATRIX_SECOND_LOW, roll);
		memcpy(player->get_animation_layer().get(), backup_layers, player->get_animation_layers_count() * sizeof(AnimationLayer));

		if (data->choke > 1)
		{
			//auto delta = math::normalize_yaw(resolver[data->i]->goal_feet_yaw[GOAL_FEET_YAW_FIRST] - resolver[data->i]->goal_feet_yaw[GOAL_FEET_YAW_ORIGINAL]);
			//auto negative_delta = delta < 0.0f;

			//if (negative_delta)
			{
				//swap(data->matrix[MATRIX_SECOND], data->matrix[MATRIX_FIRST]);
				//swap(data->matrix[MATRIX_SECOND_LOW], data->matrix[MATRIX_FIRST_LOW]);

				//swap(resolver[data->i]->goal_feet_yaw[GOAL_FEET_YAW_FIRST], resolver[data->i]->goal_feet_yaw[GOAL_FEET_YAW_SECOND]);
			}
		}
	}

	memcpy(animation_state.get(), &data->animation_state, sizeof(AnimationState));
	//resolver[data->i]->initialize(player, data, previous_data);

	if (config->player_list.player_settings[data->i].force_body_yaw)
	{
		animation_state->goal_feet_yaw = math::normalize_yaw(player->m_angEyeAngles().y + (float)config->player_list.player_settings[data->i].body_yaw);

		///*data->resolver_type = RESOLVER_TYPE_CUSTOM;
		//*/data->resolver_side = RESOLVER_SIDE_CUSTOM;
	}
	else
	{
		//resolver[data->i]->resolve_yaw();

		//if (data->resolver_side == RESOLVER_SIDE_ORIGINAL)
		//	animation_state->goal_feet_yaw = player_data[data->i].goal_feet_yaw;
		//else if (data->resolver_side == RESOLVER_SIDE_ZERO)
		//	animation_state->goal_feet_yaw = math::normalize_yaw(player->m_angEyeAngles().y);
		//else if (data->resolver_side == RESOLVER_SIDE_FIRST)
		//	animation_state->goal_feet_yaw = math::normalize_yaw(player->m_angEyeAngles().y + 120.0f);
		//else if (data->resolver_side == RESOLVER_SIDE_SECOND)
		//	animation_state->goal_feet_yaw = math::normalize_yaw(player->m_angEyeAngles().y - 120.0f);
	}

	if (config->player_list.player_settings[data->i].force_pitch)
		player->m_angEyeAngles().x = (float)config->player_list.player_settings[data->i].pitch;

	player->update_animations();

	setup_matrix(player, data, MATRIX_MAIN, roll);
	setup_matrix(player, data, MATRIX_VISUAL_INTERPOLATED, roll);

	memcpy(data->matrix[MATRIX_VISUAL], data->matrix[MATRIX_VISUAL_INTERPOLATED], MAXSTUDIOBONES * sizeof(matrix3x4_t));

	globals->curtime = backup_curtime;
	globals->frametime = backup_frametime;

	player->m_flLowerBodyYawTarget() = backup_lower_body_yaw_target;
	player->m_flDuckAmount() = backup_duck_amount;
	player->m_fFlags() = backup_flags;

	memcpy(player->get_animation_layer().get(), data->layers, player->get_animation_layers_count() * sizeof(AnimationLayer));
	data->store(player, false);

	if (convars_manager->convars[CONVAR_CL_LAGCOMPENSATION]->GetBool())
	{
		data->invalid = data->simulation_time < player_data[data->i].max_previous_simtime;
		player_data[data->i].max_previous_simtime = max(data->simulation_time, player_data[data->i].max_previous_simtime);
	}
}