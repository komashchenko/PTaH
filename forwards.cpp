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
#include "forwards.h"
#include "classes.h"
#include "natives.h"


decltype(CForwardManager::m_Hooks) CForwardManager::m_Hooks;
CForwardManager g_ForwardManager;

// GiveNamedItem
SH_DECL_MANUALHOOK5(GiveNamedItem, 0, 0, 0, CBaseEntity*, const char*, int, CEconItemView*, bool, Vector*);

// WeaponCanUse
SH_DECL_MANUALHOOK1(Weapon_CanUse, 0, 0, 0, bool, CBaseCombatWeapon*);

// SetModel
SH_DECL_MANUALHOOK1_void(SetEntityModel, 0, 0, 0, const char*);

// ClientVoiceTo
SH_DECL_HOOK3(IVoiceServer, SetClientListening, SH_NOATTRIB, 0, bool, int, int, bool);
SH_DECL_HOOK1_void(IServerGameClients, ClientVoice, SH_NOATTRIB, 0, edict_t*);

// ClientPrint
SH_DECL_HOOK0_void_vafmt(IClient, ClientPrintf, SH_NOATTRIB, 0);
#ifdef PLATFORM_LINUX
SH_DECL_MANUALHOOK0_void_vafmt(CBaseClient_ClientPrintf, 0, 0, 0);
#endif

// ExecuteStringCommand
SH_DECL_HOOK1(IClient, ExecuteStringCommand, SH_NOATTRIB, 0, bool, const char*);
#ifdef PLATFORM_LINUX
SH_DECL_MANUALHOOK1(CGameClient_ExecuteStringCommand, 0, 0, 0, bool, const char*);
#endif

// ClientConnect
SH_DECL_MANUALHOOK13(ConnectClient, 0, 0, 0, IClient*, const ns_address&, int, int, int, const char*, const char*, const char*, int, CUtlVector<CCLCMsg_SplitPlayerConnect_t*>&, bool, CrossPlayPlatform_t, const byte*, int);

// InventoryUpdate
SH_DECL_MANUALHOOK0_void(SendInventoryUpdateEvent, 0, 0, 0);


void CForwardManager::Init()
{
	for(auto& hook : m_Hooks)
	{
		hook.second->Init();
	}
}

void CForwardManager::Shutdown()
{
	for(auto& hook : m_Hooks)
	{
		hook.second->Shutdown();
	}
}

bool CForwardManager::UpdateHook(PTaH_HookEvent htType, IPluginFunction* pFunction, bool bHook)
{
	auto hook = m_Hooks.find(htType);
	if(hook != m_Hooks.end())
	{
		return hook->second->UpdateHook(pFunction, bHook);
	}

	return false;
}

CForwardManager::CBaseHook::CBaseHook()
{
	m_pForward = nullptr;
	m_bHooked = false;
}

CForwardManager::CBaseHook::HookTypeRegistrator::HookTypeRegistrator(CBaseHook* pBaseHook, PTaH_HookEvent htType)
{
	m_Hooks[htType] = pBaseHook;
}

void CForwardManager::CBaseHook::Shutdown()
{
	if(m_pForward)
	{
		if(m_bHooked)
		{
			OnInternalHookDeactivated();
		}

		forwards->ReleaseForward(m_pForward);
	}
}

bool CForwardManager::CBaseHook::UpdateHook(SourcePawn::IPluginFunction* pFunction, bool bHook)
{
	if(m_pForward)
	{
		bool bRet = bHook ? m_pForward->AddFunction(pFunction) : m_pForward->RemoveFunction(pFunction);

		UpdateInternalHook();

		return bRet;
	}

	return false;
}

void CForwardManager::CBaseHook::UpdateInternalHook()
{
	if(m_pForward && (m_pForward->GetFunctionCount() > 0) != m_bHooked)
	{
		m_bHooked = !m_bHooked;

		if(m_bHooked)
		{
			OnInternalHookActivated();
		}
		else
		{
			OnInternalHookDeactivated();
		}
	}
}

void CForwardManager::CBaseHook::OnInternalHookActivated()
{
	plsys->AddPluginsListener(this);
}

void CForwardManager::CBaseHook::OnInternalHookDeactivated()
{
	plsys->RemovePluginsListener(this);
}

void CForwardManager::CBaseHook::OnPluginUnloaded(IPlugin* plugin)
{
	UpdateInternalHook();
}

CForwardManager::CBaseManualPlayerHook::CBaseManualPlayerHook()
{
	memset(m_iHookID, 0xFF, sizeof(m_iHookID));
}

void CForwardManager::CBaseManualPlayerHook::OnInternalHookActivated()
{
	BaseClass::OnInternalHookActivated();
	playerhelpers->AddClientListener(this);

	int iMaxClients = playerhelpers->GetMaxClients();
	IGamePlayer* pPlayer;
	for (int i = 1; i <= iMaxClients; i++) if ((pPlayer = playerhelpers->GetGamePlayer(i)) && pPlayer->IsInGame())
	{
		OnClientPutInServer(i);
	}
}

void CForwardManager::CBaseManualPlayerHook::OnInternalHookDeactivated()
{
	BaseClass::OnInternalHookDeactivated();
	playerhelpers->RemoveClientListener(this);

	int iMaxClients = playerhelpers->GetMaxClients();
	for (int i = 1; i <= iMaxClients; i++)
	{
		OnClientDisconnected(i);
	}
}

void CForwardManager::CBaseManualPlayerHook::OnClientPutInServer(int iClient)
{
	m_iHookID[iClient] = ManualHook(iClient);
}

void CForwardManager::CBaseManualPlayerHook::OnClientDisconnected(int iClient)
{
	if(m_iHookID[iClient] != -1)
	{
		SH_REMOVE_HOOK_ID(m_iHookID[iClient]);
		m_iHookID[iClient] = -1;
	}
}

CForwardManager::CBaseVPHook::CBaseVPHook(bool bInGame)
{
	m_iHookID = -1;
	m_bInGame = bInGame;
}

void CForwardManager::CBaseVPHook::OnClientValid(int iClient)
{
	// Protection against multiple calls in the same frame
	if(m_iHookID == -1)
	{
		m_iHookID = VPHook(iClient);

		// Using RemoveClientListener inside the handler will cause a crash
		smutils->AddFrameAction([](void* pThis)
		{
			playerhelpers->RemoveClientListener(reinterpret_cast<CForwardManager::CBaseVPHook*>(pThis));
		}, this);
	}
}

void CForwardManager::CBaseVPHook::OnInternalHookActivated()
{
	CBaseHook::OnInternalHookActivated();

	auto fPlayerValid = m_bInGame ? &IGamePlayer::IsInGame : &IGamePlayer::IsConnected;
	int iMaxClients = playerhelpers->GetMaxClients();
	IGamePlayer* pPlayer;
	for (int i = 1; i <= iMaxClients; i++) if ((pPlayer = playerhelpers->GetGamePlayer(i)) && (pPlayer->*fPlayerValid)())
	{
		m_iHookID = VPHook(i);

		return;
	}

	playerhelpers->AddClientListener(this);
}

void CForwardManager::CBaseVPHook::OnInternalHookDeactivated()
{
	CBaseHook::OnInternalHookDeactivated();

	if(m_iHookID != -1)
	{
		SH_REMOVE_HOOK_ID(m_iHookID);
		m_iHookID = -1;
	}
	else
	{
		playerhelpers->RemoveClientListener(this);
	}
}

void CForwardManager::CBaseVPHook::OnClientConnected(int iClient)
{
	if(!m_bInGame)
	{
		OnClientValid(iClient);
	}
}

void CForwardManager::CBaseVPHook::OnClientPutInServer(int iClient)
{
	if(m_bInGame)
	{
		OnClientValid(iClient);
	}
}

CForwardManager::CBaseClientHook::CBaseClientHook() : CBaseVPHook(false)
{
#ifdef PLATFORM_LINUX
	m_iGameHookId = -1;
#endif
}

#ifdef PLATFORM_LINUX
void CForwardManager::CBaseClientHook::OnInternalHookDeactivated()
{
	CBaseHook::OnInternalHookDeactivated();

	if(m_iGameHookId != -1)
	{
		SH_REMOVE_HOOK_ID(m_iGameHookId);
		m_iGameHookId = -1;
	}
}

int CForwardManager::CBaseClientHook::GetParentVFuncOffset(IClient* pClient, size_t vtbIndex)
{
	CGameClient* pGameClient = dynamic_cast<CGameClient*>(pClient);

	// Getting the trampoline function address
	// If the function has been patched, get the original via GetOrigVfnPtrEntry
	void** vfnPtr = *reinterpret_cast<void***>(pClient) + vtbIndex;
	void* vfnOrigPtr = SH_GLOB_SHPTR->GetOrigVfnPtrEntry(vfnPtr);
	uintptr_t tmplPtr = reinterpret_cast<uintptr_t>(vfnOrigPtr ? vfnOrigPtr : *vfnPtr);

	// Checking that it is a trampoline function
	// sub dword ptr [esp+4], 4
	if(*reinterpret_cast<uint32_t*>(tmplPtr) != 0x4246C83)
	{
		return -1;
	}

	uint8_t jmp = *reinterpret_cast<uint8_t*>(tmplPtr + 0x5);
	//        jmp            jmp short
	if(jmp != 0xE9 && jmp != 0xEB)
	{
		return -1;
	}

	void* funcPtr;
	// Getting the function address
	{
		uintptr_t jmpPtr = tmplPtr + 0x5;
		if(jmp == 0xE9)
		{
			funcPtr = reinterpret_cast<void*>(jmpPtr + *reinterpret_cast<uint32_t*>(jmpPtr + 0x1) + 0x5);
		}
		else
		{
			funcPtr = reinterpret_cast<void*>(jmpPtr + *reinterpret_cast<int8_t*>(jmpPtr + 0x1) + 0x2);
		}
	}

	// Finding a function in the CGameClient virtual table
	for(int i = 0; i < 90; i++)
	{
		void** vfnGamePtr = *reinterpret_cast<void***>(pGameClient) + i;
		void* vfnGameOrigPtr = SH_GLOB_SHPTR->GetOrigVfnPtrEntry(vfnGamePtr);

		if((vfnGameOrigPtr ? vfnGameOrigPtr : *vfnGamePtr) == funcPtr)
		{
			return i;
		}
	}

	return -1;
}
#endif
inline IClient* CForwardManager::CBaseClientHook::ForceCastToIClient(IClient* pClient)
{
#ifdef PLATFORM_LINUX
	// Checking offset to this (CGameClient 0) (IClient -4)
	if(*reinterpret_cast<int32_t*>(*reinterpret_cast<uintptr_t*>(pClient) - 0x8) == 0)
	{
		pClient = reinterpret_cast<CGameClient*>(pClient);
	}
#endif

	return pClient;
}

bool CForwardManager::GiveNamedItemHook::Configure()
{
	int offset = -1;
	if(!g_pGameConf[GameConf_SDKT]->GetOffset("GiveNamedItem", &offset))
	{
		smutils->LogError(myself, "Failed to get GiveNamedItem offset, hook %s will be unavailable.", GetHookName());

		return false;
	}

	SH_MANUALHOOK_RECONFIGURE(GiveNamedItem, offset, 0, 0);

	return true;
}

DETOUR_DECL_MEMBER4(CCSPlayer_FindMatchingWeaponsForTeamLoadout, uint64_t, const char*, szItem, int, iTeam, bool, bMustBeTeamSpecific, CUtlVector<CEconItemView*>&, matchingWeapons)
{
	// Function can be called twice if the game mode is GunGame
	if(g_ForwardManager.m_GiveNamedItemPreHook.m_iFrameCount == gpGlobals->framecount)
	{
		return 0LL;
	}

	return DETOUR_MEMBER_CALL(CCSPlayer_FindMatchingWeaponsForTeamLoadout)(szItem, iTeam, bMustBeTeamSpecific, matchingWeapons);
}

CForwardManager::GiveNamedItemPreHook::GiveNamedItemPreHook()
{
	m_Hooks[PTaH_GiveNamedItemPre] = this;

	m_pFindMatchingWeaponsForTeamLoadoutHook = nullptr;
	m_iFrameCount = 0;
}

void CForwardManager::GiveNamedItemPreHook::Init()
{
	if(BaseClass::Configure())
	{
		m_pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 6, nullptr, Param_Cell, Param_String, Param_CellByRef, Param_CellByRef, Param_CellByRef, Param_Array);

		m_pFindMatchingWeaponsForTeamLoadoutHook = DETOUR_CREATE_MEMBER(CCSPlayer_FindMatchingWeaponsForTeamLoadout, "CCSPlayer::FindMatchingWeaponsForTeamLoadout");
		if(!m_pFindMatchingWeaponsForTeamLoadoutHook)
		{
			smutils->LogError(myself, "Detour failed CCSPlayer::FindMatchingWeaponsForTeamLoadout, %s hook functionality will be limited.", GetHookName());
		}
	}
}

void CForwardManager::GiveNamedItemPreHook::Shutdown()
{
	BaseClass::Shutdown();

	if(m_pFindMatchingWeaponsForTeamLoadoutHook)
	{
		m_pFindMatchingWeaponsForTeamLoadoutHook->Destroy();
	}
}

int CForwardManager::GiveNamedItemPreHook::ManualHook(int iClient)
{
	return SH_ADD_MANUALHOOK(GiveNamedItem, gamehelpers->ReferenceToEntity(iClient), SH_MEMBER(this, &CForwardManager::GiveNamedItemPreHook::Handler), false);
}

void CForwardManager::GiveNamedItemPreHook::OnInternalHookActivated()
{
	BaseClass::OnInternalHookActivated();

	if(m_pFindMatchingWeaponsForTeamLoadoutHook)
	{
		m_pFindMatchingWeaponsForTeamLoadoutHook->EnableDetour();
	}
}

void CForwardManager::GiveNamedItemPreHook::OnInternalHookDeactivated()
{
	BaseClass::OnInternalHookDeactivated();

	if(m_pFindMatchingWeaponsForTeamLoadoutHook)
	{
		m_pFindMatchingWeaponsForTeamLoadoutHook->DisableDetour();
	}
}

CBaseEntity* CForwardManager::GiveNamedItemPreHook::Handler(const char* pchName, int iSubType, CEconItemView* pScriptItem, bool bForce, Vector* pOrigin)
{
	m_iFrameCount = 0;

	if(!pchName)
	{
		RETURN_META_VALUE(MRES_IGNORED, nullptr);
	}

	cell_t res = Pl_Continue;
	cell_t pScriptItemNew = reinterpret_cast<cell_t>(pScriptItem);
	cell_t IgnoredCEconItemViewNew = false;
	cell_t OriginIsNull = pOrigin == nullptr;
	cell_t Origin[3] = { 0, 0, 0 };
	char szNewName[64];

	V_strncpy(szNewName, pchName, sizeof(szNewName));

	if(pOrigin)
	{
		Origin[0] = sp_ftoc(pOrigin->x);
		Origin[1] = sp_ftoc(pOrigin->y);
		Origin[2] = sp_ftoc(pOrigin->z);
	}

	m_pForward->PushCell(gamehelpers->EntityToBCompatRef(META_IFACEPTR(CBaseEntity)));
	m_pForward->PushStringEx(szNewName, sizeof(szNewName), SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
	m_pForward->PushCellByRef(&pScriptItemNew);
	m_pForward->PushCellByRef(&IgnoredCEconItemViewNew);
	m_pForward->PushCellByRef(&OriginIsNull);
	m_pForward->PushArray(Origin, 3, SM_PARAM_COPYBACK);
	m_pForward->Execute(&res);

	if(res != Pl_Continue)
	{
		if(res == Pl_Changed)
		{
			if(IgnoredCEconItemViewNew)
			{
				m_iFrameCount = gpGlobals->framecount;
				pScriptItemNew = 0;
			}

			if(OriginIsNull == false)
			{
				Vector vecOrigin(sp_ctof(Origin[0]), sp_ctof(Origin[1]), sp_ctof(Origin[2]));

				RETURN_META_VALUE_MNEWPARAMS(MRES_HANDLED, nullptr, GiveNamedItem, (szNewName, iSubType, reinterpret_cast<CEconItemView*>(pScriptItemNew), bForce, &vecOrigin));
			}

			RETURN_META_VALUE_MNEWPARAMS(MRES_HANDLED, nullptr, GiveNamedItem, (szNewName, iSubType, reinterpret_cast<CEconItemView*>(pScriptItemNew), bForce, nullptr));
		}

		RETURN_META_VALUE(MRES_SUPERCEDE, nullptr);
	}

	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

CForwardManager::GiveNamedItemPostHook::GiveNamedItemPostHook()
{
	m_Hooks[PTaH_GiveNamedItemPost] = this;
}

void CForwardManager::GiveNamedItemPostHook::Init()
{
	if(BaseClass::Configure())
	{
		m_pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 6, nullptr, Param_Cell, Param_String, Param_Cell, Param_Cell, Param_Cell, Param_Array);
	}
}

int CForwardManager::GiveNamedItemPostHook::ManualHook(int iClient)
{
	return SH_ADD_MANUALHOOK(GiveNamedItem, gamehelpers->ReferenceToEntity(iClient), SH_MEMBER(this, &CForwardManager::GiveNamedItemPostHook::Handler), true);
}

CBaseEntity* CForwardManager::GiveNamedItemPostHook::Handler(const char* pchName, int iSubType, CEconItemView* pScriptItem, bool bForce, Vector* pOrigin)
{
	if(!pchName)
	{
		RETURN_META_VALUE(MRES_IGNORED, nullptr);
	}

	cell_t Origin[3] = { 0, 0, 0 };

	if(pOrigin)
	{
		Origin[0] = sp_ftoc(pOrigin->x);
		Origin[1] = sp_ftoc(pOrigin->y);
		Origin[2] = sp_ftoc(pOrigin->z);
	}

	m_pForward->PushCell(gamehelpers->EntityToBCompatRef(META_IFACEPTR(CBaseEntity)));
	m_pForward->PushString(pchName);
	m_pForward->PushCell(reinterpret_cast<cell_t>(pScriptItem));
	m_pForward->PushCell(gamehelpers->EntityToBCompatRef(META_RESULT_ORIG_RET(CBaseEntity*)));
	m_pForward->PushCell(pOrigin == nullptr);
	m_pForward->PushArray(Origin, 3);
	m_pForward->Execute(nullptr);

	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

bool CForwardManager::WeaponCanUseHook::Configure()
{
	int offset = -1;
	if(!g_pGameConf[GameConf_SDKH]->GetOffset("Weapon_CanUse", &offset))
	{
		smutils->LogError(myself, "Failed to get Weapon_CanUse offset, hook %s will be unavailable.", GetHookName());

		return false;
	}

	SH_MANUALHOOK_RECONFIGURE(Weapon_CanUse, offset, 0, 0);

	return true;
}

CForwardManager::WeaponCanUsePreHook::WeaponCanUsePreHook()
{
	m_Hooks[PTaH_WeaponCanUsePre] = this;
}

void CForwardManager::WeaponCanUsePreHook::Init()
{
	if(BaseClass::Configure())
	{
		m_pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 3, nullptr, Param_Cell, Param_Cell, Param_CellByRef);
	}
}

int CForwardManager::WeaponCanUsePreHook::ManualHook(int iClient)
{
	return SH_ADD_MANUALHOOK(Weapon_CanUse, gamehelpers->ReferenceToEntity(iClient), SH_MEMBER(this, &CForwardManager::WeaponCanUsePreHook::Handler), false);
}

bool CForwardManager::WeaponCanUsePreHook::Handler(CBaseCombatWeapon* pWeapon)
{
	cell_t res = Pl_Continue;
	cell_t ret = META_RESULT_ORIG_RET(bool);

	m_pForward->PushCell(gamehelpers->EntityToBCompatRef(META_IFACEPTR(CBaseEntity)));
	m_pForward->PushCell(gamehelpers->EntityToBCompatRef(pWeapon));
	m_pForward->PushCellByRef(&ret);
	m_pForward->Execute(&res);

	if(res != Pl_Continue)
	{
		if(res == Pl_Changed)
		{
			RETURN_META_VALUE(MRES_SUPERCEDE, static_cast<bool>(ret));
		}

		RETURN_META_VALUE(MRES_SUPERCEDE, false);
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

CForwardManager::WeaponCanUsePostHook::WeaponCanUsePostHook()
{
	m_Hooks[PTaH_WeaponCanUsePost] = this;
}

void CForwardManager::WeaponCanUsePostHook::Init()
{
	if(BaseClass::Configure())
	{
		m_pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 3, nullptr, Param_Cell, Param_Cell, Param_Cell);
	}
}

int CForwardManager::WeaponCanUsePostHook::ManualHook(int iClient)
{
	return SH_ADD_MANUALHOOK(Weapon_CanUse, gamehelpers->ReferenceToEntity(iClient), SH_MEMBER(this, &CForwardManager::WeaponCanUsePostHook::Handler), true);
}

bool CForwardManager::WeaponCanUsePostHook::Handler(CBaseCombatWeapon* pWeapon)
{
	m_pForward->PushCell(gamehelpers->EntityToBCompatRef(META_IFACEPTR(CBaseEntity)));
	m_pForward->PushCell(gamehelpers->EntityToBCompatRef(pWeapon));
	m_pForward->PushCell(META_RESULT_ORIG_RET(bool));
	m_pForward->Execute(nullptr);

	RETURN_META_VALUE(MRES_IGNORED, true);
}

bool CForwardManager::SetPlayerModelHook::Configure()
{
	int offset = -1;
	if(!g_pGameConf[GameConf_SDKT]->GetOffset("SetEntityModel", &offset))
	{
		smutils->LogError(myself, "Failed to get SetEntityModel offset, hook %s will be unavailable.", GetHookName());

		return false;
	}

	SH_MANUALHOOK_RECONFIGURE(SetEntityModel, offset, 0, 0);

	return true;
}

CForwardManager::SetPlayerModelPreHook::SetPlayerModelPreHook()
{
	m_Hooks[PTaH_SetPlayerModelPre] = this;
}

void CForwardManager::SetPlayerModelPreHook::Init()
{
	if(BaseClass::Configure())
	{
		m_pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 3, nullptr, Param_Cell, Param_String, Param_String);
	}
}

int CForwardManager::SetPlayerModelPreHook::ManualHook(int iClient)
{
	return SH_ADD_MANUALHOOK(SetEntityModel, gamehelpers->ReferenceToEntity(iClient), SH_MEMBER(this, &CForwardManager::SetPlayerModelPreHook::Handler), false);
}

void CForwardManager::SetPlayerModelPreHook::Handler(const char* pszModelName)
{
	cell_t res = Pl_Continue;
	int iClient = gamehelpers->EntityToBCompatRef(META_IFACEPTR(CBaseEntity));
	char szModelNewName[256];

	V_strncpy(szModelNewName, pszModelName, sizeof(szModelNewName));

	m_pForward->PushCell(iClient);
	m_pForward->PushString(playerhelpers->GetGamePlayer(iClient)->GetPlayerInfo()->GetModelName());
	m_pForward->PushStringEx(szModelNewName, sizeof(szModelNewName), SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
	m_pForward->Execute(&res);

	if(res != Pl_Continue)
	{
		if(res == Pl_Changed)
		{
			RETURN_META_MNEWPARAMS(MRES_HANDLED, SetEntityModel, (szModelNewName));
		}

		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

CForwardManager::SetPlayerModelPostHook::SetPlayerModelPostHook()
{
	m_Hooks[PTaH_SetPlayerModelPost] = this;
}

void CForwardManager::SetPlayerModelPostHook::Init()
{
	if(BaseClass::Configure())
	{
		m_pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 2, nullptr, Param_Cell, Param_String);
	}
}

int CForwardManager::SetPlayerModelPostHook::ManualHook(int iClient)
{
	return SH_ADD_MANUALHOOK(SetEntityModel, gamehelpers->ReferenceToEntity(iClient), SH_MEMBER(this, &CForwardManager::SetPlayerModelPostHook::Handler), true);
}

void CForwardManager::SetPlayerModelPostHook::Handler(const char* pszModelName)
{
	m_pForward->PushCell(gamehelpers->EntityToBCompatRef(META_IFACEPTR(CBaseEntity)));
	m_pForward->PushString(pszModelName);
	m_pForward->Execute(nullptr);

	RETURN_META(MRES_IGNORED);
}

CForwardManager::ClientVoiceToPreHook::ClientVoiceToPreHook()
{
	m_Hooks[PTaH_ClientVoiceToPre] = this;

	memset(m_bStartVoice, 0x0, sizeof(m_bStartVoice));
}

void CForwardManager::ClientVoiceToPreHook::Init()
{
	if(!g_pCPlayerVoiceListener)
	{
		smutils->LogError(myself, "g_pCPlayerVoiceListener is nullptr, hook ClientVoiceToPre will be unavailable.");

		return;
	}

	m_pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 3, nullptr, Param_Cell, Param_Cell, Param_CellByRef);
}

void CForwardManager::ClientVoiceToPreHook::OnInternalHookActivated()
{
	CBaseHook::OnInternalHookActivated();

	SH_ADD_HOOK(IVoiceServer, SetClientListening, voiceserver, SH_MEMBER(this, &CForwardManager::ClientVoiceToPreHook::Handler), false);
	SH_ADD_HOOK(IServerGameClients, ClientVoice, serverClients, SH_MEMBER(this, &CForwardManager::ClientVoiceToPreHook::ClientVoiceHandler), false);
}

void CForwardManager::ClientVoiceToPreHook::OnInternalHookDeactivated()
{
	CBaseHook::OnInternalHookDeactivated();

	SH_REMOVE_HOOK(IVoiceServer, SetClientListening, voiceserver, SH_MEMBER(this, &CForwardManager::ClientVoiceToPreHook::Handler), false);
	SH_REMOVE_HOOK(IServerGameClients, ClientVoice, serverClients, SH_MEMBER(this, &CForwardManager::ClientVoiceToPreHook::ClientVoiceHandler), false);

	memset(m_bStartVoice, 0x0, sizeof(m_bStartVoice));
}

bool CForwardManager::ClientVoiceToPreHook::Handler(int iReceiver, int iSender, bool bListen)
{
	// IsPlayerSpeaking is required to protect plugins from useless spam, but this has its disadvantage because it needs another hook.
	if(iReceiver != iSender && (m_bStartVoice[iSender] || g_pCPlayerVoiceListener->IsPlayerSpeaking(iSender)))
	{
		cell_t res = Pl_Continue;
		cell_t ret = bListen;

		m_pForward->PushCell(iSender);
		m_pForward->PushCell(iReceiver);
		m_pForward->PushCellByRef(&ret);
		m_pForward->Execute(&res);

		if(res != Pl_Continue)
		{
			if(res == Pl_Changed)
			{
				RETURN_META_VALUE_NEWPARAMS(MRES_HANDLED, true, &IVoiceServer::SetClientListening, (iReceiver, iSender, ret));
			}

			RETURN_META_VALUE_NEWPARAMS(MRES_HANDLED, true, &IVoiceServer::SetClientListening, (iReceiver, iSender, false));
		}
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

void CForwardManager::ClientVoiceToPreHook::ClientVoiceHandler(edict_t* pEdict)
{
	int iSender = gamehelpers->IndexOfEdict(pEdict);

	if(!g_pCPlayerVoiceListener->IsPlayerSpeaking(iSender))
	{
		m_bStartVoice[iSender] = true;

		IGamePlayer* pPlayer;
		int iMaxClients = playerhelpers->GetMaxClients();
		for (int iReceiver = 1; iReceiver <= iMaxClients; iReceiver++) if ((pPlayer = playerhelpers->GetGamePlayer(iReceiver)) && pPlayer->IsInGame())
		{
			voiceserver->SetClientListening(iReceiver, iSender, voiceserver->GetClientListening(iReceiver, iSender));
		}

		m_bStartVoice[iSender] = false;
	}

	RETURN_META(MRES_IGNORED);
}

CForwardManager::ClientVoiceToPostHook::ClientVoiceToPostHook()
{
	m_Hooks[PTaH_ClientVoiceToPost] = this;
}

void CForwardManager::ClientVoiceToPostHook::Init()
{
	if(!g_pCPlayerVoiceListener)
	{
		smutils->LogError(myself, "g_pCPlayerVoiceListener is nullptr, hook ClientVoiceToPost will be unavailable.");

		return;
	}

	m_pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 3, nullptr, Param_Cell, Param_Cell, Param_Cell);
}

void CForwardManager::ClientVoiceToPostHook::OnInternalHookActivated()
{
	CBaseHook::OnInternalHookActivated();

	SH_ADD_HOOK(IVoiceServer, SetClientListening, voiceserver, SH_MEMBER(this, &CForwardManager::ClientVoiceToPostHook::Handler), true);
}

void CForwardManager::ClientVoiceToPostHook::OnInternalHookDeactivated()
{
	CBaseHook::OnInternalHookDeactivated();

	SH_REMOVE_HOOK(IVoiceServer, SetClientListening, voiceserver, SH_MEMBER(this, &CForwardManager::ClientVoiceToPostHook::Handler), true);
}

bool CForwardManager::ClientVoiceToPostHook::Handler(int iReceiver, int iSender, bool bListen)
{
	if(iReceiver != iSender && g_pCPlayerVoiceListener->IsPlayerSpeaking(iSender))
	{
		m_pForward->PushCell(iSender);
		m_pForward->PushCell(iReceiver);
		m_pForward->PushCell(bListen);
		m_pForward->Execute(nullptr);
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

CForwardManager::ConsolePrintPreHook::ConsolePrintPreHook()
{
	m_Hooks[PTaH_ConsolePrintPre] = this;
}

void CForwardManager::ConsolePrintPreHook::Init()
{
	m_pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 2, nullptr, Param_Cell, Param_String);
}

int CForwardManager::ConsolePrintPreHook::VPHook(int iClient)
{
	IClient* pClient = iserver->GetClient(iClient - 1);

#ifdef PLATFORM_LINUX
	SourceHook::MemFuncInfo mfi;
	SourceHook::GetFuncInfo(&IClient::ClientPrintf, mfi);

	int offset = GetParentVFuncOffset(pClient, mfi.vtblindex);
	if(offset != -1)
	{
		SH_MANUALHOOK_RECONFIGURE(CBaseClient_ClientPrintf, offset, 0, 0);

		m_iGameHookId = SH_ADD_MANUALVPHOOK(CBaseClient_ClientPrintf, static_cast<CGameClient*>(pClient), SH_MEMBER(this, &CForwardManager::ConsolePrintPreHook::Handler), false);
	}
	else
	{
		smutils->LogError(myself, "Failed to get CBaseClient::ClientPrintf offset, ConsolePrintPre hook functionality will be limited.");
	}
#endif

    return SH_ADD_VPHOOK(IClient, ClientPrintf, pClient, SH_MEMBER(this, &CForwardManager::ConsolePrintPreHook::Handler), false);
}

void CForwardManager::ConsolePrintPreHook::Handler(const char* szFormat)
{
	IClient* pClient = ForceCastToIClient(META_IFACEPTR(IClient));

	cell_t res = Pl_Continue;
	char szMsg[1024];

	V_strncpy(szMsg, szFormat, sizeof(szMsg));

	m_pForward->PushCell(pClient->GetPlayerSlot() + 1);
	m_pForward->PushStringEx(szMsg, sizeof(szMsg), SM_PARAM_STRING_UTF8 | SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
	m_pForward->Execute(&res);

	if(res != Pl_Continue)
	{
		if(res == Pl_Changed)
		{
#ifdef PLATFORM_LINUX
			// If it's a CGameClient, the pointers will differ
			if(pClient != META_IFACEPTR(IClient))
			{
				RETURN_META_MNEWPARAMS(MRES_HANDLED, CBaseClient_ClientPrintf, (szMsg));
			}
#endif

			RETURN_META_NEWPARAMS(MRES_HANDLED, &IClient::ClientPrintf, (szMsg));
		}

		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

CForwardManager::ConsolePrintPostHook::ConsolePrintPostHook()
{
	m_Hooks[PTaH_ConsolePrintPost] = this;
}

void CForwardManager::ConsolePrintPostHook::Init()
{
	m_pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 2, nullptr, Param_Cell, Param_String);
}

int CForwardManager::ConsolePrintPostHook::VPHook(int iClient)
{
	IClient* pClient = iserver->GetClient(iClient - 1);

#ifdef PLATFORM_LINUX
	SourceHook::MemFuncInfo mfi;
	SourceHook::GetFuncInfo(&IClient::ClientPrintf, mfi);

	int offset = GetParentVFuncOffset(pClient, mfi.vtblindex);
	if (offset != -1)
	{
		SH_MANUALHOOK_RECONFIGURE(CBaseClient_ClientPrintf, offset, 0, 0);

		m_iGameHookId = SH_ADD_MANUALVPHOOK(CBaseClient_ClientPrintf, static_cast<CGameClient*>(pClient), SH_MEMBER(this, &CForwardManager::ConsolePrintPostHook::Handler), true);
	}
	else
	{
		smutils->LogError(myself, "Failed to get CBaseClient::ClientPrintf offset, ConsolePrintPost hook functionality will be limited.");
	}
#endif

	return SH_ADD_VPHOOK(IClient, ClientPrintf, pClient, SH_MEMBER(this, &CForwardManager::ConsolePrintPostHook::Handler), true);
}

void CForwardManager::ConsolePrintPostHook::Handler(const char* szFormat)
{
	IClient* pClient = ForceCastToIClient(META_IFACEPTR(IClient));

	m_pForward->PushCell(pClient->GetPlayerSlot() + 1);
	m_pForward->PushString(szFormat);
	m_pForward->Execute(nullptr);

	RETURN_META(MRES_IGNORED);
}

CForwardManager::ExecuteStringCommandPreHook::ExecuteStringCommandPreHook()
{
	m_Hooks[PTaH_ExecuteStringCommandPre] = this;
}

void CForwardManager::ExecuteStringCommandPreHook::Init()
{
	m_pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 2, nullptr, Param_Cell, Param_String);
}

int CForwardManager::ExecuteStringCommandPreHook::VPHook(int iClient)
{
	IClient* pClient = iserver->GetClient(iClient - 1);

#ifdef PLATFORM_LINUX
	SourceHook::MemFuncInfo mfi;
	SourceHook::GetFuncInfo(&IClient::ExecuteStringCommand, mfi);

	int offset = GetParentVFuncOffset(pClient, mfi.vtblindex);
	if (offset != -1)
	{
		SH_MANUALHOOK_RECONFIGURE(CGameClient_ExecuteStringCommand, offset, 0, 0);

		m_iGameHookId = SH_ADD_MANUALVPHOOK(CGameClient_ExecuteStringCommand, static_cast<CGameClient*>(pClient), SH_MEMBER(this, &CForwardManager::ExecuteStringCommandPreHook::Handler), false);
	}
	else
	{
		smutils->LogError(myself, "Failed to get CGameClient::ExecuteStringCommand offset, ExecuteStringCommandPre hook functionality will be limited.");
	}
#endif

	return SH_ADD_VPHOOK(IClient, ExecuteStringCommand, pClient, SH_MEMBER(this, &CForwardManager::ExecuteStringCommandPreHook::Handler), false);
}

bool CForwardManager::ExecuteStringCommandPreHook::Handler(const char *pCommandString)
{
	IClient* pClient = ForceCastToIClient(META_IFACEPTR(IClient));

	cell_t res = Pl_Continue;
	char szCommandString[512];

	V_strncpy(szCommandString, pCommandString, sizeof(szCommandString));

	m_pForward->PushCell(pClient->GetPlayerSlot() + 1);
	m_pForward->PushStringEx(szCommandString, sizeof(szCommandString), SM_PARAM_STRING_UTF8 | SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
	m_pForward->Execute(&res);

	if (res != Pl_Continue)
	{
		if (res == Pl_Changed)
		{
#ifdef PLATFORM_LINUX
			// If it's a CGameClient, the pointers will differ
			if (pClient != META_IFACEPTR(IClient))
			{
				RETURN_META_VALUE_MNEWPARAMS(MRES_HANDLED, true, CGameClient_ExecuteStringCommand, (szCommandString));
			}
#endif

			RETURN_META_VALUE_NEWPARAMS(MRES_HANDLED, true, &IClient::ExecuteStringCommand, (szCommandString));
		}

		RETURN_META_VALUE(MRES_SUPERCEDE, false);
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

CForwardManager::ExecuteStringCommandPostHook::ExecuteStringCommandPostHook()
{
	m_Hooks[PTaH_ExecuteStringCommandPost] = this;
}

void CForwardManager::ExecuteStringCommandPostHook::Init()
{
	m_pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 2, nullptr, Param_Cell, Param_String);
}

int CForwardManager::ExecuteStringCommandPostHook::VPHook(int iClient)
{
	IClient* pClient = iserver->GetClient(iClient - 1);

#ifdef PLATFORM_LINUX
	SourceHook::MemFuncInfo mfi;
	SourceHook::GetFuncInfo(&IClient::ExecuteStringCommand, mfi);

	int offset = GetParentVFuncOffset(pClient, mfi.vtblindex);
	if (offset != -1)
	{
		SH_MANUALHOOK_RECONFIGURE(CGameClient_ExecuteStringCommand, offset, 0, 0);

		m_iGameHookId = SH_ADD_MANUALVPHOOK(CGameClient_ExecuteStringCommand, static_cast<CGameClient*>(pClient), SH_MEMBER(this, &CForwardManager::ExecuteStringCommandPostHook::Handler), true);
	}
	else
	{
		smutils->LogError(myself, "Failed to get CGameClient::ExecuteStringCommand offset, ExecuteStringCommandPost hook functionality will be limited.");
	}
#endif

	return SH_ADD_VPHOOK(IClient, ExecuteStringCommand, pClient, SH_MEMBER(this, &CForwardManager::ExecuteStringCommandPostHook::Handler), true);
}

bool CForwardManager::ExecuteStringCommandPostHook::Handler(const char *pCommandString)
{
	IClient* pClient = ForceCastToIClient(META_IFACEPTR(IClient));

	m_pForward->PushCell(pClient->GetPlayerSlot() + 1);
	m_pForward->PushString(pCommandString);
	m_pForward->Execute(nullptr);

	RETURN_META_VALUE(MRES_IGNORED, true);
}

CForwardManager::ClientConnectHook::ClientConnectHook()
{
	m_iHookID = -1;
}

bool CForwardManager::ClientConnectHook::Configure()
{
	if(!iserver)
	{
		smutils->LogError(myself, "iserver is nullptr, hook %s will be unavailable.", GetHookName());

		return false;
	}

	int offset = -1;
	if(!g_pGameConf[GameConf_PTaH]->GetOffset("CBaseServer::ConnectClient", &offset))
	{
		smutils->LogError(myself, "Failed to get CBaseServer::ConnectClient offset, hook %s will be unavailable.", GetHookName());

		return false;
	}

	SH_MANUALHOOK_RECONFIGURE(ConnectClient, offset, 0, 0);

	return true;
}

// Thanks Peace-Maker!
// https://github.com/peace-maker/sourcetvmanager/blob/067b238bd21c4fba4b877cb0a514011a1141e1a6/forwards.cpp#L267
const char* CForwardManager::ClientConnectHook::ExtractPlayerName(CUtlVector<CCLCMsg_SplitPlayerConnect_t*>& splitScreenClients)
{
	for (int i = 0; i < splitScreenClients.Count(); i++)
	{
		CCLCMsg_SplitPlayerConnect_t* split = splitScreenClients[i];
		if(!split->has_convars())
		{
			continue;
		}

		const CMsg_CVars& cvars = split->convars();
		for (int c = 0; c < cvars.cvars_size(); c++)
		{
			const CMsg_CVars_CVar& cvar = cvars.cvars(c);
			if(!cvar.has_name() || !cvar.has_value())
			{
				continue;
			}

			if(cvar.name() == "name")
			{
				return cvar.value().c_str();
			}
		}
	}

	return "";
}

void CForwardManager::ClientConnectHook::OnInternalHookActivated()
{
	BaseClass::OnInternalHookDeactivated();

	m_iHookID = ManualHook();
}

void CForwardManager::ClientConnectHook::OnInternalHookDeactivated()
{
	BaseClass::OnInternalHookDeactivated();

	if(m_iHookID != -1)
	{
		SH_REMOVE_HOOK_ID(m_iHookID);
		m_iHookID = -1;
	}
}

CForwardManager::ClientConnectPreHook::ClientConnectPreHook()
{
	m_Hooks[PTaH_ClientConnectPre] = this;

	m_iRejectConnectionOffset = -1;
}

void CForwardManager::ClientConnectPreHook::Init()
{
	if(BaseClass::Configure())
	{
		m_pForward = forwards->CreateForwardEx(nullptr, ET_Hook, 5, nullptr, Param_Cell, Param_String, Param_String, Param_String, Param_String);

		if (!g_pGameConf[GameConf_PTaH]->GetOffset("CBaseServer::RejectConnection", &m_iRejectConnectionOffset))
		{
			smutils->LogError(myself, "Failed to get CBaseServer::RejectConnection offset, %s hook functionality will be limited.", GetHookName());
		}
	}
}

int CForwardManager::ClientConnectPreHook::ManualHook()
{
	return SH_ADD_MANUALHOOK(ConnectClient, iserver, SH_MEMBER(this, &CForwardManager::ClientConnectPreHook::Handler), false);
}

IClient* CForwardManager::ClientConnectPreHook::Handler(const ns_address& adr, int protocol, int challenge, int authProtocol, const char* name, const char* password, const char* hashedCDkey, int cdKeyLen, CUtlVector<CCLCMsg_SplitPlayerConnect_t*>& splitScreenClients, bool isClientLowViolence, CrossPlayPlatform_t clientPlatform, const byte* pbEncryptionKey, int nEncryptionKeyIndex)
{
	if(authProtocol == 3 && cdKeyLen >= static_cast<int>(sizeof(CSteamID)))
	{
		cell_t res = Pl_Continue;
		ns_address_render sAdr(adr);
		char rejectReason[255];
		char passwordNew[128];

		V_strncpy(passwordNew, password, sizeof(passwordNew));

		m_pForward->PushCell(reinterpret_cast<const CSteamID*>(hashedCDkey)->GetAccountID());
		m_pForward->PushString(sAdr.String());
		m_pForward->PushString(ExtractPlayerName(splitScreenClients));
		m_pForward->PushStringEx(passwordNew, sizeof(passwordNew), SM_PARAM_STRING_UTF8 | SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
		m_pForward->PushStringEx(rejectReason, sizeof(rejectReason), SM_PARAM_STRING_UTF8 | SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
		m_pForward->Execute(&res);

		if(res != Pl_Continue)
		{
			if(res == Pl_Changed)
			{
				RETURN_META_VALUE_MNEWPARAMS(MRES_HANDLED, nullptr, ConnectClient, (adr, protocol, challenge, authProtocol, name, passwordNew, hashedCDkey, cdKeyLen, splitScreenClients, isClientLowViolence, clientPlatform, pbEncryptionKey, nEncryptionKeyIndex));
			}

			if(m_iRejectConnectionOffset != -1)
			{
				CallVFMTFunc<void, const ns_address&>(m_iRejectConnectionOffset, iserver, adr, "%s", rejectReason);
			}

			RETURN_META_VALUE(MRES_SUPERCEDE, nullptr);
		}
	}

	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

CForwardManager::ClientConnectPostHook::ClientConnectPostHook()
{
	m_Hooks[PTaH_ClientConnectPost] = this;
}

void CForwardManager::ClientConnectPostHook::Init()
{
	if(BaseClass::Configure())
	{
		m_pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 4, nullptr, Param_Cell, Param_Cell, Param_String, Param_String);
	}
}

int CForwardManager::ClientConnectPostHook::ManualHook()
{
	return SH_ADD_MANUALHOOK(ConnectClient, iserver, SH_MEMBER(this, &CForwardManager::ClientConnectPostHook::Handler), true);
}

IClient* CForwardManager::ClientConnectPostHook::Handler(const ns_address& adr, int protocol, int challenge, int authProtocol, const char* name, const char* password, const char* hashedCDkey, int cdKeyLen, CUtlVector<CCLCMsg_SplitPlayerConnect_t*>& splitScreenClients, bool isClientLowViolence, CrossPlayPlatform_t clientPlatform, const byte* pbEncryptionKey, int nEncryptionKeyIndex)
{
	if(authProtocol == 3 && cdKeyLen >= static_cast<int>(sizeof(CSteamID)))
	{
		IClient* pClient = META_RESULT_ORIG_RET(IClient*);
		if(pClient)
		{
			ns_address_render sAdr(adr);

			m_pForward->PushCell(pClient->GetPlayerSlot() + 1);
			m_pForward->PushCell(reinterpret_cast<const CSteamID*>(hashedCDkey)->GetAccountID());
			m_pForward->PushString(sAdr.String());
			m_pForward->PushString(ExtractPlayerName(splitScreenClients));
			m_pForward->Execute(nullptr);
		}
	}

	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

CForwardManager::InventoryUpdatePostHook::InventoryUpdatePostHook() : CBaseVPHook(true)
{
	m_Hooks[PTaH_InventoryUpdatePost] = this;
}

void CForwardManager::InventoryUpdatePostHook::Init()
{
	int offset = -1;
	if (!g_pGameConf[GameConf_PTaH]->GetOffset("CPlayerInventory::SendInventoryUpdateEvent", &offset))
	{
		smutils->LogError(myself, "Failed to get CPlayerInventory::SendInventoryUpdateEvent offset, hook InventoryUpdatePost will be unavailable.");

		return;
	}

	SH_MANUALHOOK_RECONFIGURE(SendInventoryUpdateEvent, offset, 0, 0);

	m_pForward = forwards->CreateForwardEx(nullptr, ET_Ignore, 2, nullptr, Param_Cell, Param_Cell);
}

int CForwardManager::InventoryUpdatePostHook::VPHook(int iClient)
{
	CCSPlayerInventory* pPlayerInventory = CCSPlayerInventory::FromPlayer(gamehelpers->ReferenceToEntity(iClient));

	return SH_ADD_MANUALVPHOOK(SendInventoryUpdateEvent, pPlayerInventory, SH_MEMBER(this, &CForwardManager::InventoryUpdatePostHook::Handler), true);
}

void CForwardManager::InventoryUpdatePostHook::Handler()
{
	CCSPlayerInventory* pPlayerInventory = META_IFACEPTR(CCSPlayerInventory);

	m_pForward->PushCell(gamehelpers->EntityToBCompatRef(pPlayerInventory->ToPlayer()));
	m_pForward->PushCell(reinterpret_cast<cell_t>(pPlayerInventory));
	m_pForward->Execute(nullptr);

	RETURN_META(MRES_IGNORED);
}
