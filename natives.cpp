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
#include "natives.h"
#include "forwards.h"
#include "classes.h"
#include "server_class.h"


static cell_t PTaH_Version(IPluginContext* pContext, const cell_t* params)
{
	if (params[2] != 0) pContext->StringToLocal(params[1], params[2], SMEXT_CONF_VERSION);

	return PTaH_VERSION;
}

static cell_t PTaH_(IPluginContext* pContext, const cell_t* params)
{
	if (params[2] == 0)
	{
		switch (params[1])
		{
			case PTaH_GiveNamedItem: return g_pPTaHForwards.m_pGiveNamedItem->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_GiveNamedItemPre: return g_pPTaHForwards.m_pGiveNamedItemPre->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_WeaponCanUse: return g_pPTaHForwards.m_pWeaponCanUse->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_SetPlayerModel: return g_pPTaHForwards.m_pSetModel->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_SetPlayerModelPre: return g_pPTaHForwards.m_pSetModelPre->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_ConsolePrint: return g_pPTaHForwards.m_pClientPrintf->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_MapContentList: return g_pPTaHForwards.m_pMapContentList->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_OnClientConnect: return g_pPTaHForwards.m_pOnClientConnect->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_ExecuteStringCommand: return g_pPTaHForwards.m_pExecuteStringCommand->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_ServerConsolePrint: return g_pPTaHForwards.m_pServerConsolePrint->AddFunction(pContext, static_cast<funcid_t>(params[3]));
		}
	}
	else
	{
		switch (params[1])
		{
			case PTaH_GiveNamedItem: return g_pPTaHForwards.m_pGiveNamedItem->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_GiveNamedItemPre: return g_pPTaHForwards.m_pGiveNamedItemPre->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_WeaponCanUse: return g_pPTaHForwards.m_pWeaponCanUse->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_SetPlayerModel: return g_pPTaHForwards.m_pSetModel->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_SetPlayerModelPre: return g_pPTaHForwards.m_pSetModelPre->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_ConsolePrint: return g_pPTaHForwards.m_pClientPrintf->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_MapContentList: return g_pPTaHForwards.m_pMapContentList->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_OnClientConnect: return g_pPTaHForwards.m_pOnClientConnect->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_ExecuteStringCommand: return g_pPTaHForwards.m_pExecuteStringCommand->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_ServerConsolePrint: return g_pPTaHForwards.m_pServerConsolePrint->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
		}
	}

	return false;
}

static cell_t PTaH_GetItemDefinitionByName(IPluginContext* pContext, const cell_t* params)
{
	if (g_pCEconItemSchema)
	{
		char* strSource; pContext->LocalToString(params[1], &strSource);

		return reinterpret_cast<cell_t>(g_pCEconItemSchema->GetItemDefinitionByName(strSource));
	}

	return pContext->ThrowNativeError("g_pCEconItemSchema == nullptr");
}

static cell_t PTaH_GetItemDefinitionByDefIndex(IPluginContext* pContext, const cell_t* params)
{
	if (g_pCEconItemSchema)
	{
		CEconItemDefinition* pItemDefinition = g_pCEconItemSchema->GetItemDefinitionByDefIndex(params[1]);

		return reinterpret_cast<cell_t>(pItemDefinition);
	}

	return pContext->ThrowNativeError("g_pCEconItemSchema == nullptr");
}

static cell_t PTaH_GetItemInLoadout(IPluginContext* pContext, const cell_t* params)
{
	if ((params[1] < 1) || (params[1] > playerhelpers->GetMaxClients()))
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}

	CBaseEntity* pEntity;

	if ((pEntity = gamehelpers->ReferenceToEntity(params[1])) == nullptr)
	{
		return pContext->ThrowNativeError("Client %d is not in game", params[1]);
	}
	if (params[2] != 2 && params[2] != 3)
	{
		return pContext->ThrowNativeError("Team index %d is invalid", params[2]);
	}

	static int iGetItemInLoadoutOffset = -1, iInventoryOffset = -1;

	if (iGetItemInLoadoutOffset == -1 || iInventoryOffset == -1)
	{
		if (!g_pGameConf[GameConf_CSST]->GetOffset("GetItemInLoadout", &iGetItemInLoadoutOffset) || iGetItemInLoadoutOffset == -1)
		{
			smutils->LogError(myself, "Failed to get GetItemInLoadout offset.");

			return 0;
		}

		void* addr = nullptr;

		if (!g_pGameConf[GameConf_CSST]->GetOffset("CCSPlayerInventoryOffset", &iInventoryOffset) || iInventoryOffset == -1)
		{
			smutils->LogError(myself, "Failed to get CCSPlayerInventoryOffset offset.");

			return 0;
		}

		if (!g_pGameConf[GameConf_CSST]->GetMemSig("HandleCommand_Buy_Internal", &addr) || !addr)
		{
			smutils->LogError(myself, "Failed to get HandleCommand_Buy_Internal address.");

			return 0;
		}

		iInventoryOffset = *(int*)((intptr_t)addr + iInventoryOffset);
	}

	void* pCCSPlayerInventory = (void*)((intptr_t)pEntity + iInventoryOffset);

	CEconItemView* pItemView = ((CEconItemView*(VCallingConvention*)(void*, int, int))(*(void***)pCCSPlayerInventory)[iGetItemInLoadoutOffset])
		(pCCSPlayerInventory, params[2], params[3]);

	return reinterpret_cast<cell_t>(pItemView);
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

		if ((table = prop->GetDataTable()) != NULL)
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
static cell_t PTaH_GetEconItemViewFromWeapon(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);

	if (!pEntity)
	{
		return pContext->ThrowNativeError("Entity %d is invalid", params[1]);
	}

	IServerNetworkable* pNet = ((IServerUnknown*)pEntity)->GetNetworkable();

	if (!pNet || !UTIL_ContainsDataTable(pNet->GetServerClass()->m_pTable, "DT_BaseCombatWeapon"))
	{
		return pContext->ThrowNativeError("Entity %d is not weapon", params[1]);
	}

	static unsigned int offset = 0;

	if (offset == 0)
	{
		sm_sendprop_info_t info;
		gamehelpers->FindSendPropInfo("CEconEntity", "m_Item", &info);
		offset = info.actual_offset;
	}

	return (intptr_t)pEntity + offset;
}

static cell_t PTaH_GivePlayerItem(IPluginContext* pContext, const cell_t* params)
{
	if ((params[1] < 1) || (params[1] > playerhelpers->GetMaxClients()))
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}

	CBaseEntity* pEntity;

	if ((pEntity = gamehelpers->ReferenceToEntity(params[1])) == nullptr)
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
		if (!g_pGameConf[GameConf_SDKT]->GetOffset("GiveNamedItem", &iGiveNamedItemOffset) || iGiveNamedItemOffset == -1)
		{
			smutils->LogError(myself, "Failed to get GiveNamedItem offset.");

			return -1;
		}
	}

	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[3]);

	CBaseEntity* pItem = ((CBaseEntity*(VCallingConvention*)(void*, const char*, int, CEconItemView*, bool, Vector*))
		(*(void***)pEntity)[iGiveNamedItemOffset])(pEntity, strSource, 0, pItemView, false, Origin.IsValid() ? &Origin : nullptr);

	return gamehelpers->EntityToBCompatRef(pItem);
}

static cell_t PTaH_SpawnItemFromDefIndex(IPluginContext* pContext, const cell_t* params)
{
	if ((params[1] < 1))
	{
		return pContext->ThrowNativeError("Defenition index %d is invalid", params[1]);
	}
	
	cell_t* source_origin;
	pContext->LocalToPhysAddr(params[2], &source_origin);

	Vector Origin; Origin.Invalidate();
 	if (source_origin != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		Origin.x = sp_ctof(source_origin[0]);
		Origin.y = sp_ctof(source_origin[1]);
		Origin.z = sp_ctof(source_origin[2]);
	}
	else
	{
		return pContext->ThrowNativeError("NULL not allowed");
	}
 
	cell_t* source_qangle;
	pContext->LocalToPhysAddr(params[3], &source_qangle);
	
	QAngle Angles;
	if (source_qangle != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		Angles.x = sp_ctof(source_qangle[0]);
		Angles.y = sp_ctof(source_qangle[1]);
		Angles.z = sp_ctof(source_qangle[2]);
	}
	else
	{
		return pContext->ThrowNativeError("NULL not allowed");
	}
	
	#ifdef WIN32
	CBaseEntity* (__stdcall* SpawnItem)(uint16_t, Vector*, QAngle*, int, int, int);
	#else
	CBaseEntity* (__cdecl* SpawnItem)(void*, uint16_t, Vector*, QAngle*, int, int, int);
	#endif
	
	if (!g_pGameConf[GameConf_PTaH]->GetMemSig("SpawnItem", (void**)&SpawnItem) || !SpawnItem)
	{
		smutils->LogError(myself, "Failed to get SpawnItem function.");
		return -1;
	}

	CBaseEntity* pEntity;
	#ifdef WIN32
	pEntity = SpawnItem(params[1], &Origin, &Angles, 1, 4, 0);
	#else
	pEntity = SpawnItem(nullptr, params[1], &Origin, &Angles, 1, 4, 0);
	#endif
	return gamehelpers->EntityToBCompatRef(pEntity);
}	

static cell_t PTaH_FX_FireBullets(IPluginContext* pContext, const cell_t* params)
{
	if ((params[1] < 1) || (params[1] > playerhelpers->GetMaxClients()))
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}
	CBaseEntity* pEntity;
	if((pEntity = gamehelpers->ReferenceToEntity(params[1])) == nullptr)
	{
		return pContext->ThrowNativeError("Client %d is not in game", params[1]);
	}
	
	CEconItemView* pItem = (CEconItemView*)params[3];
	CBaseEntity* pWeapon = gamehelpers->ReferenceToEntity(params[2]);
	if(!pWeapon && !pItem)
	{
		return pContext->ThrowNativeError("Entity %d is invalid", params[2]);
	}
	
	IServerNetworkable* pNet = ((IServerUnknown*)pWeapon)->GetNetworkable();
	if (!pNet || !UTIL_ContainsDataTable(pNet->GetServerClass()->m_pTable, "DT_BaseCombatWeapon"))
	{
		return pContext->ThrowNativeError("Entity %d is not weapon", params[2]);
	}
	
	cell_t* source_origin;
	pContext->LocalToPhysAddr(params[4], &source_origin);

	Vector Origin; Origin.Invalidate();
 	if (source_origin != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		Origin.x = sp_ctof(source_origin[0]);
		Origin.y = sp_ctof(source_origin[1]);
		Origin.z = sp_ctof(source_origin[2]);
	}
	else
	{
		return pContext->ThrowNativeError("NULL not allowed");
	}
 
	cell_t* source_qangle;
	pContext->LocalToPhysAddr(params[5], &source_qangle);
	
	QAngle Angles;
	if (source_qangle != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		Angles.x = sp_ctof(source_qangle[0]);
		Angles.y = sp_ctof(source_qangle[1]);
		Angles.z = sp_ctof(source_qangle[2]);
	}
	else
	{
		return pContext->ThrowNativeError("NULL not allowed");
	}

	#ifdef WIN32
	void (__thiscall* FX_FireBullets)(int, CBaseCombatWeapon*, CEconItemView*, Vector*, QAngle*, int, int, float, float, float, float, int, float);
	#else
	long double (__cdecl* FX_FireBullets)(int, CBaseCombatWeapon*, CEconItemView*, Vector*, QAngle*, int, int, float, float, float, float, int, float);
	#endif
	
	if (!g_pGameConf[GameConf_PTaH]->GetMemSig("FX_FireBullets", (void**)&FX_FireBullets) || !FX_FireBullets)
	{
		smutils->LogError(myself, "Failed to get FX_FireBullets function.");
		return 0;
	}

	/*FX_FireBullets(
		int iPlayerIndex,
		CWeaponCSBaseGun *weapon,
		void *ItemDefinition, // ? может быть 0, тогда находиться по индексу переданому во 2ом параметре
		Vector *vOrigin,
		QAngle *vAngles,
		int iMode,
		int iSeed, 
		float flInaccuracy, // Inaccuracy ?
		float flSpread, // Spread
		float flFishtail, // AccuracyFishtail
		float flNextAttack, // not used
		WeaponSound_t sound_type, // 1 or 12 if silenced
		float m_flRecoilIndex
	);
	static unsigned int offset = 0;
	if(offset == 0)
	{
		if(!UTIL_GetDataMapOffset(pEntity, "m_bLagCompensate", offset))
		{
			smutils->LogError(myself, "Failed to get m_bLagCompensate prop.");
			return 0;
		}
	}*/

	//bool prev = *(bool *)((char *)pEntity + offset);
	//*(bool *)((char *)pEntity + offset) = false;
	FX_FireBullets(params[1], pWeapon, pItem, &Origin, &Angles, params[6], params[7], sp_ctof(params[8]), sp_ctof(params[9]), sp_ctof(params[10]), 0.f, params[11], sp_ctof(params[12]));
	//*(bool *)((char *)pEntity + offset) = prev; /// LagCompensation fix
	return 1;
}

static cell_t PTaH_GetDefinitionIndex(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);
	if (pItemDefinition)
	{
		return pItemDefinition->GetDefinitionIndex();
	}

	return pContext->ThrowNativeError("CEconItemDefinition == NULL");
}

static cell_t PTaH_GetLoadoutSlot(IPluginContext* pContext, const cell_t* params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);
	if (pItemDefinition)
	{
		return pItemDefinition->GetLoadoutSlot(params[2]);
	}

	return pContext->ThrowNativeError("CEconItemDefinition == NULL");
}

static cell_t PTaH_GetNumSupportedStickerSlots(IPluginContext* pContext, const cell_t *params)
{
	CEconItemDefinition* pItemDefinition = reinterpret_cast<CEconItemDefinition*>(params[1]);
	if (pItemDefinition)
	{
		return pItemDefinition->GetNumSupportedStickerSlots();
	}

	return pContext->ThrowNativeError("CEconItemDefinition == NULL");
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

	return pContext->ThrowNativeError("CEconItemDefinition == NULL");
}

static cell_t PTaH_GetCustomPaintKitIndex(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return pItemView->GetCustomPaintKitIndex();
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_GetCustomPaintKitSeed(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return pItemView->GetCustomPaintKitSeed();
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_GetCustomPaintKitWear(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return sp_ftoc(pItemView->GetCustomPaintKitWear(sp_ctof(params[2])));
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
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

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_IsTradable(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return pItemView->IsTradable();
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_IsMarketable(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return pItemView->IsMarketable();
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_GetItemDefinition(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return reinterpret_cast<cell_t>(pItemView->GetItemDefinition());
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_GetAccountID(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return pItemView->GetAccountID();
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_GetQuality(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return pItemView->GetQuality();
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_GetRarity(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return pItemView->GetRarity();
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_GetFlags(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return pItemView->GetFlags();
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_GetOrigin(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return pItemView->GetOrigin();
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
}

static cell_t PTaH_GetKillEater(IPluginContext* pContext, const cell_t* params)
{
	CEconItemView* pItemView = reinterpret_cast<CEconItemView*>(params[1]);
	if (pItemView)
	{
		return pItemView->GetKillEaterValue();
	}

	return pContext->ThrowNativeError("CEconItemView == NULL");
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

	return pContext->ThrowNativeError("CEconItemView == NULL");
}


extern const sp_nativeinfo_t g_ExtensionNatives[] =
{
	{ "PTaH_Version",										PTaH_Version },
	{ "PTaH",												PTaH_ },
	{ "PTaH_GetItemDefinitionByName",						PTaH_GetItemDefinitionByName },
	{ "PTaH_GetItemDefinitionByDefIndex",					PTaH_GetItemDefinitionByDefIndex },
	{ "PTaH_GetItemInLoadout",								PTaH_GetItemInLoadout },
	{ "PTaH_GetEconItemViewFromWeapon",						PTaH_GetEconItemViewFromWeapon },
	{ "PTaH_GivePlayerItem",								PTaH_GivePlayerItem },
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
	{ NULL,													NULL }
};