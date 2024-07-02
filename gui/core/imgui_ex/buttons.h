#pragma once
#include "../core.h"

namespace imgui_ex
{
	bool button(const char* label,
		const ImVec2& size_arg, bool centered = false, float offset_x = 0.0f);

	int dropdown_button(const char* label,
		const ImVec2& size_arg, std::vector<std::string> labels , bool centered = false);

	void button_disabled(const char* label,
		const ImVec2& size_arg, bool centered = false);

	bool button_colored(const char* label,
		const ImVec2& size_arg, ImColor color, ImColor color_hovered, ImColor color_active, bool centered = false
	);

	bool button_lua(const char* label,
		const ImVec2& size_arg, bool is_red = true);

	bool button_lock(const char* label, const ImVec2& size_arg,
		bool& is_locked, bool centered = false);
}
