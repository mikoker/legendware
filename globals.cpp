// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "globals.h"

crypt_ptr <Ctx> ctx;

crypt_ptr <Player> Ctx::local()
{
	return stored_local;
}

crypt_ptr <AnimationState> Ctx::animation_state()
{
	return stored_animation_state;
}

crypt_ptr <Weapon> Ctx::weapon()
{
	return stored_weapon;
}

crypt_ptr <WeaponData> Ctx::weapon_data()
{
	return stored_weapon_data;
}

void Ctx::store()
{
	stored_local = (Player*)entitylist->GetClientEntity(engine->GetLocalPlayer());

	if (!stored_local || !stored_local->valid(false))
	{
		stored_animation_state = nullptr;
		stored_weapon = nullptr;
		stored_weapon_data = nullptr;
		return;
	}

	stored_animation_state = stored_local->get_animation_state();

	auto local_weapon = crypt_ptr <Weapon>(stored_local->m_hActiveWeapon().Get());
	if (!local_weapon)
	{
		stored_weapon = nullptr;
		stored_weapon_data = nullptr;
		return;
	}

	if (local_weapon->m_hOwnerEntity().Get() != stored_local.get())
	{
		stored_weapon = nullptr;
		stored_weapon_data = nullptr;
		return;
	}

	stored_weapon = local_weapon;

	auto weapon_data = stored_weapon->get_weapon_data();
	if (!weapon_data)
	{
		stored_weapon_data = nullptr;
		return;
	}

	stored_weapon_data = weapon_data;
}