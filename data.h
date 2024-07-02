#pragma once

#include "includes.h"
#include "sdk\interfaces\IVEngineClient.hpp"
#include "sdk\interfaces\IInputSystem.hpp"
#include "sdk\interfaces\IBaseClientDll.hpp"
#include "sdk\interfaces\IClientEntityList.hpp"
#include "sdk\interfaces\IClientMode.hpp"
#include "sdk\interfaces\ICvar.hpp"
#include "sdk\interfaces\IEngineTrace.hpp"
#include "sdk\interfaces\IEngineSound.hpp"
#include "sdk\interfaces\IRenderView.hpp"
#include "sdk\interfaces\IVModelRender.hpp"
#include "sdk\interfaces\IMaterialSystem.hpp"
#include "sdk\interfaces\IPanel.hpp"
#include "sdk\interfaces\IVModelInfoClient.hpp"
#include "sdk\interfaces\IMDLCache.hpp"
#include "sdk\interfaces\memalloc.h"
#include "sdk\interfaces\CClientState.hpp"
#include "sdk\interfaces\IPrediction.hpp"
#include "sdk\interfaces\IMoveHelper.hpp"
#include "sdk\interfaces\CInput.hpp"
#include "sdk\interfaces\ISurface.hpp"
#include "sdk\interfaces\IVDebugOverlay.hpp"
#include "sdk\interfaces\IViewRenderBeams.hpp"
#include "sdk\interfaces\IPhysics.hpp"
#include "sdk\interfaces\ILocalize.hpp"
#include "sdk\interfaces\ISoundServices.hpp"
#include "sdk\interfaces\IGameEventManager.hpp"
#include "sdk\interfaces\IStudioRender.hpp"
#include "sdk\interfaces\ITexture.hpp"
#include "sdk\interfaces\IPanorama.hpp"
#include "sdk\interfaces\IClientLeafSystem.hpp"
#include "sdk\interfaces\IRenderToRTHelper.hpp"
#include "sdk\misc\GlobalVars.hpp"
#include "sdk\misc\glow_outline_effect.hpp"
#include "sdk\misc\EHandle.hpp"
#include "sdk\misc\KeyValues.hpp"
#include "sdk\misc\datamap.h"
#include "sdk\misc\CSGameRulesproxy.h"
#include "sdk\math\Vector.hpp"

#include "directx_renderer/texture_pool.hpp"
#include "directx_renderer/texture_info.hpp"
#include "resource_manager.h"

#define INRANGE(x, a, b) (x >= a && x <= b)
#define GETBITS(x) (INRANGE((x & (~0x20)),'A','F') ? ((x & (~0x20)) - 'A' + 0xA) : (INRANGE(x, '0', '9') ? x - '0' : 0))
#define GETBYTE(x) (GETBITS(x[0]) << 4 | GETBITS(x[1]))

enum
{
	HOOK_RESET = 16,
	HOOK_PRESENT = 17,
	HOOK_OVERRIDE_VIEW = 18,
	HOOK_CREATE_MOVE = 24,
	HOOK_FRAME_STAGE_NOTIFY = 37,
	HOOK_PAINT_TRAVERSE = 41,
	HOOK_POST_SCREEN_EFFECTS = 44,
	HOOK_LOCK_CURSOR = 67
};

enum
{
	INDEX_SET_MODEL_INDEX = 75,
	INDEX_THINK = 138,
	INDEX_IS_WEAPON = 166,
	INDEX_UPDATE_CLIENTSIDE_ANIMATION = 224,
	INDEX_PRE_THINK = 318,
	INDEX_POST_THINK = 319,
	INDEX_GET_SPREAD = 453,
	INDEX_WEAPON_DATA = 461,
	INDEX_GET_INACCURACY = 483,
	INDEX_UPDATE_ACCURACY_PENALTY = 484,
};

enum
{
	SIGNATURE_INPUT,
	SIGNATURE_MOVEHELPER,
	SIGNATURE_GLOW,
	SIGNATURE_DEVICE,
	SIGNATURE_SETUP_BONES,
	SIGNATURE_UPDATE_CLIENTSIDE_ANIMATION,
	SIGNATURE_PROCESS_INTERPOLATED_LIST,
	SIGNATURE_DO_EXTRA_BONES_PROCESSING,
	SIGNATURE_SHOULD_SKEEP_ANIMATION_FRAME,
	SIGNATURE_MODIFY_EYE_POSITION,
	SIGNATURE_CACHED_BONE_DATA,
	SIGNATURE_HLTV,
	SIGNATURE_INVALIDATE_PHYSICS_RECURSIVE,
	SIGNATURE_PREDICTION_PLAYER,
	SIGNATURE_PREDICTION_RANDOM_SEED,
	SIGNATURE_CURRENT_COMMAND,
	SIGNATURE_INVALIDATE_BONE_CACHE,
	SIGNATURE_SET_ABS_ANGLES,
	SIGNATURE_SET_ABS_ORIGIN,
	SIGNATURE_APPLY_CLAN_TAG,
	SIGNATURE_KEY_VALUES,
	SIGNATURE_LOAD_FROM_BUFFER,
	SIGNATURE_IS_BREAKABLE,
	SIGNATURE_TRACE_FILTER_SIMPLE,
	SIGNATURE_SET_COLLISION_BOUNDS,
	SIGNATURE_CREATE_STATE,
	SIGNATURE_UPDATE_STATE,
	SIGNATURE_RESET_STATE,
	SIGNATURE_LOOKUP_BONE,
	SIGNATURE_STUDIO_HDR,
	SIGNATURE_DRAW_MODEL_STATS_OVERLAY,
	SIGNATURE_WRITE_USER_CMD,
	SIGNATURE_ALLOW_EXTRAPOLATION,
	SIGNATURE_CUSTOM_MATERIALS,
	SIGNATURE_CUSTOM_MATERIALS_INITIALIZE,
	SIGNATURE_HUD_WEAPON_ICON,
	SIGNATURE_SMOKE_COUNT,
	SIGNATURE_BEAMS,
	SIGNATURE_INVENTORY_MANAGER,
	SIGNATURE_GC_CLIENT,
	SIGNATURE_GAMERULES,
	SIGNATURE_KEY_VALUES_INIT,
	SIGNATURE_FIND_KEY,
	SIGNATURE_SET_STRING,
	SIGNATURE_DISABLE_RENDER_TARGET_ALOCATION_FOREVER,
	SIGNATURE_VIEW_MATRIX,
	SIGNATURE_UPDATE_EQUIPPED_STATE,
	SIGNATURE_PERFORM_SCREEN_OVERLAY,
	SIGNATURE_SEQUENCE_ACTIVITY,
	SIGNATURE_SET_ATTRIBUTE_VALUE,
	SIGNATURE_CREATE_ECON_ITEM,
	SIGNATURE_GET_ITEM_SCHEMA,
	SIGNATURE_REMOVE_ITEM,
	SIGNATURE_SEND_UPDATE,
	SIGNATURE_FIND_CACHE,
	SIGNATURE_CREATE_CACHE,
	SIGNATURE_ADD_ECON_ITEM,
	SIGNATURE_GET_INVENTORY_ITEM,
	SIGNATURE_LOCAL_INVENTORY,
	SIGNATURE_EQUIP_ITEM_IN_LOADOUT,
	SIGNATURE_SETUP_BONES_FOR_ATTACHMENT_QUERIES,
	SIGNATURE_SET_MERGED_MDL,
	SIGNATURE_FIND_HUD_ELEMENT,
	SIGNATURE_HUD,
	SIGNATURE_CHOKE_LIMIT,
	SIGNATURE_MERGED_MDL,
	SIGNATURE_GET_STATIC_DATA,
	SIGNATURE_GET_SOC_DATA,
	SIGNATURE_LIST_LEAVES_IN_BOX,
	SIGNATURE_POSTPROCESSING,
	SIGNATURE_CALCVIEWMODELBOB,
	SIGNATURE_CLEAR_DEATH_NOTICES,
	SIGNATURE_SET_LOCAL_PLAYER_READY,
	SIGNATURE_STANDARD_BLENDING_RULES,
	SIGNATURE_RENDER_MODELS,
	SIGNATURE_SETUP_ALIVE_LOOP,
	SIGNATURE_LAYER_TO_INDEX,
	SIGNATURE_INVENTORY_ACCESS,
	SIGNATURE_INTERPOLATION_FIX,
	SIGNATURE_NOTE_PREDICTION_ERROR,
	SIGNATURE_VIEWMODEL_INTERPOLATE,
	SIGNATURE_CL_MOVE,
	SIGNATURE_CL_READ_PACKETS,
	SIGNATURE_CLIP_RAY_TO_HITBOX,
	SIGNATURE_ENTITY_INTERPOLATE,
	SIGNATURE_LOAD_SKYBOX,
	SIGNATURE_ATTACHMENT_HELPER,
	SIGNATURE_INIT_POSE,
	SIGNATURE_ACCUMULATE_POSE,
	SIGNATURE_CALC_AUTOPLAY_SEQUENCES,
	SIGNATURE_CALC_BONE_ADJUST,
	SIGNATURE_POSE_PARAM_DESC,
	SIGNATURE_UPDATE_CACHE,
	SIGNATURE_BONE_MERGE,
	SIGNATURE_LINE_GOES_THROUGH_SMOKE,
	SIGNATURE_SPAWN_SMOKE_EFFECT,
	SIGNATURE_RENDER_SMOKE_OVERLAY,
	SIGNATURE_STEAM_API,
	SIGNATURE_SHOULD_COLLIDE,
	SIGNATURE_MODIFY_BONES,
	SIGNATURE_FROM_STRING,
	SIGNATURE_RET_EYEPOS_AND_VECTORS,
	SIGNATURE_GET_EYE_ANGLES,
	SIGNATURE_ON_RENDER_START,
	SIGNATURE_RETURN_ADDRESS_CLIENT,
	SIGNATURE_RETURN_ADDRESS_ENGINE,
	SIGNATURE_RETURN_ADDRESS_STUDIORENDER,
	SIGNATURE_RETURN_ADDRESS_MATERIALSYSTEM,
	SIGNATURE_IS_CHANNEL_ENABLED,
	SIGNATURE_FORCE_UPDATE,
	SIGNATURE_CONSTRUCT_VOICE_MESSAGE,
	SIGNATURE_EQUIP,
	SIGNATURE_INITIALIZEATTRIBUTES,
	SIGNATURE_VOICE_DATA_REJECT_PRINT,
	SIGNATURE_PANORAMA_MARSHALL_HELPER,
	SIGNATURE_ACKNOWLEDGE_NEW_ITEM_RETURN_ADDRESS,
	SIGNATURE_CLEAR_INVENTORY_IMAGE_RGBA,
	SIGNATURE_PLAYER_RESOURCE,
	SIGNATURE_ECON_SET_CUSTOM_NAME,
	SIGNATURE_GET_PAINT_KIT_DEFINITION,
	SIGNATURE_SET_VALUE_KEY_VALUE,
	SIGNATURE_GET_INT_KEY_VALUE,
	SIGNATURE_GET_STR_KEY_VALUE,
	SIGNATURE_GET_FLOAT_KEY_VALUE,
	SIGNATURE_LOAD_FROM_FILE_KEY_VALUE,
	SIGNATURE_MERGE_FROM_KEY_VALUE,
	SIGNATURE_DESTRUCTOR_KEY_VALUE,
	SIGNATURE_FILE_SYSTEM,
	SIGNATURE_ENFORCE_COMPETITIVE_CVAR,
	SIGNATURE_UPDATE_GENERATED_MATERIAL,
	SIGNATURE_PRECACHE_CUSTOM_MATERIALS,
	SIGNATURE_C_ECON_ITEM_VIEW_UPDATE,
	SIGNATURE_PROCESS_RENDER_TO_RT_HELPER,
	SIGNATURE_CREATE_REFERENCE_ECON_ITEM,
	SIGNATURE_REMOVE_REFERENCE_ECON_ITEM,
	SIGNATURE_CLEAR_CUSTOM_MATERIALS,
	SIGNATURE_ADD_ECON_ATTRIBUTE,
	SIGNATURE_IMGUTL_READ_PNG_AS_RGBA_FROM_BUFFER,
	SIGNATURE_IMGUTL_READ_JPEG_AS_RGBA,
	SIGNATURE_IMGUTL_READ_TGA_AS_RGBA,
	SIGNATURE_IMGUTL_READ_VTF_AS_RGBA,
	SIGNATURE_RETURN_TO_CLAMPBONESINBBOX,
	SIGNATURE_SHUTDOWN,

	SIGNATURE_MAX
};

enum
{
	CONVAR_SV_GRAVITY,
	CONVAR_R_JIGGLE_BONES,
	CONVAR_CL_FORWARDSPEED,
	CONVAR_CL_SIDESPEED,
	CONVAR_CL_UPSPEED,
	CONVAR_SV_AIRACCELERATE,
	CONVAR_MP_DAMAGE_SCALE_CT_HEAD,
	CONVAR_MP_DAMAGE_SCALE_T_HEAD,
	CONVAR_MP_DAMAGE_SCALE_CT_BODY,
	CONVAR_MP_DAMAGE_SCALE_T_BODY,
	CONVAR_FF_DAMAGE_REDUCTION_BULLETS,
	CONVAR_FF_DAMAGE_BULLET_PENETRATION,
	CONVAR_CL_INTERP,
	CONVAR_CL_INTERP_RATIO,
	CONVAR_SV_CLIENT_MIN_INTERP_RATIO,
	CONVAR_SV_CLIENT_MAX_INTERP_RATIO,
	CONVAR_CL_UPDATERATE,
	CONVAR_SV_MINUPDATERATE,
	CONVAR_SV_MAXUPDATERATE,
	CONVAR_DRAW_MODEL_STATS_OVERLAY,
	CONVAR_SV_MAXUNLAG,
	CONVAR_CL_MOUSEENABLE,
	CONVAR_SV_ENABLEBUNNYHOPPING,
	CONVAR_WEAPON_ACCURACY_NOSPREAD,
	CONVAR_WEAPON_RECOIL_SCALE,
	CONVAR_R_DRAWSPECIFICSTATICPROP,
	CONVAR_SV_FRICTION,
	CONVAR_SV_STOPSPEED,
	CONVAR_MOLOTOV_THROW_DETONATE_TIME,
	CONVAR_WEAPON_MOLOTOV_MAXDETONATESLOPE,
	CONVAR_MP_C4TIMER,
	CONVAR_INFERNO_FLAME_LIFETIME,
	CONVAR_VIEWMODEL_OFFSET_X,
	CONVAR_VIEWMODEL_OFFSET_Y,
	CONVAR_VIEWMODEL_OFFSET_Z,
	CONVAR_SV_SKYNAME,
	CONVAR_SV_JUMP_IMPULSE,
	CONVAR_SV_MAXUSRCMDPROCESSTICKS,
	CONVAR_SV_ACCELERATE,
	CONVAR_SV_ACCELERATE_USE_WEAPON_SPEED,
	CONVAR_WEAPON_DEBUG_SPREAD_SHOW,
	CONVAR_MP_SOLID_TEAMMATES,
	CONVAR_CL_LAGCOMPENSATION,
	CONVAR_ZOOM_SENSITIVITY_RATIO_MOUSE,
	CONVAR_MP_TEAMMATES_ARE_ENEMIES,
	CONVAR_SV_CHEATS,
	CONVAR_SV_PURE,
	CONVAR_MAX
};

void signatures_initialize();
void interfaces_initialize();
void netvars_initialize();
void convars_initialize();
void hooks_initialize();

class Signatures
{
public:
	uintptr_t signatures[SIGNATURE_MAX];

	virtual uintptr_t find_signature(const char* module, const char* signature);
	virtual uintptr_t follow_rel32(uintptr_t address, uintptr_t offset = 0);
};
enum EGCResult
{
	k_EGCResultOK = 0,
	k_EGCResultNoMessage = 1,           // There is no message in the queue
	k_EGCResultBufferTooSmall = 2,      // The buffer is too small for the requested message
	k_EGCResultNotLoggedOn = 3,         // The client is not logged onto Steam
	k_EGCResultInvalidMessage = 4,      // Something was wrong with the message being sent with SendMessage
};
class gc_client_system
{
public:
	virtual EGCResult GCSendMessage(int unMsgType, const void* pubData, int cubData) = 0;
};

class PanoramaMarshallHelper {
public:
};
class IFileSystem {};
extern crypt_ptr <IVEngineClient> engine;
extern crypt_ptr <IBaseClientDLL> client;
extern crypt_ptr <IClientEntityList> entitylist;
extern crypt_ptr <CGlobalVarsBase> globals;
extern crypt_ptr <IEngineTrace> enginetrace;
extern crypt_ptr <ICvar> cvar;
extern crypt_ptr <IPanel> panel;
extern crypt_ptr <IClientMode> clientmode;
extern crypt_ptr <IVDebugOverlay> debugoverlay;
extern crypt_ptr <ISurface> surface;
extern crypt_ptr <CInput> input;
extern crypt_ptr <IVModelInfoClient> modelinfo;
extern crypt_ptr <IVModelRender> modelrender;
extern crypt_ptr <IVRenderView> renderview;
extern crypt_ptr <IMaterialSystem> materialsystem;
extern crypt_ptr <IGameEventManager2> events;
extern crypt_ptr <IMoveHelper> movehelper;
extern crypt_ptr <IMDLCache> modelcache;
extern crypt_ptr <IPrediction> prediction;
extern crypt_ptr <CGameMovement> movement;
extern crypt_ptr <IEngineSound> enginesound;
extern crypt_ptr <CGlowObjectManager> glow;
extern crypt_ptr <CClientState> clientstate;
extern crypt_ptr <IPhysicsSurfaceProps> physicssurface;
extern crypt_ptr <IInputSystem> inputsystem;
extern crypt_ptr <IMemAlloc> memalloc;
extern crypt_ptr <IDirect3DDevice9> device;
extern crypt_ptr <IViewRenderBeams> beams;
extern crypt_ptr <IStudioRender> studiorender;

extern crypt_ptr <gc_client_system> gc_client;
extern crypt_ptr <ILocalize> localize;
extern crypt_ptr <IBaseFileSystem> basefilesys;
extern crypt_ptr <CSGameRulesProxy> gamerules;
extern crypt_ptr <IPanorama> panorama;
extern crypt_ptr <IClientLeafSystem> clientleafsystem;
extern crypt_ptr <IRenderToRTHelper> rendertorthelper;

extern crypt_ptr <IEngineVoice> EngineVoice;

extern crypt_ptr <resource_manager> resources;
extern DWORD postprocessing;
extern crypt_ptr <PanoramaMarshallHelper> panorama_marshall_helper;
extern crypt_ptr <IFileSystem> file_system;

class Interfaces
{
public:
	virtual CreateInterfaceFn get_module(HMODULE module_address);

	template <typename T>
	crypt_ptr <T> get_interface(CreateInterfaceFn create_interface, const char* interface_version);
};

class Convars
{
public:
	crypt_ptr <ConVar> convars[CONVAR_MAX];
};

class Netvars
{
public:
	unordered_map <uint32_t, crypt_ptr <RecvTable>> tables;

	virtual crypt_ptr <RecvTable> get_table(uint32_t table);
	virtual uintptr_t get_prop(uint32_t table, uint32_t netvar, crypt_ptr <crypt_ptr <RecvProp>> prop = nullptr);
	virtual uintptr_t get_prop(crypt_ptr <RecvTable> recv_table, uint32_t netvar, crypt_ptr <crypt_ptr <RecvProp>> prop = nullptr);
	virtual uintptr_t find_offset(uint32_t table, uint32_t netvar);
	virtual uintptr_t find_in_datamap(crypt_ptr <datamap_t> map, uint32_t netvar);
};

#define NETVAR(type, name, table, netvar) \
    type& name() \
    { \
        static auto _##name = netvars_manager->find_offset(table, netvar); \
        return *(type*)((uintptr_t)this + _##name); \
    }

#define PNETVAR(type, name, table, netvar) \
    type* name() \
    { \
        static auto _##name = netvars_manager->find_offset(table, netvar); \
        return (type*)((uintptr_t)this + _##name); \
    }

#define DATAMAP(type, name, netvar) \
    type& name() \
    { \
        static auto _##name = netvars_manager->find_in_datamap(call_virtual <datamap_t* (__thiscall*)(void*)> (this, 17)(this), netvar); \
        return *(type*)((uintptr_t)this + _##name); \
    }

#define OFFSET(type, name, offset) \
	type& name() \
	{ \
		static auto current_offset = offset; \
		return *(type*)((uintptr_t)this + current_offset); \
	}

#define POFFSET(type, name, offset) \
	type* name() \
	{ \
		return (type*)((uintptr_t)this + offset); \
	}

extern crypt_ptr <Signatures> signatures_manager;
extern crypt_ptr <Interfaces> interfaces_manager;
extern crypt_ptr <Convars> convars_manager;
extern crypt_ptr <Netvars> netvars_manager;