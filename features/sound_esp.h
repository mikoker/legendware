#pragma once
#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"

struct draw_sound {
	draw_sound(int player_is, uint32_t guid, float ReceiveTime, float radius, Vector origin) {
		this->player_is = player_is;
		this->m_guid = guid;
		this->m_iReceiveTime = ReceiveTime;
		this->radius = radius;
		this->origin = origin;
	}

	int player_is;
	uint32_t m_guid;
	float m_iReceiveTime;
	float radius;
	float alpha;
	Vector origin;
};

class SoundEsp {

public:
	virtual void run();
	virtual void draw();
private:
	int player_is = -1;
	CUtlVector <SndInfo_t> m_cursoundlist;
	std::vector<draw_sound> buffer;
};