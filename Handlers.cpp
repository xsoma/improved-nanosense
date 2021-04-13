#include "Gamehooking.hpp"
#include "helpers/Utils.hpp"


#include "Options.hpp"

#include "helpers/Math.hpp"
#include "Install.hpp"
#include "features/Visuals.hpp"
#include "features/Glow.hpp"
#include "features/Miscellaneous.hpp"
#include "features/PredictionSystem.hpp"
#include "features/AngryPew.hpp"
#include "features/LegitPew.h"
#include "features/LagCompensation.hpp"
#include "features/Resolver.hpp"
#include "features/AntiAim.hpp"
#include "features/PlayerHurt.hpp"
#include "features/BulletImpact.hpp"
#include "features/GrenadePrediction.h"
#include "features/ServerSounds.hpp"
#include "features/Skinchanger.hpp"
#include "helpers/bfReader.h"
#include <intrin.h>
#include <experimental/filesystem> // hack
#include "globals/HanaSong.h"
#include "menu/Menu.h"
#include <d3dx9.h>
#include "menu/MenuBackground.h"
#include "menu/TabFontsCreatedHanaSong.h"
#include "menu/MenuFonts.h"
#include "features/Visuals.hpp"
#include "Install.hpp"
#include "helpers/antileak.h"
#include <filesystem>

#pragma warning (disable: 4715) // condition expression is constant

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

namespace fs = std::filesystem;

extern LRESULT vsonyp0wer_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int32_t originalCorrectedFakewalkIdx = 0;
int32_t tickHitPlayer = 0;
int32_t tickHitWall = 0;
int32_t originalShotsMissed = 0;

#ifdef INSTANT_DEFUSE_PLANT_EXPLOIT
int32_t nTickBaseShift = 0;
int32_t nSinceUse = 0;
bool bInSendMove = false, bFirstSendMovePack = false;
#endif

DWORD stream;

namespace Global
{
	char my_documents_folder[MAX_PATH];

	int iHwidIndex = 0;
	bool bIsPanorama = true;

	bool bMenuOpen = true;

	bool left = true;
	bool right = false;
	bool backwards = false;
	int iScreenX;
	int iScreenY;

	float smt = 0.f;
	QAngle visualAngles = QAngle(0.f, 0.f, 0.f);
	bool bSendPacket = false;
	bool bAimbotting = false;
	bool bVisualAimbotting = false;
	QAngle vecVisualAimbotAngs = QAngle(0.f, 0.f, 0.f);
	CUserCmd* userCMD = nullptr;

	float lastdmg[65] = { 0 };

	float realyaw, fakeyaw, realpitch, fakepitch, lasthc, lastawdmg = 0;

	int aimbot_target;
	int aim_hitbox;
	int seed;
	bool hit_while_brute[65] = { false };


	double LastLBYUpdate = 0, NextLBYUpdate = 0;
	int prevChoked = 0;
	int fps = 0;

	char* szLastFunction = "<No function was called>";

	HMODULE hmDll = nullptr;
	HANDLE handleDll = nullptr;

	bool bFakelag = false;
	float flFakewalked = 0.f;
	Vector vecUnpredictedVel = Vector(0, 0, 0);

	float flFakeLatencyAmount = 0.f;
	float flEstFakeLatencyOnServer = 0.f;

	matrix3x4_t traceHitboxbones[128];

	bool bBaim[65] = { false };

	int AimTargets = 0;
	bool last_packet = true;

	std::array<std::string, 64> resolverModes;

	QAngle view_punch_old = QAngle(0, 0, 0);

	INetChannel* netchan = nullptr;

	bool use_ud_vmt = true;

	Vector vecAimpos = Vector(0, 0, 0);

	int PlayersChockedPackets[65] = { 0 };
	Vector FakelagUnfixedPos[65] = { Vector(0, 0, 0) };
	bool FakelagFixed[65] = { false };

	bool bShouldUnload = false;

	std::string currentLUA = "";
}

void __fastcall Handlers::PaintTraverse_h(void *thisptr, void*, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	g_EngineClient->GetScreenSize(Global::iScreenX, Global::iScreenY);
	static uint32_t HudZoomPanel;
	if (!HudZoomPanel)
		if (!strcmp("HudZoom", g_VGuiPanel->GetName(vguiPanel)))
			HudZoomPanel = vguiPanel;
	//XSystemCFG.removals_scope

	if (HudZoomPanel == vguiPanel && XSystemCFG.removals_scope && g_LocalPlayer && g_LocalPlayer->m_hActiveWeapon().Get())
	{
		if (g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() && g_LocalPlayer->m_bIsScoped())
			return;
	}

	o_PaintTraverse(thisptr, vguiPanel, forceRepaint, allowForce);

	static uint32_t FocusOverlayPanel;
	if (!FocusOverlayPanel)
	{
		const char* szName = g_VGuiPanel->GetName(vguiPanel);

		if (lstrcmpA(szName, "MatSystemTopPanel") == 0)
		{
			FocusOverlayPanel = vguiPanel;

			Visuals::InitFont();

			g_EngineClient->ExecuteClientCmd("clear");
			g_CVar->ConsoleColorPrintf(Color(238, 75, 181, 255), "|==============================================|\n");
			g_CVar->ConsoleColorPrintf(Color(238, 75, 181, 255), "|                                              |\n");
			g_CVar->ConsoleColorPrintf(Color(238, 75, 181, 255), "|              im so fucking sad:c             |\n");;
			g_CVar->ConsoleColorPrintf(Color(238, 75, 181, 255), "|                                              |\n");
			g_CVar->ConsoleColorPrintf(Color(238, 75, 181, 255), "|==============================================|\n");


			long res = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, Global::my_documents_folder);
			if (res == S_OK)
			{
				std::string config_folder = std::string(Global::my_documents_folder) + "\\nanosense\\";
				Config::Get().CreateConfigFolder(config_folder);
			}
			Skinchanger::Get().LoadSkins();
		}
	}

	if (FocusOverlayPanel == vguiPanel)
	{

		//g_InputSystem->EnableInput(!Menu::Get().isOpen); /// disable input system

		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && g_LocalPlayer)
		{
			ServerSound::Get().Start();
			for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++)
			{
				C_BasePlayer *entity = C_BasePlayer::GetPlayerByIndex(i);

				if (!entity)
					continue;

				if (i < 65 && Visuals::ValidPlayer(entity))
				{
					if (Visuals::Begin(entity))
					{
						Visuals::RenderFill();
						Visuals::RenderBox();

						if (XSystemCFG.esp_player_weapons) Visuals::RenderWeapon();
						if (XSystemCFG.esp_player_name) Visuals::RenderName();
						if (XSystemCFG.esp_player_health) Visuals::RenderHealth();
						if (XSystemCFG.esp_player_skelet) Visuals::RenderSkelet();
						if (XSystemCFG.esp_backtracked_player_skelet) Visuals::RenderBacktrackedSkelet();					
					}
				}
				else if (XSystemCFG.esp_dropped_weapons && entity->IsWeapon())
					Visuals::RenderWeapon((C_BaseCombatWeapon*)entity);
				else if (entity->IsPlantedC4())
					if (XSystemCFG.esp_planted_c4)
						Visuals::RenderPlantedC4(entity);

				Visuals::RenderNadeEsp((C_BaseCombatWeapon*)entity);
			}
			ServerSound::Get().Finish();

			if (XSystemCFG.removals_scope && (g_LocalPlayer && g_LocalPlayer->m_hActiveWeapon().Get() && g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() && g_LocalPlayer->m_bIsScoped()))
			{

				static float rainbow; // del this if dont work
				rainbow += 0.0005; // del this if dont work

				if (rainbow > 1.f) // del this if dont work
					rainbow = 0.f; // del this if dont work

				Color line_color = Color::FromHSB(rainbow, 1.f, 1.f);

				g_VGuiSurface->DrawSetColor(line_color);
				g_VGuiSurface->DrawLine(Global::iScreenX / 2, 0, Global::iScreenX / 2, Global::iScreenY);
				g_VGuiSurface->DrawLine(0, Global::iScreenY / 2, Global::iScreenX, Global::iScreenY / 2);
			}

			if (XSystemCFG.visuals_others_grenade_pred)
				CCSGrenadeHint::Get().Paint();

			if (XSystemCFG.visuals_others_hitmarker || XSystemCFG.misc_logevents)
				PlayerHurtEvent::Get().Paint();

			if (g_LocalPlayer->IsAlive()) {
				Visuals::RenderSnapline();
				if (XSystemCFG.esp_player_anglelines)
					Visuals::DrawAngleLines();
			}
			Visuals::SpoofZeug();
			if (XSystemCFG.visuals_draw_xhair)
				Visuals::GayPornYes();
		}
		//if (XSystemCFG.visuals_others_watermark)
		Visuals::DrawWatermark();
		Visuals::Indicators();
		if (XSystemCFG.misc_spectatorlist)
			Visuals::RenderSpectatorList();

		if (g_LocalPlayer)
		{
			Visuals::NightMode();
			Visuals::AsusWalls();
		}
	}

	const char* pszPanelName = g_VGuiPanel->GetName(vguiPanel);
	if (!strstr(pszPanelName, "FocusOverlayPanel"))
		return;

	g_VGuiPanel->set_mouse_input_enabled((unsigned int)vguiPanel, Menu::Get().isOpen);


}

bool __stdcall Handlers::CreateMove_h(float smt, CUserCmd *userCMD)
{
	if (!userCMD->command_number || !g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return o_CreateMove(g_ClientMode, smt, userCMD);

	// Update tickbase correction.
	AngryPew::Get().GetTickbase(userCMD);

	QAngle org_angle = userCMD->viewangles;

	uintptr_t *framePtr;
	__asm mov framePtr, ebp;


	Global::smt = smt;
	Global::bFakelag = false;
	Global::bSendPacket = true;
	Global::userCMD = userCMD;
	Global::vecUnpredictedVel = g_LocalPlayer->m_vecVelocity();

	if (XSystemCFG.misc_bhop)
		Miscellaneous::Get().Bhop(userCMD);

	if (XSystemCFG.misc_autostrafe)
		Miscellaneous::Get().AutoStrafe(userCMD);

	if (!g_EngineClient->IsInGame() && !g_EngineClient->IsConnected())
		g_EngineClient->ExecuteClientCmd("crosshair 1");
	else 
		if (XSystemCFG.removals_crosshair)
			g_EngineClient->ExecuteClientCmd("crosshair 0");
		else g_EngineClient->ExecuteClientCmd("crosshair 1");

	QAngle wish_angle = userCMD->viewangles;
	userCMD->viewangles = org_angle;
	if (XSystemCFG.misc_fakewalk)
		AntiAim::Get().Fakewalk(userCMD);

	// -----------------------------------------------
	// Do engine prediction
	PredictionSystem::Get().Start(userCMD, g_LocalPlayer);
	{

		if (XSystemCFG.misc_fakelag_value)
			Miscellaneous::Get().Fakelag(userCMD);
		Miscellaneous::Get().AutoPistol(userCMD);
		Miscellaneous::Get().AldaVerdammteScheisse(userCMD);
		LegitPew::Get().Work(userCMD);
		AngryPew::Get().Work(userCMD);
		Miscellaneous::Get().AntiAim(userCMD);
		Miscellaneous::Get().FixMovement(userCMD, wish_angle);
//		AntiAim::Get().PastedLBYBreaker(userCMD);
		AntiAim::Get().Fakewalk(userCMD);
	}
	PredictionSystem::Get().End(g_LocalPlayer);
	CCSGrenadeHint::Get().Tick(userCMD->buttons);

	if (XSystemCFG.angrpwenabled && Global::bAimbotting && userCMD->buttons & IN_ATTACK)
		*(bool*)(*framePtr - 0x1C) = false;

	if (XSystemCFG.hvh_disable_antiut)
	{
		//userCMD->viewangles.pitch = Math::FindSmallestFake2(userCMD->viewangles.pitch, Utils::RandomInt(0, 3));
	}

	if (!XSystemCFG.hvh_disable_antiut)
	{
		//userCMD->forwardmove = Miscellaneous::Get().clamp(userCMD->forwardmove, -450.f, 450.f);
		//userCMD->sidemove = Miscellaneous::Get().clamp(userCMD->sidemove, -450.f, 450.f);
		//userCMD->upmove = Miscellaneous::Get().clamp(userCMD->upmove, -320.f, 320.f);
		//userCMD->viewangles.Clamp();
	}

	*(bool*)(*framePtr - 0x1C) = Global::bSendPacket;

	if (XSystemCFG.hvh_show_real_angles == 0)
	{
		if (!Global::bSendPacket)
			Global::visualAngles = userCMD->viewangles;
	}
	else if(Global::bSendPacket)
		Global::visualAngles = userCMD->viewangles;

	static float real_yaw = 0, real_pitch = 0, next_yaw = 0, next_pitch = 0;

	if (Global::bSendPacket && Global::last_packet)
	{
		Global::fakeyaw = userCMD->viewangles.yaw;
		Global::realyaw = userCMD->viewangles.yaw;

		if (XSystemCFG.hvh_disable_antiut)
		{
			Global::fakepitch = Math::ClampPitch(userCMD->viewangles.pitch);
			Global::realpitch = Math::ComputeBodyPitch(userCMD->viewangles.pitch);
		}
		else
		{
			Global::fakepitch = userCMD->viewangles.pitch;
			Global::realpitch = userCMD->viewangles.pitch;
		}
	}
	else if (Global::bSendPacket)
	{
		Global::fakeyaw = userCMD->viewangles.yaw;

		if (XSystemCFG.hvh_disable_antiut)
		{
			Global::fakepitch = Math::ClampPitch(userCMD->viewangles.pitch);
		}
		else
		{
			Global::fakepitch = userCMD->viewangles.pitch;
		}
	}
	else if (Global::last_packet)
	{
		real_yaw = next_yaw;
		real_pitch = next_pitch;
		next_yaw = userCMD->viewangles.yaw;

		if (XSystemCFG.hvh_disable_antiut)
		{
			next_pitch = (Math::ComputeBodyPitch(userCMD->viewangles.pitch)) * ((Global::fakepitch < 0) ? (-1) : (1));
		}
		else
		{
			next_pitch = userCMD->viewangles.pitch;
		}
	}

	Global::realyaw = real_yaw;
	Global::realpitch = real_pitch;
	if (!XSystemCFG.hvh_disable_antiut) Global::realpitch = Global::fakepitch;

	if (XSystemCFG.hvh_show_real_angles) Global::visualAngles = QAngle(Global::realpitch, Global::realyaw, 0);

	else Global::visualAngles = QAngle(Global::fakepitch, Global::fakeyaw, 0);

	userCMD->forwardmove = Miscellaneous::Get().clamp(userCMD->forwardmove, -450.f, 450.f);
	userCMD->sidemove = Miscellaneous::Get().clamp(userCMD->sidemove, -450.f, 450.f);
	userCMD->upmove = Miscellaneous::Get().clamp(userCMD->upmove, -320.f, 320.f);
	userCMD->viewangles.Clamp();

	if (!XSystemCFG.angrpwsilent && Global::bVisualAimbotting)
		g_EngineClient->SetViewAngles(Global::vecVisualAimbotAngs);

	if (!o_TempEntities)
	{
		g_pClientStateHook->Setup((uintptr_t*)((uintptr_t)g_ClientState + 0x8));
		g_pClientStateHook->Hook(36, Handlers::TempEntities_h);
		o_TempEntities = g_pClientStateHook->GetOriginal<TempEntities_t>(36);
	}

	return false;
}
void __fastcall Handlers::LockCursor_h(void* ecx, void*)
{

	if (menuOpen) // this might be a retard moment from me
	{
		o_UnlockCursor(ecx);
		if (XSystemCFG.unload) {
			Installer::UnLoadvsonyp0wer();
		}
	}
	else
	{
		o_LockCursor(ecx);
		if (XSystemCFG.unload) {
			o_UnlockCursor(ecx);
			Installer::UnLoadvsonyp0wer();
		}
			
	}
}
void __stdcall Handlers::FrameStageNotify_h(ClientFrameStage_t stage)
{
	g_LocalPlayer = C_BasePlayer::GetLocalPlayer(true);

	if (!g_LocalPlayer || !g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return o_FrameStageNotify(stage);

	QAngle aim_punch_old;
	QAngle view_punch_old;

	QAngle *aim_punch = nullptr;
	QAngle *view_punch = nullptr;

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		if (XSystemCFG.skinchanger_enabled)
			Skinchanger::Get().Work();

		Miscellaneous::Get().PunchAngleFix_FSN();
	}

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END)
	{
		for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
		{
			C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

			if (!player)
				continue;

			if (player == g_LocalPlayer)
				continue;

			if (!player->IsAlive())
				continue;

			if (player->IsTeamMate())
				continue;

			VarMapping_t *map = player->VarMapping();
			if (map)
			{
				for (int j = 0; j < map->m_nInterpolatedEntries; j++)
				{
					map->m_Entries[j].m_bNeedsToInterpolate = !XSystemCFG.angrpwaccboost > 0;
				}
			}
		}

		if (XSystemCFG.hvh_resolver)
			Resolver::Get().Resolve();

		//if (XSystemCFG.hvh_resolver_custom)
			//Resolver::Get().ResolveCustom();
	}

	if (stage == ClientFrameStage_t::FRAME_RENDER_START)
	{
		*(bool*)Offsets::bOverridePostProcessingDisable = XSystemCFG.removals_postprocessing;

		if (g_LocalPlayer->IsAlive())
		{
			static ConVar *default_skyname = g_CVar->FindVar("sv_skyname");
			static int iOldSky = 0;

			if (iOldSky != XSystemCFG.visuals_others_sky)
			{
				Utils::LoadNamedSkys(XSystemCFG.visuals_others_sky == 0 ? default_skyname->GetString() : opt_Skynames[XSystemCFG.visuals_others_sky]);
				iOldSky = XSystemCFG.visuals_others_sky;
			}

			if (XSystemCFG.removals_novisualrecoil)
			{
				aim_punch = &g_LocalPlayer->m_aimPunchAngle();
				view_punch = &g_LocalPlayer->m_viewPunchAngle();

				aim_punch_old = *aim_punch;
				view_punch_old = *view_punch;

				*aim_punch = QAngle(0.f, 0.f, 0.f);
				*view_punch = QAngle(0.f, 0.f, 0.f);
			}

			if (g_Input->m_fCameraInThirdPerson)
				g_LocalPlayer->visuals_Angles() = Global::visualAngles;

			if (XSystemCFG.removals_smoke)
				*(int*)Offsets::smokeCount = 0;

			for (int i = 1; i <= g_GlobalVars->maxClients; i++)
			{
				if (i == g_EngineClient->GetLocalPlayer()) continue;

				IClientEntity* pCurEntity = g_EntityList->GetClientEntity(i);
				if (!pCurEntity) continue;

				*(int*)((uintptr_t)pCurEntity + 0xA30) = g_GlobalVars->framecount; //we'll skip occlusion checks now
				*(int*)((uintptr_t)pCurEntity + 0xA28) = 0;//clear occlusion flags
			}
		}

		if (XSystemCFG.removals_flash && g_LocalPlayer)
			if (g_LocalPlayer->m_flFlashDuration() > 0.f)
				g_LocalPlayer->m_flFlashDuration() = 0.f;
	}

	o_FrameStageNotify(stage);

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_END)
	{

		if (XSystemCFG.angrpwaccboost > 0)
			CMBacktracking::Get().FrameUpdatePostEntityThink();
	}

	if (stage == ClientFrameStage_t::FRAME_RENDER_START)
	{
		if (g_LocalPlayer && g_LocalPlayer->IsAlive())
		{
			if (XSystemCFG.removals_novisualrecoil && (aim_punch && view_punch))
			{
				*aim_punch = aim_punch_old;
				*view_punch = view_punch_old;
			}
		}
	}
}

bool __fastcall Handlers::FireEventClientSide_h(void *thisptr, void*, IGameEvent *gEvent)
{
	if (!gEvent)
		return o_FireEventClientSide(thisptr, gEvent);

	if (strcmp(gEvent->GetName(), "game_newmap") == 0)
	{
		static ConVar *default_skyname = g_CVar->FindVar("sv_skyname");
		Utils::LoadNamedSkys(XSystemCFG.visuals_others_sky == 0 ? default_skyname->GetString() : opt_Skynames[XSystemCFG.visuals_others_sky]);
	}

	return o_FireEventClientSide(thisptr, gEvent);
}

void __fastcall Handlers::BeginFrame_h(void *thisptr, void*, float ft)
{
	Miscellaneous::Get().NameChanger();
	Miscellaneous::Get().ChatSpamer();
	Miscellaneous::Get().ClanTag();
	BulletImpactEvent::Get().Paint();

	o_BeginFrame(thisptr, ft);
}

void __fastcall Handlers::SetKeyCodeState_h(void* thisptr, void* EDX, ButtonCode_t code, bool bDown)
{
	if (input_shouldListen && bDown)
	{
		input_shouldListen = false;
		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return o_SetKeyCodeState(thisptr, code, bDown);
}

void __fastcall Handlers::SetMouseCodeState_h(void* thisptr, void* EDX, ButtonCode_t code, MouseCodeState_t state)
{
	if (input_shouldListen && state == BUTTON_PRESSED)
	{
		input_shouldListen = false;
		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return o_SetMouseCodeState(thisptr, code, state);
}

void __stdcall Handlers::OverrideView_h(CViewSetup* pSetup)
{
	if (XSystemCFG.removals_zoom)
	{
		if (g_EngineClient->IsConnected() && g_EngineClient->IsInGame())
		{
			if (g_LocalPlayer->IsAlive())
			{
				if (g_LocalPlayer->m_bIsScoped())
				{
					if (!g_LocalPlayer->m_hActiveWeapon().Get())
						return;

					if (g_LocalPlayer->m_hActiveWeapon().Get()->m_zoomLevel() != 2)
						pSetup->fov = 90 + XSystemCFG.visuals_others_player_fov;
					else pSetup->fov = 50 + XSystemCFG.visuals_others_player_fov; // to still zoom in a bit when scoping twice
				}
				else pSetup->fov = 90 + XSystemCFG.visuals_others_player_fov;
			}
		}
	}
	else pSetup->fov += XSystemCFG.visuals_others_player_fov;

	o_OverrideView(pSetup);

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
	{
		if (g_LocalPlayer)
		{
			CCSGrenadeHint::Get().View();

			Miscellaneous::Get().ThirdPerson();
		}
	}
}

void Proxies::didSmokeEffect(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	if (XSystemCFG.removals_smoke)
	{
		if (XSystemCFG.removals_smoke_type == 0)
			*(bool*)((DWORD)pOut + 0x1) = true;

		std::vector<const char*> wireframesmoke_mats =
		{
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
		};

		if (XSystemCFG.removals_smoke_type == 1)
		{
			for (auto smoke_mat : wireframesmoke_mats)
			{
				IMaterial* mat = g_MatSystem->FindMaterial(smoke_mat, TEXTURE_GROUP_OTHER);
				mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
			}
		}
	}
	else
	{
		std::vector<const char*> wireframesmoke_mats =
		{
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
		};

		if (XSystemCFG.removals_smoke_type == 1)
		{
			for (auto smoke_mat : wireframesmoke_mats)
			{
				IMaterial* mat = g_MatSystem->FindMaterial(smoke_mat, TEXTURE_GROUP_OTHER);
				mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
			}
		}
	}

	o_didSmokeEffect(pData, pStruct, pOut);
}

bool __stdcall Handlers::InPrediction_h()
{
	return o_OriginalInPrediction(g_Prediction);
}

bool __fastcall Handlers::SetupBones_h(void* ECX, void* EDX, matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	// Supposed to only setupbones tick by tick, instead of frame by frame.
	if (XSystemCFG.angrpwaccboost > 0)
	{
		if (ECX && ((IClientRenderable*)ECX)->GetIClientUnknown())
		{
			IClientNetworkable* pNetworkable = ((IClientRenderable*)ECX)->GetIClientUnknown()->GetClientNetworkable();
			if (pNetworkable && pNetworkable->GetClientClass() && pNetworkable->GetClientClass()->m_ClassID == ClassId::ClassId_CCSPlayer)
			{
				static auto host_timescale = g_CVar->FindVar(("host_timescale"));
				auto player = (C_BasePlayer*)ECX;
				float OldCurTime = g_GlobalVars->curtime;
				float OldRealTime = g_GlobalVars->realtime;
				float OldFrameTime = g_GlobalVars->frametime;
				float OldAbsFrameTime = g_GlobalVars->absoluteframetime;
				float OldAbsFrameTimeStart = g_GlobalVars->absoluteframestarttimestddev;
				float OldInterpAmount = g_GlobalVars->interpolation_amount;
				int OldFrameCount = g_GlobalVars->framecount;
				int OldTickCount = g_GlobalVars->tickcount;

				g_GlobalVars->curtime = player->m_flSimulationTime();
				g_GlobalVars->realtime = player->m_flSimulationTime();
				g_GlobalVars->frametime = g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
				g_GlobalVars->absoluteframetime = g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
				g_GlobalVars->absoluteframestarttimestddev = player->m_flSimulationTime() - g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
				g_GlobalVars->interpolation_amount = 0;
				g_GlobalVars->framecount = TIME_TO_TICKS(player->m_flSimulationTime());
				g_GlobalVars->tickcount = TIME_TO_TICKS(player->m_flSimulationTime());

				*(int*)((int)player + 236) |= 8; // IsNoInterpolationFrame
				bool ret_value = o_SetupBones(player, pBoneToWorldOut, nMaxBones, boneMask, g_GlobalVars->curtime);
				*(int*)((int)player + 236) &= ~8; // (1 << 3)

				g_GlobalVars->curtime = OldCurTime;
				g_GlobalVars->realtime = OldRealTime;
				g_GlobalVars->frametime = OldFrameTime;
				g_GlobalVars->absoluteframetime = OldAbsFrameTime;
				g_GlobalVars->absoluteframestarttimestddev = OldAbsFrameTimeStart;
				g_GlobalVars->interpolation_amount = OldInterpAmount;
				g_GlobalVars->framecount = OldFrameCount;
				g_GlobalVars->tickcount = OldTickCount;
				return ret_value;
			}
		}
	}
	return o_SetupBones(ECX, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}

const char* GetChamsMaterial(int type)
{
	switch (type)
	{
		case 0:
			return "debug/debugambientcube";
		case 1:
			return "models/inventory_items/trophy_majors/gold";
		case 2:
			return "models/inventory_items/cologne_prediction/cologne_prediction_glass";
		case 3:
			return "models/inventory_items/trophy_majors/crystal_clear";
		case 4:
			return "models/gibs/glass/glass";
		case 5:
			return "models/inventory_items/trophy_majors/gloss";
		case 6:
			return "models/inventory_items/trophy_majors/velvet";
		case 7:
			return "models/inventory_items/dogtags/dogtags_outline";
		case 8:
			return "models/effects/cube_white";
		case 9:
			return "models/skeet_glow";
	}
}

void __fastcall Handlers::SceneEnd_h(void* thisptr, void* edx)
{
	if (!g_LocalPlayer || !g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return o_SceneEnd(thisptr);

	o_SceneEnd(thisptr);

		constexpr float color_gray[4] = { 166, 167, 169, 255 };
		QAngle OrigAng;
		IMaterial* local_mat = nullptr;

		if (GetChamsMaterial(XSystemCFG.esp_localplayer_chams_type) == "debug/debugambientcube")
		{
		local_mat = g_MatSystem->FindMaterial(GetChamsMaterial(XSystemCFG.esp_localplayer_chams_type), TEXTURE_GROUP_OTHER);
		}
		else 
		{
		local_mat = g_MatSystem->FindMaterial(GetChamsMaterial(XSystemCFG.esp_localplayer_chams_type), TEXTURE_GROUP_MODEL);
		}
		IMaterial *localfake_mat = g_MatSystem->FindMaterial(GetChamsMaterial(XSystemCFG.esp_localplayer_fakechams_type), TEXTURE_GROUP_MODEL);

		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && !g_LocalPlayer->IsDormant())
		{
			if (XSystemCFG.esp_localplayer_chams)
			{
				if (XSystemCFG.esp_localplayer_chams_xyz)
				{	// XQZ Chams
					if (XSystemCFG.fake_chams)
					{
						// Fake
						QAngle OrigAng;
						OrigAng = g_LocalPlayer->m_angEyeAngles();

						g_LocalPlayer->SetFakeAngle(QAngle(0, g_LocalPlayer->m_angEyeAngles().yaw, 0));
						g_RenderView->SetColorModulation(XSystemCFG.esp_localplayer_fakechams_color_invisible);
						localfake_mat->IncrementReferenceCount();
						localfake_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						g_MdlRender->ForcedMaterialOverride(localfake_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);

						g_RenderView->SetColorModulation(XSystemCFG.esp_localplayer_fakechams_color_visible);
						localfake_mat->IncrementReferenceCount();
						localfake_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
						g_MdlRender->ForcedMaterialOverride(localfake_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
						//g_LocalPlayer->SetFakeAngle(OrigAng);	//Really don't understand what for this needed, just for breaking textures???

						// Real

						//What the fuck is this??? Drawing real chams while fake bool is true???

						/*g_LocalPlayer->SetAbsAngles(QAngle(0, g_LocalPlayer->m_flLowerBodyYawTarget(), 0));	//Already set xD
						g_RenderView->SetColorModulation(XSystemCFG.esp_localplayer_chams_color_invisible);
						local_mat->IncrementReferenceCount();
						local_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						g_MdlRender->ForcedMaterialOverride(local_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);

						g_RenderView->SetColorModulation(XSystemCFG.esp_localplayer_chams_color_visible);
						local_mat->IncrementReferenceCount();
						local_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
						g_MdlRender->ForcedMaterialOverride(local_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
						g_LocalPlayer->SetFakeAngle(OrigAng);*/	//this shit will not work bcuz of it's logic
					}
					else
					{ // Local Player XQZ
						// invisible
						g_RenderView->SetColorModulation(g_LocalPlayer->m_bGunGameImmunity() ? color_gray : XSystemCFG.esp_localplayer_chams_color_invisible);
						local_mat->IncrementReferenceCount();
						local_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						g_MdlRender->ForcedMaterialOverride(local_mat);
						g_LocalPlayer->DrawModel(0x1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);

						// visible
						g_RenderView->SetColorModulation(g_LocalPlayer->m_bGunGameImmunity() ? color_gray : XSystemCFG.esp_localplayer_chams_color_visible);
						local_mat->IncrementReferenceCount();
						local_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
						g_MdlRender->ForcedMaterialOverride(local_mat);
						g_LocalPlayer->DrawModel(0x1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
					}
				}
				else
				{	// non-xqz chams
					if (XSystemCFG.fake_chams)
					{
						// Fake
						QAngle OrigAng;
						OrigAng = g_LocalPlayer->m_angEyeAngles();
						g_LocalPlayer->SetFakeAngle(QAngle(0, g_LocalPlayer->m_angEyeAngles().yaw, 0));
						g_RenderView->SetColorModulation(XSystemCFG.esp_localplayer_fakechams_color_visible);
						g_MdlRender->ForcedMaterialOverride(localfake_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);

						//g_LocalPlayer->SetFakeAngle(OrigAng);	//Really don't understand what for this needed

						// Real

						//Facepalm, why this should really needed?

						/*g_LocalPlayer->SetAbsAngles(QAngle(0, g_LocalPlayer->m_flLowerBodyYawTarget(), 0));	//Already set xD
						g_RenderView->SetColorModulation(XSystemCFG.esp_localplayer_chams_color_visible);
						g_MdlRender->ForcedMaterialOverride(local_mat);
						g_LocalPlayer->DrawModel(1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
						g_LocalPlayer->SetAbsAngles(OrigAng);*/ //this shit will not work bcuz of it's logic
					}
					else
					{
						// Local Player
						g_RenderView->SetColorModulation(XSystemCFG.esp_localplayer_chams_color_visible);
						g_MdlRender->ForcedMaterialOverride(local_mat);
						g_LocalPlayer->DrawModel(0x1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
					}
				}
			}
		}

		if (XSystemCFG.esp_player_chams)
		{
			int chams_material = 0;
			switch (XSystemCFG.esp_player_chams_type)
			{
			case 2:
				chams_material = 1;
				break;
			case 3:
				chams_material = 2;
				break;
			case 4:
				chams_material = 3;
				break;
			case 5:
				chams_material = 4;
				break;
			case 6:
				chams_material = 5;
				break;
			case 7:
				chams_material = 6;
				break;
			case 8:
				chams_material = 7;
				break;
			case 9:
				chams_material = 8;
				break;
			}

			constexpr float color_gray[4] = { 166, 167, 169, 255 };
			IMaterial *mat = g_MatSystem->FindMaterial(GetChamsMaterial(chams_material), TEXTURE_GROUP_MODEL);

			if (!mat || mat->IsErrorMaterial())
				return;

			for (int i = 1; i < g_GlobalVars->maxClients; ++i) {
				auto ent = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

				if (ent && ent->IsAlive() && !ent->IsDormant() && ent != g_LocalPlayer) {

					if (XSystemCFG.esp_enemies_only && ent->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
						continue;

					if (XSystemCFG.esp_player_chams_type >= 1)
					{	// XQZ Chams
						g_RenderView->SetColorModulation(ent->m_bGunGameImmunity() ? color_gray : (ent->m_iTeamNum() == 2 ? XSystemCFG.esp_player_chams_color_t : XSystemCFG.esp_player_chams_color_ct));

						mat->IncrementReferenceCount();
						mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

						g_MdlRender->ForcedMaterialOverride(mat);

						ent->DrawModel(0x1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);

						g_RenderView->SetColorModulation(ent->m_bGunGameImmunity() ? color_gray : (ent->m_iTeamNum() == 2 ? XSystemCFG.esp_player_chams_color_t_visible : XSystemCFG.esp_player_chams_color_ct_visible));

						mat->IncrementReferenceCount();
						mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

						g_MdlRender->ForcedMaterialOverride(mat);

						ent->DrawModel(0x1, 255);
						g_MdlRender->ForcedMaterialOverride(nullptr);
					}
					else
					{	// Normal Chams
						g_RenderView->SetColorModulation(ent->m_iTeamNum() == 2 ? XSystemCFG.esp_player_chams_color_t_visible : XSystemCFG.esp_player_chams_color_ct_visible);

						g_MdlRender->ForcedMaterialOverride(mat);

						ent->DrawModel(0x1, 255);

						g_MdlRender->ForcedMaterialOverride(nullptr);
					}
				}
			}
		}

		if (g_EngineClient->IsConnected() && g_EngineClient->IsInGame())
			if (g_LocalPlayer->IsAlive())
				if (XSystemCFG.esp_localplayer_viewmodel_chams && XSystemCFG.esp_localplayer_chams) {
					IMaterial *Viewmodel = g_MatSystem->FindMaterial(GetChamsMaterial(XSystemCFG.esp_localplayer_viewmodel_materials), TEXTURE_GROUP_CLIENT_EFFECTS);
					g_MdlRender->ForcedMaterialOverride(Viewmodel);
				}

	if (XSystemCFG.glow_enabled)
		Glow::Get().Run();
}

void __stdcall FireBullets_PostDataUpdate(C_TEFireBullets *thisptr, DataUpdateType_t updateType)
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return o_FireBullets(thisptr, updateType);

	if (XSystemCFG.angrpwlagcompensation && thisptr)
	{
		int iPlayer = thisptr->m_iPlayer + 1;
		if (iPlayer < 64)
		{
			auto player = C_BasePlayer::GetPlayerByIndex(iPlayer);
			
			if (player && player != g_LocalPlayer && !player->IsDormant() && !player->IsTeamMate())
			{
				QAngle eyeAngles = QAngle(thisptr->m_vecAngles.pitch, thisptr->m_vecAngles.yaw, thisptr->m_vecAngles.roll);
				QAngle calcedAngle = Math::CalcAngle(player->GetEyePos(), g_LocalPlayer->GetEyePos());
				
				thisptr->m_vecAngles.pitch = calcedAngle.pitch;
				thisptr->m_vecAngles.yaw = calcedAngle.yaw;
				thisptr->m_vecAngles.roll = 0.f;

				float
					event_time = g_GlobalVars->tickcount,
					player_time = player->m_flSimulationTime();

				// Extrapolate tick to hit scouters etc
				auto lag_records = CMBacktracking::Get().m_LagRecord[iPlayer];

				float shot_time = TICKS_TO_TIME(event_time);
				for (auto& record : lag_records)
				{
					if (record.m_iTickCount <= event_time)
					{
						shot_time = record.m_flSimulationTime + TICKS_TO_TIME(event_time - record.m_iTickCount); // also get choked from this
						break;
					}
				}
				CMBacktracking::Get().SetOverwriteTick(player, calcedAngle, shot_time, 1);
			}
		}
	}

	o_FireBullets(thisptr, updateType);
}

__declspec (naked) void __stdcall Handlers::TEFireBulletsPostDataUpdate_h(DataUpdateType_t updateType)
{
	__asm
	{
		push[esp + 4]
		push ecx
		call FireBullets_PostDataUpdate
		retn 4
	}
}

bool __fastcall Handlers::TempEntities_h(void* ECX, void* EDX, void* msg)
{
	if (!g_LocalPlayer || !g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return o_TempEntities(ECX, msg);

	bool ret = o_TempEntities(ECX, msg);

	auto CL_ParseEventDelta = [](void *RawData, void *pToData, RecvTable *pRecvTable)
	{
		// "RecvTable_DecodeZeros: table '%s' missing a decoder.", look at the function that calls it. Actually useless.
		static uintptr_t CL_ParseEventDeltaF = (uintptr_t)Utils::PatternScan(GetModuleHandle("engine.dll"), ("55 8B EC 83 E4 F8 53 57"));
	};

	// Filtering events
	if (!XSystemCFG.angrpwlagcompensation || !g_LocalPlayer->IsAlive())
		return ret;

	CEventInfo *ei = g_ClientState->events;
	CEventInfo *next = NULL;

	if (!ei)
		return ret;

	do
	{
		next = *(CEventInfo**)((uintptr_t)ei + 0x38);

		uint16_t classID = ei->classID - 1;

		auto m_pCreateEventFn = ei->pClientClass->m_pCreateEventFn; // ei->pClientClass->m_pCreateEventFn ptr
		if (!m_pCreateEventFn)
			continue;

		IClientNetworkable *pCE = m_pCreateEventFn();
		if (!pCE)
			continue;

		if (classID == ClassId::ClassId_CTEFireBullets)
		{
			// set fire_delay to zero to send out event so its not here later.
			ei->fire_delay = 0.0f;
		}
		ei = next;
	} while (next != NULL);

	return ret;
}

float __fastcall Handlers::GetViewModelFov_h(void* ECX, void* EDX)
{
	return XSystemCFG.visuals_others_player_fov_viewmodel + o_GetViewmodelFov(ECX);
}

bool __fastcall Handlers::GetBool_SVCheats_h(PVOID pConVar, int edx)
{
	// xref : "Pitch: %6.1f   Yaw: %6.1f   Dist: %6.1f %16s"
	static DWORD CAM_THINK = (DWORD)Utils::PatternScan(GetModuleHandle("client.dll"), "85 C0 75 30 38 86");
	if (!pConVar)
		return false;

	if (XSystemCFG.misc_thirdperson)
	{
		if ((DWORD)_ReturnAddress() == CAM_THINK)
			return true;
	}

	return o_GetBool(pConVar);
}

void __fastcall Handlers::RunCommand_h(void* ECX, void* EDX, C_BasePlayer* player, CUserCmd* cmd, IMoveHelper* helper)
{
	o_RunCommand(ECX, player, cmd, helper);

	Miscellaneous::Get().PunchAngleFix_RunCommand(player);
}

int __fastcall Handlers::SendDatagram_h(void *ECX, void *EDX, bf_write *data)
{
	INetChannel *net_channel = (INetChannel*)ECX;

	int32_t reliable_state = net_channel->m_nInReliableState;
	int32_t sequencenr = net_channel->m_nInSequenceNr;

	int ret = o_SendDatagram(net_channel, data);

	net_channel->m_nInReliableState = reliable_state;
	net_channel->m_nInSequenceNr = sequencenr;

	return ret;
}

int __stdcall Handlers::IsBoxVisible_h(const Vector &mins, const Vector &maxs)
{
	if (!memcmp(_ReturnAddress(), "\x85\xC0\x74\x2D\x83\x7D\x10\x00\x75\x1C", 10))
		return 1;

	return o_IsBoxVisible(mins, maxs);
}

bool __fastcall Handlers::IsHLTV_h(void *ECX, void *EDX)
{
	uintptr_t player;
	__asm
	{
		mov player, edi
	}

	if ((DWORD)_ReturnAddress() != Offsets::reevauluate_anim_lod)
		return o_IsHLTV(ECX);

	if (!player || player == 0x000FFFF)
		return o_IsHLTV(ECX);

	*(int32_t*)(player + 0xA24) = -1;
	*(int32_t*)(player + 0xA2C) = *(int32_t*)(player + 0xA28);
	*(int32_t*)(player + 0xA28) = 0;

	return true;
}





void OpenMenu()
{
	static bool is_down = false;
	static bool is_clicked = false;
	if (GetAsyncKeyState(VK_INSERT))
	{
		is_clicked = false;
		is_down = true;
	}
	else if (!GetAsyncKeyState(VK_INSERT) && is_down)
	{
		is_clicked = true;
		is_down = false;
	}
	else
	{
		is_clicked = false;
		is_down = false;
	}

	if (is_clicked)
	{
		Menu::Get().isOpen = !Menu::Get().isOpen;

	}
}
bool PressedKeys[256] = {};



LRESULT idirect3ddevice9::wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		PressedKeys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		PressedKeys[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		PressedKeys[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		PressedKeys[VK_RBUTTON] = false;
		break;
	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		PressedKeys[VK_MBUTTON] = true;
		break;
	case WM_MBUTTONUP:
		PressedKeys[VK_MBUTTON] = false;
		break;
	case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
	{
		UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			PressedKeys[VK_XBUTTON1] = true;
		}
		else if (button == XBUTTON2)
		{
			PressedKeys[VK_XBUTTON2] = true;
		}
		break;
	}
	case WM_XBUTTONUP:
	{
		UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			PressedKeys[VK_XBUTTON1] = false;
		}
		else if (button == XBUTTON2)
		{
			PressedKeys[VK_XBUTTON2] = false;
		}
		break;
	}
	case WM_KEYDOWN:
		PressedKeys[wParam] = true;
		break;
	case WM_KEYUP:
		PressedKeys[wParam] = false;
		break;
	default: break;
	}

	OpenMenu();

	if (Menu::Get().d3d9 && Menu::Get().isOpen && vsonyp0wer_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) && !input_shouldListen)
		return true;

	return CallWindowProc(idirect3ddevice9::Get().original_proc, hWnd, uMsg, wParam, lParam);
}
#include "vfunct/vfuncthook.h"
#include "Handlers.h"

vfunc_hook direct3d_hook;


void idirect3ddevice9::ini(IDirect3DDevice9* fix)
{

	

	D3DDEVICE_CREATION_PARAMETERS params;
	fix->GetCreationParameters(&params);
	idirect3ddevice9::Get().window = params.hFocusWindow;

	vsonyp0wer::CreateContext();
	vsonyp0wer_ImplWin32_Init(idirect3ddevice9::Get().window);
	vsonyp0wer_ImplDX9_Init(fix);
	if (window) original_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndproc)));
	


	direct3d_hook.setup(fix);
	direct3d_hook.hook_index(16, reset);
	direct3d_hook.hook_index(17, end_scene);

}
IDirect3DDevice9* idirect3ddevice9::direct_device()
{
    static const auto direct_device = **reinterpret_cast<IDirect3DDevice9 * **>(Utils::PatternScan("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
	//static const auto direct_device = **reinterpret_cast<IDirect3DDevice9 * **>(Utils::PatternScan("shaderapidx9.dll", "A1 ? ? ? ? 6A 00 53") + 1);

	//A1 ? ? ? ? 6A 00 53

	return direct_device;
}
HRESULT idirect3ddevice9::reset(IDirect3DDevice9* dev, D3DPRESENT_PARAMETERS* param)
{
	static auto oReset = direct3d_hook.get_original<decltype(&reset)>(16);
	vsonyp0wer_ImplDX9_InvalidateDeviceObjects();
	auto hr = oReset(dev, param);
	if (hr >= 0)
		vsonyp0wer_ImplDX9_CreateDeviceObjects();
	return hr;
}
typedef long(__stdcall* fnPresent)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);


HRESULT idirect3ddevice9::end_scene(IDirect3DDevice9* pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
	static auto oPresent = direct3d_hook.get_original<fnPresent>(17);
	IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
	pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state);
	pDevice->GetVertexDeclaration(&vertDec);
	pDevice->GetVertexShader(&vertShader);
	static auto wanted_ret_address = _ReturnAddress();
	if (_ReturnAddress() == wanted_ret_address)
	{
		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		if (!Menu::Get().d3d9)
		{
			IDirect3DSwapChain9* pChain = nullptr;
			D3DPRESENT_PARAMETERS pp = {};
			D3DDEVICE_CREATION_PARAMETERS param = {};
			pDevice->GetCreationParameters(&param);
			pDevice->GetSwapChain(0, &pChain);
			if (pChain)
				pChain->GetPresentParameters(&pp);
			vsonyp0wer_ImplWin32_Init(param.hFocusWindow);
			vsonyp0wer_ImplDX9_Init(pDevice);


			ImFontConfig font_config;
			font_config.OversampleH = 1; //or 2 is the same
			font_config.OversampleV = 1;
			font_config.PixelSnapH = 1;
			static const ImWchar ranges[] =
			{
				0x0020, 0x00FF, // Basic Latin + Latin Supplement
				0x0400, 0x044F, // Cyrillic
				0,
			};
			vsonyp0werIO& io = vsonyp0wer::GetIO();

			globals::tabFont = io.Fonts->AddFontFromMemoryTTF(&nanosense, sizeof nanosense, 50.0f, &font_config, ranges);
			globals::boldMenuFont = vsonyp0wer::GetIO().Fonts->AddFontFromMemoryCompressedTTF(verdanab_compressed_data, verdanab_compressed_size, 11, NULL, vsonyp0wer::GetIO().Fonts->GetGlyphRangesCyrillic());
			globals::menuFont = vsonyp0wer::GetIO().Fonts->AddFontFromMemoryCompressedTTF(verdana_compressed_data, verdana_compressed_size, 11, NULL, vsonyp0wer::GetIO().Fonts->GetGlyphRangesCyrillic());
			globals::controlFont = vsonyp0wer::GetIO().Fonts->AddFontFromMemoryCompressedTTF(comboarrow_compressed_data, comboarrow_compressed_size, 12, NULL, vsonyp0wer::GetIO().Fonts->GetGlyphRangesCyrillic());
			globals::weaponFont = io.Fonts->AddFontFromMemoryTTF(&WeaponFont, sizeof WeaponFont, 26.0f, &font_config, ranges);
			globals::weaponzFont = io.Fonts->AddFontFromMemoryTTF(&WeaponzFont, sizeof WeaponzFont, 26.0f, &font_config, ranges);

			//	forTabs_text = vsonyp0wer::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Ubuntu_compressed_data, Ubuntu_compressed_size, 15, NULL, vsonyp0wer::GetIO().Fonts->GetGlyphRangesCyrillic());


			if (globals::menuBg == nullptr)D3DXCreateTextureFromFileInMemoryEx(pDevice
				, &menuBackground2, sizeof(menuBackground2),
				1000, 1000, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, NULL, NULL, &globals::menuBg);

			Menu::Get().d3d9 = true;

		}

		vsonyp0wer_ImplWin32_NewFrame();
		vsonyp0wer_ImplDX9_NewFrame();
		vsonyp0wer::NewFrame();

		if (Menu::Get().isOpen)
			Menu::Get().Render();



		vsonyp0wer::Render();
		vsonyp0wer_ImplDX9_RenderDrawData(vsonyp0wer::GetDrawData());


		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
	}

	pixel_state->Apply();
	pixel_state->Release();
	pDevice->SetVertexDeclaration(vertDec);
	pDevice->SetVertexShader(vertShader);
	return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

bool __fastcall Handlers::DispatchUserMessage_h(void* thisptr, void*, int msg_type, int unk1, int nBytes, bf_read& msg_data)
{
	if (XSystemCFG.misc_devinfo)
	{
		if (msg_type == static_cast<int>(usermsg_type::CS_UM_VoteStart))
		{
			bf_read read = bf_read(msg_data);
			read.SetOffset(2);
			auto ent_index = read.ReadByte();
			auto vote_type = read.ReadByte();

			g_CVar->ConsoleColorPrintf(Color(0, 153, 204, 255), "CS_UM_VoteStart: index: %d, type: %d", ent_index, vote_type);
		}
	}

	if (XSystemCFG.misc_antikick && msg_type == static_cast<int>(usermsg_type::CS_UM_VoteStart))
	{
		g_EngineClient->ExecuteClientCmd("callvote swapteams");
	}
	return o_DispatchUserMessage(thisptr, msg_type, unk1, nBytes, msg_data);
}












































































// Junk Code By Troll Face & Thaisen's Gen
void xYbSAsKRqxITqucaJfSqRPyVZWyMqSdXBVnShams66587451() {     int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc88636550 = -178822316;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc99175287 = -86975518;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc27186113 = -533730030;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc51803823 = -995322367;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc288401 = -296905834;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc55432788 = -19366313;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc84028503 = 55654704;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc70338665 = -120480592;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc82862138 = -711940270;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45396868 = 20175618;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc42859051 = 32047485;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc96270865 = -643346969;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc4045743 = -319161146;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45694102 = -683533366;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc66686546 = 11857057;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc1939832 = -105278950;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc74637513 = -712335271;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc35349778 = -86681325;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc53230233 = -766613791;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc59663165 = -636308181;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc89208309 = -486859621;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc22952191 = -423840706;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc6443153 = -289179549;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc22609636 = -616303279;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc63065146 = -930176580;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc68473471 = -344030370;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc61964161 = 36951698;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc24226701 = -313530401;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc39862913 = 19062559;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45438657 = -655837225;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc70636586 = -884068241;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76184032 = -359854846;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc67827220 = -21710661;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc58402422 = -676276005;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc38248471 = -61518870;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76535549 = -374917319;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc73441352 = -232836117;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc34553927 = -634522530;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc2379190 = -81179560;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc77028405 = -702361599;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc94088721 = -496983427;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc4572406 = -559234577;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc51090329 = -451338919;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc46134020 = -28919539;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc79927600 = -133842265;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc3096163 = -434815483;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28645547 = -923325218;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc49833810 = -812498886;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc90576236 = -135650193;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc92416130 = -975563502;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc29232103 = -910508629;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28939197 = 1744505;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc56158138 = 16949109;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc91761691 = -578580700;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc77379141 = -267361987;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc65684360 = -754981610;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc92732135 = -797795970;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc4576477 = -917426751;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc88738677 = 34854212;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc31814929 = -952875464;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc93468627 = 43681989;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc59801803 = -630814896;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc30475753 = -39543151;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc37423482 = 43896954;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc74760281 = -95756142;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc66675019 = -608097670;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28443646 = -521636309;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45643321 = -642885142;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc7445632 = -522014497;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc90150997 = -613225624;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28498479 = -872442833;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc40083586 = 22187259;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc32970588 = 94498234;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76201828 = 35747807;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc65574443 = -39324754;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc84635903 = -927625045;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc71861861 = -972501788;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc60309133 = -160260010;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc42682035 = -382461014;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc59968983 = -395361098;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc39827924 = -420705153;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc12130351 = -150549417;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc33650464 = -77880209;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc47446782 = -5373940;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc16206554 = -745328596;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc41697389 = -785812746;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc20025894 = -276803956;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76065528 = -443129961;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc81023280 = -308914019;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc72564111 = -306537260;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc83803413 = -577121350;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc68864876 = -315409366;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45815250 = -569376743;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc70564260 = -128304096;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc83559777 = -646043588;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc34286918 = -866168532;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc74096653 = -419691426;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc13666848 = -395235874;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc71373738 = -933163398;    int XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc13252582 = -178822316;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc88636550 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc99175287;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc99175287 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc27186113;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc27186113 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc51803823;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc51803823 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc288401;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc288401 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc55432788;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc55432788 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc84028503;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc84028503 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc70338665;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc70338665 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc82862138;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc82862138 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45396868;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45396868 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc42859051;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc42859051 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc96270865;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc96270865 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc4045743;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc4045743 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45694102;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45694102 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc66686546;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc66686546 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc1939832;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc1939832 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc74637513;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc74637513 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc35349778;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc35349778 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc53230233;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc53230233 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc59663165;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc59663165 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc89208309;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc89208309 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc22952191;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc22952191 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc6443153;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc6443153 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc22609636;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc22609636 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc63065146;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc63065146 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc68473471;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc68473471 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc61964161;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc61964161 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc24226701;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc24226701 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc39862913;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc39862913 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45438657;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45438657 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc70636586;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc70636586 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76184032;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76184032 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc67827220;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc67827220 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc58402422;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc58402422 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc38248471;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc38248471 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76535549;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76535549 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc73441352;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc73441352 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc34553927;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc34553927 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc2379190;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc2379190 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc77028405;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc77028405 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc94088721;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc94088721 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc4572406;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc4572406 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc51090329;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc51090329 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc46134020;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc46134020 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc79927600;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc79927600 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc3096163;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc3096163 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28645547;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28645547 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc49833810;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc49833810 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc90576236;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc90576236 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc92416130;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc92416130 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc29232103;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc29232103 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28939197;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28939197 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc56158138;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc56158138 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc91761691;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc91761691 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc77379141;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc77379141 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc65684360;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc65684360 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc92732135;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc92732135 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc4576477;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc4576477 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc88738677;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc88738677 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc31814929;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc31814929 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc93468627;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc93468627 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc59801803;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc59801803 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc30475753;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc30475753 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc37423482;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc37423482 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc74760281;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc74760281 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc66675019;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc66675019 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28443646;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28443646 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45643321;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45643321 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc7445632;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc7445632 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc90150997;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc90150997 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28498479;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc28498479 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc40083586;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc40083586 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc32970588;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc32970588 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76201828;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76201828 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc65574443;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc65574443 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc84635903;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc84635903 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc71861861;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc71861861 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc60309133;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc60309133 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc42682035;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc42682035 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc59968983;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc59968983 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc39827924;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc39827924 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc12130351;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc12130351 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc33650464;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc33650464 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc47446782;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc47446782 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc16206554;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc16206554 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc41697389;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc41697389 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc20025894;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc20025894 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76065528;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc76065528 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc81023280;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc81023280 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc72564111;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc72564111 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc83803413;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc83803413 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc68864876;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc68864876 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45815250;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc45815250 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc70564260;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc70564260 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc83559777;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc83559777 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc34286918;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc34286918 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc74096653;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc74096653 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc13666848;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc13666848 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc71373738;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc71373738 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc13252582;     XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc13252582 = XxSmgobTmYDwzxxiaFsdtxbXUkSVHKtjtZTUwLolvfaSPhYDYErbhIzZVuSwWGMceUCZdc88636550;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void xmMnfyFYHdfQlgEeazCpSQzIsYTcAxfsWjNXXdHmBnOZhtDpQRI28666349() {     float JqYsgnDvGfdYKymtCSgHxrPnfCab47083440 = -792773855;    float JqYsgnDvGfdYKymtCSgHxrPnfCab70638694 = -893027874;    float JqYsgnDvGfdYKymtCSgHxrPnfCab2043120 = -94384169;    float JqYsgnDvGfdYKymtCSgHxrPnfCab35656079 = -626309024;    float JqYsgnDvGfdYKymtCSgHxrPnfCab62022086 = -174866667;    float JqYsgnDvGfdYKymtCSgHxrPnfCab75796603 = -377793982;    float JqYsgnDvGfdYKymtCSgHxrPnfCab60758685 = 29666082;    float JqYsgnDvGfdYKymtCSgHxrPnfCab51263642 = -862392327;    float JqYsgnDvGfdYKymtCSgHxrPnfCab43617092 = -588749737;    float JqYsgnDvGfdYKymtCSgHxrPnfCab57379205 = -186457978;    float JqYsgnDvGfdYKymtCSgHxrPnfCab95864616 = -466177775;    float JqYsgnDvGfdYKymtCSgHxrPnfCab94374967 = -749675932;    float JqYsgnDvGfdYKymtCSgHxrPnfCab2320572 = -128948027;    float JqYsgnDvGfdYKymtCSgHxrPnfCab14304032 = -67886949;    float JqYsgnDvGfdYKymtCSgHxrPnfCab16555716 = -167536431;    float JqYsgnDvGfdYKymtCSgHxrPnfCab29817355 = 69563210;    float JqYsgnDvGfdYKymtCSgHxrPnfCab65408310 = -316485057;    float JqYsgnDvGfdYKymtCSgHxrPnfCab82117480 = -587188749;    float JqYsgnDvGfdYKymtCSgHxrPnfCab21668432 = -435747284;    float JqYsgnDvGfdYKymtCSgHxrPnfCab60988281 = -312011393;    float JqYsgnDvGfdYKymtCSgHxrPnfCab21220056 = -32462355;    float JqYsgnDvGfdYKymtCSgHxrPnfCab27504106 = -978187936;    float JqYsgnDvGfdYKymtCSgHxrPnfCab21626330 = -190241680;    float JqYsgnDvGfdYKymtCSgHxrPnfCab39845869 = -615615100;    float JqYsgnDvGfdYKymtCSgHxrPnfCab1152060 = -45071426;    float JqYsgnDvGfdYKymtCSgHxrPnfCab15156328 = 30151765;    float JqYsgnDvGfdYKymtCSgHxrPnfCab37863343 = -853362542;    float JqYsgnDvGfdYKymtCSgHxrPnfCab54885195 = -713800515;    float JqYsgnDvGfdYKymtCSgHxrPnfCab9691300 = -354118599;    float JqYsgnDvGfdYKymtCSgHxrPnfCab85501529 = -640951244;    float JqYsgnDvGfdYKymtCSgHxrPnfCab9537862 = -39390463;    float JqYsgnDvGfdYKymtCSgHxrPnfCab95821133 = -200173080;    float JqYsgnDvGfdYKymtCSgHxrPnfCab61828116 = -134010957;    float JqYsgnDvGfdYKymtCSgHxrPnfCab68076236 = -711590688;    float JqYsgnDvGfdYKymtCSgHxrPnfCab99469641 = -659794989;    float JqYsgnDvGfdYKymtCSgHxrPnfCab39378196 = -323912347;    float JqYsgnDvGfdYKymtCSgHxrPnfCab99182222 = -357913177;    float JqYsgnDvGfdYKymtCSgHxrPnfCab43290826 = -892597528;    float JqYsgnDvGfdYKymtCSgHxrPnfCab51482552 = -680648823;    float JqYsgnDvGfdYKymtCSgHxrPnfCab50442600 = -447187549;    float JqYsgnDvGfdYKymtCSgHxrPnfCab67110487 = 8600097;    float JqYsgnDvGfdYKymtCSgHxrPnfCab87732129 = 95332985;    float JqYsgnDvGfdYKymtCSgHxrPnfCab56454065 = 24953026;    float JqYsgnDvGfdYKymtCSgHxrPnfCab95370698 = -46612649;    float JqYsgnDvGfdYKymtCSgHxrPnfCab56835161 = -655028842;    float JqYsgnDvGfdYKymtCSgHxrPnfCab51385591 = -256299923;    float JqYsgnDvGfdYKymtCSgHxrPnfCab43969075 = -364217286;    float JqYsgnDvGfdYKymtCSgHxrPnfCab97163101 = -735458179;    float JqYsgnDvGfdYKymtCSgHxrPnfCab11149427 = -681678845;    float JqYsgnDvGfdYKymtCSgHxrPnfCab5647254 = -831960473;    float JqYsgnDvGfdYKymtCSgHxrPnfCab78681668 = 90092028;    float JqYsgnDvGfdYKymtCSgHxrPnfCab24240936 = -905053593;    float JqYsgnDvGfdYKymtCSgHxrPnfCab83735619 = -482529188;    float JqYsgnDvGfdYKymtCSgHxrPnfCab31264181 = -65165711;    float JqYsgnDvGfdYKymtCSgHxrPnfCab7446293 = -242829175;    float JqYsgnDvGfdYKymtCSgHxrPnfCab19579334 = -814585919;    float JqYsgnDvGfdYKymtCSgHxrPnfCab49012365 = -602786195;    float JqYsgnDvGfdYKymtCSgHxrPnfCab62197251 = -478769070;    float JqYsgnDvGfdYKymtCSgHxrPnfCab34504019 = -481237599;    float JqYsgnDvGfdYKymtCSgHxrPnfCab46865759 = -105018433;    float JqYsgnDvGfdYKymtCSgHxrPnfCab37933261 = -524431441;    float JqYsgnDvGfdYKymtCSgHxrPnfCab5873491 = -256533404;    float JqYsgnDvGfdYKymtCSgHxrPnfCab41572342 = -408273729;    float JqYsgnDvGfdYKymtCSgHxrPnfCab58115562 = -947798494;    float JqYsgnDvGfdYKymtCSgHxrPnfCab47841344 = -47067515;    float JqYsgnDvGfdYKymtCSgHxrPnfCab43483 = -166004695;    float JqYsgnDvGfdYKymtCSgHxrPnfCab32546851 = -515664976;    float JqYsgnDvGfdYKymtCSgHxrPnfCab34244336 = -417357339;    float JqYsgnDvGfdYKymtCSgHxrPnfCab14834390 = -408091960;    float JqYsgnDvGfdYKymtCSgHxrPnfCab77177520 = -843624085;    float JqYsgnDvGfdYKymtCSgHxrPnfCab30635132 = -572523613;    float JqYsgnDvGfdYKymtCSgHxrPnfCab22117485 = -423887530;    float JqYsgnDvGfdYKymtCSgHxrPnfCab30634928 = -906539927;    float JqYsgnDvGfdYKymtCSgHxrPnfCab71225831 = -988559736;    float JqYsgnDvGfdYKymtCSgHxrPnfCab93877793 = -220611490;    float JqYsgnDvGfdYKymtCSgHxrPnfCab33487927 = -27795340;    float JqYsgnDvGfdYKymtCSgHxrPnfCab71050041 = -903140963;    float JqYsgnDvGfdYKymtCSgHxrPnfCab26255631 = -43629032;    float JqYsgnDvGfdYKymtCSgHxrPnfCab83010707 = -960586258;    float JqYsgnDvGfdYKymtCSgHxrPnfCab49766469 = -788771503;    float JqYsgnDvGfdYKymtCSgHxrPnfCab71187253 = -605630949;    float JqYsgnDvGfdYKymtCSgHxrPnfCab40700241 = -17904363;    float JqYsgnDvGfdYKymtCSgHxrPnfCab43735768 = 67878329;    float JqYsgnDvGfdYKymtCSgHxrPnfCab4044047 = -522158126;    float JqYsgnDvGfdYKymtCSgHxrPnfCab6819861 = -631043272;    float JqYsgnDvGfdYKymtCSgHxrPnfCab85296925 = -134336871;    float JqYsgnDvGfdYKymtCSgHxrPnfCab12085515 = -717643893;    float JqYsgnDvGfdYKymtCSgHxrPnfCab30563936 = 31154753;    float JqYsgnDvGfdYKymtCSgHxrPnfCab60629943 = -368761514;    float JqYsgnDvGfdYKymtCSgHxrPnfCab79890307 = -845209070;    float JqYsgnDvGfdYKymtCSgHxrPnfCab90365831 = -721126152;    float JqYsgnDvGfdYKymtCSgHxrPnfCab36984972 = -879144108;    float JqYsgnDvGfdYKymtCSgHxrPnfCab8786807 = -311359930;    float JqYsgnDvGfdYKymtCSgHxrPnfCab4616794 = -475630391;    float JqYsgnDvGfdYKymtCSgHxrPnfCab12509340 = -922756108;    float JqYsgnDvGfdYKymtCSgHxrPnfCab61236997 = -734866499;    float JqYsgnDvGfdYKymtCSgHxrPnfCab46159787 = -496393287;    float JqYsgnDvGfdYKymtCSgHxrPnfCab98338502 = -27248480;    float JqYsgnDvGfdYKymtCSgHxrPnfCab47529354 = -999545134;    float JqYsgnDvGfdYKymtCSgHxrPnfCab56791679 = -792773855;     JqYsgnDvGfdYKymtCSgHxrPnfCab47083440 = JqYsgnDvGfdYKymtCSgHxrPnfCab70638694;     JqYsgnDvGfdYKymtCSgHxrPnfCab70638694 = JqYsgnDvGfdYKymtCSgHxrPnfCab2043120;     JqYsgnDvGfdYKymtCSgHxrPnfCab2043120 = JqYsgnDvGfdYKymtCSgHxrPnfCab35656079;     JqYsgnDvGfdYKymtCSgHxrPnfCab35656079 = JqYsgnDvGfdYKymtCSgHxrPnfCab62022086;     JqYsgnDvGfdYKymtCSgHxrPnfCab62022086 = JqYsgnDvGfdYKymtCSgHxrPnfCab75796603;     JqYsgnDvGfdYKymtCSgHxrPnfCab75796603 = JqYsgnDvGfdYKymtCSgHxrPnfCab60758685;     JqYsgnDvGfdYKymtCSgHxrPnfCab60758685 = JqYsgnDvGfdYKymtCSgHxrPnfCab51263642;     JqYsgnDvGfdYKymtCSgHxrPnfCab51263642 = JqYsgnDvGfdYKymtCSgHxrPnfCab43617092;     JqYsgnDvGfdYKymtCSgHxrPnfCab43617092 = JqYsgnDvGfdYKymtCSgHxrPnfCab57379205;     JqYsgnDvGfdYKymtCSgHxrPnfCab57379205 = JqYsgnDvGfdYKymtCSgHxrPnfCab95864616;     JqYsgnDvGfdYKymtCSgHxrPnfCab95864616 = JqYsgnDvGfdYKymtCSgHxrPnfCab94374967;     JqYsgnDvGfdYKymtCSgHxrPnfCab94374967 = JqYsgnDvGfdYKymtCSgHxrPnfCab2320572;     JqYsgnDvGfdYKymtCSgHxrPnfCab2320572 = JqYsgnDvGfdYKymtCSgHxrPnfCab14304032;     JqYsgnDvGfdYKymtCSgHxrPnfCab14304032 = JqYsgnDvGfdYKymtCSgHxrPnfCab16555716;     JqYsgnDvGfdYKymtCSgHxrPnfCab16555716 = JqYsgnDvGfdYKymtCSgHxrPnfCab29817355;     JqYsgnDvGfdYKymtCSgHxrPnfCab29817355 = JqYsgnDvGfdYKymtCSgHxrPnfCab65408310;     JqYsgnDvGfdYKymtCSgHxrPnfCab65408310 = JqYsgnDvGfdYKymtCSgHxrPnfCab82117480;     JqYsgnDvGfdYKymtCSgHxrPnfCab82117480 = JqYsgnDvGfdYKymtCSgHxrPnfCab21668432;     JqYsgnDvGfdYKymtCSgHxrPnfCab21668432 = JqYsgnDvGfdYKymtCSgHxrPnfCab60988281;     JqYsgnDvGfdYKymtCSgHxrPnfCab60988281 = JqYsgnDvGfdYKymtCSgHxrPnfCab21220056;     JqYsgnDvGfdYKymtCSgHxrPnfCab21220056 = JqYsgnDvGfdYKymtCSgHxrPnfCab27504106;     JqYsgnDvGfdYKymtCSgHxrPnfCab27504106 = JqYsgnDvGfdYKymtCSgHxrPnfCab21626330;     JqYsgnDvGfdYKymtCSgHxrPnfCab21626330 = JqYsgnDvGfdYKymtCSgHxrPnfCab39845869;     JqYsgnDvGfdYKymtCSgHxrPnfCab39845869 = JqYsgnDvGfdYKymtCSgHxrPnfCab1152060;     JqYsgnDvGfdYKymtCSgHxrPnfCab1152060 = JqYsgnDvGfdYKymtCSgHxrPnfCab15156328;     JqYsgnDvGfdYKymtCSgHxrPnfCab15156328 = JqYsgnDvGfdYKymtCSgHxrPnfCab37863343;     JqYsgnDvGfdYKymtCSgHxrPnfCab37863343 = JqYsgnDvGfdYKymtCSgHxrPnfCab54885195;     JqYsgnDvGfdYKymtCSgHxrPnfCab54885195 = JqYsgnDvGfdYKymtCSgHxrPnfCab9691300;     JqYsgnDvGfdYKymtCSgHxrPnfCab9691300 = JqYsgnDvGfdYKymtCSgHxrPnfCab85501529;     JqYsgnDvGfdYKymtCSgHxrPnfCab85501529 = JqYsgnDvGfdYKymtCSgHxrPnfCab9537862;     JqYsgnDvGfdYKymtCSgHxrPnfCab9537862 = JqYsgnDvGfdYKymtCSgHxrPnfCab95821133;     JqYsgnDvGfdYKymtCSgHxrPnfCab95821133 = JqYsgnDvGfdYKymtCSgHxrPnfCab61828116;     JqYsgnDvGfdYKymtCSgHxrPnfCab61828116 = JqYsgnDvGfdYKymtCSgHxrPnfCab68076236;     JqYsgnDvGfdYKymtCSgHxrPnfCab68076236 = JqYsgnDvGfdYKymtCSgHxrPnfCab99469641;     JqYsgnDvGfdYKymtCSgHxrPnfCab99469641 = JqYsgnDvGfdYKymtCSgHxrPnfCab39378196;     JqYsgnDvGfdYKymtCSgHxrPnfCab39378196 = JqYsgnDvGfdYKymtCSgHxrPnfCab99182222;     JqYsgnDvGfdYKymtCSgHxrPnfCab99182222 = JqYsgnDvGfdYKymtCSgHxrPnfCab43290826;     JqYsgnDvGfdYKymtCSgHxrPnfCab43290826 = JqYsgnDvGfdYKymtCSgHxrPnfCab51482552;     JqYsgnDvGfdYKymtCSgHxrPnfCab51482552 = JqYsgnDvGfdYKymtCSgHxrPnfCab50442600;     JqYsgnDvGfdYKymtCSgHxrPnfCab50442600 = JqYsgnDvGfdYKymtCSgHxrPnfCab67110487;     JqYsgnDvGfdYKymtCSgHxrPnfCab67110487 = JqYsgnDvGfdYKymtCSgHxrPnfCab87732129;     JqYsgnDvGfdYKymtCSgHxrPnfCab87732129 = JqYsgnDvGfdYKymtCSgHxrPnfCab56454065;     JqYsgnDvGfdYKymtCSgHxrPnfCab56454065 = JqYsgnDvGfdYKymtCSgHxrPnfCab95370698;     JqYsgnDvGfdYKymtCSgHxrPnfCab95370698 = JqYsgnDvGfdYKymtCSgHxrPnfCab56835161;     JqYsgnDvGfdYKymtCSgHxrPnfCab56835161 = JqYsgnDvGfdYKymtCSgHxrPnfCab51385591;     JqYsgnDvGfdYKymtCSgHxrPnfCab51385591 = JqYsgnDvGfdYKymtCSgHxrPnfCab43969075;     JqYsgnDvGfdYKymtCSgHxrPnfCab43969075 = JqYsgnDvGfdYKymtCSgHxrPnfCab97163101;     JqYsgnDvGfdYKymtCSgHxrPnfCab97163101 = JqYsgnDvGfdYKymtCSgHxrPnfCab11149427;     JqYsgnDvGfdYKymtCSgHxrPnfCab11149427 = JqYsgnDvGfdYKymtCSgHxrPnfCab5647254;     JqYsgnDvGfdYKymtCSgHxrPnfCab5647254 = JqYsgnDvGfdYKymtCSgHxrPnfCab78681668;     JqYsgnDvGfdYKymtCSgHxrPnfCab78681668 = JqYsgnDvGfdYKymtCSgHxrPnfCab24240936;     JqYsgnDvGfdYKymtCSgHxrPnfCab24240936 = JqYsgnDvGfdYKymtCSgHxrPnfCab83735619;     JqYsgnDvGfdYKymtCSgHxrPnfCab83735619 = JqYsgnDvGfdYKymtCSgHxrPnfCab31264181;     JqYsgnDvGfdYKymtCSgHxrPnfCab31264181 = JqYsgnDvGfdYKymtCSgHxrPnfCab7446293;     JqYsgnDvGfdYKymtCSgHxrPnfCab7446293 = JqYsgnDvGfdYKymtCSgHxrPnfCab19579334;     JqYsgnDvGfdYKymtCSgHxrPnfCab19579334 = JqYsgnDvGfdYKymtCSgHxrPnfCab49012365;     JqYsgnDvGfdYKymtCSgHxrPnfCab49012365 = JqYsgnDvGfdYKymtCSgHxrPnfCab62197251;     JqYsgnDvGfdYKymtCSgHxrPnfCab62197251 = JqYsgnDvGfdYKymtCSgHxrPnfCab34504019;     JqYsgnDvGfdYKymtCSgHxrPnfCab34504019 = JqYsgnDvGfdYKymtCSgHxrPnfCab46865759;     JqYsgnDvGfdYKymtCSgHxrPnfCab46865759 = JqYsgnDvGfdYKymtCSgHxrPnfCab37933261;     JqYsgnDvGfdYKymtCSgHxrPnfCab37933261 = JqYsgnDvGfdYKymtCSgHxrPnfCab5873491;     JqYsgnDvGfdYKymtCSgHxrPnfCab5873491 = JqYsgnDvGfdYKymtCSgHxrPnfCab41572342;     JqYsgnDvGfdYKymtCSgHxrPnfCab41572342 = JqYsgnDvGfdYKymtCSgHxrPnfCab58115562;     JqYsgnDvGfdYKymtCSgHxrPnfCab58115562 = JqYsgnDvGfdYKymtCSgHxrPnfCab47841344;     JqYsgnDvGfdYKymtCSgHxrPnfCab47841344 = JqYsgnDvGfdYKymtCSgHxrPnfCab43483;     JqYsgnDvGfdYKymtCSgHxrPnfCab43483 = JqYsgnDvGfdYKymtCSgHxrPnfCab32546851;     JqYsgnDvGfdYKymtCSgHxrPnfCab32546851 = JqYsgnDvGfdYKymtCSgHxrPnfCab34244336;     JqYsgnDvGfdYKymtCSgHxrPnfCab34244336 = JqYsgnDvGfdYKymtCSgHxrPnfCab14834390;     JqYsgnDvGfdYKymtCSgHxrPnfCab14834390 = JqYsgnDvGfdYKymtCSgHxrPnfCab77177520;     JqYsgnDvGfdYKymtCSgHxrPnfCab77177520 = JqYsgnDvGfdYKymtCSgHxrPnfCab30635132;     JqYsgnDvGfdYKymtCSgHxrPnfCab30635132 = JqYsgnDvGfdYKymtCSgHxrPnfCab22117485;     JqYsgnDvGfdYKymtCSgHxrPnfCab22117485 = JqYsgnDvGfdYKymtCSgHxrPnfCab30634928;     JqYsgnDvGfdYKymtCSgHxrPnfCab30634928 = JqYsgnDvGfdYKymtCSgHxrPnfCab71225831;     JqYsgnDvGfdYKymtCSgHxrPnfCab71225831 = JqYsgnDvGfdYKymtCSgHxrPnfCab93877793;     JqYsgnDvGfdYKymtCSgHxrPnfCab93877793 = JqYsgnDvGfdYKymtCSgHxrPnfCab33487927;     JqYsgnDvGfdYKymtCSgHxrPnfCab33487927 = JqYsgnDvGfdYKymtCSgHxrPnfCab71050041;     JqYsgnDvGfdYKymtCSgHxrPnfCab71050041 = JqYsgnDvGfdYKymtCSgHxrPnfCab26255631;     JqYsgnDvGfdYKymtCSgHxrPnfCab26255631 = JqYsgnDvGfdYKymtCSgHxrPnfCab83010707;     JqYsgnDvGfdYKymtCSgHxrPnfCab83010707 = JqYsgnDvGfdYKymtCSgHxrPnfCab49766469;     JqYsgnDvGfdYKymtCSgHxrPnfCab49766469 = JqYsgnDvGfdYKymtCSgHxrPnfCab71187253;     JqYsgnDvGfdYKymtCSgHxrPnfCab71187253 = JqYsgnDvGfdYKymtCSgHxrPnfCab40700241;     JqYsgnDvGfdYKymtCSgHxrPnfCab40700241 = JqYsgnDvGfdYKymtCSgHxrPnfCab43735768;     JqYsgnDvGfdYKymtCSgHxrPnfCab43735768 = JqYsgnDvGfdYKymtCSgHxrPnfCab4044047;     JqYsgnDvGfdYKymtCSgHxrPnfCab4044047 = JqYsgnDvGfdYKymtCSgHxrPnfCab6819861;     JqYsgnDvGfdYKymtCSgHxrPnfCab6819861 = JqYsgnDvGfdYKymtCSgHxrPnfCab85296925;     JqYsgnDvGfdYKymtCSgHxrPnfCab85296925 = JqYsgnDvGfdYKymtCSgHxrPnfCab12085515;     JqYsgnDvGfdYKymtCSgHxrPnfCab12085515 = JqYsgnDvGfdYKymtCSgHxrPnfCab30563936;     JqYsgnDvGfdYKymtCSgHxrPnfCab30563936 = JqYsgnDvGfdYKymtCSgHxrPnfCab60629943;     JqYsgnDvGfdYKymtCSgHxrPnfCab60629943 = JqYsgnDvGfdYKymtCSgHxrPnfCab79890307;     JqYsgnDvGfdYKymtCSgHxrPnfCab79890307 = JqYsgnDvGfdYKymtCSgHxrPnfCab90365831;     JqYsgnDvGfdYKymtCSgHxrPnfCab90365831 = JqYsgnDvGfdYKymtCSgHxrPnfCab36984972;     JqYsgnDvGfdYKymtCSgHxrPnfCab36984972 = JqYsgnDvGfdYKymtCSgHxrPnfCab8786807;     JqYsgnDvGfdYKymtCSgHxrPnfCab8786807 = JqYsgnDvGfdYKymtCSgHxrPnfCab4616794;     JqYsgnDvGfdYKymtCSgHxrPnfCab4616794 = JqYsgnDvGfdYKymtCSgHxrPnfCab12509340;     JqYsgnDvGfdYKymtCSgHxrPnfCab12509340 = JqYsgnDvGfdYKymtCSgHxrPnfCab61236997;     JqYsgnDvGfdYKymtCSgHxrPnfCab61236997 = JqYsgnDvGfdYKymtCSgHxrPnfCab46159787;     JqYsgnDvGfdYKymtCSgHxrPnfCab46159787 = JqYsgnDvGfdYKymtCSgHxrPnfCab98338502;     JqYsgnDvGfdYKymtCSgHxrPnfCab98338502 = JqYsgnDvGfdYKymtCSgHxrPnfCab47529354;     JqYsgnDvGfdYKymtCSgHxrPnfCab47529354 = JqYsgnDvGfdYKymtCSgHxrPnfCab56791679;     JqYsgnDvGfdYKymtCSgHxrPnfCab56791679 = JqYsgnDvGfdYKymtCSgHxrPnfCab47083440;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dUsSDTWvlBMmqlzOVZypzqhpC81476237() {     long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX53229983 = -24281930;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX3548422 = -956236274;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX45057759 = -765242612;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX60801390 = -639403182;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX8991917 = -731370766;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX80412263 = -728129204;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX576870 = -414441762;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24625911 = 81891289;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX52434477 = -776309872;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX70084060 = 21077345;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86951891 = -561813864;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX42401625 = -773459708;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX79395369 = -248871115;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48497957 = -147928025;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX70932146 = -68566416;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX49682419 = -104609673;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX43834290 = -761085052;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX99751148 = -95097906;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX93888373 = -615000125;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX45177183 = -516057057;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX63755628 = -335848315;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX30223328 = -925039009;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX41638074 = -963013127;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX75209687 = -589440978;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX6213536 = -603649682;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX5406630 = -233347624;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX881074 = 49514385;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX17082841 = -806733722;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX45773966 = -89653;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX92239216 = -620082794;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX18094788 = -286241814;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX63778641 = 59465402;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX2016498 = 22810245;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX96876767 = -438000983;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX67090532 = -27323645;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48405323 = -835759149;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24917410 = -155304638;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX1505874 = -681077448;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX42883387 = -206159940;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX91259310 = -994722804;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX97066641 = -397619183;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX38860233 = -291137218;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX47051776 = -854414727;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX322299 = -50359601;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX73646059 = -953263487;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX29086770 = -275095558;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86497122 = -45366168;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX2749006 = -235973773;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX55308352 = -399154492;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX49185848 = -666380990;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX67043944 = 10383792;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX10100329 = -144810929;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX982816 = -429578206;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX68446007 = -952279660;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX17720609 = -8950796;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX23006656 = -99242921;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX61910347 = -993223148;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX69848072 = -75801635;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX54587855 = 64246500;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX3585288 = -398023143;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX79531189 = -677643590;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX83494029 = -607708041;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX78851944 = -918019059;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX60195261 = -56227078;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX51989273 = -692680842;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX23173251 = -521279267;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX40385127 = -696269953;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX82518601 = -810870133;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX81407424 = -20604380;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX22526823 = -232807267;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24765009 = -949305035;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX42328417 = 19992395;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX56867761 = -888937966;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX2629063 = -620277322;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48110541 = -18437875;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24895396 = 55288903;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX83171552 = 29375717;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX41315775 = -812653526;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX1563629 = -636177492;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX77126766 = -228554124;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX18909507 = -87981456;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX98132068 = -714511842;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX61774488 = -307579231;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX96588117 = -333708663;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX25195272 = -530466586;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX7994460 = -41430885;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX62795825 = -510956393;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX33570491 = -24910096;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX79156158 = -329050188;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX44083877 = -928080725;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86494976 = -842536001;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX55069338 = 20496996;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX46918018 = -645323948;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX39298100 = -808136798;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX11728122 = -217079214;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX13572613 = -789911142;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX60008288 = -373118160;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86856515 = -698187649;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48333026 = -357678759;    long TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX50472808 = -24281930;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX53229983 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX3548422;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX3548422 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX45057759;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX45057759 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX60801390;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX60801390 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX8991917;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX8991917 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX80412263;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX80412263 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX576870;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX576870 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24625911;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24625911 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX52434477;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX52434477 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX70084060;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX70084060 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86951891;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86951891 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX42401625;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX42401625 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX79395369;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX79395369 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48497957;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48497957 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX70932146;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX70932146 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX49682419;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX49682419 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX43834290;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX43834290 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX99751148;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX99751148 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX93888373;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX93888373 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX45177183;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX45177183 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX63755628;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX63755628 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX30223328;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX30223328 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX41638074;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX41638074 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX75209687;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX75209687 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX6213536;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX6213536 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX5406630;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX5406630 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX881074;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX881074 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX17082841;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX17082841 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX45773966;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX45773966 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX92239216;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX92239216 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX18094788;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX18094788 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX63778641;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX63778641 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX2016498;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX2016498 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX96876767;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX96876767 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX67090532;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX67090532 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48405323;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48405323 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24917410;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24917410 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX1505874;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX1505874 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX42883387;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX42883387 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX91259310;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX91259310 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX97066641;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX97066641 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX38860233;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX38860233 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX47051776;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX47051776 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX322299;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX322299 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX73646059;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX73646059 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX29086770;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX29086770 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86497122;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86497122 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX2749006;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX2749006 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX55308352;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX55308352 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX49185848;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX49185848 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX67043944;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX67043944 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX10100329;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX10100329 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX982816;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX982816 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX68446007;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX68446007 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX17720609;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX17720609 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX23006656;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX23006656 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX61910347;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX61910347 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX69848072;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX69848072 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX54587855;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX54587855 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX3585288;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX3585288 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX79531189;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX79531189 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX83494029;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX83494029 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX78851944;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX78851944 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX60195261;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX60195261 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX51989273;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX51989273 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX23173251;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX23173251 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX40385127;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX40385127 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX82518601;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX82518601 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX81407424;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX81407424 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX22526823;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX22526823 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24765009;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24765009 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX42328417;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX42328417 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX56867761;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX56867761 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX2629063;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX2629063 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48110541;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48110541 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24895396;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX24895396 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX83171552;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX83171552 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX41315775;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX41315775 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX1563629;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX1563629 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX77126766;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX77126766 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX18909507;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX18909507 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX98132068;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX98132068 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX61774488;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX61774488 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX96588117;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX96588117 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX25195272;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX25195272 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX7994460;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX7994460 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX62795825;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX62795825 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX33570491;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX33570491 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX79156158;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX79156158 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX44083877;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX44083877 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86494976;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86494976 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX55069338;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX55069338 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX46918018;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX46918018 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX39298100;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX39298100 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX11728122;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX11728122 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX13572613;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX13572613 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX60008288;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX60008288 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86856515;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX86856515 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48333026;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX48333026 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX50472808;     TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX50472808 = TdBWVVTGPAcWIdAaMtVFumAytYGDhxmmCQjpVYQgsfalX53229983;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kpZZAtYvKfoBiiIBvnwfvdeDeXneBLgValBeY72798070() {     double YFnTPlWdNyNtMxShlBHKW60538991 = -421511382;    double YFnTPlWdNyNtMxShlBHKW2467831 = -483644134;    double YFnTPlWdNyNtMxShlBHKW30472265 = -712434894;    double YFnTPlWdNyNtMxShlBHKW58740387 = -717819164;    double YFnTPlWdNyNtMxShlBHKW5695885 = -317163591;    double YFnTPlWdNyNtMxShlBHKW62553266 = 87282552;    double YFnTPlWdNyNtMxShlBHKW61720912 = -278762603;    double YFnTPlWdNyNtMxShlBHKW64525990 = 84589500;    double YFnTPlWdNyNtMxShlBHKW65335652 = -773375684;    double YFnTPlWdNyNtMxShlBHKW16905073 = -811518968;    double YFnTPlWdNyNtMxShlBHKW32107031 = -677645817;    double YFnTPlWdNyNtMxShlBHKW78928190 = -804009250;    double YFnTPlWdNyNtMxShlBHKW99447864 = -749364027;    double YFnTPlWdNyNtMxShlBHKW8116593 = -58459002;    double YFnTPlWdNyNtMxShlBHKW44311489 = -800079380;    double YFnTPlWdNyNtMxShlBHKW53195253 = -742160073;    double YFnTPlWdNyNtMxShlBHKW91672452 = -906842848;    double YFnTPlWdNyNtMxShlBHKW63367291 = -856901311;    double YFnTPlWdNyNtMxShlBHKW89573094 = -382678515;    double YFnTPlWdNyNtMxShlBHKW91781221 = -884012037;    double YFnTPlWdNyNtMxShlBHKW7896512 = -650716804;    double YFnTPlWdNyNtMxShlBHKW58819096 = -973845796;    double YFnTPlWdNyNtMxShlBHKW15275410 = 77333913;    double YFnTPlWdNyNtMxShlBHKW18564225 = -682115469;    double YFnTPlWdNyNtMxShlBHKW31634652 = -5587539;    double YFnTPlWdNyNtMxShlBHKW352801 = -61005507;    double YFnTPlWdNyNtMxShlBHKW91451599 = -755234598;    double YFnTPlWdNyNtMxShlBHKW51332492 = -470200129;    double YFnTPlWdNyNtMxShlBHKW63459115 = -651236301;    double YFnTPlWdNyNtMxShlBHKW75734222 = -11322685;    double YFnTPlWdNyNtMxShlBHKW12510832 = -24253769;    double YFnTPlWdNyNtMxShlBHKW87020834 = -168264986;    double YFnTPlWdNyNtMxShlBHKW41394512 = -693441097;    double YFnTPlWdNyNtMxShlBHKW33204989 = -947553386;    double YFnTPlWdNyNtMxShlBHKW90468947 = -21866530;    double YFnTPlWdNyNtMxShlBHKW20551389 = -557502565;    double YFnTPlWdNyNtMxShlBHKW39870078 = -763286711;    double YFnTPlWdNyNtMxShlBHKW48263394 = -161065366;    double YFnTPlWdNyNtMxShlBHKW10273375 = -714199471;    double YFnTPlWdNyNtMxShlBHKW14484485 = -618988305;    double YFnTPlWdNyNtMxShlBHKW6144943 = -688888715;    double YFnTPlWdNyNtMxShlBHKW60129592 = -837924532;    double YFnTPlWdNyNtMxShlBHKW86404126 = -839908062;    double YFnTPlWdNyNtMxShlBHKW45900329 = -437143305;    double YFnTPlWdNyNtMxShlBHKW29441274 = -346114619;    double YFnTPlWdNyNtMxShlBHKW90123552 = -829042107;    double YFnTPlWdNyNtMxShlBHKW13184319 = -943053548;    double YFnTPlWdNyNtMxShlBHKW43789928 = -608806756;    double YFnTPlWdNyNtMxShlBHKW49412904 = -45687418;    double YFnTPlWdNyNtMxShlBHKW75401211 = -780628922;    double YFnTPlWdNyNtMxShlBHKW42252927 = -39835408;    double YFnTPlWdNyNtMxShlBHKW18660952 = -493408809;    double YFnTPlWdNyNtMxShlBHKW73687655 = -552491832;    double YFnTPlWdNyNtMxShlBHKW6238489 = -659420986;    double YFnTPlWdNyNtMxShlBHKW64901558 = -421377348;    double YFnTPlWdNyNtMxShlBHKW1719896 = -447665587;    double YFnTPlWdNyNtMxShlBHKW87192421 = -460978047;    double YFnTPlWdNyNtMxShlBHKW11908040 = 69680574;    double YFnTPlWdNyNtMxShlBHKW27105735 = -612231625;    double YFnTPlWdNyNtMxShlBHKW5343084 = -156158085;    double YFnTPlWdNyNtMxShlBHKW71101666 = -157482851;    double YFnTPlWdNyNtMxShlBHKW10388421 = -808562475;    double YFnTPlWdNyNtMxShlBHKW1066876 = -264174199;    double YFnTPlWdNyNtMxShlBHKW89601430 = -662052999;    double YFnTPlWdNyNtMxShlBHKW4394241 = -687265199;    double YFnTPlWdNyNtMxShlBHKW45086197 = -409380832;    double YFnTPlWdNyNtMxShlBHKW37533678 = -10568154;    double YFnTPlWdNyNtMxShlBHKW66242876 = -801810642;    double YFnTPlWdNyNtMxShlBHKW17647645 = 63407527;    double YFnTPlWdNyNtMxShlBHKW23760101 = -142576816;    double YFnTPlWdNyNtMxShlBHKW13325175 = -978873363;    double YFnTPlWdNyNtMxShlBHKW43409059 = -645777482;    double YFnTPlWdNyNtMxShlBHKW53093916 = -42701840;    double YFnTPlWdNyNtMxShlBHKW75088609 = -763690211;    double YFnTPlWdNyNtMxShlBHKW85636278 = -95123323;    double YFnTPlWdNyNtMxShlBHKW47766919 = -812792272;    double YFnTPlWdNyNtMxShlBHKW72414970 = -33937734;    double YFnTPlWdNyNtMxShlBHKW69375080 = -485522782;    double YFnTPlWdNyNtMxShlBHKW89122951 = -236000851;    double YFnTPlWdNyNtMxShlBHKW41511099 = -176545433;    double YFnTPlWdNyNtMxShlBHKW87168482 = -117951959;    double YFnTPlWdNyNtMxShlBHKW47661672 = -46427842;    double YFnTPlWdNyNtMxShlBHKW1919589 = -324512711;    double YFnTPlWdNyNtMxShlBHKW88057903 = -870607380;    double YFnTPlWdNyNtMxShlBHKW33481295 = -971487278;    double YFnTPlWdNyNtMxShlBHKW93849880 = -530844961;    double YFnTPlWdNyNtMxShlBHKW13333179 = -615773154;    double YFnTPlWdNyNtMxShlBHKW35156023 = 65979889;    double YFnTPlWdNyNtMxShlBHKW68303430 = -426176038;    double YFnTPlWdNyNtMxShlBHKW88749051 = -574200943;    double YFnTPlWdNyNtMxShlBHKW33358968 = 3475482;    double YFnTPlWdNyNtMxShlBHKW27962039 = -732967285;    double YFnTPlWdNyNtMxShlBHKW21157659 = -548833741;    double YFnTPlWdNyNtMxShlBHKW4930292 = -458041387;    double YFnTPlWdNyNtMxShlBHKW43382819 = -361505455;    double YFnTPlWdNyNtMxShlBHKW95756522 = -880326241;    double YFnTPlWdNyNtMxShlBHKW59062716 = -473750334;    double YFnTPlWdNyNtMxShlBHKW96802695 = -77855063;    double YFnTPlWdNyNtMxShlBHKW41506089 = -749878106;    double YFnTPlWdNyNtMxShlBHKW84355077 = -421511382;     YFnTPlWdNyNtMxShlBHKW60538991 = YFnTPlWdNyNtMxShlBHKW2467831;     YFnTPlWdNyNtMxShlBHKW2467831 = YFnTPlWdNyNtMxShlBHKW30472265;     YFnTPlWdNyNtMxShlBHKW30472265 = YFnTPlWdNyNtMxShlBHKW58740387;     YFnTPlWdNyNtMxShlBHKW58740387 = YFnTPlWdNyNtMxShlBHKW5695885;     YFnTPlWdNyNtMxShlBHKW5695885 = YFnTPlWdNyNtMxShlBHKW62553266;     YFnTPlWdNyNtMxShlBHKW62553266 = YFnTPlWdNyNtMxShlBHKW61720912;     YFnTPlWdNyNtMxShlBHKW61720912 = YFnTPlWdNyNtMxShlBHKW64525990;     YFnTPlWdNyNtMxShlBHKW64525990 = YFnTPlWdNyNtMxShlBHKW65335652;     YFnTPlWdNyNtMxShlBHKW65335652 = YFnTPlWdNyNtMxShlBHKW16905073;     YFnTPlWdNyNtMxShlBHKW16905073 = YFnTPlWdNyNtMxShlBHKW32107031;     YFnTPlWdNyNtMxShlBHKW32107031 = YFnTPlWdNyNtMxShlBHKW78928190;     YFnTPlWdNyNtMxShlBHKW78928190 = YFnTPlWdNyNtMxShlBHKW99447864;     YFnTPlWdNyNtMxShlBHKW99447864 = YFnTPlWdNyNtMxShlBHKW8116593;     YFnTPlWdNyNtMxShlBHKW8116593 = YFnTPlWdNyNtMxShlBHKW44311489;     YFnTPlWdNyNtMxShlBHKW44311489 = YFnTPlWdNyNtMxShlBHKW53195253;     YFnTPlWdNyNtMxShlBHKW53195253 = YFnTPlWdNyNtMxShlBHKW91672452;     YFnTPlWdNyNtMxShlBHKW91672452 = YFnTPlWdNyNtMxShlBHKW63367291;     YFnTPlWdNyNtMxShlBHKW63367291 = YFnTPlWdNyNtMxShlBHKW89573094;     YFnTPlWdNyNtMxShlBHKW89573094 = YFnTPlWdNyNtMxShlBHKW91781221;     YFnTPlWdNyNtMxShlBHKW91781221 = YFnTPlWdNyNtMxShlBHKW7896512;     YFnTPlWdNyNtMxShlBHKW7896512 = YFnTPlWdNyNtMxShlBHKW58819096;     YFnTPlWdNyNtMxShlBHKW58819096 = YFnTPlWdNyNtMxShlBHKW15275410;     YFnTPlWdNyNtMxShlBHKW15275410 = YFnTPlWdNyNtMxShlBHKW18564225;     YFnTPlWdNyNtMxShlBHKW18564225 = YFnTPlWdNyNtMxShlBHKW31634652;     YFnTPlWdNyNtMxShlBHKW31634652 = YFnTPlWdNyNtMxShlBHKW352801;     YFnTPlWdNyNtMxShlBHKW352801 = YFnTPlWdNyNtMxShlBHKW91451599;     YFnTPlWdNyNtMxShlBHKW91451599 = YFnTPlWdNyNtMxShlBHKW51332492;     YFnTPlWdNyNtMxShlBHKW51332492 = YFnTPlWdNyNtMxShlBHKW63459115;     YFnTPlWdNyNtMxShlBHKW63459115 = YFnTPlWdNyNtMxShlBHKW75734222;     YFnTPlWdNyNtMxShlBHKW75734222 = YFnTPlWdNyNtMxShlBHKW12510832;     YFnTPlWdNyNtMxShlBHKW12510832 = YFnTPlWdNyNtMxShlBHKW87020834;     YFnTPlWdNyNtMxShlBHKW87020834 = YFnTPlWdNyNtMxShlBHKW41394512;     YFnTPlWdNyNtMxShlBHKW41394512 = YFnTPlWdNyNtMxShlBHKW33204989;     YFnTPlWdNyNtMxShlBHKW33204989 = YFnTPlWdNyNtMxShlBHKW90468947;     YFnTPlWdNyNtMxShlBHKW90468947 = YFnTPlWdNyNtMxShlBHKW20551389;     YFnTPlWdNyNtMxShlBHKW20551389 = YFnTPlWdNyNtMxShlBHKW39870078;     YFnTPlWdNyNtMxShlBHKW39870078 = YFnTPlWdNyNtMxShlBHKW48263394;     YFnTPlWdNyNtMxShlBHKW48263394 = YFnTPlWdNyNtMxShlBHKW10273375;     YFnTPlWdNyNtMxShlBHKW10273375 = YFnTPlWdNyNtMxShlBHKW14484485;     YFnTPlWdNyNtMxShlBHKW14484485 = YFnTPlWdNyNtMxShlBHKW6144943;     YFnTPlWdNyNtMxShlBHKW6144943 = YFnTPlWdNyNtMxShlBHKW60129592;     YFnTPlWdNyNtMxShlBHKW60129592 = YFnTPlWdNyNtMxShlBHKW86404126;     YFnTPlWdNyNtMxShlBHKW86404126 = YFnTPlWdNyNtMxShlBHKW45900329;     YFnTPlWdNyNtMxShlBHKW45900329 = YFnTPlWdNyNtMxShlBHKW29441274;     YFnTPlWdNyNtMxShlBHKW29441274 = YFnTPlWdNyNtMxShlBHKW90123552;     YFnTPlWdNyNtMxShlBHKW90123552 = YFnTPlWdNyNtMxShlBHKW13184319;     YFnTPlWdNyNtMxShlBHKW13184319 = YFnTPlWdNyNtMxShlBHKW43789928;     YFnTPlWdNyNtMxShlBHKW43789928 = YFnTPlWdNyNtMxShlBHKW49412904;     YFnTPlWdNyNtMxShlBHKW49412904 = YFnTPlWdNyNtMxShlBHKW75401211;     YFnTPlWdNyNtMxShlBHKW75401211 = YFnTPlWdNyNtMxShlBHKW42252927;     YFnTPlWdNyNtMxShlBHKW42252927 = YFnTPlWdNyNtMxShlBHKW18660952;     YFnTPlWdNyNtMxShlBHKW18660952 = YFnTPlWdNyNtMxShlBHKW73687655;     YFnTPlWdNyNtMxShlBHKW73687655 = YFnTPlWdNyNtMxShlBHKW6238489;     YFnTPlWdNyNtMxShlBHKW6238489 = YFnTPlWdNyNtMxShlBHKW64901558;     YFnTPlWdNyNtMxShlBHKW64901558 = YFnTPlWdNyNtMxShlBHKW1719896;     YFnTPlWdNyNtMxShlBHKW1719896 = YFnTPlWdNyNtMxShlBHKW87192421;     YFnTPlWdNyNtMxShlBHKW87192421 = YFnTPlWdNyNtMxShlBHKW11908040;     YFnTPlWdNyNtMxShlBHKW11908040 = YFnTPlWdNyNtMxShlBHKW27105735;     YFnTPlWdNyNtMxShlBHKW27105735 = YFnTPlWdNyNtMxShlBHKW5343084;     YFnTPlWdNyNtMxShlBHKW5343084 = YFnTPlWdNyNtMxShlBHKW71101666;     YFnTPlWdNyNtMxShlBHKW71101666 = YFnTPlWdNyNtMxShlBHKW10388421;     YFnTPlWdNyNtMxShlBHKW10388421 = YFnTPlWdNyNtMxShlBHKW1066876;     YFnTPlWdNyNtMxShlBHKW1066876 = YFnTPlWdNyNtMxShlBHKW89601430;     YFnTPlWdNyNtMxShlBHKW89601430 = YFnTPlWdNyNtMxShlBHKW4394241;     YFnTPlWdNyNtMxShlBHKW4394241 = YFnTPlWdNyNtMxShlBHKW45086197;     YFnTPlWdNyNtMxShlBHKW45086197 = YFnTPlWdNyNtMxShlBHKW37533678;     YFnTPlWdNyNtMxShlBHKW37533678 = YFnTPlWdNyNtMxShlBHKW66242876;     YFnTPlWdNyNtMxShlBHKW66242876 = YFnTPlWdNyNtMxShlBHKW17647645;     YFnTPlWdNyNtMxShlBHKW17647645 = YFnTPlWdNyNtMxShlBHKW23760101;     YFnTPlWdNyNtMxShlBHKW23760101 = YFnTPlWdNyNtMxShlBHKW13325175;     YFnTPlWdNyNtMxShlBHKW13325175 = YFnTPlWdNyNtMxShlBHKW43409059;     YFnTPlWdNyNtMxShlBHKW43409059 = YFnTPlWdNyNtMxShlBHKW53093916;     YFnTPlWdNyNtMxShlBHKW53093916 = YFnTPlWdNyNtMxShlBHKW75088609;     YFnTPlWdNyNtMxShlBHKW75088609 = YFnTPlWdNyNtMxShlBHKW85636278;     YFnTPlWdNyNtMxShlBHKW85636278 = YFnTPlWdNyNtMxShlBHKW47766919;     YFnTPlWdNyNtMxShlBHKW47766919 = YFnTPlWdNyNtMxShlBHKW72414970;     YFnTPlWdNyNtMxShlBHKW72414970 = YFnTPlWdNyNtMxShlBHKW69375080;     YFnTPlWdNyNtMxShlBHKW69375080 = YFnTPlWdNyNtMxShlBHKW89122951;     YFnTPlWdNyNtMxShlBHKW89122951 = YFnTPlWdNyNtMxShlBHKW41511099;     YFnTPlWdNyNtMxShlBHKW41511099 = YFnTPlWdNyNtMxShlBHKW87168482;     YFnTPlWdNyNtMxShlBHKW87168482 = YFnTPlWdNyNtMxShlBHKW47661672;     YFnTPlWdNyNtMxShlBHKW47661672 = YFnTPlWdNyNtMxShlBHKW1919589;     YFnTPlWdNyNtMxShlBHKW1919589 = YFnTPlWdNyNtMxShlBHKW88057903;     YFnTPlWdNyNtMxShlBHKW88057903 = YFnTPlWdNyNtMxShlBHKW33481295;     YFnTPlWdNyNtMxShlBHKW33481295 = YFnTPlWdNyNtMxShlBHKW93849880;     YFnTPlWdNyNtMxShlBHKW93849880 = YFnTPlWdNyNtMxShlBHKW13333179;     YFnTPlWdNyNtMxShlBHKW13333179 = YFnTPlWdNyNtMxShlBHKW35156023;     YFnTPlWdNyNtMxShlBHKW35156023 = YFnTPlWdNyNtMxShlBHKW68303430;     YFnTPlWdNyNtMxShlBHKW68303430 = YFnTPlWdNyNtMxShlBHKW88749051;     YFnTPlWdNyNtMxShlBHKW88749051 = YFnTPlWdNyNtMxShlBHKW33358968;     YFnTPlWdNyNtMxShlBHKW33358968 = YFnTPlWdNyNtMxShlBHKW27962039;     YFnTPlWdNyNtMxShlBHKW27962039 = YFnTPlWdNyNtMxShlBHKW21157659;     YFnTPlWdNyNtMxShlBHKW21157659 = YFnTPlWdNyNtMxShlBHKW4930292;     YFnTPlWdNyNtMxShlBHKW4930292 = YFnTPlWdNyNtMxShlBHKW43382819;     YFnTPlWdNyNtMxShlBHKW43382819 = YFnTPlWdNyNtMxShlBHKW95756522;     YFnTPlWdNyNtMxShlBHKW95756522 = YFnTPlWdNyNtMxShlBHKW59062716;     YFnTPlWdNyNtMxShlBHKW59062716 = YFnTPlWdNyNtMxShlBHKW96802695;     YFnTPlWdNyNtMxShlBHKW96802695 = YFnTPlWdNyNtMxShlBHKW41506089;     YFnTPlWdNyNtMxShlBHKW41506089 = YFnTPlWdNyNtMxShlBHKW84355077;     YFnTPlWdNyNtMxShlBHKW84355077 = YFnTPlWdNyNtMxShlBHKW60538991;}
// Junk Finished
