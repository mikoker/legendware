#pragma once

#include "UtlVector.hpp"
#include "../Interfaces/IClientEntity.hpp"

class CGlowObjectManager {
public:

	int RegisterGlowObject(Entity* pEntity, const Vector& vGlowColor, float flGlowAlpha, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot) {
		int nIndex;
		if (m_nFirstFreeSlot == GlowObjectDefinition_t::END_OF_FREE_LIST) {
			nIndex = m_GlowObjectDefinitions.AddToTail();
		}
		else {
			nIndex = m_nFirstFreeSlot;
			m_nFirstFreeSlot = m_GlowObjectDefinitions[nIndex].m_nNextFreeSlot;
		}

		m_GlowObjectDefinitions[nIndex].m_hEntity = pEntity;
		m_GlowObjectDefinitions[nIndex].m_vGlowColor = vGlowColor;
		m_GlowObjectDefinitions[nIndex].m_flGlowAlpha = flGlowAlpha;
		m_GlowObjectDefinitions[nIndex].m_flGlowAlphaFunctionOfMaxVelocity = 0.0f;
		m_GlowObjectDefinitions[nIndex].m_flGlowAlphaMax = 1.0f;
		m_GlowObjectDefinitions[nIndex].m_flGlowPulseOverdrive = 0.0f;
		m_GlowObjectDefinitions[nIndex].m_bRenderWhenOccluded = bRenderWhenOccluded;
		m_GlowObjectDefinitions[nIndex].m_bRenderWhenUnoccluded = bRenderWhenUnoccluded;
		m_GlowObjectDefinitions[nIndex].m_bFullBloomRender = false;
		m_GlowObjectDefinitions[nIndex].m_nFullBloomStencilTestValue = 0;
		m_GlowObjectDefinitions[nIndex].m_nSplitScreenSlot = nSplitScreenSlot;
		m_GlowObjectDefinitions[nIndex].m_nNextFreeSlot = GlowObjectDefinition_t::ENTRY_IN_USE;

		return nIndex;
	}
	constexpr int RegisterGlowObject(Entity* entity) noexcept
	{
		int index = m_nFirstFreeSlot;
		if (index != -1) {
			m_nFirstFreeSlot = m_GlowObjectDefinitions[index].m_nNextFreeSlot;
			m_GlowObjectDefinitions[index].m_hEntity = entity;
			m_GlowObjectDefinitions[index].m_bFullBloomRender = false;
			m_GlowObjectDefinitions[index].m_nFullBloomStencilTestValue = 0;
			m_GlowObjectDefinitions[index].m_nSplitScreenSlot = -1;
			m_GlowObjectDefinitions[index].m_nNextFreeSlot = -2;
		}
		return index;
	}

	void UnregisterGlowObject(int nGlowObjectHandle) {
		Assert(!m_GlowObjectDefinitions[nGlowObjectHandle].IsUnused());

		m_GlowObjectDefinitions[nGlowObjectHandle].m_nNextFreeSlot = m_nFirstFreeSlot;
		m_GlowObjectDefinitions[nGlowObjectHandle].m_hEntity = NULL;
		m_nFirstFreeSlot = nGlowObjectHandle;
	}

	int HasGlowEffect(Entity* pEntity) const { //-V2009
		for (int i = 0; i < m_GlowObjectDefinitions.Count(); ++i) {
			if (!m_GlowObjectDefinitions[i].IsUnused() && m_GlowObjectDefinitions[i].m_hEntity == pEntity) {
				return i;
			}
		}

		return NULL;
	}

	class GlowObjectDefinition_t {
	public:
		void Set(float r, float g, float b, float a, float bloom, int style) {
			m_vGlowColor = Vector(r, g, b);
			m_flGlowAlpha = a;
			m_bRenderWhenOccluded = true;
			m_bRenderWhenUnoccluded = false;
			m_flGlowAlphaMax = bloom;
			m_nRenderStyle = style;
		}

		Entity* GetEnt() {
			return m_hEntity;
		}

		bool IsUnused() const { return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE; }

	public:
		// Linked list of free slots
		int m_nNextFreeSlot;
		Entity* m_hEntity;
		Vector m_vGlowColor;
		float m_flGlowAlpha;
		bool m_bGlowAlphaCappedByRenderAlpha;
		float m_flGlowAlphaFunctionOfMaxVelocity;
		float m_flGlowAlphaMax;
		float m_flGlowPulseOverdrive;
		bool m_bRenderWhenOccluded;
		bool m_bRenderWhenUnoccluded;
		bool m_bFullBloomRender;
		int m_nFullBloomStencilTestValue;
		int m_nRenderStyle;
		int m_nSplitScreenSlot;

		// Special values for GlowObjectDefinition_t::m_nNextFreeSlot
		static const int END_OF_FREE_LIST = -1;
		static const int ENTRY_IN_USE = -2;
	};

	CUtlVector< GlowObjectDefinition_t > m_GlowObjectDefinitions;
	int m_nFirstFreeSlot;
};