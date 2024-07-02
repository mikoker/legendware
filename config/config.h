#pragma once

#include "..\includes.h"
#include "..\directx_renderer\color.hpp"
#include "..\data.h"

enum
{
	ALWAYS_ON,
	HOLD,
	TOGGLE
};

enum
{
	ITEM_INVALID = -1,
	ITEM_BOOL,
	ITEM_FLOAT_COLOR,
	ITEM_BOOL_ARRAY,
	ITEM_INT_ARRAY,
	ITEM_FLOAT_COLOR_ARRAY,
	ITEM_INT,
	ITEM_FLOAT,
	ITEM_COLOR,
	ITEM_VECTOR_STRING,
	ITEM_KEY_BIND,
	ITEM_SKINS
};

enum Share_type {
	LINK,
	GIVE_TO_USER
};

enum Dependence
{
	DEPENDENCE_NONE,
	DEPENDENCE_LEGIT,
	DEPENDENCE_RAGE,
	DEPENDENCE_ANTI_AIM,
	DEPENDENCE_MAX
};

struct KeyBind
{
	bool state;
	bool holding;
	bool visible = true;

	int key;
	int mode;

	string name;
	Dependence dependence;

	KeyBind()
	{
		state = false;
		holding = false;
		key = KEY_NONE;
		mode = HOLD;
		dependence = DEPENDENCE_NONE;
	}

	KeyBind(const string& name, int mode, Dependence dependence = DEPENDENCE_NONE) //-V818
	{
		state = false;
		holding = false;
		key = KEY_NONE;

		this->mode = mode;
		this->name = name;
		this->dependence = dependence;
	}

	bool valid()
	{
		return key != KEY_NONE || mode == ALWAYS_ON;
	}
};

struct KeyBind_backup {
	bool state;
	bool holding;

	int key;
	int mode;
};

class ConfigItem
{
	union any_value {
		bool bool_value;
		int int_value;
		float float_value;
		KeyBind_backup key_bind_value;
		float float_clr[4];
		bool* bool_array_value;
	};
public:
	bool save;

	int type;
	int array_size;

	string name;
	crypt_ptr <void> pointer;

	any_value default_value;
	bool has_default_value = false;
	bool show = true;

	ConfigItem()
	{
		save = true;
		type = ITEM_INVALID;
		array_size = -1;
	}


	ConfigItem(const string& name, crypt_ptr <void> pointer, int type, int array_size, bool save = true, bool show = true)  //-V818
	{
		this->save = save;
		this->name = name; //-V820
		this->pointer = pointer;
		this->type = type; //-V820
		this->array_size = array_size;
		this->has_default_value = true;
		this->show = show;
		switch (type)
		{
		case ITEM_BOOL:
			default_value.bool_value = *(bool*)pointer.get();
			break;
		case ITEM_INT:
			default_value.int_value = *(int*)pointer.get();
			break;
		case ITEM_FLOAT:
			default_value.float_value = *(float*)pointer.get();
			break;
		case ITEM_KEY_BIND:
			default_value.key_bind_value.state = ((KeyBind*)pointer.get())->state;
			default_value.key_bind_value.holding = ((KeyBind*)pointer.get())->holding;
			default_value.key_bind_value.key = ((KeyBind*)pointer.get())->key;
			default_value.key_bind_value.mode = ((KeyBind*)pointer.get())->mode;
			break;
		case ITEM_BOOL_ARRAY:
			default_value.bool_array_value = new bool[array_size];
			memcpy(default_value.bool_array_value, pointer.get(), sizeof(bool) * array_size);
			break;
		case ITEM_FLOAT_COLOR:
			memcpy(default_value.float_clr, pointer.get(), sizeof(float) * 4);
			break;
		default:
			this->has_default_value = false;
			break;
		}
	}
};

enum
{
	CHECKS_FLASH,
	CHECKS_SMOKE,
	CHECKS_MAX
};

enum
{
	MODIFIERS_MOVE_BETWEEN_SHOTS,
	MODIFIERS_FORCE_ACCURACY,
	MODIFIERS_EARLY,
	MODIFIERS_MAX
};

enum
{
	FLAGS_HELMET,
	FLAGS_MONEY,
	FLAGS_DEFUSER,
	FLAGS_BOMB,
	FLAGS_RELOAD,
	FLAGS_ZOOM,
	FLAGS_FLASH,
	FLAGS_DUCK,
	FLAGS_EXPLOIT,
	FLAGS_PLANT,
	FLAGS_DEFUSE,
	FLAGS_IGNORE,
	FLAGS_MAX
};

enum
{
	REMOVALS_SCOPE,
	REMOVALS_ZOOM,
	REMOVALS_SMOKE,
	REMOVALS_FLASH,
	REMOVALS_RECOIL,
	REMOVALS_LANDING_BOB,
	REMOVALS_POSTPROCESSING,
	REMOVALS_FOGS,
	REMOVALS_MAX
};

enum
{
	OTHERS_ARMOR,
	OTHERS_KIT,
	OTHERS_TASER,
	OTHERS_HE,
	OTHERS_MOLOTOV,
	OTHERS_SMOKE,
	OTHERS_FLASHBANG,
	OTHERS_DECOY,
	OTHERS_MAX
};

enum
{
	AUTOMATIC_PEEK_SHOT,
	AUTOMATIC_PEEK_KEY,
	AUTOMATIC_PEEK_MAX
};

enum
{
	LOGS_HITS,
	LOGS_MISSES,
	LOGS_HURTS,
	LOGS_PURCHASES,
	LOGS_MAX
};

enum
{
	LOGS_POSITION_SCREEN,
	LOGS_POSITION_CONSOLE,
	LOGS_POSITION_MAX
};

enum
{
	BYPASS_SV_CHEATS,
	BYPASS_SV_PURE,
	BYPASS_INVENTORY_RESTRICTION,
	BYPASS_MAX
};

struct Login_packet
{
	char m_session[32] = { 0 };
	std::uint64_t m_req_type;
};
struct Loaded_info {
	int id = -1;
	int author = -1;
	Loaded_info() {
		id = -1;
		author = -1;
	}
};

struct ConfigInfo
{
	unsigned int id;
	char name[64] = { 0 };
	bool can_share;
	char cant_share_reason[128] = { 0 };
	unsigned int author;
};

struct Invite_info
{
	int id = -1;
	std::string name;
	std::string username;
	bool is_update = false;
	Invite_info(int id, std::string name, std::string username, bool is_update)
	{
		this->id = id;
		this->name = name;
		this->username = username;
		this->is_update = is_update;
	}
};



enum
{
	WEAPON_CONFIG_INVALID = -1,
	WEAPON_CONFIG_HEAVY_PISTOL,
	WEAPON_CONFIG_PISTOL,
	WEAPON_CONFIG_AUTO,
	WEAPON_CONFIG_SCOUT,
	WEAPON_CONFIG_AWP,
	WEAPON_CONFIG_RIFLE,
	WEAPON_CONFIG_SMG,
	WEAPON_CONFIG_HEAVY,
	WEAPON_CONFIG_MACHINE_GUN,
	WEAPON_CONFIG_TASER,
	WEAPON_CONFIG_KNIFE,
	WEAPON_CONFIG_MAX
};
enum Import_Status : int {
	NONE = -1,
	OK = 0,
	AUTH_ERROR,
	CONFIG_IS_NOT_FOUND,
	CONFIG_MAX_USED,
	CONFIG_LINK_IS_EXPIRED,
	UNKNOWN_ERROR,


};
class Config  //-V730
{
private:
	virtual void save_last_load_cfg(int id);
	virtual bool load_last_cfg();
public:
	vector <ConfigInfo> cfgs;
	vector <Invite_info> invites;
	unordered_map <string, ConfigItem> items;

	std::string default_cfg;

	virtual void initialize();
	virtual void add_item(void* pointer, const string& name, int type, int array_size = -1, bool save = true, bool show = true);

	virtual string items_to_json();
	virtual void json_to_items(const string& data);
	virtual std::string sha256(const std::string str);

	virtual void send_cfg(int id, const string& name = "");
	virtual void reset_cfg();
	virtual void create_cfg(const string& name);
	virtual string empty_cfg();

	virtual void get_cfgs();
	virtual void get_invites();
	virtual void load_cfg(int id);
	virtual void delete_cfg(int id);

	virtual void open_recucle_bin();
	virtual void open_share_url(int id);
	virtual void one_time_link(int id);

	virtual void share_cfg_invite(int id, std::string username);

	virtual void get_import_info(std::string link);
	virtual void get_by_link(std::string link);
	virtual void get_default();


	virtual void accept_invite(int id, std::string name);
	virtual void deny_invite(int id);

	virtual void import_cfg(std::string link, std::string name);

	std::vector<std::string> get_list_users(std::string username);
	bool processing = false;
	string process;

	bool load = false;
	string load_config;


	std::string link_code;

	Loaded_info m_loaded;
	int selected_config = -1;

	struct Rage
	{
		bool enable = false;
		bool silent = false;
		bool automatic_fire = false;
		bool automatic_wall = false;
		int field_of_view = 1;
		KeyBind double_tap_key = KeyBind(crypt_str("Double tap"), TOGGLE, DEPENDENCE_RAGE);
		KeyBind hide_shots_key = KeyBind(crypt_str("Hide shots"), TOGGLE, DEPENDENCE_RAGE);
		KeyBind force_damage_key = KeyBind(crypt_str("Force damage"), HOLD, DEPENDENCE_RAGE);

		struct RageWeapon
		{ //-V802
			bool automatic_stop = false;

			bool modifiers[MODIFIERS_MAX] =
			{
				false,
				false,
				false
			};

			bool extended_teleport = false;
			bool lag_exploit = false;
			int hit_chance = 0;
			int double_tap_hit_chance = 0;
			int minimum_damage = 0;
			int force_damage_value = 0;

			bool hitboxes[6] =
			{
				false,
				false,
				false,
				false,
				false,
				false
			};

			int body_aim = 0;

			int safe_hitboxes[6] =
			{
				0,
				0,
				0,
				0,
				0,
				0
			};

			bool static_point_scale = false;
			float head_scale = 0.0f;
			float body_scale = 0.0f;
			bool force_body_aim_if_lethal = false;
			bool force_safe_points_if_lethal = false;
		}
		weapon[WEAPON_CONFIG_MAX];
	}
	rage;

	struct Legit
	{
		bool enable = false;
		KeyBind enable_key = KeyBind(crypt_str("Legit"), HOLD, DEPENDENCE_LEGIT);
		KeyBind automatic_wall_key = KeyBind(crypt_str("Automatic wall"), HOLD, DEPENDENCE_LEGIT);
		KeyBind automatic_fire_key = KeyBind(crypt_str("Automatic fire"), HOLD, DEPENDENCE_LEGIT);
		bool checks[CHECKS_MAX] = { false, false };
		int backtrack_limit = 0;

		struct LegitWeapon
		{ //-V802
			float field_of_view = 1.0f;
			int smooth = 0;
			float smooth_value = 1.0f;
			int pitch_recoil = 0;
			int yaw_recoil = 0;
			int first_shot_delay = 0;
			int target_switch_delay = 0;

			bool hitboxes[6] =
			{
				false,
				false,
				false,
				false,
				false,
				false
			};

			int hitboxes_priority[6] =
			{
				0,
				0,
				0,
				0,
				0,
				0
			};

			bool automatic_wall = false;
			bool automatic_fire = false;
			bool automatic_attack = false;
			int fire_delay = 0;
			bool automatic_stop = false;
			int hit_chance = 0;
			int minimum_damage = 0;
			int delay = 0;
		}
		weapon[WEAPON_CONFIG_MAX];
	}
	legit;

	struct AntiAim
	{
		bool enable = false;
		bool enable_fake_lag = false;
		int fake_lag_type = 0;
		int fake_lag_limit = 1;
		int pitch = 0;
		int target_yaw = 0;
		bool freestanding = false;
		KeyBind freestanding_key = KeyBind(crypt_str("Freestanding"), ALWAYS_ON, DEPENDENCE_ANTI_AIM);
		int yaw_offset = 0;
		int yaw_modifier = 0;
		int yaw_modifier_range = 1;
		int freestanding_yaw_modifier = 0;
		int freestanding_yaw_modifier_range = 1;
		int desync_type = 0;
		int desync_range = 60;
		int desync_range_inverted = 60;
		int leg_movement = 0;
		bool roll = false;
		bool disable_on_high_velocity = false;
		int roll_range = 50;

		KeyBind manual_forward_key = KeyBind(crypt_str("Manual forward"), TOGGLE, DEPENDENCE_ANTI_AIM);
		KeyBind manual_back_key = KeyBind(crypt_str("Manual back"), TOGGLE, DEPENDENCE_ANTI_AIM);
		KeyBind manual_left_key = KeyBind(crypt_str("Manual left"), TOGGLE, DEPENDENCE_ANTI_AIM);
		KeyBind manual_right_key = KeyBind(crypt_str("Manual right"), TOGGLE, DEPENDENCE_ANTI_AIM);
		KeyBind invert_desync_key = KeyBind(crypt_str("Invert desync"), TOGGLE, DEPENDENCE_ANTI_AIM);
		KeyBind slow_walk_key = KeyBind(crypt_str("Slow walk"), HOLD, DEPENDENCE_ANTI_AIM);
		KeyBind fake_duck_key = KeyBind(crypt_str("Fake duck"), HOLD, DEPENDENCE_ANTI_AIM);
	}
	anti_aim;

	struct Visuals
	{
		struct player //-V730
		{

			bool chams_enable = false;

			bool chams_visible = false;
			float chams_clr_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			int chams_type_visible = 0;

			bool chams_modifiers_visible = false;
			bool chams_modifiers_arr_visible[3] = { false, false,false }; // glow, wirefreame
			float chams_modifiers_clr_visible[3][4] = {
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f }
			}; // glow, wirefreame

			bool chams_invisible = false;
			float chams_clr_invisible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			int chams_type_invisible = 0;

			bool chams_modifiers_invisible = false;
			bool chams_modifiers_arr_invisible[3] = { false, false,false }; // mettalic, glow, wirefreame
			float chams_modifiers_clr_invisible[3][4] = {
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f }
			}; //mettalic, glow, wirefreame


			bool chams_ragdoll = false;

			bool chams_shot = false;
			float chams_shot_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			int chams_shot_type = 0;
			bool chams_shot_modifiers = false;
			bool chams_shot_modifiers_arr[3] = { false, false,false }; // mettalic  glow, wirefreame
			float chams_shot_modifiers_clr[3][4] = {
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f }
			}; // mettalic glow, wirefreame

			bool chams_ghost = false;
			float chams_ghost_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			int chams_ghost_type = 0;
			bool chams_ghost_modifiers = false;
			bool chams_ghost_modifiers_arr[3] = { false, false,false }; // mettalic  glow, wirefreame
			float chams_ghost_modifiers_clr[3][4] = {
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f }
			}; // mettalic glow, wirefreame

			int transparency_in_scope = 0;
			bool chams_fake = false;
			bool layered = false;
			bool visualize_lag = false;
			float chams_fake_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			int chams_fake_type = 0;
			bool chams_fake_modifiers = false;
			bool chams_fake_modifiers_arr[3] = { false, false,false }; // mettalic  glow, wirefreame
			float chams_fake_modifiers_clr[3][4] = {
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f }
			}; // mettalic glow, wirefreame

			bool chams_hand = false;
			int chams_hand_type = 0;
			float chams_hand_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			bool chams_hand_modifiers = false;
			bool chams_hand_modifiers_arr[3] = { false, false,false }; // mettalic  glow, wirefreame
			float chams_hand_modifiers_clr[3][4] = {
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f }
			}; // mettalic glow, wirefreame

			bool chams_weapon = false;
			int chams_weapon_type = 0;
			float chams_weapon_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			bool chams_weapon_modifiers = false;
			bool chams_weapon_modifiers_arr[3] = { false, false,false }; // mettalic  glow, wirefreame
			float chams_weapon_modifiers_clr[3][4] = {
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f }
			}; // mettalic glow, wirefreame

			bool glow_enable = false;
			float glow_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			int glow_type = 0;

			bool esp_enable = false;
			bool esp_box = false;
			float esp_box_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			bool esp_name = false;
			float esp_name_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };;
			bool esp_hp_bar = false;
			bool esp_hp_bar_custom_color = false;
			float esp_hp_bar_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };;
			bool esp_skeleton = false;
			float esp_skeleton_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };;
			bool esp_flags = false;

			bool esp_flags_arr[FLAGS_MAX] = { false, false,	false, false, false, false, false, false, false, false, false, false };
			float esp_flags_combo_clr[FLAGS_MAX][4] =
			{
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f, 1.0f }
			};

			bool esp_weapon = false;
			float esp_weapon_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			float esp_knife_taser_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			bool esp_weapon_arr[2] = { 1, 1 };



			bool esp_ammo = false;
			float esp_ammo_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

			bool sounds = false;
			float sounds_color[4] = { 1,1,1,1 };
			float sounds_time = 1;
			int sounds_radius = 5;

#ifdef BETA
			bool multipoints = false;
#endif

		}enemy, local, team;

		struct World //-V730
		{
			struct world_color_modulate //-V730
			{ //-V802
				bool world = false;
				float world_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				float props_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			}world_color_modulate;
			struct Grenade_esp
			{ //-V802
				bool enable = false;
				bool box = false;
				float box_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				bool name = false;
				float name_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				bool name_types[2] = { 0 };
				bool prediction = false;
				float prediction_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				float prediction_points_clr[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
				float prediction_last_point_clr[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
				bool inferno_indicator = false;
				float inferno_indicator_clr[4] = { 1.0f, 0.0f, 0.0f, 0.58f };
				bool smoke_indicator = false;
				float smoke_indicator_clr[4] = { 1.0f, 1.0f, 1.0f, 0.58f };
			}grenade_esp;

			struct weapon //-V730
			{ //-V802
				bool esp = false;
				bool box = false;
				float box_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				bool ammo = false;
				float ammo_clr[4] = { 1.0f,1.0f,1.0f,1.0f };
				bool ammo_type[2] = { false,false };


				bool chams = false;
				float chams_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				int chams_type = 0;
				bool chams_modifer = false;
				bool chams_modifiers_arr[3] = { false,false,false }; // glow, wirefreame
				float chams_modifiers_clr[3][4] = {
					{ 1.0f, 1.0f, 1.0f, 1.0f },
					{ 1.0f, 1.0f, 1.0f, 1.0f },
					{ 1.0f, 1.0f, 1.0f, 1.0f }
				};

				bool glow = false;
				float glow_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				int glow_type = 0;

				bool icon = false;
				float icon_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			}weapon, nade, c4;

			bool removals[REMOVALS_MAX] = { false, false, false, false, false, false, false, false };

			bool manual_anti_aim = false;
			float manual_anti_aim_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

			bool offscreen_enable = false;
			float offscreen_color[4] = { 1,1,1,1 };
			float offscreen_color_out[4] = { 1,1,1,1 };
			bool offscreen_fade = false;
			int offscreen_distance = 40;
			int offscreen_size = 15;
			bool force_crosshair = false;
			bool penetration_crosshair = false;
			bool client_bullet_impacts = false;
			float client_bullet_impacts_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			bool server_bullet_impacts = false;
			float server_bullet_impacts_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			bool bullet_tracer = false;
			float bullet_tracer_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			bool hit_sound = false;
			bool damage_marker = false;
			bool damage_marker_custom_color = false;
			float damage_marker_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			bool bomb_timer = false;
			int skybox = 0;
		}world;

	}
	visuals;

	struct Misc
	{
		bool anti_untrusted = true;
		bool automatic_jump = false;
		int automatic_strafe = 0;
		int smoothing = 60;
		bool fast_stop = false;
		bool infinite_duck = false;
		KeyBind automatic_peek_key = KeyBind(crypt_str("Automatic peek"), HOLD);
		bool automatic_peek[AUTOMATIC_PEEK_MAX] = { true, false };
		float automatic_peek_clr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		bool rank_reveal = false;
		bool automatic_accept = false;
		bool clan_tag = false;
		bool adblock = false;
		bool display_lw_users = false;
		bool logs[LOGS_MAX] = { false, false, false, false };
		float logs_color[LOGS_MAX][4] =
		{
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f }
		};
		bool logs_position[LOGS_POSITION_MAX] = { true, true };
		bool bypasses[BYPASS_MAX] = { false, false, false };
		bool preserve_killfeed = false;
		bool automatic_buy = false;
		int primary = 0;
		int secondary = 0;
		bool others[OTHERS_MAX] = { false, false, false, false, false, false, false, false };
		int field_of_view = 90;
		bool viewmodel_changer = false;
		int viewmodel_field_of_view = 68;
		int viewmodel_x = 0;
		int viewmodel_y = 0;
		int viewmodel_z = 0;
		int viewmodel_roll = 0;
		bool aspect_ratio = false;
		float aspect_ratio_value = 1.0f;
		KeyBind edge_jump_key = KeyBind(crypt_str("Edge jump"), HOLD);
		KeyBind third_person_key = KeyBind(crypt_str("Third-person"), TOGGLE);
		int third_person_distance = 100;
		bool watermark = false;
		bool key_binds = false;
		bool spectator_list = false;

	}
	misc;

	struct PlayerList
	{
		bool updating = false;

		vector <string> players;
		vector <int> indexes;

		vector <string> cached_players;
		vector <int> cached_indexes;

		struct PlayerSettings
		{ //-V802
			bool ignore = false;

			bool force_pitch = false;
			int pitch = 0;

			bool force_body_yaw = false;
			int body_yaw = 0;

			bool force_roll = false;
			int roll = 0;

			bool force_body_aim = false;
			bool force_safe_points = false;

			bool hightlight = false;
			float hightlight_color[4] = { 1,1,1,1 };
			float hightlight_color_out[4] = { 1,1,1,1 };
			bool hightlight_fade = false;
			int hightlight_distance = 40;
			int hightlight_size = 15;

			void reset()
			{
				ignore = false;

				force_pitch = false;
				pitch = 0;

				force_body_yaw = false;
				body_yaw = 0;

				force_roll = false;
				roll = 0;

				force_body_aim = false;
				force_safe_points = false;

				hightlight = false;

				for (int i = 0; i < 4; i++) {
					hightlight_color[i] = 1;
					hightlight_color_out[i] = 1;
				}

				hightlight_fade = false;
				hightlight_distance = 40;
				hightlight_size = 15;
			}
		}
		player_settings[65];
	}
	player_list;

	struct Scripts
	{
		bool developer_mode = false;
		bool allow_http = false;
		bool allow_file = false;
		vector <string> scripts;
	}
	scripts;

	bool skins_auto_equip = false;
	bool skins_stattrak_auto_update = false;
	bool skins_custom_color = false;
};

extern crypt_ptr <Config> config;