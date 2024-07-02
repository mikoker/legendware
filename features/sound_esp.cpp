#include "sound_esp.h"
#include "dormant.h"
#include "../render.h"
#include "../ImGui Render/ImGUI_Renderer.h"

enum
{
	ENEMY,
	LOCAL,
	TEAM
};

void SoundEsp::run()
{
	if (!engine->IsInGame())
		buffer.clear();

	draw();

	m_cursoundlist.RemoveAll();
	enginesound->GetActiveSounds(m_cursoundlist);

	if (!m_cursoundlist.Count())
		return;

	for (auto i = 0; i < m_cursoundlist.Count(); i++)
	{
		auto& sound = m_cursoundlist[i];

		if (!sound.m_bUpdatePositions)
			continue;

		if (!sound.m_bFromServer)
			continue;

		if (sound.m_nSoundSource < 1 || sound.m_nSoundSource > 64)
			continue;

		auto player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(sound.m_nSoundSource));

		if (!player)
			continue;

		if (!player->valid(false))
			continue;

		if (sound.m_nSoundSource != ctx->local()->EntIndex()) {
			if (player->m_iTeamNum() != ctx->local()->m_iTeamNum() || ctx->friendly_fire)
				player_is = ENEMY;
			else if (player->m_iTeamNum() == ctx->local()->m_iTeamNum())
				player_is = TEAM;
		}
		else
			player_is = LOCAL;

		if (!buffer.empty())
		{
			bool check = false;
			for (auto& sound_check : buffer)
				if (sound_check.m_guid == sound.m_nGuid)
					check = true;

			if (check)
				continue;

		}



		if (sound.m_pOrigin->IsZero())
			continue;
		Vector origin;




		Vector src3D, dst3D;
		CGameTrace tr;
		Ray_t ray;
		CTraceFilter filter;
		if (!player->IsDormant())
			src3D = player->GetAbsOrigin() + Vector(0.0f, 0.0f, 1.0f);
		else
			src3D = *sound.m_pOrigin + Vector(0.0f, 0.0f, 1.0f);
		dst3D = src3D - Vector(0.0f, 0.0f, 100.0f);

		filter.pSkip = player.get();
		ray.Init(src3D, dst3D);

		enginetrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

		if (tr.allsolid)
			continue;

		origin = tr.fraction <= 0.97f ? tr.endpos : *sound.m_pOrigin;

		int sounds_radius;

		switch (player_is)
		{
		case ENEMY:
			sounds_radius = config->visuals.enemy.sounds_radius;
			break;
		case LOCAL:
			sounds_radius = config->visuals.local.sounds_radius;
			break;
		case TEAM:
			sounds_radius = config->visuals.team.sounds_radius;
			break;
		}

		buffer.emplace_back(draw_sound(player_is, sound.m_nGuid, globals->realtime, sounds_radius, origin));
	}
}

void SoundEsp::draw()
{
	if (buffer.empty())
		return;

	for (auto i = 0; i < buffer.size(); ++i)
	{
		auto& circle = buffer.at(i); //-V831

		if (circle.player_is == ENEMY && !config->visuals.enemy.sounds || circle.player_is == LOCAL && !config->visuals.local.sounds || circle.player_is == TEAM && !config->visuals.team.sounds)
			continue;

		float sounds_time;
		float sounds_color[4];

		switch (circle.player_is)
		{
		case ENEMY:
			sounds_time = config->visuals.enemy.sounds_time;
			sounds_color[0] = config->visuals.enemy.sounds_color[0];
			sounds_color[1] = config->visuals.enemy.sounds_color[1];
			sounds_color[2] = config->visuals.enemy.sounds_color[2];
			sounds_color[3] = config->visuals.enemy.sounds_color[3];
			break;
		case LOCAL:
			sounds_time = config->visuals.local.sounds_time;
			sounds_color[0] = config->visuals.local.sounds_color[0];
			sounds_color[1] = config->visuals.local.sounds_color[1];
			sounds_color[2] = config->visuals.local.sounds_color[2];
			sounds_color[3] = config->visuals.local.sounds_color[3];
			break;
		case TEAM:
			sounds_time = config->visuals.team.sounds_time;
			sounds_color[0] = config->visuals.team.sounds_color[0];
			sounds_color[1] = config->visuals.team.sounds_color[1];
			sounds_color[2] = config->visuals.team.sounds_color[2];
			sounds_color[3] = config->visuals.team.sounds_color[3];
			break;
		}

		auto procent = 1.0f - ((((float)circle.m_iReceiveTime + sounds_time) - (float)globals->realtime) / sounds_time);

		if (procent <= 1)
		{
			auto radius = circle.radius + (sounds_time *(circle.radius * procent));

			renderer_imgui.RenderCircle3D(circle.origin, 144, radius, Color(sounds_color[0], sounds_color[1], sounds_color[2], sounds_color[3] * (1.0f - procent)));
			renderer_imgui.RenderCircle3DFilled(circle.origin, 144, radius, Color(sounds_color[0], sounds_color[1], sounds_color[2], (sounds_color[3] * (1.0f - procent)) * 0.75f));
		}
		else
		{
			buffer.erase(buffer.begin() + i);

			if (i > 0)
				--i;

			continue;
		}
	}
}