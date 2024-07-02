// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

bool __fastcall hooked_isconnected(void* ecx, void* edx)
{
	static auto original = hooks_manager->hooks[HOOK_ENGINE]->get_func_address <IsConnected> (crypt_hash_n(27));

	if (!config->misc.bypasses[BYPASS_INVENTORY_RESTRICTION])
		return original(ecx);

	if ((uintptr_t)_ReturnAddress() != signatures_manager->signatures[SIGNATURE_INVENTORY_ACCESS])
		return original(ecx);

	if (gamerules && gamerules->m_bIsValveDS() && config->misc.anti_untrusted)
		return original(ecx);

	return false;
}