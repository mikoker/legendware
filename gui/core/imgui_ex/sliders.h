#pragma once
#include "../core.h"

namespace imgui_ex
{
	bool dpi_slider_scalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min,
		const void* p_max, const char* values[], ImGuiSliderFlags flags);
	bool slider_scalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min,
		const void* p_max, const char* format = nullptr, ImGuiSliderFlags flags = 0);
	bool slider_dpi(const char* label, int* v, int v_min,
		int v_max, const char* values[], float width);

	bool slider_int(const char* label, int* v, int v_min,
		int v_max, ImGuiSliderFlags flags = 0, const char* format = "%d", float width = 205.0f);

	bool slider_float(const char* label, float* v, float v_min,
		float v_max, const char* format = "%.2f", float width = 205.0f, ImGuiSliderFlags flags = 0);
}
