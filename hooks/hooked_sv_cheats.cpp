#include "hooks.h"

bool __fastcall hooked_sv_cheats(ConVar* convar, void* edx)
{
	static auto original = hooks_manager->hooks[HOOL_CONVAR_SV_CHEATS]->get_func_address <SvCheats>(crypt_hash_n(13));
	if (config->misc.bypasses[BYPASS_SV_CHEATS])
		return true;

	return original(convar);
}