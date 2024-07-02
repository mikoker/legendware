#include "legit.h"
#include "..\imgui_ex.h"

int legit_wconfig = 0;

namespace core::tabs::legit
{
	void first_tab()
	{
		imgui_ex::checkbox(crypt_str("Enable"), &config->legit.enable);

		if (config->legit.enable)
			imgui_ex::hotkey(crypt_str("##enable_key"), config->legit.enable_key);

		const char* checks[] = { "Flash", "Smoke" };
		imgui_ex::mcombo(crypt_str("Checks"), config->legit.checks, checks, IM_ARRAYSIZE(checks));
		imgui_ex::slider_int(crypt_str("Backtrack limit"), &config->legit.backtrack_limit, 0, 200, ImGuiSliderFlags_NoneIfZero, crypt_str("%d ms"));
	}

	void second_tab()
	{
		ImGui::PushStyleColor(ImGuiCol_Button, 0);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0);

		ImGui::PushStyleColor(ImGuiCol_TabText, s::is_theme_dark ? IM_COL32A(120, 120, 120) : IM_COL32A(203, 203, 203));
		ImGui::PushStyleColor(ImGuiCol_TabTextActive, s::is_theme_dark ? IM_COL32A(203, 203, 203) : IM_COL32A(120, 120, 120));

		imgui_ex::subtab(crypt_str("A"), ImVec2(42.0f, 25.0f), legit_wconfig, WEAPON_CONFIG_HEAVY_PISTOL, f::weapon_icons, 16.0f, ImVec2(-2.0f, 0.0f));

		imgui_ex::same_line(24.0f);
		imgui_ex::subtab(crypt_str("B"), ImVec2(70.0f, 25.0f), legit_wconfig, WEAPON_CONFIG_PISTOL, f::weapon_icons, 16.0f, ImVec2(2.5f, 1.0f));

		imgui_ex::same_line(14.0f);
		imgui_ex::subtab(crypt_str("Y"), ImVec2(60.0f, 25.0f), legit_wconfig, WEAPON_CONFIG_AUTO, f::weapon_icons, 16.0f, ImVec2(1.0f, 1.0f));

		imgui_ex::subtab(crypt_str("a"), ImVec2(50.0f, 25.0f), legit_wconfig, WEAPON_CONFIG_SCOUT, f::weapon_icons, 16.0f, ImVec2(5.0f, 0.0f));

		imgui_ex::same_line(14.0f);
		imgui_ex::subtab(crypt_str("Z"), ImVec2(70.0f, 25.0f), legit_wconfig, WEAPON_CONFIG_AWP, f::weapon_icons, 16.0f, ImVec2(10.0f, 0.0f));

		imgui_ex::same_line(14.0f);
		imgui_ex::subtab(crypt_str("W"), ImVec2(60.0f, 25.0f), legit_wconfig, WEAPON_CONFIG_RIFLE, f::weapon_icons, 16.0f, ImVec2(4.0f, 0.0f));

		imgui_ex::subtab(crypt_str("P"), ImVec2(45.0f, 25.0f), legit_wconfig, WEAPON_CONFIG_SMG, f::weapon_icons, 16.0f, ImVec2(0.0f, 0.0f));

		imgui_ex::same_line(25.0f);
		imgui_ex::subtab(crypt_str("e"), ImVec2(50.0f, 25.0f), legit_wconfig, WEAPON_CONFIG_HEAVY, f::weapon_icons, 16.0f, ImVec2(9.0f, 0.0f));

		imgui_ex::same_line(34.0f);
		imgui_ex::subtab(crypt_str("f"), ImVec2(60.0f, 25.0f), legit_wconfig, WEAPON_CONFIG_MACHINE_GUN, f::weapon_icons, 16.0f, ImVec2(-6.0f, 0.0f));

		ImGui::PopStyleColor(5);
	}

	void third_tab()
	{
		imgui_ex::slider_float(crypt_str("Field of view"), &config->legit.weapon[legit_wconfig].field_of_view, 1.0f, 30.0f, crypt_str("%.1f"));
		imgui_ex::combo(crypt_str("Smooth"), &config->legit.weapon[legit_wconfig].smooth, v::smooth, IM_ARRAYSIZE(v::smooth));

		if (config->legit.weapon[legit_wconfig].smooth)
			imgui_ex::slider_float(crypt_str("Smooth value"), &config->legit.weapon[legit_wconfig].smooth_value, 1.0f, 30.0f, crypt_str("%.1f"));

		imgui_ex::slider_int(crypt_str("Pitch recoil"), &config->legit.weapon[legit_wconfig].pitch_recoil, 0, 100, ImGuiSliderFlags_NoneIfZero);
		imgui_ex::slider_int(crypt_str("Yaw recoil"), &config->legit.weapon[legit_wconfig].yaw_recoil, 0, 100, ImGuiSliderFlags_NoneIfZero);
		imgui_ex::slider_int(crypt_str("First shot delay"), &config->legit.weapon[legit_wconfig].first_shot_delay, 0, 500, ImGuiSliderFlags_NoneIfZero, crypt_str("%d ms"));
		imgui_ex::slider_int(crypt_str("Target switch delay"), &config->legit.weapon[legit_wconfig].target_switch_delay, 0, 500, ImGuiSliderFlags_NoneIfZero, crypt_str("%d ms"));
	}

	void fifth_tab()
	{
		imgui_ex::checkbox(crypt_str("Automatic attack"), &config->legit.weapon[legit_wconfig].automatic_attack);

		if (config->legit.weapon[legit_wconfig].automatic_attack)
			imgui_ex::slider_int(crypt_str("Attack delay"), &config->legit.weapon[legit_wconfig].fire_delay, 0, 500, ImGuiSliderFlags_NoneIfZero, crypt_str("%d ms"));

		imgui_ex::checkbox(crypt_str("Automatic wall"), &config->legit.weapon[legit_wconfig].automatic_wall);

		if (config->legit.weapon[legit_wconfig].automatic_wall)
			imgui_ex::hotkey(crypt_str("##automatic_wall_key"), config->legit.automatic_wall_key);
		imgui_ex::set_hotkey_visible(config->legit.automatic_wall_key, config->legit.weapon[legit_wconfig].automatic_wall);

		imgui_ex::checkbox(crypt_str("Automatic fire"), &config->legit.weapon[legit_wconfig].automatic_fire);

		if (config->legit.weapon[legit_wconfig].automatic_fire)
			imgui_ex::hotkey(crypt_str("##automatic_fire_key"), config->legit.automatic_fire_key);
		imgui_ex::set_hotkey_visible(config->legit.automatic_fire_key, config->legit.weapon[legit_wconfig].automatic_fire);

		if (config->legit.weapon[legit_wconfig].automatic_fire && config->legit.automatic_fire_key.valid())
		{
			imgui_ex::checkbox(crypt_str("Automatic stop"), &config->legit.weapon[legit_wconfig].automatic_stop);
			imgui_ex::slider_int(crypt_str("Hitchance"), &config->legit.weapon[legit_wconfig].hit_chance, 0, 100, ImGuiSliderFlags_NoneIfZero);
			imgui_ex::slider_int(crypt_str("Minimum damage"), &config->legit.weapon[legit_wconfig].minimum_damage, 0, 120, ImGuiSliderFlags_NoneIfZero, config->legit.weapon[legit_wconfig].minimum_damage > 100 ? (crypt_str("HP + ") + to_string(config->legit.weapon[legit_wconfig].minimum_damage - 100)).c_str() : nullptr);
			imgui_ex::slider_int(crypt_str("Delay"), &config->legit.weapon[legit_wconfig].delay, 0, 500, ImGuiSliderFlags_NoneIfZero, crypt_str("%d ms"));
		}
	}
}
