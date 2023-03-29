/**
* vim: set ts=4 :
* =============================================================================
* SourceMod P Tools and Hooks Extension
* Copyright (C) 2016-2023 Phoenix (˙·٠●Феникс●٠·˙).  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "extension.h"
#include "classes.h"


void* CEconItemSchema::operator new(size_t) throw()
{
	//Called once, no static needed
	CEconItemSchema* (*GetItemSchema)(void);

	if (!g_pGameConf[GameConf_CSST]->GetMemSig("GetItemSchema", (void**)&GetItemSchema) || !GetItemSchema)
	{
		smutils->LogError(myself, "Failed to get GetItemSchema function.");

		return nullptr;
	}

#ifdef PLATFORM_WINDOWS
	return GetItemSchema() + sizeof(void*);
#else
	return GetItemSchema();
#endif
};

CUtlHashMapLarge<int, CEconItemDefinition*>* CEconItemSchema::GetItemDefinitionMap()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CEconItemSchema::m_mapItems", &offset))
		{
			smutils->LogError(myself, "Failed to get CEconItemSchema::m_mapItems offset.");

			return nullptr;
		}
	}
	
	return (CUtlHashMapLarge<int, CEconItemDefinition*>*)((intptr_t)this + offset);
}

CUtlVector<CEconItemAttributeDefinition*>* CEconItemSchema::GetAttributeDefinitionContainer()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CEconItemSchema::m_mapAttributesContainer", &offset))
		{
			smutils->LogError(myself, "Failed to get CEconItemSchema::m_mapAttributesContainer offset.");

			return nullptr;
		}
	}

	return (CUtlVector<CEconItemAttributeDefinition*>*)((intptr_t)this + offset);
}

CEconItemDefinition* CEconItemSchema::GetItemDefinitionByName(const char* pszDefName)
{
	auto pMapItemDef = GetItemDefinitionMap();

	if (pMapItemDef)
	{
		FOR_EACH_MAP_FAST(*pMapItemDef, i)
		{
			if (!strcmp(pszDefName, pMapItemDef->Element(i)->GetDefinitionName()))
			{
				return pMapItemDef->Element(i);
			}
		}
	}

	return nullptr;
}

CEconItemDefinition* CEconItemSchema::GetItemDefinitionByDefIndex(uint16_t iItemIndex)
{
	auto pMapItemDef = GetItemDefinitionMap();

	if (pMapItemDef)
	{
		int iIndex = pMapItemDef->Find(iItemIndex);
		
		if (pMapItemDef->IsValidIndex(iIndex))
		{
			return pMapItemDef->Element(iIndex);
		}
	}

	return nullptr;
}

CEconItemAttributeDefinition* CEconItemSchema::GetAttributeDefinitionByName(const char* pszDefName)
{
	auto pAttributesContainer = GetAttributeDefinitionContainer();

	if (pAttributesContainer)
	{
		FOR_EACH_VEC(*pAttributesContainer, i)
		{
			CEconItemAttributeDefinition* pItemAttributeDefinition = pAttributesContainer->Element(i);

			if (pItemAttributeDefinition && !strcmp(pszDefName, pItemAttributeDefinition->GetDefinitionName()))
			{
				return pItemAttributeDefinition;
			}
		}
	}
	
	return nullptr;
}

CEconItemAttributeDefinition* CEconItemSchema::GetAttributeDefinitionByDefIndex(uint16_t iDefIndex)
{
	auto pAttributesContainer = GetAttributeDefinitionContainer();

	if (pAttributesContainer)
	{
		if (pAttributesContainer->IsValidIndex(iDefIndex))
		{
			return pAttributesContainer->Element(iDefIndex);
		}
	}

	return nullptr;
}

ESchemaAttributeType CEconItemAttributeDefinition::GetAttributeType()
{
	if (IsAttributeType<CSchemaAttributeType_Default>())
	{
		if (IsStoredAsInteger())
		{
			return ESchemaAttribute_Uint32;
		}

		return ESchemaAttribute_Float;
	}

	if (IsAttributeType<CSchemaAttributeType_Uint32>())
	{
		return ESchemaAttribute_Uint32;
	}

	if (IsAttributeType<CSchemaAttributeType_Float>())
	{
		return ESchemaAttribute_Float;
	}

	if (IsAttributeType<CSchemaAttributeType_String>())
	{
		return ESchemaAttribute_String;
	}

	if (IsAttributeType<CSchemaAttributeType_Vector>())
	{
		return ESchemaAttribute_Vector;
	}

	return ESchemaAttribute_Unknown;
}

int CEconItemDefinition::GetLoadoutSlot(int iTeam)
{
	static int (WIN_LINUX(__thiscall, __cdecl)* GetLoadoutSlot)(void*, int) = nullptr;

	if (GetLoadoutSlot == nullptr)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetMemSig("CCStrike15ItemDefinition::GetLoadoutSlot", (void**)&GetLoadoutSlot) || !GetLoadoutSlot)
		{
			smutils->LogError(myself, "Failed to get CCStrike15ItemDefinition::GetLoadoutSlot function.");

			return -1;
		}
	}

	return GetLoadoutSlot(this, iTeam);
}

int CEconItemDefinition::GetUsedByTeam()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CCStrike15ItemDefinition::m_vbClassUsability", &offset))
		{
			smutils->LogError(myself, "Failed to get CCStrike15ItemDefinition::m_vbClassUsability offset.");

			return -1;
		}
	}

	CBitVec<4>* pClassUsability = (CBitVec<4>*)((intptr_t)this + offset);
	
	if (pClassUsability->IsBitSet(2) && pClassUsability->IsBitSet(3))
	{
		return 0;
	}

	if (pClassUsability->IsBitSet(3))
	{
		return 3;
	}

	if (pClassUsability->IsBitSet(2))
	{
		return 2;
	}
	
	return 0;
}

int CEconItemDefinition::GetNumSupportedStickerSlots()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CEconItemDefinition::GetNumSupportedStickerSlots", &offset))
		{
			smutils->LogError(myself, "Failed to get CEconItemDefinition::GetNumSupportedStickerSlots offset.");

			return -1;
		}
	}

	return CallVFunc<int>(offset, this);
}

const char* CEconItemDefinition::GetInventoryImage()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CEconItemDefinition::GetInventoryImage", &offset))
		{
			smutils->LogError(myself, "Failed to get CEconItemDefinition::GetInventoryImage offset.");

			return nullptr;
		}
	}

	return CallVFunc<const char*>(offset, this);
}

const char* CEconItemDefinition::GetBasePlayerDisplayModel()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CEconItemDefinition::GetBasePlayerDisplayModel", &offset))
		{
			smutils->LogError(myself, "Failed to get CEconItemDefinition::GetBasePlayerDisplayModel offset.");

			return nullptr;
		}
	}

	return CallVFunc<const char*>(offset, this);
}

const char* CEconItemDefinition::GetWorldDisplayModel()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CEconItemDefinition::GetWorldDisplayModel", &offset))
		{
			smutils->LogError(myself, "Failed to get CEconItemDefinition::GetWorldDisplayModel offset.");

			return nullptr;
		}
	}

	return CallVFunc<const char*>(offset, this);
}

const char* CEconItemDefinition::GetWorldDroppedModel()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CEconItemDefinition::GetWorldDroppedModel", &offset))
		{
			smutils->LogError(myself, "Failed to get CEconItemDefinition::GetWorldDroppedModel offset.");

			return nullptr;
		}
	}

	return CallVFunc<const char*>(offset, this);
}

const char* CEconItemDefinition::GetDefinitionName()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CEconItemDefinition::m_pszDefinitionName", &offset))
		{
			smutils->LogError(myself, "Failed to get CEconItemDefinition::m_pszDefinitionName offset.");

			return nullptr;
		}
	}

	return *(const char**)((intptr_t)this + offset);
}

// Thank you Kailo
int CEconItemView::GetKillEaterValue()
{
	if (g_pCEconItemSchema)
	{
		CEconItemAttributeDefinition* pItemAttrDef = g_pCEconItemSchema->GetAttributeDefinitionByDefIndex(80); //"kill eater"
		unsigned int KillEaterValue;

		CAttributeIterator_GetTypedAttributeValue<unsigned int, unsigned int> it(pItemAttrDef, &KillEaterValue);
		this->IterateAttributes(&it);

		if (it.m_found) return KillEaterValue;
	}
	else smutils->LogError(myself, "g_pCEconItemSchema == nullptr.");

	return -1;
}

void* CPlayerVoiceListener::operator new(size_t) throw()
{
	void* addr = nullptr;

	if (!g_pGameConf[GameConf_PTaH]->GetAddress("g_CPlayerVoiceListener", &addr) || !addr)
	{
		smutils->LogError(myself, "Failed to get g_CPlayerVoiceListener address.");
	}

	return addr;
}

bool CPlayerVoiceListener::IsPlayerSpeaking(int iClient)
{
	return *(float*)((intptr_t)this + 4 * iClient + 12) + 0.5f > gpGlobals->curtime;
}

intptr_t CCSPlayerInventory::GetInventoryOffset()
{
	static intptr_t iInventoryOffset = -1;

	if (iInventoryOffset == -1)
	{
		void* addr = nullptr;

		if (!g_pGameConf[GameConf_CSST]->GetOffset("CCSPlayerInventoryOffset", &iInventoryOffset))
		{
			smutils->LogError(myself, "Failed to get CCSPlayerInventoryOffset offset.");

			return -1;
		}

		if (!g_pGameConf[GameConf_CSST]->GetMemSig("HandleCommand_Buy_Internal", &addr) || !addr)
		{
			smutils->LogError(myself, "Failed to get HandleCommand_Buy_Internal address.");

			return -1;
		}

		iInventoryOffset = *(intptr_t*)((intptr_t)addr + iInventoryOffset);
	}

	return iInventoryOffset;
}

CCSPlayerInventory* CCSPlayerInventory::FromPlayer(CBaseEntity* pPlayer)
{
	int offset = GetInventoryOffset();

	if (offset == -1)
	{
		return nullptr;
	}

	return (CCSPlayerInventory*)((intptr_t)pPlayer + offset);
}

CBaseEntity* CCSPlayerInventory::ToPlayer()
{
	int offset = GetInventoryOffset();

	if (offset == -1)
	{
		return nullptr;
	}

	return (CBaseEntity*)((intptr_t)this - offset);
}

CEconItemView* CCSPlayerInventory::GetItemInLoadout(int iTeam, int iLoadoutSlot)
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_CSST]->GetOffset("GetItemInLoadout", &offset))
		{
			smutils->LogError(myself, "Failed to get GetItemInLoadout offset.");

			return nullptr;
		}
	}

	return CallVFunc<CEconItemView*, int, int>(offset, this, iTeam, iLoadoutSlot);
}

CUtlVector<CEconItemView*>* CCSPlayerInventory::GetItemVector()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CPlayerInventory::m_vecInventoryItems", &offset))
		{
			smutils->LogError(myself, "Failed to get CPlayerInventory::m_vecInventoryItems offset.");

			return nullptr;
		}
	}

	return (CUtlVector<CEconItemView*>*)((intptr_t)this + offset);
}

CEconItemAttribute::CEconItemAttribute()
{
	m_iAttributeDefinitionIndex = 0;
	m_flValue = 0.f;
	m_flInitialValue = 0.f;
	m_nRefundableCurrency = 0;
	m_bSetBonus = false;
};

CEconItemAttribute::CEconItemAttribute(const uint16 iAttributeIndex, uint32 unValue)
{
	CEconItemAttribute();

	m_iAttributeDefinitionIndex = iAttributeIndex;

	m_flValue = *reinterpret_cast<float*>(&unValue);
	m_flInitialValue = m_flValue;
};

CEconItemAttribute::CEconItemAttribute(const uint16 iAttributeIndex, float fValue)
{
	CEconItemAttribute();

	m_iAttributeDefinitionIndex = iAttributeIndex;
	m_flValue = fValue;
	m_flInitialValue = m_flValue;
};

void CAttributeList::RemoveAttributeByDefIndex(uint16_t unAttrDefIndex)
{
	FOR_EACH_VEC(m_Attributes, i)
	{
		if (m_Attributes[i].m_iAttributeDefinitionIndex == unAttrDefIndex)
		{
			m_Attributes.Remove(i);

			return;
		}
	}
}

void CAttributeList::SetOrAddAttributeValue(uint16_t unAttrDefIndex, uint32_t unValue)
{
	CEconItemAttribute* pAttribute = GetAttributeByDefIndex(unAttrDefIndex);

	if (pAttribute)
	{
		pAttribute->m_flValue = *reinterpret_cast<float*>(&unValue);
	}
	else
	{
		CEconItemAttribute attribute(unAttrDefIndex, unValue);

		AddAttribute(attribute);
	}
}

CEconItemAttribute* CAttributeList::GetAttributeByDefIndex(uint16_t unAttrDefIndex)
{
	FOR_EACH_VEC(m_Attributes, i)
	{
		if (m_Attributes[i].m_iAttributeDefinitionIndex == unAttrDefIndex)
		{
			return &m_Attributes[i];
		}
	}

	return nullptr;
}

bool CNetMessagePB_PlayerAvatarData::WriteToBuffer(bf_write& buffer) const
{
	int size = ByteSize();

	void* serializeBuffer = stackalloc(size);

	if (!SerializeWithCachedSizesToArray((google::protobuf::uint8*)serializeBuffer))
	{
		return false;
	}

	buffer.WriteVarInt32(GetType());
	buffer.WriteVarInt32(size);

	return buffer.WriteBytes(serializeBuffer, size);
}

const char* CNetMessagePB_PlayerAvatarData::ToString() const
{
	m_toString = DebugString();

	return m_toString.c_str();
}
