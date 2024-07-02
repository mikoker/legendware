// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

float __fastcall hooked_getscreenaspectratio(void* ecx, void* edx, int width, int height)
{
	static auto original = hooks_manager->hooks[HOOK_ENGINE]->get_func_address <GetScreenAspectRatio> (crypt_hash_n(101));

	if (!config->misc.aspect_ratio)
		return original(ecx, width, height);

	if (!config->misc.aspect_ratio_value)
		return original(ecx, width, height);

	return config->misc.aspect_ratio_value;
}