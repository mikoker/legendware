// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\menu\menu.h"
#include "../gui/core/imgui_ex/lua_editor.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall hooked_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (!engine->IsActiveApp())
        return CallWindowProc((WNDPROC)menu->old_window, hwnd, msg, wparam, lparam);

    static auto holding = false;

    if (!holding && msg == WM_KEYDOWN && (wparam == VK_INSERT || wparam == VK_DELETE))
    {
        holding = true;
        core::s::is_opened = !core::s::is_opened;

        if (core::s::is_opened && ctx->local()->valid() && ctx->weapon_config != WEAPON_CONFIG_INVALID && ctx->weapon_config != WEAPON_CONFIG_TASER && ctx->weapon_config != WEAPON_CONFIG_KNIFE)
        {
            legit_wconfig = ctx->weapon_config;
            weapon_config = ctx->weapon_config;
        }
    }
    else if (holding && msg == WM_KEYUP && (wparam == VK_INSERT || wparam == VK_DELETE))
        holding = false;

 
    if (core::s::is_opened)
    {
        auto skip = false;
        auto key = KEY_NONE;

        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
            key = MOUSE_LEFT;

        if (key != KEY_NONE)
        {
            for (auto& item : config->items)
            {
                if (item.second.type == ITEM_KEY_BIND)
                {
                    auto key_bind = crypt_ptr <KeyBind> ((KeyBind*)item.second.pointer.get());

                    if (key_bind->mode == HOLD && key_bind->state && key_bind->key == key)
                        skip = true;
                }
            }
        }

        if (!skip)
        {
            if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
                return true;

            ImGuiContext* g = GImGui;
            if (g && g->IO.WantTextInput) {
                inputsystem->ResetInputState();
                return true;
            }
  
           /* if (lua_editor.is_open()) {
                inputsystem->ResetInputState();
                return true;
            }*/
            auto pressed_buttons = msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_MOUSEMOVE || msg == WM_MOUSEWHEEL;

            if (pressed_buttons)
                return false;
        }

        if (msg == WM_LBUTTONDBLCLK)
            return false;
    }

    return CallWindowProc((WNDPROC)menu->old_window, hwnd, msg, wparam, lparam);
}