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
#include "forwards.h"
#include "natives.h"
#include "classes.h"


/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

PTaH g_PTaH;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_PTaH);

IGameConfig *g_pGameConf[4];

IBinTools *bintools = nullptr;
ISDKTools *sdktools = nullptr;
IServer *iserver = nullptr;

CEconItemSchema *g_pCEconItemSchema = nullptr;

extern const sp_nativeinfo_t g_ExtensionNatives[];

//LevelShutdown
void LevelShutdown();
SH_DECL_HOOK0_void(IServerGameDLL, LevelShutdown, SH_NOATTRIB, false);

bool PTaH::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	char conf_error[255];
	
	if(!gameconfs->LoadGameConfigFile("sdktools.games", &g_pGameConf[GameConf_SDKT], conf_error, sizeof(conf_error)))
	{
		snprintf(error, maxlength, "Could not read sdktools.games: %s", conf_error);
		return false;
	}
	
	if(!gameconfs->LoadGameConfigFile("sdkhooks.games", &g_pGameConf[GameConf_SDKH], conf_error, sizeof(conf_error)))
	{
		snprintf(error, maxlength, "Could not read sdkhooks.games: %s", conf_error);
		return false;
	}
	
	if(!gameconfs->LoadGameConfigFile("PTaH.games", &g_pGameConf[GameConf_PTaH], conf_error, sizeof(conf_error)))
	{
		snprintf(error, maxlength, "Could not read PTaH.games: %s", conf_error);
		return false;
	}
	
	if(!gameconfs->LoadGameConfigFile("sm-cstrike.games", &g_pGameConf[GameConf_CSST], conf_error, sizeof(conf_error)))
	{
		snprintf(error, maxlength, "Could not read sm-cstrike.games: %s", conf_error);
		return false;
	}
	
	CDetourManager::Init(smutils->GetScriptingEngine(), g_pGameConf[GameConf_PTaH]);
	
	sharesys->AddDependency(myself, "bintools.ext", true, true);
	sharesys->AddDependency(myself, "sdktools.ext", true, true);
	
	sharesys->AddNatives(myself, g_ExtensionNatives);
	sharesys->RegisterLibrary(myself, "PTaH");

	playerhelpers->AddClientListener(&g_PTaH);
	return true;
}

void PTaH::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(BINTOOLS, bintools);
	SM_GET_LATE_IFACE(SDKTOOLS, sdktools);
	
	iserver = sdktools->GetIServer();
	g_pCEconItemSchema = new CEconItemSchema();
	
	SH_ADD_HOOK(IServerGameDLL, LevelShutdown, gamedll, SH_STATIC(LevelShutdown), true);
	
	g_pPTaHForwards.Init();
}

void PTaH::SDK_OnUnload()
{
	for(int i = 0; i <= MAXPLAYERS; i++)
	{
		g_pPTaHForwards.UnhookClient(i);
	}
	gameconfs->CloseGameConfigFile(g_pGameConf[GameConf_SDKT]);
	gameconfs->CloseGameConfigFile(g_pGameConf[GameConf_SDKH]);
	gameconfs->CloseGameConfigFile(g_pGameConf[GameConf_PTaH]);
	gameconfs->CloseGameConfigFile(g_pGameConf[GameConf_CSST]);
	g_pPTaHForwards.Shutdown();
	SH_REMOVE_HOOK(IServerGameDLL, LevelShutdown, gamedll, SH_STATIC(LevelShutdown), true);
}

void PTaH::OnClientPutInServer(int client)
{
	g_pPTaHForwards.HookClient(client);
}
void PTaH::OnClientDisconnected(int client)
{
	g_pPTaHForwards.UnhookClient(client);
}

void LevelShutdown()
{
	for(int i = 0; i <= MAXPLAYERS; i++)
	{
		g_pPTaHForwards.UnhookClient(i);
	}
}