#include "hooks.h"

int __fastcall hooked_get_unverified_file_hashes(void* ecx, void* edx, void* pFiles, int nMaxFiles)
{
	static auto original = hooks_manager->hooks[HOOK_FILESYSTEM]->get_func_address <GetUnverifiedFileHashes>(crypt_hash_n(101));

	if (config->misc.bypasses[BYPASS_SV_PURE])
		return 0;
	return original(ecx, pFiles, nMaxFiles);
}