#include "lua.h"
#include "..\imgui_ex.h"
#include "..\..\Lua\Clua.h"

namespace core::tabs::lua
{
	void first_tab()
	{
		//auto previous_check_box = false;
		//for (auto& current : Lua_manager->scripts)
		//{
		//	auto& items = Lua_manager->items.at(Lua_manager->get_script_id(current));

		//	for (auto& item : items)
		//	{
		//		std::string item_name;

		//		auto first_point = false;
		//		auto item_str = false;

		//		for (auto& c : item.first)
		//		{
		//			if (c == '.')
		//			{
		//				if (first_point)
		//				{
		//					item_str = true;
		//					continue;
		//				}
		//				else
		//					first_point = true;
		//			}

		//			if (item_str)
		//				item_name.push_back(c);
		//		}
		//		char** arr;
		//		switch (item.second.type)
		//		{
		//		case NEXT_LINE:
		//			previous_check_box = false;
		//			break;
		//		case CHECK_BOX:
		//			previous_check_box = true;
		//			imgui_ex::checkbox(item_name.c_str(), &item.second.check_box_value);
		//			break;
		//		case COMBO_BOX:
		//			previous_check_box = false;
		//			 arr = new char* [item.second.combo_box_labels.size()];
		//			for (size_t i = 0; i < item.second.combo_box_labels.size(); i++) {
		//				arr[i] = new char[item.second.combo_box_labels[i].size() + 1];
		//				strcpy(arr[i], item.second.combo_box_labels[i].c_str());
		//			}

		//			imgui_ex::combo(item_name.c_str(), &item.second.combo_box_value, arr, item.second.combo_box_labels.size());
		//			for (size_t i = 0; i < item.second.combo_box_labels.size(); i++) {
		//				delete[] arr[i];
		//			}
		//			delete[] arr;
		//			break;
		//		case SLIDER_INT:
		//			previous_check_box = false;
		//			imgui_ex::slider_int(item_name.c_str(), &item.second.slider_int_value, item.second.slider_int_min, item.second.slider_int_max);
		//			break;
		//		case SLIDER_FLOAT:
		//			previous_check_box = false;
		//			imgui_ex::slider_float(item_name.c_str(), &item.second.slider_float_value, item.second.slider_float_min, item.second.slider_float_max);
		//			break;
		//		case COLOR_PICKER:
		//			if (previous_check_box)
		//				previous_check_box = false;
		//			else
		//				imgui_ex::text((item_name + ' ').c_str());

		//			ImGui::SameLine();
		//			imgui_ex::color_edit4((crypt_str("##") + item_name).c_str(), &item.second.color_picker_value);
		//			break;
		//		case KEY_BIND:
		//			if (previous_check_box)
		//				previous_check_box = false;
		//			else
		//				imgui_ex::text((item_name + ' ').c_str());

		//			ImGui::SameLine();
		//			imgui_ex::hotkey((crypt_str("##") + item_name).c_str(), item.second.m_bind);
		//			break;
		//		case BUTTON:
		//			previous_check_box = false;

		//			if (imgui_ex::button(item_name.c_str(), ImVec2(206.0f, 30.0f)))
		//			{
		//				//if (item.second.m_callback != sol::nil)
		//				{
		//					item.second.m_callback();
		//				}
		//			}

		//			break;
		//		}
		//	}
		//}

		ImGui::Spacing();
	}

	void second_tab()
	{
		imgui_ex::checkbox(crypt_str("Developer mode"), &config->scripts.developer_mode);
		imgui_ex::checkbox(crypt_str("Allow file edit"), &config->scripts.allow_file);
		imgui_ex::checkbox(crypt_str("Allow HTTP requests"), &config->scripts.allow_http);
	}
}
