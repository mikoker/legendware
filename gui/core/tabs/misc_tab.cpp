#include "misc.h"
#include "..\imgui_ex.h"
#include "..\..\..\features\features.h"
#include "..\..\..\features\misc.h"
namespace core::tabs::misc
{
	void first_tab()
	{
		imgui_ex::checkbox(crypt_str("Anti-untrusted"), &config->misc.anti_untrusted);
		
		imgui_ex::mcombo(crypt_str("Bypasses"), config->misc.bypasses, v::bypasses, IM_ARRAYSIZE(v::bypasses));

		if (imgui_ex::button(crypt_str("Unlock hidden convars"), ImVec2(207.0f, 28.0f), false, -1.0f))
			::misc->unlock_cvars();
		ImGui::Spacing();

		imgui_ex::checkbox(crypt_str("Adblock"), &config->misc.adblock);
		imgui_ex::checkbox(crypt_str("Preserve kill feed"), &config->misc.preserve_killfeed);
		imgui_ex::checkbox(crypt_str("Clan tag"), &config->misc.clan_tag);
		imgui_ex::checkbox(crypt_str("Show Legendware users"), &config->misc.display_lw_users);
		imgui_ex::checkbox(crypt_str("Automatic accept"), &config->misc.automatic_accept);
		imgui_ex::checkbox(crypt_str("Rank reveal"), &config->misc.rank_reveal);
		imgui_ex::mcombo_clr(crypt_str("Logs"), config->misc.logs, config->misc.logs_color, v::logs, IM_ARRAYSIZE(v::logs));

		if (config->misc.logs[LOGS_HITS] || config->misc.logs[LOGS_MISSES] || config->misc.logs[LOGS_HURTS] || config->misc.logs[LOGS_PURCHASES])
			imgui_ex::mcombo(crypt_str("Position"), config->misc.logs_position, v::logs_position, IM_ARRAYSIZE(v::logs_position));

		imgui_ex::checkbox(crypt_str("Automatic jump"), &config->misc.automatic_jump);
		imgui_ex::combo(crypt_str("Automatic strafe"), &config->misc.automatic_strafe, v::automatic_strafes, IM_ARRAYSIZE(v::automatic_strafes));
		if (config->misc.automatic_strafe)
			imgui_ex::slider_int(crypt_str("Smoothing"), &config->misc.smoothing, 0, 100);

		imgui_ex::text(crypt_str("Edge jump"));
		imgui_ex::hotkey(crypt_str("##edge_jump_key"), config->misc.edge_jump_key);

		imgui_ex::checkbox(crypt_str("Infinite duck"), &config->misc.infinite_duck);
		imgui_ex::checkbox(crypt_str("Fast stop"), &config->misc.fast_stop);
		
		imgui_ex::text(crypt_str("Automatic peek"));
		imgui_ex::hotkey(crypt_str("##automatic_peek_key"), config->misc.automatic_peek_key);

		if (config->misc.automatic_peek_key.valid())
		{
			imgui_ex::mcombo(crypt_str("Modes"), config->misc.automatic_peek, v::automatic_peek_mode, IM_ARRAYSIZE(v::automatic_peek_mode));
			imgui_ex::text(crypt_str("Color"));
			imgui_ex::color_edit4(crypt_str("##automatic_peek_clr"), config->misc.automatic_peek_clr);
		}

		ImGui::Spacing();
	}

	void second_tab()
	{
		imgui_ex::checkbox(crypt_str("Watermark"), &config->misc.watermark);
		imgui_ex::checkbox(crypt_str("Key binds"), &config->misc.key_binds);
		imgui_ex::checkbox(crypt_str("Spectator list"), &config->misc.spectator_list);
	}

	void third_tab()
	{

		imgui_ex::checkbox(crypt_str("Automatic buy"), &config->misc.automatic_buy);

		if (config->misc.automatic_buy)
		{
			const char* primary[] = { "None", "G3SG1/SCAR-20", "Galil AR/FAMAS", "AK-47/M4A4/M4A1-S", "SSG-08", "SG 553/AUG", "AWP" };
			imgui_ex::combo(crypt_str("Primary"), &config->misc.primary, primary, IM_ARRAYSIZE(primary));

			const char* secondary[] = { "None", "Dual Berettas", "P250", "Tec-9/Five-SeveN", "Desert Eagle/R8 Revolver" };
			imgui_ex::combo(crypt_str("Secondary"), &config->misc.secondary, secondary, IM_ARRAYSIZE(secondary));

			const char* others[] = { "Armor", "Defuser", "Taser", "HE", "Molotov", "Smoke", "Flash", "Decoy" };
			imgui_ex::mcombo(crypt_str("Others"), config->misc.others, others, IM_ARRAYSIZE(others));
		}


		imgui_ex::text(crypt_str("Third-person"));
		imgui_ex::hotkey(crypt_str("##third_person_key"), config->misc.third_person_key);

		if (config->misc.third_person_key.valid())
			imgui_ex::slider_int(crypt_str("Third-person distance"), &config->misc.third_person_distance, 50, 300);

		imgui_ex::slider_int(crypt_str("Field of view"), &config->misc.field_of_view, 70, 140);
		imgui_ex::checkbox(crypt_str("Viewmodel changer"), &config->misc.viewmodel_changer);

		if (config->misc.viewmodel_changer)
		{
			imgui_ex::slider_int(crypt_str("Viewmodel field of view"), &config->misc.viewmodel_field_of_view, 50, 140);
			imgui_ex::slider_int(crypt_str("Viewmodel x"), &config->misc.viewmodel_x, -100, 100);
			imgui_ex::slider_int(crypt_str("Viewmodel y"), &config->misc.viewmodel_y, -100, 100);
			imgui_ex::slider_int(crypt_str("Viewmodel z"), &config->misc.viewmodel_z, -100, 100);
			imgui_ex::slider_int(crypt_str("Viewmodel roll"), &config->misc.viewmodel_roll, -180, 180);
		}

		imgui_ex::checkbox(crypt_str("Aspect ratio"), &config->misc.aspect_ratio);

		if (config->misc.aspect_ratio)
			imgui_ex::slider_float(crypt_str("Value"), &config->misc.aspect_ratio_value, 0.5f, 2.0f);

		ImGui::Spacing();
	}
}