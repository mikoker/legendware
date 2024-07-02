#pragma once

#include "..\includes.h"
#include "..\render.h"
#include "..\config\config.h"

struct Flags
{
	std::string m_text;
	Color m_clr;

	Flags(const std::string& text, const Color& clr) { //-V818
		m_text = text;
		m_clr = clr;
	}
};
enum
{
	ENEMY,
	LOCAL,
	TEAM
};
struct Box
{
	int x;
	int y;
	int w;
	int h;
};
class RadarPlayer
{
public:
	Vector pos; //0x0000
	Vector angle; //0x000C
	Vector spotted_map_angle_related; //0x0018
	DWORD tab_related; //0x0024
	char pad_0x0028[0xC]; //0x0028
	float spotted_time; //0x0034
	float spotted_fraction; //0x0038
	float time; //0x003C
	char pad_0x0040[0x4]; //0x0040
	__int32 player_index; //0x0044
	__int32 entity_index; //0x0048
	char pad_0x004C[0x4]; //0x004C
	__int32 health; //0x0050
	char name[32]; //0x785888
	char pad_0x0074[0x75]; //0x0074
	unsigned char spotted; //0x00E9
	char pad_0x00EA[0x8A]; //0x00EA
};
struct offscreen_data
{
	std::array <Vector2D, 3> points;
	bool is_valid = false;

};
class CCSGO_HudRadar
{
public:
	char pad_0x0000[0x14C];
	RadarPlayer radar_info[65];
};
class Esp //-V730
{
	crypt_ptr <Player> player;
	int i = -1;
	float prev_player_hp[65] = { 0 };
	float prev_armor_hp[65] = { 0 };
	float prev_ammo_hp[65] = { 0 };

	float old_hp[65] = { 0 };
	float old_armor[65] = { 0 };
	float old_ammo[65] = { 0 };


	float alpha_hp[65] = { 0 };
	float alpha_ammo[65] = { 0 };
	float alpha_armor[65] = { 0 };
	//float alpha_ammo[65] = { 0 };
	float alpha_name[65] = { 0 };
	float alpha_flags[65][FLAGS_MAX] = { 0 };
	float alpha_box[65] = { 0 };
	float alpha_skeleton[65] = { 0 };
	float alpha_weapon[65] = { 0 };

	float alpha_oof_alpha[65] = { 0 };

	int healths[65] = { 0 };

	int player_is = -1;
	bool flags_current[FLAGS_MAX] = { 0 };
	bool esp_weapon_arr_current[2] = { 0 };
	void box();
	void name();
	void health();
	void flags();
	void weapon();
	void draw_skeleton();

	virtual void ammo();
	virtual Color alpha_flags_set(int FLAG);
	virtual bool get_bbox(crypt_ptr <Player> e, Box& box);
	Box boxes[65];
	offscreen_data offscreen[65];



public:
	int weapons[65] = { 0 };
	float esp_alpha_fade[65] = { 0 };
	virtual void reset_animation(int i);
	Vector dormant_origin[65];
	Vector origin[65];
	virtual void run();
};