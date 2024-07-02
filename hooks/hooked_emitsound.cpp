// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\features.h"
#include "..\features\logs.h"

void __fastcall hooked_emitsound(void* ecx, void* edx, void* filter, int entity_index, int channel, const char* sound_entry, unsigned int sound_entry_hash, const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const Vector* origin, const Vector* direction, void* vec_origins, bool update_positions, float sound_time, int speaker_entity, int test)
{
	static auto original = hooks_manager->hooks[HOOK_ENGINESOUND]->get_func_address <EmitSound> (crypt_hash_n(5));

	if (!config->misc.automatic_accept)
		return original(ecx, filter, entity_index, channel, sound_entry, sound_entry_hash, sample, volume, attenuation, seed, flags, pitch, origin, direction, vec_origins, update_positions, sound_time, speaker_entity, test);

	if (crypt_hash_r(sample) == crypt_hash("UI/competitive_accept_beep.wav"))
		reinterpret_cast <bool(__stdcall*)(const char*)> (signatures_manager->signatures[SIGNATURE_SET_LOCAL_PLAYER_READY])(crypt_str(""));

	return original(ecx, filter, entity_index, channel, sound_entry, sound_entry_hash, sample, volume, attenuation, seed, flags, pitch, origin, direction, vec_origins, update_positions, sound_time, speaker_entity, test);
}