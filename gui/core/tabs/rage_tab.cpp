#include "rage.h"
#include "..\imgui_ex.h"
#include "..\..\..\globals.h"

int weapon_config = 0;

namespace core::tabs::rage
{
	void first_tab()
	{
		imgui_ex::checkbox(crypt_str("Enable"), &config->rage.enable);
		imgui_ex::checkbox(crypt_str("Silent"), &config->rage.silent);
		imgui_ex::checkbox(crypt_str("Automatic fire"), &config->rage.automatic_fire);
		imgui_ex::checkbox(crypt_str("Automatic wall"), &config->rage.automatic_wall);
		imgui_ex::slider_int(crypt_str("Field of view"), &config->rage.field_of_view, 1, 180);
		imgui_ex::checkbox(crypt_str("Multithreading"), &ctx->mt_point_scan);
	}

	void second_tab()
	{
		ImGui::PushStyleColor(ImGuiCol_Button, 0);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0);

		ImGui::PushStyleColor(ImGuiCol_TabText, s::is_theme_dark ? IM_COL32A(120, 120, 120) : IM_COL32A(203, 203, 203));
		ImGui::PushStyleColor(ImGuiCol_TabTextActive, s::is_theme_dark ? IM_COL32A(203, 203, 203) : IM_COL32A(120, 120, 120));

		imgui_ex::subtab(crypt_str("A"), ImVec2(42.0f, 25.0f), weapon_config, WEAPON_CONFIG_HEAVY_PISTOL, f::weapon_icons, 16.0f, ImVec2(-2.0f, 0.0f));

		imgui_ex::same_line(24.0f);
		imgui_ex::subtab(crypt_str("B"), ImVec2(70.0f, 25.0f), weapon_config, WEAPON_CONFIG_PISTOL, f::weapon_icons, 16.0f, ImVec2(2.5f, 1.0f));

		imgui_ex::same_line(14.0f);
		imgui_ex::subtab(crypt_str("Y"), ImVec2(60.0f, 25.0f), weapon_config, WEAPON_CONFIG_AUTO, f::weapon_icons, 16.0f, ImVec2(1.0f, 1.0f));

		imgui_ex::subtab(crypt_str("a"), ImVec2(50.0f, 25.0f), weapon_config, WEAPON_CONFIG_SCOUT, f::weapon_icons, 16.0f, ImVec2(5.0f, 0.0f));

		imgui_ex::same_line(14.0f);
		imgui_ex::subtab(crypt_str("Z"), ImVec2(70.0f, 25.0f), weapon_config, WEAPON_CONFIG_AWP, f::weapon_icons, 16.0f, ImVec2(10.0f, 0.0f));

		imgui_ex::same_line(14.0f);
		imgui_ex::subtab(crypt_str("W"), ImVec2(60.0f, 25.0f), weapon_config, WEAPON_CONFIG_RIFLE, f::weapon_icons, 16.0f, ImVec2(4.0f, 0.0f));

		imgui_ex::subtab(crypt_str("P"), ImVec2(45.0f, 25.0f), weapon_config, WEAPON_CONFIG_SMG, f::weapon_icons, 16.0f, ImVec2(0.0f, 0.0f));

		imgui_ex::same_line(25.0f);
		imgui_ex::subtab(crypt_str("e"), ImVec2(50.0f, 25.0f), weapon_config, WEAPON_CONFIG_HEAVY, f::weapon_icons, 16.0f, ImVec2(9.0f, 0.0f));

		imgui_ex::same_line(34.0f);
		imgui_ex::subtab(crypt_str("f"), ImVec2(60.0f, 25.0f), weapon_config, WEAPON_CONFIG_MACHINE_GUN, f::weapon_icons, 16.0f, ImVec2(-6.0f, 0.0f));

		ImGui::PopStyleColor(5);
	}

	void third_tab()
	{
		imgui_ex::slider_int(crypt_str("Hitchance"), &config->rage.weapon[weapon_config].hit_chance, 0, 100, ImGuiSliderFlags_NoneIfZero);

		imgui_ex::slider_int(crypt_str("Minimum damage"), &config->rage.weapon[weapon_config].minimum_damage, 0, 120, ImGuiSliderFlags_NoneIfZero, config->rage.weapon[weapon_config].minimum_damage > 100 ? (crypt_str("HP + ") + to_string(config->rage.weapon[weapon_config].minimum_damage - 100)).c_str() : nullptr);
		imgui_ex::text(crypt_str("Force damage"));
		imgui_ex::hotkey(crypt_str("##force_damage_key"), config->rage.force_damage_key);

		if (config->rage.force_damage_key.valid())
			imgui_ex::slider_int(crypt_str("Value"), &config->rage.weapon[weapon_config].force_damage_value, 0, 120, ImGuiSliderFlags_NoneIfZero, config->rage.weapon[weapon_config].force_damage_value > 100 ? (crypt_str("HP + ") + to_string(config->rage.weapon[weapon_config].force_damage_value - 100)).c_str() : nullptr);

		imgui_ex::combo(crypt_str("Body aim"), &config->rage.weapon[weapon_config].body_aim, v::body_aim, IM_ARRAYSIZE(v::body_aim));
	}

	void forth_tab()
	{
		imgui_ex::text(crypt_str("Double tap"));
		imgui_ex::hotkey(crypt_str("##double_tap_key"), config->rage.double_tap_key);

		if (config->rage.double_tap_key.valid())
		{
			imgui_ex::slider_int(crypt_str("Double tap hitchance"), &config->rage.weapon[weapon_config].double_tap_hit_chance, 0, 100, ImGuiSliderFlags_NoneIfZero);
			imgui_ex::checkbox(crypt_str("Lag exploit"), &config->rage.weapon[weapon_config].lag_exploit);
		}

		imgui_ex::text(crypt_str("Hide shots"));
		imgui_ex::hotkey(crypt_str("##hide_shots_key"), config->rage.hide_shots_key);
	}

	void fifth_tab()
	{
		imgui_ex::checkbox(crypt_str("Automatic stop"), &config->rage.weapon[weapon_config].automatic_stop);

		if (config->rage.weapon[weapon_config].automatic_stop)
			imgui_ex::mcombo(crypt_str("Modifiers"), config->rage.weapon[weapon_config].modifiers, v::modifiers, IM_ARRAYSIZE(v::modifiers));

		if (config->rage.double_tap_key.valid())
			imgui_ex::checkbox(crypt_str("Extended teleport"), &config->rage.weapon[weapon_config].extended_teleport);

		imgui_ex::checkbox(crypt_str("Force body aim if lethal"), &config->rage.weapon[weapon_config].force_body_aim_if_lethal);
		imgui_ex::checkbox(crypt_str("Force safe points if lethal"), &config->rage.weapon[weapon_config].force_safe_points_if_lethal);
		imgui_ex::checkbox(crypt_str("Static point scale"), &config->rage.weapon[weapon_config].static_point_scale);

		if (config->rage.weapon[weapon_config].static_point_scale)
		{
			imgui_ex::slider_float(crypt_str("Head scale"), &config->rage.weapon[weapon_config].head_scale, 0.0f, 1.0f, crypt_str("%.2f"), 205.0f, ImGuiSliderFlags_NoneIfZero);
			imgui_ex::slider_float(crypt_str("Body scale"), &config->rage.weapon[weapon_config].body_scale, 0.0f, 1.0f, crypt_str("%.2f"), 205.0f, ImGuiSliderFlags_NoneIfZero);
		}

		ImGui::Spacing();
	}
}