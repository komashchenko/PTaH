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

enum EStickerAttributeType 
{ 
    StickerID,
    WearProgress,
    PatternScale,
    PatternRotation 
};

class CEconItemDefinition
{
public:
	int GetDefinitionIndex();
	int GetLoadoutSlot(int def);
	int GetNumSupportedStickerSlots();
};

class CEconItemSchema
{
private:
	void *pSchema = nullptr;
public:
	CEconItemSchema();
	CEconItemDefinition *GetItemDefinitionByName(const char *classname);
};

class CEconItemView
{
public:
	int GetCustomPaintKitIndex();
	int GetCustomPaintKitSeed();
	float GetCustomPaintKitWear(float def);
	float GetStickerAttributeBySlotIndexFloat(int slot, EStickerAttributeType StickerAttribut, float def);
	int GetStickerAttributeBySlotIndexInt(int slot, EStickerAttributeType StickerAttribut, int def);
	bool IsTradable();
	bool IsMarketable();
	CEconItemDefinition *GetItemDefinition();
	int GetAccountID();
	int GetQuality();
	int GetRarity();
	int GetFlags();
	int GetOrigin();
	char *GetCustomName();
	int GetKillEaterValueByType(unsigned int type);
};

extern CEconItemSchema *g_pCEconItemSchema;