// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "esp.h"

#include "../globals.h"
#include "../Gui/core/core.h"
#include "../Gui/devkit/utils/utils.h"
#include "dormant.h"
#include <math.h>
#include "../ImGui Render/ImGUI_Renderer.h"
#include <codecvt>
#include <string>
#include "features.h"
#include "aim.h"
#include "dormant.h"
#include "animations.h"
#include "exploits.h"


constexpr float SPEED_FREQ = 255 / 2.0f;
void Esp::reset_animation(int m_i)
{
	prev_player_hp[m_i] = 0.0f;
	alpha_hp[m_i] = 0.0f;
	alpha_name[m_i] = 0.0f;
	alpha_box[m_i] = 0.0f;
	old_ammo[m_i] = 0.0f;
	prev_ammo_hp[m_i] = 0.0f;
	esp_alpha_fade[m_i] = 0.0f;
	alpha_weapon[m_i] = 0.0f;
	alpha_skeleton[m_i] = 0.0f;
	alpha_armor[m_i] = 0.0f;
	alpha_ammo[m_i] = 0.0f;
	prev_armor_hp[m_i] = 0.0f;
	alpha_oof_alpha[m_i] = 0.0f;
	for (int flag = 0; flag < FLAGS_MAX; flag++)
	{
		alpha_flags[m_i][flag] = 0.0f;
	}
}
void rotate_triangle(std::array<Vector2D, 3>& points, float rotation)
{
	const auto pointsCenter = (points.at(0) + points.at(1) + points.at(2)) / 3;
	for (auto& point : points)
	{
		point -= pointsCenter;

		const auto tempX = point.x;
		const auto tempY = point.y;

		const auto theta = DirectX::XMConvertToRadians(rotation);
		const auto c = cos(theta);
		const auto s = sin(theta);

		point.x = tempX * c - tempY * s;
		point.y = tempX * s + tempY * c;

		point += pointsCenter;
	}
}

bool Esp::get_bbox(crypt_ptr <Player> ent, Box& box) {
	if (ent->GetClientClass()->m_ClassID != CCSPlayer)
	{
		Vector /*vOrigin, */min, max, flb, brt, blb, frt, frb, brb, blt, flt;
		//float left, top, right, bottom;

		auto* collideable = ent->GetCollideable();

		if (!collideable)
			return false;

		min = collideable->OBBMins();
		max = collideable->OBBMaxs();

		matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

		Vector points[] =
		{
			Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z)
		};

		Vector pointsTransformed[8];
		for (int i = 0; i < 8; i++) { //-V688
			math::vector_transform(points[i], trans, pointsTransformed[i]);
		}

		Vector pos = origin[ent->EntIndex()];

		if (!render->world_to_screen(pointsTransformed[3], flb) || !render->world_to_screen(pointsTransformed[5], brt)
			|| !render->world_to_screen(pointsTransformed[0], blb) || !render->world_to_screen(pointsTransformed[4], frt)
			|| !render->world_to_screen(pointsTransformed[2], frb) || !render->world_to_screen(pointsTransformed[1], brb)
			|| !render->world_to_screen(pointsTransformed[6], blt) || !render->world_to_screen(pointsTransformed[7], flt))
			return false;

		Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
		//+1 for each cuz of borders at the original box
		float left = flb.x;        // left
		float top = flb.y;        // top
		float right = flb.x;    // right
		float bottom = flb.y;    // bottom

		for (int i = 1; i < 8; i++) //-V688
		{
			if (left > arr[i].x)
				left = arr[i].x;
			if (bottom < arr[i].y)
				bottom = arr[i].y;
			if (right < arr[i].x)
				right = arr[i].x;
			if (top > arr[i].y)
				top = arr[i].y;
		}

		box.x = left;
		box.y = bottom;
		box.w = right;
		box.h = top;

		return true;
	}

	auto min = ent->GetCollideable()->OBBMins();
	auto max = ent->GetCollideable()->OBBMaxs();

	Vector dir, vF, vR, vU;

	engine->GetViewAngles(dir);
	dir.x = 0;
	dir.z = 0;

	math::angle_vectors(dir, &vF, &vR, &vU);

	auto zh = vU * max.z + vF * max.y + vR * min.x; // = Front left front
	auto e = vU * max.z + vF * max.y + vR * max.x; //  = Front right front
	auto d = vU * max.z + vF * min.y + vR * min.x; //  = Front left back
	auto c = vU * max.z + vF * min.y + vR * max.x; //  = Front right back

	auto g = vU * min.z + vF * max.y + vR * min.x; //  = Bottom left front
	auto f = vU * min.z + vF * max.y + vR * max.x; //  = Bottom right front
	auto a = vU * min.z + vF * min.y + vR * min.x; //  = Bottom left back
	auto b = vU * min.z + vF * min.y + vR * max.x; //  = Bottom right back

	Vector pointList[] = {
		a,
		b,
		c,
		d,
		e,
		f,
		g,
		zh,
	};

	Vector transformed[ARRAYSIZE(pointList)];

	for (int i = 0; i < ARRAYSIZE(pointList); i++) //-V688
	{
		pointList[i] += origin[ent->EntIndex()];

		if (!render->world_to_screen(pointList[i], transformed[i]))
			return false;
	}

	float left = FLT_MAX;
	float top = -FLT_MAX;
	float right = -FLT_MAX;
	float bottom = FLT_MAX;
	for (int i = 0; i < ARRAYSIZE(pointList); i++) { //-V688
		if (left > transformed[i].x)
			left = transformed[i].x;
		if (top < transformed[i].y)
			top = transformed[i].y;
		if (right < transformed[i].x)
			right = transformed[i].x;
		if (bottom > transformed[i].y)
			bottom = transformed[i].y;
	}

	box.x = left;
	box.y = bottom;
	box.w = right;
	box.h = top;

	return true;
}

void Esp::run()
{
	if (!ctx->local())
		return;

	static auto FindHudElement = reinterpret_cast <DWORD(__thiscall*)(void*, const char*)> (signatures_manager->signatures[SIGNATURE_FIND_HUD_ELEMENT]);
	static auto hud_ptr = *(DWORD**)(signatures_manager->signatures[SIGNATURE_HUD]);

	auto update_alpha = true;
	auto radar_base = FindHudElement(hud_ptr, crypt_str("CCSGO_HudRadar"));
	auto hud_radar = (CCSGO_HudRadar*)(radar_base - 0x14);

	dormant->start();

	for (auto i = 1; i <= globals->maxclients; ++i) //-V688
	{
		player = crypt_ptr <Player>((Player*)entitylist->GetClientEntity(i));

		if (!player)
			continue;

		auto valid_dormant = false;

		auto backup_flags = player->m_fFlags();
		auto backup_origin = player->GetAbsOrigin();

		if (player->IsDormant())
		{
			valid_dormant = dormant->adjust_sound(player.get());

			if (radar_base && hud_radar && player.get() != ctx->local().get() && (player->m_iTeamNum() != ctx->local()->m_iTeamNum() || ctx->friendly_fire) && player->m_bSpotted())
			{
				if (!valid_dormant && !hud_radar->radar_info[i].pos.IsZero())
				{
					valid_dormant = true;
					player->set_abs_origin(hud_radar->radar_info[i].pos);
				}
				//healths[i] = hud_radar->radar_info[i].health;
			}
			static PlayerResource** player_resource = *reinterpret_cast<PlayerResource***>(signatures_manager->signatures[SIGNATURE_PLAYER_RESOURCE]);
			auto pr = *player_resource;
			healths[i] = pr->m_iHealth()[i];
		}
		else
		{
			if (player->valid())
			{
				auto weapon = crypt_ptr <Weapon>(player->m_hActiveWeapon().Get());

				if (weapon)
					weapons[i] = weapon->m_iItemDefinitionIndex();

				healths[i] = player->m_iHealth();
				dormant->m_cSoundPlayers[i].reset(true, player->GetAbsOrigin(), player->m_fFlags());
			}
			else
				continue;
		}

		origin[i] = player->GetAbsOrigin();

		if (!healths[i])
		{
			if (player->IsDormant())
			{
				player->m_fFlags() = backup_flags;
				player->set_abs_origin(backup_origin);
			}

			reset_animation(i);
			continue;
		}

		if (!player->valid(false))
		{
			if (player->IsDormant())
			{
				player->m_fFlags() = backup_flags;
				player->set_abs_origin(backup_origin);
			}

			reset_animation(i);
			continue;
		}

		auto fast = 0.5f * globals->frametime; //-V807
		auto slow = 0.25f * globals->frametime;

		if (player->IsDormant())
		{

			if (!valid_dormant && esp_alpha_fade[i] > 0.0f)
				esp_alpha_fade[i] -= slow;
			else if (valid_dormant && esp_alpha_fade[i] < 1.0f)
				esp_alpha_fade[i] += fast;
			if (!valid_dormant)
			{
				player->m_fFlags() = backup_flags;
				player->set_abs_origin(backup_origin);

				esp_alpha_fade[i] = 0.0f;
				continue;
			}

		}
		else if (esp_alpha_fade[i] < 1.0f)
			esp_alpha_fade[i] += fast;

		if (i != ctx->local()->EntIndex()) {
			if (player->m_iTeamNum() != ctx->local()->m_iTeamNum() || ctx->friendly_fire)
				player_is = ENEMY;
			else if (player->m_iTeamNum() == ctx->local()->m_iTeamNum())
				player_is = TEAM;
		}
		else
			player_is = LOCAL;

		Vector screenPos;
		debugoverlay->ScreenPosition(player->get_shoot_position_abs(), screenPos);

		static int width, height;
		engine->GetScreenSize(width, height);

		auto invalid_screen = screenPos.x < 0 || screenPos.y < 0 || screenPos.x > width || screenPos.y > height;

		if (config->visuals.world.offscreen_enable && invalid_screen && player_is == ENEMY && ctx->local()->valid())
		{
			auto alpha_modifier = 1.0f;

			Vector viewAngles;
			engine->GetViewAngles(viewAngles);

			auto screenCenter = Vector2D(width * 0.5f, height * 0.5f);
			auto angleYawRad = DirectX::XMConvertToRadians(viewAngles.y - math::calculate_angle(ctx->shoot_position, player->get_shoot_position_abs()).y - 90.0f);

			auto radius = config->player_list.player_settings[i].hightlight ? config->player_list.player_settings[i].hightlight_distance : config->visuals.world.offscreen_distance;
			auto size = config->player_list.player_settings[i].hightlight ? config->player_list.player_settings[i].hightlight_size : config->visuals.world.offscreen_size;

			auto angle_cos = 0.0f, angle_sin = 0.0f;
			DirectX::XMScalarSinCosEst(&angle_sin, &angle_cos, angleYawRad);

			auto newPointX = screenCenter.x + ((((width - (size * 3)) * 0.5f) * (radius / 100.0f)) * angle_cos) + (int)(6.0f * (((float)size - 4.0f) / 16.0f));
			auto newPointY = screenCenter.y + ((((height - (size * 3)) * 0.5f) * (radius / 100.0f)) * angle_sin);

			std::array <Vector2D, 3> points
			{
				Vector2D(newPointX - size, newPointY - size),
				Vector2D(newPointX + size, newPointY),
				Vector2D(newPointX - size, newPointY + size)
			};

			rotate_triangle(points, viewAngles.y - math::calculate_angle(ctx->shoot_position, player->get_shoot_position_abs()).y - 90.0f);

			if ((!config->player_list.player_settings[i].hightlight && config->visuals.world.offscreen_fade) || (config->player_list.player_settings[i].hightlight && config->player_list.player_settings[i].hightlight_fade))
			{
				static auto switch_alpha = false;
				static auto alpha = 1.0f;

				if (update_alpha)
				{
					update_alpha = false;

					if (alpha >= 1.0f)
						switch_alpha = true;
					else if (alpha <= 0.0f)
						switch_alpha = false;

					if (switch_alpha)
						alpha -= globals->frametime;
					else
						alpha += globals->frametime;
				}

				alpha_modifier = clamp(alpha, 0.0f, 1.0f);
			}

			Color temp = player->IsDormant() ? Color(130, 130, 130) : 
				Color(config->player_list.player_settings[i].hightlight ? config->player_list.player_settings[i].hightlight_color : config->visuals.world.offscreen_color);
			temp.SetAlpha(std::clamp((int)(temp.a() * esp->esp_alpha_fade[i] * alpha_modifier), 0, 255));

			Color temp_out = player->IsDormant() ? Color(130, 130, 130) : 
				Color(config->player_list.player_settings[i].hightlight ? config->player_list.player_settings[i].hightlight_color_out : config->visuals.world.offscreen_color_out);
			temp_out.SetAlpha(std::clamp((int)(temp_out.a() * esp->esp_alpha_fade[i] * alpha_modifier), 0, 255));

			renderer_imgui.RenderTriangleFilled(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, temp);
			renderer_imgui.RenderTriangle(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, temp_out);
		}

		if (!get_bbox(player, boxes[i]))
		{
			if (player->IsDormant())
			{
				player->m_fFlags() = backup_flags;
				player->set_abs_origin(backup_origin);
			}

			continue;
		}

		this->i = i;

		if (player_is == ENEMY) {
			for (auto i = 0; i < FLAGS_MAX; ++i) //-V688
				flags_current[i] = config->visuals.enemy.esp_flags_arr[i];

			for (auto i = 0; i < 2; ++i) //-V688
				esp_weapon_arr_current[i] = config->visuals.enemy.esp_weapon_arr[i];

			if (!config->visuals.enemy.esp_enable)
			{
				if (player->IsDormant())
				{
					player->m_fFlags() = backup_flags;
					player->set_abs_origin(backup_origin);
				}

				continue;
			}
			if (config->visuals.enemy.esp_box)
				box();
			if (config->visuals.enemy.esp_name)
				name();
			if (config->visuals.enemy.esp_hp_bar)
				health();
			if (config->visuals.enemy.esp_flags)
				flags();
			if (config->visuals.enemy.esp_weapon)
				weapon();
			if (config->visuals.enemy.esp_skeleton)
				draw_skeleton();
			if (config->visuals.enemy.esp_ammo)
				ammo();

#ifdef BETA
			if (!animations->animation_data[i].empty() && config->visuals.enemy.multipoints)
			{
				auto& last_data = animations->animation_data[i].front();

				for (auto i = (int)HITBOX_HEAD; i < HITBOX_MAX; ++i)
				{
					std::vector <Point> points;
					aim->get_points(points, i, player, &last_data);

					for (auto& point : points)
					{
						Vector screen;

						if (!render->world_to_screen(point.point, screen))
							continue;

						renderer_imgui.RenderBoxFilled(screen.x - 1.0f, screen.y - 1.0f, screen.x + 1.0f, screen.y + 1.0f, i == HITBOX_HEAD ? Color::Red : Color::Green);
					}
				}
			}
#endif
		}
		else if (player_is == LOCAL)
		{

			if (!input->m_fCameraInThirdPerson)
				continue;
			for (auto i = 0; i < FLAGS_MAX; ++i) //-V688
				flags_current[i] = config->visuals.local.esp_flags_arr[i];

			for (auto i = 0; i < 2; ++i) //-V688
				esp_weapon_arr_current[i] = config->visuals.local.esp_weapon_arr[i];
			if (!config->visuals.local.esp_enable)
				continue;
			if (config->visuals.local.esp_box)
				box();
			if (config->visuals.local.esp_name)
				name();
			if (config->visuals.local.esp_hp_bar)
				health();
			if (config->visuals.local.esp_flags)
				flags();
			if (config->visuals.local.esp_weapon)
				weapon();
			if (config->visuals.local.esp_skeleton)
				draw_skeleton();
			if (config->visuals.local.esp_ammo)
				ammo();
		}
		else if (player_is == TEAM)
		{
			for (auto i = 0; i < FLAGS_MAX; ++i) //-V688
				flags_current[i] = config->visuals.team.esp_flags_arr[i];

			for (auto i = 0; i < 2; ++i) //-V688
				esp_weapon_arr_current[i] = config->visuals.team.esp_weapon_arr[i];
			if (!config->visuals.team.esp_enable)
				continue;
			if (config->visuals.team.esp_box)
				box();
			if (config->visuals.team.esp_name)
				name();
			if (config->visuals.team.esp_hp_bar)
				health();
			if (config->visuals.team.esp_flags)
				flags();
			if (config->visuals.team.esp_weapon)
				weapon();
			if (config->visuals.team.esp_skeleton)
				draw_skeleton();
			if (config->visuals.team.esp_ammo)
				ammo();
		}


		if (player->IsDormant())
		{
			player->m_fFlags() = backup_flags;
			player->set_abs_origin(backup_origin);
		}
	}
}

void Esp::ammo() {


	auto weapon = player->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (!weapon->can_fire())
		return;

	auto animLayer = player->get_animation_layer(1);


	auto activity = player->get_sequence_activity(animLayer->m_nSequence);
	int iClip = weapon->m_iClip1();
	int iClipMax = weapon->get_weapon_data()->max_clip1;

	float ammo = (float)iClip / (float)iClipMax;
	if (ammo > 1)
		ammo = 1;

	if (prev_ammo_hp[i] == ammo) //-V550
		prev_ammo_hp[i] = ammo;
	else if (prev_ammo_hp[i] > ammo) {
		prev_ammo_hp[i] -= 150.0f * globals->frametime;
		{
			if (prev_ammo_hp[i] < ammo)
			{
				prev_ammo_hp[i] = ammo;
			}
		}
	}
	else if (prev_ammo_hp[i] < ammo) {
		prev_ammo_hp[i] += 50.0f * globals->frametime * (1 - (prev_ammo_hp[i] / (ammo * 1.05f)));
		if (prev_ammo_hp[i] > 1.0f)
		{
			prev_ammo_hp[i] = 1.0f;
		}
	}

	if (alpha_ammo[i] < 127.0f)
	{
		alpha_ammo[i] += 500.0f * globals->frametime * (1 - (prev_ammo_hp[i] / (ammo * 1.05f)));
		if (alpha_ammo[i] > 127.0f)
			alpha_ammo[i] = 127.0f;
	}

	Color temp;



	if (player->IsDormant())
	{
		temp = Color(130, 130, 130);
	}
	else
	{
		if (player_is == ENEMY)
		{
			temp = Color(config->visuals.enemy.esp_ammo_clr);
		}
		else if (player_is == LOCAL)
		{
			temp = Color(config->visuals.local.esp_ammo_clr);
		}
		else if (player_is == TEAM)
		{
			temp = Color(config->visuals.team.esp_ammo_clr);
		}
	}
	temp.SetAlpha(min(temp.a(), 255.0f * esp_alpha_fade[i]));
	int x = boxes[i].x;
	int y = boxes[i].y;
	float box_h = (float)fabs(boxes[i].h - boxes[i].y);
	float off = 8;

	int w = 4;
	int h = box_h;
	if (h) {
		float box_w = (float)fabsf(box_h / 2);
		float width;
		if (activity == 967 && animLayer->m_flWeight != 0.f) {
			float cycle = animLayer->m_flCycle; // 1 = finished 0 = just started
			width = ((((boxes[i].w - boxes[i].x) * cycle) / 1.f));
		}
		else
			width = (((boxes[i].w - boxes[i].x) * ammo));

		renderer_imgui.RenderBoxFilled(float(x), float(y + h + 3), float(boxes[i].x + width), float(y + h + 5), temp);
		renderer_imgui.RenderBox(float(x - 1), float(y + h + 2), float(boxes[i].w + 1), float(y + h + 6), Color(0, 0, 0, int(100 * (temp.a() / 255.0f))));
	}

}
void Esp::draw_skeleton()
{
	static Color clr;
	auto alpha = 255.0f * esp_alpha_fade[i];
	if (player->IsDormant())
		return;
	if (player_is == ENEMY)
	{
		clr = Color(config->visuals.enemy.esp_skeleton_clr);
	}
	else if (player_is == LOCAL) {
		clr = Color(config->visuals.local.esp_skeleton_clr);
	}
	else if (player_is == TEAM)
	{
		clr = Color(config->visuals.team.esp_skeleton_clr);
	}
	if (alpha_skeleton[i] < clr.a())
	{
		alpha_skeleton[i] += 300.0f * globals->frametime;
		if (alpha_skeleton[i] > clr.a())
			alpha_skeleton[i] = clr.a();
	}
	else if (alpha_skeleton[i] > clr.a())
	{
		alpha_skeleton[i] -= 300.0f * globals->frametime;
		if (alpha_skeleton[i] < clr.a())
			alpha_skeleton[i] = clr.a();
	}

	if (player->IsDormant())
		clr = Color(130, 130, 130);
	clr.SetAlpha((int)min(alpha_skeleton[i], alpha));


	auto model = player->GetModel();

	if (!model)
		return;

	auto studio_model = modelinfo->GetStudioModel(model);

	if (!studio_model)
		return;

	auto get_bone_position = [&](int bone) -> Vector
	{
		return Vector(player->m_CachedBoneData().Base()[bone][0][3], player->m_CachedBoneData().Base()[bone][1][3], player->m_CachedBoneData().Base()[bone][2][3]);
	};

	auto upper_direction = get_bone_position(7) - get_bone_position(6);
	auto breast_bone = get_bone_position(6) + upper_direction * 0.5f;

	for (auto k = 0; k < studio_model->numbones; k++)
	{
		auto bone = studio_model->pBone(k);

		if (!bone)
			continue;

		if (bone->parent == -1)
			continue;

		if (!(bone->flags & BONE_USED_BY_HITBOX))
			continue;

		auto child = get_bone_position(k);
		auto parent = get_bone_position(bone->parent);

		auto delta_child = child - breast_bone;
		auto delta_parent = parent - breast_bone;

		if (delta_parent.Length() < 9.0f && delta_child.Length() < 9.0f)
			parent = breast_bone;

		if (k == 5)
			child = breast_bone;

		if (fabs(delta_child.z) < 5.0f && delta_parent.Length() < 5.0f && delta_child.Length() < 5.0f || k == 6)
			continue;

		Vector schild;
		Vector sparent;

		if (render->world_to_screen(child, schild) && render->world_to_screen(parent, sparent))
			renderer_imgui.RenderLine(schild.x, schild.y, sparent.x, sparent.y, clr);
	}

}

#pragma optimize("", off)
const char* clean_item_name(const char* name) {
	if (name[0] == 'C')
		name++;

	auto start = strstr(name, "Weapon");
	if (start != nullptr)
		name = start + 6;

	return name;
}

void Esp::weapon()
{
	auto GetLocalizeName = [](Weapon* weapon) {
		static auto GetVerifyName = [](Weapon* weapon) -> std::string {

			std::string final_str_weapon;
			const char* name = nullptr;
			char buffer[256] = { '\0' };
			if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
				name = crypt_str("REVOLVER");
			else if (weapon->m_iItemDefinitionIndex() == WEAPON_USP_SILENCER)
				name = crypt_str("USP-S");
			else if (weapon->m_iItemDefinitionIndex() == WEAPON_M4A1_SILENCER)
				name = crypt_str("M4A1-S");
			else if (weapon->m_iItemDefinitionIndex() == WEAPON_HKP2000)
				name = crypt_str("P2000");
			else if (weapon->m_iItemDefinitionIndex() == WEAPON_MP5SD)
				name = crypt_str("MP5-SD");
			else if (weapon->m_iItemDefinitionIndex() == WEAPON_CZ75A)
				name = crypt_str("CZ75");
			else {
				name = clean_item_name(((Entity*)weapon)->GetClientClass()->m_pNetworkName); //-V1027
				std::transform(name, name + strlen(name), buffer, ::toupper);
				name = buffer;
			}

			final_str_weapon += name;
			return final_str_weapon;
		};


		static std::unordered_map<short, std::string> names;
		if (names.find(weapon->m_iItemDefinitionIndex()) != names.end())
			return names[weapon->m_iItemDefinitionIndex()];


		names[weapon->m_iItemDefinitionIndex()] = GetVerifyName(weapon);
		return names[weapon->m_iItemDefinitionIndex()];
	};



	static auto GetIcon = [&](Weapon* weapon) -> std::string {
		std::string final_str_weapon;
		static auto Get_UTF8 = [&](const std::wstring& str) -> std::string {
			static std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.to_bytes(str);
		};

		static wchar_t buf[2] = { 0 };
		if (weapons[i] == WEAPON_M4A1_SILENCER)
			buf[0] = 57360;
		else if (weapons[i] == WEAPON_M4A1)
			buf[0] = 57358;
		else if (weapons[i] == WEAPON_P250)
			buf[0] = 57376;
		else if (weapons[i] == WEAPON_P90)
			buf[0] = 57380;
		else if (weapons[i] == WEAPON_HKP2000)
			buf[0] = 57363;
		else if (weapons[i] == WEAPON_M249)
			buf[0] = 57404;
		else
			buf[0] = 57344 + weapons[i];

		return Get_UTF8(buf);
	};
	auto weapon = player->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto weapon_data = weapon->get_weapon_data();

	if (!weapon_data)
		return;

	Color clr_temp;
	auto alpha = 255.0f * esp_alpha_fade[i];

	Color clr;
	if (player_is == ENEMY)
	{
		clr = weapon_data->weapon_type == WEAPONTYPE_KNIFE_TASER ? Color(config->visuals.enemy.esp_knife_taser_clr) : Color(config->visuals.enemy.esp_weapon_clr);
	}
	else if (player_is == LOCAL) {
		clr = weapon_data->weapon_type == WEAPONTYPE_KNIFE_TASER ? Color(config->visuals.local.esp_knife_taser_clr) : Color(config->visuals.local.esp_weapon_clr);
	}
	else if (player_is == TEAM)
	{
		clr = weapon_data->weapon_type == WEAPONTYPE_KNIFE_TASER ? Color(config->visuals.team.esp_knife_taser_clr) : Color(config->visuals.team.esp_weapon_clr);
	}

	if (esp_weapon_arr_current[0] || esp_weapon_arr_current[1]) {
		if (alpha_weapon[i] < clr.a())
		{
			alpha_weapon[i] += 300.0f * globals->frametime;
			if (alpha_weapon[i] > clr.a())
				alpha_weapon[i] = clr.a();
		}
		else if (alpha_weapon[i] > clr.a())
		{
			alpha_weapon[i] -= 300.0f * globals->frametime;
			if (alpha_weapon[i] < clr.a())
				alpha_weapon[i] = clr.a();
		}
	}
	if (!player->IsDormant())
		clr_temp = clr;
	else
		clr_temp = Color(130, 130, 130);
	clr_temp.SetAlpha((int)min(alpha_weapon[i], alpha));

	auto scale = u::dpi(14.0f);
	float offsets = 0;

	if (esp_weapon_arr_current[0])
	{
		offsets += u::dpi(31.0f) / 2;

		renderer_imgui.RenderText(GetIcon(weapon), ImVec2((boxes[i].x + (boxes[i].w - boxes[i].x) / 2), (boxes[i].h) + scale / 2), u::dpi(11.25f), clr_temp, true, core::f::icons_weapon);
	}
	if (esp_weapon_arr_current[1])
	{
		renderer_imgui.RenderText(GetLocalizeName(weapon), ImVec2((boxes[i].x + (boxes[i].w - boxes[i].x) / 2), (boxes[i].h) + scale / 2 + offsets), u::dpi(9.75f), clr_temp, true);

	}

}
#pragma optimize("", on)

void Esp::box()
{

	Color clr_temp;
	auto alpha = 255.0f * esp_alpha_fade[i];

	Color clr;
	if (player_is == ENEMY)
	{
		clr = Color(config->visuals.enemy.esp_box_clr);
	}
	else if (player_is == LOCAL) {
		clr = Color(config->visuals.local.esp_box_clr);
	}
	else if (player_is == TEAM)
	{
		clr = Color(config->visuals.team.esp_box_clr);
	}

	if (alpha_box[i] < clr.a())
	{
		alpha_box[i] += 300.0f * globals->frametime;
		if (alpha_box[i] > clr.a())
			alpha_box[i] = clr.a();
	}
	else if (alpha_box[i] > clr.a())
	{
		alpha_box[i] -= 300.0f * globals->frametime;
		if (alpha_box[i] < clr.a())
			alpha_box[i] = clr.a();
	}

	if (!player->IsDormant()) {
		clr_temp = clr;
	}
	else
		clr_temp = Color(130, 130, 130);
	clr_temp.SetAlpha((int)min(alpha_box[i], alpha));

	renderer_imgui.RenderBox(boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h, clr_temp);
	renderer_imgui.RenderBox(boxes[i].x - 1, boxes[i].y - 1, boxes[i].w + 1, boxes[i].h + 1, Color(0, 0, 0, int(100 * (clr_temp.a() / 255.0f))));
	renderer_imgui.RenderBox(boxes[i].x + 1, boxes[i].y + 1, boxes[i].w - 1, boxes[i].h - 1, Color(0, 0, 0, int(100 * (clr_temp.a() / 255.0f))));

}

void Esp::name()
{

	{
		static player_info_t player_info;

		if (!engine->GetPlayerInfo(i, &player_info))
			return;
		auto scale = u::dpi(10.0f);

		Color clr_temp;
		auto alpha = 255.0f * esp_alpha_fade[i];

		Color clr;
		if (player_is == ENEMY)
		{
			clr = Color(config->visuals.enemy.esp_name_clr);
		}
		else if (player_is == LOCAL)
		{
			clr = Color(config->visuals.local.esp_name_clr);
		}
		else if (player_is == TEAM)
		{
			clr = Color(config->visuals.team.esp_name_clr);
		}
		if (alpha_name[i] < clr.a())
		{
			alpha_name[i] += 300.0f * globals->frametime;
			if (alpha_name[i] > clr.a())
				alpha_name[i] = clr.a();
		}
		else if (alpha_name[i] > clr.a())
		{
			alpha_name[i] -= 300.0f * globals->frametime;
			if (alpha_name[i] < clr.a())
				alpha_name[i] = clr.a();
		}
		if (!player->IsDormant())
			clr_temp = clr;
		else
			clr_temp = Color(130, 130, 130);
		clr_temp.SetAlpha((int)min(alpha_name[i], alpha));
		renderer_imgui.RenderText(player_info.szName, ImVec2((boxes[i].x + (boxes[i].w - boxes[i].x) / 2), (boxes[i].y - 14) - scale / 2), u::dpi(10.5f), clr_temp, true);

	}
}



void Esp::health()
{
	{


		int player_hp = clamp(healths[i], 0, 100);

		int player_hp_max = 100;

		if (player->m_lifeState() != LIFE_ALIVE)
		{
			prev_player_hp[i] = 0;
		}

		if (prev_player_hp[i] > player_hp) {
			prev_player_hp[i] -= 150.0f * globals->frametime;
			{
				if (prev_player_hp[i] < player_hp)
				{
					prev_player_hp[i] = player_hp;
				}
			}
		}
		else if (prev_player_hp[i] < player_hp) {
			prev_player_hp[i] += 200.0f * globals->frametime * (1 - (prev_player_hp[i] / (player_hp * 1.05f)));
			if (prev_player_hp[i] > player_hp_max)
			{
				prev_player_hp[i] = player_hp_max;
			}
		}

		if (alpha_hp[i] < 127.0f)
		{
			alpha_hp[i] += 300.0f * globals->frametime * (1 - (prev_player_hp[i] / (player_hp * 1.05f)));
			if (alpha_hp[i] > 127.0f)
				alpha_hp[i] = 127.0f;
		}
		float
			x = boxes[i].x - 7,
			y = boxes[i].y,
			height = fabsf(boxes[i].h - boxes[i].y) - (((fabsf(boxes[i].h - boxes[i].y) * prev_player_hp[i]) / player_hp_max));
		float w = 4, h = fabsf(boxes[i].h - boxes[i].y);


		if ((player_is == ENEMY && config->visuals.enemy.esp_hp_bar_custom_color) || (player_is == LOCAL && config->visuals.local.esp_hp_bar_custom_color) || (player_is == TEAM && config->visuals.team.esp_hp_bar_custom_color)) {
			Color temp;



			if (player->IsDormant())
			{
				temp = Color(130, 130, 130);
			}
			else
			{
				if (player_is == ENEMY)
				{
					temp = Color(config->visuals.enemy.esp_hp_bar_color);
				}
				else if (player_is == LOCAL) {
					temp = Color(config->visuals.local.esp_hp_bar_color);
				}
				else if (player_is == TEAM)
				{
					temp = Color(config->visuals.team.esp_hp_bar_color);
				}

			}
			temp.SetAlpha(min(temp.a(), 255.0f * esp_alpha_fade[i]));
			//renderer_imgui.RenderLine(boxes[i].x - 7, boxes[i].y + (int)height, boxes[i].x - 7, boxes[i].h + 1, temp,3);
			renderer_imgui.RenderBoxFilled(boxes[i].x - 7, boxes[i].y + (int)height, boxes[i].x - 4, boxes[i].h, temp);

		}
		else
		{
			Color  color = Color(static_cast<int> (float(130.0f - player_hp * 1.3f)), static_cast<int> (float(player_hp * 2.55f)), 0, 255);
			Color temp;
			if (player->IsDormant())
			{
				temp = Color(130, 130, 130);
			}
			else
			{
				temp = color;
			}
			temp.SetAlpha(min(color.a(), 255.0f * esp_alpha_fade[i]));
			renderer_imgui.RenderBoxFilled(boxes[i].x - 7, boxes[i].y + (int)height, boxes[i].x - 4, boxes[i].h, temp);
		}


		if (player_hp != 100) {
			renderer_imgui.RenderText(to_string(player_hp).c_str(), ImVec2(boxes[i].x - u::dpi(/*25*/27.0f) / 2, boxes[i].y + (int)height - 5), u::dpi(9.75f), Color(255, 255, 255, (int)min((alpha_hp[i] * (255.0f / 127.0f)), 255.0f * esp_alpha_fade[i])), true);
		}


		renderer_imgui.RenderBox(boxes[i].x - 7, boxes[i].y - 1, boxes[i].x - 4, boxes[i].h + 1, Color(0, 0, 0, (int)min((int)alpha_hp[i], 255.0f * esp_alpha_fade[i])));
	}

}

Color Esp::alpha_flags_set(int FLAG)
{
	Color clr;

	if (!player->IsDormant()) {
		Color clr_temp;
		if (player_is == ENEMY)
		{
			clr_temp = Color(config->visuals.enemy.esp_flags_combo_clr[FLAG]);
		}
		else if (player_is == LOCAL) {
			clr_temp = Color(config->visuals.local.esp_flags_combo_clr[FLAG]);
		}
		else if (player_is == TEAM)
		{
			clr_temp = Color(config->visuals.team.esp_flags_combo_clr[FLAG]);
		}
		clr = clr_temp;
		if (alpha_flags[i][FLAG] < clr_temp.a())
		{
			alpha_flags[i][FLAG] += 300.0f * globals->frametime;
			if (alpha_flags[i][FLAG] > clr_temp.a())
				alpha_flags[i][FLAG] = clr_temp.a();
		}
		else if (alpha_flags[i][FLAG] > clr_temp.a())
		{
			alpha_flags[i][FLAG] -= 300.0f * globals->frametime;
			if (alpha_flags[i][FLAG] < clr_temp.a())
				alpha_flags[i][FLAG] = clr_temp.a();
		}
	}
	else {
		clr = Color(130, 130, 130);
	}
	clr.SetAlpha((int)min(alpha_flags[i][FLAG], 255.0f * esp_alpha_fade[i]));
	return clr;
}

void Esp::flags()
{
	auto y = 0;

	static std::vector <Flags> flags; //-V827

	if (flags_current[FLAGS_HELMET]) {
		auto kevlar = player->m_ArmorValue() > 0;
		if (kevlar)
		{

			if (player->m_bHasHelmet()) {
				flags.emplace_back(Flags(crypt_str("HK"), alpha_flags_set(FLAGS_HELMET)));
			}
			else {
				flags.emplace_back(Flags(crypt_str("K"), alpha_flags_set(FLAGS_HELMET)));
			}
		}
		else
			alpha_flags[i][FLAGS_HELMET] = 0;
	}

	if (flags_current[FLAGS_MONEY]) {
		auto money = player->m_iAccount();

		flags.emplace_back(Flags(std::to_string(money) + crypt_str("$"), alpha_flags_set(FLAGS_MONEY)));
	}

	if (flags_current[FLAGS_DEFUSER])
	{
		if (player->m_bHasDefuser())
			flags.emplace_back(Flags(crypt_str("KIT"), alpha_flags_set(FLAGS_DEFUSER)));
		else
			alpha_flags[i][FLAGS_DEFUSER] = 0;
	}

	if (flags_current[FLAGS_BOMB])// TODO:
	{
		auto found_c4 = false;
		auto weapons = player->m_hMyWeapons(); //-V688

		for (size_t i = 0; i < 48; ++i) { //-V688
			auto weapon_handle = weapons[i];
			if (!weapon_handle.IsValid())
				break;
			auto weapon = (Weapon*)weapon_handle.Get();
			if (!weapon)
				continue;
			auto definition_index = weapon->m_iItemDefinitionIndex();
			if (definition_index == WEAPON_C4)
			{
				found_c4 = true;
				flags.emplace_back(Flags(crypt_str("BOMB"), alpha_flags_set(FLAGS_BOMB)));
			}
		}

		if (!found_c4)
		{
			if (player->m_hCarriedHostage().Get())
				flags.emplace_back(Flags(crypt_str("HOSTAGE"), alpha_flags_set(FLAGS_BOMB)));
			else
				alpha_flags[i][FLAGS_BOMB] = 0;
		}
	}

	if (flags_current[FLAGS_RELOAD])
	{
		auto animLayer = player->get_animation_layer(1);
		auto activity = player->get_sequence_activity(animLayer->m_nSequence);
		if (activity == 967 && animLayer->m_flWeight != 0.f)
			flags.emplace_back(Flags(crypt_str("RELOAD"), alpha_flags_set(FLAGS_RELOAD)));
		else
			alpha_flags[i][FLAGS_RELOAD] = 0;

	}
	if (flags_current[FLAGS_ZOOM])
	{
		if (player->m_bIsScoped())
			flags.emplace_back(Flags(crypt_str("ZOOM"), alpha_flags_set(FLAGS_ZOOM)));
		else
			alpha_flags[i][FLAGS_ZOOM] = 0;
	}
	if (flags_current[FLAGS_FLASH])
	{
		if (player->m_flFlashDuration() > 0.0f)
		{
			auto flash_value = (player->m_flFlashBangTime() - globals->curtime) / player->m_flFlashDuration();

			if (flash_value > 0.5f) //-V550
				flags.emplace_back(Flags(crypt_str("FLASH"), alpha_flags_set(FLAGS_FLASH)));
			else
				alpha_flags[i][FLAGS_FLASH] = 0;
		}
		else
			alpha_flags[i][FLAGS_FLASH] = 0;
	}
	if (flags_current[FLAGS_DUCK])
	{
		if (player.get() == ctx->local().get())
		{
			if (ctx->fake_ducking)
				flags.emplace_back(Flags(crypt_str("FD"), alpha_flags_set(FLAGS_DUCK)));
			else
				alpha_flags[i][FLAGS_DUCK] = 0;
		}
		else
		{
			auto animstate = player->get_animation_state();
			if (animstate.get())
			{
				auto fakeducking = [&]() -> bool
				{
					static auto stored_tick = 0;
					static int crouched_ticks[65];

					if (animstate->duck_amount) //-V550
					{
						if (animstate->duck_amount < 0.9f && animstate->duck_amount > 0.5f) //-V550
						{
							if (stored_tick != globals->tickcount)
							{
								crouched_ticks[i]++;
								stored_tick = globals->tickcount;
							}

							return crouched_ticks[i] > 16;
						}
						else
							crouched_ticks[i] = 0;
					}

					return false;
				};
				if (fakeducking() && player->m_fFlags() & FL_ONGROUND && !animstate->landing)
				{
					flags.emplace_back(Flags(crypt_str("FD"), alpha_flags_set(FLAGS_DUCK)));
				}
				else
					alpha_flags[i][FLAGS_DUCK] = 0;
			}
		}
	}

	if (flags_current[FLAGS_EXPLOIT])
	{
		if (player.get() == ctx->local().get())
		{
			if (exploits->target_tickbase_shift)
				flags.emplace_back(Flags(crypt_str("EXPLOIT"), alpha_flags_set(FLAGS_EXPLOIT)));
			else
				alpha_flags[i][FLAGS_EXPLOIT] = 0;
		}
		else if (!animations->animation_data[i].empty())
		{
			auto exploit = false;
			auto counter = 0;

			for (auto& data : animations->animation_data[i])
			{
				if (counter > 16)
					break;

				if (data.exploit)
				{
					exploit = true;
					break;
				}

				++counter;
			}

			if (exploit)
				flags.emplace_back(Flags(crypt_str("EXPLOIT"), alpha_flags_set(FLAGS_EXPLOIT)));
			else
				alpha_flags[i][FLAGS_EXPLOIT] = 0;
		}
	}

	if (flags_current[FLAGS_PLANT])
	{
		if (player->m_bDuckOverride())
		{
			auto weapon = crypt_ptr <Weapon>(player->m_hActiveWeapon().Get());

			if (weapon && weapon->m_iItemDefinitionIndex() == WEAPON_C4)
			{
				auto bomb = crypt_ptr <Bomb>((Bomb*)weapon.get());

				if (bomb->m_bStartedArming())
					flags.emplace_back(Flags(crypt_str("PLANT"), alpha_flags_set(FLAGS_PLANT)));
				else
					alpha_flags[i][FLAGS_PLANT] = 0;
			}
			else
				alpha_flags[i][FLAGS_PLANT] = 0;
		}
		else
			alpha_flags[i][FLAGS_PLANT] = 0;
	}

	if (flags_current[FLAGS_DEFUSE])
	{
		if (player->m_bIsDefusing())
			flags.emplace_back(Flags(crypt_str("DEFUSE"), alpha_flags_set(FLAGS_DEFUSE)));
		else
			alpha_flags[i][FLAGS_DEFUSE] = 0;
	}

	if (flags_current[FLAGS_IGNORE])
	{
		if (config->player_list.player_settings[i].ignore)
			flags.emplace_back(Flags(crypt_str("IGNORE"), alpha_flags_set(FLAGS_IGNORE)));
		else
			alpha_flags[i][FLAGS_IGNORE] = 0;
	}

	for (auto flag : flags)
	{
		renderer_imgui.RenderText(flag.m_text, ImVec2(boxes[i].w + 5, boxes[i].y + y), u::dpi(8.25f), flag.m_clr);
		y += u::dpi(10);
	}

	flags.clear();
}