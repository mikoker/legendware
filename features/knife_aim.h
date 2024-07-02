#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"
#include "aim.h"

class KnifeAim
{
	vector <AnimationData> backup;
	vector <PreparedTarget> targets;

	Target final_target;

	virtual void prepare();
	virtual void scan();
	virtual void fire(crypt_ptr <CUserCmd> cmd);
public:
	virtual void run(crypt_ptr <CUserCmd> cmd);
};