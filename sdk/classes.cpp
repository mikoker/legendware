// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "classes.h"
#include "..\config\config.h"
#include "..\globals.h"
#include "..\features\features.h"
#include "..\features\local_animations.h"
#include "..\features\exploits.h"
#include "..\features\build_bones.h"

bool Entity::is_player()
{
	if (!this) //-V704
		return false;

	if (!engine->IsInGame())
		return false;

	auto client_class = crypt_ptr <ClientClass>(GetClientClass());

	if (!client_class)
		return false;

	return client_class->m_ClassID == CCSPlayer;
}

bool Entity::is_weapon()
{
	if (!this) //-V704
		return false;

	return call_virtual <bool(__thiscall*)(void*)>(this, INDEX_IS_WEAPON)(this);
}

void Entity::set_abs_angles(const Vector& angle)
{
	if (!this) //-V704
		return;

	return reinterpret_cast <void(__thiscall*)(void*, const Vector&)> (signatures_manager->signatures[SIGNATURE_SET_ABS_ANGLES])(this, angle);
}

void Entity::set_abs_origin(const Vector& origin)
{
	if (!this) //-V704
		return;

	return reinterpret_cast <void(__thiscall*)(void*, const Vector&)> (signatures_manager->signatures[SIGNATURE_SET_ABS_ORIGIN])(this, origin);
}

bool Player::valid(bool check_team)
{
	if (!this) //-V704
		return false;

	if (!is_player())
		return false;

	if (m_lifeState() != LIFE_ALIVE)
		return false;

	if (check_team && ctx->local() && m_iTeamNum() == ctx->local()->m_iTeamNum())
		return false;

	return true;
}

uintptr_t& Player::get_bone_merge()
{
	return *(uintptr_t*)((uintptr_t)this + signatures_manager->signatures[SIGNATURE_BONE_MERGE]);
}

uint32_t& Player::m_iModelBoneCounter()
{
	static auto model_bone_counter = *(uintptr_t*)(signatures_manager->signatures[SIGNATURE_INVALIDATE_BONE_CACHE] + 0xA);
	return *(uint32_t*)model_bone_counter;
}

uint32_t& Player::m_iMostRecentModelBoneCounter()
{
	static auto most_recent_model_bone_counter = *(uintptr_t*)(signatures_manager->signatures[SIGNATURE_INVALIDATE_BONE_CACHE] + 0x1B);
	return *(uint32_t*)((uintptr_t)this + most_recent_model_bone_counter);
}

float& Player::m_flLastBoneSetupTime()
{
	static auto last_bone_setup_time = *(uintptr_t*)(signatures_manager->signatures[SIGNATURE_INVALIDATE_BONE_CACHE] + 0x11);
	return *(float*)((uintptr_t)this + last_bone_setup_time);
}
Vector& Player::m_angAbsRotation()
{
	//datamap. Hardcoded becuz never changed since 2017
	return *(Vector*)(uintptr_t(this) + 0xC4);
}

Vector& Player::m_vecOriginPredMap()
{
	//datamap. Hardcoded becuz never changed since 2017
	return *(Vector*)(uintptr_t(this) + 0xAC);
}

Vector& Player::m_angRotation()
{
	//datamap. Hardcoded becuz never changed since 2017
	return *(Vector*)(uintptr_t(this) + 0xD0);
}

Vector& Player::m_vecAbsOrigin()
{
	//real abs origin (when u call virtual GetAbsOrigin it recalculates origin and sets it to this datamap value. Hardcoded becuz never changed since 2017
	return *(Vector*)(uintptr_t(this) + 0xA0);
}

void Studio_BuildMatrices(crypt_ptr<CStudioHdr> hdr, const matrix3x4_t& rotationmatrix, Vector* pos, Quaternion* q, int boneMask, matrix3x4_t* bonetoworld) {
	int i, j;

	int					chain[128] = {};
	int					chainlength = 0;

	// just stub
	int iBone = -1;

	auto pStudioHdr = hdr;

	if (iBone < -1 || iBone >= pStudioHdr->m_pStudioHdr->numbones)
		iBone = 0;

	auto boneFlags = &pStudioHdr->m_boneFlags;
	auto boneParent = &pStudioHdr->m_boneParent;

	// build list of what bones to use
	if (iBone <= -1) {
		// all bones
		chainlength = pStudioHdr->m_pStudioHdr->numbones;
		for (i = 0; i < pStudioHdr->m_pStudioHdr->numbones; i++) {
			chain[chainlength - i - 1] = i;
		}

	}
	else {
		// only the parent bones
		i = iBone;

		do {
			chain[chainlength++] = i;
			i = boneParent->m_Memory.Element(i);
		} while (i != -1);
	}

	// missing some parts with 0xF flag

	matrix3x4_t bonematrix;
	for (j = chainlength - 1; j >= 0; j--) {
		i = chain[j];

		//if (((1 << (i & 0x1F)) & boneComputed[i >> 5]))
		//	continue;

		int flag = boneFlags->m_Memory.Element(i);
		if ((flag & boneMask) != 0) {
			bonematrix.QuaternionMatrix(q[i], pos[i]);


			auto parent = boneParent->m_Memory.Element(i);
			if (parent == -1) {
				bonetoworld[i] = rotationmatrix.ConcatTransforms(bonematrix);
			}
			else {
				bonetoworld[i] = bonetoworld[parent].ConcatTransforms(bonematrix);
			}
		}
	}
}

void Player::StandardBlendingRules(CStudioHdr* hdr, Vector* pos, Quaternion* q, float time, int mask) {
	using StandardBlendingRules_t = void(__thiscall*)(void*, CStudioHdr*, Vector*, Quaternion*, float, int);
	return call_virtual< StandardBlendingRules_t >(this, 206)(this, hdr, pos, q, time, mask);
}

bool Player::setup_bones_rebuilded(int mask, matrix3x4_t* mx)
{
	// get hdr.
	auto hdr = get_studio_hdr().get();
	if (!hdr
		|| !hdr->m_pStudioHdr
		|| *(bool*)(uintptr_t(hdr->m_pStudioHdr) + 0x150) && !hdr->m_pVModel)
		return false;

	if (m_nSequence() == -1)
		return 0;

	// backup original.
	const auto backup_origin = GetAbsOrigin();
	const auto backup_rotation = m_angRotation();
	const auto backup_absrotation = m_angAbsRotation();
	const auto backup_effects = m_fEffects();
	const auto backup_enteffects = m_ClientEntEffects();
	const auto backup_animlod = m_nAnimLODflags();
	const auto backup_ik = m_pIK();
	const auto backup_origin_predmap = m_vecOriginPredMap();
	const auto backup_abs_origin_var = m_vecAbsOrigin();
	const auto backup_lean = get_animation_layers_count() <= 12 ? 0 : get_animation_layer(12)->m_flWeight;

	// compute transform from raw data.
	matrix3x4_t transform;
	math::AngleMatrix(GetAbsAngles(), GetAbsOrigin(), transform);

	m_fEffects() |= 8u;
	m_ClientEntEffects() |= 2u;
	m_nAnimLODflags() = 0;
	m_pIK() = 0;

	if (get_animation_layers_count() > 12)
		get_animation_layer(12)->m_flWeight = 0;

	m_vecOriginPredMap() = GetAbsOrigin();
	m_vecAbsOrigin() = GetAbsOrigin();
	m_angRotation() = GetAbsAngles();
	m_angAbsRotation() = GetAbsAngles();

	Vector					 pos[128] = {};
	alignas(16) Quaternion     q[128] = {};

	StandardBlendingRules(hdr, pos, q, m_flSimulationTime(), mask);
	Studio_BuildMatrices(hdr, transform, pos, q, mask, mx);

	if (get_animation_layers_count() > 12)
		get_animation_layer(12)->m_flWeight = backup_lean;

	m_angRotation() = backup_rotation;
	m_angAbsRotation() = backup_absrotation;
	m_fEffects() = backup_effects;
	m_nAnimLODflags() = backup_animlod;
	m_ClientEntEffects() = backup_enteffects;
	m_vecOriginPredMap() = backup_origin_predmap;
	m_vecAbsOrigin() = backup_abs_origin_var;
	m_pIK() = backup_ik;

	return true;
}

void Player::setup_bones(crypt_ptr <matrix3x4_t> matrix, int mask)
{
	if (!valid())
		return;

	if (!force_animations_data())
		return;

	auto framecount = globals->framecount;
	auto ik = m_pIK();
	auto effects = m_fEffects();
	auto anim_lod_flags = m_nAnimLODflags();
	auto client_ent_effects = m_ClientEntEffects();
	auto backup_ishltv = m_bIsHLTV();

	globals->framecount = INT_MIN;

	m_pIK() = 0;
	m_fEffects() |= 8;
	m_nAnimLODflags() &= ~2;
	m_ClientEntEffects() |= 2;

	m_iMostRecentModelBoneCounter() = 0;
	m_flLastBoneSetupTime() = -FLT_MAX;
	m_nCustomBlendingRuleMask() = -1;
	m_nLastNonSkippedFrame() = 0;
	m_BoneAccessor().m_ReadableBones = 0;

	m_bIsHLTV() = true;

	if (mask == BONE_USED_BY_ANYTHING)
	{
		ctx->setuping_bones = true;
		SetupBones(matrix.get(), MAXSTUDIOBONES, mask, m_flSimulationTime());
		ctx->setuping_bones = false;
	}
	else
	{
		build_bones->initialize(this, get_animation_state(), matrix, mask);
		build_bones->setup();
	}

	globals->framecount = framecount;

	m_pIK() = ik;
	m_fEffects() = effects;
	m_nAnimLODflags() = anim_lod_flags;
	m_ClientEntEffects() = client_ent_effects;
	m_bIsHLTV() = backup_ishltv;
}

bool Player::can_be_animated()
{
	if (!this)
		return false;

	if (!m_bCustomPlayer())
		return false;

	if (!get_animation_state())
		return false;

	auto weapon = m_hActiveWeapon().Get();

	if (!weapon)
		return false;

	if (this == ctx->local().get())
		return true;

	auto weapon_world_model = crypt_ptr <Weapon>(weapon->m_hWeaponWorldModel().Get());

	if (!weapon_world_model || *(short*)((uintptr_t)weapon_world_model.get() + 0x26E) == -1)
		return false;

	return true;
}

bool Player::can_shoot(bool check_revolver, float delay)
{
	if (!this)
		return false;

	if (this == ctx->local().get() && ctx->local()->m_fFlags() & FL_FROZEN)
		return false;

	auto weapon = m_hActiveWeapon().Get();

	if (!weapon)
		return false;

	if (weapon->is_grenade())
		return true;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_C4)
		return true;

	auto weapon_data = weapon->get_weapon_data();

	if (!weapon_data)
		return false;

	auto is_knife = weapon_data->weapon_type == WEAPONTYPE_KNIFE_TASER && weapon->m_iItemDefinitionIndex() != WEAPON_TASER;

	if (weapon_data->weapon_type != is_knife && !weapon->m_iClip1())
		return false;

	auto time = TICKS_TO_TIME(ctx->tickbase) - delay;

	if (time < m_flNextAttack())
		return false;

	if (time < weapon->m_flNextPrimaryAttack())
		return false;

	if (check_revolver && weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (weapon->m_flPostponeFireReadyTime() > time)
			return false;

		if (weapon->m_nSequence() != 5)
			return false;
	}

	return true;
}

Vector Player::get_shoot_position_abs()
{
	if (!this)
		return Vector();

	Vector shoot_position;

	if (!this) //-V704
		return shoot_position;

	shoot_position = GetAbsOrigin() + m_vecViewOffset();

	if (this == ctx->local().get())
		modify_shoot_position(shoot_position);

	return shoot_position;
}

Vector Player::get_shoot_position()
{
	if (!this)
		return Vector();

	Vector shoot_position;

	if (!this) //-V704
		return shoot_position;

	shoot_position = m_vecOrigin() + m_vecViewOffset();

	if (this == ctx->local().get())
		modify_shoot_position(shoot_position);

	return shoot_position;
}

void Player::modify_shoot_position(Vector& eye_position)
{
	if (!this) //-V704
		return;

	auto animstate = get_animation_state();

	if (!animstate)
		return;

	if (!animstate->landing && animstate->duck_amount <= 0.0f)
		return;

	auto head_bone = reinterpret_cast <int(__thiscall*)(void*, const char*)> (signatures_manager->signatures[SIGNATURE_LOOKUP_BONE])(this, crypt_str("head_0"));

	if (head_bone == -1)
		return;

	auto head_position = Vector(ctx->prediction[head_bone][0][3], ctx->prediction[head_bone][1][3], ctx->prediction[head_bone][2][3] + 1.7f);

	if (head_position.z >= eye_position.z)
		return;

	eye_position.z = math::lerp(eye_position.z, head_position.z, math::simple_spline_remap_val_clamped(abs(eye_position.z - head_position.z), 4.0f, 10.0f, 0.0f, 1.0f));
}

Vector& Player::third_person_angles()
{
	return *(Vector*)((uintptr_t)this + 0x31E8);
}

void Player::attachment_helper()
{
	if (!this) //-V704
		return;

	auto hdr = get_studio_hdr();

	if (!hdr)
		return;

	return reinterpret_cast <void(__thiscall*)(void*, CStudioHdr*)> (signatures_manager->signatures[SIGNATURE_ATTACHMENT_HELPER])(this, hdr.get());
}

bool Player::force_animations_data()
{
	if (!this) //-V704
		return false;

	for (auto i = 0; i < get_animation_layers_count(); ++i)
	{
		auto layer = get_animation_layer(i);

		if (!layer)
			return false;

		layer->m_pOwner = this;
	}

	auto animation_state = get_animation_state();

	if (!animation_state)
		return false;

	animation_state->weapon_last_bone_setup = m_hActiveWeapon().Get();
	animation_state->player = this;

	return true;
}

bool& Player::m_bIsHLTV()
{
	return *(bool*)((uintptr_t)clientstate.get() + signatures_manager->signatures[SIGNATURE_HLTV]);
}

void Player::update_animations()
{
	if (!valid()) //-V704
		return;

	auto animstate = get_animation_state();

	if (!animstate)
		return;

	if (animstate->last_update_time == globals->curtime)
		animstate->last_update_time = globals->curtime + globals->intervalpertick;

	if (animstate->last_update_frame == globals->framecount)
		animstate->last_update_frame = globals->framecount - 1;

	if (!force_animations_data())
		return;

	auto bone_snapshot_first = m_BoneSnapshotFirst();
	auto bone_snapshot_second = m_BoneSnapshotSecond();

	auto backup_eflags = m_iEFlags();
	auto backup_ishltv = m_bIsHLTV();

	m_BoneSnapshotFirst() = 1.0f;
	m_BoneSnapshotSecond() = 1.0f;

	m_iEFlags() &= ~EFL_DIRTY_ABSVELOCITY;
	m_vecAbsVelocity() = m_vecVelocity();
	m_bIsHLTV() = true;

	ctx->updating_animation = true;
	call_virtual <void(__thiscall*)(void*)>(this, INDEX_UPDATE_CLIENTSIDE_ANIMATION)(this);
	ctx->updating_animation = false;

	m_iEFlags() = backup_eflags;
	m_bIsHLTV() = backup_ishltv;

	m_BoneSnapshotFirst() = bone_snapshot_first;
	m_BoneSnapshotSecond() = bone_snapshot_second;
}

void Player::invalidate_physics_recursive(int change_flags)
{
	if (!this) //-V704
		return;

	reinterpret_cast <void(__thiscall*)(void*, int)> (signatures_manager->signatures[SIGNATURE_INVALIDATE_PHYSICS_RECURSIVE])(this, change_flags);
}

void Player::set_current_command(crypt_ptr <CUserCmd> cmd)
{
	if (!this)
		return;

	*(CUserCmd**)((uintptr_t)this + signatures_manager->signatures[SIGNATURE_CURRENT_COMMAND]) = cmd.get();
}

void Player::think()
{
	if (!this)
		return;

	call_virtual <void(__thiscall*)(void*)>(this, INDEX_THINK)(this);
}

void Player::pre_think()
{
	if (!this)
		return;

	call_virtual <void(__thiscall*)(void*)>(this, INDEX_PRE_THINK)(this);
}

void Player::post_think()
{
	if (!this)
		return;

	call_virtual <void(__thiscall*)(void*)>(this, INDEX_POST_THINK)(this);
}

crypt_ptr <mstudiobbox_t> Player::get_hitbox(int hitbox_index)
{
	if (!this)
		return nullptr;

	auto hdr = crypt_ptr <studiohdr_t>(modelinfo->GetStudioModel(GetModel()));

	if (!hdr)
		return nullptr;

	auto hitbox_set = crypt_ptr <mstudiohitboxset_t>(hdr->pHitboxSet(m_nHitboxSet()));

	if (!hitbox_set)
		return nullptr;

	return hitbox_set->pHitbox(hitbox_index);
}

Vector Player::get_bone_position(int bone)
{
	if (!this)
		return Vector();

	return Vector(m_CachedBoneData().Base()[bone][0][3], m_CachedBoneData().Base()[bone][1][3], m_CachedBoneData().Base()[bone][2][3]);
}

Vector Player::get_hitbox_position(int hitbox_index, crypt_ptr <matrix3x4_t> matrix, bool immortal_alah)
{
	if (!this)
		return Vector();

	auto hitbox = get_hitbox(hitbox_index);

	if (!hitbox)
		return Vector();

	if (!immortal_alah) {
		Vector min;
		math::vector_transform(hitbox->bbmin, matrix.get()[hitbox->bone], min);

		Vector max;
		math::vector_transform(hitbox->bbmax, matrix.get()[hitbox->bone], max);

		auto center = (min + max) * 0.5f;
		return center;
	}

	auto bobo = Vector((hitbox->bbmin.x + hitbox->bbmax.x) * 0.5f, (hitbox->bbmin.y + hitbox->bbmax.y) * 0.5f, (hitbox->bbmin.z + hitbox->bbmin.z) * 0.9f);
	Vector arti;
	math::vector_transform(bobo, matrix.get()[hitbox->bone], arti);

	return arti;
}

crypt_ptr <AnimationState> Player::get_animation_state()
{
	if (!this) //-V704
		return nullptr;

	return *(AnimationState**)((void*)((uintptr_t)this + 0x9960));
}

crypt_ptr <AnimationLayer> Player::get_animation_layer(int index)
{
	if (!this)
		return nullptr;

	return &(*(AnimationLayer**)((uintptr_t)this + 0x2990))[index];
}

int Player::get_animation_layers_count()
{
	if (!this) //-V704
		return 0;

	return *(int*)((uintptr_t)this + 0x299C);
}

CUtlVector <matrix3x4_t>& Player::m_CachedBoneData()
{
	return *(CUtlVector <matrix3x4_t>*)((uintptr_t)this + signatures_manager->signatures[SIGNATURE_CACHED_BONE_DATA]);
}

void Player::UpdateCollisionBounds()
{
	set_collision_bounds(movement->GetPlayerMins(m_fFlags() & FL_DUCKING), movement->GetPlayerMaxs(m_fFlags() & FL_DUCKING), true);
}

void Player::set_collision_bounds(const Vector& mins, const Vector& maxs, bool ignore_restore)
{
	if (!this)
		return;

	auto collideable = crypt_ptr <ICollideable>(GetCollideable());

	if (!collideable)
		return;

	const auto c_time = m_flCollisionChangeTime();
	const auto c_org = m_flCollisionChangeOrigin();

	reinterpret_cast <void(__thiscall*)(ICollideable*, const Vector&, const Vector&)> (signatures_manager->signatures[SIGNATURE_SET_COLLISION_BOUNDS])(collideable.get(), mins, maxs);

	if (!ignore_restore) {
		m_flCollisionChangeOrigin() = c_time;
		m_flCollisionChangeTime() = c_org;
	}
}

crypt_ptr <CStudioHdr> Player::get_studio_hdr()
{
	if (!this)
		return nullptr;

	return *(CStudioHdr**)((uintptr_t)this + *(uintptr_t*)(signatures_manager->signatures[SIGNATURE_STUDIO_HDR] + 0x2) + 0x4);
}

int Player::get_sequence_activity(int sequence)
{
	if (!this) //-V704
		return 0;

	auto model = crypt_ptr <model_t>((model_t*)GetModel());

	if (!model)
		return 0;

	auto hdr = crypt_ptr <studiohdr_t>(modelinfo->GetStudioModel(model.get()));

	if (!hdr)
		return 0;

	return reinterpret_cast <int(__fastcall*)(void*, studiohdr_t*, int)> (signatures_manager->signatures[SIGNATURE_SEQUENCE_ACTIVITY])(this, hdr.get(), sequence);
}

bool Weapon::can_fire()
{
	if (!this) //-V704
		return false;

	auto weapon_data = get_weapon_data();

	if (!weapon_data)
		return false;

	if (weapon_data->weapon_type == WEAPONTYPE_KNIFE_TASER && m_iItemDefinitionIndex() != WEAPON_TASER)
		return false;

	if (weapon_data->weapon_type == WEAPONTYPE_OTHER || weapon_data->weapon_type >= WEAPONTYPE_MAX)
		return false;

	return true;
}

bool Weapon::executed(crypt_ptr <CUserCmd> cmd, bool anti_aim) //-V813
{
	if (!this) //-V704
		return false;

	auto weapon_data = this == ctx->weapon().get() ? ctx->weapon_data() : get_weapon_data();

	if (!weapon_data)
		return false;

	if (weapon_data->weapon_type == WEAPONTYPE_KNIFE_TASER && m_iItemDefinitionIndex() != WEAPON_TASER)
		return cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2;
	else if (is_grenade())
		return anti_aim ? m_fThrowTime() > 0.0f : false;
	else if (weapon_data->weapon_type == WEAPONTYPE_OTHER || weapon_data->weapon_type >= WEAPONTYPE_MAX)
		return false;

	return !ctx->automatic_revolver && cmd->buttons & IN_ATTACK;
}

bool Weapon::is_grenade()
{
	if (!this)
		return false;

	if (m_iItemDefinitionIndex() == WEAPON_FLASHBANG)
		return true;

	if (m_iItemDefinitionIndex() == WEAPON_HEGRENADE)
		return true;

	if (m_iItemDefinitionIndex() == WEAPON_SMOKEGRENADE)
		return true;

	if (m_iItemDefinitionIndex() == WEAPON_MOLOTOV)
		return true;

	if (m_iItemDefinitionIndex() == WEAPON_DECOY)
		return true;

	if (m_iItemDefinitionIndex() == WEAPON_INCGRENADE)
		return true;

	if (m_iItemDefinitionIndex() == WEAPON_TAGRENADE)
		return true;

	if (m_iItemDefinitionIndex() == WEAPON_FRAG_GRENADE)
		return true;

	return false;
}

CEconItemDefinition* C_EconItemView::GetStaticData()
{
	if (!this) //-V704
		return nullptr;

	return reinterpret_cast <CEconItemDefinition * (__thiscall*)(void*)> (signatures_manager->signatures[SIGNATURE_GET_STATIC_DATA])(this);
}

CEconItem* C_EconItemView::GetSOCData()
{
	if (!this) //-V704
		return nullptr;

	return reinterpret_cast <CEconItem * (__thiscall*)(C_EconItemView*)> (signatures_manager->signatures[SIGNATURE_GET_SOC_DATA])(this);
}

int CEconItemDefinition::GetEquippedPosition()
{
	if (!this) //-V704
		return 0;

	return *(uintptr_t*)((uintptr_t)this + 0x24C); //-V110
}

int Weapon::get_weapon_group()
{
	if (!this) //-V704
		return WEAPON_CONFIG_INVALID;

	auto weapon_data = get_weapon_data();

	if (!weapon_data)
		return WEAPON_CONFIG_INVALID;

	if (weapon_data->weapon_type == WEAPONTYPE_KNIFE_TASER && m_iItemDefinitionIndex() != WEAPON_TASER)
		return WEAPON_CONFIG_KNIFE;

	switch (m_iItemDefinitionIndex())
	{
	case WEAPON_DEAGLE:
	case WEAPON_REVOLVER:
		return WEAPON_CONFIG_HEAVY_PISTOL;
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_P250:
	case WEAPON_GLOCK:
	case WEAPON_HKP2000:
	case WEAPON_CZ75A:
	case WEAPON_USP_SILENCER:
	case WEAPON_TEC9:
		return WEAPON_CONFIG_PISTOL;
	case WEAPON_G3SG1:
	case WEAPON_SCAR20:
		return WEAPON_CONFIG_AUTO;
	case WEAPON_SSG08:
		return WEAPON_CONFIG_SCOUT;
	case WEAPON_AWP:
		return WEAPON_CONFIG_AWP;
	case WEAPON_AK47:
	case WEAPON_M4A1:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_GALILAR:
	case WEAPON_FAMAS:
	case WEAPON_AUG:
	case WEAPON_SG553:
		return WEAPON_CONFIG_RIFLE;
	case WEAPON_MAC10:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_P90:
	case WEAPON_BIZON:
	case WEAPON_UMP45:
	case WEAPON_MP5SD:
		return WEAPON_CONFIG_SMG;
	case WEAPON_XM1014:
	case WEAPON_NOVA:
	case WEAPON_SAWEDOFF:
	case WEAPON_MAG7:
		return WEAPON_CONFIG_HEAVY;
	case WEAPON_M249:
	case WEAPON_NEGEV:
		return WEAPON_CONFIG_MACHINE_GUN;
	case WEAPON_TASER:
		return WEAPON_CONFIG_TASER;
	}

	return WEAPON_CONFIG_INVALID;
}

bool& Weapon::m_bCustomMaterialInitialized()
{
	return *(bool*)((uintptr_t)this + signatures_manager->signatures[SIGNATURE_CUSTOM_MATERIALS_INITIALIZE]);
}

CUtlVector <CRefCounted*>& Weapon::m_CustomMaterials()
{
	return *(CUtlVector< CRefCounted* >*)((uintptr_t)this + signatures_manager->signatures[SIGNATURE_CUSTOM_MATERIALS]);
}

void Entity::set_model_index(int index)
{
	if (!this) //-V704
		return;

	return call_virtual <void(__thiscall*)(void*, int)>(this, INDEX_SET_MODEL_INDEX)(this, index);
}

string Entity::get_class_name()
{
	if (!this) //-V704
		return string();

	auto client_class = crypt_ptr <ClientClass>(GetClientClass());

	if (!client_class)
		return string();

	return client_class->m_pNetworkName;
}

int Entity::get_class_id()
{
	if (!this) //-V704
		return -1;

	auto client_class = crypt_ptr <ClientClass>(GetClientClass());

	if (!client_class)
		return -1;

	return client_class->m_ClassID;
}

bool Entity::is_breakable()
{
	if (!this) //-V704
		return false;

	if (!EntIndex())
		return false;

	if (reinterpret_cast <bool(__thiscall*)(void*)> (signatures_manager->signatures[SIGNATURE_IS_BREAKABLE])(this))
		return true;

	auto client_class = crypt_ptr <ClientClass>(GetClientClass());

	if (!client_class)
		return false;

	auto collideable = crypt_ptr <ICollideable>(GetCollideable());

	if (!collideable)
		return false;

	return client_class->m_ClassID == CBaseDoor || client_class->m_ClassID == CBreakableSurface || client_class->m_ClassID == CFuncBrush || client_class->m_ClassID == CBaseEntity && collideable->GetSolid() == 1; //-V648
}

bool Entity::get_prop_bool(const string& table, const string& netvar)
{
	if (!this) //-V704
		return false;

	auto full_name = table + netvar;

	if (ctx->props.find(full_name) == ctx->props.end())
		ctx->props[full_name] = netvars_manager->find_offset(crypt_hash_r(table.c_str()), crypt_hash_r(netvar.c_str()));

	if (!ctx->props[full_name])
		return false;

	return *(bool*)((uintptr_t)this + ctx->props[full_name]);
}

int Entity::get_prop_int(const string& table, const string& netvar)
{
	if (!this) //-V704
		return 0;

	auto full_name = table + netvar;

	if (ctx->props.find(full_name) == ctx->props.end())
		ctx->props[full_name] = netvars_manager->find_offset(crypt_hash_r(table.c_str()), crypt_hash_r(netvar.c_str()));

	if (!ctx->props[full_name])
		return 0;

	return *(int*)((uintptr_t)this + ctx->props[full_name]);
}

float Entity::get_prop_float(const string& table, const string& netvar)
{
	if (!this) //-V704
		return 0.0f;

	auto full_name = table + netvar;

	if (ctx->props.find(full_name) == ctx->props.end())
		ctx->props[full_name] = netvars_manager->find_offset(crypt_hash_r(table.c_str()), crypt_hash_r(netvar.c_str()));

	if (!ctx->props[full_name])
		return 0.0f;

	return *(float*)((uintptr_t)this + ctx->props[full_name]);
}

string Entity::get_prop_string(const string& table, const string& netvar)
{
	if (!this) //-V704
		return string();

	auto full_name = table + netvar;

	if (ctx->props.find(full_name) == ctx->props.end())
		ctx->props[full_name] = netvars_manager->find_offset(crypt_hash_r(table.c_str()), crypt_hash_r(netvar.c_str()));

	if (!ctx->props[full_name])
		return string();

	return *(string*)((uintptr_t)this + ctx->props[full_name]);
}

void Entity::set_prop_bool(const string& table, const string& netvar, const bool& value)
{
	if (!this) //-V704
		return;

	auto full_name = table + netvar;

	if (ctx->props.find(full_name) == ctx->props.end())
		ctx->props[full_name] = netvars_manager->find_offset(crypt_hash_r(table.c_str()), crypt_hash_r(netvar.c_str()));

	if (!ctx->props[full_name])
		return;

	*(bool*)((uintptr_t)this + ctx->props[full_name]) = value;
}

void Entity::set_prop_int(const string& table, const string& netvar, const int& value)
{
	if (!this) //-V704
		return;

	auto full_name = table + netvar;

	if (ctx->props.find(full_name) == ctx->props.end())
		ctx->props[full_name] = netvars_manager->find_offset(crypt_hash_r(table.c_str()), crypt_hash_r(netvar.c_str()));

	if (!ctx->props[full_name])
		return;

	*(int*)((uintptr_t)this + ctx->props[full_name]) = value;
}

void Entity::set_prop_float(const string& table, const string& netvar, const float& value)
{
	if (!this) //-V704
		return;

	auto full_name = table + netvar;

	if (ctx->props.find(full_name) == ctx->props.end())
		ctx->props[full_name] = netvars_manager->find_offset(crypt_hash_r(table.c_str()), crypt_hash_r(netvar.c_str()));

	if (!ctx->props[full_name])
		return;

	*(float*)((uintptr_t)this + ctx->props[full_name]) = value;
}

void Entity::set_prop_string(const string& table, const string& netvar, const string& value)
{
	if (!this) //-V704
		return;

	auto full_name = table + netvar;

	if (ctx->props.find(full_name) == ctx->props.end())
		ctx->props[full_name] = netvars_manager->find_offset(crypt_hash_r(table.c_str()), crypt_hash_r(netvar.c_str()));

	if (!ctx->props[full_name])
		return;

	*(string*)((uintptr_t)this + ctx->props[full_name]) = value;
}

float Weapon::get_spread()
{
	if (!this) //-V704
		return 0.0f;

	return call_virtual <float(__thiscall*)(void*)>(this, INDEX_GET_SPREAD)(this);
}

float Weapon::get_inaccuracy()
{
	if (!this) //-V704
		return 0.0f;

	return call_virtual <float(__thiscall*)(void*)>(this, INDEX_GET_INACCURACY)(this);
}

void Weapon::update_accuracy_penalty()
{
	if (!this) //-V704
		return;

	call_virtual <void(__thiscall*)(void*)>(this, INDEX_UPDATE_ACCURACY_PENALTY)(this);
}

crypt_ptr <WeaponData> Weapon::get_weapon_data()
{
	if (!this) //-V704
		return nullptr;

	return call_virtual <WeaponData* (__thiscall*)(void*)>(this, INDEX_WEAPON_DATA)(this);
}