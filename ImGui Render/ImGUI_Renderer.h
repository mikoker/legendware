#pragma once
#include "../gui/core/core.h"


class Renderer_ImGui
{
public:
	ImU32 GetU32(Color _color);




	ImDrawList* draw_list_act;
	ImDrawList* draw_list_rendering;
	ImDrawList* draw_list;

	bool init = false;

	Renderer_ImGui();
	~Renderer_ImGui();

	void Initialize();

	ImDrawList* Render();


	void BeginScene();

	float RenderText(const std::string& text, ImVec2 position, float size, Color color, bool center = false, ImFont* font = core::f::esp_font, bool outline = true, bool center_y = false);

	void RenderCircle3D(Vector position, int points, float radius, Color color);
	void RenderCircle3DFilled(Vector position, int points, float radius, Color color);
	void RenderCircle3DShadow(Vector position, int points, float radius, Color color, float thickness);

	void RenderImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 1), ImU32 col = 0xFFFFFFFF) {
		draw_list->AddImage(user_texture_id, a, b, uv_a, uv_b, col);
	}

	template <class T>
	inline void RenderBoxByType(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, int type = 0) {
		if (type == 0)
			RenderBox(x1, y1, x2, y2, color, thickness);
		else if (type == 1)
			RenderCoalBox(x1, y1, x2, y2, color);
		else if (type == 2)
			RenderBox(x1, y1, x2, y2, color, thickness, 8.f);
	}

	template <class T>
	inline void RenderBoxFilledByType(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, int type = 0) {
		if (type == 0 || type == 1)
			RenderBoxFilled(x1, y1, x2, y2, color, thickness);
		else if (type == 2)
			RenderBoxFilled(x1, y1, x2, y2, color, thickness, 8.f);
	}

	template <class T>
	inline void RenderCoalBox(T x1, T y1, T x2, T y2, Color color, float th = 1.f) {
		int w = x2 - x1;
		int h = y2 - y1;

		int iw = w / 4;
		int ih = h / 4;
		// top
		RenderLine(x1, y1, x1 + iw, y1, color, th);					// left
		RenderLine(x1 + w - iw, y1, x1 + w, y1, color, th);			// right
		RenderLine(x1, y1, x1, y1 + ih, color, th);					// top left
		RenderLine(x1 + w - 1, y1, x1 + w - 1, y1 + ih, color, th);	// top right
																	// bottom
		RenderLine(x1, y1 + h, x1 + iw, y1 + h, color, th);			// left
		RenderLine(x1 + w - iw, y1 + h, x1 + w, y1 + h, color, th);	// right
		RenderLine(x1, y1 + h - ih, x1, y1 + h, color, th);			// bottom left
		RenderLine(x1 + w - 1, y1 + h - ih, x1 + w - 1, y1 + h, color, th);	// bottom right

	}

	template <class T>
	inline void RenderBox(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, float rounding = 0.f) {
		draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), rounding, ~0, thickness);
	}
	inline void RenderBox(RECT r, Color color, float thickness = 1.f, float rounding = 0.f) { //-V813
		RenderBox(r.left, r.top, r.right, r.bottom, color, thickness, rounding);
	}
	template <class T>
	inline void RenderBoxFilled(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, float rounding = 0.f) {
		draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), rounding);
	}
	template <class T>
	inline void RenderLine(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f) {
		draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), thickness);
	}
	template <class T>
	inline float RenderText(const std::string& text, T x, T y, float size, Color clr, bool center = false, bool outline = true, ImFont* pFont = core::f::esp_font) {
		return RenderText(text, ImVec2(x, y), size, clr, center, outline, pFont);
	}
	template <class T>
	inline void RenderCircle(T x, T y, float radius, int points, Color color, float thickness = 1.f) {
		draw_list->AddCircle(ImVec2(x, y), radius, GetU32(color), points, thickness);
	}
	template <class T>
	inline void RenderCircleFilled(T x, T y, float radius, int points, Color color) {
		draw_list->AddCircleFilled(ImVec2(x, y), radius, GetU32(color), points);
	}

	void RenderCircleProgress(const Vector& position, int radius, Color color, float procent);

	template <class T>
	inline void RenderTriangle(T x1, T y1, T x2, T y2, T x3, T y3, Color color, float thickness = 1.f) {
		draw_list->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), GetU32(color), thickness);
	}
	template <class T>
	inline void RenderTriangleFilled(T x1, T y1, T x2, T y2, T x3, T y3, Color color) {
		draw_list->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), GetU32(color));
	}
	inline ImVec2 SizeText(std::string text, float size, ImFont* font = core::f::esp_font) {
		return font->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
	}
	template <class T>
	inline void RenderShadowRect(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f) {
		draw_list->AddShadowRect(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), thickness);
	}
};

extern Renderer_ImGui renderer_imgui;