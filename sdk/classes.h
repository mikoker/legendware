#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "ItemSchema.h"
#include <filesystem>
class Weapon;
class Viewmodel;

struct ClientHitVerify
{
	Vector position;
	float time;
	float expires;
};
struct CAddonModel
{
	CBaseHandle m_hEnt;	// The model for the addon.
	int m_iAddon;						// One of the ADDON_ bits telling which model this is.
	int m_iAttachmentPoint;				// Which attachment point on the player model this guy is on.
};
struct RenderableInfo
{
	IClientRenderable* renderable;
	char pad[0x12];
	uint16_t flags;
	uint16_t flags2;
};

struct AnimstatePoseParamCache
{
	bool m_initialized;
	int m_index;
	const char* m_name;

	AnimstatePoseParamCache()
	{
		m_initialized = false;
		m_index = -1;
		m_name = "\0";
	}
};

class AnimationState //-V690
{
public:
	AnimationState& operator=(const AnimationState& animation_state)
	{
		memcpy(this, &animation_state, sizeof(AnimationState));
		return *this;
	}

	int* layer_order_preset = nullptr;
	bool first_run_since_init = false;
	bool first_foot_plant_since_init = false;
	int last_update_tick = 0;
	float eye_position_smooth_lerp = 0.0f;
	float strafe_change_weight_smooth_fall_off = 0.0f;
	float stand_walk_duration_state_has_been_valid = 0.0f;
	float stand_walk_duration_state_has_been_invalid = 0.0f;
	float stand_walk_how_long_to_wait_until_transition_can_blend_in = 0.0f;
	float stand_walk_how_long_to_wait_until_transition_can_blend_out = 0.0f;
	float stand_walk_blend_value = 0.0f;
	float stand_run_duration_state_has_been_valid = 0.0f;
	float stand_run_duration_state_has_been_invalid = 0.0f;
	float stand_run_how_long_to_wait_until_transition_can_blend_in = 0.0f;
	float stand_run_how_long_to_wait_until_transition_can_blend_out = 0.0f;
	float stand_run_blend_value = 0.0f;
	float crouch_walk_duration_state_has_been_valid = 0.0f;
	float crouch_walk_duration_state_has_been_invalid = 0.0f;
	float crouch_walk_how_long_to_wait_until_transition_can_blend_in = 0.0f;
	float crouch_walk_how_long_to_wait_until_transition_can_blend_out = 0.0f;
	float crouch_walk_blend_value = 0.0f;
	int cached_model_index = 0;
	float step_height_left = 0.0f;
	float step_height_right = 0.0f;
	Weapon* weapon_last_bone_setup = nullptr;
	Player* player = nullptr;//0x0060
	Weapon* weapon = nullptr;//0x0064
	Weapon* weapon_last = nullptr;//0x0068
	float last_update_time = 0.0f;//0x006C
	int last_update_frame = 0;//0x0070
	float last_update_increment = 0.0f;//0x0074
	float eye_yaw = 0.0f; //0x0078
	float eye_pitch = 0.0f; //0x007C
	float goal_feet_yaw = 0.0f; //0x0080
	float goal_feet_yaw_last = 0.0f; //0x0084
	float move_yaw = 0.0f; //0x0088
	float move_yaw_ideal = 0.0f; //0x008C
	float move_yaw_current_to_ideal = 0.0f; //0x0090
	float time_to_align_lower_body = 0.0f;
	float primary_cycle = 0.0f; //0x0098
	float move_weight = 0.0f; //0x009C
	float move_weight_smoothed = 0.0f;
	float duck_amount = 0.0f; //0x00A4
	float duck_additional = 0.0f; //0x00A8
	float recrouch_weight = 0.0f;
	Vector position_current; //0x00B0
	Vector position_last; //0x00BC
	Vector velocity; //0x00C8
	Vector velocity_normalized; //
	Vector velocity_normalized_non_zero; //0x00E0
	float velocity_length_xy = 0.0f; //0x00EC
	float velocity_length_z = 0.0f; //0x00F0
	float speed_as_portion_of_run_top_speed = 0.0f; //0x00F4
	float speed_as_portion_of_walk_top_speed = 0.0f; //0x00F8
	float speed_as_portion_of_crouch_top_speed = 0.0f; //0x00FC
	float duration_moving = 0.0f; //0x0100
	float duration_still = 0.0f; //0x0104
	bool on_ground = false; //0x0108
	bool landing = false; //0x0109
	float jump_to_fall = 0.0f;
	float duration_in_air = 0.0f; //0x0110
	float left_ground_height = 0.0f; //0x0114
	float land_multiplier = 0.0f; //0x0118
	float walk_run_transition = 0.0f; //0x011C
	bool landed_on_ground_this_frame = false;
	bool left_the_ground_this_frame = false;
	float in_air_smooth_value = 0.0f;
	bool on_ladder = false; //0x0124
	float ladder_weight = 0.0f; //0x0128
	float ladder_speed = 0.0f;
	bool walk_to_run_transition_state = false;
	bool defuse_started = false;
	bool plant_anistarted = false;
	bool twitch_anistarted = false;
	bool adjust_started = false;
	char activity_modifiers_server[20] = "\0";
	float next_twitch_time = 0.0f;
	float time_of_last_known_injury = 0.0f;
	float last_velocity_test_time = 0.0f;
	Vector velocity_last;
	Vector target_acceleration;
	Vector acceleration;
	float acceleration_weight = 0.0f;
	float aimatrix_transition = 0.0f;
	float aimatrix_transition_delay = 0.0f;
	bool flashed = false;
	float strafe_change_weight = 0.0f;
	float strafe_change_target_weight = 0.0f;
	float strafe_change_cycle = 0.0f;
	int strafe_sequence = 0;
	bool strafe_changing = false;
	float duration_strafing = 0.0f;
	float foot_lerp = 0.0f;
	bool feet_crossed = false;
	bool player_is_accelerating = false;
	AnimstatePoseParamCache pose_paramappings[20];
	float duration_move_weight_is_too_high = 0.0f;
	float static_approach_speed = 0.0f;
	int previous_move_state = 0;
	float stutter_step = 0.0f;
	float action_weight_bias_remainder = 0.0f;
	Vector foot_left_pos_anim;
	Vector foot_left_pos_anilast;
	Vector foot_left_pos_plant;
	Vector foot_left_plant_vel;
	float foot_left_lock_amount = 0.0f;
	float foot_left_last_plant_time = 0.0f;
	Vector foot_right_pos_anim;
	Vector foot_right_pos_anilast;
	Vector foot_right_pos_plant;
	Vector foot_right_plant_vel;
	float foot_right_lock_amount = 0.0f;
	float foot_right_last_plant_time = 0.0f;
	float camera_smooth_height = 0.0f;
	bool smooth_height_valid = false;
	float last_time_velocity_over_ten = 0.0f;
	char pad_0x4[0x4];
	float yaw_min = 0.0f;
	float yaw_max = 0.0f;
	float pitch_min = 0.0f;
	float pitch_max = 0.0f;
	int animstate_model_version = 0;

	__forceinline float get_desync_delta()
	{
		auto speed_fraction = clamp(speed_as_portion_of_walk_top_speed, 0.0f, 1.0f);
		auto speed_factor = clamp(speed_as_portion_of_crouch_top_speed, 0.0f, 1.0f);
		auto modifier = (walk_run_transition * -0.30000001f - 0.19999999f) * speed_fraction + 1.0f;

		if (duck_amount > 0.0f)
			modifier += duck_amount * speed_factor * (0.5f - modifier);

		return yaw_max * modifier;
	}
};

class _ST_m_Attributes_32
{
public:
	int lengthprop32; // 0x0
	int m_iAttributeDefinitionIndex; // 0x4
	int m_iRawValue32; // 0x8
	int m_iRawInitialValue32; // 0xc
	int m_nRefundableCurrency; // 0x10
	int m_bSetBonus; // 0x14
};

class CAttributeList
{
public:
	_ST_m_Attributes_32    m_Attributes; // 0x0
};

class WeaponData
{
public:
	virtual ~WeaponData() = default;
	char* console_name;
	char pad_0008[8];
	void* weapon_def;
	int max_clip1;
	int max_clip2;
	int default_clip1;
	int default_clip2;
	char pad_0024[8];
	char* sz_world_model;
	char* sz_view_model;
	char* sz_dropped_model;
	char pad_0038[4];
	char* n0000023e;
	char pad_0040[56];
	char* sz_empty_sound;
	char pad_007c[4];
	char* sz_bullet_type;
	char pad_0084[4];
	char* sz_hud_name;
	char* sz_weapon_name;
	char pad_0090[60];
	int weapon_type;
	int weapon_price;
	int kill_award;
	char* sz_animation_prefix;
	float cycle_time;
	float cycle_time_alt;
	float time_to_idle;
	float idle_interval;
	bool full_auto;
	char pad_0x00e5[3];
	int damage;
	float headshot_multiplier;
	float armor_ratio;
	int bullets;
	float penetration;
	float flinch_velocity_modifier_large;
	float flinch_velocity_modifier_small;
	float range;
	float range_modifier;
	float throw_velocity;
	char pad_0x010c[16];
	bool has_silencer;
	char pad_0x0119[3];
	char* p_silencer_model;
	int crosshair_min_distance;
	float max_player_speed;
	float max_player_speed_alt;
	char pad_0x0130[4];
	float spread;
	float spread_alt;
	float inaccuracy_crouch;
	float inaccuracy_crouch_alt;
	float inaccuracy_stand;
	float inaccuracy_stand_alt;
	float inaccuracy_jump_initial;
	float inaccuracy_jump;
	float inaccuracy_jump_alt;
	float inaccuracy_land;
	float inaccuracy_land_alt;
	float inaccuracy_ladder;
	float inaccuracy_ladder_alt;
	float inaccuracy_fire;
	float inaccuracy_fire_alt;
	float inaccuracy_move;
	float inaccuracy_move_alt;
	float inaccuracy_reload;
	int recoil_seed;
	float recoil_angle;
	float recoil_angle_alt;
	float recoil_angle_variance;
	float recoil_angle_variance_alt;
	float recoil_magnitude;
	float recoil_magnitude_alt;
	float recoil_magnitude_variance;
	float recoil_magnitude_variance_alt;
	float recovery_time_crouch;
	float recovery_time_stand;
	float recovery_time_crouch_final;
	float recovery_time_stand_final;
	int recovery_transition_start_bullet;
	int recovery_transition_end_bullet;
	bool unzoom_after_shot;
	bool hide_view_model_zoomed;
	char pad_0x01b5[2];
	char zoom_levels[3];
	int	 zoom_fov[2];
	float zoom_time[3];
	char* sz_weapon_class;
	float addon_scale;
	char pad_0x01dc[4];
	char* sz_eject_brass_effect;
	char* sz_tracer_effect;
	int tracer_frequency;
	int tracer_frequency_alt;
	char* sz_muzzle_flash_effect_1st_person;
	char pad_0x01f4[4];
	char* sz_muzzle_flash_effect_3rd_person;
	char pad_0x01fc[4];
	char* sz_muzzle_smoke_effect;
	float heat_per_shot;
	char* sz_zoom_in_sound;
	char* sz_zoom_out_sound;
	float inaccuracy_pitch_shift;
	float inaccuracy_sound_threshold;
	float bot_audible_range;
	char pad_0x0218[8];
	char* p_wrong_team_msg;
	bool has_burst_mode;
	char pad_0x0225[3];
	bool is_revolver;
	bool cannot_shoot_underwater;
};

class ShortAnimationLayer
{
public:
	int pad1;
	float_t m_flCycle;           //0x002C
	int pad2;
	float_t m_flPlaybackRate;    //0x0028
	int pad3;
	uint32_t m_nOrder;           //0x0014
	int pad4;
	float_t m_flWeight;          //0x0020
	int pad5;
	uint32_t m_nSequence;        //0x0018
};

class AnimationLayer
{
public:
	bool m_bClientBlend;				 //0x0000
	float m_flBlendIn;					 //0x0004
	void* m_pStudioHdr;					 //0x0008
	int m_nDispatchSequence;     //0x000C
	int m_nDispatchSequence_2;   //0x0010
	uint32_t m_nOrder;           //0x0014
	uint32_t m_nSequence;        //0x0018
	float_t m_flPrevCycle;       //0x001C
	float_t m_flWeight;          //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate;    //0x0028
	float_t m_flCycle;           //0x002C
	void* m_pOwner;              //0x0030 // player's thisptr
	char pad_0038[4];            //0x0034
};

class Entity : public IClientEntity
{
public:
	NETVAR(int, m_fEffects, crypt_hash("CBaseEntity"), crypt_hash("m_fEffects"));
	DATAMAP(int, m_iEFlags, crypt_hash("m_iEFlags"));
	NETVAR(int, m_iTeamNum, crypt_hash("CBaseEntity"), crypt_hash("m_iTeamNum"));
	NETVAR(int, m_nSequence, crypt_hash("CBaseAnimating"), crypt_hash("m_nSequence"));
	DATAMAP(float, m_flAnimTime, crypt_hash("m_flAnimTime"));
	DATAMAP(float, m_flCycle, crypt_hash("m_flCycle"));
	DATAMAP(int, m_nSkin, crypt_hash("m_nSkin"));
	DATAMAP(int, m_nBody, crypt_hash("m_nBody"));
	NETVAR(float, m_flSimulationTime, crypt_hash("CBaseEntity"), crypt_hash("m_flSimulationTime"));
	OFFSET(float, m_flOldSimulationTime, netvars_manager->find_offset(crypt_hash("CBaseEntity"), crypt_hash("m_flSimulationTime")) + 0x4);
	NETVAR(Vector, m_vecOrigin, crypt_hash("CBaseEntity"), crypt_hash("m_vecOrigin"));
	NETVAR(Vector, m_vecNetworkOrigin, crypt_hash("CBaseEntity"), crypt_hash("m_vecNetworkOrigin"));
	OFFSET(matrix3x4_t, m_rgflCoordinateFrame, netvars_manager->find_offset(crypt_hash("CBaseEntity"), crypt_hash("m_CollisionGroup")) - 0x30);
	NETVAR(CHandle <Player>, m_hOwnerEntity, crypt_hash("CBaseEntity"), crypt_hash("m_hOwnerEntity"));
	NETVAR(int, m_nModelIndex, crypt_hash("CBaseEntity"), crypt_hash("m_nModelIndex"));
	NETVAR(int, m_nExplodeEffectTickBegin, crypt_hash("CBaseCSGrenadeProjectile"), crypt_hash("m_nExplodeEffectTickBegin"));
	NETVAR(CHandle <Player>, m_hThrower, crypt_hash("CBaseGrenade"), crypt_hash("m_hThrower"));
	NETVAR(Vector, m_vecVelocity, crypt_hash("CBaseGrenade"), crypt_hash("m_vecVelocity"));
	PNETVAR(float, m_flPoseParameters, crypt_hash("CBaseAnimating"), crypt_hash("m_flPoseParameter"));
	OFFSET(float, creation_time, 0x2D8);



	bool is_player();
	bool is_weapon();
	void set_abs_angles(const Vector& angle);
	void set_abs_origin(const Vector& origin);
	void set_model_index(int index);
	string get_class_name();
	int get_class_id();
	bool is_breakable();

	bool get_prop_bool(const string& table, const string& netvar);
	int get_prop_int(const string& table, const string& netvar);
	float get_prop_float(const string& table, const string& netvar);
	string get_prop_string(const string& table, const string& netvar);

	void set_prop_bool(const string& table, const string& netvar, const bool& value);
	void set_prop_int(const string& table, const string& netvar, const int& value);
	void set_prop_float(const string& table, const string& netvar, const float& value);
	void set_prop_string(const string& table, const string& netvar, const string& value);

	int& handle()
	{
		using Handle_t = int& (__thiscall*)(void*);
		return call_virtual< Handle_t>(this, 2)(this);
	}
};

class CBoneAccessor
{
public:
	alignas(16) matrix3x4_t* m_pBones;

	int m_ReadableBones;
	int m_WritableBones;
};

class Player : public Entity
{
public:
	NETVAR(bool, m_bClientSideAnimation, crypt_hash("CBaseAnimating"), crypt_hash("m_bClientSideAnimation"));
	NETVAR(bool, m_bGunGameImmunity, crypt_hash("CCSPlayer"), crypt_hash("m_bGunGameImmunity"));
	NETVAR(bool, m_bHasHelmet, crypt_hash("CCSPlayer"), crypt_hash("m_bHasHelmet"));
	NETVAR(bool, m_bHasDefuser, crypt_hash("CCSPlayer"), crypt_hash("m_bHasDefuser"));
	NETVAR(bool, m_bIsScoped, crypt_hash("CCSPlayer"), crypt_hash("m_bIsScoped"));
	NETVAR(float, m_flGroundAccelLinearFracLastTime, crypt_hash("CCSPlayer"), crypt_hash("m_flGroundAccelLinearFracLastTime"));
	NETVAR(bool, m_bHasHeavyArmor, crypt_hash("CCSPlayer"), crypt_hash("m_bHasHeavyArmor"));
	NETVAR(bool, m_bIsWalking, crypt_hash("CCSPlayer"), crypt_hash("m_bIsWalking"));
	NETVAR(bool, m_bStrafing, crypt_hash("CCSPlayer"), crypt_hash("m_bStrafing"));
	NETVAR(bool, m_bIsDefusing, crypt_hash("CCSPlayer"), crypt_hash("m_bIsDefusing"));
	NETVAR(bool, m_bDuckOverride, crypt_hash("CCSPlayer"), crypt_hash("m_bDuckOverride"));
	NETVAR(bool, m_bInBombZone, crypt_hash("CCSPlayer"), crypt_hash("m_bInBombZone"));
	NETVAR(bool, m_bSpotted, crypt_hash("CBasePlayer"), crypt_hash("m_bSpotted"));
	DATAMAP(int, m_nImpulse, crypt_hash("m_nImpulse"));
	DATAMAP(int, m_nButtons, crypt_hash("m_nButtons"));
	DATAMAP(int, m_afButtonLast, crypt_hash("m_afButtonLast"));
	DATAMAP(int, m_afButtonPressed, crypt_hash("m_afButtonPressed"));
	DATAMAP(int, m_afButtonReleased, crypt_hash("m_afButtonReleased"));
	OFFSET(int, m_moveType, 0x25C);
	NETVAR(int, m_fFlags, crypt_hash("CBasePlayer"), crypt_hash("m_fFlags"));
	NETVAR(int, m_iHealth, crypt_hash("CBasePlayer"), crypt_hash("m_iHealth"));
	NETVAR(int, m_lifeState, crypt_hash("CBasePlayer"), crypt_hash("m_lifeState"));
	NETVAR(int, m_ArmorValue, crypt_hash("CCSPlayer"), crypt_hash("m_ArmorValue"));
	NETVAR(int, m_iAccount, crypt_hash("CCSPlayer"), crypt_hash("m_iAccount"));
	NETVAR(int, m_iShotsFired, crypt_hash("CCSPlayer"), crypt_hash("m_iShotsFired"));
	OFFSET(int, m_nFinalPredictedTick, 0x3444);
	NETVAR(int, m_nTickBase, crypt_hash("CCSPlayer"), crypt_hash("m_nTickBase"));
	NETVAR(int, m_nHitboxSet, crypt_hash("CBasePlayer"), crypt_hash("m_nHitboxSet"));
	NETVAR(float, m_flFallVelocity, crypt_hash("CBasePlayer"), crypt_hash("m_flFallVelocity"));
	NETVAR(float, m_flDuckAmount, crypt_hash("CBasePlayer"), crypt_hash("m_flDuckAmount"));
	NETVAR(float, m_flDuckSpeed, crypt_hash("CBasePlayer"), crypt_hash("m_flDuckSpeed"));
	NETVAR(float, m_flMaxspeed, crypt_hash("CBasePlayer"), crypt_hash("m_flMaxspeed"));
	NETVAR(float, m_flLowerBodyYawTarget, crypt_hash("CCSPlayer"), crypt_hash("m_flLowerBodyYawTarget"));
	NETVAR(float, m_flVelocityModifier, crypt_hash("CCSPlayer"), crypt_hash("m_flVelocityModifier"));
	NETVAR(float, m_flFlashDuration, crypt_hash("CCSPlayer"), crypt_hash("m_flFlashDuration"));
	NETVAR(float, m_flNextAttack, crypt_hash("CBaseCombatCharacter"), crypt_hash("m_flNextAttack"));
	OFFSET(float, m_flSpawnTime, 0x103C0);
	NETVAR(Vector, m_aimPunchAngle, crypt_hash("CBasePlayer"), crypt_hash("m_aimPunchAngle"));
	NETVAR(Vector, m_aimPunchAngleVel, crypt_hash("CBasePlayer"), crypt_hash("m_aimPunchAngleVel"));
	DATAMAP(Vector, m_vecAbsVelocity, crypt_hash("m_vecAbsVelocity"));
	NETVAR(Vector, m_vecBaseVelocity, crypt_hash("CBasePlayer"), crypt_hash("m_vecBaseVelocity"));
	NETVAR(Vector, m_vecVelocity, crypt_hash("CBasePlayer"), crypt_hash("m_vecVelocity[0]"));
	NETVAR(Vector, m_vecViewOffset, crypt_hash("CBasePlayer"), crypt_hash("m_vecViewOffset[0]"));
	NETVAR(Vector, m_viewPunchAngle, crypt_hash("CBasePlayer"), crypt_hash("m_viewPunchAngle"));
	NETVAR(Vector, m_angEyeAngles, crypt_hash("CCSPlayer"), crypt_hash("m_angEyeAngles[0]"));
	NETVAR(CHandle <Weapon>, m_hActiveWeapon, crypt_hash("CBaseCombatCharacter"), crypt_hash("m_hActiveWeapon"));
	POFFSET(CBaseHandle, m_hMyWeapons, netvars_manager->find_offset(crypt_hash("CBaseCombatCharacter"), crypt_hash("m_hActiveWeapon")) - 0x100);
	NETVAR(CHandle <Entity>, m_hCarriedHostage, crypt_hash("CCSPlayer"), crypt_hash("m_hCarriedHostage"));
	NETVAR(CHandle <Viewmodel>, m_hViewModel, crypt_hash("CBasePlayer"), crypt_hash("m_hViewModel[0]"));
	PNETVAR(CBaseHandle, m_hMyWearables, crypt_hash("CBaseCombatCharacter"), crypt_hash("m_hMyWearables"));
	NETVAR(int, m_iObserverMode, crypt_hash("CBasePlayer"), crypt_hash("m_iObserverMode"));
	DATAMAP(float, m_surfaceFriction, crypt_hash("m_surfaceFriction"));
	DATAMAP(int, m_hGroundEntity, crypt_hash("m_hGroundEntity"));
	NETVAR(CHandle <Player>, m_hObserverTarget, crypt_hash("CBasePlayer"), crypt_hash("m_hObserverTarget"));
	NETVAR(bool, m_bCustomPlayer, crypt_hash("CCSPlayer"), crypt_hash("m_bCustomPlayer"));
	NETVAR(float, m_flThirdpersonRecoil, crypt_hash("CCSPlayer"), crypt_hash("m_flThirdpersonRecoil"));
	NETVAR(int, m_hRagdoll, crypt_hash("CCSPlayer"), crypt_hash("m_hRagdoll"));
	NETVAR(int, m_iAddonBits, crypt_hash("CCSPlayer"), crypt_hash("m_iAddonBits"));
	NETVAR(int, m_unMusicID, crypt_hash("CCSPlayer"), crypt_hash("m_unMusicID"));
	PNETVAR(int, m_vecPlayerPatchEconIndices, crypt_hash("CCSPlayer"), crypt_hash("m_vecPlayerPatchEconIndices"));
	OFFSET(int, m_nLastNonSkippedFrame, 0xA68);
	OFFSET(int, m_nCustomBlendingRuleMask, 0xA24);
	OFFSET(int, m_nAnimLODflags, 0xA28);
	OFFSET(int, m_ClientEntEffects, 0x68);
	OFFSET(int, m_pIK, netvars_manager->find_offset(crypt_hash("CBaseAnimating"), crypt_hash("m_nForceBone")) - 0x1C);
	OFFSET(CBoneAccessor, m_BoneAccessor, netvars_manager->find_offset(crypt_hash("CBaseAnimating"), crypt_hash("m_nForceBone")) - 0x1C);
	NETVAR(BYTE, m_ubEFNoInterpParity, crypt_hash("CBasePlayer"), crypt_hash("m_ubEFNoInterpParity"));
	OFFSET(BYTE, m_ubEFNoInterpParityOld, netvars_manager->find_offset(crypt_hash("CBasePlayer"), crypt_hash("m_ubEFNoInterpParity")) + 0x1);
	OFFSET(float, m_flFlashBangTime, netvars_manager->find_offset(crypt_hash("CCSPlayer"), crypt_hash("m_flFlashDuration")) - 0x14);
	OFFSET(float, m_BoneSnapshotFirst, 0x9B24);
	OFFSET(float, m_BoneSnapshotSecond, 0xCF74);
	OFFSET(Vector, m_vecLastPredictedPosition, 0x35B8);
	OFFSET(float, m_flCollisionChangeTime, netvars_manager->find_offset(crypt_hash("CCSPlayer"), crypt_hash("m_bIsScoped")) - 0x50);
	OFFSET(float, m_flCollisionChangeOrigin, netvars_manager->find_offset(crypt_hash("CCSPlayer"), crypt_hash("m_bIsScoped")) - 0x54);

	bool valid(bool check_team = false);
	void StandardBlendingRules(CStudioHdr* hdr, Vector* pos, Quaternion* q, float time, int mask);
	bool setup_bones_rebuilded(int mask, matrix3x4_t* mx);
	void setup_bones(crypt_ptr <matrix3x4_t> matrix, int mask);
	bool can_be_animated();
	bool can_shoot(bool check_revolver = true, float delay = 0.0f);
	Vector get_shoot_position();
	Vector get_shoot_position_abs();
	void modify_shoot_position(Vector& eye_position);
	uintptr_t& get_bone_merge();
	uint32_t& m_iModelBoneCounter();
	uint32_t& m_iMostRecentModelBoneCounter();
	float& m_flLastBoneSetupTime();
	Vector& m_angAbsRotation();
	Vector& m_vecOriginPredMap();
	Vector& m_angRotation();
	Vector& m_vecAbsOrigin();
	Vector& third_person_angles();
	void attachment_helper();
	bool force_animations_data();
	bool& m_bIsHLTV();
	void update_animations();
	void invalidate_physics_recursive(int change_flags);
	void set_current_command(crypt_ptr <CUserCmd> cmd);
	void think();
	void pre_think();
	void post_think();
	crypt_ptr <mstudiobbox_t> get_hitbox(int hitbox_index);
	Vector get_bone_position(int bone);
	Vector get_hitbox_position(int hitbox_index, crypt_ptr <matrix3x4_t> matrix, bool immortal_alal = false);
	crypt_ptr <AnimationState> get_animation_state();
	crypt_ptr <AnimationLayer> get_animation_layer(int index = 0);
	int get_animation_layers_count();
	CUtlVector <matrix3x4_t>& m_CachedBoneData();
	void UpdateCollisionBounds();
	void set_collision_bounds(const Vector& mins, const Vector& maxs, bool ignore_restore = false);
	crypt_ptr <CStudioHdr> get_studio_hdr();
	int get_sequence_activity(int sequence);
};

class CEconItemDefinition : public EconItemDefinition
{
public:
	int GetEquippedPosition();
};

class CEconItem;
class C_EconItemView
{
public:
	NETVAR(bool, m_bInitialized, crypt_hash("CBaseAttributableItem"), crypt_hash("m_bInitialized"));
	NETVAR(int32_t, m_iItemDefinitionIndex, crypt_hash("CBaseAttributableItem"), crypt_hash("m_iItemDefinitionIndex"));
	NETVAR(int32_t, m_iEntityLevel, crypt_hash("CBaseAttributableItem"), crypt_hash("m_iEntityLevel"));
	NETVAR(int32_t, m_iAccountID, crypt_hash("CBaseAttributableItem"), crypt_hash("m_iAccountID"));
	NETVAR(int32_t, m_iItemIDLow, crypt_hash("CBaseAttributableItem"), crypt_hash("m_iItemIDLow"));
	NETVAR(int32_t, m_iItemIDHigh, crypt_hash("CBaseAttributableItem"), crypt_hash("m_iItemIDHigh"));
	NETVAR(int32_t, m_iEntityQuality, crypt_hash("CBaseAttributableItem"), crypt_hash("m_iEntityQuality"));
	NETVAR(int32_t, m_nFallbackPaintKit2, crypt_hash("CBaseAttributableItem"), crypt_hash("m_nFallbackPaintKit"));
	NETVAR(int32_t, m_iItemID, crypt_hash("CBaseAttributableItem"), crypt_hash("m_iItemID"));

	CEconItemDefinition* GetStaticData();
	CEconItem* GetSOCData();
	std::uint64_t itemID() noexcept
	{
		return (std::uint64_t(m_iItemIDHigh()) << 32) | m_iItemIDLow();
	}

	void clear_inventory_image_RGBA()
	{
		reinterpret_cast <void(__thiscall*)(void*)> (signatures_manager->signatures[SIGNATURE_CLEAR_INVENTORY_IMAGE_RGBA])(this);
	}
	void clear_custom_materials()
	{
		reinterpret_cast <void(__thiscall*)(void*,int)> (signatures_manager->signatures[SIGNATURE_CLEAR_CUSTOM_MATERIALS])(this, 0);
	}

	void add_attribute(std::string name, float value)
	{
		static auto add_attribute = (void(__thiscall*)(C_EconItemView*, const char*))(signatures_manager->signatures[SIGNATURE_ADD_ECON_ATTRIBUTE]);
		_asm {
			movd xmm2, value
		}
		add_attribute(this, name.c_str());
	}

	OFFSET(CUtlVector<CRefCounted*>, m_CustomMaterials, netvars_manager->find_offset(crypt_hash("CBaseAttributableItem"), crypt_hash("m_Item")) + 0x14);
	OFFSET(CUtlVector<CRefCounted*>, m_VisualsDataProcessors, netvars_manager->find_offset(crypt_hash("CBaseAttributableItem"), crypt_hash("m_Item")) + 0x230);
};

class C_BaseAttributableItem : public Entity
{
public:
	NETVAR(uint64_t, m_OriginalOwnerXuid, crypt_hash("CBaseAttributableItem"), crypt_hash("m_OriginalOwnerXuidLow"));
	NETVAR(int32_t, m_OriginalOwnerXuidLow, crypt_hash("CBaseAttributableItem"), crypt_hash("m_OriginalOwnerXuidLow"));
	NETVAR(int32_t, m_OriginalOwnerXuidHigh, crypt_hash("CBaseAttributableItem"), crypt_hash("m_OriginalOwnerXuidHigh"));
	NETVAR(int32_t, m_nFallbackStatTrak, crypt_hash("CBaseAttributableItem"), crypt_hash("m_nFallbackStatTrak"));
	NETVAR(int32_t, m_nFallbackPaintKit, crypt_hash("CBaseAttributableItem"), crypt_hash("m_nFallbackPaintKit"));
	NETVAR(int32_t, m_nFallbackSeed, crypt_hash("CBaseAttributableItem"), crypt_hash("m_nFallbackSeed"));
	NETVAR(float_t, m_flFallbackWear, crypt_hash("CBaseAttributableItem"), crypt_hash("m_flFallbackWear"));
	NETVAR(int32_t, m_iItemDefinitionIndex, crypt_hash("CBaseAttributableItem"), crypt_hash("m_iItemDefinitionIndex"));

	std::uint64_t originalOwnerXuid() noexcept
	{
		return (std::uint64_t(m_OriginalOwnerXuidHigh()) << 32) | m_OriginalOwnerXuidLow();
	}
	C_EconItemView& m_Item()
	{
		return *(C_EconItemView*)this; //-V1027
	}
};

class Weapon : public C_BaseAttributableItem
{
public:
	NETVAR(short, m_iItemDefinitionIndex, crypt_hash("CBaseCombatWeapon"), crypt_hash("m_iItemDefinitionIndex"));
	OFFSET(int, m_bIsReloading, 0x32B5);
	NETVAR(int, m_zoomLevel, crypt_hash("CWeaponCSBaseGun"), crypt_hash("m_zoomLevel"));
	NETVAR(int, m_iClip1, crypt_hash("CBaseCombatWeapon"), crypt_hash("m_iClip1"));
	NETVAR(int, m_iPrimaryReserveAmmoCount, crypt_hash("CBaseCombatWeapon"), crypt_hash("m_iPrimaryReserveAmmoCount"));
	NETVAR(float, m_flRecoilIndex, crypt_hash("CWeaponCSBase"), crypt_hash("m_flRecoilIndex"));
	NETVAR(float, m_fAccuracyPenalty, crypt_hash("CWeaponCSBase"), crypt_hash("m_fAccuracyPenalty"));
	NETVAR(float, m_flNextPrimaryAttack, crypt_hash("CBaseCombatWeapon"), crypt_hash("m_flNextPrimaryAttack"));
	NETVAR(float, m_flNextSecondaryAttack, crypt_hash("CBaseCombatWeapon"), crypt_hash("m_flNextSecondaryAttack"));
	NETVAR(float, m_flPostponeFireReadyTime, crypt_hash("CWeaponCSBase"), crypt_hash("m_flPostponeFireReadyTime"));
	NETVAR(float, m_fLastShotTime, crypt_hash("CWeaponCSBase"), crypt_hash("m_fLastShotTime"));
	PNETVAR(C_EconItemView, get_econ_item_view, crypt_hash("CBaseAttributableItem"), crypt_hash("m_Item"));
	NETVAR(int, weapon, crypt_hash("CBaseViewModel"), crypt_hash("m_hWeapon"));
	NETVAR(CHandle <Weapon>, m_hWeaponWorldModel, crypt_hash("CBaseCombatWeapon"), crypt_hash("m_hWeaponWorldModel"));
	NETVAR(float, m_fThrowTime, crypt_hash("CBaseCSGrenade"), crypt_hash("m_fThrowTime"));
	NETVAR(float, m_flThrowStrength, crypt_hash("CBaseCSGrenade"), crypt_hash("m_flThrowStrength"));
	NETVAR(bool, m_bPinPulled, crypt_hash("CBaseCSGrenade"), crypt_hash("m_bPinPulled"));
	DATAMAP(int, m_Activity, crypt_hash("m_Activity"));
	NETVAR(bool, m_bInitialized, crypt_hash("CBaseAttributableItem"), crypt_hash("m_bInitialized"));
	bool can_fire();
	bool executed(crypt_ptr <CUserCmd> cmd, bool anti_aim = false);
	bool is_grenade();
	float get_spread();
	float get_inaccuracy();
	void update_accuracy_penalty();
	crypt_ptr <WeaponData> get_weapon_data();
	int get_weapon_group();
	bool& m_bCustomMaterialInitialized();
	CUtlVector<CRefCounted*>& m_CustomMaterials();
};


class Viewmodel : public Entity
{
public:
	NETVAR(int, m_nAnimationParity, crypt_hash("CBaseViewModel"), crypt_hash("m_nAnimationParity"));
	NETVAR(CHandle <Weapon>, m_hWeapon, crypt_hash("CBaseViewModel"), crypt_hash("m_hWeapon"));
};

class Inferno : public Entity
{
public:
	NETVAR(int, fireCount, crypt_hash("CInferno"), crypt_hash("m_fireCount"));
	PNETVAR(bool, fireIsBurning, crypt_hash("CInferno"), crypt_hash("m_bFireIsBurning"));
	PNETVAR(int, fireXDelta, crypt_hash("CInferno"), crypt_hash("m_fireXDelta"));
	PNETVAR(int, fireYDelta, crypt_hash("CInferno"), crypt_hash("m_fireYDelta"));
	PNETVAR(int, fireZDelta, crypt_hash("CInferno"), crypt_hash("m_fireZDelta"));
	NETVAR(int, m_nFireEffectTickBegin, crypt_hash("CInferno"), crypt_hash("m_nFireEffectTickBegin"));
};

class Smoke : public Entity
{
public:
	NETVAR(int, m_nSmokeEffectTickBegin, crypt_hash("CSmokeGrenadeProjectile"), crypt_hash("m_nSmokeEffectTickBegin"));
};

class Bomb : public Entity
{
public:
	NETVAR(float, m_flC4Blow, crypt_hash("CPlantedC4"), crypt_hash("m_flC4Blow"));
	NETVAR(bool, m_bStartedArming, crypt_hash("CWeaponC4"), crypt_hash("m_bStartedArming"));
};

class EnvTonemapController : public Entity
{
public:
	NETVAR(bool, m_bUseCustomAutoExposureMin, crypt_hash("CEnvTonemapController"), crypt_hash("m_bUseCustomAutoExposureMin"));
	NETVAR(bool, m_bUseCustomAutoExposureMax, crypt_hash("CEnvTonemapController"), crypt_hash("m_bUseCustomAutoExposureMax"));
	NETVAR(bool, m_bUseCustomBloomScale, crypt_hash("CEnvTonemapController"), crypt_hash("m_bUseCustomBloomScale"));
	NETVAR(float, m_flCustomAutoExposureMin, crypt_hash("CEnvTonemapController"), crypt_hash("m_flCustomAutoExposureMin"));
	NETVAR(float, m_flCustomAutoExposureMax, crypt_hash("CEnvTonemapController"), crypt_hash("m_flCustomAutoExposureMax"));
	NETVAR(float, m_flCustomBloomScale, crypt_hash("CEnvTonemapController"), crypt_hash("m_flCustomBloomScale"));
	NETVAR(float, m_flCustomBloomScaleMinimum, crypt_hash("CEnvTonemapController"), crypt_hash("m_flCustomBloomScaleMinimum"));
	NETVAR(float, m_flBloomExponent, crypt_hash("CEnvTonemapController"), crypt_hash("m_flBloomExponent"));
	NETVAR(float, m_flBloomSaturation, crypt_hash("CEnvTonemapController"), crypt_hash("m_flBloomSaturation"));
	NETVAR(float, m_flTonemapPercentTarget, crypt_hash("CEnvTonemapController"), crypt_hash("m_flTonemapPercentTarget"));
	NETVAR(float, m_flTonemapPercentBrightPixels, crypt_hash("CEnvTonemapController"), crypt_hash("m_flTonemapPercentBrightPixels"));
	NETVAR(float, m_flTonemapMinAvgLum, crypt_hash("CEnvTonemapController"), crypt_hash("m_flTonemapMinAvgLum"));
	NETVAR(float, m_flTonemapRate, crypt_hash("CEnvTonemapController"), crypt_hash("m_flTonemapRate"));
};

class PlayerResource {
public:
	//PNETVAR(int, m_nMusicID, crypt_hash("m_nMusicID"), crypt_hash("CCSPlayerResource"));

	int* m_nMusicID()
	{
		return (int*)(this + 0x00004cd0);
	}
	int* m_iHealth() {
		static auto offset = netvars_manager->find_offset(crypt_hash("CPlayerResource"), crypt_hash("m_iHealth"));
		return (int*)(this + offset);
	}
};