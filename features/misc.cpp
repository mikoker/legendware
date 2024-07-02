// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "misc.h"
#include "..\config\config.h"

void Misc::run(crypt_ptr <CUserCmd> cmd) //-V813
{
	clan_tag();
	reveal_ranks(cmd);
}

void Misc::clan_tag()
{
	auto apply_clan_tag = [](const char* clan_tag) -> void
	{
		reinterpret_cast <int(__fastcall*)(const char*, const char*)> (signatures_manager->signatures[SIGNATURE_APPLY_CLAN_TAG])(clan_tag, clan_tag);
	};

	if (!config->misc.clan_tag)
	{
		if (!removed_clan_tag)
		{
			removed_clan_tag = true;
			apply_clan_tag(crypt_str(""));
		}

		return;
	}

	auto net_channel_info = engine->GetNetChannelInfo();

	if (!net_channel_info)
		return;

	auto current_tick = globals->tickcount / (int)(0.2f / globals->intervalpertick);

	if (current_tick == clan_tag_tick)
		return;

	clan_tag_tick = current_tick;

	if (ctx->show_full_clantag)
	{
		apply_clan_tag(crypt_str("legendware"));
		removed_clan_tag = false;
		return;
	}

	switch (clan_tag_tick % 21)
	{
	case 0:
		apply_clan_tag(crypt_str("legendware")); //-V1037
		break;
	case 1:
		apply_clan_tag(crypt_str("legendwar")); //-V1037
		break;
	case 2:
		apply_clan_tag(crypt_str("legendwa")); //-V1037
		break;
	case 3:
		apply_clan_tag(crypt_str("legendw")); //-V1037
		break;
	case 4:
		apply_clan_tag(crypt_str("legend")); //-V1037
		break;
	case 5:
		apply_clan_tag(crypt_str("legen")); //-V1037
		break;
	case 6:
		apply_clan_tag(crypt_str("lege")); //-V1037
		break;
	case 7:
		apply_clan_tag(crypt_str("leg")); //-V1037
		break;
	case 8:
		apply_clan_tag(crypt_str("le")); //-V1037
		break;
	case 9:
		apply_clan_tag(crypt_str("l")); //-V1037
		break;
	case 10:
		apply_clan_tag(crypt_str(""));
		break;
	case 11:
		apply_clan_tag(crypt_str("l"));
		break;
	case 12:
		apply_clan_tag(crypt_str("le"));
		break;
	case 13:
		apply_clan_tag(crypt_str("leg"));
		break;
	case 14:
		apply_clan_tag(crypt_str("lege"));
		break;
	case 15:
		apply_clan_tag(crypt_str("legen"));
		break;
	case 16:
		apply_clan_tag(crypt_str("legend"));
		break;
	case 17:
		apply_clan_tag(crypt_str("legendw"));
		break;
	case 18:
		apply_clan_tag(crypt_str("legendwa"));
		break;
	case 19:
		apply_clan_tag(crypt_str("legendwar"));
		break;
	case 20:
		apply_clan_tag(crypt_str("legendware"));
		break;
	}

	removed_clan_tag = false;
}

void Misc::unlock_cvars()
{
	void* v0; // eax
	int v1; // esi
	void* v2; // eax
	void* v3; // edi
	int Flags; // eax

	v0 = cvar.get();
	v1 = (*(int(__thiscall**)(void*))(*(DWORD*)v0 + 180))(v0);
	(**(void(__thiscall***)(int))v1)(v1);
	while ((*(unsigned __int8(__thiscall**)(int))(*(DWORD*)v1 + 8))(v1))
	{
		v2 = (void*)(*(int(__thiscall**)(int))(*(DWORD*)v1 + 12))(v1);
		v3 = v2;
		if (v2)
		{
			Flags = *((DWORD*)v2 + 5);
			if ((Flags & 0x12) != 0)
				Flags = Flags & 0x7FFFFFED | 0x80000000;

			int v2; // eax

			v2 = Flags | *((DWORD*)v3 + 2) & 1;
			*((DWORD*)v3 + 5) = Flags;
			*((DWORD*)v3 + 2) = v2;

		}
		(*(void(__thiscall**)(int))(*(DWORD*)v1 + 4))(v1);
	}
}

void Misc::reveal_ranks(crypt_ptr <CUserCmd> cmd) //-V813
{
	if (!config->misc.rank_reveal)
		return;

	if (!ctx->reveal_ranks)
		return;

	client->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0, 0, nullptr);
	ctx->reveal_ranks = false;
}