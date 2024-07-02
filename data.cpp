// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "data.h"

crypt_ptr <IVEngineClient> engine;
crypt_ptr <IBaseClientDLL> client;
crypt_ptr <IClientEntityList> entitylist;
crypt_ptr <CGlobalVarsBase> globals;
crypt_ptr <IEngineTrace> enginetrace;
crypt_ptr <ICvar> cvar;
crypt_ptr <IPanel> panel;
crypt_ptr <IClientMode> clientmode;
crypt_ptr <IVDebugOverlay> debugoverlay;
crypt_ptr <ISurface> surface;
crypt_ptr <CInput> input;
crypt_ptr <IVModelInfoClient> modelinfo;
crypt_ptr <IVModelRender> modelrender;
crypt_ptr <IVRenderView> renderview;
crypt_ptr <IMaterialSystem> materialsystem;
crypt_ptr <IGameEventManager2> events;
crypt_ptr <IMoveHelper> movehelper;
crypt_ptr <IMDLCache> modelcache;
crypt_ptr <IPrediction> prediction;
crypt_ptr <CGameMovement> movement;
crypt_ptr <IEngineSound> enginesound;
crypt_ptr <CGlowObjectManager> glow;
crypt_ptr <CClientState> clientstate;
crypt_ptr <IPhysicsSurfaceProps> physicssurface;
crypt_ptr <IInputSystem> inputsystem;
crypt_ptr <IMemAlloc> memalloc;
crypt_ptr <IDirect3DDevice9> device;
crypt_ptr <IViewRenderBeams> beams;
crypt_ptr <IStudioRender> studiorender;

crypt_ptr <gc_client_system> gc_client;
crypt_ptr <ILocalize> localize;
crypt_ptr <IBaseFileSystem> basefilesys;
crypt_ptr <CSGameRulesProxy> gamerules;
crypt_ptr <IPanorama> panorama;
crypt_ptr <IClientLeafSystem> clientleafsystem;

crypt_ptr <IEngineVoice> EngineVoice;
DWORD postprocessing;
crypt_ptr <PanoramaMarshallHelper> panorama_marshall_helper;
crypt_ptr <Signatures> signatures_manager;
crypt_ptr <Interfaces> interfaces_manager;
crypt_ptr <Convars> convars_manager;
crypt_ptr <Netvars> netvars_manager;
crypt_ptr <resource_manager> resources;
crypt_ptr <IRenderToRTHelper> rendertorthelper;
crypt_ptr <IFileSystem> file_system;
void signatures_initialize()
{
	signatures_manager->signatures[SIGNATURE_INPUT] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 0x1;
	signatures_manager->signatures[SIGNATURE_MOVEHELPER] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("8B 0D ?? ?? ?? ?? 8B 46 08 68")) + 0x2;
	signatures_manager->signatures[SIGNATURE_GLOW] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("0F 11 05 ?? ?? ?? ?? 83 C8 01")) + 0x3;
	signatures_manager->signatures[SIGNATURE_DEVICE] = signatures_manager->find_signature(crypt_str("shaderapidx9.dll"), crypt_str("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1;
	signatures_manager->signatures[SIGNATURE_SETUP_BONES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F0 B8 D8"));
	signatures_manager->signatures[SIGNATURE_UPDATE_CLIENTSIDE_ANIMATION] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"));
	signatures_manager->signatures[SIGNATURE_DO_EXTRA_BONES_PROCESSING] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C"));
	signatures_manager->signatures[SIGNATURE_SHOULD_SKEEP_ANIMATION_FRAME] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"));
	signatures_manager->signatures[SIGNATURE_MODIFY_EYE_POSITION] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14 83 7F 60"));
	signatures_manager->signatures[SIGNATURE_PROCESS_INTERPOLATED_LIST] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("53 0F B7 1D ? ? ? ? 56"));
	signatures_manager->signatures[SIGNATURE_CACHED_BONE_DATA] = *(uintptr_t*)(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("FF B7 ?? ?? ?? ?? 52")) + 0x2) + 0x4;
	signatures_manager->signatures[SIGNATURE_HLTV] = *(uintptr_t*)(signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("74 0F 80 BF ?? ?? ?? ?? ??")) + 0x4);
	signatures_manager->signatures[SIGNATURE_INVALIDATE_PHYSICS_RECURSIVE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56 83 E0 04"));
	signatures_manager->signatures[SIGNATURE_PREDICTION_PLAYER] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("89 35 ? ? ? ? F3 0F 10 48")) + 0x2;
	signatures_manager->signatures[SIGNATURE_PREDICTION_RANDOM_SEED] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("A3 ? ? ? ? 66 0F 6E 86")) + 0x1;
	signatures_manager->signatures[SIGNATURE_CURRENT_COMMAND] = *(uintptr_t*)(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("89 BE ? ? ? ? E8 ? ? ? ? 85 FF")) + 0x2);
	signatures_manager->signatures[SIGNATURE_INVALIDATE_BONE_CACHE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81"));
	signatures_manager->signatures[SIGNATURE_SET_ABS_ANGLES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));
	signatures_manager->signatures[SIGNATURE_SET_ABS_ORIGIN] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 51 53 56 57 8B F1"));
	signatures_manager->signatures[SIGNATURE_APPLY_CLAN_TAG] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("53 56 57 8B DA 8B F9 FF 15"));
	signatures_manager->signatures[SIGNATURE_KEY_VALUES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 51 33 C0 C7 45"));
	signatures_manager->signatures[SIGNATURE_LOAD_FROM_BUFFER] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"));
	signatures_manager->signatures[SIGNATURE_IS_BREAKABLE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 51 56 8B F1 85 F6 74 68"));
	signatures_manager->signatures[SIGNATURE_TRACE_FILTER_SIMPLE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F0 83 EC 7C 56 52"));
	signatures_manager->signatures[SIGNATURE_SET_COLLISION_BOUNDS] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 18 56 57 8B 7B"));
	signatures_manager->signatures[SIGNATURE_CREATE_STATE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"));
	signatures_manager->signatures[SIGNATURE_UPDATE_STATE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));
	signatures_manager->signatures[SIGNATURE_RESET_STATE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("56 6A 01 68 ? ? ? ? 8B F1"));
	signatures_manager->signatures[SIGNATURE_LOOKUP_BONE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 53 56 8B F1 57 83 BE ?? ?? ?? ?? ?? 75 14"));
	signatures_manager->signatures[SIGNATURE_STUDIO_HDR] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("8B B7 ?? ?? ?? ?? 89 74 24 20"));
	signatures_manager->signatures[SIGNATURE_DRAW_MODEL_STATS_OVERLAY] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 56 57 8B F9 8B 4D 04 E8 ? ? ? ? 8B ? ? ? ? ? 85 F6 74 5B"));
	signatures_manager->signatures[SIGNATURE_WRITE_USER_CMD] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 51 53 56 8B D9"));
	signatures_manager->signatures[SIGNATURE_ALLOW_EXTRAPOLATION] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("A2 ? ? ? ? 8B 45 E8")) + 0x1;
	signatures_manager->signatures[SIGNATURE_CUSTOM_MATERIALS] = *(uintptr_t*)(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("83 BE ? ? ? ? ? 7F 67")) + 0x2) - 0xC;
	signatures_manager->signatures[SIGNATURE_CUSTOM_MATERIALS_INITIALIZE] = *(uintptr_t*)(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("C6 86 ? ? ? ? ? FF 50 04")) + 0x2);
	signatures_manager->signatures[SIGNATURE_SMOKE_COUNT] = *(uintptr_t*)(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("A3 ? ? ? ? 57 8B CB")) + 0x1);
	signatures_manager->signatures[SIGNATURE_BEAMS] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("B9 ?? ?? ?? ?? A1 ?? ?? ?? ?? FF 10 A1 ?? ?? ?? ?? B9")) + 0x1;
	signatures_manager->signatures[SIGNATURE_INVENTORY_MANAGER] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("B9 ?? ?? ?? ?? 8D 44 24 10 89 54 24 14")) + 0x1;
	signatures_manager->signatures[SIGNATURE_GC_CLIENT] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("8B 0D ? ? ? ? 6A 00 83 EC 10")) + 0x2;
	signatures_manager->signatures[SIGNATURE_GAMERULES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("A1 ? ? ? ? 8B 0D ? ? ? ? 6A 00 68 ? ? ? ? C6")) + 0x1;
	signatures_manager->signatures[SIGNATURE_KEY_VALUES_INIT] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 56 8B F1 33 C0 8B 4D 0C 81 26 ? ? ? ? 89 46 10 8A 45 10 C6 46 03 00 89 4E 14 88 46 18 C7 46 ? ? ? ? ? C7 46"));
	signatures_manager->signatures[SIGNATURE_FIND_KEY] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 1C 53 8B D9 85"));
	signatures_manager->signatures[SIGNATURE_SET_STRING] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01"));
	signatures_manager->signatures[SIGNATURE_DISABLE_RENDER_TARGET_ALOCATION_FOREVER] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("80 B9 ? ? ? ? ? 74 0F")) + 0x2;
	signatures_manager->signatures[SIGNATURE_VIEW_MATRIX] = *(uintptr_t*)(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9")) + 0x3) + 0xB0;
	signatures_manager->signatures[SIGNATURE_UPDATE_EQUIPPED_STATE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 8B 45 08 8B D0 C1 EA 10"));
	signatures_manager->signatures[SIGNATURE_PERFORM_SCREEN_OVERLAY] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 51 A1 ? ? ? ? 53 56 8B D9"));
	signatures_manager->signatures[SIGNATURE_SEQUENCE_ACTIVITY] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 53 8B 5D 08 56 8B F1 83"));
	signatures_manager->signatures[SIGNATURE_SET_ATTRIBUTE_VALUE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A 00"));
	signatures_manager->signatures[SIGNATURE_CREATE_ECON_ITEM] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4")) + 0x3;
	signatures_manager->signatures[SIGNATURE_GET_ITEM_SCHEMA] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("A1 ? ? ? ? 85 C0 75 53"));
	signatures_manager->signatures[SIGNATURE_REMOVE_ITEM] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 51 53 56 57 FF 75 0C 8B F1"));
	signatures_manager->signatures[SIGNATURE_SEND_UPDATE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("E8 ? ? ? ? 83 C4 04 80 BF ? ? ? ? ? 74 0C"));
	signatures_manager->signatures[SIGNATURE_FIND_CACHE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 1C 0F 10 45 08"));
	signatures_manager->signatures[SIGNATURE_CREATE_CACHE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 51 53 56 8B D9 8D 45 08"));
	signatures_manager->signatures[SIGNATURE_GET_INVENTORY_ITEM] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 8B 55 08 83 EC 10 8B C2"));
	signatures_manager->signatures[SIGNATURE_LOCAL_INVENTORY] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("8B 8F ? ? ? ? 0F B7 C0 50")) + 0x2;
	signatures_manager->signatures[SIGNATURE_EQUIP_ITEM_IN_LOADOUT] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 24 83 3D ? ? ? ? ? 53 56 57 8B F9"));
	signatures_manager->signatures[SIGNATURE_SETUP_BONES_FOR_ATTACHMENT_QUERIES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 14 83 3D ? ? ? ? ? 53"));
	signatures_manager->signatures[SIGNATURE_SET_MERGED_MDL] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 57 8B F9 8B 0D ? ? ? ? 85 C9 75"));
	signatures_manager->signatures[SIGNATURE_FIND_HUD_ELEMENT] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	signatures_manager->signatures[SIGNATURE_HUD] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("81 25 ? ? ? ? ? ? ? ? 8B 01")) + 0x2;
	signatures_manager->signatures[SIGNATURE_CHOKE_LIMIT] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC")) + 0x1;
	signatures_manager->signatures[SIGNATURE_MERGED_MDL] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("53 8B D9 56 57 8D 4B 04 C7 03 ? ? ? ? E8 ? ? ? ? 6A"));
	signatures_manager->signatures[SIGNATURE_GET_STATIC_DATA] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 51 53 8B D9 8B ? ? ? ? ? 56 57 8B ? ? ? ? ? 85 FF 74 16"));
	signatures_manager->signatures[SIGNATURE_GET_SOC_DATA] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F0 83 EC 18 56 8B F1 57 8B 86"));
	signatures_manager->signatures[SIGNATURE_LIST_LEAVES_IN_BOX] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("56 52 FF 50 18")) + 0x5;
	signatures_manager->signatures[SIGNATURE_POSTPROCESSING] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("80 3D ? ? ? ? ? 53 56 57 0F 85")) + 0x2;
	signatures_manager->signatures[SIGNATURE_CALCVIEWMODELBOB] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9"));
	signatures_manager->signatures[SIGNATURE_CLEAR_DEATH_NOTICES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 0C 53 56 8B 71 58"));
	signatures_manager->signatures[SIGNATURE_SET_LOCAL_PLAYER_READY] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"));
	signatures_manager->signatures[SIGNATURE_STANDARD_BLENDING_RULES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6"));
	signatures_manager->signatures[SIGNATURE_RENDER_MODELS] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 51 8B 45 18 53 56 8B F1 57 85 C0"));
	signatures_manager->signatures[SIGNATURE_SETUP_ALIVE_LOOP] = signatures_manager->follow_rel32(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("E8 ?? ?? ?? ?? 8B CF E8 ?? ?? ?? ?? 8B 47 60")), 0x1);
	signatures_manager->signatures[SIGNATURE_LAYER_TO_INDEX] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 51 53 8B 5D ?? 33 C0 56 8B 31 8B 49"));
	signatures_manager->signatures[SIGNATURE_INVENTORY_ACCESS] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("84 C0 75 05 B0 01 5F"));
	signatures_manager->signatures[SIGNATURE_INTERPOLATION_FIX] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("84 C0 75 14 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0 74 07"));
	signatures_manager->signatures[SIGNATURE_NOTE_PREDICTION_ERROR] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 0C 56 8B F1 8B 06 8B 80 ? ? ? ? FF D0 84 C0 75"));
	signatures_manager->signatures[SIGNATURE_VIEWMODEL_INTERPOLATE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 57 83 BE 50"));
	signatures_manager->signatures[SIGNATURE_CL_MOVE] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("55 8B EC 81 EC 64 01 00 00 53 56 8A F9"));
	signatures_manager->signatures[SIGNATURE_CL_READ_PACKETS] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("53 8A D9 8B 0D ? ? ? ? 56 57 8B B9"));
	signatures_manager->signatures[SIGNATURE_CLIP_RAY_TO_HITBOX] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 F3 0F 10 42"));
	signatures_manager->signatures[SIGNATURE_ENTITY_INTERPOLATE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 51 8B 45 14 56 8B F1 C7"));
	signatures_manager->signatures[SIGNATURE_LOAD_SKYBOX] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
	signatures_manager->signatures[SIGNATURE_ATTACHMENT_HELPER] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4"));
	signatures_manager->signatures[SIGNATURE_INIT_POSE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 10 53 8B D9 89 55 F8 56 57 89 5D F4 8B 0B 89 4D F0"));
	signatures_manager->signatures[SIGNATURE_ACCUMULATE_POSE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F0 B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? A1 ?? ?? ?? ??"));
	signatures_manager->signatures[SIGNATURE_CALC_AUTOPLAY_SEQUENCES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 10 53 56 57 8B 7D 10 8B D9 F3 0F 11 5D ??"));
	signatures_manager->signatures[SIGNATURE_CALC_BONE_ADJUST] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 81 EC ?? ?? ?? ?? 8B C1 89 54 24 04 89 44 24 2C 56 57 8B 00"));
	signatures_manager->signatures[SIGNATURE_POSE_PARAM_DESC] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 8B 45 08 57 8B F9 8B 4F 04 85 C9 75 15 8B"));
	signatures_manager->signatures[SIGNATURE_UPDATE_CACHE] = signatures_manager->follow_rel32(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("E8 ?? ?? ?? ?? 83 7E 10 00 74 64")), 0x1);
	signatures_manager->signatures[SIGNATURE_BONE_MERGE] = *(int*)(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("89 86 ?? ?? ?? ?? E8 ?? ?? ?? ?? FF 75 08")) + 0x2);
	signatures_manager->signatures[SIGNATURE_LINE_GOES_THROUGH_SMOKE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"));
	signatures_manager->signatures[SIGNATURE_SPAWN_SMOKE_EFFECT] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 30 57 8B F9 80 BF 2C 2A"));
	signatures_manager->signatures[SIGNATURE_RENDER_SMOKE_OVERLAY] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 30 80 7D 08 00"));
	signatures_manager->signatures[SIGNATURE_STEAM_API] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("FF 15 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A")) + 0x7;
	signatures_manager->signatures[SIGNATURE_SHOULD_COLLIDE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 B9 ? ? ? ? ? 75 0F"));
	signatures_manager->signatures[SIGNATURE_MODIFY_BONES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 38 83 BF 50 29"));
	signatures_manager->signatures[SIGNATURE_FROM_STRING] = signatures_manager->follow_rel32(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("E8 ? ? ? ? 83 C4 04 89 45 D8")) + 0x1);
	signatures_manager->signatures[SIGNATURE_GET_EYE_ANGLES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("56 8B F1 85 F6 74 32"));
	signatures_manager->signatures[SIGNATURE_ON_RENDER_START] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 0C 53 56 57 8B D9 E8 ?? ?? ?? ?? 8B CB"));
	signatures_manager->signatures[SIGNATURE_RET_EYEPOS_AND_VECTORS] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("FF 90 ? ? ? ? 8B 55 0C 8B C8")) + 0x6;
	signatures_manager->signatures[SIGNATURE_RETURN_ADDRESS_CLIENT] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 56 8B F1 33 C0 57 8B 7D 08"));
	signatures_manager->signatures[SIGNATURE_RETURN_ADDRESS_ENGINE] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("55 8B EC 56 8B F1 33 C0 57 8B 7D 08"));
	signatures_manager->signatures[SIGNATURE_RETURN_ADDRESS_STUDIORENDER] = signatures_manager->find_signature(crypt_str("studiorender.dll"), crypt_str("55 8B EC 56 8B F1 33 C0 57 8B 7D 08"));
	signatures_manager->signatures[SIGNATURE_RETURN_ADDRESS_MATERIALSYSTEM] = signatures_manager->find_signature(crypt_str("materialsystem.dll"), crypt_str("55 8B EC 56 8B F1 33 C0 57 8B 7D 08"));
	signatures_manager->signatures[SIGNATURE_IS_CHANNEL_ENABLED] = signatures_manager->find_signature(crypt_str("tier0.dll"), crypt_str("55 8B EC 8B 45 08 85 C0 78 16 3B 01 7D 12 6B D0 34 8B"));
	signatures_manager->signatures[SIGNATURE_FORCE_UPDATE] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85"));
	signatures_manager->signatures[SIGNATURE_CONSTRUCT_VOICE_MESSAGE] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("56 57 8B F9 8D 4F 08 C7 07 ? ? ? ? E8 ? ? ? ? C7"));
	signatures_manager->signatures[SIGNATURE_EQUIP] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 10 53 8B 5D 08 57 8B F9"));
	signatures_manager->signatures[SIGNATURE_INITIALIZEATTRIBUTES] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 8B 86"));
	signatures_manager->signatures[SIGNATURE_VOICE_DATA_REJECT_PRINT] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("74 26 8B 06 8B CE FF 74 24 28 8B 40 24 FF D0 50 8B 07 8B CF 8B 40 04 FF D0 50"));
	signatures_manager->signatures[SIGNATURE_PANORAMA_MARSHALL_HELPER] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("68 ? ? ? ? 8B C8 E8 ? ? ? ? 8D 4D F4 FF 15 ? ? ? ? 8B CF FF 15 ? ? ? ? 5F 5E 8B E5 5D C3")) + 0x1;
	signatures_manager->signatures[SIGNATURE_ACKNOWLEDGE_NEW_ITEM_RETURN_ADDRESS] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("85 C0 74 33 8B C8 E8 ? ? ? ? B9"));
	signatures_manager->signatures[SIGNATURE_CLEAR_INVENTORY_IMAGE_RGBA] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 81 EC ? ? ? ? 57 8B F9 C7 47"));
	signatures_manager->signatures[SIGNATURE_PLAYER_RESOURCE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("74 30 8B 35 ? ? ? ? 85 F6")) + 0x4;
	signatures_manager->signatures[SIGNATURE_ECON_SET_CUSTOM_NAME] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("E8 ? ? ? ? 8B 46 78 C1 E8 0A A8 01 74 13 8B 46 34"));
	signatures_manager->signatures[SIGNATURE_GET_PAINT_KIT_DEFINITION] = signatures_manager->follow_rel32(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("E8 ? ? ? ? 85 C0 74 47 8B 07 8B")), 1);
	signatures_manager->signatures[SIGNATURE_SET_VALUE_KEY_VALUE] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("55 8B EC 8B 49 08 6A 01 FF 75 08 E8 ? ? ? ? 85 C0 74 0E"));
	signatures_manager->signatures[SIGNATURE_GET_INT_KEY_VALUE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 6A 00 FF 75 08 E8 ? ? ? ? 85 C0 74 42"));
	signatures_manager->signatures[SIGNATURE_GET_STR_KEY_VALUE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 C0 81 ? ? ? ? ? 53 8B 5D 08 56 57 6A"));
	signatures_manager->signatures[SIGNATURE_GET_FLOAT_KEY_VALUE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 51 6A 00 FF 75 08 F3 0F 11 55 ?? E8 ?? ?? ?? ?? 85 C0 74 7C 0F BE 48 10 49"));
	signatures_manager->signatures[SIGNATURE_LOAD_FROM_FILE_KEY_VALUE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 14 53 56 8B 75 08 57 FF 75 10"));
	signatures_manager->signatures[SIGNATURE_MERGE_FROM_KEY_VALUE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 53 8B D9 56 85 DB"));
	signatures_manager->signatures[SIGNATURE_DESTRUCTOR_KEY_VALUE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("56 8B F1 E8 ? ? ? ? 8B 4E 14"));
	signatures_manager->signatures[SIGNATURE_FILE_SYSTEM] = signatures_manager->find_signature(crypt_str("engine.dll"), crypt_str("8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6")) + 0x2;
	signatures_manager->signatures[SIGNATURE_ENFORCE_COMPETITIVE_CVAR] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 C0 83 EC 3C 56 6A"));
	signatures_manager->signatures[SIGNATURE_UPDATE_GENERATED_MATERIAL] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 53 56 57 8B 7D 10 8B F1"));
	signatures_manager->signatures[SIGNATURE_PRECACHE_CUSTOM_MATERIALS] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 0C 53 8B 5D 08 56 57 8B 7D 0C"));
	signatures_manager->signatures[SIGNATURE_C_ECON_ITEM_VIEW_UPDATE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC A1 ? ? ? ? 83 EC 3C 53 56 8B D9 B9 ? ? ? ? 57 FF"));
	signatures_manager->signatures[SIGNATURE_PROCESS_RENDER_TO_RT_HELPER] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("8B 0D ? ? ? ? 85 C9 74 07 8B 01 8B 40 1C FF E0 32 C0"));
	signatures_manager->signatures[SIGNATURE_CREATE_REFERENCE_ECON_ITEM] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 08 A1 ? ? ? ? 53 8B D9 56 8B 08 57 68 ? ? ? ? 89 5D F8 8B 01"));
	signatures_manager->signatures[SIGNATURE_REMOVE_REFERENCE_ECON_ITEM] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC 10 A1 ? ? ? ? 53 56 57 8B F9 89 7D F4 A8 01"));
	signatures_manager->signatures[SIGNATURE_CLEAR_CUSTOM_MATERIALS] = signatures_manager->follow_rel32(signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 51 53 56 8B F1 33 DB 39 5E 10")),1);
	signatures_manager->signatures[SIGNATURE_ADD_ECON_ATTRIBUTE] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 56 FF 75 08 8B F1 8D 8E ? ? ? ? E8 ? ? ? ? 8B 8E ? ? ? ? 85 C9"));

	signatures_manager->signatures[SIGNATURE_IMGUTL_READ_PNG_AS_RGBA_FROM_BUFFER] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC ? 53 56 57 6A ? 6A ? FF 31 89 55 E0 89 4D D8 E8 ? ? ? ? 83 C4 0C"));
	signatures_manager->signatures[SIGNATURE_IMGUTL_READ_JPEG_AS_RGBA] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 81 EC ? ? ? ? 53 56 57 68 ? ? ? ? 51 89 55 E0"));
	signatures_manager->signatures[SIGNATURE_IMGUTL_READ_TGA_AS_RGBA] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC ? 56 68 ? ? ? ? 51 89 55 FC E8 ? ? ? ? 8B F0 83 C4 ? 85 F6"));
	signatures_manager->signatures[SIGNATURE_IMGUTL_READ_VTF_AS_RGBA] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("55 8B EC 83 EC ? 83 65 ? ? 53 56 57 8B F1 C7 45 ? ? ? ? ? 8B 0D ? ? ? ? 8B DA"));
	signatures_manager->signatures[SIGNATURE_SHUTDOWN] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("56 57 8B F9 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B"));
	signatures_manager->signatures[SIGNATURE_RETURN_TO_CLAMPBONESINBBOX] = signatures_manager->find_signature(crypt_str("client.dll"), crypt_str("F3 0F 10 7C 24 ?? F3 0F 10 54 24 ?? 0F 28 CF"));
}

void interfaces_initialize()
{
	auto client_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("client.dll")));
	auto engine_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("engine.dll")));
	auto valvestd_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("vstdlib.dll")));
	auto vgui_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("vguimatsurface.dll")));
	auto vgui2_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("vgui2.dll")));
	auto materialsystem_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("materialsystem.dll")));
	auto datacache_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("datacache.dll")));
	auto vphysics_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("vphysics.dll")));
	auto inputsystem_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("inputsystem.dll")));
	auto tier0_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("tier0.dll")));
	auto studiorender_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("studiorender.dll")));
	auto localize_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("localize.dll")));
	auto file_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("filesystem_stdio.dll")));
	auto panorama_module = interfaces_manager->get_module(GetModuleHandle(crypt_str("panorama.dll")));

	client = interfaces_manager->get_interface <IBaseClientDLL>(client_module, crypt_str("VClient018"));
	entitylist = interfaces_manager->get_interface <IClientEntityList>(client_module, crypt_str("VClientEntityList003"));
	prediction = interfaces_manager->get_interface <IPrediction>(client_module, crypt_str("VClientPrediction001"));
	movement = interfaces_manager->get_interface <CGameMovement>(client_module, crypt_str("GameMovement001"));
	modelcache = interfaces_manager->get_interface <IMDLCache>(datacache_module, crypt_str("MDLCache004"));
	engine = interfaces_manager->get_interface <IVEngineClient>(engine_module, crypt_str("VEngineClient014"));
	modelinfo = interfaces_manager->get_interface <IVModelInfoClient>(engine_module, crypt_str("VModelInfoClient004"));
	modelrender = interfaces_manager->get_interface <IVModelRender>(engine_module, crypt_str("VEngineModel016"));
	renderview = interfaces_manager->get_interface <IVRenderView>(engine_module, crypt_str("VEngineRenderView014"));
	enginetrace = interfaces_manager->get_interface <IEngineTrace>(engine_module, crypt_str("EngineTraceClient004"));
	debugoverlay = interfaces_manager->get_interface <IVDebugOverlay>(engine_module, crypt_str("VDebugOverlay004"));
	events = interfaces_manager->get_interface <IGameEventManager2>(engine_module, crypt_str("GAMEEVENTSMANAGER002"));
	enginesound = interfaces_manager->get_interface <IEngineSound>(engine_module, crypt_str("IEngineSoundClient003"));
	materialsystem = interfaces_manager->get_interface <IMaterialSystem>(materialsystem_module, crypt_str("VMaterialSystem080"));
	cvar = interfaces_manager->get_interface <ICvar>(valvestd_module, crypt_str("VEngineCvar007"));
	panel = interfaces_manager->get_interface <IPanel>(vgui2_module, crypt_str("VGUI_Panel009"));
	surface = interfaces_manager->get_interface <ISurface>(vgui_module, crypt_str("VGUI_Surface031"));
	physicssurface = interfaces_manager->get_interface <IPhysicsSurfaceProps>(vphysics_module, crypt_str("VPhysicsSurfaceProps001"));
	inputsystem = interfaces_manager->get_interface <IInputSystem>(inputsystem_module, crypt_str("InputSystemVersion001"));
	studiorender = interfaces_manager->get_interface <IStudioRender>(studiorender_module, crypt_str("VStudioRender026"));
	localize = interfaces_manager->get_interface <ILocalize>(localize_module, crypt_str("Localize_001"));
	basefilesys = interfaces_manager->get_interface <IBaseFileSystem>(file_module, crypt_str("VBaseFileSystem011"));
	panorama = interfaces_manager->get_interface <IPanorama>(panorama_module, crypt_str("PanoramaUIEngine001"));
	rendertorthelper = interfaces_manager->get_interface< IRenderToRTHelper>(client_module, crypt_str("RenderToRTHelper001"));

	clientleafsystem = interfaces_manager->get_interface <IClientLeafSystem>(client_module, crypt_str("ClientLeafSystem002"));
	input = *(CInput**)signatures_manager->signatures[SIGNATURE_INPUT];
	movehelper = **(IMoveHelper***)signatures_manager->signatures[SIGNATURE_MOVEHELPER];
	glow = *(CGlowObjectManager**)signatures_manager->signatures[SIGNATURE_GLOW];
	device = **(IDirect3DDevice9***)signatures_manager->signatures[SIGNATURE_DEVICE];
	beams = *(IViewRenderBeams**)signatures_manager->signatures[SIGNATURE_BEAMS];

	gc_client = **(gc_client_system***)signatures_manager->signatures[SIGNATURE_GC_CLIENT];
	gamerules = **(CSGameRulesProxy***)signatures_manager->signatures[SIGNATURE_GAMERULES];
	postprocessing = *(DWORD*)signatures_manager->signatures[SIGNATURE_POSTPROCESSING];
	panorama_marshall_helper = *(PanoramaMarshallHelper**)(signatures_manager->signatures[SIGNATURE_PANORAMA_MARSHALL_HELPER]);

	clientmode = **(IClientMode***)((*(uintptr_t**)client.get())[10] + 5);
	globals = **(CGlobalVarsBase***)((*(uintptr_t**)client.get())[0] + 31);
	clientstate = **(CClientState***)((*(uintptr_t**)engine.get())[12] + 16);
	memalloc = *(IMemAlloc**)(GetProcAddress(GetModuleHandle(crypt_str("tier0.dll")), crypt_str("g_pMemAlloc")));

	file_system = **(IFileSystem***)(signatures_manager->signatures[SIGNATURE_FILE_SYSTEM]);
}

void netvars_initialize()
{
	auto client_classes = crypt_ptr <ClientClass>(client->GetAllClasses());

	while (client_classes)
	{
		auto recv_table = crypt_ptr <RecvTable>(client_classes->m_pRecvTable);

		if (recv_table)
			netvars_manager->tables.emplace(crypt_hash_r(client_classes->m_pNetworkName), recv_table);

		client_classes = client_classes->m_pNext;
	}
}

void convars_initialize()
{
	convars_manager->convars[CONVAR_SV_GRAVITY] = cvar->FindVar(crypt_str("sv_gravity"));
	convars_manager->convars[CONVAR_R_JIGGLE_BONES] = cvar->FindVar(crypt_str("r_jiggle_bones"));
	convars_manager->convars[CONVAR_CL_FORWARDSPEED] = cvar->FindVar(crypt_str("cl_forwardspeed"));
	convars_manager->convars[CONVAR_CL_SIDESPEED] = cvar->FindVar(crypt_str("cl_sidespeed"));
	convars_manager->convars[CONVAR_CL_UPSPEED] = cvar->FindVar(crypt_str("cl_upspeed"));
	convars_manager->convars[CONVAR_SV_AIRACCELERATE] = cvar->FindVar(crypt_str("sv_airaccelerate"));
	convars_manager->convars[CONVAR_MP_DAMAGE_SCALE_CT_HEAD] = cvar->FindVar(crypt_str("mp_damage_scale_ct_head"));
	convars_manager->convars[CONVAR_MP_DAMAGE_SCALE_T_HEAD] = cvar->FindVar(crypt_str("mp_damage_scale_t_head"));
	convars_manager->convars[CONVAR_MP_DAMAGE_SCALE_CT_BODY] = cvar->FindVar(crypt_str("mp_damage_scale_ct_body"));
	convars_manager->convars[CONVAR_MP_DAMAGE_SCALE_T_BODY] = cvar->FindVar(crypt_str("mp_damage_scale_t_body"));
	convars_manager->convars[CONVAR_FF_DAMAGE_REDUCTION_BULLETS] = cvar->FindVar(crypt_str("ff_damage_reduction_bullets"));
	convars_manager->convars[CONVAR_FF_DAMAGE_BULLET_PENETRATION] = cvar->FindVar(crypt_str("ff_damage_bullet_penetration"));
	convars_manager->convars[CONVAR_CL_INTERP] = cvar->FindVar(crypt_str("cl_interp"));
	convars_manager->convars[CONVAR_CL_INTERP_RATIO] = cvar->FindVar(crypt_str("cl_interp_ratio"));
	convars_manager->convars[CONVAR_SV_CLIENT_MIN_INTERP_RATIO] = cvar->FindVar(crypt_str("sv_client_min_interp_ratio"));
	convars_manager->convars[CONVAR_SV_CLIENT_MAX_INTERP_RATIO] = cvar->FindVar(crypt_str("sv_client_max_interp_ratio"));
	convars_manager->convars[CONVAR_CL_UPDATERATE] = cvar->FindVar(crypt_str("cl_updaterate"));
	convars_manager->convars[CONVAR_SV_MINUPDATERATE] = cvar->FindVar(crypt_str("sv_minupdaterate"));
	convars_manager->convars[CONVAR_SV_MAXUPDATERATE] = cvar->FindVar(crypt_str("sv_maxupdaterate"));
	convars_manager->convars[CONVAR_DRAW_MODEL_STATS_OVERLAY] = cvar->FindVar(crypt_str("r_drawmodelstatsoverlay"));
	convars_manager->convars[CONVAR_SV_MAXUNLAG] = cvar->FindVar(crypt_str("sv_maxunlag"));
	convars_manager->convars[CONVAR_CL_MOUSEENABLE] = cvar->FindVar(crypt_str("cl_mouseenable"));
	convars_manager->convars[CONVAR_SV_ENABLEBUNNYHOPPING] = cvar->FindVar(crypt_str("sv_enablebunnyhopping"));
	convars_manager->convars[CONVAR_WEAPON_ACCURACY_NOSPREAD] = cvar->FindVar(crypt_str("weapon_accuracy_nospread"));
	convars_manager->convars[CONVAR_WEAPON_RECOIL_SCALE] = cvar->FindVar(crypt_str("weapon_recoil_scale"));
	convars_manager->convars[CONVAR_R_DRAWSPECIFICSTATICPROP] = cvar->FindVar(crypt_str("r_drawspecificstaticprop"));
	convars_manager->convars[CONVAR_SV_FRICTION] = cvar->FindVar(crypt_str("sv_friction"));
	convars_manager->convars[CONVAR_SV_STOPSPEED] = cvar->FindVar(crypt_str("sv_stopspeed"));
	convars_manager->convars[CONVAR_MOLOTOV_THROW_DETONATE_TIME] = cvar->FindVar(crypt_str("molotov_throw_detonate_time"));
	convars_manager->convars[CONVAR_WEAPON_MOLOTOV_MAXDETONATESLOPE] = cvar->FindVar(crypt_str("weapon_molotov_maxdetonateslope"));
	convars_manager->convars[CONVAR_MP_C4TIMER] = cvar->FindVar(crypt_str("mp_c4timer"));
	convars_manager->convars[CONVAR_INFERNO_FLAME_LIFETIME] = cvar->FindVar(crypt_str("inferno_flame_lifetime"));
	convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_X] = cvar->FindVar(crypt_str("viewmodel_offset_x"));
	convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Y] = cvar->FindVar(crypt_str("viewmodel_offset_y"));
	convars_manager->convars[CONVAR_VIEWMODEL_OFFSET_Z] = cvar->FindVar(crypt_str("viewmodel_offset_z"));
	convars_manager->convars[CONVAR_SV_SKYNAME] = cvar->FindVar(crypt_str("sv_skyname"));
	convars_manager->convars[CONVAR_SV_JUMP_IMPULSE] = cvar->FindVar(crypt_str("sv_jump_impulse"));
	convars_manager->convars[CONVAR_SV_MAXUSRCMDPROCESSTICKS] = cvar->FindVar(crypt_str("sv_maxusrcmdprocessticks"));
	convars_manager->convars[CONVAR_SV_ACCELERATE] = cvar->FindVar(crypt_str("sv_accelerate"));
	convars_manager->convars[CONVAR_SV_ACCELERATE_USE_WEAPON_SPEED] = cvar->FindVar(crypt_str("sv_accelerate_use_weapon_speed"));
	convars_manager->convars[CONVAR_WEAPON_DEBUG_SPREAD_SHOW] = cvar->FindVar(crypt_str("weapon_debug_spread_show"));
	convars_manager->convars[CONVAR_MP_SOLID_TEAMMATES] = cvar->FindVar(crypt_str("mp_solid_teammates"));
	convars_manager->convars[CONVAR_CL_LAGCOMPENSATION] = cvar->FindVar(crypt_str("cl_lagcompensation"));
	convars_manager->convars[CONVAR_ZOOM_SENSITIVITY_RATIO_MOUSE] = cvar->FindVar(crypt_str("zoom_sensitivity_ratio_mouse"));
	convars_manager->convars[CONVAR_MP_TEAMMATES_ARE_ENEMIES] = cvar->FindVar(crypt_str("mp_teammates_are_enemies"));
	convars_manager->convars[CONVAR_SV_CHEATS] = cvar->FindVar(crypt_str("sv_cheats"));
	convars_manager->convars[CONVAR_SV_PURE] = cvar->FindVar(crypt_str("sv_pure"));
}

uintptr_t Signatures::find_signature(const char* module, const char* signature)
{
	MODULEINFO module_info;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(module), &module_info, sizeof(MODULEINFO));

	auto start_address = (DWORD)module_info.lpBaseOfDll;
	auto end_address = start_address + module_info.SizeOfImage;

	auto current_signature = signature;
	auto address = (uintptr_t)0;

	for (auto current_address = start_address; current_address < end_address; ++current_address)
	{
		if (!*current_signature)
			break;

		if (*(PBYTE)current_signature == '\?' || *(BYTE*)current_address == GETBYTE(current_signature))
		{
			if (!address)
				address = current_address; //-V101

			if (!current_signature[2])
				break;

			if (*(PWORD)current_signature == '\?\?' || *(PBYTE)current_signature != '\?')
				current_signature += 3;
			else
				current_signature += 2;
		}
		else
		{
			current_signature = signature;
			address = 0;
		}
	}

#ifndef Debug //!RELEASE
	auto console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (!address)
	{
		if (SUCCEEDED(console_handle))
			SetConsoleTextAttribute(console_handle, FOREGROUND_RED);

		cout << module << crypt_str(": ") << signature << crypt_str(" is outdated!") << endl;
	}
	else
	{
		if (SUCCEEDED(console_handle))
			SetConsoleTextAttribute(console_handle, FOREGROUND_GREEN);

		cout << module << crypt_str(": ") << signature << crypt_str(" initialized!") << endl;
		}
#endif

	return address;
	}

uintptr_t Signatures::follow_rel32(uintptr_t address, uintptr_t offset)
{
	if (!address)
		return 0;

	auto offset_address = address + offset;
	auto relative = *(uintptr_t*)(offset_address);

	if (!relative)
		return 0;

	return (uintptr_t)(offset_address + relative + sizeof(uintptr_t));
}

CreateInterfaceFn Interfaces::get_module(HMODULE module_address)
{
	return (CreateInterfaceFn)(GetProcAddress(module_address, crypt_str("CreateInterface")));
}

template <typename T>
crypt_ptr <T> Interfaces::get_interface(CreateInterfaceFn create_interface, const char* interface_version)
{
	auto result = crypt_ptr <T>((T*)create_interface(interface_version, nullptr));

#if !RELEASE
	auto console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (!result)
	{
		if (SUCCEEDED(console_handle))
			SetConsoleTextAttribute(console_handle, FOREGROUND_RED);

		cout << interface_version << crypt_str(" is outdated!") << endl;
	}
	else
	{
		if (SUCCEEDED(console_handle))
			SetConsoleTextAttribute(console_handle, FOREGROUND_GREEN);

		cout << interface_version << crypt_str(" initialized!") << endl;
	}
#endif

	return result;
}

uintptr_t Netvars::get_prop(uint32_t table, uint32_t netvar, crypt_ptr <crypt_ptr <RecvProp>> prop) //-V813
{
	auto recv_table = get_table(table);

	if (!recv_table)
		return 0;

	auto offset = get_prop(recv_table, netvar, prop);

	if (!offset)
		return 0;

	return offset;
}

uintptr_t Netvars::get_prop(crypt_ptr <RecvTable> recv_table, uint32_t netvar, crypt_ptr <crypt_ptr <RecvProp>> prop)
{
	uintptr_t extra_offset = 0;

	for (auto i = 0; i < recv_table->propCount; i++)
	{
		auto recv_prop = crypt_ptr <RecvProp>(&recv_table->props[i]);
		auto recv_child = crypt_ptr <RecvTable>(recv_prop->dataTable);

		if (recv_child && recv_child->propCount > 0)
		{
			auto temp_offset = get_prop(recv_child, netvar, prop);

			if (temp_offset)
				extra_offset += (recv_prop->offset + temp_offset);
		}

		if (crypt_hash_r(recv_prop->name) != netvar)
			continue;

		if (prop)
			*prop.get() = recv_prop;

		return recv_prop->offset + extra_offset;
	}

	return extra_offset;
}

crypt_ptr <RecvTable> Netvars::get_table(uint32_t table)
{
	if (tables.empty())
		return nullptr;

	for (auto& current_table : tables)
		if (current_table.first == table)
			return current_table.second;

	return nullptr;
}

uintptr_t Netvars::find_offset(uint32_t table, uint32_t netvar)
{
	auto offset = get_prop(table, netvar);

	if (!offset)
		return 0;

	return offset;
}

uintptr_t Netvars::find_in_datamap(crypt_ptr <datamap_t> map, uint32_t netvar)
{
	while (map)
	{
		for (auto i = 0; i < map->m_data_num_fields; ++i)
		{
			if (!map->m_data_desc[i].fieldName)
				continue;

			if (netvar == crypt_hash_r(map->m_data_desc[i].fieldName))
				return map->m_data_desc[i].fieldOffset[TD_OFFSET_NORMAL];

			if (map->m_data_desc[i].fieldType == FIELD_EMBEDDED)
			{
				if (map->m_data_desc[i].td)
				{
					auto offset = find_in_datamap(map->m_data_desc[i].td, netvar);

					if (offset)
						return offset;
				}
			}
		}

		map = map->m_base_map;
	}

	return 0;
}