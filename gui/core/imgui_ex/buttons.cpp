// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "buttons.h"

namespace imgui_ex
{
	bool button_ex(const char* label, const ImVec2& size_arg,
		bool draw_icon, bool& is_locked, bool centered, float offset_x = 0.0f)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImVec2 label_size = ImGui::CalcTextSize(core::f::dinpro,
			u::dpi(17.0f), label, nullptr, true);

		ImVec2 pos = window->DC.CursorPos + u::dpi(ImVec2(offset_x, 0.0f));

		ImVec2 size = ImGui::CalcItemSize(u::dpi(size_arg), label_size.x +
			style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos - ImVec2(0.0f, 0.0f), pos + size);

		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			is_locked = !is_locked;

		// Render
		const ImU32 col = ImGui::GetColorU32(held && hovered ? ImGuiCol_ButtonActive
			: hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

		ImGui::RenderNavHighlight(bb, id);
		ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

		if (centered)
		{
			window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f), bb.Min + (
				size - label_size) / 2.0f, ImGui::GetColorU32(ImGuiCol_Text), label);
		}
		else
		{
			window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f), bb.Min + ImVec2(
				u::dpi(14.0f), (size.y - label_size.y) / 2.0f), ImGui::GetColorU32(ImGuiCol_Text), label);
		}

		if (draw_icon)
		{
			const char* icon = is_locked ? (char*)u8"\uf023" : (char*)u8"\uf09c";

			const ImVec2 icon_size = ImGui::CalcTextSize(core::f::awesome,
				u::dpi(7.0f), icon, nullptr, true);

			window->DrawList->AddText(core::f::awesome, u::dpi(7.0f), bb.Max - ImVec2(icon_size.x +
				10.0f, size.y - icon_size.y - u::dpi(7.0f)), ImGui::GetColorU32(ImGuiCol_Text), icon);
		}

		return pressed;
	}

	bool button_lua(const char* label,
		const ImVec2& size_arg, bool is_red)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImVec2 label_size = ImGui::CalcTextSize(core::f::dinpro, 17.0f,
			label, nullptr, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f,
			label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);

		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		ImU32 red_clr = held && hovered ? IM_COL32A(220, 122, 122) : hovered ? IM_COL32A(216, 119, 119) : IM_COL32A(214, 117, 117),
			green_clr = held && hovered ? IM_COL32A(123, 219, 135) : hovered ? IM_COL32A(119, 216, 132) : IM_COL32A(117, 214, 130);

		// Render
		const ImU32 col = is_red ? red_clr : green_clr;

		ImGui::RenderNavHighlight(bb, id);
		ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

		window->DrawList->AddText(core::f::dinpro, 17.0f,
			bb.Min + ImVec2(10.0f, (size_arg.y - label_size.y) / 2.0f), IM_COL32_WHITE, label);

		return pressed;
	}

	bool button(const char* label, const ImVec2& size_arg, bool centered, float offset_x)
	{
		ImGui::PushStyleColor(ImGuiCol_Button,
			ImGui::GetColorU32(ImGuiCol_ButtonEx));

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
			ImGui::GetColorU32(ImGuiCol_ButtonHoveredEx));

		ImGui::PushStyleColor(ImGuiCol_ButtonActive,
			ImGui::GetColorU32(ImGuiCol_ButtonActiveEx));

		bool unused = false;

		const bool result = button_ex(label, size_arg,
			false, unused, centered, offset_x);

		ImGui::PopStyleColor(3);

		return result;
	}

	bool BeginButtonDropDown(const char* label, ImVec2 buttonSize)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		auto& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		float x = ImGui::GetCursorPosX();
		float y = ImGui::GetCursorPosY();

		bool pressed = imgui_ex::button(label, buttonSize);

		// Popup
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, u::dpi(ImVec2(8.0f, 8.0f)));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);
		ImVec2 popupPos;

		popupPos.x = window->Pos.x + x;
		popupPos.y = window->Pos.y + y + buttonSize.y;

		ImGui::SetNextWindowPos(popupPos);

		if (pressed)
		{
			ImGui::OpenPopup(label);
		}

		if (ImGui::BeginPopup(label))
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, style.Colors[ImGuiCol_Button]);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, style.Colors[ImGuiCol_Button]);

			return true;
		}

		return false;
	}

	void EndButtonDropDown()
	{
		ImGui::PopStyleColor(2);

		ImGui::EndPopup();
	}

	int dropdown_button(const char* label, const ImVec2& size_arg, std::vector<std::string> labels, bool centered)
	{
		int ret = -1;
		if (BeginButtonDropDown(label, size_arg))
		{
			for (int i = 0; i < labels.size(); i++)
			{
				if (imgui_ex::button(labels.at(i).c_str(), size_arg, centered)) {
					ret = i;
					ImGui::CloseCurrentPopup();
				}
			}
			EndButtonDropDown();
		}
		ImGui::PopStyleVar(3);
		return ret;
	}

	bool button_lock(const char* label, const ImVec2& size_arg,
		bool& is_locked, bool centered)
	{
		ImGui::PushStyleColor(ImGuiCol_Button,
			ImGui::GetColorU32(ImGuiCol_ButtonEx));

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
			ImGui::GetColorU32(ImGuiCol_ButtonHoveredEx));

		ImGui::PushStyleColor(ImGuiCol_ButtonActive,
			ImGui::GetColorU32(ImGuiCol_ButtonActiveEx));

		const bool result = button_ex(label, size_arg,
			true, is_locked, centered);

		ImGui::PopStyleColor(3);

		return result;
	}
	void button_disabled(const char* label, const ImVec2& size_arg, bool centered)
	{
		ImGui::PushStyleColor(ImGuiCol_Button,
			ImGui::GetColorU32(ImGuiCol_ButtonDisabled));

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
			ImGui::GetColorU32(ImGuiCol_ButtonDisabled));

		ImGui::PushStyleColor(ImGuiCol_ButtonActive,
			ImGui::GetColorU32(ImGuiCol_ButtonDisabled));

		ImGui::PushStyleColor(ImGuiCol_Text,
			ImGui::GetColorU32(ImGuiCol_ButtonDisabledText));

		bool unused = false;

		const bool result = button_ex(label, size_arg,
			false, unused, centered);

		ImGui::PopStyleColor(4);
	}

	bool button_colored(const char* label, const ImVec2& size_arg, ImColor color, ImColor color_hovered, ImColor color_active, bool centered)
	{
		ImGui::PushStyleColor(ImGuiCol_Button,
			ImGui::GetColorU32(color.Value));

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
			ImGui::GetColorU32(color_hovered.Value));

		ImGui::PushStyleColor(ImGuiCol_ButtonActive,
			ImGui::GetColorU32(color_active.Value));

		ImGui::PushStyleColor(ImGuiCol_Text,
			IM_COL32_WHITE);

		bool unused = false;

		const bool result = button_ex(label, size_arg,
			false, unused, centered);

		ImGui::PopStyleColor(4);

		return result;
	}
}
