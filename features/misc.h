#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"

class Misc
{
	bool removed_clan_tag = false;
	int clan_tag_tick = 0;

	virtual void reveal_ranks(crypt_ptr <CUserCmd> cmd);
public:
	virtual void run(crypt_ptr <CUserCmd> cmd);
	virtual void clan_tag();
	virtual void unlock_cvars();
};