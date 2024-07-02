// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "aim.h"
#include "penetration.h"
#include "prediction.h"
#include "movement.h"
#include "exploits.h"
#include "..\config\config.h"
#include "..\sdk\misc\RayTracer.hpp"

#include "logs.h"

#include "../new_threads.h"

vector <Shot> shots;

void Aim::run(crypt_ptr <CUserCmd> cmd) //-V813
{
	stop = false;
	early_stop = false;
	full_stop = false;
	skip_automatic_scope = false;
	ticks_to_stop = 0;

	backup.clear();
	targets.clear();
	final_target.reset();

	if (!ctx->weapon()->can_fire())
		return;

	if (ctx->weapon_config == WEAPON_CONFIG_INVALID)
		return;
	else if (ctx->weapon_config == WEAPON_CONFIG_TASER)
	{
		config->rage.weapon[WEAPON_CONFIG_TASER].hit_chance = 75;
		config->rage.weapon[WEAPON_CONFIG_TASER].minimum_damage = 100;
		config->rage.weapon[WEAPON_CONFIG_TASER].body_aim = 1;

		for (auto i = 0; i < 6; ++i)
		{
			config->rage.weapon[WEAPON_CONFIG_TASER].hitboxes[i] = true;
			config->rage.weapon[WEAPON_CONFIG_TASER].safe_hitboxes[i] = 2;
		}
	}

	jump_scout = ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_SSG08 && !(engine_prediction->flags & FL_ONGROUND) && !(ctx->local()->m_fFlags() & FL_ONGROUND) && engine_prediction->velocity.Length2D() < 5.0f && abs(engine_prediction->velocity.z) < 5.0f;

	automatic_revolver(cmd);
	prepare();

	auto on_ground = engine_prediction->flags & FL_ONGROUND && ctx->local()->m_fFlags() & FL_ONGROUND;
	auto valid_stop = on_ground || ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_SSG08 && !(engine_prediction->flags & FL_ONGROUND) && !(ctx->local()->m_fFlags() & FL_ONGROUND) && engine_prediction->velocity.Length2D() < 5.0f;

	if (on_ground)
	{
		RestoreData restore_data;
		engine_prediction->store_restore_data(restore_data);

		CUserCmd command;
		memcpy(&command, cmd.get(), sizeof(CUserCmd)); //-V598

		for (ticks_to_stop = 0; ticks_to_stop < 14; ++ticks_to_stop)
		{
			if (ctx->local()->m_vecVelocity().Length2D() + 1.0f < ctx->max_speed * 0.34f)
				break;

			Vector angle;
			math::vector_angles(-ctx->local()->m_vecVelocity(), angle);

			angle.y = math::normalize_yaw(movement_system->wish_angle.y - angle.y);

			Vector direction;
			math::angle_vectors(angle, &direction, nullptr, nullptr);

			auto stop = direction * ctx->local()->m_vecVelocity().Length2D(); //-V688

			command.forwardmove = stop.x;
			command.sidemove = stop.y;

			engine_prediction->start(&command);
		}

		engine_prediction->apply_restore_data(restore_data);

		if (ticks_to_stop && config->rage.weapon[ctx->weapon_config].modifiers[MODIFIERS_EARLY])
		{
			auto predicted_shoot_position = ctx->shoot_position + ctx->local()->m_vecVelocity() * globals->intervalpertick * (float)ticks_to_stop;
			auto backup_shoot_position = ctx->shoot_position;

			ctx->shoot_position = predicted_shoot_position;

			optimized_scan = true;
			scan();
			optimized_scan = false;

			ctx->shoot_position = backup_shoot_position;
			final_target.reset();
		}
	}

	scan();

	if (valid_stop && (stop || early_stop))
	{
		full_stop = ctx->local()->m_vecVelocity().Length2D() + 1.0f > ctx->max_speed * 0.34f;

		if (!skip_automatic_scope) //-V648
		{
			auto zoom_weapon = ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_SCAR20 || ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_G3SG1 || ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_SSG08 || ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_AWP;

			if (zoom_weapon && !ctx->weapon()->m_zoomLevel())
			{
				if (cmd->buttons & IN_ATTACK)
					cmd->buttons &= ~IN_ATTACK;

				cmd->buttons |= IN_ATTACK2;
			}
		}
	}
	else
	{
		cmd->forwardmove = backup_forwardmove;
		cmd->sidemove = backup_sidemove;

		engine_prediction->apply_restore_data();
		engine_prediction->start(cmd);
		engine_prediction->update_data();
	}

	fire(cmd);

	for (auto& data : backup)
		data.apply(MATRIX_MAIN, true);
}

void Aim::automatic_revolver(crypt_ptr <CUserCmd> cmd)
{
	if (cmd->buttons & IN_ATTACK)
		return;

	if (ctx->weapon()->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
		return;

	if (!engine->IsActiveApp())
		return;

	auto time = TICKS_TO_TIME(ctx->original_tickbase + 1);

	if (time >= ctx->local()->m_flNextAttack())
	{
		auto second_attack = time >= ctx->weapon()->m_flNextPrimaryAttack() && ctx->weapon()->m_Activity() == ACT_VM_HAULBACK && time > ctx->weapon()->m_flPostponeFireReadyTime();

		if (second_attack && time < ctx->weapon()->m_flNextSecondaryAttack())
			cmd->buttons |= IN_ATTACK2;
		else if (!second_attack)
			cmd->buttons |= IN_ATTACK;
	}

	if (!ctx->weapon()->m_iClip1() && ctx->weapon()->m_iPrimaryReserveAmmoCount() > 0)
		cmd->buttons &= ~IN_ATTACK | IN_RELOAD;

	ctx->automatic_revolver = true;
}

void Aim::prepare()
{
	vector <PreparedTarget> prepared_targets;

	for (auto i = 1; i <= globals->maxclients; ++i)
	{
		auto player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(i));

		if (player.get() == ctx->local().get())
			continue;

		if (!player->valid(!ctx->friendly_fire))
			continue;

		if (animations->animation_data[i].empty())
			continue;

		if (config->player_list.player_settings[i].ignore)
			continue;

		prepared_targets.emplace_back(PreparedTarget(i, player));
	}

	sort(prepared_targets.begin(), prepared_targets.end(),
		[](PreparedTarget& first, PreparedTarget& second)
		{
			return first.data->front().origin.DistTo(ctx->shoot_position) < second.data->front().origin.DistTo(ctx->shoot_position);
		}
	);

	if (additional_index >= prepared_targets.size())
		additional_index = 0;

	for (auto i = 0; i < prepared_targets.size(); ++i)
	{
		auto index = (i + additional_index) % prepared_targets.size();
		auto target = crypt_ptr <PreparedTarget>(&prepared_targets.at(index));

		if (!target)
			continue;

		targets.emplace_back(PreparedTarget(target->index, target->player));

		if (targets.size() >= 3)
			break;
	}

	additional_index += 3;
}

struct AimPlayer
{
	crypt_ptr <AnimationData> data;
	crypt_ptr <Player> player;
	vector<Hitbox> hitboxes;
};

inline void thread_aim(crypt_ptr<Player> player, crypt_ptr <AnimationData> data, vector<Hitbox>& hitboxes)
{
	aim->scan_hitboxes_new(player, data, hitboxes);
}

void Aim::scan()
{
	vector<AimPlayer> aim_players{};
	aim_players.reserve(250);

	auto valid_count = 0;
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

		vector <crypt_ptr <AnimationData>> scan_data;

		//if (target.data->size() > 0 && resolver[target.index]->should_resolve)
		//{
		//	auto first_data = crypt_ptr <AnimationData>(&target.data->front());

		//	if (first_data->valid())
		//	{
		//		first_data->apply(MATRIX_FIRST);

		//		auto first_position = target.player->get_hitbox_position(HITBOX_HEAD, first_data->matrix[MATRIX_FIRST], true);
		//		auto first_penetration_info = penetration->run(ctx->shoot_position, first_position, target.player, true);

		//		/*debugoverlay->BoxOverlay(first_position, Vector(-1.5f, -1.5f, -1.5f), Vector(1.5f, 1.5f, 1.5f), QAngle(0.0f, 0.0f, 0.0f),
		//			255,
		//			0,
		//			0,
		//			255, 2 * globals->intervalpertick);*/

		//		if (first_penetration_info.hitgroup == HITGROUP_HEAD)
		//		{
		//			first_data->apply(MATRIX_SECOND);

		//			auto second_position = target.player->get_hitbox_position(HITBOX_HEAD, first_data->matrix[MATRIX_SECOND], true);
		//			auto second_penetration_info = penetration->run(ctx->shoot_position, second_position, target.player, true);

		//			/*debugoverlay->BoxOverlay(second_position, Vector(-1.5f, -1.5f, -1.5f), Vector(1.5f, 1.5f, 1.5f), QAngle(0.0f, 0.0f, 0.0f),
		//				255,
		//				255,
		//				0,
		//				255, 2 * globals->intervalpertick);*/

		//			if (second_penetration_info.hitgroup == HITGROUP_HEAD)
		//			{
		//				if (min(first_penetration_info.damage, second_penetration_info.damage) < 1
		//					|| abs(first_penetration_info.damage - second_penetration_info.damage) >= 10)
		//				{
		//					auto freestand_side = 0;

		//					if (first_penetration_info.damage < second_penetration_info.damage)
		//						freestand_side = RESOLVER_SIDE_FIRST;
		//					else
		//						freestand_side = RESOLVER_SIDE_SECOND;

		//					resolver[target.index]->freestand_side = freestand_side;
		//					resolver[target.index]->freestand_time = globals->realtime;
		//				}
		//			}
		//		}
		//	}
		//}

		if (target.data->size() > 3)
		{
			auto first_data = get_data(FIRST, target.data);

			if (first_data)
				scan_data.emplace_back(first_data);

			auto best_data = get_data(BEST, target.data);

			if (best_data && best_data.get() != first_data.get())
				scan_data.emplace_back(best_data);
			else
			{
				auto last_data = get_data(LAST, target.data);

				if (last_data && last_data.get() != first_data.get())
					scan_data.emplace_back(last_data);
			}
		}
		else
		{
			for (auto i = 0; i < target.data->size(); ++i)
			{
				auto data = crypt_ptr <AnimationData>(&target.data->at(i));

				if (!data->valid())
					continue;

				scan_data.emplace_back(data);
			}
		}

		if (scan_data.empty())
		{
			if (target.data->size() >= 2)
			{
				auto last_data = crypt_ptr <AnimationData>(&target.data->front());
				auto previous_data = crypt_ptr <AnimationData>(&target.data->at(1));

				if ((!convars_manager->convars[CONVAR_CL_LAGCOMPENSATION]->GetBool() || (last_data->origin - previous_data->origin).LengthSqr() > 64.0f) && last_data->choke > 0 && last_data->choke < 20 && (!(last_data->flags & FL_ONGROUND) || last_data->layers[LAYERS_ORIGINAL][6].m_flPlaybackRate > 0.0f))
				{
					auto latency = clamp(TICKS_TO_TIME(engine_prediction->latency), 0.0f, 1.0f);
					auto correct = clamp(latency + ctx->interpolation, 0.0f, convars_manager->convars[CONVAR_SV_MAXUNLAG]->GetFloat());
					auto delta_time = correct - (TICKS_TO_TIME(ctx->tickbase) - last_data->simulation_time);
					auto predicted_tick = ((int)clientstate->m_ClockDriftMgr.m_nServerTick + TIME_TO_TICKS(latency) - last_data->server_tick) / last_data->choke;

					if (predicted_tick > 0 && predicted_tick < 20)
					{
						auto max_backtrack_time = ceil(((delta_time - 0.2f) / globals->intervalpertick + 0.5f) / (float)last_data->choke);
						auto prediction_ticks = predicted_tick;

						if (max_backtrack_time > 0.0f && predicted_tick >= TIME_TO_TICKS(max_backtrack_time))
							prediction_ticks = TIME_TO_TICKS(max_backtrack_time);

						if (prediction_ticks > 0)
						{
							target.extrapolated_data = *last_data.get();

							do
							{
								for (auto current_prediction_tick = 0; current_prediction_tick < last_data->choke; ++current_prediction_tick)
									extrapolate(target.player, &target.extrapolated_data, last_data, previous_data);

								--prediction_ticks;
							} while (prediction_ticks);

							for (auto i = (int)MATRIX_MAIN; i < MATRIX_MAX; ++i)
							{
								for (auto j = 0; j < target.extrapolated_data.bone_count; ++j)
								{
									target.extrapolated_data.matrix[i][j][0][3] -= last_data->origin.x;
									target.extrapolated_data.matrix[i][j][1][3] -= last_data->origin.y;
									target.extrapolated_data.matrix[i][j][2][3] -= last_data->origin.z;

									target.extrapolated_data.matrix[i][j][0][3] += target.extrapolated_data.origin.x;
									target.extrapolated_data.matrix[i][j][1][3] += target.extrapolated_data.origin.y;
									target.extrapolated_data.matrix[i][j][2][3] += target.extrapolated_data.origin.z;
								}
							}

							scan_data.emplace_back(&target.extrapolated_data);
						}
					}
				}
			}

			if (scan_data.empty())
				continue;
		}

		for (auto& data : scan_data)
		{
			data->apply();

			if (optimized_scan || !ctx->mt_point_scan)
			{
				scan_hitboxes(target.player, data);

				if (early_stop)
					return;
			}
			else
			{
				auto& aim_player = aim_players.emplace_back();
				aim_player.data = data;
				aim_player.player = target.player;

				data->apply();

				++valid_count;
				thread_pool->add_task(thread_aim, target.player, data, std::ref(aim_player.hitboxes));
			}
		}
	}

	if (!optimized_scan)
	{
		if (ctx->mt_point_scan)
		{
			if (valid_count < 1)
				return;

			thread_pool->wait_all();

			for (auto& aim : aim_players)
			{
				auto hit_chance = exploits->double_tap_shot ? (float)config->rage.weapon[ctx->weapon_config].double_tap_hit_chance : (float)config->rage.weapon[ctx->weapon_config].hit_chance;
				auto minimum_damage = config->rage.weapon[ctx->weapon_config].minimum_damage;

				if (minimum_damage > 100)
					minimum_damage = clamp(minimum_damage, 1, aim.player->m_iHealth() + minimum_damage - 100);
				else
					minimum_damage = clamp(minimum_damage, 1, aim.player->m_iHealth());

				if (config->rage.force_damage_key.state)
				{
					auto force_damage = config->rage.weapon[ctx->weapon_config].force_damage_value;

					if (force_damage > 100)
						minimum_damage = clamp(force_damage, 1, aim.player->m_iHealth() + force_damage - 100);
					else
						minimum_damage = clamp(force_damage, 1, aim.player->m_iHealth());
				}

				aim.data->apply();

				for (auto& hitbox : aim.hitboxes)
				{
					if (hitbox.points.empty())
						continue;

					for (auto& point : hitbox.points)
					{
						auto angle = math::calculate_angle(ctx->shoot_position, point.point);

						if (hitbox.hitbox == HITBOX_HEAD)
						{
							auto valid_head_point = is_valid_head_point(aim.player, aim.data, MATRIX_MAIN, point.point);
							if (!valid_head_point)
								continue;
						}

						if (point.penetration_info.damage < minimum_damage)
							continue;

						if (point.penetration_info.penetration_count < final_target.penetration_count)
							continue;

						if (!hitbox_equal(point.penetration_info.hitbox, hitbox.hitbox))
							continue;

						stop = true;

						if (!skip_automatic_scope)
						{
							if (point.low_accuracy >= (hit_chance * 0.01f))
								skip_automatic_scope = true;
						}

						if (point.accuracy < (hit_chance * 0.01f) && !jump_scout)
							continue;

						if (point.penetration_info.damage > final_target.damage)
						{
							final_target.visible = point.penetration_info.visible;
							final_target.damage = point.penetration_info.damage;
							final_target.hitbox = hitbox.hitbox;
							final_target.hitgroup = point.penetration_info.hitgroup;
							final_target.penetration_count = point.penetration_info.penetration_count;
							final_target.point = point;
							final_target.player = aim.player;
							final_target.data = aim.data;
						}
					}
				}
			}
		}

		if (final_target.data)
			additional_index -= 3;
	}
}

void Aim::fire(crypt_ptr <CUserCmd> cmd)
{
	if (!final_target.data)
		return;

	if (exploits->charging)
		return;

	if (full_stop && config->rage.weapon[ctx->weapon_config].modifiers[MODIFIERS_FORCE_ACCURACY])
		return;

	if (cmd->buttons & IN_ATTACK2)
		return;

	if (!ctx->local()->can_shoot())
		return;

	if (!config->rage.automatic_fire && !(cmd->buttons & IN_ATTACK && !ctx->automatic_revolver))
		return;

	final_target.data->apply();
	auto angle = math::calculate_angle(ctx->shoot_position, final_target.point.point);

	if (!jump_scout && !exploits->double_tap_shot && ctx->weapon_config != WEAPON_CONFIG_TASER && !is_hit_chanced((float)config->rage.weapon[ctx->weapon_config].hit_chance, angle, MATRIX_MAIN, final_target.hitbox, final_target.player, final_target.data, true, (int)min((float)final_target.damage * (float)config->rage.weapon[ctx->weapon_config].hit_chance * 0.01f, (float)final_target.player->m_iHealth())))
		return;

	cmd->buttons |= IN_ATTACK;
	cmd->viewangles = angle;
	cmd->tickcount = TIME_TO_TICKS(final_target.data->simulation_time + ctx->interpolation);

	if (!config->rage.silent)
		engine->SetViewAngles(cmd->viewangles);

	auto backtrack_ticks = 0;

	if (convars_manager->convars[CONVAR_CL_LAGCOMPENSATION]->GetBool())
	{
		auto original_tickbase = ctx->tickbase;

		if (exploits->hide_shots || exploits->double_tap)
			original_tickbase += exploits->target_tickbase_shift;

		auto correct = clamp(TICKS_TO_TIME(engine_prediction->latency) + ctx->interpolation, 0.0f, convars_manager->convars[CONVAR_SV_MAXUNLAG]->GetFloat());
		auto delta_time = correct - (TICKS_TO_TIME(original_tickbase) - final_target.data->simulation_time);

		backtrack_ticks = TIME_TO_TICKS(abs(delta_time));
	}

	auto hit_chance = get_hit_chance(angle, MATRIX_MAIN, final_target.hitbox, final_target.player, final_target.data);

	auto get_hitbox_name = [](int hitbox) -> string
	{
		switch (hitbox)
		{
		case HITBOX_HEAD:
			return crypt_str("head");
		case HITBOX_UPPER_CHEST:
			return crypt_str("upper chest");
		case HITBOX_CHEST:
			return crypt_str("chest");
		case HITBOX_LOWER_CHEST:
			return crypt_str("lower chest");
		case HITBOX_STOMACH:
			return crypt_str("stomach");
		case HITBOX_PELVIS:
			return crypt_str("pelvis");
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_RIGHT_FOREARM:
			return crypt_str("right arm");
		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_LEFT_FOREARM:
			return crypt_str("left arm");
		case HITBOX_RIGHT_THIGH:
		case HITBOX_RIGHT_CALF:
			return crypt_str("right leg");
		case HITBOX_LEFT_THIGH:
		case HITBOX_LEFT_CALF:
			return crypt_str("left leg");
		case HITBOX_RIGHT_FOOT:
			return crypt_str("right foot");
		case HITBOX_LEFT_FOOT:
			return crypt_str("left foot");
		}
	};

#if BETA
	string log;

	player_info_t player_info;
	engine->GetPlayerInfo(final_target.data->i, &player_info);

	auto get_resolver_type = [](int type) -> string
	{
		/*switch (type)
		{
		case RESOLVER_TYPE_ORIGINAL:
			return crypt_str("original");
		case RESOLVER_TYPE_OPPOSITE:
			return crypt_str("opposite");
		case RESOLVER_TYPE_FREESTAND:
			return crypt_str("freestand");
		case RESOLVER_TYPE_DIRECTIONAL:
			return crypt_str("directional");
		case RESOLVER_TYPE_ANIMATIONS:
			return crypt_str("animations");
		case RESOLVER_TYPE_ANIMATIONS_HISTORY:
			return crypt_str("animations history");
		case RESOLVER_TYPE_SHOT_OPPOSITE:
			return crypt_str("shot opposite");
		case RESOLVER_TYPE_SHOT_NO_OPPOSITE:
			return crypt_str("shot no opposite");
		case RESOLVER_TYPE_SHOT_FAKE:
			return crypt_str("shot fake");
		case RESOLVER_TYPE_BRUTEFORCE:
			return crypt_str("bruteforce");
		case RESOLVER_TYPE_CUSTOM:
			return crypt_str("custom");
		}*/
		return crypt_str("custom");
	};

	auto get_resolver_side = [](int side) -> string
	{
		/*switch (side)
		{
		case RESOLVER_SIDE_ORIGINAL:
			return crypt_str("original");
		case RESOLVER_SIDE_ZERO:
			return crypt_str("zero");
		case RESOLVER_SIDE_FIRST:
			return crypt_str("first");
		case RESOLVER_SIDE_SECOND:
			return crypt_str("second");
		case RESOLVER_SIDE_CUSTOM:
			return crypt_str("custom");
		}*/
		return crypt_str("custom");
	};

	log += crypt_str("Fired shot at ") + (string)player_info.szName;
	log += crypt_str(", hitbox: ") + get_hitbox_name(final_target.hitbox);
	log += crypt_str(", scale: ") + to_string(final_target.point.scale);
	log += crypt_str(", damage: ") + to_string(final_target.damage);
	log += crypt_str(", hitchance: ") + to_string(hit_chance);
	log += crypt_str(", backtrack: ") + to_string(backtrack_ticks);
	log += crypt_str(", choke: ") + to_string(final_target.data->choke);
	log += crypt_str(", safe: ") + to_string(final_target.point.safe);
	log += crypt_str(", penetration count: ") + to_string(final_target.penetration_count);
	log += crypt_str(", resolver type: ") + get_resolver_type(final_target.data->resolver_type);
	log += crypt_str(", resolver side: ") + get_resolver_side(final_target.data->resolver_side);

	logs->add(log, Color::LightBlue, crypt_str("[ SHOT ] "));
#endif

	Shot shot;

	shot.safe = final_target.point.safe >= 5;
	shot.index = final_target.data->i;
	shot.tickcount = globals->tickcount;
	shot.hitgroup = final_target.hitgroup;
	shot.distance = ctx->shoot_position.DistTo(final_target.point.point);
	shot.shoot_position = ctx->shoot_position;
	shot.player = final_target.player;
	shot.data = *final_target.data.get();

	shot.shot_info.client_hitbox = get_hitbox_name(final_target.hitbox);
	shot.shot_info.safe = final_target.point.safe >= 3;
	shot.shot_info.target_index = final_target.data->i;
	shot.shot_info.client_damage = final_target.damage;
	shot.shot_info.hitchance = hit_chance;
	shot.shot_info.backtrack_ticks = backtrack_ticks;
	shot.shot_info.aim_point = final_target.point.point;

	shots.emplace_back(shot);

	ctx->shots_data.emplace_front(ShotData(cmd->command_number, ctx->local()->m_flVelocityModifier(), globals->curtime));
	ctx->automatic_revolver = false;
}

void Aim::automatic_stop(crypt_ptr <CUserCmd> cmd)
{
	if (!config->rage.enable)
		return;

	if (!ctx->weapon()->can_fire())
		return;

	backup_forwardmove = cmd->forwardmove;
	backup_sidemove = cmd->sidemove;

	if (!config->rage.weapon[ctx->weapon_config].automatic_stop)
		return;

	if (ctx->weapon_config == WEAPON_CONFIG_INVALID)
		return;

	if (ctx->weapon_config == WEAPON_CONFIG_TASER)
		return;

	if (ctx->weapon_config == WEAPON_CONFIG_KNIFE)
		return;

	if (!(engine_prediction->flags & FL_ONGROUND))
		return;

	if (ctx->weapon()->m_bIsReloading())
		return;

	if (TICKS_TO_TIME(ctx->tickbase) < ctx->local()->m_flNextAttack())
		return;

	if (!ctx->local()->can_shoot() && config->rage.weapon[ctx->weapon_config].modifiers[MODIFIERS_MOVE_BETWEEN_SHOTS])
		return;

	auto velocity = engine_prediction->velocity;
	velocity.z = 0.0f;

	movement_system->friction(&velocity);

	if (full_stop || velocity.Length2D() + 1.0f > ctx->max_speed * 0.33f)
	{
		Vector angle;
		math::vector_angles(-velocity, angle);

		angle.y = math::normalize_yaw(movement_system->wish_angle.y - angle.y);

		Vector direction;
		math::angle_vectors(angle, &direction, nullptr, nullptr);

		auto stop = direction * engine_prediction->velocity.Length2D(); //-V688

		cmd->forwardmove = stop.x;
		cmd->sidemove = stop.y;
	}
	else
		movement_system->slow_walk(cmd);
}

crypt_ptr <AnimationData> Aim::get_data(int type, crypt_ptr <deque <AnimationData>> data) //-V813
{
	auto& crypt_record = *data.get();
	switch (type)
	{
	case FIRST:
	{
		auto record = std::find_if(crypt_record.begin(), crypt_record.end(), [&](AnimationData& data)
			{
				return data.valid()
					&& data.old_simulation_time > 0.0f
					&& data.simulation_time > data.old_simulation_time;
			});

		if (record != crypt_record.end())
			return &*record;

		return nullptr;
	}
	break;
	case LAST:
	{
		auto record = std::find_if(crypt_record.rbegin(), crypt_record.rend(), [&](AnimationData& data)
			{
				return data.valid()
					&& data.old_simulation_time > 0.0f
					&& data.simulation_time > data.old_simulation_time;
			});

		if (record != crypt_record.rend())
			return &*record;

		return nullptr;
	}
	break;
	case BEST:
	{
		auto best_shot = false;
		auto best_animations_resolver = false;
		auto best_duck_amount = 0.5f;
		auto best_speed = -1.f;

		crypt_ptr <AnimationData> best_data;

		for (auto i = 0; i < crypt_record.size(); ++i)
		{
			auto current_data = crypt_ptr <AnimationData>(&crypt_record.at(i));

			if (!current_data->valid())
				continue;

			if (current_data->old_simulation_time <= 0.0f)
				continue;

			if (current_data->old_simulation_time >= current_data->simulation_time)
				continue;

			// just a temp record selection cuz i don't have resolver 
			if (best_speed < current_data->velocity.Length2D())
			{
				best_data = current_data;
				best_speed = current_data->velocity.Length2D();
			}
			/*auto current_animations_resolver = current_data->resolver_type == RESOLVER_TYPE_ANIMATIONS;

			if (current_data->shot == best_shot)
			{
				if (current_animations_resolver == best_animations_resolver)
				{
					if (current_data->duck_amount < best_duck_amount)
					{
						best_duck_amount = current_data->duck_amount;
						best_data = current_data;
					}
					else
						best_duck_amount = current_data->duck_amount;
				}
				else if (current_animations_resolver)
				{
					best_animations_resolver = current_animations_resolver;
					best_data = current_data;
				}
			}
			else if (current_data->shot)
			{
				best_shot = current_data->shot;
				best_animations_resolver = current_animations_resolver;
				best_data = current_data;
			}*/
		}

		return best_data;
	}
	break;
	}
}

bool Aim::hitbox_equal(int first, int second)
{
	auto get_hitgroup = [](int hitbox) -> int
	{
		switch (hitbox)
		{
		case HITBOX_HEAD:
		case HITBOX_NECK:
			return HIT_GROUP_HEAD;
		case HITBOX_PELVIS:
		case HITBOX_STOMACH:
			return HIT_GROUP_BODY;
		case HITBOX_LOWER_CHEST:
		case HITBOX_CHEST:
		case HITBOX_UPPER_CHEST:
			return HIT_GROUP_CHEST;
		case HITBOX_RIGHT_THIGH:
		case HITBOX_LEFT_THIGH:
		case HITBOX_RIGHT_CALF:
		case HITBOX_LEFT_CALF:
		case HITBOX_RIGHT_FOOT:
		case HITBOX_LEFT_FOOT:
			return HIT_GROUP_LEGS;
		case HITBOX_RIGHT_HAND:
		case HITBOX_LEFT_HAND:
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_RIGHT_FOREARM:
		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_LEFT_FOREARM:
			return HIT_GROUP_ARMS;
		}
	};

	return get_hitgroup(first) == get_hitgroup(second);
}

#pragma optimize("", off)
bool Aim::clip_ray_to_hitbox(const Ray_t& ray, crypt_ptr <mstudiobbox_t> hitbox, matrix3x4_t& matrix, CGameTrace& trace)
{
	trace.fraction = 1.0f;
	trace.startsolid = false;

	return reinterpret_cast <int(__fastcall*)(const Ray_t&, mstudiobbox_t*, matrix3x4_t&, CGameTrace&)> (signatures_manager->signatures[SIGNATURE_CLIP_RAY_TO_HITBOX])(ray, hitbox.get(), matrix, trace) >= 0;
}
#pragma optimize("", on)

bool Aim::is_valid_head_point(crypt_ptr <Player> player, crypt_ptr <AnimationData> data, int matrix, const Vector& point)
{
	auto hdr = crypt_ptr <studiohdr_t>(modelinfo->GetStudioModel(player->GetModel()));

	if (!hdr)
		return false;

	auto hitbox_set = crypt_ptr <mstudiohitboxset_t>(hdr->pHitboxSet(player->m_nHitboxSet()));

	if (!hitbox_set)
		return false;

	auto hitbox = crypt_ptr <mstudiobbox_t>(hitbox_set->pHitbox(HITBOX_HEAD));

	if (!hitbox)
		return false;

	Ray_t ray;
	ray.Init(ctx->shoot_position, point);

	CGameTrace main_trace;

	if (!clip_ray_to_hitbox(ray, hitbox, data->matrix[matrix][hitbox->bone], main_trace))
		return false;

	CGameTrace additional_trace;

	auto hitbox_index = 0;
	auto main_fraction = main_trace.fraction;

	do
	{
		auto current_hitbox = hitbox_set->pHitbox(hitbox_index);
		auto valid_hitbox = current_hitbox->radius > 0.0f && (current_hitbox->group == HITGROUP_CHEST || current_hitbox->group == HITGROUP_STOMACH);

		if (valid_hitbox && clip_ray_to_hitbox(ray, current_hitbox, data->matrix[matrix][current_hitbox->bone], additional_trace) && main_fraction > additional_trace.fraction)
			return false;

		++hitbox_index;
	} while (hitbox_index < hitbox_set->numhitboxes);

	return true;
}

void Aim::scan_hitboxes(crypt_ptr <Player> player, crypt_ptr <AnimationData> data) //-V813
{
	vector <Hitbox> hitboxes; //-V827

	if (config->rage.weapon[ctx->weapon_config].hitboxes[2])
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[2] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[2] == 2, HITBOX_STOMACH));

	if (config->rage.weapon[ctx->weapon_config].hitboxes[1])
	{
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[1] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[1] == 2, HITBOX_LOWER_CHEST));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[1] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[1] == 2, HITBOX_UPPER_CHEST));
	}

	if (config->rage.weapon[ctx->weapon_config].hitboxes[0])
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[0] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[0] == 2, HITBOX_HEAD));

	if (config->rage.weapon[ctx->weapon_config].hitboxes[3])
	{
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 2, HITBOX_RIGHT_THIGH));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 2, HITBOX_LEFT_THIGH));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 2, HITBOX_RIGHT_CALF));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 2, HITBOX_LEFT_CALF));
	}

	if (config->rage.weapon[ctx->weapon_config].hitboxes[4])
	{
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[4] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[4] == 2, HITBOX_RIGHT_FOOT));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[4] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[4] == 2, HITBOX_LEFT_FOOT));
	}

	if (config->rage.weapon[ctx->weapon_config].hitboxes[5])
	{
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 2, HITBOX_RIGHT_UPPER_ARM));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 2, HITBOX_LEFT_UPPER_ARM));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 2, HITBOX_RIGHT_FOREARM));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 2, HITBOX_LEFT_FOREARM));
	}

	auto hit_chance = exploits->double_tap_shot ? (float)config->rage.weapon[ctx->weapon_config].double_tap_hit_chance : (float)config->rage.weapon[ctx->weapon_config].hit_chance;
	auto minimum_damage = config->rage.weapon[ctx->weapon_config].minimum_damage;

	if (minimum_damage > 100)
		minimum_damage = clamp(minimum_damage, 1, player->m_iHealth() + minimum_damage - 100);
	else
		minimum_damage = clamp(minimum_damage, 1, player->m_iHealth());

	if (config->rage.force_damage_key.state)
	{
		auto force_damage = config->rage.weapon[ctx->weapon_config].force_damage_value;

		if (force_damage > 100)
			minimum_damage = clamp(force_damage, 1, player->m_iHealth() + force_damage - 100);
		else
			minimum_damage = clamp(force_damage, 1, player->m_iHealth());
	}

	auto body_aim = true;
	auto found_safe = 0;

	Vector view_angle;
	engine->GetViewAngles(view_angle);
	for (auto& hitbox : hitboxes)
	{
		auto none_body_hitbox = hitbox.hitbox < HITBOX_PELVIS || hitbox.hitbox > HITBOX_UPPER_CHEST;

		if (body_aim && none_body_hitbox && (config->rage.weapon[ctx->weapon_config].body_aim || config->player_list.player_settings[data->i].force_body_aim || config->rage.weapon[ctx->weapon_config].force_body_aim_if_lethal && player->m_iHealth() <= ctx->weapon_data()->damage)) //-V648
		{
			body_aim = false;

			if (config->rage.weapon[ctx->weapon_config].body_aim == 2 || config->player_list.player_settings[data->i].force_body_aim || config->rage.weapon[ctx->weapon_config].force_body_aim_if_lethal && player->m_iHealth() <= ctx->weapon_data()->damage) //-V648
				break;
			else if (final_target.damage >= minimum_damage)
				break;
			else if (config->rage.weapon[ctx->weapon_config].body_aim && exploits->double_tap && (float)final_target.damage >= (float)player->m_iHealth() * 0.5f)
				break;
		}

		get_points(hitbox.points, hitbox.hitbox, player, data);

		if (hitbox.points.empty())
			continue;

		for (auto& point : hitbox.points)
		{
			if (!config->rage.weapon[ctx->weapon_config].static_point_scale && point.scale >= 1.0f)
				continue;

			auto angle = math::calculate_angle(ctx->shoot_position, point.point);
			auto field_of_view = math::get_field_of_view(view_angle, angle);

			if (field_of_view > (float)config->rage.field_of_view)
				continue;

			if (hitbox.hitbox == HITBOX_HEAD)
			{
				auto valid_head_point = is_valid_head_point(player, data, MATRIX_MAIN, point.point);

				if (!valid_head_point)
					continue;
			}

			for (auto matrix = (int)MATRIX_ZERO; matrix < MATRIX_MAX; ++matrix)
				if (hitbox_intersection(hitbox.hitbox, matrix, player, data, point.point))
					++point.safe;

			if (point.safe < 5)
			{
				if (!point.safe)
					continue;
				else if (hitbox.force_safe || config->player_list.player_settings[data->i].force_safe_points || config->rage.weapon[ctx->weapon_config].force_safe_points_if_lethal && player->m_iHealth() <= ctx->weapon_data()->damage) //-V648
					continue;
				else if (hitbox.prefer_safe && point.safe < found_safe)
					continue;
			}

			point.penetration_info = penetration->run(ctx->shoot_position, point.point, player, config->rage.automatic_wall);

			if (point.penetration_info.damage < minimum_damage)
				continue;

			if (point.penetration_info.penetration_count < final_target.penetration_count)
				continue;

			if (!hitbox_equal(point.penetration_info.hitbox, hitbox.hitbox))
				continue;

			if (optimized_scan)
			{
				early_stop = true;

				if (!skip_automatic_scope)
				{
					auto backup_inaccuracy = ctx->inaccuracy;
					ctx->inaccuracy = engine_prediction->flags & FL_DUCKING ? ctx->weapon_data()->inaccuracy_crouch : ctx->weapon_data()->inaccuracy_stand;

					if (is_hit_chanced(hit_chance, angle, MATRIX_MAIN, hitbox.hitbox, player, data))
						skip_automatic_scope = true;

					ctx->inaccuracy = backup_inaccuracy;
				}

				return;
			}
			else
			{
				stop = true;

				if (!skip_automatic_scope)
				{
					auto backup_inaccuracy = ctx->inaccuracy;
					ctx->inaccuracy = engine_prediction->flags & FL_DUCKING ? ctx->weapon_data()->inaccuracy_crouch : ctx->weapon_data()->inaccuracy_stand;

					if (is_hit_chanced(hit_chance, angle, MATRIX_MAIN, hitbox.hitbox, player, data))
						skip_automatic_scope = true;

					ctx->inaccuracy = backup_inaccuracy;
				}
			}

			if (!is_hit_chanced(hit_chance, angle, MATRIX_MAIN, hitbox.hitbox, player, data) && !jump_scout)
				continue;

			if (point.penetration_info.damage > final_target.damage)
			{
				if (point.safe >= 2)
					found_safe = point.safe;

				final_target.visible = point.penetration_info.visible;
				final_target.damage = point.penetration_info.damage;
				final_target.hitbox = hitbox.hitbox;
				final_target.hitgroup = point.penetration_info.hitgroup;
				final_target.penetration_count = point.penetration_info.penetration_count;
				final_target.point = point;
				final_target.player = player;
				final_target.data = data;
			}
		}
	}
}

void Aim::scan_hitboxes_new(crypt_ptr <Player> player, crypt_ptr <AnimationData> data, vector<Hitbox>& hitboxes)
{
	if (config->rage.weapon[ctx->weapon_config].hitboxes[2])
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[2] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[2] == 2, HITBOX_STOMACH));

	if (config->rage.weapon[ctx->weapon_config].hitboxes[1])
	{
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[1] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[1] == 2, HITBOX_LOWER_CHEST));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[1] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[1] == 2, HITBOX_UPPER_CHEST));
	}

	if (config->rage.weapon[ctx->weapon_config].hitboxes[0])
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[0] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[0] == 2, HITBOX_HEAD));

	if (config->rage.weapon[ctx->weapon_config].hitboxes[3])
	{
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 2, HITBOX_RIGHT_THIGH));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 2, HITBOX_LEFT_THIGH));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 2, HITBOX_RIGHT_CALF));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[3] == 2, HITBOX_LEFT_CALF));
	}

	if (config->rage.weapon[ctx->weapon_config].hitboxes[4])
	{
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[4] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[4] == 2, HITBOX_RIGHT_FOOT));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[4] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[4] == 2, HITBOX_LEFT_FOOT));
	}

	if (config->rage.weapon[ctx->weapon_config].hitboxes[5])
	{
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 2, HITBOX_RIGHT_UPPER_ARM));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 2, HITBOX_LEFT_UPPER_ARM));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 2, HITBOX_RIGHT_FOREARM));
		hitboxes.emplace_back(Hitbox(config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 1, config->rage.weapon[ctx->weapon_config].safe_hitboxes[5] == 2, HITBOX_LEFT_FOREARM));
	}

	Vector view_angle;
	engine->GetViewAngles(view_angle);

	auto spread = ctx->inaccuracy + ctx->spread;
	auto low_inacc = engine_prediction->flags & FL_DUCKING ? ctx->weapon_data()->inaccuracy_crouch : ctx->weapon_data()->inaccuracy_stand;
	auto low_spread = low_inacc + ctx->spread;

	for (auto& hitbox : hitboxes)
	{
		get_points(hitbox.points, hitbox.hitbox, player, data);

		if (hitbox.points.empty())
			continue;

		for (auto& point : hitbox.points)
		{
			if (!config->rage.weapon[ctx->weapon_config].static_point_scale && point.scale >= 1.0f)
				continue;

			auto angle = math::calculate_angle(ctx->shoot_position, point.point);
			auto field_of_view = math::get_field_of_view(view_angle, angle);

			if (field_of_view > (float)config->rage.field_of_view)
				continue;

			for (auto matrix = (int)MATRIX_ZERO; matrix < MATRIX_MAX; ++matrix)
				if (hitbox_intersection(hitbox.hitbox, matrix, player, data, point.point))
					++point.safe;

			point.penetration_info = penetration->run(ctx->shoot_position, point.point, player, config->rage.automatic_wall);

			point.accuracy = get_point_accuracy(angle, MATRIX_MAIN, hitbox.hitbox, spread, player, data);
			point.low_accuracy = get_point_accuracy(angle, MATRIX_MAIN, hitbox.hitbox, low_spread, player, data);
		}
	}
}

void Aim::get_points(vector <Point>& points, int hitbox_index, crypt_ptr <Player> player, crypt_ptr <AnimationData> data) //-V813
{
	auto hitbox = player->get_hitbox(hitbox_index);

	if (!hitbox)
		return;

	auto center = player->get_hitbox_position(hitbox_index, data->matrix[MATRIX_MAIN]);

	if (hitbox_index >= HITBOX_RIGHT_THIGH)
	{
		points.emplace_back(Point(center, 0.0f));
		return;
	}

	vector <Point> current_points; //-V827

	auto spread = ctx->spread + ctx->inaccuracy;
	auto distance = center.DistTo(ctx->shoot_position);

	distance /= DirectX::XMScalarSinEst(DirectX::XMConvertToRadians(90.0f - DirectX::XMConvertToDegrees(spread)));
	spread = DirectX::XMScalarSinEst(spread);

	auto radius = max(hitbox->radius - distance * spread, 0.0f);
	auto scale = clamp(radius / hitbox->radius, 0.0f, 1.0f);

	if (scale <= 0.0f)
		scale = 1.0f;

	if (config->rage.weapon[ctx->weapon_config].static_point_scale)
		scale = hitbox_index == HITBOX_HEAD ? config->rage.weapon[ctx->weapon_config].head_scale : config->rage.weapon[ctx->weapon_config].body_scale;

	Vector min;
	math::vector_transform(hitbox->bbmin, data->matrix[MATRIX_MAIN][hitbox->bone], min);

	Vector max;
	math::vector_transform(hitbox->bbmax, data->matrix[MATRIX_MAIN][hitbox->bone], max);

	auto delta = (center - ctx->shoot_position).Normalized();
	auto max_min = (max - min).Normalized();
	auto cross = max_min.Cross(delta);

	Vector forward, right, up;

	if (hitbox_index == HITBOX_HEAD)
	{
		Vector angles;

		math::vector_angles(cross, angles);
		math::angle_vectors(angles, &forward, &right, &up);
	}
	else
		delta.GetVectors(right, up);

	RayTracer::Hitbox box(min, max, hitbox->radius);
	RayTracer::Trace trace;

	if (hitbox_index == HITBOX_HEAD)
	{
		auto middle_first = (right.Normalized() + up.Normalized()) * 0.5f;
		auto middle_second = (right.Normalized() - up.Normalized()) * 0.5f;

		auto ray = RayTracer::Ray(ctx->shoot_position, center + middle_first * 1000.0f);

		RayTracer::TraceFromCenter(ray, box, trace, RayTracer::Flags_RETURNEND);
		current_points.emplace_back(Point(trace.m_traceEnd, scale));

		ray = RayTracer::Ray(ctx->shoot_position, center - middle_second * 1000.0f);

		RayTracer::TraceFromCenter(ray, box, trace, RayTracer::Flags_RETURNEND);
		current_points.emplace_back(Point(trace.m_traceEnd, scale));

		ray = RayTracer::Ray(ctx->shoot_position, center + forward * 1000.0f);

		RayTracer::TraceFromCenter(ray, box, trace, RayTracer::Flags_RETURNEND);
		current_points.emplace_back(Point(trace.m_traceEnd, scale));

		ray = RayTracer::Ray(ctx->shoot_position, center - forward * 1000.0f);

		RayTracer::TraceFromCenter(ray, box, trace, RayTracer::Flags_RETURNEND);
		current_points.emplace_back(Point(trace.m_traceEnd, scale));
	}
	else
	{
		auto ray = RayTracer::Ray(ctx->shoot_position, center - right * 1000.0f);

		RayTracer::TraceFromCenter(ray, box, trace, RayTracer::Flags_RETURNEND);
		current_points.emplace_back(Point(trace.m_traceEnd, scale));

		ray = RayTracer::Ray(ctx->shoot_position, center + right * 1000.0f);

		RayTracer::TraceFromCenter(ray, box, trace, RayTracer::Flags_RETURNEND);
		current_points.emplace_back(Point(trace.m_traceEnd, scale));
	}

	if (scale)
	{
		if (scale > 0.2f)
			points.emplace_back(Point(center, 0.0f));

		for (auto& point : current_points)
			points.emplace_back(Point(center + (point.point - center) * scale, scale));
	}
	else
		points.emplace_back(Point(center, 0.0f));
}

// it's public and was in yours cheat before (LW v3 or smth :D)
// but it's pretty good way to calc simple hitchance traces less than 256
// for choosing point this is going to be enough (@opai)
float Aim::get_point_accuracy(const Vector& angle, int matrix, int hitbox_index, float spread, crypt_ptr <Player> player, crypt_ptr <AnimationData> data, bool debug)
{
	if (convars_manager->convars[CONVAR_WEAPON_ACCURACY_NOSPREAD]->GetBool())
		return 1.f;

	auto hits = 0;

	Vector forward, right, up;
	math::angle_vectors(angle, &forward, &right, &up);

	auto weapon_data = ctx->weapon_data();
	for (auto i = 1; i <= 6; ++i)
	{
		for (auto j = 0; j < 8; ++j)
		{
			auto current_spread = spread * ((float)i / 6.f);

			float value = (float)j / 8.0f * (M_PI * 2.f);

			auto direction_cos = std::cos(value);
			auto direction_sin = std::sin(value);

			auto spread_x = direction_sin * current_spread;
			auto spread_y = direction_cos * current_spread;

			Vector direction{};
			direction.x = forward.x + spread_x * right.x + spread_y * up.x;
			direction.y = forward.y + spread_x * right.y + spread_y * up.y;
			direction.z = forward.z + spread_x * right.z + spread_y * up.z;

			auto end = ctx->shoot_position + direction * weapon_data->range;
			
			if (debug)
				debugoverlay->AddBoxOverlay(end, { -2, -2, -2 }, { 2, 2, 2 }, {}, 255, 255, 255, 255, globals->intervalpertick * 2.f);

			if (hitbox_intersection(hitbox_index, matrix, player, data, end))
				++hits;
		}
	}

	return (float)hits / 48.f;
}

bool Aim::is_hit_chanced(float hit_chance, const Vector& angle, int matrix, int hitbox_index, crypt_ptr <Player> player, crypt_ptr <AnimationData> data, bool check_damage, int damage)
{
	if (convars_manager->convars[CONVAR_WEAPON_ACCURACY_NOSPREAD]->GetBool())
		return true;

	auto hits = 0;

	Vector forward, right, up;
	math::angle_vectors(angle, &forward, &right, &up);

	auto needed_hits = (int)(hit_chance * 2.56f);
	auto allowed_misses = 256 - needed_hits;
	auto weapon_data = ctx->weapon_data();
	for (auto i = 0; i < 256; ++i)
	{
		auto spread = get_spread(i);
		auto direction = (forward + right * spread.x + up * spread.y).Normalized();
		auto end = ctx->shoot_position + direction * weapon_data->range;
		if (hitbox_intersection(hitbox_index, matrix, player, data, end))
		{
			if (check_damage)
			{
				auto penetration_info = penetration->run(ctx->shoot_position, end, player, config->rage.automatic_wall);

				if (penetration_info.damage >= damage)
					++hits;
			}
			else
				++hits;
		}

		if (hits >= needed_hits)
			return true;

		if (i - hits > allowed_misses)
			return false;
	}

	return false;
}

float Aim::get_hit_chance(const Vector& angle, int matrix, int hitbox_index, crypt_ptr <Player> player, crypt_ptr <AnimationData> data) //-V813
{
	if (convars_manager->convars[CONVAR_WEAPON_ACCURACY_NOSPREAD]->GetBool())
		return 100.0f;

	auto hits = 0;

	Vector forward, right, up;
	math::angle_vectors(angle, &forward, &right, &up);

	for (auto i = 0; i < 256; ++i)
	{
		auto spread = get_spread(i);
		auto direction = (forward + right * spread.x + up * spread.y).Normalized();
		auto end = ctx->shoot_position + direction * ctx->weapon_data()->range;

		if (hitbox_intersection(hitbox_index, matrix, player, data, end))
			++hits;
	}

	return min((float)hits / 2.56f, 100.0f);
}

Vector Aim::get_spread(int seed)
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

bool Aim::hitbox_intersection(int hitbox_index, int matrix, crypt_ptr <Player> player, crypt_ptr <AnimationData> data, const Vector& end) //-V813
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

void Aim::extrapolate(crypt_ptr <Player> player, crypt_ptr <AnimationData> extrapolated_data, crypt_ptr <AnimationData> last_data, crypt_ptr <AnimationData> previous_data)
{
	auto move_speed = ((extrapolated_data->origin - previous_data->origin) * (1.0f / (extrapolated_data->simulation_time - previous_data->simulation_time))).Length2D();
	auto should_jump = !(last_data->flags & FL_ONGROUND);

	if (!(extrapolated_data->flags & FL_ONGROUND))
		extrapolated_data->velocity.z -= convars_manager->convars[CONVAR_SV_GRAVITY]->GetFloat() * globals->intervalpertick;
	else if (should_jump)
		extrapolated_data->velocity.z = convars_manager->convars[CONVAR_SV_JUMP_IMPULSE]->GetFloat();

	auto origin = extrapolated_data->origin;
	auto end = origin + extrapolated_data->velocity * globals->intervalpertick;

	CGameTrace trace;

	Ray_t ray;
	ray.Init(origin, end, extrapolated_data->mins, extrapolated_data->maxs);

	CTraceFilter filter;
	filter.pSkip = player.get();

	enginetrace->TraceRay(ray, CONTENTS_SOLID, &filter, &trace);

	if (extrapolated_data->flags & FL_ONGROUND && trace.fraction != 1.0f && !should_jump && extrapolated_data->velocity.Dot(trace.plane.normal) <= FLT_EPSILON) //-V550
	{
		ray.Init(origin + Vector(0.0f, 0.0f, 1.0f), end + Vector(0.0f, 0.0f, 1.0f), extrapolated_data->mins, extrapolated_data->maxs);
		enginetrace->TraceRay(ray, CONTENTS_SOLID, &filter, &trace);
	}

	if (trace.fraction != 1.0f) //-V550
	{
		for (auto i = 0; i < 2; ++i)
		{
			extrapolated_data->velocity -= trace.plane.normal * extrapolated_data->velocity.Dot(trace.plane.normal);
			auto dot = extrapolated_data->velocity.Dot(trace.plane.normal);

			if (dot < 0.0f)
				extrapolated_data->velocity -= trace.plane.normal * dot;

			end = trace.endpos + extrapolated_data->velocity * globals->intervalpertick * (1.0f - trace.fraction);
			ray.Init(trace.endpos, end, extrapolated_data->mins, extrapolated_data->maxs);

			enginetrace->TraceRay(ray, CONTENTS_SOLID, &filter, &trace);

			if (trace.fraction == 1.0f) //-V550
				break;
		}
	}

	extrapolated_data->origin = trace.endpos;
	end = trace.endpos;

	ray.Init(extrapolated_data->origin, end, extrapolated_data->mins, extrapolated_data->maxs);
	enginetrace->TraceRay(ray, CONTENTS_SOLID, &filter, &trace);

	extrapolated_data->flags &= ~FL_ONGROUND;

	if (trace.DidHit() && trace.plane.normal.z > 0.7f)
		extrapolated_data->flags |= FL_ONGROUND;

	extrapolated_data->simulation_time += globals->intervalpertick;
}