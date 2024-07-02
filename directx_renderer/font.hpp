#pragma once
#include "renderer.h"

class Font
{
	ID3DXFont* font;
public:
	Font()
	{
		font = nullptr;
	}

	~Font()
	{
		release();
	}

	ID3DXFont* get()
	{
		return font;
	}

	void release()
	{
		if (!font)
			return;

		font->Release();
		font = nullptr;
	}

	bool update(crypt_ptr <IDirect3DDevice9> device, const string& name, unsigned int size, unsigned int weight, bool antialiased)
	{

		release();

		return D3DXCreateFontA(device.get(), size, 0, weight, 0, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, antialiased ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, name.c_str(), &font) == D3D_OK;


	}
};