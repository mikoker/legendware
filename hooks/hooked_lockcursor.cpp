// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\menu\menu.h"
#include "..\features\features.h"

void __stdcall hooked_lockcursor()
{
	static auto original = hooks_manager->hooks[HOOK_SURFACE]->get_func_address <LockCursor> (crypt_hash_n(67));

	if (!core::s::is_opened)
		return original(surface.get());

	surface->UnlockCursor();
}

