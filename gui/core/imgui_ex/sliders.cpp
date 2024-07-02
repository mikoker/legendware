#include "sliders.h"
#include "..\..\..\config\config.h"	
//static const char* PatchFormatStringFloatToInt(const char* fmt)
//{
//	if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
//		return "%d";
//
//	const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
//	const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
//	if (fmt_end > fmt_start && fmt_end[-1] == 'f')
//	{
//#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
//		if (fmt_start == fmt && fmt_end[0] == 0)
//			return "%d";
//		ImGuiContext& g = *GImGui;
//		ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
//		return g.TempBuffer;
//#else
//		IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
//#endif
//	}
//
//	return fmt;
//}
//
//namespace imgui_ex
//{
//	bool dpi_slider_scalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min,
//		const void* p_max, const char* values[], ImGuiSliderFlags flags)
//	{
//		ImGuiWindow* window = ImGui::GetCurrentWindow();
//
//		if (window->SkipItems)
//			return false;
//
//		ImGuiContext& g = *GImGui;
//
//		const ImGuiStyle& style = g.Style;
//		const ImGuiID id = window->GetID(label);
//
//		const float w = ImGui::CalcItemWidth();
//		const char* format = "%d";
//
//		const ImVec2 label_size = ImGui::CalcTextSize(g.Font,
//			u::dpi(g.FontSize), label, nullptr, true);
//		float offset_y = u::dpi(5.0f);
//
//
//
//		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos +
//			ImVec2(w, label_size.y + style.FramePadding.y * 2.0f + offset_y));
//
//		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x >
//			0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
//
//		ImGui::ItemSize(total_bb, style.FramePadding.y);
//
//		if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
//			return false;
//
//		// Default format string when passing NULL
//		if (format == nullptr)
//			format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
//		else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
//			format = PatchFormatStringFloatToInt(format);
//
//		// Tabbing or CTRL-clicking on Slider turns it into an input box
//		const bool hovered = ImGui::ItemHoverable(frame_bb, id);
//		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
//
//		bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
//
//		if (!temp_input_is_active)
//		{
//			const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister(window, id);
//			const bool clicked = (hovered && g.IO.MouseClicked[0]);
//
//			if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
//			{
//				ImGui::SetActiveID(id, window);
//				ImGui::SetFocusID(id, window);
//				ImGui::FocusWindow(window);
//
//				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
//
//				if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
//				{
//					temp_input_is_active = true;
//					ImGui::FocusableItemUnregister(window);
//				}
//			}
//		}
//
//		if (temp_input_is_active)
//		{
//			// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
//			const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
//			return ImGui::TempInputScalar(frame_bb, id, label, ImGuiDataType_S32, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
//		}
//
//		// Draw frame
//		ImGui::RenderNavHighlight(frame_bb, id);
//		float offset = core::s::temp_dpi_scale >= 6 ? (core::s::temp_dpi_scale >= 9 ? 19.0f : 20.0f) : 21.0f;
//		float offset2 = core::s::temp_dpi_scale >= 3 ? 2.0f : 3.0f;
//		ImGui::RenderFrame(frame_bb.Min + ImVec2(0.0f, u::dpi(offset)), frame_bb.Max - ImVec2(0.0f, u::dpi(offset2)), ImGui::GetColorU32(ImGuiCol_SliderGrab), false, 0.0f);
//
//		// Slider behavior
//		ImRect grab_bb;
//
//		const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
//
//		if (value_changed)
//			ImGui::MarkItemEdited(id);
//
//		grab_bb.Min += ImVec2(1.0f, u::dpi(19.0f));
//		grab_bb.Max -= ImVec2(-1.0f, u::dpi(4.0f));
//
//		// Render grab
//		if (grab_bb.Max.x > grab_bb.Min.x)
//		{
//			window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(0.0f, u::dpi(offset)), grab_bb.Max + ImVec2(2.0f, 2.0f - u::dpi(offset2)),
//				ImGui::GetColorU32(ImGuiCol_SliderGrabActive), style.GrabRounding);
//		}
//
//		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
//		ImVec2 value_size = ImGui::CalcTextSize(core::f::dinpro_s, u::dpi(12.0f), values[core::s::temp_dpi_scale - 1]);
//
//		window->DrawList->AddText(core::f::dinpro_s, u::dpi(12.0f), ImVec2(frame_bb.Max.x - value_size.x,
//			frame_bb.Min.y - 4.0f), ImGui::GetColorU32(ImGuiCol_Text), values[core::s::temp_dpi_scale - 1]);
//
//		if (label_size.x > 0.0f)
//		{
//			const char* text = ImGui::FindRenderedTextEnd(label);
//
//			window->DrawList->AddText(g.Font, u::dpi(g.FontSize), ImVec2(frame_bb.Min.x,
//				frame_bb.Min.y - 4.0f), ImGui::GetColorU32(ImGuiCol_Text), label, text);
//		}
//
//		return value_changed;
//	}
//
//	bool slider_scalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min,
//		const void* p_max, const char* format, ImGuiSliderFlags flags)
//	{
//		ImGuiWindow* window = ImGui::GetCurrentWindow();
//
//		if (window->SkipItems)
//			return false;
//
//		ImGuiContext& g = *GImGui;
//		const ImGuiStyle& style = g.Style;
//		const ImGuiID id = window->GetID(label);
//
//		const float w = ImGui::CalcItemWidth();
//
//		const ImVec2 label_size = ImGui::CalcTextSize(g.Font,
//			u::dpi(g.FontSize), label, nullptr, true);
//
//
//		float offset_y = u::dpi(5.0f);
//
//		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos +
//			ImVec2(w, label_size.y + style.FramePadding.y * 2.0f + offset_y));
//
//		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x >
//			0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
//
//		ImGui::ItemSize(total_bb, style.FramePadding.y);
//
//		if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
//			return false;
//
//		// Default format string when passing NULL
//		if (format == nullptr)
//			format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
//		else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
//			format = PatchFormatStringFloatToInt(format);
//
//		// Tabbing or CTRL-clicking on Slider turns it into an input box
//		const bool hovered = ImGui::ItemHoverable(frame_bb, id);
//		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
//
//		bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
//
//		if (!temp_input_is_active)
//		{
//			const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister(window, id);
//			const bool clicked = (hovered && g.IO.MouseClicked[0]);
//
//			if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
//			{
//				ImGui::SetActiveID(id, window);
//				ImGui::SetFocusID(id, window);
//				ImGui::FocusWindow(window);
//
//				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
//
//				if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
//				{
//					temp_input_is_active = true;
//					ImGui::FocusableItemUnregister(window);
//				}
//			}
//		}
//
//		if (temp_input_is_active)
//		{
//			// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
//			const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
//			return ImGui::TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
//		}
//
//		// Draw frame
//		ImGui::RenderNavHighlight(frame_bb, id);
//
//		float offset = core::s::temp_dpi_scale >= 6 ? (core::s::temp_dpi_scale >= 9 ? 19.0f : 20.0f) : 21.0f;
//		float offset2 = core::s::temp_dpi_scale >= 3 ? 2.0f : 3.0f;
//
//		ImGui::RenderFrame(frame_bb.Min + ImVec2(0.0f, u::dpi(offset)), frame_bb.Max - ImVec2(0.0f, u::dpi(offset2)), ImGui::GetColorU32(ImGuiCol_SliderGrab), false, 0.0f);
//
//		// Slider behavior
//		ImRect grab_bb;
//
//		 bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
//		if (hovered)
//		{
//			window->not_scroll = true;
//
//			bool integral = false;
//			float max_range = 0;
//			if (data_type == ImGuiDataType_S32)
//				integral = true;
//			if (integral)
//				max_range = *(int*)p_max - *(int*)p_min;
//			else
//				max_range = *(float*)p_max - *(float*)p_min;
//			float shift_val = shift_val = 0.01 * max_range;;
//
//			if (g.IO.KeyShift)
//				shift_val = 0.001 * max_range;
//
//			if (g.IO.MouseWheel < 0) {
//
//				if (integral) {
//					if (*(int*)p_data > *(int*)p_min)
//						*(int*)p_data = *(int*)p_data - 1;
//				}
//				else {
//					if (*(float*)p_data > *(float*)p_min)
//						*(float*)p_data = *(float*)p_data - shift_val;
//				}
//				value_changed = true;
//
//			}
//			else if (g.IO.MouseWheel > 0) {
//				if (integral) {
//					if (*(int*)p_data <*(int*)p_max)
//					*(int*)p_data = *(int*)p_data + 1;
//				}
//				else {
//					if (*(float*)p_data < *(float*)p_max)
//					*(float*)p_data = *(float*)p_data + shift_val;
//				}
//				value_changed = true;
//
//			}
//		}else
//			window->not_scroll = false;
//		if (value_changed)
//			ImGui::MarkItemEdited(id);
//
//		grab_bb.Min += ImVec2(1.0f, u::dpi(19.0f));
//		grab_bb.Max -= ImVec2(-1.0f, u::dpi(4.0f));
//
//		// Render grab
//		if (grab_bb.Max.x > grab_bb.Min.x)
//		{
//			window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(0.0f, u::dpi(offset)), grab_bb.Max + ImVec2(2.0f, 2.0f - u::dpi(offset2)),
//				ImGui::GetColorU32(ImGuiCol_SliderGrabActive), style.GrabRounding);
//		}
//
//		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
//		char value_buf[64];
//		const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
//
//		ImVec2 value_size = ImGui::CalcTextSize(core::f::dinpro_s, u::dpi(12.0f), value_buf, value_buf_end);
//
//		window->DrawList->AddText(core::f::dinpro_s, u::dpi(12.0f), ImVec2(frame_bb.Max.x - value_size.x,
//			frame_bb.Min.y - 4.0f), ImGui::GetColorU32(ImGuiCol_Text), value_buf, value_buf_end);
//
//		if (label_size.x > 0.0f)
//		{
//			const char* text = ImGui::FindRenderedTextEnd(label);
//
//			window->DrawList->AddText(g.Font, u::dpi(g.FontSize), ImVec2(frame_bb.Min.x,
//				frame_bb.Min.y - 4.0f), ImGui::GetColorU32(ImGuiCol_Text), label, text);
//		}
//		if (config->scripts.developer_mode && ImGui::IsItemHovered())
//		{
//			for (auto& item : config->items)
//			{
//				if (p_data == item.second.pointer.get())
//				{
//					ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
//						ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
//					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
//					ImGui::SetTooltip(item.second.name.c_str());
//					ImGui::PopStyleVar();
//					ImGui::PopStyleColor();
//					break;
//				}
//			}
//		}
//
//		return value_changed;
//	}
//
//	bool slider_dpi(const char* label, int* v, int v_min,
//		int v_max, const char* values[], float width)
//	{
//		ImGui::PushItemWidth(u::dpi(width));
//
//		const bool result = dpi_slider_scalar(label, ImGuiDataType_S32, v,
//			&v_min, &v_max, values, ImGuiSliderFlags_NoInput);
//
//		ImGui::PopItemWidth();
//
//		return result;
//	}
//
//	bool slider_int(const char* label, int* v, int v_min,
//		int v_max, ImGuiSliderFlags flags , const char* format , float width )
//	{
//		ImGui::PushItemWidth(u::dpi(width));
//
//		const bool result = slider_scalar(label, ImGuiDataType_S32, v,
//			&v_min, &v_max, format, flags | ImGuiSliderFlags_NoInput);
//
//		ImGui::PopItemWidth();
//
//		return result;
//	}
//
//	bool slider_float(const char* label, float* v, float v_min,
//		float v_max, const char* format, float width, ImGuiSliderFlags flags)
//	{
//		ImGui::PushItemWidth(u::dpi(width));
//
//		const bool result = slider_scalar(label, ImGuiDataType_Float, v,
//			&v_min, &v_max, format, flags | ImGuiSliderFlags_NoInput);
//
//		ImGui::PopItemWidth();
//
//		return result;
//	}
//}


static const char* PatchFormatStringFloatToInt(const char* fmt)
{
	if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
		return "%d";

	const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
	const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
	if (fmt_end > fmt_start && fmt_end[-1] == 'f')
	{
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
		if (fmt_start == fmt && fmt_end[0] == 0)
			return "%d";
		ImGuiContext& g = *GImGui;
		ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
		return g.TempBuffer;
#else
		IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
	}

	return fmt;
}

namespace imgui_ex
{
	bool dpi_slider_scalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min,
		const void* p_max, const char* values[], ImGuiSliderFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float w = ImGui::CalcItemWidth();
		const char* format = "%d";

		const ImVec2 label_size = ImGui::CalcTextSize(g.Font,
			u::dpi(g.FontSize), label, nullptr, true);

		float offset_y = u::dpi(5.0f);

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos +
			ImVec2(w, label_size.y + style.FramePadding.y * 2.0f + offset_y));

		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x >
			0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		ImGui::ItemSize(total_bb, style.FramePadding.y);

		if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
			return false;

		// Default format string when passing NULL
		if (format == nullptr)
			format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
		else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
			format = PatchFormatStringFloatToInt(format);

		// Tabbing or CTRL-clicking on Slider turns it into an input box
		const bool hovered = ImGui::ItemHoverable(frame_bb, id);
		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;

		bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);

		if (!temp_input_is_active)
		{
			const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister(window, id);
			const bool clicked = (hovered && g.IO.MouseClicked[0]);

			if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
			{
				ImGui::SetActiveID(id, window);
				ImGui::SetFocusID(id, window);
				ImGui::FocusWindow(window);

				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);

				if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
				{
					temp_input_is_active = true;
					ImGui::FocusableItemUnregister(window);
				}
			}
		}

		if (temp_input_is_active)
		{
			// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
			const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
			return ImGui::TempInputScalar(frame_bb, id, label, ImGuiDataType_S32, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
		}

		// Draw frame
		ImGui::RenderNavHighlight(frame_bb, id);
		float offset = core::s::temp_dpi_scale >= 6 ? (core::s::temp_dpi_scale >= 9 ? 19.0f : 20.0f) : 21.0f;
		float offset2 = core::s::temp_dpi_scale >= 3 ? 2.0f : 3.0f;

		ImGui::RenderFrame(frame_bb.Min + ImVec2(0.0f, u::dpi(offset)), frame_bb.Max - ImVec2(0.0f, u::dpi(offset2)), ImGui::GetColorU32(ImGuiCol_SliderGrab), false, 0.0f);

		// Slider behavior
		ImRect grab_bb;

		const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);

		if (value_changed)
			ImGui::MarkItemEdited(id);

		// Render grab
		if (grab_bb.Max.x > grab_bb.Min.x)
		{
			window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(0.0f, u::dpi(offset)), grab_bb.Max + ImVec2(2.0f, 2.0f - u::dpi(offset2)),
				ImGui::GetColorU32(ImGuiCol_SliderGrabActive), style.GrabRounding);
		}

		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
		ImVec2 value_size = ImGui::CalcTextSize(core::f::dinpro_s, u::dpi(12.0f), values[core::s::temp_dpi_scale - 1]);

		window->DrawList->AddText(core::f::dinpro_s, u::dpi(12.0f), ImVec2(frame_bb.Max.x - value_size.x,
			frame_bb.Min.y - 4.0f), ImGui::GetColorU32(ImGuiCol_Text), values[core::s::temp_dpi_scale - 1]);

		if (label_size.x > 0.0f)
		{
			const char* text = ImGui::FindRenderedTextEnd(label);

			window->DrawList->AddText(g.Font, u::dpi(g.FontSize), ImVec2(frame_bb.Min.x,
				frame_bb.Min.y - 4.0f), ImGui::GetColorU32(ImGuiCol_Text), label, text);
		}

		return value_changed;
	}

	bool slider_scalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min,
		const void* p_max, const char* format, ImGuiSliderFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float w = ImGui::CalcItemWidth();

		const ImVec2 label_size = ImGui::CalcTextSize(g.Font,
			u::dpi(g.FontSize), label, nullptr, true);

		float offset_y = u::dpi(5.0f);

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos +
			ImVec2(w, label_size.y + style.FramePadding.y * 2.0f + offset_y));

		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x >
			0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		ImGui::ItemSize(total_bb, style.FramePadding.y);

		if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
			return false;

		// Default format string when passing NULL
		if (format == nullptr)
			format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
		else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
			format = PatchFormatStringFloatToInt(format);

		// Tabbing or CTRL-clicking on Slider turns it into an input box
		const bool hovered = ImGui::ItemHoverable(frame_bb, id);
		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;

		bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);

		if (!temp_input_is_active)
		{
			const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister(window, id);
			const bool clicked = (hovered && g.IO.MouseClicked[0]);

			if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
			{
				ImGui::SetActiveID(id, window);
				ImGui::SetFocusID(id, window);
				ImGui::FocusWindow(window);

				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);

				if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
				{
					temp_input_is_active = true;
					ImGui::FocusableItemUnregister(window);
				}
			}
		}

		if (temp_input_is_active)
		{
			// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
			const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
			return ImGui::TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
		}

		// Draw frame
		ImGui::RenderNavHighlight(frame_bb, id);

		float offset = core::s::temp_dpi_scale >= 6 ? (core::s::temp_dpi_scale >= 9 ? 19.0f : 20.0f) : 21.0f;
		float offset2 = core::s::temp_dpi_scale >= 3 ? 2.0f : 3.0f;

		ImGui::RenderFrame(frame_bb.Min + ImVec2(0.0f, u::dpi(offset)), frame_bb.Max - ImVec2(0.0f, u::dpi(offset2)), ImGui::GetColorU32(ImGuiCol_SliderGrab), false, 0.0f);

		// Slider behavior
		ImRect grab_bb;

		bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
		if (hovered && g.IO.KeyCtrl)
		{
			window->not_scroll = true;

			bool integral = false;
			float max_range = 0;
			if (data_type == ImGuiDataType_S32)
				integral = true;
			if (integral)
				max_range = *(int*)p_max - *(int*)p_min;
			else
				max_range = *(float*)p_max - *(float*)p_min;
			float shift_val = shift_val = 0.01 * max_range;;

			if (g.IO.KeyShift)
				shift_val = 0.001 * max_range;

			if (g.IO.MouseWheel < 0) {

				if (integral) {
					if (*(int*)p_data > *(int*)p_min)
						*(int*)p_data = *(int*)p_data - 1;
					if (*(int*)p_data < *(int*)p_min)
						*(int*)p_data = *(int*)p_min;
				}
				else {
					if (*(float*)p_data > *(float*)p_min)
						*(float*)p_data = *(float*)p_data - shift_val;
					if (*(float*)p_data < *(float*)p_min)
						*(float*)p_data = *(float*)p_min;
				}
				value_changed = true;

			}
			else if (g.IO.MouseWheel > 0) {
				if (integral) {
					if (*(int*)p_data < *(int*)p_max)
						*(int*)p_data = *(int*)p_data + 1;
					if (*(int*)p_data > *(int*)p_max)
						*(int*)p_data = *(int*)p_max;
				}
				else {
					if (*(float*)p_data < *(float*)p_max)
						*(float*)p_data = *(float*)p_data + shift_val;
					if (*(float*)p_data > *(float*)p_max)
						*(float*)p_data = *(float*)p_max;
				}

				value_changed = true;

			}
		}

		if (value_changed)
			ImGui::MarkItemEdited(id);

		// Render grab
		if (grab_bb.Max.x > grab_bb.Min.x)
		{
			window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(0.0f, u::dpi(offset)), grab_bb.Max + ImVec2(2.0f, 2.0f - u::dpi(offset2)),
				ImGui::GetColorU32(ImGuiCol_SliderGrabActive), style.GrabRounding);
		}

		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
		char value_buf[64];
		const char* value_buf_end = "";

		if (flags & ImGuiSliderFlags_NoneIfZero && !*(int*)p_data)
			value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), crypt_str("%s"), crypt_str("None"));
		else
			value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, data_type == ImGuiDataType_Float && (int)*(float*)p_data == *(float*)p_data ? crypt_str("%.0f") : format);

		ImVec2 value_size = ImGui::CalcTextSize(core::f::dinpro_s, u::dpi(12.0f), value_buf, value_buf_end);


		window->DrawList->AddText(core::f::dinpro_s, u::dpi(12.0f), ImVec2(frame_bb.Min.x + w - value_size.x,
			frame_bb.Min.y - 4.0f), ImGui::GetColorU32(ImGuiCol_Text), value_buf, value_buf_end);

		if (label_size.x > 0.0f)
		{
			const char* text = ImGui::FindRenderedTextEnd(label);

			window->DrawList->AddText(g.Font, u::dpi(g.FontSize), ImVec2(frame_bb.Min.x,
				frame_bb.Min.y - 4.0f), ImGui::GetColorU32(ImGuiCol_Text), label, text);
		}
		if (config->scripts.developer_mode && ImGui::IsItemHovered())
		{
			for (auto& item : config->items)
			{
				if (p_data == item.second.pointer.get() && item.second.show)
				{
					ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ? ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
					ImGui::SetTooltip(item.second.name.c_str());
					ImGui::PopStyleVar();
					ImGui::PopStyleColor();
					break;
				}
			}
		}
		return value_changed;
	}



	bool slider_dpi(const char* label, int* v, int v_min,
		int v_max, const char* values[], float width)
	{
		ImGui::PushItemWidth(u::dpi(width));

		const bool result = dpi_slider_scalar(label, ImGuiDataType_S32, v,
			&v_min, &v_max, values, ImGuiSliderFlags_NoInput);

		ImGui::PopItemWidth();

		return result;
	}
	bool slider_int(const char* label, int* v, int v_min,
		int v_max, ImGuiSliderFlags flags, const char* format, float width)
	{
		*v = clamp(*v, v_min, v_max);
		ImGui::PushItemWidth(u::dpi(width));

		const bool result = slider_scalar(label, ImGuiDataType_S32, v,
			&v_min, &v_max, format, flags | ImGuiSliderFlags_NoInput);

		ImGui::PopItemWidth();

		return result;
	}
	bool slider_float(const char* label, float* v, float v_min,
		float v_max, const char* format, float width, ImGuiSliderFlags flags)
	{
		*v = clamp(*v, v_min, v_max);
		ImGui::PushItemWidth(u::dpi(width));

		const bool result = slider_scalar(label, ImGuiDataType_Float, v,
			&v_min, &v_max, format, flags | ImGuiSliderFlags_NoInput);

		ImGui::PopItemWidth();

		return result;
	}
}
