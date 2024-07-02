#pragma once

#include "IAppSystem.hpp"
#include "IVModelRender.hpp"
#include "IMatRenderContext.hpp"
#include "IRefCounted.hpp"
#include "ITexture.hpp"
#include "../misc/UtlBuffer.hpp"

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define MAXSTUDIOSKINS		32

#define TEXTURE_GROUP_LIGHTMAP						"Lightmaps"
#define TEXTURE_GROUP_WORLD							"World textures"
#define TEXTURE_GROUP_MODEL							"Model textures"
#define TEXTURE_GROUP_VGUI							"VGUI textures"
#define TEXTURE_GROUP_PARTICLE						"Particle textures"
#define TEXTURE_GROUP_DECAL							"Decal textures"
#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
#define TEXTURE_GROUP_OTHER							"Other textures"
#define TEXTURE_GROUP_PRECACHED						"Precached"
#define TEXTURE_GROUP_CUBE_MAP						"CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					"RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					"Unaccounted textures"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			"Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			"Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			"Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					"DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					"ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					"Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				"Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			"RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					"Morph Targets"

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
class IMaterial;
class IMesh;
class IVertexBuffer;
class IIndexBuffer;
struct MaterialSystem_Config_t;
class VMatrix;
class matrix3x4_t;
class ITexture;
struct MaterialSystemHWID_t;
class KeyValues;
class IShader;
class IVertexTexture;
class IMorph;
class IMatRenderContext;
class ICallQueue;
struct MorphWeight_t;
class IFileList;
struct VertexStreamSpec_t;
struct ShaderStencilState_t;
struct MeshInstanceData_t;
class IClientMaterialSystem;
class CPaintMaterial;
class IPaintMapDataManager;
class IPaintMapTextureManager;
class GPUMemoryStats;
struct AspectRatioInfo_t;
struct CascadedShadowMappingState_t;

class IMaterialProxyFactory;
class ITexture;
class IMaterialSystemHardwareConfig;
class CShadowMgr;

enum CompiledVtfFlags
{
	TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
	TEXTUREFLAGS_TRILINEAR = 0x00000002,
	TEXTUREFLAGS_CLAMPS = 0x00000004,
	TEXTUREFLAGS_CLAMPT = 0x00000008,
	TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
	TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
	TEXTUREFLAGS_PWL_CORRECTED = 0x00000040,
	TEXTUREFLAGS_NORMAL = 0x00000080,
	TEXTUREFLAGS_NOMIP = 0x00000100,
	TEXTUREFLAGS_NOLOD = 0x00000200,
	TEXTUREFLAGS_ALL_MIPS = 0x00000400,
	TEXTUREFLAGS_PROCEDURAL = 0x00000800,
	TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
	TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
	TEXTUREFLAGS_ENVMAP = 0x00004000,
	TEXTUREFLAGS_RENDERTARGET = 0x00008000,
	TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
	TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
	TEXTUREFLAGS_SINGLECOPY = 0x00040000,
	TEXTUREFLAGS_PRE_SRGB = 0x00080000,
	TEXTUREFLAGS_UNUSED_00100000 = 0x00100000,
	TEXTUREFLAGS_UNUSED_00200000 = 0x00200000,
	TEXTUREFLAGS_UNUSED_00400000 = 0x00400000,
	TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
	TEXTUREFLAGS_UNUSED_01000000 = 0x01000000,
	TEXTUREFLAGS_CLAMPU = 0x02000000,
	TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,
	TEXTUREFLAGS_SSBUMP = 0x08000000,
	TEXTUREFLAGS_UNUSED_10000000 = 0x10000000,
	TEXTUREFLAGS_BORDER = 0x20000000,
	TEXTUREFLAGS_UNUSED_40000000 = 0x40000000,
	TEXTUREFLAGS_UNUSED_80000000 = 0x80000000
};

enum StandardLightmap_t
{
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE = -1,
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE_BUMP = -2,
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_USER_DEFINED = -3
};


struct MaterialSystem_SortInfo_t
{
	IMaterial	*material;
	int			lightmapPageID;
};

enum MaterialThreadMode_t
{
	MATERIAL_SINGLE_THREADED,
	MATERIAL_QUEUED_SINGLE_THREADED,
	MATERIAL_QUEUED_THREADED
};

enum MaterialContextType_t
{
	MATERIAL_HARDWARE_CONTEXT,
	MATERIAL_QUEUED_CONTEXT,
	MATERIAL_NULL_CONTEXT
};

enum
{
	MATERIAL_ADAPTER_NAME_LENGTH = 512
};

struct MaterialTextureInfo_t
{
	int iExcludeInformation;
};

struct ApplicationPerformanceCountersInfo_t
{
	float msMain;
	float msMST;
	float msGPU;
	float msFlip;
	float msTotal;
};

struct ApplicationInstantCountersInfo_t
{
	uint32_t m_nCpuActivityMask;
	uint32_t m_nDeferredWordsAllocated;
};
struct MaterialAdapterInfo_t
{
	char m_pDriverName[MATERIAL_ADAPTER_NAME_LENGTH];
	unsigned int m_VendorID;
	unsigned int m_DeviceID;
	unsigned int m_SubSysID;
	unsigned int m_Revision;
	int m_nDXSupportLevel;			// This is the *preferred* dx support level
	int m_nMinDXSupportLevel;
	int m_nMaxDXSupportLevel;
	unsigned int m_nDriverVersionHigh;
	unsigned int m_nDriverVersionLow;
};

struct MaterialVideoMode_t
{
	int m_Width;			// if width and height are 0 and you select 
	int m_Height;			// windowed mode, it'll use the window size
	ImageFormat m_Format;	// use ImageFormats (ignored for windowed mode)
	int m_RefreshRate;		// 0 == default (ignored for windowed mode)
};
enum HDRType_t
{
	HDR_TYPE_NONE,
	HDR_TYPE_INTEGER,
	HDR_TYPE_FLOAT,
};

enum RestoreChangeFlags_t
{
	MATERIAL_RESTORE_VERTEX_FORMAT_CHANGED = 0x1,
	MATERIAL_RESTORE_RELEASE_MANAGED_RESOURCES = 0x2,
};

enum RenderTargetSizeMode_t
{
	RT_SIZE_NO_CHANGE = 0,
	RT_SIZE_DEFAULT = 1,
	RT_SIZE_PICMIP = 2,
	RT_SIZE_HDR = 3,
	RT_SIZE_FULL_FRAME_BUFFER = 4,
	RT_SIZE_OFFSCREEN = 5,
	RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP = 6
};

enum EClearFlags : unsigned int
{
	VIEW_CLEAR_COLOR = 0x1,
	VIEW_CLEAR_DEPTH = 0x2,
	VIEW_CLEAR_FULL_TARGET = 0x4,
	VIEW_NO_DRAW = 0x8,
	VIEW_CLEAR_OBEY_STENCIL = 0x10,
	VIEW_CLEAR_STENCIL = 0x20
};

enum MaterialRenderTargetDepth_t
{
	MATERIAL_RT_DEPTH_SHARED = 0x0,
	MATERIAL_RT_DEPTH_SEPARATE = 0x1,
	MATERIAL_RT_DEPTH_NONE = 0x2,
	MATERIAL_RT_DEPTH_ONLY = 0x3,
};

enum ECreateRenderTargetFlags : unsigned int
{
	CREATERENDERTARGETFLAGS_HDR = 0x00000001,
	CREATERENDERTARGETFLAGS_AUTOMIPMAP = 0x00000002,
	CREATERENDERTARGETFLAGS_UNFILTERABLE_OK = 0x00000004,
	CREATERENDERTARGETFLAGS_NOEDRAM = 0x00000008,
	CREATERENDERTARGETFLAGS_TEMP = 0x00000010
};

typedef void(*MaterialBufferReleaseFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*MaterialBufferRestoreFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*ModeChangeCallbackFunc_t)(void);
typedef void(*EndFrameCleanupFunc_t)(void);
typedef bool(*EndFramePriorToNextContextFunc_t)(void);
typedef void(*OnLevelShutdownFunc_t)(void *pUserData);

typedef unsigned short MaterialHandle_t;
DECLARE_POINTER_HANDLE(MaterialLock_t);

struct UberlightState_t
{
	UberlightState_t()
	{
		m_fNearEdge = 2.0f;
		m_fFarEdge = 100.0f;
		m_fCutOn = 10.0f;
		m_fCutOff = 650.0f;
		m_fShearx = 0.0f;
		m_fSheary = 0.0f;
		m_fWidth = 0.3f;
		m_fWedge = 0.05f;
		m_fHeight = 0.3f;
		m_fHedge = 0.05f;
		m_fRoundness = 0.8f;
	}

	float m_fNearEdge;
	float m_fFarEdge;
	float m_fCutOn;
	float m_fCutOff;
	float m_fShearx;
	float m_fSheary;
	float m_fWidth;
	float m_fWedge;
	float m_fHeight;
	float m_fHedge;
	float m_fRoundness;

	IMPLEMENT_OPERATOR_EQUAL(UberlightState_t);
};

struct FlashlightState_t
{
	FlashlightState_t()
	{
		m_bEnableShadows = false;						// Provide reasonable defaults for shadow depth mapping parameters
		m_bDrawShadowFrustum = false;
		m_flShadowMapResolution = 1024.0f;
		m_flShadowFilterSize = 3.0f;
		m_flShadowSlopeScaleDepthBias = 16.0f;
		m_flShadowDepthBias = 0.0005f;
		m_flShadowJitterSeed = 0.0f;
		m_flShadowAtten = 0.0f;
		m_flAmbientOcclusion = 0.0f;
		m_nShadowQuality = 0;
		m_bShadowHighRes = false;

		m_bScissor = false;
		m_nLeft = -1;
		m_nTop = -1;
		m_nRight = -1;
		m_nBottom = -1;

		m_bUberlight = false;

		m_bVolumetric = false;
		m_flNoiseStrength = 0.8f;
		m_flFlashlightTime = 0.0f;
		m_nNumPlanes = 64;
		m_flPlaneOffset = 0.0f;
		m_flVolumetricIntensity = 1.0f;

		m_bOrtho = false;
		m_fOrthoLeft = -1.0f;
		m_fOrthoRight = 1.0f;
		m_fOrthoTop = -1.0f;
		m_fOrthoBottom = 1.0f;

		m_fBrightnessScale = 1.0f;
		m_pSpotlightTexture = NULL;
		m_pProjectedMaterial = NULL;
		m_bShareBetweenSplitscreenPlayers = false;
	}

	Vector m_vecLightOrigin;
	Quaternion m_quatOrientation;
	float m_NearZ;
	float m_FarZ;
	float m_fHorizontalFOVDegrees;
	float m_fVerticalFOVDegrees;
	bool  m_bOrtho;
	float m_fOrthoLeft;
	float m_fOrthoRight;
	float m_fOrthoTop;
	float m_fOrthoBottom;
	float m_fQuadraticAtten;
	float m_fLinearAtten;
	float m_fConstantAtten;
	float m_FarZAtten;
	float m_Color[4];
	float m_fBrightnessScale;
	ITexture* m_pSpotlightTexture;
	IMaterial* m_pProjectedMaterial;
	int m_nSpotlightTextureFrame;

	// Shadow depth mapping parameters
	bool  m_bEnableShadows;
	bool  m_bDrawShadowFrustum;
	float m_flShadowMapResolution;
	float m_flShadowFilterSize;
	float m_flShadowSlopeScaleDepthBias;
	float m_flShadowDepthBias;
	float m_flShadowJitterSeed;
	float m_flShadowAtten;
	float m_flAmbientOcclusion;
	int   m_nShadowQuality;
	bool  m_bShadowHighRes;

	// simple projection
	float m_flProjectionSize;
	float m_flProjectionRotation;

	// Uberlight parameters
	bool m_bUberlight;
	UberlightState_t m_uberlightState;

	bool m_bVolumetric;
	float m_flNoiseStrength;
	float m_flFlashlightTime;
	int m_nNumPlanes;
	float m_flPlaneOffset;
	float m_flVolumetricIntensity;
	bool m_bShareBetweenSplitscreenPlayers;	// When true, this flashlight will render for all splitscreen players

	// Getters for scissor members
	bool DoScissor() const { return m_bScissor; }
	int GetLeft()	 const { return m_nLeft; }
	int GetTop()	 const { return m_nTop; }
	int GetRight()	 const { return m_nRight; }
	int GetBottom()	 const { return m_nBottom; }

private:

	friend class CShadowMgr;

	bool m_bScissor;
	int m_nLeft;
	int m_nTop;
	int m_nRight;
	int m_nBottom;

	IMPLEMENT_OPERATOR_EQUAL(FlashlightState_t);
};

struct FlashlightInstance_t
{
	IMaterial* m_pDebugMaterial;
	FlashlightState_t m_FlashlightState;
	VMatrix m_WorldToTexture;
	ITexture* m_pFlashlightDepthTexture;
};

struct StudioArrayData_t
{
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pHardwareData;
	void* m_pInstanceData; // See StudioShadowArrayInstanceData_t or StudioArrayInstanceData_t
	int m_nCount;
};

struct StudioModelArrayInfo2_t
{
	int						m_nFlashlightCount;
	FlashlightInstance_t* m_pFlashlights;	// NOTE: Can have at most MAX_FLASHLIGHTS_PER_INSTANCE_DRAW_CALL of these
};

struct StudioModelArrayInfo_t : public StudioModelArrayInfo2_t
{
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pHardwareData;
};

enum RenderableLightingModel_t
{
	LIGHTING_MODEL_NONE = -1,
	LIGHTING_MODEL_STANDARD = 0,
	LIGHTING_MODEL_STATIC_PROP,
	LIGHTING_MODEL_PHYSICS_PROP,

	LIGHTING_MODEL_COUNT,
};

class IClientModelRenderable;

struct RenderableInstance_t
{
	uint8_t m_nAlpha;
};

struct ModelRenderSystemData_t
{
	IClientRenderable* m_pRenderable;
	IClientModelRenderable* m_pModelRenderable;
	RenderableInstance_t m_InstanceData;
};

struct ModelListNode_t
{
	ModelRenderSystemData_t m_Entry;
	int m_nInitialListIndex : 24;
	uint32 m_bBoneMerge : 1;
	int m_nLOD : 7;
	ShaderStencilState_t* m_pStencilState;
	ModelListNode_t* m_pNext;
};

struct StudioShadowArrayInstanceData_t
{
	int m_nLOD;
	int m_nBody;
	int m_nSkin;
	matrix3x4_t* m_pPoseToWorld;
	float* m_pFlexWeights;
	float* m_pDelayedFlexWeights;
};

struct StudioArrayInstanceData_t : public StudioShadowArrayInstanceData_t
{
	MaterialLightingState_t* m_pLightingState;
	MaterialLightingState_t* m_pDecalLightingState;
	ITexture* m_pEnvCubemapTexture;
	StudioDecalHandle_t m_Decals;
	uint32 m_nFlashlightUsage;	// Mask indicating which flashlights to use.
	ShaderStencilState_t* m_pStencilState;
	ColorMeshInfo_t* m_pColorMeshInfo;
	bool m_bColorMeshHasIndirectLightingOnly;
	Vector4D m_DiffuseModulation;
};


struct RenderModelInfo_t : public StudioArrayInstanceData_t
{
	ModelRenderSystemData_t m_Entry;
	ModelInstanceHandle_t m_hInstance;
	matrix3x4_t* m_pBoneToWorld;
	uint32 m_nInitialListIndex : 24;
	uint32 m_bSetupBonesOnly : 1;
	uint32 m_bBoneMerge : 1;
};

struct ModelListByType_t : public StudioModelArrayInfo_t
{
	RenderableLightingModel_t m_nLightingModel;
	const model_t* m_pModel;
	ModelListNode_t* m_pFirstNode;
	int m_nCount;
	int m_nSetupBoneCount;
	uint32 m_nParentDepth : 31;
	uint32 m_bWantsStencil : 1;
	RenderModelInfo_t* m_pRenderModels;
	ModelListByType_t* m_pNextLightingModel;

	// speed up std::sort by implementing these
	ModelListByType_t& operator=(const ModelListByType_t& rhs)
	{
		memcpy(this, &rhs, sizeof(ModelListByType_t));
		return *this;
	}

	ModelListByType_t() {}

	ModelListByType_t(const ModelListByType_t& rhs)
	{
		memcpy(this, &rhs, sizeof(ModelListByType_t));
	}
};
enum CompositeTextureFormat_t
{
	COMPOSITE_TEXTURE_FORMAT_DXT1 = 1,
	COMPOSITE_TEXTURE_FORMAT_DXT5 = 5
};

enum CompositeTextureSize_t
{
	COMPOSITE_TEXTURE_SIZE_256 = 8,
	COMPOSITE_TEXTURE_SIZE_512,
	COMPOSITE_TEXTURE_SIZE_1024,
	COMPOSITE_TEXTURE_SIZE_2048
};


template <class T> struct C11RemoveReference { typedef T Type; };
template <class T> struct C11RemoveReference<T&> { typedef T Type; };
template <class T> struct C11RemoveReference<T&&> { typedef T Type; };

template <class T>
inline typename C11RemoveReference<T>::Type&& Move(T&& obj)
{
	return static_cast<typename C11RemoveReference<T>::Type&&>(obj);
}

template <class T>
inline T&& Forward(typename C11RemoveReference<T>::Type& obj)
{
	return static_cast<T&&>(obj);
}

template <class T>
inline T&& Forward(typename C11RemoveReference<T>::Type&& obj)
{
	return static_cast<T&&>(obj);
}
class IVisualsDataCompare
{
public:
	virtual void FillCompareBlob() {};
	virtual const CUtlBuffer& GetCompareBlob() const = 0;
	virtual bool Compare(const CUtlBuffer& pOther) { return false; };
};
class IVisualsDataProcessor : public CRefCounted
{
public:
	IVisualsDataProcessor() {}

	virtual KeyValues* GenerateCustomMaterialKeyValues() = 0;
	virtual KeyValues* GenerateCompositeMaterialKeyValues(int nMaterialParamId) = 0;
	virtual IVisualsDataCompare* GetCompareObject() = 0;
	virtual bool HasCustomMaterial() const = 0;
	virtual const char* GetOriginalMaterialName() const = 0;
	virtual const char* GetOriginalMaterialBaseName() const = 0;
	virtual const char* GetPatternVTFName() const = 0;
	virtual void Refresh() = 0;

protected:
	virtual ~IVisualsDataProcessor() {}
};


struct SCompositeTextureInfo
{
	IVisualsDataProcessor* m_pVisualsDataProcessor;
	CompositeTextureSize_t m_size;
	CompositeTextureFormat_t m_format;
	int m_nMaterialParamID;
	bool m_bSRGB;
};
class ICompositeTexture : public IRefCounted
{
public:
	virtual bool IsReady() const = 0;
	virtual bool GenerationComplete() const = 0;
	virtual void* GetResultVTF() = 0;
	virtual	const char* GetName() = 0;

	char m_szTextureName[MAX_PATH];
	CompositeTextureSize_t m_size;
	CompositeTextureFormat_t m_format;	// DXT1 or DXT5 only
	int m_nMaterialParamNameId;	// the material parameter that this texture will fill/replace in the eventual Custom Material, also used to interact with the VisualsDataProcessor
	bool m_bSRGB;

};
template< class T >
class CBaseVisualsDataProcessor : public IVisualsDataProcessor
{
public:
	CBaseVisualsDataProcessor() {}

	virtual bool Compare(const CUtlBuffer& otherBuf) { return GetCompareObject()->Compare(otherBuf); }
	virtual IVisualsDataCompare* GetCompareObject() { return &m_compareObject; }
	virtual const char* GetPatternVTFName() const { return NULL; }

	//protected:
	T m_compareObject;
};

class CBaseVisualsDataCompare : public IVisualsDataCompare
{
public:
	CBaseVisualsDataCompare() = default;

	CBaseVisualsDataCompare(CBaseVisualsDataCompare&& moveFrom) noexcept
		: m_nIndex(Move(moveFrom.m_nIndex))
		, m_nSeed(Move(moveFrom.m_nSeed))
		, m_flWear(Move(moveFrom.m_flWear))
		, m_nLOD(Move(moveFrom.m_nLOD))
		, m_nModelID(Move(moveFrom.m_nModelID))
		, m_compareBlob(Move(moveFrom.m_compareBlob))
	{}

	CBaseVisualsDataCompare& operator=(CBaseVisualsDataCompare&& moveFrom) noexcept
	{
		m_nIndex = Move(moveFrom.m_nIndex);
		m_nSeed = Move(moveFrom.m_nSeed);
		m_flWear = Move(moveFrom.m_flWear);
		m_nLOD = Move(moveFrom.m_nLOD);
		m_nModelID = Move(moveFrom.m_nModelID);
		m_compareBlob = Move(moveFrom.m_compareBlob);
		return *this;
	}

	int m_nIndex;
	int m_nSeed;
	float m_flWear;
	int m_nLOD;
	int m_nModelID;

	//private:
	CUtlBuffer m_compareBlob;
	virtual const CUtlBuffer& GetCompareBlob() const
	{
		return m_compareBlob;
	}
};
enum ELightType : int
{
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT
};

enum ELightTypeOptimizationFlags
{
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0 = 1,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1 = 2,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2 = 4,
	LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED = 8
};

struct LightDesc_t
{
	void InitDirectional(const Vector& vecDirection, const Vector& vecColor)
	{
		this->nType = MATERIAL_LIGHT_DIRECTIONAL;
		this->vecColor = vecColor;
		this->vecDirection = vecDirection;
		this->flRange = 0.0f;
		this->flAttenuation0 = 1.0f;
		this->flAttenuation1 = 0.f;
		this->flAttenuation2 = 0.f;

		fFlags = LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED;

		if (flAttenuation0)
			fFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0;
		if (flAttenuation1)
			fFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1;
		if (flAttenuation2)
			fFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2;

		if (nType == MATERIAL_LIGHT_SPOT)
		{
			flThetaDot = std::cosf(flTheta);
			flPhiDot = std::cosf(flPhi);

			if (const float flSpread = flThetaDot - flPhiDot; flSpread > 1.0e-10f)
				// note - this quantity is very sensitive to round off error. the sse reciprocal approximation won't cut it here.
				flOneOverThetaDotMinusPhiDot = 1.0f / flSpread;
			else
				// hard falloff instead of divide by zero
				flOneOverThetaDotMinusPhiDot = 1.0f;
		}
		else if (nType == MATERIAL_LIGHT_DIRECTIONAL)
		{
			// set position to be real far away in the right direction
			vecPosition = vecDirection;
			vecPosition *= 2.0e6;
		}

		flRangeSquared = flRange * flRange;
	}

	ELightType	nType;
	Vector		vecColor;
	Vector		vecPosition;
	Vector		vecDirection;
	float		flRange;
	float		flFalloff;
	float		flAttenuation0;
	float		flAttenuation1;
	float		flAttenuation2;
	float		flTheta;
	float		flPhi;
	float		flThetaDot;
	float		flPhiDot;
	float		flOneOverThetaDotMinusPhiDot;
	std::uint32_t fFlags;
protected:
	float		flRangeSquared;
};
class CCSWeaponVisualsDataCompare : public CBaseVisualsDataCompare
{
public:
	CCSWeaponVisualsDataCompare() = default;

	CCSWeaponVisualsDataCompare(CCSWeaponVisualsDataCompare&& moveFrom) noexcept
		: CBaseVisualsDataCompare(Move(moveFrom))
		, m_flWeaponLength(Move(moveFrom.m_flWeaponLength))
		, m_flUVScale(Move(moveFrom.m_flUVScale))
	{
	}

	CCSWeaponVisualsDataCompare& operator= (CCSWeaponVisualsDataCompare&& moveFrom) noexcept
	{
		*(CBaseVisualsDataCompare*)this = Move(moveFrom);
		m_flWeaponLength = Move(moveFrom.m_flWeaponLength);
		m_flUVScale = Move(moveFrom.m_flUVScale);
		return *this;
	}

	float m_flWeaponLength;
	float m_flUVScale;
};

class CCSWeaponVisualsDataProcessor : public CRefCounted
{
public:

	virtual KeyValues* GenerateCustomMaterialKeyValues(bool m_bBumpMap) = 0;
	virtual KeyValues* GenerateCompositeMaterialKeyValues(int nMaterialParamId) = 0;
	virtual void Function4() = 0;
	virtual bool HasCustomMaterial() const = 0;
	virtual const char* GetOriginalMaterialName() const = 0;
	virtual const char* GetOriginalMaterialBaseName() const = 0;
	virtual const char* GetPatternVTFName() const = 0;
	virtual void Refresh() = 0;
	virtual void Function10() = 0;
	virtual void SetVisualsData(const char* pCompositingShaderName = NULL) = 0;
};





class CMaterialReference
{
public:
	CMaterialReference() : m_pMaterial(nullptr)
	{
	};
	~CMaterialReference()
	{
		Shutdown();
	};
	IMaterial* GetMaterial() const noexcept { return m_pMaterial; }
	void Init(IMaterial* pMaterial) {
		if (m_pMaterial != pMaterial)
		{
			Shutdown();
			m_pMaterial = pMaterial;
			if (m_pMaterial)
				m_pMaterial->IncrementReferenceCount();
		};
	};
	void Shutdown() 
	{
		if (m_pMaterial)
		{
			m_pMaterial->DecrementReferenceCount();
			m_pMaterial = nullptr;
		}
	};

private:
	IMaterial* m_pMaterial;
};

class CCustomMaterial : public CRefCounted
{
public:
	virtual IMaterial* GetMaterial() = 0;
	virtual void AddTexture(ICompositeTexture* pTexture) = 0;
	virtual ICompositeTexture* GetTexture(int nIndex) = 0;
	virtual bool IsValid() const = 0;
	virtual bool CheckRegenerate(int nSize) = 0;
	virtual const char* GetBaseMaterialName() = 0;

	CMaterialReference m_Material;
	CUtlVector< ICompositeTexture* > m_pTextures;

	bool m_bValid;
	int m_nModelMaterialIndex;
	KeyValues* m_pVMTKeyValues;
	const char* m_szBaseMaterialName;

	static int m_nMaterialCount;
};
class CCustomMaterialOwner
{
public:
	virtual ~CCustomMaterialOwner() = 0;
	CCustomMaterial* GetCustomMaterial(int nIndex = 0) const; // returns NULL if the index is out of range
	virtual void SetCustomMaterial(CCustomMaterial* pCustomMaterial, int nIndex = 0) = 0;	// either replaces and existing material (releasing the old one), or adds one to the vector
	bool IsCustomMaterialValid(int nIndex = 0) const;
	int GetCustomMaterialCount() const { return m_ppCustomMaterials.Count(); }
	void ClearCustomMaterials(bool bPurge = false);
	virtual void OnCustomMaterialsUpdated() {};
	virtual void DuplicateCustomMaterialsToOther(CCustomMaterialOwner* pOther) const;

	// Pointers to custom materials owned by the mat system for this entity. Index
	// in this vector corresponds to the model material index to override with the custom material.
	CUtlVector< CCustomMaterial* > m_ppCustomMaterials;
};

inline CCustomMaterial* CCustomMaterialOwner::GetCustomMaterial(int nIndex) const
{
	return (m_ppCustomMaterials.Count() > nIndex) ? m_ppCustomMaterials[nIndex] : nullptr;
}

//inline void CCustomMaterialOwner::SetCustomMaterial(CCustomMaterial* pCustomMaterial, int nIndex)
//{
//	while (m_ppCustomMaterials.Count() <= nIndex)
//	{
//		m_ppCustomMaterials.AddToTail(NULL);
//	}
//
//	pCustomMaterial->Add();
//	if (m_ppCustomMaterials[nIndex])
//		m_ppCustomMaterials[nIndex]->Release();
//
//	m_ppCustomMaterials[nIndex] = pCustomMaterial;
//}

inline bool CCustomMaterialOwner::IsCustomMaterialValid(int nIndex) const
{
	return (m_ppCustomMaterials.Count() > nIndex && m_ppCustomMaterials[nIndex] != nullptr) ? m_ppCustomMaterials[nIndex]->IsValid() : false;
}

inline void CCustomMaterialOwner::ClearCustomMaterials(bool bPurge)
{
	for (int i = 0; i < m_ppCustomMaterials.Count(); i++)
	{
		if (m_ppCustomMaterials[i] != nullptr)
		{
			m_ppCustomMaterials[i]->Release();
			m_ppCustomMaterials[i] = nullptr;
		}
	}

	if (bPurge)
	{
		m_ppCustomMaterials.Purge();
	}
	else
	{
		m_ppCustomMaterials.RemoveAll();
	}
}

inline void CCustomMaterialOwner::DuplicateCustomMaterialsToOther(CCustomMaterialOwner* pOther) const
{
	pOther->ClearCustomMaterials();
	for (int i = 0; i < CCustomMaterialOwner::m_ppCustomMaterials.Count(); i++)
	{
		if (m_ppCustomMaterials[i] == NULL)
			continue;

		pOther->SetCustomMaterial(m_ppCustomMaterials[i], i);
	}
}

class CCustomMaterialManager
{
public:
	virtual bool Process() = 0;//0
	virtual CCustomMaterial* GetOrCreateCustomMaterial(KeyValues* pKeyValues, const CUtlVector< SCompositeTextureInfo >& vecTextureInfos, bool bIgnorePicMip = false) = 0; //unk usage
	virtual int DebugGetNumActiveCustomMaterials() = 0;
	virtual bool GetVMTKeyValues(const char* name, KeyValues* res) = 0;
	virtual ~CCustomMaterialManager() = 0;

	CConCommandMemberAccessor< CCustomMaterialManager > m_ReloadAllMaterials;
	CConCommandMemberAccessor< CCustomMaterialManager > m_Usage;
	CConCommandMemberAccessor< CCustomMaterialManager > m_ReloadVmtCache;
	CUtlVector< CCustomMaterial* > m_pCustomMaterials;
	//UtlMap< const char*, KeyValues* > m_mapVMTKeyValues;
};

class IMaterialSystem
{
public:
	auto GetBackBufferFormat() -> ImageFormat {
		using Fn = ImageFormat(__thiscall*)(void*);
		return call_virtual< Fn >(this, 36)(this);
	}

	auto CreateMaterial(const char* name, KeyValues* key) -> IMaterial* {
		using Fn = IMaterial * (__thiscall*)(void*, const char*, KeyValues*);
		return call_virtual< Fn >(this, 83)(this, name, key);
	}

	auto FindMaterial(const char* name, const char* texture_group_name, bool complain = true, const char* complain_prefix = nullptr) -> IMaterial* {
		using Fn = IMaterial * (__thiscall*)(void*, const char*, const char*, bool, const char*);
		return call_virtual< Fn >(this, 84)(this, name, texture_group_name, complain, complain_prefix);
	}

	auto FirstMaterial() -> MaterialHandle_t {
		using Fn = MaterialHandle_t(__thiscall *)(void *);
		return call_virtual< Fn >(this, 86)(this);
	}

	auto NextMaterial(MaterialHandle_t h) -> MaterialHandle_t {
		using Fn = MaterialHandle_t(__thiscall *)(void *, MaterialHandle_t);
		return call_virtual< Fn >(this, 87)(this, h);
	}

	auto InvalidMaterial() -> MaterialHandle_t {
		using Fn = MaterialHandle_t(__thiscall *)(void *);
		return call_virtual< Fn >(this, 88)(this);
	}

	auto GetMaterial(MaterialHandle_t h) -> IMaterial * {
		using Fn = IMaterial * (__thiscall*)(void *, MaterialHandle_t);
		return call_virtual< Fn >(this, 89)(this, h);
	}

	auto BeginRenderTargetAllocation() -> void {
		using Fn = void(__thiscall*)(void*);
		return call_virtual< Fn >(this, 94)(this);
	}

	auto EndRenderTargetAllocation() -> void {
		using Fn = void(__thiscall*)(void*);
		return call_virtual< Fn >(this, 95)(this);
	}

	auto CreateNamedRenderTargetTextureEx(const char* name, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth, unsigned int textureFlags, unsigned int renderTargetFlags) -> ITexture* {
		using Fn = ITexture * (__thiscall*)(void*, const char*, int, int, RenderTargetSizeMode_t, ImageFormat, MaterialRenderTargetDepth_t, unsigned int, unsigned int);
		return call_virtual< Fn >(this, 97)(this, name, w, h, sizeMode, format, depth, textureFlags, renderTargetFlags);
	}
	ITexture* CreateNamedRenderTargetTextureEx2(const char* szName, int iWidth, int iHeight, RenderTargetSizeMode_t sizeMode, EImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int fTextureFlags = 0U, unsigned int fRenderTargetFlags = CREATERENDERTARGETFLAGS_HDR)
	{
		using Fn = ITexture * (__thiscall*)(void*, const char*, int, int, RenderTargetSizeMode_t, ImageFormat, MaterialRenderTargetDepth_t, unsigned int, unsigned int);
		return call_virtual< Fn >(this, 99)(this, szName, iWidth, iHeight, sizeMode, format, depth, fTextureFlags, fRenderTargetFlags);

	}

	IMaterial* FindProceduralMaterial(const char* pMaterialName, const char* pTextureGroupName, KeyValues* pVMTKeyValues)
	{
		return call_virtual<IMaterial* (__thiscall*)(void*, const char*, const char*, KeyValues*)>(this, 124)(this, pMaterialName, pTextureGroupName, pVMTKeyValues);
	}
	auto GetRenderContext() -> IMatRenderContext* {
		using Fn = IMatRenderContext * (__thiscall*)(void*);
		return call_virtual< Fn >(this, 115)(this);
	}
	int GetNumMaterials()
	{
		using Fn = int  (__thiscall*)(void*);
		return call_virtual< Fn >(this, 90)(this);
	}
	auto FindTexture(char const* szTextureName, const char* szTextureGroupName, bool bComplain = true, int nAdditionalCreationFlags = 0) -> ITexture* {
		using Fn = ITexture*(__thiscall*)(void*, const char *, const char*,bool,int);
		return call_virtual<Fn>(this, 91)(this,szTextureName, szTextureGroupName, bComplain, nAdditionalCreationFlags);
	}
	void FinishRenderTargetAllocation()
	{
		using Fn = void * (__thiscall*)(void*);
		call_virtual<Fn>(this, 136)(this);
	}
	CCustomMaterialManager* GetCustomMaterialManager()
	{
		using Fn = CCustomMaterialManager* (__thiscall* )(void*);
		return call_virtual<Fn>(this, 152)(this);
	}
	void ReEnableRenderTargetAllocation()
	{
		using Fn = void* (__thiscall*)(void*);
		call_virtual<Fn>(this, 137)(this);
	}
	void ForceBeginRenderTargetAllocation()
	{
		const bool bOldState = DisableRenderTargetAllocationForever();

		DisableRenderTargetAllocationForever() = false;
		BeginRenderTargetAllocation();
		DisableRenderTargetAllocationForever() = bOldState;
	}
	void ForceEndRenderTargetAllocation()
	{
		const bool bOldState = DisableRenderTargetAllocationForever();

		DisableRenderTargetAllocationForever() = false;
		EndRenderTargetAllocation();
		DisableRenderTargetAllocationForever() = bOldState;
	}

	bool& DisableRenderTargetAllocationForever();
};

class CMatRenderContextPtr : public CRefPtr<IMatRenderContext>
{
	typedef CRefPtr<IMatRenderContext> CBaseClass;
public:
	CMatRenderContextPtr() = default;

	CMatRenderContextPtr(IMatRenderContext* pInit) : CBaseClass(pInit)
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->BeginRender();
	}

	CMatRenderContextPtr(IMaterialSystem* pFrom) : CBaseClass(pFrom->GetRenderContext())
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->BeginRender();
	}

	~CMatRenderContextPtr()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->EndRender();
	}

	IMatRenderContext* operator=(IMatRenderContext* pSecondContext)
	{
		if (pSecondContext != nullptr)
			pSecondContext->BeginRender();

		return CBaseClass::operator=(pSecondContext);
	}

	void SafeRelease()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->EndRender();

		CBaseClass::SafeRelease();
	}

	void AssignAddReference(IMatRenderContext* pFrom)
	{
		if (CBaseClass::pObject)
			CBaseClass::pObject->EndRender();

		CBaseClass::AssignAddReference(pFrom);
		CBaseClass::pObject->BeginRender();
	}

	void GetFrom(IMaterialSystem* pFrom)
	{
		AssignAddReference(pFrom->GetRenderContext());
	}


private:
	CMatRenderContextPtr(const CMatRenderContextPtr& pRefPtr);
	void operator=(const CMatRenderContextPtr& pSecondRefPtr);
};