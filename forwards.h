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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_FORWARDS_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_FORWARDS_H_

#include "extension.h"
#include "natives.h"
#include "classes.h"
#include "netmessages.pb.h"
#include <ns_address.h>
#include <map>

class CForwardManager
{
	friend class CCSPlayer_FindMatchingWeaponsForTeamLoadoutClass;

public:
	void Init();
	void Shutdown();
	bool UpdateHook(PTaH_HookEvent htType, IPluginFunction* pFunction, bool bHook);

private:
	class CBaseHook : public IPluginsListener
	{
	public:
		virtual void Init() = 0;
		virtual void Shutdown();
		virtual bool UpdateHook(SourcePawn::IPluginFunction* pFunction, bool bHook);

	protected:
		CBaseHook();
		virtual void UpdateInternalHook();
		virtual void OnInternalHookActivated();
		virtual void OnInternalHookDeactivated();

	private: // IPluginsListener
		void OnPluginUnloaded(IPlugin* plugin) override;

	protected:
		IChangeableForward* m_pForward;
		bool m_bHooked;

	public:
		class HookTypeRegistrator
		{
		public:
			HookTypeRegistrator() = default;
			HookTypeRegistrator(CBaseHook* pBaseHook, PTaH_HookEvent htType);
		} m_HookType;
	};

	/////////////////////////////////

	class CBaseManualPlayerHook : public CBaseHook, public IClientListener
	{
		typedef CBaseHook BaseClass;

	protected:
		CBaseManualPlayerHook();

	private:
		virtual int ManualHook(int iClient) = 0;

	protected: // CBaseHook
		void OnInternalHookActivated() override;
		void OnInternalHookDeactivated() override;

	private: // IClientListener
		void OnClientPutInServer(int iClient) override;
		void OnClientDisconnected(int iClient) override;

	private:
		int m_iHookID[SM_MAXPLAYERS + 1];
	};

	class CBaseVPHook : public CBaseHook, public IClientListener
	{
		typedef CBaseHook BaseClass;

	protected:
		CBaseVPHook(bool bInGame);

	private:
		virtual int VPHook(int iClient) = 0;
		void OnClientValid(int iClient);

	private: // CBaseHook
		void OnInternalHookActivated() override;
		void OnInternalHookDeactivated() override;

	private: // IClientListener
		void OnClientConnected(int iClient) override;
		void OnClientPutInServer(int iClient) override;

	private:
		int m_iHookID;
		bool m_bInGame;
	};

	class CBaseClientHook : public CBaseVPHook
	{
		typedef CBaseVPHook BaseClass;

	protected:
		CBaseClientHook();
		IClient* ForceCastToIClient(IClient* pClient);

#ifdef PLATFORM_LINUX
	private: // CBaseVPHook
		void OnInternalHookDeactivated() override;

	protected:
		int GetParentVFuncOffset(IClient* pClient, size_t vtbIndex);

	protected:
		int m_iGameHookId;
#endif
	};

	/////////////////////////////////

	class GiveNamedItemHook : public CBaseManualPlayerHook
	{
	protected:
		bool Configure();

	private:
		virtual const char* GetHookName() = 0;
	};

	class GiveNamedItemPreHook : public GiveNamedItemHook
	{
		typedef GiveNamedItemHook BaseClass;
		friend class CCSPlayer_FindMatchingWeaponsForTeamLoadoutClass;

	public:
		GiveNamedItemPreHook();

	private: // CBaseHook
		void Init() override;
		void Shutdown() override;

	private: // CBaseManualPlayerHook
		int ManualHook(int iClient) override;
		void OnInternalHookActivated() override;
		void OnInternalHookDeactivated() override;

	private:
		CBaseEntity* Handler(const char* pchName, int iSubType, CEconItemView* pScriptItem, bool bForce, Vector* pOrigin);

		const char* GetHookName() override { return "GiveNamedItemPre"; }

	private:
		CDetour* m_pFindMatchingWeaponsForTeamLoadoutHook;
		int m_iFrameCount;
	} m_GiveNamedItemPreHook;

	class GiveNamedItemPostHook : public GiveNamedItemHook
	{
		typedef GiveNamedItemHook BaseClass;

	public:
		GiveNamedItemPostHook();

	private: // CBaseHook
		void Init() override;

	private: // CBaseManualPlayerHook
		int ManualHook(int iClient) override;

	private:
		CBaseEntity* Handler(const char* pchName, int iSubType, CEconItemView* pScriptItem, bool bForce, Vector* pOrigin);

		const char* GetHookName() override { return "GiveNamedItemPost"; }
	} m_GiveNamedItemPostHook;

	/////////////////////////////////

	class WeaponCanUseHook : public CBaseManualPlayerHook
	{
	protected:
		bool Configure();

	private:
		virtual const char* GetHookName() = 0;
	};

	class WeaponCanUsePreHook : public WeaponCanUseHook
	{
		typedef WeaponCanUseHook BaseClass;

	public:
		WeaponCanUsePreHook();

	private: // CBaseHook
		void Init() override;

	private: // CBaseManualPlayerHook
		int ManualHook(int iClient) override;

	private:
		bool Handler(CBaseCombatWeapon* pWeapon);

		const char* GetHookName() override { return "WeaponCanUsePre"; }
	} m_WeaponCanUsePreHook;

	class WeaponCanUsePostHook : public WeaponCanUseHook
	{
		typedef WeaponCanUseHook BaseClass;

	public:
		WeaponCanUsePostHook();

	private: // CBaseHook
		void Init() override;

	private: // CBaseManualPlayerHook
		int ManualHook(int iClient) override;

	private:
		bool Handler(CBaseCombatWeapon* pWeapon);

		const char* GetHookName() override { return "WeaponCanUsePost"; }
	} m_WeaponCanUsePostHook;

	/////////////////////////////////

	class SetPlayerModelHook : public CBaseManualPlayerHook
	{
	protected:
		bool Configure();

	private:
		virtual const char* GetHookName() = 0;
	};

	class SetPlayerModelPreHook : public SetPlayerModelHook
	{
		typedef SetPlayerModelHook BaseClass;

	public:
		SetPlayerModelPreHook();

	private: // CBaseHook
		void Init() override;

	private: // CBaseManualPlayerHook
		int ManualHook(int iClient) override;

	private:
		void Handler(const char* pszModelName);

		const char* GetHookName() override { return "SetPlayerModelPre"; }
	} m_SetPlayerModelPreHook;

	class SetPlayerModelPostHook : public SetPlayerModelHook
	{
		typedef SetPlayerModelHook BaseClass;

	public:
		SetPlayerModelPostHook();

	private: // CBaseHook
		void Init() override;

	private: // CBaseManualPlayerHook
		int ManualHook(int iClient) override;

	private:
		void Handler(const char* pszModelName);

		const char* GetHookName() override { return "SetPlayerModelPost"; }
	} m_SetPlayerModelPostHook;

	/////////////////////////////////

	class ClientVoiceToPreHook : public CBaseHook
	{
		typedef CBaseHook BaseClass;

	public:
		ClientVoiceToPreHook();

	private: // CBaseHook
		void Init() override;
		void OnInternalHookActivated() override;
		void OnInternalHookDeactivated() override;

	private:
		bool Handler(int iReceiver, int iSender, bool bListen);
		void ClientVoiceHandler(edict_t* pEdict);

	private:
		bool m_bStartVoice[SM_MAXPLAYERS + 1];
	} m_ClientVoiceToPreHook;

	class ClientVoiceToPostHook : public CBaseHook
	{
		typedef CBaseHook BaseClass;

	public:
		ClientVoiceToPostHook();

	private: // CBaseHook
		void Init() override;
		void OnInternalHookActivated() override;
		void OnInternalHookDeactivated() override;

	private:
		bool Handler(int iReceiver, int iSender, bool bListen);
	} m_ClientVoiceToPostHook;

	/////////////////////////////////

	class ConsolePrintPreHook : public CBaseClientHook
	{
	public:
		ConsolePrintPreHook();

	private: // CBaseHook
		void Init() override;

	private: // CBaseVPHook
		int VPHook(int iClient) override;

	private:
		void Handler(const char* szFormat);
	} m_ConsolePrintPreHook;

	class ConsolePrintPostHook : public CBaseClientHook
	{
	public:
		ConsolePrintPostHook();

	private: // CBaseHook
		void Init() override;

	private: // CBaseVPHook
		int VPHook(int iClient) override;

	private:
		void Handler(const char* szFormat);
	} m_ConsolePrintPostHook;

	/////////////////////////////////

	class ExecuteStringCommandPreHook : public CBaseClientHook
	{
	public:
		ExecuteStringCommandPreHook();

	private: // CBaseHook
		void Init() override;

	private: // CBaseVPHook
		int VPHook(int iClient) override;

	private:
		bool Handler(const char* pCommandString);
	} m_ExecuteStringCommandPreHook;

	class ExecuteStringCommandPostHook : public CBaseClientHook
	{
	public:
		ExecuteStringCommandPostHook();

	private: // CBaseHook
		void Init() override;

	private: // CBaseVPHook
		int VPHook(int iClient) override;

	private:
		bool Handler(const char* pCommandString);
	} m_ExecuteStringCommandPostHook;

	/////////////////////////////////

	class ClientConnectHook : public CBaseHook
	{
		typedef CBaseHook BaseClass;

	protected:
		ClientConnectHook();
		bool Configure();
		const char* ExtractPlayerName(CUtlVector<CCLCMsg_SplitPlayerConnect_t*>& splitScreenClients);

	private:
		virtual const char* GetHookName() = 0;
		virtual int ManualHook() = 0;

	private: // CBaseHook
		void OnInternalHookActivated() override;
		void OnInternalHookDeactivated() override;

	private:
		int m_iHookID;
	};

	class ClientConnectPreHook : public ClientConnectHook
	{
		typedef ClientConnectHook BaseClass;

	public:
		ClientConnectPreHook();

	private: // CBaseHook
		void Init() override;

	private: // ClientConnectHook
		int ManualHook() override;

	private:
		IClient* Handler(const ns_address& adr, int protocol, int challenge, int authProtocol, const char* name, const char* password, const char* hashedCDkey, int cdKeyLen, CUtlVector<CCLCMsg_SplitPlayerConnect_t*>& splitScreenClients, bool isClientLowViolence, CrossPlayPlatform_t clientPlatform, const byte* pbEncryptionKey, int nEncryptionKeyIndex);

		const char* GetHookName() override { return "ClientConnectPre"; }

	private:
		int m_iRejectConnectionOffset;
	} m_ClientConnectPreHook;

	class ClientConnectPostHook : public ClientConnectHook
	{
		typedef ClientConnectHook BaseClass;

	public:
		ClientConnectPostHook();

	private: // CBaseHook
		void Init() override;

	private: // ClientConnectHook
		int ManualHook() override;

	private:
		IClient* Handler(const ns_address& adr, int protocol, int challenge, int authProtocol, const char* name, const char* password, const char* hashedCDkey, int cdKeyLen, CUtlVector<CCLCMsg_SplitPlayerConnect_t*>& splitScreenClients, bool isClientLowViolence, CrossPlayPlatform_t clientPlatform, const byte* pbEncryptionKey, int nEncryptionKeyIndex);

		const char* GetHookName() override { return "ClientConnectPost"; }
	} m_ClientConnectPostHook;

	/////////////////////////////////

	class InventoryUpdatePostHook : public CBaseVPHook
	{
		typedef CBaseVPHook BaseClass;

	public:
		InventoryUpdatePostHook();

	private: // CBaseHook
		void Init() override;

	private: // CBaseVPHook
		int VPHook(int iClient) override;

	private:
		void Handler();
	} m_InventoryUpdatePostHook;

	/////////////////////////////////

private:
	static std::map<PTaH_HookEvent, CBaseHook*> m_Hooks;
};

extern CForwardManager g_ForwardManager;

#endif // _INCLUDE_SOURCEMOD_EXTENSION_FORWARDS_H_
