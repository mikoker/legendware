#pragma once

#include "includes.h"
#include "sdk\classes.h"
#include "config\config.h"

class AnimationData;

struct CommandBackup
{
	int tickcount = 0;
	int old_tickcount = 0;

	float frametime = 0.0f;
	float curtime = 0.0f;
};

struct Command
{
	bool is_outgoing = false;
	bool is_used = false;

	int previous_command_number = 0;
	int command_number = 0;
};

struct DamageMarker
{
	int damage;
	float time;
	Vector position;
	Color color;

	DamageMarker(int damage, float time, Vector position, Color color) //-V818
	{
		this->damage = damage;
		this->time = time;
		this->position = position;
		this->color = color;
	}
};

struct ShotData
{
	bool processed;
	int command_number;

	float velocity_modifier;
	float time;

	ShotData(int command_number, float velocity_modifier, float time)
	{
		this->processed = false;
		this->command_number = command_number;
		this->velocity_modifier = velocity_modifier;
		this->time = time;
	}
};

class Ctx //-V730
{
public:
	crypt_ptr <Player> stored_local;
	crypt_ptr <AnimationState> stored_animation_state;
	crypt_ptr <Weapon> stored_weapon;
	crypt_ptr <WeaponData> stored_weapon_data;

	virtual void store();

	virtual crypt_ptr <Player> local();
	virtual crypt_ptr <AnimationState> animation_state();
	virtual crypt_ptr <Weapon> weapon();
	virtual crypt_ptr <WeaponData> weapon_data();

	bool setuping_bones = false;
	bool updating_animation = false;
	bool send_packet_fake_duck = false;
	bool force_send_packet = false;
	bool fake_ducking = false;
	bool visual_fake_ducking = false;
	bool freeze_period = false;
	bool automatic_peek = false;
	bool automatic_revolver = false;
	bool used_recoil_control_system = false;
	bool on_shot_chams = false;
	bool bomb_timer = false;
	bool need_update = false;
	bool clear_death_notices = false;
	bool reveal_ranks = false;
	bool loaded_script = false;

	bool arti_never_wanted_to_fix_this = false;
	bool friendly_fire = false;

	bool show_full_clantag = false;
	bool last_round_half = false;

	crypt_ptr <bool> send_packet;

	int last_predicted_command = 0;
	int tickcount = 0;
	int tickbase = 0;
	int last_tickbase = 0;
	int original_tickbase = 0;
	int original_choked_commands = 0;
	int buy_count = 0;
	int weapon_config = -1;
	int missed[65][11];
	int abs_missed[65];

	float spread = 0.0f;
	float inaccuracy = 0.0f;
	float original_forwardmove = 0.0f;
	float original_sidemove = 0.0f;
	float last_velocity_modifier = 1.0f;
	float max_speed = 0.0f;
	float interpolation = 0.0f;
	float field_of_view = 0.0f;
	float automatic_peek_radius = 0.0f;

	Vector shoot_position;
	Vector automatic_peek_position;

	CommandBackup backup_first;
	CommandBackup backup_second;

	matrix3x4_t fake[MAXSTUDIOBONES];
	matrix3x4_t prediction[MAXSTUDIOBONES];
	matrix3x4_t back_track[65][MAXSTUDIOBONES];

	vector <string> events;
	deque <int> last_latency;
	deque <Command> commands;
	deque <DamageMarker> damage_marker;
	deque <ShotData> shots_data;
	unordered_map <string, uintptr_t> props;

	Vector last_anti_aim_view_angle = Vector (0,0,0);
#ifdef RELEASE
	int user_id;

	string username;
	string days;
	string avatar;
	char token[33];
#else
	int user_id = -1;

	string username = crypt_str("OPAI");
	string days = crypt_str("Lifetime");
	string avatar = crypt_str("/data/avatars/o/0/3.jpg");
	char token[33] = crypt_str("");
#endif

#if !RELEASE && MININ
	Vector pos_skin_preview = Vector(0,0,0);
	float skin_rotation = 0;
	float radius = 0;
#endif //  !RELEASE && MININ

	bool mt_point_scan = true;
};

extern crypt_ptr <Ctx> ctx;