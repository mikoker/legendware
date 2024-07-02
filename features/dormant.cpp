// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dormant.h"
#include "features.h"
#include "esp.h"
#include "logs.h"

void Dormant_esp::start()
{
	for (auto i = 0; i < 65; ++i) {
		dormant->m_cSoundPlayers[i].found = false;
		dormant->m_cSoundPlayers[i].from_shared = false;
	}

	for (auto i = 1; i <= globals->maxclients; ++i) //-V688
	{
		auto player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(i));

		if (!player)
			continue;

		if (player->IsDormant())
			continue;

		auto observer_mode = player->m_iObserverMode();

		if (observer_mode != OBS_MODE_IN_EYE && observer_mode != OBS_MODE_CHASE)
			continue;

		auto target = crypt_ptr <Player>(player->m_hObserverTarget().Get());

		if (!target)
			continue;

		if (!target->IsDormant())
			continue;

		auto index = target->EntIndex();

		if (index == i)
			continue;

		if (index == engine->GetLocalPlayer())
			continue;

		if (dormant->m_cSoundPlayers[index].found)
			continue;

		dormant->m_cSoundPlayers[index].found = true;
		dormant->m_cSoundPlayers[index].m_iReceiveTime = globals->realtime;
		dormant->m_cSoundPlayers[index].m_vecOrigin = player->m_vecOrigin();
	}

	m_utlCurSoundList.RemoveAll();
	enginesound->GetActiveSounds(m_utlCurSoundList);

	if (!m_utlCurSoundList.Count())
		return;

	for (auto i = 0; i < m_utlCurSoundList.Count(); i++)
	{
		auto& sound = m_utlCurSoundList[i];

		if (!sound.m_bUpdatePositions)
			continue;

		if (sound.m_nSoundSource < 1 || sound.m_nSoundSource > 64)
			continue;

		if (dormant->m_cSoundPlayers[sound.m_nSoundSource].found)
			continue;

		if (sound.m_pOrigin->IsZero())
			continue;

		if (!valid_sound(sound))
			continue;

		auto player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(sound.m_nSoundSource));

		if (player.get() == ctx->local().get())
			continue;

		if (!player->valid(!ctx->friendly_fire))
			continue;

		if (!player->IsDormant())
			continue;

		setup_adjust(player, sound);
		m_cSoundPlayers[sound.m_nSoundSource].Override(sound);

	}

	m_utlvecSoundBuffer = m_utlCurSoundList;
}

void Dormant_esp::setup_adjust(crypt_ptr <Player> player, SndInfo_t& sound)
{
	Vector src3D, dst3D;
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;

	src3D = *sound.m_pOrigin + Vector(0.0f, 0.0f, 1.0f);
	dst3D = src3D - Vector(0.0f, 0.0f, 100.0f);

	filter.pSkip = player.get();
	ray.Init(src3D, dst3D);

	enginetrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

	if (tr.allsolid) {
		m_cSoundPlayers[sound.m_nSoundSource].m_iReceiveTime = 0.0f;
	}
	*sound.m_pOrigin = tr.fraction <= 0.97f ? tr.endpos : *sound.m_pOrigin;

	m_cSoundPlayers[sound.m_nSoundSource].m_nFlags = player->m_fFlags();
	m_cSoundPlayers[sound.m_nSoundSource].m_nFlags |= (tr.fraction < 0.50f ? FL_DUCKING : 0) | (tr.fraction < 1.0f ? FL_ONGROUND : 0);
	m_cSoundPlayers[sound.m_nSoundSource].m_nFlags &= (tr.fraction >= 0.50f ? ~FL_DUCKING : 0) | (tr.fraction >= 1.0f ? ~FL_ONGROUND : 0);
}

bool Dormant_esp::adjust_sound(crypt_ptr <Player> entity)
{
	auto i = entity->EntIndex();
	auto& sound_player = m_cSoundPlayers[i];

	if (sound_player.m_iReceiveTime < this->m_round_start_time && this->m_round_start_time > 0.0f)
		return false;
	if (abs(globals->realtime - sound_player.m_iReceiveTime) > 10.0f)
		return false;

	if (sound_player.m_nFlags)
		entity->m_fFlags() = sound_player.m_nFlags;

	entity->set_abs_origin(sound_player.m_vecOrigin);
	esp->dormant_origin[i] = sound_player.m_vecOrigin;

	return true;
}

bool Dormant_esp::valid_sound(SndInfo_t& sound)
{
	for (auto i = 0; i < m_utlvecSoundBuffer.Count(); ++i)
		if (m_utlvecSoundBuffer[i].m_nGuid == sound.m_nGuid)
			return false;

	return true;
}
