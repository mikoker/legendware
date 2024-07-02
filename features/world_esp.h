#pragma once

#include "..\includes.h"
#include "..\render.h"
#include "..\config\config.h"
#include "esp.h"

class WorldEsp
{
	Entity* m_e;
	Box box;
	Vector m_origin;
	ClientClass* client_class;
	virtual void rotate_triangle(std::array<Vector2D, 3>& points, float rotation);
	virtual bool getBox();
	virtual void weaponEsp();
	virtual void grenadeEsp();
	virtual void c4Esp();
	virtual void grenadeWarning();
	virtual bool inferno();
	virtual bool smoke();
public:
	virtual void run();
};