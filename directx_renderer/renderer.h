#pragma once

#include "..\includes.h"
#include "..\sdk\classes.h"
#include "color.hpp"
#include "font.hpp"

enum RenderFlags
{
	RenderFlags_None = 0 << 0,
	RenderFlags_Outlined = 1 << 0,
	RenderFlags_Filled = 1 << 1,
	RenderFlags_Gradient = 1 << 2,
	RenderFlags_OutlinedGradient = RenderFlags_Outlined | RenderFlags_Gradient,
	RenderFlags_FilledGradient = RenderFlags_Filled | RenderFlags_Gradient
};

struct RenderState
{
	D3DRENDERSTATETYPE state;
	DWORD value;

	RenderState(D3DRENDERSTATETYPE state, DWORD value)
	{
		this->state = state;
		this->value = value;
	}
};

struct Vertex
{
	float x;
	float y;
	float z;
	float rhw;
	color_t color;

	Vertex()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		rhw = 1.0f;
		color = 0;
	}

	Vertex(float x, float y, color_t color)
	{
		this->x = x;
		this->y = y;
		this->color = color.color;

		z = 0.0f;
		rhw = 1.0f;
	}
};

struct SinCos
{
	float sin = 0.0f;
	float cos = 0.0f;
};

typedef float matrix4x4[4][4];

class Render
{
	DWORD colorwrite, srgbwrite;
	IDirect3DVertexDeclaration9* vert_dec = nullptr;
	IDirect3DVertexShader9* vert_shader = nullptr;
	DWORD dwOld_D3DRS_COLORWRITEENABLE = 0;
public:
	crypt_ptr <IDirect3DDevice9> device;

	Render(crypt_ptr <IDirect3DDevice9> device = nullptr);
	~Render();

	bool world_to_screen(const Vector& origin, Vector& screen)
	{
		static uintptr_t view_matrix = 0;

		if (!view_matrix)
			view_matrix = signatures_manager->signatures[SIGNATURE_VIEW_MATRIX];

		auto& matrix = *(matrix4x4*)(view_matrix);

		if (!matrix) //-V600
			return false;

		int width, height;
		engine->GetScreenSize(width, height);

		auto matrix_width = matrix[3][0] * origin[0] + matrix[3][1] * origin[1] + matrix[3][2] * origin[2] + matrix[3][3];

		if (matrix_width > 0.01f)
		{
			auto inverse_width = 1.0f / matrix_width;

			screen.x = width / 2.0f + (0.5f * (matrix[0][0] * origin[0] + matrix[0][1] * origin[1] + matrix[0][2] * origin[2] + matrix[0][3]) * inverse_width * width + 0.5f);
			screen.y = height / 2.0f - (0.5f * (matrix[1][0] * origin[0] + matrix[1][1] * origin[1] + matrix[1][2] * origin[2] + matrix[1][3]) * inverse_width * height + 0.5f);
			screen.z = 0.0f;

			return true;
		}

		return false;
	}

	void begin_draw();
	void end_draw();
};