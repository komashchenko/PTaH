/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod P Tools and Hooks Extension
 * Copyright (C) 2016-2019 Phoenix (˙·٠●Феникс●٠·˙).  All rights reserved.
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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_CLASSES_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_CLASSES_H_

class CEconItemAttributeDefinition;
class CAttribute_String;

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
	uint16_t GetDefinitionIndex();
	int GetLoadoutSlot(int iTeam);
	int GetNumSupportedStickerSlots();
	const char* GetClassName();
};

class CEconItemSchema
{
public:
	static void* operator new(size_t) throw();
	static void operator delete(void*) { };
	CEconItemDefinition* GetItemDefinitionByName(const char* classname);
	CEconItemDefinition* GetItemDefinitionByDefIndex(uint16_t DefIndex);

	CEconItemAttributeDefinition* GetAttributeDefinitionByDefIndex(uint16_t DefIndex);
};

class IEconItemAttributeIterator
{
public:
	virtual ~IEconItemAttributeIterator() {	};
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, unsigned int) = 0;
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, float) = 0;
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, CAttribute_String const&) = 0;
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, Vector const&) = 0;
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
	CEconItemDefinition* GetItemDefinition();
	int GetAccountID();
	int GetQuality();
	int GetRarity();
	int GetFlags();
	int GetOrigin();
	const char* GetCustomName();
	int GetKillEaterValue();

	void IterateAttributes(IEconItemAttributeIterator* AttributeIterator);
};

class IEconItemUntypedAttributeIterator : public IEconItemAttributeIterator
{
public:
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, unsigned int);
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, float);
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, CAttribute_String const&);
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, Vector const&);
	virtual bool OnIterateAttributeValueUntyped(CEconItemAttributeDefinition const*) = 0;
};

class CAttributeIterator_HasAttribute : public IEconItemUntypedAttributeIterator
{
public:
	CAttributeIterator_HasAttribute(CEconItemAttributeDefinition const*);
	virtual bool OnIterateAttributeValueUntyped(CEconItemAttributeDefinition const*);

	CEconItemAttributeDefinition const* m_pItemAttrDef;
	bool m_found;
};

class CAttributeIterator_GetTypedAttributeValueBase : public IEconItemAttributeIterator
{
public:
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, unsigned int) { return true; }
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, float) { return true; }
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, CAttribute_String const&) { return true; }
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const*, Vector const&) { return true; }
};

template <class A, class B>
class CAttributeIterator_GetTypedAttributeValue : public CAttributeIterator_GetTypedAttributeValueBase
{
public:
	CAttributeIterator_GetTypedAttributeValue(CEconItemAttributeDefinition const* pItemAttrDef, B* value)
		: m_pItemAttrDef(pItemAttrDef), m_value(value), m_found(false)
	{
	}
	virtual bool OnIterateAttributeValue(CEconItemAttributeDefinition const* pItemAttrDef, A value)
	{
		if (m_pItemAttrDef == pItemAttrDef)
		{
			m_found = true;
			*m_value = (B)value;
		}

		return !m_found;
	}

	CEconItemAttributeDefinition const* m_pItemAttrDef;
	B* m_value;
	bool m_found;
};

class CPlayerVoiceListener
{
public:
	static void* operator new(size_t) throw();
	static void operator delete(void*) { };

	bool IsPlayerSpeaking(int iClient);
};

class CGameClient
{
public:
	inline IClient* ToIClient()
	{
		return (IClient*)((intptr_t)this + sizeof(void*));
	}
};

#define IClientToGameClient(pClient) (CGameClient*)((intptr_t)(pClient) - sizeof(void*))


extern CEconItemSchema* g_pCEconItemSchema;
extern CPlayerVoiceListener* g_pCPlayerVoiceListener;

#endif // _INCLUDE_SOURCEMOD_EXTENSION_CLASSES_H_