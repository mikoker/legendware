// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "build_bones.h"

void BuildBones::initialize(crypt_ptr <Player> player, crypt_ptr <AnimationState> animation_state, crypt_ptr <matrix3x4_t> matrix, int mask) //-V688
{
	this->player = player;
	this->matrix = matrix ? matrix : player->m_CachedBoneData().Base();
	this->mask = mask & ~BONE_USED_BY_BONE_MERGE;

	animation_layers_count = this->player->get_animation_layers_count();
	time = this->player->m_flSimulationTime();

	angles = Vector(this->player->GetAbsAngles().x, animation_state->goal_feet_yaw, this->player->GetAbsAngles().z);
	origin = this->player->m_vecOrigin();

	weapon = this->player->m_hActiveWeapon().Get();
	weapon_world_model = weapon ? weapon->m_hWeaponWorldModel().Get() : nullptr;
	studio_hdr = this->player->get_studio_hdr();
	animation_layers = this->player->get_animation_layer();

	memcpy(pose_parameters, this->player->m_flPoseParameters(), 24 * sizeof(float));

	if (weapon_world_model)
		memcpy(world_pose_parameters, weapon_world_model->m_flPoseParameters(), 24 * sizeof(float));
	else
		memcpy(world_pose_parameters, pose_parameters, 24 * sizeof(float));
}

void BuildBones::setup()
{
	if (!studio_hdr)
		return;

	player->m_iEFlags() |= EFL_SETTING_UP_BONES;

	if (!*(bool*)(uintptr_t(studio_hdr->m_pStudioHdr) + 0x150) || *(int*)((uintptr_t)studio_hdr.get() + 0x4))
		get_skeleton();

	matrix3x4_t transform;

	transform.AngleMatrix(angles, origin);
	studio_build_matrices(transform);

	player->m_iEFlags() &= ~EFL_SETTING_UP_BONES;
	fix_bones_rotations();
}

void BuildBones::get_skeleton()
{
	alignas(16) char buffer[32];
	alignas(16) auto bone_setup = (BoneSetup*)&buffer;

	bone_setup->studio_hdr = studio_hdr.get();
	bone_setup->mask = mask;
	bone_setup->pose_parameter = pose_parameters;
	bone_setup->pose_debugger = nullptr;

	bone_setup->init_pose(vector_bones, quaternion_bones, studio_hdr.get());
	bone_setup->accumulate_pose(vector_bones, quaternion_bones, player->m_nSequence(), player->m_flCycle(), 1.0f, time);

	int layers[13];

	for (auto i = 0; i < animation_layers_count; ++i)
	{
		auto layer = crypt_ptr <AnimationLayer> (&animation_layers.get()[i]);

		if (layer->m_flWeight > 0.0f && layer->m_nOrder < animation_layers_count)
			layers[layer->m_nOrder] = i;
	}

	if (player->can_be_animated())
	{
		auto weapon_world_model_player = crypt_ptr <Player> ((Player*)weapon_world_model.get()); //-V1027

		if (weapon_world_model_player)
		{
			auto bone_merge = weapon_world_model_player->get_bone_merge();

			if (bone_merge)
			{
				merge_matching_pose_params(bone_merge, world_pose_parameters, pose_parameters);

				for (auto i = 0; i < animation_layers_count; ++i)
				{
					auto layer = crypt_ptr <AnimationLayer> (&animation_layers.get()[i]);

					if (!layer)
						continue;

					if (layer->m_nSequence <= 1)
						continue;

					if (layer->m_flWeight <= 0.0f)
						continue;

					bone_setup->accumulate_pose(vector_bones, quaternion_bones, layer->m_nSequence, layer->m_flCycle, layer->m_flWeight, time);
				}
			}
		}
	}
	else
	{
		for (auto i = 0; i < animation_layers_count; ++i)
		{
			auto count = layers[i];

			if (count >= 0 && count < animation_layers_count)
			{
				auto layer = crypt_ptr <AnimationLayer> (&animation_layers.get()[i]);

				alignas(16) Vector pos2[256];
				alignas(16) Quaternion rot2[256];

				bone_setup->accumulate_pose(pos2, rot2, layer->m_nSequence, layer->m_flCycle, layer->m_flWeight, time);
			}
		}
	}
}

void BuildBones::studio_build_matrices(matrix3x4_t& transform)
{
	auto bone = -1;
	auto hdr = crypt_ptr <studiohdr_t> (*(studiohdr_t**)studio_hdr.get());

	if (bone >= hdr->numbones)
		bone = 0;

	auto chain_length = 0;
	auto bone_parent = (CUtlVector <int>*) ((uintptr_t)studio_hdr.get() + 0x44);

	int chain[MAXSTUDIOBONES];

	if (bone <= -1)
	{
		chain_length = hdr->numbones;

		for (auto i = 0; i < hdr->numbones; ++i)
			chain[chain_length - i - 1] = i;
	}
	else
	{
		auto i = bone;

		do
		{
			chain[chain_length++] = i;
			i = bone_parent->m_Memory.Element(i);
		}
		while (i != -1);
	}

	uint32_t bone_computed[8] =
	{
		0
	};

	auto bone_flags = (CUtlVector <int>*) ((uintptr_t)studio_hdr.get() + 0x30);

	for (auto j = chain_length - 1; j >= 0; --j)
	{
		auto i = chain[j];

		if (1 << (i & 0x1F) & bone_computed[i >> 5])
			continue;

		auto flag = bone_flags->m_Memory.Element(i);

		if (mask & flag && quaternion_bones)
		{
			matrix3x4_t bone_matrix;
			bone_matrix.QuaternionMatrix(quaternion_bones[i], vector_bones[i]);

			auto parent = bone_parent->m_Memory.Element(i);

			if (parent == -1)
				matrix.get()[i] = transform.ConcatTransforms(bone_matrix);
			else
				matrix.get()[i] = matrix.get()[parent].ConcatTransforms(bone_matrix);
		}
	}
}

void BuildBones::fix_bones_rotations()
{
	auto hdr = crypt_ptr <studiohdr_t> (*(studiohdr_t**)studio_hdr.get());

	if (!hdr)
		return;

	auto hitbox_set = crypt_ptr <mstudiohitboxset_t> (hdr->pHitboxSet(player->m_nHitboxSet()));

	if (!hitbox_set)
		return;

	for (auto i = 0; i < hitbox_set->numhitboxes; ++i)
	{
		auto hitbox = crypt_ptr <mstudiobbox_t> (hitbox_set->pHitbox(i));

		if (hitbox->rotation.IsZero())
			continue;

		matrix3x4_t hitbox_transform;
		hitbox_transform.AngleMatrix(hitbox->rotation);

		matrix.get()[hitbox->bone] = matrix.get()[hitbox->bone].ConcatTransforms(hitbox_transform);
	}
}

float BuildBones::get_pose_param_value(crypt_ptr <CStudioHdr> hdr, int index, float value)
{
	if (index < 0 || index > 24)
		return 0.0f;

	auto pose_param = get_pose_param_desc(hdr, index);

	if (!pose_param)
		return 0.0f;

	if (pose_param->loop)
	{ //-V550
		auto wrap = (pose_param->start + pose_param->end) / 2.0f + pose_param->loop / 2.0f;
		auto shift = pose_param->loop - wrap;

		value -= pose_param->loop * floor((value + shift) / pose_param->loop);
	}

	return (value - pose_param->start) / (pose_param->end - pose_param->start);
}

crypt_ptr <mstudioposeparamdesc_t> BuildBones::get_pose_param_desc(crypt_ptr <CStudioHdr> hdr, int index)
{
	return reinterpret_cast <mstudioposeparamdesc_t*(__thiscall*)(CStudioHdr*, int)> (signatures_manager->signatures[SIGNATURE_POSE_PARAM_DESC])(hdr.get(), index);
}

void BuildBones::merge_matching_pose_params(uintptr_t bone_merge, crypt_ptr <float> poses, crypt_ptr <float> target_poses)
{
	reinterpret_cast <void(__thiscall*)(uintptr_t)> (signatures_manager->signatures[SIGNATURE_UPDATE_CACHE])(bone_merge);

	if (*(DWORD*)(bone_merge + 0x10) && *(DWORD*)(bone_merge + 0x8C))
	{
		auto i = 0;
		auto index = (int*)(bone_merge + 0x20);

		do
		{
			if (*index != -1)
			{
				auto target = *(Player**)(bone_merge + 0x4);
				auto hdr = target->get_studio_hdr();
				auto pose_param_value = 0.0f;

				if (hdr && *(studiohdr_t**)hdr.get())
				{
					auto pose_param_desc = get_pose_param_desc(hdr, i);

					if (pose_param_desc)
						pose_param_value = target_poses.get()[i] * (pose_param_desc->end - pose_param_desc->start) + pose_param_desc->start;
				}

				auto target2 = *(Player**)(bone_merge);
				auto hdr2 = target2->get_studio_hdr();

				poses.get()[*index] = get_pose_param_value(hdr2, *index, pose_param_value);
			}

			++i;
			++index;
		}
		while (i < 24);
	}
}