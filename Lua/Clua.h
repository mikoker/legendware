//#pragma once
//
//
//#pragma comment(lib, "lua51.lib")
//#pragma comment(lib, "luajit.lib")
//
//#include "CLuaHook.h"
//#include "menu_item.h"
//#include "../gui/core/core.h"
//#include <filesystem>
//#include "../globals.h"
//#include "../directx_renderer/renderer.h"
//#include "../hooks/hooks.h"
//#include "../features/chams.h"
//#include "../features/penetration.h"
//struct Img_lua
//{
//	void* ptr;
//	int script_id;
//};
//struct Font_lua //-V730
//{
//	ImFont* m_font;
//	int script_id;
//};
//enum dme_type
//{
//	SetBlend_,
//	ForcedMaterial_,
//	Color_modulate_lua_,
//	DrawExtraPass_
//};
//struct dme_call //-V730
//{
//	dme_type type;
//	IMaterial* mat = nullptr;
//	float a = 0.0f;
//	Color m_clr;
//};
//
//struct Material_lua //-V730
//{
//	IMaterial* m_material = nullptr;
//	std::string m_name;
//	int m_script;
//	int m_id = 0;
//};
//enum draw_type
//{
//	DRAW_LINE,
//	DRAW_RECT,
//	DRAW_RECT_FILLED,
//	GET_TEXT_WIDTH,
//	DRAW_TEXT,
//	DRAW_RECT_FILLED_GRADIENT,
//	DRAW_CIRCLE,
//	DRAW_CIRCLE_FILLED,
//	DRAW_TRIANGLE,
//	DRAW_TRIANGLE_FILLED,
//	DRAW_CIRCLE_SECTOR,
//	CREATE_FONT
//	//draw_rect_filled
//};
//struct draw_elements //-V730
//{
//	draw_type m_type;
//	int m_x = 0;
//	int m_y = 0;
//	int m_x2 = 0;
//	int m_y2 = 0;
//	int m_x3 = 0;
//	int m_y3 = 0;
//
//	float m_points = 0;
//	float m_radius = 0;
//
//	int m_angle1 = 0;
//	int m_angle2 = 0;
//
//	int font_id = 0;
//	int script_id = 0;
//	std::string m_text = "";
//	float m_size = 0;
//	float m_weight = 0;
//
//	Color m_color1;
//	Color m_color2;
//	Color m_color3;
//	Color m_color4;
//
//};
//
//class state
//{
//public:
//	sol::state* ptr;
//	int id;
//	c_lua_hookManager hooks;
//	auto getHooks(std::string hook) { return hooks.getHooks(hook); }
//	~state() {
//		auto unload_hooks = hooks.getHooks(crypt_str("unload"));
//		for (auto& hook : unload_hooks)
//			hook.func();
//		hooks.unregisterHooks();
//	}
//};
//class c_lua
//{
//public:
//
//	void initialize();
//	void refresh_scripts();
//
//	void load_script(int id);
//	void unload_script(int id);
//
//	void reload_all_scripts();
//	void unload_all_scripts();
//
//	void init_state(sol::state &lua);
//
//	int get_script_id(const std::string& name);
//	int get_script_id_by_path(const std::string& path);
//
//
//
//	bool lock_copy;
//
//
//	std::vector <draw_elements> elements;
//	std::vector <bool> loaded;
//	std::vector <std::string> scripts;
//	std::vector <dme_call> dme_buf;
//	std::vector <std::vector <std::pair <std::string, menu_item>>> items;
//	std::vector<Font_lua> fonts;
//	std::vector<Material_lua> materials;
//	std::vector<Img_lua> imgs;
//	std::vector<state> states;
//	std::unordered_map <int, std::unordered_map <std::string, sol::protected_function>> events;
//	bool now_unload;
//	ImVec2 window_size = ImVec2(0,0);
//	ImVec2 window_pos = ImVec2(0, 0);
//	
//private:
//	std::string get_script_path(const std::string& name);
//	std::string get_script_path(int id);
//
//	std::vector <std::filesystem::path> pathes;
//};
//extern crypt_ptr <c_lua> Lua_manager;
//
//
//struct dme_info //-V730
//{
//	IMatRenderContext* context = nullptr;
//	DrawModelState_t state;
//	ModelRenderInfo_t info;
//	matrix3x4_t* bone_to_world = nullptr;
//};
//class Replace_mat_lua
//{
//	IMaterial* m_mat;
//	std::string m_groupName;
//public:
//	void force_material(IMaterial* mat)
//	{
//		m_mat = mat;
//	}
//	IMaterial* get_mat()
//	{
//		return m_mat;
//	}
//	std::string get_groupName()
//	{
//		return m_groupName;
//	}
//	void set_groupName(const std::string& name)
//	{
//		m_groupName = name;
//	}
//
//
//};
//class Model_lua
//{
//	Entity* m_ent;
//	std::string m_name;
//
//
//
//public:
//	Model_lua() {}; //-V730 //-V832
//	dme_info m_info;
//
//
//	Entity* get_entity()
//	{
//		return m_ent;
//	}
//	void ForcedMaterial(IMaterial* mat)
//	{
//
//
//		dme_call temp;
//		temp.type = ForcedMaterial_;
//		temp.mat = mat;
//
//		Lua_manager->dme_buf.emplace_back(temp);
//	}
//	std::string get_name()
//	{
//		return m_name;
//	}
//	void color_modulate_lua_clr(Color clr, IMaterial* mat)
//	{
//		dme_call temp;
//		temp.type = Color_modulate_lua_;
//		temp.mat = mat;
//		temp.m_clr = Color(clr);
//		Lua_manager->dme_buf.emplace_back(temp);
//	}
//
//	void color_modulate_lua_float(float* clr, IMaterial* mat)
//	{
//		dme_call temp;
//		temp.type = Color_modulate_lua_;
//		temp.mat = mat;
//		temp.m_clr = Color(clr);
//		Lua_manager->dme_buf.emplace_back(temp);
//
//	}
//
//	void set_entity(Entity* ent)
//	{
//		m_ent = ent;
//	}
//	void set_name(const std::string& name)
//	{
//		m_name = name;
//	}
//	void SetBlend(float a)
//	{
//		dme_call temp;
//		temp.type = SetBlend_;
//
//		a = a / 255.0f;
//		temp.a = a;
//		Lua_manager->dme_buf.emplace_back(temp);
//	}
//
//
//
//	void DrawExtraPass()
//	{
//		dme_call temp;
//		temp.type = DrawExtraPass_;
//		Lua_manager->dme_buf.emplace_back(temp);
//	}
//};
//
//class LuaTraceFilter : public ITraceFilter
//{
//public:
//	virtual bool ShouldHitEntity(IHandleEntity* pEntityHandle, int contentsMask);
//	virtual TraceType GetTraceType() const
//	{
//		return m_type;
//	}
//
//	TraceType m_type = TraceType::TRACE_EVERYTHING;
//	sol::object m_filter = sol::nil;
//
//	int m_script_id;
//};
//
//class LuaPenetration : public Penetration
//{
//	crypt_ptr <WeaponData> get_weapon_data()
//	{
//		if (!from_entity)
//			return nullptr;
//
//		auto weapon = crypt_ptr <Weapon>(from_entity->m_hActiveWeapon().Get());
//
//		if (!weapon)
//			return nullptr;
//
//		if (weapon->m_hOwnerEntity().Get() != from_entity)
//			return nullptr;
//
//		if (!weapon)
//			return nullptr;
//
//		return weapon->get_weapon_data();
//	}
//
//public:
//	virtual bool fire_bullet(const Vector& shoot_position, const Vector& point, float& current_damage);
//	virtual bool handle_bullet_penetration(CGameTrace& enter_trace, Vector& shoot_position, const Vector& direction, int& penetration_count, float& current_damage);
//	sol::object m_skip;
//	int m_script_id;
//	Player* from_entity;
//	CGameTrace enter_trace;
//};