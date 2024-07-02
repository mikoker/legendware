#pragma once

#include "..\includes.h"
#include "..\Gui\core\core.h"
#include "..\Gui\core\imgui_ex.h"

class Menu
{
public:
	bool initialized_imgui = false;
	bool initialized_fonts = false;

	HWND window = nullptr;
	HWND old_window = nullptr;

	virtual void initialize();
	virtual void initialize_gui(crypt_ptr <IDirect3DDevice9> device);
	virtual void draw();
	virtual void release();
};

extern crypt_ptr <Menu> menu;