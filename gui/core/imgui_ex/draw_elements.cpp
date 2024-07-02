// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "draw_elements.h"

namespace imgui_ex
{
	void draw_bar_element(const ImVec2& pos, const char* text,
		const char* icon, int& tab, int tab_num, ImDrawList* draw_list, const ImVec2& win_pos)
	{
		ImGuiContext& g = *GImGui;

		draw_list->AddText(core::f::icons, u::dpi(10.0f), win_pos + u::dpi(ImVec2(pos.x - 34.0f,
			pos.y + 17.0f)), ImGui::GetColorU32(ImGuiCol_Text), icon);

		auto text_pos = win_pos + u::dpi(pos);
		text_pos.y += u::dpi(2);
		draw_list->AddText(tab == tab_num ? core::f::dinpro_bold : core::f::dinpro,
			u::dpi(17.0f), text_pos, ImGui::GetColorU32(ImGuiCol_Text), text);

		std::string label = "##" + std::string(text);

		ImGui::SetCursorPos(u::dpi(ImVec2(pos.x - 34.0f, pos.y - 2.0f)));

		if (ImGui::InvisibleButton(label.c_str(), u::dpi(ImVec2(100.0f, 25.0f)))
			|| g.DragDropActive && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			tab = tab_num;
		}
	}

	void draw_triangle(ImDrawList* draw_list,
		ImVec2 pos1, ImVec2 pos2, ImVec2 pos3)
	{
		ImVec2 win_pos = ImGui::GetWindowPos();
		ImU32 col = ImGui::GetColorU32(ImGuiCol_Triangles, ImGui::GetStyle().Alpha);

		draw_list->AddTriangleFilled(win_pos + u::dpi(pos1), win_pos +
			u::dpi(pos2), win_pos + u::dpi(pos3), col);
	}

	void draw_text(ImDrawList* draw_list, const char* text, const ImVec2& pos,
		ImFont* font, float font_size, const char* text_end)
	{
		ImVec2 win_pos = ImGui::GetWindowPos();

		draw_list->AddText(font, u::dpi(font_size), win_pos + pos,
			ImGui::GetColorU32(ImGuiCol_Text, ImGui::GetStyle().Alpha), text, text_end);
	}
}