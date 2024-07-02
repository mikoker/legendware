// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "features.h"
#include "visuals.h"
#include "aim.h"
#include "penetration.h"
#include "exploits.h"
#include "movement.h"
#include "..\config\config.h"
#include "..\ImGui Render\ImGUI_Renderer.h"

void Visuals::run()
{
	if (!ctx->local()->valid())
		return;

	damage_marker();
	automatic_peek();
	manual_anti_aim();
	scope_lines();
	crosshair();
	field_of_view_circle();
}

void Visuals::crosshair()
{
	if (!config->visuals.world.penetration_crosshair)
		return;

	auto color = Color::Red;

	if (ctx->weapon_data() && ctx->weapon()->can_fire())
	{
		Vector view_angles;
		engine->GetViewAngles(view_angles);

		Vector direction;
		math::angle_vectors(view_angles, &direction, nullptr, nullptr);

		CGameTrace trace;
		CTraceFilter filter;
		Ray_t ray;

		ray.Init(ctx->shoot_position, ctx->shoot_position + direction * ctx->weapon_data()->range);
		filter.pSkip = ctx->local().get();

		enginetrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

		if (trace.fraction < 1.0f) //-V550
		{
			auto penetration_info = penetration->run(ctx->shoot_position, ctx->shoot_position + direction * ctx->weapon_data()->range, nullptr, true);

			if (!penetration_info.visible)
				color = Color::Green;
		}
	}

	renderer_imgui.RenderBoxFilled(render.width / 2 - 1, render.height / 2 - 1, render.width / 2 + 2, render.height / 2 + 2, color);
	renderer_imgui.RenderBox(render.width / 2 - 2, render.height / 2 - 2, render.width / 2 + 3, render.height / 2 + 3, Color::Black);
}

void Visuals::damage_marker()
{
	if (!config->visuals.world.damage_marker)
		return;

	if (ctx->damage_marker.empty())
		return;

	while (globals->curtime - ctx->damage_marker.back().time > 2.0f)
	{
		ctx->damage_marker.pop_back();

		if (ctx->damage_marker.empty())
			return;
	}

	for (auto& damage_marker : ctx->damage_marker)
	{
		Vector screen;

		if (!render->world_to_screen(damage_marker.position, screen))
			continue;

		damage_marker.position.z += (2.0f - (globals->curtime - damage_marker.time)) * globals->frametime * 30.0f;
		damage_marker.color.SetAlpha((int)((2.0f - (globals->curtime - damage_marker.time)) * 127.5f));

		renderer_imgui.RenderText(to_string(damage_marker.damage), ImVec2(screen.x, screen.y), u::dpi(12.0f), damage_marker.color, true);
	}
}

void Visuals::automatic_peek()
{
	if (ctx->automatic_peek_position.IsZero())
		return;

	auto delta = ctx->automatic_peek_position - ctx->local()->m_vecOrigin();
	auto retreating = delta.Length2D() >= 10.0f;

	if (ctx->automatic_peek)
		ctx->automatic_peek_radius = clamp(ctx->automatic_peek_radius + globals->frametime * 50.0f, 0.0f, 15.0f);
	else if (ctx->automatic_peek_radius != 0.0f)
		ctx->automatic_peek_radius = clamp(ctx->automatic_peek_radius - globals->frametime * 50.0f, 0.0f, 15.0f);

	auto filled_color = retreating && movement_system->holding_automatic_peek ? Color::Green : Color(config->misc.automatic_peek_clr);
	auto color = retreating && movement_system->holding_automatic_peek ? Color::Green : Color(config->misc.automatic_peek_clr);

	filled_color.SetAlpha((int)(config->misc.automatic_peek_clr[3] * 127.5f));
	color.SetAlpha((int)(config->misc.automatic_peek_clr[3] * 255.0f));

	renderer_imgui.RenderCircle3DFilled(ctx->automatic_peek_position, 144, ctx->automatic_peek_radius, filled_color);
	renderer_imgui.RenderCircle3D(ctx->automatic_peek_position, 144, ctx->automatic_peek_radius, color);
	renderer_imgui.RenderCircle3DShadow(ctx->automatic_peek_position, 144, ctx->automatic_peek_radius, color, 50.0f);
}

void Visuals::manual_anti_aim()
{
	if (!config->anti_aim.enable)
		return;

	if (!config->visuals.world.manual_anti_aim)
		return;

	if (config->anti_aim.manual_forward_key.state)
		renderer_imgui.RenderTriangleFilled((float)render.width / 2.0f, (float)render.height / 2.0f - 80.0f, (float)render.width / 2.0f - 10.0f, (float)render.height / 2.0f - 60.0f, (float)render.width / 2.0f + 10.0f, (float)render.height / 2.0f - 60.0f, Color(config->visuals.world.manual_anti_aim_clr[0], config->visuals.world.manual_anti_aim_clr[1], config->visuals.world.manual_anti_aim_clr[2], config->visuals.world.manual_anti_aim_clr[3]));

	if (config->anti_aim.manual_back_key.state)
		renderer_imgui.RenderTriangleFilled((float)render.width / 2.0f, (float)render.height / 2.0f + 80.0f, (float)render.width / 2.0f - 10.0f, (float)render.height / 2.0f + 60.0f, (float)render.width / 2.0f + 10.0f, (float)render.height / 2.0f + 60.0f, Color(config->visuals.world.manual_anti_aim_clr[0], config->visuals.world.manual_anti_aim_clr[1], config->visuals.world.manual_anti_aim_clr[2], config->visuals.world.manual_anti_aim_clr[3]));

	if (config->anti_aim.manual_left_key.state)
		renderer_imgui.RenderTriangleFilled((float)render.width / 2.0f - 55.0f, (float)render.height / 2.0f + 10.0f, (float)render.width / 2.0f - 75.0f, (float)render.height / 2.0f, (float)render.width / 2.0f - 55.0f, (float)render.height / 2.0f - 10.0f, Color(config->visuals.world.manual_anti_aim_clr[0], config->visuals.world.manual_anti_aim_clr[1], config->visuals.world.manual_anti_aim_clr[2], config->visuals.world.manual_anti_aim_clr[3]));

	if (config->anti_aim.manual_right_key.state)
		renderer_imgui.RenderTriangleFilled((float)render.width / 2.0f + 55.0f, (float)render.height / 2.0f - 10.0f, (float)render.width / 2.0f + 75.0f, (float)render.height / 2.0f, (float)render.width / 2.0f + 55.0f, (float)render.height / 2.0f + 10.0f, Color(config->visuals.world.manual_anti_aim_clr[0], config->visuals.world.manual_anti_aim_clr[1], config->visuals.world.manual_anti_aim_clr[2], config->visuals.world.manual_anti_aim_clr[3]));
}

void Visuals::scope_lines()
{
	if (!config->visuals.world.removals[REMOVALS_SCOPE])
		return;

	if (!ctx->local()->m_bIsScoped())
		return;

	auto valid_weapon = true;

	if (ctx->weapon() && (ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_AUG || ctx->weapon()->m_iItemDefinitionIndex() == WEAPON_SG553))
		valid_weapon = false;

	if (!valid_weapon && !input->m_fCameraInThirdPerson)
		return;

	renderer_imgui.RenderLine(render.width / 2, 0, render.width / 2, render.height, Color::Black);
	renderer_imgui.RenderLine(0, render.height / 2, render.width, render.height / 2, Color::Black);
}

void Visuals::field_of_view_circle()
{
	if (!config->legit.enable)
		return;

	if (!core::s::is_opened)
		return;

	int width, height;
	engine->GetScreenSize(width, height);

	auto aim_field_of_view_radians = DirectX::XMConvertToRadians(config->legit.weapon[ctx->weapon_config].field_of_view * 0.8f);
	auto view_field_of_view_radians = DirectX::XMConvertToRadians(ctx->field_of_view);

	auto radius = tan(aim_field_of_view_radians * 0.5f) / tan(view_field_of_view_radians * 0.5f) * (float)width;

	if (radius <= 0.0f)
		return;

	renderer_imgui.RenderCircleFilled((float)width * 0.5f, (float)height * 0.5f, radius, 144, Color(255, 255, 255, 127));
}