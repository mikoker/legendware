#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"

struct HeadPosition
{
	float angle;
	float fraction;
	Vector position;
	Vector end_position;

	// Dodanie konstruktora przyjmuj¹cego 4 argumenty
	HeadPosition(float a, float f, const Vector& pos, const Vector& end_pos)
		: angle(a), fraction(f), position(pos), end_position(end_pos) {}
};

class AntiAim
{
	bool switch_move = false;
	bool switch_spin = false;
	bool roll_opposite = false;
	bool inverted = false;
	bool inverted_desync = false;
	bool inverted_roll = false;
	bool inverted_choke_cycle = false;

	int desync_random_side = 0;

	float flick_time = 0.0f;
	float spin_offset = 0.0f;

	virtual float get_pitch(crypt_ptr <CUserCmd> cmd);
	virtual float get_yaw(crypt_ptr <CUserCmd> cmd);
	virtual float get_target_yaw(crypt_ptr <CUserCmd> cmd);
	virtual float get_edge_yaw(crypt_ptr <CUserCmd> cmd);
public:
	bool flicked = false;
	bool force_disable_desync = false;
	bool edge_yaw = false;

	virtual void run(crypt_ptr <CUserCmd> cmd);
	virtual float get_roll(crypt_ptr <CUserCmd> cmd);
	virtual void micro_movements(crypt_ptr <CUserCmd> cmd);
};