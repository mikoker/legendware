// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\aim.h"
#include "..\features\knife_aim.h"
#include "..\features\legit_aim.h"
#include "..\features\features.h"
#include "..\features\misc.h"
#include "..\config\config.h"
#include "..\features\exploits.h"
#include "..\features\prediction.h"
#include "..\features\movement.h"
#include "..\features\fake_lag.h"

#include "..\features\logs.h"
#include "..\menu\menu.h"
#include "..\features\anti_aim.h"
#include "..\Lua\Clua.h"

bool send_packet = false;

void __stdcall hooked_createmove(int sequence_number, float input_sample_frametime, bool active)
{
	static auto original = hooks_manager->hooks[HOOK_CLIENT]->get_func_address <CreateMove> (crypt_hash_n(22));

	auto cmd = crypt_ptr <CUserCmd> (input->GetUserCmd(sequence_number));
	auto verified_cmd = crypt_ptr <CVerifiedUserCmd> (input->GetVerifiedUserCmd(sequence_number));

	original(client.get(), sequence_number, input_sample_frametime, active);

	ctx->store();

	if (!cmd)
		return;

	if (!cmd->command_number)
		return;

	misc->run(cmd);

	if (!ctx->local()->valid())
		return;

	if (!ctx->weapon())
		return;

	if (!ctx->weapon_data())
		return;

	if (core::s::is_opened)
	{
		cmd->buttons &= ~IN_ATTACK;
		cmd->buttons &= ~IN_ATTACK2;
	}

	send_packet = true;

	ctx->send_packet = &send_packet;
	ctx->send_packet_fake_duck = false;
	ctx->automatic_revolver = false;
	ctx->used_recoil_control_system = false;
	ctx->tickcount = globals->tickcount;
	ctx->tickbase = ctx->local()->m_nTickBase();
	ctx->original_tickbase = ctx->local()->m_nTickBase(); //-V656
	ctx->original_choked_commands = clientstate->m_nChokedCommands;
	ctx->weapon_config = ctx->weapon()->get_weapon_group();
	ctx->max_speed = ctx->local()->m_bIsScoped() ? ctx->weapon_data()->max_player_speed_alt : ctx->weapon_data()->max_player_speed;
	ctx->friendly_fire = convars_manager->convars[CONVAR_MP_TEAMMATES_ARE_ENEMIES]->GetBool();

	auto update_rate = clamp(convars_manager->convars[CONVAR_CL_UPDATERATE]->GetFloat(), convars_manager->convars[CONVAR_SV_MINUPDATERATE]->GetFloat(), convars_manager->convars[CONVAR_SV_MAXUPDATERATE]->GetFloat());
	auto lerp_ratio = clamp(convars_manager->convars[CONVAR_CL_INTERP_RATIO]->GetFloat(), convars_manager->convars[CONVAR_SV_CLIENT_MIN_INTERP_RATIO]->GetFloat(), convars_manager->convars[CONVAR_SV_CLIENT_MAX_INTERP_RATIO]->GetFloat());

	ctx->interpolation = clamp(lerp_ratio / update_rate, convars_manager->convars[CONVAR_CL_INTERP]->GetFloat(), 1.0f);
	engine_prediction->store_data(cmd);

	auto net_channel_info = crypt_ptr <INetChannelInfo> (engine->GetNetChannelInfo());

	if (net_channel_info)
	{
		auto outgoing = net_channel_info->GetLatency(FLOW_OUTGOING);
		auto incoming = net_channel_info->GetLatency(FLOW_INCOMING);

		engine_prediction->latency = TIME_TO_TICKS(outgoing + incoming);
	}
	else
		engine_prediction->latency = 0;

	exploits->update();

	if (config->rage.enable && !ctx->local()->can_shoot())
	{
		cmd->buttons &= ~IN_ATTACK;

		if (ctx->weapon_config == WEAPON_CONFIG_KNIFE)
			cmd->buttons &= ~IN_ATTACK2;
	}
	else if (ctx->weapon_config != WEAPON_CONFIG_INVALID && config->legit.enable && config->legit.weapon[ctx->weapon_config].automatic_attack)
	{
		auto fire_delay = config->legit.weapon[ctx->weapon_config].automatic_attack ? (float)config->legit.weapon[ctx->weapon_config].fire_delay * 0.001f : 0.0f;

		if (!ctx->local()->can_shoot(false, fire_delay))
		{
			cmd->buttons &= ~IN_ATTACK;

			if (ctx->weapon_config == WEAPON_CONFIG_KNIFE)
				cmd->buttons &= ~IN_ATTACK2;
		}
	}

	/*if (ctx->loaded_script)
	{
		auto& states = Lua_manager->states;

		for (auto& state : states)
		{
			for (auto& current : state.getHooks(crypt_str("create_move"))) {
				sol::protected_function_result result = current.func(cmd.get());
				if (!result.valid()) {

					sol::error error = result;
					auto log = crypt_str("Lua error: ") + (std::string)error.what();

					logs->add(log, Color::Red, crypt_str("[ LUA ] "), true);
					Lua_manager->unload_script(state.id);
				}
			}
		}
	}*/
	
	movement_system->run(cmd);

	aim->automatic_stop(cmd);

	engine_prediction->store_restore_data();
	engine_prediction->start(cmd);
	engine_prediction->update_data();

	movement_system->edge_jump(cmd);

	if (config->rage.enable)
	{
		aim->run(cmd);
		knife_aim->run(cmd);
	}
	else if (config->legit.enable)
		legit_aim->run(cmd);

	if (!ctx->used_recoil_control_system)
		legit_aim->previous_aim_punch_angle.Zero();

	fake_lag->run(cmd);

	if (ctx->force_send_packet)
		*ctx->send_packet.get() = true;

	ctx->force_send_packet = false;

	if (anti_aim->flicked)
	{
		ctx->force_send_packet = true;
		*ctx->send_packet.get() = false;
	}

	anti_aim->flicked = false;

	if (ctx->weapon()->executed(cmd))
	{
		if (!ctx->fake_ducking)
			*ctx->send_packet.get() = true;

		if (config->rage.enable)
			cmd->viewangles -= ctx->local()->m_aimPunchAngle() * convars_manager->convars[CONVAR_WEAPON_RECOIL_SCALE]->GetFloat();
	}

	auto max_choke = gamerules && gamerules->m_bIsValveDS() ? 6 : 16;

	if (clientstate->m_nChokedCommands >= max_choke)
		*ctx->send_packet.get() = true;

	if (exploits->charge(cmd))
		return;

	if (config->misc.automatic_peek[AUTOMATIC_PEEK_KEY] && !(cmd->buttons & IN_FORWARD || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVERIGHT || cmd->buttons & IN_JUMP))
		movement_system->holding_automatic_peek = true;
	else if (config->misc.automatic_peek[AUTOMATIC_PEEK_SHOT] && cmd->buttons & IN_ATTACK && !ctx->automatic_revolver && ctx->weapon()->can_fire())
	{
		movement_system->holding_automatic_peek = true;
		movement_system->automatic_peek_shot = true;
	}

	exploits->run(cmd);

	if (exploits->charging)
		*ctx->send_packet.get() = true;

	anti_aim->run(cmd);

	movement_system->fix_movement(cmd, movement_system->wish_angle, abs(cmd->viewangles.z) > 0.0f);
	local_animations->run(cmd, *ctx->send_packet.get());

	engine_prediction->end();
	exploits->correct_charge(cmd);
	

	auto& out = ctx->commands.emplace_back();

	out.is_outgoing = *ctx->send_packet.get();
	out.is_used = false;
	out.command_number = cmd->command_number;
	out.previous_command_number = 0;

	while (ctx->commands.size() > TIME_TO_TICKS(1.0f))
		ctx->commands.pop_front();

	auto net_channel = crypt_ptr <INetChannel> (clientstate->m_NetChannel);

	if (!*ctx->send_packet.get() && net_channel && net_channel->m_nChokedPackets > 0 && !(net_channel->m_nChokedPackets % 4))
	{
		auto backup_choke = net_channel->m_nChokedPackets;

		net_channel->m_nChokedPackets = 0;
		net_channel->send_datagram();
		--net_channel->m_nOutSequenceNr;

		net_channel->m_nChokedPackets = backup_choke;
	}

	DWORD old = 0;

	VirtualProtect((void*)signatures_manager->signatures[SIGNATURE_CHOKE_LIMIT], sizeof(uint32_t), PAGE_READWRITE, &old);
	*(uintptr_t*)signatures_manager->signatures[SIGNATURE_CHOKE_LIMIT] = 62;
	VirtualProtect((void*)signatures_manager->signatures[SIGNATURE_CHOKE_LIMIT], sizeof(uint32_t), old, &old);

	if (config->misc.automatic_buy && ctx->buy_count)
	{
		--ctx->buy_count;

		if (!ctx->buy_count)
		{
			string buy;

			switch (config->misc.primary)
			{
			case 1:
				buy += crypt_str("buy g3sg1; buy scar20;");
				break;
			case 2:
				buy += crypt_str("buy galilar; buy famas;");
				break;
			case 3:
				buy += crypt_str("buy ak47; buy m4a1; buy m4a1_silencer;");
				break;
			case 4:
				buy += crypt_str("buy ssg08;");
				break;
			case 5:
				buy += crypt_str("buy sg556; buy aug;");
				break;
			case 6:
				buy += crypt_str("buy awp;");
				break;
			}

			switch (config->misc.secondary)
			{
			case 1:
				buy += crypt_str("buy elite;");
				break;
			case 2:
				buy += crypt_str("buy p250;");
				break;
			case 3:
				buy += crypt_str("buy tec9; buy fn57;");
				break;
			case 4:
				buy += crypt_str("buy deagle;");
				break;
			}

			if (ctx->local()->m_iAccount() > 800)
			{
				if (config->misc.others[OTHERS_HE])
					buy += crypt_str("buy hegrenade;");

				if (config->misc.others[OTHERS_MOLOTOV])
					buy += crypt_str("buy molotov;");

				if (config->misc.others[OTHERS_SMOKE])
					buy += crypt_str("buy smokegrenade;");

				if (config->misc.others[OTHERS_FLASHBANG])
					buy += crypt_str("buy flashbang;");

				if (config->misc.others[OTHERS_DECOY])
					buy += crypt_str("buy decoy;");

				if (config->misc.others[OTHERS_ARMOR])
					buy += crypt_str("buy vesthelm; buy vest;");

				if (config->misc.others[OTHERS_KIT])
					buy += crypt_str("buy defuser;");

				if (config->misc.others[OTHERS_TASER])
					buy += crypt_str("buy taser;");
			}

			engine->ExecuteClientCmd(buy.c_str());
		}
	}

	verified_cmd->m_cmd = *cmd.get();
	verified_cmd->m_crc = cmd->GetChecksum();
}

__declspec(naked) void __fastcall hooked_createmove_proxy(void* ecx, void* edx, int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		mov send_packet, bl
		push ebp
		mov ebp, esp
		sub esp, 8
		pushad
		push active
		push input_sample_frametime
		push sequence_number
		call hooked_createmove
		popad
		mov bl, send_packet
		mov esp, ebp
		pop ebp
		retn 0xc
	}
}