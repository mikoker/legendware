//// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
//// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
//#define CPPHTTPLIB_OPENSSL_SUPPORT
//#include "../httplib.h"
//#include "Clua.h"
//#include <ShlObj_core.h>
//#include <Windows.h>
//#include <any>
//
//#include "..\features\aim.h"
//#include "../gui/core/core.h"
//#include "../hooks/hooks.h"
//#include "../render.h"
//#include "../features/chams.h"
//#include "../config/config.h"
//#include "../features/features.h"
//#include "../ImGui Render/ImGUI_Renderer.h"
//#include "../features/logs.h"
//#include "../features/prediction.h"
//
//crypt_ptr <c_lua> Lua_manager;
//
//int my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) { //-V813
//
//
//	if (maybe_exception) {
//
//		const std::exception& ex = *maybe_exception;
//
//		logs->add(crypt_str("Lua error: ") + std::string(ex.what()), Color::Red, crypt_str("[ LUA ] "), true);
//	}
//	else {
//
//
//		std::string err = std::string(description.data(), static_cast<std::streamsize>(description.size()));
//		logs->add(crypt_str("Lua error: ") + err, Color::Red, crypt_str("[ LUA ] "), true);
//
//	}
//
//	return sol::stack::push(L, description);
//}
//void lua_panic(sol::optional <std::string> message)
//{
//
//}
//
//std::string get_current_script(sol::this_state s)
//{
//	sol::state_view lua_state(s);
//	sol::table rs = lua_state["debug"]["getinfo"](2, ("S"));
//	std::string source = rs["source"];
//	std::string filename = std::filesystem::path(source.substr(1)).filename().string();
//
//	return filename;
//}
//
//int get_current_script_id(sol::this_state s)
//{
//	return Lua_manager->get_script_id(get_current_script(s));
//}
//
//namespace ns_client
//{
//	void add_callback(sol::this_state s, std::string eventname, sol::protected_function func) //-V813
//	{
//		if (eventname != crypt_str("on_paint") && 
//			eventname != crypt_str("create_move") && 
//			eventname != crypt_str("on_shot") && 
//			eventname != crypt_str("unload") && 
//			eventname != crypt_str("drawmodel")&&
//			eventname != crypt_str("pre_render")&&
//			eventname != crypt_str("post_render")&&
//			eventname != crypt_str("net_update_start")&&
//			eventname != crypt_str("net_update_end"))
//		{
//			logs->add(crypt_str("Lua error: invalid callback \"") + eventname + '\"', Color::Red, crypt_str("[ LUA ] ") + get_current_script(s), true);
//			return;
//		}
//
//		if (Lua_manager->loaded.at(get_current_script_id(s))) {
//			for (auto& state : Lua_manager->states) {
//				if (state.id == get_current_script_id(s))
//					state.hooks.registerHook(eventname, func);
//			}
//
//		}
//	}
//
//	void load_script(std::string name) //-V813
//	{
//		Lua_manager->refresh_scripts();
//		Lua_manager->load_script(Lua_manager->get_script_id(name));
//	}
//
//	void unload_script(std::string name) //-V813
//	{
//		Lua_manager->refresh_scripts();
//		Lua_manager->unload_script(Lua_manager->get_script_id(name));
//	}
//
//	void log(sol::this_state s, const std::string& text)
//	{
//		logs->add(text, Color::White, crypt_str(""), true);
//	}
//}
//
//std::vector <std::pair <std::string, menu_item>>::iterator find_item(std::vector <std::pair <std::string, menu_item>>& items, const std::string& name)
//{
//	for (auto it = items.begin(); it != items.end(); ++it)
//		if (it->first == name)
//			return it;
//
//	return items.end();
//}
//
//menu_item find_item(std::vector <std::vector <std::pair <std::string, menu_item>>>& scripts, const std::string& name)
//{
//	for (auto& script : scripts)
//	{
//		for (auto& item : script)
//		{
//			std::string item_name;
//
//			auto first_point = false;
//			auto second_point = false;
//
//			for (auto& c : item.first)
//			{
//				if (c == '.')
//				{
//					if (first_point)
//					{
//						second_point = true;
//						continue;
//					}
//					else
//					{
//						first_point = true;
//						continue;
//					}
//				}
//
//				if (!second_point)
//					continue;
//
//				item_name.push_back(c);
//			}
//
//			if (item_name == name)
//				return item.second;
//		}
//	}
//
//	return menu_item();
//}
//
//namespace ns_menu
//{
//	bool get_visible()
//	{
//		return core::s::is_opened;
//	}
//
//	void set_visible(bool visible)
//	{
//		core::s::is_opened = visible;
//	}
//	
//	Vector get_position()
//	{
//		return Vector(Lua_manager->window_pos.x, Lua_manager->window_pos.y, 0);
//	}
//
//	Vector get_size()
//	{
//		return Vector(Lua_manager->window_size.x, Lua_manager->window_size.y, 0);
//	}
//
//	auto next_line_counter = 0;
//
//	void next_line(sol::this_state s)
//	{
//		Lua_manager->items.at(get_current_script_id(s)).emplace_back(std::make_pair(crypt_str("next_line_") + std::to_string(next_line_counter), menu_item()));
//		++next_line_counter;
//	}
//
//	void add_check_box(sol::this_state s, const std::string& name)
//	{
//		auto script = get_current_script(s);
//		auto script_id = Lua_manager->get_script_id(script);
//
//		auto& items = Lua_manager->items.at(script_id);
//		auto full_name = script + '.' + name;
//
//		if (find_item(items, full_name) != items.end())
//			return;
//
//		items.emplace_back(std::make_pair(full_name, menu_item(false)));
//	}
//
//	void add_combo_box(sol::this_state s, std::string name, std::vector <std::string> labels) //-V813
//	{
//		if (labels.empty())
//			return;
//
//		auto script = get_current_script(s);
//		auto script_id = Lua_manager->get_script_id(script);
//
//		auto& items = Lua_manager->items.at(script_id);
//		auto full_name = script + '.' + name;
//
//		if (find_item(items, full_name) != items.end())
//			return;
//
//		items.emplace_back(std::make_pair(full_name, menu_item(labels, 0)));
//	}
//
//	void add_slider_int(sol::this_state s, const std::string& name, int min, int max)
//	{
//		auto script = get_current_script(s);
//		auto script_id = Lua_manager->get_script_id(script);
//
//		auto& items = Lua_manager->items.at(script_id);
//		auto full_name = script + '.' + name;
//
//		if (find_item(items, full_name) != items.end())
//			return;
//
//		items.emplace_back(std::make_pair(full_name, menu_item(min, max, min)));
//	}
//
//	void add_slider_float(sol::this_state s, const std::string& name, float min, float max)
//	{
//		auto script = get_current_script(s);
//		auto script_id = Lua_manager->get_script_id(script);
//
//		auto& items = Lua_manager->items.at(script_id);
//		auto full_name = script + '.' + name;
//
//		if (find_item(items, full_name) != items.end())
//			return;
//
//		items.emplace_back(std::make_pair(full_name, menu_item(min, max, min)));
//	}
//
//	void add_color_picker(sol::this_state s, const std::string& name)
//	{
//		auto script = get_current_script(s);
//		auto script_id = Lua_manager->get_script_id(script);
//
//		auto& items = Lua_manager->items.at(script_id);
//		auto full_name = script + '.' + name;
//
//		if (find_item(items, full_name) != items.end())
//			return;
//
//		items.emplace_back(std::make_pair(full_name, menu_item(Color::White)));
//	}
//
//	void add_button(sol::this_state s, const std::string& label)
//	{
//		auto script = get_current_script(s);
//		auto script_id = Lua_manager->get_script_id(script);
//
//		auto& items = Lua_manager->items.at(script_id);
//		auto full_name = script + '.' + label;
//
//		if (find_item(items, full_name) != items.end())
//			return;
//
//		items.emplace_back(std::make_pair(full_name, menu_item(label, sol::protected_function())));
//	}
//
//	void add_key_bind(sol::this_state s, const std::string& label)
//	{
//		auto script = get_current_script(s);
//		auto script_id = Lua_manager->get_script_id(script);
//
//		auto& items = Lua_manager->items.at(script_id);
//		auto full_name = script + '.' + label;
//
//		if (find_item(items, full_name) != items.end())
//			return;
//
//		items.emplace_back(std::make_pair(full_name, menu_item(label)));
//	}
//
//	void add_callback(std::string label, sol::protected_function func) //-V813
//	{
//		auto it = find_item(Lua_manager->items, label);
//
//		if (it.type == BUTTON)
//			it.m_callback = func;
//	}
//
//	std::unordered_map <std::string, bool> first_update;
//	std::unordered_map <std::string, menu_item> stored_values;
//	std::unordered_map <std::string, void*> config_items;
//
//	bool find_config_item(std::string name, int type) //-V813
//	{
//		if (config_items.find(name) == config_items.end())
//		{
//			auto found = false;
//			bool have_index = false;
//			int index = 0;
//			std::string search_name = name;
//			std::smatch matches;
//			const std::regex reg(crypt_str("(.+)\\[(\\d+)\\]$"));
//			if (std::regex_search(name, matches, reg)) {
//
//				have_index = true;
//				search_name = matches[1];
//				index = atoi(matches[2].str().c_str());
//
//				switch (type)
//				{
//				case ITEM_BOOL:
//					type = ITEM_BOOL_ARRAY;
//					break;
//				case ITEM_INT:
//					type = ITEM_INT_ARRAY;
//					break;
//				case ITEM_FLOAT_COLOR:
//					type = ITEM_FLOAT_COLOR_ARRAY;
//					break;
//				}
//			}
//
//
//			for (auto& item : config->items)
//			{
//				if (item.second.name == search_name && item.second.show)
//				{
//
//					if (item.second.type != type)
//					{
//						std::string type_str;
//						switch (item.second.type)
//						{
//						case ITEM_BOOL:
//							type_str = crypt_str("bool"); //-V1037
//							break;
//						case ITEM_FLOAT_COLOR:
//							type_str = crypt_str("color"); //-V1037
//							break;
//						case ITEM_BOOL_ARRAY:
//							type_str = crypt_str("bool");
//							break;
//						case ITEM_FLOAT_COLOR_ARRAY:
//							type_str = crypt_str("color");
//							break;
//						case ITEM_INT:
//							type_str = crypt_str("int");
//							break;
//						case ITEM_FLOAT:
//							type_str = crypt_str("float");
//							break;
//						case ITEM_COLOR:
//							type_str = crypt_str("color");
//							break;
//						case ITEM_KEY_BIND:
//							type_str = crypt_str("key_bind");
//							break;
//						default:
//							break;
//						}
//						logs->add(crypt_str("Lua error: invalid config item type, must be ") + type_str, Color::Red, crypt_str("[ LUA ] "), true);
//
//						return false;
//					}
//					if (have_index && item.second.array_size <= index) {
//						logs->add(crypt_str("Lua error: invalid config item index"), Color::Red, crypt_str("[ LUA ] "), true);
//						return false;
//					}
//
//					found = true;
//					if (have_index) {
//						switch (type)
//						{
//						case ITEM_BOOL_ARRAY:
//							config_items[name] = &(((bool*)(item.second.pointer.get()))[index]);
//							break;
//						case ITEM_INT_ARRAY:
//							config_items[name] = &(((int*)(item.second.pointer.get()))[index]);
//							break;
//						case ITEM_FLOAT_COLOR_ARRAY:
//							config_items[name] = &(((float(*)[4])(item.second.pointer.get()))[index]);
//							break;
//						}
//					}
//					else
//						config_items[name] = item.second.pointer.get();
//					break;
//				}
//			}
//
//			if (!found)
//			{
//				logs->add(crypt_str("Lua error: cannot find config variable \"") + name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//				return false;
//			}
//		}
//
//
//		return true;
//	}
//
//	bool get_bool(std::string name)
//	{
//		if (first_update.find(name) == first_update.end())
//			first_update[name] = false;
//
//		if (!core::s::is_opened && first_update[name])
//		{
//			if (stored_values.find(name) != stored_values.end())
//				return stored_values[name].check_box_value;
//			else if (config_items.find(name) != config_items.end())
//				return *(bool*)config_items[name];
//			else
//				return false;
//		}
//
//		auto it = find_item(Lua_manager->items, name);
//
//		if (it.type == NEXT_LINE) {
//			if (!find_config_item(name, ITEM_BOOL))
//				return false;
//			return *(bool*)config_items[name];
//		}
//
//
//
//		first_update[name] = true;
//		stored_values[name] = it;
//
//		return it.check_box_value;
//		//return 1;
//	}
//
//	int get_int(std::string name)
//	{
//		if (first_update.find(name) == first_update.end())
//			first_update[name] = false;
//
//		if (!core::s::is_opened && first_update[name])
//		{
//			if (stored_values.find(name) != stored_values.end())
//				return stored_values[name].type == COMBO_BOX ? stored_values[name].combo_box_value : stored_values[name].slider_int_value;
//			else if (config_items.find(name) != config_items.end())
//				return *(int*)config_items[name]; //-V206
//			else
//				return 0;
//		}
//
//		auto it = find_item(Lua_manager->items, name);
//
//		if (it.type == NEXT_LINE)
//		{
//			if (find_config_item(name, ITEM_INT))
//				return *(int*)config_items[name]; //-V206
//			logs->add(crypt_str("Lua error: cannot find menu variable \"") + name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//			return 0;
//		}
//
//		first_update[name] = true;
//		stored_values[name] = it;
//
//		return it.type == COMBO_BOX ? it.combo_box_value : it.slider_int_value;
//		//return 1;
//	}
//
//	float get_float(std::string name) //-V813
//	{
//		if (first_update.find(name) == first_update.end())
//			first_update[name] = false;
//
//		if (!core::s::is_opened && first_update[name])
//		{
//			if (stored_values.find(name) != stored_values.end())
//				return stored_values[name].slider_float_value;
//			else if (config_items.find(name) != config_items.end())
//				return *(float*)config_items[name];
//			else
//				return 0.0f;
//		}
//
//		auto it = find_item(Lua_manager->items, name);
//
//		if (it.type == NEXT_LINE)
//		{
//			if (find_config_item(name, ITEM_FLOAT))
//				return *(float*)config_items[name];
//			logs->add(crypt_str("Lua error: cannot find menu variable \"") + name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//			return 0.0f;
//		}
//
//		first_update[name] = true;
//		stored_values[name] = it;
//
//		return it.slider_float_value;
//		//return 1;
//	}
//
//	Color get_color(std::string name)
//	{
//		if (first_update.find(name) == first_update.end())
//			first_update[name] = false;
//
//		if (!core::s::is_opened && first_update[name])
//		{
//			if (stored_values.find(name) != stored_values.end())
//				return stored_values[name].color_picker_value;
//			else if (config_items.find(name) != config_items.end())
//				return *(Color*)config_items[name];
//			else
//				return Color::White;
//		}
//
//		auto it = find_item(Lua_manager->items, name);
//
//		if (it.type == NEXT_LINE)
//		{
//
//			if (find_config_item(name, ITEM_FLOAT_COLOR))
//			{
//				float* clr = (float*)config_items[name];
//				return Color(clr);
//			}
//
//			logs->add(crypt_str("Lua error: cannot find menu variable \"") + name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//			return Color::White;
//		}
//
//		first_update[name] = true;
//		stored_values[name] = it;
//
//		return it.color_picker_value;
//
//	}
//
//	bool get_key_bind_state(std::string name)
//	{
//		if (first_update.find(name) == first_update.end())
//			first_update[name] = false;
//
//		if (!core::s::is_opened && first_update[name])
//			if (config_items.find(name) != config_items.end())
//				return ((KeyBind*)config_items[name])->state;
//
//		auto it = find_item(Lua_manager->items, name);
//
//		if (it.type == NEXT_LINE)
//		{
//			if (find_config_item(name, ITEM_KEY_BIND))
//				return ((KeyBind*)config_items[name])->state;
//
//			logs->add(crypt_str("Lua error: cannot find menu variable \"") + name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//			return 0;
//		}
//
//		first_update[name] = true;
//		stored_values[name] = it;
//
//		return stored_values[name].m_bind.state;
//	}
//
//	int get_key_bind_mode(std::string name)
//	{
//		if (first_update.find(name) == first_update.end())
//			first_update[name] = false;
//
//		if (!core::s::is_opened && first_update[name])
//		{
//			if (stored_values.find(name) != stored_values.end())
//				return stored_values[name].m_bind.mode;
//			else if (config_items.find(name) != config_items.end())
//				return ((KeyBind*)config_items[name])->mode;
//			else
//				return 0.0f;
//		}
//
//		auto it = find_item(Lua_manager->items, name);
//
//		if (it.type == NEXT_LINE)
//		{
//			if (find_config_item(name, ITEM_KEY_BIND))
//				return ((KeyBind*)config_items[name])->mode;
//			logs->add(crypt_str("Lua error: cannot find menu variable \"") + name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//			return 0;
//		}
//
//		first_update[name] = true;
//		stored_values[name] = it;
//
//		return stored_values[name].m_bind.mode;
//	}
//
//	void set_bool(std::string name, bool value)
//	{
//		if (!find_config_item(name, ITEM_BOOL))
//			return;
//
//		*(bool*)config_items[name] = value;
//	}
//
//	void set_int(std::string name, int value)
//	{
//		if (!find_config_item(name, ITEM_INT))
//			return;
//
//		*(int*)config_items[name] = value; //-V206
//	}
//
//	void set_float(std::string name, float value)
//	{
//		if (!find_config_item(name, ITEM_FLOAT))
//			return;
//
//		*(float*)config_items[name] = value;
//	}
//
//	void set_color(std::string name, Color value) //-V813
//	{
//		if (!find_config_item(name, ITEM_FLOAT_COLOR))
//			return;
//
//		float* clr = (float*)config_items[name];
//		clr[0] = value.r() / 255.0f;
//		clr[1] = value.g() / 255.0f;
//		clr[2] = value.b() / 255.0f;
//		clr[3] = value.a() / 255.0f;
//
//	}
//}
//namespace ns_panorama
//{
//	void run_script(std::string script)
//	{
//		panorama->RunScript(script.c_str());
//	}
//}
//namespace ns_globals
//{
//	int get_ping()
//	{
//		return (int)(TICKS_TO_TIME(engine_prediction->latency) * 1000.0f);
//	}
//
//	std::string get_server_address()
//	{
//		if (!engine->IsInGame())
//			return crypt_str("Unknown");
//
//		auto nci = engine->GetNetChannelInfo();
//
//		if (!nci)
//			return crypt_str("Unknown");
//
//		auto server = nci->GetAddress();
//
//		if (crypt_hash_r(server) == crypt_hash("loopback"))
//			server = crypt_str("Local server");
//		else if (gamerules && gamerules->m_bIsValveDS())
//			server = crypt_str("Valve server");
//
//		return server;
//	}
//
//	std::string get_username()
//	{
//		return ctx->username;
//	}
//
//	float get_realtime()
//	{
//		return globals->realtime;
//	}
//
//	float get_curtime()
//	{
//		return globals->curtime;
//	}
//
//	float get_frametime()
//	{
//		return globals->frametime;
//	}
//
//	int get_tickcount()
//	{
//		return globals->tickcount;
//	}
//
//	float get_intervalpertick()
//	{
//		return globals->intervalpertick;
//	}
//
//	int get_maxclients()
//	{
//		return globals->maxclients;
//	}
//
//	int get_user_id()
//	{
//		return ctx->user_id;
//	}
//}
//
//namespace ns_engine
//{
//	static int width, height;
//
//	int get_screen_width()
//	{
//		engine->GetScreenSize(width, height);
//		return width;
//	}
//
//	int get_screen_height()
//	{
//		engine->GetScreenSize(width, height);
//		return height;
//	}
//
//	player_info_t get_player_info(int i)
//	{
//		player_info_t player_info;
//		engine->GetPlayerInfo(i, &player_info);
//
//		return player_info;
//	}
//
//	int get_player_for_user_id(int i)
//	{
//		return engine->GetPlayerForUserID(i);
//	}
//
//	int get_local_player_index()
//	{
//		return engine->GetLocalPlayer();
//	}
//
//	Vector get_view_angles()
//	{
//		Vector view_angles;
//		engine->GetViewAngles(view_angles);
//
//		return view_angles;
//	}
//
//	void set_view_angles(Vector view_angles)
//	{
//		engine->SetViewAngles(view_angles);
//	}
//
//	bool is_in_game()
//	{
//		return engine->IsInGame();
//	}
//
//	bool is_connected()
//	{
//		return engine->IsConnected();
//	}
//
//	std::string get_level_name()
//	{
//		return engine->GetLevelName();
//	}
//
//	std::string get_level_name_short()
//	{
//		return engine->GetLevelNameShort();
//	}
//
//	std::string get_map_group_name()
//	{
//		return engine->GetMapGroupName();
//	}
//
//	bool is_playing_demo()
//	{
//		return engine->IsPlayingDemo();
//	}
//
//	bool is_recording_demo()
//	{
//		return engine->IsRecordingDemo();
//	}
//
//	bool is_paused()
//	{
//		return engine->IsPaused();
//	}
//
//	bool is_taking_screenshot()
//	{
//		return engine->IsTakingScreenshot();
//	}
//
//	bool is_hltv()
//	{
//		return engine->IsHLTV();
//	}
//
//
//
//	sol::object trace_line(Vector vector, Vector to, sol::object skip, unsigned int mask, unsigned int type, sol::this_state s)
//	{
//		if (!engine->IsInGame())
//			return sol::make_object(sol::state_view(s), sol::nullopt);
//
//		LuaTraceFilter filter;
//		bool valid = skip.is<Entity*>() || skip.get_type() == sol::type::table || skip.get_type() == sol::type::function || skip.get_type() == sol::type::nil;
//		if (!valid) {
//			Lua_manager->unload_script(get_current_script_id(s));
//			logs->add(crypt_str("The \"skip\" parameter can only have the type Entity, table Entity or Function"), Color::Red, crypt_str("[ LUA ] "), true);
//			return sol::make_object(sol::state_view(s), sol::nullopt);
//		}
//		filter.m_filter = skip;
//		filter.m_script_id = get_current_script_id(s);
//		if (type < 0 || type > 3) {
//			Lua_manager->unload_script(get_current_script_id(s));
//			logs->add(crypt_str("\"type\" parameter can only be 0-3"), Color::Red, crypt_str("[ LUA ] "), true);
//			return sol::make_object(sol::state_view(s), sol::nullopt);
//		}
//
//		CGameTrace trace;
//		Ray_t ray;
//		ray.Init(vector, to);
//		enginetrace->TraceRay(ray, mask, &filter, &trace);
//
//		return sol::make_object(sol::state_view(s), trace);
//	}
//
//	sol::object trace_hull(Vector from, Vector to, Vector mins, Vector maxs, sol::object skip, unsigned int mask, unsigned int type, sol::this_state s)
//	{
//		if (!engine->IsInGame())
//			return sol::make_object(sol::state_view(s), sol::nullopt);
//
//		LuaTraceFilter filter;
//		bool valid = skip.is<Entity*>() || skip.get_type() == sol::type::table || skip.get_type() == sol::type::function || skip.get_type() == sol::type::nil;
//		if (!valid) {
//			Lua_manager->unload_script(get_current_script_id(s));
//			logs->add(crypt_str("The \"skip\" parameter can only have the type Entity, table Entity or Function"), Color::Red, crypt_str("[ LUA ] "), true);
//			return sol::make_object(sol::state_view(s), sol::nullopt);
//		}
//		filter.m_filter = skip;
//		filter.m_script_id = get_current_script_id(s);
//		if (type < 0 || type > 3) {
//			Lua_manager->unload_script(get_current_script_id(s));
//			logs->add(crypt_str("\"type\" parameter can only be 0-3"), Color::Red, crypt_str("[ LUA ] "), true);
//			return sol::make_object(sol::state_view(s), sol::nullopt);
//		}
//
//		CGameTrace trace;
//		Ray_t ray;
//		ray.Init(from, to, mins, maxs);
//		enginetrace->TraceRay(ray, mask, &filter, &trace);
//
//		return sol::make_object(sol::state_view(s), trace);
//	}
//
//	std::tuple<sol::object, sol::object> trace_bullet(Player* player, Vector from, Vector to, sol::object skip, sol::this_state s)
//	{
//		static LuaPenetration lua_penetration;
//
//		if (!engine->IsInGame())
//			return std::make_tuple(sol::make_object(sol::state_view(s), sol::nullopt), sol::make_object(sol::state_view(s), sol::nullopt));
//
//		if (!player)
//			return std::make_tuple(sol::make_object(sol::state_view(s), sol::nullopt), sol::make_object(sol::state_view(s), sol::nullopt));
//
//
//		LuaTraceFilter filter;
//		bool valid = skip.is<Entity*>() || skip.get_type() == sol::type::table || skip.get_type() == sol::type::function || skip.get_type() == sol::type::nil;
//		if (!valid) {
//			Lua_manager->unload_script(get_current_script_id(s));
//			logs->add(crypt_str("Incorrect use of the function engine.trace_bullet. See docs"), Color::Red, crypt_str("[ LUA ] "), true);
//			return std::make_tuple(sol::make_object(sol::state_view(s), sol::nullopt), sol::make_object(sol::state_view(s), sol::nullopt));
//
//		}
//
//
//		lua_penetration.m_skip = skip;
//		lua_penetration.m_script_id = get_current_script_id(s);
//		lua_penetration.from_entity = player;
//
//		auto damage = 0.0f;
//
//		if (!lua_penetration.fire_bullet(from, to, damage))
//			return std::make_tuple(sol::make_object(sol::state_view(s), sol::nullopt), sol::make_object(sol::state_view(s), sol::nullopt));
//
//		return std::make_tuple(sol::make_object(sol::state_view(s), damage), sol::make_object(sol::state_view(s), lua_penetration.enter_trace));
//
//	}
//}
//
//namespace ns_render
//{
//	Vector world_to_screen(const Vector& world)
//	{
//		Vector screen;
//
//		if (!render->world_to_screen(world, screen))
//			return Vector();
//
//		return screen;
//	}
//
//	int get_text_width(vgui::HFont font, const std::string& text)
//	{
//		return surface_render.text_width(font, text.c_str());
//	}
//
//	vgui::HFont create_font(const std::string& name, float size, float weight, std::optional <bool> antialias, std::optional <bool> dropshadow, std::optional <bool> outline)
//	{
//		DWORD flags = FONTFLAG_NONE;
//
//		if (antialias.value_or(false))
//			flags |= FONTFLAG_ANTIALIAS;
//
//		if (dropshadow.value_or(false))
//			flags |= FONTFLAG_DROPSHADOW;
//
//		if (outline.value_or(false))
//			flags |= FONTFLAG_OUTLINE;
//
//		auto font = surface->FontCreate();
//		surface->SetFontGlyphSet(font, name.c_str(), (int)size, (int)weight, 0, 0, flags);
//
//		return font;
//	}
//
//	void draw_text(vgui::HFont font, float x, float y, Color color, const std::string& text)
//	{
//		surface_render.text(font, (int)x, (int)y, color, HFONT_CENTERED_NONE, text.c_str());
//	}
//
//	void draw_text_centered(vgui::HFont font, float x, float y, Color color, bool centered_x, bool centered_y, const std::string& text)
//	{
//		DWORD centered_flags = HFONT_CENTERED_NONE;
//
//		if (centered_x)
//		{
//			centered_flags &= ~HFONT_CENTERED_NONE; //-V753
//			centered_flags |= HFONT_CENTERED_X;
//		}
//
//		if (centered_y)
//		{
//			centered_flags &= ~HFONT_CENTERED_NONE;
//			centered_flags |= HFONT_CENTERED_Y;
//		}
//
//		surface_render.text(font, (int)x, (int)y, color, centered_flags, text.c_str());
//	}
//
//	void draw_line(float x, float y, float x2, float y2, Color color)
//	{
//		surface_render.line((int)x, (int)y, (int)x2, (int)y2, color);
//	}
//
//	void draw_rect(float x, float y, float w, float h, Color color)
//	{
//		surface_render.rect((int)x, (int)y, (int)w, (int)h, color);
//	}
//
//	void draw_rect_filled(float x, float y, float w, float h, Color color)
//	{
//		surface_render.rect_filled((int)x, (int)y, (int)w, (int)h, color);
//	}
//
//	void draw_circle(float x, float y, float points, float radius, Color color)
//	{
//		surface_render.circle((int)x, (int)y, (int)points, (int)radius, color);
//	}
//
//	void draw_circle_filled(float x, float y, float points, float radius, Color color)
//	{
//		surface_render.circle_filled((int)x, (int)y, (int)points, (int)radius, color);
//	}
//
//	void draw_triangle(float x, float y, float x2, float y2, float x3, float y3, Color color)
//	{
//		surface_render.triangle(Vector2D(x, y), Vector2D(x2, y2), Vector2D(x3, y3), color);
//	}
//
//	void* create_image(sol::this_state s, std::string& data)
//	{
//		if (data.length() > 0) {
//			IDirect3DTexture9* texture;
//			D3DXCreateTextureFromFileInMemory(render->device.get(), data.c_str(), data.size(), &texture); //-V107
//			Img_lua temp;
//			temp.ptr = texture;
//			temp.script_id = get_current_script_id(s);
//			Lua_manager->imgs.emplace_back(temp);
//			return texture;
//		}
//		else
//			return nullptr;
//	}
//
//	void draw_image(int x, int y, int x2, int y2, void* img)
//	{
//		if (img != nullptr)
//			renderer_imgui.RenderImage(img, ImVec2(x, y), ImVec2(x2, y2));
//	}
//}
//
//namespace ns_console
//{
//	void execute(std::string& command)
//	{
//		if (command.empty())
//			return;
//
//		engine->ExecuteClientCmd(command.c_str());
//	}
//
//	std::unordered_map <std::string, ConVar*> convars;
//
//	bool get_bool(const std::string& convar_name)
//	{
//		if (convars.find(convar_name) == convars.end())
//		{
//			convars[convar_name] = cvar->FindVar(convar_name.c_str());
//
//			if (!convars[convar_name])
//			{
//				logs->add(crypt_str("Lua error: cannot find ConVar \"") + convar_name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//				return false;
//			}
//		}
//
//		if (!convars[convar_name])
//			return false;
//
//		return convars[convar_name]->GetBool();
//	}
//
//	int get_int(const std::string& convar_name)
//	{
//		if (convars.find(convar_name) == convars.end())
//		{
//			convars[convar_name] = cvar->FindVar(convar_name.c_str());
//
//			if (!convars[convar_name])
//			{
//				logs->add(crypt_str("Lua error: cannot find ConVar \"") + convar_name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//				return 0;
//			}
//		}
//
//		if (!convars[convar_name])
//			return 0;
//
//		return convars[convar_name]->GetInt();
//	}
//
//	float get_float(const std::string& convar_name)
//	{
//		if (convars.find(convar_name) == convars.end())
//		{
//			convars[convar_name] = cvar->FindVar(convar_name.c_str());
//
//			if (!convars[convar_name])
//			{
//				logs->add(crypt_str("Lua error: cannot find ConVar \"") + convar_name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//				return 0.0f;
//			}
//		}
//
//		if (!convars[convar_name])
//			return 0.0f;
//
//		return convars[convar_name]->GetFloat();
//	}
//
//	std::string get_string(const std::string& convar_name)
//	{
//		if (convars.find(convar_name) == convars.end())
//		{
//			convars[convar_name] = cvar->FindVar(convar_name.c_str());
//
//			if (!convars[convar_name])
//			{
//				logs->add(crypt_str("Lua error: cannot find ConVar \"") + convar_name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//				return crypt_str("");
//			}
//		}
//
//		if (!convars[convar_name])
//			return "";
//
//		return convars[convar_name]->GetString();
//	}
//
//	void set_bool(const std::string& convar_name, bool value)
//	{
//		if (convars.find(convar_name) == convars.end())
//		{
//			convars[convar_name] = cvar->FindVar(convar_name.c_str());
//
//			if (!convars[convar_name])
//			{
//				logs->add(crypt_str("Lua error: cannot find ConVar \"") + convar_name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//				return;
//			}
//		}
//
//		if (!convars[convar_name])
//			return;
//
//		if (convars[convar_name]->GetBool() != value)
//			convars[convar_name]->SetValue(value);
//	}
//
//	void set_int(const std::string& convar_name, int value)
//	{
//		if (convars.find(convar_name) == convars.end())
//		{
//			convars[convar_name] = cvar->FindVar(convar_name.c_str());
//
//			if (!convars[convar_name])
//			{
//				logs->add(crypt_str("Lua error: cannot find ConVar \"") + convar_name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//				return;
//			}
//		}
//
//		if (!convars[convar_name])
//			return;
//
//		if (convars[convar_name]->GetInt() != value)
//			convars[convar_name]->SetValue(value);
//	}
//
//	void set_float(const std::string& convar_name, float value)
//	{
//		if (convars.find(convar_name) == convars.end())
//		{
//			convars[convar_name] = cvar->FindVar(convar_name.c_str());
//
//			if (!convars[convar_name])
//			{
//				logs->add(crypt_str("Lua error: cannot find ConVar \"") + convar_name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//				return;
//			}
//		}
//
//		if (!convars[convar_name])
//			return;
//
//		if (convars[convar_name]->GetFloat() != value) //-V550
//			convars[convar_name]->SetValue(value);
//	}
//
//	void set_string(const std::string& convar_name, const std::string& value)
//	{
//		if (convars.find(convar_name) == convars.end())
//		{
//			convars[convar_name] = cvar->FindVar(convar_name.c_str());
//
//			if (!convars[convar_name])
//			{
//				logs->add(crypt_str("Lua error: cannot find ConVar \"") + convar_name + '\"', Color::Red, crypt_str("[ LUA ] "), true);
//
//				return;
//			}
//		}
//
//		if (!convars[convar_name])
//			return;
//
//		if (convars[convar_name]->GetString() != value)
//			convars[convar_name]->SetValue(value.c_str());
//	}
//}
//
//namespace ns_events
//{
//	void register_event(sol::this_state s, std::string event_name, sol::protected_function function)
//	{
//		if (std::find(ctx->events.begin(), ctx->events.end(), event_name) == ctx->events.end())
//		{
//			events->AddListener(events_manager.get(), event_name.c_str(), false);
//			ctx->events.emplace_back(event_name);
//		}
//
//		Lua_manager->events[get_current_script_id(s)][event_name] = function;
//	}
//}
//
//namespace ns_entitylist
//{
//	sol::optional <Player*> get_local_player()
//	{
//		if (!engine->IsInGame())
//			return sol::optional <Player*>(sol::nullopt);
//
//		return (Player*)entitylist->GetClientEntity(engine->GetLocalPlayer());
//	}
//
//	sol::optional <Entity*> get_entity_by_index(int i)
//	{
//		if (!engine->IsInGame())
//			return sol::optional <Entity*>(sol::nullopt);
//
//		return sol::optional< Entity*>((Entity*)entitylist->GetClientEntity(i));
//	}
//
//	sol::optional <Weapon*> get_weapon_by_player(sol::optional <Player*> player)
//	{
//		if (!engine->IsInGame())
//			return sol::optional <Weapon*>(sol::nullopt);
//
//		if (!player.has_value())
//			return sol::optional <Weapon*>(sol::nullopt);
//
//		return player.value()->m_hActiveWeapon().Get();
//	}
//
//	sol::optional <Player*> entity_to_player(sol::optional <Entity*> ent)
//	{
//		if (!engine->IsInGame())
//			return sol::optional <Player*>(sol::nullopt);
//		if (!ent.has_value())
//			return sol::optional <Player*>(sol::nullopt);
//
//		if (!ent.value()->is_player())
//			return sol::optional <Player*>(sol::nullopt);
//
//		return sol::optional <Player*>((Player*)ent.value());
//	}
//	sol::optional <Weapon*> entity_to_weapon(sol::optional <Weapon*> ent)
//	{
//		if (!engine->IsInGame())
//			return sol::optional <Weapon*>(sol::nullopt);
//		if (!ent.has_value())
//			return sol::optional <Weapon*>(sol::nullopt);
//
//		if (!ent.value()->is_weapon())
//			return sol::optional <Weapon*>(sol::nullopt);
//
//		return sol::optional <Weapon*>((Weapon*)ent.value());
//	}
//
//}
//
//namespace ns_utils
//{
//	uintptr_t find_signature(const std::string& szModule, const std::string& szSignature)
//	{
//		MODULEINFO module_info;
//		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(szModule.c_str()), &module_info, sizeof(MODULEINFO));
//
//		auto start_address = (DWORD)module_info.lpBaseOfDll;
//		auto end_address = start_address + module_info.SizeOfImage;
//
//		auto current_signature = szSignature.c_str();
//		auto address = (uintptr_t)0;
//
//		for (auto current_address = start_address; current_address < end_address; ++current_address)
//		{
//			if (!*current_signature)
//				break;
//
//			if (*(PBYTE)current_signature == '\?' || *(BYTE*)current_address == GETBYTE(current_signature))
//			{
//				if (!address)
//					address = current_address; //-V101
//
//				if (!current_signature[2])
//					break;
//
//				if (*(PWORD)current_signature == '\?\?' || *(PBYTE)current_signature != '\?')
//					current_signature += 3;
//				else
//					current_signature += 2;
//			}
//			else
//			{
//				current_signature = szSignature.c_str();
//				address = 0;
//			}
//		}
//
//		return address;
//	}
//
//	uintptr_t create_interface(const std::string& dllname, const std::string& interfacename)
//	{
//		CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(dllname.c_str()), crypt_str("CreateInterface"));
//		int returnCode = 0;
//		void* interface_ = CreateInterface(interfacename.c_str(), &returnCode);
//		return	reinterpret_cast<std::uintptr_t>(interface_);
//	}
//}
//
//namespace ns_http //new
//{
//	std::string get(sol::this_state s, std::string& host, std::string& link)
//	{
//		if (!config->scripts.allow_http)
//		{
//			Lua_manager->unload_script(get_current_script_id(s));
//			logs->add(crypt_str("Please, allow HTTP requests"), Color::Red, crypt_str("[ LUA ] "), true);
//
//			return "";
//		}
//
//		httplib::Client cli(host.c_str());
//		if (auto res = cli.Get(link.c_str())) {
//			if (res->status == 200) {
//				return  res->body;
//			}
//			else
//			{
//				logs->add(crypt_str("Server returned code: ") + std::to_string(res->status), Color::Red, crypt_str("[ LUA ] "), true);
//			}
//		}
//		else
//		{
//			logs->add(httplib::to_string(res.error()), Color::Red, crypt_str("[ LUA ] "), true);
//		}
//
//
//		return "";
//	}
//
//	std::string post(sol::this_state s, std::string& host, std::string& link, std::string& params, std::string& type)
//	{
//		if (!config->scripts.allow_http)
//		{
//			Lua_manager->unload_script(get_current_script_id(s));
//			logs->add(crypt_str("Please, allow HTTP requests"), Color::Red, crypt_str("[ LUA ] "), true);
//
//			return "";
//		}
//
//		httplib::Client cli(host.c_str());
//		if (auto res = cli.Post(link.c_str(), params.c_str(), type.c_str())) {
//			if (res->status == 200) {
//				return  res->body;
//			}
//			else
//			{
//				logs->add(crypt_str("Server returned code: ") + std::to_string(res->status), Color::Red, crypt_str("[ LUA ] "), true);
//			}
//		}
//		else
//		{
//			logs->add(httplib::to_string(res.error()), Color::Red, crypt_str("[ LUA ] "), true);
//		}
//
//
//		return "";
//
//	}
//}
//
//namespace ns_file //new
//{
//	void append(sol::this_state s, std::string& path, std::string& data)
//	{
//		if (!config->scripts.allow_file)
//		{
//			Lua_manager->unload_script(get_current_script_id(s));
//			logs->add(crypt_str("Please, allow files read or write"), Color::Red, crypt_str("[ LUA ] "), true);
//
//			return;
//		}
//
//		std::ofstream out(path, std::ios::app | std::ios::binary);
//
//		if (out.is_open())
//			out << data;
//		else
//			logs->add(crypt_str("Can't append to file: ") + path, Color::Red, crypt_str("[ LUA ] "), true);
//
//
//		out.close();
//	}
//	void write(sol::this_state s, std::string& path, std::string& data)
//	{
//		if (!config->scripts.allow_file)
//		{
//			Lua_manager->unload_script(get_current_script_id(s));
//			logs->add(crypt_str("Please, allow files read or write"), Color::Red, crypt_str("[ LUA ] "), true);
//
//			return;
//		}
//
//		std::ofstream out(path, std::ios::binary);
//
//		if (out.is_open())
//			out << data;
//
//
//		out.close();
//	}
//	std::string read(sol::this_state s, std::string& path)
//	{
//		if (!config->scripts.allow_file)
//		{
//			Lua_manager->unload_script(get_current_script_id(s));
//			logs->add(crypt_str("Please, allow files read or write"), Color::Red, crypt_str("[ LUA ] "), true);
//			return "";
//		}
//
//		std::ifstream file(path, std::ios::binary);
//
//		if (file.is_open())
//		{
//			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//			file.close();
//			return content;
//		}
//		else
//		{
//			logs->add(crypt_str("Can't read file: ") + path, Color::Red, crypt_str("[ LUA ] "), true);
//			file.close();
//			return "";
//		}
//		return "";
//	}
//}
//
//
//namespace ns_material
//{
//
//	void SetValue(sol::this_state s, IMaterial* mat, std::string& var_name, string& str)
//	{
//
//		auto found = false;
//		auto var = mat->FindVar(var_name.c_str(), &found);
//
//		if (found)
//			var->set_string_value(str.c_str());
//
//
//	}
//
//
//	IMaterial* find_matrial(sol::this_state s, std::string& name, std::string& material_name)
//	{
//		IMaterial* material = materialsystem->FindMaterial(material_name.c_str(), nullptr);
//		Material_lua temp;
//		temp.m_material = material;
//		temp.m_name = name;
//		temp.m_script = get_current_script_id(s);
//
//		int max = 0;
//
//		for (auto& mat : Lua_manager->materials)
//		{
//			if (max < mat.m_id)
//				max = mat.m_id;
//		}
//		temp.m_id = max + 1;
//		Lua_manager->materials.push_back(temp);
//
//
//		return material;
//	}
//
//	IMaterial* create_material_lua(sol::this_state s, std::string& name, std::string& data, bool flat)
//	{
//		Material_lua temp;
//		temp.m_name = name;
//		temp.m_script = get_current_script_id(s);
//		int max = 0;
//		for (auto& mat : Lua_manager->materials)
//		{
//			if (max < mat.m_id)
//				max = mat.m_id;
//		}
//		temp.m_id = max + 1;
//		std::string type = (flat) ? crypt_str("UnlitGeneric") : crypt_str("VertexLitGeneric");
//		std::string matdata = crypt_str("\"") + type + crypt_str("\"\n") + data + ("\n");
//		auto material = chams->create_material(!flat, matdata);
//		temp.m_material = material.get();
//
//		Lua_manager->materials.push_back(temp);
//		return material.get();
//	}
//}
//
//
//void print(sol::this_state s) //-V813
//{
//	auto L = s.lua_state();
//	std::string str;
//	std::size_t size = lua_gettop(L);
//	for (std::size_t i = 1; i <= size; ++i) {
//		size_t l;
//		const char* s = luaL_tolstring(L, i, &l);
//		if (i > 1)
//			str.append(crypt_str(" | "));
//		str.append(s, l);
//		lua_pop(L, 1);
//	}
//	logs->add(str, Color::White, crypt_str(""), true);
//}
//
//int setLuaPath(lua_State* L, const char* path)
//{
//	lua_getglobal(L, crypt_str("package"));
//	lua_getfield(L, -1, crypt_str("path")); // get field "path" from table at top of stack (-1)
//	std::string cur_path = lua_tostring(L, -1); // grab path string from top of stack
//	cur_path.append(crypt_str(";")); // do your path magic here
//	cur_path.append(path);
//	cur_path.append(crypt_str("?.lua"));
//	lua_pop(L, 1); // get rid of the string on the stack we just pushed on line 5
//	lua_pushstring(L, cur_path.c_str()); // push the new one
//	lua_setfield(L, -2, crypt_str("path")); // set the field "path" in table at -2 with value at top of stack
//	lua_pop(L, 1); // get rid of package table from top of stack
//	return 0; // al
//}
//
//void c_lua::initialize()
//{
//	refresh_scripts();
//}
//
//int c_lua::get_script_id(const std::string& name)
//{
//	for (auto i = 0; i < scripts.size(); i++)
//		if (scripts.at(i) == name) //-V106 //-V831
//			return i;
//
//	return -1;
//}
//
//int c_lua::get_script_id_by_path(const std::string& path)
//{
//	for (auto i = 0; i < pathes.size(); i++)
//		if (pathes.at(i).string() == path) //-V106 //-V831
//			return i;
//
//	return -1;
//}
//
//void c_lua::refresh_scripts()
//{
//	auto oldLoaded = loaded;
//	auto oldScripts = scripts;
//
//	loaded.clear();
//	pathes.clear();
//	scripts.clear();
//	ns_console::convars.clear();
//
//	std::string folder;
//	static TCHAR path[MAX_PATH];
//
//	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
//	{
//		folder = std::string(path) + crypt_str("\\Legendware\\Scripts\\");
//		CreateDirectory(folder.c_str(), NULL);
//
//		auto i = 0;
//
//		for (auto& entry : std::filesystem::directory_iterator(folder))
//		{
//			if (entry.path().extension() == crypt_str(".lua") || entry.path().extension() == crypt_str(".luac"))
//			{
//				auto path = entry.path();
//				auto filename = path.filename().string();
//
//				auto didPut = false;
//
//				for (auto i = 0; i < oldScripts.size(); i++)
//				{
//					if (filename == oldScripts.at(i)) //-V106 //-V831
//					{
//						loaded.emplace_back(oldLoaded.at(i)); //-V106
//						didPut = true;
//					}
//				}
//
//				if (!didPut)
//					loaded.emplace_back(false);
//
//				pathes.emplace_back(path);
//				scripts.emplace_back(filename);
//
//				items.emplace_back(std::vector <std::pair <std::string, menu_item>>());
//				++i;
//			}
//		}
//	}
//}
//
//void c_lua::load_script(int id)
//{
//	if (id == -1) 
//		return;
//	
//	if (loaded.at(id)) 
//		return;
//	
//	for (auto& state : Lua_manager->states)
//		if (state.id == id)
//			return;
//	
//	auto path = get_script_path(id);
//
//	if (path == crypt_str("")) 
//		return;
//	
//
//	auto error_load = false;
//	loaded.at(id) = true;
//	sol::state* l = new sol::state(sol::c_call <decltype(&lua_panic), &lua_panic>);
//	Lua_manager->init_state(*l);
//	Lua_manager->states.emplace_back(state{ l,id });
//	l->set_exception_handler(&my_exception_handler);
//	l->script(crypt_str("package.loaded[\"ffi\"] = nil"));
//	l->safe_script_file(path,
//		[&error_load](lua_State* L, sol::protected_function_result result)
//		{
//
//			if (!result.valid())
//			{
//				sol::error error = result;
//				auto log = crypt_str("Lua error: ") + (std::string)error.what();
//
//				logs->add(log, Color::Red, crypt_str("[ LUA ] "), true);
//				error_load = true;
//
//			}
//
//			return result;
//		}
//	);
//	
//	if (error_load || loaded.at(id) == false)
//	{
//		unload_script(id);
//		return;
//	}
//	
//	//-V106
//	ctx->loaded_script = true;
//}
//
//
//
//
//void c_lua::unload_script(int id)
//{
//	static std::mutex lock;
//	if (id == -1)
//		return;
//
//	if (!loaded.at(id)) //-V106
//		return;
//	
//	lock.lock();
//	std::vector<Material_lua> temp;
//
//	for (int i = 0; i < Lua_manager->materials.size(); i++)
//	{
//		if (Lua_manager->materials.at(i).m_script != id)
//			temp.push_back(Lua_manager->materials.at(i));
//	}
//	Lua_manager->materials.clear();
//	Lua_manager->materials = temp; //-V820
//	if (Lua_manager->events.find(id) != Lua_manager->events.end()) //-V807
//		Lua_manager->events.at(id).clear();
//
//	for (int i = 0; i < Lua_manager->fonts.size(); i++)
//	{
//		if (Lua_manager->fonts.at(i).script_id == id)
//			Lua_manager->fonts.erase(Lua_manager->fonts.begin() + i);
//	}
//	Lua_manager->now_unload = true;
//
//	Lua_manager->states.erase(
//		std::remove_if(Lua_manager->states.begin(), Lua_manager->states.end(), [&](auto const& state) {
//			return state.id == id;
//			}),
//		Lua_manager->states.end());
//
//
//
//
//	items.at(id).clear(); 
//	loaded.at(id) = false; 
//	Lua_manager->now_unload = false;
//	lock.unlock();
//}
//
//void c_lua::reload_all_scripts()
//{
//	for (auto current : scripts)
//	{
//		if (!loaded.at(get_script_id(current))) //-V106
//			continue;
//
//		unload_script(get_script_id(current));
//		load_script(get_script_id(current));
//	}
//}
//
//void c_lua::unload_all_scripts()
//{
//	for (auto s : scripts)
//		unload_script(get_script_id(s));
//}
//
//void c_lua::init_state(sol::state& lua)
//{
//	lua.open_libraries();
//
//	string folder;
//	TCHAR path[MAX_PATH];
//
//	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
//		folder = std::string(path) + crypt_str("\\Legendware\\Scripts\\");
//
//	lua[crypt_str("collectgarbage")] = sol::nil;
//	lua[crypt_str("dofilsse")] = sol::nil;
//	lua[crypt_str("load")] = sol::nil;
//	lua[crypt_str("loadfile")] = sol::nil;
//	lua[crypt_str("print")] = print;
//	lua[crypt_str("__nil_callback")] = sol::nil;
//
//	lua.new_usertype<Entity>(crypt_str("entity"),
//		(std::string)crypt_str("is_player"), &Entity::is_player,
//		(std::string)crypt_str("is_weapon"), &Entity::is_weapon,
//		(std::string)crypt_str("set_abs_angles"), &Entity::set_abs_angles,
//		(std::string)crypt_str("set_abs_origin"), &Entity::set_abs_origin,
//		(std::string)crypt_str("get_class_name"), &Entity::get_class_name,
//		(std::string)crypt_str("get_class_id"), &Entity::get_class_id,
//		(std::string)crypt_str("get_prop_bool"), &Entity::get_prop_bool,
//		(std::string)crypt_str("get_prop_int"), &Entity::get_prop_int,
//		(std::string)crypt_str("get_prop_float"), &Entity::get_prop_float,
//		(std::string)crypt_str("get_prop_string"), &Entity::get_prop_string,
//		(std::string)crypt_str("set_prop_bool"), &Entity::set_prop_bool,
//		(std::string)crypt_str("set_prop_int"), &Entity::set_prop_int,
//		(std::string)crypt_str("set_prop_float"), &Entity::set_prop_float,
//		(std::string)crypt_str("set_prop_string"), &Entity::set_prop_string
//		);
//
//	lua.new_usertype <Color>(crypt_str("color"), sol::constructors <Color(), Color(int, int, int), Color(int, int, int, int)>(),
//		(std::string)crypt_str("r"), &Color::r,
//		(std::string)crypt_str("g"), &Color::g,
//		(std::string)crypt_str("b"), &Color::b,
//		(std::string)crypt_str("a"), &Color::a
//		);
//
//	lua.new_usertype <Vector>(crypt_str("vector"), sol::constructors <Vector(), Vector(float, float, float)>(),
//		(std::string)crypt_str("x"), &Vector::x,
//		(std::string)crypt_str("y"), &Vector::y,
//		(std::string)crypt_str("z"), &Vector::z,
//		(std::string)crypt_str("length"), &Vector::Length,
//		(std::string)crypt_str("length_sqr"), &Vector::LengthSqr,
//		(std::string)crypt_str("length_2d"), &Vector::Length2D,
//		(std::string)crypt_str("length_2d_sqr"), &Vector::Length2DSqr,
//		(std::string)crypt_str("is_zero"), &Vector::IsZero,
//		(std::string)crypt_str("is_valid"), &Vector::IsValid,
//		(std::string)crypt_str("zero"), &Vector::Zero,
//		(std::string)crypt_str("dist_to"), &Vector::DistTo,
//		(std::string)crypt_str("dist_to_sqr"), &Vector::DistToSqr,
//		(std::string)crypt_str("cross_product"), &Vector::Cross,
//		(std::string)crypt_str("normalize"), &Vector::Normalize
//		);
//
//	lua.new_usertype <player_info_t>(crypt_str("player_info"),
//		(std::string)crypt_str("bot"), &player_info_t::fakeplayer,
//		(std::string)crypt_str("name"), &player_info_t::szName,
//		(std::string)crypt_str("steam_id"), &player_info_t::szSteamID
//		);
//
//	lua.new_usertype<Replace_mat_lua>(crypt_str("replace_material"),
//		(std::string)crypt_str("force_material"), &Replace_mat_lua::force_material,
//		(std::string)crypt_str("get_group_name"), &Replace_mat_lua::get_groupName
//		);
//
//	lua.new_usertype <Model_lua>(crypt_str("model"),
//		(std::string)crypt_str("get_entity"), &Model_lua::get_entity,
//		(std::string)crypt_str("get_name"), &Model_lua::get_name,
//		(std::string)crypt_str("force_material"), &Model_lua::ForcedMaterial,
//		(std::string)crypt_str("set_blend"), &Model_lua::SetBlend,
//		(std::string)crypt_str("draw_extra_pass"), &Model_lua::DrawExtraPass,
//		(std::string)crypt_str("color_modulate"), sol::overload(&Model_lua::color_modulate_lua_clr, &Model_lua::color_modulate_lua_float)
//
//		);
//
//	lua.new_usertype<IMaterial>(crypt_str("IMaterial"),
//		(std::string)crypt_str("get_name"), &IMaterial::GetName,
//		(std::string)crypt_str("get_texture_group_name"), &IMaterial::GetTextureGroupName,
//		(std::string)crypt_str("alpha_modulate"), &IMaterial::AlphaModulate,
//		(std::string)crypt_str("color_modulate"), &IMaterial::ColorModulate
//		);
//
//	lua.new_usertype <IGameEvent>(crypt_str("game_event"),
//		(std::string)crypt_str("get_bool"), &IGameEvent::GetBool,
//		(std::string)crypt_str("get_int"), &IGameEvent::GetInt,
//		(std::string)crypt_str("get_float"), &IGameEvent::GetFloat,
//		(std::string)crypt_str("get_string"), &IGameEvent::GetString,
//		(std::string)crypt_str("set_bool"), &IGameEvent::SetBool,
//		(std::string)crypt_str("set_int"), &IGameEvent::SetInt,
//		(std::string)crypt_str("set_float"), &IGameEvent::SetFloat,
//		(std::string)crypt_str("set_string"), &IGameEvent::SetString
//		);
//
//	lua.new_enum(crypt_str("hitboxes"),
//		crypt_str("head"), HITBOX_HEAD,
//		crypt_str("neck"), HITBOX_NECK,
//		crypt_str("pelvis"), HITBOX_PELVIS,
//		crypt_str("stomach"), HITBOX_STOMACH,
//		crypt_str("lower_chest"), HITBOX_LOWER_CHEST,
//		crypt_str("chest"), HITBOX_CHEST,
//		crypt_str("upper_chest"), HITBOX_UPPER_CHEST,
//		crypt_str("right_thigh"), HITBOX_RIGHT_THIGH,
//		crypt_str("left_thigh"), HITBOX_LEFT_THIGH,
//		crypt_str("right_calf"), HITBOX_RIGHT_CALF,
//		crypt_str("left_calf"), HITBOX_LEFT_CALF,
//		crypt_str("right_foot"), HITBOX_RIGHT_FOOT,
//		crypt_str("left_foot"), HITBOX_LEFT_FOOT,
//		crypt_str("right_hand"), HITBOX_RIGHT_HAND,
//		crypt_str("left_hand"), HITBOX_LEFT_HAND,
//		crypt_str("right_upper_arm"), HITBOX_RIGHT_UPPER_ARM,
//		crypt_str("right_forearm"), HITBOX_RIGHT_FOREARM,
//		crypt_str("left_upper_arm"), HITBOX_LEFT_UPPER_ARM,
//		crypt_str("left_forearm"), HITBOX_LEFT_FOREARM
//	);
//
//	lua.new_usertype <CUserCmd>(crypt_str("cmd"), sol::constructors <>(),
//		(std::string)crypt_str("command_number"), &CUserCmd::command_number,
//		(std::string)crypt_str("tickcount"), &CUserCmd::tickcount,
//		(std::string)crypt_str("viewangles"), &CUserCmd::viewangles,
//		(std::string)crypt_str("aimdirection"), &CUserCmd::aimdirection,
//		(std::string)crypt_str("forwardmove"), &CUserCmd::forwardmove,
//		(std::string)crypt_str("sidemove"), &CUserCmd::sidemove,
//		(std::string)crypt_str("upmove"), &CUserCmd::upmove,
//		(std::string)crypt_str("buttons"), &CUserCmd::buttons,
//		(std::string)crypt_str("impulse"), &CUserCmd::impulse,
//		(std::string)crypt_str("weaponselect"), &CUserCmd::weaponselect,
//		(std::string)crypt_str("random_seed"), &CUserCmd::random_seed,
//		(std::string)crypt_str("mousedx"), &CUserCmd::mousedx,
//		(std::string)crypt_str("mousedy"), &CUserCmd::mousedy,
//		(std::string)crypt_str("predicted"), &CUserCmd::predicted
//		);
//
//	lua.new_usertype <Player>(crypt_str("player"), sol::base_classes, sol::bases<Entity>(), //new
//		(std::string)crypt_str("get_index"), &Player::EntIndex,
//		(std::string)crypt_str("get_dormant"), &Player::IsDormant,
//		(std::string)crypt_str("get_team"), &Player::m_iTeamNum,
//		(std::string)crypt_str("get_velocity"), &Player::m_vecVelocity,
//		(std::string)crypt_str("get_origin"), &Player::GetAbsOrigin,
//		(std::string)crypt_str("get_angles"), &Player::m_angEyeAngles,
//		(std::string)crypt_str("has_helmet"), &Player::m_bHasHelmet,
//		(std::string)crypt_str("has_heavy_armor"), &Player::m_bHasHeavyArmor,
//		(std::string)crypt_str("is_scoped"), &Player::m_bIsScoped,
//		(std::string)crypt_str("get_health"), &Player::m_iHealth
//		);
//
//	lua.new_usertype <Weapon>(crypt_str("weapon"), sol::base_classes, sol::bases<Entity>(),
//		(std::string)crypt_str("can_fire"), &Weapon::can_fire,
//		(std::string)crypt_str("get_inaccuracy"), &Weapon::get_inaccuracy,
//		(std::string)crypt_str("get_spread"), &Weapon::get_spread
//		);
//
//	lua.new_enum(crypt_str("buttons"),
//		crypt_str("in_attack"), IN_ATTACK,
//		crypt_str("in_jump"), IN_JUMP,
//		crypt_str("in_duck"), IN_DUCK,
//		crypt_str("in_forward"), IN_FORWARD,
//		crypt_str("in_back"), IN_BACK,
//		crypt_str("in_use"), IN_USE,
//		crypt_str("in_cancel"), IN_CANCEL,
//		crypt_str("in_left"), IN_LEFT,
//		crypt_str("in_right"), IN_RIGHT,
//		crypt_str("in_moveleft"), IN_MOVELEFT,
//		crypt_str("in_moveright"), IN_MOVERIGHT,
//		crypt_str("in_attack2"), IN_ATTACK2,
//		crypt_str("in_run"), IN_RUN,
//		crypt_str("in_reload"), IN_RELOAD,
//		crypt_str("in_alt1"), IN_ALT1,
//		crypt_str("in_alt2"), IN_ALT2,
//		crypt_str("in_score"), IN_SCORE,
//		crypt_str("in_speed"), IN_SPEED,
//		crypt_str("in_walk"), IN_WALK,
//		crypt_str("in_zoom"), IN_ZOOM,
//		crypt_str("in_weapon1"), IN_WEAPON1,
//		crypt_str("in_weapon2"), IN_WEAPON2,
//		crypt_str("in_bullrush"), IN_BULLRUSH,
//		crypt_str("in_grenade1"), IN_GRENADE1,
//		crypt_str("in_grenade2"), IN_GRENADE2,
//		crypt_str("in_lookspin"), IN_LOOKSPIN
//	);
//
//	lua.new_usertype <ShotInfo>(crypt_str("shot_info"),
//		(std::string)crypt_str("result"), &ShotInfo::result,
//		(std::string)crypt_str("client_hitbox"), &ShotInfo::client_hitbox,
//		(std::string)crypt_str("server_hitbox"), &ShotInfo::server_hitbox,
//		(std::string)crypt_str("safe"), &ShotInfo::safe,
//		(std::string)crypt_str("target_index"), &ShotInfo::target_index,
//		(std::string)crypt_str("client_damage"), &ShotInfo::client_damage,
//		(std::string)crypt_str("server_damage"), &ShotInfo::server_damage,
//		(std::string)crypt_str("hitchance"), &ShotInfo::hitchance,
//		(std::string)crypt_str("backtrack_ticks"), &ShotInfo::backtrack_ticks,
//		(std::string)crypt_str("aim_point"), &ShotInfo::aim_point
//		);
//
//
//
//	lua.new_usertype <cplane_t>(crypt_str("plane"),
//		(std::string)crypt_str("normal"), &cplane_t::normal,
//		(std::string)crypt_str("dist"), &cplane_t::dist,
//		(std::string)crypt_str("type"), &cplane_t::type,
//		(std::string)crypt_str("signbits"), &cplane_t::signbits
//		);
//
//	lua.new_usertype <csurface_t>(crypt_str("surface"),
//		(std::string)crypt_str("name"), &csurface_t::name,
//		(std::string)crypt_str("props"), &csurface_t::surfaceProps,
//		(std::string)crypt_str("flags "), &csurface_t::flags
//		);
//
//	lua.new_usertype <CGameTrace>(crypt_str("trace"),
//		(std::string)crypt_str("start_pos"), &CGameTrace::startpos,
//		(std::string)crypt_str("end_pos"), &CGameTrace::endpos,
//		(std::string)crypt_str("plane"), &CGameTrace::plane,
//		(std::string)crypt_str("fraction"), &CGameTrace::fraction,
//		(std::string)crypt_str("contents"), &CGameTrace::contents,
//		(std::string)crypt_str("disp_flags"), &CGameTrace::dispFlags,
//		(std::string)crypt_str("all_solid"), &CGameTrace::allsolid,
//		(std::string)crypt_str("start_solid"), &CGameTrace::startsolid,
//		(std::string)crypt_str("fraction_left_solid"), &CGameTrace::fractionleftsolid,
//		(std::string)crypt_str("surface"), &CGameTrace::surface,
//		(std::string)crypt_str("hitgroup"), &CGameTrace::hitgroup,
//		(std::string)crypt_str("physics_bone"), &CGameTrace::physicsbone,
//		(std::string)crypt_str("world_surface_index"), &CGameTrace::worldSurfaceIndex,
//		(std::string)crypt_str("entity"), &CGameTrace::hit_entity,
//		(std::string)crypt_str("hitbox"), &CGameTrace::hitbox,
//			(std::string)crypt_str("did_hit"), &CGameTrace::DidHit,
//			(std::string)crypt_str("did_hit_world"), &CGameTrace::DidHitWorld,
//			(std::string)crypt_str("did_hit_non_world"), &CGameTrace::DidHitNonWorldEntity,
//			(std::string)crypt_str("is_visible"), &CGameTrace::IsVisible
//			);
//
//
//	auto client = lua.create_table();
//	client[crypt_str("add_callback")] = ns_client::add_callback;
//	client[crypt_str("load_script")] = ns_client::load_script;
//	client[crypt_str("unload_script")] = ns_client::unload_script;
//	client[crypt_str("log")] = ns_client::log;
//	client[crypt_str("get_script_name")] = get_current_script;
//
//
//	auto menu = lua.create_table();
//	menu[crypt_str("next_line")] = ns_menu::next_line;
//	menu[crypt_str("add_check_box")] = ns_menu::add_check_box;
//	menu[crypt_str("add_combo_box")] = ns_menu::add_combo_box;
//	menu[crypt_str("add_slider_int")] = ns_menu::add_slider_int;
//	menu[crypt_str("add_slider_float")] = ns_menu::add_slider_float;
//	menu[crypt_str("add_color_picker")] = ns_menu::add_color_picker;
//	menu[crypt_str("add_key_bind")] = ns_menu::add_key_bind;
//
//	menu[crypt_str("get_bool")] = ns_menu::get_bool;
//	menu[crypt_str("get_int")] = ns_menu::get_int;
//	menu[crypt_str("get_float")] = ns_menu::get_float;
//	menu[crypt_str("get_color")] = ns_menu::get_color;
//	menu[crypt_str("get_key_bind_state")] = ns_menu::get_key_bind_state;
//	menu[crypt_str("get_key_bind_mode")] = ns_menu::get_key_bind_mode;
//	menu[crypt_str("set_bool")] = ns_menu::set_bool;
//	menu[crypt_str("set_int")] = ns_menu::set_int;
//	menu[crypt_str("set_float")] = ns_menu::set_float;
//	menu[crypt_str("set_color")] = ns_menu::set_color;
//	menu[crypt_str("get_visible")] = ns_menu::get_visible;
//	menu[crypt_str("set_visible")] = ns_menu::set_visible;
//	menu[crypt_str("get_position")] = ns_menu::get_position;
//	menu[crypt_str("get_size")] = ns_menu::get_size;
//
//	auto globals = lua.create_table();
//	globals[crypt_str("get_ping")] = ns_globals::get_ping;
//	globals[crypt_str("get_server_address")] = ns_globals::get_server_address;
//	globals[crypt_str("get_username")] = ns_globals::get_username;
//	globals[crypt_str("get_realtime")] = ns_globals::get_realtime;
//	globals[crypt_str("get_curtime")] = ns_globals::get_curtime;
//	globals[crypt_str("get_frametime")] = ns_globals::get_frametime;
//	globals[crypt_str("get_tickcount")] = ns_globals::get_tickcount;
//	globals[crypt_str("get_intervalpertick")] = ns_globals::get_intervalpertick;
//	globals[crypt_str("get_maxclients")] = ns_globals::get_maxclients;
//	globals[crypt_str("get_user_id")] = ns_globals::get_user_id;
//
//	auto engine = lua.create_table();
//	engine[crypt_str("get_screen_width")] = ns_engine::get_screen_width;
//	engine[crypt_str("get_screen_height")] = ns_engine::get_screen_height;
//	engine[crypt_str("get_level_name")] = ns_engine::get_level_name;
//	engine[crypt_str("get_level_name_short")] = ns_engine::get_level_name_short;
//	engine[crypt_str("get_local_player_index")] = ns_engine::get_local_player_index;
//	engine[crypt_str("get_map_group_name")] = ns_engine::get_map_group_name;
//	engine[crypt_str("get_player_for_user_id")] = ns_engine::get_player_for_user_id;
//	engine[crypt_str("get_player_info")] = ns_engine::get_player_info;
//	engine[crypt_str("get_view_angles")] = ns_engine::get_view_angles;
//	engine[crypt_str("is_connected")] = ns_engine::is_connected;
//	engine[crypt_str("is_hltv")] = ns_engine::is_hltv;
//	engine[crypt_str("is_in_game")] = ns_engine::is_in_game;
//	engine[crypt_str("is_paused")] = ns_engine::is_paused;
//	engine[crypt_str("is_playing_demo")] = ns_engine::is_playing_demo;
//	engine[crypt_str("is_recording_demo")] = ns_engine::is_recording_demo;
//	engine[crypt_str("is_taking_screenshot")] = ns_engine::is_taking_screenshot;
//	engine[crypt_str("set_view_angles")] = ns_engine::set_view_angles;
//	engine[crypt_str("trace_line")] = sol::overload(
//		[](Vector vector, Vector to, sol::this_state s) {
//			auto skip = sol::make_object(sol::state_view(s), sol::nil);
//			return ns_engine::trace_line(vector, to, skip, MASK_ALL, (unsigned int)TraceType::TRACE_EVERYTHING, s);
//		}, [](Vector vector, Vector to, sol::lua_table table, sol::this_state s) {
//
//			auto skip = sol::make_object(sol::state_view(s), sol::nil);
//			skip = table.get_or<sol::object>(crypt_str("skip"), skip);
//			unsigned int mask = MASK_ALL;
//			mask = table.get_or<unsigned int>(crypt_str("mask"), mask);
//			unsigned int type = table.get_or<unsigned int>(crypt_str("type"), TraceType::TRACE_EVERYTHING);
//			return ns_engine::trace_line(vector, to, skip, mask, type, s);
//		}, [](Vector vector, Vector to, sol::object skip, sol::this_state s) {
//			return ns_engine::trace_line(vector, to, skip, MASK_ALL, (unsigned int)TraceType::TRACE_EVERYTHING, s);
//		}, [](Vector vector, Vector to, sol::object skip, unsigned int mask, sol::this_state s) {
//			return ns_engine::trace_line(vector, to, skip, mask, (unsigned int)TraceType::TRACE_EVERYTHING, s);
//		}, [](Vector vector, Vector to, sol::object skip, unsigned int mask, unsigned int type, sol::this_state s) {
//			return ns_engine::trace_line(vector, to, skip, mask, type, s);
//		}
//		);
//
//	engine[crypt_str("trace_hull")] = sol::overload(
//		[](Vector from, Vector to, Vector mins, Vector maxs, sol::this_state s) {
//			auto skip = sol::make_object(sol::state_view(s), sol::nil);
//			return ns_engine::trace_hull(from, to, mins, maxs, skip, MASK_ALL, (unsigned int)TraceType::TRACE_EVERYTHING, s);
//		},
//		[](Vector from, Vector to, Vector mins, Vector maxs, sol::lua_table table, sol::this_state s) {
//			auto skip = sol::make_object(sol::state_view(s), sol::nil);
//			skip = table.get_or<sol::object>(crypt_str("skip"), skip);
//			unsigned int mask = MASK_ALL;
//			mask = table.get_or<unsigned int>(crypt_str("mask"), mask);
//			unsigned int type = table.get_or<unsigned int>(crypt_str("type"), TraceType::TRACE_EVERYTHING);
//			return ns_engine::trace_hull(from, to, mins, maxs, skip, mask, type, s);
//		}, [](Vector from, Vector to, Vector mins, Vector maxs, sol::object skip, sol::this_state s) {
//			return ns_engine::trace_hull(from, to, mins, maxs, skip, MASK_ALL, (unsigned int)TraceType::TRACE_EVERYTHING, s);
//		}, [](Vector from, Vector to, Vector mins, Vector maxs, sol::object skip, unsigned int mask, sol::this_state s) {
//			return ns_engine::trace_hull(from, to, mins, maxs, skip, mask, (unsigned int)TraceType::TRACE_EVERYTHING, s);
//		}, [](Vector from, Vector to, Vector mins, Vector maxs, sol::object skip, unsigned int mask, unsigned int type, sol::this_state s) {
//			return ns_engine::trace_hull(from, to, mins, maxs, skip, mask, type, s);
//		}
//		);
//	engine[crypt_str("trace_bullet")] = ns_engine::trace_bullet;
//
//	auto render = lua.create_table();
//
//	render[crypt_str("world_to_screen")] = ns_render::world_to_screen;
//	render[crypt_str("get_text_width")] = ns_render::get_text_width;
//	render[crypt_str("create_font")] = ns_render::create_font;
//	render[crypt_str("draw_text")] = ns_render::draw_text;
//	render[crypt_str("draw_text_centered")] = ns_render::draw_text_centered;
//	render[crypt_str("draw_line")] = ns_render::draw_line;
//	render[crypt_str("draw_rect")] = ns_render::draw_rect;
//	render[crypt_str("draw_rect_filled")] = ns_render::draw_rect_filled;
//	render[crypt_str("draw_circle")] = ns_render::draw_circle;
//	render[crypt_str("draw_circle_filled")] = ns_render::draw_circle_filled;
//	render[crypt_str("draw_triangle")] = ns_render::draw_triangle;
//	render[crypt_str("create_image")] = ns_render::create_image;
//	render[crypt_str("draw_image")] = ns_render::draw_image;
//
//	auto console = lua.create_table();
//	console[crypt_str("execute")] = ns_console::execute;
//	console[crypt_str("get_int")] = ns_console::get_int;
//	console[crypt_str("get_float")] = ns_console::get_float;
//	console[crypt_str("get_string")] = ns_console::get_string;
//	console[crypt_str("set_int")] = ns_console::set_int;
//	console[crypt_str("set_float")] = ns_console::set_float;
//	console[crypt_str("set_string")] = ns_console::set_string;
//
//	auto events = lua.create_table(); //-V688
//	events[crypt_str("register_event")] = ns_events::register_event;
//
//	auto entitylist = lua.create_table();
//	entitylist[crypt_str("get_local_player")] = ns_entitylist::get_local_player;
//	entitylist[crypt_str("get_player_by_index")] = ns_entitylist::get_entity_by_index;
//	entitylist[crypt_str("get_weapon_by_player")] = ns_entitylist::get_weapon_by_player;
//	entitylist[crypt_str("entity_to_player")] = ns_entitylist::entity_to_player;
//	entitylist[crypt_str("entity_to_weapon")] = ns_entitylist::entity_to_weapon;
//
//	auto utils = lua.create_table();
//	utils[crypt_str("find_signature")] = ns_utils::find_signature;
//	utils[crypt_str("create_interface")] = ns_utils::create_interface;
//
//	auto http = lua.create_table();
//	http[crypt_str("get")] = ns_http::get;
//	http[crypt_str("post")] = ns_http::post;
//
//	auto file = lua.create_table();
//	file[crypt_str("append")] = ns_file::append;
//	file[crypt_str("write")] = ns_file::write;
//	file[crypt_str("read")] = ns_file::read;
//
//	auto material = lua.create_table();
//	material[crypt_str("find")] = ns_material::find_matrial;
//	material[crypt_str("create")] = ns_material::create_material_lua;
//	material[crypt_str("setvalue")] = ns_material::SetValue;
//
//	auto panorama = lua.create_table();
//	panorama[crypt_str("run_script")] = ns_panorama::run_script;
//
//	lua[crypt_str("client")] = client;
//	lua[crypt_str("menu")] = menu;
//	lua[crypt_str("globals")] = globals;
//	lua[crypt_str("engine")] = engine;
//	lua[crypt_str("render")] = render;
//	lua[crypt_str("console")] = console;
//	lua[crypt_str("events")] = events;
//	lua[crypt_str("entitylist")] = entitylist;
//
//	lua[crypt_str("utils")] = utils;
//
//	lua[crypt_str("http")] = http;
//	lua[crypt_str("file")] = file;
//	lua[crypt_str("material")] = material;
//	lua[crypt_str("panorama")] = panorama;
//
//	setLuaPath(lua.lua_state(), folder.c_str());
//}
//
//std::string c_lua::get_script_path(const std::string& name)
//{
//	return get_script_path(get_script_id(name));
//}
//
//std::string c_lua::get_script_path(int id)
//{
//	if (id == -1)
//		return crypt_str("");
//
//	return pathes.at(id).string(); //-V106
//}
//
//
//bool LuaPenetration::fire_bullet(const Vector& shoot_position, const Vector& point, float& current_damage)
//{
//	auto penetration_count = 4;
//
//	LuaTraceFilter filter;
//	memset(&enter_trace, 0, sizeof(enter_trace));
//
//	filter.m_filter = m_skip;
//	current_damage = this->get_weapon_data()->damage;
//
//	auto current_shoot_position = shoot_position;
//	auto current_distance = 0.0f;
//	auto max_range = this->get_weapon_data()->range;
//
//	while (current_damage >= 1.0f)
//	{
//		max_range -= current_distance;
//
//		auto direction = (point - shoot_position).Normalized();
//		auto end = current_shoot_position + direction * max_range;
//
//		Ray_t ray;
//		ray.Init(current_shoot_position, end);
//
//		LuaTraceFilter filter;
//		filter.m_filter = m_skip;
//
//		enginetrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enter_trace);
//
//		if (enter_trace.fraction == 1.0f) //-V550
//			break;
//
//		current_distance += enter_trace.fraction * max_range;
//		current_damage *= pow(ctx->weapon_data()->range_modifier, current_distance * 0.002f);
//
//		auto enter_surface_data = physicssurface->GetSurfaceData(enter_trace.surface.surfaceProps);
//
//		if (current_distance > 3000.0f && ctx->weapon_data()->penetration || enter_surface_data->game.flPenetrationModifier < 0.1f) //-V1051
//			break;
//
//		if (!handle_bullet_penetration(enter_trace, current_shoot_position, direction, penetration_count, current_damage))
//			break;
//
//		if (shoot_position.DistTo(current_shoot_position) >= shoot_position.DistTo(point))
//			return true;
//	}
//
//	return false;
//}
//
//bool LuaPenetration::handle_bullet_penetration(CGameTrace& enter_trace, Vector& shoot_position, const Vector& direction, int& penetration_count, float& current_damage)
//{
//	if (this->get_weapon_data()->penetration <= 0.0f)
//		return false;
//
//	if (penetration_count <= 0)
//		return false;	
//
//	auto contents_grate = enter_trace.contents & CONTENTS_GRATE;
//	auto surf_nodraw = enter_trace.surface.flags & SURF_NODRAW;
//
//	auto enter_surface_data = physicssurface->GetSurfaceData(enter_trace.surface.surfaceProps);
//	auto enter_material = enter_surface_data->game.material;
//
//	auto is_solid_surf = enter_trace.contents >> 3 & CONTENTS_SOLID;
//	auto is_light_surf = enter_trace.surface.flags >> 7 & SURF_LIGHT;
//
//	CGameTrace exit_trace;
//
//	if (!trace_to_exit(enter_trace, exit_trace, enter_trace.endpos, direction) && !(enginetrace->GetPointContents(enter_trace.endpos, MASK_SHOT_HULL) & MASK_SHOT_HULL))
//		return false;
//
//	auto enter_penetration_modifier = enter_surface_data->game.flPenetrationModifier;
//	auto exit_surface_data = physicssurface->GetSurfaceData(exit_trace.surface.surfaceProps);
//
//	if (!exit_surface_data)
//		return false;
//
//	auto exit_material = exit_surface_data->game.material;
//	auto exit_penetration_modifier = exit_surface_data->game.flPenetrationModifier;
//
//	auto combined_damage_modifier = 0.16f;
//	auto combined_penetration_modifier = (enter_penetration_modifier + exit_penetration_modifier) * 0.5f;
//
//	if (enter_material == CHAR_TEX_GLASS || enter_material == CHAR_TEX_GRATE)
//	{
//		combined_penetration_modifier = 3.0f;
//		combined_damage_modifier = 0.05f;
//	}
//	else if (contents_grate || surf_nodraw)
//		combined_penetration_modifier = 1.0f;
//	else if (enter_material == CHAR_TEX_FLESH && ((Player*)enter_trace.hit_entity)->m_iTeamNum() == ctx->local()->m_iTeamNum() && !convars_manager->convars[CONVAR_FF_DAMAGE_REDUCTION_BULLETS]->GetFloat())
//	{
//		if (!convars_manager->convars[CONVAR_FF_DAMAGE_BULLET_PENETRATION]->GetFloat()) //-V550
//			return false;
//
//		combined_penetration_modifier = convars_manager->convars[CONVAR_FF_DAMAGE_BULLET_PENETRATION]->GetFloat();
//		combined_damage_modifier = 0.16f;
//	}
//
//	if (enter_material == exit_material)
//	{
//		if (exit_material == CHAR_TEX_WOOD || exit_material == CHAR_TEX_CARDBOARD)
//			combined_penetration_modifier = 3.0f;
//		else if (exit_material == CHAR_TEX_PLASTIC)
//			combined_penetration_modifier = 2.0f;
//	}
//
//	auto penetration_modifier = max(0.0f, 1.0f / combined_penetration_modifier);
//	auto penetration_distance = (exit_trace.endpos - enter_trace.endpos).Length();
//
//	penetration_distance = penetration_distance * penetration_distance * penetration_modifier * 0.041666668f;
//
//	auto damage_modifier = max(0.0f, 3.0f / ctx->weapon_data()->penetration * 1.25f) * penetration_modifier * 3.0f + current_damage * combined_damage_modifier + penetration_distance;
//	auto damage_lost = max(0.0f, damage_modifier);
//
//	if (damage_lost > current_damage)
//		return false;
//
//	current_damage -= damage_lost;
//
//	if (current_damage < 1.0f)
//		return false;
//
//	shoot_position = exit_trace.endpos;
//	--penetration_count;
//
//	return true;
//}
//
//bool LuaTraceFilter::ShouldHitEntity(IHandleEntity* pEntityHandle, int contentsMask)
//{
//	if (m_filter == sol::nil)
//		return true;
//	if (m_filter.is<sol::function>()) {
//		auto result = m_filter.as<sol::function>()((Entity*)pEntityHandle, contentsMask);
//		if (!result.valid()) {
//			sol::error error = result;
//			auto log = crypt_str("Lua error: ") + (std::string)error.what();
//			logs->add(log, Color::Red, crypt_str("[ LUA ] "), true);
//			Lua_manager->unload_script(m_script_id);
//			return true;
//		}
//		sol::object ret_value = result;
//		if (!ret_value.is<bool>()) {
//			auto log = crypt_str("Lua error: filter should return a bool");
//			logs->add(log, Color::Red, crypt_str("[ LUA ] "), true);
//			Lua_manager->unload_script(m_script_id);
//			return true;
//		}
//		return ret_value.as<bool>();
//	}
//
//	if (m_filter.is<Entity*>())
//		return !(pEntityHandle == m_filter.as<Entity*>());
//
//	if (m_filter.get_type() == sol::type::table) {
//		sol::table table = m_filter;
//		for (auto& item : table) {
//			if (!item.second.is<Entity*>()) {
//				auto log = crypt_str("Lua error: the table should consist of Entity");
//				logs->add(log, Color::Red, crypt_str("[ LUA ] "), true);
//				Lua_manager->unload_script(m_script_id);
//				return true;
//			}
//
//			if (pEntityHandle == item.second.as<Entity*>())
//				return false;
//		}
//	}
//
//	return true;
//}
