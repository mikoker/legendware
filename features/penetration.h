#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"

struct PenetrationInfo
{
	bool visible;

	int damage;
	int hitbox;
	int hitgroup;
	int penetration_count;

	PenetrationInfo()
	{
		visible = true;

		damage = -1;
		hitbox = -1;
		hitgroup = -1;
		penetration_count = 4;
	}

	PenetrationInfo(bool visible, int damage, int hitbox, int hitgroup, int penetration_count)
	{
		this->visible = visible;

		this->damage = damage;
		this->hitbox = hitbox;
		this->hitgroup = hitgroup;
		this->penetration_count = penetration_count;
	}
};

class Penetration
{
protected:
	virtual void scale_damage(crypt_ptr <Player> player, CGameTrace& enter_trace, float& current_damage);
	virtual bool trace_to_exit(CGameTrace& enter_trace, CGameTrace& exit_trace, Vector start_position, const Vector& direction);
	virtual bool fire_bullet(Vector& direction, bool& visible, float& current_damage, int& hitbox, int& hitgroup, int& penetration_count, crypt_ptr <Player> entity, const Vector& shoot_position, bool penetrate_walls);
	virtual bool handle_bullet_penetration(CGameTrace& enter_trace, Vector& shoot_position, const Vector& direction, int& penetration_count, float& current_damage);
	virtual void clip_trace_to_player(crypt_ptr <Player> player, const Vector& start, const Vector& end, unsigned int mask, crypt_ptr <CTraceFilter> filter, crypt_ptr <CGameTrace> trace);
public:
	virtual bool is_breakable_entity(crypt_ptr <IClientEntity> entity);
	virtual PenetrationInfo run(const Vector& shoot_position, const Vector& point, crypt_ptr <Player> player, bool penetrate_walls);
};