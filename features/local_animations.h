#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"

enum
{
	SIDE_REAL,
	SIDE_FAKE,
	SIDE_MAX
};

class LocalAnimations //-V730
{
	bool angle_locked = false;
	bool update_fake_bones = false;

	float last_spawn = 0.0f;
	Vector angle;

	float angles[SIDE_MAX];
	float pose_parameters[SIDE_MAX][24];

	AnimationLayer layers[SIDE_MAX][13];
public:
	float pose_parameters_shoot[24];
	AnimationLayer layers_shoot[13];

	Vector sent_angle;
	AnimationState fake_animstate;

	virtual void render();
	virtual void run(crypt_ptr <CUserCmd> cmd, bool send_packet);
};