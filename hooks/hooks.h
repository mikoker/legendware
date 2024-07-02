#pragma once

#include "MinHook.h"
#include "vfunc_hook.hpp"
#include "..\globals.h"
#include "..\data.h"
#include "..\sdk\classes.h"

enum
{
	HOOK_MOVEMENT,
	HOOK_ENGINESOUND,
	HOOK_CLIENTSTATE,
	HOOK_BSPQUERY,
	HOOK_CONVAR_DRAW_MODEL_STATS_OVERLAY,
	HOOK_DIRECTX,
	HOOK_MODELRENDER,
	HOOK_CLIENT,
	HOOK_PANEL,
	HOOK_CLIENTMODE,
	HOOK_PREDICTION,
	HOOK_SURFACE,
	HOOK_ENGINE,
	HOOK_INVENTORY,
	HOOK_STUDIORENDER,
	HOOK_MATERIALSYSTEM,
	HOOK_PANORAMA_MARSHALL,
	HOOL_CONVAR_SV_CHEATS,
	HOOK_FILESYSTEM,
	HOOK_MAX
};

class Events : public IGameEventListener2
{
public:
	virtual void FireGameEvent(IGameEvent* event);
	virtual void RegisterSelf();
	virtual void RemoveSelf();
	virtual int GetEventDebugID(void);
};

class Hooks
{
public:
	crypt_ptr <vmthook> hooks[HOOK_MAX];
};

extern crypt_ptr <Hooks> hooks_manager;
extern crypt_ptr <Events> events_manager;

extern DWORD original_setupbones;
extern DWORD original_standardblendingrules;
extern DWORD original_updateclientsideanimation;
extern DWORD original_processinterpolatedlist;
extern DWORD original_performscreenoverlay;
extern DWORD original_calcviewmodelbob;
extern DWORD original_rendermodels;
extern DWORD original_setupaliveloop;
extern DWORD original_viewmodelinterpolate;
extern DWORD original_clmove;
extern DWORD original_clreadpackets;
extern DWORD original_entityinterpolate;
extern DWORD original_spawnsmokeeffect;
extern DWORD original_rendersmokeoverlay;
extern DWORD original_shouldcollide;
extern DWORD original_get_eye_angles;
extern DWORD original_modifybones;
extern DWORD original_set_collision_bounds;
extern DWORD original_layertoindex;
extern DWORD original_doextrabonesprocessing;
extern DWORD original_onrenderstart;
extern DWORD original_update_generated_material;
extern DWORD original_test_func;

using ProcessMovement = void(__thiscall*)(void*, Player*, CMoveData*);
void __fastcall hooked_processmovement(void* ecx, void* edx, Player* player, CMoveData* move_data);

using PacketStart = void(__thiscall*)(void*, int, int);
void __fastcall hooked_packetstart(void* ecx, void* edx, int incoming, int outgoing);

using PacketEnd = void(__thiscall*)(void*);
void __fastcall hooked_packetend(void* ecx, void* edx);

using Reset = long(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
long __stdcall hooked_reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* presentation_parameters);

using Present = long(__stdcall*)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
long __stdcall hooked_present(IDirect3DDevice9* device, RECT* src_rect, RECT* dest_rect, HWND dest_wnd_override, RGNDATA* dirty_region);

using DrawModelExecute = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);

using FrameStageNotify = void(__stdcall*)(ClientFrameStage_t);
void __stdcall hooked_framestagenotify(ClientFrameStage_t stage);

using PaintTraverse = void(__thiscall*)(void*, vgui::VPANEL, bool, bool);
void __fastcall hooked_painttraverse(void* ecx, void* edx, vgui::VPANEL panel, bool force_repaint, bool allow_force);

using OverrideView = void(__stdcall*)(CViewSetup*);
void __stdcall hooked_overrideview(CViewSetup* setup);

using CreateMove = void(__thiscall*)(IBaseClientDLL*, int, float, bool);
void __stdcall hooked_createmove(int sequence_number, float input_sample_frametime, bool active);
void __fastcall hooked_createmove_proxy(void* ecx, void* edx, int sequence_number, float input_sample_frametime, bool active);

using DoPostScreenEffects = void(__thiscall*)(void*, CViewSetup*);
void __fastcall hooked_postscreeneffects(void* ecx, void* edx, CViewSetup* setup);

using RunCommand = void(__thiscall*)(void*, Player*, CUserCmd*, IMoveHelper*);
void __fastcall hooked_runcommand(void* ecx, void* edx, Player* player, CUserCmd* cmd, IMoveHelper* move_helper);

using LockCursor = void(__thiscall*)(void*);
void __stdcall hooked_lockcursor();

using SetupBones = bool(__thiscall*)(void*, matrix3x4_t*, int, int, float);
bool __fastcall hooked_setupbones(void* ecx, void* edx, matrix3x4_t* bone_world_out, int max_bones, int bone_mask, float current_time);

using StandardBlendingRules = void(__thiscall*)(Player*, CStudioHdr*, Vector*, Quaternion*, float, int);
void __fastcall hooked_standardblendingrules(Player* player, void* edx, CStudioHdr* hdr, Vector* position, Quaternion* q, float current_time, int bone_mask);

using UpdateClientSideAnimation = void(__fastcall*)(Player*);
void __fastcall hooked_updateclientsideanimation(Player* player, void* edx);

using SetupAliveLoop = void(__fastcall*)(AnimationState*);
void __fastcall hooked_setupaliveloop(AnimationState* animation_state, void* edx);

using DoExtraBonesProcessing = void(__thiscall*)(Player*, CStudioHdr*, Vector*, Quaternion*, const matrix3x4_t&, uint8_t*, void*);
void __fastcall hooked_doextrabonesprocessing(Player* player, void* edx, CStudioHdr* hdr, Vector* position, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_list, void* context);

using DrawModelStatsOverlay = int(__thiscall*)(void*);

using ProcessInterpolatedList = int(*)(void);
int hooked_processinterpolatedlist();

void hooked_sequenceproxy(RecvProxyData& data, void* arg2, void* arg3);
LRESULT __stdcall hooked_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

using EquipItemInLoadout = bool(__thiscall*)(PVOID, int, int, uint64_t, bool);
using DrawModel = void(__thiscall*)(IStudioRender*, DrawModelResults_t*, const DrawModelInfo_t&, matrix3x4_t*, float*, float*, const Vector&, int);

using GetScreenAspectRatio = float(__thiscall*)(void*, int, int);
float __fastcall hooked_getscreenaspectratio(void* ecx, void* edx, int width, int height);

using DispatchUserMessage = bool(__thiscall*) (void*, int, int, int, bf_read&);
bool __fastcall hooked_dispatchusermessage(void* ecx, void* edx, int message_type, int unknown, int bytes, bf_read& message_data);

using PerformScreenOverlay = void(__thiscall*) (void*, int, int, int, int);
void __fastcall hooked_performscreenoverlay(void* ecx, void* edx, int x, int y, int w, int h);

using EmitSound = void(__thiscall*)(void*, void*, int, int, const char*, unsigned int, const char*, float, float, int, int, int, const Vector*, const Vector*, void*, bool, float, int, int);
void __fastcall hooked_emitsound(void* ecx, void* edx, void* filter, int entity_index, int channel, const char* sound_entry, unsigned int sound_entry_hash, const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const Vector* origin, const Vector* direction, void* vec_origins, bool update_positions, float sound_time, int speaker_entity, int test);

using ListLeavesInBox = int(__thiscall*)(void*, Vector&, Vector&, unsigned short*, int);
int __fastcall hooked_listleavesinbox(void* ecx, void* edx, Vector& mins, Vector& maxs, unsigned short* list, int list_max);

using CalcViewmodelBob = void(__thiscall*)(Player*, Vector&);


using RenderModels = void(__thiscall*)(void*, void*, int, ModelListByType_t*, int, int, bool);
void __fastcall hooked_rendermodels(void* ecx, void* edx, void* info, int model_type_count, ModelListByType_t* model_list, int total_model_count, int render_mode, bool shadow_depth_include_translucent_materials);

using DrawModelArray = void(__thiscall*)(void*, const StudioModelArrayInfo2_t&, int, StudioArrayData_t*, int, int);

using IsConnected = bool(__thiscall*)(void*);
bool __fastcall hooked_isconnected(void* ecx, void* edx);

using ViewmodelInterpolate = bool(__thiscall*)(Viewmodel*, float);
bool __fastcall hooked_viewmodelinterpolate(Viewmodel* viewmodel, void* edx, float time);

using CLMove = void(__vectorcall*)(float, bool);
void __vectorcall hooked_clmove(float accumulated_extra_samples, bool final_tick);

using CLReadPackets = void(__cdecl*)(bool);
void __cdecl hooked_clreadpackets(bool final_tick);

using EntityInterpolate = int(__thiscall*)(Entity*, float&, Vector&, Vector&, int&);
int __fastcall hooked_entityinterpolate(Entity* entity, void* edx, float& time, Vector& old_origin, Vector& old_angles, int& no_more_changes);

using GetViewmodelFOV = float(__stdcall*)();
float __stdcall hooked_getviewmodelfov();


using SpawnSmokeEffect = void(__thiscall*)(void*);
void __fastcall hooked_spawnsmokeeffect(void* ecx, void* edx);

using RenderSmokeOverlay = void(__thiscall*)(void*, bool);
void __fastcall hooked_rendersmokeoverlay(void* ecx, void* edx, bool pre_view_model);

using ShouldCollide = bool(__thiscall*)(void*, int, int);
bool __fastcall hooked_shouldcollide(void* ecx, void* edx, int collision_group, int contents_mask);

using GetEyeAngles = Vector * (__thiscall*)(Player*);
Vector* __fastcall hooked_get_eye_angles(Player* player, void* edx);

using ModifyBones = void(__thiscall*)(Player*, void*, int);
void __fastcall hooked_modifybones(Player* player, void* edx, void* bones, int mask);

using OnRenderStart = void(__thiscall*)(void*);
void __fastcall hooked_onrenderstart(void* ecx, void* edx);

using LayerToIndex = bool(__thiscall*)(AnimationState*, AnimationLayer*, int&);
bool __fastcall hooked_layertoindex(AnimationState* animation_state, void* edx, AnimationLayer* layer, int& index);

using SVCMsgVoiceData = void(__thiscall*)(void*, void*);


using GetArgsAsStr = char* (__thiscall*)(void*, void*, int);


using SvCheats= int(__thiscall*)(void*);
bool __fastcall hooked_sv_cheats(ConVar* convar, void* edx);

using CanLoadThirdPartyFiles = int(__thiscall*)(void*);


using GetUnverifiedFileHashes = int(__thiscall*)(void*, void*, int);
int __fastcall hooked_get_unverified_file_hashes(void* ecx, void* edx, void* pFiles, int nMaxFiles);

void __fastcall hooked_set_collision_bounds(ICollideable* p, int edx, const Vector& mi, const Vector& ma);

void __fastcall hooked_modifyeyeposition(AnimationState* state, void* edx, Vector& position);
bool __fastcall hooked_shouldskipanimationframe();
void __fastcall hooked_notepredictionerror(Player* player, void* edx, const Vector& delta);

void hooked_velocitymodifier(RecvProxyData& data, void* entity, void* output);
void hooked_spotted(RecvProxyData& data, void* entity, void* output);
void hooked_returnaddress();
void __cdecl hooked_enforce_competitive_cvar(const char* cvar_name, float a2, float a3, int a4);



using Shutdown = void(*)(void*);
void __fastcall hooked_shutdown(void* ecx, void* edx);