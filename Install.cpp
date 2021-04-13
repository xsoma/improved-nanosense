#include "Install.hpp"
#include "Gamehooking.hpp"
#include "helpers/Utils.hpp"
#include "features/Glow.hpp"
#include "features/PlayerHurt.hpp"
#include "features/BulletImpact.hpp"
#include "features/KitParser.hpp"
#include <thread>
#include <chrono>
#include "Handlers.h"
#include "helpers/sha256.h"
#include "helpers/antileak.h"

IVEngineClient			*g_EngineClient = nullptr;
IBaseClientDLL			*g_CHLClient = nullptr;
IClientEntityList		*g_EntityList = nullptr;
CGlobalVarsBase			*g_GlobalVars = nullptr;
IEngineTrace			*g_EngineTrace = nullptr;
ICvar					*g_CVar = nullptr;
IPanel					*g_VGuiPanel = nullptr;
IClientMode				*g_ClientMode = nullptr;
IVDebugOverlay			*g_DebugOverlay = nullptr;
ISurface				*g_VGuiSurface = nullptr;
CInput					*g_Input = nullptr;
IVModelInfoClient		*g_MdlInfo = nullptr;
IVModelRender			*g_MdlRender = nullptr;
IVRenderView			*g_RenderView = nullptr;
IMaterialSystem			*g_MatSystem = nullptr;
IGameEventManager2		*g_GameEvents = nullptr;
IMoveHelper				*g_MoveHelper = nullptr;
IMDLCache				*g_MdlCache = nullptr;
IPrediction				*g_Prediction = nullptr;
CGameMovement			*g_GameMovement = nullptr;
IEngineSound			*g_EngineSound = nullptr;
CGlowObjectManager		*g_GlowObjManager = nullptr;
CClientState			*g_ClientState = nullptr;
IPhysicsSurfaceProps	*g_PhysSurface = nullptr;
IInputSystem			*g_InputSystem = nullptr;
DWORD					*g_InputInternal = nullptr;
//IMemAlloc				*g_pMemAlloc = nullptr;
IViewRenderBeams	    *g_RenderBeams = nullptr;
ILocalize				*g_Localize = nullptr;
C_BasePlayer		    *g_LocalPlayer = nullptr;
IMemAlloc* g_pMemAlloc;

namespace Offsets
{
	DWORD invalidateBoneCache = 0x00;
	DWORD smokeCount = 0x00;
	DWORD playerResource = 0x00;
	DWORD bOverridePostProcessingDisable = 0x00;
	DWORD getSequenceActivity = 0x00;
	DWORD lgtSmoke = 0x00;
	DWORD dwCCSPlayerRenderablevftable = 0x00;
	DWORD reevauluate_anim_lod = 0x00;
}

std::unique_ptr<ShadowVTManager> g_pVguiPanelHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pClientModeHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pVguiSurfHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pD3DDevHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pClientHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pMaterialSystemHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pDMEHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pInputInternalHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pSceneEndHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pFireBulletHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pPredictionHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pConvarHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pClientStateHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pNetChannelHook = nullptr;
std::unique_ptr<ShadowVTManager> g_pEngineClientHook = nullptr;

PaintTraverse_t o_PaintTraverse = nullptr;
CreateMove_t o_CreateMove = nullptr;
EndScene_t o_EndScene = nullptr;
Reset_t o_Reset = nullptr;
FrameStageNotify_t o_FrameStageNotify = nullptr;
FireEventClientSide_t o_FireEventClientSide = nullptr;
BeginFrame_t o_BeginFrame = nullptr;
OverrideView_t o_OverrideView = nullptr;
SetMouseCodeState_t o_SetMouseCodeState = nullptr;
SetKeyCodeState_t o_SetKeyCodeState = nullptr;
FireBullets_t o_FireBullets = nullptr;
InPrediction_t o_OriginalInPrediction = nullptr;
TempEntities_t o_TempEntities = nullptr;
SceneEnd_t o_SceneEnd = nullptr;
SetupBones_t o_SetupBones = nullptr;
GetBool_t o_GetBool = nullptr;
GetViewmodelFov_t o_GetViewmodelFov = nullptr;
RunCommand_t o_RunCommand = nullptr;
SendDatagram_t o_SendDatagram = nullptr;
WriteUsercmdDeltaToBuffer_t o_WriteUsercmdDeltaToBuffer = nullptr;
IsBoxVisible_t o_IsBoxVisible = nullptr;
IsHLTV_t o_IsHLTV = nullptr;
CusorFunc_t o_LockCursor = nullptr;
CusorFunc_t o_UnlockCursor = nullptr;
DrawModelExecute_t o_DrawModelExecute = nullptr;
DispatchUserMessage_t o_DispatchUserMessage = nullptr;

RecvVarProxyFn o_didSmokeEffect = nullptr;
RecvVarProxyFn o_nSequence = nullptr;

HWND window = nullptr;
WNDPROC oldWindowProc = nullptr;

unsigned long __stdcall Installer::Loadvsonyp0wer(void *unused)
{

	


	FUNCTION_GUARD;

	while (!GetModuleHandleA("serverbrowser.dll"))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	g_CHLClient = Iface::IfaceMngr::getIface<IBaseClientDLL>("client.dll", "VClient0");
	g_EntityList = Iface::IfaceMngr::getIface<IClientEntityList>("client.dll", "VClientEntityList");
	g_Prediction = Iface::IfaceMngr::getIface<IPrediction>("client.dll", "VClientPrediction");
	g_GameMovement = Iface::IfaceMngr::getIface<CGameMovement>("client.dll", "GameMovement");
	g_MdlCache = Iface::IfaceMngr::getIface<IMDLCache>("datacache.dll", "MDLCache");
	g_EngineClient = Iface::IfaceMngr::getIface<IVEngineClient>("engine.dll", "VEngineClient");
	g_MdlInfo = Iface::IfaceMngr::getIface<IVModelInfoClient>("engine.dll", "VModelInfoClient");
	g_MdlRender = Iface::IfaceMngr::getIface<IVModelRender>("engine.dll", "VEngineModel");
	g_RenderView = Iface::IfaceMngr::getIface<IVRenderView>("engine.dll", "VEngineRenderView");
	g_EngineTrace = Iface::IfaceMngr::getIface<IEngineTrace>("engine.dll", "EngineTraceClient");
	g_DebugOverlay = Iface::IfaceMngr::getIface<IVDebugOverlay>("engine.dll", "VDebugOverlay");
	g_GameEvents = Iface::IfaceMngr::getIface<IGameEventManager2>("engine.dll", "GAMEEVENTSMANAGER002", true);
	g_EngineSound = Iface::IfaceMngr::getIface<IEngineSound>("engine.dll", "IEngineSoundClient");
	g_MatSystem = Iface::IfaceMngr::getIface<IMaterialSystem>("materialsystem.dll", "VMaterialSystem");
	g_CVar = Iface::IfaceMngr::getIface<ICvar>("vstdlib.dll", "VEngineCvar");
	g_VGuiPanel = Iface::IfaceMngr::getIface<IPanel>("vgui2.dll", "VGUI_Panel");
	g_VGuiSurface = Iface::IfaceMngr::getIface<ISurface>("vguimatsurface.dll", "VGUI_Surface");
	g_PhysSurface = Iface::IfaceMngr::getIface<IPhysicsSurfaceProps>("vphysics.dll", "VPhysicsSurfaceProps");
	g_InputSystem = Iface::IfaceMngr::getIface<IInputSystem>("inputsystem.dll", "InputSystemVersion");
	g_InputInternal = Iface::IfaceMngr::getIface<DWORD>("vgui2.dll", "VGUI_InputInternal");
	g_pMemAlloc = *(IMemAlloc * *)GetProcAddress(GetModuleHandleW(L"tier0.dll"), "g_pMemAlloc");
	g_Localize = Iface::IfaceMngr::getIface<ILocalize>("localize.dll", "Localize_");
	g_GlobalVars = **(CGlobalVarsBase***)((*(DWORD**)(g_CHLClient))[11] + 10); // Œÿ»¡ ¿
	g_ClientMode = **(IClientMode***)((*(DWORD**)g_CHLClient)[10] + 0x5);
	//g_pMemAlloc = *(IMemAlloc**)(GetProcAddress(GetModuleHandle("tier0.dll"), "g_pMemAlloc"));

	auto client = GetModuleHandle("client.dll");
	auto engine = GetModuleHandle("engine.dll");
	auto dx9api = GetModuleHandle("shaderapidx9.dll");

	g_ClientState = **(CClientState***)(Utils::PatternScan(engine, "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1); //+
	g_GlowObjManager = *(CGlowObjectManager**)(Utils::PatternScan(client, "0F 11 05 ? ? ? ? 83 C8 01") + 3); //+
	g_MoveHelper = **(IMoveHelper***)(Utils::PatternScan(client, "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01") + 2); //+
	g_RenderBeams = *(IViewRenderBeams**)(Utils::PatternScan(client, "A1 ? ? ? ? FF 10 A1 ? ? ? ? B9") + 1); //+
	g_Input = *(CInput**)(Utils::PatternScan(client, "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 1); //+

	auto D3DDevice9 = **(IDirect3DDevice9***)(Utils::PatternScan(dx9api, "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1); //+
	auto dwFireBullets = *(DWORD**)(Utils::PatternScan(client, "55 8B EC 51 53 56 8B F1 BB ? ? ? ? B8") + 0x131); //+

	Offsets::lgtSmoke = (DWORD)Utils::PatternScan(client, "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");
	Offsets::getSequenceActivity = (DWORD)Utils::PatternScan(client, "55 8B EC 83 7D 08 FF 56 8B F1 74"); //+
	Offsets::smokeCount = *(DWORD*)(Utils::PatternScan(client, "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0") + 0x8);
	Offsets::invalidateBoneCache = (DWORD)Utils::PatternScan(client, "55 8B 0D ? ? ? ? 89 E5 8B 45 ? 8D 51");
	Offsets::playerResource = *(DWORD*)(Utils::PatternScan(client, "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7") + 2);
	Offsets::bOverridePostProcessingDisable = *(DWORD*)(Utils::PatternScan(client, "80 3D ? ? ? ? ? 53 56 57 0F 85") + 2);
	Offsets::dwCCSPlayerRenderablevftable = *(DWORD*)(Utils::PatternScan(client, "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C") + 0x4E);
	Offsets::reevauluate_anim_lod = (DWORD)Utils::PatternScan(client, "84 C0 0F 85 ? ? ? ? A1 ? ? ? ? 8B B7");

	NetMngr::Get().init();

	g_pDMEHook = std::make_unique<ShadowVTManager>();
	g_pD3DDevHook = std::make_unique<ShadowVTManager>();
	g_pClientHook = std::make_unique<ShadowVTManager>();
	g_pSceneEndHook = std::make_unique<ShadowVTManager>();
	g_pVguiPanelHook = std::make_unique<ShadowVTManager>();
	g_pVguiSurfHook = std::make_unique<ShadowVTManager>();
	g_pClientModeHook = std::make_unique<ShadowVTManager>();
	g_pPredictionHook = std::make_unique<ShadowVTManager>();
	g_pFireBulletHook = std::make_unique<ShadowVTManager>();
	g_pMaterialSystemHook = std::make_unique<ShadowVTManager>();
	g_pInputInternalHook = std::make_unique<ShadowVTManager>();
	g_pConvarHook = std::make_unique<ShadowVTManager>();
	g_pClientStateHook = std::make_unique<ShadowVTManager>();
	g_pEngineClientHook = std::make_unique<ShadowVTManager>();

	g_pFireBulletHook->Setup((PDWORD)dwFireBullets);
	g_pDMEHook->Setup(g_MdlRender);
	g_pD3DDevHook->Setup(D3DDevice9);
	g_pClientHook->Setup(g_CHLClient);
	g_pSceneEndHook->Setup(g_RenderView);
	g_pVguiPanelHook->Setup(g_VGuiPanel);
	g_pVguiSurfHook->Setup(g_VGuiSurface);
	g_pClientModeHook->Setup(g_ClientMode);
	g_pPredictionHook->Setup(g_Prediction);
	g_pMaterialSystemHook->Setup(g_MatSystem);
	g_pInputInternalHook->Setup(g_InputInternal);
	g_pConvarHook->Setup(g_CVar->FindVar("sv_cheats"));
	g_pEngineClientHook->Setup(g_EngineClient);


	
	d3d9hook.ini(D3DDevice9);


	g_pFireBulletHook->Hook(7, (FireBullets_t)Handlers::TEFireBulletsPostDataUpdate_h);
	g_pInputInternalHook->Hook(92, Handlers::SetMouseCodeState_h);
	g_pInputInternalHook->Hook(91, Handlers::SetKeyCodeState_h);
	g_pClientModeHook->Hook(35, Handlers::GetViewModelFov_h);
	g_pClientModeHook->Hook(18, Handlers::OverrideView_h);
	g_pClientModeHook->Hook(24, Handlers::CreateMove_h);
	g_pClientHook->Hook(37, Handlers::FrameStageNotify_h);
	g_pPredictionHook->Hook(14, Handlers::InPrediction_h);
	g_pPredictionHook->Hook(19, Handlers::RunCommand_h);
	g_pVguiPanelHook->Hook(41, Handlers::PaintTraverse_h);
	g_pMaterialSystemHook->Hook(42, Handlers::BeginFrame_h);
	g_pConvarHook->Hook(13, Handlers::GetBool_SVCheats_h);
	g_pVguiSurfHook->Hook(67, Handlers::LockCursor_h);
	g_pSceneEndHook->Hook(9, Handlers::SceneEnd_h);
	g_pEngineClientHook->Hook(32, Handlers::IsBoxVisible_h);
	g_pEngineClientHook->Hook(93, Handlers::IsHLTV_h);


	o_FireBullets = g_pFireBulletHook->GetOriginal<FireBullets_t>(7);
	o_SetMouseCodeState = g_pInputInternalHook->GetOriginal<SetMouseCodeState_t>(92);
	o_SetKeyCodeState = g_pInputInternalHook->GetOriginal<SetKeyCodeState_t>(91);
	o_GetViewmodelFov = g_pClientModeHook->GetOriginal<GetViewmodelFov_t>(35);
	o_OverrideView = g_pClientModeHook->GetOriginal<OverrideView_t>(18);
	o_CreateMove = g_pClientModeHook->GetOriginal<CreateMove_t>(24);
	o_FrameStageNotify = g_pClientHook->GetOriginal<FrameStageNotify_t>(37);
	o_OriginalInPrediction = g_pPredictionHook->GetOriginal<InPrediction_t>(14);
	o_RunCommand = g_pPredictionHook->GetOriginal<RunCommand_t>(19);
	o_PaintTraverse = g_pVguiPanelHook->GetOriginal<PaintTraverse_t>(41);
	o_BeginFrame = g_pMaterialSystemHook->GetOriginal<BeginFrame_t>(42);
	o_GetBool = g_pConvarHook->GetOriginal<GetBool_t>(13);
	o_SceneEnd = g_pSceneEndHook->GetOriginal<SceneEnd_t>(9);
	o_EndScene = g_pD3DDevHook->GetOriginal<EndScene_t>(42);
	o_Reset = g_pD3DDevHook->GetOriginal<Reset_t>(16);
	o_IsBoxVisible = g_pEngineClientHook->GetOriginal<IsBoxVisible_t>(32);
	o_IsHLTV = g_pEngineClientHook->GetOriginal<IsHLTV_t>(93);
	o_UnlockCursor = g_pVguiSurfHook->GetOriginal<CusorFunc_t>(66);
	o_LockCursor = g_pVguiSurfHook->GetOriginal<CusorFunc_t>(67);

#ifdef INSTANT_DEFUSE_PLANT_EXPLOIT // lmao
	o_WriteUsercmdDeltaToBuffer = g_pClientHook->Hook(24, (WriteUsercmdDeltaToBuffer_t)Handlers::WriteUsercmdDeltaToBuffer_h);
#endif

	InitializeKits();

	NetMngr::Get().hookProp("CSmokeGrenadeProjectile", "m_bDidSmokeEffect", Proxies::didSmokeEffect, o_didSmokeEffect);
	NetMngr::Get().hookProp("CBaseViewModel", "m_nSequence", Proxies::nSequence, o_nSequence);
	PlayerHurtEvent::Get().RegisterSelf();
	BulletImpactEvent::Get().RegisterSelf();

	/*
	Utils::AttachConsole();
	Utils::ConsolePrint(true, "[//////////////////////////////////////////////////////] -\n");
	Utils::ConsolePrint(true, "Activating the dark magic of ChaiSense made %s at %s...\n", __DATE__, __TIME__);
	Utils::ConsolePrint(true, "Created by MisterStefan -\n");
	Utils::ConsolePrint(true, "[//////////////////////////////////////////////////////] -\n");
	Utils::ConsolePrint(true, "Loaded Successfully.\n");
	Utils::ConsolePrint(true, "Go Get Banned Cuz Gs Pasta!\n");
	*/

	return EXIT_SUCCESS;
}


void Installer::UnLoadvsonyp0wer() //here
{
//	Glow::ClearGlow();
	PlayerHurtEvent::Get().UnregisterSelf();
	BulletImpactEvent::Get().UnregisterSelf();
	SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oldWindowProc);
	g_pDMEHook->RestoreTable();
	g_pD3DDevHook->RestoreTable();
	g_pClientHook->RestoreTable();
	g_pSceneEndHook->RestoreTable();
	g_pVguiPanelHook->RestoreTable();
	g_pVguiSurfHook->RestoreTable();
	g_pClientModeHook->RestoreTable();
	g_pPredictionHook->RestoreTable();
	g_pMaterialSystemHook->RestoreTable();
	g_pInputInternalHook->RestoreTable();
	g_pConvarHook->RestoreTable();
	g_pEngineClientHook->RestoreTable();
	g_pClientStateHook->RestoreTable();
}












































































































































// Junk Code By Troll Face & Thaisen's Gen
void lORHiOWJImLOwCTRBkatzBtsPpSynueyZvTcKaak71821329() {     int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq45034487 = -616949556;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq88631494 = -690196180;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq60077603 = 54936740;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq16964341 = -767017883;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq40413876 = -741547576;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50983749 = -683253361;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq68221881 = -713778981;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq81787684 = -564606733;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq68509420 = -632584764;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq1026585 = -429263045;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq91362288 = -55691469;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq51527361 = -545404605;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq65227708 = 20191381;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq55122976 = -40876176;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq44460741 = -519601219;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq35228956 = -956127906;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq60623852 = -129511577;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq84878620 = -391482760;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq13489346 = -257861688;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56935090 = -476911912;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq69543269 = -949667973;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq53753901 = -401275968;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq98164502 = -185463114;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq12125620 = -885785393;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq19680259 = -440377153;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq21702236 = -258091524;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq40005743 = -604301658;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq31350520 = -493704212;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq88662604 = -978493325;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq39746812 = -714206772;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq17795075 = -731956690;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq39300551 = -55348364;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq76760877 = -83179891;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq46728480 = -446141815;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq18010209 = -172049366;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq92026464 = -484374562;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq7867358 = -878050759;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10967679 = -597885566;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq28781685 = -842593787;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq61256100 = -642496257;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq33912625 = -893762500;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq19150388 = -913046182;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq48319544 = -43707951;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq49024365 = 18217672;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56918306 = -550282694;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq9589252 = -337441199;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq46663212 = -261122277;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq33016108 = -708604259;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq73614469 = -269628284;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50857273 = -490813068;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq78451322 = -803034528;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq82117027 = -222238853;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq91814451 = -427012026;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56423851 = -799366683;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq2278060 = -996909771;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq91280586 = -115673588;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq90466991 = -404733067;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq47951984 = -59277868;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq97284082 = -226640731;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq18711640 = -383456053;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10978007 = 21048296;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq36871362 = -120074770;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq93125079 = -586113408;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq28762609 = -918377992;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq83231509 = -697306356;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq52061738 = 99656895;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq74766483 = -362224715;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq18499229 = -533666805;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq37112768 = -868826811;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq52434277 = 64773342;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq27361599 = 21922852;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq49656173 = -531626011;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56096935 = -548888974;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq52233246 = -615365432;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq23022466 = -583149413;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50392882 = 63378209;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq5434357 = -257568018;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq49140138 = -103680787;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq55207313 = -235502699;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10091008 = -2935954;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq75039023 = -996969247;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq6989635 = -895697399;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq57736050 = -124075929;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq37805331 = -387680257;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq61295489 = -911172244;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq35678047 = -409717837;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq47486099 = -628336339;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq20337026 = -283813209;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq44450421 = -449232044;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq26729622 = 43624221;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq1559473 = 20358504;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq59915373 = -718772892;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq13683597 = -271244836;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10070046 = -359137735;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50278093 = -563544554;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq97041263 = -673687731;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq26025308 = -226932775;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq19556935 = -125329959;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq65792855 = -284475973;    int DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq4856569 = -616949556;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq45034487 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq88631494;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq88631494 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq60077603;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq60077603 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq16964341;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq16964341 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq40413876;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq40413876 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50983749;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50983749 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq68221881;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq68221881 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq81787684;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq81787684 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq68509420;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq68509420 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq1026585;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq1026585 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq91362288;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq91362288 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq51527361;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq51527361 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq65227708;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq65227708 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq55122976;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq55122976 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq44460741;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq44460741 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq35228956;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq35228956 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq60623852;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq60623852 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq84878620;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq84878620 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq13489346;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq13489346 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56935090;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56935090 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq69543269;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq69543269 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq53753901;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq53753901 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq98164502;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq98164502 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq12125620;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq12125620 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq19680259;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq19680259 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq21702236;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq21702236 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq40005743;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq40005743 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq31350520;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq31350520 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq88662604;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq88662604 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq39746812;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq39746812 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq17795075;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq17795075 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq39300551;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq39300551 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq76760877;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq76760877 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq46728480;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq46728480 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq18010209;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq18010209 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq92026464;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq92026464 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq7867358;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq7867358 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10967679;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10967679 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq28781685;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq28781685 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq61256100;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq61256100 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq33912625;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq33912625 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq19150388;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq19150388 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq48319544;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq48319544 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq49024365;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq49024365 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56918306;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56918306 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq9589252;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq9589252 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq46663212;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq46663212 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq33016108;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq33016108 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq73614469;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq73614469 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50857273;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50857273 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq78451322;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq78451322 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq82117027;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq82117027 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq91814451;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq91814451 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56423851;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56423851 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq2278060;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq2278060 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq91280586;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq91280586 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq90466991;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq90466991 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq47951984;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq47951984 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq97284082;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq97284082 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq18711640;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq18711640 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10978007;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10978007 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq36871362;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq36871362 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq93125079;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq93125079 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq28762609;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq28762609 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq83231509;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq83231509 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq52061738;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq52061738 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq74766483;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq74766483 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq18499229;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq18499229 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq37112768;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq37112768 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq52434277;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq52434277 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq27361599;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq27361599 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq49656173;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq49656173 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56096935;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq56096935 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq52233246;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq52233246 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq23022466;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq23022466 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50392882;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50392882 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq5434357;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq5434357 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq49140138;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq49140138 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq55207313;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq55207313 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10091008;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10091008 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq75039023;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq75039023 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq6989635;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq6989635 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq57736050;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq57736050 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq37805331;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq37805331 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq61295489;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq61295489 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq35678047;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq35678047 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq47486099;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq47486099 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq20337026;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq20337026 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq44450421;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq44450421 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq26729622;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq26729622 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq1559473;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq1559473 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq59915373;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq59915373 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq13683597;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq13683597 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10070046;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq10070046 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50278093;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq50278093 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq97041263;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq97041263 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq26025308;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq26025308 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq19556935;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq19556935 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq65792855;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq65792855 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq4856569;     DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq4856569 = DMmfkHJJINTSzkckVYrswYVOINoBjqGaKpqmPNHHgIpZbMypSiHlihoeqYXXSIaSGtHKtq45034487;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void yVeMioVCxqgkThBHYPDauABWjKLSalNeMcwlBQpVcjvMmwnPpIP28336152() {     float PyzgyUHcXdDcMcCXrlQGHZiyLFok1744310 = -257777297;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok79063997 = -329997563;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok61462554 = -522640169;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok22011160 = -811870605;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok27633655 = -478946585;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok55165568 = -807512006;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok13312054 = -567690124;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok11224695 = -558783242;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok48651338 = -921957038;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok43371677 = -828452376;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok54155306 = -32062275;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok42848003 = -100621659;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok34284973 = 89170143;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok53426242 = -434837475;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok1607681 = -111945211;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok45020343 = -327800703;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok9387315 = -970470316;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok26713535 = -253681595;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok2460763 = -103576565;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok6549074 = -45353088;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok75725019 = -890079132;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok460430 = -634640161;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok21217765 = -318027259;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok97624129 = -433735394;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok68461986 = 8708976;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok39823198 = -65351091;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok69631685 = -7407108;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok72730670 = -112885387;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok36910530 = -88293159;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok3847069 = -156277838;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok27830711 = -201169754;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok42088887 = -219847847;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok44031570 = -488584355;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok62449291 = -515029765;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok51615761 = -618344324;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok83876548 = -492142197;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok61945342 = -108686419;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok86350385 = -771263870;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok66938604 = -349916009;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok2556103 = -496064462;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok35214221 = -16006163;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok38709127 = -318407020;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok87170888 = -356422037;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok45133200 = 36672129;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok32801173 = -314287038;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok66291224 = -513044867;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok32744994 = 90005952;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok45206830 = -104349195;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok34833804 = 82354828;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok30443493 = -384954881;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok40535838 = 11431533;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok99075828 = 62026173;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok40015274 = -330390637;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok78429667 = -529132344;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok52611132 = -881838578;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok1283880 = -623137137;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok57846232 = 88029695;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok63838424 = 11095224;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok53549173 = -720579581;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok87810457 = -313595494;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok85533882 = -700104899;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok40581383 = -354804738;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok74314165 = -370490083;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok44804270 = -665679200;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok15540966 = -527282623;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok12066419 = -812214429;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok98816433 = -612037304;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok71835681 = -395800092;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok1810482 = -816493152;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok17731132 = -619803014;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok83075000 = -119114285;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok23036930 = -99206446;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok59774930 = -903765587;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok99904659 = -607512104;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok71334853 = 70653075;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok37015893 = -471672113;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok13289542 = -178218124;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok76084565 = -254699388;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok64822957 = -19448356;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok2170762 = -478246158;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok7078204 = -55357044;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok24424856 = -903057913;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok37896866 = -95240215;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok6467037 = -703338278;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok63311230 = -67709372;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok28754883 = -163195927;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok2073613 = -889457210;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok65601902 = -959452012;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok9838160 = -633191188;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok50331881 = -995207187;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok26030316 = -480171893;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok98106917 = -19781644;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok32801212 = 49315710;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok79128147 = 63679485;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok17022220 = -795959563;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok94632838 = -661201426;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok64394962 = -947916937;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok42366618 = -690742838;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok29592234 = -436045249;    float PyzgyUHcXdDcMcCXrlQGHZiyLFok20734754 = -257777297;     PyzgyUHcXdDcMcCXrlQGHZiyLFok1744310 = PyzgyUHcXdDcMcCXrlQGHZiyLFok79063997;     PyzgyUHcXdDcMcCXrlQGHZiyLFok79063997 = PyzgyUHcXdDcMcCXrlQGHZiyLFok61462554;     PyzgyUHcXdDcMcCXrlQGHZiyLFok61462554 = PyzgyUHcXdDcMcCXrlQGHZiyLFok22011160;     PyzgyUHcXdDcMcCXrlQGHZiyLFok22011160 = PyzgyUHcXdDcMcCXrlQGHZiyLFok27633655;     PyzgyUHcXdDcMcCXrlQGHZiyLFok27633655 = PyzgyUHcXdDcMcCXrlQGHZiyLFok55165568;     PyzgyUHcXdDcMcCXrlQGHZiyLFok55165568 = PyzgyUHcXdDcMcCXrlQGHZiyLFok13312054;     PyzgyUHcXdDcMcCXrlQGHZiyLFok13312054 = PyzgyUHcXdDcMcCXrlQGHZiyLFok11224695;     PyzgyUHcXdDcMcCXrlQGHZiyLFok11224695 = PyzgyUHcXdDcMcCXrlQGHZiyLFok48651338;     PyzgyUHcXdDcMcCXrlQGHZiyLFok48651338 = PyzgyUHcXdDcMcCXrlQGHZiyLFok43371677;     PyzgyUHcXdDcMcCXrlQGHZiyLFok43371677 = PyzgyUHcXdDcMcCXrlQGHZiyLFok54155306;     PyzgyUHcXdDcMcCXrlQGHZiyLFok54155306 = PyzgyUHcXdDcMcCXrlQGHZiyLFok42848003;     PyzgyUHcXdDcMcCXrlQGHZiyLFok42848003 = PyzgyUHcXdDcMcCXrlQGHZiyLFok34284973;     PyzgyUHcXdDcMcCXrlQGHZiyLFok34284973 = PyzgyUHcXdDcMcCXrlQGHZiyLFok53426242;     PyzgyUHcXdDcMcCXrlQGHZiyLFok53426242 = PyzgyUHcXdDcMcCXrlQGHZiyLFok1607681;     PyzgyUHcXdDcMcCXrlQGHZiyLFok1607681 = PyzgyUHcXdDcMcCXrlQGHZiyLFok45020343;     PyzgyUHcXdDcMcCXrlQGHZiyLFok45020343 = PyzgyUHcXdDcMcCXrlQGHZiyLFok9387315;     PyzgyUHcXdDcMcCXrlQGHZiyLFok9387315 = PyzgyUHcXdDcMcCXrlQGHZiyLFok26713535;     PyzgyUHcXdDcMcCXrlQGHZiyLFok26713535 = PyzgyUHcXdDcMcCXrlQGHZiyLFok2460763;     PyzgyUHcXdDcMcCXrlQGHZiyLFok2460763 = PyzgyUHcXdDcMcCXrlQGHZiyLFok6549074;     PyzgyUHcXdDcMcCXrlQGHZiyLFok6549074 = PyzgyUHcXdDcMcCXrlQGHZiyLFok75725019;     PyzgyUHcXdDcMcCXrlQGHZiyLFok75725019 = PyzgyUHcXdDcMcCXrlQGHZiyLFok460430;     PyzgyUHcXdDcMcCXrlQGHZiyLFok460430 = PyzgyUHcXdDcMcCXrlQGHZiyLFok21217765;     PyzgyUHcXdDcMcCXrlQGHZiyLFok21217765 = PyzgyUHcXdDcMcCXrlQGHZiyLFok97624129;     PyzgyUHcXdDcMcCXrlQGHZiyLFok97624129 = PyzgyUHcXdDcMcCXrlQGHZiyLFok68461986;     PyzgyUHcXdDcMcCXrlQGHZiyLFok68461986 = PyzgyUHcXdDcMcCXrlQGHZiyLFok39823198;     PyzgyUHcXdDcMcCXrlQGHZiyLFok39823198 = PyzgyUHcXdDcMcCXrlQGHZiyLFok69631685;     PyzgyUHcXdDcMcCXrlQGHZiyLFok69631685 = PyzgyUHcXdDcMcCXrlQGHZiyLFok72730670;     PyzgyUHcXdDcMcCXrlQGHZiyLFok72730670 = PyzgyUHcXdDcMcCXrlQGHZiyLFok36910530;     PyzgyUHcXdDcMcCXrlQGHZiyLFok36910530 = PyzgyUHcXdDcMcCXrlQGHZiyLFok3847069;     PyzgyUHcXdDcMcCXrlQGHZiyLFok3847069 = PyzgyUHcXdDcMcCXrlQGHZiyLFok27830711;     PyzgyUHcXdDcMcCXrlQGHZiyLFok27830711 = PyzgyUHcXdDcMcCXrlQGHZiyLFok42088887;     PyzgyUHcXdDcMcCXrlQGHZiyLFok42088887 = PyzgyUHcXdDcMcCXrlQGHZiyLFok44031570;     PyzgyUHcXdDcMcCXrlQGHZiyLFok44031570 = PyzgyUHcXdDcMcCXrlQGHZiyLFok62449291;     PyzgyUHcXdDcMcCXrlQGHZiyLFok62449291 = PyzgyUHcXdDcMcCXrlQGHZiyLFok51615761;     PyzgyUHcXdDcMcCXrlQGHZiyLFok51615761 = PyzgyUHcXdDcMcCXrlQGHZiyLFok83876548;     PyzgyUHcXdDcMcCXrlQGHZiyLFok83876548 = PyzgyUHcXdDcMcCXrlQGHZiyLFok61945342;     PyzgyUHcXdDcMcCXrlQGHZiyLFok61945342 = PyzgyUHcXdDcMcCXrlQGHZiyLFok86350385;     PyzgyUHcXdDcMcCXrlQGHZiyLFok86350385 = PyzgyUHcXdDcMcCXrlQGHZiyLFok66938604;     PyzgyUHcXdDcMcCXrlQGHZiyLFok66938604 = PyzgyUHcXdDcMcCXrlQGHZiyLFok2556103;     PyzgyUHcXdDcMcCXrlQGHZiyLFok2556103 = PyzgyUHcXdDcMcCXrlQGHZiyLFok35214221;     PyzgyUHcXdDcMcCXrlQGHZiyLFok35214221 = PyzgyUHcXdDcMcCXrlQGHZiyLFok38709127;     PyzgyUHcXdDcMcCXrlQGHZiyLFok38709127 = PyzgyUHcXdDcMcCXrlQGHZiyLFok87170888;     PyzgyUHcXdDcMcCXrlQGHZiyLFok87170888 = PyzgyUHcXdDcMcCXrlQGHZiyLFok45133200;     PyzgyUHcXdDcMcCXrlQGHZiyLFok45133200 = PyzgyUHcXdDcMcCXrlQGHZiyLFok32801173;     PyzgyUHcXdDcMcCXrlQGHZiyLFok32801173 = PyzgyUHcXdDcMcCXrlQGHZiyLFok66291224;     PyzgyUHcXdDcMcCXrlQGHZiyLFok66291224 = PyzgyUHcXdDcMcCXrlQGHZiyLFok32744994;     PyzgyUHcXdDcMcCXrlQGHZiyLFok32744994 = PyzgyUHcXdDcMcCXrlQGHZiyLFok45206830;     PyzgyUHcXdDcMcCXrlQGHZiyLFok45206830 = PyzgyUHcXdDcMcCXrlQGHZiyLFok34833804;     PyzgyUHcXdDcMcCXrlQGHZiyLFok34833804 = PyzgyUHcXdDcMcCXrlQGHZiyLFok30443493;     PyzgyUHcXdDcMcCXrlQGHZiyLFok30443493 = PyzgyUHcXdDcMcCXrlQGHZiyLFok40535838;     PyzgyUHcXdDcMcCXrlQGHZiyLFok40535838 = PyzgyUHcXdDcMcCXrlQGHZiyLFok99075828;     PyzgyUHcXdDcMcCXrlQGHZiyLFok99075828 = PyzgyUHcXdDcMcCXrlQGHZiyLFok40015274;     PyzgyUHcXdDcMcCXrlQGHZiyLFok40015274 = PyzgyUHcXdDcMcCXrlQGHZiyLFok78429667;     PyzgyUHcXdDcMcCXrlQGHZiyLFok78429667 = PyzgyUHcXdDcMcCXrlQGHZiyLFok52611132;     PyzgyUHcXdDcMcCXrlQGHZiyLFok52611132 = PyzgyUHcXdDcMcCXrlQGHZiyLFok1283880;     PyzgyUHcXdDcMcCXrlQGHZiyLFok1283880 = PyzgyUHcXdDcMcCXrlQGHZiyLFok57846232;     PyzgyUHcXdDcMcCXrlQGHZiyLFok57846232 = PyzgyUHcXdDcMcCXrlQGHZiyLFok63838424;     PyzgyUHcXdDcMcCXrlQGHZiyLFok63838424 = PyzgyUHcXdDcMcCXrlQGHZiyLFok53549173;     PyzgyUHcXdDcMcCXrlQGHZiyLFok53549173 = PyzgyUHcXdDcMcCXrlQGHZiyLFok87810457;     PyzgyUHcXdDcMcCXrlQGHZiyLFok87810457 = PyzgyUHcXdDcMcCXrlQGHZiyLFok85533882;     PyzgyUHcXdDcMcCXrlQGHZiyLFok85533882 = PyzgyUHcXdDcMcCXrlQGHZiyLFok40581383;     PyzgyUHcXdDcMcCXrlQGHZiyLFok40581383 = PyzgyUHcXdDcMcCXrlQGHZiyLFok74314165;     PyzgyUHcXdDcMcCXrlQGHZiyLFok74314165 = PyzgyUHcXdDcMcCXrlQGHZiyLFok44804270;     PyzgyUHcXdDcMcCXrlQGHZiyLFok44804270 = PyzgyUHcXdDcMcCXrlQGHZiyLFok15540966;     PyzgyUHcXdDcMcCXrlQGHZiyLFok15540966 = PyzgyUHcXdDcMcCXrlQGHZiyLFok12066419;     PyzgyUHcXdDcMcCXrlQGHZiyLFok12066419 = PyzgyUHcXdDcMcCXrlQGHZiyLFok98816433;     PyzgyUHcXdDcMcCXrlQGHZiyLFok98816433 = PyzgyUHcXdDcMcCXrlQGHZiyLFok71835681;     PyzgyUHcXdDcMcCXrlQGHZiyLFok71835681 = PyzgyUHcXdDcMcCXrlQGHZiyLFok1810482;     PyzgyUHcXdDcMcCXrlQGHZiyLFok1810482 = PyzgyUHcXdDcMcCXrlQGHZiyLFok17731132;     PyzgyUHcXdDcMcCXrlQGHZiyLFok17731132 = PyzgyUHcXdDcMcCXrlQGHZiyLFok83075000;     PyzgyUHcXdDcMcCXrlQGHZiyLFok83075000 = PyzgyUHcXdDcMcCXrlQGHZiyLFok23036930;     PyzgyUHcXdDcMcCXrlQGHZiyLFok23036930 = PyzgyUHcXdDcMcCXrlQGHZiyLFok59774930;     PyzgyUHcXdDcMcCXrlQGHZiyLFok59774930 = PyzgyUHcXdDcMcCXrlQGHZiyLFok99904659;     PyzgyUHcXdDcMcCXrlQGHZiyLFok99904659 = PyzgyUHcXdDcMcCXrlQGHZiyLFok71334853;     PyzgyUHcXdDcMcCXrlQGHZiyLFok71334853 = PyzgyUHcXdDcMcCXrlQGHZiyLFok37015893;     PyzgyUHcXdDcMcCXrlQGHZiyLFok37015893 = PyzgyUHcXdDcMcCXrlQGHZiyLFok13289542;     PyzgyUHcXdDcMcCXrlQGHZiyLFok13289542 = PyzgyUHcXdDcMcCXrlQGHZiyLFok76084565;     PyzgyUHcXdDcMcCXrlQGHZiyLFok76084565 = PyzgyUHcXdDcMcCXrlQGHZiyLFok64822957;     PyzgyUHcXdDcMcCXrlQGHZiyLFok64822957 = PyzgyUHcXdDcMcCXrlQGHZiyLFok2170762;     PyzgyUHcXdDcMcCXrlQGHZiyLFok2170762 = PyzgyUHcXdDcMcCXrlQGHZiyLFok7078204;     PyzgyUHcXdDcMcCXrlQGHZiyLFok7078204 = PyzgyUHcXdDcMcCXrlQGHZiyLFok24424856;     PyzgyUHcXdDcMcCXrlQGHZiyLFok24424856 = PyzgyUHcXdDcMcCXrlQGHZiyLFok37896866;     PyzgyUHcXdDcMcCXrlQGHZiyLFok37896866 = PyzgyUHcXdDcMcCXrlQGHZiyLFok6467037;     PyzgyUHcXdDcMcCXrlQGHZiyLFok6467037 = PyzgyUHcXdDcMcCXrlQGHZiyLFok63311230;     PyzgyUHcXdDcMcCXrlQGHZiyLFok63311230 = PyzgyUHcXdDcMcCXrlQGHZiyLFok28754883;     PyzgyUHcXdDcMcCXrlQGHZiyLFok28754883 = PyzgyUHcXdDcMcCXrlQGHZiyLFok2073613;     PyzgyUHcXdDcMcCXrlQGHZiyLFok2073613 = PyzgyUHcXdDcMcCXrlQGHZiyLFok65601902;     PyzgyUHcXdDcMcCXrlQGHZiyLFok65601902 = PyzgyUHcXdDcMcCXrlQGHZiyLFok9838160;     PyzgyUHcXdDcMcCXrlQGHZiyLFok9838160 = PyzgyUHcXdDcMcCXrlQGHZiyLFok50331881;     PyzgyUHcXdDcMcCXrlQGHZiyLFok50331881 = PyzgyUHcXdDcMcCXrlQGHZiyLFok26030316;     PyzgyUHcXdDcMcCXrlQGHZiyLFok26030316 = PyzgyUHcXdDcMcCXrlQGHZiyLFok98106917;     PyzgyUHcXdDcMcCXrlQGHZiyLFok98106917 = PyzgyUHcXdDcMcCXrlQGHZiyLFok32801212;     PyzgyUHcXdDcMcCXrlQGHZiyLFok32801212 = PyzgyUHcXdDcMcCXrlQGHZiyLFok79128147;     PyzgyUHcXdDcMcCXrlQGHZiyLFok79128147 = PyzgyUHcXdDcMcCXrlQGHZiyLFok17022220;     PyzgyUHcXdDcMcCXrlQGHZiyLFok17022220 = PyzgyUHcXdDcMcCXrlQGHZiyLFok94632838;     PyzgyUHcXdDcMcCXrlQGHZiyLFok94632838 = PyzgyUHcXdDcMcCXrlQGHZiyLFok64394962;     PyzgyUHcXdDcMcCXrlQGHZiyLFok64394962 = PyzgyUHcXdDcMcCXrlQGHZiyLFok42366618;     PyzgyUHcXdDcMcCXrlQGHZiyLFok42366618 = PyzgyUHcXdDcMcCXrlQGHZiyLFok29592234;     PyzgyUHcXdDcMcCXrlQGHZiyLFok29592234 = PyzgyUHcXdDcMcCXrlQGHZiyLFok20734754;     PyzgyUHcXdDcMcCXrlQGHZiyLFok20734754 = PyzgyUHcXdDcMcCXrlQGHZiyLFok1744310;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void rNvyPnxMkIhdCdwPQZSNpTrtK2741947() {     long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl24160863 = -111787446;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl52947945 = -632149237;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl17204289 = -182319535;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl35088379 = -640623664;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl3882252 = -5857814;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl83247142 = -753959372;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl84797010 = -694820297;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl10460931 = -898222357;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl89989359 = -125680546;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl6610458 = -34682909;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl22674151 = -246885179;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl14954568 = 69255474;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl92158832 = -209579176;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl47091243 = -510348740;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl96976961 = -49990742;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl67635926 = -910641586;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl43800643 = 24194982;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl53270466 = -547810799;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl54132493 = -12162435;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl88704093 = -145130287;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl30590506 = -229464945;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl80473846 = -78327831;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl8932041 = -420361811;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl75495360 = -749249141;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl65364059 = -898232372;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl93654818 = -894089381;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl81279058 = -61454470;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl27732641 = 71655906;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl13753502 = -956138628;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl51126298 = -238234388;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl88435893 = -616016319;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69976963 = -902833904;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl55547673 = -968493278;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl55807946 = -814025145;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl33991364 = -510896297;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl65870632 = -5358268;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl99858307 = -785389962;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl64101321 = -655863252;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl33037317 = -282549661;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl76056589 = -706384524;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl88258522 = -860129214;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl46195164 = -192643636;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl5251813 = 57484209;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl62121178 = 54904466;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl5642872 = -759767006;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl40153490 = -728853481;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69791864 = -50777645;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl87434467 = -451504248;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl63776905 = -599100529;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl58154181 = -274384849;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl2066652 = -581059309;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl47976758 = -907824125;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl87328416 = -157561298;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl79929158 = -271456957;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl72151441 = -88132532;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl43687017 = 66540384;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl44015905 = -111787426;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl41708928 = -433070394;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69724320 = -742391292;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl10227433 = -111768433;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl1968084 = -592504902;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl57064370 = -666476203;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl96707429 = -942083730;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl38863061 = -887446159;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl18174564 = -418666591;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl52697188 = -344051275;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl59406894 = 37748752;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl36350886 = -395554032;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl13099880 = -999452444;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl31106329 = 55367526;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl67777618 = -25251624;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl79699322 = -319941767;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl20233149 = -165261139;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl78075903 = -305777912;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl445572 = -285001073;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl84395341 = 63178690;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl75222033 = -35812041;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl46810862 = -375266277;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69852489 = -989482135;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl25210569 = -69378892;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl23862954 = -743311737;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl93844591 = -609950223;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl63955735 = -329243565;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl55599321 = -581753779;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl49059646 = -657175079;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl40459136 = -708192194;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl82648546 = -645272607;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl75618515 = -597036321;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl83656505 = -625892614;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl90304346 = -477436682;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl21854727 = -893570842;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl58149380 = -252319568;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl94377001 = -913471960;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl22809885 = -70781228;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl74088506 = -13879622;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl31194152 = -93653011;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl49487734 = -250559906;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl66388752 = -55069632;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl43946614 = -526428944;    long FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl52945684 = -111787446;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl24160863 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl52947945;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl52947945 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl17204289;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl17204289 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl35088379;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl35088379 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl3882252;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl3882252 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl83247142;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl83247142 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl84797010;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl84797010 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl10460931;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl10460931 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl89989359;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl89989359 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl6610458;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl6610458 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl22674151;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl22674151 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl14954568;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl14954568 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl92158832;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl92158832 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl47091243;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl47091243 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl96976961;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl96976961 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl67635926;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl67635926 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl43800643;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl43800643 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl53270466;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl53270466 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl54132493;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl54132493 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl88704093;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl88704093 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl30590506;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl30590506 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl80473846;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl80473846 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl8932041;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl8932041 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl75495360;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl75495360 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl65364059;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl65364059 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl93654818;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl93654818 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl81279058;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl81279058 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl27732641;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl27732641 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl13753502;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl13753502 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl51126298;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl51126298 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl88435893;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl88435893 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69976963;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69976963 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl55547673;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl55547673 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl55807946;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl55807946 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl33991364;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl33991364 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl65870632;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl65870632 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl99858307;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl99858307 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl64101321;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl64101321 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl33037317;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl33037317 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl76056589;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl76056589 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl88258522;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl88258522 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl46195164;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl46195164 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl5251813;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl5251813 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl62121178;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl62121178 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl5642872;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl5642872 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl40153490;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl40153490 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69791864;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69791864 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl87434467;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl87434467 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl63776905;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl63776905 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl58154181;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl58154181 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl2066652;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl2066652 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl47976758;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl47976758 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl87328416;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl87328416 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl79929158;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl79929158 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl72151441;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl72151441 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl43687017;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl43687017 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl44015905;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl44015905 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl41708928;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl41708928 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69724320;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69724320 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl10227433;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl10227433 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl1968084;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl1968084 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl57064370;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl57064370 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl96707429;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl96707429 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl38863061;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl38863061 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl18174564;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl18174564 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl52697188;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl52697188 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl59406894;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl59406894 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl36350886;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl36350886 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl13099880;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl13099880 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl31106329;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl31106329 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl67777618;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl67777618 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl79699322;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl79699322 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl20233149;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl20233149 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl78075903;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl78075903 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl445572;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl445572 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl84395341;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl84395341 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl75222033;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl75222033 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl46810862;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl46810862 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69852489;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl69852489 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl25210569;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl25210569 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl23862954;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl23862954 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl93844591;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl93844591 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl63955735;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl63955735 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl55599321;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl55599321 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl49059646;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl49059646 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl40459136;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl40459136 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl82648546;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl82648546 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl75618515;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl75618515 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl83656505;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl83656505 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl90304346;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl90304346 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl21854727;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl21854727 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl58149380;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl58149380 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl94377001;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl94377001 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl22809885;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl22809885 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl74088506;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl74088506 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl31194152;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl31194152 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl49487734;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl49487734 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl66388752;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl66388752 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl43946614;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl43946614 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl52945684;     FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl52945684 = FndpMOMPbCKLFRKpaSICMSuTQcGIXiXWBnKzTWXuUNbbl24160863;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void udGttzySaxhddWiILfrIaQYaloNOnojuUCebh94063779() {     double aVmXKnBfPjbkZaVPCvdFg31469871 = -509016899;    double aVmXKnBfPjbkZaVPCvdFg51867354 = -159557096;    double aVmXKnBfPjbkZaVPCvdFg2618795 = -129511817;    double aVmXKnBfPjbkZaVPCvdFg33027375 = -719039646;    double aVmXKnBfPjbkZaVPCvdFg586219 = -691650639;    double aVmXKnBfPjbkZaVPCvdFg65388145 = 61452385;    double aVmXKnBfPjbkZaVPCvdFg45941053 = -559141138;    double aVmXKnBfPjbkZaVPCvdFg50361011 = -895524146;    double aVmXKnBfPjbkZaVPCvdFg2890535 = -122746358;    double aVmXKnBfPjbkZaVPCvdFg53431470 = -867279222;    double aVmXKnBfPjbkZaVPCvdFg67829290 = -362717132;    double aVmXKnBfPjbkZaVPCvdFg51481133 = 38705932;    double aVmXKnBfPjbkZaVPCvdFg12211328 = -710072088;    double aVmXKnBfPjbkZaVPCvdFg6709879 = -420879718;    double aVmXKnBfPjbkZaVPCvdFg70356304 = -781503707;    double aVmXKnBfPjbkZaVPCvdFg71148760 = -448191986;    double aVmXKnBfPjbkZaVPCvdFg91638804 = -121562813;    double aVmXKnBfPjbkZaVPCvdFg16886608 = -209614204;    double aVmXKnBfPjbkZaVPCvdFg49817214 = -879840824;    double aVmXKnBfPjbkZaVPCvdFg35308132 = -513085267;    double aVmXKnBfPjbkZaVPCvdFg74731388 = -544333434;    double aVmXKnBfPjbkZaVPCvdFg9069614 = -127134617;    double aVmXKnBfPjbkZaVPCvdFg82569375 = -480014771;    double aVmXKnBfPjbkZaVPCvdFg18849899 = -841923632;    double aVmXKnBfPjbkZaVPCvdFg90785174 = -300170230;    double aVmXKnBfPjbkZaVPCvdFg88600990 = -721747264;    double aVmXKnBfPjbkZaVPCvdFg71849585 = -866203454;    double aVmXKnBfPjbkZaVPCvdFg61982292 = -691810502;    double aVmXKnBfPjbkZaVPCvdFg31438651 = -507285276;    double aVmXKnBfPjbkZaVPCvdFg34621304 = -729474279;    double aVmXKnBfPjbkZaVPCvdFg82851937 = -354028275;    double aVmXKnBfPjbkZaVPCvdFg93219156 = -30564292;    double aVmXKnBfPjbkZaVPCvdFg94925687 = -584744620;    double aVmXKnBfPjbkZaVPCvdFg92136167 = -223577548;    double aVmXKnBfPjbkZaVPCvdFg57369778 = -505439182;    double aVmXKnBfPjbkZaVPCvdFg38016697 = -827101685;    double aVmXKnBfPjbkZaVPCvdFg14810976 = -293372035;    double aVmXKnBfPjbkZaVPCvdFg10858842 = -135851170;    double aVmXKnBfPjbkZaVPCvdFg427305 = -790589192;    double aVmXKnBfPjbkZaVPCvdFg99281763 = -330650025;    double aVmXKnBfPjbkZaVPCvdFg97336822 = -51398746;    double aVmXKnBfPjbkZaVPCvdFg67464523 = -739430950;    double aVmXKnBfPjbkZaVPCvdFg44604163 = 71990875;    double aVmXKnBfPjbkZaVPCvdFg7699210 = -331879238;    double aVmXKnBfPjbkZaVPCvdFg61438087 = -152618138;    double aVmXKnBfPjbkZaVPCvdFg1190273 = -182800030;    double aVmXKnBfPjbkZaVPCvdFg96479061 = -948465025;    double aVmXKnBfPjbkZaVPCvdFg28475390 = -824337231;    double aVmXKnBfPjbkZaVPCvdFg57881457 = -245633456;    double aVmXKnBfPjbkZaVPCvdFg84369544 = -388632781;    double aVmXKnBfPjbkZaVPCvdFg77275635 = -631278508;    double aVmXKnBfPjbkZaVPCvdFg56537382 = -156422006;    double aVmXKnBfPjbkZaVPCvdFg60033256 = -280474924;    double aVmXKnBfPjbkZaVPCvdFg17721641 = 21401717;    double aVmXKnBfPjbkZaVPCvdFg19332391 = -500559085;    double aVmXKnBfPjbkZaVPCvdFg22400258 = -281882283;    double aVmXKnBfPjbkZaVPCvdFg69297978 = -679542326;    double aVmXKnBfPjbkZaVPCvdFg83768895 = -287588186;    double aVmXKnBfPjbkZaVPCvdFg42242200 = -318869417;    double aVmXKnBfPjbkZaVPCvdFg11985229 = -969903376;    double aVmXKnBfPjbkZaVPCvdFg93538560 = -72344162;    double aVmXKnBfPjbkZaVPCvdFg83958761 = -867330637;    double aVmXKnBfPjbkZaVPCvdFg18922361 = -288238871;    double aVmXKnBfPjbkZaVPCvdFg68269230 = -393272080;    double aVmXKnBfPjbkZaVPCvdFg70579532 = -413250948;    double aVmXKnBfPjbkZaVPCvdFg74610134 = -232152841;    double aVmXKnBfPjbkZaVPCvdFg56555445 = -376549449;    double aVmXKnBfPjbkZaVPCvdFg20075161 = -386494540;    double aVmXKnBfPjbkZaVPCvdFg49340100 = -915440537;    double aVmXKnBfPjbkZaVPCvdFg32339607 = -954402022;    double aVmXKnBfPjbkZaVPCvdFg56337784 = -54819952;    double aVmXKnBfPjbkZaVPCvdFg80779963 = -985711644;    double aVmXKnBfPjbkZaVPCvdFg16459304 = -419025013;    double aVmXKnBfPjbkZaVPCvdFg50535451 = -449190800;    double aVmXKnBfPjbkZaVPCvdFg37971309 = -361686521;    double aVmXKnBfPjbkZaVPCvdFg7266866 = -804902485;    double aVmXKnBfPjbkZaVPCvdFg64465451 = -99125492;    double aVmXKnBfPjbkZaVPCvdFg74870166 = -48135533;    double aVmXKnBfPjbkZaVPCvdFg57411812 = -589305494;    double aVmXKnBfPjbkZaVPCvdFg89594901 = -17370201;    double aVmXKnBfPjbkZaVPCvdFg92121929 = -773282239;    double aVmXKnBfPjbkZaVPCvdFg43374195 = 58133776;    double aVmXKnBfPjbkZaVPCvdFg4100835 = -346177046;    double aVmXKnBfPjbkZaVPCvdFg47069106 = -18652496;    double aVmXKnBfPjbkZaVPCvdFg57345669 = 1804229;    double aVmXKnBfPjbkZaVPCvdFg26314556 = -97606269;    double aVmXKnBfPjbkZaVPCvdFg33185901 = -750089368;    double aVmXKnBfPjbkZaVPCvdFg77204047 = -506146337;    double aVmXKnBfPjbkZaVPCvdFg72803776 = -723018464;    double aVmXKnBfPjbkZaVPCvdFg34969521 = -123556900;    double aVmXKnBfPjbkZaVPCvdFg68718718 = -47559359;    double aVmXKnBfPjbkZaVPCvdFg31042081 = 94216150;    double aVmXKnBfPjbkZaVPCvdFg68616641 = -816981754;    double aVmXKnBfPjbkZaVPCvdFg88442076 = -820685816;    double aVmXKnBfPjbkZaVPCvdFg5743204 = -158305863;    double aVmXKnBfPjbkZaVPCvdFg13378062 = -184068110;    double aVmXKnBfPjbkZaVPCvdFg48542163 = -351192079;    double aVmXKnBfPjbkZaVPCvdFg76334932 = -534737046;    double aVmXKnBfPjbkZaVPCvdFg37119678 = -918628291;    double aVmXKnBfPjbkZaVPCvdFg86827953 = -509016899;     aVmXKnBfPjbkZaVPCvdFg31469871 = aVmXKnBfPjbkZaVPCvdFg51867354;     aVmXKnBfPjbkZaVPCvdFg51867354 = aVmXKnBfPjbkZaVPCvdFg2618795;     aVmXKnBfPjbkZaVPCvdFg2618795 = aVmXKnBfPjbkZaVPCvdFg33027375;     aVmXKnBfPjbkZaVPCvdFg33027375 = aVmXKnBfPjbkZaVPCvdFg586219;     aVmXKnBfPjbkZaVPCvdFg586219 = aVmXKnBfPjbkZaVPCvdFg65388145;     aVmXKnBfPjbkZaVPCvdFg65388145 = aVmXKnBfPjbkZaVPCvdFg45941053;     aVmXKnBfPjbkZaVPCvdFg45941053 = aVmXKnBfPjbkZaVPCvdFg50361011;     aVmXKnBfPjbkZaVPCvdFg50361011 = aVmXKnBfPjbkZaVPCvdFg2890535;     aVmXKnBfPjbkZaVPCvdFg2890535 = aVmXKnBfPjbkZaVPCvdFg53431470;     aVmXKnBfPjbkZaVPCvdFg53431470 = aVmXKnBfPjbkZaVPCvdFg67829290;     aVmXKnBfPjbkZaVPCvdFg67829290 = aVmXKnBfPjbkZaVPCvdFg51481133;     aVmXKnBfPjbkZaVPCvdFg51481133 = aVmXKnBfPjbkZaVPCvdFg12211328;     aVmXKnBfPjbkZaVPCvdFg12211328 = aVmXKnBfPjbkZaVPCvdFg6709879;     aVmXKnBfPjbkZaVPCvdFg6709879 = aVmXKnBfPjbkZaVPCvdFg70356304;     aVmXKnBfPjbkZaVPCvdFg70356304 = aVmXKnBfPjbkZaVPCvdFg71148760;     aVmXKnBfPjbkZaVPCvdFg71148760 = aVmXKnBfPjbkZaVPCvdFg91638804;     aVmXKnBfPjbkZaVPCvdFg91638804 = aVmXKnBfPjbkZaVPCvdFg16886608;     aVmXKnBfPjbkZaVPCvdFg16886608 = aVmXKnBfPjbkZaVPCvdFg49817214;     aVmXKnBfPjbkZaVPCvdFg49817214 = aVmXKnBfPjbkZaVPCvdFg35308132;     aVmXKnBfPjbkZaVPCvdFg35308132 = aVmXKnBfPjbkZaVPCvdFg74731388;     aVmXKnBfPjbkZaVPCvdFg74731388 = aVmXKnBfPjbkZaVPCvdFg9069614;     aVmXKnBfPjbkZaVPCvdFg9069614 = aVmXKnBfPjbkZaVPCvdFg82569375;     aVmXKnBfPjbkZaVPCvdFg82569375 = aVmXKnBfPjbkZaVPCvdFg18849899;     aVmXKnBfPjbkZaVPCvdFg18849899 = aVmXKnBfPjbkZaVPCvdFg90785174;     aVmXKnBfPjbkZaVPCvdFg90785174 = aVmXKnBfPjbkZaVPCvdFg88600990;     aVmXKnBfPjbkZaVPCvdFg88600990 = aVmXKnBfPjbkZaVPCvdFg71849585;     aVmXKnBfPjbkZaVPCvdFg71849585 = aVmXKnBfPjbkZaVPCvdFg61982292;     aVmXKnBfPjbkZaVPCvdFg61982292 = aVmXKnBfPjbkZaVPCvdFg31438651;     aVmXKnBfPjbkZaVPCvdFg31438651 = aVmXKnBfPjbkZaVPCvdFg34621304;     aVmXKnBfPjbkZaVPCvdFg34621304 = aVmXKnBfPjbkZaVPCvdFg82851937;     aVmXKnBfPjbkZaVPCvdFg82851937 = aVmXKnBfPjbkZaVPCvdFg93219156;     aVmXKnBfPjbkZaVPCvdFg93219156 = aVmXKnBfPjbkZaVPCvdFg94925687;     aVmXKnBfPjbkZaVPCvdFg94925687 = aVmXKnBfPjbkZaVPCvdFg92136167;     aVmXKnBfPjbkZaVPCvdFg92136167 = aVmXKnBfPjbkZaVPCvdFg57369778;     aVmXKnBfPjbkZaVPCvdFg57369778 = aVmXKnBfPjbkZaVPCvdFg38016697;     aVmXKnBfPjbkZaVPCvdFg38016697 = aVmXKnBfPjbkZaVPCvdFg14810976;     aVmXKnBfPjbkZaVPCvdFg14810976 = aVmXKnBfPjbkZaVPCvdFg10858842;     aVmXKnBfPjbkZaVPCvdFg10858842 = aVmXKnBfPjbkZaVPCvdFg427305;     aVmXKnBfPjbkZaVPCvdFg427305 = aVmXKnBfPjbkZaVPCvdFg99281763;     aVmXKnBfPjbkZaVPCvdFg99281763 = aVmXKnBfPjbkZaVPCvdFg97336822;     aVmXKnBfPjbkZaVPCvdFg97336822 = aVmXKnBfPjbkZaVPCvdFg67464523;     aVmXKnBfPjbkZaVPCvdFg67464523 = aVmXKnBfPjbkZaVPCvdFg44604163;     aVmXKnBfPjbkZaVPCvdFg44604163 = aVmXKnBfPjbkZaVPCvdFg7699210;     aVmXKnBfPjbkZaVPCvdFg7699210 = aVmXKnBfPjbkZaVPCvdFg61438087;     aVmXKnBfPjbkZaVPCvdFg61438087 = aVmXKnBfPjbkZaVPCvdFg1190273;     aVmXKnBfPjbkZaVPCvdFg1190273 = aVmXKnBfPjbkZaVPCvdFg96479061;     aVmXKnBfPjbkZaVPCvdFg96479061 = aVmXKnBfPjbkZaVPCvdFg28475390;     aVmXKnBfPjbkZaVPCvdFg28475390 = aVmXKnBfPjbkZaVPCvdFg57881457;     aVmXKnBfPjbkZaVPCvdFg57881457 = aVmXKnBfPjbkZaVPCvdFg84369544;     aVmXKnBfPjbkZaVPCvdFg84369544 = aVmXKnBfPjbkZaVPCvdFg77275635;     aVmXKnBfPjbkZaVPCvdFg77275635 = aVmXKnBfPjbkZaVPCvdFg56537382;     aVmXKnBfPjbkZaVPCvdFg56537382 = aVmXKnBfPjbkZaVPCvdFg60033256;     aVmXKnBfPjbkZaVPCvdFg60033256 = aVmXKnBfPjbkZaVPCvdFg17721641;     aVmXKnBfPjbkZaVPCvdFg17721641 = aVmXKnBfPjbkZaVPCvdFg19332391;     aVmXKnBfPjbkZaVPCvdFg19332391 = aVmXKnBfPjbkZaVPCvdFg22400258;     aVmXKnBfPjbkZaVPCvdFg22400258 = aVmXKnBfPjbkZaVPCvdFg69297978;     aVmXKnBfPjbkZaVPCvdFg69297978 = aVmXKnBfPjbkZaVPCvdFg83768895;     aVmXKnBfPjbkZaVPCvdFg83768895 = aVmXKnBfPjbkZaVPCvdFg42242200;     aVmXKnBfPjbkZaVPCvdFg42242200 = aVmXKnBfPjbkZaVPCvdFg11985229;     aVmXKnBfPjbkZaVPCvdFg11985229 = aVmXKnBfPjbkZaVPCvdFg93538560;     aVmXKnBfPjbkZaVPCvdFg93538560 = aVmXKnBfPjbkZaVPCvdFg83958761;     aVmXKnBfPjbkZaVPCvdFg83958761 = aVmXKnBfPjbkZaVPCvdFg18922361;     aVmXKnBfPjbkZaVPCvdFg18922361 = aVmXKnBfPjbkZaVPCvdFg68269230;     aVmXKnBfPjbkZaVPCvdFg68269230 = aVmXKnBfPjbkZaVPCvdFg70579532;     aVmXKnBfPjbkZaVPCvdFg70579532 = aVmXKnBfPjbkZaVPCvdFg74610134;     aVmXKnBfPjbkZaVPCvdFg74610134 = aVmXKnBfPjbkZaVPCvdFg56555445;     aVmXKnBfPjbkZaVPCvdFg56555445 = aVmXKnBfPjbkZaVPCvdFg20075161;     aVmXKnBfPjbkZaVPCvdFg20075161 = aVmXKnBfPjbkZaVPCvdFg49340100;     aVmXKnBfPjbkZaVPCvdFg49340100 = aVmXKnBfPjbkZaVPCvdFg32339607;     aVmXKnBfPjbkZaVPCvdFg32339607 = aVmXKnBfPjbkZaVPCvdFg56337784;     aVmXKnBfPjbkZaVPCvdFg56337784 = aVmXKnBfPjbkZaVPCvdFg80779963;     aVmXKnBfPjbkZaVPCvdFg80779963 = aVmXKnBfPjbkZaVPCvdFg16459304;     aVmXKnBfPjbkZaVPCvdFg16459304 = aVmXKnBfPjbkZaVPCvdFg50535451;     aVmXKnBfPjbkZaVPCvdFg50535451 = aVmXKnBfPjbkZaVPCvdFg37971309;     aVmXKnBfPjbkZaVPCvdFg37971309 = aVmXKnBfPjbkZaVPCvdFg7266866;     aVmXKnBfPjbkZaVPCvdFg7266866 = aVmXKnBfPjbkZaVPCvdFg64465451;     aVmXKnBfPjbkZaVPCvdFg64465451 = aVmXKnBfPjbkZaVPCvdFg74870166;     aVmXKnBfPjbkZaVPCvdFg74870166 = aVmXKnBfPjbkZaVPCvdFg57411812;     aVmXKnBfPjbkZaVPCvdFg57411812 = aVmXKnBfPjbkZaVPCvdFg89594901;     aVmXKnBfPjbkZaVPCvdFg89594901 = aVmXKnBfPjbkZaVPCvdFg92121929;     aVmXKnBfPjbkZaVPCvdFg92121929 = aVmXKnBfPjbkZaVPCvdFg43374195;     aVmXKnBfPjbkZaVPCvdFg43374195 = aVmXKnBfPjbkZaVPCvdFg4100835;     aVmXKnBfPjbkZaVPCvdFg4100835 = aVmXKnBfPjbkZaVPCvdFg47069106;     aVmXKnBfPjbkZaVPCvdFg47069106 = aVmXKnBfPjbkZaVPCvdFg57345669;     aVmXKnBfPjbkZaVPCvdFg57345669 = aVmXKnBfPjbkZaVPCvdFg26314556;     aVmXKnBfPjbkZaVPCvdFg26314556 = aVmXKnBfPjbkZaVPCvdFg33185901;     aVmXKnBfPjbkZaVPCvdFg33185901 = aVmXKnBfPjbkZaVPCvdFg77204047;     aVmXKnBfPjbkZaVPCvdFg77204047 = aVmXKnBfPjbkZaVPCvdFg72803776;     aVmXKnBfPjbkZaVPCvdFg72803776 = aVmXKnBfPjbkZaVPCvdFg34969521;     aVmXKnBfPjbkZaVPCvdFg34969521 = aVmXKnBfPjbkZaVPCvdFg68718718;     aVmXKnBfPjbkZaVPCvdFg68718718 = aVmXKnBfPjbkZaVPCvdFg31042081;     aVmXKnBfPjbkZaVPCvdFg31042081 = aVmXKnBfPjbkZaVPCvdFg68616641;     aVmXKnBfPjbkZaVPCvdFg68616641 = aVmXKnBfPjbkZaVPCvdFg88442076;     aVmXKnBfPjbkZaVPCvdFg88442076 = aVmXKnBfPjbkZaVPCvdFg5743204;     aVmXKnBfPjbkZaVPCvdFg5743204 = aVmXKnBfPjbkZaVPCvdFg13378062;     aVmXKnBfPjbkZaVPCvdFg13378062 = aVmXKnBfPjbkZaVPCvdFg48542163;     aVmXKnBfPjbkZaVPCvdFg48542163 = aVmXKnBfPjbkZaVPCvdFg76334932;     aVmXKnBfPjbkZaVPCvdFg76334932 = aVmXKnBfPjbkZaVPCvdFg37119678;     aVmXKnBfPjbkZaVPCvdFg37119678 = aVmXKnBfPjbkZaVPCvdFg86827953;     aVmXKnBfPjbkZaVPCvdFg86827953 = aVmXKnBfPjbkZaVPCvdFg31469871;}
// Junk Finished
