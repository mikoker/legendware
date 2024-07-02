#pragma once
#include "../core.h"

namespace imgui_ex
{
	void draw_bar_element(const ImVec2& pos, const char* text,
		const char* icon, int& tab, int tab_num, ImDrawList* draw_list, const ImVec2& win_pos);

	void draw_triangle(ImDrawList* draw_list,
		ImVec2 pos1, ImVec2 pos2, ImVec2 pos3);

	void draw_text(ImDrawList* draw_list, const char* text, const ImVec2& pos,
		ImFont* font = core::f::dinpro_bold, float font_size = 16.0f, const char* text_end = nullptr);
}
