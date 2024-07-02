// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

void __fastcall hooked_processmovement(void* ecx, void* edx, Player* player, CMoveData* move_data)
{
	static auto original = hooks_manager->hooks[HOOK_MOVEMENT]->get_func_address <ProcessMovement> (crypt_hash_n(1));
	return original(ecx, player, move_data);
}