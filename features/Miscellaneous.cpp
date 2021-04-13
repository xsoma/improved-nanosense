#include "Miscellaneous.hpp"

#include "../Structs.hpp"
#include "../Options.hpp"
#include "../helpers/Math.hpp"

#include "AntiAim.hpp"
#include "AngryPew.hpp"
#include "PredictionSystem.hpp"
#include <chrono>
#include <fstream>
#include "../menu/AntiCheatScan.h"

template<class T, class U>
T Clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}
std::vector<std::string> msgs =
{
"Cum and go",
"Drip and dip",
"Nail and bail",
"Nut then strut",
"Boom n' Zoom",
"Skeet and yeet",
"Hit it and quit it",
"Shoot and scoot",
"Smash and dash",
"Hump then dump",
"Rape and escape",
"Breed then proceed",
"Ejaculate and evacuate",
"Impregnate and immigrate",
"Tear her apart then depart",
"Fuck her ass then hit the gas",
"Bust a nut then leave the hut",
"Shoot the Jizz then out you is",
"Make a baby and run like crazy",
"Smash her rear then disappear",
"Hit the spot then ditch the thot",
"Blow your load and hit the road",
"Shoot the glaze then run aways",
"Shoot your goo and say adieu",
"Shoot your shit then fucking split",
"Drop that skeet then hit that yeet",
"Get some brain then ride the train",
"Rock her box then change the locks",
"Make her your play mate and vacate",
"Slide in your meat then hit the street",
"Fertilize her seed then need for speed",
"Cum on her back then get on the track",
"Prove you ain't gay then run the fuck away",
"Take her virginity then change your vicinity",
"Prompt her wetting then change your setting",
"Explore your sexuality then reposition your locality",
"Preform vaginal penetration then proceed to change your location",
"Put yourself inside her a fair amount then change your physical whereabouts"
};

void Miscellaneous::Bhop(CUserCmd *userCMD)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER) return;
	if (userCMD->buttons & IN_JUMP && !(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) {
		userCMD->buttons &= ~IN_JUMP;
	}
}

void Miscellaneous::DetectAC(acinfo& output)
{
	g_EngineClient->ExecuteClientCmd("clear");
	ConVar* con_filter_enable = g_CVar->FindVar("con_filter_enable");
	if (checks::is_bad_ptr(con_filter_enable))
		return;
	bool backup = con_filter_enable->GetBool();
	if (con_filter_enable->GetBool())
		con_filter_enable->SetValue(0);

	std::ofstream file;
	file.open("csgo\cfg\findac.cfg", std::ios::app);
	file << "sm plugins list" << std::endl;
	for (int i = 1; i < 10; i++)
	{
		file << "sm plugins " << std::to_string(i) << std::to_string(i) << std::endl;
	}
	file << "condump" << std::endl;
	file.close();
	g_EngineClient->ExecuteClientCmd("exec findac");

	std::string dump;
	std::ifstream dumpfile("csgo\condump000.txt");
	if (!dumpfile.is_open())
		return;
	dumpfile.seekg(dumpfile.end);
	int lenght = dumpfile.tellg();
	dumpfile.seekg(dumpfile.beg);
	char* dumpcstr = new char[lenght];

	dumpfile.read(dumpcstr, lenght);
	dumpfile.close();
	dump = dumpcstr;

	if (dump.find("SMAC") != std::string::npos)	// gay af stack of if but there's no other great way to do this
		output.smac_core = true;
	if (dump.find("SMAC Aimbot") != std::string::npos)
		output.smac_aimbot = true;
	if (dump.find("SMAC AutoTrigger") != std::string::npos)
		output.smac_autotrigger = true;
	if (dump.find("SMAC Client") != std::string::npos)
		output.smac_client = true;
	if (dump.find("SMAC Command") != std::string::npos)
		output.smac_commands = true;
	if (dump.find("SMAC ConVar") != std::string::npos)
		output.smac_cvars = true;
	if (dump.find("SMAC Eye Angle") != std::string::npos)
		output.smac_eyetest = true;
	if (dump.find("SMAC Anti-Speedhack") != std::string::npos)
		output.smac_speedhack = true;
	if (dump.find("SMAC Spinhack") != std::string::npos)
		output.smac_spinhack = true;

	std::remove("csgo\condump000.txt");

	if (backup)
		con_filter_enable->SetValue(1);
}


void Miscellaneous::AutoStrafe(CUserCmd *userCMD)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER || !g_LocalPlayer->IsAlive()) return;

	// If we're not jumping or want to manually move out of the way/jump over an obstacle don't strafe.
	if (!g_InputSystem->IsButtonDown(ButtonCode_t::KEY_SPACE) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_A) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_D) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_S) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_W))
		return;

	if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) {
		if (userCMD->mousedx > 1 || userCMD->mousedx < -1) {
			userCMD->sidemove = clamp(userCMD->mousedx < 0.f ? -400.f : 400.f, -400, 400);
		}
		else {
			if (g_LocalPlayer->m_vecVelocity().Length2D() == 0 || g_LocalPlayer->m_vecVelocity().Length2D() == NAN || g_LocalPlayer->m_vecVelocity().Length2D() == INFINITE)
			{
				userCMD->forwardmove = 400;
				return;
			}
			userCMD->forwardmove = clamp(5850.f / g_LocalPlayer->m_vecVelocity().Length2D(), -400, 400);
			if (userCMD->forwardmove < -400 || userCMD->forwardmove > 400)
				userCMD->forwardmove = 0;
			userCMD->sidemove = clamp((userCMD->command_number % 2) == 0 ? -400.f : 400.f, -400, 400);
			if (userCMD->sidemove < -400 || userCMD->sidemove > 400)
				userCMD->sidemove = 0;
		}
	}
}

void Miscellaneous::Fakelag(CUserCmd *userCMD)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (!XSystemCFG.misc_fakelag_enabled)
		return;

	int choke = std::min<int>(XSystemCFG.misc_fakelag_adaptive ? static_cast<int>(std::ceilf(64 / (g_LocalPlayer->m_vecVelocity().Length() * g_GlobalVars->interval_per_tick))) : XSystemCFG.misc_fakelag_value, 14);

	if (Global::bAimbotting && userCMD->buttons & IN_ATTACK)
		return;
	if (XSystemCFG.misc_fakelag_activation_type == 1 && g_LocalPlayer->m_vecVelocity().Length() < 3.0f)
		return;
	if (XSystemCFG.misc_fakelag_activation_type == 2 && (g_LocalPlayer->m_fFlags() & FL_ONGROUND))
		return;

	if (XSystemCFG.misc_fakelag_adaptive && choke > 13)
		return;

	if (!(Global::flFakewalked == PredictionSystem::Get().GetOldCurTime()))
		Global::bSendPacket = (choked > choke);

	if (Global::bSendPacket)
		choked = 0;
	else
		choked++;

	Global::bFakelag = true;
}

void Miscellaneous::ChangeName(std::string name)
{
	ConVar* cv = g_CVar->FindVar("name");
	*(int*)((DWORD)& cv->m_fnChangeCallbacks + 0xC) = 0;
	cv->SetValue(name.c_str());
}

void Miscellaneous::NameChanger()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (changes == -1)
		return;

	long curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	static long timestamp = curTime;

	if ((curTime - timestamp) < 150)
		return;

	timestamp = curTime;
	++changes;

	if (changes >= 5)
	{
		std::string name = "\n";
		char chars[3] = { '\n', '\0', '\t' };

		for (int i = 0; i < 127; i++)
			name += chars[rand() % 2];

		ChangeName(name.c_str());

		changes = -1;

		return;
	}
	ChangeName(setStrRight("nanosense", strlen("nanosense") + changes));
}

const char *Miscellaneous::setStrRight(std::string txt, unsigned int value)
{
	txt.insert(txt.length(), value - txt.length(), ' ');

	return txt.c_str();
}

void Miscellaneous::ChatSpamer()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (g_LocalPlayer->m_iTeamNum() == 0)
		return;

	long curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	static long timestamp = curTime;

	if ((curTime - timestamp) < 850)
		return;

	if (XSystemCFG.misc_chatspamer)
	{
		if (msgs.empty())
			return;

		std::srand(time(NULL));

		std::string msg = msgs[rand() % msgs.size()];

		std::string str;
		str.append("say ");
		str.append(msg);

		g_EngineClient->ExecuteClientCmd(str.c_str());
	}
	timestamp = curTime;
}

/*void Miscellaneous::ClanTag()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (g_LocalPlayer->m_iTeamNum() == 0)
		return;

	if (!XSystemCFG.misc_animated_clantag)
		return;

	static size_t lastTime = 0;



	if (GetTickCount() > lastTime)
	{
		gladTag += gladTag.at(0);
		gladTag.erase(0, 1);

			Utils::SetClantag(gladTag.c_str());

		lastTime = GetTickCount() + 350; // 650
	}
}*/
void Miscellaneous::ClanTag()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!XSystemCFG.misc_animated_clantag)
		return;

	static size_t lastTime = 0;
	if (GetTickCount() > lastTime)
	{


		gladTag += gladTag.at(0);
		gladTag.erase(0, 1);

		Utils::SetClantag(gladTag.c_str());

		lastTime = GetTickCount() + 450;
	}
}
void Miscellaneous::ThirdPerson()
{
	static size_t lastTime = 0;

	static QAngle vecAngles;
	g_EngineClient->GetViewAngles(vecAngles);

	vecAngles.roll = XSystemCFG.misc_thirdperson_distance;

	if (g_InputSystem->IsButtonDown(XSystemCFG.misc_thirdperson_bind))
	{
		if (GetTickCount() > lastTime) {
			XSystemCFG.misc_thirdperson = !XSystemCFG.misc_thirdperson;

			lastTime = GetTickCount() + 650;
		}
	}
	g_Input->m_vecCameraOffset = Vector(vecAngles.pitch, vecAngles.yaw, vecAngles.roll);
	g_Input->m_fCameraInThirdPerson = XSystemCFG.misc_thirdperson && g_LocalPlayer && g_LocalPlayer->IsAlive();
}

void Miscellaneous::PunchAngleFix_RunCommand(void* base_player)
{
	if (g_LocalPlayer &&  g_LocalPlayer->IsAlive() && g_LocalPlayer == (C_BasePlayer*)base_player)
		m_aimPunchAngle[AngryPew::Get().GetTickbase() % 128] = g_LocalPlayer->m_aimPunchAngle();
}

void Miscellaneous::PunchAngleFix_FSN()
{
	if (g_LocalPlayer && g_LocalPlayer->IsAlive())
	{
		QAngle new_punch_angle = m_aimPunchAngle[AngryPew::Get().GetTickbase() % 128];

		if (!new_punch_angle.IsValid())
			return;

		g_LocalPlayer->m_aimPunchAngle() = new_punch_angle;
	}
}

template<class T, class U>
T Miscellaneous::clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

void Miscellaneous::FixMovement(CUserCmd *usercmd, QAngle &wish_angle)
{
	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto viewangles = usercmd->viewangles;
	viewangles.Normalize();

	Math::AngleVectors(wish_angle, view_fwd, view_right, view_up);
	Math::AngleVectors(viewangles, cmd_fwd, cmd_right, cmd_up);

	const float v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const float v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const float v12 = sqrtf(view_up.z * view_up.z);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const float v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const float v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const float v18 = sqrtf(cmd_up.z * cmd_up.z);

	const Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const Vector norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const float v22 = norm_view_fwd.x * usercmd->forwardmove;
	const float v26 = norm_view_fwd.y * usercmd->forwardmove;
	const float v28 = norm_view_fwd.z * usercmd->forwardmove;
	const float v24 = norm_view_right.x * usercmd->sidemove;
	const float v23 = norm_view_right.y * usercmd->sidemove;
	const float v25 = norm_view_right.z * usercmd->sidemove;
	const float v30 = norm_view_up.x * usercmd->upmove;
	const float v27 = norm_view_up.z * usercmd->upmove;
	const float v29 = norm_view_up.y * usercmd->upmove;

	usercmd->forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	usercmd->sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	usercmd->upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	usercmd->forwardmove = clamp(usercmd->forwardmove, -450.f, 450.f);
	usercmd->sidemove = clamp(usercmd->sidemove, -450.f, 450.f);
	usercmd->upmove = clamp(usercmd->upmove, -320.f, 320.f);
}

void Miscellaneous::AutoPistol(CUserCmd *usercmd)
{
	if (!XSystemCFG.misc_auto_pistol)
		return;

	if (!g_LocalPlayer)
		return;

	C_BaseCombatWeapon* local_weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!local_weapon)
		return;

	if (!local_weapon->IsPistol())
		return;

	float cur_time = AngryPew::Get().GetTickbase() * g_GlobalVars->interval_per_tick;
	if (cur_time >= local_weapon->m_flNextPrimaryAttack() && cur_time >= g_LocalPlayer->m_flNextAttack())
		return;

	usercmd->buttons &= ~IN_ATTACK;
}

void Miscellaneous::AldaVerdammteScheisse(CUserCmd *usercmd)
{
	if (!g_EngineClient->IsInGame() && !g_EngineClient->IsConnected())
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	if (XSystemCFG.misc_infinite_duck)
		usercmd->buttons |= IN_BULLRUSH; // Infinite Duck
}

void Miscellaneous::AntiAim(CUserCmd* usercmd)
{
	if (!g_LocalPlayer || !g_LocalPlayer->m_hActiveWeapon().Get())
		return;

	bool can_shoot = g_LocalPlayer->m_hActiveWeapon().Get()->CanFire();

	if ((g_LocalPlayer->m_hActiveWeapon().Get()->m_iItemDefinitionIndex() != WEAPON_REVOLVER && !(usercmd->buttons & IN_ATTACK) || !(can_shoot)) && !(usercmd->buttons & IN_USE) || g_LocalPlayer->m_hActiveWeapon().Get()->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (XSystemCFG.hvh_antiaim_y || XSystemCFG.hvh_antiaim_x || XSystemCFG.hvh_antiaim_legit)
		{
			Global::bAimbotting = false;
			AntiAim::Get().Work(usercmd);
			if (XSystemCFG.hvh_antiaim_legit)
			{
				usercmd->buttons &= ~IN_MOVERIGHT;
				usercmd->buttons &= ~IN_MOVELEFT;
				usercmd->buttons &= ~IN_FORWARD;
				usercmd->buttons &= ~IN_BACK;

				if (usercmd->forwardmove > 0.f)
					usercmd->buttons |= IN_FORWARD;
				else if (usercmd->forwardmove < 0.f)
					usercmd->buttons |= IN_BACK;
				if (usercmd->sidemove > 0.f)
				{
					usercmd->buttons |= IN_MOVERIGHT;
				}
				else if (usercmd->sidemove < 0.f)
				{
					usercmd->buttons |= IN_MOVELEFT;
				}
			}
		}
	}
}
void Miscellaneous::ChangeRegion()
{
	switch (XSystemCFG.misc_region_changer) {
	case 0:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster ams");
		break;
	case 1:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster atl");
		break;
	case 2:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster bom");
		break;
	case 3:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster can");
		break;
	case 4:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster canm");
		break;
	case 5:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster cant");
		break;
	case 6:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster canu");
		break;
	case 7:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster dxb");
		break;
	case 8:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster eat");
		break;
	case 9:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster fra");
		break;
	case 10:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster gru");
		break;
	case 11:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster hkg");
		break;
	case 12:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster iad");
		break;
	case 13:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster jnb");
		break;
	case 14:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lax");
		break;
	case 15:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lhr");
		break;
	case 16:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lim");
		break;
	case 17:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lux");
		break;
	case 18:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster maa");
		break;
	case 19:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster mad");
		break;
	case 20:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster man");
		break;
	case 21:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster okc");
		break;
	case 22:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster ord");
		break;
	case 23:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster par");
		break;
	case 24:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwg");
		break;
	case 25:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwj");
		break;
	case 26:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwu");
		break;
	case 27:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pww");
		break;
	case 28:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwz");
		break;
	case 29:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster scl");
		break;
	case 30:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sea");
		break;
	case 31:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sgp");
		break;
	case 32:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sha");
		break;
	case 33:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sham");
		break;
	case 34:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shat");
		break;
	case 35:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shau");
		break;
	case 36:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shb");
		break;
	case 37:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sto");
		break;
	case 38:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sto2");
		break;
	case 39:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster syd");
		break;
	case 40:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsn");
		break;
	case 41:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnm");
		break;
	case 42:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnt");
		break;
	case 43:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnu");
		break;
	case 44:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tyo");
		break;
	case 45:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tyo1");
		break;
	case 46:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster vie");
		break;
	case 47:
		g_EngineClient->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster waw");
		break;
	}
}











































































































// Junk Code By Troll Face & Thaisen's Gen
void ZoPRhdThZKurblQVhDsyBLrKkcHFXpzAtreuwLfU54814562() {     int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu49687267 = -197608175;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu70272193 = -180409776;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu96138552 = -44730027;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu84669247 = 96744498;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu17075572 = 18928355;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu73859504 = -737262400;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu31459419 = -666805770;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu78266300 = -441219294;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu26968872 = -882849652;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu82818448 = -342266033;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu75053734 = -120916059;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu17864993 = -115698285;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu87008252 = -613763542;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu36550462 = -289268017;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu35977842 = -417401064;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu18637628 = -944486383;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu74418806 = -8015048;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu33225341 = -279315133;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu94817014 = -148168803;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu42588087 = -425284171;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu23635014 = -895367717;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu49580556 = -420218047;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu43853933 = -611945996;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu74466515 = -5056340;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu97543546 = -94964068;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu92086702 = -788851790;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu34551062 = -134345865;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu93450400 = -103997820;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu81729892 = -145255779;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu28204689 = -923822584;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu77853773 = -827602524;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu66473125 = -14800333;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu15779860 = -415183559;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu41455082 = -920433057;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu23103875 = -454741107;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu90060053 = 72688405;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu60557187 = -478390724;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu91424337 = -470630257;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38379733 = -577712742;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu78210718 = 71837222;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu86835942 = -753298633;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu52249461 = -469026290;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu29390567 = -573995832;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu97826738 = -444703105;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu87906882 = -526115143;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu75029845 = -84241982;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu20061372 = -958499817;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu289313 = -563446970;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu45621833 = -335299438;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu50710292 = -627588584;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu6879705 = -354888782;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu25135992 = -7841273;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu67404542 = -414940985;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu66402176 = -553233130;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu31179549 = -232043272;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu106712 = -777390129;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu26418260 = -568463780;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu21672038 = 60326313;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu87125700 = -808291434;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu24988870 = -192219855;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu39308443 = -502916536;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38009018 = -462807951;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu96536407 = -195963516;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu98764183 = -959027069;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu4964676 = -514663509;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu8580610 = -6115727;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu2085134 = -700514726;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu45553170 = -693330485;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu13446588 = -834526911;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu45917788 = -390089469;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu58080441 = -366095660;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu82994468 = -537384791;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu94845607 = -701602391;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu16606296 = -120006026;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu55752144 = -671985539;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu71385552 = -326341428;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu20189989 = -846222215;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu46027195 = -67242892;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu86559632 = -478941198;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu22513702 = 89277913;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu72025330 = -830351974;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu34261750 = -570898895;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu47828567 = -768698382;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu31019601 = -517667196;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu21324984 = -468933802;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu52717782 = -719761252;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu99068583 = -599859348;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu49377683 = -861950430;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu10275533 = -588389786;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu22997164 = -677350979;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu63641794 = -358847815;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38885149 = -438717037;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu4298638 = -662338823;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu13390864 = -285492888;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38902276 = -425246243;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu48826925 = -190490682;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu55713054 = -173062774;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu30626383 = -614968764;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu92862063 = -930039596;    int AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu79326273 = -197608175;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu49687267 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu70272193;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu70272193 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu96138552;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu96138552 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu84669247;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu84669247 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu17075572;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu17075572 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu73859504;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu73859504 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu31459419;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu31459419 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu78266300;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu78266300 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu26968872;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu26968872 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu82818448;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu82818448 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu75053734;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu75053734 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu17864993;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu17864993 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu87008252;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu87008252 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu36550462;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu36550462 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu35977842;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu35977842 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu18637628;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu18637628 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu74418806;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu74418806 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu33225341;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu33225341 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu94817014;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu94817014 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu42588087;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu42588087 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu23635014;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu23635014 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu49580556;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu49580556 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu43853933;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu43853933 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu74466515;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu74466515 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu97543546;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu97543546 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu92086702;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu92086702 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu34551062;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu34551062 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu93450400;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu93450400 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu81729892;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu81729892 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu28204689;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu28204689 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu77853773;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu77853773 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu66473125;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu66473125 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu15779860;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu15779860 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu41455082;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu41455082 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu23103875;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu23103875 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu90060053;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu90060053 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu60557187;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu60557187 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu91424337;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu91424337 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38379733;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38379733 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu78210718;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu78210718 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu86835942;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu86835942 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu52249461;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu52249461 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu29390567;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu29390567 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu97826738;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu97826738 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu87906882;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu87906882 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu75029845;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu75029845 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu20061372;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu20061372 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu289313;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu289313 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu45621833;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu45621833 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu50710292;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu50710292 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu6879705;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu6879705 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu25135992;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu25135992 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu67404542;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu67404542 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu66402176;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu66402176 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu31179549;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu31179549 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu106712;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu106712 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu26418260;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu26418260 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu21672038;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu21672038 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu87125700;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu87125700 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu24988870;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu24988870 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu39308443;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu39308443 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38009018;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38009018 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu96536407;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu96536407 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu98764183;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu98764183 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu4964676;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu4964676 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu8580610;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu8580610 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu2085134;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu2085134 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu45553170;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu45553170 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu13446588;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu13446588 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu45917788;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu45917788 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu58080441;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu58080441 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu82994468;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu82994468 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu94845607;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu94845607 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu16606296;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu16606296 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu55752144;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu55752144 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu71385552;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu71385552 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu20189989;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu20189989 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu46027195;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu46027195 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu86559632;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu86559632 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu22513702;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu22513702 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu72025330;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu72025330 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu34261750;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu34261750 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu47828567;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu47828567 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu31019601;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu31019601 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu21324984;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu21324984 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu52717782;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu52717782 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu99068583;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu99068583 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu49377683;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu49377683 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu10275533;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu10275533 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu22997164;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu22997164 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu63641794;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu63641794 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38885149;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38885149 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu4298638;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu4298638 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu13390864;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu13390864 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38902276;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu38902276 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu48826925;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu48826925 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu55713054;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu55713054 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu30626383;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu30626383 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu92862063;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu92862063 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu79326273;     AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu79326273 = AwsERFASlfpWYmzOwEhEVNJjKtxkHWYVYYyFSnsecpTsAcbHrQxseaymmItXHQLOGTyAnu49687267;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void QxJKEaayQWsCvYeUTghhKCcFxinXFsntEMzFgPoSaHqAFdPOPEu41323443() {     float ZGOXOUDNVnDtXizxiZZoDrOHDMcz56392235 = -567829023;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz64633933 = -952157502;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz23508413 = -865153396;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz78525961 = -638513846;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz10925428 = -619737145;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz4145398 = -636095654;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz2960093 = -574119263;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz9613849 = -763528792;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz19165913 = -682456478;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz22643177 = -744060517;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz53087206 = -616890919;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz19904394 = -22524110;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz29955202 = -836028636;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz236893 = -392094105;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz77003862 = 18220306;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz9352426 = -290755918;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz65071837 = -163684714;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz17310655 = -714317684;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz24109633 = 92629621;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz96257392 = 97256315;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz89568831 = -68628657;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz30009284 = -211076152;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz94565992 = -263728522;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz42702607 = -13696733;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz92657291 = -790898329;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz97638221 = 22734196;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz41843191 = -863051099;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz61383195 = -729904236;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz89486653 = -14608349;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz74372347 = -122467180;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz12948919 = -37135514;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz57804355 = 76833864;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz97139869 = -147046185;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz57388021 = -71832307;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz68477955 = 4478492;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz14031280 = -819903540;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz48591198 = -58766418;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz69245303 = -640455570;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz53021850 = -344546027;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz98415390 = -863804747;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz79029289 = -216500219;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz61081446 = -19731189;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz38454431 = -756057609;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz13359498 = -93971980;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz76803288 = -920064038;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz62052794 = -393879153;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz76916498 = -418332053;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz44017720 = -690762923;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz95834960 = -481139222;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz95330578 = -211999062;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz28908748 = -324338976;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz3005236 = -835185558;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz47191626 = 37639898;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz46095696 = -956938680;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz51754612 = 65353463;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz26382952 = -256752871;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz70067940 = -588428981;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz80805806 = -751456664;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz85868669 = -847615517;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz13287206 = -542471342;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz62302206 = -773044555;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz41576898 = -844215028;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz20127195 = -648920443;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz44793566 = -459989298;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz9694258 = -606925003;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz95282851 = -593724783;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz22764524 = -875477925;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz72567180 = -664196329;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz31758938 = -296572597;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz62972583 = -161876155;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz60761228 = -131989501;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz95826534 = -523229145;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz64288804 = -269771658;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz25694243 = -43565633;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz17228103 = -686243466;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz28487386 = 51102531;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz91554853 = -455018543;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz81206495 = -69756542;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz65899318 = -93632696;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz30604498 = -297019177;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz20721724 = -558933751;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz97825471 = -72288177;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz65548235 = -148765014;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz94156075 = -802609288;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz45463600 = -798128204;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz9943684 = -201949956;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz10612730 = -960806035;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz51044174 = -190107506;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz5633410 = -37185771;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz42095004 = -738768637;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz43963340 = -131474559;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz67785392 = -307309754;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz83376634 = -792840053;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz39734644 = -802074685;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz36113184 = 9239808;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz37452392 = -372285192;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz40954251 = -370810746;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz93660865 = -196068311;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz3665240 = -487046978;    float ZGOXOUDNVnDtXizxiZZoDrOHDMcz81520437 = -567829023;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz56392235 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz64633933;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz64633933 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz23508413;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz23508413 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz78525961;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz78525961 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz10925428;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz10925428 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz4145398;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz4145398 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz2960093;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz2960093 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz9613849;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz9613849 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz19165913;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz19165913 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz22643177;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz22643177 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz53087206;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz53087206 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz19904394;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz19904394 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz29955202;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz29955202 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz236893;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz236893 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz77003862;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz77003862 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz9352426;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz9352426 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz65071837;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz65071837 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz17310655;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz17310655 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz24109633;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz24109633 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz96257392;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz96257392 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz89568831;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz89568831 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz30009284;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz30009284 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz94565992;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz94565992 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz42702607;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz42702607 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz92657291;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz92657291 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz97638221;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz97638221 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz41843191;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz41843191 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz61383195;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz61383195 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz89486653;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz89486653 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz74372347;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz74372347 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz12948919;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz12948919 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz57804355;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz57804355 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz97139869;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz97139869 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz57388021;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz57388021 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz68477955;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz68477955 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz14031280;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz14031280 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz48591198;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz48591198 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz69245303;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz69245303 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz53021850;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz53021850 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz98415390;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz98415390 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz79029289;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz79029289 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz61081446;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz61081446 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz38454431;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz38454431 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz13359498;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz13359498 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz76803288;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz76803288 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz62052794;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz62052794 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz76916498;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz76916498 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz44017720;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz44017720 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz95834960;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz95834960 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz95330578;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz95330578 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz28908748;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz28908748 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz3005236;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz3005236 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz47191626;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz47191626 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz46095696;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz46095696 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz51754612;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz51754612 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz26382952;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz26382952 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz70067940;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz70067940 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz80805806;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz80805806 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz85868669;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz85868669 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz13287206;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz13287206 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz62302206;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz62302206 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz41576898;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz41576898 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz20127195;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz20127195 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz44793566;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz44793566 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz9694258;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz9694258 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz95282851;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz95282851 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz22764524;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz22764524 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz72567180;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz72567180 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz31758938;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz31758938 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz62972583;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz62972583 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz60761228;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz60761228 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz95826534;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz95826534 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz64288804;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz64288804 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz25694243;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz25694243 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz17228103;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz17228103 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz28487386;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz28487386 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz91554853;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz91554853 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz81206495;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz81206495 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz65899318;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz65899318 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz30604498;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz30604498 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz20721724;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz20721724 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz97825471;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz97825471 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz65548235;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz65548235 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz94156075;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz94156075 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz45463600;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz45463600 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz9943684;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz9943684 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz10612730;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz10612730 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz51044174;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz51044174 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz5633410;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz5633410 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz42095004;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz42095004 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz43963340;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz43963340 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz67785392;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz67785392 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz83376634;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz83376634 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz39734644;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz39734644 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz36113184;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz36113184 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz37452392;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz37452392 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz40954251;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz40954251 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz93660865;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz93660865 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz3665240;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz3665240 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz81520437;     ZGOXOUDNVnDtXizxiZZoDrOHDMcz81520437 = ZGOXOUDNVnDtXizxiZZoDrOHDMcz56392235;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void EWoBaBVptmWGZccxnExoPzocv21727589() {     long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG88124055 = -324370198;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG93940803 = -170843679;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG99402227 = -238473375;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG49393203 = -658930897;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27237263 = -123163532;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG25770334 = -41411880;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG48099121 = -500498314;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG97986241 = -199927055;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG53312591 = -266240658;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG54506414 = -871086718;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG58508035 = 77045105;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG3248708 = -490016792;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG83610776 = -720200089;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG25990536 = -446659474;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG87649181 = -871355636;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG36938533 = -901120278;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG43295934 = -296604503;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG56060226 = -738504201;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG57794296 = -869597078;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG41607760 = -81228725;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG33113669 = -833714398;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG34231613 = -577660155;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG18341535 = 19407926;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG79780467 = -946371590;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG52621907 = -916972728;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG17377661 = -905215735;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG87248831 = -625987307;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG87479640 = 47500325;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG33446532 = -996873254;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG34432525 = -10508292;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG43552480 = -62633896;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG62951794 = 62676511;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG58515303 = -438046120;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG39775624 = -954387574;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG37503834 = -64486076;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27850258 = -749345058;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG23971771 = -336669823;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG3033038 = -277650314;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG85346263 = -328395465;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG48015774 = -781310321;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG56136725 = -97779689;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG56219139 = -915239896;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG78252361 = -564031744;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG89104376 = -566134531;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG85595062 = -57319800;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG6154296 = -935222325;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG19213000 = -131949796;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG57716395 = -384461364;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG90805205 = -298291096;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG92679168 = -994442732;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27407271 = -652705814;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG16123208 = -253022074;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG82512425 = -477307670;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG52176430 = 40883590;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG88613919 = -175858575;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG53892443 = -746710044;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG75599268 = -90251605;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG19621761 = -292101785;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG96771295 = -741958170;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG9859603 = -217947797;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG38521502 = -415424573;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG60619481 = -447998639;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG64539709 = -203053802;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG18880066 = -155732366;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG10953934 = -708452822;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG95556241 = -985631407;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG44733404 = 48029327;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG43835153 = -765812516;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG88486701 = -282173399;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG59798923 = -22010579;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG12966762 = -464450456;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG40262896 = 81045811;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG70713963 = -310108736;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG9778522 = 11713243;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG85471035 = -983449037;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG76894530 = -918474503;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG55979251 = 86371589;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG29237159 = -414457543;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG94185405 = -789051791;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG46467612 = -981750404;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG98164660 = -673265940;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG29532437 = -141525943;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG96674435 = -654208580;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG40767364 = 97569478;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG7025255 = -357802478;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27429273 = -809611823;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG80439368 = -460015820;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG6338873 = -378929710;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG51161705 = -678528999;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG83611391 = -317776032;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG52250989 = -559093453;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG4350011 = 55431961;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG6261742 = -535692145;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG75486661 = -10447668;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG9494272 = -265885749;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG95517243 = -649781050;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG91679429 = -612186094;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG59372296 = -308299379;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG78150442 = -857681710;    long LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG90038820 = -324370198;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG88124055 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG93940803;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG93940803 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG99402227;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG99402227 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG49393203;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG49393203 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27237263;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27237263 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG25770334;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG25770334 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG48099121;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG48099121 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG97986241;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG97986241 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG53312591;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG53312591 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG54506414;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG54506414 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG58508035;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG58508035 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG3248708;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG3248708 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG83610776;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG83610776 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG25990536;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG25990536 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG87649181;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG87649181 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG36938533;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG36938533 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG43295934;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG43295934 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG56060226;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG56060226 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG57794296;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG57794296 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG41607760;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG41607760 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG33113669;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG33113669 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG34231613;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG34231613 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG18341535;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG18341535 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG79780467;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG79780467 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG52621907;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG52621907 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG17377661;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG17377661 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG87248831;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG87248831 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG87479640;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG87479640 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG33446532;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG33446532 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG34432525;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG34432525 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG43552480;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG43552480 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG62951794;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG62951794 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG58515303;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG58515303 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG39775624;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG39775624 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG37503834;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG37503834 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27850258;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27850258 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG23971771;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG23971771 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG3033038;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG3033038 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG85346263;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG85346263 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG48015774;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG48015774 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG56136725;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG56136725 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG56219139;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG56219139 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG78252361;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG78252361 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG89104376;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG89104376 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG85595062;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG85595062 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG6154296;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG6154296 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG19213000;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG19213000 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG57716395;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG57716395 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG90805205;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG90805205 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG92679168;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG92679168 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27407271;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27407271 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG16123208;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG16123208 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG82512425;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG82512425 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG52176430;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG52176430 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG88613919;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG88613919 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG53892443;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG53892443 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG75599268;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG75599268 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG19621761;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG19621761 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG96771295;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG96771295 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG9859603;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG9859603 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG38521502;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG38521502 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG60619481;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG60619481 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG64539709;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG64539709 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG18880066;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG18880066 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG10953934;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG10953934 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG95556241;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG95556241 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG44733404;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG44733404 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG43835153;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG43835153 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG88486701;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG88486701 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG59798923;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG59798923 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG12966762;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG12966762 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG40262896;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG40262896 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG70713963;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG70713963 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG9778522;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG9778522 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG85471035;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG85471035 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG76894530;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG76894530 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG55979251;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG55979251 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG29237159;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG29237159 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG94185405;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG94185405 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG46467612;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG46467612 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG98164660;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG98164660 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG29532437;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG29532437 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG96674435;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG96674435 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG40767364;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG40767364 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG7025255;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG7025255 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27429273;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG27429273 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG80439368;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG80439368 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG6338873;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG6338873 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG51161705;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG51161705 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG83611391;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG83611391 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG52250989;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG52250989 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG4350011;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG4350011 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG6261742;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG6261742 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG75486661;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG75486661 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG9494272;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG9494272 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG95517243;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG95517243 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG91679429;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG91679429 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG59372296;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG59372296 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG78150442;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG78150442 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG90038820;     LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG90038820 = LHNXEfpfjKVbNLsrvDShCqtTarIhGPFiMGOtISpGhapSG88124055;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void JnWiwGFmHByNzbplOwqulhHxxsdRckjjQVFOU13049421() {     double XyuVKcTYgSYkeHvxRpbfZ95433063 = -721599651;    double XyuVKcTYgSYkeHvxRpbfZ92860212 = -798251538;    double XyuVKcTYgSYkeHvxRpbfZ84816733 = -185665657;    double XyuVKcTYgSYkeHvxRpbfZ47332199 = -737346880;    double XyuVKcTYgSYkeHvxRpbfZ23941231 = -808956357;    double XyuVKcTYgSYkeHvxRpbfZ7911337 = -326000124;    double XyuVKcTYgSYkeHvxRpbfZ9243165 = -364819155;    double XyuVKcTYgSYkeHvxRpbfZ37886322 = -197228844;    double XyuVKcTYgSYkeHvxRpbfZ66213766 = -263306469;    double XyuVKcTYgSYkeHvxRpbfZ1327426 = -603683031;    double XyuVKcTYgSYkeHvxRpbfZ3663175 = -38786849;    double XyuVKcTYgSYkeHvxRpbfZ39775273 = -520566335;    double XyuVKcTYgSYkeHvxRpbfZ3663273 = -120693001;    double XyuVKcTYgSYkeHvxRpbfZ85609170 = -357190452;    double XyuVKcTYgSYkeHvxRpbfZ61028524 = -502868601;    double XyuVKcTYgSYkeHvxRpbfZ40451367 = -438670679;    double XyuVKcTYgSYkeHvxRpbfZ91134095 = -442362299;    double XyuVKcTYgSYkeHvxRpbfZ19676369 = -400307606;    double XyuVKcTYgSYkeHvxRpbfZ53479017 = -637275468;    double XyuVKcTYgSYkeHvxRpbfZ88211798 = -449183705;    double XyuVKcTYgSYkeHvxRpbfZ77254552 = -48582887;    double XyuVKcTYgSYkeHvxRpbfZ62827380 = -626466941;    double XyuVKcTYgSYkeHvxRpbfZ91978870 = -40245034;    double XyuVKcTYgSYkeHvxRpbfZ23135005 = 60953919;    double XyuVKcTYgSYkeHvxRpbfZ78043023 = -318910586;    double XyuVKcTYgSYkeHvxRpbfZ12323832 = -732873618;    double XyuVKcTYgSYkeHvxRpbfZ77819357 = -330736290;    double XyuVKcTYgSYkeHvxRpbfZ21729293 = -715966083;    double XyuVKcTYgSYkeHvxRpbfZ51131682 = -548019902;    double XyuVKcTYgSYkeHvxRpbfZ17927532 = -501748183;    double XyuVKcTYgSYkeHvxRpbfZ37968524 = -900645852;    double XyuVKcTYgSYkeHvxRpbfZ86193987 = -165053877;    double XyuVKcTYgSYkeHvxRpbfZ97893317 = -54297462;    double XyuVKcTYgSYkeHvxRpbfZ76103845 = -363939977;    double XyuVKcTYgSYkeHvxRpbfZ60882248 = -59028960;    double XyuVKcTYgSYkeHvxRpbfZ99996323 = -471088475;    double XyuVKcTYgSYkeHvxRpbfZ38924439 = -944651896;    double XyuVKcTYgSYkeHvxRpbfZ49790558 = -857638233;    double XyuVKcTYgSYkeHvxRpbfZ52736251 = -836434996;    double XyuVKcTYgSYkeHvxRpbfZ71240948 = -405575823;    double XyuVKcTYgSYkeHvxRpbfZ65215025 = -389049221;    double XyuVKcTYgSYkeHvxRpbfZ77488498 = -362027210;    double XyuVKcTYgSYkeHvxRpbfZ17604712 = -549525079;    double XyuVKcTYgSYkeHvxRpbfZ34682407 = -952918235;    double XyuVKcTYgSYkeHvxRpbfZ41390277 = -550170932;    double XyuVKcTYgSYkeHvxRpbfZ67191078 = -389168874;    double XyuVKcTYgSYkeHvxRpbfZ45900197 = 70362824;    double XyuVKcTYgSYkeHvxRpbfZ98757316 = -757294347;    double XyuVKcTYgSYkeHvxRpbfZ84909757 = 55175977;    double XyuVKcTYgSYkeHvxRpbfZ18894532 = -8690663;    double XyuVKcTYgSYkeHvxRpbfZ2616254 = -702925014;    double XyuVKcTYgSYkeHvxRpbfZ24683831 = -601619954;    double XyuVKcTYgSYkeHvxRpbfZ55217265 = -600221296;    double XyuVKcTYgSYkeHvxRpbfZ89968912 = -766257737;    double XyuVKcTYgSYkeHvxRpbfZ35794869 = -588285128;    double XyuVKcTYgSYkeHvxRpbfZ32605683 = 4867289;    double XyuVKcTYgSYkeHvxRpbfZ881343 = -658006505;    double XyuVKcTYgSYkeHvxRpbfZ61681728 = -146619577;    double XyuVKcTYgSYkeHvxRpbfZ69289176 = -318436294;    double XyuVKcTYgSYkeHvxRpbfZ11617399 = 23917260;    double XyuVKcTYgSYkeHvxRpbfZ30091979 = -995263834;    double XyuVKcTYgSYkeHvxRpbfZ87513871 = -648853073;    double XyuVKcTYgSYkeHvxRpbfZ86754640 = -649208942;    double XyuVKcTYgSYkeHvxRpbfZ48286235 = -761558287;    double XyuVKcTYgSYkeHvxRpbfZ63358902 = -703037179;    double XyuVKcTYgSYkeHvxRpbfZ17469188 = -873732972;    double XyuVKcTYgSYkeHvxRpbfZ41881956 = -366268873;    double XyuVKcTYgSYkeHvxRpbfZ27559428 = -756753025;    double XyuVKcTYgSYkeHvxRpbfZ24726922 = -198161492;    double XyuVKcTYgSYkeHvxRpbfZ61032201 = 68219873;    double XyuVKcTYgSYkeHvxRpbfZ1526928 = -494018783;    double XyuVKcTYgSYkeHvxRpbfZ41343537 = -584724066;    double XyuVKcTYgSYkeHvxRpbfZ66940117 = -563872610;    double XyuVKcTYgSYkeHvxRpbfZ82238069 = -131699646;    double XyuVKcTYgSYkeHvxRpbfZ22996774 = 39865515;    double XyuVKcTYgSYkeHvxRpbfZ99766053 = -686555678;    double XyuVKcTYgSYkeHvxRpbfZ45222669 = 23058137;    double XyuVKcTYgSYkeHvxRpbfZ57296463 = -87326799;    double XyuVKcTYgSYkeHvxRpbfZ81744728 = -388875150;    double XyuVKcTYgSYkeHvxRpbfZ10851945 = -929741713;    double XyuVKcTYgSYkeHvxRpbfZ66423635 = -703236442;    double XyuVKcTYgSYkeHvxRpbfZ79062040 = -573441943;    double XyuVKcTYgSYkeHvxRpbfZ36819535 = -671142061;    double XyuVKcTYgSYkeHvxRpbfZ32237150 = -439329239;    double XyuVKcTYgSYkeHvxRpbfZ15311278 = -798823170;    double XyuVKcTYgSYkeHvxRpbfZ13284694 = -199025898;    double XyuVKcTYgSYkeHvxRpbfZ30976723 = -564832581;    double XyuVKcTYgSYkeHvxRpbfZ7924405 = -288039726;    double XyuVKcTYgSYkeHvxRpbfZ40308976 = -775654849;    double XyuVKcTYgSYkeHvxRpbfZ28276566 = 36103750;    double XyuVKcTYgSYkeHvxRpbfZ99114981 = -813081970;    double XyuVKcTYgSYkeHvxRpbfZ77242711 = -698032320;    double XyuVKcTYgSYkeHvxRpbfZ80501382 = -439201939;    double XyuVKcTYgSYkeHvxRpbfZ41118853 = -760352257;    double XyuVKcTYgSYkeHvxRpbfZ41148969 = -410311989;    double XyuVKcTYgSYkeHvxRpbfZ77701153 = -740196149;    double XyuVKcTYgSYkeHvxRpbfZ90733858 = -712818268;    double XyuVKcTYgSYkeHvxRpbfZ69318476 = -787966793;    double XyuVKcTYgSYkeHvxRpbfZ71323505 = -149881057;    double XyuVKcTYgSYkeHvxRpbfZ23921090 = -721599651;     XyuVKcTYgSYkeHvxRpbfZ95433063 = XyuVKcTYgSYkeHvxRpbfZ92860212;     XyuVKcTYgSYkeHvxRpbfZ92860212 = XyuVKcTYgSYkeHvxRpbfZ84816733;     XyuVKcTYgSYkeHvxRpbfZ84816733 = XyuVKcTYgSYkeHvxRpbfZ47332199;     XyuVKcTYgSYkeHvxRpbfZ47332199 = XyuVKcTYgSYkeHvxRpbfZ23941231;     XyuVKcTYgSYkeHvxRpbfZ23941231 = XyuVKcTYgSYkeHvxRpbfZ7911337;     XyuVKcTYgSYkeHvxRpbfZ7911337 = XyuVKcTYgSYkeHvxRpbfZ9243165;     XyuVKcTYgSYkeHvxRpbfZ9243165 = XyuVKcTYgSYkeHvxRpbfZ37886322;     XyuVKcTYgSYkeHvxRpbfZ37886322 = XyuVKcTYgSYkeHvxRpbfZ66213766;     XyuVKcTYgSYkeHvxRpbfZ66213766 = XyuVKcTYgSYkeHvxRpbfZ1327426;     XyuVKcTYgSYkeHvxRpbfZ1327426 = XyuVKcTYgSYkeHvxRpbfZ3663175;     XyuVKcTYgSYkeHvxRpbfZ3663175 = XyuVKcTYgSYkeHvxRpbfZ39775273;     XyuVKcTYgSYkeHvxRpbfZ39775273 = XyuVKcTYgSYkeHvxRpbfZ3663273;     XyuVKcTYgSYkeHvxRpbfZ3663273 = XyuVKcTYgSYkeHvxRpbfZ85609170;     XyuVKcTYgSYkeHvxRpbfZ85609170 = XyuVKcTYgSYkeHvxRpbfZ61028524;     XyuVKcTYgSYkeHvxRpbfZ61028524 = XyuVKcTYgSYkeHvxRpbfZ40451367;     XyuVKcTYgSYkeHvxRpbfZ40451367 = XyuVKcTYgSYkeHvxRpbfZ91134095;     XyuVKcTYgSYkeHvxRpbfZ91134095 = XyuVKcTYgSYkeHvxRpbfZ19676369;     XyuVKcTYgSYkeHvxRpbfZ19676369 = XyuVKcTYgSYkeHvxRpbfZ53479017;     XyuVKcTYgSYkeHvxRpbfZ53479017 = XyuVKcTYgSYkeHvxRpbfZ88211798;     XyuVKcTYgSYkeHvxRpbfZ88211798 = XyuVKcTYgSYkeHvxRpbfZ77254552;     XyuVKcTYgSYkeHvxRpbfZ77254552 = XyuVKcTYgSYkeHvxRpbfZ62827380;     XyuVKcTYgSYkeHvxRpbfZ62827380 = XyuVKcTYgSYkeHvxRpbfZ91978870;     XyuVKcTYgSYkeHvxRpbfZ91978870 = XyuVKcTYgSYkeHvxRpbfZ23135005;     XyuVKcTYgSYkeHvxRpbfZ23135005 = XyuVKcTYgSYkeHvxRpbfZ78043023;     XyuVKcTYgSYkeHvxRpbfZ78043023 = XyuVKcTYgSYkeHvxRpbfZ12323832;     XyuVKcTYgSYkeHvxRpbfZ12323832 = XyuVKcTYgSYkeHvxRpbfZ77819357;     XyuVKcTYgSYkeHvxRpbfZ77819357 = XyuVKcTYgSYkeHvxRpbfZ21729293;     XyuVKcTYgSYkeHvxRpbfZ21729293 = XyuVKcTYgSYkeHvxRpbfZ51131682;     XyuVKcTYgSYkeHvxRpbfZ51131682 = XyuVKcTYgSYkeHvxRpbfZ17927532;     XyuVKcTYgSYkeHvxRpbfZ17927532 = XyuVKcTYgSYkeHvxRpbfZ37968524;     XyuVKcTYgSYkeHvxRpbfZ37968524 = XyuVKcTYgSYkeHvxRpbfZ86193987;     XyuVKcTYgSYkeHvxRpbfZ86193987 = XyuVKcTYgSYkeHvxRpbfZ97893317;     XyuVKcTYgSYkeHvxRpbfZ97893317 = XyuVKcTYgSYkeHvxRpbfZ76103845;     XyuVKcTYgSYkeHvxRpbfZ76103845 = XyuVKcTYgSYkeHvxRpbfZ60882248;     XyuVKcTYgSYkeHvxRpbfZ60882248 = XyuVKcTYgSYkeHvxRpbfZ99996323;     XyuVKcTYgSYkeHvxRpbfZ99996323 = XyuVKcTYgSYkeHvxRpbfZ38924439;     XyuVKcTYgSYkeHvxRpbfZ38924439 = XyuVKcTYgSYkeHvxRpbfZ49790558;     XyuVKcTYgSYkeHvxRpbfZ49790558 = XyuVKcTYgSYkeHvxRpbfZ52736251;     XyuVKcTYgSYkeHvxRpbfZ52736251 = XyuVKcTYgSYkeHvxRpbfZ71240948;     XyuVKcTYgSYkeHvxRpbfZ71240948 = XyuVKcTYgSYkeHvxRpbfZ65215025;     XyuVKcTYgSYkeHvxRpbfZ65215025 = XyuVKcTYgSYkeHvxRpbfZ77488498;     XyuVKcTYgSYkeHvxRpbfZ77488498 = XyuVKcTYgSYkeHvxRpbfZ17604712;     XyuVKcTYgSYkeHvxRpbfZ17604712 = XyuVKcTYgSYkeHvxRpbfZ34682407;     XyuVKcTYgSYkeHvxRpbfZ34682407 = XyuVKcTYgSYkeHvxRpbfZ41390277;     XyuVKcTYgSYkeHvxRpbfZ41390277 = XyuVKcTYgSYkeHvxRpbfZ67191078;     XyuVKcTYgSYkeHvxRpbfZ67191078 = XyuVKcTYgSYkeHvxRpbfZ45900197;     XyuVKcTYgSYkeHvxRpbfZ45900197 = XyuVKcTYgSYkeHvxRpbfZ98757316;     XyuVKcTYgSYkeHvxRpbfZ98757316 = XyuVKcTYgSYkeHvxRpbfZ84909757;     XyuVKcTYgSYkeHvxRpbfZ84909757 = XyuVKcTYgSYkeHvxRpbfZ18894532;     XyuVKcTYgSYkeHvxRpbfZ18894532 = XyuVKcTYgSYkeHvxRpbfZ2616254;     XyuVKcTYgSYkeHvxRpbfZ2616254 = XyuVKcTYgSYkeHvxRpbfZ24683831;     XyuVKcTYgSYkeHvxRpbfZ24683831 = XyuVKcTYgSYkeHvxRpbfZ55217265;     XyuVKcTYgSYkeHvxRpbfZ55217265 = XyuVKcTYgSYkeHvxRpbfZ89968912;     XyuVKcTYgSYkeHvxRpbfZ89968912 = XyuVKcTYgSYkeHvxRpbfZ35794869;     XyuVKcTYgSYkeHvxRpbfZ35794869 = XyuVKcTYgSYkeHvxRpbfZ32605683;     XyuVKcTYgSYkeHvxRpbfZ32605683 = XyuVKcTYgSYkeHvxRpbfZ881343;     XyuVKcTYgSYkeHvxRpbfZ881343 = XyuVKcTYgSYkeHvxRpbfZ61681728;     XyuVKcTYgSYkeHvxRpbfZ61681728 = XyuVKcTYgSYkeHvxRpbfZ69289176;     XyuVKcTYgSYkeHvxRpbfZ69289176 = XyuVKcTYgSYkeHvxRpbfZ11617399;     XyuVKcTYgSYkeHvxRpbfZ11617399 = XyuVKcTYgSYkeHvxRpbfZ30091979;     XyuVKcTYgSYkeHvxRpbfZ30091979 = XyuVKcTYgSYkeHvxRpbfZ87513871;     XyuVKcTYgSYkeHvxRpbfZ87513871 = XyuVKcTYgSYkeHvxRpbfZ86754640;     XyuVKcTYgSYkeHvxRpbfZ86754640 = XyuVKcTYgSYkeHvxRpbfZ48286235;     XyuVKcTYgSYkeHvxRpbfZ48286235 = XyuVKcTYgSYkeHvxRpbfZ63358902;     XyuVKcTYgSYkeHvxRpbfZ63358902 = XyuVKcTYgSYkeHvxRpbfZ17469188;     XyuVKcTYgSYkeHvxRpbfZ17469188 = XyuVKcTYgSYkeHvxRpbfZ41881956;     XyuVKcTYgSYkeHvxRpbfZ41881956 = XyuVKcTYgSYkeHvxRpbfZ27559428;     XyuVKcTYgSYkeHvxRpbfZ27559428 = XyuVKcTYgSYkeHvxRpbfZ24726922;     XyuVKcTYgSYkeHvxRpbfZ24726922 = XyuVKcTYgSYkeHvxRpbfZ61032201;     XyuVKcTYgSYkeHvxRpbfZ61032201 = XyuVKcTYgSYkeHvxRpbfZ1526928;     XyuVKcTYgSYkeHvxRpbfZ1526928 = XyuVKcTYgSYkeHvxRpbfZ41343537;     XyuVKcTYgSYkeHvxRpbfZ41343537 = XyuVKcTYgSYkeHvxRpbfZ66940117;     XyuVKcTYgSYkeHvxRpbfZ66940117 = XyuVKcTYgSYkeHvxRpbfZ82238069;     XyuVKcTYgSYkeHvxRpbfZ82238069 = XyuVKcTYgSYkeHvxRpbfZ22996774;     XyuVKcTYgSYkeHvxRpbfZ22996774 = XyuVKcTYgSYkeHvxRpbfZ99766053;     XyuVKcTYgSYkeHvxRpbfZ99766053 = XyuVKcTYgSYkeHvxRpbfZ45222669;     XyuVKcTYgSYkeHvxRpbfZ45222669 = XyuVKcTYgSYkeHvxRpbfZ57296463;     XyuVKcTYgSYkeHvxRpbfZ57296463 = XyuVKcTYgSYkeHvxRpbfZ81744728;     XyuVKcTYgSYkeHvxRpbfZ81744728 = XyuVKcTYgSYkeHvxRpbfZ10851945;     XyuVKcTYgSYkeHvxRpbfZ10851945 = XyuVKcTYgSYkeHvxRpbfZ66423635;     XyuVKcTYgSYkeHvxRpbfZ66423635 = XyuVKcTYgSYkeHvxRpbfZ79062040;     XyuVKcTYgSYkeHvxRpbfZ79062040 = XyuVKcTYgSYkeHvxRpbfZ36819535;     XyuVKcTYgSYkeHvxRpbfZ36819535 = XyuVKcTYgSYkeHvxRpbfZ32237150;     XyuVKcTYgSYkeHvxRpbfZ32237150 = XyuVKcTYgSYkeHvxRpbfZ15311278;     XyuVKcTYgSYkeHvxRpbfZ15311278 = XyuVKcTYgSYkeHvxRpbfZ13284694;     XyuVKcTYgSYkeHvxRpbfZ13284694 = XyuVKcTYgSYkeHvxRpbfZ30976723;     XyuVKcTYgSYkeHvxRpbfZ30976723 = XyuVKcTYgSYkeHvxRpbfZ7924405;     XyuVKcTYgSYkeHvxRpbfZ7924405 = XyuVKcTYgSYkeHvxRpbfZ40308976;     XyuVKcTYgSYkeHvxRpbfZ40308976 = XyuVKcTYgSYkeHvxRpbfZ28276566;     XyuVKcTYgSYkeHvxRpbfZ28276566 = XyuVKcTYgSYkeHvxRpbfZ99114981;     XyuVKcTYgSYkeHvxRpbfZ99114981 = XyuVKcTYgSYkeHvxRpbfZ77242711;     XyuVKcTYgSYkeHvxRpbfZ77242711 = XyuVKcTYgSYkeHvxRpbfZ80501382;     XyuVKcTYgSYkeHvxRpbfZ80501382 = XyuVKcTYgSYkeHvxRpbfZ41118853;     XyuVKcTYgSYkeHvxRpbfZ41118853 = XyuVKcTYgSYkeHvxRpbfZ41148969;     XyuVKcTYgSYkeHvxRpbfZ41148969 = XyuVKcTYgSYkeHvxRpbfZ77701153;     XyuVKcTYgSYkeHvxRpbfZ77701153 = XyuVKcTYgSYkeHvxRpbfZ90733858;     XyuVKcTYgSYkeHvxRpbfZ90733858 = XyuVKcTYgSYkeHvxRpbfZ69318476;     XyuVKcTYgSYkeHvxRpbfZ69318476 = XyuVKcTYgSYkeHvxRpbfZ71323505;     XyuVKcTYgSYkeHvxRpbfZ71323505 = XyuVKcTYgSYkeHvxRpbfZ23921090;     XyuVKcTYgSYkeHvxRpbfZ23921090 = XyuVKcTYgSYkeHvxRpbfZ95433063;}
// Junk Finished
