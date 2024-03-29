﻿/**
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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_NATIVES_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_NATIVES_H_

enum PTaH_HookEvent
{
	PTaH_GiveNamedItemPre = 10,
	PTaH_GiveNamedItemPost,
	PTaH_WeaponCanUsePre,
	PTaH_WeaponCanUsePost,
	PTaH_SetPlayerModelPre,
	PTaH_SetPlayerModelPost,
	PTaH_ClientVoiceToPre,
	PTaH_ClientVoiceToPost,
	PTaH_ConsolePrintPre,
	PTaH_ConsolePrintPost,
	PTaH_ExecuteStringCommandPre,
	PTaH_ExecuteStringCommandPost,
	PTaH_ClientConnectPre,
	PTaH_ClientConnectPost,
	PTaH_InventoryUpdatePost = 25,

	PTaH_MAXHOOKS
};

enum PTaH_ModelType
{
	ViewModel = 0,
	WorldModel,
	DroppedModel
};

extern const sp_nativeinfo_t g_ExtensionNatives[];

#endif // _INCLUDE_SOURCEMOD_EXTENSION_NATIVES_H_