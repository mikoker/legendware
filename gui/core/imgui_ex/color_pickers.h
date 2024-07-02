#pragma once
#include "../core.h"

namespace imgui_ex
{
	bool color_button(const char* desc_id, const ImVec4& col,
		ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0.0f, 0.0f));

	bool color_picker4(const char* label, float col[4],
		ImGuiColorEditFlags flags = 0, float alpha = 255.0f);

	bool color_picker3(const char* label, float col[3],
		ImGuiColorEditFlags flags = 0);

	bool color_edit4(const char* label, float col[4],
		bool offset_y = false, bool is_in_selecteable = false, int id = -1, float offset = 0);
	bool color_edit4(const char* label, Color* col,
		bool offset_y = false, bool is_in_selecteable = false);

	bool color_edit3(const char* label, float col[3],
		bool offset_y = false, bool is_in_selecteable = false);
}
