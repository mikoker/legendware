// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "combos.h"
#include "../../../config/config.h"
// Getter for the old Combo() API: const char*[]
static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = static_cast<const char* const*>(data);

	if (out_text)
		*out_text = items[idx];

	return true;
}

// Getter for the old Combo() API: "item1\0item2\0item3\0"
static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
{
	// FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
	const char* items_separated_by_zeros = static_cast<const char*>(data);
	const char* p = items_separated_by_zeros;

	int items_count = 0;

	while (*p)
	{
		if (idx == items_count)
			break;

		p += strlen(p) + 1;
		items_count++;
	}

	if (!*p)

		return false;

	if (out_text)
		*out_text = p;

	return true;
}

static float CalcMaxPopupHeightFromItemCount(int items_count)
{
	ImGuiContext& g = *GImGui;

	if (items_count <= 0)
		return FLT_MAX;

	return (g.FontSize + g.Style.ItemSpacing.y) * static_cast<float>(
		items_count) - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

namespace imgui_ex
{
	bool selectable(const char* label, bool selected = false,
		ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = ImVec2(0.0f, 0.0f), float expected_h = 0.0f, int item_id = -1, void* bool_ptr = nullptr);

	bool selectable_color(const char* label, bool selected, float clr[4], ImGuiSelectableFlags flags = 0,
		const ImVec2& size_arg = ImVec2(0.0f, 0.0f), float expected_h = 0.0f, bool close = false, int id_clr = -1, void* bool_ptr = nullptr);
	bool selectable_color_clr(const char* label, bool selected, Color* clr, ImGuiSelectableFlags flags = 0,
		const ImVec2& size_arg = ImVec2(0.0f, 0.0f), float expected_h = 0.0f, bool close = false);
	bool selectable_color3(const char* label, bool selected, float clr[3], ImGuiSelectableFlags flags, const ImVec2& size_arg, float expected_h, bool close, int id_clr, void* ptr_bool);
	bool combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**),
		void* data, int items_count)
	{
		ImGuiContext& g = *GImGui;

		// Call the getter to obtain the preview string which is a parameter to BeginCombo()
		const char* preview_value = nullptr;

		if (*current_item >= 0 && *current_item < items_count)
			items_getter(data, *current_item, &preview_value);

		// The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(6)));

		float expected_h = 0.0f;

		ImU32 bg = ImGui::GetColorU32(ImGuiCol_ScrollbarBg), grab = ImGui::GetColorU32(ImGuiCol_ScrollbarGrab),
			graba = ImGui::GetColorU32(ImGuiCol_ScrollbarGrabActive), grabh = ImGui::GetColorU32(ImGuiCol_ScrollbarGrabHovered),
			pl = ImGui::GetColorU32(ImGuiCol_PlotLines);

		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, items_count <= 4 ? 0 : bg);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, items_count <= 4 ? 0 : grab);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, items_count <= 4 ? 0 : graba);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, items_count <= 4 ? 0 : grabh);
		ImGui::PushStyleColor(ImGuiCol_PlotLines, items_count <= 4 ? 0 : pl);

		if (!ImGui::BeginCombo(label, preview_value ? preview_value : "", ImGuiComboFlags_None, &expected_h))
		{
			ImGui::PopStyleColor(5);

			return false;
		}

		ImGui::PopStyleColor(5);

		// Display items
		// FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
		bool value_changed = false;

		for (int i = 0; i < items_count; i++)
		{
			ImGui::PushID((void*)(intptr_t)i);

			const bool item_selected = (i == *current_item);
			const char* item_text;

			if (!items_getter(data, i, &item_text))
				item_text = "*Unknown item*";

			if (selectable(item_text, item_selected, 0,
				ImVec2(0.0f, 0.0f), expected_h))
			{
				value_changed = true;
				*current_item = i;
			}

			if (item_selected)
				ImGui::SetItemDefaultFocus();

			ImGui::PopID();
		}

		ImGui::EndCombo();

		return value_changed;
	}

	bool combo(const char* label, int* current_item, const char* const items[],
		int items_count)
	{
		const bool has_shit = strstr(label, "##") != nullptr;

		ImGui::PushItemWidth(u::dpi(206.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

		ImGui::PushStyleColor(ImGuiCol_WindowShadow, 0);

		if (!has_shit)
			ImGui::Spacing();

		const bool result = combo(label, current_item, Items_ArrayGetter,
			(void*)items, items_count);

		if (!has_shit)
			ImGui::Spacing();

		ImGui::PopStyleColor();

		ImGui::PopStyleVar(3);
		ImGui::PopItemWidth();
		if (config->scripts.developer_mode && ImGui::IsItemHovered())
		{
			for (auto& item : config->items)
			{
				if (DWORD(current_item) == DWORD(item.second.pointer.get()) && item.second.show)
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
		return result;
	}

	bool combo(const char* label, int* current_item,
		const char* items_separated_by_zeros)
	{
		const bool has_shit = strstr(label, "##") != nullptr;

		ImGui::PushItemWidth(u::dpi(206.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

		ImGui::PushStyleColor(ImGuiCol_WindowShadow, 0);

		if (!has_shit)
			ImGui::Spacing();

		int items_count = 0;
		const char* p = items_separated_by_zeros;

		while (*p)
		{
			p += strlen(p) + 1;
			items_count++;
		}

		const bool result = combo(label, current_item, Items_SingleStringGetter,
			(void*)items_separated_by_zeros, items_count);

		if (!has_shit)
			ImGui::Spacing();

		ImGui::PopStyleColor();

		ImGui::PopStyleVar(3);
		ImGui::PopItemWidth();

		return result;
	}


	bool mcombo(const char* label, bool* v, const char* items[], int items_count)
	{
		const bool has_shit = strstr(label, "##") != nullptr;

		ImGui::PushItemWidth(u::dpi(206.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

		ImGui::PushStyleColor(ImGuiCol_WindowShadow, 0);

		if (!has_shit)
			ImGui::Spacing();

		std::string preview = "";
		ImVec2 preview_size = ImVec2(0.0f, 0.0f);

		for (int i = 0; i < items_count; i++)
		{
			if (v[i])
			{
				preview_size = ImGui::CalcTextSize(preview.c_str());

				if (preview.empty())
					preview.assign(items[i]);
				else if (preview_size.x >= 90.0f && preview.find(", ...") == std::string::npos)
					preview.append(", ...");
				else if (preview.find(", ...") == std::string::npos)
					preview.append(", ").append(items[i]);
			}
		}

		if (preview.empty())
			preview.assign("None");

		bool value_changed = false;

		float expected_h = 0.0f;

		if (ImGui::BeginCombo(label, preview.c_str(), 0, &expected_h))
		{
			for (int i = 0; i < items_count; i++)
			{
				if (selectable(items[i], v[i],
					ImGuiSelectableFlags_DontClosePopups, ImVec2(0.0f, 0.0f), expected_h, i , &v[i]))
				{
					v[i] = !v[i];

					value_changed = true;
				}
			}

			ImGui::EndCombo();
		}

		if (!has_shit)
			ImGui::Spacing();

		ImGui::PopStyleColor();

		ImGui::PopStyleVar(3);
		ImGui::PopItemWidth();

		return value_changed;
	}

	bool mcombo_clr(const char* label, bool* v,
		float clr[][4], const char* items[], int items_count, float width)
	{
		const bool has_shit = strstr(label, "##") != nullptr;

		ImGui::PushItemWidth(width);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

		ImGui::PushStyleColor(ImGuiCol_WindowShadow, 0);

		if (!has_shit)
			ImGui::Spacing();

		std::string preview = "";
		ImVec2 preview_size = ImVec2(0.0f, 0.0f);

		for (int i = 0; i < items_count; i++)
		{
			if (v[i])
			{
				preview_size = ImGui::CalcTextSize(preview.c_str());

				if (preview.empty())
					preview.assign(items[i]);
				else if (preview_size.x >= 90.0f && preview.find(", ...") == std::string::npos)
					preview.append(", ...");
				else if (preview.find(", ...") == std::string::npos)
					preview.append(", ").append(items[i]);
			}
		}

		if (preview.empty())
			preview.assign("None");

		bool value_changed = false;

		float expected_h = 0.0f;
		bool close = false;

		if (ImGui::BeginCombo(label, preview.c_str(),
			ImGuiComboFlags_ColorPicker, &expected_h, &close))
		{
			for (int i = 0; i < items_count; i++)
			{
				if (selectable_color(items[i], v[i], clr[i],
					ImGuiSelectableFlags_DontClosePopups, ImVec2(0.0f, 0.0f), expected_h, close,i,&v[i]))
				{
					v[i] = !v[i];

					value_changed = true;
				}
			}

			ImGui::EndCombo();
		}

		if (!has_shit)
			ImGui::Spacing();

		ImGui::PopStyleColor();

		ImGui::PopStyleVar(3);
		ImGui::PopItemWidth();

		return value_changed;
	}

	bool mcombo_clr3(const char* label, bool* v,
		float clr[][3], const char* items[], int items_count, float width)
	{
		const bool has_shit = strstr(label, "##") != nullptr;

		ImGui::PushItemWidth(width);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 5.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

		ImGui::PushStyleColor(ImGuiCol_WindowShadow, 0);

		if (!has_shit)
			ImGui::Spacing();

		std::string preview = "";
		ImVec2 preview_size = ImVec2(0.0f, 0.0f);

		for (int i = 0; i < items_count; i++)
		{
			if (v[i])
			{
				preview_size = ImGui::CalcTextSize(preview.c_str());

				if (preview.empty())
					preview.assign(items[i]);
				else if (preview_size.x >= 90.0f && preview.find(", ...") == std::string::npos)
					preview.append(", ...");
				else if (preview.find(", ...") == std::string::npos)
					preview.append(", ").append(items[i]);
			}
		}

		if (preview.empty())
			preview.assign("None");

		bool value_changed = false;

		float expected_h = 0.0f;
		bool close = false;

		if (ImGui::BeginCombo(label, preview.c_str(),
			ImGuiComboFlags_ColorPicker, &expected_h, &close))
		{
			for (int i = 0; i < items_count; i++)
			{
				if (selectable_color3(items[i], v[i], clr[i],
					ImGuiSelectableFlags_DontClosePopups, ImVec2(0.0f, 0.0f), expected_h, close, i, &v[i]))
				{
					v[i] = !v[i];

					value_changed = true;
				}
			}

			ImGui::EndCombo();
		}

		if (!has_shit)
			ImGui::Spacing();

		ImGui::PopStyleColor();

		ImGui::PopStyleVar(3);
		ImGui::PopItemWidth();

		return value_changed;
	}
}
