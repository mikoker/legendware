#pragma once
#include "../core.h"

namespace imgui_ex
{
	void begin_child(const char* label, const ImVec2& size,
		ImGuiWindowFlags flags = 0, ImVec2 offset = ImVec2(0.0f, 0.0f));

	void end_child();
}
