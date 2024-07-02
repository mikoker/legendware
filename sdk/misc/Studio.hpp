#pragma once

#include "../math/Vector.hpp"
#include "../math/VMatrix.hpp"

typedef float RadianEuler[3];

#define MAX_QPATH 260

#define BONE_CALCULATE_MASK             0x1F
#define BONE_PHYSICALLY_SIMULATED       0x01    // bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL         0x02    // procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL          0x04    // bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE        0x08    // bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER      0x10    // bone aligns to the screen, constrained by it's own axis.

#define BONE_USED_BY_ANYTHING           0x0007FF00
#define BONE_USED_BY_HITBOX             0x00000100    // bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT         0x00000200    // bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK        0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0        0x00000400    // bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1        0x00000800    
#define BONE_USED_BY_VERTEX_LOD2        0x00001000  
#define BONE_USED_BY_VERTEX_LOD3        0x00002000
#define BONE_USED_BY_VERTEX_LOD4        0x00004000
#define BONE_USED_BY_VERTEX_LOD5        0x00008000
#define BONE_USED_BY_VERTEX_LOD6        0x00010000
#define BONE_USED_BY_VERTEX_LOD7        0x00020000
#define BONE_USED_BY_BONE_MERGE         0x00040000    // bone is available for bone merge to occur against it

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS 8
#define MAXSTUDIOBONES		128		// total bones actually used

#define BONE_TYPE_MASK                  0x00F00000
#define BONE_FIXED_ALIGNMENT            0x00100000    // bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

#define BONE_HAS_SAVEFRAME_POS          0x00200000    // Vector48
#define BONE_HAS_SAVEFRAME_ROT64        0x00400000    // Quaternion64
#define BONE_HAS_SAVEFRAME_ROT32        0x00800000    // Quaternion32

#define Assert( _exp ) ((void)0)

#define HITGROUP_GENERIC 0
#define HITGROUP_HEAD 1
#define HITGROUP_CHEST 2
#define HITGROUP_STOMACH 3
#define HITGROUP_LEFTARM 4    
#define HITGROUP_RIGHTARM 5
#define HITGROUP_LEFTLEG 6
#define HITGROUP_RIGHTLEG 7
#define HITGROUP_GEAR 8

enum modtype_t
{
    mod_bad = 0,
    mod_brush,
    mod_sprite,
    mod_studio
};

enum Hitboxes
{
    HITBOX_HEAD,
    HITBOX_NECK,
    HITBOX_PELVIS,
    HITBOX_STOMACH,
    HITBOX_LOWER_CHEST,
    HITBOX_CHEST,
    HITBOX_UPPER_CHEST,
    HITBOX_RIGHT_THIGH,
    HITBOX_LEFT_THIGH,
    HITBOX_RIGHT_CALF,
    HITBOX_LEFT_CALF,
    HITBOX_RIGHT_FOOT,
    HITBOX_LEFT_FOOT,
    HITBOX_RIGHT_HAND,
    HITBOX_LEFT_HAND,
    HITBOX_RIGHT_UPPER_ARM,
    HITBOX_RIGHT_FOREARM,
    HITBOX_LEFT_UPPER_ARM,
    HITBOX_LEFT_FOREARM,
    HITBOX_MAX
};

typedef unsigned short MDLHandle_t;
struct studiohdr_t;

struct virtualgroup_t
{
	void* pCache; // 0x00
	CUtlVector<int> vecBoneMap; // 0x04 // maps global bone to local bone
	CUtlVector<int> vecMasterBone; // 0x18 // maps local bone to global bone
	CUtlVector<int> vecMasterSequence; // 0x2C // maps local sequence to master sequence
	CUtlVector<int> vecMasterAnim; // 0x40 // maps local animation to master animation
	CUtlVector<int> vecMasterAttachment; // 0x54 // maps local attachment to global
	CUtlVector<int> vecMasterPose; // 0x68 // maps local pose parameter to global
	CUtlVector<int> vecMasterNode; // 0x7C // maps local transition nodes to global
};
static_assert(sizeof(virtualgroup_t) == 0x90);

struct virtualsequence_t
{
	int  nFlags; // 0x00
	int iActivity; // 0x04
	int iGroup; // 0x08
	int nIndex; // 0x0C
};
static_assert(sizeof(virtualsequence_t) == 0x10);

struct virtualgeneric_t
{
	int iGroup; // 0x00
	int nIndex; // 0x04
};
static_assert(sizeof(virtualgeneric_t) == 0x8);


struct virtualmodel_t
{
	// note: user must manage mutex for this
	inline virtualgroup_t* GetAnimationGroup(const int iAnimation)
	{
		return &vecGroup[vecAnimation[iAnimation].iGroup];
	}

	// note: user must manage mutex for this
	inline virtualgroup_t* GetSequenceGroup(const int iSequence)
	{
		return &vecGroup[vecSequence[iSequence].iGroup];
	}

	std::byte pad0[0x8]; // 0x00 // CThreadFastMutex
	CUtlVector<virtualsequence_t> vecSequence; // 0x08
	CUtlVector<virtualgeneric_t> vecAnimation; // 0x1C
	CUtlVector<virtualgeneric_t> vecAttachment; // 0x30
	CUtlVector<virtualgeneric_t> vecPose; // 0x44
	CUtlVector<virtualgroup_t> vecGroup; // 0x58
	CUtlVector<virtualgeneric_t> vecNode; // 0x6C
	CUtlVector<virtualgeneric_t> vecIKLock; // 0x80
	CUtlVector<unsigned short> vecAutoplaySequences; // 0x94
};
struct mstudioseqdesc_t
{
	int nBaseStudio;
	inline studiohdr_t* GetStudioHdr() const
	{
		return reinterpret_cast<studiohdr_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + nBaseStudio));
	}

	int  iLabelOffset;
	inline const char* GetLabel() const
	{
		return reinterpret_cast<const char*>(this) + iLabelOffset;
	}

	int  iActivityNameOffset;
	inline const char* GetActivityName() const
	{
		return reinterpret_cast<const char*>(this) + iActivityNameOffset;
	}

	int  nFlags;

	int  iActivity;
	int  iActivityWeight;

	int nEventCount;
	int iEventOffset;
	inline void* GetEvent(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nEventCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iEventOffset) + nIndex;
	}

	Vector vecBBMin;
	Vector vecBBMax;

	int  nBlendCount;
	int  iBlendOffset;

	inline int GetAnimValue(int x, int y) const
	{
		if (x >= iGroupSize[0])
			x = iGroupSize[0] - 1;

		if (y >= iGroupSize[1])
			y = iGroupSize[1] - 1;

		const int iOffset = y * iGroupSize[0] + x;
		const short* arrBlends = reinterpret_cast<short*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iBlendOffset));
		return arrBlends[iOffset];
	}

	int  iMovementOffset;    // [blend] float array for blended movement
	int iGroupSize[2];
	int nParamIndex[2];    // X, Y, Z, XR, YR, ZR
	float flParamStart[2];  // local (0..1) starting value
	float flParamEnd[2];  // local (0..1) ending value
	int  iParamParent;

	float flFadeInTime;    // ideal cross fate in time (0.2 default)
	float flFadeOutTime;  // ideal cross fade out time (0.2 default)

	int  iEntryNode;      // transition node at entry
	int  iExitNode;      // transition node at exit
	int  iNodeFlags;      // transition rules

	float flEntryPhase;    // used to match entry gait
	float flExitPhase;    // used to match exit gait

	float flLastFrame;    // frame that should generation EndOfSequence

	int  iNextSequence;    // auto advancing sequences
	int  iPose;        // index of delta animation between end and nextseq

	int  nIKRuleCount; int  nAutoLayerCount;
	int  iAutoLayerOffset;
	inline void* GetAutoLayer(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nAutoLayerCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iAutoLayerOffset) + nIndex;
	}

	int  iWeightListOffset;
	inline float* GetBoneWeight(int nIndex) const
	{
		return reinterpret_cast<float*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iWeightListOffset)) + nIndex;
	}

	int iPoseKeyOffset;
	inline float* GetPoseKey(int iParameter, int iAnimation) const
	{
		return reinterpret_cast<float*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iPoseKeyOffset)) + iParameter * iGroupSize[0] + iAnimation;
	}

	int  nIKLockCount;
	int  iIKLockOffset;
	inline void* GetIKLock(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nIKLockCount)
			return nullptr;

		return const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + iIKLockOffset) + nIndex;
	}

	int  iKeyValueOffset;
	int  nKeyValueSize;
	inline const char* KeyValueText() const
	{
		if (nKeyValueSize == 0)
			return nullptr;

		return reinterpret_cast<const char*>(this) + iKeyValueOffset;
	}

	int  iCyclePoseOffset;

	int iActivityModifierOffset;
	int  nActivityModifierCount;

	int  iAnimTagOffset;
	int  nAnimTagCount;

	int  iRootDriverOffset;
	std::byte unused0[0x8];
};
struct mstudiobone_t
{
	int sznameindex;
	inline char *const pszName(void) const { return ((char *)this) + sznameindex; }
	int parent;
	int bonecontroller[6];

	Vector pos;
	Quaternion quat;
	RadianEuler rot;

	Vector posscale;
	Vector rotscale;

	matrix3x4_t poseToBone;
	Quaternion qAlignment;
	int flags;
	int proctype;
	int procindex;
	mutable int physicsbone;
	inline void *pProcedure() const { if (procindex == 0) return NULL; else return  (void *)((( unsigned char *)this) + procindex); };
	int surfacepropidx;
	inline char *const pszSurfaceProp(void) const { return ((char *)this) + surfacepropidx; }
	inline int GetSurfaceProp(void) const { return surfacepropLookup; }

	int contents;
	int surfacepropLookup;
	int unused[7];
};


struct mstudiobbox_t
{
	int		bone;
	int		group;
	Vector	bbmin;
	Vector	bbmax;
	int		hitboxnameindex;
	Vector	rotation;
	float	radius;
	int		pad2[4];
};

struct mstudiohitboxset_t
{
	int    sznameindex;
	int    numhitboxes;
	int    hitboxindex;

	inline char *const pszName(void) const
	{
		return ((char*)this) + sznameindex;
	}

	inline mstudiobbox_t *pHitbox(int i) const
	{
		return (mstudiobbox_t*)((( unsigned char*)this) + hitboxindex) + i;
	}
};

struct mstudioposeparamdesc_t
{
	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int					flags;	// ????
	float				start;	// starting value
	float				end;	// ending value
	float				loop;	// looping range, 0 for no looping, 360 for rotations, etc.
};

struct model_t
{
	void* fnHandle;
	char name[260];
	__int32 nLoadFlags;
	__int32 nServerCount;
	__int32 type;
	__int32 flags;
	Vector mins, maxs;
	float radius;
	void* m_pKeyValues;
	union
	{
		void* brush;
		MDLHandle_t studio;
		void* sprite;
	};
};

class studiohdr_t
{
public:
	__int32 id;                     //0x0000 
	__int32 version;                //0x0004 
	long    checksum;               //0x0008 
	char    szName[64];             //0x000C 
	__int32 length;                 //0x004C 
	Vector  vecEyePos;              //0x0050 
	Vector  vecIllumPos;            //0x005C 
	Vector  vecHullMin;             //0x0068 
	Vector  vecHullMax;             //0x0074 
	Vector  vecBBMin;               //0x0080 
	Vector  vecBBMax;               //0x008C 
	__int32 flags;                  //0x0098 
	__int32 numbones;               //0x009C 
	__int32 boneindex;              //0x00A0 
	__int32 numbonecontrollers;     //0x00A4 
	__int32 bonecontrollerindex;    //0x00A8 
	__int32 numhitboxsets;          //0x00AC 
	__int32 hitboxsetindex;         //0x00B0 
	__int32 numlocalanim;           //0x00B4 
	__int32 localanimindex;         //0x00B8 
	__int32 numlocalseq;            //0x00BC 
	__int32 localseqindex;          //0x00C0 
	__int32 activitylistversion;    //0x00C4 
	__int32 evenstsindexed;          //0x00C8 
	__int32 numtextures;            //0x00CC 
	__int32 textureindex;           //0x00D0
	__int32	numcdtextures;
	__int32	cdtextureindex;
	__int32	numskinref;
	__int32	numskinfamilies;
	__int32	skinindex;
	__int32	numbodyparts;
	__int32	bodypartindex;
	__int32	numlocalattachments;
	__int32	localattachmentindex;
	__int32	numlocalnodes;
	__int32	localnodeindex;
	__int32	localnodenameindex;
	__int32	numflexdesc;
	__int32	flexdescindex;
	__int32	numflexcontrollers;
	__int32	flexcontrollerindex;
	__int32	numflexrules;
	__int32	flexruleindex;
	__int32	numikchains;
	__int32	ikchainindex;
	__int32	nummouths;
	__int32	mouthindex;
	__int32	numlocalposeparameters;
	__int32	localposeparamindex;
	__int32	surfacepropindex;
	__int32	keyvalueindex;
	__int32	keyvaluesize;
	__int32	numlocalikautoplaylocks;
	__int32	localikautoplaylockindex;
	float	mass;
	__int32 contents;
	__int32 numincludemodels; //  int  nIncludeModelCount; // 0x0150 // @ida: datacache.dll -> ["83 BF ? ? ? ? ? 75 07" + 0x2]
	__int32 includemodelindex; // int  iIncludeModelOffset;

	inline const char *pszName(void) const { return szName; }
	
	mstudioseqdesc_t* GetLocalSequenceDescription(int iSequence) const
	{
		if (iSequence < 0 || iSequence >= numlocalseq)
			iSequence = 0;

		return reinterpret_cast<mstudioseqdesc_t*>(const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this) + localseqindex)) + iSequence;
	}
	mstudioseqdesc_t& GetSequenceDescription(int iSequence, virtualmodel_t* pVirtualModel = nullptr, const studiohdr_t* pStudioHdr = nullptr) const
	{
		if (numincludemodels == 0 || pVirtualModel == nullptr || pStudioHdr == nullptr)
			return *GetLocalSequenceDescription(iSequence);

		return *pStudioHdr->GetLocalSequenceDescription(pVirtualModel->vecSequence[iSequence].nIndex);
	}

	mstudiohitboxset_t* pHitboxSet(int i)
	{
		if (i > numhitboxsets) return nullptr;
		return (mstudiohitboxset_t*)((uint8_t*)this + hitboxsetindex) + i;
	}

	mstudiobone_t* pBone(int i)
	{
		if (i > numbones) return nullptr;
		return (mstudiobone_t*)((uint8_t*)this + boneindex) + i;
	}
};

class c_studio_hdr
{
public:
	void* m_pStudioHdr; //0x0000 
	void* m_pVModel; //0x0004 
	char pad_unknown[0x4]; //0x0008 
	CUtlVector <const studiohdr_t*> m_pStudioHdrCache; //0x000C 
	int m_nFrameUnlockCounter; //0x0020 
	int* m_pFrameUnlockCounter; //0x0024 
	char pad_mutex[0x8]; //0x0028 
	CUtlVector <int> bone_flags; //0x0030
	CUtlVector <int> m_boneParent; //0x0050
};

