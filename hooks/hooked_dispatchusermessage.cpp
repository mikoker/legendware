// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

bool __fastcall hooked_dispatchusermessage(void* ecx, void* edx, int message_type, int unknown, int bytes, bf_read& message_data)
{
	static auto original = hooks_manager->hooks[HOOK_CLIENT]->get_func_address <DispatchUserMessage> (crypt_hash_n(38));

	if (!config->misc.adblock)
		return original(ecx, message_type, unknown, bytes, message_data);

	if (message_type == CS_UM_TextMsg || message_type == CS_UM_HudMsg || message_type == CS_UM_SayText)
		return true;

	return original(ecx, message_type, unknown, bytes, message_data);
}