 // This is an independent project of an individual developer. Dear PVS-Studio, please check it.
 // PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "local_animations.h"
#include "exploits.h"
#include "prediction.h"
#include "features.h"
#include "anti_aim.h"

void LocalAnimations::render()
{
	auto backup_angles = ctx->local()->GetAbsAngles();

	AnimationLayer backup_layers[13];
	memcpy(backup_layers, ctx->local()->get_animation_layer().get(), ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

	float backup_pose_parameters[24];
	memcpy(backup_pose_parameters, ctx->local()->m_flPoseParameters(), 24 * sizeof(float)); //-V512

	if (update_fake_bones || !config->visuals.local.visualize_lag)
	{
		update_fake_bones = false;

		memcpy(ctx->local()->m_flPoseParameters(), pose_parameters[SIDE_FAKE], 24 * sizeof(float)); //-V512
		ctx->local()->set_abs_angles(Vector(0.0f, angles[SIDE_FAKE], 0.0f));
		memcpy(ctx->local()->get_animation_layer().get(), layers[SIDE_FAKE], ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

		ctx->local()->setup_bones(ctx->fake, BONE_USED_BY_ANYTHING);
	}

	memcpy(ctx->local()->m_flPoseParameters(), pose_parameters[SIDE_REAL], 24 * sizeof(float)); //-V512
	ctx->local()->set_abs_angles(Vector(0.0f, angles[SIDE_REAL], 0.0f));
	memcpy(ctx->local()->get_animation_layer().get(), layers[SIDE_REAL], ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

	ctx->local()->setup_bones(nullptr, BONE_USED_BY_ANYTHING);

#if 1
	auto backup_curtime = globals->curtime;

	ctx->arti_never_wanted_to_fix_this = ctx->local()->m_fFlags() & FL_ONGROUND && engine_prediction->flags & FL_ONGROUND;
	globals->curtime = clientstate->m_ClockDriftMgr.m_nServerTick;

	((void(__thiscall*)(void*, void*, int))signatures_manager->signatures[SIGNATURE_MODIFY_BONES])(ctx->local().get(), ctx->local()->m_CachedBoneData().Base(), BONE_USED_BY_ANYTHING);

	globals->curtime = backup_curtime;
	ctx->arti_never_wanted_to_fix_this = false;
#endif

	ctx->local()->attachment_helper();

	memcpy(ctx->local()->m_flPoseParameters(), backup_pose_parameters, 24 * sizeof(float)); //-V512
	memcpy(ctx->local()->get_animation_layer().get(), backup_layers, ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

	ctx->local()->set_abs_angles(backup_angles);
}

void LocalAnimations::run(crypt_ptr <CUserCmd> cmd, bool send_packet) //-V813
{
	if (!ctx->local()->valid())
		return;

	if (send_packet)
		sent_angle = cmd->viewangles;

	if (!ctx->animation_state())
		return;

	if (cmd->buttons & IN_ATTACK && !ctx->automatic_revolver)
	{
		angle_locked = true;
		angle = cmd->viewangles;
	}
	else if (!angle_locked)
		angle = cmd->viewangles;

	auto backup_abs_angles = ctx->local()->GetAbsAngles();
	auto backup_angles = ctx->local()->third_person_angles();

	float backup_pose_parameters[24];
	memcpy(backup_pose_parameters, ctx->local()->m_flPoseParameters(), 24 * sizeof(float)); //-V512

	AnimationLayer backup_layers[13];
	memcpy(backup_layers, ctx->local()->get_animation_layer().get(), ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

	if (ctx->local()->m_flSpawnTime() != last_spawn || ctx->animation_state()->player != ctx->local().get()) //-V550
	{
		ctx->animation_state()->player = ctx->local().get();
		ctx->local()->update_animations();

		fake_animstate = *ctx->animation_state().get();
		memcpy(ctx->local()->get_animation_layer().get(), backup_layers, ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

		last_spawn = ctx->local()->m_flSpawnTime();
	}

	if (!ctx->original_choked_commands)
	{
		ctx->animation_state()->primary_cycle = ctx->local()->get_animation_layer(6)->m_flCycle;
		ctx->animation_state()->move_weight = ctx->local()->get_animation_layer(6)->m_flWeight;
		ctx->animation_state()->strafe_sequence = ctx->local()->get_animation_layer(7)->m_nSequence;
		ctx->animation_state()->strafe_change_weight = ctx->local()->get_animation_layer(7)->m_flWeight;
		ctx->animation_state()->strafe_change_cycle = ctx->local()->get_animation_layer(7)->m_flCycle;
		ctx->animation_state()->acceleration_weight = ctx->local()->get_animation_layer(12)->m_flWeight;
	}

	ctx->local()->third_person_angles() = angle;
	ctx->local()->update_animations();

	memcpy(pose_parameters_shoot, ctx->local()->m_flPoseParameters(), 24 * sizeof(float)); //-V512
	memcpy(layers_shoot, ctx->local()->get_animation_layer().get(), ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer)); //-V512

	if (!send_packet)
	{
		memcpy(ctx->local()->m_flPoseParameters(), backup_pose_parameters, 24 * sizeof(float)); //-V512
		memcpy(ctx->local()->get_animation_layer().get(), backup_layers, ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

		ctx->local()->third_person_angles() = backup_angles;
		ctx->local()->set_abs_angles(backup_abs_angles);
		return;
	}

	update_fake_bones = true;

	if (!angle_locked || !exploits->tickbase_shift)
	{
		memcpy(pose_parameters[SIDE_REAL], ctx->local()->m_flPoseParameters(), 24 * sizeof(float)); //-V512
		angles[SIDE_REAL] = ctx->animation_state()->goal_feet_yaw;
		memcpy(layers[SIDE_REAL], ctx->local()->get_animation_layer().get(), ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

		memcpy(ctx->local()->m_flPoseParameters(), backup_pose_parameters, 24 * sizeof(float)); //-V512
		memcpy(ctx->local()->get_animation_layer().get(), backup_layers, ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));
	}

	auto backup_state = *ctx->animation_state().get();

	fake_animstate.landing = false;
	fake_animstate.duck_additional = 0.0f;
	fake_animstate.land_multiplier = 1.0f;
	fake_animstate.last_update_time = globals->curtime - TICKS_TO_TIME(ctx->original_choked_commands + 1);

	*ctx->animation_state().get() = fake_animstate;

	ctx->local()->third_person_angles() = angle;
	ctx->local()->update_animations();

	fake_animstate = *ctx->animation_state().get();

	*ctx->animation_state().get() = backup_state;
	ctx->local()->get_animation_layer(12).get()->m_flWeight = 0.0f;

	memcpy(pose_parameters[SIDE_FAKE], ctx->local()->m_flPoseParameters(), 24 * sizeof(float)); //-V512
	angles[SIDE_FAKE] = fake_animstate.goal_feet_yaw;
	memcpy(layers[SIDE_FAKE], ctx->local()->get_animation_layer().get(), ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

	memcpy(ctx->local()->m_flPoseParameters(), backup_pose_parameters, 24 * sizeof(float)); //-V512
	memcpy(ctx->local()->get_animation_layer().get(), backup_layers, ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

	ctx->local()->third_person_angles() = backup_angles;
	ctx->local()->set_abs_angles(backup_abs_angles);

	angle_locked = false;
}