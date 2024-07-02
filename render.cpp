// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "render.h"
#include "config\config.h"

Renderer render;
SurfaceRender surface_render;

void SurfaceRender::set_alphafactor(float factor)
{
	alpha_factor = factor;
}

float SurfaceRender::get_alphafactor()
{
	return alpha_factor;
}

void SurfaceRender::rect(int x, int y, int w, int h, Color color)
{
	if (!surface)
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	surface->DrawSetColor(color);
	surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void SurfaceRender::rect_filled(int x, int y, int w, int h, Color color)
{
	if (!surface)
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	surface->DrawSetColor(color);
	surface->DrawFilledRect(x, y, x + w, y + h);
}

void SurfaceRender::rounded_box(int x, int y, int w, int h, int points, int radius, Color color)
{
	if (!surface)
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	Vertex_t* round = new Vertex_t[4 * points]; //-V121

	for (int i = 0; i < 4; i++) {
		int _x = x + ((i < 2) ? (w - radius) : radius);
		int _y = y + ((i % 3) ? (h - radius) : radius);

		float a = 90.f * i;

		for (int j = 0; j < points; j++) {
			float _a = DirectX::XMConvertToRadians(a + (j / (float)(points - 1)) * 90.f);

			round[(i * points) + j] = Vertex_t(Vector2D(_x + radius * sin(_a), _y - radius * cos(_a)));
		}
	}

	static int Texture = surface->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	surface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	surface->DrawSetColor(color);
	surface->DrawSetTexture(Texture);

	surface->DrawTexturedPolygon(4 * points, round);
}

void SurfaceRender::circle(int x, int y, int points, int radius, Color color)
{
	if (!surface)
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	static bool once = true;

	static std::vector<float> temppointsx;
	static std::vector<float> temppointsy;

	if (once)
	{
		float step = (float)DirectX::XM_PI * 2.0f / points;
		for (float a = 0; a < (DirectX::XM_PI * 2.0f); a += step) //-V1034
		{
			temppointsx.push_back(cosf(a));
			temppointsy.push_back(sinf(a));
		}
		once = false;
	}

	std::vector<int> pointsx;
	std::vector<int> pointsy;
	std::vector<Vertex_t> vertices;

	for (int i = 0; i < temppointsx.size(); i++)
	{
		float eeks = radius * temppointsx[i] + x;
		float why = radius * temppointsy[i] + y;
		pointsx.push_back(eeks);
		pointsy.push_back(why);

		vertices.emplace_back(Vertex_t(Vector2D(eeks, why)));
	}

	surface->DrawSetColor(color);
	surface->DrawPolyLine(pointsx.data(), pointsy.data(), points); // only if you want en extra outline
}

void SurfaceRender::circle_filled(int x, int y, int points, int radius, Color color)
{
	if (!surface)
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	static bool once = true;

	static std::vector<float> temppointsx;
	static std::vector<float> temppointsy;

	if (once)
	{
		float step = (float)DirectX::XM_PI * 2.0f / points;
		for (float a = 0; a < (DirectX::XM_PI * 2.0f); a += step) //-V1034
		{
			temppointsx.push_back(cosf(a));
			temppointsy.push_back(sinf(a));
		}
		once = false;
	}

	std::vector<int> pointsx;
	std::vector<int> pointsy;
	std::vector<Vertex_t> vertices;

	for (int i = 0; i < temppointsx.size(); i++)
	{
		float eeks = radius * temppointsx[i] + x;
		float why = radius * temppointsy[i] + y;
		pointsx.push_back(eeks);
		pointsy.push_back(why);

		vertices.emplace_back(Vertex_t(Vector2D(eeks, why)));
	}

	surface->DrawSetColor(color);
	surface->DrawTexturedPolygon(points, vertices.data());
}

void SurfaceRender::triangle(Vector2D point_one, Vector2D point_two, Vector2D point_three, Color color)
{
	if (!surface)
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	Vertex_t verts[3] = {
		Vertex_t(point_one),
		Vertex_t(point_two),
		Vertex_t(point_three)
	};

	static int texture = surface->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	surface->DrawSetTextureRGBA(texture, buffer, 1, 1);
	surface->DrawSetColor(color);
	surface->DrawSetTexture(texture);

	surface->DrawTexturedPolygon(3, verts);
}

void SurfaceRender::line(int x, int y, int x2, int y2, Color color)
{
	if (!surface)
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	surface->DrawSetColor(color);
	surface->DrawLine(x, y, x2, y2);
}

void SurfaceRender::text(vgui::HFont font, int x, int y, Color color, DWORD flags, const char* msg, ...)
{
	if (!surface)
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	va_list va_alist;
	char buffer[1024];
	va_start(va_alist, msg);
	_vsnprintf(buffer, sizeof(buffer), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];

	MultiByteToWideChar(CP_UTF8, 0, buffer, 256, wbuf, 256);

	int width, height;
	surface->GetTextSize(font, wbuf, width, height);

	if (!(flags & HFONT_CENTERED_NONE))
	{
		if (flags & HFONT_CENTERED_X)
			x -= width * 0.5f;

		if (flags & HFONT_CENTERED_Y)
			y -= height * 0.5f;
	}

	surface->DrawSetTextFont(font);
	surface->DrawSetTextColor(color);
	surface->DrawSetTextPos(x, y);
	surface->DrawPrintText(wbuf, wcslen(wbuf));
}

void SurfaceRender::wtext(vgui::HFont font, int x, int y, Color color, DWORD flags, wchar_t* msg, ...)
{
	if (!surface)
		return;

	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	wchar_t buffer[256];
	va_list args;

	va_start(args, msg);
	wvsprintfW(buffer, msg, args);
	va_end(args);

	int width, height;
	surface->GetTextSize(font, buffer, width, height);

	if (!(flags & HFONT_CENTERED_NONE)) {
		if (flags & HFONT_CENTERED_X)
			x -= width * 0.5f;

		if (flags & HFONT_CENTERED_Y)
			y -= height * 0.5f;
	}

	surface->DrawSetTextFont(font);
	surface->DrawSetTextColor(color);
	surface->DrawSetTextPos(x, y);
	surface->DrawPrintText(buffer, wcslen(buffer));
}

int SurfaceRender::text_width(vgui::HFont font, const char* msg, ...)
{
	if (!surface)
		return 0.0f;

	va_list va_alist;
	char buffer[1024];
	va_start(va_alist, msg);
	_vsnprintf(buffer, sizeof(buffer), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];

	MultiByteToWideChar(CP_UTF8, 0, buffer, 256, wbuf, 256);

	int width, height;
	surface->GetTextSize(font, wbuf, width, height);

	return width;
}

Renderer::Renderer()
{
	width = 0;
	height = 0;
}

crypt_ptr <Render> Renderer::operator->()
{
	return &renderer;
}

void Renderer::update(crypt_ptr <IDirect3DDevice9> device)
{
	renderer.device = device.get();
	engine->GetScreenSize(width, height);
}