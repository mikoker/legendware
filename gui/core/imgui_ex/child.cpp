// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "child.h"

namespace imgui_ex
{
	void begin_child(const char* label, const ImVec2& size,
		ImGuiWindowFlags flags, ImVec2 offset)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(ImGuiCol_FrameBg));

		ImGui::BeginChild(label, size, false, flags | ImGuiWindowFlags_DrawShadow);

		ImGui::SetCursorPos(u::dpi(ImVec2(23.0f, 23.0f) + offset));

		ImGui::BeginGroup();
	}

	void end_child()
	{
		ImGui::EndGroup();

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		ImGui::EndChild();
	}
}
