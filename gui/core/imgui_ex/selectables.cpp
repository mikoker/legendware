// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "selectables.h"
#include "../../../config/config.h"
namespace imgui_ex
{
	bool color_edit4(const char* label,
		float col[4], bool offset_y = false, bool is_in_selecteable = false, int id = -1, float offset = 0);
	bool color_edit3(const char* label,
		float col[4], bool offset_y = false, bool is_in_selecteable = false);
	bool selectable(const char* label, bool selected,
		ImGuiSelectableFlags flags, const ImVec2& size_arg, float expected_h, int item_id, void* bool_ptr)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;

		if (span_all_columns && window->DC.CurrentColumns) // FIXME-OPT: Avoid if vertically clipped.
			ImGui::PushColumnsBackground();

		// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
		ImGuiID id = window->GetID(std::string(std::string(label) + std::to_string(item_id)).c_str());

		ImVec2 label_size = ImGui::CalcTextSize(g.Font,
			u::dpi(g.FontSize), label, nullptr, true);
		ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);

		ImVec2 pos = window->DC.CursorPos;
		pos.y += window->DC.CurrLineTextBaseOffset;

		ImGui::ItemSize(size, 0.0f);

		// Fill horizontal space
		// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitely right-aligned sizes not visibly match other widgets.
		const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
		const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;

		if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
			size.x = ImMax(label_size.x, max_x - min_x);

		// Text stays at the submission position, but bounding box may be extended on both sides
		const ImVec2 text_min = pos;
		const ImVec2 text_max(min_x + size.x, pos.y + size.y);

		// Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
		ImRect bb(min_x, pos.y, text_max.x, text_max.y);

		if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
		{
			const float spacing_x = style.ItemSpacing.x;
			const float spacing_y = style.ItemSpacing.y;
			const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
			const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
			bb.Min.x -= spacing_L;
			bb.Min.y -= spacing_U;
			bb.Max.x += (spacing_x - spacing_L);
			bb.Max.y += (spacing_y - spacing_U);
		}
		//if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

		bool item_add;

		if (flags & ImGuiSelectableFlags_Disabled)
		{
			ImGuiItemFlags backup_item_flags = window->DC.ItemFlags;
			window->DC.ItemFlags |= ImGuiItemFlags_Disabled | ImGuiItemFlags_NoNavDefaultFocus;
			item_add = ImGui::ItemAdd(bb, id);
			window->DC.ItemFlags = backup_item_flags;
		}
		else
			item_add = ImGui::ItemAdd(bb, id);

		if (!item_add)
		{
			if (span_all_columns && window->DC.CurrentColumns)
				ImGui::PopColumnsBackground();

			auto it = g.Animations.find(id);

			if (it != g.Animations.end())
				it->second = 0.0f;

			return false;
		}

		// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
		ImGuiButtonFlags button_flags = 0;
		if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
		if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
		if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
		if (flags & ImGuiSelectableFlags_Disabled) { button_flags |= ImGuiButtonFlags_Disabled; }
		if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
		if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

		if (flags & ImGuiSelectableFlags_Disabled)
			selected = false;

		const bool was_selected = selected;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);

		// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
		if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
		{
			if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
			{
				g.NavDisableHighlight = true;
				ImGui::SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent);
			}
		}

		if (pressed)
			ImGui::MarkItemEdited(id);

		if (flags & ImGuiSelectableFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		// In this branch, Selectable() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		if (held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
			hovered = true;

		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PopColumnsBackground();

		auto it = g.Animations.find(id);
		float value = it != g.Animations.end() ? it->second : 0.0f;

		if (expected_h <= ImGui::GetWindowSize().y)
		{
			u::fade(false, value, 0.0f, 255.0f);


			if (selected) ImGui::PushFont(core::f::dinpro_bold);
			window->DrawList->AddText(g.Font, u::dpi(g.FontSize), text_min +
				ImVec2(15.0f, 0.0f), ImGui::GetColorU32(ImGuiCol_Text, value / 255.0f), label);


			if (selected) ImGui::PopFont();

		}

		if (it != g.Animations.end())
			it->second = value;
		else if (value > 0.0f)
			g.Animations.insert(std::pair(id, value));

		// Automatically close popups
		if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
			ImGui::CloseCurrentPopup();


		if (bool_ptr != nullptr && item_id > -1)
		{
			if (config->scripts.developer_mode && ImGui::IsItemHovered())
			{
				for (auto& item : config->items)
				{
					if (DWORD(bool_ptr) == DWORD(item.second.pointer.get()) + item_id && item.second.show)
					{
						ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
							ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
						ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
						ImGui::SetTooltip((item.second.name + "[" + std::to_string(item_id) + "]").c_str());
						ImGui::PopStyleVar();
						ImGui::PopStyleColor();
						break;
					}
				}
			}
		}
		return pressed;
	}
	bool selectable_color_clr(const char* label, bool selected, Color* clr,
		ImGuiSelectableFlags flags, const ImVec2& size_arg, float expected_h, bool close)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;

		if (span_all_columns && window->DC.CurrentColumns) // FIXME-OPT: Avoid if vertically clipped.
			ImGui::PushColumnsBackground();

		// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
		ImGuiID id = window->GetID(label);

		ImVec2 label_size = ImGui::CalcTextSize(g.Font,
			u::dpi(g.FontSize), label, nullptr, true);
		ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);

		ImVec2 pos = window->DC.CursorPos;
		pos.y += window->DC.CurrLineTextBaseOffset;

		ImGui::ItemSize(size, 0.0f);

		// Fill horizontal space
		// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitely right-aligned sizes not visibly match other widgets.
		const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
		const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;

		if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
			size.x = ImMax(label_size.x, max_x - min_x);

		// Text stays at the submission position, but bounding box may be extended on both sides
		const ImVec2 text_min = pos;
		const ImVec2 text_max(min_x + size.x - u::dpi(40.0f), pos.y + size.y);

		// Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
		ImRect bb(min_x, pos.y, text_max.x, text_max.y);

		if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
		{
			const float spacing_x = style.ItemSpacing.x;
			const float spacing_y = style.ItemSpacing.y;
			const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
			const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
			bb.Min.x -= spacing_L;
			bb.Min.y -= spacing_U;
			bb.Max.x += (spacing_x - spacing_L);
			bb.Max.y += (spacing_y - spacing_U);
		}
		//if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

		bool item_add;

		if (flags & ImGuiSelectableFlags_Disabled)
		{
			ImGuiItemFlags backup_item_flags = window->DC.ItemFlags;
			window->DC.ItemFlags |= ImGuiItemFlags_Disabled | ImGuiItemFlags_NoNavDefaultFocus;
			item_add = ImGui::ItemAdd(bb, id);
			window->DC.ItemFlags = backup_item_flags;
		}
		else
			item_add = ImGui::ItemAdd(bb, id);

		if (!item_add)
		{
			if (span_all_columns && window->DC.CurrentColumns)
				ImGui::PopColumnsBackground();

			auto it = g.Animations.find(id);

			if (it != g.Animations.end())
				it->second = 0.0f;

			return false;
		}

		// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
		ImGuiButtonFlags button_flags = 0;
		if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
		if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
		if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
		if (flags & ImGuiSelectableFlags_Disabled) { button_flags |= ImGuiButtonFlags_Disabled; }
		if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
		if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

		if (flags & ImGuiSelectableFlags_Disabled)
			selected = false;

		const bool was_selected = selected;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);

		// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
		if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
		{
			if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
			{
				g.NavDisableHighlight = true;
				ImGui::SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent);
			}
		}

		if (pressed)
			ImGui::MarkItemEdited(id);

		if (flags & ImGuiSelectableFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		// In this branch, Selectable() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		if (held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
			hovered = true;

		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PopColumnsBackground();

		auto it = g.Animations.find(id);
		float value = it != g.Animations.end() ? it->second : 0.0f;

		float fixed_color[4] =
		{
			(float)clr->r() / 255.0f,
			(float)clr->g() / 255.0f,
			(float)clr->b() / 255.0f,
			(float)clr->a() / 255.0f
		};

		if (expected_h <= ImGui::GetWindowSize().y + 15.0f)
		{
			u::fade(false, value, 0.0f, 255.0f);


			if (selected) ImGui::PushFont(core::f::dinpro_bold);

			const ImRect h(window->Pos - ImVec2(0.0f, 25.0f), window->Pos + ImVec2(window->SizeFull.x, expected_h));

			bool is_h = ImGui::IsMouseHoveringRect(h.Min, h.Max);

			if (g.DragDropActive && !is_h && close ||
				ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ImGui::CloseCurrentPopup();
			}
			window->DrawList->AddText(g.Font, u::dpi(g.FontSize), text_min +
				ImVec2(15.0f, 0.0f), ImGui::GetColorU32(ImGuiCol_Text, value / 255.0f), label);


			if (selected) ImGui::PopFont();


			ImGui::SetNextWindowBgAlpha(value / 255.0f);

			color_edit4(label, fixed_color, true, true);
		}
		clr->SetColor(fixed_color);
		if (it != g.Animations.end())
			it->second = value;
		else if (value > 0.0f)
			g.Animations.insert(std::pair(id, value));

		// Automatically close popups
		if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
			ImGui::CloseCurrentPopup();

		return pressed;
	}


	bool selectable_color(const char* label, bool selected, float clr[4],
		ImGuiSelectableFlags flags, const ImVec2& size_arg, float expected_h, bool close, int id_clr, void* ptr_bool)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;

		if (span_all_columns && window->DC.CurrentColumns) // FIXME-OPT: Avoid if vertically clipped.
			ImGui::PushColumnsBackground();

		// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
		ImGuiID id = window->GetID(label);

		ImVec2 label_size = ImGui::CalcTextSize(g.Font,
			u::dpi(g.FontSize), label, nullptr, true);
		ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);

		ImVec2 pos = window->DC.CursorPos;
		pos.y += window->DC.CurrLineTextBaseOffset;

		ImGui::ItemSize(size, 0.0f);

		// Fill horizontal space
		// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitely right-aligned sizes not visibly match other widgets.
		const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
		const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;

		if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
			size.x = ImMax(label_size.x, max_x - min_x);

		// Text stays at the submission position, but bounding box may be extended on both sides
		const ImVec2 text_min = pos;
		const ImVec2 text_max(min_x + size.x - u::dpi(40.0f), pos.y + size.y);

		// Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
		ImRect bb(min_x, pos.y, text_max.x, text_max.y);

		if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
		{
			const float spacing_x = style.ItemSpacing.x;
			const float spacing_y = style.ItemSpacing.y;
			const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
			const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
			bb.Min.x -= spacing_L;
			bb.Min.y -= spacing_U;
			bb.Max.x += (spacing_x - spacing_L);
			bb.Max.y += (spacing_y - spacing_U);
		}
		//if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

		bool item_add;

		if (flags & ImGuiSelectableFlags_Disabled)
		{
			ImGuiItemFlags backup_item_flags = window->DC.ItemFlags;
			window->DC.ItemFlags |= ImGuiItemFlags_Disabled | ImGuiItemFlags_NoNavDefaultFocus;
			item_add = ImGui::ItemAdd(bb, id);
			window->DC.ItemFlags = backup_item_flags;
		}
		else
			item_add = ImGui::ItemAdd(bb, id);

		if (!item_add)
		{
			if (span_all_columns && window->DC.CurrentColumns)
				ImGui::PopColumnsBackground();

			auto it = g.Animations.find(id);

			if (it != g.Animations.end())
				it->second = 0.0f;

			return false;
		}

		// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
		ImGuiButtonFlags button_flags = 0;
		if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
		if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
		if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
		if (flags & ImGuiSelectableFlags_Disabled) { button_flags |= ImGuiButtonFlags_Disabled; }
		if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
		if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

		if (flags & ImGuiSelectableFlags_Disabled)
			selected = false;

		const bool was_selected = selected;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);

		// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
		if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
		{
			if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
			{
				g.NavDisableHighlight = true;
				ImGui::SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent);
			}
		}

		if (pressed)
			ImGui::MarkItemEdited(id);

		if (flags & ImGuiSelectableFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		// In this branch, Selectable() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		if (held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
			hovered = true;

		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PopColumnsBackground();

		auto it = g.Animations.find(id);
		float value = it != g.Animations.end() ? it->second : 0.0f;

		if (expected_h <= ImGui::GetWindowSize().y + 15.0f)
		{
			u::fade(false, value, 0.0f, 255.0f);


			if (selected) ImGui::PushFont(core::f::dinpro_bold);

			const ImRect h(window->Pos - ImVec2(0.0f, 25.0f), window->Pos + ImVec2(window->SizeFull.x, expected_h));

			bool is_h = ImGui::IsMouseHoveringRect(h.Min, h.Max);

			if (g.DragDropActive && !is_h && close ||
				ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ImGui::CloseCurrentPopup();
			}

			window->DrawList->AddText(g.Font, u::dpi(g.FontSize), text_min +
				ImVec2(15.0f, 0.0f), ImGui::GetColorU32(ImGuiCol_Text, value / 255.0f), label);

			if (selected) ImGui::PopFont();


			ImGui::SetNextWindowBgAlpha(value / 255.0f);

			if (ptr_bool != nullptr)
			{
				if (config->scripts.developer_mode && ImGui::IsItemHovered())
				{
					for (auto& item : config->items)
					{
						if (DWORD(ptr_bool) == DWORD(item.second.pointer.get()) + id_clr && item.second.show)
						{
							ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
								ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
							ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
							ImGui::SetTooltip((item.second.name + "[" + std::to_string(id_clr) + "]").c_str());
							ImGui::PopStyleVar();
							ImGui::PopStyleColor();
							break;
						}
					}
				}
			}

			color_edit4(label, clr, true, true, id_clr);
		}

		if (it != g.Animations.end())
			it->second = value;
		else if (value > 0.0f)
			g.Animations.insert(std::pair(id, value));

		// Automatically close popups
		if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
			ImGui::CloseCurrentPopup();

		return pressed;
	}

	bool selectable_color3(const char* label, bool selected, float clr[3], ImGuiSelectableFlags flags, const ImVec2& size_arg, float expected_h, bool close, int id_clr, void* ptr_bool)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;

		if (span_all_columns && window->DC.CurrentColumns) // FIXME-OPT: Avoid if vertically clipped.
			ImGui::PushColumnsBackground();

		// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
		ImGuiID id = window->GetID(label);

		ImVec2 label_size = ImGui::CalcTextSize(g.Font,
			u::dpi(g.FontSize), label, nullptr, true);
		ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);

		ImVec2 pos = window->DC.CursorPos;
		pos.y += window->DC.CurrLineTextBaseOffset;

		ImGui::ItemSize(size, 0.0f);

		// Fill horizontal space
		// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitely right-aligned sizes not visibly match other widgets.
		const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
		const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;

		if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
			size.x = ImMax(label_size.x, max_x - min_x);

		// Text stays at the submission position, but bounding box may be extended on both sides
		const ImVec2 text_min = pos;
		const ImVec2 text_max(min_x + size.x - u::dpi(40.0f), pos.y + size.y);

		// Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
		ImRect bb(min_x, pos.y, text_max.x, text_max.y);

		if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
		{
			const float spacing_x = style.ItemSpacing.x;
			const float spacing_y = style.ItemSpacing.y;
			const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
			const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
			bb.Min.x -= spacing_L;
			bb.Min.y -= spacing_U;
			bb.Max.x += (spacing_x - spacing_L);
			bb.Max.y += (spacing_y - spacing_U);
		}
		//if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

		bool item_add;

		if (flags & ImGuiSelectableFlags_Disabled)
		{
			ImGuiItemFlags backup_item_flags = window->DC.ItemFlags;
			window->DC.ItemFlags |= ImGuiItemFlags_Disabled | ImGuiItemFlags_NoNavDefaultFocus;
			item_add = ImGui::ItemAdd(bb, id);
			window->DC.ItemFlags = backup_item_flags;
		}
		else
			item_add = ImGui::ItemAdd(bb, id);

		if (!item_add)
		{
			if (span_all_columns && window->DC.CurrentColumns)
				ImGui::PopColumnsBackground();

			auto it = g.Animations.find(id);

			if (it != g.Animations.end())
				it->second = 0.0f;

			return false;
		}

		// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
		ImGuiButtonFlags button_flags = 0;
		if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
		if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
		if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
		if (flags & ImGuiSelectableFlags_Disabled) { button_flags |= ImGuiButtonFlags_Disabled; }
		if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
		if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

		if (flags & ImGuiSelectableFlags_Disabled)
			selected = false;

		const bool was_selected = selected;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);

		// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
		if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
		{
			if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
			{
				g.NavDisableHighlight = true;
				ImGui::SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent);
			}
		}

		if (pressed)
			ImGui::MarkItemEdited(id);

		if (flags & ImGuiSelectableFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		// In this branch, Selectable() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		if (held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
			hovered = true;

		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PopColumnsBackground();

		auto it = g.Animations.find(id);
		float value = it != g.Animations.end() ? it->second : 0.0f;

		if (expected_h <= ImGui::GetWindowSize().y + 15.0f)
		{
			u::fade(false, value, 0.0f, 255.0f);


			if (selected) ImGui::PushFont(core::f::dinpro_bold);

			const ImRect h(window->Pos - ImVec2(0.0f, 25.0f), window->Pos + ImVec2(window->SizeFull.x, expected_h));

			bool is_h = ImGui::IsMouseHoveringRect(h.Min, h.Max);

			if (g.DragDropActive && !is_h && close ||
				ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ImGui::CloseCurrentPopup();
			}

			window->DrawList->AddText(g.Font, u::dpi(g.FontSize), text_min +
				ImVec2(15.0f, 0.0f), ImGui::GetColorU32(ImGuiCol_Text, value / 255.0f), label);

			if (selected) ImGui::PopFont();


			ImGui::SetNextWindowBgAlpha(value / 255.0f);

			if (ptr_bool != nullptr)
			{
				if (config->scripts.developer_mode && ImGui::IsItemHovered())
				{
					for (auto& item : config->items)
					{
						if (DWORD(ptr_bool) == DWORD(item.second.pointer.get()) + id_clr && item.second.show)
						{
							ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
								ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
							ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
							ImGui::SetTooltip((item.second.name + "[" + std::to_string(id_clr) + "]").c_str());
							ImGui::PopStyleVar();
							ImGui::PopStyleColor();
							break;
						}
					}
				}
			}
			
			color_edit3(label, clr, true, true);
			//color_edit4(label, clr, true, true, id_clr);
		}

		if (it != g.Animations.end())
			it->second = value;
		else if (value > 0.0f)
			g.Animations.insert(std::pair(id, value));

		// Automatically close popups
		if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
			ImGui::CloseCurrentPopup();

		return pressed;
	}



	bool selectable(const char* label, bool* p_selected,
		ImGuiSelectableFlags flags, const ImVec2& size_arg)
	{
		if (selectable(label, *p_selected, flags, size_arg))
		{
			*p_selected = !*p_selected;

			return true;
		}

		return false;
	}

	bool selectable_color(const char* label, bool* p_selected, float clr[4],
		ImGuiSelectableFlags flags, const ImVec2& size_arg)
	{
		if (selectable_color(label, *p_selected, clr, flags, size_arg))
		{
			*p_selected = !*p_selected;

			return true;
		}

		return false;
	}
	bool selectable_preview(const char* label, bool selected,
		ImGuiSelectableFlags flags, const ImVec2& size_arg, float expected_h)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;

		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PushColumnsBackground();

		const ImGuiID id = window->GetID(label);

		const std::string text_label = std::string("##text_") + label;
		const ImGuiID id2 = window->GetID(text_label.c_str());

		const std::string timer_label = std::string("##timer_") + label;
		const ImGuiID id3 = window->GetID(timer_label.c_str());

		ImFont* font = selected ? core::f::dinpro_bold : core::f::dinpro;
		float font_size = selected ? core::f::dinpro_bold->FontSize : core::f::dinpro->FontSize;
		ImVec2 label_size = ImGui::CalcTextSize(g.Font,
			u::dpi(font_size), label, nullptr, true);
		

		ImVec2 size(size_arg.x != 0.0f ? u::dpi(size_arg.x) : label_size.x, size_arg.y != 0.0f ? u::dpi(size_arg.y) : label_size.y);

		ImVec2 pos = window->DC.CursorPos;
		pos.y += window->DC.CurrLineTextBaseOffset;

		ImGui::ItemSize(size, 0.0f);

		const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
		const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;

		if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
			size.x = ImMax(label_size.x, max_x - min_x);

		const ImVec2 text_max(min_x + size.x, pos.y + size.y);

		ImRect bb(min_x, pos.y, text_max.x, text_max.y);

		if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
		{
			const float spacing_x = style.ItemSpacing.x;
			const float spacing_y = style.ItemSpacing.y;
			const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
			const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
			bb.Min.x -= spacing_L;
			bb.Min.y -= spacing_U;
			bb.Max.x += (spacing_x - spacing_L);
			bb.Max.y += (spacing_y - spacing_U);
		}

		bool item_add;

		if (flags & ImGuiSelectableFlags_Disabled)
		{
			ImGuiItemFlags backup_item_flags = window->DC.ItemFlags;
			window->DC.ItemFlags |= ImGuiItemFlags_Disabled | ImGuiItemFlags_NoNavDefaultFocus;
			item_add = ImGui::ItemAdd(bb, id);
			window->DC.ItemFlags = backup_item_flags;
		}
		else
			item_add = ImGui::ItemAdd(bb, id);

		if (!item_add)
		{
			if (span_all_columns && window->DC.CurrentColumns)
				ImGui::PopColumnsBackground();

			auto it = g.Animations.find(id);

			if (it != g.Animations.end())
				it->second = 0.0f;

			return false;
		}

		ImGuiButtonFlags button_flags = 0;

		if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
		if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
		if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
		if (flags & ImGuiSelectableFlags_Disabled) { button_flags |= ImGuiButtonFlags_Disabled; }
		if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
		if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

		if (flags & ImGuiSelectableFlags_Disabled)
			selected = false;

		const bool was_selected = selected;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);

		if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
		{
			if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
			{
				g.NavDisableHighlight = true;
				ImGui::SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent);
			}
		}

		if (pressed)
			ImGui::MarkItemEdited(id);

		if (flags & ImGuiSelectableFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		if (selected != was_selected)
			window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		if (held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
			hovered = true;

		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PopColumnsBackground();

		auto it = g.Animations.find(id);
		float value = it != g.Animations.end() ? it->second : 0.0f;

		const float max_elapsed_time = 15.0f;
		const float initial_label_pos_x = 15.0f;

		const float label_min_pos_x = -std::fabs(size.x - label_size.x) - 7.5f;

		auto label_pos_it = g.Animations.find(id2);
		float label_pos_value = label_pos_it != g.Animations.end() ? label_pos_it->second : initial_label_pos_x;

		auto elapsed_time_it = g.Animations.find(id3);
		float elapsed_time_value = elapsed_time_it != g.Animations.end() ? elapsed_time_it->second : 0.0f;

		const bool is_larger = label_size.x > size.x;
		const bool should_move = hovered && is_larger && elapsed_time_value >= max_elapsed_time;

		if (expected_h <= ImGui::GetWindowSize().y)
		{
			u::fade(false, value, 0.0f, 255.0f);

			if (is_larger)
			{
				if (hovered)
					u::fade_noclamp(false, elapsed_time_value, 0.8f);

				if (should_move)
					u::fade(true, label_pos_value, label_min_pos_x, 15.0f, 0.65f);

				if (!hovered)
				{
					if (label_pos_value < initial_label_pos_x)
						u::fade(false, label_pos_value, label_min_pos_x, 15.0f, 1.0f);

					if (!hovered || elapsed_time_value >= max_elapsed_time)
						elapsed_time_value = 0.0f;
				}
			}

			window->DrawList->AddText(selected ? core::f::dinpro_bold : core::f::dinpro, u::dpi(g.FontSize), pos +
				ImVec2(label_pos_value, 0.0f), ImGui::GetColorU32(ImGuiCol_Text, value / 255.0f), label);

		}

		if (it != g.Animations.end())
			it->second = value;
		else if (value > 0.0f)
			g.Animations.insert(std::pair(id, value));

		if (label_pos_it != g.Animations.end())
			label_pos_it->second = label_pos_value;
		else if (label_pos_value < initial_label_pos_x)
			g.Animations.insert(std::pair(id2, label_pos_value));

		if (elapsed_time_it != g.Animations.end())
			elapsed_time_it->second = elapsed_time_value;
		else if (elapsed_time_value > 0.0f)
			g.Animations.insert(std::pair(id3, elapsed_time_value));

		if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
			ImGui::CloseCurrentPopup();

		return pressed;
	}

	bool selectable_preview(const char* label, bool* p_selected,
		ImGuiSelectableFlags flags, const ImVec2& size_arg)
	{
		if (selectable_preview(label, *p_selected, flags, size_arg))
		{
			*p_selected = !*p_selected;

			return true;
		}

		return false;
	}
}
