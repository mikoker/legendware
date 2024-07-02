// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\menu\menu.h"

long __stdcall hooked_reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* presentation_parameters)
{
    static auto original = hooks_manager->hooks[HOOK_DIRECTX]->get_func_address <Reset> (crypt_hash_n(16));

	ImGui_ImplDX9_InvalidateDeviceObjects();
	

	return original(device, presentation_parameters);
}