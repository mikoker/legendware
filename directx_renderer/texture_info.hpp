#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "../gui/third-party/imgui/imgui.h"
#include "../gui/third-party/imgui/imgui_internal.h"
#include "../gui/third-party/imgui/impl/imgui_impl_dx9.h"
#include "../gui/third-party/imgui/impl/imgui_impl_win32.h"
class texture_info
{
public:
	static IDirect3DDevice9* device;
	static void for_each(void(*pred)(texture_info*));

	static void on_lost_device();
	static void on_reset_device();

	unsigned char* data = nullptr;
	size_t size = 0;
	ImVec2 resolution = ImVec2();
	IDirect3DTexture9* texture = nullptr;

	texture_info(
		unsigned char* data,
		size_t size,
		ImVec2 resolution
	);

	texture_info() { }
	~texture_info();

	void initialize();
	void release();
	void draw(ImVec2 pos, ImU32 col = ~0U) const;
};