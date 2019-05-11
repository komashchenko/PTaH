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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_FORWARDS_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_FORWARDS_H_

#include "extension.h"
#include "netmessages.pb.h"
#include <amtl/am-thread-utils.h> 


template <int Type, class NetMessage, int Group, bool reliable>
class CNetMessagePB : public INetMessage, public NetMessage {
public:
	~CNetMessagePB() {}

};

typedef CNetMessagePB<16, CCLCMsg_SplitPlayerConnect, 0, true>	NetMsg_SplitPlayerConnect;

class CEconItemView;

class CForwardManager
{
public:
	bool Init();
	void Shutdown();

	//GiveNamedItem
	CBaseEntity *GiveNamedItem(const char *szItem, int iSubType, CEconItemView *pView, bool removeIfNotCarried, Vector *pOrigin);
	//GiveNamedItemPre
	CBaseEntity *GiveNamedItemPre(const char *szItem, int iSubType, CEconItemView *pView, bool removeIfNotCarried, Vector *pOrigin);
	//WeaponCanUse
	bool WeaponCanUse(CBaseCombatWeapon *pWeapon);
	//SetModel
	CBaseEntity *SetModel(const char *sModel);
	//SetModelPre
	CBaseEntity *SetModelPre(const char *sModel);
	//ClientPrint
	void ClientPrint(edict_t *pEdict, const char *szMsg);
	//OnClientConnect
	IClient *OnClientConnect(const netadr_t & address, int nProtocol, int iChallenge, int nAuthProtocol, const char *pchName, const char *pchPassword, const char *pCookie, int cbCookie, CUtlVector<NetMsg_SplitPlayerConnect *> &pSplitPlayerConnectVector, bool bUnknown, CrossPlayPlatform_t platform, const unsigned char *pUnknown, int iUnknown);


	void HookClient(int client);
	void UnhookClient(int client);

	void OnGameFrame(bool simulating);

	int m_iHookId[5][MAXPLAYERS + 1];

	IChangeableForward *m_pGiveNamedItem;
	IChangeableForward *m_pGiveNamedItemPre;
	bool IgnoredCEconItemView;
	CDetour *m_pFindMatchingWeaponsForTeamLoadout;
	IChangeableForward *m_pWeaponCanUse;
	IChangeableForward *m_pSetModel;
	IChangeableForward *m_pSetModelPre;
	IChangeableForward *m_pClientPrintf;
	IChangeableForward *m_pOnClientConnect;
	CDetour *m_pDExecuteStringCommand;
	IChangeableForward *m_pExecuteStringCommand;
	CDetour *m_pCDownloadListGenerator;
	IChangeableForward *m_pMapContentList;
	CDetour *m_pLoggingSeverity;
	IChangeableForward *m_pServerConsolePrint;

	ke::ThreadId Thread_Id;
};

extern CForwardManager g_pPTaHForwards;

#endif // _INCLUDE_SOURCEMOD_EXTENSION_FORWARDS_H_
