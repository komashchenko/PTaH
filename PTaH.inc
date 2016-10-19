#if defined _PTaH_included
  #endinput
#endif
#define _PTaH_included

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
	PTaH_ExecuteStringCommand
};

enum EStickerAttributeType 
{ 
    StickerID,
    WearProgress,
    PatternScale,
    PatternRotation 
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
	 * @param def		default value.
	 *	-
	 * @return	Returns LoadoutSlot.
	 *	-
	 * @error CEconItemDefinition == NULL.
	*/
	public native int GetLoadoutSlot(int def = -1);
	
	/**
	 *	Gets the amount slots for Sticker.
	 *	-
	 * @return	Returns StickerSlotsCount.
	 *	-
	 * @error CEconItemDefinition == NULL.
	*/
	public native int GetNumSupportedStickerSlots();
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
		return this.GetAccountID() != 0 ? true:false;
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
	 *	-
	 * @noreturn
	*/
	function void (int iClient, const char[] sClassname, const CEconItemView Item, int iEnt);
	
	/** GiveNamedItemPre
	 *
	 *	Called before the issuance of the item.
	 *	-
	 * @param iClient			Player Index.
	 * @param sClassname		Classname weapons.
	 * @param Item				Customization item.
	 *	-
	 * @return	Return Plugin_Stop or Plugin_Handled stop granting item. Return Plugin_Continue allow issuance item without changes. Returns Plugin_Changed allow issuance item with changes.
	*/
	function Action (int iClient, char sClassname[64], CEconItemView &Item);
	
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
	
	/** ConsolePrint/ExecuteStringCommand
	 * ConsolePrint
	 *
	 *	Called before displaying messages to the console player.
	 *	-
	 * @param iClient			Player Index.
	 * @param sMessage			Text Message.
	 *	-
	 * @return	Return Plugin_Stop or Plugin_Handled restrict display message. Return Plugin_Continue allow the display message without changes. Return Plugin_Changed allow display changed message.
	 *	-
	 * ExecuteStringCommand
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
	 * @return	Return Plugin_Stop or Plugin_Handled restrict autherization client. Return Plugin_Continue allow autherization without changes. Возврат Plugin_Changed allow autherization with changes.
	*/
	function Action (const char[] sName, char sPassword[128], const char[] sIp, const char[] sSteamID, char rejectReason[512]);
};

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
 * @param sClassName					ClassName weapons.
 *	-
 * @return	Returns CEconItemDefinition.
*/
native CEconItemDefinition PTaH_GetItemDefinitionByName(const char[] sClassName);

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
 *	-
 * @return	Return index item.
 *	-
 * @error invalid iClient.
*/
native int PTaH_GivePlayerItem(int iClient, const char[] sClassname, CEconItemView Item);

/**
 *	gets md5 hash file.
 * @param sFile					The path to the file.
 * @param sBuffer				The string will be recorded hash.
 * @param len					length of the line.
 *	-
 * @return	Return true if successful.
 *	-
 * @error sFile does not exist.
*/
native bool PTaH_MD5File(const char[] sFile, char[] sBuffer, int len);

/**
 *	Gets ip host.
 * @param sHostName					domain.
 *	-
 * @return	Return true if successful.
 *	-
 * @error -.
*/
native int PTaH_GetHostByName(const char[] sHostName);

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
	MarkNativeAsOptional("PTaH");
	MarkNativeAsOptional("PTaH_GetItemDefinitionByName");
	MarkNativeAsOptional("CEconItemDefinition.GetDefinitionIndex");
	MarkNativeAsOptional("CEconItemDefinition.GetLoadoutSlot");
	MarkNativeAsOptional("CEconItemDefinition.GetNumSupportedStickerSlots");
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
	MarkNativeAsOptional("PTaH_GetHostByName");
}
#endif
