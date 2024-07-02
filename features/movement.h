#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"

class Movement
{
	virtual void fake_duck(crypt_ptr <CUserCmd> cmd);
	virtual void automatic_jump(crypt_ptr <CUserCmd> cmd);
	virtual void automatic_strafe(crypt_ptr <CUserCmd> cmd);
	virtual void infinite_duck(crypt_ptr <CUserCmd> cmd);
	virtual void fast_stop(crypt_ptr <CUserCmd> cmd);
	virtual void automatic_peek(crypt_ptr <CUserCmd> cmd, bool previous_on_ground);
public:
	bool holding_automatic_peek = false;
	bool automatic_peek_shot = false;

	Vector wish_angle;
	Vector strafe_angle;

	virtual void run(crypt_ptr <CUserCmd> cmd);
	virtual void edge_jump(crypt_ptr <CUserCmd> cmd);
	virtual void slow_walk(crypt_ptr <CUserCmd> cmd, float speed = 0.33f);
	virtual void fix_movement(crypt_ptr <CUserCmd> cmd, const Vector& wish_angle, bool roll = false);
	virtual void friction(crypt_ptr <Vector> velocity);
	virtual void accelerate(crypt_ptr <CUserCmd> cmd, crypt_ptr <Vector> velocity, float wish_speed, Vector wish_direction, float max_unclamped_speed);
};