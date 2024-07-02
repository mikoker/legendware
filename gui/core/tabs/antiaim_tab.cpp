#include "antiaim.h"
#include "..\imgui_ex.h"

namespace core::tabs::antiaim
{
	void first_tab()
	{
		imgui_ex::checkbox(crypt_str("Enable"), &config->anti_aim.enable);
		imgui_ex::combo(crypt_str("Pitch"), &config->anti_aim.pitch, v::pitch, IM_ARRAYSIZE(v::pitch));
		imgui_ex::combo(crypt_str("Target yaw"), &config->anti_aim.target_yaw, v::target_yaw, IM_ARRAYSIZE(v::target_yaw));

		imgui_ex::slider_int(crypt_str("Yaw offset"), &config->anti_aim.yaw_offset, -180, 180);

		imgui_ex::combo(crypt_str("Yaw modifier"), &config->anti_aim.yaw_modifier, v::yaw_modifier, IM_ARRAYSIZE(v::yaw_modifier));

		if (config->anti_aim.yaw_modifier)
			imgui_ex::slider_int(crypt_str("Range"), &config->anti_aim.yaw_modifier_range, 1, 180);

		imgui_ex::checkbox(crypt_str("Freestanding"), &config->anti_aim.freestanding);

		if (config->anti_aim.freestanding)
		{
			imgui_ex::hotkey(crypt_str("##freestanding_key"), config->anti_aim.freestanding_key);
			imgui_ex::combo(crypt_str("Freestanding yaw modifier"), &config->anti_aim.freestanding_yaw_modifier, v::freestanding_yaw_modifier, IM_ARRAYSIZE(v::freestanding_yaw_modifier));

			if (config->anti_aim.freestanding_yaw_modifier >= 2)
				imgui_ex::slider_int(crypt_str("Freestanding range"), &config->anti_aim.freestanding_yaw_modifier_range, 1, 180);
		}
		imgui_ex::set_hotkey_visible(config->anti_aim.freestanding_key, config->anti_aim.freestanding);

		imgui_ex::text(crypt_str("Manual forward"));
		imgui_ex::hotkey(crypt_str("##manual_forward_key"), config->anti_aim.manual_forward_key);

		imgui_ex::text(crypt_str("Manual back"));
		imgui_ex::hotkey(crypt_str("##manual_back_key"), config->anti_aim.manual_back_key);

		imgui_ex::text(crypt_str("Manual left"));
		imgui_ex::hotkey(crypt_str("##manual_left_key"), config->anti_aim.manual_left_key);

		imgui_ex::text(crypt_str("Manual right"));
		imgui_ex::hotkey(crypt_str("##manual_right_key"), config->anti_aim.manual_right_key);
	}

	void second_tab()
	{
		imgui_ex::combo(crypt_str("Desync type"), &config->anti_aim.desync_type, v::desync_type, IM_ARRAYSIZE(v::desync_type));

		if (config->anti_aim.desync_type)
		{
			imgui_ex::slider_int(crypt_str("Desync range"), &config->anti_aim.desync_range, 1, 60);
			imgui_ex::slider_int(crypt_str("Desync range inverted"), &config->anti_aim.desync_range_inverted, 1, 60); //, ImGuiSliderFlags_None, crypt_str("%d°")

			if (config->anti_aim.desync_type != 2)
			{
				imgui_ex::text(crypt_str("Invert desync"));
				imgui_ex::hotkey(crypt_str("##invert_desync_key"), config->anti_aim.invert_desync_key);
			}
		}
		imgui_ex::set_hotkey_visible(config->anti_aim.invert_desync_key, config->anti_aim.desync_type && config->anti_aim.desync_type != 2);

		imgui_ex::checkbox(crypt_str("Roll"), &config->anti_aim.roll);

		if (config->anti_aim.roll)
		{
			imgui_ex::checkbox(crypt_str("Disable on high velocity"), &config->anti_aim.disable_on_high_velocity);
			imgui_ex::slider_int(crypt_str("Roll range"), &config->anti_aim.roll_range, 1, 50);
		}
	}

	void third_tab()
	{
		imgui_ex::checkbox(crypt_str("Enable fake lag"), &config->anti_aim.enable_fake_lag);
		imgui_ex::combo(crypt_str("Type"), &config->anti_aim.fake_lag_type, v::fake_lag_type, IM_ARRAYSIZE(v::fake_lag_type));
		imgui_ex::slider_int(crypt_str("Limit"), &config->anti_aim.fake_lag_limit, 1, 16);
	}

	void forth_tab()
	{
		imgui_ex::combo(crypt_str("Leg movement"), &config->anti_aim.leg_movement, v::leg_movement, IM_ARRAYSIZE(v::leg_movement));
		imgui_ex::text(crypt_str("Slow walk"));
		imgui_ex::hotkey(crypt_str("##slow_walk_key"), config->anti_aim.slow_walk_key);
		imgui_ex::text(crypt_str("Fake duck"));
		imgui_ex::hotkey(crypt_str("##fake_duck_key"), config->anti_aim.fake_duck_key);
	}
}