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
#include "forwards.h"
#include "classes.h"
#include "natives.h"


CForwardManager g_ForwardManager;


//LevelShutdown
SH_DECL_HOOK0_void(IServerGameDLL, LevelShutdown, SH_NOATTRIB, false);

//GiveNamedItem
SH_DECL_MANUALHOOK5(GiveNamedItem, 0, 0, 0, CBaseEntity*, const char*, int, CEconItemView*, bool, Vector*);

//WeaponCanUse
SH_DECL_MANUALHOOK1(Weapon_CanUse, 0, 0, 0, bool, CBaseCombatWeapon*);

//SetModel
SH_DECL_MANUALHOOK1(SetEntityModel, 0, 0, 0, CBaseEntity*, const char*);

//ClientVoiceTo
SH_DECL_HOOK3(IVoiceServer, SetClientListening, SH_NOATTRIB, 0, bool, int, int, bool);
SH_DECL_HOOK1_void(IServerGameClients, ClientVoice, SH_NOATTRIB, 0, edict_t*);

//ClientPrint
SH_DECL_MANUALHOOK0_void_vafmt(ClientPrintf, 0, 0, 0);

//ExecuteStringCommand
SH_DECL_MANUALHOOK1(ExecuteStringCommand, 0, 0, 0, bool, const char*);

//ClientConnect
SH_DECL_MANUALHOOK13(ConnectClient, 0, 0, 0, IClient*, const netadr_t&, int, int, int, const char*, const char*, const char*, int, CUtlVector<NetMsg_SplitPlayerConnect*>&, bool, CrossPlayPlatform_t, const unsigned char*, int);
SH_DECL_MANUALHOOK1_void_vafmt(RejectConnection, 0, 0, 0, const netadr_t&);

//InventoryUpdate
SH_DECL_MANUALHOOK0_void(SendInventoryUpdateEvent, 0, 0, 0);


void CForwardManager::Init()
{
	GiveNamedItemPre.Init();
	GiveNamedItemPost.Init();
	WeaponCanUsePre.Init();
	WeaponCanUsePost.Init();
	SetPlayerModelPre.Init();
	SetPlayerModelPost.Init();
	ClientVoiceToPre.Init();
	ClientVoiceToPost.Init();
	ConsolePrintPre.Init();
	ConsolePrintPost.Init();
	ExecuteStringCommandPre.Init();
	ExecuteStringCommandPost.Init();
	ClientConnectPre.Init();
	ClientConnectPost.Init();
	InventoryUpdatePost.Init();

	SH_ADD_HOOK(IServerGameDLL, LevelShutdown, gamedll, SH_MEMBER(this, &CForwardManager::LevelShutdown), true);

	playerhelpers->AddClientListener(this);
	plsys->AddPluginsListener(this);
}

void CForwardManager::Shutdown()
{
	playerhelpers->RemoveClientListener(this);
	plsys->RemovePluginsListener(this);

	SH_REMOVE_HOOK(IServerGameDLL, LevelShutdown, gamedll, SH_MEMBER(this, &CForwardManager::LevelShutdown), true);

	for (int i = 1; i <= SM_MAXPLAYERS; i++)
	{
		OnClientDisconnected(i);
	}

	GiveNamedItemPre.Shutdown();
	GiveNamedItemPost.Shutdown();
	WeaponCanUsePre.Shutdown();
	WeaponCanUsePost.Shutdown();
	SetPlayerModelPre.Shutdown();
	SetPlayerModelPost.Shutdown();
	ClientVoiceToPre.Shutdown();
	ClientVoiceToPost.Shutdown();
	ConsolePrintPre.Shutdown();
	ConsolePrintPost.Shutdown();
	ExecuteStringCommandPre.Shutdown();
	ExecuteStringCommandPost.Shutdown();
	ClientConnectPre.Shutdown();
	ClientConnectPost.Shutdown();
	InventoryUpdatePost.Shutdown();
}

bool CForwardManager::FunctionUpdateHook(PTaH_HookEvent htType, IPluginFunction* pFunction, bool bHook)
{
	switch (htType)
	{
	case PTaH_GiveNamedItemPre: return GiveNamedItemPre.UpdateForward(pFunction, bHook);
	case PTaH_GiveNamedItemPost: return GiveNamedItemPost.UpdateForward(pFunction, bHook);
	case PTaH_WeaponCanUsePre: return WeaponCanUsePre.UpdateForward(pFunction, bHook);
	case PTaH_WeaponCanUsePost: return WeaponCanUsePost.UpdateForward(pFunction, bHook);
	case PTaH_SetPlayerModelPre: return SetPlayerModelPre.UpdateForward(pFunction, bHook);
	case PTaH_SetPlayerModelPost: return SetPlayerModelPost.UpdateForward(pFunction, bHook);
	case PTaH_ClientVoiceToPre: return ClientVoiceToPre.UpdateForward(pFunction, bHook);
	case PTaH_ClientVoiceToPost: return ClientVoiceToPost.UpdateForward(pFunction, bHook);
	case PTaH_ConsolePrintPre: return ConsolePrintPre.UpdateForward(pFunction, bHook);
	case PTaH_ConsolePrintPost: return ConsolePrintPost.UpdateForward(pFunction, bHook);
	case PTaH_ExecuteStringCommandPre: return ExecuteStringCommandPre.UpdateForward(pFunction, bHook);
	case PTaH_ExecuteStringCommandPost: return ExecuteStringCommandPost.UpdateForward(pFunction, bHook);
	case PTaH_ClientConnectPre: return ClientConnectPre.UpdateForward(pFunction, bHook);
	case PTaH_ClientConnectPost: return ClientConnectPost.UpdateForward(pFunction, bHook);
	case PTaH_InventoryUpdatePost: return InventoryUpdatePost.UpdateForward(pFunction, bHook);
	}

	return false;
}

void CForwardManager::LevelShutdown()
{
	for (int i = 1; i <= SM_MAXPLAYERS; i++)
	{
		OnClientDisconnected(i);
	}

	RETURN_META(MRES_IGNORED);
}

void CForwardManager::OnClientConnected(int iClient)
{
	ConsolePrintPre.Hook(iClient);
	ConsolePrintPost.Hook(iClient);
	ExecuteStringCommandPre.Hook(iClient);
	ExecuteStringCommandPost.Hook(iClient);
}

void CForwardManager::OnClientPutInServer(int iClient)
{
	GiveNamedItemPre.HookClient(iClient);
	GiveNamedItemPost.HookClient(iClient);
	WeaponCanUsePre.HookClient(iClient);
	WeaponCanUsePost.HookClient(iClient);
	SetPlayerModelPre.HookClient(iClient);
	SetPlayerModelPost.HookClient(iClient);
	InventoryUpdatePost.Hook(iClient);
}

void CForwardManager::OnClientDisconnected(int iClient)
{
	GiveNamedItemPre.UnHookClient(iClient);
	GiveNamedItemPost.UnHookClient(iClient);
	WeaponCanUsePre.UnHookClient(iClient);
	WeaponCanUsePost.UnHookClient(iClient);
	SetPlayerModelPre.UnHookClient(iClient);
	SetPlayerModelPost.UnHookClient(iClient);
}

void CForwardManager::OnPluginUnloaded(IPlugin* plugin)
{
	GiveNamedItemPre.UpdateHook();
	GiveNamedItemPost.UpdateHook();
	WeaponCanUsePre.UpdateHook();
	WeaponCanUsePost.UpdateHook();
	SetPlayerModelPre.UpdateHook();
	SetPlayerModelPost.UpdateHook();
	ClientVoiceToPre.UpdateHook();
	ClientVoiceToPost.UpdateHook();
	ConsolePrintPre.UpdateHook();
	ConsolePrintPost.UpdateHook();
	ExecuteStringCommandPre.UpdateHook();
	ExecuteStringCommandPost.UpdateHook();
	ClientConnectPre.UpdateHook();
	ClientConnectPost.UpdateHook();
	InventoryUpdatePost.UpdateHook();
}

CForwardManager::TempleHookClient::TempleHookClient()
{
	memset(iHookId, 0xFF, sizeof(iHookId));
}

void CForwardManager::TempleHookClient::Shutdown()
{
	if (iOffset != -1) forwards->ReleaseForward(pForward);
}

void CForwardManager::TempleHookClient::UpdateHook()
{
	if (iOffset != -1 && (pForward->GetFunctionCount() > 0) != bHooked)
	{
		bHooked = !bHooked;

		int iMaxClients = playerhelpers->GetMaxClients();

		if (bHooked)
		{
			IGamePlayer* pPlayer;

			for (int i = 1; i <= iMaxClients; i++) if ((pPlayer = playerhelpers->GetGamePlayer(i)) && pPlayer->IsInGame())
			{
				HookClient(i);
			}
		}
		else
		{
			for (int i = 1; i <= iMaxClients; i++) UnHookClient(i);
		}
	}
}

void CForwardManager::TempleHookClient::HookClient(int iClient)
{
	if (bHooked && iHookId[iClient] == -1)
	{
		CBaseEntity* pEnt = gamehelpers->ReferenceToEntity(iClient);

		iHookId[iClient] = __SH_ADD_MANUALHOOK(pEnt);
	}
}

void CForwardManager::TempleHookClient::UnHookClient(int iClient)
{
	if (iHookId[iClient] != -1)
	{
		SH_REMOVE_HOOK_ID(iHookId[iClient]);

		iHookId[iClient] = -1;
	}
}

bool CForwardManager::TempleHookClient::UpdateForward(IPluginFunction* pFunc, bool bHook)
{
	if (iOffset != -1)
	{
		bool bBuf;

		if (bHook) bBuf = pForward->AddFunction(pFunc);
		else bBuf = pForward->RemoveFunction(pFunc);

		UpdateHook();

		return bBuf;
	}

	return false;
}

void CForwardManager::TempleHookVP::Shutdown()
{
	if (iOffset != -1)
	{
		forwards->ReleaseForward(pForward);

		if (iHookId != -1) SH_REMOVE_HOOK_ID(iHookId);
	}
}

bool CForwardManager::TempleHookVP::UpdateForward(IPluginFunction* pFunc, bool bHook)
{
	if (iOffset != -1)
	{
		bool bBuf;

		if (bHook) bBuf = pForward->AddFunction(pFunc);
		else bBuf = pForward->RemoveFunction(pFunc);

		UpdateHook();

		return bBuf;
	}

	return false;
}

void CForwardManager::TempleHookVP::UpdateHook()
{
	if (iOffset != -1 && (pForward->GetFunctionCount() > 0) != bHooked)
	{
		bHooked = !bHooked;

		if (bHooked)
		{
			IGamePlayer* pPlayer;
			int iMaxClients = playerhelpers->GetMaxClients();

			auto fPlayerValid = bInGame ? &IGamePlayer::IsInGame : &IGamePlayer::IsConnected;

			for (int i = 1; i <= iMaxClients; i++) if ((pPlayer = playerhelpers->GetGamePlayer(i)) && (pPlayer->*fPlayerValid)())
			{
				Hook(i);

				return;
			}
		}
		else if (iHookId != -1)
		{
			SH_REMOVE_HOOK_ID(iHookId);

			iHookId = -1;
		}
	}
}

void CForwardManager::TempleHookBaseClient::Hook(int iClient)
{
	if (bHooked && iHookId == -1)
	{
		IClient* pClient = iserver->GetClient(iClient - 1);

		iHookId = __SH_ADD_MANUALVPHOOK(static_cast<CBaseClient*>(pClient));
	}
}

DETOUR_DECL_MEMBER4(CCSPlayer_FindMatchingWeaponsForTeamLoadout, uint64_t, const char*, szItem, int, iTeam, bool, bMustBeTeamSpecific, CUtlVector<CEconItemView*>&, matchingWeapons)
{
	if (g_ForwardManager.GiveNamedItemPre.bIgnoredCEconItemView)
	{
		g_ForwardManager.GiveNamedItemPre.bIgnoredCEconItemView = false;

		return 0LL;
	}

	return DETOUR_MEMBER_CALL(CCSPlayer_FindMatchingWeaponsForTeamLoadout)(szItem, iTeam, bMustBeTeamSpecific, matchingWeapons);
}

CBaseEntity* CForwardManager::GiveNamedItemPre::SHHook(const char* szItem, int iSubType, CEconItemView* pView, bool removeIfNotCarried, Vector* pOrigin)
{
	cell_t res = Pl_Continue;
	cell_t pViewNew = reinterpret_cast<cell_t>(pView);
	cell_t IgnoredCEconItemViewNew = false;
	cell_t OriginIsnullptr = pOrigin == nullptr;
	cell_t Origin[3] = { 0, 0, 0 };
	char szItemByf[64];

	V_strncpy(szItemByf, szItem, sizeof(szItemByf));

	if (pOrigin)
	{
		Origin[0] = sp_ftoc(pOrigin->x);
		Origin[1] = sp_ftoc(pOrigin->y);
		Origin[2] = sp_ftoc(pOrigin->z);
	}

	pForward->PushCell(gamehelpers->EntityToBCompatRef(META_IFACEPTR(CBaseEntity)));
	pForward->PushStringEx(szItemByf, sizeof(szItemByf), SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
	pForward->PushCellByRef(&pViewNew);
	pForward->PushCellByRef(&IgnoredCEconItemViewNew);
	pForward->PushCellByRef(&OriginIsnullptr);
	pForward->PushArray(Origin, 3, SM_PARAM_COPYBACK);
	pForward->Execute(&res);

	if (res != Pl_Continue)
	{
		if (res == Pl_Changed)
		{
			if (IgnoredCEconItemViewNew)
			{
				bIgnoredCEconItemView = true;
				pViewNew = 0;
			}

			Vector OriginNew; OriginNew.Invalidate();

			if (OriginIsnullptr == false)
			{
				OriginNew.x = sp_ctof(Origin[0]);
				OriginNew.y = sp_ctof(Origin[1]);
				OriginNew.z = sp_ctof(Origin[2]);
			}

			RETURN_META_VALUE_MNEWPARAMS(MRES_HANDLED, nullptr, GiveNamedItem, (const_cast<const char*>(szItemByf), iSubType, reinterpret_cast<CEconItemView*>(pViewNew), removeIfNotCarried, OriginNew.IsValid() ? &OriginNew : nullptr));
		}
		else RETURN_META_VALUE(MRES_SUPERCEDE, nullptr);
	}

	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

void CForwardManager::GiveNamedItemPre::Init()
{
	if (g_pGameConf[GameConf_SDKT]->GetOffset("GiveNamedItem", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(GiveNamedItem, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 6, nullptr, Param_Cell, Param_String, Param_CellByRef, Param_CellByRef, Param_CellByRef, Param_Array);
		pFindMatchingWeaponsForTeamLoadout = DETOUR_CREATE_MEMBER(CCSPlayer_FindMatchingWeaponsForTeamLoadout, "CCSPlayer::FindMatchingWeaponsForTeamLoadout");

		if (!pFindMatchingWeaponsForTeamLoadout) smutils->LogError(myself, "Detour failed CCSPlayer::FindMatchingWeaponsForTeamLoadout, functionality GiveNamedItemPre will be limited.");
	}
	else smutils->LogError(myself, "Failed to get GiveNamedItem offset, Hook GiveNamedItemPre will be unavailable.");
}

void CForwardManager::GiveNamedItemPre::Shutdown()
{
	if (iOffset != -1)
	{
		if (pFindMatchingWeaponsForTeamLoadout) pFindMatchingWeaponsForTeamLoadout->Destroy();

		forwards->ReleaseForward(pForward);
	}
}

void CForwardManager::GiveNamedItemPre::UpdateHook()
{
	if (iOffset != -1 && (pForward->GetFunctionCount() > 0) != bHooked)
	{
		bHooked = !bHooked;

		int iMaxClients = playerhelpers->GetMaxClients();

		if (bHooked)
		{
			if (pFindMatchingWeaponsForTeamLoadout) pFindMatchingWeaponsForTeamLoadout->EnableDetour();

			IGamePlayer* pPlayer;

			for (int i = 1; i <= iMaxClients; i++) if ((pPlayer = playerhelpers->GetGamePlayer(i)) && pPlayer->IsInGame())
			{
				HookClient(i);
			}
		}
		else
		{
			if (pFindMatchingWeaponsForTeamLoadout) pFindMatchingWeaponsForTeamLoadout->DisableDetour();

			for (int i = 1; i <= iMaxClients; i++) UnHookClient(i);
		}
	}
}

int CForwardManager::GiveNamedItemPre::__SH_ADD_MANUALHOOK(CBaseEntity* pEnt)
{
	return SH_ADD_MANUALHOOK(GiveNamedItem, pEnt, SH_MEMBER(this, &CForwardManager::GiveNamedItemPre::SHHook), false);
}

void CForwardManager::GiveNamedItemPost::Init()
{
	if (g_pGameConf[GameConf_SDKT]->GetOffset("GiveNamedItem", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(GiveNamedItem, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 6, nullptr, Param_Cell, Param_String, Param_Cell, Param_Cell, Param_Cell, Param_Array);
	}
	else smutils->LogError(myself, "Failed to get GiveNamedItem offset, Hook GiveNamedItemPost will be unavailable.");
}

int CForwardManager::GiveNamedItemPost::__SH_ADD_MANUALHOOK(CBaseEntity* pEnt)
{
	return SH_ADD_MANUALHOOK(GiveNamedItem, pEnt, SH_MEMBER(this, &CForwardManager::GiveNamedItemPost::SHHook), true);
}

CBaseEntity* CForwardManager::GiveNamedItemPost::SHHook(const char* szItem, int iSubType, CEconItemView* pView, bool removeIfNotCarried, Vector* pOrigin)
{
	CBaseEntity* pEnt = META_IFACEPTR(CBaseEntity);

	cell_t Origin[3] = { 0, 0, 0 };

	if (pOrigin)
	{
		Origin[0] = sp_ftoc(pOrigin->x);
		Origin[1] = sp_ftoc(pOrigin->y);
		Origin[2] = sp_ftoc(pOrigin->z);
	}

	pForward->PushCell(gamehelpers->EntityToBCompatRef(pEnt));
	pForward->PushString(szItem);
	pForward->PushCell(reinterpret_cast<cell_t>(pView));
	pForward->PushCell(gamehelpers->EntityToBCompatRef(META_RESULT_ORIG_RET(CBaseEntity*)));
	pForward->PushCell(pOrigin == nullptr);
	pForward->PushArray(Origin, 3);
	pForward->Execute(nullptr);

	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

void CForwardManager::WeaponCanUsePre::Init()
{
	if (g_pGameConf[GameConf_SDKH]->GetOffset("Weapon_CanUse", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(Weapon_CanUse, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 3, nullptr, Param_Cell, Param_Cell, Param_CellByRef);
	}
	else smutils->LogError(myself, "Failed to get Weapon_CanUse offset, Hook WeaponCanUsePre will be unavailable.");
}

int CForwardManager::WeaponCanUsePre::__SH_ADD_MANUALHOOK(CBaseEntity* pEnt)
{
	return SH_ADD_MANUALHOOK(Weapon_CanUse, pEnt, SH_MEMBER(this, &CForwardManager::WeaponCanUsePre::SHHook), false);
}

bool CForwardManager::WeaponCanUsePre::SHHook(CBaseCombatWeapon* pWeapon)
{
	cell_t res = Pl_Continue;
	cell_t ret = META_RESULT_ORIG_RET(bool);

	pForward->PushCell(gamehelpers->EntityToBCompatRef(META_IFACEPTR(CBaseEntity)));
	pForward->PushCell(gamehelpers->EntityToBCompatRef(pWeapon));
	pForward->PushCellByRef(&ret);
	pForward->Execute(&res);

	if (res != Pl_Continue)
	{
		if (res == Pl_Changed) RETURN_META_VALUE(MRES_SUPERCEDE, static_cast<bool>(ret));
		else RETURN_META_VALUE(MRES_SUPERCEDE, false);
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

void CForwardManager::WeaponCanUsePost::Init()
{
	if (g_pGameConf[GameConf_SDKH]->GetOffset("Weapon_CanUse", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(Weapon_CanUse, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 3, nullptr, Param_Cell, Param_Cell, Param_Cell);
	}
	else smutils->LogError(myself, "Failed to get Weapon_CanUse offset, Hook WeaponCanUsePost will be unavailable.");
}

int CForwardManager::WeaponCanUsePost::__SH_ADD_MANUALHOOK(CBaseEntity* pEnt)
{
	return SH_ADD_MANUALHOOK(Weapon_CanUse, pEnt, SH_MEMBER(this, &CForwardManager::WeaponCanUsePost::SHHook), true);
}

bool CForwardManager::WeaponCanUsePost::SHHook(CBaseCombatWeapon* pWeapon)
{
	pForward->PushCell(gamehelpers->EntityToBCompatRef(META_IFACEPTR(CBaseEntity)));
	pForward->PushCell(gamehelpers->EntityToBCompatRef(pWeapon));
	pForward->PushCell(META_RESULT_ORIG_RET(bool));
	pForward->Execute(nullptr);

	RETURN_META_VALUE(MRES_IGNORED, true);
}

void CForwardManager::SetPlayerModelPre::Init()
{
	if (g_pGameConf[GameConf_SDKT]->GetOffset("SetEntityModel", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(SetEntityModel, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 3, nullptr, Param_Cell, Param_String, Param_String);
	}
	else smutils->LogError(myself, "Failed to get SetEntityModel offset, Hook SetPlayerModelPre will be unavailable.");
}

int CForwardManager::SetPlayerModelPre::__SH_ADD_MANUALHOOK(CBaseEntity* pEnt)
{
	return SH_ADD_MANUALHOOK(SetEntityModel, pEnt, SH_MEMBER(this, &CForwardManager::SetPlayerModelPre::SHHook), false);
}

CBaseEntity* CForwardManager::SetPlayerModelPre::SHHook(const char* sModel)
{
	cell_t res = Pl_Continue;
	CBaseEntity* pEnt = META_IFACEPTR(CBaseEntity);
	char sModelNew[256];

	V_strncpy(sModelNew, sModel, sizeof(sModelNew));

	pForward->PushCell(gamehelpers->EntityToBCompatRef(pEnt));
	pForward->PushString(playerhelpers->GetGamePlayer(gamehelpers->EntityToBCompatRef(pEnt))->GetPlayerInfo()->GetModelName());
	pForward->PushStringEx(sModelNew, sizeof(sModelNew), SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
	pForward->Execute(&res);

	if (res != Pl_Continue)
	{
		if (res == Pl_Changed) RETURN_META_VALUE_MNEWPARAMS(MRES_HANDLED, nullptr, SetEntityModel, (const_cast<const char*>(sModelNew)));
		else RETURN_META_VALUE(MRES_SUPERCEDE, nullptr);
	}

	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

void CForwardManager::SetPlayerModelPost::Init()
{
	if (g_pGameConf[GameConf_SDKT]->GetOffset("SetEntityModel", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(SetEntityModel, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 2, nullptr, Param_Cell, Param_String);
	}
	else smutils->LogError(myself, "Failed to get SetEntityModel offset, Hook SetPlayerModelPost will be unavailable.");
}

int CForwardManager::SetPlayerModelPost::__SH_ADD_MANUALHOOK(CBaseEntity* pEnt)
{
	return SH_ADD_MANUALHOOK(SetEntityModel, pEnt, SH_MEMBER(this, &CForwardManager::SetPlayerModelPost::SHHook), true);
}

CBaseEntity* CForwardManager::SetPlayerModelPost::SHHook(const char* sModel)
{
	pForward->PushCell(gamehelpers->EntityToBCompatRef(META_IFACEPTR(CBaseEntity)));
	pForward->PushString(sModel);
	pForward->Execute(nullptr);

	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

void CForwardManager::ClientVoiceToPre::Init()
{
	if (g_pCPlayerVoiceListener) pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 3, nullptr, Param_Cell, Param_Cell, Param_CellByRef);
	else smutils->LogError(myself, "g_pCPlayerVoiceListener == nullptr, Hook ClientVoiceToPre will be unavailable.");
}

void CForwardManager::ClientVoiceToPre::Shutdown()
{
	if (g_pCPlayerVoiceListener)
	{
		if (bHooked)
		{
			SH_REMOVE_HOOK(IVoiceServer, SetClientListening, voiceserver, SH_MEMBER(this, &CForwardManager::ClientVoiceToPre::SHHook), false);
			SH_REMOVE_HOOK(IServerGameClients, ClientVoice, serverClients, SH_MEMBER(this, &CForwardManager::ClientVoiceToPre::SHHookClientVoice), false);
		}

		forwards->ReleaseForward(pForward);
	}
}

bool CForwardManager::ClientVoiceToPre::UpdateForward(IPluginFunction* pFunc, bool bHook)
{
	if (g_pCPlayerVoiceListener)
	{
		bool bBuf;

		if (bHook) bBuf = pForward->AddFunction(pFunc);
		else bBuf = pForward->RemoveFunction(pFunc);

		UpdateHook();

		return bBuf;
	}

	return false;
}

void CForwardManager::ClientVoiceToPre::UpdateHook()
{
	if (g_pCPlayerVoiceListener && (pForward->GetFunctionCount() > 0) != bHooked)
	{
		bHooked = !bHooked;

		if (bHooked)
		{
			SH_ADD_HOOK(IVoiceServer, SetClientListening, voiceserver, SH_MEMBER(this, &CForwardManager::ClientVoiceToPre::SHHook), false);
			SH_ADD_HOOK(IServerGameClients, ClientVoice, serverClients, SH_MEMBER(this, &CForwardManager::ClientVoiceToPre::SHHookClientVoice), false);
		}
		else
		{
			SH_REMOVE_HOOK(IVoiceServer, SetClientListening, voiceserver, SH_MEMBER(this, &CForwardManager::ClientVoiceToPre::SHHook), false);
			SH_REMOVE_HOOK(IServerGameClients, ClientVoice, serverClients, SH_MEMBER(this, &CForwardManager::ClientVoiceToPre::SHHookClientVoice), false);
		}
	}
}

bool CForwardManager::ClientVoiceToPre::SHHook(int iReceiver, int iSender, bool bListen)
{
	//IsPlayerSpeaking is required to protect plugins from useless spam, but this has its disadvantage because it needs another hook.
	if (iReceiver != iSender && (bStartVoice[iSender] || g_pCPlayerVoiceListener->IsPlayerSpeaking(iSender)))
	{
		cell_t res = Pl_Continue;
		cell_t ret = bListen;

		pForward->PushCell(iSender);
		pForward->PushCell(iReceiver);
		pForward->PushCellByRef(&ret);
		pForward->Execute(&res);

		if (res != Pl_Continue)
		{
			if (res == Pl_Changed) RETURN_META_VALUE_NEWPARAMS(MRES_HANDLED, true, &IVoiceServer::SetClientListening, (iReceiver, iSender, ret));
			else RETURN_META_VALUE_NEWPARAMS(MRES_HANDLED, true, &IVoiceServer::SetClientListening, (iReceiver, iSender, false));
		}
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

void CForwardManager::ClientVoiceToPre::SHHookClientVoice(edict_t* pEdict)
{
	int iSender = gamehelpers->IndexOfEdict(pEdict);

	if (!g_pCPlayerVoiceListener->IsPlayerSpeaking(iSender))
	{
		bStartVoice[iSender] = true;

		IGamePlayer* pPlayer;
		int iMaxClients = playerhelpers->GetMaxClients();

		for (int iReceiver = 1; iReceiver <= iMaxClients; iReceiver++) if ((pPlayer = playerhelpers->GetGamePlayer(iReceiver)) && pPlayer->IsInGame())
		{
			voiceserver->SetClientListening(iReceiver, iSender, voiceserver->GetClientListening(iReceiver, iSender));
		}

		bStartVoice[iSender] = false;
	}

	RETURN_META(MRES_IGNORED);
}

void CForwardManager::ClientVoiceToPost::Init()
{
	if (g_pCPlayerVoiceListener) pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 3, nullptr, Param_Cell, Param_Cell, Param_Cell);
	else smutils->LogError(myself, "g_pCPlayerVoiceListener == nullptr, Hook ClientVoiceToPost will be unavailable.");
}

void CForwardManager::ClientVoiceToPost::Shutdown()
{
	if (g_pCPlayerVoiceListener)
	{
		if (bHooked) SH_REMOVE_HOOK(IVoiceServer, SetClientListening, voiceserver, SH_MEMBER(this, &CForwardManager::ClientVoiceToPost::SHHook), true);

		forwards->ReleaseForward(pForward);
	}
}

bool CForwardManager::ClientVoiceToPost::UpdateForward(IPluginFunction* pFunc, bool bHook)
{
	if (g_pCPlayerVoiceListener)
	{
		bool bBuf;

		if (bHook) bBuf = pForward->AddFunction(pFunc);
		else bBuf = pForward->RemoveFunction(pFunc);

		UpdateHook();

		return bBuf;
	}

	return false;
}

void CForwardManager::ClientVoiceToPost::UpdateHook()
{
	if (g_pCPlayerVoiceListener && (pForward->GetFunctionCount() > 0) != bHooked)
	{
		bHooked = !bHooked;

		if (bHooked) SH_ADD_HOOK(IVoiceServer, SetClientListening, voiceserver, SH_MEMBER(this, &CForwardManager::ClientVoiceToPost::SHHook), true);
		else SH_REMOVE_HOOK(IVoiceServer, SetClientListening, voiceserver, SH_MEMBER(this, &CForwardManager::ClientVoiceToPost::SHHook), true);
	}
}

bool CForwardManager::ClientVoiceToPost::SHHook(int iReceiver, int iSender, bool bListen)
{
	if (iReceiver != iSender && g_pCPlayerVoiceListener->IsPlayerSpeaking(iSender))
	{
		pForward->PushCell(iSender);
		pForward->PushCell(iReceiver);
		pForward->PushCell(bListen);
		pForward->Execute(nullptr);
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

void CForwardManager::ConsolePrintPre::Init()
{
	if (g_pGameConf[GameConf_PTaH]->GetOffset("CBaseClient::ClientPrintf", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(ClientPrintf, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 2, nullptr, Param_Cell, Param_String);
	}
	else smutils->LogError(myself, "Failed to get CBaseClient::ClientPrintf offset, Hook ConsolePrintPre will be unavailable.");
}

int CForwardManager::ConsolePrintPre::__SH_ADD_MANUALVPHOOK(CBaseClient* pBaseClient)
{
	return SH_ADD_MANUALVPHOOK(ClientPrintf, pBaseClient, SH_MEMBER(this, &CForwardManager::ConsolePrintPre::SHHook), false);
}

void CForwardManager::ConsolePrintPre::SHHook(const char* szFormat)
{
	cell_t res = Pl_Continue;
	char cMsg[1024];

	V_strncpy(cMsg, szFormat, sizeof(cMsg));

	pForward->PushCell(META_IFACEPTR(CBaseClient)->GetPlayerSlot() + 1);
	pForward->PushStringEx(cMsg, sizeof(cMsg), SM_PARAM_STRING_UTF8 | SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
	pForward->Execute(&res);

	if (res != Pl_Continue)
	{
		if (res == Pl_Changed) RETURN_META_MNEWPARAMS(MRES_HANDLED, ClientPrintf, (const_cast<const char*>(cMsg)));
		else RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void CForwardManager::ConsolePrintPost::Init()
{
	if (g_pGameConf[GameConf_PTaH]->GetOffset("CBaseClient::ClientPrintf", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(ClientPrintf, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 2, nullptr, Param_Cell, Param_String);
	}
	else smutils->LogError(myself, "Failed to get CBaseClient::ClientPrintf offset, Hook ConsolePrintPost will be unavailable.");
}

int CForwardManager::ConsolePrintPost::__SH_ADD_MANUALVPHOOK(CBaseClient* pBaseClient)
{
	return SH_ADD_MANUALVPHOOK(ClientPrintf, pBaseClient, SH_MEMBER(this, &CForwardManager::ConsolePrintPost::SHHook), true);
}

void CForwardManager::ConsolePrintPost::SHHook(const char* szFormat)
{
	pForward->PushCell(META_IFACEPTR(CBaseClient)->GetPlayerSlot() + 1);
	pForward->PushString(szFormat);
	pForward->Execute(nullptr);

	RETURN_META(MRES_IGNORED);
}

void CForwardManager::ExecuteStringCommandPre::Init()
{
	if (g_pGameConf[GameConf_PTaH]->GetOffset("CGameClient::ExecuteStringCommand", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(ExecuteStringCommand, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 2, nullptr, Param_Cell, Param_String);
	}
	else smutils->LogError(myself, "Failed to get CGameClient::ExecuteStringCommand offset, Hook ExecuteStringCommandPre will be unavailable.");
}

int CForwardManager::ExecuteStringCommandPre::__SH_ADD_MANUALVPHOOK(CBaseClient* pBaseClient)
{
	return SH_ADD_MANUALVPHOOK(ExecuteStringCommand, static_cast<CGameClient*>(pBaseClient), SH_MEMBER(this, &CForwardManager::ExecuteStringCommandPre::SHHook), false);
}

bool CForwardManager::ExecuteStringCommandPre::SHHook(const char* pCommandString)
{
	cell_t res = Pl_Continue;
	char cMsg[512];

	V_strncpy(cMsg, pCommandString, sizeof(cMsg));

	pForward->PushCell(META_IFACEPTR(CGameClient)->GetPlayerSlot() + 1);
	pForward->PushStringEx(cMsg, sizeof(cMsg), SM_PARAM_STRING_UTF8 | SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
	pForward->Execute(&res);

	if (res != Pl_Continue)
	{
		if (res == Pl_Changed) RETURN_META_VALUE_MNEWPARAMS(MRES_HANDLED, true, ExecuteStringCommand, (const_cast<const char*>(cMsg)));
		else RETURN_META_VALUE(MRES_SUPERCEDE, false);
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

void CForwardManager::ExecuteStringCommandPost::Init()
{
	if (g_pGameConf[GameConf_PTaH]->GetOffset("CGameClient::ExecuteStringCommand", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(ExecuteStringCommand, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 2, nullptr, Param_Cell, Param_String);
	}
	else smutils->LogError(myself, "Failed to get CGameClient::ExecuteStringCommand offset, Hook ExecuteStringCommandPost will be unavailable.");
}

int CForwardManager::ExecuteStringCommandPost::__SH_ADD_MANUALVPHOOK(CBaseClient* pBaseClient)
{
	return SH_ADD_MANUALVPHOOK(ExecuteStringCommand, static_cast<CGameClient*>(pBaseClient), SH_MEMBER(this, &CForwardManager::ExecuteStringCommandPost::SHHook), true);
}

bool CForwardManager::ExecuteStringCommandPost::SHHook(const char* pCommandString)
{
	pForward->PushCell(META_IFACEPTR(CGameClient)->GetPlayerSlot() + 1);
	pForward->PushString(pCommandString);
	pForward->Execute(nullptr);

	RETURN_META_VALUE(MRES_IGNORED, true);
}

void CForwardManager::ClientConnectPre::Init()
{
	if (g_pGameConf[GameConf_PTaH]->GetOffset("CBaseServer::RejectConnection", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(RejectConnection, iOffset, 0, 0);

		iOffset = -1;

		if (g_pGameConf[GameConf_PTaH]->GetOffset("CBaseServer::ConnectClient", &iOffset))
		{
			SH_MANUALHOOK_RECONFIGURE(ConnectClient, iOffset, 0, 0);

			pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 5, nullptr, Param_Cell, Param_String, Param_String, Param_String, Param_String);
		}
		else smutils->LogError(myself, "Failed to get CBaseServer::ConnectClient offset, Hook ClientConnectPre will be unavailable.");
	}
	else smutils->LogError(myself, "Failed to get CBaseServer::RejectConnection offset, Hook ClientConnectPre will be unavailable.");
}

void CForwardManager::ClientConnectPre::Shutdown()
{
	if (iOffset != -1)
	{
		forwards->ReleaseForward(pForward);

		if (iHookId != -1) SH_REMOVE_HOOK_ID(iHookId);
	}
}

bool CForwardManager::ClientConnectPre::UpdateForward(IPluginFunction* pFunc, bool bHook)
{
	if (iOffset != -1)
	{
		bool bBuf;

		if (bHook) bBuf = pForward->AddFunction(pFunc);
		else bBuf = pForward->RemoveFunction(pFunc);

		UpdateHook();

		return bBuf;
	}

	return false;
}

void CForwardManager::ClientConnectPre::UpdateHook()
{
	if (iOffset != -1 && (pForward->GetFunctionCount() > 0) != bHooked)
	{
		bHooked = !bHooked;

		if (bHooked) iHookId = SH_ADD_MANUALHOOK(ConnectClient, iserver, SH_MEMBER(this, &CForwardManager::ClientConnectPre::SHHook), false);
		else if (iHookId != -1)
		{
			SH_REMOVE_HOOK_ID(iHookId);

			iHookId = -1;
		}
	}
}

//https://github.com/peace-maker/sourcetvmanager/blob/067b238bd21c4fba4b877cb0a514011a1141e1a6/forwards.cpp#L267
//Thanks Peace-Maker!
static const char* ExtractPlayerName(CUtlVector<NetMsg_SplitPlayerConnect*>& pSplitPlayerConnectVector)
{
	for (int i = 0; i < pSplitPlayerConnectVector.Count(); i++)
	{
		NetMsg_SplitPlayerConnect* split = pSplitPlayerConnectVector[i];
		if (!split->has_convars())
			continue;

		const CMsg_CVars cvars = split->convars();
		for (int c = 0; c < cvars.cvars_size(); c++)
		{
			const CMsg_CVars_CVar cvar = cvars.cvars(c);
			if (!cvar.has_name() || !cvar.has_value())
				continue;

			if (!strcmp(cvar.name().c_str(), "name"))
			{
				return cvar.value().c_str();
			}
		}
	}
	return "";
}

IClient* CForwardManager::ClientConnectPre::SHHook(const netadr_t& address, int nProtocol, int iChallenge, int nAuthProtocol, const char* pchName, const char* pchPassword, const char* pCookie, int cbCookie, CUtlVector<NetMsg_SplitPlayerConnect*>& pSplitPlayerConnectVector, bool bUnknown, CrossPlayPlatform_t platform, const unsigned char* pUnknown, int iUnknown)
{
	if (nAuthProtocol == 3 && cbCookie >= static_cast<int>(sizeof(CSteamID)))
	{
		cell_t res = Pl_Continue;
		const CSteamID* SteamID = reinterpret_cast<const CSteamID*>(pCookie);
		char rejectReason[255];
		char passwordBuffer[128];
		char ipString[16];

		V_strncpy(passwordBuffer, pchPassword, sizeof(passwordBuffer));
		V_snprintf(ipString, sizeof(ipString), "%u.%u.%u.%u", address.ip[0], address.ip[1], address.ip[2], address.ip[3]);

		pForward->PushCell(SteamID->GetAccountID());
		pForward->PushString(ipString);
		pForward->PushString(ExtractPlayerName(pSplitPlayerConnectVector));
		pForward->PushStringEx(passwordBuffer, sizeof(passwordBuffer), SM_PARAM_STRING_UTF8 | SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
		pForward->PushStringEx(rejectReason, sizeof(rejectReason), SM_PARAM_STRING_UTF8 | SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
		pForward->Execute(&res);

		if (res != Pl_Continue)
		{
			if (res == Pl_Changed)
			{
				RETURN_META_VALUE_MNEWPARAMS(MRES_HANDLED, nullptr, ConnectClient, (address, nProtocol, iChallenge, nAuthProtocol, pchName, const_cast<const char*>(passwordBuffer), pCookie, cbCookie, pSplitPlayerConnectVector, bUnknown, platform, pUnknown, iUnknown));
			}
			else
			{
				SH_MCALL(iserver, RejectConnection)(address, rejectReason);

				RETURN_META_VALUE(MRES_SUPERCEDE, nullptr);
			}
		}
	}

	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

void CForwardManager::ClientConnectPost::Init()
{
	if (g_pGameConf[GameConf_PTaH]->GetOffset("CBaseServer::ConnectClient", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(ConnectClient, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 4, nullptr, Param_Cell, Param_Cell, Param_String, Param_String);
	}
	else smutils->LogError(myself, "Failed to get CBaseServer::ConnectClient offset, Hook ClientConnectPost will be unavailable.");
}

void CForwardManager::ClientConnectPost::Shutdown()
{
	if (iOffset != -1)
	{
		forwards->ReleaseForward(pForward);

		if (iHookId != -1) SH_REMOVE_HOOK_ID(iHookId);
	}
}

bool CForwardManager::ClientConnectPost::UpdateForward(IPluginFunction* pFunc, bool bHook)
{
	if (iOffset != -1)
	{
		bool bBuf;

		if (bHook) bBuf = pForward->AddFunction(pFunc);
		else bBuf = pForward->RemoveFunction(pFunc);

		UpdateHook();

		return bBuf;
	}

	return false;
}

void CForwardManager::ClientConnectPost::UpdateHook()
{
	if (iOffset != -1 && (pForward->GetFunctionCount() > 0) != bHooked)
	{
		bHooked = !bHooked;

		if (bHooked) iHookId = SH_ADD_MANUALHOOK(ConnectClient, iserver, SH_MEMBER(this, &CForwardManager::ClientConnectPost::SHHook), true);
		else if (iHookId != -1)
		{
			SH_REMOVE_HOOK_ID(iHookId);

			iHookId = -1;
		}
	}
}

IClient* CForwardManager::ClientConnectPost::SHHook(const netadr_t& address, int nProtocol, int iChallenge, int nAuthProtocol, const char* pchName, const char* pchPassword, const char* pCookie, int cbCookie, CUtlVector<NetMsg_SplitPlayerConnect*>& pSplitPlayerConnectVector, bool bUnknown, CrossPlayPlatform_t platform, const unsigned char* pUnknown, int iUnknown)
{
	if (nAuthProtocol == 3 && cbCookie >= static_cast<int>(sizeof(CSteamID)))
	{
		IClient* pClient = META_RESULT_ORIG_RET(IClient*);

		if (pClient)
		{
			const CSteamID* SteamID = reinterpret_cast<const CSteamID*>(pCookie);
			char ipString[16];

			V_snprintf(ipString, sizeof(ipString), "%u.%u.%u.%u", address.ip[0], address.ip[1], address.ip[2], address.ip[3]);

			pForward->PushCell(pClient->GetPlayerSlot() + 1);
			pForward->PushCell(SteamID->GetAccountID());
			pForward->PushString(ipString);
			pForward->PushString(ExtractPlayerName(pSplitPlayerConnectVector));
			pForward->Execute(nullptr);
		}
	}

	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

CForwardManager::SendInventoryUpdateEventPost::SendInventoryUpdateEventPost()
{
	bInGame = true;
}

void CForwardManager::SendInventoryUpdateEventPost::Init()
{
	if (g_pGameConf[GameConf_PTaH]->GetOffset("CPlayerInventory::SendInventoryUpdateEvent", &iOffset))
	{
		SH_MANUALHOOK_RECONFIGURE(SendInventoryUpdateEvent, iOffset, 0, 0);

		pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 2, nullptr, Param_Cell, Param_Cell);
	}
	else smutils->LogError(myself, "Failed to get CPlayerInventory::SendInventoryUpdateEvent offset, Hook InventoryUpdatePost will be unavailable.");
}

void CForwardManager::SendInventoryUpdateEventPost::Hook(int iClient)
{
	if (bHooked && iHookId == -1)
	{
		CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(iClient);

		CCSPlayerInventory* pPlayerInventory = CCSPlayerInventory::FromPlayer(pEntity);

		iHookId = SH_ADD_MANUALVPHOOK(SendInventoryUpdateEvent, pPlayerInventory, SH_MEMBER(this, &CForwardManager::SendInventoryUpdateEventPost::SHHook), true);
	}
}

void CForwardManager::SendInventoryUpdateEventPost::SHHook()
{
	CCSPlayerInventory* pPlayerInventory = META_IFACEPTR(CCSPlayerInventory);

	pForward->PushCell(gamehelpers->EntityToBCompatRef(pPlayerInventory->ToPlayer()));
	pForward->PushCell(reinterpret_cast<cell_t>(pPlayerInventory));
	pForward->Execute(nullptr);

	RETURN_META(MRES_IGNORED);
}
