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
#include "natives.h"
#include "forwards.h"
#include "classes.h"


static cell_t PTaH_Version(IPluginContext* pContext, const cell_t* params)
{
	if (params[2] != 0) pContext->StringToLocal(params[1], params[2], SMEXT_CONF_VERSION);

	return PTaH_VERSION;
}

static cell_t PTaH_(IPluginContext* pContext, const cell_t* params)
{
	IPluginFunction* pFunction = pContext->GetFunctionById(static_cast<funcid_t>(params[3]));

	if (pFunction)
	{
		if (params[1] >= PTaH_GiveNamedItemPre && PTaH_MAXHOOKS > params[1])
		{
			return g_ForwardManager.FunctionUpdateHook(static_cast<PTaH_HookEvent>(params[1]), pFunction, static_cast<bool>(params[2]));
		}
		else return pContext->ThrowNativeError("Invalid event PTaH_HookType specified");
	}

	return pContext->ThrowNativeError("Invalid funcid");
}

static cell_t PTaH_GetItemDefinitionByName(IPluginContext* pContext, const cell_t* params)
{
	if (g_pCEconItemSchema)
	{
		char* strSource; pContext->LocalToString(params[1], &strSource);

		return reinterpret_cast<cell_t>(g_pCEconItemSchema->GetItemDefinitionByName(strSource));
	}

	smutils->LogError(myself, "g_pCEconItemSchema == nullptr.");

	return 0;
}

static cell_t PTaH_GetItemDefinitionByDefIndex(IPluginContext* pContext, const cell_t* params)
{
	if (g_pCEconItemSchema)
	{
		CEconItemDefinition* pItemDefinition = g_pCEconItemSchema->GetItemDefinitionByDefIndex(params[1]);

		return reinterpret_cast<cell_t>(pItemDefinition);
	}

	smutils->LogError(myself, "g_pCEconItemSchema == nullptr.");

	return 0;
}

static cell_t PTaH_GetItemInLoadoutDeprecated(IPluginContext* pContext, const cell_t* params)
{
	IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(params[1]);

	if (!pPlayer)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}

	if (!pPlayer->IsInGame())
	{
		return pContext->ThrowNativeError("Client %d is not in game", params[1]);
	}

	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);

	CCSPlayerInventory* pPlayerInventory = CCSPlayerInventory::FromPlayer(pEntity);

	if (pPlayerInventory)
	{
		CEconItemView* pItemView = pPlayerInventory->GetItemInLoadout(params[2], params[3]);

		return reinterpret_cast<cell_t>(pItemView);
	}

	return 0;
}

//https://github.com/alliedmodders/sourcemod/blob/0c8e6e29184bf58851954019a2060d84f0c556f9/extensions/sdkhooks/util.cpp#L37
bool UTIL_ContainsDataTable(SendTable* pTable, const char* name)
{
	const char* pname = pTable->GetName();
	int props = pTable->GetNumProps();
	SendProp* prop;
	SendTable* table;

	if (pname && strcmp(name, pname) == 0)
		return true;

	for (int i = 0; i < props; i++)
	{
		prop = pTable->GetProp(i);

		if ((table = prop->GetDataTable()) != nullptr)
		{
			pname = table->GetName();
			if (pname && strcmp(name, pname) == 0)
			{
				return true;
			}

			if (UTIL_ContainsDataTable(table, name))
			{
				return true;
			}
		}
	}

	return false;
}

//Thank you GoD-Tony https://github.com/komashchenko/PTaH/pull/1
static cell_t PTaH_GetEconItemViewFromEconEntity(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);

	if (!pEntity)
	{
		return pContext->ThrowNativeError("Entity %d is invalid", params[1]);
	}

	IServerNetworkable* pNet = reinterpret_cast<IServerUnknown*>(pEntity)->GetNetworkable();

	if (!pNet || !UTIL_ContainsDataTable(pNet->GetServerClass()->m_pTable, "DT_EconEntity"))
	{
		return pContext->ThrowNativeError("Entity %d is not CEconEntity", params[1]);
	}

	static unsigned int offset = 0;

	if (offset == 0)
	{
		sm_sendprop_info_t info;
		gamehelpers->FindSendPropInfo("CEconEntity", "m_Item", &info);
		offset = info.actual_offset;
	}

	return reinterpret_cast<cell_t>(pEntity) + offset;
}

static cell_t PTaH_GetPlayerInventory(IPluginContext* pContext, const cell_t* params)
{
	IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(params[1]);

	if (!pPlayer)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}

	if (!pPlayer->IsInGame())
	{
		return pContext->ThrowNativeError("Client %d is not in game", params[1]);
	}

	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);

	CCSPlayerInventory* pPlayerInventory = CCSPlayerInventory::FromPlayer(pEntity);

	return reinterpret_cast<cell_t>(pPlayerInventory);
}

static cell_t PTaH_GivePlayerItem(IPluginContext* pContext, const cell_t* params)
{
	IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(params[1]);

	if (!pPlayer)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}

	if (!pPlayer->IsInGame())
	{
		return pContext->ThrowNativeError("Client %d is not in game", params[1]);
	}

	char* strSource; pContext->LocalToString(params[2], &strSource);
	Vector Origin; Origin.Invalidate();

	if (params[0] > 3)
	{
		cell_t* source_origin; pContext->LocalToPhysAddr(params[4], &source_origin);

		if (source_origin != pContext->GetNullRef(SP_NULL_VECTOR))
		{
			Origin.x = sp_ctof(source_origin[0]);
			Origin.y = sp_ctof(source_origin[1]);
			Origin.z = sp_ctof(source_origin[2]);
		}
	}

	static int iGiveNamedItemOffset = -1;

	if (iGiveNamedItemOffset == -1)
	{
		if (!g_pGameConf[GameConf_SDKT]->GetOffset("GiveNamedItem", &iGiveNamedItemOffset))
		{
			smutils->LogError(myself, "Failed to get GiveNamedItem offset.");

			return -1;
		}
	}

	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);

	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[3]);

	CBaseEntity* pItem = ((CBaseEntity*(VCallingConvention*)(void*, const char*, int, CEconItemView*, bool, Vector*))
		(*(void***)pEntity)[iGiveNamedItemOffset])(pEntity, strSource, 0, pItemView, false, Origin.IsValid() ? &Origin : nullptr);

	return gamehelpers->EntityToBCompatRef(pItem);
}

static cell_t PTaH_ForceFullUpdate(IPluginContext* pContext, const cell_t* params)
{
	IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(params[1]);

	if (!pPlayer)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}

	if (!pPlayer->IsInGame())
	{
		return pContext->ThrowNativeError("Client %d is not in game", params[1]);
	}

	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("UpdateAcknowledgedFramecount", &offset))
		{
			smutils->LogError(myself, "Failed to get UpdateAcknowledgedFramecount offset.");

			return 0;
		}
	}

	IClient* pClient = iserver->GetClient(params[1] - 1);
	CGameClient* pGameClient = IClientToGameClient(pClient);

	((bool(VCallingConvention*)(void*, int))(*(void***)pGameClient)[offset])(pGameClient, -1);

	return 0;
}

static cell_t PTaH_SpawnItemFromDefIndex(IPluginContext* pContext, const cell_t* params)
{
	if (!g_pCEconItemSchema)
	{
		smutils->LogError(myself, "g_pCEconItemSchema == nullptr.");

		return -1;
	}

	CEconItemDefinition* pItemDefinition = g_pCEconItemSchema->GetItemDefinitionByDefIndex(params[1]);

	if (!pItemDefinition)
	{
		return pContext->ThrowNativeError("Defenition index %d is invalid", params[1]);
	}
	else
	{
		const char* sBuf = pItemDefinition->GetClassName();

		//weapon_* or item_*
		if (!((sBuf[0] == 'w' && sBuf[6] == '_') || (sBuf[0] == 'i' && sBuf[4] == '_')))
		{
			return pContext->ThrowNativeError("Defenition index %d is not weapon_* or item_*", params[1]);
		}
	}

	cell_t* source_origin; pContext->LocalToPhysAddr(params[2], &source_origin);

	if (source_origin == pContext->GetNullRef(SP_NULL_VECTOR))
	{
		return pContext->ThrowNativeError("Origin cannot be NULL_VECTOR");
	}

	cell_t* source_angles; pContext->LocalToPhysAddr(params[3], &source_angles);

	if (source_angles == pContext->GetNullRef(SP_NULL_VECTOR))
	{
		return pContext->ThrowNativeError("Angles cannot be NULL_VECTOR");
	}

	Vector Origin;
	Origin.x = sp_ctof(source_origin[0]);
	Origin.y = sp_ctof(source_origin[1]);
	Origin.z = sp_ctof(source_origin[2]);

	QAngle Angles;
	Angles.x = sp_ctof(source_angles[0]);
	Angles.y = sp_ctof(source_angles[1]);
	Angles.z = sp_ctof(source_angles[2]);

#ifdef WIN32
	static CBaseEntity* (__stdcall* SpawnItem)(uint16_t, Vector*, QAngle*, int, int, int) = nullptr;
#else
	static CBaseEntity* (__cdecl* SpawnItem)(void*, uint16_t, Vector*, QAngle*, int, int, int) = nullptr;
#endif

	if (SpawnItem == nullptr)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetMemSig("SpawnItem", (void**)&SpawnItem))
		{
			smutils->LogError(myself, "Failed to get SpawnItem function.");

			return -1;
		}
	}

	CBaseEntity* pItem;

#ifdef WIN32
	pItem = SpawnItem(params[1], &Origin, &Angles, 1, 4, 0);
#else
	pItem = SpawnItem(nullptr, params[1], &Origin, &Angles, 1, 4, 0);
#endif

	return gamehelpers->EntityToBCompatRef(pItem);
}

static cell_t PTaH_FX_FireBullets(IPluginContext* pContext, const cell_t* params)
{
	IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(params[1]);

	if (!pPlayer)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}

	if (!pPlayer->IsInGame())
	{
		return pContext->ThrowNativeError("Client %d is not in game", params[1]);
	}

	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[2]);

	if (!pItemView)
	{
		return pContext->ThrowNativeError("CEconItemView == nullptr");
	}

	cell_t* source_origin; pContext->LocalToPhysAddr(params[3], &source_origin);

	if (source_origin == pContext->GetNullRef(SP_NULL_VECTOR))
	{
		return pContext->ThrowNativeError("Origin cannot be NULL_VECTOR");
	}

	cell_t* source_angles; pContext->LocalToPhysAddr(params[4], &source_angles);

	if (source_angles == pContext->GetNullRef(SP_NULL_VECTOR))
	{
		return pContext->ThrowNativeError("Angles cannot be NULL_VECTOR");
	}

	Vector Origin;
	Origin.x = sp_ctof(source_origin[0]);
	Origin.y = sp_ctof(source_origin[1]);
	Origin.z = sp_ctof(source_origin[2]);

	QAngle Angles;
	Angles.x = sp_ctof(source_angles[0]);
	Angles.y = sp_ctof(source_angles[1]);
	Angles.z = sp_ctof(source_angles[2]);

#ifdef WIN32
	//Very similar to __fastcall, but does not clean stack.
	static void (__fastcall* FX_FireBullets)(int, CBaseCombatWeapon*, CEconItemView*, Vector*, QAngle*, int, int, float, float, float, float, int, float) = nullptr;
#else
	static void (__cdecl* FX_FireBullets)(int, CBaseCombatWeapon*, CEconItemView*, Vector*, QAngle*, int, int, float, float, float, float, int, float) = nullptr;
#endif

	if (FX_FireBullets == nullptr)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetMemSig("FX_FireBullets", (void**)&FX_FireBullets))
		{
			smutils->LogError(myself, "Failed to get FX_FireBullets function.");

			return 0;
		}
	}

	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);

	static unsigned int m_bLagCompensationOffset = 0;
	if (m_bLagCompensationOffset == 0)
	{
		sm_datatable_info_t info;
		gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), "m_bLagCompensation", &info);
		m_bLagCompensationOffset = info.actual_offset;
	}

	bool* bLagCompensation = (bool*)((intptr_t)pEntity + m_bLagCompensationOffset);
	bool bSave = *bLagCompensation;
	*bLagCompensation = false;

	FX_FireBullets(params[1], nullptr, pItemView, &Origin, &Angles, params[5], params[6], sp_ctof(params[7]), sp_ctof(params[8]), sp_ctof(params[9]), 0.f, params[10], sp_ctof(params[11]));
#ifdef WIN32
	//Clearing stack after call.
	__asm add esp, 2Ch
#endif

	*bLagCompensation = bSave;

	return 0;
}

static cell_t PTaH_GetDefinitionIndex(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		return pItemDefinition->GetDefinitionIndex();
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t PTaH_GetLoadoutSlot(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		return pItemDefinition->GetLoadoutSlot(params[2]);
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t PTaH_GetNumSupportedStickerSlots(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		return pItemDefinition->GetNumSupportedStickerSlots();
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t PTaH_GetClassName(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		size_t numBytes;
		const char* sBuf = pItemDefinition->GetClassName();

		pContext->StringToLocalUTF8(params[2], params[3], sBuf ? sBuf : "", &numBytes);

		return numBytes;
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t PTaH_GetCustomPaintKitIndex(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetCustomPaintKitIndex();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetCustomPaintKitSeed(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetCustomPaintKitSeed();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetCustomPaintKitWear(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return sp_ftoc(pItemView->GetCustomPaintKitWear(sp_ctof(params[2])));
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetStickerAttributeBySlotIndex(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		EStickerAttributeType StickerAttributeType = static_cast<EStickerAttributeType>(params[3]);

		if (StickerAttributeType == StickerID)
		{
			return pItemView->GetStickerAttributeBySlotIndexInt(params[2], StickerAttributeType, params[4]);
		}
		else
		{
			return sp_ftoc(pItemView->GetStickerAttributeBySlotIndexFloat(params[2], StickerAttributeType, sp_ctof(params[4])));
		}
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_IsTradable(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->IsTradable();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_IsMarketable(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->IsMarketable();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetItemDefinition(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return reinterpret_cast<cell_t>(pItemView->GetItemDefinition());
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetAccountID(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetAccountID();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetQuality(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetQuality();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetRarity(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetRarity();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetFlags(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetFlags();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetOrigin(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetOrigin();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetCustomName(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		size_t numBytes;
		const char* sBuf = pItemView->GetCustomName();

		pContext->StringToLocalUTF8(params[2], params[3], sBuf ? sBuf : "", &numBytes);

		return numBytes;
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetKillEater(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetKillEaterValue();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetAttributeValueByIndex(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		if (g_pCEconItemSchema)
		{
			CEconItemAttributeDefinition* pItemAttrDef = g_pCEconItemSchema->GetAttributeDefinitionByDefIndex(params[2]);

			if (!pItemAttrDef)
			{
				return pContext->ThrowNativeError("Attribute index %d is invalid", params[2]);
			}

			cell_t* pValue;

			pContext->LocalToPhysAddr(params[3], &pValue);

			if (pItemAttrDef->IsAttributeType<CSchemaAttributeType_Float>())
			{
				CAttributeIterator_GetTypedAttributeValue<float, float> it(pItemAttrDef, reinterpret_cast<float*>(pValue));
				
				pItemView->IterateAttributes(&it);

				if (it.m_found)
				{
					return 1;
				}
			}
			else
			{
				CAttributeIterator_GetTypedAttributeValue<unsigned int, unsigned int> it(pItemAttrDef, reinterpret_cast<unsigned int*>(pValue));

				pItemView->IterateAttributes(&it);

				if (it.m_found)
				{
					return 1;
				}
			}
		}
		else
		{
			smutils->LogError(myself, "g_pCEconItemSchema == nullptr.");
		}

		return 0;
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t PTaH_GetItemInLoadout(IPluginContext* pContext, const cell_t* params)
{
	CCSPlayerInventory* pPlayerInventory = reinterpret_cast<CCSPlayerInventory*>(params[1]);

	if (pPlayerInventory)
	{
		CEconItemView* pItemView = pPlayerInventory->GetItemInLoadout(params[2], params[3]);

		return reinterpret_cast<cell_t>(pItemView);
	}

	return pContext->ThrowNativeError("CCSPlayerInventory == nullptr");
}

static cell_t PTaH_GetItemsCount(IPluginContext* pContext, const cell_t* params)
{
	CCSPlayerInventory* pPlayerInventory = reinterpret_cast<CCSPlayerInventory*>(params[1]);

	if (pPlayerInventory)
	{
		CUtlVector<CEconItemView*>* pItems = pPlayerInventory->GetItems();

		if (pItems)
		{
			return pItems->Count();
		}

		return 0;
	}

	return pContext->ThrowNativeError("CCSPlayerInventory == nullptr");
}

static cell_t PTaH_GetItem(IPluginContext* pContext, const cell_t* params)
{
	CCSPlayerInventory* pPlayerInventory = reinterpret_cast<CCSPlayerInventory*>(params[1]);

	if (pPlayerInventory)
	{
		CUtlVector<CEconItemView*>* pItems = pPlayerInventory->GetItems();

		if (pItems)
		{
			int iIndex = params[2];

			if (iIndex >= 0 && iIndex < pItems->Count())
			{
				CEconItemView* pItemView = pItems->Element(iIndex);

				return reinterpret_cast<cell_t>(pItemView);
			}
			else
			{
				return pContext->ThrowNativeError("Item index %d is invalid", iIndex);
			}
		}

		return 0;
	}

	return pContext->ThrowNativeError("CCSPlayerInventory == nullptr");
}


extern const sp_nativeinfo_t g_ExtensionNatives[] =
{
	{ "PTaH_Version",										PTaH_Version },
	{ "PTaH",												PTaH_ },
	{ "PTaH_GetItemDefinitionByName",						PTaH_GetItemDefinitionByName },
	{ "PTaH_GetItemDefinitionByDefIndex",					PTaH_GetItemDefinitionByDefIndex },
	{ "PTaH_GetEconItemViewFromEconEntity",					PTaH_GetEconItemViewFromEconEntity },
	{ "PTaH_GetPlayerInventory",							PTaH_GetPlayerInventory },
	{ "PTaH_GivePlayerItem",								PTaH_GivePlayerItem },
	{ "PTaH_ForceFullUpdate",								PTaH_ForceFullUpdate },
	{ "PTaH_SpawnItemFromDefIndex",							PTaH_SpawnItemFromDefIndex },
	{ "PTaH_FX_FireBullets",								PTaH_FX_FireBullets },
	{ "CEconItemDefinition.GetDefinitionIndex",				PTaH_GetDefinitionIndex },
	{ "CEconItemDefinition.GetLoadoutSlot",					PTaH_GetLoadoutSlot },
	{ "CEconItemDefinition.GetNumSupportedStickerSlots",	PTaH_GetNumSupportedStickerSlots },
	{ "CEconItemDefinition.GetClassName",					PTaH_GetClassName },
	{ "CEconItemView.GetCustomPaintKitIndex",				PTaH_GetCustomPaintKitIndex },
	{ "CEconItemView.GetCustomPaintKitSeed",				PTaH_GetCustomPaintKitSeed },
	{ "CEconItemView.GetCustomPaintKitWear",				PTaH_GetCustomPaintKitWear },
	{ "CEconItemView.GetStickerAttributeBySlotIndex",		PTaH_GetStickerAttributeBySlotIndex },
	{ "CEconItemView.IsTradable",							PTaH_IsTradable },
	{ "CEconItemView.IsMarketable",							PTaH_IsMarketable },
	{ "CEconItemView.GetItemDefinition",					PTaH_GetItemDefinition },
	{ "CEconItemView.GetAccountID",							PTaH_GetAccountID },
	{ "CEconItemView.GetQuality",							PTaH_GetQuality },
	{ "CEconItemView.GetRarity",							PTaH_GetRarity },
	{ "CEconItemView.GetFlags",								PTaH_GetFlags },
	{ "CEconItemView.GetOrigin",							PTaH_GetOrigin },
	{ "CEconItemView.GetCustomName",						PTaH_GetCustomName },
	{ "CEconItemView.GetStatTrakKill",						PTaH_GetKillEater },
	{ "CEconItemView.GetAttributeValueByIndex",				PTaH_GetAttributeValueByIndex },
	{ "CCSPlayerInventory.GetItemInLoadout",				PTaH_GetItemInLoadout },
	{ "CCSPlayerInventory.GetItemsCount",					PTaH_GetItemsCount },
	{ "CCSPlayerInventory.GetItem",							PTaH_GetItem },
	// Deprecated
	{ "PTaH_GetItemInLoadout",								PTaH_GetItemInLoadoutDeprecated },
	{ "PTaH_GetEconItemViewFromWeapon",						PTaH_GetEconItemViewFromEconEntity },
	{ nullptr,												nullptr }
};