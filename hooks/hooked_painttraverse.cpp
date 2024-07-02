// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\features.h"
#include "..\ImGui Render\ImGUI_Renderer.h"


void __fastcall hooked_painttraverse(void* ecx, void* edx, vgui::VPANEL panel_index, bool force_repaint, bool allow_force)
{
	static auto original = hooks_manager->hooks[HOOK_PANEL]->get_func_address <PaintTraverse> (crypt_hash_n(41));
	static auto panel_id = (vgui::VPANEL)0;

	ctx->store();

	if (config->visuals.world.removals[REMOVALS_SCOPE] && crypt_hash("HudZoom") == crypt_hash_r(panel->GetName(panel_index)))
		return;

	original(ecx, panel_index, force_repaint, allow_force);

	if (!panel_id && crypt_hash("FocusOverlayPanel") == crypt_hash_r(panel->GetName(panel_index)))
		panel_id = panel_index;
	else if (panel_id == panel_index)
	{
		static auto initialize = false;
		if (core::s::menu_alpha > 0 && core::s::tab == 3 || !initialize)
		{



			initialize = true;
		}



		renderer_imgui.BeginScene();
	}
}