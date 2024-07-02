#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"
#include "animations.h"
#include "penetration.h"

struct LegitPreparedTarget
{
	int index;

	crypt_ptr <Player> player;
	crypt_ptr <deque <AnimationData>> data;

	LegitPreparedTarget()
	{
		reset();
	}

	LegitPreparedTarget(int index, crypt_ptr <Player> player) //-V818 //-V688
	{
		this->index = index;
		this->player = player;

		if (this->player)
			data = &animations->animation_data[index];
	}

	virtual void reset()
	{
		index = -1;

		player = nullptr;
		data = nullptr;
	}
};

struct LegitPoint
{
	Vector point;
	PenetrationInfo penetration_info;

	LegitPoint()
	{

	}

	LegitPoint(const Vector& point) //-V818
	{
		this->point = point;
	}
};

struct LegitTarget
{
	bool visible;

	int hitbox;
	int hitgroup;

	float field_of_view;
	LegitPoint point;

	crypt_ptr <Player> player;
	crypt_ptr <AnimationData> data;

	LegitTarget()
	{
		reset(); //-V1053
	}

	void reset()
	{
		visible = true;

		hitbox = -1;
		hitgroup = -1;
		field_of_view = FLT_MAX;

		player = nullptr;
		data = nullptr;
	}
};

struct LegitHitbox
{
	int hitbox;
	int priority;

	LegitHitbox()
	{
		hitbox = -1;
		priority = -1;
	}

	LegitHitbox(int hitbox, int priority)
	{
		this->hitbox = hitbox;
		this->priority = priority;
	}
};

class LegitAim
{
	bool attack = false;
	bool first_shot = false;

	int previous_target = 0;

	float delay = 0.0f;
	float first_shot_delay = 0.0f;
	float target_switch_delay = 0.0f;

	Vector original_viewangles;
	LegitTarget final_target;

	vector <LegitPreparedTarget> targets;
	vector <AnimationData> backup;

	virtual void prepare();
	virtual void scan();
	virtual void fire(crypt_ptr <CUserCmd> cmd);
	virtual void automatic_fire(crypt_ptr <CUserCmd> cmd);
	virtual void recoil_control_system(crypt_ptr <CUserCmd> cmd);
	virtual void scan_hitboxes(crypt_ptr <Player> player, crypt_ptr <AnimationData> data);
	virtual float get_smooth_value();
	virtual bool is_hit_chanced(float hit_chance, const Vector& angle, int matrix, int hitbox_index, crypt_ptr <Player> player, crypt_ptr <AnimationData> data);
	virtual Vector get_spread(int seed);
	virtual bool clip_ray_to_hitbox(const Ray_t& ray, crypt_ptr <mstudiobbox_t> hitbox, matrix3x4_t& matrix, CGameTrace& trace);
	virtual bool hitbox_intersection(int hitbox_index, int matrix, crypt_ptr <Player> player, crypt_ptr <AnimationData> data, const Vector& end);
public:
	Vector previous_aim_punch_angle;

	virtual void run(crypt_ptr <CUserCmd> cmd);
};