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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

 /**
 * @file extension.h
 * @brief Sample extension code header.
 */


// networkvar.h must be included before smsdk_ext.h !!!
#include "networkvar.h"
#include "smsdk_ext.h"
#include <ISDKTools.h>
#include <iplayerinfo.h>
#include <iserver.h>
#include <server_class.h>
#include <ivoiceserver.h>
#include <CDetour/detours.h>
#include <inetmessage.h>


#ifdef PLATFORM_WINDOWS
#define WIN_LINUX(win, linux) win
#else
#define WIN_LINUX(win, linux) linux
#endif

template <typename T, typename ...Args>
constexpr T CallVFunc(size_t index, void* pThis, Args... args) noexcept
{
    return reinterpret_cast<T(WIN_LINUX(__thiscall, __cdecl)*)(void*, Args...)>(reinterpret_cast<void***>(pThis)[0][index])(pThis, args...);
}

template <typename T, typename ...Args, typename ...Va>
constexpr T CallVFMTFunc(size_t index, void* pThis, Args... args, const char* fmt, Va... va) noexcept
{
	return reinterpret_cast<T(__cdecl*)(void*, Args..., const char*, ...)>(reinterpret_cast<void***>(pThis)[0][index])(pThis, args..., fmt, va...);
}


/**
 * @brief Sample implementation of the SDK Extension.
 * Note: Uncomment one of the pre-defined virtual functions in order to use it.
 */
class PTaH : public SDKExtension
{
public:
	/**
	 * @brief This is called after the initial loading sequence has been processed.
	 *
	 * @param error		Error message buffer.
	 * @param maxlength	Size of error message buffer.
	 * @param late		Whether or not the module was loaded after map load.
	 * @return			True to succeed loading, false to fail.
	 */
	virtual bool SDK_OnLoad(char* error, size_t maxlength, bool late);

	/**
	 * @brief This is called right before the extension is unloaded.
	 */
	virtual void SDK_OnUnload();

	/**
	 * @brief This is called once all known extensions have been loaded.
	 * Note: It is is a good idea to add natives here, if any are provided.
	 */
	virtual void SDK_OnAllLoaded();

	/**
	 * @brief Called when the pause state is changed.
	 */
	 //virtual void SDK_OnPauseChange(bool paused);

	 /**
	  * @brief this is called when Core wants to know if your extension is working.
	  *
	  * @param error		Error message buffer.
	  * @param maxlength	Size of error message buffer.
	  * @return			True if working, false otherwise.
	  */
	  //virtual bool QueryRunning(char *error, size_t maxlength);
public:
#if defined SMEXT_CONF_METAMOD
	/**
	 * @brief Called when Metamod is attached, before the extension version is called.
	 *
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @param late			Whether or not Metamod considers this a late load.
	 * @return				True to succeed, false to fail.
	 */
	virtual bool SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlength, bool late);

	/**
	 * @brief Called when Metamod is detaching, after the extension version is called.
	 * NOTE: By default this is blocked unless sent from SourceMod.
	 *
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @return				True to succeed, false to fail.
	 */
	 //virtual bool SDK_OnMetamodUnload(char *error, size_t maxlength);

	 /**
	  * @brief Called when Metamod's pause state is changing.
	  * NOTE: By default this is blocked unless sent from SourceMod.
	  *
	  * @param paused		Pause state being set.
	  * @param error			Error buffer.
	  * @param maxlength		Maximum size of error buffer.
	  * @return				True to succeed, false to fail.
	  */
	  //virtual bool SDK_OnMetamodPauseChange(bool paused, char *error, size_t maxlength);
#endif
};

/* Interfaces from SourceMod */
#define GameConf_SDKT 0
#define GameConf_SDKH 1
#define GameConf_PTaH 2
#define GameConf_CSST 3

extern IGameConfig* g_pGameConf[4];
extern ISDKTools* sdktools;
extern IServer* iserver;
extern IVoiceServer* voiceserver;
extern CGlobalVars* gpGlobals;
extern IServerGameClients* serverClients;

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_