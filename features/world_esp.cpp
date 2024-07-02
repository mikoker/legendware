// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "world_esp.h"
#include "../ImGui Render/ImGUI_Renderer.h"
#include <codecvt>
#include <string>
#include "../globals.h"
#include "esp.h"
#include "features.h"

#include "logs.h"
#include <stack>

void  WorldEsp::rotate_triangle(std::array<Vector2D, 3>& points, float rotation)
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
bool WorldEsp::getBox()
{
	if (m_e->GetClientClass()->m_ClassID != 40)
	{
		Vector /*vOrigin, */min, max, flb, brt, blb, frt, frb, brb, blt, flt;
		//float left, top, right, bottom;

		auto* collideable = m_e->GetCollideable();

		if (!collideable)
			return false;

		min = collideable->OBBMins();
		max = collideable->OBBMaxs();

		matrix3x4_t& trans = m_e->m_rgflCoordinateFrame();

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

		Vector pos = m_origin;

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

	auto min = m_e->GetCollideable()->OBBMins();
	auto max = m_e->GetCollideable()->OBBMaxs();

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
		pointList[i] += m_origin;

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

void WorldEsp::weaponEsp()
{
	if (m_e->m_hOwnerEntity().IsValid() ||
		m_origin == Vector(0, 0, 0))
		return;
	auto weapon = (Weapon*)m_e;
	if (config->visuals.world.weapon.box)
	{
		Color clr_temp = Color(config->visuals.world.weapon.box_clr);
		renderer_imgui.RenderBox(box.x, box.y, box.w, box.h, clr_temp);
		renderer_imgui.RenderBox(box.x - 1, box.y - 1, box.w + 1, box.h + 1, Color(0, 0, 0, int(100 * (clr_temp.a() / 255.0f))));
		renderer_imgui.RenderBox(box.x + 1, box.y + 1, box.w - 1, box.h - 1, Color(0, 0, 0, int(100 * (clr_temp.a() / 255.0f))));
	}

	int iClip = weapon->m_iClip1();
	int iClipMax = weapon->get_weapon_data()->max_clip1;
	if (config->visuals.world.weapon.ammo && !(client_class->m_ClassID == CBaseGrenade || client_class->m_ClassID == CC4 ||
		client_class->m_ClassID == CSensorGrenade || client_class->m_ClassID == CSmokeGrenade || client_class->m_ClassID == CDecoyGrenade ||
		client_class->m_ClassID == CHEGrenade || client_class->m_ClassID == CIncendiaryGrenade || client_class->m_ClassID == CMolotovGrenade ||
		client_class->m_ClassID == CFlashbang)) {

		if (config->visuals.world.weapon.ammo_type[0]) {
			int x = box.x;
			int y = box.y;
			float box_h = (float)fabs(box.h - box.y);
			float off = 8;

			int h = box_h;



			if (h) {
				float box_w = (float)fabsf(box_h / 2);

				float width = ((((box.w - box.x) * iClip) / iClipMax)); //-V636


				Color clr_temp = Color(config->visuals.world.weapon.ammo_clr);

				// Render Outlile
				renderer_imgui.RenderBox(x - 1, box.y + 2, box.w + 1, box.y + 6, Color(0, 0, 0, int(100 * (clr_temp.a() / 255.0f))));
				//Render Main Visible rect
				renderer_imgui.RenderBoxFilled(x, box.y + 3, (int)(box.x + width), box.y + 5, clr_temp);
			}
		}
	}

	auto scale = u::dpi(22.0f);

	std::string data;
	if (config->visuals.world.weapon.ammo && !(client_class->m_ClassID == CBaseGrenade || client_class->m_ClassID == CC4 ||
		client_class->m_ClassID == CSensorGrenade || client_class->m_ClassID == CSmokeGrenade || client_class->m_ClassID == CDecoyGrenade ||
		client_class->m_ClassID == CHEGrenade || client_class->m_ClassID == CIncendiaryGrenade || client_class->m_ClassID == CMolotovGrenade ||
		client_class->m_ClassID == CFlashbang)) {
		if (config->visuals.world.weapon.ammo_type[1]) {
			if (iClip != -1 && iClipMax != -1)
				data = crypt_str(" ") + to_string(iClip) + crypt_str("/") + to_string(iClipMax);
		}
	}
	Color clr_temp = Color(config->visuals.world.weapon.ammo_clr);
	Color clr_icon = Color(config->visuals.world.weapon.icon_clr);




	static auto GetIcon = [&](Weapon* weapon) -> std::string {
		std::string final_str_weapon;
		static auto Get_UTF8 = [&](const std::wstring& str) -> std::string {
			static std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.to_bytes(str);
		};

		static wchar_t buf[2] = { 0 };
		if (weapon->m_iItemDefinitionIndex() == WEAPON_M4A1_SILENCER)
			buf[0] = 57360;
		else if (weapon->m_iItemDefinitionIndex() == WEAPON_M4A1)
			buf[0] = 57358;
		else if (weapon->m_iItemDefinitionIndex() == WEAPON_P250)
			buf[0] = 57376;
		else if (weapon->m_iItemDefinitionIndex() == WEAPON_P90)
			buf[0] = 57380;
		else if (weapon->m_iItemDefinitionIndex() == WEAPON_HKP2000)
			buf[0] = 57363;
		else if (weapon->m_iItemDefinitionIndex() == WEAPON_M249)
			buf[0] = 57404;
		else
			buf[0] = 57344 + weapon->m_iItemDefinitionIndex();

		return Get_UTF8(buf);
	};


	if (config->visuals.world.weapon.ammo && !(client_class->m_ClassID == CBaseGrenade || client_class->m_ClassID == CC4 ||
		client_class->m_ClassID == CSensorGrenade || client_class->m_ClassID == CSmokeGrenade || client_class->m_ClassID == CDecoyGrenade ||
		client_class->m_ClassID == CHEGrenade || client_class->m_ClassID == CIncendiaryGrenade || client_class->m_ClassID == CMolotovGrenade ||
		client_class->m_ClassID == CFlashbang))
	{
		if (config->visuals.world.weapon.ammo_type[1]) {
			renderer_imgui.RenderText(data, ImVec2((box.x + (box.w - box.x) / 2), (box.y) + 9), u::dpi(10.5f), clr_temp, true);
			if (config->visuals.world.weapon.icon)
				renderer_imgui.RenderText(GetIcon(weapon), ImVec2((box.x + (box.w - box.x) / 2), (box.y) + 18 + u::dpi(10.5f) / 2), u::dpi(15), clr_icon, true, core::f::icons_weapon);
		}
		else
			if (config->visuals.world.weapon.icon)
				renderer_imgui.RenderText(GetIcon(weapon), ImVec2((box.x + (box.w - box.x) / 2), (box.y) + 9), u::dpi(11.25f), clr_icon, true, core::f::icons_weapon);
	}
	else
		if (config->visuals.world.weapon.icon)
			renderer_imgui.RenderText(GetIcon(weapon), ImVec2((box.x + (box.w - box.x) / 2), (box.y) + 9), u::dpi(11.25f), clr_icon, true, core::f::icons_weapon);




}

void WorldEsp::c4Esp()
{
	if (!ctx->bomb_timer)
		return;

	if (config->visuals.world.c4.box)
	{
		Color clr_temp = Color(config->visuals.world.c4.box_clr);

		renderer_imgui.RenderBox(box.x, box.y, box.w, box.h, clr_temp);
		renderer_imgui.RenderBox(box.x - 1, box.y - 1, box.w + 1, box.h + 1, Color(0, 0, 0, int(100 * (clr_temp.a() / 255.0f))));
		renderer_imgui.RenderBox(box.x + 1, box.y + 1, box.w - 1, box.h - 1, Color(0, 0, 0, int(100 * (clr_temp.a() / 255.0f))));
	}

	const int temp = 57344 + WEAPON_C4;

	static wchar_t buf[2] = { 0 };
	buf[0] = temp;
	auto Get_UTF8 = [&](const std::wstring& str) -> std::string {
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		return myconv.to_bytes(str);
	};

	if (config->visuals.world.c4.icon)
		renderer_imgui.RenderText(Get_UTF8(buf), ImVec2((box.x + (box.w - box.x) / 2), box.y + u::dpi(14.0f)), u::dpi(11.25f), Color(config->visuals.world.c4.icon_clr), true, core::f::icons_weapon, true,true);

	if (config->visuals.world.bomb_timer)
	{
		auto bomb = crypt_ptr <Bomb>((Bomb*)m_e);
		auto time = bomb->m_flC4Blow() - globals->curtime;

		if (time < 0.0f)
			return;

		auto procent_time = clamp(time / convars_manager->convars[CONVAR_MP_C4TIMER]->GetFloat(), 0.0f, 1.0f);

		renderer_imgui.RenderCircleFilled(box.x + (box.w - box.x) / 2, box.y, u::dpi(25), 144, Color(120, 120, 120, 120));
		renderer_imgui.RenderCircleProgress(Vector(box.x + (box.w - box.x) / 2, box.y, 0.0f), u::dpi(50), Color(1.0f - procent_time, procent_time, 0.0f), procent_time);
		renderer_imgui.RenderText(to_string((int)time), ImVec2((box.x + (box.w - box.x) / 2), box.y), u::dpi(11.25f), Color(1.0f - procent_time, procent_time, 0.0f), true,core::f::esp_font,true,true);
	}
}

void WorldEsp::grenadeWarning()
{
	
}

// A global point needed for  sorting points with reference
// to  the first point Used in compare function of qsort()
Vector p0;

// A utility function to find next to top in a stack
Vector nextToTop(stack<Vector>& S)
{
	Vector p = S.top();
	S.pop();
	Vector res = S.top();
	S.push(p);
	return res;
}

// A utility function to swap two points
void swap(Vector& p1, Vector& p2)
{
	Vector temp = p1;
	p1 = p2;
	p2 = temp;
}

// A utility function to return square of distance
// between p1 and p2
int distSq(const Vector& p1, const Vector& p2)
{
	return (p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y);
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(const Vector& p, const Vector& q, const Vector& r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear
	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

// A function used by library function qsort() to sort an array of
// points with respect to the first point
int compare(const void* vp1, const void* vp2)
{
	Vector* p1 = (Vector*)vp1;
	Vector* p2 = (Vector*)vp2;

	// Find orientation
	int o = orientation(p0, *p1, *p2);
	if (o == 0)
		return (distSq(p0, *p2) >= distSq(p0, *p1)) ? -1 : 1;

	return (o == 2) ? -1 : 1;
}

// Prints convex hull of a set of n points.
vector <ImVec2> convexHull(Vector points[], int n)
{
	vector <ImVec2> final_points;

	if (n <= 3)
	{
		for (auto i = 0; i < n; ++i)
			final_points.emplace_back(ImVec2(points[i].x, points[i].y));

		return final_points;
	}

	// Find the bottommost point
	int ymin = points[0].y, min = 0;
	for (int i = 1; i < n; i++)
	{
		int y = points[i].y;

		// Pick the bottom-most or chose the left
		// most point in case of tie
		if ((y < ymin) || (ymin == y &&
			points[i].x < points[min].x))
			ymin = points[i].y, min = i;
	}

	// Place the bottom-most point at first position
	swap(points[0], points[min]);

	// Sort n-1 points with respect to the first point.
	// A point p1 comes before p2 in sorted output if p2
	// has larger polar angle (in counterclockwise
	// direction) than p1
	p0 = points[0];
	qsort(&points[1], n - 1, sizeof(Vector), compare);

	// If two or more points make same angle with p0,
	// Remove all but the one that is farthest from p0
	// Remember that, in above sorting, our criteria was
	// to keep the farthest point at the end when more than
	// one points have same angle.
	int m = 1; // Initialize size of modified array
	for (int i = 1; i < n; i++)
	{
		// Keep removing i while angle of i and i+1 is same
		// with respect to p0
		while (i < n - 1 && orientation(p0, points[i],
			points[i + 1]) == 0)
			i++;


		points[m] = points[i];
		m++;  // Update size of modified array
	}

	// If modified array of points has less than 3 points,
	// convex hull is not possible
	if (m < 3) return final_points;

	// Create an empty stack and push first three points
	// to it.
	stack<Vector> S;
	S.push(points[0]);
	S.push(points[1]);
	S.push(points[2]);

	// Process remaining n-3 points
	for (int i = 3; i < m; i++)
	{
		// Keep removing top while the angle formed by
		// points next-to-top, top, and points[i] makes
		// a non-left turn
		while (S.size() > 1 && orientation(nextToTop(S), S.top(), points[i]) != 2)
			S.pop();
		S.push(points[i]);
	}

	// Now stack has the output points, return contents of stack
	while (!S.empty())
	{
		Vector p = S.top();
		final_points.emplace_back(ImVec2(p.x, p.y));
		S.pop();
	}

	return final_points;
}

bool WorldEsp::inferno()
{
	auto owner = crypt_ptr <Player>(m_e->m_hOwnerEntity().Get());

	if (owner->valid() && (owner->m_iTeamNum() == ctx->local()->m_iTeamNum() || ctx->friendly_fire) && owner.get() != ctx->local().get())
		return false;

	auto inferno = crypt_ptr <Inferno>((Inferno*)m_e);

	if (inferno->fireCount() < 3)
		return false;

	vector <Vector> points;

	for (auto i = 0; i < inferno->fireCount(); ++i)
		if (inferno->fireIsBurning()[i])
			points.emplace_back((float)inferno->fireXDelta()[i] + m_origin.x, (float)inferno->fireYDelta()[i] + m_origin.y, (float)inferno->fireZDelta()[i] + m_origin.z);

	vector <Vector> screen_points;

	for (auto& point : points)
	{
		Vector screen;

		if (!render->world_to_screen(point, screen))
			continue;

		screen_points.emplace_back(screen);
	}

	auto convex = convexHull(screen_points.data(), screen_points.size());
	Color color(config->visuals.world.grenade_esp.inferno_indicator_clr);
	if (convex.size() >= 3)
	{
		
		Color filled_color = color;
		filled_color.SetAlpha(filled_color.a() / 3);
		renderer_imgui.draw_list->AddConvexPolyFilled(convex.data(), convex.size(), renderer_imgui.GetU32(filled_color));
		renderer_imgui.draw_list->AddPolyline(convex.data(), convex.size(), renderer_imgui.GetU32(color), true, 3);
	}

	Vector screen;

	if (!render->world_to_screen(m_origin, screen))
		return false;

	auto procent_time = clamp((globals->curtime - TICKS_TO_TIME(inferno->m_nFireEffectTickBegin())) / convars_manager->convars[CONVAR_INFERNO_FLAME_LIFETIME]->GetFloat(), 0.0f, 1.0f);
	color.SetAlpha(255);
	renderer_imgui.RenderCircleFilled(screen.x, screen.y, u::dpi(25), 144, Color(120, 120, 120, 120));
	renderer_imgui.RenderCircleProgress(Vector(screen.x, screen.y, 0.0f), u::dpi(50), color, 1.0f - procent_time);

	const int temp = 57344 + WEAPON_MOLOTOV;

	static wchar_t buf[2] = { 0 };
	buf[0] = temp;
	auto Get_UTF8 = [&](const std::wstring& str) -> std::string {
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		return myconv.to_bytes(str);
	};

	renderer_imgui.RenderText(Get_UTF8(buf), ImVec2(screen.x, screen.y), u::dpi(16.5f), Color::White, true, core::f::icons_weapon, true,true);
	return true;
}

bool WorldEsp::smoke()
{
	Vector screen;

	if (!render->world_to_screen(m_origin, screen))
		return false;

	auto smoke = crypt_ptr <Smoke>((Smoke*)m_e);

	if (!smoke->m_nSmokeEffectTickBegin())
		return false;

	auto time = globals->curtime - TICKS_TO_TIME(smoke->m_nSmokeEffectTickBegin());

	if (time < 0.0f || time > 17.5f)
		return false;

	auto procent_time = clamp(time / 17.5f, 0.0f, 1.0f);

	Color color(config->visuals.world.grenade_esp.smoke_indicator_clr);
	Color filled_color = color;
	filled_color.SetAlpha(filled_color.a() / 3);

	renderer_imgui.RenderCircle3DFilled(m_origin, 144, 160.0f, filled_color);
	renderer_imgui.RenderCircle3D(m_origin, 144, 160.0f, color);

	color.SetAlpha(255);
	renderer_imgui.RenderCircleFilled(screen.x, screen.y, u::dpi(25), 144, Color(120, 120, 120, 120));
	renderer_imgui.RenderCircleProgress(Vector(screen.x, screen.y, 0.0f), u::dpi(50), color, 1.0f - procent_time);

	const int temp = 57344 + WEAPON_SMOKEGRENADE;
	
	static wchar_t buf[2] = { 0 };
	buf[0] = temp;
	auto Get_UTF8 = [&](const std::wstring& str) -> std::string {
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		return myconv.to_bytes(str);
	};

	renderer_imgui.RenderText(Get_UTF8(buf), ImVec2(screen.x, screen.y), u::dpi(16.5f), Color::White, true, core::f::icons_weapon, true,true);
	return true;
}

void WorldEsp::grenadeEsp()
{
	if (m_e->m_nExplodeEffectTickBegin())
		return;

	auto model = m_e->GetModel();
	if (!model)
		return;

	auto studio_model = modelinfo->GetStudioModel(model);


	if (!studio_model)
		return;

	auto name = (std::string)studio_model->szName;
	std::string grenade_name;
	wchar_t buf[2] = { 0 };
	if (name.find(crypt_str("flashbang")) != std::string::npos)
	{
		grenade_name = crypt_str("FLASHBANG");

		int temp = 57344 + WEAPON_FLASHBANG;
		buf[0] = temp;

	}
	else if (name.find(crypt_str("smokegrenade")) != std::string::npos)
	{
		grenade_name = crypt_str("SMOKE");
		int temp = 57344 + WEAPON_SMOKEGRENADE;
		buf[0] = temp;
	}
	else if (name.find(crypt_str("incendiarygrenade")) != std::string::npos)
	{
		grenade_name = crypt_str("INCENDIARY");
		int temp = 57344 + WEAPON_INCGRENADE;
		buf[0] = temp;
	}
	else if (name.find(crypt_str("molotov")) != std::string::npos)
	{
		grenade_name = crypt_str("MOLOTOV");
		int temp = 57344 + WEAPON_MOLOTOV;
		buf[0] = temp;
	}
	else if (name.find(crypt_str("fraggrenade")) != std::string::npos)
	{
		grenade_name = crypt_str("HE GRENADE");
		int temp = 57344 + WEAPON_HEGRENADE;
		buf[0] = temp;
	}
	else if (name.find(crypt_str("decoy")) != std::string::npos)
	{
		grenade_name = crypt_str("DECOY");
		int temp = 57344 + WEAPON_DECOY;
		buf[0] = temp;
	}
	else
	{
		grenade_name = crypt_str("UNKNOWN");
		int temp = 57344;
		buf[0] = temp;
	}
	if (config->visuals.world.grenade_esp.box)
	{
		Color clr_temp = Color(config->visuals.world.grenade_esp.box_clr);
		renderer_imgui.RenderBox(box.x, box.y, box.w, box.h, clr_temp);
		renderer_imgui.RenderBox(box.x - 1, box.y - 1, box.w + 1, box.h + 1, Color(0, 0, 0, int(100 * (clr_temp.a() / 255.0f))));
		renderer_imgui.RenderBox(box.x + 1, box.y + 1, box.w - 1, box.h - 1, Color(0, 0, 0, int(100 * (clr_temp.a() / 255.0f))));
	}
	if (config->visuals.world.grenade_esp.name)
	{
		Color clr = Color(config->visuals.world.grenade_esp.name_clr);
		float offset = 0;
		auto scale = u::dpi(10.0f);
		auto Get_UTF8 = [&](const std::wstring& str) -> std::string {
			static std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.to_bytes(str);
		};
		if (config->visuals.world.grenade_esp.name_types[0])
		{
			offset += u::dpi(32.0f) / 2;
			renderer_imgui.RenderText(Get_UTF8(buf), ImVec2(box.x + (box.w - box.x) / 2, box.h + scale / 2), u::dpi(12), clr, true, core::f::icons_weapon, true, true);
		}
		if (config->visuals.world.grenade_esp.name_types[1])
		{
			renderer_imgui.RenderText(grenade_name, ImVec2(box.x + (box.w - box.x) / 2, box.h + scale / 2 + offset), u::dpi(9.75f), clr, true,core::f::esp_font,true, true);
		}
	}
}


void WorldEsp::run()
{
	if (!ctx->local())
		return;

	grenadeWarning();

	for (int i = 1; i <= entitylist->GetHighestEntityIndex(); i++)  //-V807
	{
		m_e = static_cast<Entity*>(entitylist->GetClientEntity(i));

		if (!m_e)
			continue;

		if (m_e->IsDormant())
			continue;

		client_class = m_e->GetClientClass();

		if (!client_class)
			continue;

		auto skip_weapon_esp = false;

		if (client_class->m_ClassID == CInferno && config->visuals.world.grenade_esp.inferno_indicator) {
			m_origin = m_e->GetAbsOrigin();
			if (getBox())
				skip_weapon_esp = inferno();
		}
		else if (client_class->m_ClassID == CSmokeGrenadeProjectile && config->visuals.world.grenade_esp.smoke_indicator) {
			m_origin = m_e->GetAbsOrigin();
			if (getBox())
				skip_weapon_esp = smoke();
		}

		if (!skip_weapon_esp)
		{
			if (config->visuals.world.weapon.esp && (client_class->m_ClassID == CAK47 || client_class->m_ClassID == CDEagle || client_class->m_ClassID == CBaseGrenade || client_class->m_ClassID == CC4 ||
				client_class->m_ClassID == CSensorGrenade || client_class->m_ClassID == CSmokeGrenade || client_class->m_ClassID == CDecoyGrenade ||
				client_class->m_ClassID == CHEGrenade || client_class->m_ClassID == CIncendiaryGrenade || client_class->m_ClassID == CMolotovGrenade ||
				client_class->m_ClassID == CFlashbang || client_class->m_ClassID >= CWeaponAug && client_class->m_ClassID <= CWeaponZoneRepulsor)) //-V648
			{
				m_origin = m_e->GetAbsOrigin();
				if (getBox())
					weaponEsp();
			}
			else if (config->visuals.world.grenade_esp.enable && (client_class->m_ClassID == CSmokeGrenadeProjectile || client_class->m_ClassID == CMolotovProjectile || client_class->m_ClassID == CBaseCSGrenadeProjectile || client_class->m_ClassID == CDecoyProjectile))
			{
				m_origin = m_e->GetAbsOrigin();
				if (getBox())
					grenadeEsp();
			}
			else if (config->visuals.world.c4.esp && client_class->m_ClassID == CPlantedC4)
			{
				m_origin = m_e->GetAbsOrigin();
				if (getBox())
					c4Esp();
			}
		}

		client_class = nullptr;
		m_e = nullptr;
	}
}