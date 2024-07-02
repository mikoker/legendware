#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"

class FakeLag
{
	bool choke_trigger = false;
	bool force_trigger_lag = false;

	int lag_limit = 0;
	int dynamic_choke = 0;

	float velocity = 0.0f;

	virtual bool trigger_lag();
	virtual bool trigger_peek(crypt_ptr <CUserCmd> cmd);
public:	
	bool peek = false;

	virtual void run(crypt_ptr <CUserCmd> cmd);
	virtual void jitter_velocity(crypt_ptr <CUserCmd> cmd);
};