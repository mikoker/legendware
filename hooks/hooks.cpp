// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\menu\menu.h"

#include "..\features\features.h"
#include "..\features\exploits.h"
#include "..\features\dormant.h"

crypt_ptr <Hooks> hooks_manager;
crypt_ptr <Events> events_manager;

DWORD original_setupbones;
DWORD original_standardblendingrules;
DWORD original_updateclientsideanimation;
DWORD original_processinterpolatedlist;
DWORD original_performscreenoverlay;
DWORD original_calcviewmodelbob;
DWORD original_rendermodels;
DWORD original_setupaliveloop;
DWORD original_viewmodelinterpolate;
DWORD original_clmove;
DWORD original_clreadpackets;
DWORD original_entityinterpolate;
DWORD original_spawnsmokeeffect;
DWORD original_rendersmokeoverlay;
DWORD original_shouldcollide;
DWORD original_get_eye_angles;
DWORD original_modifybones;
DWORD original_set_collision_bounds;
DWORD original_layertoindex;
DWORD original_doextrabonesprocessing;
DWORD original_onrenderstart;
DWORD original_channel_enable;
DWORD original_get_halloween_mask_model_addon;
DWORD original_update_generated_material;
DWORD original_test;


bool __fastcall hooked_channel_enable(void* ecx, void* edx, int channelID, int severity)
{
	return true;
}



void hooks_initialize()
{
	menu = new Menu;
	menu->initialize();

	events_manager = new Events;
	events_manager->RegisterSelf();

	hooks_manager->hooks[HOOK_MOVEMENT] = new vmthook((PDWORD*)movement.get());
	hooks_manager->hooks[HOOK_MOVEMENT]->hook_function((DWORD)hooked_processmovement, crypt_hash_n(1));

	hooks_manager->hooks[HOOK_ENGINESOUND] = new vmthook((PDWORD*)enginesound.get()); //-V1032
	hooks_manager->hooks[HOOK_ENGINESOUND]->hook_function((DWORD)hooked_emitsound, crypt_hash_n(5));

	hooks_manager->hooks[HOOK_CLIENTSTATE] = new vmthook((PDWORD*)((uintptr_t)clientstate.get() + 0x8));
	hooks_manager->hooks[HOOK_CLIENTSTATE]->hook_function((DWORD)hooked_packetstart, crypt_hash_n(5));
	hooks_manager->hooks[HOOK_CLIENTSTATE]->hook_function((DWORD)hooked_packetend, crypt_hash_n(6));



	hooks_manager->hooks[HOOK_BSPQUERY] = new vmthook((PDWORD*)engine->GetBSPTreeQuery());
	hooks_manager->hooks[HOOK_BSPQUERY]->hook_function((DWORD)hooked_listleavesinbox, crypt_hash_n(6));



	hooks_manager->hooks[HOOK_DIRECTX] = new vmthook((PDWORD*)device.get());
	hooks_manager->hooks[HOOK_DIRECTX]->hook_function((DWORD)hooked_reset, crypt_hash_n(16));
	hooks_manager->hooks[HOOK_DIRECTX]->hook_function((DWORD)hooked_present, crypt_hash_n(17));

	hooks_manager->hooks[HOOK_CLIENTMODE] = new vmthook((PDWORD*)clientmode.get());

	hooks_manager->hooks[HOOK_CLIENTMODE]->hook_function((DWORD)hooked_overrideview, crypt_hash_n(18));
	hooks_manager->hooks[HOOK_CLIENTMODE]->hook_function((DWORD)hooked_getviewmodelfov, crypt_hash_n(35));
	hooks_manager->hooks[HOOK_CLIENTMODE]->hook_function((DWORD)hooked_postscreeneffects, crypt_hash_n(44));

	hooks_manager->hooks[HOOK_PREDICTION] = new vmthook((PDWORD*)prediction.get());
	hooks_manager->hooks[HOOK_PREDICTION]->hook_function((DWORD)hooked_runcommand, crypt_hash_n(19));


	hooks_manager->hooks[HOOK_CLIENT] = new vmthook((PDWORD*)client.get());
	hooks_manager->hooks[HOOK_CLIENT]->hook_function((DWORD)hooked_createmove_proxy, crypt_hash_n(22));
	hooks_manager->hooks[HOOK_CLIENT]->hook_function((DWORD)hooked_framestagenotify, crypt_hash_n(37));
	hooks_manager->hooks[HOOK_CLIENT]->hook_function((DWORD)hooked_dispatchusermessage, crypt_hash_n(38));

	hooks_manager->hooks[HOOK_PANEL] = new vmthook((PDWORD*)panel.get()); //-V1032
	hooks_manager->hooks[HOOK_PANEL]->hook_function((DWORD)hooked_painttraverse, crypt_hash_n(41));

	hooks_manager->hooks[HOOK_SURFACE] = new vmthook((PDWORD*)surface.get());
	hooks_manager->hooks[HOOK_SURFACE]->hook_function((DWORD)hooked_lockcursor, crypt_hash_n(67));

	hooks_manager->hooks[HOOK_ENGINE] = new vmthook((PDWORD*)engine.get());
	hooks_manager->hooks[HOOK_ENGINE]->hook_function((DWORD)hooked_isconnected, crypt_hash_n(27));
	hooks_manager->hooks[HOOK_ENGINE]->hook_function((DWORD)hooked_getscreenaspectratio, crypt_hash_n(101));





	hooks_manager->hooks[HOOL_CONVAR_SV_CHEATS] = new vmthook((PDWORD*)convars_manager->convars[CONVAR_SV_CHEATS].get());
	hooks_manager->hooks[HOOL_CONVAR_SV_CHEATS]->hook_function((DWORD)hooked_sv_cheats, crypt_hash_n(13));




	auto hook_initialize = crypt_ptr <MH_STATUS __stdcall()>(MH_Initialize);
	auto create_hook = crypt_ptr <MH_STATUS __stdcall(LPVOID, LPVOID, LPVOID*)>(MH_CreateHook);
	auto enable_hook = crypt_ptr <MH_STATUS __stdcall(LPVOID)>(MH_EnableHook);

	hook_initialize.get()();

	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_SETUP_BONES], (LPVOID)hooked_setupbones, (LPVOID*)&original_setupbones); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_STANDARD_BLENDING_RULES], (LPVOID)hooked_standardblendingrules, (LPVOID*)&original_standardblendingrules); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_UPDATE_CLIENTSIDE_ANIMATION], (LPVOID)hooked_updateclientsideanimation, (LPVOID*)&original_updateclientsideanimation); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_PROCESS_INTERPOLATED_LIST], (LPVOID)hooked_processinterpolatedlist, (LPVOID*)&original_processinterpolatedlist); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_PERFORM_SCREEN_OVERLAY], (LPVOID)hooked_performscreenoverlay, (LPVOID*)&original_performscreenoverlay); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_RENDER_MODELS], (LPVOID)hooked_rendermodels, (LPVOID*)&original_rendermodels); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_SETUP_ALIVE_LOOP], (LPVOID)hooked_setupaliveloop, (LPVOID*)&original_setupaliveloop); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_VIEWMODEL_INTERPOLATE], (LPVOID)hooked_viewmodelinterpolate, (LPVOID*)&original_viewmodelinterpolate); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_CL_MOVE], (LPVOID)hooked_clmove, (LPVOID*)&original_clmove); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_CL_READ_PACKETS], (LPVOID)hooked_clreadpackets, (LPVOID*)&original_clreadpackets); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_ENTITY_INTERPOLATE], (LPVOID)hooked_entityinterpolate, (LPVOID*)&original_entityinterpolate); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_SPAWN_SMOKE_EFFECT], (LPVOID)hooked_spawnsmokeeffect, (LPVOID*)&original_spawnsmokeeffect); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_RENDER_SMOKE_OVERLAY], (LPVOID)hooked_rendersmokeoverlay, (LPVOID*)&original_rendersmokeoverlay); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_SHOULD_COLLIDE], (LPVOID)hooked_shouldcollide, (LPVOID*)&original_shouldcollide); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_MODIFY_BONES], (LPVOID)hooked_modifybones, (LPVOID*)&original_modifybones); //-V206 //-V114
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_DO_EXTRA_BONES_PROCESSING], (LPVOID)hooked_doextrabonesprocessing, (LPVOID*)&original_doextrabonesprocessing); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_SHOULD_SKEEP_ANIMATION_FRAME], (LPVOID)hooked_shouldskipanimationframe, nullptr); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_MODIFY_EYE_POSITION], (LPVOID)hooked_modifyeyeposition, nullptr); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_LAYER_TO_INDEX], (LPVOID)hooked_layertoindex, (LPVOID*)&original_layertoindex); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_NOTE_PREDICTION_ERROR], (LPVOID)hooked_notepredictionerror, nullptr); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_GET_EYE_ANGLES], (LPVOID)hooked_get_eye_angles, (LPVOID*)&original_get_eye_angles); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_SET_COLLISION_BOUNDS], (LPVOID)hooked_set_collision_bounds, (LPVOID*)&original_set_collision_bounds); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_ON_RENDER_START], (LPVOID)hooked_onrenderstart, (LPVOID*)&original_onrenderstart); //-V206
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_RETURN_ADDRESS_CLIENT], (LPVOID)hooked_returnaddress, nullptr);
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_RETURN_ADDRESS_ENGINE], (LPVOID)hooked_returnaddress, nullptr);
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_RETURN_ADDRESS_STUDIORENDER], (LPVOID)hooked_returnaddress, nullptr);
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_RETURN_ADDRESS_MATERIALSYSTEM], (LPVOID)hooked_returnaddress, nullptr);
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_ENFORCE_COMPETITIVE_CVAR], (LPVOID)hooked_enforce_competitive_cvar, nullptr);


	//create_hook.get()((LPVOID)signatures_manager->follow_rel32(signatures_manager->find_signature("client.dll", "E8 ? ? ? ? A1 ? ? ? ? 6A 30"),1), (LPVOID)sub_107198F0, (LPVOID*)&original_test);

	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_SHUTDOWN], (LPVOID)hooked_shutdown, nullptr); //-V206 //-V114

	{
		DWORD old = 0;
		VirtualProtect((void*)signatures_manager->signatures[SIGNATURE_VOICE_DATA_REJECT_PRINT], sizeof(BYTE), PAGE_READWRITE, &old);
		*(BYTE*)signatures_manager->signatures[SIGNATURE_VOICE_DATA_REJECT_PRINT] = 0xEB;
		VirtualProtect((void*)signatures_manager->signatures[SIGNATURE_VOICE_DATA_REJECT_PRINT], sizeof(BYTE), old, &old);
	}
#if !RELEASE
	create_hook.get()((LPVOID)signatures_manager->signatures[SIGNATURE_IS_CHANNEL_ENABLED], (LPVOID)hooked_channel_enable, nullptr); //-V206 //-V114
#endif

	enable_hook.get()(MH_ALL_HOOKS);
}



