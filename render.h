#pragma once

#include "includes.h"
#include "directx_renderer\renderer.h"
#include "data.h"
#include "Gui\core\core.h"

enum
{
	FONT_NAME,
	FONT_ESP,
	FONT_ICONS,
	FONT_MAX
};

class Renderer
{
	Render renderer;
public:
	int width;
	int height;

	Renderer();
	void update(crypt_ptr <IDirect3DDevice9> device);
	crypt_ptr <Render> operator->();
};


enum FontCenteringFlags
{
	HFONT_CENTERED_NONE = (1 << 0),
	HFONT_CENTERED_X = (1 << 1),
	HFONT_CENTERED_Y = (1 << 2)
};

enum GradientType
{
	GRADIENT_HORIZONTAL,
	GRADIENT_VERTICAL
};

template<typename T>
class singleton;

struct CUSTOMVERTEX2 {
	FLOAT x, y, z;
	FLOAT rhw;
	DWORD color;
};

class SurfaceRender {
	float alpha_factor = 1.0f;
public:
	float get_alphafactor();
	void set_alphafactor(float factor);
	void rect(int x, int y, int w, int h, Color color);
	void rect_filled(int x, int y, int w, int h, Color color);
	void rounded_box(int x, int y, int w, int h, int points, int radius, Color color);
	void circle(int x, int y, int points, int radius, Color color);
	void circle_filled(int x, int y, int points, int radius, Color color);
	void triangle(Vector2D point_one, Vector2D point_two, Vector2D point_three, Color color);
	void line(int x, int y, int x2, int y2, Color color);
	void text(vgui::HFont font, int x, int y, Color color, DWORD flags, const char* msg, ...);
	void wtext(vgui::HFont font, int x, int y, Color color, DWORD flags, wchar_t* msg, ...);
	int text_width(vgui::HFont font, const char* msg, ...);
};

extern SurfaceRender surface_render;
extern Renderer render;