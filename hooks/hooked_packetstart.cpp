// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

void __fastcall hooked_packetstart(void* ecx, void* edx, int incoming, int outgoing)
{
	static auto original = hooks_manager->hooks[HOOK_CLIENTSTATE]->get_func_address <PacketStart> (crypt_hash_n(5));

    if (ctx->commands.empty())
        return original(ecx, incoming, outgoing);
    if (!ctx->local())
        return original(ecx, incoming, outgoing);
    if (!ctx->local()->valid())
        return original(ecx, incoming, outgoing);

    for (auto it = ctx->commands.rbegin(); it != ctx->commands.rend(); ++it)
    {
        if (!it->is_outgoing)
            continue;

        if (it->command_number == outgoing || outgoing > it->command_number && (!it->is_used || it->previous_command_number == outgoing))
        {
            it->previous_command_number = outgoing;
            it->is_used = true;
            original(ecx, incoming, outgoing);
            break;
        }
    }

    auto result = false;

    for (auto it = ctx->commands.begin(); it != ctx->commands.end();)
    {
        if (outgoing == it->command_number || outgoing == it->previous_command_number)
            result = true;

        if (outgoing > it->command_number && outgoing > it->previous_command_number)
            it = ctx->commands.erase(it);
        else
            ++it;
    }

    if (!result)
        return original(ecx, incoming, outgoing);
}