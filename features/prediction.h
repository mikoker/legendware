#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"
#include "local_animations.h"
#include "features.h"

struct ViewmodelData
{
	Weapon* weapon = nullptr;

	int sequence = 0;
	int animation_parity = 0;

	float animation_time = 0.0f;
	float cycle = 0.0f;
};

struct NetvarsData
{
	int m_command_number = 0;

	int m_nTickBase = 0;

	float m_vecViewOffset = 0.0f;
	float m_viewPunchAngle = 0.0f;
	float m_flPostponeFireReadyTime = 0.0f;

	Vector m_aimPunchAngle;
	Vector m_aimPunchAngleVel;
	Vector m_vecOrigin;
};

struct TickbaseData
{
	int command_number = 0;
	int tickbase = 0;
};

struct RestoreData //-V730
{
	bool valid = false;

	int m_hGroundEntity = 0;
	int m_nMoveType = 0;
	int m_nFlags = 0;
	int m_nTickBase = 0;

	float m_flFallVelocity = 0.0f;
	float m_flVelocityModifier = 0.0f;
	float m_flDuckAmount = 0.0f;
	float m_flDuckSpeed = 0.0f;
	float m_surfaceFriction = 0.0f;
	float m_fAccuracyPenalty = 0.0f;
	float m_flRecoilIndex = 0.0f;

	Vector m_aimPunchAngle;
	Vector m_aimPunchAngleVel;
	Vector m_viewPunchAngle;
	Vector m_vecViewOffset;
	Vector m_vecBaseVelocity;
	Vector m_vecVelocity;
	Vector m_vecAbsVelocity;
	Vector m_vecOrigin;

	CMoveData move_data;

	void reset()
	{
		valid = false;

		m_hGroundEntity = 0;
		m_nMoveType = 0;
		m_nFlags = 0;
		m_nTickBase = 0;

		m_flFallVelocity = 0.0f;
		m_flVelocityModifier = 0.0f;
		m_flDuckAmount = 0.0f;
		m_flDuckSpeed = 0.0f;
		m_surfaceFriction = 0.0f;
		m_fAccuracyPenalty = 0.0f;
		m_flRecoilIndex = 0.0f;

		m_aimPunchAngle.Zero();
		m_aimPunchAngleVel.Zero();
		m_viewPunchAngle.Zero();
		m_vecViewOffset.Zero();
		m_vecBaseVelocity.Zero();
		m_vecVelocity.Zero();
		m_vecAbsVelocity.Zero();
		m_vecOrigin.Zero();
	}
};

class Prediction //-V730
{
	float curtime = 0.0f;
	float frametime = 0.0f;

	crypt_ptr <int> prediction_random_seed;
	crypt_ptr <int> prediction_player;

	ViewmodelData viewmodel_data;
	RestoreData restore_data;
public:
	int buttons = 0;
	int flags = 0;
	int move_type = MOVETYPE_NONE;
	int latency = 0;
	int final_predicted_tick = 0;

	bool first_time_predicted;
	bool in_prediction;

	float duck_amount = 0.0f;
	float velocity_modifier = 0.0f;
	float interpolation_amount = 0.0f;

	Vector origin;
	Vector velocity;

	CMoveData move_data;
	NetvarsData netvars_data[MULTIPLAYER_BACKUP];

	virtual void start(crypt_ptr <CUserCmd> cmd);
	virtual void update_data();
	virtual void store_data(crypt_ptr <CUserCmd> cmd);
	virtual void end();
	virtual void store_netvars(int command_number);
	virtual void restore_netvars(int command_number);
	virtual void detect_prediction_error(int command_number);
	virtual void store_viewmodel();
	virtual void restore_viewmodel();
	virtual void fix_velocity_modifier(bool post_prediction, int command_number);
	virtual void fix_interpolation(bool post_interpolation, bool viewmodel = false);
	virtual void store_restore_data(RestoreData& restore_data = engine_prediction->restore_data);
	virtual void apply_restore_data(RestoreData& restore_data = engine_prediction->restore_data);
};