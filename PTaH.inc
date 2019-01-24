#if defined _PTaH_included
  #endinput
#endif
#define _PTaH_included


#define AF_UNSPEC	0
#define AF_INET	2
#define AF_INET6	10

#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46


enum PTaH_HookType
{
	Hook,
	UnHook
};

enum PTaH_Hook
{
	PTaH_GiveNamedItem = 0,
	PTaH_GiveNamedItemPre,
	PTaH_WeaponCanUse,
	PTaH_SetPlayerModel,
	PTaH_SetPlayerModelPre,
	PTaH_ConsolePrint,
	PTaH_MapContentList,
	PTaH_OnClientConnect,
	PTaH_ExecuteStringCommand,
	PTaH_ServerConsolePrint
};

enum EStickerAttributeType 
{ 
    StickerID,
    WearProgress,
    PatternScale,
    PatternRotation 
};

enum LoggingSeverity
{
	//-----------------------------------------------------------------------------
	// An informative logging message.
	//-----------------------------------------------------------------------------
	LS_MESSAGE = 0,

	//-----------------------------------------------------------------------------
	// A warning, typically non-fatal
	//-----------------------------------------------------------------------------
	LS_WARNING = 1,

	//-----------------------------------------------------------------------------
	// A message caused by an Assert**() operation.
	//-----------------------------------------------------------------------------
	LS_ASSERT = 2,

	//-----------------------------------------------------------------------------
	// An error, typically fatal/unrecoverable.
	//-----------------------------------------------------------------------------
	LS_ERROR = 3,

	//-----------------------------------------------------------------------------
	// A placeholder level, higher than any legal value.
	// Not a real severity value!
	//-----------------------------------------------------------------------------
	LS_HIGHEST_SEVERITY = 4,
};

methodmap CEconItemDefinition
{
	//!!!!!!!!!!!!!!!!!!!!! CEconItemDefinition is not Handle, CloseHandle() - NOT NEEDED !!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!! Always check, if not wounded CEconItemDefinition - NULL ( if(ItemDefinition) ) !!!!!!!!!!!!!!!!!!!!!

	/**
	 *	Gets DefinitionIndex.
	 *	-
	 * @return	Returns DefinitionIndex.
	 *	-
	 * @error CEconItemDefinition == NULL.
	*/
	public native int GetDefinitionIndex();
	
	/**
	 *	Gets LoadoutSlot.
	 * @param iTeam		Index Commands.
	 *	-
	 * @return	Returns LoadoutSlot.
	 *	-
	 * @error CEconItemDefinition == NULL.
	*/
	public native int GetLoadoutSlot(int iTeam = -1);
	
	/**
	 *	Gets the amount slots for Sticker.
	 *	-
	 * @return	Returns StickerSlotsCount.
	 *	-
	 * @error CEconItemDefinition == NULL.
	*/
	public native int GetNumSupportedStickerSlots();
	
	/**
	 *	Gets ClassName.
	 * @param sBuf			line.
	 * @param iLen			row size.
	 *	-
	 * @return	Returns length ClassName.
	 *	-
	 * @error CEconItemDefinition == NULL.
	*/
	public native int GetClassName(char[] sBuf, int iLen);
};

methodmap CEconItemView
{
	//!!!!!!!!!!!!!!!!!!!!! CEconItemView is not Handle, CloseHandle() - NOT NEEDED !!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!! Always check, if not wounded CEconItemDefinition - NULL ( if(ItemDefinition) ) !!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!! If a player will left from a server after function call to obtain CEconItemView (PTaH_GetEconItemViewFromWeapon this applies if iEnt will be destroyed) You get crash server!!!!!!!!!!!!!!!!!!!!!
	
	/**
	 *	Gets the index of the skin weapons.
	 *	-
	 * @return	Returns PaintKitIndex.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native int GetCustomPaintKitIndex();
	
	/**
	 *	Gets displacement of weapon skins.
	 *	-
	 * @return	Returns PaintKitSeed.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native int GetCustomPaintKitSeed();
	
	/**
	 *	Gets quality skins weapons.
	 * @param def			default value.
	 *	-
	 * @return	Returns PaintKitWear.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native float GetCustomPaintKitWear(float def = -1.0);
	
	/**
	 *	Get the value of an attribute by number slots Sticker.
	 * @param iSlot			Index Slot.
	 * @param ESAT			attribute.
	 * @param def			default value.
	 *	-
	 * @return	Returns StickerAttribute.
	 *	-
	 * @error CEconItemView == NULL.
	 * -
	 * StickerID - returns int, def also need to transmit int
	 * WearProgress, PatternScale, PatternRotation - returns float, def also need to transmit float
	*/
	public native any GetStickerAttributeBySlotIndex(int iSlot, EStickerAttributeType ESAT, any def);
	
	/**
	 *	Check: Is it possible to exchange weapons
	 *	-
	 * @return	Returns Tradable.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native bool IsTradable();
	
	/**
	 *	Check: Is it possible to sell weapons - http://steamcommunity.com/market/
	 *	-
	 * @return	Returns Marketable.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native bool IsMarketable();
	
	/**
	 *	Gets ItemDefinition.
	 *	-
	 * @return	Returns CEconItemDefinition.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native CEconItemDefinition GetItemDefinition();
	
	/**
	 *	Gets AccountID Owner skin (You can verify by GetSteamAccountID).
	 *	-
	 * @return	Returns AccountID.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native int GetAccountID();
	
	/**
	 *	Gets Index Owner weapons.
	 *	-
	 * @return	Returns iClient.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public int GetClientIndex() 
    { 
		int AccountID = this.GetAccountID();
		for (int i = 1; i <= MaxClients; i++) if(IsClientInGame(i) && AccountID == GetSteamAccountID(i)) return i;
		return 0;
    }
	
	/**
	 *	Gets Custom ItemView.
	 *	-
	 * @return	Returns IsCustom.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public bool IsCustomItemView() 
    { 
		int iBuf = this.GetAccountID();
		return iBuf != 0 && iBuf != 1;
    }
	
	/**
	 *	Gets Quality skins.
	 *	-
	 * @return	Returns Quality.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native int GetQuality();
	
	/**
	 *	Gets Rarity skins.
	 *	-
	 * @return	Returns Rarity.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native int GetRarity();
	
	/**
	 *	Gets Flags skins.
	 *	-
	 * @return	Returns Flags.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native int GetFlags();
	
	/**
	 *	Gets Origin skins.
	 *	-
	 * @return	Returns Origin.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native int GetOrigin();
	
	/**
	 *	Gets Name-tag skins.
	 * @param sBuf			line.
	 * @param iLen			row size (maximum length, which can be - 161).
	 *	-
	 * @return	Returns length name-tag.
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native int GetCustomName(char[] sBuf, int iLen);
	
	/**
	 *	Gets amount StatTrak.
	 *	-
	 * @return	Returns amount StatTrak (if StatTrak = -1 therefore it is not).
	 *	-
	 * @error CEconItemView == NULL.
	*/
	public native int GetStatTrakKill();
};

methodmap AddrInfo
{
	//!!!!!!!!!!!!!!!!!!!!! AddrInfo is not Handle, CloseHandle() - NOT NEEDED !!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!! Always check, if not wounded AddrInfo - NULL ( if(AddrInfo) ) !!!!!!!!!!!!!!!!!!!!!
	
	/**
	 *	Gets the version IP addresses (AF_INET или AF_INET6).
	 *	-
	 * @return	AF_INET или AF_INET6.
	 *	-
	 * @error AddrInfo == NULL.
	*/
	property int Family
	{
		public native get();
	}
	
	/**
	 *	It records the IP address in line.
	 *	-
	 * @noreturn
	 *	-
	 * @error AddrInfo == NULL.
	*/
	public native void GetIP(char[] sBuffer, int len);
	
	/**
	 *	Gets AddrInfo next IP.
	 *	-
	 * @return	AddrInfo.
	 *	-
	 * @error AddrInfo == NULL.
	*/
	property AddrInfo NextIP
	{
		public native get();
	}
	
	/**
	 *	Clears the selected for AddrInfo memory (!!! only need to clean the main AddrInfo, AddrInfo received from NextIP You do not need to clean it !!!).
	 *	-
	 * @noreturn
	 *	-
	 * @error AddrInfo == NULL.
	*/
	public native void ClearMem();
};


typeset PTaHCB
{
	/** GiveNamedItem
	 *
	 *	It called when a player receives a item.
	 *	-
	 * @param iClient			Player Index.
	 * @param sClassname		Classname item.
	 * @param Item				Customization item.
	 * @param iEnt				Index Entities item.
	 * @param OriginIsNULL		True if Origin is NULL_VECTOR.
	 * @param Origin			Coordinates where the item was created at (You cannot compare Origin == NULL_VECTOR, use OriginIsNULL param for it).
	 *	-
	 * @noreturn
	*/
	function void (int iClient, const char[] sClassname, const CEconItemView Item, int iEnt, bool OriginIsNULL, const float Origin[3]);
	
	/** GiveNamedItemPre
	 *
	 *	Called before the issuance of the item.
	 *	-
	 * @param iClient			Player Index.
	 * @param sClassname		Classname weapons.
	 * @param Item				Customization item.
	 * @param IgnoredCEconItemView	If true ignores CEconItemView the item.
	 * @param OriginIsNULL		True if Origin is NULL_VECTOR.
	 * @param Origin			Coordinates where the item was created at (You cannot compare Origin == NULL_VECTOR or set Origin = NULL_VECTOR, use OriginIsNULL param for it).
	 *	-
	 * @return	Return Plugin_Stop or Plugin_Handled stop granting item. Return Plugin_Continue allow issuance item without changes. Returns Plugin_Changed allow issuance item with changes.
	*/
	function Action (int iClient, char sClassname[64], CEconItemView &Item, bool &IgnoredCEconItemView, bool &OriginIsNULL, float Origin[3]);
	
	/** WeaponCanUse
	 *
	 *	Called when a player is trying to pickup the item.
	 *	-
	 * @param iClient			Player Index.
	 * @param iEnt				Index Entities item.
	 * @param CanUse			For return without change.
	 *	-
	 * @return	Return false restrict pickup item. Return CanUse returns a response without changes. Return true allow pickup item.
	*/
	function bool (int iClient, int iEnt, bool CanUse);
	
	/** SetPlayerModel
	 *
	 *	Called after the change of the player model.
	 *	-
	 * @param iClient			Player Index.
	 * @param sModel			The path to the model.
	 *	-
	 * @noreturn
	*/
	function void (int iClient, const char[] sModel);
	
	/** SetPlayerModelPre
	 *
	 *	Called before changing the player model.
	 *	-
	 * @param iClient			Player Index.
	 * @param sModel			The path to the current model player.
	 * @param sNewModel			The path to a new model.
	 *	-
	 * @return	Return Plugin_Stop or Plugin_Handled stop changing models. Returns Plugin_Continue allow change model without changes. Returns Plugin_Changed allow the change to the modified model.
	*/
	function Action (int iClient, const char[] sModel, char sNewModel[128]);
	
	/** ConsolePrint
	 *
	 *	Called before displaying messages to the console player.
	 *	-
	 * @param iClient			Player Index.
	 * @param sMessage			Text Message.
	 *	-
	 * @return	Return Plugin_Stop or Plugin_Handled restrict display message. Return Plugin_Continue allow the display message without changes. Return Plugin_Changed allow display changed message.
	 *	-
	 */
	function Action (int iClient, char sMessage[1024]);
	
	/** ExecuteStringCommand
	 *
	 *	Called before executing the command player of the team on the server.
	 *	-
	 * @param iClient			Player Index.
	 * @param sMessage			Command.
	 *	-
	 * @return	Return Plugin_Stop or Plugin_Handled restrict execution. Return Plugin_Continue allow execution without changes. Return Plugin_Changed allow execution with changes.
	*/
	function Action (int iClient, char sMessage[512]);
	
	/** MapContentList
	 *
	 *	Called before creating a map of content that will be added to the download(AddFileToDownloadsTable it does not cause this event).
	 *	-
	 * @param sFile				The path to the file that will be added to the download.
	 *	-
	 * @return	Return Plugin_Stop or Plugin_Handled restrict add content to download. Return Plugin_Continue allow add content to download without changes. Returns Plugin_Changed allow add content to download with changes.
	*/
	function Action (char sFile[128]);	
	
	/** OnClientConnect
	 *
	 *	Called before the authorization of the client to the server.
	 *	-
	 * @param sName				Nickname client.
	 * @param sPassword			Password witch he introduced.
	 * @param sIp				IP client.
	 * @param sSteamID			SteamID (2 version) client.
	 * @param rejectReason		The reason is not authorized.
	 *	-
	 * @return	Return Plugin_Stop or Plugin_Handled restrict autherization client. Return Plugin_Continue allow autherization without changes. Return Plugin_Changed allow autherization with changes.
	*/
	function Action (const char[] sName, char sPassword[128], const char[] sIp, const char[] sSteamID, char rejectReason[512]);
	
	/** ServerConsolePrint
	 *
	 *	Called before displaying a message in the server console (Be careful with the messages).
	 *	-
	 * @param sMessage				The message is printed to the server console.
	 * @param severity				Logging severity message.
	 * @param bBlockable			False if message was cached from another thread (Plugin_Stop and Plugin_Handled are ignored).
	 *	-
	 * @return	Return Plugin_Stop or Plugin_Handled restrict the display messages. Return Plugin_Continue allow display messages without changes.
	*/
	function Action (const char[] sMessage, LoggingSeverity severity, bool bBlockable);
};

/**
 *	PTaH Version.
 * @param sBuffer				The string will be written version (only need to specify if you want to get the version as a string).
 * @param len					Length of the line.
 *	-
 * @return	Return PTaH Version (or example 108, sBuffer = "1.0.8").
*/
native int PTaH_Version(char[] sBuf = NULL_STRING, int iLen = 0);

/**
 *	Enables Hook.
 * @param type					Type Hook.
 * @param hk					Hook/UnHook.
 * @param callback				Callback.
 *	-
 * @return	Return true if one is fortunate, otherwise false.
*/
native bool PTaH(PTaH_Hook hk, PTaH_HookType type, PTaHCB callback);

/**
 *	Gets CEconItemDefinition from ClassName weapons.
 * @param sClassName				ClassName weapons.
 *	-
 * @return	Returns CEconItemDefinition.
*/
native CEconItemDefinition PTaH_GetItemDefinitionByName(const char[] sClassName);

/**
 *	Gets CEconItemDefinition from DefinitionIndex.
 * @param DefIndex					DefinitionIndex weapons.
 *	-
 * @return	Returns CEconItemDefinition.
*/
native CEconItemDefinition PTaH_GetItemDefinitionByDefIndex(int DefIndex);

/**
 *	Gets CEconItemView weapons client.
 * @param iClient					Index Client.
 * @param iTeam						Index Commands.
 * @param iLoadoutSlot				Index Slot Weapons.
 *	-
 * @return	Returns CEconItemDefinition.
 *	-
 * @error invalid iClient, !(2 <= iTeam <= 3).
*/
native CEconItemView PTaH_GetItemInLoadout(int iClient, int iTeam, int iLoadoutSlot);

/**
 *	Gets CEconItemView weapons.
 * @param iEnt					Index weapons.
 *	-
 * @return	Returns CEconItemDefinition.
 *	-
 * @error invalid iEnt, iEnt not a weapon
*/
native CEconItemView PTaH_GetEconItemViewFromWeapon(int iEnt);

/**
 *	It gives the player item with the specified CEconItemView.
 * @param iClient				Index Player.
 * @param sClassname			Classname item.
 * @param Item					Customization item.
 * @param Origin				Coordinates the item will be created at, or NULL_VECTOR.
 *	-
 * @return	Return index item.
 *	-
 * @error invalid iClient.
*/
native int PTaH_GivePlayerItem(int iClient, const char[] sClassname, CEconItemView Item = view_as<CEconItemView>(0), const float Origin[3] = NULL_VECTOR);

/**
 *	gets md5 hash file.
 * @param sFile					The path to the file.
 * @param sBuffer				The string will be recorded hash.
 * @param len					Length of the line.
 *	-
 * @return	Return true if successful.
 *	-
 * @error sFile does not exist.
*/
native bool PTaH_MD5File(const char[] sFile, char[] sBuffer, int len);

/**
 *	Gets AddrInfo on hostname.
 * @param sHostName				Hostname.
 * @param family				Generation IP (AF_INET | AF_INET6 | AF_UNSPEC).
 * @param addr					The variable which will be recorded AddrInfo If successful.
 *	-
 * @return	Index error (if there is).
*/
native int PTaH_GetAddrInfo(const char[] sHostName, int family, AddrInfo &addr);

/**
 *	Gets an error in its index.
 * @param error					Index error.
 * @param sBuffer				Line where the error will be recorded.
 * @param len					Length sBuffer.
 *	-
 * @return	Amount written characters.
*/
native int PTaH_Gai_StrError(int error, char[] sBuffer, int len);

public Extension __ext_PTaH = 
{
	name = "PTaH",
	file = "PTaH.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public __ext_PTaH_SetNTVOptional()
{
	MarkNativeAsOptional("PTaH_Version");
	MarkNativeAsOptional("PTaH");
	MarkNativeAsOptional("PTaH_GetItemDefinitionByName");
	MarkNativeAsOptional("PTaH_GetItemDefinitionByDefIndex");
	MarkNativeAsOptional("CEconItemDefinition.GetDefinitionIndex");
	MarkNativeAsOptional("CEconItemDefinition.GetLoadoutSlot");
	MarkNativeAsOptional("CEconItemDefinition.GetNumSupportedStickerSlots");
	MarkNativeAsOptional("CEconItemDefinition.GetClassName");
	MarkNativeAsOptional("PTaH_GetItemInLoadout");
	MarkNativeAsOptional("PTaH_GetEconItemViewFromWeapon");
	MarkNativeAsOptional("CEconItemView.GetCustomPaintKitIndex");
	MarkNativeAsOptional("CEconItemView.GetCustomPaintKitSeed");
	MarkNativeAsOptional("CEconItemView.GetCustomPaintKitWear");
	MarkNativeAsOptional("CEconItemView.GetStickerAttributeBySlotIndex");
	MarkNativeAsOptional("CEconItemView.IsTradable");
	MarkNativeAsOptional("CEconItemView.IsMarketable");	
	MarkNativeAsOptional("CEconItemView.GetItemDefinition");
	MarkNativeAsOptional("CEconItemView.GetAccountID");
	MarkNativeAsOptional("CEconItemView.GetQuality");
	MarkNativeAsOptional("CEconItemView.GetRarity");
	MarkNativeAsOptional("CEconItemView.GetFlags");
	MarkNativeAsOptional("CEconItemView.GetOrigin");
	MarkNativeAsOptional("CEconItemView.GetCustomName");
	MarkNativeAsOptional("CEconItemView.GetStatTrakKill");
	MarkNativeAsOptional("PTaH_GivePlayerItem");
	MarkNativeAsOptional("PTaH_MD5File");
	MarkNativeAsOptional("PTaH_GetAddrInfo");
	MarkNativeAsOptional("PTaH_Gai_StrError");
	MarkNativeAsOptional("AddrInfo.Family.get");
	MarkNativeAsOptional("AddrInfo.GetIP");
	MarkNativeAsOptional("AddrInfo.NextIP.get");
	MarkNativeAsOptional("AddrInfo.ClearMem");
}
#endif