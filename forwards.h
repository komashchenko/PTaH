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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_FORWARDS_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_FORWARDS_H_

#include "extension.h"
#include "natives.h"
#include "classes.h"
#include "netmessages.pb.h"


template <int Type, class NetMessage, int Group, bool reliable>
class CNetMessagePB : public INetMessage, public NetMessage {
public:
	~CNetMessagePB() { }
};

typedef CNetMessagePB<16, CCLCMsg_SplitPlayerConnect, 0, true>	NetMsg_SplitPlayerConnect;

class CEconItemView;
class CGameClient;

class CForwardManager : public IClientListener, public IPluginsListener
{
public:
	void Init();
	void Shutdown();

	bool FunctionUpdateHook(PTaH_HookEvent htType, IPluginFunction* pFunction, bool bHook);

	void LevelShutdown();

public: // IClientListener
	virtual void OnClientConnected(int iClient);
	virtual void OnClientPutInServer(int iClient);
	virtual void OnClientDisconnected(int iClient);

public: // IPluginsListener
	virtual void OnPluginUnloaded(IPlugin* plugin);

	class TempleHookClient
	{
	public:
		TempleHookClient();

		virtual void Shutdown();
		void HookClient(int iClient);
		void UnHookClient(int iClient);
		bool UpdateForward(IPluginFunction* pFunc, bool bHook);
		virtual void UpdateHook();

	protected:
		virtual int __SH_ADD_MANUALHOOK(CBaseEntity* pEnt) = 0; //Crutch

		IChangeableForward* pForward = nullptr;

		int iHookId[SM_MAXPLAYERS + 1];
		bool bHooked = false;
		int iOffset = -1;
	};

	class TempleHookVP
	{
	public:
		void Shutdown();
		virtual void Hook(int iClient) = 0;
		bool UpdateForward(IPluginFunction* pFunc, bool bHook);
		void UpdateHook();

	protected:
		IChangeableForward* pForward = nullptr;

		int iHookId = -1;
		bool bHooked = false;
		int iOffset = -1;

		bool bInGame = false;
	};

	class TempleHookBaseClient : public TempleHookVP
	{
	public:
		virtual void Hook(int iClient) override;

	protected:
		virtual int __SH_ADD_MANUALVPHOOK(CBaseClient* pBaseClient) = 0; //Crutch
	};

	class GiveNamedItemPre : public TempleHookClient
	{
	public:
		void Init();
		virtual void Shutdown() override;
		virtual void UpdateHook() override;

		bool bIgnoredCEconItemView = false;

	private:
		virtual int __SH_ADD_MANUALHOOK(CBaseEntity* pEnt) override;

		CBaseEntity* SHHook(const char* szItem, int iSubType, CEconItemView* pView, bool removeIfNotCarried, Vector* pOrigin);

		CDetour* pFindMatchingWeaponsForTeamLoadout = nullptr;
	} GiveNamedItemPre;

	class GiveNamedItemPost : public TempleHookClient
	{
	public:
		void Init();

	private:
		virtual int __SH_ADD_MANUALHOOK(CBaseEntity* pEnt) override;

		CBaseEntity* SHHook(const char* szItem, int iSubType, CEconItemView* pView, bool removeIfNotCarried, Vector* pOrigin);
	} GiveNamedItemPost;

	class WeaponCanUsePre : public TempleHookClient
	{
	public:
		void Init();

	private:
		virtual int __SH_ADD_MANUALHOOK(CBaseEntity* pEnt) override;

		bool SHHook(CBaseCombatWeapon* pWeapon);
	} WeaponCanUsePre;

	class WeaponCanUsePost : public TempleHookClient
	{
	public:
		void Init();

	private:
		virtual int __SH_ADD_MANUALHOOK(CBaseEntity* pEnt) override;

		bool SHHook(CBaseCombatWeapon* pWeapon);
	} WeaponCanUsePost;

	class SetPlayerModelPre : public TempleHookClient
	{
	public:
		void Init();

	private:
		virtual int __SH_ADD_MANUALHOOK(CBaseEntity* pEnt) override;

		CBaseEntity* SHHook(const char* sModel);
	} SetPlayerModelPre;

	class SetPlayerModelPost : public TempleHookClient
	{
	public:
		void Init();

	private:
		virtual int __SH_ADD_MANUALHOOK(CBaseEntity* pEnt) override;

		CBaseEntity* SHHook(const char* sModel);
	} SetPlayerModelPost;

	class ClientVoiceToPre
	{
	public:
		void Init();
		void Shutdown();
		bool UpdateForward(IPluginFunction* pFunc, bool bHook);
		void UpdateHook();

	protected:
		bool SHHook(int iReceiver, int iSender, bool bListen);
		void SHHookClientVoice(edict_t* pEdict);

		IChangeableForward* pForward = nullptr;

		bool bStartVoice[SM_MAXPLAYERS + 1] = { };
		bool bHooked = false;
	} ClientVoiceToPre;

	class ClientVoiceToPost
	{
	public:
		void Init();
		void Shutdown();
		bool UpdateForward(IPluginFunction* pFunc, bool bHook);
		void UpdateHook();

	protected:
		bool SHHook(int iReceiver, int iSender, bool bListen);

		IChangeableForward* pForward = nullptr;

		bool bHooked = false;
	} ClientVoiceToPost;

	class ConsolePrintPre : public TempleHookBaseClient
	{
	public:
		void Init();

	protected:
		virtual int __SH_ADD_MANUALVPHOOK(CBaseClient* pBaseClient) override;

		void SHHook(const char* szFormat);
	} ConsolePrintPre;

	class ConsolePrintPost : public TempleHookBaseClient
	{
	public:
		void Init();

	protected:
		virtual int __SH_ADD_MANUALVPHOOK(CBaseClient* pBaseClient) override;

		void SHHook(const char* szFormat);
	} ConsolePrintPost;

	class ExecuteStringCommandPre : public TempleHookBaseClient
	{
	public:
		void Init();

	protected:
		virtual int __SH_ADD_MANUALVPHOOK(CBaseClient* pBaseClient) override;

		bool SHHook(const char* pCommandString);
	} ExecuteStringCommandPre;

	class ExecuteStringCommandPost : public TempleHookBaseClient
	{
	public:
		void Init();

	protected:
		virtual int __SH_ADD_MANUALVPHOOK(CBaseClient* pBaseClient) override;

		bool SHHook(const char* pCommandString);
	} ExecuteStringCommandPost;

	class ClientConnectPre
	{
	public:
		void Init();
		void Shutdown();
		bool UpdateForward(IPluginFunction* pFunc, bool bHook);
		void UpdateHook();

	protected:
		IClient* SHHook(const netadr_t& address, int nProtocol, int iChallenge, int nAuthProtocol, const char* pchName, const char* pchPassword, const char* pCookie, int cbCookie, CUtlVector<NetMsg_SplitPlayerConnect*>& pSplitPlayerConnectVector, bool bUnknown, CrossPlayPlatform_t platform, const unsigned char* pUnknown, int iUnknown);

		IChangeableForward* pForward = nullptr;

		bool bHooked = false;
		int iOffset = -1;
		int iHookId = -1;
	} ClientConnectPre;

	class ClientConnectPost
	{
	public:
		void Init();
		void Shutdown();
		bool UpdateForward(IPluginFunction* pFunc, bool bHook);
		void UpdateHook();

	protected:
		IClient* SHHook(const netadr_t& address, int nProtocol, int iChallenge, int nAuthProtocol, const char* pchName, const char* pchPassword, const char* pCookie, int cbCookie, CUtlVector<NetMsg_SplitPlayerConnect*>& pSplitPlayerConnectVector, bool bUnknown, CrossPlayPlatform_t platform, const unsigned char* pUnknown, int iUnknown);

		IChangeableForward* pForward = nullptr;

		bool bHooked = false;
		int iOffset = -1;
		int iHookId = -1;
	} ClientConnectPost;

	class SendInventoryUpdateEventPost : public TempleHookVP
	{
	public:
		SendInventoryUpdateEventPost();

		void Init();
		virtual void Hook(int iClient) override;

	protected:
		void SHHook();
	} InventoryUpdatePost;
};

extern CForwardManager g_ForwardManager;

#endif // _INCLUDE_SOURCEMOD_EXTENSION_FORWARDS_H_
