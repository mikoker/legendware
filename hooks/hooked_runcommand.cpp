// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\features.h"
#include "..\features\exploits.h"
#include "..\features\prediction.h"

void __fastcall hooked_runcommand(void* ecx, void* edx, Player* player, CUserCmd* cmd, IMoveHelper* move_helper)
{
	static auto original = hooks_manager->hooks[HOOK_PREDICTION]->get_func_address <RunCommand> (crypt_hash_n(19));

	if (!player->valid())
		return original(ecx, player, cmd, move_helper);

	if (player != ctx->local().get())
		return original(ecx, player, cmd, move_helper);

	auto should_return = false;

	if (cmd->tickcount == 0x7F7FFFFF)
	{
		++player->m_nFinalPredictedTick();

		cmd->predicted = true;
		should_return = true;
	}
	else if (exploits->tickbase_adjust)
	{
		player->m_nTickBase() -= exploits->tickbase_adjust;
		exploits->tickbase_adjust = 0;
	}

	static int command_number[MULTIPLAYER_BACKUP];
	static int activity[MULTIPLAYER_BACKUP];

	command_number[cmd->command_number % MULTIPLAYER_BACKUP] = cmd->command_number;
	ctx->last_predicted_command = cmd->command_number;

	if (ctx->weapon())
		activity[cmd->command_number % MULTIPLAYER_BACKUP] = ctx->weapon()->m_Activity();

	if (should_return)
		return;

	engine_prediction->restore_netvars(cmd->command_number - 1);
	engine_prediction->fix_velocity_modifier(false, cmd->command_number);

	original(ecx, player, cmd, move_helper);

	if (config->rage.enable && ctx->weapon() && ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		auto previous_command_number = cmd->command_number - 1;

		if (command_number[previous_command_number % MULTIPLAYER_BACKUP] == previous_command_number && activity[previous_command_number % MULTIPLAYER_BACKUP] != ctx->weapon()->m_Activity() && ctx->weapon()->m_Activity() == ACT_VM_HAULBACK)
			ctx->weapon()->m_flPostponeFireReadyTime() = TICKS_TO_TIME(player->m_nTickBase() - 1) + 0.23125f;
	}

	engine_prediction->fix_velocity_modifier(true, cmd->command_number);
	engine_prediction->store_netvars(cmd->command_number);
}