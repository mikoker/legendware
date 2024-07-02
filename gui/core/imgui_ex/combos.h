#pragma once
#include "../core.h"

namespace imgui_ex
{
	bool combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text),
		void* data, int items_count);

	bool combo(const char* label, int* current_item, const char* const items[],
		int items_count);

	bool combo(const char* label, int* current_item,
		const char* items_separated_by_zeros);

	bool mcombo(const char* label, bool* v,
		const char* items[], int items_count);

	bool mcombo_clr(const char* label, bool* v, float clr[][4],
		const char* items[], int items_count,float width = u::dpi(206.0f));

	bool mcombo_clr3(const char* label, bool* v, float clr[][3], 
		const char* items[], int items_count, float width = u::dpi(206.0f));
}
