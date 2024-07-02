// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\prediction.h"
#include "..\features\logs.h"
#include "..\features\features.h"

void __fastcall hooked_packetend(void* ecx, void* edx)
{
    static auto original = hooks_manager->hooks[HOOK_CLIENTSTATE]->get_func_address <PacketEnd> (crypt_hash_n(6));
    return original(ecx);
}