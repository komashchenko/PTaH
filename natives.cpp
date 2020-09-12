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
#include <inetchannel.h>


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

		CEconItemDefinition* pItemDefinition = g_pCEconItemSchema->GetItemDefinitionByName(strSource);

		return reinterpret_cast<cell_t>(pItemDefinition);
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

static cell_t PTaH_GetAttributeDefinitionByName(IPluginContext* pContext, const cell_t* params)
{
	if (g_pCEconItemSchema)
	{
		char* strSource; pContext->LocalToString(params[1], &strSource);

		CEconItemAttributeDefinition* pItemAttributeDefinition = g_pCEconItemSchema->GetAttributeDefinitionByName(strSource);

		return reinterpret_cast<cell_t>(pItemAttributeDefinition);
	}

	smutils->LogError(myself, "g_pCEconItemSchema == nullptr.");

	return 0;
}

static cell_t PTaH_GetAttributeDefinitionByDefIndex(IPluginContext* pContext, const cell_t* params)
{
	if (g_pCEconItemSchema)
	{
		CEconItemAttributeDefinition* pItemAttributeDefinition = g_pCEconItemSchema->GetAttributeDefinitionByDefIndex(params[1]);

		return reinterpret_cast<cell_t>(pItemAttributeDefinition);
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

	if (pPlayer->IsFakeClient())
	{
		return pContext->ThrowNativeError("Client %d is a bot", params[1]);
	}

	static int offset = -1;

	if (offset == -1)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CGameClient::UpdateAcknowledgedFramecount", &offset))
		{
			smutils->LogError(myself, "Failed to get CGameClient::UpdateAcknowledgedFramecount offset.");

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
		const char* sBuf = pItemDefinition->GetDefinitionName();

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

#ifdef PLATFORM_WINDOWS
	static CBaseEntity* (__stdcall* SpawnItem)(uint16_t, Vector*, QAngle*, int, int, int) = nullptr;
#else
	static CBaseEntity* (__cdecl* SpawnItem)(void*, uint16_t, Vector*, QAngle*, int, int, int) = nullptr;
#endif

	if (SpawnItem == nullptr)
	{
		if (!g_pGameConf[GameConf_PTaH]->GetMemSig("CItemGeneration::SpawnItem", (void**)&SpawnItem))
		{
			smutils->LogError(myself, "Failed to get CItemGeneration::SpawnItem function.");

			return -1;
		}
	}

	CBaseEntity* pItem;

#ifdef PLATFORM_WINDOWS
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

#ifdef PLATFORM_WINDOWS
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
#ifdef PLATFORM_WINDOWS
	//Clearing stack after call.
	__asm add esp, 2Ch
#endif

	*bLagCompensation = bSave;

	return 0;
}

static cell_t PTaH_SetPlayerAvatar(IPluginContext* pContext, const cell_t* params)
{
	int iClient = params[1], iTarget = params[2];

	IGamePlayer* pClient = playerhelpers->GetGamePlayer(iClient);

	if (!pClient)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", iClient);
	}

	if (!pClient->IsConnected())
	{
		return pContext->ThrowNativeError("Client %d is not connected", iClient);
	}

	if (pClient->IsFakeClient())
	{
		return pContext->ThrowNativeError("Client %d is a bot", iClient);
	}

	IGamePlayer* pTarget;

	if (params[2] != -1)
	{
		pTarget = playerhelpers->GetGamePlayer(params[2]);

		if (!pTarget)
		{
			return pContext->ThrowNativeError("Target index %d is invalid", iTarget);
		}

		if (!pTarget->IsConnected())
		{
			return pContext->ThrowNativeError("Target %d is not connected", iTarget);
		}

		if (pClient->IsFakeClient())
		{
			return pContext->ThrowNativeError("Target %d is a bot", iTarget);
		}
	}

	cell_t* pAvatarValue;
	pContext->LocalToPhysAddr(params[3], &pAvatarValue);

	CNetMessagePB_PlayerAvatarData msgAvatarData;

	msgAvatarData.set_accountid(pClient->GetSteamAccountID());
	msgAvatarData.set_rgb(pAvatarValue, 64*64*3);

	INetChannel* pNetChan;

	if (iTarget != -1)
	{
		if (pNetChan = static_cast<INetChannel*>(engine->GetPlayerNetInfo(iTarget)))
		{
			return pNetChan->EnqueueVeryLargeAsyncTransfer(msgAvatarData);
		}
	}
	else
	{
		bool bResult = true;

		for (int i = 1; i <= playerhelpers->GetMaxClients(); i++)
		{
			if (pNetChan = static_cast<INetChannel*>(engine->GetPlayerNetInfo(i)))
			{
				if (!pNetChan->EnqueueVeryLargeAsyncTransfer(msgAvatarData))
				{
					bResult = false;
				}
			}
		}

		return bResult;
	}

	return false;
}

static cell_t CEconItemDefinition_GetDefinitionIndex(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		return pItemDefinition->GetDefinitionIndex();
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t CEconItemDefinition_GetLoadoutSlot(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		return pItemDefinition->GetLoadoutSlot(params[2]);
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t CEconItemDefinition_GetUsedByTeam(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		return pItemDefinition->GetUsedByTeam();
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t CEconItemDefinition_GetNumSupportedStickerSlots(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		return pItemDefinition->GetNumSupportedStickerSlots();
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t CEconItemDefinition_GetEconImage(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		size_t numBytes = 0;
		const char* sBuf = pItemDefinition->GetInventoryImage();

		if (sBuf)
		{
			pContext->StringToLocalUTF8(params[2], params[3], sBuf, &numBytes);
		}

		return numBytes;
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t CEconItemDefinition_GetModel(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		size_t numBytes = 0;
		const char* sBuf;

		switch (static_cast<PTaH_ModelType>(params[2]))
		{
			case ViewModel:
			{
				sBuf = pItemDefinition->GetBasePlayerDisplayModel();
				break;
			}
			case WorldModel:
			{
				sBuf = pItemDefinition->GetWorldDisplayModel();
				break;
			}
			case DroppedModel:
			{
				sBuf = pItemDefinition->GetWorldDroppedModel();
				break;
			}
			default:
			{
				return pContext->ThrowNativeError("Invalid PTaH_ModelType value");
			}
		}

		if (sBuf)
		{
			pContext->StringToLocalUTF8(params[3], params[4], sBuf, &numBytes);
		}

		return numBytes;
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t CEconItemDefinition_GetClassName(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);

	if (pItemDefinition)
	{
		size_t numBytes = 0;
		const char* sBuf = pItemDefinition->GetDefinitionName();

		if (sBuf)
		{
			pContext->StringToLocalUTF8(params[2], params[3], sBuf, &numBytes);
		}

		return numBytes;
	}

	return pContext->ThrowNativeError("CEconItemDefinition == nullptr");
}

static cell_t CEconItemView_GetCustomPaintKitIndex(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetCustomPaintKitIndex();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetCustomPaintKitSeed(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetCustomPaintKitSeed();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetCustomPaintKitWear(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return sp_ftoc(pItemView->GetCustomPaintKitWear(sp_ctof(params[2])));
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetStickerAttributeBySlotIndex(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		EStickerAttributeType StickerAttributeType = static_cast<EStickerAttributeType>(params[3]);

		if (StickerAttributeType == EStickerAttribute_ID)
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

static cell_t CEconItemView_IsTradable(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->IsTradable();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_IsMarketable(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->IsMarketable();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetItemDefinition(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return reinterpret_cast<cell_t>(pItemView->GetItemDefinition());
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetAccountID(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetAccountID();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetItemID(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if(pItemView)
	{
		cell_t *pItemID;

		uint64_t iItemID = pItemView->GetItemID();

		pContext->LocalToPhysAddr(params[2], &pItemID);

		pItemID[0] = iItemID & 0xFFFFFFFF;
		pItemID[1] = iItemID >> 32;

		return 0;
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetQuality(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetQuality();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetRarity(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetRarity();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetFlags(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetFlags();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetOrigin(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetOrigin();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetCustomName(IPluginContext* pContext, const cell_t* params)
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

static cell_t CEconItemView_GetStatTrakKill(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return pItemView->GetKillEaterValue();
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_GetAttributeValueByIndex(IPluginContext* pContext, const cell_t* params)
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

static cell_t CEconItemView_AttributeList_get(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return reinterpret_cast<cell_t>(&pItemView->m_AttributeList);
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CEconItemView_NetworkedDynamicAttributesForDemos_get(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);

	if (pItemView)
	{
		return reinterpret_cast<cell_t>(&pItemView->m_NetworkedDynamicAttributesForDemos);
	}

	return pContext->ThrowNativeError("CEconItemView == nullptr");
}

static cell_t CCSPlayerInventory_GetItemInLoadout(IPluginContext* pContext, const cell_t* params)
{
	CCSPlayerInventory* pPlayerInventory = reinterpret_cast<CCSPlayerInventory*>(params[1]);

	if (pPlayerInventory)
	{
		CEconItemView* pItemView = pPlayerInventory->GetItemInLoadout(params[2], params[3]);

		return reinterpret_cast<cell_t>(pItemView);
	}

	return pContext->ThrowNativeError("CCSPlayerInventory == nullptr");
}

static cell_t CCSPlayerInventory_GetItemsCount(IPluginContext* pContext, const cell_t* params)
{
	CCSPlayerInventory* pPlayerInventory = reinterpret_cast<CCSPlayerInventory*>(params[1]);

	if (pPlayerInventory)
	{
		CUtlVector<CEconItemView*>* pItems = pPlayerInventory->GetItemVector();

		if (pItems)
		{
			return pItems->Count();
		}

		return 0;
	}

	return pContext->ThrowNativeError("CCSPlayerInventory == nullptr");
}

static cell_t CCSPlayerInventory_GetItem(IPluginContext* pContext, const cell_t* params)
{
	CCSPlayerInventory* pPlayerInventory = reinterpret_cast<CCSPlayerInventory*>(params[1]);

	if (pPlayerInventory)
	{
		CUtlVector<CEconItemView*>* pItems = pPlayerInventory->GetItemVector();

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

static cell_t CAttributeList_DestroyAllAttributes(IPluginContext* pContext, const cell_t* params)
{
	CAttributeList* pAttributeList = reinterpret_cast<CAttributeList*>(params[1]);

	if (pAttributeList)
	{
		pAttributeList->DestroyAllAttributes();

		return 0;
	}

	return pContext->ThrowNativeError("CAttributeList == nullptr");
}

static cell_t CAttributeList_GetAttributesCount(IPluginContext* pContext, const cell_t* params)
{
	CAttributeList* pAttributeList = reinterpret_cast<CAttributeList*>(params[1]);

	if (pAttributeList)
	{
		return pAttributeList->GetNumAttributes();
	}

	return pContext->ThrowNativeError("CAttributeList == nullptr");
}

static cell_t CAttributeList_GetAttribute(IPluginContext* pContext, const cell_t* params)
{
	CAttributeList* pAttributeList = reinterpret_cast<CAttributeList*>(params[1]);

	if (pAttributeList)
	{
		int iIndex = params[2];

		if (iIndex >= 0 && iIndex < pAttributeList->GetNumAttributes())
		{
			CEconItemAttribute& pAttribute = pAttributeList->GetAttribute(iIndex);

			return reinterpret_cast<cell_t>(&pAttribute);
		}
		else
		{
			return pContext->ThrowNativeError("Attribute index %d is invalid", iIndex);
		}
	}

	return pContext->ThrowNativeError("CAttributeList == nullptr");
}

static cell_t CAttributeList_GetAttributeByDefIndex(IPluginContext* pContext, const cell_t* params)
{
	CAttributeList* pAttributeList = reinterpret_cast<CAttributeList*>(params[1]);

	if (pAttributeList)
	{
		CEconItemAttribute* pAttribute = pAttributeList->GetAttributeByDefIndex(params[2]);

		return reinterpret_cast<cell_t>(pAttribute);
	}

	return pContext->ThrowNativeError("CAttributeList == nullptr");
}

static cell_t CAttributeList_RemoveAttribute(IPluginContext* pContext, const cell_t* params)
{
	CAttributeList* pAttributeList = reinterpret_cast<CAttributeList*>(params[1]);

	if (pAttributeList)
	{
		int iIndex = params[2];

		if (iIndex >= 0 && iIndex < pAttributeList->GetNumAttributes())
		{
			pAttributeList->RemoveAttribute(iIndex);

			return 0;
		}
		else
		{
			return pContext->ThrowNativeError("Attribute index %d is invalid", iIndex);
		}
	}

	return pContext->ThrowNativeError("CAttributeList == nullptr");
}

static cell_t CAttributeList_RemoveAttributeByDefIndex(IPluginContext* pContext, const cell_t* params)
{
	CAttributeList* pAttributeList = reinterpret_cast<CAttributeList*>(params[1]);

	if (pAttributeList)
	{
		pAttributeList->RemoveAttributeByDefIndex(params[2]);

		return 0;
	}

	return pContext->ThrowNativeError("CAttributeList == nullptr");
}

static cell_t CAttributeList_SetOrAddAttributeValue(IPluginContext* pContext, const cell_t* params)
{
	CAttributeList* pAttributeList = reinterpret_cast<CAttributeList*>(params[1]);

	if (pAttributeList)
	{
		if (!g_pCEconItemSchema)
		{
			smutils->LogError(myself, "g_pCEconItemSchema == nullptr.");

			return 0;
		}

		if (!g_pCEconItemSchema->GetAttributeDefinitionByDefIndex(params[2]))
		{
			return pContext->ThrowNativeError("Attribute index %d is invalid", params[2]);
		}

		pAttributeList->SetOrAddAttributeValue(params[2], params[3]);

		return 0;
	}

	return pContext->ThrowNativeError("CAttributeList == nullptr");
}

static cell_t CEconItemAttribute_DefinitionIndex_get(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttribute* pAttribute = reinterpret_cast<CEconItemAttribute*>(params[1]);

	if (pAttribute)
	{
		return pAttribute->m_iAttributeDefinitionIndex;
	}

	return pContext->ThrowNativeError("CEconItemAttribute == nullptr");
}

static cell_t CEconItemAttribute_Value_set(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttribute* pAttribute = reinterpret_cast<CEconItemAttribute*>(params[1]);

	if (pAttribute)
	{
		pAttribute->m_flValue = sp_ctof(params[2]);

		return 0;
	}
	
	return pContext->ThrowNativeError("CEconItemAttribute == nullptr");
}

static cell_t CEconItemAttribute_Value_get(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttribute* pAttribute = reinterpret_cast<CEconItemAttribute*>(params[1]);

	if (pAttribute)
	{
		return sp_ftoc(pAttribute->m_flValue);
	}

	return pContext->ThrowNativeError("CEconItemAttribute == nullptr");
}

static cell_t CEconItemAttribute_InitialValue_get(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttribute* pAttribute = reinterpret_cast<CEconItemAttribute*>(params[1]);

	if (pAttribute)
	{
		return sp_ftoc(pAttribute->m_flInitialValue);
	}

	return pContext->ThrowNativeError("CEconItemAttribute == nullptr");
}

static cell_t CEconItemAttribute_RefundableCurrency_set(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttribute* pAttribute = reinterpret_cast<CEconItemAttribute*>(params[1]);

	if (pAttribute)
	{
		pAttribute->m_nRefundableCurrency = params[2];

		return 0;
	}

	return pContext->ThrowNativeError("CEconItemAttribute == nullptr");
}

static cell_t CEconItemAttribute_RefundableCurrency_get(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttribute* pAttribute = reinterpret_cast<CEconItemAttribute*>(params[1]);

	if (pAttribute)
	{
		return pAttribute->m_nRefundableCurrency;
	}

	return pContext->ThrowNativeError("CEconItemAttribute == nullptr");
}

static cell_t CEconItemAttribute_SetBonus_set(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttribute* pAttribute = reinterpret_cast<CEconItemAttribute*>(params[1]);

	if (pAttribute)
	{
		pAttribute->m_bSetBonus = static_cast<bool>(params[2]);

		return 0;
	}

	return pContext->ThrowNativeError("CEconItemAttribute == nullptr");
}

static cell_t CEconItemAttribute_SetBonus_get(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttribute* pAttribute = reinterpret_cast<CEconItemAttribute*>(params[1]);

	if (pAttribute)
	{
		return pAttribute->m_bSetBonus;
	}

	return pContext->ThrowNativeError("CEconItemAttribute == nullptr");
}

static cell_t CEconItemAttributeDefinition_GetDefinitionIndex(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttributeDefinition* pItemAttributeDefinition = reinterpret_cast<CEconItemAttributeDefinition*>(params[1]);

	if (pItemAttributeDefinition)
	{
		return pItemAttributeDefinition->GetDefinitionIndex();
	}

	return pContext->ThrowNativeError("CEconItemAttributeDefinition == nullptr");
}

static cell_t CEconItemAttributeDefinition_GetDefinitionName(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttributeDefinition* pItemAttributeDefinition = reinterpret_cast<CEconItemAttributeDefinition*>(params[1]);

	if (pItemAttributeDefinition)
	{
		size_t numBytes = 0;
		const char* sBuf = pItemAttributeDefinition->GetDefinitionName();

		if (sBuf)
		{
			pContext->StringToLocalUTF8(params[2], params[3], sBuf, &numBytes);
		}

		return numBytes;
	}

	return pContext->ThrowNativeError("CEconItemAttributeDefinition == nullptr");
}

static cell_t CEconItemAttributeDefinition_GetAttributeType(IPluginContext* pContext, const cell_t* params)
{
	CEconItemAttributeDefinition* pItemAttributeDefinition = reinterpret_cast<CEconItemAttributeDefinition*>(params[1]);

	if (pItemAttributeDefinition)
	{
		return pItemAttributeDefinition->GetAttributeType();
	}

	return pContext->ThrowNativeError("CEconItemAttributeDefinition == nullptr");
}


extern const sp_nativeinfo_t g_ExtensionNatives[] =
{
	{ "PTaH_Version",											PTaH_Version },
	{ "PTaH",													PTaH_ },
	{ "PTaH_GetItemDefinitionByName",							PTaH_GetItemDefinitionByName },
	{ "PTaH_GetItemDefinitionByDefIndex",						PTaH_GetItemDefinitionByDefIndex },
	{ "PTaH_GetAttributeDefinitionByName",						PTaH_GetAttributeDefinitionByName },
	{ "PTaH_GetAttributeDefinitionByDefIndex",					PTaH_GetAttributeDefinitionByDefIndex },
	{ "PTaH_GetEconItemViewFromEconEntity",						PTaH_GetEconItemViewFromEconEntity },
	{ "PTaH_GetPlayerInventory",								PTaH_GetPlayerInventory },
	{ "PTaH_GivePlayerItem",									PTaH_GivePlayerItem },
	{ "PTaH_ForceFullUpdate",									PTaH_ForceFullUpdate },
	{ "PTaH_SpawnItemFromDefIndex",								PTaH_SpawnItemFromDefIndex },
	{ "PTaH_FX_FireBullets",									PTaH_FX_FireBullets },
	{ "PTaH_SetPlayerAvatar",									PTaH_SetPlayerAvatar },
	{ "CEconItemDefinition.GetDefinitionIndex",					CEconItemDefinition_GetDefinitionIndex },
	{ "CEconItemDefinition.GetLoadoutSlot",						CEconItemDefinition_GetLoadoutSlot },
	{ "CEconItemDefinition.GetUsedByTeam",						CEconItemDefinition_GetUsedByTeam },
	{ "CEconItemDefinition.GetNumSupportedStickerSlots",		CEconItemDefinition_GetNumSupportedStickerSlots },
	{ "CEconItemDefinition.GetEconImage",						CEconItemDefinition_GetEconImage },
	{ "CEconItemDefinition.GetModel",							CEconItemDefinition_GetModel },
	{ "CEconItemDefinition.GetClassName",						CEconItemDefinition_GetClassName },
	{ "CEconItemView.GetCustomPaintKitIndex",					CEconItemView_GetCustomPaintKitIndex },
	{ "CEconItemView.GetCustomPaintKitSeed",					CEconItemView_GetCustomPaintKitSeed },
	{ "CEconItemView.GetCustomPaintKitWear",					CEconItemView_GetCustomPaintKitWear },
	{ "CEconItemView.GetStickerAttributeBySlotIndex",			CEconItemView_GetStickerAttributeBySlotIndex },
	{ "CEconItemView.IsTradable",								CEconItemView_IsTradable },
	{ "CEconItemView.IsMarketable",								CEconItemView_IsMarketable },
	{ "CEconItemView.GetItemDefinition",						CEconItemView_GetItemDefinition },
	{ "CEconItemView.GetItemID",								CEconItemView_GetItemID },
	{ "CEconItemView.GetAccountID",								CEconItemView_GetAccountID },
	{ "CEconItemView.GetQuality",								CEconItemView_GetQuality },
	{ "CEconItemView.GetRarity",								CEconItemView_GetRarity },
	{ "CEconItemView.GetFlags",									CEconItemView_GetFlags },
	{ "CEconItemView.GetOrigin",								CEconItemView_GetOrigin },
	{ "CEconItemView.GetCustomName",							CEconItemView_GetCustomName },
	{ "CEconItemView.GetStatTrakKill",							CEconItemView_GetStatTrakKill },
	{ "CEconItemView.GetAttributeValueByIndex",					CEconItemView_GetAttributeValueByIndex },
	{ "CEconItemView.AttributeList.get",						CEconItemView_AttributeList_get },
	{ "CEconItemView.NetworkedDynamicAttributesForDemos.get",	CEconItemView_NetworkedDynamicAttributesForDemos_get },
	{ "CCSPlayerInventory.GetItemInLoadout",					CCSPlayerInventory_GetItemInLoadout },
	{ "CCSPlayerInventory.GetItemsCount",						CCSPlayerInventory_GetItemsCount },
	{ "CCSPlayerInventory.GetItem",								CCSPlayerInventory_GetItem },
	{ "CAttributeList.DestroyAllAttributes",					CAttributeList_DestroyAllAttributes },
	{ "CAttributeList.GetAttributesCount",						CAttributeList_GetAttributesCount },
	{ "CAttributeList.GetAttribute",							CAttributeList_GetAttribute },
	{ "CAttributeList.GetAttributeByDefIndex",					CAttributeList_GetAttributeByDefIndex },
	{ "CAttributeList.RemoveAttribute",							CAttributeList_RemoveAttribute},
	{ "CAttributeList.RemoveAttributeByDefIndex",				CAttributeList_RemoveAttributeByDefIndex },
	{ "CAttributeList.SetOrAddAttributeValue",					CAttributeList_SetOrAddAttributeValue },
	{ "CEconItemAttribute.DefinitionIndex.get",					CEconItemAttribute_DefinitionIndex_get },
	{ "CEconItemAttribute.Value.set",							CEconItemAttribute_Value_set },
	{ "CEconItemAttribute.Value.get",							CEconItemAttribute_Value_get },
	{ "CEconItemAttribute.InitialValue.get",					CEconItemAttribute_InitialValue_get },
	{ "CEconItemAttribute.RefundableCurrency.set",				CEconItemAttribute_RefundableCurrency_set },
	{ "CEconItemAttribute.RefundableCurrency.get",				CEconItemAttribute_RefundableCurrency_get },
	{ "CEconItemAttribute.SetBonus.set",						CEconItemAttribute_SetBonus_set },
	{ "CEconItemAttribute.SetBonus.get",						CEconItemAttribute_SetBonus_get },
	{ "CEconItemAttributeDefinition.GetDefinitionIndex",		CEconItemAttributeDefinition_GetDefinitionIndex },
	{ "CEconItemAttributeDefinition.GetDefinitionName",			CEconItemAttributeDefinition_GetDefinitionName },
	{ "CEconItemAttributeDefinition.GetAttributeType",			CEconItemAttributeDefinition_GetAttributeType },
	// Deprecated
	{ "PTaH_GetItemInLoadout",									PTaH_GetItemInLoadoutDeprecated },
	{ "PTaH_GetEconItemViewFromWeapon",							PTaH_GetEconItemViewFromEconEntity },
	{ nullptr,													nullptr }
};