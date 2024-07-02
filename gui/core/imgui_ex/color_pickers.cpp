// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "color_pickers.h"
#include "../../../config/config.h"
#include <string> 
ImVec4 g_popup_col4 = { 1.00f, 1.00f, 1.00f, 1.00f };

namespace imgui_ex
{
	void draw_text(ImDrawList* draw_list, const char* text, const ImVec2& pos,
		ImFont* font = core::f::dinpro_bold, float font_size = 16.0f, const char* text_end = nullptr);

	bool color_button(const char* desc_id, const ImVec4& col,
		ImGuiColorEditFlags flags, ImVec2 size)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiID id = window->GetID(desc_id);

		float default_size = u::dpi(ImGui::GetFrameHeight() - 6.0f);

		if (size.x == 0.0f)
			size.x = default_size;

		if (size.y == 0.0f)
			size.y = default_size;

		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
		ImGui::ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);

		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		if (flags & ImGuiColorEditFlags_NoAlpha)
			flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

		ImVec4 col_rgb = col;

		if (flags & ImGuiColorEditFlags_InputHSV)
			ImGui::ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

		ImVec4 col_rgb_without_alpha(col_rgb.x, col_rgb.y, col_rgb.z, 1.0f);
		float grid_step = ImMin(size.x, size.y) / 2.99f;
		float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);
		ImRect bb_inner = bb;
		float off = 0.0f;
		if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
		{
			off = -0.75f; // The border (using Col_FrameBg) tends to look off when color is near-opaque and rounding is enabled. This offset seemed like a good middle ground to reduce those artifacts.
			bb_inner.Expand(off);
		}
		if ((flags & ImGuiColorEditFlags_AlphaPreviewHalf) && col_rgb.w < 1.0f)
		{
			float mid_x = IM_ROUND((bb_inner.Min.x + bb_inner.Max.x) * 0.5f);
			ImGui::RenderColorRectWithAlphaCheckerboard(window->DrawList, ImVec2(bb_inner.Min.x + grid_step, bb_inner.Min.y), bb_inner.Max, ImGui::GetColorU32(col_rgb), grid_step, ImVec2(-grid_step + off, off), rounding, ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight);
			window->DrawList->AddRectFilled(bb_inner.Min, ImVec2(mid_x, bb_inner.Max.y), ImGui::GetColorU32(col_rgb_without_alpha), rounding, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft);
		}
		else
		{
			// Because GetColorU32() multiplies by the global style Alpha and we don't want to display a checkerboard if the source code had no alpha
			ImVec4 col_source = (flags & ImGuiColorEditFlags_AlphaPreview) ? col_rgb : col_rgb_without_alpha;

			if (col_source.w < 1.0f)
				ImGui::RenderColorRectWithAlphaCheckerboard(window->DrawList, bb_inner.Min, bb_inner.Max, ImGui::GetColorU32(col_source), grid_step, ImVec2(off, off), rounding);
			else
				window->DrawList->AddRectFilled(bb_inner.Min, bb_inner.Max, ImGui::GetColorU32(col_source), rounding, ImDrawCornerFlags_All);
		}

		ImGui::RenderNavHighlight(bb, id);

		window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_Border),
			rounding, ImDrawCornerFlags_All, 0.5f);

		// Drag and Drop Source
		// NB: The ActiveId test is merely an optional micro-optimization, BeginDragDropSource() does the same test.
		if (!(flags & ImGuiColorEditFlags_NoDragDrop) && ImGui::BeginDragDropSource(
			ImGuiDragDropFlags_SourceAllowNullID | ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
		{
			if (flags & ImGuiColorEditFlags_NoAlpha)
				ImGui::SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F, &col_rgb, sizeof(float) * 3, ImGuiCond_Once);
			else
				ImGui::SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &col_rgb, sizeof(float) * 4, ImGuiCond_Once);

			ImGui::ColorButton(desc_id, col, flags);
			ImGui::SameLine();
			ImGui::SetCursorPosY(36.5f);
			ImGui::TextEx("Color");

			g_popup_col4 = col;

			ImGui::EndDragDropSource();
		}

		// Tooltip
		if (!(flags & ImGuiColorEditFlags_NoTooltip) && hovered)
			ImGui::ColorTooltip(desc_id, &col.x, flags & (ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf));

		return pressed;
	}

	bool color_edit4(const char* label, float col[4], ImGuiColorEditFlags flags,
		bool offset_y = false, bool is_in_selecteable = false, int id_clr = -1)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;

		const float square_sz = ImGui::GetFrameHeight();
		const float w_full = ImGui::CalcItemWidth();
		const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
		const float w_inputs = w_full - w_button;

		const char* label_display_end = ImGui::FindRenderedTextEnd(label);

		const std::string& label_x = std::string("##x_") + label,
			label_y = std::string("##y_") + label, label_a = std::string("##a_") + label;

		const ImGuiID& id = ImGui::GetID(label),
			id1 = ImGui::GetID(label_x.c_str()),
			id2 = ImGui::GetID(label_y.c_str()),
			id3 = ImGui::GetID(label_a.c_str());

		g.NextItemData.ClearFlags();

		ImGui::BeginGroup();
		ImGui::PushID(id);

		// If we're not showing any slider there's no point in doing any HSV conversions
		const ImGuiColorEditFlags flags_untouched = flags;

		if (flags & ImGuiColorEditFlags_NoInputs)
			flags = (flags & (~ImGuiColorEditFlags__DisplayMask)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

		// Context menu: display and modify options (before defaults are applied)
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			ImGui::ColorEditOptionsPopup(col, flags);

		// Read stored options
		if (!(flags & ImGuiColorEditFlags__DisplayMask))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags__DisplayMask);

		if (!(flags & ImGuiColorEditFlags__DataTypeMask))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags__DataTypeMask);

		if (!(flags & ImGuiColorEditFlags__PickerMask))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags__PickerMask);

		if (!(flags & ImGuiColorEditFlags__InputMask))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags__InputMask);

		flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags__DisplayMask | ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask | ImGuiColorEditFlags__InputMask));

		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__DisplayMask)); // Check that only 1 is selected
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__InputMask));   // Check that only 1 is selected

		const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
		const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
		const int components = alpha ? 4 : 3;

		// Convert to the formats we need
		float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };

		if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
			ImGui::ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
		else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
		{
			// Hue is lost when converting from greyscale rgb (saturation=0). Restore it.
			ImGui::ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

			if (memcmp(g.ColorEditLastColor, col, sizeof(float) * 3) == 0)
			{
				if (f[1] == 0)
					f[0] = g.ColorEditLastHue;
				if (f[2] == 0)
					f[1] = g.ColorEditLastSat;
			}
		}

		int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

		bool value_changed = false;
		bool value_changed_as_float = false;

		const ImVec2 pos = window->DC.CursorPos;
		const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;

		window->DC.CursorPos.x = pos.x + inputs_offset_x;
		
		auto it = g.Animations.find(id1), it2 = g.Animations.find(id2),
			it3 = g.Animations.find(id3);

		float size_x = it != g.Animations.end() ? it->second : 1.0f,
			size_y = it2 != g.Animations.end() ? it2->second : 1.0f,
			alpha_m = it3 != g.Animations.end() ? it3->second : 0.0f;

		ImVec2 size = u::dpi(ImVec2(300.0f,
			is_in_selecteable ? 310.0f : 285.0f));
		
		u::fade(false, size_x, 1.0f, size.x);
		u::fade(false, size_y, 1.0f, size.y);
		
		bool is_full_size = size_x >= size.x && size_y >= size.y;

		if (is_full_size)
			u::fade(false, alpha_m, 0.0f, 255.0f, 15.0f);
		
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, u::dpi(ImVec2(0.0f, 10.0f)));

		if ((flags & ImGuiColorEditFlags_DisplayHex) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
		{
			// RGB Hexadecimal Input
			char buf[64];

			if (alpha)
				ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
			else
				ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));

			ImGui::SetNextItemWidth(u::dpi(core::s::temp_dpi_scale > 3 ? 202.0f : 200.0f));
			ImGui::PushFont(core::f::dinpro);

			if (ImGui::InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase))
			{
				value_changed = true;
				char* p = buf;

				while (*p == '#' || ImCharIsBlankA(*p))
					p++;

				i[0] = i[1] = i[2] = i[3] = 0;

				if (alpha)
					sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
				else
					sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
			}
			ImGui::PopFont();
			ImGui::SameLine();

			if (ImGui::InvisibleButton(label, u::dpi(ImVec2(25.0f, 23.0f))))
				core::s::is_taking_pixel = true;

			if (core::s::is_taking_pixel)
			{
				HDC hdc = GetDC(nullptr);

				if (hdc)
				{
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						POINT cursor{};

						if (GetCursorPos(&cursor) != 0)
						{
							int pixel = GetPixel(hdc, cursor.x, cursor.y);

							int r_clr = GetRValue(pixel), g_clr = GetGValue(pixel),
								b_clr = GetBValue(pixel);

							col[0] = static_cast<float>(r_clr) / 255.0f;
							col[1] = static_cast<float>(g_clr) / 255.0f;
							col[2] = static_cast<float>(b_clr) / 255.0f;
							col[3] = 1.0f;

							core::s::is_taking_pixel = false;
						}
					}
					
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
						core::s::is_taking_pixel = false;
					
					if (!core::s::is_opened)
						core::s::is_taking_pixel = false;
					
					ReleaseDC(nullptr, hdc);
				}

				if (!(flags & ImGuiColorEditFlags_NoOptions))
					ImGui::OpenPopupOnItemClick("context");
			}
		}

		ImGuiWindow* picker_active_window = NULL;

		if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
		{
			const float button_offset_x = window->Size.x - u::dpi(37.0f);
			const float offset = offset_y ? 3.0f : 0.0f;
			if (!offset_y || is_in_selecteable)
				window->DC.CursorPos = ImVec2(window->Pos.x + button_offset_x, pos.y - 2.0f + offset);

			const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);

			if (color_button("##ColorButton", col_v4, flags))
			{
				if (!(flags & ImGuiColorEditFlags_NoPicker))
				{
					// Store current color and open a picker
					g.ColorPickerRef = col_v4;
					ImGui::OpenPopup("picker");
					ImGui::SetNextWindowPos(window->DC.LastItemRect.GetTR() + ImVec2(15.0f, 0.0f));
				}
			}

			if (!(flags & ImGuiColorEditFlags_NoOptions))
				ImGui::OpenPopupOnItemClick("context");

			ImGui::SetNextWindowSize(ImVec2(size_x, size_y));

			if (ImGui::BeginPopup("picker", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
			{
				picker_active_window = g.CurrentWindow;

				if (label != label_display_end)
				{
					draw_text(ImGui::GetCurrentWindow()->DrawList, label, u::dpi(ImVec2(35.0f,
						20.0f)), g.Font, core::s::temp_dpi_scale < 2 ? g.FontSize + 1 : g.FontSize);

					ImGui::Dummy(ImVec2(0.0f, u::dpi(20.0f)));
				}

				ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask | ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;

				ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags__DisplayMask | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;

				ImGui::SetCursorPosX(u::dpi(35.0f));
				ImGui::SetNextItemWidth(square_sz* u::dpi(10.0f)); // Use 256 + bar sizes?
				value_changed |= color_picker4("##picker", col, picker_flags, 255.0f);

				ImGui::EndPopup();
			}
		}

		ImGui::PopStyleVar();

		if (ImGui::IsItemActive())
		{
			if (it != g.Animations.end())
				it->second = 1.0f;

			if (size_x >= size.x)
				size_x = 1.0f;

			if (it2 != g.Animations.end())
				it2->second = 1.0f;

			if (size_y >= size.y)
				size_y = 1.0f;

			if (it3 != g.Animations.end())
				it3->second = 0.0f;

			if (alpha_m >= 255.0f)
				alpha_m = 0.0f;
		}
		
		if (it != g.Animations.end())
			it->second = size_x;
		else if (size_x > 1.0f)
			g.Animations.insert(std::pair(id1, size_x));

		if (it2 != g.Animations.end())
			it2->second = size_y;
		else if (size_y > 1.0f)
			g.Animations.insert(std::pair(id2, size_y));

		if (it3 != g.Animations.end())
			it3->second = alpha_m;
		else if (alpha_m > 0.0f)
			g.Animations.insert(std::pair(id3, alpha_m));

		if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
		{
			const float text_offset_x = (flags & ImGuiColorEditFlags_NoInputs) ? w_button : w_full + style.ItemInnerSpacing.x;
			window->DC.CursorPos = ImVec2(pos.x + text_offset_x, pos.y + style.FramePadding.y);

			ImGui::TextEx(label, label_display_end);
		}

		// Convert back
		if (value_changed && picker_active_window == NULL)
		{
			if (!value_changed_as_float)
				for (int n = 0; n < 4; n++)
					f[n] = i[n] / 255.0f;

			if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
			{
				g.ColorEditLastHue = f[0];
				g.ColorEditLastSat = f[1];
				ImGui::ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
				memcpy(g.ColorEditLastColor, f, sizeof(float) * 3);
			}

			if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
				ImGui::ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

			col[0] = f[0];
			col[1] = f[1];
			col[2] = f[2];

			if (alpha)
				col[3] = f[3];
		}

		if (config->scripts.developer_mode && ImGui::IsItemHovered())
		{
			for (auto& item : config->items)
			{
				if (id_clr == -1) {
					if (col == item.second.pointer.get() && item.second.show)
					{
						ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
						ImGui::SetTooltip(item.second.name.c_str());
						ImGui::PopStyleVar();
						break;
					}
				}
				else
				{

					DWORD ptr = (DWORD)item.second.pointer.get();
					if ((DWORD)col == (ptr + sizeof(float[4]) * id_clr) &&item.second.show)
					{
						ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
						ImGui::SetTooltip((item.second.name + "[" + std::to_string(id_clr) + "]").c_str());
						ImGui::PopStyleVar();
						break;
					}

				}
			}
		}

		ImGui::PopID();
		ImGui::EndGroup();

		// Drag and Drop Target
		// NB: The flag test is merely an optional micro-optimization, BeginDragDropTarget() does the same test.
		if ((window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(flags & ImGuiColorEditFlags_NoDragDrop) && ImGui::BeginDragDropTarget())
		{
			bool accepted_drag_drop = false;

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
			{
				memcpy((float*)col, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512
				value_changed = accepted_drag_drop = true;
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
			{
				memcpy((float*)col, payload->Data, sizeof(float) * components);
				value_changed = accepted_drag_drop = true;
			}

			// Drag-drop payloads are always RGB
			if (accepted_drag_drop && (flags & ImGuiColorEditFlags_InputHSV))
				ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);

			ImGui::EndDragDropTarget();
		}

		// When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
		if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
			window->DC.LastItemId = g.ActiveId;

		if (value_changed)
			ImGui::MarkItemEdited(window->DC.LastItemId);

		return value_changed;
	}

	bool color_edit3(const char* label, float col[3],
		ImGuiColorEditFlags flags)
	{
		const bool result = color_edit4(label, col, flags | ImGuiColorEditFlags_NoAlpha);

		return result;
	}

	bool color_picker4(const char* label, float col[4],
		ImGuiColorEditFlags flags, float alpha)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImDrawList* draw_list = window->DrawList;
		ImGuiStyle& style = g.Style;
		ImGuiIO& io = g.IO;

		const float width = ImGui::CalcItemWidth();
		g.NextItemData.ClearFlags();

		ImGui::PushID(label);
		ImGui::BeginGroup();

		if (!(flags & ImGuiColorEditFlags_NoSidePreview))
			flags |= ImGuiColorEditFlags_NoSmallPreview;

		// Context menu: display and store options.
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			ImGui::ColorPickerOptionsPopup(col, flags);

		// Read stored options
		if (!(flags & ImGuiColorEditFlags__PickerMask))
			flags |= ((g.ColorEditOptions & ImGuiColorEditFlags__PickerMask) ? g.ColorEditOptions : ImGuiColorEditFlags__OptionsDefault) & ImGuiColorEditFlags__PickerMask;
		if (!(flags & ImGuiColorEditFlags__InputMask))
			flags |= ((g.ColorEditOptions & ImGuiColorEditFlags__InputMask) ? g.ColorEditOptions : ImGuiColorEditFlags__OptionsDefault) & ImGuiColorEditFlags__InputMask;
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__PickerMask)); // Check that only 1 is selected
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags__InputMask));  // Check that only 1 is selected
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

		// Setup
		int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
		bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
		ImVec2 picker_pos = window->DC.CursorPos;
		float square_sz = ImGui::GetFrameHeight();
		float bars_width = u::dpi(square_sz - 5.0f);

		float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x)); // Saturation/Value picking box
		float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
		float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
		float bars_triangles_half_sz = IM_FLOOR(bars_width * 0.20f);

		float backup_initial_col[4];
		memcpy(backup_initial_col, col, components * sizeof(float));

		float wheel_thickness = sv_picker_size * 0.08f;
		float wheel_r_outer = sv_picker_size * 0.50f;
		float wheel_r_inner = wheel_r_outer - wheel_thickness;
		ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width) * 0.5f, picker_pos.y + sv_picker_size * 0.5f);

		// Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated for logic.
		float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
		ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f); // Hue point.
		ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
		ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

		float H = col[0], S = col[1], V = col[2];
		float R = col[0], G = col[1], B = col[2];
		if (flags & ImGuiColorEditFlags_InputRGB)
		{
			// Hue is lost when converting from greyscale rgb (saturation=0). Restore it.
			ImGui::ColorConvertRGBtoHSV(R, G, B, H, S, V);
			if (memcmp(g.ColorEditLastColor, col, sizeof(float) * 3) == 0)
			{
				if (S == 0)
					H = g.ColorEditLastHue;
				if (V == 0)
					S = g.ColorEditLastSat;
			}
		}
		else if (flags & ImGuiColorEditFlags_InputHSV)
		{
			ImGui::ColorConvertHSVtoRGB(H, S, V, R, G, B);
		}

		bool value_changed = false, value_changed_h = false, value_changed_sv = false;

		ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
		if (flags & ImGuiColorEditFlags_PickerHueWheel)
		{
			// Hue wheel + SV triangle logic
			ImGui::InvisibleButton("hsv", ImVec2(sv_picker_size + style.ItemInnerSpacing.x + bars_width, sv_picker_size));
			if (ImGui::IsItemActive())
			{
				ImVec2 initial_off = g.IO.MouseClickedPos[0] - wheel_center;
				ImVec2 current_off = g.IO.MousePos - wheel_center;
				float initial_dist2 = ImLengthSqr(initial_off);
				if (initial_dist2 >= (wheel_r_inner - 1) * (wheel_r_inner - 1) && initial_dist2 <= (wheel_r_outer + 1) * (wheel_r_outer + 1))
				{
					// Interactive with Hue wheel
					H = ImAtan2(current_off.y, current_off.x) / IM_PI * 0.5f;
					if (H < 0.0f)
						H += 1.0f;
					value_changed = value_changed_h = true;
				}
				float cos_hue_angle = ImCos(-H * 2.0f * IM_PI);
				float sin_hue_angle = ImSin(-H * 2.0f * IM_PI);
				if (ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, ImRotate(initial_off, cos_hue_angle, sin_hue_angle)))
				{
					// Interacting with SV triangle
					ImVec2 current_off_unrotated = ImRotate(current_off, cos_hue_angle, sin_hue_angle);
					if (!ImTriangleContainsPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated))
						current_off_unrotated = ImTriangleClosestPoint(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated);
					float uu, vv, ww;
					ImTriangleBarycentricCoords(triangle_pa, triangle_pb, triangle_pc, current_off_unrotated, uu, vv, ww);
					V = ImClamp(1.0f - vv, 0.0001f, 1.0f);
					S = ImClamp(uu / V, 0.0001f, 1.0f);
					value_changed = value_changed_sv = true;
				}
			}

			if (!(flags & ImGuiColorEditFlags_NoOptions))
				ImGui::OpenPopupOnItemClick("context");
		}
		else if (flags & ImGuiColorEditFlags_PickerHueBar)
		{
			// SV rectangle logic
			ImGui::InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));

			if (ImGui::IsItemActive())
			{
				S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
				V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
				value_changed = value_changed_sv = true;
			}

			if (!(flags & ImGuiColorEditFlags_NoOptions))
				ImGui::OpenPopupOnItemClick("context");

			// Hue bar logic
			ImGui::SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
			ImGui::InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));

			if (ImGui::IsItemActive())
			{
				H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
				value_changed = value_changed_h = true;
			}
		}

		// Alpha bar logic
		if (alpha_bar)
		{
			ImGui::SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
			ImGui::InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));

			if (ImGui::IsItemActive())
			{
				col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
				value_changed = true;
			}
		}

		ImGui::PopItemFlag(); // ImGuiItemFlags_NoNav

		if (!(flags & ImGuiColorEditFlags_NoLabel))
		{
			const char* label_display_end = ImGui::FindRenderedTextEnd(label);

			if (label != label_display_end)
			{
				if ((flags & ImGuiColorEditFlags_NoSidePreview))
					ImGui::SameLine(0, style.ItemInnerSpacing.x);

				ImGui::TextEx(label, label_display_end);
			}
		}

		// Convert back color to RGB
		if (value_changed_h || value_changed_sv)
		{
			if (flags & ImGuiColorEditFlags_InputRGB)
			{
				ImGui::ColorConvertHSVtoRGB(H >= 1.0f ? H - 10 * 1e-6f : H, S > 0.0f ? S : 10 * 1e-6f, V > 0.0f ? V : 1e-6f, col[0], col[1], col[2]);
				g.ColorEditLastHue = H;
				g.ColorEditLastSat = S;
				memcpy(g.ColorEditLastColor, col, sizeof(float) * 3);
			}
			else if (flags & ImGuiColorEditFlags_InputHSV)
			{
				col[0] = H;
				col[1] = S;
				col[2] = V;
			}
		}

		// R,G,B and H,S,V slider color editor
		bool value_changed_fix_hue_wrap = false;
		if ((flags & ImGuiColorEditFlags_NoInputs) == 0)
		{
			ImGui::PushItemWidth((alpha_bar ? bar1_pos_x : bar0_pos_x) + bars_width - picker_pos.x);
			ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
			ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;
			if (flags & ImGuiColorEditFlags_DisplayRGB || (flags & ImGuiColorEditFlags__DisplayMask) == 0)
				if (color_edit4("##rgb", col, sub_flags | ImGuiColorEditFlags_DisplayRGB))
				{
					// FIXME: Hackily differentiating using the DragInt (ActiveId != 0 && !ActiveIdAllowOverlap) vs. using the InputText or DropTarget.
					// For the later we don't want to run the hue-wrap canceling code. If you are well versed in HSV picker please provide your input! (See #2050)
					value_changed_fix_hue_wrap = (g.ActiveId != 0 && !g.ActiveIdAllowOverlap);
					value_changed = true;
				}

			if (flags & ImGuiColorEditFlags_DisplayHSV || (flags & ImGuiColorEditFlags__DisplayMask) == 0)
				value_changed |= color_edit4("##hsv", col, sub_flags | ImGuiColorEditFlags_DisplayHSV);

			if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags__DisplayMask) == 0)
				value_changed |= color_edit4("##hex", col, sub_flags | ImGuiColorEditFlags_DisplayHex);

			ImGui::PopItemWidth();
		}

		// Try to cancel hue wrap (after ColorEdit4 call), if any
		if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
		{
			float new_H, new_S, new_V;
			ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);

			if (new_H <= 0 && H > 0)
			{
				if (new_V <= 0 && V != new_V)
					ImGui::ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
				else if (new_S <= 0)
					ImGui::ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
			}
		}

		if (value_changed)
		{
			if (flags & ImGuiColorEditFlags_InputRGB)
			{
				R = col[0];
				G = col[1];
				B = col[2];

				ImGui::ColorConvertRGBtoHSV(R, G, B, H, S, V);

				if (memcmp(g.ColorEditLastColor, col, sizeof(float) * 3) == 0) // Fix local Hue as display below will use it immediately.
				{
					if (S == 0)
						H = g.ColorEditLastHue;

					if (V == 0)
						S = g.ColorEditLastSat;
				}
			}
			else if (flags & ImGuiColorEditFlags_InputHSV)
			{
				H = col[0];
				S = col[1];
				V = col[2];

				ImGui::ColorConvertHSVtoRGB(H, S, V, R, G, B);
			}
		}

		const int style_alpha8 = static_cast<int>(alpha);
		const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
		const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
		const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
		const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

		ImVec4 hue_color_f(1, 1, 1, alpha / 255.0f);
		ImGui::ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
		ImU32 hue_color32 = ImGui::ColorConvertFloat4ToU32(hue_color_f);
		ImU32 user_col32_striped_of_alpha = ImGui::ColorConvertFloat4ToU32(ImVec4(R, G, B, alpha / 255.0f)); // Important: this is still including the main rendering/style alpha!!

		ImVec2 sv_cursor_pos;
		float sv_cursor_rad = u::dpi(7.0f);
		if (flags & ImGuiColorEditFlags_PickerHueWheel)
		{
			// Render Hue Wheel
			const float aeps = 0.5f / wheel_r_outer; // Half a pixel arc length in radians (2pi cancels out).
			const int segment_per_arc = ImMax(4, (int)wheel_r_outer / 12);
			for (int n = 0; n < 6; n++)
			{
				const float a0 = (n) / 6.0f * 2.0f * IM_PI - aeps;
				const float a1 = (n + 1.0f) / 6.0f * 2.0f * IM_PI + aeps;
				const int vert_start_idx = draw_list->VtxBuffer.Size;
				draw_list->PathArcTo(wheel_center, (wheel_r_inner + wheel_r_outer) * 0.5f, a0, a1, segment_per_arc);
				draw_list->PathStroke(col_white, false, wheel_thickness);
				const int vert_end_idx = draw_list->VtxBuffer.Size;

				// Paint colors over existing vertices
				ImVec2 gradient_p0(wheel_center.x + ImCos(a0) * wheel_r_inner, wheel_center.y + ImSin(a0) * wheel_r_inner);
				ImVec2 gradient_p1(wheel_center.x + ImCos(a1) * wheel_r_inner, wheel_center.y + ImSin(a1) * wheel_r_inner);
				ImGui::ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, col_hues[n], col_hues[n + 1]);
			}

			// Render Cursor + preview on Hue Wheel
			float cos_hue_angle = ImCos(H * 2.0f * IM_PI);
			float sin_hue_angle = ImSin(H * 2.0f * IM_PI);
			ImVec2 hue_cursor_pos(wheel_center.x + cos_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f, wheel_center.y + sin_hue_angle * (wheel_r_inner + wheel_r_outer) * 0.5f);
			float hue_cursor_rad = value_changed_h ? wheel_thickness * 0.65f : wheel_thickness * 0.55f;
			int hue_cursor_segments = ImClamp((int)(hue_cursor_rad / 1.4f), 9, 32);
			draw_list->AddCircleFilled(hue_cursor_pos, hue_cursor_rad, hue_color32, hue_cursor_segments);
			draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad + 1, col_midgrey, hue_cursor_segments);
			draw_list->AddCircle(hue_cursor_pos, hue_cursor_rad, col_white, hue_cursor_segments);

			// Render SV triangle (rotated according to hue)
			ImVec2 tra = wheel_center + ImRotate(triangle_pa, cos_hue_angle, sin_hue_angle);
			ImVec2 trb = wheel_center + ImRotate(triangle_pb, cos_hue_angle, sin_hue_angle);
			ImVec2 trc = wheel_center + ImRotate(triangle_pc, cos_hue_angle, sin_hue_angle);
			ImVec2 uv_white = ImGui::GetFontTexUvWhitePixel();
			draw_list->PrimReserve(6, 6);
			draw_list->PrimVtx(tra, uv_white, hue_color32);
			draw_list->PrimVtx(trb, uv_white, hue_color32);
			draw_list->PrimVtx(trc, uv_white, col_white);
			draw_list->PrimVtx(tra, uv_white, 0);
			draw_list->PrimVtx(trb, uv_white, col_black);
			draw_list->PrimVtx(trc, uv_white, 0);
			draw_list->AddTriangle(tra, trb, trc, col_midgrey, 1.5f);
			sv_cursor_pos = ImLerp(ImLerp(trc, tra, ImSaturate(S)), trb, ImSaturate(1 - V));
		}
		else if (flags & ImGuiColorEditFlags_PickerHueBar)
		{
			// Render SV Square
			draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), col_white, hue_color32, hue_color32, col_white);
			draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0, 0, col_black, col_black);
			ImGui::RenderFrameBorder(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0.0f);
			sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * sv_picker_size), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
			sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * sv_picker_size), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

			// Render Hue Bar
			for (int i = 0; i < 6; ++i)
				draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1]);

			float bar0_line_y = IM_ROUND(picker_pos.y + H * sv_picker_size);
			ImGui::RenderFrameBorder(ImVec2(bar0_pos_x, picker_pos.y), ImVec2(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size), 0.0f);

			int max_a = ImClamp(static_cast<int>(alpha), 0, 50);

			draw_list->AddCircle(ImRect(ImVec2(bar0_pos_x, bar0_line_y), ImVec2(bar0_pos_x + bars_width,
				bar0_line_y + H)).GetCenter(), sv_cursor_rad - 1.0f, IM_COL32(0, 0, 0, max_a), 32, 2.0f);

			draw_list->AddCircle(ImRect(ImVec2(bar0_pos_x, bar0_line_y), ImVec2(bar0_pos_x + bars_width,
				bar0_line_y + H)).GetCenter(), sv_cursor_rad, col_white, 32, 2.0f);
		}

		// Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)


		int max_a = ImClamp(static_cast<int>(alpha), 0, 50);

		draw_list->AddCircleFilled(sv_cursor_pos, sv_cursor_rad, user_col32_striped_of_alpha, 32);
		draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad + 1, IM_COL32(0, 0, 0, max_a), 32, 2.0f);
		draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad, col_white, 32, 2.0f);

		// Render alpha bar
		if (alpha_bar)
		{
			float alpha_s = ImSaturate(col[3]);
			ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);

			if (alpha >= 255.0f)
				ImGui::RenderColorRectWithAlphaCheckerboard(draw_list, bar1_bb.Min, bar1_bb.Max, 0, bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f));

			draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha & ~IM_COL32_A_MASK);

			float bar1_line_y = IM_ROUND(picker_pos.y + (1.0f - alpha_s) * sv_picker_size);

			ImGui::RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);

			draw_list->AddCircle(ImRect(ImVec2(bar1_pos_x, bar1_line_y), ImVec2(bar1_pos_x + bars_width,
				bar1_line_y + H)).GetCenter(), sv_cursor_rad - 1.0f, IM_COL32(0, 0, 0, max_a), 32, 2.0f);
			draw_list->AddCircle(ImRect(ImVec2(bar1_pos_x, bar1_line_y), ImVec2(bar1_pos_x + bars_width,
				bar1_line_y + H)).GetCenter(), sv_cursor_rad, col_white, 32, 2.0f);

			if (alpha >= 255.0f)
			{
				window->DrawList->AddText(core::f::awesome, u::dpi(6.0f), bar1_bb.Max - u::dpi(ImVec2(15.0f, -40.0f)),
					core::s::is_theme_dark ? IM_COL32_WHITE : IM_COL32(102, 102, 102, (int)alpha), (char*)u8"\uf1fb");
			}
		}

		ImGui::EndGroup();

		if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0)
			value_changed = false;

		if (value_changed)
			ImGui::MarkItemEdited(window->DC.LastItemId);

		ImGui::PopID();

		return value_changed;
	}

	bool color_picker3(const char* label, float col[3],
		ImGuiColorEditFlags flags)
	{
		float col4[4] = { col[0], col[1], col[2], 1.0f };

		if (!color_picker4(label, col4, flags | ImGuiColorEditFlags_NoAlpha))
			return false;

		col[0] = col4[0]; col[1] = col4[1]; col[2] = col4[2];

		return true;
	}
	bool color_edit4(const char* label, Color *col,
		bool offset_y, bool is_in_selecteable)
	{
		ImGui::SameLine();

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(35.0f, 35.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(15.0f, 15.0f));

		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, 0);
		ImGui::PushStyleColor(ImGuiCol_PlotLines, 0);

		ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
			ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
		float fixed_color[4] =
		{
			(float)col->r() / 255.0f,
			(float)col->g() / 255.0f,
			(float)col->b() / 255.0f,
			(float)col->a() / 255.0f
		};
		const bool result = color_edit4(label, fixed_color, ImGuiColorEditFlags_NoInputs |
			ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaBar |
			ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview, offset_y, is_in_selecteable);

		ImGui::PopStyleColor(6);
		ImGui::PopStyleVar(4);

		col->SetColor(fixed_color);
		return result;
	}

	bool color_edit4(const char* label, float col[4],
		bool offset_y, bool is_in_selecteable, int id_clr, float offset )
	{
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(35.0f, 35.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(15.0f, 15.0f));

		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, 0);
		ImGui::PushStyleColor(ImGuiCol_PlotLines, 0);

		ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
			ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);


		const bool result = color_edit4(label, col, ImGuiColorEditFlags_NoInputs |
			ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview |
			ImGuiColorEditFlags_NoLabel, offset_y, is_in_selecteable, id_clr);

		ImGui::PopStyleColor(6);
		ImGui::PopStyleVar(4);


		return result;
	}

	bool color_edit3(const char* label, float col[3],
		bool offset_y, bool is_in_selecteable)
	{
		ImGui::SameLine();

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(35.0f, 35.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(15.0f, 15.0f));

		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, 0);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, 0);
		ImGui::PushStyleColor(ImGuiCol_PlotLines, 0);
		
		ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
			ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
		
		const bool result = color_edit4(label, col, ImGuiColorEditFlags_NoInputs |
			ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoAlpha |
			ImGuiColorEditFlags_NoLabel, offset_y, is_in_selecteable);

		ImGui::PopStyleColor(6);
		ImGui::PopStyleVar(4);

		return result;
	}
}
