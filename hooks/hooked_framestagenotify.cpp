// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\json.hpp"
#include "hooks.h"
#include "..\render.h"
#include "..\sdk\classes.h"
#include "..\features\features.h"
#include "..\features\aim.h"
#include "..\features\animations.h"
#include "..\features\movement.h"

#include "..\features\exploits.h"
#include "..\features\draw_beams.h"

#include "..\features\world_color.h"
#include "..\features\spectator_list.h"
#include "..\features\dormant.h"
#include "..\features\logs.h"

#include "..\config\config.h"
#include "..\Lua\Clua.h"

bool is_button_down(int code)
{
	if (code <= KEY_NONE || code >= KEY_MAX)
		return false;

	if (!engine->IsActiveApp())
		return false;

	if (engine->Con_IsVisible())
		return false;

	if (!convars_manager->convars[CONVAR_CL_MOUSEENABLE]->GetBool())
		return false;

	return inputsystem->IsButtonDown((ButtonCode_t)code);
}

auto skybox = 0;
string backup_skybox;

Vector backup_aim_punch;
Vector backup_view_punch;

SpoofedConvar* spoofed_sv_cheats = nullptr;
SpoofedConvar* spoofed_sv_pure = nullptr;

void run_lua_callback(ClientFrameStage_t stage)
{
	/*if (ctx->loaded_script)
	{
		std::string current_callback;
		switch (stage)
		{
		case FRAME_NET_UPDATE_START:
			current_callback = crypt_str("net_update_start");
			break;
		case FRAME_NET_UPDATE_END:
			current_callback = crypt_str("net_update_end");
			break;
		case FRAME_RENDER_START:
			current_callback = crypt_str("pre_render");
			break;
		case FRAME_RENDER_END:
			current_callback = crypt_str("post_render");
			break;
		default:
			break;
		}

		auto& states = Lua_manager->states;

		for (auto& state : states)
		{
			for (auto& current : state.getHooks(current_callback)) {
				sol::protected_function_result result = current.func();
				if (!result.valid()) {

					sol::error error = result;
					auto log = crypt_str("Lua error: ") + (std::string)error.what();

					logs->add(log, Color::Red, crypt_str("[ LUA ] "), true);
					Lua_manager->unload_script(state.id);
				}
			}
		}

	}*/


}

void __stdcall hooked_framestagenotify(ClientFrameStage_t stage)
{
	static auto original = hooks_manager->hooks[HOOK_CLIENT]->get_func_address <FrameStageNotify> (crypt_hash_n(37));

	static auto was_in_game = false;
	static string server_name;

	if (!engine->IsInGame())
	{
		if (was_in_game)
		{
			movement_system->holding_automatic_peek = false;
			movement_system->automatic_peek_shot = false;
			ctx->automatic_peek = false;
			ctx->bomb_timer = true;

			bullet_tracer->bulletImpactInfo.clear();

			ctx->freeze_period = false;
			ctx->buy_count = 2;
			ctx->damage_marker.clear();
			ctx->shots_data.clear();
			world_color->force_update = true;

			gamerules = nullptr;
			config->player_list.players.clear();
			config->player_list.indexes.clear();
			config->player_list.cached_players.clear();
			config->player_list.cached_indexes.clear();
			spectator_list->clear();
			dormant->set_round_start_time(0);

			ctx->show_full_clantag = false;
			ctx->last_round_half = false;
			//chams->attachment_info.clear();
			for (auto i = 0; i < 65; ++i)
			{
				animations->animation_data[i].clear();
				animations->player_data[i].reset();

				config->player_list.player_settings[i].reset();
			}

			ctx->stored_local = nullptr;
			ctx->stored_animation_state = nullptr;
			ctx->stored_weapon = nullptr;
			ctx->stored_weapon_data = nullptr;

			was_in_game = false;
		}


		run_lua_callback(stage);
		return original(stage);
	}
	else if (!was_in_game && engine->IsInGame())
	{

		skybox = 0;
		backup_skybox = convars_manager->convars[CONVAR_SV_SKYNAME]->GetString();
		ctx->reveal_ranks = true;

		was_in_game = true;
	}

	ctx->store();

	if (!gamerules)
		gamerules = **reinterpret_cast <CSGameRulesProxy***> (signatures_manager->signatures[SIGNATURE_GAMERULES]);

	static auto was_valid = false;

	if (was_valid && !ctx->local()->valid())
	{
		movement_system->holding_automatic_peek = false;
		movement_system->automatic_peek_shot = false;
		ctx->automatic_peek = false;
		exploits->double_tap_shot = false;
		exploits->tickbase_cycle = false;
		exploits->last_shot = FLT_MAX;
		exploits->last_double_tap_shot = FLT_MAX;
		ctx->visual_fake_ducking = false;
		ctx->last_velocity_modifier = 1.0f;

		for (auto i = 0; i < 65; ++i)
		{
			ctx->abs_missed[i] = 0;
		}

		was_valid = false;
	}
	else if (!was_valid && ctx->local()->valid())
		was_valid = true;

	auto backup_manual_forward_key = config->anti_aim.manual_forward_key.state;
	auto backup_manual_back_key = config->anti_aim.manual_back_key.state;
	auto backup_manual_left_key = config->anti_aim.manual_left_key.state;
	auto backup_manual_right_key = config->anti_aim.manual_right_key.state;

	auto backup_recoil = false;
	auto nci = crypt_ptr <INetChannelInfo> (engine->GetNetChannelInfo());

	string server;

	switch (stage) //-V719
	{
	case FRAME_START:
		if (engine->IsInGame())
		{
			if (nci && gamerules)
			{
				server = nci->GetAddress();

				if (crypt_hash_r(server.c_str()) == crypt_hash("loopback"))
					server = crypt_str("Local server");
				else if (gamerules->m_bIsValveDS())
					server = crypt_str("Valve server");
			}

			if (!server.empty() && crypt_hash_r(server.c_str()) != crypt_hash_r(server_name.c_str()))
			{

			}
		}

		for (auto& item : config->items)
		{
			if (item.second.type == ITEM_KEY_BIND)
			{
				auto key_bind = crypt_ptr <KeyBind> ((KeyBind*)item.second.pointer.get());
				auto is_down = is_button_down(key_bind->key);

				if (key_bind->mode == HOLD)
				{
					if (is_down)
						key_bind->state = true;
					else
						key_bind->state = false;
				}
				else if (key_bind->mode == TOGGLE)
				{
					if (!key_bind->holding && is_down)
					{
						key_bind->state = !key_bind->state;
						key_bind->holding = true;
					}
					else if (key_bind->holding && !is_down)
						key_bind->holding = false;
				}
				else if (key_bind->mode == ALWAYS_ON)
					key_bind->state = true;
			}
		}

		/*for (auto& current : Lua_manager->scripts)
		{
			auto& items = Lua_manager->items.at(Lua_manager->get_script_id(current));

			for (auto& item : items)
			{
				if (item.second.type == KEY_BIND)
				{
					auto key_bind = crypt_ptr <KeyBind>((KeyBind*)&item.second.m_bind);
					auto is_down = is_button_down(key_bind->key);

					if (key_bind->mode == HOLD)
					{
						if (is_down)
							key_bind->state = true;
						else
							key_bind->state = false;
					}
					else if (key_bind->mode == TOGGLE)
					{
						if (!key_bind->holding && is_down)
						{
							key_bind->state = !key_bind->state;
							key_bind->holding = true;
						}
						else if (key_bind->holding && !is_down)
							key_bind->holding = false;
					}
					else if (key_bind->mode == ALWAYS_ON)
						key_bind->state = true;
				}
			}
		}*/

		if (config->anti_aim.manual_forward_key.state && !backup_manual_forward_key)
		{
			config->anti_aim.manual_back_key.state = false;
			config->anti_aim.manual_left_key.state = false;
			config->anti_aim.manual_right_key.state = false;
		}
		else if (config->anti_aim.manual_back_key.state && !backup_manual_back_key)
		{
			config->anti_aim.manual_forward_key.state = false;
			config->anti_aim.manual_left_key.state = false;
			config->anti_aim.manual_right_key.state = false;
		}
		else if (config->anti_aim.manual_left_key.state && !backup_manual_left_key)
		{
			config->anti_aim.manual_forward_key.state = false;
			config->anti_aim.manual_back_key.state = false;
			config->anti_aim.manual_right_key.state = false;
		}
		else if (config->anti_aim.manual_right_key.state && !backup_manual_right_key)
		{
			config->anti_aim.manual_forward_key.state = false;
			config->anti_aim.manual_back_key.state = false;
			config->anti_aim.manual_left_key.state = false;
		}

		config->player_list.updating = true;
		config->player_list.cached_players.clear();
		config->player_list.cached_indexes.clear();

		for (auto i = 1; i <= globals->maxclients; ++i)
		{
			auto player = crypt_ptr <Player> ((Player*)entitylist->GetClientEntity(i));

			if (!player)
			{
				config->player_list.player_settings[i].reset();
				continue;
			}

			if (player.get() == ctx->local().get())
			{
				config->player_list.player_settings[i].reset();
				continue;
			}

			player_info_t player_info;
			engine->GetPlayerInfo(i, &player_info);

			if (player_info.ishltv)
			{
				config->player_list.player_settings[i].reset();
				continue;
			}

			config->player_list.cached_players.emplace_back(player_info.szName);
			config->player_list.cached_indexes.emplace_back(i);
		}

		config->player_list.updating = false;
		spectator_list->run();
		break;
	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		engine_prediction->restore_viewmodel();

		if (ctx->local()->valid())
			engine_prediction->detect_prediction_error(engine->GetLastAcknowledgedCommand());


		break;
	case FRAME_NET_UPDATE_END:
		if (!shots.empty())
		{
			auto current_shot = shots.end();

			for (auto shot = shots.begin(); shot != shots.end(); ++shot)
			{
				if (shot->end)
				{
					current_shot = shot;
					break;
				}
				else if (shot->impacts && globals->tickcount > shot->event_tickcount)
				{
					current_shot = shot;
					break;
				}
				else if (globals->tickcount - engine_prediction->latency - 16 > shot->tickcount)
				{
					current_shot = shot;

					if (ctx->local()->valid())
						current_shot->latency = true;
					else
						current_shot->local_death = true;

					break;
				}
			}

			/*if (current_shot != shots.end())
			{
				bullet_tracer->pushImpactInfo({ globals->curtime, current_shot->shoot_position, current_shot->shot_info.aim_point });

				if (!current_shot->latency)
				{
					std::vector<std::string> additional;

					if (current_shot->hurt)
						current_shot->shot_info.result = crypt_str("Hit");
					else if (!current_shot->local_death && !current_shot->enemy_death)
					{
						if (current_shot->impact_hit)
						{
							resolver[current_shot->index]->process_missed_shot(&current_shot->data);

							if (config->misc.logs[LOGS_MISSES])
							{
								std::string reason = "Missed shot due to resolver";

								if (current_shot->shot_info.backtrack_ticks > 2)
								{
									additional.insert(additional.begin(), std::format("BT:{}", current_shot->shot_info.backtrack_ticks));
								}

								if (current_shot->shot_info.safe)
								{
									additional.insert(additional.begin(), crypt_str("SP"));
								}
									
								if (!additional.empty())
								{
									reason = logs->detailed_data(reason, additional);
								}

								logs->add(reason, Color(config->misc.logs_color[LOGS_MISSES]), crypt_str("[ MISS ] "));
							}

							current_shot->shot_info.result = crypt_str("Resolver");
						}
						else if (!current_shot->impacts)
						{
							if (config->misc.logs[LOGS_MISSES])
								logs->add(crypt_str("Missed shot due to unregistered"), Color(config->misc.logs_color[LOGS_MISSES]), crypt_str("[ MISS ] "));

							current_shot->shot_info.result = crypt_str("Unregistered");
						}
						else
						{
							if (config->misc.logs[LOGS_MISSES])
							{
								if (current_shot->occlusion)
								{
									std::string reason = "Missed shot due to occlusion";

									if (current_shot->shot_info.hitchance)
									{
										additional.insert(additional.begin(), std::format("HC:{}", current_shot->shot_info.hitchance));
									}

									if (current_shot->shot_info.client_damage)
									{
										additional.insert(additional.begin(), std::format("DMG:{}", current_shot->shot_info.client_damage));
									}

									if (!additional.empty())
									{
										reason = logs->detailed_data(reason, additional);
									}

									logs->add(reason, Color(config->misc.logs_color[LOGS_MISSES]), crypt_str("[ MISS ] "));
								}
								else
								{
									std::string reason = "Missed shot due to spread";

									if (current_shot->shot_info.hitchance)
									{
										additional.insert(additional.begin(), std::format("HC:{}", current_shot->shot_info.hitchance));
									}

									if (!additional.empty())
									{
										reason = logs->detailed_data(reason, additional);
									}

									logs->add(reason, Color(config->misc.logs_color[LOGS_MISSES]), crypt_str("[ MISS ] "));
								}
							}

							if (current_shot->occlusion)
								current_shot->shot_info.result = crypt_str("Occlusion");
							else
								current_shot->shot_info.result = crypt_str("Spread");
						}
					}
					else if (current_shot->local_death)
					{
						if (config->misc.logs[LOGS_MISSES])
							logs->add(crypt_str("Missed shot due to local death"), Color(config->misc.logs_color[LOGS_MISSES]), crypt_str("[ MISS ] "));

						current_shot->shot_info.result = crypt_str("Local death");
					}
					else if (current_shot->enemy_death)
					{
						if (config->misc.logs[LOGS_MISSES])
							logs->add(crypt_str("Missed shot due to target death"), Color(config->misc.logs_color[LOGS_MISSES]), crypt_str("[ MISS ] "));

						current_shot->shot_info.result = crypt_str("Target death");
					}

					if (ctx->loaded_script)
					{
						auto& states = Lua_manager->states;

						for (auto& state : states)
						{
							for (auto& current : state.getHooks(crypt_str("on_shot"))) {
								sol::protected_function_result result = current.func(current_shot->shot_info);
								if (!result.valid()) {

									sol::error error = result;
									auto log = crypt_str("Lua error: ") + (std::string)error.what();

									logs->add(log, Color::Red, crypt_str("[ LUA ] "), true);
									Lua_manager->unload_script(state.id);
								}
							}
						}
					}
				}

				shots.erase(current_shot);
			}*/
		}

		animations->run();
		break;
	case FRAME_RENDER_START:
		if (config->visuals.world.removals[REMOVALS_FLASH])
			ctx->local()->m_flFlashDuration() = 0.0f;
		if (ctx->local()->valid())
		{
			if (config->visuals.world.removals[REMOVALS_RECOIL])
			{
				backup_aim_punch = ctx->local()->m_aimPunchAngle();
				backup_view_punch = ctx->local()->m_viewPunchAngle();

				ctx->local()->m_aimPunchAngle().Zero();
				ctx->local()->m_viewPunchAngle().Zero();

				backup_recoil = true;
			}

			if (config->visuals.world.client_bullet_impacts)
			{
				static auto last_count = INT_MAX;
				auto& client_impact_list = *(CUtlVector <ClientHitVerify>*)((uintptr_t)ctx->local().get() + 0x11C50);

				for (auto i = client_impact_list.Count(); i > last_count; --i)
					debugoverlay->BoxOverlay(client_impact_list[i - 1].position, Vector(-1.5f, -1.5f, -1.5f), Vector(1.5f, 1.5f, 1.5f), QAngle(0.0f, 0.0f, 0.0f), (int)(config->visuals.world.client_bullet_impacts_clr[0] * 255.0f), (int)(config->visuals.world.client_bullet_impacts_clr[1] * 255.0f), (int)(config->visuals.world.client_bullet_impacts_clr[2] * 255.0f), (int)(config->visuals.world.client_bullet_impacts_clr[3] * 255.0f), 4.0f);

				if (client_impact_list.Count() != last_count)
					last_count = client_impact_list.Count();
			}
		}

		static auto backup_weapon_debug_spread_show = convars_manager->convars[CONVAR_WEAPON_DEBUG_SPREAD_SHOW]->GetInt();

		if (config->visuals.world.force_crosshair)
		{
			auto valid = ctx->local()->valid() && !ctx->local()->m_bIsScoped();

			if (convars_manager->convars[CONVAR_WEAPON_DEBUG_SPREAD_SHOW]->GetInt() != 3 && valid) //-V550
				convars_manager->convars[CONVAR_WEAPON_DEBUG_SPREAD_SHOW]->SetValue(3);
			else if (convars_manager->convars[CONVAR_WEAPON_DEBUG_SPREAD_SHOW]->GetInt() && !valid) //-V550
				convars_manager->convars[CONVAR_WEAPON_DEBUG_SPREAD_SHOW]->SetValue(0);
		}
		else
		{
			if (convars_manager->convars[CONVAR_WEAPON_DEBUG_SPREAD_SHOW]->GetInt() != backup_weapon_debug_spread_show) //-V550
				convars_manager->convars[CONVAR_WEAPON_DEBUG_SPREAD_SHOW]->SetValue(backup_weapon_debug_spread_show);

			backup_weapon_debug_spread_show = convars_manager->convars[CONVAR_WEAPON_DEBUG_SPREAD_SHOW]->GetInt();
		}

		static auto backup_viewmodel_x = convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_X]->GetFloat();
		static auto backup_viewmodel_y = convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Y]->GetFloat();
		static auto backup_viewmodel_z = convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Z]->GetFloat();

		if (config->misc.viewmodel_changer)
		{
			if (convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_X]->GetFloat() != (float)config->misc.viewmodel_x * 0.5f) //-V550
			{
				convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_X]->m_fnChangeCallbacks.m_Size = 0;
				convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_X]->SetValue((float)config->misc.viewmodel_x * 0.5f);
			}

			if (convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Y]->GetFloat() != (float)config->misc.viewmodel_y * 0.5f) //-V550
			{
				convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Y]->m_fnChangeCallbacks.m_Size = 0;
				convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Y]->SetValue((float)config->misc.viewmodel_y * 0.5f);
			}

			if (convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Z]->GetFloat() != (float)config->misc.viewmodel_z * 0.5f) //-V550
			{
				convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Z]->m_fnChangeCallbacks.m_Size = 0;
				convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Z]->SetValue((float)config->misc.viewmodel_z * 0.5f);
			}
		}
		else
		{
			if (convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_X]->GetFloat() != backup_viewmodel_x) //-V550
				convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_X]->SetValue(backup_viewmodel_x);

			if (convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Y]->GetFloat() != backup_viewmodel_y) //-V550
				convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Y]->SetValue(backup_viewmodel_y);

			if (convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Z]->GetFloat() != backup_viewmodel_z) //-V550
				convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Z]->SetValue(backup_viewmodel_z);

			backup_viewmodel_x = convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_X]->GetFloat();
			backup_viewmodel_y = convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Y]->GetFloat();
			backup_viewmodel_z = convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Z]->GetFloat();
		}

		
		/*if (convars_manager->convars[CONVAR_SV_CHEATS]->GetBool() != config->misc.spoof_sv_cheats)
		{
			if (!spoofed_sv_cheats)
				spoofed_sv_cheats = new SpoofedConvar(crypt_str("sv_cheats"));

			if (config->misc.spoof_sv_cheats)
			{
				spoofed_sv_cheats->Spoof();
				spoofed_sv_cheats->SetBool(true);
			}
			else
				spoofed_sv_cheats->Restore();
		}*/

		/*if ((convars_manager->convars[CONVAR_SV_PURE]->GetInt() == -1) != config->misc.spoof_sv_pure)
		{
			if (!spoofed_sv_pure)
				spoofed_sv_pure = new SpoofedConvar(crypt_str("sv_pure"));

			if (config->misc.spoof_sv_pure)
			{
				spoofed_sv_pure->Spoof();
				spoofed_sv_pure->SetInt(-1);
			}
			else
				spoofed_sv_pure->Restore();
		}*/

		if (skybox != config->visuals.world.skybox)
		{
			skybox = config->visuals.world.skybox;

			switch (config->visuals.world.skybox)
			{
			case 0:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(backup_skybox.c_str());
				backup_skybox = convars_manager->convars[CONVAR_SV_SKYNAME]->GetString();
				break;
			case 1:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("cs_baggage_skybox"));
				break;
			case 2:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("cs_tibet"));
				break;
			case 3:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("clearsky"));
				break;
			case 4:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("clearsky_hdr"));
				break;
			case 5:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("embassy"));
				break;
			case 6:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("italy"));
				break;
			case 7:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_cs15_daylight01_hdr"));
				break;
			case 8:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_cs15_daylight02_hdr"));
				break;
			case 9:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_cs15_daylight03_hdr"));
				break;
			case 10:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_cs15_daylight04_hdr"));
				break;
			case 11:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_csgo_cloudy01"));
				break;
			case 12:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_csgo_night02"));
				break;
			case 13:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_csgo_night02b"));
				break;
			case 14:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_csgo_night_flat"));
				break;
			case 15:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_day02_05_hdr"));
				break;
			case 16:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_day02_05"));
				break;
			case 17:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_l4d_rural02_ldr"));
				break;
			case 18:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("vertigo_hdr"));
				break;
			case 19:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("vertigoblue_hdr"));
				break;
			case 20:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("vertigo"));
				break;
			case 21:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("vietnam"));
				break;
			case 22:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("sky_dust"));
				break;
			case 23:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("jungle"));
				break;
			case 24:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("nukeblank"));
				break;
			case 25:
				reinterpret_cast <void(__fastcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX])(crypt_str("office"));
				break;
			}
		}

		*(bool*)postprocessing = config->visuals.world.removals[REMOVALS_POSTPROCESSING];
		break;
	case FRAME_RENDER_END:
		bullet_tracer->drawBeam();
		world_color->apply();
		break;
	}


	run_lua_callback(stage);
	original(stage);

	if (backup_recoil && ctx->local()->valid())
	{
		ctx->local()->m_aimPunchAngle() = backup_aim_punch;
		ctx->local()->m_viewPunchAngle() = backup_view_punch;
	}

	static DWORD death_notice = 0;

	if (ctx->local()->valid())
	{
		static auto FindHudElement = reinterpret_cast <DWORD(__thiscall*)(void*, const char*)> (signatures_manager->signatures[SIGNATURE_FIND_HUD_ELEMENT]);
		static auto hud_ptr = *(DWORD**)(signatures_manager->signatures[SIGNATURE_HUD]);

		if (!death_notice)
			death_notice = FindHudElement(hud_ptr, crypt_str("CCSGO_HudDeathNotice"));

		if (death_notice)
		{
			auto local_death_notice = (float*)((uintptr_t)death_notice + 0x50); //-V201

			if (local_death_notice)
				*local_death_notice = config->misc.preserve_killfeed ? FLT_MAX : 1.5f;

			if (ctx->clear_death_notices)
			{
				if (config->misc.preserve_killfeed)
					reinterpret_cast <void(__thiscall*)(uintptr_t)> (signatures_manager->signatures[SIGNATURE_CLEAR_DEATH_NOTICES])((uintptr_t)death_notice - 0x14); //-V201

				ctx->clear_death_notices = false;
			}
		}
	}
	else
		death_notice = 0;
}