#pragma once

#include "../math/VMatrix.hpp"
#include "../misc/Studio.hpp"

#define VIRTUAL(function_name, index, type, ...) \
auto function_name { \
return call_virtual <type> (this, index)(this, __VA_ARGS__); \
};

enum PreviewImageRetVal_t
{
    MATERIAL_PREVIEW_IMAGE_BAD = 0,
    MATERIAL_PREVIEW_IMAGE_OK,
    MATERIAL_NO_PREVIEW_IMAGE,
};

enum MaterialVarFlags_t
{
    MATERIAL_VAR_DEBUG = (1 << 0),
    MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
    MATERIAL_VAR_NO_DRAW = (1 << 2),
    MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
    MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
    MATERIAL_VAR_VERTEXALPHA = (1 << 5),
    MATERIAL_VAR_SELFILLUM = (1 << 6),
    MATERIAL_VAR_ADDITIVE = (1 << 7),
    MATERIAL_VAR_ALPHATEST = (1 << 8),
    MATERIAL_VAR_ZNEARER = (1 << 10),
    MATERIAL_VAR_MODEL = (1 << 11),
    MATERIAL_VAR_FLAT = (1 << 12),
    MATERIAL_VAR_NOCULL = (1 << 13),
    MATERIAL_VAR_NOFOG = (1 << 14),
    MATERIAL_VAR_IGNOREZ = (1 << 15),
    MATERIAL_VAR_DECAL = (1 << 16),
    MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
    MATERIAL_VAR_UNUSED = (1 << 18), // UNUSED
    MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
    MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
    MATERIAL_VAR_TRANSLUCENT = (1 << 21),
    MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
    MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
    MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
    MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
    MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
    MATERIAL_VAR_HALFLAMBERT = (1 << 27),
    MATERIAL_VAR_WIREFRAME = (1 << 28),
    MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
    MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
    MATERIAL_VAR_VERTEXFOG = (1 << 31),
};

typedef unsigned short ModelInstanceHandle_t;
typedef void* LightCacheHandle_t;
typedef void* StudioDecalHandle_t;
typedef int OverrideType_t;
typedef int ImageFormat;
typedef int VertexFormat_t;
typedef int MaterialPropertyTypes_t;

class IClientRenderable;
class IMaterial;
class IMatRenderContext;
class DataCacheHandle_t;
class ITexture;
class KeyValues;
struct model_t;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct Ray_t;
struct DrawModelInfo_t;
//struct studiohwdata_t;
struct MaterialLightingState_t;
struct ColorMeshInfo_t;
struct CStudioHdr;
struct virtualmodel_t;

class IMaterial;

struct studioloddata_t
{
    // not needed - this is really the same as studiohwdata_t.m_NumStudioMeshes
    //int					m_NumMeshes; 
    void*               m_pMeshData; // there are studiohwdata_t.m_NumStudioMeshes of these.
    float				m_SwitchPoint;
    int					numMaterials;
    IMaterial**         ppMaterials; /* will have studiohdr_t.numtextures elements allocated */
    int*                pMaterialFlags; /* will have studiohdr_t.numtextures elements allocated */
    int*                m_pHWMorphDecalBoneRemap;
    int					m_nDecalBoneCount;
};

struct studiohwdata_t
{
    int					m_RootLOD;	// calced and clamped, nonzero for lod culling
    int					m_NumLODs;
    studioloddata_t*    m_pLODs;
    int					m_NumStudioMeshes;

    inline float LODMetric(float unitSphereSize) const { return (unitSphereSize != 0.0f) ? (100.0f / unitSphereSize) : 0.0f; }
    inline int GetLODForMetric(float lodMetric) const
    {
        if (!m_NumLODs)
            return 0;

        // shadow lod is specified on the last lod with a negative switch
        // never consider shadow lod as viable candidate
        int numLODs = (m_pLODs[m_NumLODs - 1].m_SwitchPoint < 0.0f) ? m_NumLODs - 1 : m_NumLODs;

        for (int i = m_RootLOD; i < numLODs - 1; i++)
        {
            if (m_pLODs[i + 1].m_SwitchPoint > lodMetric)
                return i;
        }

        return numLODs - 1;
    }
};
struct DrawModelState_t
{
    studiohdr_t*            m_pStudioHdr;
    studiohwdata_t*         m_pStudioHWData;
    IClientRenderable*      m_pRenderable;
    const matrix3x4_t*      m_pModelToWorld;
    StudioDecalHandle_t     m_decals;
    int                     m_drawFlags;
    int                     m_lod;
};

struct StaticPropRenderInfo_t
{
    const matrix3x4_t*      pModelToWorld;
    const model_t*          pModel;
    IClientRenderable*      pRenderable;
    Vector*                 pLightingOrigin;
    short                   skin;
    ModelInstanceHandle_t   instance;
};

struct ModelRenderInfo_t
{
    Vector                  origin;
    Vector                  angles;
	char pad[ 0x4 ];
    IClientRenderable*      pRenderable;
    const model_t*          pModel;
    const matrix3x4_t*      pModelToWorld;
    const matrix3x4_t*      pLightingOffset;
    const Vector*           pLightingOrigin;
    int                     flags;
    int                     entity_index;
    int                     skin;
    int                     body;
    int                     hitboxset;
    ModelInstanceHandle_t   instance;

    ModelRenderInfo_t() //-V730
    {
        pModelToWorld = NULL;
        pLightingOffset = NULL;
        pLightingOrigin = NULL;
    }
};

struct CStudioHdr
{
    mutable /*const*/ studiohdr_t* m_pStudioHdr;
    mutable virtualmodel_t* m_pVModel;
    mutable CUtlVector<const studiohdr_t*> m_pStudioHdrCache;
    int m_nFrameUnlockCounter;
    int* m_pFrameUnlockCounter;
    char  m_FrameUnlockCounterMutex[0x8];
    int unknown;
    CUtlVector<int>    m_boneFlags; //48
    CUtlVector<int>    m_boneParent;
};

struct LightingQuery_t
{
    Vector                  m_LightingOrigin;
    ModelInstanceHandle_t   m_InstanceHandle;
    bool                    m_bAmbientBoost;
};

struct StaticLightingQuery_t : public LightingQuery_t
{
    IClientRenderable*        m_pRenderable;
};

class IMaterialVar 
{
public:
    VIRTUAL(set_float_value(float val), 4, void(__thiscall*)(void*, float), val);
    VIRTUAL(set_int_value(int val), 5, void(__thiscall*)(void*, int), val);
    VIRTUAL(set_string_value(const char* val), 6, void(__thiscall*)(void*, const char*), val);
    VIRTUAL(set_vec_value(float x, float y, float z), 11, void(__thiscall*)(void*, float, float, float), x, y, z);
};

class IMaterial
{
public:
    virtual const char* GetName() const = 0; 
    virtual const char* GetTextureGroupName() const = 0;
    virtual PreviewImageRetVal_t GetPreviewImageProperties(int* width, int* height, ImageFormat* imageFormat, bool* isTranslucent) const = 0;
    virtual PreviewImageRetVal_t GetPreviewImage(unsigned char* data, int width, int height, ImageFormat imageFormat) const = 0;
    virtual int GetMappingWidth() = 0;
    virtual int GetMappingHeight() = 0;
    virtual int GetNumAnimationFrames() = 0;
    virtual bool InMaterialPage(void) = 0;
    virtual void GetMaterialOffset(float* pOffset) = 0;
    virtual void GetMaterialScale(float* pScale) = 0;
    virtual IMaterial* GetMaterialPage(void) = 0;
    virtual IMaterialVar* FindVar(const char* varName, bool* found, bool complain = true) = 0;
    virtual void IncrementReferenceCount(void) = 0;
    virtual void DecrementReferenceCount(void) = 0;
    inline void AddRef()
    {
        IncrementReferenceCount();
    }
    inline void Release()
    {
        DecrementReferenceCount();
    }
    virtual int GetEnumerationID(void) const = 0;
    virtual void GetLowResColorSample(float s, float t, float* color) const = 0;
    virtual void RecomputeStateSnapshots() = 0;
    virtual bool IsTranslucent() = 0;
    virtual bool IsAlphaTested() = 0;
    virtual bool IsVertexLit() = 0;
    virtual VertexFormat_t GetVertexFormat() const = 0;
    virtual bool HasProxy(void) const = 0;
    virtual bool UsesEnvCubemap(void) = 0;
    virtual bool NeedsTangentSpace(void) = 0;
    virtual bool NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
    virtual bool NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
    virtual bool NeedsSoftwareSkinning(void) = 0;
    virtual void AlphaModulate(float alpha) = 0;
    virtual void ColorModulate(float r, float g, float b) = 0;
    virtual void SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
    virtual bool GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;
    virtual void GetReflectivity(Vector& reflect) = 0;
    virtual bool GetPropertyFlag(MaterialPropertyTypes_t type) = 0;
    virtual bool IsTwoSided() = 0;
    virtual void SetShader(const char* pShaderName) = 0;
    virtual int GetNumPasses(void) = 0;
    virtual int GetTextureMemoryBytes(void) = 0;
    virtual void Refresh() = 0;

    // GR - returns true is material uses lightmap alpha for blending
    virtual bool NeedsLightmapBlendAlpha(void) = 0;

    // returns true if the shader doesn't do lighting itself and requires
    // the data that is sent to it to be prelighted
    virtual bool NeedsSoftwareLighting(void) = 0;

    // Gets at the shader parameters
    virtual int ShaderParamCount() const = 0;

    virtual IMaterialVar** GetShaderParams(void) = 0;

    // Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
    // the material can't be found.
    virtual bool IsErrorMaterial() const = 0;

    virtual void Unused() = 0;

    // Gets the current alpha modulation
    virtual float GetAlphaModulation() = 0;

    virtual void GetColorModulation(float* r, float* g, float* b) = 0;

    // Is this translucent given a particular alpha modulation?
    virtual bool IsTranslucentUnderModulation(float fAlphaModulation = 1.0f) const = 0;

    // fast find that stores the index of the found var in the string table in local cache
    virtual IMaterialVar* FindVarFast(char const* pVarName, unsigned int* pToken) = 0;

    // Sets new VMT shader parameters for the material
    virtual void SetShaderAndParams(KeyValues* pKeyValues) = 0;

    virtual const char* GetShaderName() const = 0;

    virtual void DeleteIfUnreferenced() = 0;

    virtual bool IsSpriteCard() = 0;

    virtual void CallBindProxy(void* proxyData) = 0;

    virtual void RefreshPreservingMaterialVars() = 0;

    virtual bool WasReloadedFromWhitelist() = 0;

    virtual bool SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;

    virtual int GetReferenceCount() const = 0;
};

class IVModelRender
{
public:
    virtual int                     DrawModel(int flags, IClientRenderable *pRenderable, ModelInstanceHandle_t instance, int entity_index, const model_t *model, Vector const& origin, Vector const& angles, int skin, int body, int hitboxset, const matrix3x4_t *modelToWorld = NULL, const matrix3x4_t *pLightingOffset = NULL) = 0;
    virtual void                    ForcedMaterialOverride(IMaterial *newMaterial, OverrideType_t nOverrideType = 0, int nOverrides = 0) = 0;
    virtual bool                    IsForcedMaterialOverride(void) = 0;
    virtual void                    SetViewTarget(const CStudioHdr *pStudioHdr, int nBodyIndex, const Vector& target) = 0;
    virtual ModelInstanceHandle_t   CreateInstance(IClientRenderable *pRenderable, LightCacheHandle_t *pCache = NULL) = 0;
    virtual void                    DestroyInstance(ModelInstanceHandle_t handle) = 0;
    virtual void                    SetStaticLighting(ModelInstanceHandle_t handle, LightCacheHandle_t* pHandle) = 0;
    virtual LightCacheHandle_t      GetStaticLighting(ModelInstanceHandle_t handle) = 0;
    virtual bool                    ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable *pRenderable) = 0;
    virtual void                    AddDecal(ModelInstanceHandle_t handle, Ray_t const& ray, Vector const& decalUp, int decalIndex, int body, bool noPokeThru, int maxLODToDecal) = 0;
    virtual void                    RemoveAllDecals(ModelInstanceHandle_t handle) = 0;
    virtual bool                    ModelHasDecals(ModelInstanceHandle_t handle) = 0;
    virtual void                    RemoveAllDecalsFromAllModels() = 0;
    virtual matrix3x4_t*            DrawModelShadowSetup(IClientRenderable *pRenderable, int body, int skin, DrawModelInfo_t *pInfo, matrix3x4_t *pCustomBoneToWorld = NULL) = 0;
    virtual void                    DrawModelShadow(IClientRenderable *pRenderable, const DrawModelInfo_t &info, matrix3x4_t *pCustomBoneToWorld = NULL) = 0;
    virtual bool                    RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;
    virtual void                    ReleaseAllStaticPropColorData(void) = 0;
    virtual void                    RestoreAllStaticPropColorData(void) = 0;
    virtual int                     DrawModelEx(ModelRenderInfo_t &pInfo) = 0;
    virtual int                     DrawModelExStaticProp(ModelRenderInfo_t &pInfo) = 0;
    virtual bool                    DrawModelSetup(ModelRenderInfo_t &pInfo, DrawModelState_t *pState, matrix3x4_t **ppBoneToWorldOut) = 0;
    virtual void                    DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
    virtual void                    SetupLighting(const Vector &vecCenter) = 0;
    virtual int                     DrawStaticPropArrayFast(StaticPropRenderInfo_t *pProps, int count, bool bShadowDepth) = 0;
    //virtual void                    SuppressEngineLighting(bool bSuppress) = 0;
    void SuppressEngineLighting(bool bSuppress)
    {
        using Fn = void* (__thiscall*)(void*,bool);
        call_virtual<Fn>(this, 24)(this, bSuppress);
    }


    virtual void                    SetupColorMeshes(int nTotalVerts) = 0;
    virtual void                    SetupLightingEx(const Vector &vecCenter, ModelInstanceHandle_t handle) = 0;
    virtual bool                    GetBrightestShadowingLightSource(const Vector &vecCenter, Vector& lightPos, Vector& lightBrightness, bool bAllowNonTaggedLights) = 0;
    virtual void                    ComputeLightingState(int nCount, const LightingQuery_t *pQuery, MaterialLightingState_t *pState, ITexture **ppEnvCubemapTexture) = 0;
    virtual void                    GetModelDecalHandles(StudioDecalHandle_t *pDecals, int nDecalStride, int nCount, const ModelInstanceHandle_t *pHandles) = 0;
    virtual void                    ComputeStaticLightingState(int nCount, const StaticLightingQuery_t *pQuery, MaterialLightingState_t *pState, MaterialLightingState_t *pDecalState, ColorMeshInfo_t **ppStaticLighting, ITexture **ppEnvCubemapTexture, DataCacheHandle_t *pColorMeshHandles) = 0;
    virtual void                    CleanupStaticLightingState(int nCount, DataCacheHandle_t *pColorMeshHandles) = 0;
};