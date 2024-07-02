// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

float __stdcall hooked_getviewmodelfov()
{
	static auto original = hooks_manager->hooks[HOOK_CLIENTMODE]->get_func_address <GetViewmodelFOV> (crypt_hash_n(35));

	if (!config->misc.viewmodel_changer)
		return original();

	return config->misc.viewmodel_field_of_view;
}