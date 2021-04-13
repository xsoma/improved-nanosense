#include "../SDK.hpp"
#pragma once

class bf_read
{
public:
	uintptr_t base_address;
	uintptr_t cur_offset;

	bf_read::bf_read(uintptr_t addr)
	{
		base_address = addr;
		cur_offset = 0;
	}

	void bf_read::SetOffset(uintptr_t offset)
	{
		cur_offset = offset;
	}

	void bf_read::Skip(uintptr_t length)
	{
		cur_offset += length;
	}

	int bf_read::ReadByte()
	{
		auto val = *reinterpret_cast<char*>(base_address + cur_offset);
		++cur_offset;
		return val;
	}

	bool bf_read::ReadBool()
	{
		auto val = *reinterpret_cast<bool*>(base_address + cur_offset);
		++cur_offset;
		return val;
	}

	std::string bf_read::ReadString()
	{
		char buffer[256];
		auto str_length = *reinterpret_cast<char*>(base_address + cur_offset);
		++cur_offset;
		memcpy(buffer, reinterpret_cast<void*>(base_address + cur_offset), str_length > 255 ? 255 : str_length);
		buffer[str_length > 255 ? 255 : str_length] = '\0';
		cur_offset += str_length + 1;
		return std::string(buffer);
	}
};

enum class usermsg_type
{
	CS_UM_VGUIMenu = 1,
	CS_UM_Geiger,
	CS_UM_Train,
	CS_UM_HudText,
	CS_UM_SayText,							// team chat?
	CS_UM_SayText2,							// all chat
	CS_UM_TextMsg,
	CS_UM_HudMsg,
	CS_UM_ResetHud,
	CS_UM_GameTitle,
	CS_UM_Shake = 12,						// map/server plugin shake effect?
	CS_UM_Fade,								// fade HUD in/out
	CS_UM_Rumble,							// controller rumble?
	CS_UM_CloseCaption,
	CS_UM_CloseCaptionDirect,
	CS_UM_SendAudio,
	CS_UM_RawAudio,
	CS_UM_VoiceMask,
	CS_UM_RequestState,
	CS_UM_Damage,
	CS_UM_RadioText,
	CS_UM_HintText,
	CS_UM_KeyHintText,
	CS_UM_ProcessSpottedEntityUpdate,
	CS_UM_ReloadEffect,
	CS_UM_AdjustMoney,
	CS_UM_UpdateTeamMoney,
	CS_UM_StopSpectatorMode,
	CS_UM_KillCam,
	CS_UM_DesiredTimescale,
	CS_UM_CurrentTimescale,
	CS_UM_AchievementEvent,
	CS_UM_MatchEndConditions,
	CS_UM_DisconnectToLobby,
	CS_UM_DisplayInventory = 37,
	CS_UM_WarmupHasEnded,
	CS_UM_ClientInfo,
	CS_UM_CallVoteFailed = 45,
	CS_UM_VoteStart,
	CS_UM_VotePass,
	CS_UM_VoteFailed,
	CS_UM_VoteSetup,
	CS_UM_SendLastKillerDamageToClient = 51,
	CS_UM_ItemPickup = 53,
	CS_UM_ShowMenu,							// show hud menu
	CS_UM_BarTime,							// For the C4 progress bar.
	CS_UM_AmmoDenied,
	CS_UM_MarkAchievement,
	CS_UM_ItemDrop = 59,
	CS_UM_GlowPropTurnOff
};
//https://github.com/ValveSoftware/csgo-demoinfo/blob/master/demoinfogo/cstrike15_usermessages_public.proto#L68