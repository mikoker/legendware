#pragma once
#include "../globals.h"
struct BulletImpactInfo
{
	float m_flExpTime = 0.0f;
	Vector start;
	Vector m_vecHitPos;
};

class BulletTracer
{
public:
	std::vector<BulletImpactInfo> bulletImpactInfo;

	virtual void pushImpactInfo(BulletImpactInfo beam_info);
	virtual void drawBeam();
};