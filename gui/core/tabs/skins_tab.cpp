#include "skins.h"
#include "../imgui_ex.h"


#include <future>
#include <codecvt>
#include <string>
#include "../../../features/features.h"

static void draw_text(const char* label,
	bool extra_space = true)
{
	if (extra_space)
		ImGui::NewLine();

	ImGui::GetWindowDrawList()->AddText(core::f::dinpro_l, u::dpi(23.0f),
		imgui_ex::get_scrollable_pos(), ImGui::GetColorU32(ImGuiCol_Text), label);

	ImGui::PushStyleColor(ImGuiCol_Text, 0);
	ImGui::Text(core::f::dinpro, u::dpi(17.0f), crypt_str("%s"), label);
	ImGui::PopStyleColor();

	ImGui::NewLine();
	ImGui::Spacing();
}

bool findStringIC(std::string strHaystack, std::string strNeedle)
{
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wstrHaystack = converter.from_bytes(strHaystack);
	std::wstring wstrNeedle = converter.from_bytes(strNeedle);
	static auto locale = std::locale("");
	auto it = std::search(
		wstrHaystack.begin(), wstrHaystack.end(),
		wstrNeedle.begin(), wstrNeedle.end(),
		[](wchar_t ch1, wchar_t ch2) { return std::tolower(ch1, locale) == std::tolower(ch2, locale); }
	);
	return (it != wstrHaystack.end());
}

namespace core::tabs::skins
{
	void first_tab() {}
}