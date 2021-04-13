#include "LegitPew.h"

#include <random>

#include "../Options.hpp"
#include "../helpers/Math.hpp"
#include "../helpers/Utils.hpp"
#include "LagCompensation.hpp"

LineGoesThroughSmokeFn LineGoesThroughSmoke;

void LegitPew::Work(CUserCmd *usercmd)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	this->usercmd = usercmd;

	if (!g_LocalPlayer->m_hActiveWeapon().Get())
		return;

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsWeaponNonAim() || g_LocalPlayer->m_hActiveWeapon().Get()->m_iClip1() < 1)
		return;

	if (!XSystemCFG.lgtpew_enabled)
		return;

	if (XSystemCFG.lgtpew_enabled)
	{
		if (XSystemCFG.lgtpew_on_press && g_InputSystem->IsButtonDown(XSystemCFG.lgtpew_aim_keybind) || !XSystemCFG.lgtpew_on_press)
		{
			GetBestTarget();
			if (HasTarget())
				TargetRegion();
		}
	}

	if (XSystemCFG.lgtpew_trigger)
		if (!XSystemCFG.trigger_on_press || XSystemCFG.trigger_on_press && g_InputSystem->IsButtonDown(XSystemCFG.trigger_keybind))
			Triggerbot();

	if (XSystemCFG.lgtpew_rcs)
		if (!XSystemCFG.rcs_on_press || XSystemCFG.rcs_on_press && g_InputSystem->IsButtonDown(XSystemCFG.rcs_keybind))
			RecoilControlSystem();


	if (!XSystemCFG.angrpwsilent)
		g_EngineClient->SetViewAngles(usercmd->viewangles);

	if (XSystemCFG.legit_backtrack && usercmd->buttons & IN_ATTACK)
		CMBacktracking::Get().LegitBackTrack(usercmd);
}

void LegitPew::GetBestTarget()
{
	float maxDistance = 8192.f;
	float nearest;
	int index = -1;

	float fov = XSystemCFG.lgtpew_fov;

	int firedShots = g_LocalPlayer->m_iShotsFired();

	for (int i = 1; i < g_GlobalVars->maxClients; i++)
	{
		auto player = C_BasePlayer::GetPlayerByIndex(i);

		if (!CheckTarget(player))
			continue;

		QAngle viewangles = usercmd->viewangles;

		Vector targetpos;

		if (firedShots > XSystemCFG.lgtpew_aftershots)
			targetpos = player->GetBonePos(8 - XSystemCFG.lgtpew_afteraim);
		else if (firedShots < XSystemCFG.lgtpew_aftershots)
			targetpos = player->GetBonePos(8 - XSystemCFG.lgtpew_preaim);

		nearest = Math::GetFov(viewangles, Math::CalcAngle(g_LocalPlayer->GetEyePos(), targetpos));

		if (nearest > fov)
			continue;

		float distance = Math::GetDistance(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin());

		if (fabsf(fov - nearest) < 5)
		{
			if (distance < maxDistance)
			{
				fov = nearest;
				maxDistance = distance;
				index = i;
			}
		}
		else if (nearest < fov)
		{
			fov = nearest;
			maxDistance = distance;
			index = i;
		}
	}
	SetTarget(index);
}

bool LegitPew::CheckTarget(C_BasePlayer *player)
{
	if (!player || player == nullptr)
		return false;

	if (player == g_LocalPlayer)
		return false;

	if (player->IsTeamMate())
		return false;

	if (player->IsDormant())
		return false;

	if (player->m_bGunGameImmunity())
		return false;

	if (!player->IsAlive())
		return false;

	int firedShots = g_LocalPlayer->m_iShotsFired();
	Vector targetpos;

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;
	auto start = g_LocalPlayer->GetEyePos();

	if (firedShots > XSystemCFG.lgtpew_aftershots)
		targetpos = player->GetBonePos(8 - XSystemCFG.lgtpew_afteraim);
	else if (firedShots < XSystemCFG.lgtpew_aftershots)
		targetpos = player->GetBonePos(8 - XSystemCFG.lgtpew_preaim);

	ray.Init(start, targetpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	if (tr.hit_entity != player)
		return false;

	if (IsBehindSmoke(player->GetEyePos(), targetpos))
		return false;

	return true;
}

bool LegitPew::IsBehindSmoke(Vector src, Vector rem)
{
	LineGoesThroughSmoke = (LineGoesThroughSmokeFn)(Offsets::lgtSmoke);

	return LineGoesThroughSmoke(src, rem, true);
}

void LegitPew::TargetRegion()
{
	auto player = C_BasePlayer::GetPlayerByIndex(m_iTarget);

	int firedShots = g_LocalPlayer->m_iShotsFired();

	QAngle aimPunchAngle = g_LocalPlayer->m_aimPunchAngle();

	std::random_device r3nd0m;
	std::mt19937 r3nd0mGen(r3nd0m());

	std::uniform_real<float> r3nd0mXAngle(1.7f, 1.9f);
	std::uniform_real<float> r3nd0mYAngle(1.7f, 1.9f);

	aimPunchAngle.pitch *= r3nd0mXAngle(r3nd0mGen);
	aimPunchAngle.yaw *= r3nd0mYAngle(r3nd0mGen);
	aimPunchAngle.roll = 0.0f;

	Math::NormalizeAngles(aimPunchAngle);

	QAngle viewangles = usercmd->viewangles;

	Vector targetpos;

	if (firedShots > XSystemCFG.lgtpew_aftershots)
		targetpos = player->GetBonePos(8 - XSystemCFG.lgtpew_afteraim);
	else if (firedShots < XSystemCFG.lgtpew_aftershots)
		targetpos = player->GetBonePos(8 - XSystemCFG.lgtpew_preaim);

	QAngle angle = Math::CalcAngle(g_LocalPlayer->GetEyePos(), targetpos);
	angle.pitch -= aimPunchAngle.pitch;
	angle.yaw -= aimPunchAngle.yaw;

	Math::SmoothAngle(viewangles, angle, XSystemCFG.lgtpew_smooth_factor);

	Math::NormalizeAngles(angle);

	usercmd->viewangles = angle;
}

void LegitPew::Triggerbot()
{
	Vector rem, forward,
		src = g_LocalPlayer->GetEyePos();

	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	QAngle viewangles = usercmd->viewangles;

	viewangles += g_LocalPlayer->m_aimPunchAngle() * 2.f;

	Math::AngleVectors(viewangles, forward);

	forward *= g_LocalPlayer->m_hActiveWeapon().Get()->GetWeapInfo()->m_fRange();

	rem = src + forward;

	ray.Init(src, rem);
	g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &tr);

	if (!tr.hit_entity)
		return;

	bool dh = false;
	if (!XSystemCFG.lgtpew_hsonly)
	{
		if (tr.hitgroup == HITGROUP_HEAD || tr.hitgroup == HITGROUP_CHEST || tr.hitgroup == HITGROUP_STOMACH || (tr.hitgroup == HITGROUP_LEFTARM || tr.hitgroup == HITGROUP_RIGHTARM) || (tr.hitgroup == HITGROUP_LEFTLEG || tr.hitgroup == HITGROUP_RIGHTLEG))
			dh = true;
	}
	else
	{
		if (tr.hitgroup == HITGROUP_HEAD)
			dh = true;
	}

	auto player = reinterpret_cast<C_BasePlayer*>(tr.hit_entity);
	if (player && !player->IsDormant() && player->m_iHealth() > 0 && player->IsPlayer())
	{
		if (player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
		{
				if (dh && (!(usercmd->buttons & IN_ATTACK) && !g_InputSystem->IsButtonDown(ButtonCode_t::MOUSE_LEFT))) // if you don't attack currently
					usercmd->buttons |= IN_ATTACK;

				static bool already_shot = false;
				if (g_LocalPlayer->m_hActiveWeapon().Get()->IsPistol2())
				{
					if (usercmd->buttons & IN_ATTACK)
						if (already_shot)
							usercmd->buttons &= ~IN_ATTACK;

					already_shot = usercmd->buttons & IN_ATTACK ? true : false;
				}
		}
	}
}

QAngle oldPunch;

void LegitPew::RecoilControlSystem()
{
	if (usercmd->buttons & IN_ATTACK)
	{
		static QAngle oldPunch{ };
		QAngle aimPunch = g_LocalPlayer->m_aimPunchAngle() * g_CVar->FindVar("weapon_recoil_scale")->GetFloat();

		aimPunch.pitch *= XSystemCFG.lgtpew_rcsy;
		aimPunch.yaw *= XSystemCFG.lgtpew_rcsx;

		g_EngineClient->SetViewAngles(usercmd->viewangles += (oldPunch - aimPunch));

		oldPunch = aimPunch;
	}
}
/*
{
	int firedShots = g_LocalPlayer->m_iShotsFired();

	if (usercmd->buttons & IN_ATTACK)
	{
		QAngle aimPunchAngle = g_LocalPlayer->m_aimPunchAngle();

		std::random_device r3nd0m;
		std::mt19937 r3nd0mGen(r3nd0m());

		std::uniform_real<float> r3nd0mXAngle(1.7f, 1.9f);
		std::uniform_real<float> r3nd0mYAngle(1.7f, 1.9f);

		aimPunchAngle.pitch *= r3nd0mXAngle(r3nd0mGen);
		aimPunchAngle.yaw *= r3nd0mYAngle(r3nd0mGen);
		aimPunchAngle.roll = 0.0f;

		Math::NormalizeAngles(aimPunchAngle);

		if (firedShots > 2)
		{
			QAngle viewangles = usercmd->viewangles;

			QAngle viewangles_mod = aimPunchAngle;

			viewangles_mod -= oldPunch;
			viewangles_mod.roll = 0.0f;

			Math::NormalizeAngles(viewangles_mod);

			viewangles -= viewangles_mod;
			viewangles.roll = 0.0f;

			Math::NormalizeAngles(viewangles);

			usercmd->viewangles = viewangles;
		}
		oldPunch = aimPunchAngle;
	}
	else
		oldPunch.Init();
}

*/










































































































// Junk Code By Troll Face & Thaisen's Gen
void uemsnZrXVFNeAdJZUorKjYigBLVUThnxMVnDtqyt33879050() {     int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24095519 = -645099216;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12447367 = 32472876;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR64572589 = -199397105;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24027178 = -816473439;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR56573669 = -402504680;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR91655660 = -281714208;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR94685909 = -889071030;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR32470227 = -864714732;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR84379743 = -100271677;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR60299580 = -744511380;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR81040785 = -869960244;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR96839014 = -507467741;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR42280388 = -871173651;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR98834965 = -659896775;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24881063 = -491567958;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR85481132 = -841090557;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR30473449 = -139309823;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR35109969 = -160109394;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR53780563 = 16822788;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR53500383 = 37130753;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR2295173 = -144134310;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR26373715 = -510477000;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR76968532 = 73188267;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR16402581 = -27127883;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR71083097 = -954161779;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR79171642 = -32607177;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR22384735 = -869332443;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR64955125 = -483302575;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR86531127 = -555032244;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR50972068 = -690344396;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR89219817 = -336048728;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR14007051 = -132826260;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR80045230 = -169306638;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR88150848 = -740969819;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR4056925 = -12619120;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR28096393 = -589482625;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR22853169 = -97532157;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR85769330 = -617178111;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR32769752 = -832055837;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR41299937 = -167624145;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR27540329 = -266182342;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR93937532 = -153779868;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR40473707 = -4519705;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR86265357 = -633251949;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR79944059 = 39646573;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR49057491 = -473739115;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR47990711 = -307311579;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR67560122 = -979025480;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR13468903 = -899387073;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR16945720 = -366590317;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR10002827 = -784785187;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12424670 = -211907842;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24779287 = -839096445;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR7753539 = -770089200;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR31583878 = -613852132;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR97721803 = -34622216;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR35478834 = 59284608;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR48170008 = -72269222;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR52944081 = -862311661;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR77402026 = -269897503;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR69270926 = -412381766;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR29730785 = -305768456;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR45939099 = -209682489;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR33407676 = -409927282;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR71079762 = -308462653;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR67033735 = -637133985;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR16793784 = -238161103;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR54129540 = -30203833;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR94778040 = -547277655;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR96784669 = -902085333;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR62627963 = -643558400;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR44704119 = -522131713;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR2340218 = -328053558;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12480626 = -815553068;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR25960054 = -696686906;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR8357640 = -990354443;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR85900007 = -405957296;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR90703174 = -293559784;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR36458522 = 33225543;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR22025606 = -380422664;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR31180932 = -725295599;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR54824613 = -890306964;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR51486223 = -583915503;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR69585408 = -88441927;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR40969241 = -905559209;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR76795148 = -24140887;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR89227763 = -293729816;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR72291692 = -399217439;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR56566970 = -27117687;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR6335121 = -977996905;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR92617558 = -548767234;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR74683161 = 74737064;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR32825250 = -754866451;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR55367725 = -462158334;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR72029011 = -755242379;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR97809544 = -960413887;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR47998433 = -944097379;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR7066032 = -594592424;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR15185596 = -224789297;    int WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12910324 = -645099216;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24095519 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12447367;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12447367 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR64572589;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR64572589 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24027178;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24027178 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR56573669;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR56573669 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR91655660;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR91655660 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR94685909;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR94685909 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR32470227;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR32470227 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR84379743;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR84379743 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR60299580;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR60299580 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR81040785;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR81040785 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR96839014;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR96839014 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR42280388;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR42280388 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR98834965;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR98834965 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24881063;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24881063 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR85481132;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR85481132 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR30473449;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR30473449 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR35109969;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR35109969 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR53780563;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR53780563 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR53500383;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR53500383 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR2295173;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR2295173 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR26373715;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR26373715 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR76968532;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR76968532 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR16402581;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR16402581 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR71083097;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR71083097 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR79171642;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR79171642 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR22384735;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR22384735 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR64955125;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR64955125 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR86531127;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR86531127 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR50972068;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR50972068 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR89219817;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR89219817 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR14007051;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR14007051 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR80045230;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR80045230 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR88150848;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR88150848 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR4056925;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR4056925 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR28096393;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR28096393 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR22853169;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR22853169 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR85769330;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR85769330 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR32769752;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR32769752 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR41299937;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR41299937 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR27540329;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR27540329 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR93937532;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR93937532 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR40473707;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR40473707 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR86265357;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR86265357 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR79944059;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR79944059 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR49057491;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR49057491 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR47990711;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR47990711 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR67560122;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR67560122 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR13468903;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR13468903 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR16945720;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR16945720 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR10002827;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR10002827 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12424670;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12424670 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24779287;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24779287 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR7753539;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR7753539 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR31583878;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR31583878 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR97721803;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR97721803 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR35478834;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR35478834 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR48170008;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR48170008 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR52944081;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR52944081 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR77402026;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR77402026 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR69270926;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR69270926 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR29730785;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR29730785 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR45939099;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR45939099 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR33407676;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR33407676 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR71079762;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR71079762 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR67033735;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR67033735 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR16793784;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR16793784 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR54129540;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR54129540 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR94778040;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR94778040 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR96784669;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR96784669 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR62627963;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR62627963 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR44704119;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR44704119 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR2340218;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR2340218 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12480626;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12480626 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR25960054;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR25960054 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR8357640;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR8357640 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR85900007;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR85900007 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR90703174;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR90703174 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR36458522;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR36458522 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR22025606;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR22025606 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR31180932;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR31180932 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR54824613;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR54824613 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR51486223;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR51486223 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR69585408;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR69585408 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR40969241;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR40969241 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR76795148;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR76795148 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR89227763;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR89227763 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR72291692;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR72291692 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR56566970;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR56566970 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR6335121;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR6335121 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR92617558;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR92617558 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR74683161;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR74683161 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR32825250;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR32825250 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR55367725;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR55367725 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR72029011;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR72029011 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR97809544;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR97809544 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR47998433;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR47998433 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR7066032;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR7066032 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR15185596;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR15185596 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12910324;     WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR12910324 = WxMsdNnvcnHqJEDWwFrcEWDoRQRfqGfeqynmMqDoYZsgZwoISQHnzWpztXaNvHFVHdALQR24095519;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void cJvvNmmWZsMHSFpYBpCSIjUkzZODsbtaGFJPLOVNQNePoTOUapR38159170() {     float IMxitoLwoXjdyzlAMwaNnTpBuEEw79065036 = -899065231;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw91135123 = -662375095;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw43142089 = -122461089;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw42808491 = -635462640;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw73699592 = -233519526;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw97058199 = -21520236;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw92409741 = -973172927;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw45026297 = 36755324;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw25278708 = -109029793;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw31327184 = -604659882;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw63781558 = -304212633;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw88522037 = -479312066;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw98046543 = -384258483;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw3753678 = -36042316;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw11891826 = -578218879;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw64468658 = -475676136;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw65155956 = 73115200;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw33512361 = -132535450;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw23499333 = -314464605;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw87440114 = -830060612;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw22481638 = -334587081;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw4382990 = -127854098;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw26331077 = 29643189;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw91988422 = -164176324;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw94780983 = -604441603;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw27017748 = 24588588;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw90848229 = -35628960;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw34758695 = -725878305;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw19537816 = -374485912;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw27154643 = -527088196;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw87096154 = -312699252;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw92308549 = -817417872;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw63311931 = -418787378;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw10060075 = -231771903;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw51225877 = -436589878;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw70368009 = -145905742;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw61238954 = -133553107;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw62756684 = -703491059;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw77637025 = -153571726;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw36422193 = -484650447;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw51049589 = -710225140;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw42744117 = -815965145;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw92954339 = -285804950;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw8862298 = -357132147;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw96811256 = -853805239;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw34385994 = -359484345;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw18679643 = -404803362;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw32304065 = -701936737;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw74663577 = 18725872;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw72909747 = -91989415;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw91351977 = 54268775;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw58314160 = -577652567;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw81327624 = -642402374;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw17387817 = -458995437;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw15677532 = -286692196;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw74682047 = -671211133;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw64804046 = -592018285;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw51153667 = -958284765;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw48027507 = 68978963;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw46681844 = -158108115;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw6209970 = -985891277;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw57651046 = -147294622;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw25488482 = -588758764;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw98124064 = -581941597;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw44231029 = -191960631;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw71473009 = -486794761;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw25210106 = 39475312;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw87986469 = -52486581;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw52527800 = -599452438;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw41523817 = -332313137;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw3229705 = -242123029;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw2399272 = -223393741;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw55875335 = -978963725;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw87077140 = -829814158;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw36390526 = -19835472;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw79737521 = -518621937;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw11428650 = -842049148;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw17468780 = -613224665;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw95177165 = -310371086;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw60394990 = -144957259;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw8338106 = -570608050;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw58544164 = -333692223;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw60095118 = -644604178;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw46628068 = -182496498;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw35802665 = -481356971;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw28781994 = -735046685;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw10980926 = -75015499;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw45924114 = -959791941;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw94382542 = -945079707;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw76543830 = -765378746;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw5563963 = -553887457;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw10085287 = -175268343;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw14729177 = -672470023;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw30955182 = -995463612;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw30212223 = -498759171;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw93398543 = -462930519;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw17255635 = -127206381;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw94830274 = -703863354;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw64631268 = -65171517;    float IMxitoLwoXjdyzlAMwaNnTpBuEEw25338248 = -899065231;     IMxitoLwoXjdyzlAMwaNnTpBuEEw79065036 = IMxitoLwoXjdyzlAMwaNnTpBuEEw91135123;     IMxitoLwoXjdyzlAMwaNnTpBuEEw91135123 = IMxitoLwoXjdyzlAMwaNnTpBuEEw43142089;     IMxitoLwoXjdyzlAMwaNnTpBuEEw43142089 = IMxitoLwoXjdyzlAMwaNnTpBuEEw42808491;     IMxitoLwoXjdyzlAMwaNnTpBuEEw42808491 = IMxitoLwoXjdyzlAMwaNnTpBuEEw73699592;     IMxitoLwoXjdyzlAMwaNnTpBuEEw73699592 = IMxitoLwoXjdyzlAMwaNnTpBuEEw97058199;     IMxitoLwoXjdyzlAMwaNnTpBuEEw97058199 = IMxitoLwoXjdyzlAMwaNnTpBuEEw92409741;     IMxitoLwoXjdyzlAMwaNnTpBuEEw92409741 = IMxitoLwoXjdyzlAMwaNnTpBuEEw45026297;     IMxitoLwoXjdyzlAMwaNnTpBuEEw45026297 = IMxitoLwoXjdyzlAMwaNnTpBuEEw25278708;     IMxitoLwoXjdyzlAMwaNnTpBuEEw25278708 = IMxitoLwoXjdyzlAMwaNnTpBuEEw31327184;     IMxitoLwoXjdyzlAMwaNnTpBuEEw31327184 = IMxitoLwoXjdyzlAMwaNnTpBuEEw63781558;     IMxitoLwoXjdyzlAMwaNnTpBuEEw63781558 = IMxitoLwoXjdyzlAMwaNnTpBuEEw88522037;     IMxitoLwoXjdyzlAMwaNnTpBuEEw88522037 = IMxitoLwoXjdyzlAMwaNnTpBuEEw98046543;     IMxitoLwoXjdyzlAMwaNnTpBuEEw98046543 = IMxitoLwoXjdyzlAMwaNnTpBuEEw3753678;     IMxitoLwoXjdyzlAMwaNnTpBuEEw3753678 = IMxitoLwoXjdyzlAMwaNnTpBuEEw11891826;     IMxitoLwoXjdyzlAMwaNnTpBuEEw11891826 = IMxitoLwoXjdyzlAMwaNnTpBuEEw64468658;     IMxitoLwoXjdyzlAMwaNnTpBuEEw64468658 = IMxitoLwoXjdyzlAMwaNnTpBuEEw65155956;     IMxitoLwoXjdyzlAMwaNnTpBuEEw65155956 = IMxitoLwoXjdyzlAMwaNnTpBuEEw33512361;     IMxitoLwoXjdyzlAMwaNnTpBuEEw33512361 = IMxitoLwoXjdyzlAMwaNnTpBuEEw23499333;     IMxitoLwoXjdyzlAMwaNnTpBuEEw23499333 = IMxitoLwoXjdyzlAMwaNnTpBuEEw87440114;     IMxitoLwoXjdyzlAMwaNnTpBuEEw87440114 = IMxitoLwoXjdyzlAMwaNnTpBuEEw22481638;     IMxitoLwoXjdyzlAMwaNnTpBuEEw22481638 = IMxitoLwoXjdyzlAMwaNnTpBuEEw4382990;     IMxitoLwoXjdyzlAMwaNnTpBuEEw4382990 = IMxitoLwoXjdyzlAMwaNnTpBuEEw26331077;     IMxitoLwoXjdyzlAMwaNnTpBuEEw26331077 = IMxitoLwoXjdyzlAMwaNnTpBuEEw91988422;     IMxitoLwoXjdyzlAMwaNnTpBuEEw91988422 = IMxitoLwoXjdyzlAMwaNnTpBuEEw94780983;     IMxitoLwoXjdyzlAMwaNnTpBuEEw94780983 = IMxitoLwoXjdyzlAMwaNnTpBuEEw27017748;     IMxitoLwoXjdyzlAMwaNnTpBuEEw27017748 = IMxitoLwoXjdyzlAMwaNnTpBuEEw90848229;     IMxitoLwoXjdyzlAMwaNnTpBuEEw90848229 = IMxitoLwoXjdyzlAMwaNnTpBuEEw34758695;     IMxitoLwoXjdyzlAMwaNnTpBuEEw34758695 = IMxitoLwoXjdyzlAMwaNnTpBuEEw19537816;     IMxitoLwoXjdyzlAMwaNnTpBuEEw19537816 = IMxitoLwoXjdyzlAMwaNnTpBuEEw27154643;     IMxitoLwoXjdyzlAMwaNnTpBuEEw27154643 = IMxitoLwoXjdyzlAMwaNnTpBuEEw87096154;     IMxitoLwoXjdyzlAMwaNnTpBuEEw87096154 = IMxitoLwoXjdyzlAMwaNnTpBuEEw92308549;     IMxitoLwoXjdyzlAMwaNnTpBuEEw92308549 = IMxitoLwoXjdyzlAMwaNnTpBuEEw63311931;     IMxitoLwoXjdyzlAMwaNnTpBuEEw63311931 = IMxitoLwoXjdyzlAMwaNnTpBuEEw10060075;     IMxitoLwoXjdyzlAMwaNnTpBuEEw10060075 = IMxitoLwoXjdyzlAMwaNnTpBuEEw51225877;     IMxitoLwoXjdyzlAMwaNnTpBuEEw51225877 = IMxitoLwoXjdyzlAMwaNnTpBuEEw70368009;     IMxitoLwoXjdyzlAMwaNnTpBuEEw70368009 = IMxitoLwoXjdyzlAMwaNnTpBuEEw61238954;     IMxitoLwoXjdyzlAMwaNnTpBuEEw61238954 = IMxitoLwoXjdyzlAMwaNnTpBuEEw62756684;     IMxitoLwoXjdyzlAMwaNnTpBuEEw62756684 = IMxitoLwoXjdyzlAMwaNnTpBuEEw77637025;     IMxitoLwoXjdyzlAMwaNnTpBuEEw77637025 = IMxitoLwoXjdyzlAMwaNnTpBuEEw36422193;     IMxitoLwoXjdyzlAMwaNnTpBuEEw36422193 = IMxitoLwoXjdyzlAMwaNnTpBuEEw51049589;     IMxitoLwoXjdyzlAMwaNnTpBuEEw51049589 = IMxitoLwoXjdyzlAMwaNnTpBuEEw42744117;     IMxitoLwoXjdyzlAMwaNnTpBuEEw42744117 = IMxitoLwoXjdyzlAMwaNnTpBuEEw92954339;     IMxitoLwoXjdyzlAMwaNnTpBuEEw92954339 = IMxitoLwoXjdyzlAMwaNnTpBuEEw8862298;     IMxitoLwoXjdyzlAMwaNnTpBuEEw8862298 = IMxitoLwoXjdyzlAMwaNnTpBuEEw96811256;     IMxitoLwoXjdyzlAMwaNnTpBuEEw96811256 = IMxitoLwoXjdyzlAMwaNnTpBuEEw34385994;     IMxitoLwoXjdyzlAMwaNnTpBuEEw34385994 = IMxitoLwoXjdyzlAMwaNnTpBuEEw18679643;     IMxitoLwoXjdyzlAMwaNnTpBuEEw18679643 = IMxitoLwoXjdyzlAMwaNnTpBuEEw32304065;     IMxitoLwoXjdyzlAMwaNnTpBuEEw32304065 = IMxitoLwoXjdyzlAMwaNnTpBuEEw74663577;     IMxitoLwoXjdyzlAMwaNnTpBuEEw74663577 = IMxitoLwoXjdyzlAMwaNnTpBuEEw72909747;     IMxitoLwoXjdyzlAMwaNnTpBuEEw72909747 = IMxitoLwoXjdyzlAMwaNnTpBuEEw91351977;     IMxitoLwoXjdyzlAMwaNnTpBuEEw91351977 = IMxitoLwoXjdyzlAMwaNnTpBuEEw58314160;     IMxitoLwoXjdyzlAMwaNnTpBuEEw58314160 = IMxitoLwoXjdyzlAMwaNnTpBuEEw81327624;     IMxitoLwoXjdyzlAMwaNnTpBuEEw81327624 = IMxitoLwoXjdyzlAMwaNnTpBuEEw17387817;     IMxitoLwoXjdyzlAMwaNnTpBuEEw17387817 = IMxitoLwoXjdyzlAMwaNnTpBuEEw15677532;     IMxitoLwoXjdyzlAMwaNnTpBuEEw15677532 = IMxitoLwoXjdyzlAMwaNnTpBuEEw74682047;     IMxitoLwoXjdyzlAMwaNnTpBuEEw74682047 = IMxitoLwoXjdyzlAMwaNnTpBuEEw64804046;     IMxitoLwoXjdyzlAMwaNnTpBuEEw64804046 = IMxitoLwoXjdyzlAMwaNnTpBuEEw51153667;     IMxitoLwoXjdyzlAMwaNnTpBuEEw51153667 = IMxitoLwoXjdyzlAMwaNnTpBuEEw48027507;     IMxitoLwoXjdyzlAMwaNnTpBuEEw48027507 = IMxitoLwoXjdyzlAMwaNnTpBuEEw46681844;     IMxitoLwoXjdyzlAMwaNnTpBuEEw46681844 = IMxitoLwoXjdyzlAMwaNnTpBuEEw6209970;     IMxitoLwoXjdyzlAMwaNnTpBuEEw6209970 = IMxitoLwoXjdyzlAMwaNnTpBuEEw57651046;     IMxitoLwoXjdyzlAMwaNnTpBuEEw57651046 = IMxitoLwoXjdyzlAMwaNnTpBuEEw25488482;     IMxitoLwoXjdyzlAMwaNnTpBuEEw25488482 = IMxitoLwoXjdyzlAMwaNnTpBuEEw98124064;     IMxitoLwoXjdyzlAMwaNnTpBuEEw98124064 = IMxitoLwoXjdyzlAMwaNnTpBuEEw44231029;     IMxitoLwoXjdyzlAMwaNnTpBuEEw44231029 = IMxitoLwoXjdyzlAMwaNnTpBuEEw71473009;     IMxitoLwoXjdyzlAMwaNnTpBuEEw71473009 = IMxitoLwoXjdyzlAMwaNnTpBuEEw25210106;     IMxitoLwoXjdyzlAMwaNnTpBuEEw25210106 = IMxitoLwoXjdyzlAMwaNnTpBuEEw87986469;     IMxitoLwoXjdyzlAMwaNnTpBuEEw87986469 = IMxitoLwoXjdyzlAMwaNnTpBuEEw52527800;     IMxitoLwoXjdyzlAMwaNnTpBuEEw52527800 = IMxitoLwoXjdyzlAMwaNnTpBuEEw41523817;     IMxitoLwoXjdyzlAMwaNnTpBuEEw41523817 = IMxitoLwoXjdyzlAMwaNnTpBuEEw3229705;     IMxitoLwoXjdyzlAMwaNnTpBuEEw3229705 = IMxitoLwoXjdyzlAMwaNnTpBuEEw2399272;     IMxitoLwoXjdyzlAMwaNnTpBuEEw2399272 = IMxitoLwoXjdyzlAMwaNnTpBuEEw55875335;     IMxitoLwoXjdyzlAMwaNnTpBuEEw55875335 = IMxitoLwoXjdyzlAMwaNnTpBuEEw87077140;     IMxitoLwoXjdyzlAMwaNnTpBuEEw87077140 = IMxitoLwoXjdyzlAMwaNnTpBuEEw36390526;     IMxitoLwoXjdyzlAMwaNnTpBuEEw36390526 = IMxitoLwoXjdyzlAMwaNnTpBuEEw79737521;     IMxitoLwoXjdyzlAMwaNnTpBuEEw79737521 = IMxitoLwoXjdyzlAMwaNnTpBuEEw11428650;     IMxitoLwoXjdyzlAMwaNnTpBuEEw11428650 = IMxitoLwoXjdyzlAMwaNnTpBuEEw17468780;     IMxitoLwoXjdyzlAMwaNnTpBuEEw17468780 = IMxitoLwoXjdyzlAMwaNnTpBuEEw95177165;     IMxitoLwoXjdyzlAMwaNnTpBuEEw95177165 = IMxitoLwoXjdyzlAMwaNnTpBuEEw60394990;     IMxitoLwoXjdyzlAMwaNnTpBuEEw60394990 = IMxitoLwoXjdyzlAMwaNnTpBuEEw8338106;     IMxitoLwoXjdyzlAMwaNnTpBuEEw8338106 = IMxitoLwoXjdyzlAMwaNnTpBuEEw58544164;     IMxitoLwoXjdyzlAMwaNnTpBuEEw58544164 = IMxitoLwoXjdyzlAMwaNnTpBuEEw60095118;     IMxitoLwoXjdyzlAMwaNnTpBuEEw60095118 = IMxitoLwoXjdyzlAMwaNnTpBuEEw46628068;     IMxitoLwoXjdyzlAMwaNnTpBuEEw46628068 = IMxitoLwoXjdyzlAMwaNnTpBuEEw35802665;     IMxitoLwoXjdyzlAMwaNnTpBuEEw35802665 = IMxitoLwoXjdyzlAMwaNnTpBuEEw28781994;     IMxitoLwoXjdyzlAMwaNnTpBuEEw28781994 = IMxitoLwoXjdyzlAMwaNnTpBuEEw10980926;     IMxitoLwoXjdyzlAMwaNnTpBuEEw10980926 = IMxitoLwoXjdyzlAMwaNnTpBuEEw45924114;     IMxitoLwoXjdyzlAMwaNnTpBuEEw45924114 = IMxitoLwoXjdyzlAMwaNnTpBuEEw94382542;     IMxitoLwoXjdyzlAMwaNnTpBuEEw94382542 = IMxitoLwoXjdyzlAMwaNnTpBuEEw76543830;     IMxitoLwoXjdyzlAMwaNnTpBuEEw76543830 = IMxitoLwoXjdyzlAMwaNnTpBuEEw5563963;     IMxitoLwoXjdyzlAMwaNnTpBuEEw5563963 = IMxitoLwoXjdyzlAMwaNnTpBuEEw10085287;     IMxitoLwoXjdyzlAMwaNnTpBuEEw10085287 = IMxitoLwoXjdyzlAMwaNnTpBuEEw14729177;     IMxitoLwoXjdyzlAMwaNnTpBuEEw14729177 = IMxitoLwoXjdyzlAMwaNnTpBuEEw30955182;     IMxitoLwoXjdyzlAMwaNnTpBuEEw30955182 = IMxitoLwoXjdyzlAMwaNnTpBuEEw30212223;     IMxitoLwoXjdyzlAMwaNnTpBuEEw30212223 = IMxitoLwoXjdyzlAMwaNnTpBuEEw93398543;     IMxitoLwoXjdyzlAMwaNnTpBuEEw93398543 = IMxitoLwoXjdyzlAMwaNnTpBuEEw17255635;     IMxitoLwoXjdyzlAMwaNnTpBuEEw17255635 = IMxitoLwoXjdyzlAMwaNnTpBuEEw94830274;     IMxitoLwoXjdyzlAMwaNnTpBuEEw94830274 = IMxitoLwoXjdyzlAMwaNnTpBuEEw64631268;     IMxitoLwoXjdyzlAMwaNnTpBuEEw64631268 = IMxitoLwoXjdyzlAMwaNnTpBuEEw25338248;     IMxitoLwoXjdyzlAMwaNnTpBuEEw25338248 = IMxitoLwoXjdyzlAMwaNnTpBuEEw79065036;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void qGsCbhmYlKQvLwLxCyRMIPKzF36664751() {     long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4400538 = 25651869;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI96342707 = -367191828;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI10816111 = -370165684;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI52245249 = -654048968;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI47675927 = -825215340;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI14430816 = 61908789;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI11218559 = -478984176;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI54646159 = -679472469;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI3093063 = -668757961;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI8400826 = -648045702;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI15619000 = -82669638;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI13036937 = -560877521;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI32556925 = -877367846;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI31617391 = -96976612;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI83469922 = -945658331;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI65124504 = -976992627;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI43430523 = -137724640;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41982957 = -27652627;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI16817815 = 19052160;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI67500116 = -464935808;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI65774159 = -159247877;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI33229542 = -664504868;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI49165670 = 48802663;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI78637772 = -307138937;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI16019814 = -838641967;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI64384903 = -462248707;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI65656892 = -182111884;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI44880441 = -166058187;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI61528391 = -472677353;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI98884197 = -437901918;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI62188057 = -943535875;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI38158506 = -488126266;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI44390602 = -872832029;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4050910 = -550290926;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI69900509 = -330195468;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI57989024 = -770948581;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI24208181 = -16328527;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI52651247 = -378507098;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI24730545 = -22836584;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI8826659 = -834663442;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI91369203 = -447739562;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI26879413 = -209214226;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI45452215 = -911627490;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41908857 = -987190799;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI57607811 = -831305722;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI61887414 = -220190633;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI86034030 = -110303889;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI18974548 = -622339467;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI56930992 = -598506945;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI56805838 = -362427297;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI87316439 = -486933413;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI64617487 = -500969288;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI37130023 = -465375304;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI6243825 = -482407223;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI70890592 = -959131630;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI71170996 = -309843263;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI47177038 = -315994491;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI32178339 = 36973253;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI36225436 = -815407002;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI83291023 = -262966634;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI48773924 = -755979328;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI66338118 = -212925990;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI93117768 = -106795116;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4208865 = -130856044;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI46212769 = -704509827;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI77460493 = -594543372;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI68646335 = -688045493;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI28506015 = -227076920;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI61716882 = -766781144;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI25480898 = -74709751;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI40916324 = -860664101;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI90779276 = -759217543;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI17252413 = 95183956;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI7991157 = -146284398;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI76130912 = 82803753;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI38894747 = -950033651;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI87777326 = -752877379;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI7256813 = 35993461;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI21029961 = -475833216;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI54132400 = -518451334;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI78350872 = -251944819;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI46682345 = -559772418;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI87949448 = -567551243;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4722553 = -10250057;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI11567759 = -950968505;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI97570569 = -342566588;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI1028483 = 77249037;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI38146777 = -290424807;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI33160318 = -591159296;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI98729512 = 79647795;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI10811986 = -354954090;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI92029842 = 46698220;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI16425811 = -563100096;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41439521 = -759869951;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI60052734 = 21315885;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI25031086 = -134813573;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI33761644 = -2419111;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41243351 = -680771446;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI95696088 = -182680972;    long bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI80147317 = 25651869;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4400538 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI96342707;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI96342707 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI10816111;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI10816111 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI52245249;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI52245249 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI47675927;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI47675927 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI14430816;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI14430816 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI11218559;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI11218559 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI54646159;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI54646159 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI3093063;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI3093063 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI8400826;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI8400826 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI15619000;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI15619000 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI13036937;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI13036937 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI32556925;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI32556925 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI31617391;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI31617391 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI83469922;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI83469922 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI65124504;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI65124504 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI43430523;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI43430523 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41982957;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41982957 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI16817815;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI16817815 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI67500116;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI67500116 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI65774159;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI65774159 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI33229542;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI33229542 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI49165670;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI49165670 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI78637772;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI78637772 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI16019814;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI16019814 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI64384903;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI64384903 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI65656892;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI65656892 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI44880441;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI44880441 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI61528391;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI61528391 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI98884197;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI98884197 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI62188057;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI62188057 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI38158506;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI38158506 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI44390602;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI44390602 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4050910;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4050910 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI69900509;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI69900509 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI57989024;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI57989024 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI24208181;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI24208181 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI52651247;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI52651247 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI24730545;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI24730545 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI8826659;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI8826659 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI91369203;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI91369203 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI26879413;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI26879413 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI45452215;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI45452215 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41908857;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41908857 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI57607811;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI57607811 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI61887414;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI61887414 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI86034030;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI86034030 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI18974548;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI18974548 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI56930992;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI56930992 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI56805838;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI56805838 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI87316439;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI87316439 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI64617487;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI64617487 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI37130023;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI37130023 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI6243825;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI6243825 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI70890592;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI70890592 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI71170996;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI71170996 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI47177038;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI47177038 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI32178339;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI32178339 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI36225436;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI36225436 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI83291023;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI83291023 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI48773924;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI48773924 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI66338118;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI66338118 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI93117768;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI93117768 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4208865;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4208865 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI46212769;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI46212769 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI77460493;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI77460493 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI68646335;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI68646335 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI28506015;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI28506015 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI61716882;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI61716882 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI25480898;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI25480898 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI40916324;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI40916324 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI90779276;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI90779276 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI17252413;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI17252413 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI7991157;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI7991157 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI76130912;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI76130912 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI38894747;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI38894747 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI87777326;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI87777326 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI7256813;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI7256813 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI21029961;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI21029961 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI54132400;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI54132400 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI78350872;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI78350872 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI46682345;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI46682345 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI87949448;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI87949448 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4722553;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4722553 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI11567759;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI11567759 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI97570569;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI97570569 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI1028483;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI1028483 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI38146777;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI38146777 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI33160318;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI33160318 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI98729512;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI98729512 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI10811986;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI10811986 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI92029842;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI92029842 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI16425811;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI16425811 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41439521;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41439521 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI60052734;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI60052734 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI25031086;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI25031086 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI33761644;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI33761644 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41243351;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI41243351 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI95696088;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI95696088 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI80147317;     bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI80147317 = bHNuKivbwgDRrKXDEbtEqprSMRDMhRbvswzZZdbcVTPPI4400538;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dMDPqbbuZRiShmaBvZHpxOKcYFkbaXxivVeln27986583() {     double ddXnEncnwrSduyjMtzOCa11709546 = -371577584;    double ddXnEncnwrSduyjMtzOCa95262117 = -994599687;    double ddXnEncnwrSduyjMtzOCa96230616 = -317357967;    double ddXnEncnwrSduyjMtzOCa50184246 = -732464951;    double ddXnEncnwrSduyjMtzOCa44379894 = -411008165;    double ddXnEncnwrSduyjMtzOCa96571818 = -222679455;    double ddXnEncnwrSduyjMtzOCa72362601 = -343305017;    double ddXnEncnwrSduyjMtzOCa94546239 = -676774258;    double ddXnEncnwrSduyjMtzOCa15994238 = -665823773;    double ddXnEncnwrSduyjMtzOCa55221837 = -380642015;    double ddXnEncnwrSduyjMtzOCa60774139 = -198501591;    double ddXnEncnwrSduyjMtzOCa49563502 = -591427064;    double ddXnEncnwrSduyjMtzOCa52609420 = -277860758;    double ddXnEncnwrSduyjMtzOCa91236025 = -7507590;    double ddXnEncnwrSduyjMtzOCa56849265 = -577171296;    double ddXnEncnwrSduyjMtzOCa68637338 = -514543027;    double ddXnEncnwrSduyjMtzOCa91268684 = -283482436;    double ddXnEncnwrSduyjMtzOCa5599100 = -789456032;    double ddXnEncnwrSduyjMtzOCa12502537 = -848626230;    double ddXnEncnwrSduyjMtzOCa14104155 = -832890788;    double ddXnEncnwrSduyjMtzOCa9915042 = -474116366;    double ddXnEncnwrSduyjMtzOCa61825309 = -713311655;    double ddXnEncnwrSduyjMtzOCa22803006 = -10850297;    double ddXnEncnwrSduyjMtzOCa21992310 = -399813428;    double ddXnEncnwrSduyjMtzOCa41440930 = -240579824;    double ddXnEncnwrSduyjMtzOCa59331074 = -289906590;    double ddXnEncnwrSduyjMtzOCa56227418 = -986860867;    double ddXnEncnwrSduyjMtzOCa79130092 = -929524595;    double ddXnEncnwrSduyjMtzOCa79213540 = -23824002;    double ddXnEncnwrSduyjMtzOCa82379204 = -929141809;    double ddXnEncnwrSduyjMtzOCa56604101 = -681547831;    double ddXnEncnwrSduyjMtzOCa61400699 = -715856654;    double ddXnEncnwrSduyjMtzOCa83768616 = -489083371;    double ddXnEncnwrSduyjMtzOCa40379131 = 40156671;    double ddXnEncnwrSduyjMtzOCa93278923 = -324738353;    double ddXnEncnwrSduyjMtzOCa30135090 = -492691997;    double ddXnEncnwrSduyjMtzOCa39160849 = -624310600;    double ddXnEncnwrSduyjMtzOCa99408767 = -958495016;    double ddXnEncnwrSduyjMtzOCa92120532 = -530876115;    double ddXnEncnwrSduyjMtzOCa32051832 = -458928943;    double ddXnEncnwrSduyjMtzOCa447505 = -739009094;    double ddXnEncnwrSduyjMtzOCa48148772 = -756001540;    double ddXnEncnwrSduyjMtzOCa84804565 = -897120825;    double ddXnEncnwrSduyjMtzOCa87486887 = -273974503;    double ddXnEncnwrSduyjMtzOCa13403027 = -224156853;    double ddXnEncnwrSduyjMtzOCa22924197 = -774137182;    double ddXnEncnwrSduyjMtzOCa12721228 = 92008731;    double ddXnEncnwrSduyjMtzOCa60015469 = -995172450;    double ddXnEncnwrSduyjMtzOCa51035544 = -245039872;    double ddXnEncnwrSduyjMtzOCa83021201 = -476675228;    double ddXnEncnwrSduyjMtzOCa62525422 = -537152612;    double ddXnEncnwrSduyjMtzOCa73178111 = -849567168;    double ddXnEncnwrSduyjMtzOCa9834863 = -588288930;    double ddXnEncnwrSduyjMtzOCa44036307 = -189548549;    double ddXnEncnwrSduyjMtzOCa18071542 = -271558183;    double ddXnEncnwrSduyjMtzOCa49884236 = -658265930;    double ddXnEncnwrSduyjMtzOCa72459112 = -883749391;    double ddXnEncnwrSduyjMtzOCa74238306 = -917544539;    double ddXnEncnwrSduyjMtzOCa8743316 = -391885127;    double ddXnEncnwrSduyjMtzOCa85048820 = -21101576;    double ddXnEncnwrSduyjMtzOCa40344401 = -235818588;    double ddXnEncnwrSduyjMtzOCa93232509 = -413780423;    double ddXnEncnwrSduyjMtzOCa15332699 = -552950256;    double ddXnEncnwrSduyjMtzOCa33615034 = -736681965;    double ddXnEncnwrSduyjMtzOCa98617736 = -699094184;    double ddXnEncnwrSduyjMtzOCa99373439 = -482644937;    double ddXnEncnwrSduyjMtzOCa65794886 = -2343693;    double ddXnEncnwrSduyjMtzOCa12230290 = -218017429;    double ddXnEncnwrSduyjMtzOCa97957102 = -682769237;    double ddXnEncnwrSduyjMtzOCa26714176 = 15520701;    double ddXnEncnwrSduyjMtzOCa29476490 = -890232428;    double ddXnEncnwrSduyjMtzOCa91859917 = -324987420;    double ddXnEncnwrSduyjMtzOCa13478568 = -158579918;    double ddXnEncnwrSduyjMtzOCa80450704 = -289697287;    double ddXnEncnwrSduyjMtzOCa13656650 = 6118306;    double ddXnEncnwrSduyjMtzOCa61766270 = -718114826;    double ddXnEncnwrSduyjMtzOCa77020744 = -816190831;    double ddXnEncnwrSduyjMtzOCa35316118 = -736875795;    double ddXnEncnwrSduyjMtzOCa8589284 = -75656575;    double ddXnEncnwrSduyjMtzOCa18516733 = -466442643;    double ddXnEncnwrSduyjMtzOCa46609847 = -281915321;    double ddXnEncnwrSduyjMtzOCa96211948 = -991688418;    double ddXnEncnwrSduyjMtzOCa28094549 = -584484723;    double ddXnEncnwrSduyjMtzOCa96192338 = -547148774;    double ddXnEncnwrSduyjMtzOCa19853782 = -291989197;    double ddXnEncnwrSduyjMtzOCa83425990 = -831980664;    double ddXnEncnwrSduyjMtzOCa51565837 = -27567725;    double ddXnEncnwrSduyjMtzOCa39732310 = -199534823;    double ddXnEncnwrSduyjMtzOCa22307590 = -688285147;    double ddXnEncnwrSduyjMtzOCa43394687 = -666472424;    double ddXnEncnwrSduyjMtzOCa57675978 = -608942607;    double ddXnEncnwrSduyjMtzOCa64922543 = -706766061;    double ddXnEncnwrSduyjMtzOCa90665451 = -466609890;    double ddXnEncnwrSduyjMtzOCa7071713 = -409774539;    double ddXnEncnwrSduyjMtzOCa91707431 = -123110356;    double ddXnEncnwrSduyjMtzOCa7214996 = -225228672;    double ddXnEncnwrSduyjMtzOCa32816073 = -103051284;    double ddXnEncnwrSduyjMtzOCa51189531 = -60438860;    double ddXnEncnwrSduyjMtzOCa88869151 = -574880319;    double ddXnEncnwrSduyjMtzOCa14029587 = -371577584;     ddXnEncnwrSduyjMtzOCa11709546 = ddXnEncnwrSduyjMtzOCa95262117;     ddXnEncnwrSduyjMtzOCa95262117 = ddXnEncnwrSduyjMtzOCa96230616;     ddXnEncnwrSduyjMtzOCa96230616 = ddXnEncnwrSduyjMtzOCa50184246;     ddXnEncnwrSduyjMtzOCa50184246 = ddXnEncnwrSduyjMtzOCa44379894;     ddXnEncnwrSduyjMtzOCa44379894 = ddXnEncnwrSduyjMtzOCa96571818;     ddXnEncnwrSduyjMtzOCa96571818 = ddXnEncnwrSduyjMtzOCa72362601;     ddXnEncnwrSduyjMtzOCa72362601 = ddXnEncnwrSduyjMtzOCa94546239;     ddXnEncnwrSduyjMtzOCa94546239 = ddXnEncnwrSduyjMtzOCa15994238;     ddXnEncnwrSduyjMtzOCa15994238 = ddXnEncnwrSduyjMtzOCa55221837;     ddXnEncnwrSduyjMtzOCa55221837 = ddXnEncnwrSduyjMtzOCa60774139;     ddXnEncnwrSduyjMtzOCa60774139 = ddXnEncnwrSduyjMtzOCa49563502;     ddXnEncnwrSduyjMtzOCa49563502 = ddXnEncnwrSduyjMtzOCa52609420;     ddXnEncnwrSduyjMtzOCa52609420 = ddXnEncnwrSduyjMtzOCa91236025;     ddXnEncnwrSduyjMtzOCa91236025 = ddXnEncnwrSduyjMtzOCa56849265;     ddXnEncnwrSduyjMtzOCa56849265 = ddXnEncnwrSduyjMtzOCa68637338;     ddXnEncnwrSduyjMtzOCa68637338 = ddXnEncnwrSduyjMtzOCa91268684;     ddXnEncnwrSduyjMtzOCa91268684 = ddXnEncnwrSduyjMtzOCa5599100;     ddXnEncnwrSduyjMtzOCa5599100 = ddXnEncnwrSduyjMtzOCa12502537;     ddXnEncnwrSduyjMtzOCa12502537 = ddXnEncnwrSduyjMtzOCa14104155;     ddXnEncnwrSduyjMtzOCa14104155 = ddXnEncnwrSduyjMtzOCa9915042;     ddXnEncnwrSduyjMtzOCa9915042 = ddXnEncnwrSduyjMtzOCa61825309;     ddXnEncnwrSduyjMtzOCa61825309 = ddXnEncnwrSduyjMtzOCa22803006;     ddXnEncnwrSduyjMtzOCa22803006 = ddXnEncnwrSduyjMtzOCa21992310;     ddXnEncnwrSduyjMtzOCa21992310 = ddXnEncnwrSduyjMtzOCa41440930;     ddXnEncnwrSduyjMtzOCa41440930 = ddXnEncnwrSduyjMtzOCa59331074;     ddXnEncnwrSduyjMtzOCa59331074 = ddXnEncnwrSduyjMtzOCa56227418;     ddXnEncnwrSduyjMtzOCa56227418 = ddXnEncnwrSduyjMtzOCa79130092;     ddXnEncnwrSduyjMtzOCa79130092 = ddXnEncnwrSduyjMtzOCa79213540;     ddXnEncnwrSduyjMtzOCa79213540 = ddXnEncnwrSduyjMtzOCa82379204;     ddXnEncnwrSduyjMtzOCa82379204 = ddXnEncnwrSduyjMtzOCa56604101;     ddXnEncnwrSduyjMtzOCa56604101 = ddXnEncnwrSduyjMtzOCa61400699;     ddXnEncnwrSduyjMtzOCa61400699 = ddXnEncnwrSduyjMtzOCa83768616;     ddXnEncnwrSduyjMtzOCa83768616 = ddXnEncnwrSduyjMtzOCa40379131;     ddXnEncnwrSduyjMtzOCa40379131 = ddXnEncnwrSduyjMtzOCa93278923;     ddXnEncnwrSduyjMtzOCa93278923 = ddXnEncnwrSduyjMtzOCa30135090;     ddXnEncnwrSduyjMtzOCa30135090 = ddXnEncnwrSduyjMtzOCa39160849;     ddXnEncnwrSduyjMtzOCa39160849 = ddXnEncnwrSduyjMtzOCa99408767;     ddXnEncnwrSduyjMtzOCa99408767 = ddXnEncnwrSduyjMtzOCa92120532;     ddXnEncnwrSduyjMtzOCa92120532 = ddXnEncnwrSduyjMtzOCa32051832;     ddXnEncnwrSduyjMtzOCa32051832 = ddXnEncnwrSduyjMtzOCa447505;     ddXnEncnwrSduyjMtzOCa447505 = ddXnEncnwrSduyjMtzOCa48148772;     ddXnEncnwrSduyjMtzOCa48148772 = ddXnEncnwrSduyjMtzOCa84804565;     ddXnEncnwrSduyjMtzOCa84804565 = ddXnEncnwrSduyjMtzOCa87486887;     ddXnEncnwrSduyjMtzOCa87486887 = ddXnEncnwrSduyjMtzOCa13403027;     ddXnEncnwrSduyjMtzOCa13403027 = ddXnEncnwrSduyjMtzOCa22924197;     ddXnEncnwrSduyjMtzOCa22924197 = ddXnEncnwrSduyjMtzOCa12721228;     ddXnEncnwrSduyjMtzOCa12721228 = ddXnEncnwrSduyjMtzOCa60015469;     ddXnEncnwrSduyjMtzOCa60015469 = ddXnEncnwrSduyjMtzOCa51035544;     ddXnEncnwrSduyjMtzOCa51035544 = ddXnEncnwrSduyjMtzOCa83021201;     ddXnEncnwrSduyjMtzOCa83021201 = ddXnEncnwrSduyjMtzOCa62525422;     ddXnEncnwrSduyjMtzOCa62525422 = ddXnEncnwrSduyjMtzOCa73178111;     ddXnEncnwrSduyjMtzOCa73178111 = ddXnEncnwrSduyjMtzOCa9834863;     ddXnEncnwrSduyjMtzOCa9834863 = ddXnEncnwrSduyjMtzOCa44036307;     ddXnEncnwrSduyjMtzOCa44036307 = ddXnEncnwrSduyjMtzOCa18071542;     ddXnEncnwrSduyjMtzOCa18071542 = ddXnEncnwrSduyjMtzOCa49884236;     ddXnEncnwrSduyjMtzOCa49884236 = ddXnEncnwrSduyjMtzOCa72459112;     ddXnEncnwrSduyjMtzOCa72459112 = ddXnEncnwrSduyjMtzOCa74238306;     ddXnEncnwrSduyjMtzOCa74238306 = ddXnEncnwrSduyjMtzOCa8743316;     ddXnEncnwrSduyjMtzOCa8743316 = ddXnEncnwrSduyjMtzOCa85048820;     ddXnEncnwrSduyjMtzOCa85048820 = ddXnEncnwrSduyjMtzOCa40344401;     ddXnEncnwrSduyjMtzOCa40344401 = ddXnEncnwrSduyjMtzOCa93232509;     ddXnEncnwrSduyjMtzOCa93232509 = ddXnEncnwrSduyjMtzOCa15332699;     ddXnEncnwrSduyjMtzOCa15332699 = ddXnEncnwrSduyjMtzOCa33615034;     ddXnEncnwrSduyjMtzOCa33615034 = ddXnEncnwrSduyjMtzOCa98617736;     ddXnEncnwrSduyjMtzOCa98617736 = ddXnEncnwrSduyjMtzOCa99373439;     ddXnEncnwrSduyjMtzOCa99373439 = ddXnEncnwrSduyjMtzOCa65794886;     ddXnEncnwrSduyjMtzOCa65794886 = ddXnEncnwrSduyjMtzOCa12230290;     ddXnEncnwrSduyjMtzOCa12230290 = ddXnEncnwrSduyjMtzOCa97957102;     ddXnEncnwrSduyjMtzOCa97957102 = ddXnEncnwrSduyjMtzOCa26714176;     ddXnEncnwrSduyjMtzOCa26714176 = ddXnEncnwrSduyjMtzOCa29476490;     ddXnEncnwrSduyjMtzOCa29476490 = ddXnEncnwrSduyjMtzOCa91859917;     ddXnEncnwrSduyjMtzOCa91859917 = ddXnEncnwrSduyjMtzOCa13478568;     ddXnEncnwrSduyjMtzOCa13478568 = ddXnEncnwrSduyjMtzOCa80450704;     ddXnEncnwrSduyjMtzOCa80450704 = ddXnEncnwrSduyjMtzOCa13656650;     ddXnEncnwrSduyjMtzOCa13656650 = ddXnEncnwrSduyjMtzOCa61766270;     ddXnEncnwrSduyjMtzOCa61766270 = ddXnEncnwrSduyjMtzOCa77020744;     ddXnEncnwrSduyjMtzOCa77020744 = ddXnEncnwrSduyjMtzOCa35316118;     ddXnEncnwrSduyjMtzOCa35316118 = ddXnEncnwrSduyjMtzOCa8589284;     ddXnEncnwrSduyjMtzOCa8589284 = ddXnEncnwrSduyjMtzOCa18516733;     ddXnEncnwrSduyjMtzOCa18516733 = ddXnEncnwrSduyjMtzOCa46609847;     ddXnEncnwrSduyjMtzOCa46609847 = ddXnEncnwrSduyjMtzOCa96211948;     ddXnEncnwrSduyjMtzOCa96211948 = ddXnEncnwrSduyjMtzOCa28094549;     ddXnEncnwrSduyjMtzOCa28094549 = ddXnEncnwrSduyjMtzOCa96192338;     ddXnEncnwrSduyjMtzOCa96192338 = ddXnEncnwrSduyjMtzOCa19853782;     ddXnEncnwrSduyjMtzOCa19853782 = ddXnEncnwrSduyjMtzOCa83425990;     ddXnEncnwrSduyjMtzOCa83425990 = ddXnEncnwrSduyjMtzOCa51565837;     ddXnEncnwrSduyjMtzOCa51565837 = ddXnEncnwrSduyjMtzOCa39732310;     ddXnEncnwrSduyjMtzOCa39732310 = ddXnEncnwrSduyjMtzOCa22307590;     ddXnEncnwrSduyjMtzOCa22307590 = ddXnEncnwrSduyjMtzOCa43394687;     ddXnEncnwrSduyjMtzOCa43394687 = ddXnEncnwrSduyjMtzOCa57675978;     ddXnEncnwrSduyjMtzOCa57675978 = ddXnEncnwrSduyjMtzOCa64922543;     ddXnEncnwrSduyjMtzOCa64922543 = ddXnEncnwrSduyjMtzOCa90665451;     ddXnEncnwrSduyjMtzOCa90665451 = ddXnEncnwrSduyjMtzOCa7071713;     ddXnEncnwrSduyjMtzOCa7071713 = ddXnEncnwrSduyjMtzOCa91707431;     ddXnEncnwrSduyjMtzOCa91707431 = ddXnEncnwrSduyjMtzOCa7214996;     ddXnEncnwrSduyjMtzOCa7214996 = ddXnEncnwrSduyjMtzOCa32816073;     ddXnEncnwrSduyjMtzOCa32816073 = ddXnEncnwrSduyjMtzOCa51189531;     ddXnEncnwrSduyjMtzOCa51189531 = ddXnEncnwrSduyjMtzOCa88869151;     ddXnEncnwrSduyjMtzOCa88869151 = ddXnEncnwrSduyjMtzOCa14029587;     ddXnEncnwrSduyjMtzOCa14029587 = ddXnEncnwrSduyjMtzOCa11709546;}
// Junk Finished
