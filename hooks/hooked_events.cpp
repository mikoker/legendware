// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\features.h"
#include "..\features\aim.h"

#include "..\features\movement.h"
#include "..\features\draw_beams.h"
#include "..\features\animations.h"

#include "..\features\esp.h"
#include "..\features\dormant.h"
#include "..\features\logs.h"

#include "..\Lua\Clua.h"
#include <codecvt>
void Events::FireGameEvent(IGameEvent* event)
{
	auto event_name = event->GetName();

	//if (ctx->loaded_script)
	//{
	//	for (auto& script : Lua_manager->scripts)
	//	{
	//		auto script_id = Lua_manager->get_script_id(script);

	//		if (Lua_manager->events.find(script_id) == Lua_manager->events.end())
	//			continue;

	//		if (Lua_manager->events[script_id].find(event_name) == Lua_manager->events[script_id].end())
	//			continue;

	//		Lua_manager->events[script_id][event_name](event);
	//	}
	//}

	if (crypt_hash_r(event_name) == crypt_hash("weapon_fire"))
	{
		auto user_id = event->GetInt(crypt_str("userid"));
		auto user = engine->GetPlayerForUserID(user_id);

		if (user == engine->GetLocalPlayer())
		{
			crypt_ptr <Shot> current_shot;

			for (auto& shot : shots)
			{
				if (shot.start && shot.impacts)
				{
					shot.end = true;
					continue;
				}

				if (shot.end)
					continue;

				current_shot = &shot;
				break;
			}

			if (current_shot)
			{
				current_shot->start = true;
				current_shot->event_tickcount = globals->tickcount;
			}
		}
	}
	else if (crypt_hash_r(event_name) == crypt_hash("bullet_impact"))
	{
		auto user_id = event->GetInt(crypt_str("userid"));
		auto user = engine->GetPlayerForUserID(user_id);

		if (user == engine->GetLocalPlayer())
		{
			Vector position(event->GetFloat(crypt_str("x")), event->GetFloat(crypt_str("y")), event->GetFloat(crypt_str("z")));

			if (config->visuals.world.server_bullet_impacts)
				debugoverlay->BoxOverlay(position, Vector(-1.5f, -1.5f, -1.5f), Vector(1.5f, 1.5f, 1.5f), QAngle(0.0f, 0.0f, 0.0f), (int)(config->visuals.world.server_bullet_impacts_clr[0] * 255.0f), (int)(config->visuals.world.server_bullet_impacts_clr[1] * 255.0f), (int)(config->visuals.world.server_bullet_impacts_clr[2] * 255.0f), (int)(config->visuals.world.server_bullet_impacts_clr[3] * 255.0f), 4.0f);

			//auto player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(user));
			crypt_ptr <Shot> current_shot;

			for (auto& shot : shots)
			{
				if (!shot.start)
					continue;

				if (shot.end)
					continue;

				current_shot = &shot;
				break;
			}

			if (current_shot)
			{
				auto player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(current_shot->data.i));

				if (player->valid())
				{
					auto backup_data = AnimationData(player);
					current_shot->data.apply();

					CGameTrace trace;
					Ray_t ray;

					ray.Init(current_shot->shoot_position, position);
					enginetrace->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, player.get(), &trace);

					if (trace.hit_entity == player.get())
						current_shot->impact_hit = true;

					if (!current_shot->impact_hit)
						current_shot->occlusion = current_shot->shoot_position.DistTo(position) < current_shot->distance;

					current_shot->last_impact = position;
					backup_data.apply(MATRIX_MAIN, true);
				}
				else
					current_shot->enemy_death = true;

				current_shot->impacts = true;
			}
		}
	}
	else if (crypt_hash_r(event_name) == crypt_hash("player_hurt"))
	{
		auto get_hitgroup_name = [](int hitgroup) -> string
		{
			switch (hitgroup)
			{
			case HITGROUP_HEAD:
				return crypt_str("head");
			case HITGROUP_CHEST:
				return crypt_str("chest");
			case HITGROUP_STOMACH:
				return crypt_str("stomach");
			case HITGROUP_LEFTARM:
				return crypt_str("right arm");
			case HITGROUP_RIGHTARM:
				return crypt_str("left arm");
			case HITGROUP_LEFTLEG:
				return crypt_str("right leg");
			case HITGROUP_RIGHTLEG:
				return crypt_str("left leg");
			default:
				return crypt_str("generic");
			}
		};

		auto get_hitbox = [](int hitgroup) -> int
		{
			switch (hitgroup)
			{
			case HITGROUP_HEAD:
				return HITBOX_HEAD;
			case HITGROUP_CHEST:
				return HITBOX_CHEST;
			case HITGROUP_STOMACH:
				return HITBOX_STOMACH;
			case HITGROUP_LEFTARM:
				return HITBOX_RIGHT_UPPER_ARM;
			case HITGROUP_RIGHTARM:
				return HITBOX_LEFT_UPPER_ARM;
			case HITGROUP_LEFTLEG:
				return HITBOX_RIGHT_THIGH;
			case HITGROUP_RIGHTLEG:
				return HITBOX_LEFT_THIGH;
			default:
				return HITBOX_NECK;
			}
		};

		auto userid = event->GetInt(crypt_str("userid")), attacker = event->GetInt(crypt_str("attacker"));

		if (!userid || !attacker)
			return;

		auto userid_id = engine->GetPlayerForUserID(userid), attacker_id = engine->GetPlayerForUserID(attacker);

		player_info_t userid_info, attacker_info;

		if (!engine->GetPlayerInfo(userid_id, &userid_info))
			return;

		if (!engine->GetPlayerInfo(attacker_id, &attacker_info))
			return;

		std::stringstream ss;

		if (attacker_id == engine->GetLocalPlayer() && userid_id != engine->GetLocalPlayer())
		{
			if (config->visuals.world.hit_sound)
				surface->PlaySound_(crypt_str("buttons\\arena_switch_press_02.wav"));

			auto hitgroup = event->GetInt(crypt_str("hitgroup"));
			auto hitgroup_name = get_hitgroup_name(hitgroup);
			auto damage = event->GetInt(crypt_str("dmg_health"));

			crypt_ptr <Shot> current_shot;

			for (auto& shot : shots)
			{
				if (!shot.start)
					continue;

				if (shot.end)
					continue;

				current_shot = &shot;
				break;
			}

			auto player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(userid_id));

			if (player)
			{
				if (current_shot && current_shot->player.get() == player.get())
				{
					current_shot->end = true;
					current_shot->hurt = true;
					current_shot->shot_info.server_hitbox = hitgroup_name;
					current_shot->shot_info.server_damage = damage;

					if (hitgroup != current_shot->hitgroup)
						current_shot->hurt_fake = true;

					ctx->damage_marker.emplace_front(DamageMarker(damage, globals->curtime, player->get_hitbox_position(get_hitbox(hitgroup), current_shot->data.matrix[MATRIX_MAIN]), config->visuals.world.damage_marker_custom_color ? Color(config->visuals.world.damage_marker_color) : (hitgroup == HITGROUP_HEAD ? Color::Red : Color::White)));
				}
				else
					ctx->damage_marker.emplace_front(DamageMarker(damage, globals->curtime, player->get_hitbox_position(get_hitbox(hitgroup), player->m_CachedBoneData().Base()), config->visuals.world.damage_marker_custom_color ? Color(config->visuals.world.damage_marker_color) : (hitgroup == HITGROUP_HEAD ? Color::Red : Color::White)));
			}
			else if (current_shot)
				current_shot->enemy_death = true;

			if (config->misc.logs[LOGS_HITS])
			{
				auto health = event->GetInt(crypt_str("health"));

#if BETA
				if (current_shot && current_shot->hurt_fake)
				{
					ss << crypt_str("Hit ") << userid_info.szName << crypt_str(" in the ") << hitgroup_name << crypt_str(" for ") << damage;
					ss << crypt_str(" damage (") << health << crypt_str(" health remaining, spread or resolver mismatch)");

					logs->add(ss.str(), Color(config->misc.logs_color[LOGS_HITS]), crypt_str("[ HIT ] "));
				}
				else
				{
					ss << crypt_str("Hit ") << userid_info.szName << crypt_str(" in the ") << hitgroup_name << crypt_str(" for ") << damage;
					ss << crypt_str(" damage (") << health << crypt_str(" health remaining)");

					logs->add(ss.str(), Color(config->misc.logs_color[LOGS_HITS]), crypt_str("[ HIT ] "));
				}
#else
				ss << crypt_str("Hit ") << userid_info.szName << crypt_str(" in the ") << hitgroup_name << crypt_str(" for ") << damage;
				ss << crypt_str(" damage (") << health << crypt_str(" health remaining)");

				logs->add(ss.str(), Color(config->misc.logs_color[LOGS_HITS]), crypt_str("[ HIT ] "));
#endif
			}
		}
		else if (config->misc.logs[LOGS_HURTS] && userid_id == engine->GetLocalPlayer() && attacker_id != engine->GetLocalPlayer())
		{
			ss << crypt_str("Take ") << event->GetInt(crypt_str("dmg_health")) << crypt_str(" damage from ") << attacker_info.szName << crypt_str(" in the ") << get_hitgroup_name(event->GetInt(crypt_str("hitgroup")));
			ss << crypt_str(" (") << event->GetInt(crypt_str("health")) << crypt_str(" health remaining)");

			logs->add(ss.str(), Color(config->misc.logs_color[LOGS_HURTS]));
		}
	}
	else if (crypt_hash_r(event_name) == crypt_hash("player_death"))
	{
		auto attacker = event->GetInt(crypt_str("attacker"));
		auto user = event->GetInt(crypt_str("userid"));

		auto attacker_id = engine->GetPlayerForUserID(attacker);
		auto user_id = engine->GetPlayerForUserID(user);

		esp->reset_animation(user_id);
		std::string weapon_name = event->GetString(crypt_str("weapon"));

		if (attacker_id == engine->GetLocalPlayer() && user_id != engine->GetLocalPlayer() )
		{
		}
	}
	else if (crypt_hash_r(event_name) == crypt_hash("round_start"))
	{
		ctx->bomb_timer = true;
		ctx->automatic_peek = false;
		ctx->automatic_peek_position.Zero();
		ctx->automatic_peek_radius = 0.0f;
		ctx->freeze_period = true;
		ctx->clear_death_notices = true;
		ctx->buy_count = 2;

		ctx->show_full_clantag = false;
		ctx->last_round_half = false;

		dormant->set_round_start_time(globals->curtime + 1.0f);
		for (auto i = 0; i < 65; ++i)
		{


			ctx->abs_missed[i] = 0;
			esp->reset_animation(i);
		}
	}
	else if (crypt_hash_r(event_name) == crypt_hash("round_freeze_end"))
		ctx->freeze_period = false;
	else if (crypt_hash_r(event_name) == crypt_hash("bomb_defused"))
		ctx->bomb_timer = false;
	else if (crypt_hash_r(event_name) == crypt_hash("item_equip"))
	{
		auto user_id = event->GetInt(crypt_str("userid"));
		auto user = engine->GetPlayerForUserID(user_id);

		auto player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(user));

		if (player.get() != ctx->local().get() && player->valid(!ctx->friendly_fire) && player->IsDormant())
			esp->weapons[user] = event->GetInt(crypt_str("defindex"));
	}
	else if (crypt_hash_r(event_name) == crypt_hash("item_purchase"))
	{

		
	}

	else if (crypt_hash_r(event_name) == crypt_hash("round_mvp"))
	{

	}
	else if (crypt_hash_r(event_name) == crypt_hash("round_announce_last_round_half"))
		ctx->last_round_half = true;
	else if (crypt_hash_r(event_name) == crypt_hash("round_end"))
	{
		if (ctx->last_round_half)
			ctx->show_full_clantag = true;
	}
	else if (crypt_hash_r(event_name) == crypt_hash("cs_win_panel_match"))
		ctx->show_full_clantag = true;
}

int Events::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void Events::RegisterSelf()
{
	m_iDebugId = EVENT_DEBUG_ID_INIT;

	events->AddListener(this, crypt_str("round_freeze_end"), false);
	events->AddListener(this, crypt_str("round_start"), false);
	events->AddListener(this, crypt_str("weapon_fire"), false);
	events->AddListener(this, crypt_str("bullet_impact"), false);
	events->AddListener(this, crypt_str("player_hurt"), false);
	events->AddListener(this, crypt_str("player_death"), false);
	events->AddListener(this, crypt_str("bomb_defused"), false);
	events->AddListener(this, crypt_str("item_equip"), false);
	events->AddListener(this, crypt_str("item_purchase"), false);
	events->AddListener(this, crypt_str("player_connect"), false);
	events->AddListener(this, crypt_str("player_disconnect"), false);
	events->AddListener(this, crypt_str("round_mvp"), false);
	events->AddListener(this, crypt_str("cs_win_panel_match"), false);
	events->AddListener(this, crypt_str("round_end"), false);
	events->AddListener(this, crypt_str("round_announce_last_round_half"), false);

	ctx->events.emplace_back(crypt_str("round_freeze_end"));
	ctx->events.emplace_back(crypt_str("round_start"));
	ctx->events.emplace_back(crypt_str("weapon_fire"));
	ctx->events.emplace_back(crypt_str("bullet_impact"));
	ctx->events.emplace_back(crypt_str("player_hurt"));
	ctx->events.emplace_back(crypt_str("player_death"));
	ctx->events.emplace_back(crypt_str("bomb_defused"));
	ctx->events.emplace_back(crypt_str("item_equip"));
	ctx->events.emplace_back(crypt_str("item_purchase"));
	ctx->events.emplace_back(crypt_str("player_connect"));
	ctx->events.emplace_back(crypt_str("player_disconnect"));
	ctx->events.emplace_back(crypt_str("round_mvp"));
	ctx->events.emplace_back(crypt_str("cs_win_panel_match"));
	ctx->events.emplace_back(crypt_str("round_end"));
	ctx->events.emplace_back(crypt_str("round_announce_last_round_half"));
}

void Events::RemoveSelf()
{
	events->RemoveListener(this);
}