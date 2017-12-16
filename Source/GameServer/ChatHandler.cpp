#include "stdafx.h"
#include "DBAgent.h"
#include "../shared/DateTime.h"
#include "../shared/packets.h"
#include <boost\foreach.hpp>
#include <boost\algorithm\string.hpp>
#include <chrono>

using std::string;
using namespace std::chrono;

ServerCommandTable CGameServerDlg::s_commandTable;
ChatCommandTable CUser::s_commandTable;

void CGameServerDlg::InitServerCommands() {
	static Command<CGameServerDlg> commandTable[] =
	{
		// Command				Handler												Help message
		{ "notice",				&CGameServerDlg::HandleNoticeCommand,				"Sends a server-wide chat notice." },
		{ "noticeall",			&CGameServerDlg::HandleNoticeallCommand,			"Sends a server-wide chat notice." },
		{ "kill",				&CGameServerDlg::HandleKillUserCommand,				"Disconnects the specified player" },
		{ "open1",				&CGameServerDlg::HandleWar1OpenCommand,				"Opens war zone 1" },
		{ "open2",				&CGameServerDlg::HandleWar2OpenCommand,				"Opens war zone 2" },
		{ "open3",				&CGameServerDlg::HandleWar3OpenCommand,				"Opens war zone 3" },
		{ "open4",				&CGameServerDlg::HandleWar4OpenCommand,				"Opens war zone 4" },
		{ "open5",				&CGameServerDlg::HandleWar5OpenCommand,				"Opens war zone 5" },
		{ "open6",				&CGameServerDlg::HandleWar6OpenCommand,				"Opens war zone 6" },
		{ "openardream",		&CGameServerDlg::HandleOpenArdreamCommand,			"Opens war zone ardream" },
		{ "opencz",				&CGameServerDlg::HandleOpenCZCommand,				"Opens war zone cz" },
		{ "closeczevent",		&CGameServerDlg::HandleCloseCZEventCommand,			"Close war zone event" },
		{ "closeardreamevent",	&CGameServerDlg::HandleCloseArdreamEventCommand,			"Close war zone event" },
		{ "snowopen",			&CGameServerDlg::HandleSnowWarOpenCommand,			"Opens the snow war zone" },
		{ "snowclose",			&CGameServerDlg::HandleSnowWarCloseCommand,			"Snow Closes the active snowwar zone" },
		{ "siegewarfare",		&CGameServerDlg::HandleSiegeWarOpenCommand,			"Opens the Castle Siege War zone" },
		{ "close",				&CGameServerDlg::HandleWarCloseCommand,				"Closes the active war zone" },
		{ "down",				&CGameServerDlg::HandleShutdownCommand,				"Shuts down the server" },
		{ "discount",			&CGameServerDlg::HandleDiscountCommand,				"Enables server discounts for the winning nation of the last war" },
		{ "alldiscount",		&CGameServerDlg::HandleGlobalDiscountCommand,		"Enables server discounts for everyone" },
		{ "offdiscount",		&CGameServerDlg::HandleDiscountOffCommand,			"Disables server discounts" },
		{ "captain",			&CGameServerDlg::HandleCaptainCommand,				"Sets the captains/commanders for the war" },
		{ "santa",				&CGameServerDlg::HandleSantaCommand,				"Enables a flying Santa Claus." },
		{ "offsanta",			&CGameServerDlg::HandleSantaOffCommand,				"Disables a flying Santa Claus/angel." },
		{ "angel",				&CGameServerDlg::HandleAngelCommand,				"Enables a flying angel." },
		{ "offangel",			&CGameServerDlg::HandleSantaOffCommand,				"Disables a flying Santa Claus/angel." },
		{ "permanent",			&CGameServerDlg::HandlePermanentChatCommand,		"Sets the permanent chat bar to the specified text." },
		{ "offpermanent",		&CGameServerDlg::HandlePermanentChatOffCommand,		"Resets the permanent chat bar text." },
		{ "reload_notice",		&CGameServerDlg::HandleReloadNoticeCommand,			"Reloads the in-game notice list." },
		{ "reload_tables",		&CGameServerDlg::HandleReloadTablesCommand,			"Reloads the in-game tables." },
		{ "reload_items",		&CGameServerDlg::HandleItemTablesCommand,			"Reloads the in-game tables." },
		{ "reload_magics",		&CGameServerDlg::HandleReloadMagicsCommand,			"Reloads the in-game magic tables." },
		{ "reload_bots",		&CGameServerDlg::HandleReloadBotsCommand,			"Reloads the in-game bot tables." },
		{ "reload_quests",		&CGameServerDlg::HandleReloadQuestCommand,			"Reloads the in-game quest tables." },
		{ "reload_ranks",		&CGameServerDlg::HandleReloadRanksCommand,			"Reloads the in-game rank tables." },
		{ "reload_dupe",		&CGameServerDlg::HandleReloadDupeCommand,			"Reloads a tabela ITEMS_DUPE"},
		{ "reload_ilegal",		&CGameServerDlg::HandleReloadIlegalItemsCommand,	"Reloads a tabela ILEGAL_ITEMS"},
		{ "count",				&CGameServerDlg::HandleCountCommand,				"Get online user count." },
		{ "permitconnect",		&CGameServerDlg::HandlePermitConnectCommand,        "Player unban" },
		{ "give_item",			&CGameServerDlg::HandleGiveItemCommand,				"Gives a player an item. Arguments: character name | item ID | [optional stack size]" },
		{ "exp_add",			&CGameServerDlg::HandleExpAddCommand,				"Sets the server-wide XP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "np_add",				&CGameServerDlg::HandleNPAddCommand,				"Sets the server-wide NP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "money_add",			&CGameServerDlg::HandleMoneyAddCommand,				"Sets the server-wide coin event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "tp_all",				&CGameServerDlg::HandleTeleportAllCommand,			"Players send to home zone." },
		{ "warresult",			&CGameServerDlg::HandleWarResultCommand,			"Set result for War" },
		{ "summonmonster",		&CGameServerDlg::HandleMonSummonCommand,			"Sets the server-wide coin event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "help",				&CGameServerDlg::HandleHelpCommand,					"Displays information about all of the other commands" },
		{ "open_bdw",			&CGameServerDlg::HandleBorderDefenseWarOpenCommand, "Open border defense war" },
		{ "close_bdw",			&CGameServerDlg::HandleBorderDefenseWarCloseCommand,"Close border defense war" },
		{ "open_chaos",			&CGameServerDlg::HandleChaosOpenCommand,			"Open chaos"},
		{ "close_chaos",		&CGameServerDlg::HandleChaosCloseCommand,			"Close chaos"},
		{ "open_juraid",		&CGameServerDlg::HandleJuraidOpenCommand,			"Open juraid"},
		{ "close_juraid",		&CGameServerDlg::HandleJuraidCloseCommand,			"Close juraid"},
		{ "gm",					&CGameServerDlg::HandleGmCommand,					"Grant a player GM authority"},
		{ "remove_gm",			&CGameServerDlg::HandleRemoveGmCommand,				"Remove a player's GM authority"},
		{ "open_event",			&CGameServerDlg::HandleOpenEventCommand,			"Open a world event"},
		{ "close_event",		&CGameServerDlg::HandleCloseEventCommand,			"Close a world event"},
	};

	init_command_table(CGameServerDlg, commandTable, s_commandTable);
}

void CGameServerDlg::CleanupServerCommands() { free_command_table(s_commandTable); }

void CUser::InitChatCommands() {
	static Command<CUser> commandTable[] =
	{
		// Command				Handler											Help message
		{ "test",				&CUser::HandleTestCommand,						"Test command" },
		{ "give_item",			&CUser::HandleGiveItemCommand,					"Gives a player an item. Arguments: character name | item ID | [optional stack size]" },
		{ "zonechange",			&CUser::HandleZoneChangeCommand,				"Teleports you to the specified zone. Arguments: zone ID" },
		{ "monsummon",			&CUser::HandleMonsterSummonCommand,				"Spawns the specified monster (does not respawn). Arguments: monster's database ID" },
		{ "bot",				&CUser::HandleBotSummonCommand,				"Spawns the specified monster (does not respawn). Arguments: monster's database ID" },
		{ "npcsummon",			&CUser::HandleNPCSummonCommand,					"Spawns the specified NPC (does not respawn). Arguments: NPC's database ID" },
		{ "monkill",			&CUser::HandleMonKillCommand,					"Kill a NPC or Monster, Arguments: select an Npc and monster than use this command" },
		{ "open1",				&CUser::HandleWar1OpenCommand,					"Opens war zone 1" },
		{ "open2",				&CUser::HandleWar2OpenCommand,					"Opens war zone 2" },
		{ "open3",				&CUser::HandleWar3OpenCommand,					"Opens war zone 3" },
		{ "open4",				&CUser::HandleWar4OpenCommand,					"Opens war zone 4" },
		{ "open5",				&CUser::HandleWar5OpenCommand,					"Opens war zone 5" },
		{ "open6",				&CUser::HandleWar6OpenCommand,					"Opens war zone 6" },
		{ "openardream",		&CUser::HandleOpenArdreamCommand,				"Opens war zone ardream" },
		{ "opencz",				&CUser::HandleOpenCZCommand,					"Opens war zone cz" },
		{ "closeczevent",		&CUser::HandleCloseCZEventCommand,				"Close war zone event" },
		{ "closeardreamevent",	&CUser::HandleCloseArdreamEventCommand,			"Close war zone event" },
		{ "captain",			&CUser::HandleCaptainCommand,					"Sets the captains/commanders for the war" },
		{ "snowopen",			&CUser::HandleSnowWarOpenCommand,				"Opens the snow war zone" },
		{ "snowclose",			&CUser::HandleSnowWarCloseCommand,				"Snow Closes the active Snowwar zone" },
		{ "siegewarfare",		&CUser::HandleSiegeWarOpenCommand,				"Opens the Castle Siege War zone" },
		{ "close",				&CUser::HandleWarCloseCommand,					"Closes the active war zone" },
		{ "loyalty_change",		&CUser::HandleLoyaltyChangeCommand,				"Change a player an loyalty" },
		{ "exp_change",			&CUser::HandleExpChangeCommand,					"Change a player an exp" },
		{ "gold_change",		&CUser::HandleGoldChangeCommand,				"Change a player an gold" },
		{ "exp_add",			&CUser::HandleExpAddCommand,					"Sets the server-wide XP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "np_add",				&CUser::HandleNPAddCommand,						"Sets the server-wide NP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "money_add",			&CUser::HandleMoneyAddCommand,					"Sets the server-wide coin event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "permitconnect",		&CUser::HandlePermitConnectCommand,				"Player unban" },
		{ "tp_all",				&CUser::HandleTeleportAllCommand,				"Players send to home zone." },
		{ "summonknights",		&CUser::HandleKnightsSummonCommand,				"Teleport the clan users. Arguments: clan name" },
		{ "warresult",			&CUser::HandleWarResultCommand,					"Set result for War"},
		{ "resetranking",		&CUser::HandleResetPlayerRankingCommand,		"Reset player ranking. Arguments : Zone ID"},
		{ "nptokc",				&CUser::HandleNPtoKCCommand,				    "NP ile KC alabileceginiz sistem kullanim: +nptokc npmiktari"},
		{ "goldtokc",			&CUser::HandleGoldtoKCCommand,				    "Gold ile KC alabileceginiz sistem kullanim: +nptokc npmiktari"},
		{ "help",				&CUser::HandleHelpCommand,						"Displays information about all of the other commands" },
		{ "open_bdw",			&CUser::HandleBorderDefenseWarOpenCommand,		"Open border defense war"},
		{ "close_bdw",			&CUser::HandleBorderDefenseWarCloseCommand,		"Close border defense war"},
		{ "open_chaos",			&CUser::HandleChaosOpenCommand,					"Open chaos"},
		{ "close_chaos",		&CUser::HandleChaosCloseCommand,				"Close chaos"},
		{ "open_juraid",		&CUser::HandleJuraidOpenCommand,				"Open juraid"},
		{ "close_juraid",		&CUser::HandleJuraidCloseCommand,				"Close juraid"},
	};

	init_command_table(CUser, commandTable, s_commandTable);
}

void CUser::CleanupChatCommands() { free_command_table(s_commandTable); }

void CUser::Chat(Packet & pkt) {
	Packet result;
	uint16 sessID;
	uint8 type = pkt.read<uint8>(), bOutType = type, seekingPartyOptions, bNation;
	string chatstr, finalstr, strSender, *strMessage, chattype;
	CUser *pUser;
	CKnights * pKnights;
	DateTime time;
	DWORD checknow = GetTickCount();


	if (!isGM()) {
		if (m_mutetime > checknow) return;//süren dolmamış

		if (checknow - m_lastflood <= 1000) //birsaniyeden az // kontrol ediyim tekrar hata çıkmasın
		{
			floodcounter++;
			if (floodcounter >= 5) {
				floodcounter = 0;
				std::string buffer = string_format("[ Chat Flood Detector ] Chat has been muted for 20 seconds.");
				//adama mute yazısı gönderiyoz
				m_mutetime = (checknow + 20000);
				ChatPacket::Construct(&result, PUBLIC_CHAT, &buffer);
				Send(&result);
				return;
			}
		} else floodcounter = 0;
		m_lastflood = checknow;// tamam abi
	}
	//reyiz biz 1 saniyenin altında 5 
	bool isAnnouncement = false;

	if (isMuted() || (GetZoneID() == ZONE_PRISON && !isGM()))
		return;

	pkt >> chatstr;
	if (chatstr.empty() || chatstr.size() > 128)
		return;

	// Process GM & User chat commands
	if (ProcessChatCommand(chatstr)) {
		if (isGM())
			chattype = "GAME MASTER";
		else
			chattype = "CHAT COMMAND";

		g_pMain->WriteGMLogFile(string_format("[ %s - %d:%d:%d ] %s : %s ( Zone=%d, X=%d, Z=%d )\n", chattype.c_str(), time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), chatstr.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		return;
	}

	if (type == SEEKING_PARTY_CHAT)
		pkt >> seekingPartyOptions;

	// Handle GM notice & announcement commands
	if (type == PUBLIC_CHAT || type == ANNOUNCEMENT_CHAT) {
		// Trying to use a GM command without authorisation? Bad player!
		if (!isGM())
			return;

		if (type == ANNOUNCEMENT_CHAT)
			type = WAR_SYSTEM_CHAT;

		bOutType = type;

		// This is horrible, but we'll live with it for now.
		// Pull the notice string (#### NOTICE : %s ####) from the database.
		// Format the chat string around it, so our chat data is within the notice
		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
		isAnnouncement = true;
	}


	if (isAnnouncement) {
		// GM notice/announcements show no name, so don't bother setting it.
		strMessage = &finalstr; // use the formatted message from the user
		strSender = GetName();
		bNation = KARUS; // arbitrary nation
		sessID = -1;
	} else {
		strMessage = &chatstr; // use the raw message from the user
		strSender = GetName(); // everything else uses a name, so set it
		bNation = GetNation();
		sessID = GetSocketID();
	}

	// GMs should use GM chat to help them stand out amongst players.
	if (type == GENERAL_CHAT && isGM())
		bOutType = GM_CHAT;

	ChatPacket::Construct(&result, bOutType, strMessage, &strSender, bNation, sessID);

	switch (type) {
	case GENERAL_CHAT:
		g_pMain->Send_NearRegion(&result, GetMap(), GetRegionX(), GetRegionZ(), GetX(), GetZ(), nullptr, GetEventRoom());
		chattype = "GENERAL_CHAT";
		break;

	case PRIVATE_CHAT:
	{
		pUser = g_pMain->GetUserPtr(m_sPrivateChatUser);
		if (pUser == nullptr || !pUser->isInGame())
			return;

		chattype = "PRIVATE_CHAT";
	}
	case COMMAND_PM_CHAT:
	{
		if (type == COMMAND_PM_CHAT && GetFame() != COMMAND_CAPTAIN)
			return;

		pUser = g_pMain->GetUserPtr(m_sPrivateChatUser);
		if (pUser != nullptr)
			pUser->Send(&result);
	}
	break;
	case PARTY_CHAT:
		if (isInParty()) {
			g_pMain->Send_PartyMember(GetPartyID(), &result);
			chattype = "PARTY_CHAT";
		}
		break;
	case SHOUT_CHAT:
		if (m_sMp < (m_iMaxMp / 5))
			break;

		// Characters under level 35 require 3,000 coins to shout.
		if (!isGM()
			&& GetLevel() < 35
			&& !GoldLose(SHOUT_COIN_REQUIREMENT))
			break;

		MSpChange(-(m_iMaxMp / 5));
		SendToRegion(&result, nullptr, GetEventRoom());
		chattype = "SHOUT_CHAT";
		break;
	case KNIGHTS_CHAT:
		if (isInClan()) {
			pKnights = g_pMain->GetClanPtr(GetClanID());
			g_pMain->Send_KnightsMember(GetClanID(), &result);
			chattype = "KNIGHTS_CHAT";
		}
		break;
	case CLAN_NOTICE:
		if (isInClan()
			&& isClanLeader()) {
			pKnights = g_pMain->GetClanPtr(GetClanID());
			if (pKnights == nullptr)
				return;

			pKnights->UpdateClanNotice(chatstr);
		}
		break;
	case PUBLIC_CHAT:
	case ANNOUNCEMENT_CHAT:
		if (isGM())
			g_pMain->Send_All(&result);
		break;
	case COMMAND_CHAT:
		if (GetFame() == COMMAND_CAPTAIN) {
			g_pMain->Send_CommandChat(&result, m_bNation, this);
			chattype = "COMMAND_CHAT";
		}
		break;
	case MERCHANT_CHAT:
		if (isMerchanting() && m_bMerchantStatex)
			SendToRegion(&result, nullptr, GetEventRoom());
		break;
	case ALLIANCE_CHAT:
		if (isInClan()) {
			pKnights = g_pMain->GetClanPtr(GetClanID());
			if (pKnights != nullptr && pKnights->isInAlliance())
				g_pMain->Send_KnightsAlliance(pKnights->GetAllianceID(), &result);
			chattype = "ALLIANCE_CHAT";
		}
		break;
	case WAR_SYSTEM_CHAT:
		if (isGM())
			g_pMain->Send_All(&result);
		break;
	case SEEKING_PARTY_CHAT:
		if (m_bNeedParty == 2) {
			Send(&result);
			g_pMain->Send_Zone_Matched_Class(&result, GetZoneID(), this, GetNation(), seekingPartyOptions, GetEventRoom());
		}
		break;
	case NOAH_KNIGHTS_CHAT:
		if (GetLevel() > 50)
			break;
		g_pMain->Send_Noah_Knights(&result);
		chattype = "NOAH_KNIGHTS_CHAT";
		break;
	case CHATROM_CHAT:
		ChatRoomChat(strMessage, strSender);
		chattype = "CHATROM_CHAT";
		break;
	default:
		printf("Unknow Chat : %d", type);
		break;
	}

	if (!chattype.empty()) {
		if (pUser && type == 2)
			g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s > %s : %s ( Zone=%d, X=%d, Z=%d )\n", chattype.c_str(), time.GetHour(), time.GetMinute(), time.GetSecond(), strSender.c_str(), pUser->GetName().c_str(), chatstr.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		else if (pKnights && (type == 6 || type == 15))
			g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s > %s : %s ( Zone=%d, X=%d, Z=%d )\n", chattype.c_str(), time.GetHour(), time.GetMinute(), time.GetSecond(), strSender.c_str(), pKnights->GetName().c_str(), chatstr.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
		else
			g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s : %s ( Zone=%d, X=%d, Z=%d )\n", chattype.c_str(), time.GetHour(), time.GetMinute(), time.GetSecond(), strSender.c_str(), chatstr.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	}
}

void CUser::ChatTargetSelect(Packet & pkt) {
	uint8 type = pkt.read<uint8>();

	// TO-DO: Replace this with an enum
	// Attempt to find target player in-game
	if (type == 1) {
		Packet result(WIZ_CHAT_TARGET, type);
		std::string strUserID;
		pkt >> strUserID;
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			return;


		CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pUser == this)
			result << int16(0);
		else if (pUser == nullptr) {
			CBot * pBot = g_pMain->GetBotPtr(strUserID, TYPE_CHARACTER);

			if (pBot == nullptr)
				result << int16(0);
			else if (pBot->isInGame())
				result << int16(1) << pBot->GetName();
			else
				result << int16(0);


		} else if (pUser->isBlockingPrivateChat())
			result << int16(-1) << pUser->GetName();
		else {
			m_sPrivateChatUser = pUser->GetSocketID();
			result << int16(1) << pUser->GetName();
		}
		Send(&result);
	} else if (type == 3) {
		//Questions
		uint8 sSubType;
		uint8 sMessageLen;
		std::string sMessage;
		pkt >> sSubType >> sMessageLen >> sMessage;
	}
	// Allow/block PMs
	else {
		m_bBlockPrivateChat = pkt.read<bool>();
	}
}

/**
* @brief	Sends a notice to all users in the current zone
* 			upon death.
*
* @param	pKiller	The killer.
*/
void CUser::SendDeathNotice(Unit * pKiller, DeathNoticeType noticeType) {
	if (pKiller == nullptr)
		return;

	Packet result(WIZ_CHAT, uint8(DEATH_NOTICE));

	result.SByte();
	result << GetNation()
		<< uint8(noticeType)
		<< pKiller->GetID() // session ID?
		<< pKiller->GetName()
		<< GetID() // session ID?
		<< GetName()
		<< uint16(GetX()) << uint16(GetZ());

	if (GetZoneID() == 21 || GetZoneID() == 22)
		g_pMain->Send_NearRegion(&result, GetMap(), GetRegionX(), GetRegionZ(), GetX(), GetZ(), nullptr, GetEventRoom());
	else
		SendToZone(&result, this, pKiller->GetEventRoom(), ((isInArena() || isInPartyArena()) ? RANGE_20M : 0.0f));
}

bool CUser::ProcessChatCommand(std::string & message) {
	// Commands require at least 2 characters
	if (message.size() <= 1
		// If the prefix isn't correct
		|| message[0] != CHAT_COMMAND_PREFIX
		// or if we're saying, say, ++++ (faster than looking for the command in the map)
		|| message[1] == CHAT_COMMAND_PREFIX)
		// we're not a command.
		return false;

	// Split up the command by spaces
	CommandArgs vargs = StrSplit(message, " ");
	std::string command = vargs.front(); // grab the first word (the command)
	vargs.pop_front(); // remove the command from the argument list

	// Make the command lowercase, for 'case-insensitive' checking.
	STRTOLOWER(command);

	// Command doesn't exist
	ChatCommandTable::iterator itr = s_commandTable.find(command.c_str() + 1); // skip the prefix character
	if (itr == s_commandTable.end())
		return false;

	// Run the command
	return (this->*(itr->second->Handler))(vargs, message.c_str() + command.size() + 1, itr->second->Help);
}

long getFileSize(FILE *file) {
	long lCurPos, lEndPos;
	lCurPos = ftell(file);
	fseek(file, 0, 2);
	lEndPos = ftell(file);
	fseek(file, lCurPos, 0);
	return lEndPos;
}

COMMAND_HANDLER(CUser::HandleTestCommand) {
	if (!isGM())
		return false;

	// OpCode | Send Type
	if (vargs.size() < 1) {
		g_pMain->SendHelpDescription(this, "Using Sample : +send OpCode | SendType");
		return true;
	}

	uint8 WizCode = 0;
	uint8 Type = 1;

	WizCode = atoi(vargs.front().c_str());
	vargs.pop_front();

	if (!vargs.empty()) {
		Type = atoi(vargs.front().c_str());
		vargs.pop_front();
	}

	auto filePath = "Packet.bin";
	BYTE * fileBuf;
	FILE * file = nullptr;

	if ((file = fopen(filePath, "rb")) == nullptr)
		return true;

	auto fileSize = getFileSize(file);
	fileBuf = new BYTE[fileSize];
	fread(fileBuf, fileSize, 1, file);

	Packet result(WizCode);
	for (long i = 0; i < fileSize; i++)
		result << fileBuf[i];

	if (Type == 1)
		Send(&result);
	else if (Type == 2)
		g_pMain->Send_All(&result);

	delete[]fileBuf;
	fclose(file);

	return true;
}

COMMAND_HANDLER(HandleNPtoKCCommand);
COMMAND_HANDLER(HandleGoldtoKCCommand);
COMMAND_HANDLER(CUser::HandleWarResultCommand) { return !isGM() ? false : g_pMain->HandleWarResultCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWarResultCommand) {
	// Nation number
	if (vargs.size() < 1) {
		// send description
		printf("Using Sample : +warresult 1/2 (KARUS/HUMAN)\n");
		return true;
	}

	if (!isWarOpen()) {
		// send description
		printf("Warning : Battle is not open.\n");
		return true;
	}

	uint8 winner_nation;
	winner_nation = atoi(vargs.front().c_str());

	if (winner_nation > 0 && winner_nation < 3)
		BattleZoneResult(winner_nation);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleGiveItemCommand) {
	// Char name | item ID | [stack size]
	if (vargs.size() < 2) {
		// send description
		printf("Using Sample : /give_item CharacterName ItemID StackSize.\n");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr) {
		printf("Error : User is not online\n");
		return true;
	}

	uint32 nItemID = atoi(vargs.front().c_str());
	vargs.pop_front();
	_ITEM_TABLE *pItem = g_pMain->GetItemPtr(nItemID);
	if (pItem == nullptr) {
		// send error message saying the item does not exist
		return true;
	}

	uint16 sCount = 1;
	if (!vargs.empty()) {
		sCount = atoi(vargs.front().c_str());
		vargs.pop_front();
	}

	uint32 Time = 0;
	if (!vargs.empty())
		Time = atoi(vargs.front().c_str());


	if (!pUser->GiveItem(nItemID, sCount, true, Time)) {
		// send error message saying the item couldn't be added
	}

	return true;
}

COMMAND_HANDLER(CUser::HandleGiveItemCommand) {
	if (!isGM())
		return false;

	// Char name | item ID | [stack size]
	if (vargs.size() < 2) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +mind_start CharacterName ItemID StackSize");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr) {
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nItemID = atoi(vargs.front().c_str());
	vargs.pop_front();
	_ITEM_TABLE *pItem = g_pMain->GetItemPtr(nItemID);
	if (pItem == nullptr) {
		// send error message saying the item does not exist
		return true;
	}

	uint16 sCount = 1;
	if (!vargs.empty()) {
		sCount = atoi(vargs.front().c_str());
		vargs.pop_front();
	}

	uint32 Time = 0;
	if (!vargs.empty())
		Time = atoi(vargs.front().c_str());


	if (!pUser->GiveItem(nItemID, sCount, true, Time))
		g_pMain->SendHelpDescription(this, "Error : Item couldn't be added");

	return true;
}

COMMAND_HANDLER(CUser::HandleZoneChangeCommand) {
	if (!isGM())
		return false;

	if (vargs.empty()) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +zonechange ZoneNumber EventRoom");
		return true;
	}

	// Behave as in official (we'll fix this later)
	int nZoneID = atoi(vargs.front().c_str());
	int nEventRoom;
	if (vargs.size() == 2) {
		vargs.pop_front();
		nEventRoom = atoi(vargs.front().c_str());
	}

	_START_POSITION * pStartPosition = g_pMain->GetStartPosition(nZoneID);
	if (pStartPosition == nullptr)
		return false;

	if (nEventRoom > MAX_MONSTER_STONE_EVENT_ROOM)
		nEventRoom = 0;

	ZoneChange(nZoneID,
		(float) (GetNation() == KARUS ? pStartPosition->sKarusX : pStartPosition->sElmoradX + myrand(0, pStartPosition->bRangeX)),
		(float) (GetNation() == KARUS ? pStartPosition->sKarusZ : pStartPosition->sElmoradZ + myrand(0, pStartPosition->bRangeZ)), nEventRoom);

	g_pMain->SendHelpDescription(this, string_format("Your new event room is: %d", nEventRoom));
	return true;
}

COMMAND_HANDLER(CUser::HandleMonsterSummonCommand) {
	if (!isGM())
		return false;

	if (vargs.empty()) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +monsummon MonsterSID Count");
		return true;
	}

	int sSid = 0;
	uint16 sCount = 1;

	if (vargs.size() == 1)
		sSid = atoi(vargs.front().c_str());

	if (vargs.size() == 2) {
		sSid = atoi(vargs.front().c_str());
		vargs.pop_front();
		sCount = atoi(vargs.front().c_str());
	}


	g_pMain->SpawnEventNpc(sSid, true, GetZoneID(), GetX(), GetY(), GetZ(), sCount, 2);

	return true;
}

COMMAND_HANDLER(CUser::HandleBotSummonCommand) {
	if (!isGM())
		return false;

	if (vargs.size() < 3) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +bot Time(AS MINUTE) ResType(1 Mining 2 Fishing 3 Standing 4 Sitting) minlevel");
		return true;
	}

	int Minute = 0, Restie = 0, minlevel = 0;

	Minute = atoi(vargs.front().c_str());

	vargs.pop_front();

	Restie = atoi(vargs.front().c_str());

	vargs.pop_front();

	minlevel = atoi(vargs.front().c_str());


	if (!g_pMain->SpawnBot(Minute, GetZoneID(), GetX(), GetY(), GetZ(), Restie, minlevel)) {
		// send description
		g_pMain->SendHelpDescription(this, "There isn't any available bot.");
		return true;
	}
	g_pMain->SendHelpDescription(this, "Bot yollandi..");
	return true;
}



COMMAND_HANDLER(CUser::HandleNPCSummonCommand) {
	if (!isGM())
		return false;

	if (vargs.empty()) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +npcsummon NPCSID");
		return true;
	}

	int sSid = atoi(vargs.front().c_str());
	g_pMain->SpawnEventNpc(sSid, false, GetZoneID(), GetX(), GetY(), GetZ());

	return true;
}

COMMAND_HANDLER(CUser::HandleMonKillCommand) {
	if (!isGM())
		return false;

	if (GetTargetID() == 0 && GetTargetID() < NPC_BAND) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : Select a NPC or Monster than use +monkills");
		return false;
	}

	CNpc *pNpc = g_pMain->GetNpcPtr(GetTargetID());

	if (pNpc)
		g_pMain->KillNpc(GetTargetID());

	return true;
}

bool CGameServerDlg::ProcessServerCommand(std::string & message) {
	// Commands require at least 2 characters
	if (message.size() <= 1
		// If the prefix isn't correct
		|| message[0] != SERVER_COMMAND_PREFIX)
		// we're not a command.
		return false;

	// Split up the command by spaces
	CommandArgs vargs = StrSplit(message, " ");
	std::string command = vargs.front(); // grab the first word (the command)
	vargs.pop_front(); // remove the command from the argument list

	// Make the command lowercase, for 'case-insensitive' checking.
	STRTOLOWER(command);

	// Command doesn't exist
	ServerCommandTable::iterator itr = s_commandTable.find(command.c_str() + 1); // skip the prefix character
	if (itr == s_commandTable.end())
		return false;

	// Run the command
	return (this->*(itr->second->Handler))(vargs, message.c_str() + command.size() + 1, itr->second->Help);
}

COMMAND_HANDLER(CGameServerDlg::HandleNoticeCommand) {
	if (vargs.empty())
		return true;

	SendNotice(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleNoticeallCommand) {
	if (vargs.empty())
		return true;

	SendAnnouncement(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleKillUserCommand) {
	if (vargs.empty()) {
		// send description
		printf("Using Sample : +kill CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	CUser *pUser = GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr) {
		printf("Error : User is not online\n");
		return true;
	}

	// Disconnect the player
	pUser->Disconnect();

	// send a message saying the player was disconnected
	return true;
}


COMMAND_HANDLER(CUser::HandleWar1OpenCommand) { return !isGM() ? false : g_pMain->HandleWar1OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar1OpenCommand) {
	BattleZoneOpen(BATTLEZONE_OPEN, 1);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar2OpenCommand) { return !isGM() ? false : g_pMain->HandleWar2OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar2OpenCommand) {
	BattleZoneOpen(BATTLEZONE_OPEN, 2);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar3OpenCommand) { return !isGM() ? false : g_pMain->HandleWar3OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar3OpenCommand) {
	g_pMain->m_byBattleZoneType = ZONE_ARDREAM;
	BattleZoneOpen(BATTLEZONE_OPEN, 3);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar4OpenCommand) { return !isGM() ? false : g_pMain->HandleWar4OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar4OpenCommand) {
	BattleZoneOpen(BATTLEZONE_OPEN, 4);
	return true;
}
COMMAND_HANDLER(CUser::HandleSnowWarCloseCommand) { return !isGM() ? false : g_pMain->HandleSnowWarCloseCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleSnowWarCloseCommand) {
	SnowBattleZoneClose();
	return true;
}
COMMAND_HANDLER(CUser::HandleWar5OpenCommand) { return !isGM() ? false : g_pMain->HandleWar5OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar5OpenCommand) {
	BattleZoneOpen(BATTLEZONE_OPEN, 5);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar6OpenCommand) { return !isGM() ? false : g_pMain->HandleWar6OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar6OpenCommand) {
	BattleZoneOpen(BATTLEZONE_OPEN, 6);
	return true;
}

COMMAND_HANDLER(CUser::HandleOpenArdreamCommand) { return !isGM() ? false : g_pMain->HandleOpenArdreamCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleOpenArdreamCommand) {
	ArdreamEventOpen();
	g_pMain->m_byEventOpenedTime = int32(UNIXTIME);
	g_pMain->m_byEventRemainingTime = g_pMain->m_byEventTime;
	g_pMain->m_sEventTimeDelay = 0;
	return true;
}

COMMAND_HANDLER(CUser::HandleOpenCZCommand) { return !isGM() ? false : g_pMain->HandleOpenCZCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleOpenCZCommand) {
	CZEventOpen();
	g_pMain->m_byEventOpenedTime = int32(UNIXTIME);
	g_pMain->m_byEventRemainingTime = g_pMain->m_byEventTime;
	g_pMain->m_sEventTimeDelay = 0;
	return true;
}

COMMAND_HANDLER(CUser::HandleCloseArdreamEventCommand) { return !isGM() ? false : g_pMain->HandleCloseArdreamEventCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleCloseArdreamEventCommand) {
	ArdreamEventZoneClose();
	return true;
}
COMMAND_HANDLER(CUser::HandleCloseCZEventCommand) { return !isGM() ? false : g_pMain->HandleCloseCZEventCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleCloseCZEventCommand) {
	CZEventZoneClose();
	return true;
}


COMMAND_HANDLER(CUser::HandleSnowWarOpenCommand) { return !isGM() ? false : g_pMain->HandleSnowWarOpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleSnowWarOpenCommand) {
	BattleZoneOpen(SNOW_BATTLEZONE_OPEN);
	return true;
}

COMMAND_HANDLER(CUser::HandleSiegeWarOpenCommand) {
	if (vargs.size() < 1) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +siegewarfare Time(Minute)");
		return true;
	}

	return !isGM() ? false : g_pMain->HandleSiegeWarOpenCommand(vargs, args, description);
}
COMMAND_HANDLER(CGameServerDlg::HandleSiegeWarOpenCommand) {
	uint8 Time = atoi(vargs.front().c_str());
	CastleSiegeWarZoneOpen(CLAN_BATTLE, Time);
	return true;
}

COMMAND_HANDLER(CUser::HandleWarCloseCommand) { return !isGM() ? false : g_pMain->HandleWarCloseCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWarCloseCommand) {
	BattleZoneClose();
	return true;
}

COMMAND_HANDLER(CUser::HandleHelpCommand) {
	if (!isGM()) {
		return false;
	}
	for (const auto &command : s_commandTable) {
		std::string name = command.second->Name;
		std::string help = command.second->Help;
		g_pMain->SendHelpDescription(this, name + ": " + help);
	}
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleHelpCommand) {
	for (const auto &command : s_commandTable) {
		printf("%s: %s\n", command.second->Name, command.second->Help);
	}
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleGmCommand) {
	// Char name
	if (vargs.size() < 1) {
		// send description
		printf("Using Sample : /gm CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();
	g_DBAgent.UpdateUserAuthority(strUserID, AUTHORITY_GAME_MASTER);
	printf("%s has been promoted to Game Master..!\n", strUserID.c_str());
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleRemoveGmCommand) {
	// Char name
	if (vargs.size() < 1) {
		// send description
		printf("Using Sample : /remove_gm CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();
	g_DBAgent.UpdateUserAuthority(strUserID, AUTHORITY_PLAYER);
	printf("%s has been demoted to Player..!\n", strUserID.c_str());
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleOpenEventCommand) {
	// Event name 
	if (vargs.size() < 1) {
		// send description
		printf("Using Sample : /open_event EventName\n");
		return true;
	}

	std::string eventName = vargs.front();
	vargs.pop_front();

	if (boost::iequals(eventName, "bifrost")) {
		return g_pMain->m_pWorldEventManager->AddBifrostEvent();
	}
	return false;
}

COMMAND_HANDLER(CGameServerDlg::HandleCloseEventCommand) {
	// Event name 
	if (vargs.size() < 1) {
		// send description
		printf("Using Sample : /close_event EventName\n");
		return true;
	}

	std::string eventName = vargs.front();
	vargs.pop_front();

	if (boost::iequals(eventName, "bifrost")) {
		return g_pMain->m_pWorldEventManager->RemoveBifrostEvent();
	}
	return false;
}

COMMAND_HANDLER(CUser::HandleBorderDefenseWarOpenCommand) {
	if (!isGM()) {
		return false;
	// Time until start (seconds)
	} else if (vargs.size() < 1) {
		g_pMain->SendHelpDescription(this, "/open_bdw <seconds> Example: /open_bdw 600");
		return true;
	}
	return g_pMain->HandleBorderDefenseWarOpenCommand(vargs, args, description);
}

COMMAND_HANDLER(CGameServerDlg::HandleBorderDefenseWarOpenCommand) {
	// Time until start (seconds)
	if (vargs.size() < 1) {
		printf("/open_bdw <seconds> Example: /open_bdw 600\n");
		return true;
	}

	int userSeconds;
	try {
		userSeconds = std::stoi(vargs.front());
	} catch (std::invalid_argument&) {
		printf("/open_bdw <seconds> Example: /open_bdw 600\n");
		return false;
	} catch (std::out_of_range&) {
		printf("/open_bdw <seconds> Example: /open_bdw 600\n");
		return false;
	}

	g_pMain->m_nextEvent = TEMPLE_EVENT_BORDER_DEFENCE_WAR;
	g_pMain->m_eventStartTime = system_clock::now() + seconds(userSeconds);
	return true;
}

COMMAND_HANDLER(CUser::HandleBorderDefenseWarCloseCommand) { return !isGM() ? false : g_pMain->HandleBorderDefenseWarCloseCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleBorderDefenseWarCloseCommand) {
	g_pMain->TerminationFinish();
	g_pMain->pTempleEvent.isAttackable = false;
	g_pMain->TempleEventFinish(0, 0);
	pTempleEvent.ActiveEvent = -1;
	m_eventStartTime = system_clock::now() - hours(1);
	return true;
}

COMMAND_HANDLER(CUser::HandleChaosOpenCommand) {
	if (!isGM()) {
		return false;
		// Time until start (seconds)
	} else if (vargs.size() < 1) {
		g_pMain->SendHelpDescription(this, "/open_chaos <seconds> Example: /open_chaos 600");
		return true;
	}
	return g_pMain->HandleChaosOpenCommand(vargs, args, description);
}

COMMAND_HANDLER(CGameServerDlg::HandleChaosOpenCommand) {
	// Time until start (seconds)
	if (vargs.size() < 1) {
		printf("/open_chaos <seconds> Example: /open_chaos 600\n");
		return true;
	}

	int userSeconds;
	try {
		userSeconds = std::stoi(vargs.front());
	} catch (std::invalid_argument&) {
		printf("/open_chaos <seconds> Example: /open_chaos 600\n");
		return false;
	} catch (std::out_of_range&) {
		printf("/open_chaos <seconds> Example: /open_chaos 600\n");
		return false;
	}

	g_pMain->m_nextEvent = TEMPLE_EVENT_CHAOS;
	g_pMain->m_eventStartTime = system_clock::now() + seconds(userSeconds);
	return true;
}

COMMAND_HANDLER(CUser::HandleChaosCloseCommand) { return !isGM() ? false : g_pMain->HandleChaosCloseCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleChaosCloseCommand) {
	g_pMain->TerminationFinish();
	g_pMain->pTempleEvent.isAttackable = false;
	g_pMain->TempleEventFinish(0, 0);
	pTempleEvent.ActiveEvent = -1;
	m_eventStartTime = system_clock::now() - hours(1);
	return true;
}

COMMAND_HANDLER(CUser::HandleJuraidOpenCommand) {
	if (!isGM()) {
		return false;
		// Time until start (seconds)
	} else if (vargs.size() < 1) {
		g_pMain->SendHelpDescription(this, "/open_juraid <seconds> Example: /open_juraid 600");
		return true;
	}
	return g_pMain->HandleJuraidOpenCommand(vargs, args, description);
}

COMMAND_HANDLER(CGameServerDlg::HandleJuraidOpenCommand) {
	// Time until start (seconds)
	if (vargs.size() < 1) {
		printf("/open_juraid <seconds> Example: /open_juraid 600\n");
		return true;
	}

	int userSeconds;
	try {
		userSeconds = std::stoi(vargs.front());
	} catch (std::invalid_argument&) {
		printf("/open_juraid <seconds> Example: /open_juraid 600\n");
		return false;
	} catch (std::out_of_range&) {
		printf("/open_juraid <seconds> Example: /open_juraid 600\n");
		return false;
	}

	g_pMain->m_nextEvent = TEMPLE_EVENT_JURAD_MOUNTAIN;
	g_pMain->m_eventStartTime = system_clock::now() + seconds(userSeconds);
	return true;
}

COMMAND_HANDLER(CUser::HandleJuraidCloseCommand) { return !isGM() ? false : g_pMain->HandleJuraidCloseCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleJuraidCloseCommand) {
	g_pMain->TerminationFinish();
	g_pMain->pTempleEvent.isAttackable = false;
	g_pMain->TempleEventFinish(0, 0);
	pTempleEvent.ActiveEvent = -1;
	m_eventStartTime = system_clock::now() - hours(1);
	return true;
}

COMMAND_HANDLER(CUser::HandleLoyaltyChangeCommand) {
	if (!isGM())
		return false;

	// Char name | loyalty
	if (vargs.size() < 2) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +np_change CharacterName Loyalty(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr) {
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nLoyalty = atoi(vargs.front().c_str());

	if (nLoyalty != 0)
		pUser->SendLoyaltyChange(nLoyalty, false);

	return true;
}

COMMAND_HANDLER(CUser::HandleExpChangeCommand) {
	if (!isGM())
		return false;

	// Char name | exp
	if (vargs.size() < 2) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +exp_change CharacterName Exp(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr) {
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	int64 nExp = atoi(vargs.front().c_str());

	if (nExp != 0)
		pUser->ExpChange(nExp);

	return true;
}

COMMAND_HANDLER(CUser::HandleGoldChangeCommand) {
	if (!isGM())
		return false;

	// Char name | coins
	if (vargs.size() < 2) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +gold_change CharacterName Gold(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr) {
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nGold = atoi(vargs.front().c_str());

	if (nGold != 0) {
		if (nGold > 0)
			pUser->GoldGain(nGold);
		else
			pUser->GoldLose(nGold);
	}

	return true;
}

// Starts/stops the server XP event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleExpAddCommand) { return !isGM() ? false : g_pMain->HandleExpAddCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleExpAddCommand) {
	// Expects the bonus XP percent, e.g. '+exp_add' for a +15 XP boost.
	if (vargs.empty())
		return true;


	g_pMain->m_byExpEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byExpEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_EXP_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byExpEventAmount);
	return true;
}

// Starts/stops the server NP event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleNPAddCommand) { return !isGM() ? false : g_pMain->HandleNPAddCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleNPAddCommand) {
	// Expects the bonus np percent, e.g. '+np_add' for a +15 np boost.
	if (vargs.empty())
		return true;

	g_pMain->m_byNPEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byNPEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_NP_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byNPEventAmount);
	return true;
}

// Starts/stops the server coin event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleMoneyAddCommand) { return !isGM() ? false : g_pMain->HandleMoneyAddCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleMoneyAddCommand) {
	// Expects the bonus coin percent, e.g. '+money_add' for a +15 dropped coin boost.
	if (vargs.empty())
		return true;

	g_pMain->m_byCoinEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byCoinEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_MONEY_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byCoinEventAmount);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleMonSummonCommand) {
	if (vargs.size() < 1) {
		// send description
		printf("Using Sample : /monsummon MonsterID Zoneid X Y R\n");
		return true;
	}
	int sSid = 0;
	int nZoneID = 0;
	int nX = 0;
	int nZ = 0;
	int nR = 0;


	if (vargs.size() == 1)
		sSid = atoi(vargs.front().c_str());

	if (vargs.size() == 2) {
		sSid = atoi(vargs.front().c_str());
		vargs.pop_front();
		nZoneID = atoi(vargs.front().c_str());
	}

	if (vargs.size() == 3) {
		sSid = atoi(vargs.front().c_str());
		vargs.pop_front();
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nX = atoi(vargs.front().c_str());
	}

	if (vargs.size() == 4) {
		sSid = atoi(vargs.front().c_str());
		vargs.pop_front();
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nX = atoi(vargs.front().c_str());
		vargs.pop_front();
		nZ = atoi(vargs.front().c_str());
	}

	if (vargs.size() == 5) {
		sSid = atoi(vargs.front().c_str());
		vargs.pop_front();
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nX = atoi(vargs.front().c_str());
		vargs.pop_front();
		nZ = atoi(vargs.front().c_str());
		vargs.pop_front();
		nR = atoi(vargs.front().c_str());
	}

	if (nZoneID > 0)
		g_pMain->SpawnEventNpc(sSid, true, nZoneID, (float) nX, 0, (float) nZ, nR);

	return true;
}

COMMAND_HANDLER(CUser::HandlePermitConnectCommand) { return !isGM() ? false : g_pMain->HandlePermitConnectCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandlePermitConnectCommand) {
	// Char name
	if (vargs.size() < 1) {
		// send description
		printf("Using Sample : +permitconnect CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	g_DBAgent.UpdateUserAuthority(strUserID, AUTHORITY_PLAYER);

	std::string sNoticeMessage = string_format("%s has been unbanned..!", strUserID.c_str());

	if (!sNoticeMessage.empty())
		g_pMain->SendNotice(sNoticeMessage.c_str(), Nation::ALL);

	return true;
}
COMMAND_HANDLER(CUser::HandleTeleportAllCommand) {
	if (!isGM())
		return false;

	// Zone number
	if (vargs.size() < 1) {
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +tp_all ZoneNumber | +tp_all ZoneNumber TargetZoneNumber");
		return true;
	}

	int nZoneID = 0;
	int nTargetZoneID = 0;

	if (vargs.size() == 1)
		nZoneID = atoi(vargs.front().c_str());

	if (vargs.size() == 2) {
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nTargetZoneID = atoi(vargs.front().c_str());
	}

	if (nZoneID > 0 || nTargetZoneID > 0)
		g_pMain->KickOutZoneUsers(nZoneID, nTargetZoneID);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleTeleportAllCommand) {
	// Zone number
	if (vargs.size() < 1) {
		// send description
		printf("Using Sample : /tp_all ZoneNumber | /tp_all ZoneNumber TargetZoneNumber.\n");
		return true;
	}

	int nZoneID = 0;
	int nTargetZoneID = 0;

	if (vargs.size() == 1)
		nZoneID = atoi(vargs.front().c_str());

	if (vargs.size() == 2) {
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nTargetZoneID = atoi(vargs.front().c_str());
	}

	if (nZoneID > 0 || nTargetZoneID > 0)
		g_pMain->KickOutZoneUsers(nZoneID, nTargetZoneID);
	return true;
}

COMMAND_HANDLER(CUser::HandleKnightsSummonCommand) {
	if (!isGM())
		return false;

	// Clan name
	if (vargs.empty()) {
		// Send description
		g_pMain->SendHelpDescription(this, "Using Sample : +summonknights ClanName");
		return true;
	}

	CKnights * pKnights;
	foreach_stlmap(itr, g_pMain->m_KnightsArray) {
		if (itr->second->GetName() == vargs.front().c_str()) {
			pKnights = g_pMain->GetClanPtr(itr->first);
			break;
		}
	}

	if (pKnights == nullptr)
		return true;

	foreach_array(i, pKnights->m_arKnightsUser) {
		_KNIGHTS_USER *p = &pKnights->m_arKnightsUser[i];
		if (!p->byUsed || p->pSession == nullptr)
			continue;

		CUser* pUser = p->pSession;
		if (!pUser->isInGame() || pUser->GetName() == GetName())
			continue;

		pUser->ZoneChange(GetZoneID(), m_curx, m_curz);
	}

	return true;
}

COMMAND_HANDLER(CUser::HandleNPtoKCCommand) {
	if (!g_pMain->NpToKcSystem
		|| isMerchanting()
		|| isStoreOpen()
		|| isTrading()
		|| m_bMerchantStatex
		|| isDead())
		return true;

	if (vargs.size() < 1) {
		g_pMain->SendHelpDescription(this, "Example: +nptokc 1000");
		return true;
	} else if (vargs.size() > 1) {
		g_pMain->SendHelpDescription(this, "Example: +nptokc 1000");
		return true;

	}


	uint32 nLoyalty = atoi(vargs.front().c_str());

	if (nLoyalty > 1000000) {
		g_pMain->SendHelpDescription(this, "You can't exchange more than 1 million NP.");
		return true;
	}

	int32 CLoyalty;

	if (nLoyalty < 1) {
		g_pMain->SendHelpDescription(this, "You can't exchange this much NP.");
		return true;
	}

	if (GetLoyalty() < nLoyalty) {
		g_pMain->SendHelpDescription(this, "You haven't got this much NP.");
		return true;
	}
	CLoyalty = nLoyalty;
	if (CLoyalty % 1000 != 0) {
		g_pMain->SendHelpDescription(this, "You can only exchange 1000,2000 or x000 NP.");
		return true;
	}


	int32 ExchangeKC = (CLoyalty / 1000) * (g_pMain->KcToThousandNp);

	if (NpExchangeAsk && NpExchangeValue == CLoyalty) {
		g_pMain->SendHelpDescription(this, string_format("You earned [%d] Knight Cash!", ExchangeKC));
		SendLoyaltyChange(-CLoyalty, false, false, false);
		g_DBAgent.UpdateAccountKnightCash(GetAccountName(), ExchangeKC);
		g_DBAgent.UpdateUser(GetName(), UPDATE_LOGOUT, this);
	} else {
		g_pMain->SendHelpDescription(this, string_format("Do you really want to give [%d] loyalty for earn [%d] Knight Cash?", CLoyalty, ExchangeKC));
		g_pMain->SendHelpDescription(this, string_format("Rechat <+nptokc %d> to earn [%d] Knight Cash.", CLoyalty, ExchangeKC));
		NpExchangeAsk = 1;
		NpExchangeValue = CLoyalty;
		return true;
	}

	return true;
}

COMMAND_HANDLER(CUser::HandleGoldtoKCCommand) {
	if (!g_pMain->GoldToKcSystem
		|| isMerchanting()
		|| isStoreOpen()
		|| isTrading()
		|| m_bMerchantStatex
		|| isDead())
		return true;

	if (vargs.size() < 1) {
		g_pMain->SendHelpDescription(this, "Example: +goldtokc 1000");
		return true;
	} else if (vargs.size() > 1) {
		g_pMain->SendHelpDescription(this, "Example: +goldtokc 1000");
		return true;

	}


	uint32 nGold = atoi(vargs.front().c_str());

	if (nGold > 2100000000) {
		g_pMain->SendHelpDescription(this, "You can't exchange more than 2.1 Gold.");
		return true;
	}

	int32 CGold;

	if (nGold < 1) {
		g_pMain->SendHelpDescription(this, "You can't exchange this much Gold.");
		return true;
	}

	if (GetCoins() < nGold) {
		g_pMain->SendHelpDescription(this, "You haven't got this much Gold.");
		return true;
	}
	CGold = nGold;
	if (CGold % 100000000 != 0) {
		g_pMain->SendHelpDescription(this, "You can only exchange 10000000,20000000 or xGB Gold.");
		return true;
	}


	int32 ExchangeKC = (CGold / 100000000) * (g_pMain->KcToGoldBar);

	if (GoldExchangeAsk && GoldExchangeValue == CGold) {
		g_pMain->SendHelpDescription(this, string_format("You earned [%d] Knight Cash!", ExchangeKC));
		GoldLose(CGold, true);
		g_DBAgent.UpdateAccountKnightCash(GetAccountName(), ExchangeKC);
		g_DBAgent.UpdateUser(GetName(), UPDATE_LOGOUT, this);
	} else {
		g_pMain->SendHelpDescription(this, string_format("Do you really want to give [%d] Gold for earn [%d] Knight Cash?", CGold, ExchangeKC));
		g_pMain->SendHelpDescription(this, string_format("Rechat <+goldtokc %d> to earn [%d] Knight Cash.", CGold, ExchangeKC));
		GoldExchangeAsk = 1;
		GoldExchangeValue = CGold;
		return true;
	}

	return true;
}


COMMAND_HANDLER(CUser::HandleResetPlayerRankingCommand) {
	if (!isGM())
		return false;

	// Zone ID
	if (vargs.empty()) {
		// Send description
		g_pMain->SendHelpDescription(this, "Using Sample : +resetranking ZoneID");
		return true;
	}

	uint8 nZoneID;
	nZoneID = atoi(vargs.front().c_str());

	if (nZoneID > 0)
		g_pMain->ResetPlayerRankings(nZoneID);

	return true;
}



COMMAND_HANDLER(CGameServerDlg::HandleShutdownCommand) {
	printf("Server shutdown, %d users kicked out.\n", KickOutAllUsers());
	m_socketMgr.Shutdown();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleDiscountCommand) {
	m_sDiscount = 1;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleGlobalDiscountCommand) {
	m_sDiscount = 2;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleDiscountOffCommand) {
	m_sDiscount = 0;
	return true;
}

COMMAND_HANDLER(CUser::HandleCaptainCommand) { return !isGM() ? false : g_pMain->HandleCaptainCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleCaptainCommand) {
	m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
	m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
	LoadKnightsRankTable(true, true);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleSantaCommand) {
	m_bSantaOrAngel = FLYING_SANTA;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleSantaOffCommand) {
	m_bSantaOrAngel = FLYING_NONE;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleAngelCommand) {
	m_bSantaOrAngel = FLYING_ANGEL;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandlePermanentChatCommand) {
	if (vargs.empty()) {
		// send error saying we need args (unlike the previous implementation of this command)
		return true;
	}

	SetPermanentMessage("%s", args);
	return true;
}

void CGameServerDlg::SendHelpDescription(CUser *pUser, std::string sHelpMessage) {
	if (pUser == nullptr || sHelpMessage == "")
		return;

	Packet result(WIZ_CHAT, (uint8) PUBLIC_CHAT);
	result << pUser->GetNation() << pUser->GetSocketID() << uint8(0) << sHelpMessage;
	pUser->Send(&result);
}

void CGameServerDlg::SetPermanentMessage(const char * format, ...) {
	char buffer[128];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buffer, 128, format, ap);
	va_end(ap);

	m_bPermanentChatMode = true;
	m_strPermanentChat = buffer;

	Packet result;
	ChatPacket::Construct(&result, PERMANENT_CHAT, &m_strPermanentChat);
	Send_All(&result);
}

COMMAND_HANDLER(CGameServerDlg::HandlePermanentChatOffCommand) {
	Packet result;
	ChatPacket::Construct(&result, END_PERMANENT_CHAT);
	m_bPermanentChatMode = false;
	Send_All(&result);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadNoticeCommand) {
	// Reload the notice data
	LoadNoticeData();
	LoadNoticeUpData();

	// and update all logged in players
	// if we're using the new notice format that's always shown
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser * pUser = TO_USER(itr.second);
		if (pUser->isInGame())
			pUser->SendNotice();
		pUser->TopSendNotice();
	}
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleItemTablesCommand) {
	m_ItemtableArray.DeleteAllData();
	LoadItemTable();
	return true;
}
COMMAND_HANDLER(CGameServerDlg::HandleReloadTablesCommand) {
	m_StartPositionArray.DeleteAllData();
	LoadStartPositionTable();

	m_StartPositionRandomArray.DeleteAllData();
	LoadStartPositionRandomTable();

	m_ItemExchangeArray.DeleteAllData();
	LoadItemExchangeTable();

	m_ItemMiningArray.DeleteAllData();
	LoadItemMiningTable();

	ACHIEVE_TITLE.DeleteAllData();
	LoadAchieveTitleTable();

	ACHIEVE_COM.DeleteAllData();
	LoadAchieveComTable();

	ACHIEVE_MAIN.DeleteAllData();
	LoadAchieveMainTable();

	ACHIEVE_MONSTER.DeleteAllData();
	LoadAchieveMonsterTable();

	ACHIEVE_NORMAL.DeleteAllData();
	LoadAchieveNormalTable();

	ACHIEVE_WAR.DeleteAllData();
	LoadAchieveWarTable();

	m_ItemUpgradeArray.DeleteAllData();
	LoadItemUpgradeTable();

	m_EventTriggerArray.DeleteAllData();
	LoadEventTriggerTable();

	m_ServerResourceArray.DeleteAllData();
	LoadServerResourceTable();

	m_MonsterChallengeArray.DeleteAllData();
	LoadMonsterChallengeTable();

	m_MonsterChallengeSummonListArray.DeleteAllData();
	LoadMonsterChallengeSummonListTable();

	m_EventTimesArray.DeleteAllData();
	LoadEventTimesTable();

	m_MonsterRespawnListArray.DeleteAllData();
	LoadMonsterRespawnListTable();

	m_MonsterRespawnListInformationArray.DeleteAllData();
	LoadMonsterRespawnListInformationTable();

	m_MonsterRespawnListRandomArray.DeleteAllData();
	LoadMonsterRespawnListRandomTable();

	ReloadKnightAndUserRanks();

	g_pMain->m_MerchantListArray.DeleteAllData();

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadMagicsCommand) {
	m_IsMagicTableInUpdateProcess = true;
	m_MagictableArray.DeleteAllData();
	m_Magictype1Array.DeleteAllData();
	m_Magictype2Array.DeleteAllData();
	m_Magictype3Array.DeleteAllData();
	m_Magictype4Array.DeleteAllData();
	m_Magictype5Array.DeleteAllData();
	m_Magictype6Array.DeleteAllData();
	m_Magictype8Array.DeleteAllData();
	m_Magictype9Array.DeleteAllData();
	LoadMagicTable();
	LoadMagicType1();
	LoadMagicType2();
	LoadMagicType3();
	LoadMagicType4();
	LoadMagicType5();
	LoadMagicType6();
	LoadMagicType7();
	LoadMagicType8();
	LoadMagicType9();
	m_IsMagicTableInUpdateProcess = false;

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadBotsCommand) {
	m_arBotArray.DeleteAllData();
	LoadBotTable();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadQuestCommand) {
	m_QuestHelperArray.DeleteAllData();
	LoadQuestHelperTable();
	m_QuestMonsterArray.DeleteAllData();
	LoadQuestMonsterTable();
	GetLuaEngine()->Reset();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadRanksCommand) {
	ReloadKnightAndUserRanks();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleCountCommand) {
	uint16 count = 0, count2 = 0;
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		if (TO_USER(itr.second)->isInGame())
			count++;

		count2++;
	}

	printf("Online User Count : %d,%d\n", count, int16(MAX_USER - count2));
	return true;
}

void CGameServerDlg::SendFormattedResource(uint32 nResourceID, uint8 byNation, bool bIsNotice, ...) {
	_SERVER_RESOURCE *pResource = m_ServerResourceArray.GetData(nResourceID);
	if (pResource == nullptr)
		return;

	string buffer;
	va_list ap;
	va_start(ap, bIsNotice);
	_string_format(pResource->strResource, &buffer, ap);
	va_end(ap);

	if (bIsNotice)
		SendNotice(buffer.c_str(), byNation);
	else
		SendAnnouncement(buffer.c_str(), byNation);
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadDupeCommand) {
	m_IsDupeUpdateProcess = true;
	m_ItemDupersArray.DeleteAllData();
	LoadItemDuper();
	m_IsDupeUpdateProcess = false;

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadIlegalItemsCommand) {
	m_IlegalItemsUpdateProcess = true;
	m_IlegalItemsArray.DeleteAllData();
	LoadIlegalItems();
	m_IlegalItemsUpdateProcess = false;

	return true;
}