#pragma once
#include "../core.h"

namespace imgui_ex
{
	bool tab(const char* label, const ImVec2& size_arg,
		int& tab, int tab_num);

	bool subtab(const char* label, const ImVec2& size_arg, int& tab, int tab_num,
		ImFont* font = core::f::dinpro, float font_size = 17.0f, ImVec2 offset = ImVec2(0.0f, 0.0f));

	void rage_subtab(const char* label,
		const ImVec2& size_arg, int& tab);

	void rage_subtab(const char* label, int& tab);
}
