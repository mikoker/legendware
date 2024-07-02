#include "../../core/core.h"

#undef min
#undef max

namespace u
{
	std::string random_string(size_t length)
	{
		/*auto randchar = []() -> char
		{
			constexpr char charset[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";

			constexpr size_t max_index = (sizeof(charset) - 1);
			return charset[rand() % max_index];
		};

		std::string str(length, 0);
		std::generate_n(str.begin(), length, randchar);*/

		static const char alphabet[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";

		static std::mt19937 rg{ std::random_device{}() };
		static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(alphabet) - 2);

		std::string str;

		str.reserve(length);

		while (length--)
			str += alphabet[pick(rg)];

		return str;
	}

	void fade(bool toggle, float& result, float min,
		float max, float speed)
	{
		/* magic number */
		float step = speed * 200.0f * ImGui::GetIO().DeltaTime;

		toggle ? result -= step : result += step;
		result = std::clamp(result, min, max);
	}
	void fade_noclamp(bool toggle, float& result, float speed)
	{
		/* magic number */
		float step = speed * 200.0f * ImGui::GetIO().DeltaTime;

		toggle ? result -= step : result += step;
	}
	ImColor fade_color(ImU32 old_clr,
		ImU32 new_clr, ImVec4& clr_out, float speed)
	{
		ImVec4 old_clr4 = ImGui::ColorConvertU32ToFloat4(old_clr),
			new_clr4 = ImGui::ColorConvertU32ToFloat4(new_clr);

		old_clr4.x *= 255.0f;
		old_clr4.y *= 255.0f;
		old_clr4.z *= 255.0f;

		new_clr4.x *= 255.0f;
		new_clr4.y *= 255.0f;
		new_clr4.z *= 255.0f;
		new_clr4.w *= 255.0f;

		float r_min = min(old_clr4.x, new_clr4.x),
			g_min = min(old_clr4.y, new_clr4.y),
			b_min = min(old_clr4.z, new_clr4.z);

		float r_max = max(old_clr4.x, new_clr4.x),
			g_max = max(old_clr4.y, new_clr4.y),
			b_max = max(old_clr4.z, new_clr4.z);

		fade(old_clr4.x <= new_clr4.x, clr_out.x, r_min, r_max, speed);
		fade(old_clr4.y <= new_clr4.y, clr_out.y, g_min, g_max, speed);
		fade(old_clr4.z <= new_clr4.z, clr_out.z, b_min, b_max, speed);

		return ImColor(static_cast<int>(clr_out.x), static_cast<int>(clr_out.y),
			static_cast<int>(clr_out.z), static_cast<int>(new_clr4.w));
	}

	float dpi_ex(float value, int index)
	{
		bool act = index == 1;

		index = std::clamp(index, 0, 9);

		float percent = 0.0f;

		switch (index)
		{
		case 0: percent = -10.0f; break;
		case 1: percent = 0.0f; break;
		case 2: percent = 10.0f; break;
		case 3: percent = 20.0f; break;
		case 4: percent = 30.0f; break;
		case 5: percent = 40.0f; break;
		case 6: percent = 50.0f; break;
		case 7: percent = 60.0f; break;
		case 8: percent = 70.0f; break;
		case 9: percent = 80.0f; break;
		default: percent = 0.0f; break;
		}

		return act ? value : ImFloor((value) * (
			1.0f + percent / 100.0f));
	}

	float dpi(float value, float offset)
	{
		bool act = core::s::temp_dpi_scale == 2;

		core::s::temp_dpi_scale = std::clamp(
			core::s::temp_dpi_scale, 1, 10);

		float percent = 0.0f;

		switch (core::s::temp_dpi_scale)
		{
		case 1: percent = -10.0f; break;
		case 2: percent = 0.0f; break;
		case 3: percent = 10.0f; break;
		case 4: percent = 20.0f; break;
		case 5: percent = 30.0f; break;
		case 6: percent = 40.0f; break;
		case 7: percent = 50.0f; break;
		case 8: percent = 60.0f; break;
		case 9: percent = 70.0f; break;
		case 10: percent = 80.0f; break;
		default: percent = 0.0f; break;
		}

		return act ? value : ImFloor((value - offset) * (
			1.0f + percent / 100.0f));
	}

	ImVec2 dpi(ImVec2 value, ImVec2 offset)
	{
		bool act = core::s::temp_dpi_scale == 2;

		core::s::temp_dpi_scale = std::clamp(
			core::s::temp_dpi_scale, 1, 10);

		ImVec2 percent = ImVec2(0.0f, 0.0f);

		switch (core::s::temp_dpi_scale)
		{
		case 1: percent = ImVec2(-10.0f, -10.0f); break;
		case 2: percent = ImVec2(0.0f, 0.0f); break;
		case 3: percent = ImVec2(10.0f, 10.0f); break;
		case 4: percent = ImVec2(20.0f, 20.0f); break;
		case 5: percent = ImVec2(30.0f, 30.0f); break;
		case 6: percent = ImVec2(40.0f, 40.0f); break;
		case 7: percent = ImVec2(50.0f, 50.0f); break;
		case 8: percent = ImVec2(60.0f, 60.0f); break;
		case 9: percent = ImVec2(70.0f, 70.0f); break;
		case 10: percent = ImVec2(80.0f, 80.0f); break;
		default: percent = ImVec2(0.0f, 0.0f); break;
		}

		return act ? value : ImFloor((value - offset) * (
			ImVec2(1.0f, 1.0f) + percent / ImVec2(100.0f, 100.0f)));
	}
}
