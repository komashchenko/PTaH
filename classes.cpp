/**
* vim: set ts=4 :
* =============================================================================
* SourceMod P Tools and Hooks Extension
* Copyright (C) 2016-2020 Phoenix (˙·٠●Феникс●٠·˙).  All rights reserved.
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

	if (!g_pGameConf[GameConf_CSST]->GetMemSig("GetItemSchema", (void**)&GetItemSchema))
	{
		smutils->LogError(myself, "Failed to get GetItemSchema function.");

		return nullptr;
	}

#ifdef WIN32
	return GetItemSchema() + sizeof(void*);
#else
	return GetItemSchema();
#endif
};

CEconItemDefinition* CEconItemSchema::GetItemDefinitionByName(const char* classname)
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_CSST]->GetOffset("GetItemDefintionByName", &offset))
		{
			smutils->LogError(myself, "Failed to get GetItemDefintionByName offset.");

			return nullptr;
		}
	}

	return ((CEconItemDefinition*(VCallingConvention*)(void*, const char*))(*(void***)this)[offset])(this, classname);
}

CEconItemDefinition* CEconItemSchema::GetItemDefinitionByDefIndex(uint16_t DefIndex)
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetItemDefinitionByDefIndex", &offset))
		{
			smutils->LogError(myself, "Failed to get GetItemDefinitionByDefIndex offset.");

			return nullptr;
		}
	}

	if (DefIndex > 0)
	{
		//See GetItemDefinitionByMapIndex
		struct ItemMapMember		// CUtlHashMapLarge<int, CEconItemDefinition*, MurmurHash3Functor<int> >
		{
			uint16_t iDefIndex;
			CEconItemDefinition* pItemDefinition;
			int iHash;
		};

		ItemMapMember* MapMember = nullptr;
		int iCount = *(int*)((intptr_t)this + offset + 20);
		intptr_t ItemMap = *(intptr_t*)((intptr_t)this + offset);

		for (int i = 0; i < iCount; i++)
		{
			MapMember = (ItemMapMember*)(ItemMap + i * sizeof(ItemMapMember));

			if (MapMember->iDefIndex == DefIndex) return MapMember->pItemDefinition;
		}
	}

	return nullptr;
}

CEconItemAttributeDefinition* CEconItemSchema::GetAttributeDefinitionByDefIndex(uint16_t DefIndex)
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetAttributeDefinitionInterface", &offset))
		{
			smutils->LogError(myself, "Failed to get GetAttributeDefinitionInterface offset.");

			return nullptr;
		}
	}

	return ((CEconItemAttributeDefinition*(VCallingConvention*)(void*, uint16_t))(*(void***)this)[offset])(this, DefIndex);
}

uint16_t CEconItemDefinition::GetDefinitionIndex()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetDefinitionIndex", &offset))
		{
			smutils->LogError(myself, "Failed to get GetDefinitionIndex offset.");

			return 0;
		}
	}

	return ((uint16_t(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

int CEconItemDefinition::GetLoadoutSlot(int iTeam)
{
	static int (VCallingConvention* GetLoadoutSlot)(void*, int) = nullptr;

	if (GetLoadoutSlot == nullptr)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetMemSig("GetLoadoutSlot", (void**)&GetLoadoutSlot))
		{
			smutils->LogError(myself, "Failed to get GetLoadoutSlot function.");

			return -1;
		}
	}

	return GetLoadoutSlot(this, iTeam);
}

int CEconItemDefinition::GetNumSupportedStickerSlots()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetNumSupportedStickerSlots", &offset))
		{
			smutils->LogError(myself, "Failed to get GetNumSupportedStickerSlots offset.");

			return -1;
		}
	}

	return ((int(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

const char* CEconItemDefinition::GetEconImage()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetEconImage", &offset))
		{
			smutils->LogError(myself, "Failed to get GetEconImage offset.");

			return nullptr;
		}
	}

	return *(const char**)(this + offset);
}

const char* CEconItemDefinition::GetViewModel()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetViewModel", &offset))
		{
			smutils->LogError(myself, "Failed to get GetViewModel offset.");

			return nullptr;
		}
	}

	return *(const char**)(this + offset);
}

const char* CEconItemDefinition::GetWorldModel()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetWorldModel", &offset))
		{
			smutils->LogError(myself, "Failed to get GetWorldModel offset.");

			return nullptr;
		}
	}

	return *(const char**)(this + offset);
}

const char* CEconItemDefinition::GetDroppedModel()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetDroppedModel", &offset))
		{
			smutils->LogError(myself, "Failed to get GetDroppedModel offset.");

			return nullptr;
		}
	}

	return *(const char**)(this + offset);
}

const char* CEconItemDefinition::GetClassName()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetClassName", &offset))
		{
			smutils->LogError(myself, "Failed to get GetClassName offset.");

			return nullptr;
		}
	}

	return *(const char**)(this + offset);
}

int CEconItemView::GetCustomPaintKitIndex()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetCustomPaintKitIndex", &offset))
		{
			smutils->LogError(myself, "Failed to get GetCustomPaintKitIndex offset.");

			return -1;
		}
	}

	return ((int(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

int CEconItemView::GetCustomPaintKitSeed()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetCustomPaintKitSeed", &offset))
		{
			smutils->LogError(myself, "Failed to get GetCustomPaintKitSeed offset.");

			return -1;
		}
	}

	return ((int(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

float CEconItemView::GetCustomPaintKitWear(float def)
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetCustomPaintKitWear", &offset))
		{
			smutils->LogError(myself, "Failed to get GetCustomPaintKitWear offset.");

			return def;
		}
	}

	return ((float(VCallingConvention*)(void*, float))(*(void***)this)[offset])(this, def);
}

float CEconItemView::GetStickerAttributeBySlotIndexFloat(int slot, EStickerAttributeType StickerAttribut, float def)
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetStickerAttributeBySlotIndexFloat", &offset))
		{
			smutils->LogError(myself, "Failed to get GetStickerAttributeBySlotIndexFloat offset.");

			return def;
		}
	}

	return ((float(VCallingConvention*)(void*, int, EStickerAttributeType, float))(*(void***)this)[offset])(this, slot, StickerAttribut, def);
}

int CEconItemView::GetStickerAttributeBySlotIndexInt(int slot, EStickerAttributeType StickerAttribut, int def)
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetStickerAttributeBySlotIndexInt", &offset))
		{
			smutils->LogError(myself, "Failed to get GetStickerAttributeBySlotIndexInt offset.");

			return def;
		}
	}

	return ((int(VCallingConvention*)(void*, int, EStickerAttributeType, int))(*(void***)this)[offset])(this, slot, StickerAttribut, def);
}

bool CEconItemView::IsTradable()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("IsTradable", &offset))
		{
			smutils->LogError(myself, "Failed to get IsTradable offset.");

			return false;
		}
	}

	return ((bool(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

bool CEconItemView::IsMarketable()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("IsMarketable", &offset))
		{
			smutils->LogError(myself, "Failed to get IsMarketable offset.");

			return false;
		}
	}

	return ((bool(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

CEconItemDefinition* CEconItemView::GetItemDefinition()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetItemDefinition", &offset))
		{
			smutils->LogError(myself, "Failed to get GetItemDefinition offset.");

			return nullptr;
		}
	}

	return ((CEconItemDefinition*(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

int CEconItemView::GetAccountID()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetAccountID", &offset))
		{
			smutils->LogError(myself, "Failed to get GetAccountID offset.");

			return -1;
		}
	}

	return ((int(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

uint64_t CEconItemView::GetItemID()
{
	static int offset = -1;

	if(offset == -1)
	{
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetItemID", &offset))
		{
			smutils->LogError(myself, "Failed to get GetItemID offset.");

			return -1;
		}
	}

	return ((uint64_t(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

int CEconItemView::GetQuality()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetQuality", &offset))
		{
			smutils->LogError(myself, "Failed to get GetQuality offset.");

			return -1;
		}
	}

	return ((int(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

int CEconItemView::GetRarity()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetRarity", &offset))
		{
			smutils->LogError(myself, "Failed to get GetRarity offset.");

			return -1;
		}
	}

	return ((int(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

int CEconItemView::GetFlags()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetFlags", &offset))
		{
			smutils->LogError(myself, "Failed to get GetFlags offset.");

			return -1;
		}
	}

	return ((int(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

int CEconItemView::GetOrigin()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetOrigin", &offset))
		{
			smutils->LogError(myself, "Failed to get GetOrigin offset.");

			return -1;
		}
	}

	return ((int(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
}

const char* CEconItemView::GetCustomName()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("GetCustomName", &offset))
		{
			smutils->LogError(myself, "Failed to get GetCustomName offset.");

			return nullptr;
		}
	}

	return ((const char*(VCallingConvention*)(void*))(*(void***)this)[offset])(this);
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

void CEconItemView::IterateAttributes(IEconItemAttributeIterator* AttributeIterator)
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("IterateAttributes", &offset))
		{
			smutils->LogError(myself, "Failed to get IterateAttributes offset.");

			return;
		}
	}

	((void(VCallingConvention*)(void*, IEconItemAttributeIterator*))(*(void***)this)[offset])(this, AttributeIterator);
}

void* CPlayerVoiceListener::operator new(size_t) throw()
{
	void* addr = nullptr;

	if (!g_pGameConf[GameConf_PTaH]->GetAddress("g_CPlayerVoiceListener", &addr))
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

		if (!g_pGameConf[GameConf_CSST]->GetMemSig("HandleCommand_Buy_Internal", &addr))
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
	static int offset = GetInventoryOffset();

	if (offset == -1)
	{
		return nullptr;
	}

	return (CCSPlayerInventory*)((intptr_t)pPlayer + offset);
}

CBaseEntity* CCSPlayerInventory::ToPlayer()
{
	static int offset = GetInventoryOffset();

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

	return ((CEconItemView*(VCallingConvention*)(void*, int, int))(*(void***)this)[offset])(this, iTeam, iLoadoutSlot);
}

CUtlVector<CEconItemView*>* CCSPlayerInventory::GetItems()
{
	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("InventoryItems", &offset))
		{
			smutils->LogError(myself, "Failed to get InventoryItems offset.");

			return nullptr;
		}
	}

	return (CUtlVector<CEconItemView*>*)((intptr_t)this + offset);
}
