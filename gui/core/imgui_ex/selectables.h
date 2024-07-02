#pragma once
#include "../core.h"

namespace imgui_ex
{
	bool selectable(const char* label, bool selected = false,
		ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = ImVec2(0.0f, 0.0f), float expected_h = 0.0f, int item_id = -1, void* bool_ptr = nullptr);
	bool selectable_color_clr(const char* label, bool selected, Color* clr, ImGuiSelectableFlags flags = 0,
		const ImVec2& size_arg = ImVec2(0.0f, 0.0f), float expected_h = 0.0f, bool close = false);
	bool selectable_color(const char* label, bool selected, float clr[4], ImGuiSelectableFlags flags = 0,
		const ImVec2& size_arg = ImVec2(0.0f, 0.0f), float expected_h = 0.0f, bool close = false, int id_clr = -1, void* ptr_bool = nullptr);
	bool selectable_color3(const char* label, bool selected, float clr[3], ImGuiSelectableFlags flags = 0,
		const ImVec2& size_arg = ImVec2(0.0f, 0.0f), float expected_h = 0.0f, bool close = false, int id_clr = -1, void* ptr_bool = nullptr);

	bool selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0,
		const ImVec2& size_arg = ImVec2(0.0f, 0.0f));

	bool selectable_color(const char* label, bool* p_selected, float clr[4],
		ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = ImVec2(0.0f, 0.0f));

	bool selectable_preview(const char* label, bool selected = false,
		ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = ImVec2(0.0f, 0.0f), float expected_h = 0.0f);

	bool selectable_preview(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0,
		const ImVec2& size_arg = ImVec2(0.0f, 0.0f));
}
