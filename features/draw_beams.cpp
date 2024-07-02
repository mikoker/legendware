// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "draw_beams.h"

void BulletTracer::pushImpactInfo(BulletImpactInfo beam_info)
{
	if (!config->visuals.world.bullet_tracer)
	{
		bulletImpactInfo.clear();
		return;
	}
	bulletImpactInfo.emplace_back(beam_info);
}

void BulletTracer::drawBeam()
{
	if (!config->visuals.world.bullet_tracer) //-V807
	{
		bulletImpactInfo.clear();
		return;
	}
		
	auto local_player = ctx->local();
	if (!local_player.get() || !local_player->valid())
		return;

	std::vector<BulletImpactInfo>& impacts = bulletImpactInfo;
	if (impacts.empty())
		return;

	for (size_t i = 0; i < impacts.size(); i++) {

		auto current_impact = impacts.at(i);

		BeamInfo_t beamInfo;
		beamInfo.m_nType = TE_BEAMPOINTS;
		beamInfo.m_pszModelName = crypt_str("sprites/purplelaser1.vmt");
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = 3.0f;
		beamInfo.m_flWidth = 2.0f;
		beamInfo.m_flEndWidth = 1.0f;
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 0.0f;
	
		beamInfo.m_flSpeed = 0.0f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;

		beamInfo.m_flRed = config->visuals.world.bullet_tracer_clr[0] * 255.0f;
		beamInfo.m_flGreen = config->visuals.world.bullet_tracer_clr[1] * 255.0f;
		beamInfo.m_flBlue = config->visuals.world.bullet_tracer_clr[2] * 255.0f;
		beamInfo.m_flBrightness = config->visuals.world.bullet_tracer_clr[3] * 255.0f;

		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = 0;

		beamInfo.m_vecStart = current_impact.start;
		beamInfo.m_vecStart.z -= 2.0f;
		beamInfo.m_vecEnd = current_impact.m_vecHitPos;

		auto beam = beams->CreateBeamPoints(beamInfo);

		if (beam)
			beams->DrawBeam(beam);

		impacts.erase(impacts.begin() + i);
	}
}
