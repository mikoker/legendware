// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "renderer.h"
#include "..\render.h"
#include "..\features\esp.h"
#include "..\sdk\math.h"

Render::Render(crypt_ptr <IDirect3DDevice9> device) //-V818 //-V730
{
	this->device = device;
}

Render::~Render()
{
	device = nullptr;
}

void Render::begin_draw()
{
	if (!device)
		return;

	device->GetVertexDeclaration(&vert_dec);
	device->GetVertexShader(&vert_shader);
}

void Render::end_draw()
{
	if (!device)
		return;

	device->SetVertexDeclaration(vert_dec);
	device->SetVertexShader(vert_shader);
}