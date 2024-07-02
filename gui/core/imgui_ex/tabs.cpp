#include "tabs.h"

namespace imgui_ex
{
	bool tab(const char* label, const ImVec2& size_arg,
		int& tab, int tab_num)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImVec2 label_size = ImGui::CalcTextSize(g.Font,
			u::dpi(g.FontSize), label, nullptr, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(u::dpi(size_arg),
			label_size.x + style.FramePadding.x * 2.0f,
			label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		// Render
		ImGui::RenderNavHighlight(bb, id);
		ImGui::RenderFrame(bb.Min, bb.Max, IM_COL32_BLACK_TRANS, false, 0.0f);

		ImVec2 corrected_pos = ImVec2(bb.GetCenter().x - label_size.x / 2.0f,
			bb.GetCenter().y - label_size.y / 2.0f);

		window->DrawList->AddText(tab == tab_num ? core::f::dinpro_bold : core::f::dinpro,
			u::dpi(16.0f), corrected_pos, ImGui::GetColorU32(ImGuiCol_Text, ImGui::GetStyle().Alpha), label);

		if (hovered && held || g.DragDropActive &&
			ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			tab = tab_num;
		}

		return pressed;
	}

	bool subtab(const char* label, const ImVec2& size_arg, int& tab,
		int tab_num, ImFont* font, float font_size, ImVec2 offset)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImVec2 label_size = ImGui::CalcTextSize(font,
			u::dpi(font_size), label, nullptr, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(u::dpi(size_arg),
			label_size.x + style.FramePadding.x * 2.0f,
			label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		// Render
		const ImU32 col = ImGui::GetColorU32(tab == tab_num ? ImGuiCol_ButtonActive
			: hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button, ImGui::GetStyle().Alpha);

		ImGui::RenderNavHighlight(bb, id);
		ImGui::RenderFrame(bb.Min, bb.Max, col, false, 3.0f);

		ImVec2 corrected_pos = ImVec2(bb.GetCenter().x - label_size.x / 2.0f,
			bb.GetCenter().y - label_size.y / 2.0f);

		const ImU32 col_text = ImGui::GetColorU32(tab == tab_num ?
			ImGuiCol_TabTextActive : ImGuiCol_TabText, ImGui::GetStyle().Alpha);

		window->DrawList->AddText(font, u::dpi(font_size),
			corrected_pos + offset, col_text, label);

		if (hovered && held || g.DragDropActive && ImGui::IsItemHovered(
			ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			tab = tab_num;
		}

		return pressed;
	}

	void rage_subtab(const char* label, const ImVec2& size_arg,
		int& tab)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImVec2 label_size = ImGui::CalcTextSize(core::f::dinpro,
			u::dpi(10.0f), label, nullptr, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = u::dpi(ImGui::CalcItemSize(size_arg,
			label_size.x + style.FramePadding.x * 2.0f,
			label_size.y + style.FramePadding.y * 2.0f));

		const ImRect frame_bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(frame_bb, id))
			return;

		// Render
		const ImU32 col = ImGui::GetColorU32(ImGuiCol_Button, ImGui::GetStyle().Alpha);

		ImGui::RenderNavHighlight(frame_bb, id);

		ImGui::RenderFrame(frame_bb.Min - ImVec2(0.5f, 0.5f), frame_bb.Max + ImVec2(0.5f, 0.5f),
			ImGui::GetColorU32(ImGuiCol_Border), false, 3.0f);

		ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, col, false, 3.0f);

		const ImU32 col_right_text = ImGui::GetColorU32(tab == 0 ?
			ImGuiCol_TabTextActive : ImGuiCol_TabText, ImGui::GetStyle().Alpha);

		const ImU32 col_center_text = ImGui::GetColorU32(tab == 1 ?
			ImGuiCol_TabTextActive : ImGuiCol_TabText, ImGui::GetStyle().Alpha);

		const ImU32 col_left_text = ImGui::GetColorU32(tab == 2 ?
			ImGuiCol_TabTextActive : ImGuiCol_TabText, ImGui::GetStyle().Alpha);

		ImVec2 r_pos = frame_bb.Min + u::dpi(ImVec2(45.0f, 15.0f)),
			c_pos = frame_bb.Min + u::dpi(ImVec2(45.0f * 2.0f + 2.0f, 15.0f)),
			l_pos = frame_bb.Min + u::dpi(ImVec2(45.0f * 3.0f, 22.0f));

		window->DrawList->AddText(core::f::awesome, u::dpi(10.0f),
			r_pos, col_right_text, (char*)(char*)u8"\uf183");

		window->DrawList->AddText(core::f::awesome, u::dpi(10.0f),
			c_pos, col_center_text, (char*)(char*)u8"\uf70c");

		window->DrawList->AddText(core::f::icons, u::dpi(10.0f),
			l_pos, col_left_text, (char*)u8"\ue90c");

		const ImRect right_bb(r_pos - ImVec2(15.0f, 15.0f), r_pos + ImGui::CalcTextSize(
			core::f::awesome, u::dpi(10.0f), (char*)u8"\uf183") + ImVec2(10.0f, 5.0f));

		const ImRect center_bb(c_pos - ImVec2(15.0f, 15.0f), c_pos + ImGui::CalcTextSize(
			core::f::awesome, u::dpi(10.0f), (char*)u8"\uf70c") + ImVec2(10.0f, 5.0f));

		const ImRect left_bb(l_pos - ImVec2(5.0f, 24.0f), l_pos + ImGui::CalcTextSize(
			core::f::icons, u::dpi(10.0f), (char*)u8"\ue90c"));

		bool hovered_r, held_r, hovered_c, held_c, hovered_l, held_l;

		ImGui::ButtonBehavior(right_bb, id, &hovered_r, &held_r, 0);
		ImGui::ButtonBehavior(center_bb, id, &hovered_c, &held_c, 0);
		ImGui::ButtonBehavior(left_bb, id, &hovered_l, &held_l, 0);

		if (held_r && hovered_r)
			tab = 0;
		else if (held_c && hovered_c)
			tab = 1;
		else if (held_l && hovered_l)
			tab = 2;
	}

	void rage_subtab(const char* label, int& tab)
	{
		ImGui::PushStyleColor(ImGuiCol_Button,
			ImGui::GetColorU32(ImGuiCol_ChildBg, ImGui::GetStyle().Alpha));

		ImGui::PushStyleColor(ImGuiCol_TabText, core::s::is_theme_dark
			? IM_COL32(120, 120, 120, static_cast<int>(ImGui::GetStyle().Alpha * 255.0f)) :
			IM_COL32(209, 209, 209, static_cast<int>(ImGui::GetStyle().Alpha * 255.0f)));

		ImGui::PushStyleColor(ImGuiCol_TabTextActive, core::s::is_theme_dark
			? IM_COL32(209, 209, 209, static_cast<int>(ImGui::GetStyle().Alpha * 255.0f)) :
			IM_COL32(120, 120, 120, static_cast<int>(ImGui::GetStyle().Alpha * 255.0f)));

		rage_subtab(label, ImVec2(206.0f, 30.0f), tab);

		ImGui::PopStyleColor(3);
	}
}
