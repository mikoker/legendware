#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"

class BuildBones //-V730
{
	int mask = 0;
	int animation_layers_count = 0;

	float time = 0.0f;

	Vector angles;
	Vector origin;

	crypt_ptr <Player> player;
	crypt_ptr <Weapon> weapon;
	crypt_ptr <Weapon> weapon_world_model;
	crypt_ptr <CStudioHdr> studio_hdr;
	crypt_ptr <AnimationLayer> animation_layers;
	crypt_ptr <matrix3x4_t> matrix;

	float pose_parameters[24];
	float world_pose_parameters[24];

	alignas(16) Vector vector_bones[128];
	alignas(16) Quaternion quaternion_bones[128];
public:
	virtual void initialize(crypt_ptr <Player> player, crypt_ptr <AnimationState> animation_state, crypt_ptr <matrix3x4_t> matrix, int mask);
	virtual void setup();
	virtual void get_skeleton();
	virtual void studio_build_matrices(matrix3x4_t& transform);
	virtual void fix_bones_rotations();
	virtual float get_pose_param_value(crypt_ptr <CStudioHdr> hdr, int index, float value);
	virtual crypt_ptr <mstudioposeparamdesc_t> get_pose_param_desc(crypt_ptr <CStudioHdr> hdr, int index);
	virtual void merge_matching_pose_params(uintptr_t bone_merge, crypt_ptr <float> poses, crypt_ptr <float> target_poses);
};

struct BoneSetup
{
	CStudioHdr* studio_hdr = nullptr;
	int mask = 0;
	float* pose_parameter = nullptr;
	void* pose_debugger = nullptr;

	void init_pose(Vector pos[], Quaternion q[], CStudioHdr* hdr)
	{
		auto init_pose_address = signatures_manager->signatures[SIGNATURE_INIT_POSE];

		__asm
		{
			mov eax, this
			mov esi, q
			mov edx, pos
			push dword ptr[hdr + 4]
			mov ecx, [eax]
			push esi
			call init_pose_address
			add esp, 8
		}
	}

	void accumulate_pose(Vector pos[], Quaternion q[], int sequence, float cycle, float weight, float time)
	{
		return reinterpret_cast <void(__thiscall*)(BoneSetup*, Vector*, Quaternion*, int, float, float, float, void*)> (signatures_manager->signatures[SIGNATURE_ACCUMULATE_POSE])(this, pos, q, sequence, cycle, weight, time, nullptr);
	}

	void calc_autoplay_sequences(Vector pos[], Quaternion q[], float time)
	{
		auto calc_autoplay_sequences_address = signatures_manager->signatures[SIGNATURE_CALC_AUTOPLAY_SEQUENCES];

		__asm
		{
			movss xmm3, time
			mov eax, 0
			mov ecx, this
			push eax
			push q
			push pos
			call calc_autoplay_sequences_address
		}
	}

	void calc_bone_adj(Vector pos[], Quaternion q[], float* controllers)
	{
		auto calc_bone_adj_address = signatures_manager->signatures[SIGNATURE_CALC_BONE_ADJUST];

		__asm
		{
			mov eax, controllers
			mov ecx, this
			mov edx, pos
			push dword ptr[ecx + 4]
			mov ecx, [ecx]
			push eax
			push q
			call calc_bone_adj_address
			add esp, 0xC
		}
	}
};