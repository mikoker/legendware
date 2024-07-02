// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "ImGUI_Renderer.h"
#include  <mutex>
#include "..\features\esp.h"
#include "..\features\world_esp.h"
#include "..\features\visuals.h"
#include "..\features\features.h"
#include "..\features\logs.h"

#include "..\features\sound_esp.h"
#include "..\Lua\Clua.h"
Renderer_ImGui renderer_imgui;

std::mutex render_mutex;
ImU32 Renderer_ImGui::GetU32(Color _color)
{
	return ImGui::ColorConvertFloat4ToU32(ImVec4(
		_color.r() / 255.0f,
		_color.g() / 255.0f,
		_color.b() / 255.0f,
		_color.a() / 255.0f
	));
}
Renderer_ImGui::Renderer_ImGui() //-V730 //-V832
{
}

Renderer_ImGui::~Renderer_ImGui()
{
	draw_list->_ClearFreeMemory();
	draw_list_act->_ClearFreeMemory();
	draw_list_rendering->_ClearFreeMemory();
}

void Renderer_ImGui::Initialize()
{

}

ImDrawList* Renderer_ImGui::Render()
{


	if (render_mutex.try_lock()) {
		*draw_list_rendering = *draw_list_act; //-V1002
		render_mutex.unlock();
	}

	ImGui::GetStyle().AntiAliasedLines = false;
	return draw_list_rendering;
}

void Renderer_ImGui::BeginScene()
{
	if (!init)
		return;

	draw_list->_ResetForNewFrame();
	draw_list->PushClipRectFullScreen();
	draw_list->PushTextureID(ImGui::GetIO().Fonts->TexID);



	world_esp->run();
	esp->run();
	visuals->run();
	sound_esp->run();
	logs->draw();


	/*if (ctx->loaded_script)
	{
		auto& states = Lua_manager->states;

		for (auto& state : states )
		{
			for (auto& current : state.getHooks(crypt_str("on_paint"))) {
				sol::protected_function_result result = current.func();
				if (!result.valid()) {

					sol::error error = result;
					auto log = crypt_str("Lua error: ") + (std::string)error.what();

					logs->add(log, Color::Red, crypt_str("[ LUA ] "), true);
					Lua_manager->unload_script(state.id);
				}
			}
		}
	}*/


	render_mutex.lock();
	*draw_list_act = *draw_list; //-V1002
	render_mutex.unlock();
}

float Renderer_ImGui::RenderText(const std::string& text, ImVec2 pos, float size, Color color, bool center, ImFont* font, bool outline, bool center_y)
{
	ImVec2 textSize = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
	if (!font->ContainerAtlas) return 0.f;
	draw_list->PushTextureID(font->ContainerAtlas->TexID);

	if (center) 
		pos.x -= textSize.x / 2.0f;
	if (center_y)
		pos.y -= textSize.y / 2.0f;
	

	if (outline) {
		draw_list->AddText(font, size, ImVec2(pos.x + 1, pos.y + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(font, size, ImVec2(pos.x - 1, pos.y - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(font, size, ImVec2(pos.x + 1, pos.y - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(font, size, ImVec2(pos.x - 1, pos.y + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
	}

	draw_list->AddText(font, size, pos, GetU32(color), text.c_str());
	draw_list->PopTextureID();

	return pos.y + textSize.y;
}

void Renderer_ImGui::RenderCircle3D(Vector position, int points, float radius, Color color) //-V813
{
	Vector position2d;
	std::vector<Vector> points_;

	for (std::size_t i = 0; i < points; ++i) {
		auto flameRadius = radius;
		float sin, cos;
		DirectX::XMScalarSinCosEst(&sin, &cos, DirectX::XMConvertToRadians(i * (360.0f / points)));
		points_.emplace_back(Vector{ flameRadius * cos,
		flameRadius * sin,
		0.0f });
	}
	std::vector< ImVec2> screenPoints;

	std::size_t count = 0;

	for (const auto& point : points_) {

		Vector temp;
		ImVec2 temp2;
		if (render->world_to_screen(position + point, temp)) {
			temp2.x = temp.x;
			temp2.y = temp.y;
			screenPoints.emplace_back(ImVec2(temp2));
			++count;
		}
	}

	if (count) {
		std::swap(screenPoints[0], *std::min_element(screenPoints.begin(), screenPoints.begin() + count, [](const auto& a, const auto& b) { return a.y < b.y || (a.y == b.y && a.x < b.x); }));

		constexpr auto orientation = [](const ImVec2& a, const ImVec2& b, const ImVec2& c) {
			return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
		};
		std::sort(screenPoints.begin() + 1, screenPoints.begin() + count, [&](const auto& a, const auto& b) { return orientation(screenPoints[0], a, b) > 0.0f; });

		renderer_imgui.draw_list->AddPolyline(screenPoints.data(), count, this->GetU32(color), true, 3);

	}

}

void Renderer_ImGui::RenderCircle3DFilled(Vector position, int points, float radius, Color color)
{
	Vector position2d;
	std::vector<Vector> points_;

	for (std::size_t i = 0; i < points; ++i) {
		auto flameRadius = radius;
		float sin, cos;
		DirectX::XMScalarSinCosEst(&sin, &cos, DirectX::XMConvertToRadians(i * (360.0f / points)));
		points_.emplace_back(Vector{ flameRadius * cos,
		flameRadius * sin,
		0.0f });
	}
	std::vector< ImVec2> screenPoints;

	std::size_t count = 0;

	for (const auto& point : points_) {

		Vector temp;
		ImVec2 temp2;
		if (render->world_to_screen(position + point, temp)) {
			temp2.x = temp.x;
			temp2.y = temp.y;
			screenPoints.emplace_back(ImVec2(temp2));
			++count;
		}
	}

	if (count) {
		std::swap(screenPoints[0], *std::min_element(screenPoints.begin(), screenPoints.begin() + count, [](const auto& a, const auto& b) { return a.y < b.y || (a.y == b.y && a.x < b.x); }));

		constexpr auto orientation = [](const ImVec2& a, const ImVec2& b, const ImVec2& c) {
			return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
		};
		std::sort(screenPoints.begin() + 1, screenPoints.begin() + count, [&](const auto& a, const auto& b) { return orientation(screenPoints[0], a, b) > 0.0f; });

		renderer_imgui.draw_list->AddConvexPolyFilled(screenPoints.data(), count, this->GetU32(color));

	}
}

void Renderer_ImGui::RenderCircle3DShadow(Vector position, int points, float radius, Color color, float thickness)
{
	Vector position2d;
	std::vector<Vector> points_;

	for (std::size_t i = 0; i < points; ++i) {
		auto flameRadius = radius;
		float sin, cos;
		DirectX::XMScalarSinCosEst(&sin, &cos, DirectX::XMConvertToRadians(i * (360.0f / points)));
		points_.emplace_back(Vector{ flameRadius * cos,
		flameRadius * sin,
		0.0f });
	}
	std::vector< ImVec2> screenPoints;

	std::size_t count = 0;

	for (const auto& point : points_) {

		Vector temp;
		ImVec2 temp2;
		if (render->world_to_screen(position + point, temp)) {
			temp2.x = temp.x;
			temp2.y = temp.y;
			screenPoints.emplace_back(ImVec2(temp2));
			++count;
		}
	}

	if (count) {
		std::swap(screenPoints[0], *std::min_element(screenPoints.begin(), screenPoints.begin() + count, [](const auto& a, const auto& b) { return a.y < b.y || (a.y == b.y && a.x < b.x); }));

		constexpr auto orientation = [](const ImVec2& a, const ImVec2& b, const ImVec2& c) {
			return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
		};
		std::sort(screenPoints.begin() + 1, screenPoints.begin() + count, [&](const auto& a, const auto& b) { return orientation(screenPoints[0], a, b) > 0.0f; });

		renderer_imgui.draw_list->AddShadowConvexPoly(screenPoints.data(), count, this->GetU32(color), thickness);
	}
}

void Renderer_ImGui::RenderCircleProgress(const Vector& position, int radius, Color color, float procent)
{

	float wheel_thickness = radius * 0.05f;
	float wheel_r_outer = radius * 0.50f;
	float wheel_r_inner = wheel_r_outer - wheel_thickness;
	const float aeps = 0.5f / wheel_r_outer; // Half a pixel arc length in radians (2pi cancels out).
	const int segment_per_arc = ImMax(4, (int)wheel_r_outer / 360);
	ImVec2 pos = ImVec2(position.x, position.y);
	int progress = procent * 180 - 45;
	for (int n = -45; n < progress; n++) // 180
	{
		const float a0 = (n) / 180.0f * 2.0f * IM_PI - aeps;
		const float a1 = (n + 1.0f) / 180.0f * 2.0f * IM_PI + aeps;
		draw_list->PathArcTo(pos, (wheel_r_inner + wheel_r_outer) * 0.5f, a0, a1, segment_per_arc);
		draw_list->PathStroke(ImGui::ColorConvertFloat4ToU32(ImVec4(color.r() / 255.0f, color.g() / 255.0f, color.b() / 255.0f, color.a() / 255.0f)), false, wheel_thickness);
	}
}