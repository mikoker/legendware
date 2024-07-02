// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\render.h"
#include "..\menu\menu.h"
#include "..\features\features.h"
#include "..\ImGui Render\ImGUI_Renderer.h"

long __stdcall hooked_present(IDirect3DDevice9* device, RECT* src_rect, RECT* dest_rect, HWND dest_wnd_override, RGNDATA* dirty_region)
{
    static auto original = hooks_manager->hooks[HOOK_DIRECTX]->get_func_address <Present> (crypt_hash_n(17));
    resources->start_download();
    render.update(device);
    render->begin_draw();

    menu->initialize_gui(device);
    menu->draw();

    if (device->BeginScene() == D3D_OK)
    {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    render->end_draw();
    return original(device, src_rect, dest_rect, dest_wnd_override, dirty_region);
}