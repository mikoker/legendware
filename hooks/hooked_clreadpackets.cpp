// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

void __cdecl hooked_clreadpackets(bool final_tick)
{
    if (!engine->IsInGame())
        return ((CLReadPackets)original_clreadpackets)(final_tick);

    auto net_channel = crypt_ptr <INetChannelInfo>(engine->GetNetChannelInfo());

    if (!net_channel)
        return ((CLReadPackets)original_clreadpackets)(final_tick);

    if (net_channel->IsLoopback())
        return ((CLReadPackets)original_clreadpackets)(final_tick);

    clientstate->m_nOldTickCount = ctx->backup_second.old_tickcount;
    globals->frametime = ctx->backup_second.frametime;
    globals->curtime = ctx->backup_second.curtime;
    globals->tickcount = ctx->backup_second.tickcount;
}