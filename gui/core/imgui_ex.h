#pragma once

#include "imgui_ex\buttons.h"
#include "imgui_ex\child.h"
#include "imgui_ex\color_pickers.h"
#include "imgui_ex\combos.h"
#include "imgui_ex\draw_elements.h"
#include "imgui_ex\others.h"
#include "imgui_ex\selectables.h"
#include "imgui_ex\sliders.h"
#include "imgui_ex\tabs.h"
#include "..\..\config\config.h"

namespace imgui_ex
{
	inline ImVec2 get_scrollable_pos()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		return window->DC.CursorPos;
	}
}
