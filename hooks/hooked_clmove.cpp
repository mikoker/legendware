// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\features.h"
#include "..\features\exploits.h"

void __vectorcall hooked_clmove(float accumulated_extra_samples, bool final_tick)
{
	if (!engine->IsInGame())
		return ((CLMove)original_clmove)(accumulated_extra_samples, final_tick);

	auto net_channel = crypt_ptr <INetChannelInfo>(engine->GetNetChannelInfo());

	if (!net_channel)
		return ((CLMove)original_clmove)(accumulated_extra_samples, final_tick);

	if (net_channel->IsLoopback())
		return ((CLMove)original_clmove)(accumulated_extra_samples, final_tick);

	ctx->backup_first.tickcount = globals->tickcount;
	ctx->backup_first.old_tickcount = clientstate->m_nOldTickCount;
	ctx->backup_first.frametime = globals->frametime;
	ctx->backup_first.curtime = globals->curtime;

	((CLReadPackets)original_clreadpackets)(final_tick);

	ctx->backup_second.tickcount = globals->tickcount;
	ctx->backup_second.old_tickcount = clientstate->m_nOldTickCount;
	ctx->backup_second.frametime = globals->frametime;
	ctx->backup_second.curtime = globals->curtime;

	globals->tickcount = ctx->backup_first.tickcount;
	clientstate->m_nOldTickCount = ctx->backup_first.old_tickcount;
	globals->frametime = ctx->backup_first.frametime;
	globals->curtime = ctx->backup_first.curtime;

	prediction->Update(clientstate->m_nDeltaTick, clientstate->m_nDeltaTick > 0, clientstate->m_nLastCommandAck, clientstate->m_nLastOutgoingCommand + clientstate->m_nChokedCommands);
	return ((CLMove)original_clmove)(accumulated_extra_samples, final_tick);
}