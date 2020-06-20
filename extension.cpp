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
#include "natives.h"
#include "classes.h"


 /**
  * @file extension.cpp
  * @brief Implement extension code here.
 */

PTaH g_PTaH;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_PTaH);

IGameConfig* g_pGameConf[4];

ISDKTools* sdktools = nullptr;
IServer* iserver = nullptr;
IVoiceServer* voiceserver = nullptr;
CGlobalVars* gpGlobals = nullptr;
IServerGameClients* serverClients = nullptr;

CEconItemSchema* g_pCEconItemSchema = nullptr;
CPlayerVoiceListener* g_pCPlayerVoiceListener = nullptr;


bool PTaH::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	char conf_error[255];

	if (!gameconfs->LoadGameConfigFile("sdktools.games", &g_pGameConf[GameConf_SDKT], conf_error, sizeof(conf_error)))
	{
		snprintf(error, maxlength, "Could not read sdktools.games: %s", conf_error);

		return false;
	}

	if (!gameconfs->LoadGameConfigFile("sdkhooks.games", &g_pGameConf[GameConf_SDKH], conf_error, sizeof(conf_error)))
	{
		snprintf(error, maxlength, "Could not read sdkhooks.games: %s", conf_error);

		return false;
	}

	if (!gameconfs->LoadGameConfigFile("PTaH.games", &g_pGameConf[GameConf_PTaH], conf_error, sizeof(conf_error)))
	{
		snprintf(error, maxlength, "Could not read PTaH.games: %s", conf_error);

		return false;
	}

	if (!gameconfs->LoadGameConfigFile("sm-cstrike.games", &g_pGameConf[GameConf_CSST], conf_error, sizeof(conf_error)))
	{
		snprintf(error, maxlength, "Could not read sm-cstrike.games: %s", conf_error);

		return false;
	}

	CDetourManager::Init(smutils->GetScriptingEngine(), g_pGameConf[GameConf_PTaH]);

	sharesys->AddDependency(myself, "sdktools.ext", true, true);

	sharesys->AddNatives(myself, g_ExtensionNatives);
	sharesys->RegisterLibrary(myself, "PTaH");

	return true;
}

void PTaH::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(SDKTOOLS, sdktools);

	iserver = sdktools->GetIServer();
	g_pCEconItemSchema = new CEconItemSchema();
	g_pCPlayerVoiceListener = new CPlayerVoiceListener();

	g_ForwardManager.Init();
}

void PTaH::SDK_OnUnload()
{
	g_ForwardManager.Shutdown();

	gameconfs->CloseGameConfigFile(g_pGameConf[GameConf_SDKT]);
	gameconfs->CloseGameConfigFile(g_pGameConf[GameConf_SDKH]);
	gameconfs->CloseGameConfigFile(g_pGameConf[GameConf_PTaH]);
	gameconfs->CloseGameConfigFile(g_pGameConf[GameConf_CSST]);
}

bool PTaH::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
	GET_V_IFACE_ANY(GetEngineFactory, voiceserver, IVoiceServer, INTERFACEVERSION_VOICESERVER);
	GET_V_IFACE_ANY(GetServerFactory, serverClients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);

	gpGlobals = ismm->GetCGlobals();

	return true;
}