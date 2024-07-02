#pragma once
#include "IMatRenderContext.hpp"

#pragma region studiorender_enumerations
enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,
	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,
	STUDIORENDER_DRAW_ACCURATETIME = 0x10,
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,
	STUDIORENDER_DRAW_WIREFRAME = 0x80,
	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,
	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,
	STUDIORENDER_SSAODEPTHTEXTURE = 0x1000,
	STUDIORENDER_GENERATE_STATS = 0x8000,
};

enum EOverrideType : int
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE
};
#pragma endregion

struct MaterialLightingState_t
{
	Vector			vecAmbientCube[6];
	Vector			vecLightingOrigin;
	int				nLocalLightCount;
	LightDesc_t		localLightDesc[4];
};

struct DrawModelResults_t;
struct ColorMeshInfo_t;

struct DrawModelInfo_t
{
	studiohdr_t* pStudioHdr;
	studiohwdata_t* pHardwareData;
	StudioDecalHandle_t		hDecals;
	int						iSkin;
	int						iBody;
	int						iHitboxSet;
	IClientRenderable* pClientEntity;
	int						iLOD;
	ColorMeshInfo_t* pColorMeshes;
	bool					bStaticLighting;
	MaterialLightingState_t	lightingState;
};
enum class OverrideType {
	Normal = 0,
	BuildShadows,
	DepthWrite,
	CustomMaterial, // weapon skins
	SsaoDepthWrite
};
class IStudioRender
{

public:
	std::byte pad_0[592];
	IMaterial* materialOverride;
	std::byte pad_1[12];
	OverrideType overrideType;
	void SetAmbientLightColors(const Vector4D* pAmbientOnlyColors)
	{

		using Fn = void* (__thiscall*)(void*, const Vector4D*);
		call_virtual<Fn>(this, 20)(this, pAmbientOnlyColors);

	}

	void SetAmbientLightColors(const Vector* pAmbientOnlyColors)
	{
		using Fn = void* (__thiscall*)(void*, const Vector*);
		call_virtual<Fn>(this, 21)(this, pAmbientOnlyColors);

	}

	void SetLocalLights(int nLights, const LightDesc_t* pLights)
	{
		using Fn = void* (__thiscall*)(void*, int, const LightDesc_t*);
		call_virtual<Fn>(this, 22)(this, nLights, pLights);
	}

	void SetColorModulation(float const* arrColor)
	{
		using Fn = void* (__thiscall*)(void*, float const*);
		call_virtual<Fn>(this, 27)(this, arrColor);
	}

	void SetAlphaModulation(float flAlpha)
	{
		using Fn = void* (__thiscall*)(void*, float);
		call_virtual<Fn>(this, 28)(this, flAlpha);
	}

	void ForcedMaterialOverride(IMaterial* pMaterial, EOverrideType nOverrideType = OVERRIDE_NORMAL, int nOverrides = 0)
	{
		using Fn = void* (__thiscall*)(void*, IMaterial*, EOverrideType, int);
		call_virtual<Fn>(this, 33)(this, pMaterial, nOverrideType, nOverrides);
	}
};
