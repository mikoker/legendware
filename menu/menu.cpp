// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "menu.h"
#include "..\hooks\hooks.h"
#include "..\config\config.h"

#include "..\ImGui Render\ImGUI_Renderer.h"
#include "..\features\features.h"
#include "..\features\prediction.h"
#include "..\features\spectator_list.h"
#include "..\gui\devkit\blur\CBlurProcessor.h"
#include "..\render.h"
#include "..\resource_manager.h"
#include "..\Lua\Clua.h"
#include <fstream>
#include "../gui/core/imgui_ex/lua_editor.h"
#include <stack>
void DrawBackgroundBlur(ImDrawList* pDrawList, IDirect3DDevice9* pDevice);
void DrawBackgroundBlurRounded(ImDrawList* pDrawList, IDirect3DDevice9* pDevice, ImVec2 min, ImVec2 max, float rounding, ImDrawCornerFlags rounding_corners = 15);
crypt_ptr <Menu> menu;

using namespace core;

const std::string g_window_name = crypt_str("lw-menu");

const int g_width = 1380, g_height = 880;
ImVec4 g_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

static void initialize_textures(
	IDirect3DDevice9* d3d_device)
{
	if (!i::logo)
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_logo2"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 385, 385,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::logo);
	}

	if (!i::small_logo)
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_small_logo"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 60, 60,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::small_logo);
	}

	if (!i::hitboxes[0]) // head
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_head"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes[0]);
	}

	if (!i::hitboxes[1]) // body
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_body"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes[1]);
	}

	if (!i::hitboxes[2]) // pelvis
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_pelvis"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes[2]);
	}

	if (!i::hitboxes[3]) // legs
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_legs"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes[3]);
	}

	if (!i::hitboxes[4]) // feet
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_feet"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes[4]);
	}

	if (!i::hitboxes[5]) // arms
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_arms"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes[5]);
	}

	if (!i::hitboxes_ph[0]) // head placeholder
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_head_ph"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes_ph[0]);
	}

	if (!i::hitboxes_ph[1]) // body placeholder
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_body_ph"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes_ph[1]);
	}

	if (!i::hitboxes_ph[2]) // pelvis placeholder
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_pelvis_ph"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes_ph[2]);
	}

	if (!i::hitboxes_ph[3]) // legs placeholder
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_legs_ph"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes_ph[3]);
	}

	if (!i::hitboxes_ph[4]) // feet placeholder
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_feet_ph"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes_ph[4]);
	}

	if (!i::hitboxes_ph[5]) // arms placeholder
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_hitbox_arms_ph"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 183, 330,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::hitboxes_ph[5]);
	}

	if (!i::pfp)
	{
		auto img_data = resources->get_data(ctx->avatar);
		D3DXCreateTextureFromFileInMemory(d3d_device, img_data.buffer, img_data.size, &i::pfp);
	}

	if (!i::large_logo)
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_large_logo"));
		D3DXCreateTextureFromFileInMemory(d3d_device, img_data.buffer, img_data.size, &i::large_logo);
	}

	if (!i::playerlist_ph)
	{
		auto img_data = resources->get_data(crypt_str("/resources/images/g_playerlist_ph"));
		D3DXCreateTextureFromFileInMemoryEx(d3d_device, img_data.buffer, img_data.size, 306, 617,
			D3DUSAGE_DYNAMIC, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &i::playerlist_ph);
	}
}

void Menu::initialize()
{
	while (true)
	{
		window = FindWindow(crypt_str("Valve001"), nullptr);

		if (window)
			break;
	}

	while (true)
	{
		old_window = (HWND)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)hooked_wndproc); //-V107 //-V221

		if (old_window)
			break;
	}
}

void Menu::initialize_gui(crypt_ptr <IDirect3DDevice9> device)
{
	if (initialized_imgui)
		return;

	HKEY theme_key = nullptr;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, crypt_str("SOFTWARE\\legendware"), 0, KEY_ALL_ACCESS, &theme_key) == ERROR_SUCCESS)
	{
		DWORD theme_size = 2;
		char theme[2];

		if (RegQueryValueEx(theme_key, crypt_str("theme"), nullptr, nullptr, (LPBYTE)&theme, &theme_size) == ERROR_SUCCESS)
			core::s::is_theme_dark = theme[0] == 'd';

		RegCloseKey(theme_key);
	}

	ImGui::CreateContext();

	ImGui_ImplWin32_Init(menu->window);
	ImGui_ImplDX9_Init(device.get());
	ImGui::StyleColorsLight();

	initialize_textures(device.get());

	renderer_imgui.draw_list = new ImDrawList(ImGui::GetDrawListSharedData());
	renderer_imgui.draw_list_act = new ImDrawList(ImGui::GetDrawListSharedData());
	renderer_imgui.draw_list_rendering = new ImDrawList(ImGui::GetDrawListSharedData());
	renderer_imgui.init = true;

	//lua_editor.init();
	initialized_imgui = true;


	if (RegOpenKeyEx(HKEY_CURRENT_USER, crypt_str("SOFTWARE\\legendware"), 0, KEY_ALL_ACCESS, &theme_key) == ERROR_SUCCESS)
	{
		DWORD scale_size = sizeof(s::dpi_scale);

		bool ok = false;
		if (RegQueryValueEx(theme_key, crypt_str("scale"), nullptr, nullptr, (LPBYTE)&s::dpi_scale, &scale_size) == ERROR_SUCCESS)
			ok = true;

		RegCloseKey(theme_key);
		s::temp_dpi_scale = s::dpi_scale;
		if (ok)
			return;
	}
	const HDC hDC = ::GetDC(HWND_DESKTOP);
	const int dpix = ::GetDeviceCaps(hDC, LOGPIXELSX) / 96.0f * 100;
	::ReleaseDC(HWND_DESKTOP, hDC);

	switch (dpix)
	{
	case 75:
		s::temp_dpi_scale = 1;
		s::dpi_scale = 1;
		break;
	case 100:
		s::temp_dpi_scale = 2;
		s::dpi_scale = 2;
		break;
	case 125:
		s::temp_dpi_scale = 3;
		s::dpi_scale = 3;
		break;
	case 150:
		s::temp_dpi_scale = 4;
		s::dpi_scale = 4;
		break;
	case 175:
		s::temp_dpi_scale = 5;
		s::dpi_scale = 5;
		break;
	case 200:
		s::temp_dpi_scale = 6;
		s::dpi_scale = 6;
		break;
	case 225:
		s::temp_dpi_scale = 7;
		s::dpi_scale = 7;
		break;
	case 250:
		s::temp_dpi_scale = 8;
		s::dpi_scale = 8;
		break;
	case 275:
		s::temp_dpi_scale = 9;
		s::dpi_scale = 9;
		break;
	case 300:
		s::temp_dpi_scale = 10;
		s::dpi_scale = 10;
		break;
	default:
		s::temp_dpi_scale = 2;
		s::dpi_scale = 2;
		break;
	}

}

#pragma optimize("", off)
void Menu::draw()
{

	if (!initialized_fonts)
	{
		if (!f::dinpro)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;

			const ImWchar ranges[] = { 0x0020, 0xffff };
			auto font_data = resources->get_data(crypt_str("/resources/fonts/g_dinpro_data"));
			f::dinpro = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_data.buffer,
				font_data.size, 17.0f, &fontcfg, ranges);

		}

		if (!f::dinpro_s)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;

			auto font_data = resources->get_data(crypt_str("/resources/fonts/g_dinpro_data"));
			f::dinpro_s = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_data.buffer,
				font_data.size, 12.0f, &fontcfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

		}

		if (!f::dinpro_l)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;

			auto font_data = resources->get_data(crypt_str("/resources/fonts/g_dinpro_data"));
			f::dinpro_l = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_data.buffer,
				font_data.size, 22.0f, &fontcfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
		}

		if (!f::awesome)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.MergeMode = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 1;

			const ImWchar icons_ranges[] = { 0xf000, 0xffff, 0 };

			auto font_data = resources->get_data(crypt_str("/resources/fonts/g_awesome_font_data"));
			f::awesome = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_data.buffer,
				font_data.size, 45.0f, &fontcfg, icons_ranges);

		}


		if (!f::dinpro_m)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;

			auto font_data = resources->get_data(crypt_str("/resources/fonts/g_dinpro_data"));
			f::dinpro_m = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_data.buffer,
				font_data.size, 15.0f, &fontcfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
		}

		if (!f::dinpro_bold)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;

			auto font_data = resources->get_data(crypt_str("/resources/fonts/g_dinpro_bold_data"));
			f::dinpro_bold = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_data.buffer,
				font_data.size, 17.0f, &fontcfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
		}

		if (!f::icons)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.MergeMode = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;

			const ImWchar icons_ranges[] = { 0xe900, 0xe911, 0 };

			auto font_data = resources->get_data(crypt_str("/resources/fonts/g_icons_data"));
			f::icons = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_data.buffer,
				font_data.size, 60.0f, &fontcfg, icons_ranges);


		}

		if (!f::weapon_icons)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;

			auto font_data = resources->get_data(crypt_str("/resources/fonts/g_weapon_icons_data"));
			f::weapon_icons = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_data.buffer,
				font_data.size, 20.0f, &fontcfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());


		}
		if (!f::icons_weapon)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;

			const ImWchar icons_ranges[] = { 0xe000, 0xe20b, 0 };


			auto font_data = resources->get_data(crypt_str("/resources/fonts/g_csgo_icons_compressed_data"));
			f::icons_weapon = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_data.buffer,
				font_data.size, 20.0f, &fontcfg, icons_ranges);
		}

		if (!f::esp_font)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;
			f::esp_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(crypt_str("C:\\Windows\\Fonts\\calibrib.ttf"), 16.0f, &fontcfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
		}

		if (!f::logs_font)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;
			f::logs_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(crypt_str("C:\\Windows\\Fonts\\lucon.ttf"), 10.0f, &fontcfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
		}
		if (!f::montserrat)
		{
			ImFontConfig fontcfg{};
			fontcfg.PixelSnapH = true;
			fontcfg.OversampleH = fontcfg.OversampleV = 4;

			auto font_data = resources->get_data(crypt_str("/resources/fonts/g_montserrat_data"));
			f::montserrat = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_data.buffer,
				font_data.size, 17.0f, &fontcfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());


		}
		initialized_fonts = true;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	imgui_ex::change_theme();

	ImGui::SetNextWindowSize(u::dpi(
		ImVec2(716.0f, 646.0f)));
	u::fade(!s::is_opened, s::menu_alpha, 0.0f, 255.0f, 7.0f);
	ImGui::GetStyle().Alpha = s::menu_alpha / 255.0f;

	/*if (lua_editor.is_open())
		lua_editor.draw();*/

	if (!config->load_config.empty() || config->load)
	{
		config->load = false;
		config->json_to_items(config->load_config);
		config->load_config.clear();
	}
	//DrawBackgroundBlurRounded(ImGui::GetForegroundDrawList(), render->device.get(), ImVec2(50, 50), ImVec2(500, 500), 50);

	ImGui::Begin(crypt_str("##main_menu"), &s::is_opened, s::main_focus_flags);
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 pos = ImGui::GetWindowPos();
		ImGuiStyle* style = &ImGui::GetStyle();

		ImU32 shadow_color = IM_COL32(0, 0, 0, 100);

		if (s::is_taking_pixel)
		{
			ImGui::GetForegroundDrawList()->AddCircle(ImGui::GetMousePos(), 6.0f,
				IM_COL32(10, 10, 10, 100), 32, 2.0f);

			ImGui::GetForegroundDrawList()->AddCircle(ImGui::GetMousePos(), 7.0f,
				IM_COL32_WHITE, 32, 2.0f);

			ImGui::GetForegroundDrawList()->AddCircle(ImGui::GetMousePos(), 8.0f,
				IM_COL32(10, 10, 10, 100), 32, 2.0f);
		}

		if (s::temp_dpi_scale != s::dpi_scale && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) { //-V550
			s::temp_dpi_scale = s::dpi_scale;
			HKEY theme_key = nullptr;
			DWORD scale_size = sizeof(s::dpi_scale);

			if (RegOpenKeyEx(HKEY_CURRENT_USER, crypt_str("SOFTWARE\\legendware"), 0, KEY_ALL_ACCESS, &theme_key))
				RegCreateKeyEx(HKEY_CURRENT_USER, crypt_str("SOFTWARE\\legendware"), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &theme_key, nullptr);

			RegSetValueEx(theme_key, crypt_str("scale"), 0, REG_DWORD, (BYTE*)&s::dpi_scale, scale_size);
			RegCloseKey(theme_key);
		}

		draw_list->AddShadowRect(pos + ImVec2(u::dpi(10.0f), u::dpi(10.0f)), pos + ImVec2(u::dpi(185.0f),
			u::dpi(646.0f)), shadow_color, 25.0f, ImVec2(0.0f, 0.0f), ImDrawShadowFlags_None, 10.0f);


		ImGui::BeginChild(crypt_str("##menubar"), ImVec2(u::dpi(185.0f),
			u::dpi(646.0f)), false, s::main_flags | ImGuiWindowFlags_RoundLeft);
		{
			imgui_ex::draw_triangle(draw_list, ImVec2(85.0f, 442.0f),
				ImVec2(160.0f, 422.0f), ImVec2(135.0f, 497.0f));

			imgui_ex::draw_triangle(draw_list, ImVec2(25.0f, 367.0f),
				ImVec2(58.0f, 335.0f), ImVec2(68.0f, 380.0f));

			imgui_ex::draw_triangle(draw_list, ImVec2(87.0f, 265.0f),
				ImVec2(175.0f, 250.0f), ImVec2(140.0f, 330.0f));

			imgui_ex::draw_triangle(draw_list, ImVec2(37.0f, 220.0f),
				ImVec2(30.0f, 160.0f), ImVec2(85.0f, 180.0f));

			imgui_ex::draw_triangle(draw_list, ImVec2(105.0f, 150.0f),
				ImVec2(117.0f, 67.0f), ImVec2(183.0f, 115.0f));

			imgui_ex::draw_triangle(draw_list, ImVec2(58.0f, 90.0f),
				ImVec2(3.0f, 50.0f), ImVec2(63.0f, 27.0f));

			draw_list->AddImageRounded(i::logo, pos + u::dpi(ImVec2(0.0f, 261.0f)), pos + u::dpi(ImVec2(385.0f, 646.0f)),
				ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::GetColorU32(ImGuiCol_Logo, ImGui::GetStyle().Alpha), 5.0f);

			static std::string user_name = ctx->username,
				days_left = ctx->days;

			ImVec2 logo_size = ImGui::CalcTextSize(f::dinpro_l, u::dpi(22.0f), crypt_str("legendware")),
				user_name_size = ImGui::CalcTextSize(f::dinpro_bold, u::dpi(15.0f), user_name.c_str());

			if (user_name_size.x >= u::dpi(90.0f))
			{
				user_name = user_name.substr(0, 10);
				user_name += crypt_str("...");
			}

			draw_list->AddImage(i::large_logo, pos + u::dpi(ImVec2(24, 38)), pos + u::dpi(ImVec2(163, 68)),
				ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::GetColorU32(ImGuiCol_Text, ImGui::GetStyle().Alpha));


			imgui_ex::draw_text(draw_list, user_name.c_str(), u::dpi(ImVec2(50.0f, 560.0f)), f::dinpro_bold, 15.0f);
			imgui_ex::draw_text(draw_list, days_left.c_str(), u::dpi(ImVec2(50.0f, 575.0f)), f::dinpro);

			draw_list->AddImageRounded(i::pfp, pos + u::dpi(ImVec2(20.0f, 565.0f)), pos + u::dpi(ImVec2(44.0f, 589.0f)), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), IM_COL32(255, 255, 255, static_cast<int>(s::menu_alpha)), 5.0f);
			draw_list->AddText(f::awesome, u::dpi(9.0f), pos + u::dpi(ImVec2(185.0f - 12.0f - 25.0f, 576.0f)), s::is_theme_dark ? IM_COL32(255, 255, 255,
				static_cast<int>(s::menu_alpha)) : IM_COL32(0, 0, 0, static_cast<int>(s::menu_alpha)), (char*)u8"\uf74a");

			ImGui::SetCursorPos(u::dpi(ImVec2(185.0f - 18.0f - 25.0f, 565.0f)));

			if (ImGui::InvisibleButton(crypt_str("##theme"), u::dpi(ImVec2(25.0f, 25.0f))))
			{
				s::is_theme_dark = !s::is_theme_dark;

				HKEY theme_key = nullptr;
				DWORD theme_size = 2;

				if (RegOpenKeyEx(HKEY_CURRENT_USER, crypt_str("SOFTWARE\\legendware"), 0, KEY_ALL_ACCESS, &theme_key))
					RegCreateKeyEx(HKEY_CURRENT_USER, crypt_str("SOFTWARE\\legendware"), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &theme_key, nullptr);

				RegSetValueEx(theme_key, crypt_str("theme"), 0, REG_EXPAND_SZ, (BYTE*)(s::is_theme_dark ? crypt_str("d") : crypt_str("l")), theme_size);
				RegCloseKey(theme_key);
			}

			imgui_ex::draw_bar_element(ImVec2(65.0f, 155.0f), crypt_str("Legit"),
				(char*)u8"\ue900", s::tab, 0, draw_list, pos);

			imgui_ex::draw_bar_element(ImVec2(65.0f, 195.0f), crypt_str("Rage"),
				(char*)u8"\ue903", s::tab, 1, draw_list, pos);

			imgui_ex::draw_bar_element(ImVec2(65.0f, 235.0f), crypt_str("Anti-aim"),
				(char*)u8"\ue90f", s::tab, 2, draw_list, pos);

			imgui_ex::draw_bar_element(ImVec2(65.0f, 275.0f), crypt_str("Visuals"),
				(char*)u8"\ue904", s::tab, 3, draw_list, pos);

			imgui_ex::draw_bar_element(ImVec2(65.0f, 315.0f), crypt_str("Misc"),
				(char*)u8"\ue90d", s::tab, 4, draw_list, pos);

			imgui_ex::draw_bar_element(ImVec2(65.0f, 355.0f), crypt_str("Skins"),
				(char*)u8"\ue907", s::tab, 5, draw_list, pos);

			imgui_ex::draw_bar_element(ImVec2(65.0f, 395.0f), crypt_str("Settings"),
				(char*)u8"\ue905", s::tab, 6, draw_list, pos);

			imgui_ex::draw_bar_element(ImVec2(65.0f, 435.0f), crypt_str("Players"),
				(char*)u8"\ue90e", s::tab, 7, draw_list, pos);

			imgui_ex::draw_bar_element(ImVec2(65.0f, 475.0f), crypt_str("Lua"),
				(char*)u8"\ue906", s::tab, 8, draw_list, pos);

			

		}

		ImGui::EndChild();

		static int last_tab = s::tab;

		if (last_tab != s::tab)
		{
			a::reset_all_except(s::tab);
			last_tab = s::tab;
		}

		if (a::poses[s::tab] > 0.0f)
			u::fade(true, a::poses[s::tab], 0.0f, 900.0f, 15.0f);

		if (a::subposes[s::tab] < 0.0f)
			u::fade(false, a::subposes[s::tab], -100.0f, 0.0f, 2.0f);

		if (s::tab_alpha < 255.0f)
			u::fade(false, s::tab_alpha, 0.0f, 255.0f, 4.0f);

		switch (s::tab)
		{
		case 0: /* legit */
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, 0);

			ImGui::SetCursorPos(ImVec2(u::dpi(185.0f), a::poses[s::tab]));
			ImGui::BeginChild(crypt_str("##legit_main"), u::dpi(ImVec2(531.0f, 646.0f)), false, s::main_flags);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, u::dpi(ImVec2(10.0f, 10.0f)));

				ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
				ImGui::BeginGroup();
				{
					imgui_ex::begin_child(crypt_str("##legit_first"),
						u::dpi(ImVec2(250.0f, 160.0f)), s::flags_nm);
					{
						tabs::legit::first_tab();
					}

					imgui_ex::end_child();

					imgui_ex::begin_child(crypt_str("##legit_second"),
						u::dpi(ImVec2(250.0f, 130.0f)), s::flags_nm);
					{
						tabs::legit::second_tab();
					}

					imgui_ex::end_child();

					imgui_ex::begin_child(crypt_str("##legit_third"),
						u::dpi(ImVec2(250.0f, 315.0f)), s::flags_nm);
					{
						tabs::legit::third_tab();
					}

					imgui_ex::end_child();
				}

				ImGui::EndGroup();

				ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 10.0f)));
				ImGui::BeginGroup();
				{
					imgui_ex::begin_child(crypt_str("##legit_fourth"), u::dpi(ImVec2(250.0f, 300.0f)), s::flags_nm);
					{
						ImGui::GetWindowDrawList()->AddShadowRect(ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 25.0f)),
							ImGui::GetWindowPos() + u::dpi(ImVec2(250.0f, 25.0f)), shadow_color, 25.0f,ImVec2(0,0),0, ImGuiStyleVar_ChildRounding, ImDrawCornerFlags_Top);

						ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 0.0f)),
							ImGui::GetWindowPos() + u::dpi(ImVec2(250.0f, 25.0f)), ImGui::GetColorU32(ImGuiCol_ChildBg), ImGuiStyleVar_ChildRounding, ImDrawCornerFlags_Top);

						{
							ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));

							imgui_ex::tab(crypt_str("Hitboxes"), ImVec2(125.0f, 25.0f), s::legit_subtab, 0);

							ImGui::SetCursorPos(u::dpi(ImVec2(125.0f, 0.0f)));

							imgui_ex::tab(crypt_str("Priority"), ImVec2(125.0f, 25.0f), s::legit_subtab, 1);
						}

						ImGui::SetCursorPosY(u::dpi(30.0f));

						if (!s::legit_subtab)
						{
							imgui_ex::hitboxes(crypt_str("##legit_hitboxes"), ImVec2(250.0f, 260.0f), config->legit.weapon[legit_wconfig].hitboxes[0],
								config->legit.weapon[legit_wconfig].hitboxes[1], config->legit.weapon[legit_wconfig].hitboxes[2],
								config->legit.weapon[legit_wconfig].hitboxes[3], config->legit.weapon[legit_wconfig].hitboxes[4],
								config->legit.weapon[legit_wconfig].hitboxes[5]);
						}
						else
						{
							imgui_ex::hitbox_priority(crypt_str("##legit_hitboxes_priority"), ImVec2(250.0f, 260.0f), config->legit.weapon[legit_wconfig].hitboxes_priority[0],
								config->legit.weapon[legit_wconfig].hitboxes_priority[1], config->legit.weapon[legit_wconfig].hitboxes_priority[2],
								config->legit.weapon[legit_wconfig].hitboxes_priority[3], config->legit.weapon[legit_wconfig].hitboxes_priority[4],
								config->legit.weapon[legit_wconfig].hitboxes_priority[5]);
						}
					}

					imgui_ex::end_child();

					imgui_ex::begin_child(crypt_str("##legit_fifth"),
						u::dpi(ImVec2(250.0f, 315.0f)), s::flags_nm);
					{
						tabs::legit::fifth_tab();
					}

					imgui_ex::end_child();
				}

				ImGui::EndGroup();

				ImGui::PopStyleVar();
			}

			ImGui::EndChild();

			ImGui::PopStyleColor();

			break;
		}

		case 1: /* rage */
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, 0);

			ImGui::SetCursorPos(u::dpi(ImVec2(185.0f, a::poses[s::tab])));
			ImGui::BeginChild(crypt_str("##rage_main"), u::dpi(ImVec2(531.0f, 646.0f)), false, s::main_flags);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, u::dpi(ImVec2(10.0f, 10.0f)));

				ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
				ImGui::BeginGroup();
				{
					imgui_ex::begin_child(crypt_str("##rage_first"), u::dpi(ImVec2(250.0f, 202.0f)), s::flags_nm);
					{
						tabs::rage::first_tab();
					}

					imgui_ex::end_child();


					imgui_ex::begin_child(crypt_str("##rage_second"), u::dpi(ImVec2(250.0f, 145.0f)), s::flags_nm, ImVec2(0.0f, 2.0f));
					{
						tabs::rage::second_tab();
					}

					imgui_ex::end_child();


					imgui_ex::begin_child(crypt_str("##rage_third"), u::dpi(ImVec2(250.0f, 258.0f)), s::flags_nm);
					{
						tabs::rage::third_tab();
					}

					imgui_ex::end_child();
				}

				ImGui::EndGroup();

				ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 10.0f)));
				ImGui::BeginGroup();
				{
					imgui_ex::begin_child(crypt_str("##rage_fourth"), u::dpi(ImVec2(250.0f, 300.0f)), s::flags_nm);
					{
						ImGui::GetWindowDrawList()->AddShadowRect(ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 25.0f)),
							ImGui::GetWindowPos() + u::dpi(ImVec2(250.0f, 25.0f)), shadow_color, 25.0f,ImVec2(0, 0), 0, ImGuiStyleVar_ChildRounding, ImDrawCornerFlags_Top);

						ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 0.0f)),
							ImGui::GetWindowPos() + u::dpi(ImVec2(250.0f, 25.0f)), ImGui::GetColorU32(ImGuiCol_ChildBg), ImGuiStyleVar_ChildRounding, ImDrawCornerFlags_Top);

						{
							ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));

							imgui_ex::tab(crypt_str("Hitboxes"), ImVec2(125.0f, 25.0f), s::rage_subtab, 0);

							ImGui::SetCursorPos(u::dpi(ImVec2(125.0f, 0.0f)));

							imgui_ex::tab(crypt_str("Safe points"), ImVec2(125.0f, 25.0f), s::rage_subtab, 1);
						}

						ImGui::SetCursorPosY(u::dpi(30.0f));

						if (!s::rage_subtab)
						{
							imgui_ex::hitboxes(crypt_str("##rage_hitboxes"), ImVec2(250.0f, 260.0f), config->rage.weapon[weapon_config].hitboxes[0],
								config->rage.weapon[weapon_config].hitboxes[1], config->rage.weapon[weapon_config].hitboxes[2],
								config->rage.weapon[weapon_config].hitboxes[3], config->rage.weapon[weapon_config].hitboxes[4],
								config->rage.weapon[weapon_config].hitboxes[5]);
						}
						else
						{
							imgui_ex::safe_points(crypt_str("##rage_safe_hitboxes"), ImVec2(250.0f, 260.0f), config->rage.weapon[weapon_config].safe_hitboxes[0],
								config->rage.weapon[weapon_config].safe_hitboxes[1], config->rage.weapon[weapon_config].safe_hitboxes[2],
								config->rage.weapon[weapon_config].safe_hitboxes[3], config->rage.weapon[weapon_config].safe_hitboxes[4],
								config->rage.weapon[weapon_config].safe_hitboxes[5]);
						}
					}

					imgui_ex::end_child();

					ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 320.0f)));

					imgui_ex::begin_child(crypt_str("##rage_fifth"), u::dpi(ImVec2(250.0f, 150.0f)), s::flags_nm);
					{
						tabs::rage::forth_tab();
					}

					imgui_ex::end_child();

					ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 480.0f)));

					imgui_ex::begin_child(crypt_str("##rage_sixth"), u::dpi(ImVec2(250.0f, 155.0f)), s::flags_nm);
					{
						tabs::rage::fifth_tab();
					}

					imgui_ex::end_child();
				}

				ImGui::EndGroup();

				ImGui::PopStyleVar();
			}

			ImGui::EndChild();

			ImGui::PopStyleColor();

			break;
		}

		case 2: /* anti-aim */
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, 0);

			ImGui::SetCursorPos(u::dpi(ImVec2(185.0f, a::poses[s::tab])));
			ImGui::BeginChild(crypt_str("##antiaim_main"), u::dpi(ImVec2(531.0f, 646.0f)), false, s::main_flags);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, u::dpi(ImVec2(10.0f, 10.0f)));

				ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
				ImGui::BeginGroup();
				{
					imgui_ex::begin_child(crypt_str("##antiaim_first"), u::dpi(ImVec2(250.0f, 625.0f)), s::flags_nm);
					{
						tabs::antiaim::first_tab();
					}

					imgui_ex::end_child();
				}

				ImGui::EndGroup();

				ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 10.0f)));
				ImGui::BeginGroup();
				{
					imgui_ex::begin_child(crypt_str("##antiaim_second"), u::dpi(ImVec2(250.0f, 293.0f)), s::flags_nm);
					{
						tabs::antiaim::second_tab();
					}

					imgui_ex::end_child();

					imgui_ex::begin_child(crypt_str("##antiaim_third"), u::dpi(ImVec2(250.0f, 161.0f)), s::flags_nm);
					{
						tabs::antiaim::third_tab();
					}

					imgui_ex::end_child();

					imgui_ex::begin_child(crypt_str("##antiaim_fourth"), u::dpi(ImVec2(250.0f, 151.0f)), s::flags_nm);
					{
						tabs::antiaim::forth_tab();
					}

					imgui_ex::end_child();
				}

				ImGui::EndGroup();

				ImGui::PopStyleVar();
			}

			ImGui::EndChild();

			ImGui::PopStyleColor();

			break;
		}

		case 3: /* visuals */
		{
			draw_list->AddShadowRect(pos + u::dpi(ImVec2(188.0f, 5.0f)),
				pos + u::dpi(ImVec2(716.0f, 40.0f)),shadow_color, 25.0f, ImVec2(0, 0), 0, style->WindowRounding, ImDrawCornerFlags_TopRight);
			
			draw_list->AddRectFilled(pos + u::dpi(ImVec2(185.0f, 0.0f)),
				pos + u::dpi(ImVec2(716.0f, 40.0f)), ImGui::GetColorU32(ImGuiCol_ChildBg, s::tab_alpha / 255.0f), style->WindowRounding, ImDrawCornerFlags_TopRight);

			{
				ImGui::SetCursorPos(u::dpi(ImVec2(185.0f + 225.0f - 150.0f, a::subposes[s::tab] + 8.0f)));

				imgui_ex::tab(crypt_str("Enemies"), ImVec2(80.0f, 25.0f), s::vis_subtab, 0);

				ImGui::SetCursorPos(u::dpi(ImVec2(185.0f + 255.0f - 80.0f, a::subposes[s::tab] + 8.0f)));

				imgui_ex::tab(crypt_str("Local"), ImVec2(80.0f, 25.0f), s::vis_subtab, 1);

				ImGui::SetCursorPos(u::dpi(ImVec2(185.0f + 285.0f, a::subposes[s::tab] + 8.0f)));

				imgui_ex::tab(crypt_str("Teammates"), ImVec2(80.0f, 25.0f), s::vis_subtab, 2);

				ImGui::SetCursorPos(u::dpi(ImVec2(185.0f + 260.0f + 130.0f, a::subposes[s::tab] + 8.0f)));

				imgui_ex::tab(crypt_str("World"), ImVec2(80.0f, 25.0f), s::vis_subtab, 3);
			}

			ImGui::PushStyleColor(ImGuiCol_ChildBg, 0);

			ImGui::SetCursorPos(u::dpi(ImVec2(185.0f, a::poses[s::tab] + 40.0f)));

			ImGui::BeginChild(crypt_str("##visuals_main"), u::dpi(ImVec2(531.0f, 606.0f)), false, s::main_flags);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, u::dpi(ImVec2(10.0f, 10.0f)));

				switch (s::vis_subtab)
				{
				case 0: // enemies
				{
					ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
					ImGui::BeginGroup();
					{
						imgui_ex::begin_child(crypt_str("##visuals_enemies_first"), u::dpi(ImVec2(250.0f, 310.0f)), s::flags_nm);
						{
							tabs::visuals::enemies_first_tab();
						}

						imgui_ex::end_child();


						imgui_ex::begin_child(crypt_str("##visuals_enemies_second"), u::dpi(ImVec2(250.0f, 265.0f)), s::flags_nm);
						{
							tabs::visuals::enemies_second_tab();
						}

						imgui_ex::end_child();
					}

					ImGui::EndGroup();

					ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 10.0f)));
					ImGui::BeginGroup();
					{
						imgui_ex::begin_child(crypt_str("##visuals_enemies_third"), u::dpi(ImVec2(250.0f, 585.0f)), s::flags_nm);
						{
							tabs::visuals::enemies_third_tab();
						}

						imgui_ex::end_child();
					}

					ImGui::EndGroup();

					break;
				}

				case 1: // local
				{
					ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
					ImGui::BeginGroup();
					{
						imgui_ex::begin_child(crypt_str("##visuals_local_first"), u::dpi(ImVec2(250.0f, 310.0f)), s::flags_nm);
						{
							tabs::visuals::local_first_tab();
						}

						imgui_ex::end_child();

						imgui_ex::begin_child(crypt_str("##visuals_local_second"), u::dpi(ImVec2(250.0f, 265.0f)), s::flags_nm);
						{
							tabs::visuals::local_second_tab();
						}

						imgui_ex::end_child();
					}

					ImGui::EndGroup();

					ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 10.0f)));
					ImGui::BeginGroup();
					{
						imgui_ex::begin_child(crypt_str("##visuals_local_third"), u::dpi(ImVec2(250.0f, 585.0f)), s::flags_nm);
						{
							tabs::visuals::local_third_tab();
						}

						imgui_ex::end_child();
					}

					ImGui::EndGroup();

					break;
				}

				case 2: // teammates
				{
					ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
					ImGui::BeginGroup();
					{
						imgui_ex::begin_child(crypt_str("##visuals_teammates_first"), u::dpi(ImVec2(250.0f, 310.0f)), s::flags_nm);
						{
							tabs::visuals::teammates_first_tab();
						}

						imgui_ex::end_child();

						imgui_ex::begin_child(crypt_str("##visuals_teammates_second"), u::dpi(ImVec2(250.0f, 265.0f)), s::flags_nm);
						{
							tabs::visuals::teammates_second_tab();
						}

						imgui_ex::end_child();
					}

					ImGui::EndGroup();

					ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 10.0f)));
					ImGui::BeginGroup();
					{
						imgui_ex::begin_child(crypt_str("##visuals_teammates_third"), u::dpi(ImVec2(250.0f, 585.0f)), s::flags_nm);
						{
							tabs::visuals::teammates_third_tab();
						}

						imgui_ex::end_child();
					}

					ImGui::EndGroup();

					break;
				}

				case 3: // world
				{
					ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
					ImGui::BeginGroup();
					{
						imgui_ex::begin_child(crypt_str("##visuals_world_first"), u::dpi(ImVec2(250.0f, 220.0f)), s::flags_nm);
						{
							tabs::visuals::world_first_tab();
						}

						imgui_ex::end_child();

						imgui_ex::begin_child(crypt_str("##visuals_world_second"), u::dpi(ImVec2(250.0f, 355.0f)), s::flags_nm);
						{
							tabs::visuals::world_second_tab();
						}

						imgui_ex::end_child();

						/*imgui_ex::begin_child(crypt_str("##visuals_world_third"), u::dpi(ImVec2(250.0f, 155.0f)), s::flags_nm);
						{
							tabs::visuals::world_third_tab();
						}*/

						//imgui_ex::end_child();
					}

					ImGui::EndGroup();

					ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 10.0f)));
					ImGui::BeginGroup();
					{
						imgui_ex::begin_child(crypt_str("##visuals_world_fourth"), u::dpi(ImVec2(250.0f, 585.0f)), s::flags_nm);
						{
							/*	tabs::visuals::world_forth_tab();
							}

							imgui_ex::end_child();

							ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 180.0f)));

							imgui_ex::begin_child(crypt_str("##visuals_world_fifth"), u::dpi(ImVec2(250.0f, 155.0f)), s::flags_nm);
							{*/
							tabs::visuals::world_fifth_tab();
						}

						imgui_ex::end_child();

						/*	ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 345.0f)));

							imgui_ex::begin_child(crypt_str("##visuals_world_sixth"), u::dpi(ImVec2(250.0f, 250.0f)), s::flags_nm);
							{

								tabs::visuals::world_fifth_tab();
							}

							imgui_ex::end_child();*/
					}

					ImGui::EndGroup();

					break;
				}

				default:
					s::vis_subtab = 0;
					break;
				}

				ImGui::PopStyleVar();
			}

			ImGui::EndChild();

			ImGui::PopStyleColor();

			break;
		}

		case 4: /* misc */
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, 0);

			ImGui::SetCursorPos(u::dpi(ImVec2(185.0f, a::poses[s::tab])));
			ImGui::BeginChild(crypt_str("##misc_main"), u::dpi(ImVec2(531.0f, 646.0f)), false, s::main_flags);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, u::dpi(ImVec2(10.0f, 10.0f)));

				ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
				ImGui::BeginGroup();
				{
					imgui_ex::begin_child(crypt_str("##misc_first"), u::dpi(ImVec2(250.0f, 625.0f)), s::flags_nm);
					{
						tabs::misc::first_tab();
					}

					imgui_ex::end_child();
				}

				ImGui::EndGroup();

				ImGui::SetCursorPos(u::dpi(ImVec2(270.0f, 10.0f)));
				ImGui::BeginGroup();
				{
					imgui_ex::begin_child(crypt_str("##misc_second"), u::dpi(ImVec2(250.0f, 125.0f)), s::flags_nm);
					{
						tabs::misc::second_tab();
					}

					imgui_ex::end_child();

					imgui_ex::begin_child(crypt_str("##misc_third"), u::dpi(ImVec2(250.0f, 490.0f)), s::flags_nm);
					{
						tabs::misc::third_tab();
					}

					imgui_ex::end_child();
				}

				ImGui::EndGroup();

				ImGui::PopStyleVar();
			}

			ImGui::EndChild();

			ImGui::PopStyleColor();

			break;
		}

		case 5: /* skins */
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, 0);

			ImGui::SetCursorPos(u::dpi(ImVec2(185.0f, a::poses[s::tab])));
			ImGui::BeginChild(crypt_str("##skins_main"), u::dpi(ImVec2(531.0f, 646.0f)), false, s::main_flags);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, u::dpi(ImVec2(10.0f, 10.0f)));
				ImGuiContext& g = *GImGui;
				imgui_ex::backup_style = g.Style;
				g.Style;
				ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
				imgui_ex::begin_child(crypt_str("##skins_tf"), u::dpi(ImVec2(511.0f, 626.0f)), s::main_flags | ImGuiWindowFlags_NoMove);
				{
					ImGui::SetCursorPos(ImVec2(25.0f, 25.0f));
					ImGui::BeginChild(crypt_str("##skins_list"), u::dpi(ImVec2(466.0f, 598.0f)), false, s::main_flags);
					{
						ImGui::SetCursorPos(ImVec2(15.0f, 15.0f));
						ImGui::BeginGroup();
						{
							tabs::skins::first_tab();

							/* scrollbar fix */
							ImGui::NewLine(); ImGui::NewLine();
						}

						ImGui::EndGroup();
					}

					ImGui::EndChild();
				}

				imgui_ex::end_child();

				ImGui::PopStyleVar();
			}

			ImGui::EndChild();

			ImGui::PopStyleColor();

			break;
		}

		case 6: /* settings */
		{
			draw_list->AddShadowRect(pos + u::dpi(ImVec2(188.0f, 5.0f)),
				pos + u::dpi(ImVec2(716.0f, 40.0f)), shadow_color, 25.0f, ImVec2(0, 0), style->WindowRounding, ImDrawCornerFlags_TopRight);

			draw_list->AddRectFilled(pos + u::dpi(ImVec2(185.0f, 0.0f)),
				pos + u::dpi(ImVec2(716.0f, 40.0f)), ImGui::GetColorU32(ImGuiCol_ChildBg, s::tab_alpha / 255.0f), style->WindowRounding, ImDrawCornerFlags_TopRight);

			{
				ImGui::SetCursorPos(u::dpi(ImVec2(185.0f + 260.0f - 80.0f - 30.0f, a::subposes[s::tab] + 8.0f)));

				imgui_ex::tab(crypt_str("Configs"), ImVec2(80.0f, 25.0f), s::settings_subtab, 0);

				ImGui::SetCursorPos(u::dpi(ImVec2(185.0f + 270.0f + 30.0f, a::subposes[s::tab] + 8.0f)));

				imgui_ex::tab(crypt_str("Scripts"), ImVec2(80.0f, 25.0f), s::settings_subtab, 1);
			}

			ImGui::PushStyleColor(ImGuiCol_ChildBg, 0);

			ImGui::SetCursorPos(u::dpi(ImVec2(185.0f, a::poses[s::tab] + 40.0f)));

			ImGui::BeginChild(crypt_str("##settings_main"), u::dpi(ImVec2(531.0f, 606.0f)), false, s::main_flags);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, u::dpi(ImVec2(10.0f, 10.0f)));

				switch (s::settings_subtab)
				{
				case 0: // configs
				{
					ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
					ImGui::BeginGroup();
					{
						imgui_ex::begin_child(crypt_str("##configs_first"), u::dpi(ImVec2(511.0f, 345.0f)), s::main_flags | ImGuiWindowFlags_NoMove, ImVec2(-10.0f, -10.0f));
						{
							ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(ImGuiCol_SubChild));

							ImGui::BeginChild(crypt_str("##configs_list"), u::dpi(ImVec2(485.0f, 320.0f)), false, s::flags);
							{
								tabs::settings::configs_first_tab();
							}

							ImGui::EndChild();

							ImGui::PopStyleColor();
						}

						imgui_ex::end_child();

						imgui_ex::begin_child(crypt_str("##configs_second"), u::dpi(ImVec2(511.0f, 230.0f)), s::main_flags | ImGuiWindowFlags_NoMove, ImVec2(-11.0f, -11.0f));
						{
							tabs::settings::configs_second_tab();
						}

						imgui_ex::end_child();
					}

					ImGui::EndGroup();

					break;
				}

				case 1: // scripts
				{
					ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
					ImGui::BeginGroup();
					{
						imgui_ex::begin_child(crypt_str("##scripts_first"), u::dpi(ImVec2(511.0f, 345.0f)), s::main_flags, ImVec2(-10.0f, -10.0f));
						{
							ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(ImGuiCol_SubChild));

							ImGui::BeginChild(crypt_str("##scripts_list"), u::dpi(ImVec2(485.0f, 320.0f)), false, s::flags);
							{
								std::string script_name;
								tabs::settings::scripts_first_tab(script_name);
							}

							ImGui::EndChild();

							ImGui::PopStyleColor();
						}

						imgui_ex::end_child();

						imgui_ex::begin_child(crypt_str("##scripts_second"), u::dpi(ImVec2(511.0f, 230.0f)), s::main_flags, ImVec2(-11.0f, -11.0f));
						{
							tabs::settings::scripts_second_tab();
						}

						imgui_ex::end_child();
					}

					ImGui::EndGroup();

					break;
				}

				default:
					s::settings_subtab = 0;
					break;
				}

				ImGui::PopStyleVar();
			}

			ImGui::EndChild();

			ImGui::PopStyleColor();

			break;
		}

		case 7: /* plist */
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, 0);

			ImGui::SetCursorPos(u::dpi(ImVec2(185.0f, a::poses[s::tab])));
			ImGui::BeginChild(crypt_str("##plist_main"), u::dpi(ImVec2(531.0f, 646.0f)), false, s::main_flags);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, u::dpi(ImVec2(10.0f, 10.0f)));

				ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
				imgui_ex::begin_child(crypt_str("##plist_first"), u::dpi(ImVec2(250.0f, 625.0f)), s::flags);
				{
					ImGui::PushStyleColor(ImGuiCol_ChildBg,
						ImGui::GetColorU32(ImGuiCol_SubChild));

					ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
					ImGui::BeginChild(crypt_str("##player_list"), u::dpi(ImVec2(230.0f, 605.0f)), false, s::flags);
					{
						tabs::plist::first_tab();
					}

					ImGui::EndChild();

					ImGui::PopStyleColor();
				}

				imgui_ex::end_child();

				ImGui::SameLine();

				imgui_ex::begin_child(crypt_str("##plist_second"), u::dpi(ImVec2(250.0f, 625.0f)), s::flags);
				{
					tabs::plist::second_tab();
				}

				imgui_ex::end_child();

				ImGui::PopStyleVar();
			}

			ImGui::EndChild();

			ImGui::PopStyleColor();

			break;
		}
		case 8:
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, 0);
			ImGui::SetCursorPos(ImVec2(u::dpi(185.0f), a::poses[s::tab]));
			ImGui::BeginChild(crypt_str("##lua_main"), ImVec2(u::dpi(531.0f), u::dpi(646.0f)), false, s::main_flags);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, u::dpi(ImVec2(10.0f, 10.0f)));
				ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 10.0f)));
				imgui_ex::begin_child(crypt_str("##lua_first"), u::dpi(ImVec2(250.0f, 625.0f)), s::flags);
				{
					tabs::lua::first_tab();
				}

				imgui_ex::end_child();

				ImGui::SameLine();

				imgui_ex::begin_child(crypt_str("##lua_second"), u::dpi(ImVec2(250.0f, 625.0f)), s::flags);
				{
					tabs::lua::second_tab();
				}

				imgui_ex::end_child();

				ImGui::PopStyleVar();
			}
			ImGui::EndChild();
			ImGui::PopStyleColor();
			break;
		}
		default:
			s::tab = s::rage_subtab = s::vis_subtab = s::settings_subtab = 0;
			break;
		}
	/*	Lua_manager->window_pos = ImGui::GetWindowPos();
		Lua_manager->window_size = ImGui::GetWindowSize();*/

	}

	ImGui::End();

	u::fade(!config->misc.watermark, s::watermark_alpha, 0.0f, 255.0f, 7.0f);

	ImGui::GetStyle().Alpha = s::watermark_alpha / 255.0f;

	if (ImGui::GetStyle().Alpha != 0.0f)
	{
		auto draw_separator = []() {
			ImGui::SameLine(0.0f, u::dpi(10.0f));
			ImGui::Text(f::montserrat, u::dpi(17.0f), crypt_str("|"));
			ImGui::SameLine(0.0f, u::dpi(10.0f));
		};

		ImGui::PushStyleColor(ImGuiCol_WindowShadow, 0);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);

#ifdef RELEASE
#if BETA
		auto cheat_name = crypt_str("Legendware beta");
#else
		auto cheat_name = crypt_str("Legendware");
#endif
#else
		auto cheat_name = crypt_str("Legendware alpha");
#endif

		auto nci = engine->GetNetChannelInfo();
		auto delay = 0;

		if (nci)
		{
			auto latency = engine->IsPlayingDemo() ? 0.0f : nci->GetAvgLatency(FLOW_OUTGOING);

			if (latency > 0.0f) //-V550
				latency -= TICKS_TO_TIME(0.5f);

			delay = clamp((int)(latency * 1000.0f), 0, 999);
		}

		std::string server_name;

		if (nci && gamerules)
		{
			server_name = nci->GetAddress();

			if (crypt_hash_r(server_name.c_str()) == crypt_hash("loopback"))
				server_name = crypt_str("Local server");
			else if (gamerules->m_bIsValveDS())
				server_name = crypt_str("Valve server");
		}
		float all_size = 0;
		//static float all_size_temp = 0;
		ImVec2 text_size_cheat = ImGui::CalcTextSize(f::montserrat, u::dpi(17.0f), cheat_name);
		all_size += text_size_cheat.x + u::dpi(25.0f);
		text_size_cheat = ImGui::CalcTextSize(f::montserrat, u::dpi(17.0f), ctx->username.c_str());
		all_size += text_size_cheat.x;
		if (engine->IsInGame())
		{
			if (!server_name.empty())
			{
				all_size += u::dpi(10.0f);
				all_size += ImGui::CalcTextSize(f::montserrat, u::dpi(17.0f), crypt_str("|")).x;
				all_size += u::dpi(10.0f);
				all_size += ImGui::CalcTextSize(f::montserrat, u::dpi(17.0f), server_name.c_str()).x;
			}
			all_size += u::dpi(10.0f);
			all_size += ImGui::CalcTextSize(f::montserrat, u::dpi(17.0f), crypt_str("|")).x;
			all_size += u::dpi(10.0f);
			all_size += ImGui::CalcTextSize(f::montserrat, u::dpi(17.0f), (std::string(crypt_str("delay: ")) + std::to_string(delay) + std::string(crypt_str(" ms"))).c_str()).x;

		}
		all_size += u::dpi(10.0f);
		all_size += u::dpi(5.0f);
		all_size += u::dpi(10.0f);
		float text_size = ImGui::CalcTextSize(f::montserrat, u::dpi(17.0f), cheat_name).x + u::dpi(20.0f);

		ImGui::Begin(crypt_str("##watermark"), &config->misc.watermark, s::main_focus_flags);
		{
			static bool t = false;
			if (!t)
			{
				auto pos = ImGui::GetWindowPos();
				auto size = ImGui::GetWindowSize();

				if (all_size < size.x) {
					auto delta = size.x - all_size;
					pos.x += delta;

				}
				else {
					auto delta = all_size - size.x;
					pos.x -= delta;
				}


				ImGui::SetWindowPos(ImVec2(pos.x, pos.y));
				ImGui::SetWindowSize(ImVec2(all_size, u::dpi(30)));
				t = true;
			}

			if (all_size > 0.0f)
			{
				static float last_size = all_size;

				if (last_size != all_size)
				{
					float difference = all_size - last_size;
					ImVec2 cur_pos = ImGui::GetWindowPos();

					ImGui::SetWindowPos(ImVec2(cur_pos.x - difference, cur_pos.y));
					ImGui::SetWindowSize(ImVec2(all_size, u::dpi(30)));

					last_size = all_size;
				}
			}
			//DrawBackgroundBlur(ImGui::GetWindowDrawList(), render->device.get());
			//DrawBackgroundBlurRounded(ImGui::GetWindowDrawList(), render->device.get(), ImVec2(0.0f, 0.0f), ImVec2(2560.0f, 1440.0f), 5.0f);
			/* header */
			//DrawBackgroundBlurRounded(ImGui::GetWindowDrawList(), render->device.get(), ImGui::GetWindowPos() + ImVec2(text_size + 0.0f, 0.0f), ImGui::GetWindowPos() + ImVec2(all_size, u::dpi(30.0f)), 5.0f, ImDrawCornerFlags_Right);

			ImGui::GetWindowDrawList()->PushClipRect(ImGui::GetWindowPos() + ImVec2(text_size + 0.0f, 0.0f), ImGui::GetWindowPos() + ImVec2(all_size, u::dpi(30.0f)));
			DrawBackgroundBlur(ImGui::GetWindowDrawList(), render->device.get());
			ImGui::GetWindowDrawList()->PopClipRect();

			ImGui::RenderFrame(ImGui::GetWindowPos() + ImVec2(0.0f, 0.0f),
				ImGui::GetWindowPos() + ImVec2(text_size, u::dpi(30.0f)),
				ImGui::GetColorU32(ImGuiCol_Keybinds), true, 5.0f, ImDrawCornerFlags_Left);

			ImGui::RenderFrame(ImGui::GetWindowPos() + ImVec2(text_size, 0.0f),
				ImGui::GetWindowPos() + ImVec2(all_size, u::dpi(30.0f)),
				ImGui::GetColorU32(ImGuiCol_KeybindsBody, 0.39f), true, 5.0f, ImDrawCornerFlags_Right);


			ImGui::SetCursorPos(u::dpi(ImVec2(10.0f, 9.0f)));
			ImGui::BeginGroup();
			{
				ImGui::Text(f::montserrat, u::dpi(17.0f), cheat_name);
				ImGui::SameLine(0.0f, u::dpi(25.0f));
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32_WHITE);
				ImGui::Text(f::montserrat, u::dpi(17.0f), ctx->username.c_str());

				if (engine->IsInGame())
				{
					if (!server_name.empty())
					{
						draw_separator();
						ImGui::Text(f::montserrat, u::dpi(17.0f), server_name.c_str());
					}

					draw_separator();
					ImGui::Text(f::montserrat, u::dpi(17.0f), crypt_str("delay: %d ms"), delay);
				}

				ImGui::SameLine(0.0f, 10.0f);
				ImGui::Dummy(u::dpi(ImVec2(5.0f, 0.0f)));

				ImGui::PopStyleColor();
			}

			ImGui::EndGroup();
		}

		ImGui::End();

		ImGui::PopStyleColor(2);

	}

	/* keybinds */
	auto draw_key_binds = s::is_opened;

	if (!draw_key_binds && ctx->local())
	{
		for (auto& item : config->items)
		{
			if (item.second.type == ITEM_KEY_BIND)
			{
				KeyBind* key_bind = (KeyBind*)item.second.pointer.get();

				if (key_bind->state && key_bind->visible)
				{
					draw_key_binds = true;
					break;
				}
			}
		}

		/*for (auto& current : Lua_manager->scripts)
		{
			auto& items = Lua_manager->items.at(Lua_manager->get_script_id(current));

			for (auto& item : items)
			{
				if (item.second.type == KEY_BIND)
				{
					auto key_bind = crypt_ptr <KeyBind>((KeyBind*)&item.second.m_bind);

					if (key_bind->state && key_bind->visible)
					{
						draw_key_binds = true;
						break;
					}
				}
			}
		}*/
	}

	u::fade(!config->misc.key_binds || !draw_key_binds, s::keybind_alpha, 0.0f, 255.0f, 7.0f);

	ImGui::GetStyle().Alpha = s::keybind_alpha / 255.0f;

	if (ImGui::GetStyle().Alpha != 0.0f)
	{
		auto draw_keybind = [](const char* text, int type, ...)
		{
			ImVec2 text_size = ImGui::CalcTextSize(f::montserrat, u::dpi(17.0f), text);

			std::string final_text = text;

			if (text_size.x >= u::dpi(120.0f))
			{
				final_text = final_text.substr(0, final_text.length() - 8);
				final_text += crypt_str("...");
			}

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32_WHITE);

			ImGui::Text(f::montserrat, u::dpi(17.0f), crypt_str("%s"), final_text.c_str());
			std::string state{};
			float spacing = 154.0f;
			switch (type)
			{
			case ALWAYS_ON:
				state = crypt_str("[always on]");
				spacing = 138.5f;
				break;
			case TOGGLE:
				state = crypt_str("[toggled]");
				spacing = 150.0f;
				break;
			case HOLD:
				state = crypt_str("[held]");
				spacing = 176.0f;
				break;
			default:
				break;
			}

			ImGui::SameLine(u::dpi(spacing));
			ImGui::Text(f::montserrat, u::dpi(17.0f), crypt_str("%s"), state.c_str());

			ImGui::PopStyleColor();
		};

		ImGui::PushStyleColor(ImGuiCol_WindowShadow, 0);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);

		ImGui::SetNextWindowSizeConstraints(u::dpi(ImVec2(250.0f, 30.0f)), ImVec2(u::dpi(250.0f), FLT_MAX));

		ImGui::Begin(crypt_str("##keybinds"), &config->misc.key_binds, s::popups_flags);
		{
			/* header */
			//DrawBackgroundBlurRounded(ImGui::GetWindowDrawList(), render->device.get(), ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 30.0f)), ImGui::GetWindowPos() + ImGui::GetWindowSize(), 5.0f, ImDrawCornerFlags_Bot);

			ImGui::GetWindowDrawList()->PushClipRect(ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 30.0f)), ImGui::GetWindowPos() + ImGui::GetWindowSize(), u::dpi(30.0f));
			DrawBackgroundBlur(ImGui::GetWindowDrawList(), render->device.get());
			ImGui::GetWindowDrawList()->PopClipRect();

			ImGui::RenderFrame(ImGui::GetWindowPos() + ImVec2(0.0f, 0.0f),
				ImGui::GetWindowPos() + u::dpi(ImVec2(250.0f, 30.0f)),
				ImGui::GetColorU32(ImGuiCol_Keybinds), true, 5.0f, ImDrawCornerFlags_Top);

			ImGui::RenderFrame(ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 30.0f)),
				ImGui::GetWindowPos() + ImGui::GetWindowSize(),
				ImGui::GetColorU32(ImGuiCol_KeybindsBody, 0.39f), true, 5.0f, ImDrawCornerFlags_Bot);

			ImGui::SetCursorPos(u::dpi(ImVec2(12.0f, 17.0f)));
			ImGui::Text(f::awesome, u::dpi(10.0f), (char*)u8"\uf11c");
			
			ImGui::SetCursorPos(u::dpi(ImVec2(46.0f, 8.0f)));
			ImGui::Text(f::montserrat, u::dpi(17.0f), crypt_str("Key binds"));

			/* footer */
			std::vector<KeyBind>binds;

			for (auto& item : config->items)
			{
				if (item.second.type == ITEM_KEY_BIND)
				{
					KeyBind* key_bind = (KeyBind*)item.second.pointer.get();

					auto valid = true;

					switch (key_bind->dependence)
					{
					case DEPENDENCE_LEGIT:
						if (!config->legit.enable)
							valid = false;

						break;
					case DEPENDENCE_RAGE:
						if (!config->rage.enable)
							valid = false;

						break;
					case DEPENDENCE_ANTI_AIM:
						if (!config->anti_aim.enable)
							valid = false;

						break;
					}

					if (valid && key_bind->state && key_bind->visible)
						binds.emplace_back(KeyBind{ key_bind->name, key_bind->mode });
				}
			}
		/*	for (auto& current : Lua_manager->scripts)
			{
				auto& items = Lua_manager->items.at(Lua_manager->get_script_id(current));

				for (auto& item : items)
				{
					if (item.second.type == KEY_BIND)
					{
						if (item.second.m_bind.state) {
							binds.emplace_back(KeyBind{ item.second.m_bind.name, item.second.m_bind.mode });
						}
					}
				}
			}*/


			ImGui::SetCursorPos(u::dpi(ImVec2(17.0f, 40.0f)));
			ImGui::BeginGroup();
			{
				for (auto& bind : binds)
					draw_keybind(bind.name.c_str(), bind.mode);

				ImGui::Dummy(ImVec2(0.0f, u::dpi(5.0f)));
			}

			ImGui::EndGroup();
		}

		ImGui::End();

		ImGui::PopStyleColor(2);

	}

	u::fade(!config->misc.spectator_list || (!s::is_opened && spectator_list->get_spectators().empty()), s::spectatorlist_alpha, 0.0f, 255.0f, 7.0f);

	ImGui::GetStyle().Alpha = s::spectatorlist_alpha / 255.0f;

	if (ImGui::GetStyle().Alpha != 0.0f)
	{
		auto draw_spectators = [](std::string text, LPDIRECT3DTEXTURE9 avatar) {
			ImVec2 text_size = ImGui::CalcTextSize(f::montserrat, u::dpi(17.0f), text.c_str());

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32_WHITE);
			if (avatar) {

				ImGui::Image(avatar, u::dpi(ImVec2(18, 18)), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0), 2.0f);
				ImGui::SameLine();
			}
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
			
			if (text_size.x > u::dpi(200.0f))
			{
				ImGui::GetWindowDrawList()->PushClipRect(ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 30.0f)), ImGui::GetWindowPos() + ImGui::GetWindowSize() - u::dpi(ImVec2(30.0f, 0.0f)));
				ImGui::Text(f::montserrat, u::dpi(17.0f), crypt_str("%s"), text.c_str());
				ImGui::GetWindowDrawList()->PopClipRect();

				ImGui::SameLine();

				ImGui::SetCursorPosX(ImGui::GetWindowSize().x - u::dpi(28.0f));
				ImGui::Text(f::montserrat, u::dpi(17.0f), crypt_str("%s"), crypt_str("..."));
			}
			else
			{
				ImGui::Text(f::montserrat, u::dpi(17.0f), crypt_str("%s"), text.c_str());
			}

			ImGui::PopStyleColor();

		};

		ImGui::PushStyleColor(ImGuiCol_WindowShadow, 0);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);

		ImGui::SetNextWindowSizeConstraints(u::dpi(ImVec2(250.0f, 30.0f)), ImVec2(u::dpi(250.0f), FLT_MAX));

		ImGui::Begin(crypt_str("##spectators"), &config->misc.spectator_list, s::popups_flags);
		{
			/* header */
			//DrawBackgroundBlurRounded(ImGui::GetWindowDrawList(), render->device.get(), ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 30.0f)), ImGui::GetWindowPos() + ImGui::GetWindowSize(), 5.0f, ImDrawCornerFlags_Bot);

			ImGui::GetWindowDrawList()->PushClipRect(ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 30.0f)), ImGui::GetWindowPos() + ImGui::GetWindowSize(), u::dpi(30.0f));
			DrawBackgroundBlur(ImGui::GetWindowDrawList(), render->device.get());
			ImGui::GetWindowDrawList()->PopClipRect();

			ImGui::RenderFrame(ImGui::GetWindowPos() + ImVec2(0.0f, 0.0f),
				ImGui::GetWindowPos() + u::dpi(ImVec2(250.0f, 30.0f)),
				ImGui::GetColorU32(ImGuiCol_Keybinds), true, 5.0f, ImDrawCornerFlags_Top);

			ImGui::RenderFrame(ImGui::GetWindowPos() + u::dpi(ImVec2(0.0f, 30.0f)),
				ImGui::GetWindowPos() + ImGui::GetWindowSize(),
				ImGui::GetColorU32(ImGuiCol_KeybindsBody, 0.39f), true, 5.0f, ImDrawCornerFlags_Bot);

			ImGui::SetCursorPos(u::dpi(ImVec2(4.0f, 24.0f)));
			ImGui::Text(f::icons, u::dpi(10.0f), (char*)u8"\ue911");

			ImGui::SetCursorPos(u::dpi(ImVec2(40.0f, 8.0f)));
			ImGui::Text(f::montserrat, u::dpi(17.0f), crypt_str("Spectators"));

			/* footer */

			ImGui::SetCursorPos(u::dpi(ImVec2(17.0f, 40.0f)));
			ImGui::BeginGroup();
			{
				auto spectator_data = spectator_list->get_spectators();
				for (auto& spectator : spectator_data)
					draw_spectators(spectator.first, spectator.second);

				ImGui::Dummy(ImVec2(0.0f, u::dpi(5.0f)));
			}

			ImGui::EndGroup();
		}

		ImGui::End();

		ImGui::PopStyleColor(2);
	}

	ImGui::EndFrame();
	ImGui::GetStyle().Alpha = 1.0f;

	ImGui::Render(renderer_imgui.Render());
}
#pragma optimize("", on)

void Menu::release()
{
	SetWindowLongPtr(FindWindow(crypt_str("Valve001"), nullptr), GWL_WNDPROC, (LONG)old_window);

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}