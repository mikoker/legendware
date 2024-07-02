#pragma once
#include "../../data.h"

class IClientLeafSystem
{
public:
	void CreateRenderableHandle(void* obj)
	{
		typedef void(__thiscall* tOriginal)(void*, int, int, char, signed int, char);
		call_virtual<tOriginal>(this, 0)(this, reinterpret_cast<uintptr_t>(obj) + 0x4, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF);
	}

	void AddRenderable(void* obj)
	{
		typedef void(__thiscall* tOriginal)(void*, int, int, int, int, int);
		call_virtual<tOriginal>(this, 7)(this, reinterpret_cast<uintptr_t>(obj) + 0x4, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF);
	}
};