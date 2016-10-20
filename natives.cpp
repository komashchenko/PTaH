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
#include "tier1/checksum_md5.h"
#include "server_class.h"
#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netdb.h> 
#endif


static cell_t PTaH_(IPluginContext *pContext, const cell_t *params)
{
	if(params[2] == 0)
	{
		switch(params[1])
		{
			case PTaH_GiveNamedItem: return g_pPTaHForwards.m_pGiveNamedItem->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_GiveNamedItemPre: return g_pPTaHForwards.m_pGiveNamedItemPre->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_WeaponCanUse: return g_pPTaHForwards.m_pWeaponCanUse->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_SetPlayerModel: return g_pPTaHForwards.m_pSetModel->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_SetPlayerModelPre: return g_pPTaHForwards.m_pSetModelPre->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_ConsolePrint: return g_pPTaHForwards.m_pClientPrintf->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_MapContentList : return g_pPTaHForwards.m_pMapContentList->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_OnClientConnect : return g_pPTaHForwards.m_pOnClientConnect->AddFunction(pContext, static_cast<funcid_t>(params[3]));
			case PTaH_ExecuteStringCommand : return g_pPTaHForwards.m_pExecuteStringCommand->AddFunction(pContext, static_cast<funcid_t>(params[3]));
		}
	}
	else
	{
		switch(params[1])
		{
			case PTaH_GiveNamedItem: return g_pPTaHForwards.m_pGiveNamedItem->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_GiveNamedItemPre: return g_pPTaHForwards.m_pGiveNamedItemPre->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_WeaponCanUse: return g_pPTaHForwards.m_pWeaponCanUse->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_SetPlayerModel: return g_pPTaHForwards.m_pSetModel->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_SetPlayerModelPre: return g_pPTaHForwards.m_pSetModelPre->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_ConsolePrint: return g_pPTaHForwards.m_pClientPrintf->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_MapContentList : return g_pPTaHForwards.m_pMapContentList->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_OnClientConnect: return g_pPTaHForwards.m_pOnClientConnect->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
			case PTaH_ExecuteStringCommand : return g_pPTaHForwards.m_pExecuteStringCommand->RemoveFunction(pContext->GetFunctionById(static_cast<funcid_t>(params[3])));
		}
	}
	return false;
}

static cell_t PTaH_GetItemDefinitionByName(IPluginContext *pContext, const cell_t *params)
{
	char *strSource; pContext->LocalToString(params[1], &strSource);
	return (cell_t)g_pCEconItemSchema->GetItemDefinitionByName((const char *)strSource);
}

static cell_t PTaH_GetDefinitionIndex(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemDefinition *)params[1])->GetDefinitionIndex();
	return pContext->ThrowNativeError("CEconItemDefinition invalid");
}

static cell_t PTaH_GetLoadoutSlot(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemDefinition *)params[1])->GetLoadoutSlot(params[2]);
	return pContext->ThrowNativeError("CEconItemDefinition invalid");
}

static cell_t PTaH_GetNumSupportedStickerSlots(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemDefinition *)params[1])->GetNumSupportedStickerSlots();
	return pContext->ThrowNativeError("CEconItemDefinition invalid");
}

static cell_t PTaH_GetItemInLoadout(IPluginContext *pContext, const cell_t *params)
{
	if ((params[1] < 1) || (params[1] > playerhelpers->GetMaxClients()))
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}
	CBaseEntity *pEntity;
	if((pEntity = gamehelpers->ReferenceToEntity(params[1])) == NULL)
	{
		return pContext->ThrowNativeError("Client %d is not in game", params[1]);
	}
	if (params[2] != 2 && params[2] != 3)
	{
		return pContext->ThrowNativeError("Team index %d is invalid", params[2]);
	}
	
	static ICallWrapper *pCallWrapper = nullptr;
	static int iInventoryOffset = -1;
	if(!pCallWrapper || iInventoryOffset == -1)
	{
		int offset = -1, byteOffset;
		void *addr = nullptr;
		
		if(!g_pGameConf[GameConf_CSST]->GetOffset("GetItemInLoadout", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GetItemInLoadout offset");
			return 0;
		}
		
		PassInfo pass[2];
		PassInfo ret;
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].type  = PassType_Basic;
		pass[0].size  = sizeof(int);
		pass[1].flags = PASSFLAG_BYVAL;
		pass[1].type  = PassType_Basic;
		pass[1].size  = sizeof(int);

		ret.flags = PASSFLAG_BYVAL;
		ret.type = PassType_Basic;
		ret.size = sizeof(void *);
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, pass, 2);
		
		
		if(!g_pGameConf[GameConf_CSST]->GetOffset("CCSPlayerInventoryOffset", &byteOffset))
		{
			smutils->LogError(myself, "Failed to get CCSPlayerInventoryOffset offset");
			return 0;
		}
		if(!g_pGameConf[GameConf_CSST]->GetMemSig("HandleCommand_Buy_Internal", &addr) || !addr)
		{
			smutils->LogError(myself, "Failed to get HandleCommand_Buy_Internal address");
			return 0;
		}
		iInventoryOffset = *(int *)((intptr_t)addr + byteOffset);
	}
	
	unsigned char vstk[sizeof(void *) + sizeof(int) * 2];
	unsigned char *vptr = vstk;

	*(void **)vptr = (void *)((intptr_t)pEntity + iInventoryOffset);
	vptr += sizeof(void *);
	*(int *)vptr = params[2];
	vptr += sizeof(int);
	*(int *)vptr = params[3];

	CEconItemView *pView = nullptr;
	pCallWrapper->Execute(vstk, &pView);
	return (cell_t)pView;
}

//https://github.com/alliedmodders/sourcemod/blob/0c8e6e29184bf58851954019a2060d84f0c556f9/extensions/sdkhooks/util.cpp#L37
bool UTIL_ContainsDataTable(SendTable *pTable, const char *name)
{
	const char *pname = pTable->GetName();
	int props = pTable->GetNumProps();
	SendProp *prop;
	SendTable *table;

	if (pname && strcmp(name, pname) == 0)
		return true;

	for (int i=0; i<props; i++)
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

static cell_t PTaH_GetEconItemViewFromWeapon(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[1]);
	if(!pEntity)
	{
		return pContext->ThrowNativeError("Entity %d is invalid", params[1]);
	}
	
	IServerNetworkable *pNet = ((IServerUnknown *)pEntity)->GetNetworkable();
	if (!pNet || !UTIL_ContainsDataTable(pNet->GetServerClass()->m_pTable, "DT_BaseCombatWeapon"))
	{
		return pContext->ThrowNativeError("Entity %d is not weapon", params[1]);
	}

#ifdef WIN32
	static int offset = 0;
	
	if (offset == 0 && !g_pGameConf[GameConf_PTaH]->GetOffset("CBaseCombatWeapon::m_hEconItemView", &offset))
	{
		smutils->LogError(myself, "Failed to find offset for CBaseCombatWeapon::m_hEconItemView");
		return 0;
	}
	
	CEconItemView *pView = (CEconItemView *)((uint8 *)pEntity + offset);
	
	return (cell_t)pView;
#else
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		void *addr = nullptr;
		
		if(!g_pGameConf[GameConf_PTaH]->GetMemSig("GetEconItemViewFromWeapon", &addr) || !addr)
		{
			smutils->LogError(myself, "Failed to get GetEconItemViewFromWeapon location");
			return 0;
		}
		
		PassInfo ret;

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(void *);

		pCallWrapper = bintools->CreateCall(addr, CallConv_ThisCall, &ret, NULL, 0);
	}
	
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;
	
	*(void **)vptr = (void *)pEntity;

	CEconItemView *pView = nullptr;
	pCallWrapper->Execute(vstk, &pView);
	return (cell_t)pView;
#endif
}

static cell_t PTaH_GetCustomPaintKitIndex(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemView *)params[1])->GetCustomPaintKitIndex();
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetCustomPaintKitSeed(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemView *)params[1])->GetCustomPaintKitSeed();
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetCustomPaintKitWear(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return sp_ftoc(((CEconItemView *)params[1])->GetCustomPaintKitWear(sp_ctof(params[2])));
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetStickerAttributeBySlotIndex(IPluginContext *pContext, const cell_t *params)
{
	if(params[1])
	{
		if((EStickerAttributeType)params[3] == StickerID) return ((CEconItemView *)params[1])->GetStickerAttributeBySlotIndexInt(params[2], (EStickerAttributeType)params[3], params[4]);
		else return sp_ftoc(((CEconItemView *)params[1])->GetStickerAttributeBySlotIndexFloat(params[2], (EStickerAttributeType)params[3], sp_ctof(params[4])));
	}
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_IsTradable(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemView *)params[1])->IsTradable();
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_IsMarketable(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemView *)params[1])->IsMarketable();
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetItemDefinition(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return (cell_t)((CEconItemView *)params[1])->GetItemDefinition();
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetAccountID(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemView *)params[1])->GetAccountID();
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetQuality(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemView *)params[1])->GetQuality();
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetRarity(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemView *)params[1])->GetRarity();
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetFlags(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemView *)params[1])->GetFlags();
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetOrigin(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemView *)params[1])->GetOrigin();
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetKillEaterValueByType(IPluginContext *pContext, const cell_t *params)
{
	if(params[1]) return ((CEconItemView *)params[1])->GetKillEaterValueByType(0/*0 returns StatTrakKill, the rest is not clear*/);
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GetCustomName(IPluginContext *pContext, const cell_t *params)
{
	if(params[1])
	{
		size_t numBytes;
		char *buf = ((CEconItemView *)params[1])->GetCustomName();
		pContext->StringToLocalUTF8(params[2], params[3], (buf && buf[0]) ? buf : "", &numBytes);
		return numBytes;
	}
	return pContext->ThrowNativeError("CEconItemView invalid");
}

static cell_t PTaH_GivePlayerItem(IPluginContext *pContext, const cell_t *params)
{
	if ((params[1] < 1) || (params[1] > playerhelpers->GetMaxClients()))
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}
	CBaseEntity *pEntity;
	if((pEntity = gamehelpers->ReferenceToEntity(params[1])) == NULL)
	{
		return pContext->ThrowNativeError("Client %d is not in game", params[1]);
	}
	char *strSource; pContext->LocalToString(params[2], &strSource);
	cell_t pViewB = params[3];
	
	static ICallWrapper *pCallWrapper = nullptr;
	if(!pCallWrapper)
	{
		int offset = -1;
		
		if(!g_pGameConf[GameConf_SDKT]->GetOffset("GiveNamedItem", &offset) || offset == -1)
		{
			smutils->LogError(myself, "Failed to get GiveNamedItem offset");
			return -1;
		}
		
		PassInfo pass[4];
		PassInfo ret;
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].type  = PassType_Basic;
		pass[0].size  = sizeof(const char *);
		pass[1].flags = PASSFLAG_BYVAL;
		pass[1].type  = PassType_Basic;
		pass[1].size  = sizeof(int);
		pass[2].flags = PASSFLAG_BYVAL;
		pass[2].type  = PassType_Basic;
		pass[2].size  = sizeof(void *);
		pass[3].flags = PASSFLAG_BYVAL;
		pass[3].type  = PassType_Basic;
		pass[3].size  = sizeof(bool);

		ret.flags = PASSFLAG_BYREF;
		ret.type = PassType_Basic;
		ret.size = sizeof(CBaseEntity *);
		
		pCallWrapper = bintools->CreateVCall(offset, 0, 0, &ret, pass, 4);
	}
	
	unsigned char vstk[sizeof(void *) * 2 + sizeof(const char *) + sizeof(bool) + sizeof(int)];
	unsigned char *vptr = vstk;

	*(void **)vptr = pEntity;
	vptr += sizeof(void *);
	*(const char **)vptr = strSource;
	vptr += sizeof(const char *);
	*(void **)vptr = (CEconItemView *)params[3];
	vptr += sizeof(void *);
	*(int *)vptr = 0;
	vptr += sizeof(int);
	*(bool *)vptr = false;

	CBaseEntity *pEntityW = nullptr;
	pCallWrapper->Execute(vstk, &pEntityW);
	return gamehelpers->EntityToBCompatRef(pEntityW);
}

static cell_t PTaH_MD5File(IPluginContext *pContext, const cell_t *params)
{
	FILE *pFile;
	MD5Context_t context;
	unsigned char digest[16], temp[1024];
	unsigned int len, sum = 0;

	char *filename;
	pContext->LocalToString(params[1], &filename);
	const char *path = g_pSM->GetGamePath();

	char filepath[1024];
	g_pSM->Format(filepath, 1024, "%s/%s", path, filename);

	pFile = fopen(filepath, "rb");
	if (!pFile)
	{
		pContext->ThrowNativeError("File \"%s\" can not be open!", filepath);
		return 0;
	}

	MD5Init(&context);

	while ((len = fread(temp, 1, 1024, pFile)) > 0) 
	{
		MD5Update(&context, temp, len);
		sum += len;
	}

	MD5Final(digest, &context);
	fclose(pFile);

	char *buffer;
	pContext->LocalToString(params[2], &buffer);

	g_pSM->Format(buffer, params[3], "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7], digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], digest[14], digest[15]);
	for(int i = 0; i < params[3]; i++) buffer[i] = toupper(buffer[i]);
	return 1;
}

cell_t PTaH_GetHostByName(IPluginContext *pContext, const cell_t *params) 
{ 
	char * pHostName;  pContext->LocalToString(params[1], &pHostName); 
	struct hostent * pHostData; 
	if ((pHostData = gethostbyname(pHostName)) == NULL) return 0; 
	return *((unsigned long *)pHostData->h_addr);
}


extern const sp_nativeinfo_t g_ExtensionNatives[] =
{
	{ "PTaH",												PTaH_ },
	{ "PTaH_GetItemDefinitionByName",						PTaH_GetItemDefinitionByName },
	{ "CEconItemDefinition.GetDefinitionIndex",				PTaH_GetDefinitionIndex },
	{ "CEconItemDefinition.GetLoadoutSlot",					PTaH_GetLoadoutSlot },
	{ "CEconItemDefinition.GetNumSupportedStickerSlots",	PTaH_GetNumSupportedStickerSlots },
	{ "PTaH_GetItemInLoadout",								PTaH_GetItemInLoadout },
	{ "PTaH_GetEconItemViewFromWeapon",						PTaH_GetEconItemViewFromWeapon },
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
	{ "CEconItemView.GetStatTrakKill",						PTaH_GetKillEaterValueByType },
	{ "PTaH_GivePlayerItem",								PTaH_GivePlayerItem },
	{ "PTaH_MD5File",										PTaH_MD5File },
	{ "PTaH_GetHostByName",									PTaH_GetHostByName },
	{ NULL,													NULL }
};
