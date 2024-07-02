#pragma once
#include "..\..\data.h"

#define OFFSET(type, name, offset) \
	type &name() \
	{ \
		return *(type*)((uintptr_t)this + offset); \
	}

class CSGameRulesProxy
{
public:
	OFFSET(bool, m_bIsValveDS, 0x7C);
	OFFSET(bool, m_totalRoundsPlayed, 0x64);
};