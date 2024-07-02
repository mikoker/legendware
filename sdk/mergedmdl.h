#pragma once
// used: matrix
#include "misc/datamap.h"
// used: vector
#include "math/Vector.hpp"
// used: utlvector
#include "misc/UtlVector.hpp"
// used: MDLHandle_t
#include "interfaces/IRenderView.hpp"
// used: texture class
#include "interfaces/ITexture.hpp"

#include "misc/UtlBuffer.hpp"
#include "interfaces/IRenderToRTHelper.hpp"




struct MDLSquenceLayer_t
{
	int		nSequenceIndex;
	float	flWeight;
};
class CMDLAttachmentData
{
public:
	matrix3x4_t m_AttachmentToWorld;
	bool m_bValid;
};
class CMDL
{
public:

	std::byte	pad[0x3C]; // 0x0000
	MDLHandle_t	m_MDL; // 0x003C
	std::byte	pad0[0x8]; // 0x003E
	std::array<std::uint8_t, 4U> Color; // 0x0046
	std::byte	pad1[0x2]; // 0x004A
	int			nSkin; // 0x004C
	int			nBody; // 0x0050
	int			nSequence; // 0x0054
	int			nLOD; // 0x0058
	float		flPlaybackRate; // 0x005C
	float		flTime; // 0x0060
	float		flCurrentAnimEndTime; // 0x0064
	float		arrFlexControls[96 * 4]; // 0x0068
	Vector		vecViewTarget; // 0x0668
	bool		bWorldSpaceViewTarget; // 0x0674
	bool		bUseSequencePlaybackFPS; // 0x0675
	std::byte	pad2[0x2]; // 0x0676
	void* pProxyData; // 0x0678
	float		flTimeBasisAdjustment; // 0x067C
	std::byte	pad3[0x4]; // 0x0680 --isn't correct after this point iirc
	CUtlVector<int> arrSequenceFollowLoop; // 0x0684
	matrix3x4_t	matModelToWorld; // 0x0698
	bool		bRequestBoneMergeTakeover; // 0x06C8



	CUtlVector< CMDLAttachmentData* >* GetAttachments()
	{
		return reinterpret_cast<CUtlVector< CMDLAttachmentData* >*>(std::uintptr_t(this) + 0x1C);
	}

	CUtlVector< CCustomMaterial* >* GetCustomMaterials()
	{
		return reinterpret_cast<CUtlVector< CCustomMaterial* >*>(std::uintptr_t(this) + 0x4);
	}
	int SetCustomMaterial(void* pCustomMaterial, int nIndex)
	{
		static const auto oSetCustomMaterial = reinterpret_cast<int(__thiscall*)(void*, void*, int)>(signatures_manager->find_signature("client.dll","55 8B EC 53 8B D9 8B 4D 0C"));
		return oSetCustomMaterial(this, pCustomMaterial, nIndex);
	}
#if 0
	void ClearCustomMaterials(bool bPurge = false)
	{
		static const auto ClearCustomMaterialsPtr = reinterpret_cast<void(__thiscall*)(void*, bool)>(signatures_manager->find_signature("client.dll", "55 8B EC 51 53 56 8B F1 33 DB 39 5E 10"));
		ClearCustomMaterialsPtr(this, bPurge);
	}
	void SetSimpleMaterialOverride(IMaterial* pNewMaterial) {
		static const auto SetSimpleMaterialOverridePtr = reinterpret_cast<void(__thiscall*)(void*, IMaterial*)>(signatures_manager->find_signature("client.dll", "55 8B EC 56 8B 75 08 57 8B F9 8B 4F 30 3B CE"));
		SetSimpleMaterialOverridePtr(this, pNewMaterial);
	}

	bool SetupBones(const matrix3x4_t& matShapeToWorld, int nMaxBoneCount, matrix3x4_t* pOutMatrix, const float* pPoseParameters = nullptr, void* pSequenceLayers = nullptr, int nSequenceLayers = 0)
	{
		static const std::uintptr_t uAddress = (signatures_manager->find_signature("client.dll", "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B 3D"));
		static const auto oSetupBones = reinterpret_cast<bool(__thiscall*)(void*, const matrix3x4_t&, int, matrix3x4_t*, const float*, void*, int)>(uAddress);

		if (oSetupBones != nullptr)
			return oSetupBones(this, matShapeToWorld, nMaxBoneCount, pOutMatrix, pPoseParameters, pSequenceLayers, nSequenceLayers);
	}
	bool GetAttachment(int Attachment, matrix3x4_t* matrixOut)
	{
		static const std::uintptr_t uAddress = (signatures_manager->find_signature("client.dll", "55 8B EC 8B 45 08 83 F8 01 7C"));
		static const auto oGetAttachment = reinterpret_cast<bool(__thiscall*)(void*, int, matrix3x4_t*)>(uAddress);

		if (oGetAttachment != nullptr)
			return oGetAttachment(this, Attachment, matrixOut);
	}
	studiohdr_t* GetStudioHdr()
	{
		static const std::uintptr_t uAddress = std::uintptr_t(signatures_manager->find_signature(("client.dll"), ("8B C1 8B 0D ? ? ? ? 85 C9 75")));
		static const auto GetStudioHdrPtr = reinterpret_cast<studiohdr_t * (__thiscall*)(void*)>(uAddress);
		if (this)
			return GetStudioHdrPtr(this);
		return nullptr;
	}
#endif

}; // Size: 0x06C9 // 0x6D0?

class CMergedMDL : public IRenderToRTHelperObject
{
public:
	virtual ~CMergedMDL() = 0;
	virtual void SetMDL(MDLHandle_t hModelHandle, CCustomMaterialOwner* pCustomMaterialOwner = nullptr, void* pProxyData = nullptr) = 0;
	virtual void SetMDL(const char* szModelPath, CCustomMaterialOwner* pCustomMaterialOwner = nullptr, void* pProxyData = nullptr) = 0;

	void SetupBonesForAttachmentQueries()
	{
		static const auto oSetupBonesForAttachmentQueries = reinterpret_cast<void(__thiscall*)(void*)>(signatures_manager->signatures[SIGNATURE_SETUP_BONES_FOR_ATTACHMENT_QUERIES]);

		if (oSetupBonesForAttachmentQueries != nullptr)
			oSetupBonesForAttachmentQueries(this);
	}

	void SetSequence(const int nSequence, const bool bUseSequencePlaybackFPS, const float flPlaybackRate = 30.f)
	{
		this->RootMDL.nSequence = nSequence;
		this->RootMDL.flPlaybackRate = flPlaybackRate;
		this->RootMDL.bUseSequencePlaybackFPS = bUseSequencePlaybackFPS;
		this->RootMDL.flTimeBasisAdjustment = this->RootMDL.flTime;
	}
	void SetMergedMDL(const char* szModelPath, CCustomMaterialOwner* pCustomMaterialOwner = nullptr, void* pProxyData = nullptr, const bool bRequestBoneMergeTakeover = false)
	{
		static const auto oSetMergedMDL = reinterpret_cast<void(__thiscall*)(void*, const char*, CCustomMaterialOwner*, void*, bool)>(signatures_manager->signatures[SIGNATURE_SET_MERGED_MDL]);

		if (oSetMergedMDL != nullptr)
			oSetMergedMDL(this, szModelPath, pCustomMaterialOwner, pProxyData, bRequestBoneMergeTakeover);
	}

	CUtlVector<CMDL>& m_aMergeMDLs() {
		return *(CUtlVector<CMDL>*)((uintptr_t)this + 0x6D4);
	}
public:
	CMDL RootMDL; // 0x0000
	CUtlVector<CMDL> vecMergedModels; // 0x069C
	float arrPoseParameters[24]; // 0x06E9
	int	nSequenceLayers; // 0x0749
	MDLSquenceLayer_t sequenceLayers[8]; // 0x074D -> // end: 0x78D
}; // Expected Size: 0x075C
