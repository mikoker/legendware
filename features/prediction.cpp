// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "features.h"
#include "prediction.h"
#include "exploits.h"
#include "logs.h"

void Prediction::start(crypt_ptr <CUserCmd> cmd)
{
	cmd->viewangles = math::normalize_angles(cmd->viewangles);

	curtime = globals->curtime;
	frametime = globals->frametime;
	in_prediction = prediction->m_bInPrediction;

	if (exploits->charging)
		return;

	cmd->forwardmove = clamp(cmd->forwardmove, -convars_manager->convars[CONVAR_CL_FORWARDSPEED]->GetFloat(), convars_manager->convars[CONVAR_CL_FORWARDSPEED]->GetFloat());
	cmd->sidemove = clamp(cmd->sidemove, -convars_manager->convars[CONVAR_CL_SIDESPEED]->GetFloat(), convars_manager->convars[CONVAR_CL_SIDESPEED]->GetFloat());
	cmd->upmove = clamp(cmd->upmove, -convars_manager->convars[CONVAR_CL_UPSPEED]->GetFloat(), convars_manager->convars[CONVAR_CL_UPSPEED]->GetFloat());
	cmd->random_seed = MD5_PseudoRandom(cmd->command_number) & INT_MAX;

	ctx->local()->set_current_command(cmd.get());

	if (!prediction_random_seed)
		prediction_random_seed = *(int**)signatures_manager->signatures[SIGNATURE_PREDICTION_RANDOM_SEED];
	else
		*(int*)prediction_random_seed.get() = cmd->random_seed;

	if (!prediction_player)
		prediction_player = *(int**)signatures_manager->signatures[SIGNATURE_PREDICTION_PLAYER];
	else
		*(Player**)prediction_player.get() = ctx->local().get(); //-V114

	auto maxs = ctx->local()->GetCollideable()->OBBMaxs();

	globals->curtime = TICKS_TO_TIME(ctx->local()->m_nTickBase());
	globals->frametime = prediction->m_bEnginePaused ? 0.0f : globals->intervalpertick;

	prediction->m_bInPrediction = true;

	movement->StartTrackPredictionErrors(ctx->local().get());
	movehelper->set_host(ctx->local().get());

	prediction->SetupMove(ctx->local().get(), cmd.get(), movehelper.get(), &move_data);
	movement->ProcessMovement(ctx->local().get(), &move_data);
	prediction->FinishMove(ctx->local().get(), cmd.get(), &move_data);

	ctx->local()->UpdateCollisionBounds();
}

void Prediction::update_data()
{
	ctx->weapon()->update_accuracy_penalty();

	ctx->spread = ctx->weapon()->get_spread();
	ctx->inaccuracy = ctx->weapon()->get_inaccuracy();

	store_viewmodel();

	float backup_pose_parameters[24];
	memcpy(backup_pose_parameters, ctx->local()->m_flPoseParameters(), 24 * sizeof(float)); //-V512

	AnimationLayer backup_layers[13];
	memcpy(backup_layers, ctx->local()->get_animation_layer().get(), ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

	memcpy(ctx->local()->m_flPoseParameters(), local_animations->pose_parameters_shoot, 24 * sizeof(float));
	memcpy(ctx->local()->get_animation_layer().get(), local_animations->layers_shoot, ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

	ctx->local()->m_flPoseParameters()[12] = 0.5f;

	ctx->local()->set_abs_angles(Vector(0.0f, 0.0f, 0.0f));
	ctx->local()->setup_bones(ctx->prediction, BONE_USED_BY_HITBOX);

	memcpy(ctx->local()->m_flPoseParameters(), backup_pose_parameters, 24 * sizeof(float)); //-V512
	memcpy(ctx->local()->get_animation_layer().get(), backup_layers, ctx->local()->get_animation_layers_count() * sizeof(AnimationLayer));

	ctx->shoot_position = ctx->local()->get_shoot_position();
}

void Prediction::store_data(crypt_ptr <CUserCmd> cmd)
{
	flags = ctx->local()->m_fFlags();
	move_type = ctx->local()->m_moveType();
	duck_amount = ctx->local()->m_flDuckAmount();
	velocity = ctx->local()->m_vecVelocity();
	buttons = cmd->buttons;
	velocity_modifier = ctx->local()->m_flVelocityModifier();
	origin = ctx->local()->m_vecLastPredictedPosition();

	auto previous_command_number = cmd->command_number - 1;
	auto delta_tick = previous_command_number - clientstate->m_nLastCommandAck;
	auto previous_cmd = input->GetUserCmd(previous_command_number);
	auto data = crypt_ptr <NetvarsData>(&netvars_data[previous_command_number % MULTIPLAYER_BACKUP]);

	if (previous_cmd && previous_cmd->predicted
		&& ctx->last_predicted_command == previous_cmd->command_number
		&& previous_command_number == previous_cmd->command_number
		&& clientstate->m_nDeltaTick == prediction->m_nPreviousStartFrame
		&& previous_cmd->command_number == data->m_command_number)
	{
		if (delta_tick - 1 >= 0)
			restore_netvars(previous_command_number);
	}
	else
		prediction->Update(clientstate->m_nDeltaTick, clientstate->m_nDeltaTick > 0, clientstate->m_nLastCommandAck, previous_command_number);

	if (ctx->weapon() && previous_cmd->command_number == data->m_command_number)
		ctx->weapon()->m_flPostponeFireReadyTime() = data->m_flPostponeFireReadyTime;
}

void Prediction::end()
{
	if (!exploits->charging)
	{
		movement->FinishTrackPredictionErrors(ctx->local().get());
		ctx->local()->set_current_command(nullptr);

		*(int*)prediction_random_seed.get() = -1;
		*(Player**)prediction_player.get() = nullptr; //-V114

		prediction->m_bInPrediction = in_prediction;

		movement->Reset();
		movehelper->set_host(nullptr);
	}

	ctx->local()->m_flVelocityModifier() = velocity_modifier;
	ctx->local()->m_vecLastPredictedPosition() = origin;

	globals->curtime = curtime;
	globals->frametime = frametime;
}

void Prediction::store_netvars(int command_number)
{
	auto data = crypt_ptr <NetvarsData>(&netvars_data[command_number % MULTIPLAYER_BACKUP]);

	data->m_command_number = command_number;
	data->m_vecViewOffset = clamp(ctx->local()->m_vecViewOffset().z, 46.0f, 64.0f);
	data->m_viewPunchAngle = ctx->local()->m_viewPunchAngle().x;
	data->m_aimPunchAngle = ctx->local()->m_aimPunchAngle();
	data->m_aimPunchAngleVel = ctx->local()->m_aimPunchAngleVel();
	data->m_vecOrigin = ctx->local()->m_vecOrigin();
	data->m_nTickBase = ctx->local()->m_nTickBase();

	if (config->rage.enable && ctx->weapon())
	{
		if (ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
			data->m_flPostponeFireReadyTime = ctx->weapon()->m_flPostponeFireReadyTime();
		else
			data->m_flPostponeFireReadyTime = FLT_MAX;
	}
	else
		data->m_flPostponeFireReadyTime = FLT_MAX;
}

void Prediction::restore_netvars(int command_number)
{
	if ((ctx->local()->m_fEffects() & 8) == 0
		&& ctx->local()->m_ubEFNoInterpParity() == ctx->local()->m_ubEFNoInterpParityOld()
		&& ctx->local()->m_vecOrigin().DistToSqr(ctx->local()->m_vecLastPredictedPosition()) < 4096.0f)
		ctx->local()->m_vecLastPredictedPosition() = ctx->local()->m_vecOrigin();

	auto data = crypt_ptr <NetvarsData> (&netvars_data[command_number % MULTIPLAYER_BACKUP]);

	if (!data)
		return;

	if (data->m_command_number != command_number)
		return;

	if (data->m_nTickBase != ctx->local()->m_nTickBase())
	{
		if (fabsf(globals->realtime - exploits->last_exploit_time) > 0.25f)
			return;
	}

	auto view_offset_delta = ctx->local()->m_vecViewOffset().z - data->m_vecViewOffset;
	auto view_punch_angle_delta = ctx->local()->m_viewPunchAngle().x - data->m_viewPunchAngle;
	auto aim_punch_angle_delta = ctx->local()->m_aimPunchAngle() - data->m_aimPunchAngle;
	auto aim_punch_angle_vel_delta = ctx->local()->m_aimPunchAngleVel() - data->m_aimPunchAngleVel;

	if (abs(view_offset_delta) <= 0.03125f)
		ctx->local()->m_vecViewOffset().z = data->m_vecViewOffset;

	if (abs(view_punch_angle_delta) <= 0.03125f)
		ctx->local()->m_viewPunchAngle().x = data->m_viewPunchAngle;

	if (abs(aim_punch_angle_delta.x) <= 0.03125f && abs(aim_punch_angle_delta.y) <= 0.03125f && abs(aim_punch_angle_delta.z) <= 0.03125f)
		ctx->local()->m_aimPunchAngle() = data->m_aimPunchAngle;

	if (abs(aim_punch_angle_vel_delta.x) <= 0.03125f && abs(aim_punch_angle_vel_delta.y) <= 0.03125f && abs(aim_punch_angle_vel_delta.z) <= 0.03125f)
		ctx->local()->m_aimPunchAngleVel() = data->m_aimPunchAngleVel;

	if (!config->rage.enable)
		return;

	if (!ctx->weapon())
		return;

	ctx->weapon()->m_flPostponeFireReadyTime() = data->m_flPostponeFireReadyTime;
}

void Prediction::detect_prediction_error(int command_number)
{
	auto data = crypt_ptr <NetvarsData>(&netvars_data[command_number % MULTIPLAYER_BACKUP]);

	if (!data)
		return;

	if (data->m_command_number != command_number)
		return;

	auto repredict = false;

	if (data->m_nTickBase != ctx->local()->m_nTickBase())
	{
		if (fabsf(globals->realtime - exploits->last_exploit_time) > 0.25f)
			repredict = true;
	}

	auto view_punch_angle_delta = abs(ctx->local()->m_viewPunchAngle().x - data->m_viewPunchAngle);

	if (view_punch_angle_delta <= 0.03125f) //-V674
		ctx->local()->m_viewPunchAngle().x = data->m_viewPunchAngle;

	if (abs(ctx->local()->m_aimPunchAngle().x - data->m_aimPunchAngle.x) > 0.03125f
		|| abs(ctx->local()->m_aimPunchAngle().y - data->m_aimPunchAngle.y) > 0.03125f
		|| abs(ctx->local()->m_aimPunchAngle().z - data->m_aimPunchAngle.z) > 0.03125f)
		ctx->local()->m_aimPunchAngle() = data->m_aimPunchAngle;
	else
	{
		data->m_aimPunchAngle = ctx->local()->m_aimPunchAngle();
		repredict = true;
	}

	if (abs(ctx->local()->m_aimPunchAngleVel().x - data->m_aimPunchAngleVel.x) > 0.03125f
		|| abs(ctx->local()->m_aimPunchAngleVel().y - data->m_aimPunchAngleVel.y) > 0.03125f
		|| abs(ctx->local()->m_aimPunchAngleVel().z - data->m_aimPunchAngleVel.z) > 0.03125f)
		ctx->local()->m_aimPunchAngleVel() = data->m_aimPunchAngleVel;
	else
	{
		data->m_aimPunchAngleVel = ctx->local()->m_aimPunchAngleVel();
		repredict = true;
	}

	auto view_offset_delta = abs(ctx->local()->m_vecViewOffset().z - data->m_vecViewOffset);

	if (view_offset_delta > 0.25f) //-V674
	{
		data->m_vecViewOffset = ctx->local()->m_vecViewOffset().z;
		repredict = true;
	}
	else
		ctx->local()->m_vecViewOffset().z = data->m_vecViewOffset;

	if ((ctx->local()->m_vecOrigin() - data->m_vecOrigin).LengthSqr() >= 1.0f)
	{
		data->m_vecOrigin = ctx->local()->m_vecOrigin();
		repredict = true;
	}

	if (!repredict)
		return;

	prediction->m_bPreviousAckHadErrors = true;
	prediction->m_nCommandsPredicted = 0;
	prediction->m_nPreviousStartFrame = -1;
}

void Prediction::store_viewmodel()
{
	if (!ctx->local())
		return;

	auto viewmodel = crypt_ptr <Viewmodel>(ctx->local()->m_hViewModel().Get());

	if (!viewmodel)
		return;

	viewmodel_data.weapon = viewmodel->m_hWeapon().Get();

	viewmodel_data.sequence = viewmodel->m_nSequence();
	viewmodel_data.animation_parity = viewmodel->m_nAnimationParity();

	viewmodel_data.animation_time = viewmodel->m_flAnimTime();
	viewmodel_data.cycle = viewmodel->m_flCycle();
}

void Prediction::restore_viewmodel()
{
	if (!ctx->local())
		return;

	auto viewmodel = crypt_ptr <Viewmodel>(ctx->local()->m_hViewModel().Get());

	if (!viewmodel)
		return;

	if (viewmodel_data.weapon == viewmodel->m_hWeapon().Get() && viewmodel_data.sequence == viewmodel->m_nSequence() && viewmodel_data.animation_parity == viewmodel->m_nAnimationParity())
	{
		viewmodel->m_flAnimTime() = viewmodel_data.animation_time;
		viewmodel->m_flCycle() = viewmodel_data.cycle;
	}
}

void Prediction::fix_velocity_modifier(bool post_prediction, int command_number)
{
	auto lost_commands = command_number - clientstate->m_nLastCommandAck;

	if (!post_prediction)
		--lost_commands;

	if (lost_commands < 0)
		return;

	auto current_velocity_modifier = ctx->last_velocity_modifier;

	if (lost_commands > 0)
		current_velocity_modifier = clamp((float)lost_commands * globals->intervalpertick * 0.4f + ctx->last_velocity_modifier, 0.0f, 1.0f);
	else if (lost_commands < 0 || ctx->last_velocity_modifier >= 1.0f)
		current_velocity_modifier = ctx->local()->m_flVelocityModifier();

	ctx->local()->m_flVelocityModifier() = current_velocity_modifier;
}

void Prediction::fix_interpolation(bool post_interpolation, bool viewmodel)
{
	if (post_interpolation)
	{
		ctx->local()->m_nFinalPredictedTick() = final_predicted_tick;
		globals->interpolation_amount = interpolation_amount;
	}
	else
	{
		final_predicted_tick = ctx->local()->m_nFinalPredictedTick();
		interpolation_amount = globals->interpolation_amount;

		if (viewmodel)
		{
			if (exploits->tickbase_cycle)
				ctx->local()->m_nFinalPredictedTick() = ctx->last_tickbase + exploits->tickbase_cycle;
			else
			{
				ctx->last_tickbase = ctx->local()->m_nTickBase();
				ctx->local()->m_nFinalPredictedTick() = ctx->local()->m_nTickBase(); //-V656
			}
		}
		else
			ctx->local()->m_nFinalPredictedTick() = ctx->local()->m_nTickBase();

		if (exploits->charging)
			globals->interpolation_amount = 0.0f;
	}
}

void Prediction::store_restore_data(RestoreData& restore_data) //-V688
{
	restore_data.valid = true;

	restore_data.m_hGroundEntity = ctx->local()->m_hGroundEntity();
	restore_data.m_nMoveType = ctx->local()->m_moveType();
	restore_data.m_nFlags = ctx->local()->m_fFlags();
	restore_data.m_nTickBase = ctx->local()->m_nTickBase();

	restore_data.m_flFallVelocity = ctx->local()->m_flFallVelocity();
	restore_data.m_flVelocityModifier = ctx->local()->m_flVelocityModifier();
	restore_data.m_flDuckAmount = ctx->local()->m_flDuckAmount();
	restore_data.m_flDuckSpeed = ctx->local()->m_flDuckSpeed();
	restore_data.m_surfaceFriction = ctx->local()->m_surfaceFriction();

	if (ctx->weapon())
	{
		restore_data.m_fAccuracyPenalty = ctx->weapon()->m_fAccuracyPenalty();
		restore_data.m_flRecoilIndex = ctx->weapon()->m_flRecoilIndex();
	}

	restore_data.m_aimPunchAngle = ctx->local()->m_aimPunchAngle();
	restore_data.m_aimPunchAngleVel = ctx->local()->m_aimPunchAngleVel();
	restore_data.m_viewPunchAngle = ctx->local()->m_viewPunchAngle();
	restore_data.m_vecViewOffset = ctx->local()->m_vecViewOffset();
	restore_data.m_vecBaseVelocity = ctx->local()->m_vecBaseVelocity();
	restore_data.m_vecVelocity = ctx->local()->m_vecVelocity();
	restore_data.m_vecAbsVelocity = ctx->local()->m_vecAbsVelocity();
	restore_data.m_vecOrigin = ctx->local()->m_vecOrigin();

	memcpy(&restore_data.move_data, &move_data, sizeof(CMoveData));
}

void Prediction::apply_restore_data(RestoreData& restore_data) //-V688
{
	if (!restore_data.valid)
		return;

	ctx->local()->m_hGroundEntity() = restore_data.m_hGroundEntity;
	ctx->local()->m_moveType() = restore_data.m_nMoveType;
	ctx->local()->m_fFlags() = restore_data.m_nFlags;
	ctx->local()->m_nTickBase() = restore_data.m_nTickBase;

	ctx->local()->m_flFallVelocity() = restore_data.m_flFallVelocity;
	ctx->local()->m_flVelocityModifier() = restore_data.m_flVelocityModifier;
	ctx->local()->m_flDuckAmount() = restore_data.m_flDuckAmount;
	ctx->local()->m_flDuckSpeed() = restore_data.m_flDuckSpeed;
	ctx->local()->m_surfaceFriction() = restore_data.m_surfaceFriction;

	if (ctx->weapon())
	{
		ctx->weapon()->m_fAccuracyPenalty() = restore_data.m_fAccuracyPenalty;
		ctx->weapon()->m_flRecoilIndex() = restore_data.m_flRecoilIndex;
	}

	ctx->local()->m_aimPunchAngle() = restore_data.m_aimPunchAngle;
	ctx->local()->m_aimPunchAngleVel() = restore_data.m_aimPunchAngleVel;
	ctx->local()->m_viewPunchAngle() = restore_data.m_viewPunchAngle;
	ctx->local()->m_vecViewOffset() = restore_data.m_vecViewOffset;
	ctx->local()->m_vecBaseVelocity() = restore_data.m_vecBaseVelocity;
	ctx->local()->m_vecVelocity() = restore_data.m_vecVelocity;
	ctx->local()->m_vecAbsVelocity() = restore_data.m_vecAbsVelocity;
	ctx->local()->m_vecOrigin() = restore_data.m_vecOrigin;

	memcpy(&move_data, &restore_data.move_data, sizeof(CMoveData));
	restore_data.reset();
}