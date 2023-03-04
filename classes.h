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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_CLASSES_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_CLASSES_H_

#include "utlhashmaplarge.h"
#include "netmessages.pb.h"
#include "inetchannelinfo.h"
#include "igameevents.h"

typedef CBaseEntity CBaseCombatWeapon;
class CAttribute_String;
class stickerMaterialReference_t;
enum eEconItemOrigin { };
enum EAssetClassAttrExportRule_t { };
enum attrib_effect_types_t { };

enum EStickerAttributeType
{
	EStickerAttribute_ID,
	EStickerAttribute_Wear,
	EStickerAttribute_Scale,
	EStickerAttribute_Rotation
};

enum ESchemaAttributeType
{
	ESchemaAttribute_Unknown = -1,
	ESchemaAttribute_Uint32,
	ESchemaAttribute_Float,
	ESchemaAttribute_String,
	ESchemaAttribute_Vector
};

class ISchemaAttributeType
{
protected:
	virtual ~ISchemaAttributeType() = 0;
};

template<typename T> class ISchemaAttributeTypeBase : public ISchemaAttributeType {};
template<typename T> class CSchemaAttributeTypeBase : public ISchemaAttributeTypeBase<T> {};
template<typename T> class CSchemaAttributeTypeProtobufBase : public CSchemaAttributeTypeBase<T> {};

class CSchemaAttributeType_Default : public CSchemaAttributeTypeBase<uint32> {};
class CSchemaAttributeType_Uint32 : public CSchemaAttributeTypeBase<uint32> {};
class CSchemaAttributeType_Float : public CSchemaAttributeTypeBase<float> {};
class CSchemaAttributeType_String : public CSchemaAttributeTypeProtobufBase<CAttribute_String> {};
class CSchemaAttributeType_Vector : public CSchemaAttributeTypeBase<Vector> {};

class CEconItemAttributeDefinition
{
public:
	virtual uint16 GetDefinitionIndex() const = 0;
	virtual const char* GetDefinitionName() const = 0;
	virtual const char* GetDescriptionString() const = 0;
	virtual const char* GetAttributeClass() const = 0;
	virtual const KeyValues* GetRawDefinition() const = 0;

	uint16 GetDefinitionIndex() { return m_nDefIndex; }
	const char* GetDefinitionName() { return m_pszDefinitionName; }
	bool IsStoredAsInteger() { return m_bStoredAsInteger; }
	bool IsStoredAsFloat() { return !m_bStoredAsInteger; }

	template<typename T> bool IsAttributeType()
	{
		return (dynamic_cast<T*>(this->m_pAttrType) != nullptr);
	}

	ESchemaAttributeType GetAttributeType();

	KeyValues* m_pKVAttribute; //4
	uint16 m_nDefIndex; //8
	ISchemaAttributeType* m_pAttrType; //12
	bool m_bHidden; //16
	bool m_bWebSchemaOutputForced; //17
	bool m_bStoredAsInteger; //18
	bool m_bInstanceData; //19
	EAssetClassAttrExportRule_t m_eAssetClassAttrExportRule; //20
	uint32 m_unAssetClassBucket; //24
	attrib_effect_types_t m_iEffectType; //28
	int m_iDescriptionFormat; //32
	const char* m_pszDescriptionString; //36
	const char* m_pszDescriptionTag; //40
	const char* m_pszArmoryDesc; //44
	int m_iScore; //48
	const char* m_pszDefinitionName; //52
	const char* m_pszAttributeClass; //56
	mutable string_t m_iszAttributeClass; //60
};

class CEconItemDefinition
{
public:
	uint16 GetDefinitionIndex() { return m_nDefIndex; }
	int GetLoadoutSlot(int iTeam);
	int GetUsedByTeam();
	int GetNumSupportedStickerSlots();
	const char* GetInventoryImage();
	const char* GetBasePlayerDisplayModel();
	const char* GetWorldDisplayModel();
	const char* GetWorldDroppedModel();
	const char* GetDefinitionName();

private:
	void* m_pVTable; //0
public:
	KeyValues* m_pKVItem; //4
	uint16 m_nDefIndex; //8
};

class CEconItemSchema
{
public:
	static void* operator new(size_t) throw();
	static void operator delete(void*) { };

	CUtlHashMapLarge<int, CEconItemDefinition*>* GetItemDefinitionMap();
	CUtlVector<CEconItemAttributeDefinition*>* GetAttributeDefinitionContainer();

	CEconItemDefinition* GetItemDefinitionByName(const char* pszDefName);
	CEconItemDefinition* GetItemDefinitionByDefIndex(uint16_t iItemIndex);
	CEconItemAttributeDefinition* GetAttributeDefinitionByName(const char* pszDefName);
	CEconItemAttributeDefinition* GetAttributeDefinitionByDefIndex(uint16_t iDefIndex);
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

class CEconItemAttribute
{
	DECLARE_CLASS_NOBASE(CEconItemAttribute);

	CEconItemAttribute();

public:
	DECLARE_EMBEDDED_NETWORKVAR();

	CEconItemAttribute(const uint16 iAttributeIndex, uint32 unValue);
	CEconItemAttribute(const uint16 iAttributeIndex, float fValue);

	// This is the index of the attribute into the attributes read from the data files
	CNetworkVar(uint16, m_iAttributeDefinitionIndex);

	// This is the value of the attribute. Used to modify the item's variables.
	CNetworkVar(float, m_flValue);

	// This is the value that the attribute was first set to by an item definition
	CNetworkVar(float, m_flInitialValue);
	CNetworkVar(int, m_nRefundableCurrency);

	CNetworkVar(bool, m_bSetBonus); // Attribute has been generated by a set bonus.
};
static_assert(sizeof(CEconItemAttribute) == 24, "CEconItemAttribute - incorrect size on this compiler");

#ifdef PLATFORM_WINDOWS
//https://github.com/alliedmodders/sourcemod/blob/c5619f887d6d13643ad8281e8e7479668226c342/core/HalfLife2.cpp#L1234
template< class T, class I = int >
class CUtlMemoryGlobalMalloc : public CUtlMemory< T, I >
{
	typedef CUtlMemory< T, I > BaseClass;

public:
	using BaseClass::BaseClass;

	void Purge()
	{
		if (!IsExternallyAllocated())
		{
			if (m_pMemory)
			{
				UTLMEMORY_TRACK_FREE();
				g_pMemAlloc->Free((void*)m_pMemory);
				m_pMemory = 0;
			}
			m_nAllocationCount = 0;
		}
		BaseClass::Purge();
	}
};
#endif

class CAttributeList
{
public:
	void DestroyAllAttributes() { m_Attributes.Purge(); }
	void AddAttribute(CEconItemAttribute& pAttribute) { m_Attributes.AddToTail(pAttribute); }
	void RemoveAttribute(int iIndex) { m_Attributes.Remove(iIndex); }
	void RemoveAttributeByDefIndex(uint16_t unAttrDefIndex);
	void SetOrAddAttributeValue(uint16_t unAttrDefIndex, uint32_t unValue);
	int GetNumAttributes() { return m_Attributes.Count(); }
	CEconItemAttribute& GetAttribute(int iIndex) { return m_Attributes[iIndex]; }
	CEconItemAttribute* GetAttributeByDefIndex(uint16_t unAttrDefIndex);

private:
	void* m_pVTable; //0
#ifdef PLATFORM_WINDOWS
	CUtlVector<CEconItemAttribute, CUtlMemoryGlobalMalloc<CEconItemAttribute> > m_Attributes; //4 (20)
#else
	CUtlVector<CEconItemAttribute> m_Attributes; //4 (20)
#endif
	void* m_pAttributeManager; //24
};

#pragma pack(push, 4)
class CEconItemView
{
	virtual ~CEconItemView() = 0;
public:
	virtual int GetCustomPaintKitIndex() const = 0;
	virtual int GetCustomPaintKitSeed() const = 0;
	virtual float GetCustomPaintKitWear(float flWearDefault = 0.0f) const = 0;
	virtual float GetStickerAttributeBySlotIndexFloat(int nSlotIndex, EStickerAttributeType type, float flDefault) const = 0;
	virtual uint32 GetStickerAttributeBySlotIndexInt(int nSlotIndex, EStickerAttributeType type, uint32 uiDefault) const = 0;
	virtual bool IsTradable() const = 0;
	virtual bool IsMarketable() const = 0;
	virtual bool IsCommodity() const = 0;
	virtual bool IsUsableInCrafting() const = 0;
	virtual bool IsHiddenFromDropList() const = 0;
	virtual RTime32 GetExpirationDate() const = 0;
	virtual CEconItemDefinition* GetItemDefinition() const = 0;
	virtual uint32 GetAccountID() const = 0;
	virtual uint64 GetItemID() const = 0;
	virtual int32 GetQuality() const = 0;
	virtual int32 GetRarity() const = 0;
	virtual uint8 GetFlags() const = 0;
	virtual eEconItemOrigin GetOrigin() const = 0;
	virtual uint16 GetQuantity() const = 0;
	virtual uint32 GetItemLevel() const = 0;
	virtual bool GetInUse() const = 0;
	virtual const char* GetCustomName() const = 0;
	virtual const char* GetCustomDesc() const = 0;
	virtual int GetItemSetIndex() const = 0;
	virtual void IterateAttributes(IEconItemAttributeIterator* pIterator) const = 0;

	uint32 GetAccountID() { return m_iAccountID; }
	uint64 GetItemID() { return m_iItemID; }
	int GetKillEaterValue();

private:
	bool m_bKillEaterTypesCached; //4
	void* m_vCachedKillEaterTypes[7]; //8 (28)
	int m_nKillEaterValuesCacheFrame; //36
	void* m_vCachedKillEaterValues[6]; //40 (24)
	CUtlVector<stickerMaterialReference_t> m_pStickerMaterials; //64 (20)

public:
	uint16 m_iDefinitionIndex; //84
	int m_iEntityQuality; //88
	uint32 m_iEntityLevel; //92
	uint64 m_iItemID; //96
	uint32 m_iItemIDHigh; //104
	uint32 m_iItemIDLow; //108
	uint32 m_iAccountID; //112
	uint32 m_iInventoryPosition; //116
	void* m_pNonSOEconItem; //120
	bool m_bInitialized; //124

	CAttributeList m_AttributeList; //128 (28)
	CAttributeList m_NetworkedDynamicAttributesForDemos; //156 (28)

	char m_szCustomName[161]; //184
	char m_szCustomNameOverride[161]; //345
	void* m_autoptrInventoryImageGeneratedPath; //508
};
static_assert(sizeof(CEconItemView) == 512, "CEconItemView - incorrect size on this compiler");
#pragma pack(pop)

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

class CBaseClient : public IGameEventListener2, public IClient
{
public:
	virtual ~CBaseClient() = 0;
};

class CClientFrameManager
{
public:
	virtual ~CClientFrameManager() = 0;
};

class CGameClient : public CBaseClient, public CClientFrameManager
{
};

class CCSPlayerInventory
{
	static intptr_t GetInventoryOffset();
public:
	static CCSPlayerInventory* FromPlayer(CBaseEntity* pPlayer);
	CBaseEntity* ToPlayer();

	CEconItemView* GetItemInLoadout(int iTeam, int iLoadoutSlot);
	CUtlVector<CEconItemView*>* GetItemVector();
};

class CNetMessagePB_PlayerAvatarData : public INetMessage, public CNETMsg_PlayerAvatarData
{
public:
	CNetMessagePB_PlayerAvatarData() { }

	virtual bool ReadFromBuffer(bf_read& buffer) { return false; }
	virtual bool WriteToBuffer(bf_write& buffer);
	virtual const char* ToString() const;
	virtual int GetType() const { return net_PlayerAvatarData; }
	virtual size_t GetSize() const { return sizeof(*this); }
	virtual const char* GetName() const { return "CNETMsg_PlayerAvatarData"; };
	virtual int GetGroup() const { return INetChannelInfo::PAINTMAP; }
	virtual void SetReliable(bool state) { }
	virtual bool IsReliable() const { return true; }
	virtual INetMessage* Clone() const { return nullptr; }
	virtual void SetNetChannel(INetChannel* netchan) { }
	virtual INetChannel* GetNetChannel(void) const { return nullptr; }
	virtual bool Process() { return false; }

protected:
	mutable std::string	m_toString;
};

extern CEconItemSchema* g_pCEconItemSchema;
extern CPlayerVoiceListener* g_pCPlayerVoiceListener;

#endif // _INCLUDE_SOURCEMOD_EXTENSION_CLASSES_H_