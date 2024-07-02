#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"
#include "..\render.h"

class Visuals
{
	virtual void crosshair();
	virtual void damage_marker();
	virtual void automatic_peek();
	virtual void manual_anti_aim();
	virtual void scope_lines();
	virtual void field_of_view_circle();
public:
	virtual void run();
};