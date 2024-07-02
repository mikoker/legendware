#pragma once
#include "../../data.h"
struct Texture_t
{
	std::byte			pad0[0xC];		// 0x0000
	IDirect3DTexture9* lpRawTexture;	// 0x000C
};

class ITexture
{
private:
	std::byte	pad0[0x50];		 // 0x0000
public:
	Texture_t** pTextureHandles; // 0x0050

	IDirect3DTexture9* GetRawTexture()
	{
		if (!pTextureHandles)
			return nullptr;

		if (!pTextureHandles[0])
			return nullptr;

		return pTextureHandles[0]->lpRawTexture;
	}

	int GetActualWidth()
	{
		using Fn = int(__thiscall*)(void*);
		return call_virtual< Fn >(this, 3)(this);
	}

	int GetActualHeight()
	{
		using Fn = int(__thiscall*)(void*);
		return call_virtual< Fn >(this, 4)(this);
	}

	void IncrementReferenceCount()
	{
		using Fn = void(__thiscall*)(void*);
		call_virtual< Fn >(this, 10)(this);

	}

	void DecrementReferenceCount()
	{
		using Fn = void(__thiscall*)(void*);
		call_virtual< Fn >(this, 11)(this);
	}
};
