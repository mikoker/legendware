// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "third_person.h"

void ThirdPerson::run()
{
	if (config->misc.third_person_key.state && ctx->local()->valid())
	{
		Vector angles;
		engine->GetViewAngles(angles);

		Vector trace_angles;
		engine->GetViewAngles(trace_angles);

		trace_angles.z = (float)config->misc.third_person_distance;

		Vector forward;
		Vector right;
		Vector up;

		math::angle_vectors(trace_angles, &forward, &right, &up);

		Ray_t ray;
		CTraceFilter filter;
		CGameTrace trace;

		auto eye_position = ctx->visual_fake_ducking ? ctx->local()->GetAbsOrigin() + movement->GetPlayerViewOffset(false) : ctx->local()->GetAbsOrigin() + ctx->local()->m_vecViewOffset();
		auto camera_position = eye_position + forward * -(float)config->misc.third_person_distance + right + up;

		ray.Init(eye_position, camera_position, Vector(-16.0f, -16.0f, -16.0f), Vector(16.0f, 16.0f, 4.0f));
		filter.ccIgnore = crypt_str("CCSPlayer");

		enginetrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

		auto fraction = trace.fraction;

		if (fraction < 1.0f)
		{
			auto current_eye_pos = eye_position + right * (float)config->misc.third_person_distance * 0.5f;
			current_eye_pos.z = eye_position.z;

			ray.Init(current_eye_pos, camera_position, Vector(-16.0f, -16.0f, -16.0f), Vector(16.0f, 16.0f, 4.0f));
			enginetrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

			if (trace.fraction >= 1.0f && !trace.startsolid)
			{
				ray.Init(camera_position, camera_position, Vector(-16.0f, -16.0f, -16.0f), Vector(16.0f, 16.0f, 4.0f));
				enginetrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

				if (!trace.startsolid)
					fraction = 1.0f;
			}
			else
			{
				current_eye_pos = eye_position - right * (float)config->misc.third_person_distance * 0.5f;
				current_eye_pos.z = eye_position.z;

				ray.Init(current_eye_pos, camera_position, Vector(-16.0f, -16.0f, -16.0f), Vector(16.0f, 16.0f, 4.0f));
				enginetrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

				if (trace.fraction >= 1.0f && !trace.startsolid)
				{
					ray.Init(camera_position, camera_position, Vector(-16.0f, -16.0f, -16.0f), Vector(16.0f, 16.0f, 4.0f));
					enginetrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

					if (!trace.startsolid)
						fraction = 1.0f;
				}
			}
		}

		if (current_fraction > fraction)
			current_fraction = fraction;
		else if (current_fraction > 0.9999f)
			current_fraction = 1.0f;

		current_fraction = math::interpolate(current_fraction, fraction, globals->frametime * 8.0f);
		angles.z = (float)config->misc.third_person_distance * current_fraction;

		input->m_fCameraInThirdPerson = true;
		input->m_vecCameraOffset = angles;
	}
	else
	{
		current_fraction = 0.0f;

		if (input->m_fCameraInThirdPerson)
			input->m_fCameraInThirdPerson = false;
	}

	if (ctx->local()->valid())
	{
		require_reset = false;
		return;
	}

	if (config->misc.third_person_key.valid())
	{
		if (require_reset)
			ctx->local()->m_iObserverMode() = OBS_MODE_CHASE;

		if (ctx->local()->m_iObserverMode() == OBS_MODE_IN_EYE)
			require_reset = true;
	}
}