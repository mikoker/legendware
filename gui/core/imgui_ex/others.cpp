// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "others.h"
#include "../../../config/config.h"
#include <array>
#include "buttons.h"
#include "combos.h"
#include "../../../render.h"

#include <future>
#include <functional>
#include "../../../globals.h"
#include "color_pickers.h"

#include "../../../features/features.h"
constexpr std::array<const char*, 166U> keyNames =
{
	"",
	"mouse 1", "mouse 2", "cancel", "mouse 3", "mouse 4", "mouse 5", "",
	"backspace", "tab", "", "", "clear", "enter", "", "",
	"shift", "control", "alt", "pause", "caps", "", "", "", "", "", "",
	"escape", "", "", "", "", "space", "page up", "page down",
	"end", "home", "left", "up", "right", "down", "", "", "",
	"print", "insert", "delete", "",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"", "", "", "", "", "", "",
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
	"l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
	"v", "w", "x", "y", "z", "lwin", "rwin", "", "", "",
	"num0", "num1", "num2", "num3", "num4", "num5",
	"num6", "num7", "num8", "num9",
	"*", "+", "", "-", ".", "/",
	"f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8",
	"f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16",
	"f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24",
	"", "", "", "", "", "", "", "",
	"num lock", "scroll lock",
	"", "", "", "", "", "", "",
	"", "", "", "", "", "", "",
	"lshift", "rshift", "lctrl",
	"rctrl", "lmenu", "rmenu"
};

namespace imgui_ex
{
	ImGuiStyle backup_style = ImGuiStyle();
	bool slider_float(const char* label, float* v, float v_min,
		float v_max, const char* format = "%.2f", float width = 205.0f, ImGuiSliderFlags flags = 0);

	bool selectable(const char* label, bool selected = false,
		ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = ImVec2(0.0f, 0.0f), float expected_h = 0.0f, int item_id = -1, void* bool_ptr = nullptr);

	void draw_text(ImDrawList* draw_list, const char* text, const ImVec2& pos,
		ImFont* font = core::f::dinpro_bold, float font_size = 16.0f, const char* text_end = nullptr);

	void same_line(float spacing_w, float offset_from_start_x)
	{
		ImGui::SameLine(offset_from_start_x, u::dpi(spacing_w));
	}

	void text(const char* fmt, ...)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::Text(core::f::dinpro, u::dpi(17.0f), fmt);
		ImGui::PopStyleVar();
	}

	void hitbox_priority(const char* label, const ImVec2& size_arg, int& head_pr,
		int& body_pr, int& pelvis_pr, int& legs_pr, int& feet_pr, int& arms_pr)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImVec2 label_size = ImGui::CalcTextSize(core::f::dinpro,
			u::dpi(17.0f), label, nullptr, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(u::dpi(size_arg),
			label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return;

		std::vector<ImRect> bb_hitboxes =
		{
			{ bb.Min + u::dpi(ImVec2(40.0f, 30.0f)),
				bb.Min + u::dpi(ImVec2(80.0f, 70.0f)) }, // head
			{ bb.Min + u::dpi(ImVec2(35.0f, 70.0f)),
				bb.Min + u::dpi(ImVec2(85.0f, 105.0f)) }, // body
			{ bb.Min + u::dpi(ImVec2(35.0f, 105.0f)),
				bb.Min + u::dpi(ImVec2(85.0f, 135.0f)) }, // pelvis
			{ bb.Min + u::dpi(ImVec2(35.0f, 135.0f)),
				bb.Min + u::dpi(ImVec2(85.0f, 205.0f)) }, // legs
			{ bb.Min + u::dpi(ImVec2(30.0f, 205.0f)),
				bb.Min + u::dpi(ImVec2(90.0f, 240.0f)) }, // feet
			{ bb.Min + u::dpi(ImVec2(0.0f, 70.0f)),
				bb.Min + u::dpi(ImVec2(35.0f, 145.0f)) }, // left arm
			{ bb.Min + u::dpi(ImVec2(85.0f, 80.0f)),
				bb.Min + u::dpi(ImVec2(120.0f, 145.0f)) }, // right arm
		};

		for (size_t i = 0; i < bb_hitboxes.size(); i++)
		{

			bool hovered = false, held = false;
			ImGui::ButtonBehavior(bb_hitboxes[i], id, &hovered, &held, 0);

			if (hovered)
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					switch (i)
					{
					case 0: head_pr++; break;
					case 1: body_pr++; break;
					case 2: pelvis_pr++; break;
					case 3: legs_pr++; break;
					case 4: feet_pr++; break;
					case 5: case 6: arms_pr++; break;
					default: break;
					}
				}

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					switch (i)
					{
					case 0: head_pr--; break;
					case 1: body_pr--; break;
					case 2: pelvis_pr--; break;
					case 3: legs_pr--; break;
					case 4: feet_pr--; break;
					case 5: case 6: arms_pr--; break;
					default: break;
					}
				}
			}
		}

#define IMGUI_P ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)

		// Render
		ImGui::RenderNavHighlight(bb, id);

		auto change_color = [](int& priority, float alpha) -> ImU32
		{
			if (priority > 3 || priority < -3)
				priority = 0;

			alpha = std::clamp(alpha, 0.0f, 200.0f);

			switch (priority)
			{
			case 1:
			case -3:
				return IM_COL32(127, 210, 0,
					static_cast<int>(alpha));
			case 2:
			case -2:
				return IM_COL32(255, 170, 0,
					static_cast<int>(alpha));
			case 3:
			case -1:
				return IM_COL32(230, 98, 98,
					static_cast<int>(alpha));
			default:
				return IM_COL32(0, 0, 0, 0);
			}
		};

		for (int i = 0; i < 6; i++)
		{
			window->DrawList->AddImage(core::i::hitboxes[i], bb.Min - u::dpi(ImVec2(30.0f, -26.0f)),
				bb.Min + u::dpi(ImVec2(140.0f, i == 0 || i == 3 || i == 4 ? 252.0f : 253.0f)),
				IMGUI_P, IM_COL32(255, 255, 255, g.Style.Alpha * 255.0f));
		}

		window->DrawList->AddImage(core::i::hitboxes_ph[0], bb.Min - u::dpi(ImVec2(30.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(140.0f, 252.0f)), IMGUI_P, change_color(head_pr, g.Style.Alpha * 255.0f));

		window->DrawList->AddImage(core::i::hitboxes_ph[1], bb.Min - u::dpi(ImVec2(30.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(140.0f, 254.0f)), IMGUI_P, change_color(body_pr, g.Style.Alpha * 255.0f));

		window->DrawList->AddImage(core::i::hitboxes_ph[2], bb.Min - u::dpi(ImVec2(30.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(140.0f, 253.0f)), IMGUI_P, change_color(pelvis_pr, g.Style.Alpha * 255.0f));

		window->DrawList->AddImage(core::i::hitboxes_ph[3], bb.Min - u::dpi(ImVec2(30.0f, -25.0f)),
			bb.Min + u::dpi(ImVec2(140.0f, 252.0f)), IMGUI_P, change_color(legs_pr, g.Style.Alpha * 255.0f));

		window->DrawList->AddImage(core::i::hitboxes_ph[4], bb.Min - u::dpi(ImVec2(30.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(140.0f, 252.0f)), IMGUI_P, change_color(feet_pr, g.Style.Alpha * 255.0f));

		window->DrawList->AddImage(core::i::hitboxes_ph[5], bb.Min - u::dpi(ImVec2(30.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(140.0f, 253.0f)), IMGUI_P, change_color(arms_pr, g.Style.Alpha * 255.0f));

		window->DrawList->AddRectFilled(bb.Min + u::dpi(ImVec2(138.0f, 52.0f)),
			bb.Min + u::dpi(ImVec2(152.0f, 67.0f)), IM_COL32(230, 98, 98, g.Style.Alpha * 255.0f), 3.0f);  // W - 15, H - 15

		window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f),
			bb.Min + u::dpi(ImVec2(162.0f, 50.0f)), ImGui::GetColorU32(ImGuiCol_Text), crypt_str("High"));

		window->DrawList->AddRectFilled(bb.Min + u::dpi(ImVec2(138.0f, 77.0f)),
			bb.Min + u::dpi(ImVec2(152.0f, 91.0f)), IM_COL32(255, 170, 0, g.Style.Alpha * 255.0f), 3.0f);

		window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f),
			bb.Min + u::dpi(ImVec2(162.0f, 75.0f)), ImGui::GetColorU32(ImGuiCol_Text), crypt_str("Medium"));

		window->DrawList->AddRectFilled(bb.Min + u::dpi(ImVec2(138.0f, 102.0f)),
			bb.Min + u::dpi(ImVec2(152.0f, 116.0f)), IM_COL32(127, 210, 0, g.Style.Alpha * 255.0f), 3.0f);

		window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f),
			bb.Min + u::dpi(ImVec2(162.0f, 100.0f)), ImGui::GetColorU32(ImGuiCol_Text), crypt_str("Low"));

		window->DrawList->AddRectFilled(bb.Min + u::dpi(ImVec2(138.0f, 127.0f)),
			bb.Min + u::dpi(ImVec2(152.0f, 141.0f)), IM_COL32(196, 196, 196, g.Style.Alpha * 255.0f), 3.0f);

		window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f),
			bb.Min + u::dpi(ImVec2(162.0f, 125.0f)), ImGui::GetColorU32(ImGuiCol_Text), crypt_str("None"));
	}


	void safe_points(const char* label, const ImVec2& size_arg, int& head_pr,
		int& body_pr, int& pelvis_pr, int& legs_pr, int& feet_pr, int& arms_pr)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImVec2 label_size = ImGui::CalcTextSize(core::f::dinpro,
			u::dpi(17.0f), label, nullptr, true);

		ImVec2 pos = window->DC.CursorPos - ImVec2(0.0f, 10.0f);

		ImVec2 size = ImGui::CalcItemSize(u::dpi(size_arg),
			label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return;

		std::vector<ImRect> bb_hitboxes =
		{
			{ bb.Min + u::dpi(ImVec2(85.0f, 30.0f)),
				bb.Min + u::dpi(ImVec2(125.0f, 70.0f)) }, // head
			{ bb.Min + u::dpi(ImVec2(80.0f, 70.0f)),
				bb.Min + u::dpi(ImVec2(130.0f, 105.0f)) }, // body
			{ bb.Min + u::dpi(ImVec2(80.0f, 105.0f)),
				bb.Min + u::dpi(ImVec2(130.0f, 135.0f)) }, // pelvis
			{ bb.Min + u::dpi(ImVec2(80.0f, 135.0f)),
				bb.Min + u::dpi(ImVec2(130.0f, 205.0f)) }, // legs
			{ bb.Min + u::dpi(ImVec2(75.0f, 205.0f)),
				bb.Min + u::dpi(ImVec2(135.0f, 240.0f)) }, // feet
			{ bb.Min + u::dpi(ImVec2(45.0f, 70.0f)),
				bb.Min + u::dpi(ImVec2(80.0f, 145.0f)) }, // left arm
			{ bb.Min + u::dpi(ImVec2(130.0f, 80.0f)),
				bb.Min + u::dpi(ImVec2(165.0f, 145.0f)) }, // right arm
		};

		for (size_t i = 0; i < bb_hitboxes.size(); i++)
		{
			if (!ImGui::ItemAdd(bb_hitboxes[i], id))
				return;

			bool hovered = false, held = false;
			ImGui::ButtonBehavior(bb_hitboxes[i], id, &hovered, &held, 0);

			if (hovered)
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					switch (i)
					{
					case 0: head_pr++; break;
					case 1: body_pr++; break;
					case 2: pelvis_pr++; break;
					case 3: legs_pr++; break;
					case 4: feet_pr++; break;
					case 5: case 6: arms_pr++; break;
					default: break;
					}
				}
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					switch (i)
					{
					case 0: head_pr--; break;
					case 1: body_pr--; break;
					case 2: pelvis_pr--; break;
					case 3: legs_pr--; break;
					case 4: feet_pr--; break;
					case 5: case 6: arms_pr--; break;
					default: break;
					}
				}
			}
		}

#define IMGUI_P ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)

		// Render
		ImGui::RenderNavHighlight(bb, id);

		auto change_color = [](int& priority, float alpha) -> ImU32
		{
			if (priority > 2)
				priority = 0;

			if (priority < 0)
				priority = 2;

			alpha = std::clamp(alpha, 0.0f, 200.0f);

			switch (priority)
			{
			case 1:
				return IM_COL32(225, 165, 0, static_cast<int>(alpha));
			case 2:
				return IM_COL32(206, 77, 77, static_cast<int>(alpha));
			}

			return IM_COL32(0, 0, 0, 0);
		};

		for (int i = 0; i < 6; i++)
		{
			window->DrawList->AddImage(core::i::hitboxes[i], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
				bb.Min + u::dpi(ImVec2(185.0f, i == 0 || i == 3 || i == 4 ? 252.0f : 253.0f)),
				IMGUI_P, IM_COL32(255, 255, 255, g.Style.Alpha * 255.0f));
		}

		window->DrawList->AddImage(core::i::hitboxes_ph[0], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 252.0f)), IMGUI_P, change_color(head_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddImage(core::i::hitboxes_ph[1], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 254.0f)), IMGUI_P, change_color(body_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddImage(core::i::hitboxes_ph[2], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 253.0f)), IMGUI_P, change_color(pelvis_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddImage(core::i::hitboxes_ph[3], bb.Min - u::dpi(ImVec2(-15.0f, -25.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 252.0f)), IMGUI_P, change_color(legs_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddImage(core::i::hitboxes_ph[4], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 252.0f)), IMGUI_P, change_color(feet_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddImage(core::i::hitboxes_ph[5], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 253.0f)), IMGUI_P, change_color(arms_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddRectFilled(bb.Min + u::dpi(ImVec2(0.0f, 255.0f)),
			bb.Min + u::dpi(ImVec2(15.0f, 270.0f)), IM_COL32(196, 196, 196, g.Style.Alpha * 255.0f), 3.0f);
		window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f),
			bb.Min + u::dpi(ImVec2(23.0f, 253.0f)), ImGui::GetColorU32(ImGuiCol_Text), crypt_str("None"));
		window->DrawList->AddRectFilled(bb.Min + u::dpi(ImVec2(72.0f, 255.0f)),
			bb.Min + u::dpi(ImVec2(87.0f, 270.0f)), IM_COL32(225, 165, 0, g.Style.Alpha * 255.0f), 3.0f);
		window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f),
			bb.Min + u::dpi(ImVec2(95.0f, 253.0f)), ImGui::GetColorU32(ImGuiCol_Text), crypt_str("Prefer"));
		window->DrawList->AddRectFilled(bb.Min + u::dpi(ImVec2(149.0f, 255.0f)),
			bb.Min + u::dpi(ImVec2(164.0f, 270.0f)), IM_COL32(206, 77, 77, g.Style.Alpha * 255.0f), 3.0f);
		window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f),
			bb.Min + u::dpi(ImVec2(172.0f, 253.0f)), ImGui::GetColorU32(ImGuiCol_Text), crypt_str("Force"));
	}

	void hitboxes(const char* label, const ImVec2& size_arg, bool& head_pr,
		bool& body_pr, bool& pelvis_pr, bool& legs_pr, bool& feet_pr, bool& arms_pr)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImVec2 label_size = ImGui::CalcTextSize(core::f::dinpro,
			u::dpi(17.0f), label, nullptr, true);

		ImVec2 pos = window->DC.CursorPos - ImVec2(0.0f, 10.0f);

		ImVec2 size = ImGui::CalcItemSize(u::dpi(size_arg),
			label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return;

		std::vector<ImRect> bb_hitboxes =
		{
			{ bb.Min + u::dpi(ImVec2(85.0f, 30.0f)),
				bb.Min + u::dpi(ImVec2(125.0f, 70.0f)) }, // head
			{ bb.Min + u::dpi(ImVec2(80.0f, 70.0f)),
				bb.Min + u::dpi(ImVec2(130.0f, 105.0f)) }, // body
			{ bb.Min + u::dpi(ImVec2(80.0f, 105.0f)),
				bb.Min + u::dpi(ImVec2(130.0f, 135.0f)) }, // pelvis
			{ bb.Min + u::dpi(ImVec2(80.0f, 135.0f)),
				bb.Min + u::dpi(ImVec2(130.0f, 205.0f)) }, // legs
			{ bb.Min + u::dpi(ImVec2(75.0f, 205.0f)),
				bb.Min + u::dpi(ImVec2(135.0f, 240.0f)) }, // feet
			{ bb.Min + u::dpi(ImVec2(45.0f, 70.0f)),
				bb.Min + u::dpi(ImVec2(80.0f, 145.0f)) }, // left arm
			{ bb.Min + u::dpi(ImVec2(130.0f, 80.0f)),
				bb.Min + u::dpi(ImVec2(165.0f, 145.0f)) }, // right arm
		};

		for (size_t i = 0; i < bb_hitboxes.size(); i++)
		{
			if (!ImGui::ItemAdd(bb_hitboxes[i], id))
				return;

			bool hovered = false, held = false;
			ImGui::ButtonBehavior(bb_hitboxes[i], id, &hovered, &held, 0);

			if (hovered)
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					switch (i)
					{
					case 0: head_pr = !head_pr; break;
					case 1: body_pr = !body_pr; break;
					case 2: pelvis_pr = !pelvis_pr; break;
					case 3: legs_pr = !legs_pr; break;
					case 4: feet_pr = !feet_pr; break;
					case 5: case 6: arms_pr = !arms_pr; break;
					default: break;
					}
				}

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					switch (i)
					{
					case 0: head_pr = !head_pr; break;
					case 1: body_pr = !body_pr; break;
					case 2: pelvis_pr = !pelvis_pr; break;
					case 3: legs_pr = !legs_pr; break;
					case 4: feet_pr = !feet_pr; break;
					case 5: case 6: arms_pr = !arms_pr; break;
					default: break;
					}
				}
			}
		}

#define IMGUI_P ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)

		// Render
		ImGui::RenderNavHighlight(bb, id);

		auto change_color = [](bool& priority, float alpha) -> ImU32
		{
			alpha = std::clamp(alpha, 0.0f, 200.0f);

			if (priority)
				return IM_COL32(127, 210, 0, static_cast<int>(alpha));

			return IM_COL32(0, 0, 0, 0);
		};

		for (int i = 0; i < 6; i++)
		{
			window->DrawList->AddImage(core::i::hitboxes[i], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
				bb.Min + u::dpi(ImVec2(185.0f, i == 0 || i == 3 || i == 4 ? 252.0f : 253.0f)),
				IMGUI_P, IM_COL32(255, 255, 255, g.Style.Alpha * 255.0f));
		}
		window->DrawList->AddImage(core::i::hitboxes_ph[0], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 252.0f)), IMGUI_P, change_color(head_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddImage(core::i::hitboxes_ph[1], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 254.0f)), IMGUI_P, change_color(body_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddImage(core::i::hitboxes_ph[2], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 253.0f)), IMGUI_P, change_color(pelvis_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddImage(core::i::hitboxes_ph[3], bb.Min - u::dpi(ImVec2(-15.0f, -25.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 252.0f)), IMGUI_P, change_color(legs_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddImage(core::i::hitboxes_ph[4], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 252.0f)), IMGUI_P, change_color(feet_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddImage(core::i::hitboxes_ph[5], bb.Min - u::dpi(ImVec2(-15.0f, -26.0f)),
			bb.Min + u::dpi(ImVec2(185.0f, 253.0f)), IMGUI_P, change_color(arms_pr, g.Style.Alpha * 255.0f));
		window->DrawList->AddRectFilled(bb.Min + u::dpi(ImVec2(0.0f, 255.0f)),
			bb.Min + u::dpi(ImVec2(15.0f, 270.0f)), IM_COL32(196, 196, 196, g.Style.Alpha * 255.0f), 3.0f);
		window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f),
			bb.Min + u::dpi(ImVec2(23.0f, 253.0f)), ImGui::GetColorU32(ImGuiCol_Text), crypt_str("None"));
		window->DrawList->AddRectFilled(bb.Min + u::dpi(ImVec2(72.0f, 255.0f)),
			bb.Min + u::dpi(ImVec2(87.0f, 270.0f)), IM_COL32(127, 210, 0, g.Style.Alpha * 255.0f), 3.0f);
		window->DrawList->AddText(core::f::dinpro, u::dpi(17.0f),
			bb.Min + u::dpi(ImVec2(95.0f, 253.0f)), ImGui::GetColorU32(ImGuiCol_Text), crypt_str("On"));
	}


	bool checkbox(const char* label, bool* v)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(g.Font,
			u::dpi(g.FontSize), label, nullptr, true);

		const float square_sz = u::dpi(ImGui::GetFrameHeight() - 10.0f);
		const ImVec2 pos = window->DC.CursorPos;

		const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f),
			label_size.y + style.FramePadding.y * 2.0f));

		ImGui::ItemSize(total_bb, style.FramePadding.y);

		if (!ImGui::ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);

		if (pressed)
		{
			*v = !(*v);

			ImGui::MarkItemEdited(id);
		}

		const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));

		ImGui::RenderNavHighlight(total_bb, id);

		auto it = g.Animations.find(id);
		float value = it != g.Animations.end() ? it->second : 0.0f;

		u::fade(*v, value, 0.0f, 255.0f);

		window->DrawList->AddRect(check_bb.Min, check_bb.Max,
			ImGui::GetColorU32(ImGuiCol_FrameBgActive, g.Style.Alpha), 1.0f, ImDrawCornerFlags_All, u::dpi(2.5f));

		ImGui::RenderFrame(check_bb.Min + ImVec2(1.0f, 1.0f), check_bb.Max - ImVec2(1.0f, 1.0f),
			ImGui::GetColorU32(ImGuiCol_FrameBgActive, g.Style.Alpha), false, 0.0f);

		ImGui::RenderFrame(check_bb.Min + ImVec2(1.0f, 1.0f), check_bb.Max - ImVec2(1.0f, 1.0f),
			ImGui::GetColorU32(ImGuiCol_FrameBg, core::s::is_opened ? value / 255.0f : g.Style.Alpha), false, 0.0f);

		if (label_size.x > 0.0f)
		{
			window->DrawList->AddText(g.Font, u::dpi(g.FontSize), ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x + 7.0f,
				check_bb.Min.y + style.FramePadding.y - 6.0f), ImGui::GetColorU32(ImGuiCol_Text, g.Style.Alpha), label);
		}

		if (it != g.Animations.end())
			it->second = value;
		else if (value > 0.0f)
			g.Animations.insert(std::pair(id, value));


		if (config->scripts.developer_mode && ImGui::IsItemHovered())
		{
			for (auto& item : config->items)
			{
				if (v == item.second.pointer.get() && item.second.show)
				{
					ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
						ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
					ImGui::SetTooltip(item.second.name.c_str());
					ImGui::PopStyleVar();
					ImGui::PopStyleColor();
					break;
				}
			}
		}

		return pressed;
	}

	static std::string wns = "";

	bool add_skin_button(const ImVec2& size_arg)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID("add_a_skin");
		const ImVec2 label_size = ImGui::CalcTextSize(core::f::dinpro_bold,
			u::dpi(13.0f), "add_a_skin", nullptr, true);

		ImVec2 pos = window->DC.CursorPos, size = ImGui::CalcItemSize(u::dpi(size_arg),
			label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		// Render
		const ImU32 col = ImGui::GetColorU32(hovered ? (core::s::is_theme_dark ?
			ImGuiCol_TitleBgCollapsed : ImGuiCol_ScrollbarGrabActive) : ImGuiCol_FrameBg, g.Style.Alpha);

		ImGui::RenderNavHighlight(bb, id);

		int alpha_c = static_cast<int>(std::clamp(
			g.Style.Alpha * 255.0f, 0.0f, 100.0f));

		window->DrawList->AddShadowRect(bb.Min, bb.Max,
			IM_COL32(0, 0, 0, alpha_c), 25.0f);

		ImGui::RenderFrame(bb.Min - ImVec2(0.5f, 0.5f),
			bb.Max + ImVec2(0.5f, 0.5f), IM_COL32(112, 112, 112, alpha_c), false, style.FrameRounding);

		ImGui::RenderFrame(bb.Min, bb.Max, col, false, style.FrameRounding);

		ImVec2 center_pos = pos + size / 2.0f, offset = u::dpi(
			ImVec2(3.0f, 18.0f)), offset2 = u::dpi(ImVec2(18.0f, 3.0f));

		ImGui::RenderFrame(center_pos - offset,
			center_pos + offset, ImGui::GetColorU32(ImGuiCol_Text));

		ImGui::RenderFrame(center_pos - offset2,
			center_pos + offset2, ImGui::GetColorU32(ImGuiCol_Text));

		return hovered && held;
	}

	bool add_sticker_button(const ImVec2& size_arg,
		IDirect3DTexture9* image, int rarity, int id_sticker, bool close_button)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;

		const std::string label = u::random_string();
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label.c_str());

		const ImVec2 label_size = ImGui::CalcTextSize(core::f::dinpro_bold,
			u::dpi(13.0f), label.c_str(), nullptr, true);


		ImVec2 pos = window->DC.CursorPos, size = ImGui::CalcItemSize(u::dpi(size_arg), label_size.x +
			style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		// Render
		const ImU32 col = ImGui::GetColorU32(hovered ?
			ImGuiCol_TitleBgActive : ImGuiCol_WindowBg, g.Style.Alpha);

		ImU32 rar_col = IM_COL32(141, 159, 185,
			static_cast<int>(g.Style.Alpha * 255.0f));

		ImGui::RenderNavHighlight(bb, id);

		ImGui::RenderFrame(bb.Min, bb.Max, col, false, style.FrameRounding);

		ImVec2 center_pos = pos + size / 2.0f, offset =
			ImVec2(1.0f, 5.0f), offset2 = ImVec2(5.0f, 1.0f);


		if (image != nullptr)
		{
			switch (rarity)
			{
			case 0: rar_col = IM_COL32(141, 159, 185, static_cast<int>(g.Style.Alpha * 255.0f)); break;
			case 1: rar_col = IM_COL32(106, 148, 212, static_cast<int>(g.Style.Alpha * 255.0f)); break;
			case 2: rar_col = IM_COL32(48, 83, 222, static_cast<int>(g.Style.Alpha * 255.0f)); break;
			case 3: rar_col = IM_COL32(171, 95, 238, static_cast<int>(g.Style.Alpha * 255.0f)); break;
			case 4: rar_col = IM_COL32(238, 95, 228, static_cast<int>(g.Style.Alpha * 255.0f)); break;
			case 5: rar_col = IM_COL32(252, 130, 130, static_cast<int>(g.Style.Alpha * 255.0f)); break;
			case 6: rar_col = IM_COL32(52, 52, 52, static_cast<int>(g.Style.Alpha * 255.0f)); break;
			default: rar_col = IM_COL32_BLACK_TRANS; break;
			}

			window->DrawList->AddImage(image, bb.Min, bb.Max, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
				IM_COL32(255, 255, 255, static_cast<int>(g.Style.Alpha * 255.0f)));

			window->DrawList->AddRectFilled(bb.Min, bb.Min + ImVec2(4.0f, size.y),
				rar_col, g.Style.FrameRounding, ImDrawCornerFlags_Left);
		}
		else
		{
			ImGui::RenderFrame(center_pos - offset,
				center_pos + offset, ImGui::GetColorU32(ImGuiCol_Text));

			ImGui::RenderFrame(center_pos - offset2,
				center_pos + offset2, ImGui::GetColorU32(ImGuiCol_Text));
		}
		if (close_button)
		{
			ImVec2 size_cb = u::dpi(ImVec2(10.0f, 10.0f));

			const ImRect bb2(pos + size_cb - ImVec2(0.0f, 3.0f),
				pos + size_cb * 2.0f - ImVec2(0.0f, 3.0f));

			bool hovered2 = false, held2 = false;
			ImGui::ButtonBehavior(bb2, id, &hovered2, &held2, 0);

			if (hovered2 && held2)
			{
				if (core::i::stickers[id_sticker])
					core::i::stickers[id_sticker]->Release();
				core::i::stickers[id_sticker] = nullptr;

				core::i::sticker_id[id_sticker] = 0;
				core::i::sticker_rarity[id_sticker] = 0;
				core::i::sticker_path[id_sticker].clear();
				return false;
			}

			if (ImGui::IsMouseHoveringRect(bb.Min, bb.Max))
			{
				const ImU32 colcb = ImGui::GetColorU32(hovered2 ?
					ImGuiCol_ButtonHovered : ImGuiCol_Button);

				const ImU32 col_outline = ImGui::GetColorU32(ImGuiCol_Text);

				window->DrawList->AddText(core::f::awesome, u::dpi(6.5f),
					bb2.Min + ImVec2(1.0f, 5.0f), col_outline, (char*)(char*)u8"\uf00d");
				window->DrawList->AddText(core::f::awesome, u::dpi(6.5f),
					bb2.Min + ImVec2(-1.0f, 5.0f), col_outline, (char*)(char*)u8"\uf00d");
				window->DrawList->AddText(core::f::awesome, u::dpi(6.5f),
					bb2.Min + ImVec2(0.0f, 4.0f), col_outline, (char*)(char*)u8"\uf00d");
				window->DrawList->AddText(core::f::awesome, u::dpi(6.5f),
					bb2.Min + ImVec2(0.0f, 6.0f), col_outline, (char*)(char*)u8"\uf00d");

				window->DrawList->AddText(core::f::awesome, u::dpi(6.5f),
					bb2.Min + ImVec2(0.0f, 5.0f), colcb, (char*)(char*)u8"\uf00d");
			}
		}
		return hovered && held;
	}

	int rotation_start_index = 0;

	void ImRotateStart()
	{
		rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
	}

	ImVec2 ImRotationCenter()
	{
		ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

		const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;

		for (int i = rotation_start_index; i < buf.Size; i++)
			l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

		return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
	}

	void ImRotateEnd(float rad, ImVec2 center)
	{
		float s = sin(rad), c = cos(rad);
		center = ImRotate(center, s, c) - center;

		auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;

		for (int i = rotation_start_index; i < buf.Size; i++)
			buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
	}

	static auto lerp(float x0, float x1) {
		return [=](float t) {
			return (1 - t) * x0 + t * x1;
		};
	}

	template <int T>
	float sawtooth(float t) {
		return ImFmod(((float)T) * t, 1.0f);
	}

	static float lerp(float x0, float x1, float t) {
		return lerp(x0, x1)(t);
	}

	static auto interval(float T0, float T1, std::function<float(float)> tween = lerp(0.0, 1.0)) {
		return [=](float t) {
			return t < T0 ? 0.0f : t > T1 ? 1.0f : tween((t - T0) / (T1 - T0));
		};
	}

	std::mutex m;
	std::mutex m2;

	void get_skin(IDirect3DTexture9** texture, std::string path)
	{
		static auto old_frame = globals->framecount;

		if (old_frame == globals->framecount)
			return;

		old_frame = globals->framecount;

		if (m.try_lock())
		{
			const auto handle = basefilesys->Open(path.c_str(), crypt_str("r"), crypt_str("GAME"));

			if (handle)
			{
				int file_len = basefilesys->Size(handle);
				char* image = new char[file_len]; //-V121

				basefilesys->Read(image, file_len, handle);
				basefilesys->Close(handle);

				D3DXCreateTextureFromFileInMemory(render->device.get(), image, file_len, texture);
				delete[] image;
			}

			m.unlock();
		}
	}

	void get_skin_no_thread(IDirect3DTexture9** texture, std::string path)
	{
		const auto handle = basefilesys->Open(path.c_str(), crypt_str("r"), crypt_str("GAME"));

		if (handle)
		{
			int file_len = basefilesys->Size(handle);
			char* image = new char[file_len]; //-V121

			basefilesys->Read(image, file_len, handle);
			basefilesys->Close(handle);

			D3DXCreateTextureFromFileInMemory(render->device.get(), image, file_len, texture);
			delete[] image;
		}
	}

	std::string utf8_substr(const std::string& str, unsigned int start, unsigned int leng)
	{
		if (leng == 0) { return ""; }
		unsigned int i, ix, q, min = std::string::npos, max = std::string::npos;
		for (q = 0, i = 0, ix = str.length(); i < ix; i++, q++)
		{
			if (q == start) { min = i; }
			if (q <= start + leng || leng == std::string::npos) { max = i; }

			unsigned int c = static_cast<unsigned char>(str[i]);
			if (c >= 0 && c <= 127) i += 0;
			else if ((c & 0xE0) == 0xC0) i += 1;
			else if ((c & 0xF0) == 0xE0) i += 2;
			else if ((c & 0xF8) == 0xF0) i += 3;
			else return "";
		}
		if (q <= start + leng || leng == std::string::npos) { max = i; }
		if (min == std::string::npos || max == std::string::npos) { return ""; }
		return str.substr(min, max);
	}


	bool skin_element(const ImVec2& size_arg,
		core::s_skin_info& skin_info, bool close_button) {
		return false;
	}

	void skin_preview(
		core::s_skin_info& skin_info, int& tab, bool edit) {}

	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);

		if (idx < 0 || idx >= static_cast<int>(vector.size()))
			return false;

		*out_text = vector.at(idx).c_str();

		return true;
	};

	bool list_box(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**),
		void* data, int items_count, int height_in_items)
	{
		if (!ImGui::ListBoxHeader(label, items_count, height_in_items))
			return false;

		ImGuiContext& g = *GImGui;
		bool value_changed = false;

		ImGuiListClipper clipper;
		clipper.Begin(items_count, ImGui::GetTextLineHeightWithSpacing());

		while (clipper.Step())
		{
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				const bool item_selected = (i == *current_item);
				const char* item_text;

				if (!items_getter(data, i, &item_text))
					item_text = crypt_str("*Unknown item*");

				ImGui::PushID(i);

				if (selectable(item_text, item_selected))
				{
					*current_item = i;
					value_changed = true;
				}

				if (item_selected)
					ImGui::SetItemDefaultFocus();

				ImGui::PopID();
			}
		}

		ImGui::ListBoxFooter();

		if (value_changed)
			ImGui::MarkItemEdited(g.CurrentWindow->DC.LastItemId);

		return value_changed;
	}

	bool list_box(const char* label, int* currIndex,
		std::vector<std::string>& values, int height_in_items)
	{
		if (values.empty())
			return false;

		ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32_BLACK_TRANS);

		const bool result = list_box(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);

		ImGui::PopStyleColor();

		return result;
	}



	bool hotkey(const char* label, KeyBind& key_bind,
		const ImVec2& size_arg)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;

		const ImGuiStyle& style = g.Style;

		const ImGuiID id = window->GetID(label);

		const ImVec2 label_size = u::dpi(ImVec2(0.0f, 17.0f));

		std::string popup_label = std::string(crypt_str("##popup_")) + label;
		const ImGuiID id2 = window->GetID(popup_label.c_str());

		ImVec2 size = ImGui::CalcItemSize(u::dpi(size_arg),
			ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);

		const ImRect frame_bb(window->DC.CursorPos + ImVec2(label_size.x +
			style.ItemInnerSpacing.x, -3.0f), window->DC.CursorPos + size + ImVec2(0.0f, -3.0f));

		const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

		ImGui::ItemSize(total_bb, style.FramePadding.y);

		if (!ImGui::ItemAdd(total_bb, id))
			return false;

		const bool focus_requested = ImGui::FocusableItemRegister(window, g.ActiveId == id);
		const bool hovered = ImGui::ItemHoverable(frame_bb, id);

		if (hovered)
			ImGui::SetHoveredID(id);

		const bool user_clicked = hovered && io.MouseClicked[0];

		if (focus_requested || user_clicked && key_bind.mode != ALWAYS_ON)
		{
			if (g.ActiveId != id)
			{
				// Start edition
				memset(io.MouseDown, 0, sizeof(io.MouseDown));
				memset(io.KeysDown, 0, sizeof(io.KeysDown));

				key_bind.key = ButtonCode_t::KEY_NONE;
			}

			ImGui::SetActiveID(id, window);
			ImGui::FocusWindow(window);
		}
		else if (io.MouseClicked[0])
		{
			if (g.ActiveId == id)
				ImGui::ClearActiveID();
		}

		bool value_changed = false;
		auto key = key_bind.key;

		bool is_open = hovered && ImGui::IsMouseClicked(
			ImGuiMouseButton_Right) && g.ActiveId != id;

		if (is_open)
			ImGui::OpenPopup(popup_label.c_str());

		if (g.ActiveId == id)
		{
			for (int i = 0; i < 5; i++)
			{
				if (io.MouseDown[i])
				{
					// ���-�� ����
					// key = IInputSystem::VirtualKeyToButtonCode(VK_LBUTTON);
					// key = IInputSystem::VirtualKeyToButtonCode(VK_RBUTTON);
					// � �.�

					switch (i)
					{
					case 0:
						//key = (core::e_button_code)VK_LBUTTON; // ������ ����
						key = MOUSE_LEFT;//inputsystem->VirtualKeyToButtonCode(MOUSE_LEFT);
						break;
					case 1:
						//key = (core::e_button_code)VK_RBUTTON; // ������ ����
						key = MOUSE_RIGHT; //inputsystem->VirtualKeyToButtonCode(MOUSE_RIGHT);
						break;
					case 2:
						//key = (core::e_button_code)VK_MBUTTON; // ������ ����
						key = MOUSE_MIDDLE;// inputsystem->VirtualKeyToButtonCode(MOUSE_MIDDLE);
						break;
					case 3:
						//key = (core::e_button_code)VK_XBUTTON1; // ������ ����
						key = MOUSE_4;// inputsystem->VirtualKeyToButtonCode(MOUSE_4);
						break;
					case 4:
						//key = (core::e_button_code)VK_XBUTTON2; // ������ ����
						key = MOUSE_5;// inputsystem->VirtualKeyToButtonCode(MOUSE_5);
						break;
					default:
						break;
					}

					value_changed = true;

					ImGui::ClearActiveID();
				}
			}

			if (!value_changed)
			{
				for (int i = KEY_NONE; i <= KEY_MAX; i++)
				{

					if (inputsystem->IsButtonDown((ButtonCode_t)i) && i != KEY_ESCAPE && i != MOUSE_LEFT && i != KEY_CAPSLOCKTOGGLE && i != KEY_NUMLOCKTOGGLE && i != KEY_SCROLLLOCKTOGGLE)
					{
						key = (ButtonCode_t)i;
						value_changed = true;
						ImGui::ClearActiveID();
						//check = 0;
					}
				}
			}

			if (ImGui::IsKeyPressedMap(ImGuiKey_Escape))
			{
				key_bind.key = ButtonCode_t::KEY_NONE;
				ImGui::ClearActiveID();
			}
			else
				key_bind.key = key;
		}

		auto it = g.Animations.find(id);
		float value = it != g.Animations.end() ? it->second : 1.0f;

		u::fade(hovered, value, 1.0f, u::dpi(65.0f));

		ImGui::SetNextWindowSize(ImVec2(size.x - 4.0f, value));
		ImGui::SetNextWindowPos(frame_bb.Min + ImVec2(0.0f, size.y + 3.0f));
		ImGui::SetNextWindowBgAlpha(g.Style.Alpha);

		float max_h = u::dpi(65.0f);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, 0);
		ImGui::PushStyleColor(ImGuiCol_PlotLines, 0);

		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImGui::GetColorU32(ImGuiCol_Button));
		ImGui::PushStyleColor(ImGuiCol_WindowShadow, 0);

		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);

		auto it2 = g.Animations.find(ImGui::GetID(popup_label.c_str()));
		float alpha = it2 != g.Animations.end() ? it2->second : 0.0f;

		if (ImGui::BeginPopup(popup_label.c_str()))
		{
			ImGuiWindow* win = ImGui::GetCurrentWindow();

			if (win != nullptr)
			{
				if (max_h <= ImGui::GetWindowSize().y)
					u::fade(false, alpha, 0.0f, 255.0f);

				ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_BLACK_TRANS);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32_BLACK_TRANS);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32_BLACK_TRANS);

				if (ImGui::Button(crypt_str("##an"), u::dpi(ImVec2(size.x - 4.0f, 20.0f))))
					key_bind.mode = ALWAYS_ON;

				ImGui::SetCursorPosY(u::dpi(20.0f));

				if (ImGui::Button(crypt_str("##te"), u::dpi(ImVec2(size.x - 4.0f, 20.0f))))
					key_bind.mode = TOGGLE;

				ImGui::SetCursorPosY(u::dpi(40.0f));

				if (ImGui::Button(crypt_str("##hd"), u::dpi(ImVec2(size.x - 4.0f, 20.0f))))
					key_bind.mode = HOLD;

				ImGui::PushStyleColor(ImGuiCol_TabText, core::s::is_theme_dark ?
					ImGui::GetColorU32(ImGuiCol_Text) : ImGui::GetColorU32(ImGuiCol_TabText));

				float offset = core::s::temp_dpi_scale >= 2 ? u::dpi(4.0f) : 0.0f;

				win->DrawList->AddText(key_bind.mode == ALWAYS_ON ? core::f::dinpro_bold : core::f::dinpro, u::dpi(16.0f),
					frame_bb.Min + u::dpi(ImVec2(10.0f, 25.0f)),
					ImGui::GetColorU32(ImGuiCol_TabText, alpha / 255.0f), crypt_str("Always on"));

				win->DrawList->AddText(key_bind.mode == TOGGLE ? core::f::dinpro_bold : core::f::dinpro, u::dpi(16.0f),
					frame_bb.Min + u::dpi(ImVec2(10.0f, 45.0f)),
					ImGui::GetColorU32(ImGuiCol_TabText, alpha / 255.0f), crypt_str("Toggle"));

				win->DrawList->AddText(key_bind.mode == HOLD ? core::f::dinpro_bold : core::f::dinpro, u::dpi(16.0f),
					frame_bb.Min + u::dpi(ImVec2(10.0f, 65.0f)),
					ImGui::GetColorU32(ImGuiCol_TabText, alpha / 255.0f), crypt_str("Hold"));

				ImGui::PopStyleColor(4);
			}

			ImGui::EndPopup();
		}

		if (!is_open)
		{
			if (it != g.Animations.end())
				it->second = 1.0f;

			if (it2 != g.Animations.end())
				it2->second = 0.0f;

			if (alpha == 254.0f)
				alpha = 0.0f;
		}

		if (it != g.Animations.end())
			it->second = value;
		else if (value > 1.0f)
			g.Animations.insert(std::pair(id, value));

		if (it2 != g.Animations.end())
			it2->second = alpha - 1.0f;
		else if (alpha > 0.0f)
			g.Animations.insert(std::pair(id2, alpha - 1.0f));

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(7);

		std::string buf_display = "none";

		ImGui::RenderFrame(frame_bb.Min, frame_bb.Max,
			ImGui::GetColorU32(ImGuiCol_Button, g.Style.Alpha), false, style.FrameRounding);

		if (key_bind.key != 0 && g.ActiveId != id) {
			if (key_bind.key != KEY_CAPSLOCKTOGGLE) {
				buf_display = inputsystem->ButtonCodeToString((ButtonCode_t)key_bind.key);
				transform(buf_display.begin(), buf_display.end(), buf_display.begin(), ::tolower);
			}
			else
			{
				buf_display = crypt_str("caps");
			}
		}
		else if (g.ActiveId == id)
			buf_display = crypt_str("...");

		if (key_bind.mode == ALWAYS_ON)
			buf_display = crypt_str("Always on");

		const ImU32 clr = core::s::is_theme_dark ? ImGui::GetColorU32(ImGuiCol_Text,
			g.Style.Alpha) : ImGui::GetColorU32(ImGuiCol_TabText, g.Style.Alpha);

		const ImVec2 buf_size = ImGui::CalcTextSize(g.Font,
			u::dpi(g.FontSize), buf_display.c_str(), nullptr, true);

		window->DrawList->AddText(g.Font, u::dpi(g.FontSize), frame_bb.GetCenter() -
			ImVec2(buf_size.x / 2.0f, u::dpi(10.0f)), clr, buf_display.c_str());
		if (config->scripts.developer_mode && ImGui::IsItemHovered())
		{
			for (auto& item : config->items)
			{
				if (item.second.type != ITEM_KEY_BIND)
					continue;

				if (!item.second.pointer)
					continue;

				if (key_bind.name == ((KeyBind*)item.second.pointer.get())->name && item.second.show)
				{
					ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
						ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
					ImGui::SetTooltip((item.second.name).c_str());
					ImGui::PopStyleVar();
					ImGui::PopStyleColor();
					break;
				}
			}
		}

		return value_changed;
	}

	bool hotkey(const char* label, KeyBind& key_bind)
	{
		const char* label_end = ImGui::FindRenderedTextEnd(label);

		draw_text(ImGui::GetForegroundDrawList(), label, ImGui::GetCurrentWindow()->DC.CursorPos -
			ImGui::GetWindowPos() - u::dpi(ImVec2(0.0f, 12.0f)), core::f::dinpro, 17.0f, label_end);

		ImGui::SameLine(u::dpi(115.0f));

		const bool result = hotkey(label,
			key_bind, ImVec2(90.0f, 20.0f));

		// ��������� VirtualKeyToButtonCode(int nVirtualKey)

		return result;
	}

	void set_hotkey_visible(KeyBind& key_bind, bool visible)
	{
		if (key_bind.visible != visible)
			key_bind.visible = visible;
	}

	void key_binds(const ImVec2& size_arg,
		const std::vector<KeyBind>& keybinds)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(crypt_str("##keybinds"));

		ImVec2 label_size = ImGui::CalcTextSize(core::f::dinpro,
			u::dpi(17.0f), crypt_str("##keybinds"), nullptr, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(u::dpi(size_arg), label_size.x +
			style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		if (core::s::temp_dpi_scale >= 6 && core::s::temp_dpi_scale < 9)
			size.y += u::dpi(29.0f);
		else if (core::s::temp_dpi_scale >= 9)
			size.y += u::dpi(25.0f);
		else
			size.y += u::dpi(35.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return;

		ImGui::RenderFrame(bb.Min, bb.Min + ImVec2(0.0f, u::dpi(30.0f)), ImGui::GetColorU32(ImGuiCol_WindowBg), false);
		ImGui::RenderFrame(bb.Min + ImVec2(0.0f, u::dpi(30.0f)), bb.Min + ImVec2(FLT_MAX, FLT_MAX), ImGui::GetColorU32(ImGuiCol_Keybinds), false);



		window->DrawList->AddText(core::f::dinpro_bold, u::dpi(17.0f), bb.Min + u::dpi(ImVec2(15.0f, 6.0f)), ImGui::GetColorU32(ImGuiCol_Text), crypt_str("Key binds"));

		window->DrawList->AddImage(core::i::small_logo, bb.Min + ImVec2(size.x - u::dpi(40.0f), 0.0f), bb.Min + ImVec2(size.x - u::dpi(10.0f), u::dpi(30.0f)),
			ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::GetColorU32(ImGuiCol_SmallLogo));

		ImGui::PushFont(core::f::dinpro);

		ImGui::SetCursorPos(u::dpi(ImVec2(15.0f, 40.0f)));
		ImGui::BeginGroup();
		{
			const char* state = "";
			float spacing = 166.0f;

			for (const auto& keybind : keybinds)
			{
				switch (keybind.mode)
				{
				case ALWAYS_ON:
				{
					state = crypt_str("[always on]");

					switch (core::s::temp_dpi_scale)
					{
					case 1:
					case 5:
						spacing = 148.0f;
						break;
					case 3:
					case 4:
					case 7:
						spacing = 149.5f;
						break;
					case 6:
						spacing = 150.0f;
						break;
					case 8:
						spacing = 148.5f;
						break;
					default:
						spacing = 151.0f;
						break;
					}

					break;
				}

				case TOGGLE:
				{
					state = crypt_str("[toggled]");

					switch (core::s::temp_dpi_scale)
					{
					case 3:
					case 4:
					case 8:
					case 9:
						spacing = 164.0f;
						break;
					case 6:
					case 7:
					case 10:
						spacing = 165.0f;
						break;
					default:
						spacing = 163.0f;
						break;
					}

					break;
				}

				case HOLD:
				{
					state = crypt_str("[hold]");
					spacing = 183.0f;

					break;
				}
				default:
					break;
				}

				ImGui::Text(core::f::dinpro, u::dpi(17.0f), "%s", keybind.name.c_str());

				ImGui::PushStyleColor(ImGuiCol_Text, core::s::is_theme_dark ?
					IM_COL32A(187, 187, 187) : IM_COL32A(114, 114, 114));

				ImGui::SameLine(u::dpi(spacing));
				ImGui::Text(core::f::dinpro, u::dpi(17.0f), "%s", state);

				ImGui::PopStyleColor();
			}
		}

		ImGui::EndGroup();

		ImGui::PopFont();
	}

	bool input_text(const char* label, char* buf, size_t buf_size,
		const ImVec2& size, const ImVec2& pos)
	{
		ImGui::PushFont(core::f::dinpro);
		ImGui::PushStyleColor(ImGuiCol_FrameBg,
			ImGui::GetColorU32(ImGuiCol_ButtonEx));

		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,
			ImGui::GetColorU32(ImGuiCol_ButtonHoveredEx));

		ImGui::PushStyleColor(ImGuiCol_FrameBgActive,
			ImGui::GetColorU32(ImGuiCol_ButtonActiveEx));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
			u::dpi(ImVec2(5.0f, 5.0f)));

		const char* label_end = ImGui::FindRenderedTextEnd(label);

		draw_text(ImGui::GetCurrentWindow()->DrawList, label,
			u::dpi(ImVec2(13.0f, 5.0f)), core::f::dinpro, 17.0f, label_end);

		ImGui::Dummy(ImVec2(0.0f, u::dpi(5.0f)));

		std::string blank_label = std::string(crypt_str("##")) + label;

		if (pos.x != 0.0f || pos.y != 0.0f)
			ImGui::SetCursorPos(u::dpi(pos));

		ImGuiContext& g = *ImGui::GetCurrentContext();
		auto backup = g.FontSize;

		g.FontSize = u::dpi(g.FontSize);
		const bool result = ImGui::InputText(blank_label.c_str(), buf, buf_size, 0,
			nullptr, nullptr, size);
		g.FontSize = backup;

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		return result;
	}

	void change_theme()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		static ImVec4 clr_out1{}, clr_out2{}, clr_out3{}, clr_out4{}, clr_out5{}, clr_out6{},
			clr_out7{}, clr_out8{}, clr_out9{}, clr_out10{}, clr_out11{}, clr_out12{}, clr_out13{},
			clr_out14{}, clr_out15{}, clr_out16{}, clr_out17{}, clr_out18{}, clr_out19{}, clr_out20{},
			clr_out21{}, clr_out22{}, clr_out23{}, clr_out24{}, clr_out25{}, clr_out26{}, clr_out27{},
			clr_out28{}, clr_out29{}, clr_out30{}, clr_out31{}, clr_out32{}, clr_out33{}, clr_out34{},
			clr_out35{}, clr_out36{};

		if (core::s::is_theme_dark)
		{
			colors[ImGuiCol_Text] = u::fade_color(IM_COL32A(242, 242, 242), IM_COL32A(52, 52, 52), clr_out1);
			colors[ImGuiCol_TextDisabled] = u::fade_color(IM_COL32(242, 242, 242, 150), IM_COL32(52, 52, 52, 150), clr_out1);
			colors[ImGuiCol_WindowBg] = u::fade_color(IM_COL32A(26, 26, 26), IM_COL32A(230, 230, 230), clr_out2);
			colors[ImGuiCol_ChildBg] = u::fade_color(IM_COL32A(36, 36, 36), IM_COL32A(238, 238, 238), clr_out3);
			colors[ImGuiCol_FrameBg] = u::fade_color(IM_COL32A(36, 36, 36), IM_COL32A(245, 245, 245), clr_out4);
			colors[ImGuiCol_PopupBg] = u::fade_color(IM_COL32A(33, 33, 33), IM_COL32A(235, 235, 235), clr_out5);
			colors[ImGuiCol_TitleBg] = u::fade_color(IM_COL32A(26, 26, 26), IM_COL32A(230, 230, 230), clr_out6);
			colors[ImGuiCol_TitleBgActive] = u::fade_color(IM_COL32A(30, 30, 30), IM_COL32A(234, 234, 234), clr_out7);
			colors[ImGuiCol_TitleBgCollapsed] = u::fade_color(IM_COL32A(34, 34, 34), IM_COL32A(238, 238, 238), clr_out8);
			colors[ImGuiCol_MenuBarBg] = u::fade_color(IM_COL32A(36, 36, 36), IM_COL32A(240, 240, 240), clr_out9);
			colors[ImGuiCol_ScrollbarBg] = u::fade_color(IM_COL32A(27, 27, 27), IM_COL32A(236, 236, 236), clr_out10);
			colors[ImGuiCol_ScrollbarGrab] = u::fade_color(IM_COL32A(41, 41, 41), IM_COL32A(255, 255, 255), clr_out11);
			colors[ImGuiCol_ScrollbarGrabHovered] = u::fade_color(IM_COL32A(43, 43, 43), IM_COL32A(251, 251, 251), clr_out12);
			colors[ImGuiCol_ScrollbarGrabActive] = u::fade_color(IM_COL32A(45, 45, 45), IM_COL32A(250, 250, 250), clr_out13);
			colors[ImGuiCol_SliderGrab] = u::fade_color(IM_COL32A(123, 123, 123), IM_COL32A(203, 203, 203), clr_out14);
			colors[ImGuiCol_SliderGrabActive] = u::fade_color(IM_COL32A(242, 242, 242), IM_COL32A(120, 120, 120), clr_out15);
			colors[ImGuiCol_Button] = u::fade_color(IM_COL32A(123, 123, 123), IM_COL32A(203, 203, 203), clr_out16);
			colors[ImGuiCol_ButtonHovered] = u::fade_color(IM_COL32A(128, 128, 128), IM_COL32A(198, 198, 198), clr_out17);
			colors[ImGuiCol_ButtonActive] = u::fade_color(IM_COL32A(133, 133, 133), IM_COL32A(120, 120, 120), clr_out18);
			colors[ImGuiCol_Header] = u::fade_color(IM_COL32A(26, 26, 26), IM_COL32A(203, 203, 203), clr_out19);
			colors[ImGuiCol_HeaderHovered] = u::fade_color(IM_COL32A(29, 29, 29), IM_COL32A(208, 208, 208), clr_out20);
			colors[ImGuiCol_HeaderActive] = u::fade_color(IM_COL32A(31, 31, 31), IM_COL32A(213, 213, 213), clr_out21);
			colors[ImGuiCol_Separator] = u::fade_color(IM_COL32_BLACK, IM_COL32_WHITE, clr_out22);
			colors[ImGuiCol_SeparatorHovered] = u::fade_color(IM_COL32_BLACK, IM_COL32_WHITE, clr_out22);
			colors[ImGuiCol_SeparatorActive] = u::fade_color(IM_COL32_BLACK, IM_COL32_WHITE, clr_out22);
			colors[ImGuiCol_ResizeGrip] = u::fade_color(IM_COL32_BLACK, IM_COL32_WHITE, clr_out22);
			colors[ImGuiCol_ResizeGripHovered] = u::fade_color(IM_COL32_BLACK, IM_COL32_WHITE, clr_out22);
			colors[ImGuiCol_ResizeGripActive] = u::fade_color(IM_COL32_BLACK, IM_COL32_WHITE, clr_out22);
			colors[ImGuiCol_ComboArrow] = u::fade_color(IM_COL32A(242, 242, 242), IM_COL32A(112, 112, 112), clr_out23);
			colors[ImGuiCol_Triangles] = u::fade_color(IM_COL32A(27, 27, 27), IM_COL32A(249, 249, 249), clr_out24, 5.0f);
			colors[ImGuiCol_Logo] = u::fade_color(IM_COL32A(27, 27, 27), IM_COL32_WHITE, clr_out25, 5.0f);
			colors[ImGuiCol_TextSelectedBg] = u::fade_color(IM_COL32(27, 27, 27, 200), IM_COL32(235, 235, 235, 200), clr_out26);
			colors[ImGuiCol_SubChild] = u::fade_color(IM_COL32(32, 32, 32, 255), IM_COL32(222, 222, 222, 255), clr_out27);
			colors[ImGuiCol_SmallLogo] = u::fade_color(IM_COL32_WHITE, IM_COL32A(27, 27, 27), clr_out28);
			colors[ImGuiCol_Keybinds] = u::fade_color(IM_COL32A(36, 36, 36), IM_COL32A(248, 248, 248), clr_out29);
			colors[ImGuiCol_ButtonEx] = u::fade_color(IM_COL32A(123, 123, 123), IM_COL32A(222, 222, 222), clr_out30);
			colors[ImGuiCol_ButtonHoveredEx] = u::fade_color(IM_COL32A(125, 125, 125), IM_COL32A(224, 224, 224), clr_out31);
			colors[ImGuiCol_ButtonActiveEx] = u::fade_color(IM_COL32A(127, 127, 127), IM_COL32A(226, 226, 226), clr_out32);
			colors[ImGuiCol_LuaEditorBg] = u::fade_color(0xff101010, 0xffffffff, clr_out33);
			colors[ImGuiCol_ButtonDisabled] = u::fade_color(IM_COL32A(83, 83, 83), IM_COL32A(230, 230, 230), clr_out34);
			colors[ImGuiCol_ButtonDisabledText] = u::fade_color(IM_COL32A(156, 156, 156), IM_COL32A(149, 149, 149), clr_out35);
			colors[ImGuiCol_KeybindsBody] = u::fade_color(IM_COL32A(118, 118, 118, 100), IM_COL32A(118, 118, 118, 100), clr_out36);
		}
		else
		{
			colors[ImGuiCol_Text] = u::fade_color(IM_COL32A(52, 52, 52), IM_COL32A(242, 242, 242), clr_out1);
			colors[ImGuiCol_TextDisabled] = u::fade_color(IM_COL32(52, 52, 52, 150), IM_COL32(242, 242, 242, 150), clr_out1);
			colors[ImGuiCol_WindowBg] = u::fade_color(IM_COL32A(230, 230, 230), IM_COL32A(26, 26, 26), clr_out2);
			colors[ImGuiCol_ChildBg] = u::fade_color(IM_COL32A(238, 238, 238), IM_COL32A(36, 36, 36), clr_out3);
			colors[ImGuiCol_FrameBg] = u::fade_color(IM_COL32A(245, 245, 245), IM_COL32A(36, 36, 36), clr_out4);
			colors[ImGuiCol_PopupBg] = u::fade_color(IM_COL32A(235, 235, 235), IM_COL32A(33, 33, 33), clr_out5);
			colors[ImGuiCol_TitleBg] = u::fade_color(IM_COL32A(230, 230, 230), IM_COL32A(26, 26, 26), clr_out6);
			colors[ImGuiCol_TitleBgActive] = u::fade_color(IM_COL32A(234, 234, 234), IM_COL32A(30, 30, 30), clr_out7);
			colors[ImGuiCol_TitleBgCollapsed] = u::fade_color(IM_COL32A(238, 238, 238), IM_COL32A(34, 34, 34), clr_out8);
			colors[ImGuiCol_MenuBarBg] = u::fade_color(IM_COL32A(240, 240, 240), IM_COL32A(36, 36, 36), clr_out9);
			colors[ImGuiCol_ScrollbarBg] = u::fade_color(IM_COL32A(236, 236, 236), IM_COL32A(27, 27, 27), clr_out10);
			colors[ImGuiCol_ScrollbarGrab] = u::fade_color(IM_COL32A(255, 255, 255), IM_COL32A(41, 41, 41), clr_out11);
			colors[ImGuiCol_ScrollbarGrabHovered] = u::fade_color(IM_COL32A(251, 251, 251), IM_COL32A(43, 43, 43), clr_out12);
			colors[ImGuiCol_ScrollbarGrabActive] = u::fade_color(IM_COL32A(250, 250, 250), IM_COL32A(45, 45, 45), clr_out13);
			colors[ImGuiCol_SliderGrab] = u::fade_color(IM_COL32A(203, 203, 203), IM_COL32A(123, 123, 123), clr_out14);
			colors[ImGuiCol_SliderGrabActive] = u::fade_color(IM_COL32A(120, 120, 120), IM_COL32A(242, 242, 242), clr_out15);
			colors[ImGuiCol_Button] = u::fade_color(IM_COL32A(203, 203, 203), IM_COL32A(123, 123, 123), clr_out16);
			colors[ImGuiCol_ButtonHovered] = u::fade_color(IM_COL32A(198, 198, 198), IM_COL32A(128, 128, 128), clr_out17);
			colors[ImGuiCol_ButtonActive] = u::fade_color(IM_COL32A(120, 120, 120), IM_COL32A(133, 133, 133), clr_out18);
			colors[ImGuiCol_Header] = u::fade_color(IM_COL32A(203, 203, 203), IM_COL32A(26, 26, 26), clr_out19);
			colors[ImGuiCol_HeaderHovered] = u::fade_color(IM_COL32A(208, 208, 208), IM_COL32A(29, 29, 29), clr_out20);
			colors[ImGuiCol_HeaderActive] = u::fade_color(IM_COL32A(213, 213, 213), IM_COL32A(31, 31, 31), clr_out21);
			colors[ImGuiCol_Separator] = u::fade_color(IM_COL32_WHITE, IM_COL32_BLACK, clr_out22);
			colors[ImGuiCol_SeparatorHovered] = u::fade_color(IM_COL32_WHITE, IM_COL32_BLACK, clr_out22);
			colors[ImGuiCol_SeparatorActive] = u::fade_color(IM_COL32_WHITE, IM_COL32_BLACK, clr_out22);
			colors[ImGuiCol_ResizeGrip] = u::fade_color(IM_COL32_WHITE, IM_COL32_BLACK, clr_out22);
			colors[ImGuiCol_ResizeGripHovered] = u::fade_color(IM_COL32_WHITE, IM_COL32_BLACK, clr_out22);
			colors[ImGuiCol_ResizeGripActive] = u::fade_color(IM_COL32_WHITE, IM_COL32_BLACK, clr_out22);
			colors[ImGuiCol_ComboArrow] = u::fade_color(IM_COL32A(112, 112, 112), IM_COL32A(242, 242, 242), clr_out23);
			colors[ImGuiCol_Triangles] = u::fade_color(IM_COL32A(255, 255, 255), IM_COL32A(27, 27, 27), clr_out24, 5.0f);
			colors[ImGuiCol_Logo] = u::fade_color(IM_COL32_WHITE, IM_COL32A(27, 27, 27), clr_out25, 5.0f);
			colors[ImGuiCol_TextSelectedBg] = u::fade_color(IM_COL32(235, 235, 235, 200), IM_COL32(27, 27, 27, 200), clr_out26);
			colors[ImGuiCol_SubChild] = u::fade_color(IM_COL32(222, 222, 222, 255), IM_COL32(32, 32, 32, 255), clr_out27);
			colors[ImGuiCol_SmallLogo] = u::fade_color(IM_COL32A(27, 27, 27), IM_COL32_WHITE, clr_out28);
			colors[ImGuiCol_Keybinds] = u::fade_color(IM_COL32A(248, 248, 248), IM_COL32A(36, 36, 36), clr_out29);
			colors[ImGuiCol_ButtonEx] = u::fade_color(IM_COL32A(222, 222, 222), IM_COL32A(123, 123, 123), clr_out30);
			colors[ImGuiCol_ButtonHoveredEx] = u::fade_color(IM_COL32A(224, 224, 224), IM_COL32A(125, 125, 125), clr_out31);
			colors[ImGuiCol_ButtonActiveEx] = u::fade_color(IM_COL32A(226, 226, 226), IM_COL32A(127, 127, 127), clr_out32);
			colors[ImGuiCol_LuaEditorBg] = u::fade_color(0xffffffff, 0xff101010, clr_out33);
			colors[ImGuiCol_ButtonDisabled] = u::fade_color(IM_COL32A(230, 230, 230), IM_COL32A(83, 83, 83), clr_out34);
			colors[ImGuiCol_ButtonDisabledText] = u::fade_color(IM_COL32A(149, 149, 149), IM_COL32A(156, 156, 156), clr_out35);
			colors[ImGuiCol_KeybindsBody] = u::fade_color(IM_COL32A(118, 118, 118, 100), IM_COL32A(118, 118, 118, 100), clr_out36);
		}

		style->AntiAliasedFill = true;
		style->AntiAliasedLines = true;
		style->AntiAliasedLinesUseTex = true;

		style->ChildRounding = 7.0f;
		style->WindowRounding = 7.0f;
		style->WindowPadding = ImVec2(0.0f, 0.0f);
		style->WindowBorderSize = 0.0f;
		style->WindowMinSize = ImVec2(20.0f, 20.0f);

		style->ScrollbarRounding = 6.0f;
		style->ScrollbarSize = 5.0f;
	}
}