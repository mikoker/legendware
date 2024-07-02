#pragma once
#include "../core.h"
#include "../../../config/config.h"
namespace imgui_ex
{
	void same_line(float spacing_w = -1.0f, float offset_from_start_x = 0.0f);

	void text(const char* fmt, ...);

	void hitbox_priority(const char* label, const ImVec2& size_arg, int& head_pr,
		int& body_pr, int& pelvis_pr, int& legs_pr, int& feet_pr, int& arms_pr);

	void safe_points(const char* label, const ImVec2& size_arg, int& head_pr,
		int& body_pr, int& pelvis_pr, int& legs_pr, int& feet_pr, int& arms_pr);

	void hitboxes(const char* label, const ImVec2& size_arg, bool& head_pr,
		bool& body_pr, bool& pelvis_pr, bool& legs_pr, bool& feet_pr, bool& arms_pr);

	bool checkbox(const char* label, bool* v);

	bool add_skin_button(const ImVec2& size_arg);

	bool add_sticker_button(const ImVec2& size_arg,
		IDirect3DTexture9* image, int rarity, int id_sticker = 0, bool close_button = false);

	bool skin_element(const ImVec2& size_arg,
		core::s_skin_info& skin_info, bool close_button = false);

	void skin_preview(core::s_skin_info& skin_info, int& tab, bool edit = false);

	bool list_box(const char* label, int* currIndex,
		std::vector<std::string>& values, int height_in_items = -1);

	bool hotkey(const char* label, KeyBind& key_bind);

	void set_hotkey_visible(KeyBind& key_bind, bool visible);

	bool input_text(const char* label, char* buf, size_t buf_size,
		const ImVec2& size = ImVec2(0.0f, 0.0f), const ImVec2& pos = ImVec2(0.0f, 0.0f));

	void change_theme();
	void key_binds(const ImVec2& size_arg,
		const std::vector<KeyBind>& keybinds);

	void ImRotateStart();
	ImVec2 ImRotationCenter();
	void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter());
	
	extern ImGuiStyle backup_style;
}
