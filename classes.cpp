/**
* vim: set ts=4 :
* =============================================================================
* SourceMod P Tools and Hooks Extension
* Copyright (C) 2004-2016 AlliedModders LLC.  All rights reserved.
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
*
* As a special exception, AlliedModders LLC gives you permission to link the
* code of this program (as well as its derivative works) to "Half-Life 2," the
* "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, AlliedModders LLC grants
* this exception to all derivative works.  AlliedModders LLC defines further
* exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
* or <http://www.sourcemod.net/license.php>.
*
* Version: $Id$
*/

#include "extension.h"
#include "classes.h"


CEconItemSchema::CEconItemSchema()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		void *addr = nullptr;
		if (!g_pGameConf[GameConf_PTaH]->GetMemSig("GetItemSchema", &addr) || !addr)
		{
			smutils->LogError(myself, "Failed to get GetItemSchema function.");
		}
		PassInfo ret;
		ret.flags = PASSFLAG_BYVAL;
		ret.type = PassType_Basic;
		ret.size = sizeof(void *);
		pCallWrapper = bintools->CreateCall(addr, CallConv_Cdecl, &ret, NULL, 0);
	}
	
	pCallWrapper->Execute(NULL, &pSchema);
	#ifdef WIN32
	pSchema = (CEconItemSchema *)((intptr_t)pSchema + 4);
	#endif
}

CEconItemDefinition *CEconItemSchema::GetItemDefinitionByName(const char *classname)
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetItemDefintionByName", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetItemDefintionByName offset");
			return NULL;
		}
		
		PassInfo pass[1];
		PassInfo ret;
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].type  = PassType_Basic;
		pass[0].size  = sizeof(const char *);

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(void *);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, pass, 1);
	}
	
	unsigned char vstk[sizeof(void *) + sizeof(const char *)];
	unsigned char *vptr = vstk;

	*(void **)vptr = pSchema;
	vptr += sizeof(void *);
	*(const char **)vptr = classname;

	CEconItemDefinition *pItemDef = nullptr;
	pCallWrapper->Execute(vstk, &pItemDef);
	
	return pItemDef;
}

CEconItemDefinition *CEconItemSchema::GetItemDefinitionByDefIndex(uint16_t DefIndex)
{
	static int offset = -1;
	if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetItemDefinitionByDefIndex", &offset) || offset == -1)
	{
		smutils->LogError(myself, "Failed to get GetItemDefinitionByDefIndex offset");
		return nullptr;
	}
	
	if(DefIndex > 0)
	{
		//See GetItemDefinitionByMapIndex
		struct ItemMapMember
		{
			uint16_t DefIndex;
			CEconItemDefinition* ItemDefinition;
			int Unknown;
		};
		
		ItemMapMember *MapMember = nullptr;
		int iMaxIdx = *(int *)((intptr_t)pSchema + offset + 20);
		intptr_t ItemMap = *(intptr_t *)((intptr_t)pSchema + offset);
		for(int i = 0; i < iMaxIdx; i++)
		{
			MapMember = (ItemMapMember *)(ItemMap + i * sizeof(ItemMapMember));
			if(MapMember->DefIndex == DefIndex) return MapMember->ItemDefinition;
		}
	}
	return nullptr;
}

uint16_t CEconItemDefinition::GetDefinitionIndex()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetDefinitionIndex", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetDefinitionIndex offset");
			return -1;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(uint16_t);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	uint16_t DefinitionIndex = 0;
	pCallWrapper->Execute(vstk, &DefinitionIndex);
	return 0;
}

int CEconItemDefinition::GetLoadoutSlot(int def)
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		void *addr = nullptr;
		
		if(!g_pGameConf[GameConf_PTaH]->GetMemSig("GetLoadoutSlot", &addr) || !addr)
		{
			smutils->LogError(myself, "Failed to get GetLoadoutSlot location");
			return def;
		}
		
		PassInfo pass[1];
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].type  = PassType_Basic;
		pass[0].size  = sizeof(int);

		PassInfo ret;
		ret.flags = PASSFLAG_BYVAL;
		ret.type = PassType_Basic;
		ret.size = sizeof(int);
		pCallWrapper = bintools->CreateCall(addr, CallConv_ThisCall, &ret, pass, 1);
	}
	
	unsigned char vstk[sizeof(void *) + sizeof(int)];
	unsigned char *vptr = vstk;

	*(void **)vptr = this;
	vptr += sizeof(void *);
	*(int *)vptr = def;

	int LoadoutSlot = def;
	pCallWrapper->Execute(vstk, &LoadoutSlot);
	return LoadoutSlot;
}

int CEconItemDefinition::GetNumSupportedStickerSlots()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetNumSupportedStickerSlots", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetNumSupportedStickerSlots offset");
			return -1;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(int);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	int NumSupportedStickerSlots = -1;
	pCallWrapper->Execute(vstk, &NumSupportedStickerSlots);
	return NumSupportedStickerSlots;
}

char *CEconItemDefinition::GetClassName()
{
	static int offset = -1;
	if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetClassName", &offset) || offset == -1)
	{
		smutils->LogError(myself, "Failed to get GetClassName offset");
		return nullptr;
	}
	return *(char **)(this + offset);
}

void *CEconItemDefinition::GetCCSWeaponData()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		void *addr = nullptr;
		if (!g_pGameConf[GameConf_PTaH]->GetMemSig("GetCCSWeaponDataFromDef", &addr) || !addr)
		{
			smutils->LogError(myself, "Failed to get GetCCSWeaponDataFromDef function.");
		}
		PassInfo ret;
		PassInfo pass[1];
		ret.flags = PASSFLAG_BYVAL;
		ret.type = PassType_Basic;
		ret.size = sizeof(void *);
 		pass[0].flags = PASSFLAG_BYVAL;
 		pass[0].type = PassType_Basic;
 		pass[0].size = sizeof(CEconItemDefinition *);
		
		pCallWrapper = bintools->CreateCall(addr, CallConv_ThisCall, &ret, pass, 1);
	}
	
	unsigned char vstk[sizeof(const char *)];
	unsigned char *vptr = vstk;
	
	*(CEconItemDefinition **)vptr = this;
	
	void *pCCSWeaponData = nullptr;
	
	pCallWrapper->Execute(vstk, &pCCSWeaponData);
	
	return pCCSWeaponData;
}

int CEconItemView::GetCustomPaintKitIndex()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetCustomPaintKitIndex", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetCustomPaintKitIndex offset");
			return -1;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(int);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	int CustomPaintKitIndex = -1;
	pCallWrapper->Execute(vstk, &CustomPaintKitIndex);
	return CustomPaintKitIndex;
}

int CEconItemView::GetCustomPaintKitSeed()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetCustomPaintKitSeed", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetCustomPaintKitSeed offset");
			return -1;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(int);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	int CustomPaintKitSeed = -1;
	pCallWrapper->Execute(vstk, &CustomPaintKitSeed);
	return CustomPaintKitSeed;
}

float CEconItemView::GetCustomPaintKitWear(float def)
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetCustomPaintKitWear", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetCustomPaintKitWear offset");
			return def;
		}
		
		PassInfo pass[1];
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].type  = PassType_Float;
		pass[0].size  = sizeof(float);

		PassInfo ret;
		ret.flags = PASSFLAG_BYVAL;
		ret.type = PassType_Float;
		ret.size = sizeof(float);
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, pass, 1);
	}

	
	unsigned char vstk[sizeof(void *) + sizeof(float)];
	unsigned char *vptr = vstk;

	*(void **)vptr = this;
	vptr += sizeof(void *);
	*(float *)vptr = def;

	float CustomPaintKitWear = def;
	pCallWrapper->Execute(vstk, &CustomPaintKitWear);
	return CustomPaintKitWear;
}

float CEconItemView::GetStickerAttributeBySlotIndexFloat(int slot, EStickerAttributeType StickerAttribut, float def)
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetStickerAttributeBySlotIndexFloat", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetStickerAttributeBySlotIndexFloat offset");
			return def;
		}
		
		PassInfo pass[3];
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].type  = PassType_Basic;
		pass[0].size  = sizeof(int);
		pass[1].flags = PASSFLAG_BYVAL;
		pass[1].type  = PassType_Basic;
		pass[1].size  = sizeof(EStickerAttributeType);
		pass[2].flags = PASSFLAG_BYVAL;
		pass[2].type  = PassType_Float;
		pass[2].size  = sizeof(float);

		PassInfo ret;
		ret.flags = PASSFLAG_BYVAL;
		ret.type = PassType_Float;
		ret.size = sizeof(float);
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, pass, 3);
	}

	
	unsigned char vstk[sizeof(void *) + sizeof(int) + sizeof(float) + sizeof(EStickerAttributeType)];
	unsigned char *vptr = vstk;

	*(void **)vptr = this;
	vptr += sizeof(void *);
	*(int *)vptr = slot;
	vptr += sizeof(int);
	*(EStickerAttributeType *)vptr = StickerAttribut;
	vptr += sizeof(EStickerAttributeType);
	*(float *)vptr = def;

	float StickerAttributeBySlotIndexFloat = def;
	pCallWrapper->Execute(vstk, &StickerAttributeBySlotIndexFloat);

	return StickerAttributeBySlotIndexFloat;
}

int CEconItemView::GetStickerAttributeBySlotIndexInt(int slot, EStickerAttributeType StickerAttribut, int def)
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetStickerAttributeBySlotIndexInt", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetStickerAttributeBySlotIndexInt offset");
			return def;
		}
		
		PassInfo pass[3];
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].type  = PassType_Basic;
		pass[0].size  = sizeof(int);
		pass[1].flags = PASSFLAG_BYVAL;
		pass[1].type  = PassType_Basic;
		pass[1].size  = sizeof(EStickerAttributeType);
		pass[2].flags = PASSFLAG_BYVAL;
		pass[2].type  = PassType_Basic;
		pass[2].size  = sizeof(float);

		PassInfo ret;
		ret.flags = PASSFLAG_BYVAL;
		ret.type = PassType_Basic;
		ret.size = sizeof(float);
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, pass, 3);
	}
	
	unsigned char vstk[sizeof(void *) + sizeof(int) * 2 + sizeof(EStickerAttributeType)];
	unsigned char *vptr = vstk;

	*(void **)vptr = this;
	vptr += sizeof(void *);
	*(int *)vptr = slot;
	vptr += sizeof(int);
	*(EStickerAttributeType *)vptr = StickerAttribut;
	vptr += sizeof(EStickerAttributeType);
	*(int *)vptr = def;

	int StickerAttributeBySlotIndexInt = def;
	pCallWrapper->Execute(vstk, &StickerAttributeBySlotIndexInt);
	return StickerAttributeBySlotIndexInt;
}

bool CEconItemView::IsTradable()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("IsTradable", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get IsTradable offset");
			return false;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(bool);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	bool IsTradable = false;
	pCallWrapper->Execute(vstk, &IsTradable);
	return IsTradable;
}

bool CEconItemView::IsMarketable()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("IsMarketable", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get IsMarketable offset");
			return false;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(bool);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	bool IsMarketable = false;
	pCallWrapper->Execute(vstk, &IsMarketable);
	return IsMarketable;
}

CEconItemDefinition *CEconItemView::GetItemDefinition()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetItemDefinition", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetItemDefinition offset");
			return NULL;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(void *);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	CEconItemDefinition *ItemDefinition = nullptr;
	pCallWrapper->Execute(vstk, &ItemDefinition);
	return ItemDefinition;
}

int CEconItemView::GetAccountID()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetAccountID", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetAccountID offset");
			return -1;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(int);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	int AccountID = -1;
	pCallWrapper->Execute(vstk, &AccountID);
	return AccountID;
}

int CEconItemView::GetQuality()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetQuality", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetQuality offset");
			return -1;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(int);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	int Quality = -1;
	pCallWrapper->Execute(vstk, &Quality);
	return Quality;
}

int CEconItemView::GetRarity()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetRarity", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetRarity offset");
			return -1;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(int);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	int Rarity = -1;
	pCallWrapper->Execute(vstk, &Rarity);
	return Rarity;
}

int CEconItemView::GetFlags()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetFlags", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetFlags offset");
			return -1;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(int);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	int Flags = -1;
	pCallWrapper->Execute(vstk, &Flags);
	return Flags;
}

int CEconItemView::GetOrigin()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetOrigin", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetOrigin offset");
			return -1;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(int);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	int Origin = -1;
	pCallWrapper->Execute(vstk, &Origin);
	return Origin;
}

char *CEconItemView::GetCustomName()
{
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetCustomName", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetCustomName offset");
			return NULL;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(char *);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)this;

	char *CustomName = NULL;
	pCallWrapper->Execute(vstk, &CustomName);
	return CustomName;
}

int CEconItemView::GetKillEaterValueByType(unsigned int type)
{
	/*static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_PTaH]->GetOffset("GetKillEaterValueByType", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetKillEaterValueByType offset");
			return -1;
		}
		
		PassInfo pass[1];
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].type  = PassType_Basic;
		pass[0].size  = sizeof(unsigned int);

		PassInfo ret;
		ret.flags = PASSFLAG_BYVAL;
		ret.type = PassType_Basic;
		ret.size = sizeof(int);
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, pass, 1);
	}
	
	unsigned char vstk[sizeof(void *) + sizeof(unsigned int)];
	unsigned char *vptr = vstk;

	*(void **)vptr = this;
	vptr += sizeof(void *);
	*(unsigned int *)vptr = type;

	int KillEaterValueByType = -1;
	pCallWrapper->Execute(vstk, &KillEaterValueByType);
	return KillEaterValueByType;*/
	return -1;
}