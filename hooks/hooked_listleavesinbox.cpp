// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

int __fastcall hooked_listleavesinbox(void* ecx, void* edx, Vector& mins, Vector& maxs, unsigned short* list, int list_max)
{
	static auto original = hooks_manager->hooks[HOOK_BSPQUERY]->get_func_address <ListLeavesInBox> (crypt_hash_n(6));

	if ((uintptr_t)_ReturnAddress() != signatures_manager->signatures[SIGNATURE_LIST_LEAVES_IN_BOX])
		return original(ecx, mins, maxs, list, list_max);

	auto info = crypt_ptr <RenderableInfo> (*(RenderableInfo**)((uintptr_t)_AddressOfReturnAddress() + 0x14));

	if (!info)
		return original(ecx, mins, maxs, list, list_max);

	if (!info->renderable)
		return original(ecx, mins, maxs, list, list_max);

	auto entity = crypt_ptr <Entity> (info->renderable->GetIClientUnknown()->GetBaseEntity());

	if (!entity)
		return original(ecx, mins, maxs, list, list_max);

	if (!entity->is_player())
		return original(ecx, mins, maxs, list, list_max);

	info->flags &= ~0x100;
	info->flags2 |= 0xC0;

	auto min = Vector(-16384.0f, -16384.0f, -16384.0f);
	auto max = Vector(16384.0f, 16384.0f, 16384.0f);

	return original(ecx, min, max, list, list_max);
}