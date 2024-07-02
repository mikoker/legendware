#include "plist.h"
#include "..\imgui_ex.h"
#include "..\..\..\globals.h"
#include "..\..\..\features\features.h"
#include "..\..\..\features\logs.h"

int player = 0;
int player_index = 0;

namespace core::tabs::plist
{
	void first_tab()
	{
		if (!config->player_list.updating)
		{
			config->player_list.players = config->player_list.cached_players;
			config->player_list.indexes = config->player_list.cached_indexes;
		}

		ImGui::SetCursorPosY(10.0f);
		ImGui::BeginGroup();
		{
			if (config->player_list.players.empty())
			{
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				ImVec2 pos = ImGui::GetWindowPos();

				draw_list->AddImage(i::playerlist_ph, pos + u::dpi(ImVec2(56.0f, 140.0f)), pos + u::dpi(ImVec2(180.0f, 388.0f)),
					ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::GetColorU32(ImGuiCol_Logo, ImGui::GetStyle().Alpha));

				imgui_ex::draw_text(draw_list, crypt_str("There is no players"), u::dpi(ImVec2(65.0f, 407.0f)), f::dinpro);

				imgui_ex::draw_text(draw_list, crypt_str("for now."), u::dpi(ImVec2(94.0f, 426.0f)), f::dinpro);
			}
			else
			{
				for (auto i = 0; i < config->player_list.players.size(); ++i)
				{
					auto is_selected = player == i;

					if (imgui_ex::selectable(config->player_list.players.at(i).c_str(), is_selected))
						player = i;
				}
			}
			
		}
		ImGui::Spacing();
		ImGui::EndGroup();

		if (config->player_list.players.empty())
			return;

		if (player >= config->player_list.players.size())
			player = config->player_list.players.size() - 1;

		player_index = config->player_list.indexes.at(player);
	}

	void second_tab()
	{
		if (config->player_list.players.empty())
		{
			imgui_ex::draw_text(ImGui::GetWindowDrawList(), crypt_str("There are no options for now."), u::dpi(ImVec2(15.0f, 10.0f)), f::dinpro, 17.0f);
			return;
		}

		player_info_t player_info;
		engine->GetPlayerInfo(player_index, &player_info);

		imgui_ex::checkbox(crypt_str("Ignore"), &config->player_list.player_settings[player_index].ignore);

		imgui_ex::checkbox(crypt_str("Highlight"), &config->player_list.player_settings[player_index].hightlight);

		if (config->player_list.player_settings[player_index].hightlight)
		{
			imgui_ex::color_edit4(crypt_str("##hightlight_clr"), config->player_list.player_settings[player_index].hightlight_color);

			imgui_ex::text(crypt_str("Outline color"));
			imgui_ex::color_edit4(crypt_str("##hightlight_out_clr"), config->player_list.player_settings[player_index].hightlight_color_out);

			imgui_ex::checkbox(crypt_str("Fade"), &config->player_list.player_settings[player_index].hightlight_fade);

			imgui_ex::slider_int(crypt_str("Distance"), &config->player_list.player_settings[player_index].hightlight_distance, 1, 100);
			imgui_ex::slider_int(crypt_str("Size"), &config->player_list.player_settings[player_index].hightlight_size, 1, 100);
		}

		imgui_ex::checkbox(crypt_str("Force pitch"), &config->player_list.player_settings[player_index].force_pitch);

		if (config->player_list.player_settings[player_index].force_pitch)
			imgui_ex::slider_int(crypt_str("Pitch"), &config->player_list.player_settings[player_index].pitch, -89, 89);

		imgui_ex::checkbox(crypt_str("Force body yaw"), &config->player_list.player_settings[player_index].force_body_yaw);

		if (config->player_list.player_settings[player_index].force_body_yaw)
			imgui_ex::slider_int(crypt_str("Body yaw"), &config->player_list.player_settings[player_index].body_yaw, -60, 60);

		imgui_ex::checkbox(crypt_str("Force roll"), &config->player_list.player_settings[player_index].force_roll);

		if (config->player_list.player_settings[player_index].force_roll)
			imgui_ex::slider_int(crypt_str("Roll"), &config->player_list.player_settings[player_index].roll, -50, 50);

		imgui_ex::checkbox(crypt_str("Force body aim"), &config->player_list.player_settings[player_index].force_body_aim);
		imgui_ex::checkbox(crypt_str("Force safe points"), &config->player_list.player_settings[player_index].force_safe_points);

		if (player_info.fakeplayer)
			return;

		if (imgui_ex::button(crypt_str("Copy Steam ID"), ImVec2(206.0f, 30.0f)))
		{
			auto data = to_string(player_info.steamID64);
			const size_t len = data.length() + 1;
			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
			memcpy(GlobalLock(hMem), data.c_str(), len);
			GlobalUnlock(hMem);
			OpenClipboard(0);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hMem);
			CloseClipboard();
		}
	}
}