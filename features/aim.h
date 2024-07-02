#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"
#include "animations.h"
#include "penetration.h"

enum
{
	FIRST,
	LAST,
	BEST
};

enum
{
	HIT_GROUP_HEAD,
	HIT_GROUP_BODY,
	HIT_GROUP_CHEST,
	HIT_GROUP_LEGS,
	HIT_GROUP_ARMS,
};

struct PreparedTarget
{
	int index;
	AnimationData extrapolated_data;

	crypt_ptr <Player> player;
	crypt_ptr <deque <AnimationData>> data;

	PreparedTarget()
	{
		reset();
	}

	PreparedTarget(int index, crypt_ptr <Player> player) //-V818 //-V688
	{
		this->index = index;
		this->player = player;

		if (this->player)
			data = &animations->animation_data[index];
	}

	virtual void reset()
	{
		index = -1;

		player = nullptr;
		data = nullptr;
	}
};

struct Point
{
	int safe;
	float scale;
	float low_accuracy;
	float accuracy;
	Vector point;
	PenetrationInfo penetration_info;
	bool hit_chanced;

	Point()
	{
		safe = 0;
		scale = 0.0f;
		accuracy = 0.0f;
		low_accuracy = 0.0f;
		hit_chanced = false;
	}

	Point(const Vector& point, float scale) //-V818
	{
		this->safe = 0;
		this->scale = scale;
		this->point = point;
		this->hit_chanced = false;
	}
};
struct Hitbox
{
	bool prefer_safe;
	bool force_safe;
	int hitbox;

	vector <Point> points;
	Hitbox()
	{
		this->prefer_safe = 0;
		this->force_safe = 0;
		this->hitbox = -1;
	}

	Hitbox(bool prefer_safe, bool force_safe, int hitbox)
	{
		this->prefer_safe = prefer_safe;
		this->force_safe = force_safe;
		this->hitbox = hitbox;
	}
};
struct ThreadPoint
{
	crypt_ptr <Point> point;
	crypt_ptr <Player> player;
	crypt_ptr <Hitbox> hitbox;
	crypt_ptr <AnimationData> data;
	int minimum_damage;
	Vector angle;
	ThreadPoint(crypt_ptr <Point> point, crypt_ptr <Player> player, crypt_ptr <Hitbox> hitbox, crypt_ptr <AnimationData> data, int minimum_damage, Vector angle) //-V818
	{
		this->point = point;
		this->player = player;
		this->hitbox = hitbox;
		this->data = data;
		this->minimum_damage = minimum_damage;
		this->angle = angle;
	}
};

struct ThreadHit
{
	Vector point;

	crypt_ptr <Player> player;
	crypt_ptr <PenetrationInfo> penetration_info;

	ThreadHit(Vector point, crypt_ptr <Player> player, crypt_ptr <PenetrationInfo> penetration_info) //-V818
	{
		this->point = point;
		this->player = player;
		this->penetration_info = penetration_info;
	}
};



struct Target
{
	bool visible;

	int damage;
	int hitbox;
	Hitbox hitbox_s;
	int hitgroup;
	int penetration_count;

	Point point;

	crypt_ptr <Player> player;
	crypt_ptr <AnimationData> data;

	Target()
	{
		reset(); //-V1053
	}

	virtual void reset()
	{
		visible = true;

		damage = 0;
		hitbox = -1;
		hitgroup = -1;
		penetration_count = 0;

		player = nullptr;
		data = nullptr;
	}
};

struct ShotInfo
{
	string result = crypt_str("None");
	string client_hitbox = crypt_str("None");
	string server_hitbox = crypt_str("None");

	bool safe = false;

	int target_index = 0;
	int client_damage = 0;
	int server_damage = 0;

	int hitchance = 0;
	int backtrack_ticks = 0;

	Vector aim_point;
};

struct Shot
{
	bool safe = false;
	bool start = false;
	bool end = false;
	bool impacts = false;
	bool hurt = false;
	bool hurt_fake = false;
	bool impact_hit = false;
	bool occlusion = false;
	bool local_death = false;
	bool enemy_death = false;
	bool latency = false;

	int index = 0;
	int tickcount = 0;
	int event_tickcount = 0;
	int hitgroup = -1;

	float distance = 0.0f;

	Vector shoot_position;
	Vector last_impact;

	crypt_ptr <Player> player;
	AnimationData data;

	ShotInfo shot_info;
};

class Aim
{
	

	int additional_index = 0;
	float cock_time = 0.0f;

	vector <AnimationData> backup;
	vector <PreparedTarget> targets;

	

	virtual void automatic_revolver(crypt_ptr <CUserCmd> cmd);
	virtual void prepare();
	virtual void scan();
	virtual void fire(crypt_ptr <CUserCmd> cmd);
	virtual crypt_ptr <AnimationData> get_data(int type, crypt_ptr <deque <AnimationData>> data);

	bool clip_ray_to_hitbox(const Ray_t& ray, crypt_ptr <mstudiobbox_t> hitbox, matrix3x4_t& matrix, CGameTrace& trace);

	virtual void scan_hitboxes(crypt_ptr <Player> player, crypt_ptr <AnimationData> data);

	virtual float get_hit_chance(const Vector& angle, int matrix, int hitbox_index, crypt_ptr <Player> player, crypt_ptr <AnimationData> data);
	virtual Vector get_spread(int seed);

	virtual void extrapolate(crypt_ptr <Player> player, crypt_ptr <AnimationData> extrapolated_data, crypt_ptr <AnimationData> last_data, crypt_ptr <AnimationData> previous_data);
public:
	bool stop = false;
	bool early_stop = false;
	bool full_stop = false;
	bool skip_automatic_scope = false;
	bool jump_scout = false;
	bool optimized_scan = false;

	int ticks_to_stop = 0;

	float backup_forwardmove = 0.0f;
	float backup_sidemove = 0.0f;

	virtual void scan_hitboxes_new(crypt_ptr <Player> player, crypt_ptr <AnimationData> data, vector<Hitbox>& hitboxes);
	virtual void run(crypt_ptr <CUserCmd> cmd);
	virtual void automatic_stop(crypt_ptr <CUserCmd> cmd);
	virtual void get_points(vector <Point>& points, int hitbox_index, crypt_ptr <Player> player, crypt_ptr <AnimationData> data);
	virtual bool is_valid_head_point(crypt_ptr <Player> player, crypt_ptr <AnimationData> data, int matrix, const Vector& point);
	virtual bool hitbox_intersection(int hitbox_index, int matrix, crypt_ptr <Player> player, crypt_ptr <AnimationData> data, const Vector& end);
	virtual bool hitbox_equal(int first, int second);

	virtual float get_point_accuracy(const Vector& angle, int matrix, int hitbox_index, float spread, crypt_ptr <Player> player, crypt_ptr <AnimationData> data, bool debug  =false);
	virtual bool is_hit_chanced(float hit_chance, const Vector& angle, int matrix, int hitbox_index, crypt_ptr <Player> player, crypt_ptr <AnimationData> data, bool check_damage = false, int damage = 1);
	Target final_target;
};

extern vector <Shot> shots;