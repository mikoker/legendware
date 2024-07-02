#pragma once

#include "..\includes.h"
#include "..\hooks\hooks.h"

class ThirdPerson
{
	bool require_reset = false;
	float current_fraction = 0.0f;
public:
	virtual void run();
};