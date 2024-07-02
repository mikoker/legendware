// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "proxy.h"

void hooked_velocitymodifier(RecvProxyData& data, void* entity, void* output)
{
	if (!entity)
		return;

	proxy->get_original(PROXY_VELOCITY_MODIFIER)(data, entity, output);
	auto player = crypt_ptr <Player> ((Player*)((Player*)entity)->GetBaseEntity());

	if (player.get() != ctx->local().get())
		return;

	if (data.value._float == ctx->last_velocity_modifier) //-V550
		return;

	ctx->last_velocity_modifier = data.value._float;
	prediction->m_bPreviousAckHadErrors = TRUE;
}