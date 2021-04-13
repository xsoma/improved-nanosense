#include "AntiAim.hpp"

#include "../Structs.hpp"
#include "../Options.hpp"

#include "AngryPew.hpp"
#include "Resolver.hpp"
#include "RebuildGameMovement.hpp"
#include "Miscellaneous.hpp"
#include "PredictionSystem.hpp"

#include "../helpers/Utils.hpp"
#include "../helpers/Math.hpp"

#include <time.h>

void AntiAim::Work(CUserCmd* usercmd)
{
	if (!g_EngineClient->IsConnected() && !g_EngineClient->IsInGame())
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	this->usercmd = usercmd;

	if (usercmd->buttons & IN_USE || g_LocalPlayer->m_fFlags() & FL_FROZEN || g_LocalPlayer->IsDormant())
		return;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (usercmd->buttons & IN_ATTACK2)
			return;

		if (weapon->CanFirePostPone() && (usercmd->buttons & IN_ATTACK))
			return;
	}
	else if (weapon->GetWeapInfo()->weapon_type() == WEAPONTYPE_GRENADE)
	{
		if (weapon->IsInThrow())
			return;
	}
	else
	{
		if (weapon->GetWeapInfo()->weapon_type() == WEAPONTYPE_KNIFE && ((usercmd->buttons & IN_ATTACK) || (usercmd->buttons & IN_ATTACK2)))
			return;
		else if ((usercmd->buttons & IN_ATTACK) && (weapon->m_iItemDefinitionIndex() != WEAPON_C4 || XSystemCFG.hvh_antiaim_x != AA_PITCH_OFF))
			return;
	}

	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
		return;

	if (!Global::bFakelag)
		Global::bSendPacket = usercmd->command_number % 2;

	usercmd->viewangles.pitch = GetPitch();

	if (!Global::bSendPacket)
		usercmd->viewangles.yaw = GetYaw();
	else
		usercmd->viewangles.yaw = GetFakeYaw();
}

float AntiAim::GetPitch()
{
	switch (XSystemCFG.hvh_antiaim_x)
	{
	case AA_PITCH_OFF:

		return usercmd->viewangles.pitch;
		break;

	case AA_PITCH_DYNAMIC:

		return g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() ? (g_LocalPlayer->m_hActiveWeapon().Get()->m_zoomLevel() != 0 ? 87.f : 85.f) : 88.99f;
		break;

	case AA_PITCH_EMOTION:

		return 88.99f;
		break;

	case AA_PITCH_STRAIGHT:

		return 0.f;
		break;

	case AA_PITCH_UP:

		return -88.99f;
		break;
	}

	return usercmd->viewangles.pitch;
}

float AntiAim::GetYaw()
{
	static bool flip = false;
	flip = !flip;

	if (g_InputSystem->IsButtonDown(XSystemCFG.hvh_aa_left_bind)) { Global::left = true; Global::right = false;  Global::backwards = false; }
	else if (g_InputSystem->IsButtonDown(XSystemCFG.hvh_aa_right_bind)) { Global::left = false; Global::right = true; Global::backwards = false; }
	else if (g_InputSystem->IsButtonDown(XSystemCFG.hvh_aa_back_bind)) { Global::left = false; Global::right = false; Global::backwards = true; }

	float_t pos = usercmd->viewangles.yaw;
	float_t desync_pos_left = usercmd->viewangles.yaw += XSystemCFG.hvh_antiaim_y_desync_start_left;
	float_t desync_pos_right = usercmd->viewangles.yaw += XSystemCFG.hvh_antiaim_y_desync_start_right;
	float_t desync_pos_back = usercmd->viewangles.yaw += XSystemCFG.hvh_antiaim_y_desync_start_back;

	if (g_LocalPlayer->m_vecVelocity().Length2D() > XSystemCFG.hvh_antiaim_y_move_trigger_speed&& XSystemCFG.hvh_antiaim_y_move != 0)
	{
		switch (XSystemCFG.hvh_antiaim_y_move)
		{
		case 1:

			return pos + 180.0f;
			break;

		case 2:

			return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);

			break;

		case 3:


			if (Global::left) // Global::left real
				return pos + 90.f;

			else if (Global::right) // Global::right real
				return pos - 90.f;

			else if (Global::backwards) // Global::backwards
				return pos + 180.f;
			break;

		case 4:

			if (Global::left)
				return pos + (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 5:

			if (Global::left)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_left;

			else if (Global::right)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_right;

			else if (Global::backwards)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_back;

			break;
		case 6:

			if (Global::left)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_left + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_right + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_back + (Utils::RandomFloat(-25.5f, 25.5f));
			break;
		}
		return pos;
	}
	else
	{
		switch (XSystemCFG.hvh_antiaim_y)
		{
		case 1:

			return pos + 180.0f;
			break;

		case 2:

			return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);

			break;

		case 3:


			if (Global::left) // Global::left real
				return pos + 90.f;

			else if (Global::right) // Global::right real
				return pos - 90.f;

			else if (Global::backwards) // Global::backwards
				return pos + 180.f;
			break;

		case 4:

			if (Global::left)
				return pos + (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos - (180.f + Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 5:

			if (Global::left)
				return pos + XSystemCFG.hvh_antiaim_y_custom_left;

			else if (Global::right)
				return pos + XSystemCFG.hvh_antiaim_y_custom_right;

			else if (Global::backwards)
				return pos + XSystemCFG.hvh_antiaim_y_custom_back;

			break;
		case 6:

			if (Global::left)
				return pos + XSystemCFG.hvh_antiaim_y_custom_left + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos + XSystemCFG.hvh_antiaim_y_custom_right + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos + XSystemCFG.hvh_antiaim_y_custom_back + (Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 7:

			if (Global::left)
				return desync_pos_left;
			if (Global::right)
				return desync_pos_right;
			if (Global::backwards)
				return desync_pos_back;
			break;
		case 8:

			if (Global::left)
				return desync_pos_left + (Utils::RandomFloat(-25.5f, 25.5f));
			if (Global::right)
				return desync_pos_right + (Utils::RandomFloat(-25.5f, 25.5f));
			if (Global::backwards)
				return desync_pos_back + (Utils::RandomFloat(-25.5f, 25.5f));
			break;
		}
		return pos;
	}
}
float AntiAim::GetFakeYaw()
{
	static bool flip = false;
	flip = !flip;

	if (g_InputSystem->IsButtonDown(XSystemCFG.hvh_aa_left_bind)) { Global::left = true; Global::right = false;  Global::backwards = false; }
	else if (g_InputSystem->IsButtonDown(XSystemCFG.hvh_aa_right_bind)) { Global::left = false; Global::right = true; Global::backwards = false; }
	else if (g_InputSystem->IsButtonDown(XSystemCFG.hvh_aa_back_bind)) { Global::left = false; Global::right = false; Global::backwards = true; }

	float_t pos = usercmd->viewangles.yaw;
	float_t desync_pos_left = usercmd->viewangles.yaw += XSystemCFG.hvh_antiaim_y_desync_start_left;
	float_t desync_pos_right = usercmd->viewangles.yaw += XSystemCFG.hvh_antiaim_y_desync_start_right;
	float_t desync_pos_back = usercmd->viewangles.yaw += XSystemCFG.hvh_antiaim_y_desync_start_back;

	if (g_LocalPlayer->m_vecVelocity().Length2D() > XSystemCFG.hvh_antiaim_y_move_trigger_speed&& XSystemCFG.hvh_antiaim_y_move != 0)
	{
		switch (XSystemCFG.hvh_antiaim_y_move)
		{
		case 1:

			return pos + 180.0f;
			break;

		case 2:

			return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);

			break;

		case 3:


			if (Global::left) // Global::left real
				return pos + 90.f;

			else if (Global::right) // Global::right real
				return pos - 90.f;

			else if (Global::backwards) // Global::backwards
				return pos + 180.f;
			break;

		case 4:

			if (Global::left)
				return pos + (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 5:

			if (Global::left)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_left;

			else if (Global::right)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_right;

			else if (Global::backwards)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_back;

			break;
		case 6:

			if (Global::left)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_left + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_right + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos + XSystemCFG.hvh_antiaim_y_custom_realmove_back + (Utils::RandomFloat(-25.5f, 25.5f));
			break;
		}
		return pos;
	}
	else
	{
		switch (XSystemCFG.hvh_antiaim_y)
		{
		case 1:

			return pos + 180.0f;
			break;

		case 2:

			return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);

			break;

		case 3:


			if (Global::left) // Global::left real
				return pos + 90.f;

			else if (Global::right) // Global::right real
				return pos - 90.f;

			else if (Global::backwards) // Global::backwards
				return pos + 180.f;
			break;

		case 4:

			if (Global::left)
				return pos + (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos - (180.f + Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 5:

			if (Global::left)
				return pos + XSystemCFG.hvh_antiaim_y_custom_left;

			else if (Global::right)
				return pos + XSystemCFG.hvh_antiaim_y_custom_right;

			else if (Global::backwards)
				return pos + XSystemCFG.hvh_antiaim_y_custom_back;

			break;
		case 6:

			if (Global::left)
				return pos + XSystemCFG.hvh_antiaim_y_custom_left + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::right)
				return pos + XSystemCFG.hvh_antiaim_y_custom_right + (Utils::RandomFloat(-25.5f, 25.5f));

			else if (Global::backwards)
				return pos + XSystemCFG.hvh_antiaim_y_custom_back + (Utils::RandomFloat(-25.5f, 25.5f));

			break;
		case 7:
			if (Global::left)
				return flip ? desync_pos_left + XSystemCFG.hvh_antiaim_y_desync : desync_pos_left - XSystemCFG.hvh_antiaim_y_desync;
			if (Global::right)
				return flip ? desync_pos_right + XSystemCFG.hvh_antiaim_y_desync : desync_pos_right - XSystemCFG.hvh_antiaim_y_desync;
			if (Global::backwards)
				return flip ? desync_pos_back + XSystemCFG.hvh_antiaim_y_desync : desync_pos_back - XSystemCFG.hvh_antiaim_y_desync;
			break;
		case 8:
			if (Global::left)
				return flip ? desync_pos_left + XSystemCFG.hvh_antiaim_y_desync : desync_pos_left - XSystemCFG.hvh_antiaim_y_desync;
			if (Global::right)
				return flip ? desync_pos_right + XSystemCFG.hvh_antiaim_y_desync : desync_pos_right - XSystemCFG.hvh_antiaim_y_desync;
			if (Global::backwards)
				return flip ? desync_pos_back + XSystemCFG.hvh_antiaim_y_desync : desync_pos_back - XSystemCFG.hvh_antiaim_y_desync;
			break;
		}
		return pos;
	}
}

void AntiAim::Accelerate(C_BasePlayer* player, Vector& wishdir, float wishspeed, float accel, Vector& outVel)
{
	// See if we are changing direction a bit
	float currentspeed = outVel.Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	float addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
		return;

	// Determine amount of accleration.
	float accelspeed = accel * g_GlobalVars->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	for (int i = 0; i < 3; i++)
		outVel[i] += accelspeed * wishdir[i];
}

void AntiAim::WalkMove(C_BasePlayer* player, Vector& outVel)
{
	Vector forward, right, up, wishvel, wishdir, dest;
	float_t fmove, smove, wishspeed;

	Math::AngleVectors(player->m_angEyeAngles(), forward, right, up);  // Determine movement angles
	// Copy movement amounts
	g_MoveHelper->SetHost(player);
	fmove = g_MoveHelper->m_flForwardMove;
	smove = g_MoveHelper->m_flSideMove;
	g_MoveHelper->SetHost(nullptr);

	if (forward[2] != 0)
	{
		forward[2] = 0;
		Math::NormalizeVector(forward);
	}

	if (right[2] != 0)
	{
		right[2] = 0;
		Math::NormalizeVector(right);
	}

	for (int i = 0; i < 2; i++)	// Determine x and y parts of velocity
		wishvel[i] = forward[i] * fmove + right[i] * smove;

	wishvel[2] = 0;	// Zero out z part of velocity

	wishdir = wishvel; // Determine maginitude of speed of move
	wishspeed = wishdir.Normalize();

	// Clamp to server defined max speed
	g_MoveHelper->SetHost(player);
	if ((wishspeed != 0.0f) && (wishspeed > g_MoveHelper->m_flMaxSpeed))
	{
		VectorMultiply(wishvel, player->m_flMaxspeed() / wishspeed, wishvel);
		wishspeed = player->m_flMaxspeed();
	}
	g_MoveHelper->SetHost(nullptr);
	// Set pmove velocity
	outVel[2] = 0;
	Accelerate(player, wishdir, wishspeed, g_CVar->FindVar("sv_accelerate")->GetFloat(), outVel);
	outVel[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(outVel, player->m_vecBaseVelocity(), outVel);

	float spd = outVel.Length();

	if (spd < 1.0f)
	{
		outVel.Init();
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
		return;
	}

	g_MoveHelper->SetHost(player);
	g_MoveHelper->m_outWishVel += wishdir * wishspeed;
	g_MoveHelper->SetHost(nullptr);

	// Don't walk up stairs if not on ground.
	if (!(player->m_fFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
		return;
	}

	// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
}

void AntiAim::Fakewalk(CUserCmd* usercmd)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (!g_InputSystem->IsButtonDown(XSystemCFG.misc_fakewalk_bind))
		return;

	Vector velocity = Global::vecUnpredictedVel;
	float_t speed = velocity.Length();

	if (speed > XSystemCFG.misc_fakewalk_speed)
	{
		QAngle direction;
		Math::VectorAngles(velocity, direction);

		direction.yaw = usercmd->viewangles.yaw - direction.yaw;

		Vector forward;
		Math::AngleVectors(direction, forward);

		int divider = XSystemCFG.misc_fakewalk_speed / speed;
		Vector slowedDirection = forward * divider * speed;
		usercmd->forwardmove = slowedDirection.x;
		usercmd->sidemove = slowedDirection.y;
	}
	if (XSystemCFG.tickbase_manipulation) {
		if (!g_LocalPlayer->IsAlive() || !g_EngineClient->IsInGame())
			return;

		if (GetAsyncKeyState(XSystemCFG.tickbase_manipulation_key)) {
			static int choked = 0;
			choked = choked > 7 ? 0 : choked + 1;
			usercmd->forwardmove = choked < 2 || choked > 5 ? 0 : usercmd->forwardmove;
			usercmd->sidemove = choked < 2 || choked > 5 ? 0 : usercmd->sidemove;
			Global::bSendPacket = choked < 1;
			usercmd->tick_count = INT_MAX;
		}
	}
	if (XSystemCFG.tickbase_freeze) {
		if (!g_LocalPlayer->IsAlive() || !g_EngineClient->IsInGame())
			return;

		if (GetAsyncKeyState(XSystemCFG.tickbase_freeze_key)) {
			static int choked = 0;
			choked = choked > 7 ? 0 : choked + 1;
			Global::bSendPacket = 5 + choked < 1;
			usercmd->tick_count = INT_MAX;

		}
	}
}

void AntiAim::Friction(Vector& outVel)
{
	float speed, newspeed, control;
	float friction;
	float drop;

	speed = outVel.Length();

	if (speed <= 0.1f)
		return;

	drop = 0;

	// apply ground friction
	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
	{
		friction = g_CVar->FindVar("sv_friction")->GetFloat() * g_LocalPlayer->m_surfaceFriction();

		// Bleed off some speed, but if we have less than the bleed
		// threshold, bleed the threshold amount.
		control = (speed < g_CVar->FindVar("sv_stopspeed")->GetFloat()) ? g_CVar->FindVar("sv_stopspeed")->GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * g_GlobalVars->frametime;
	}

	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;
		// Adjust velocity according to proportion.
		VectorMultiply(outVel, newspeed, outVel);
	}
}



















































































































// Junk Code By Troll Face & Thaisen's Gen
void EudJQpKRbmEFmmBkwVnvQrGXpaJLGSxsxvMTVULy9351431() { double fmfCwREIGlTqgmlNoGTqgzL19453749 = -166190215;    double fmfCwREIGlTqgmlNoGTqgzL58064412 = -103143500;    double fmfCwREIGlTqgmlNoGTqgzL6211978 = -68636035;    double fmfCwREIGlTqgmlNoGTqgzL29114842 = -748180063;    double fmfCwREIGlTqgmlNoGTqgzL29399818 = 99150196;    double fmfCwREIGlTqgmlNoGTqgzL16094555 = -402991928;    double fmfCwREIGlTqgmlNoGTqgzL63088185 = -475061573;    double fmfCwREIGlTqgmlNoGTqgzL35150082 = -820686546;    double fmfCwREIGlTqgmlNoGTqgzL42347293 = -729129232;    double fmfCwREIGlTqgmlNoGTqgzL74627189 = -596269238;    double fmfCwREIGlTqgmlNoGTqgzL96790794 = -433686208;    double fmfCwREIGlTqgmlNoGTqgzL90639573 = -668348516;    double fmfCwREIGlTqgmlNoGTqgzL93417623 = -897723623;    double fmfCwREIGlTqgmlNoGTqgzL54174099 = -103974667;    double fmfCwREIGlTqgmlNoGTqgzL39337288 = -33837446;    double fmfCwREIGlTqgmlNoGTqgzL32829670 = -657497796;    double fmfCwREIGlTqgmlNoGTqgzL47904488 = -824535989;    double fmfCwREIGlTqgmlNoGTqgzL82926888 = -399869011;    double fmfCwREIGlTqgmlNoGTqgzL48970299 = -900686424;    double fmfCwREIGlTqgmlNoGTqgzL57893642 = -407266989;    double fmfCwREIGlTqgmlNoGTqgzL62389788 = -958766465;    double fmfCwREIGlTqgmlNoGTqgzL43131327 = -164414026;    double fmfCwREIGlTqgmlNoGTqgzL59239120 = -974550902;    double fmfCwREIGlTqgmlNoGTqgzL56300738 = -766144911;    double fmfCwREIGlTqgmlNoGTqgzL9815956 = -904871286;    double fmfCwREIGlTqgmlNoGTqgzL23742018 = -555510037;    double fmfCwREIGlTqgmlNoGTqgzL34591529 = 66253801;    double fmfCwREIGlTqgmlNoGTqgzL22304832 = -93188324;    double fmfCwREIGlTqgmlNoGTqgzL71089384 = 40072648;    double fmfCwREIGlTqgmlNoGTqgzL180565 = -96325366;    double fmfCwREIGlTqgmlNoGTqgzL24158651 = -755925377;    double fmfCwREIGlTqgmlNoGTqgzL20176590 = -464144886;    double fmfCwREIGlTqgmlNoGTqgzL94466545 = -685622949;    double fmfCwREIGlTqgmlNoGTqgzL69032856 = -983057372;    double fmfCwREIGlTqgmlNoGTqgzL54618484 = -317547369;    double fmfCwREIGlTqgmlNoGTqgzL64757257 = -600783251;    double fmfCwREIGlTqgmlNoGTqgzL71166365 = -277332886;    double fmfCwREIGlTqgmlNoGTqgzL35135606 = -978667061;    double fmfCwREIGlTqgmlNoGTqgzL3073632 = -894613515;    double fmfCwREIGlTqgmlNoGTqgzL17960633 = -132487645;    double fmfCwREIGlTqgmlNoGTqgzL8262379 = -462517520;    double fmfCwREIGlTqgmlNoGTqgzL52130942 = -377790745;    double fmfCwREIGlTqgmlNoGTqgzL18291977 = 51446559;    double fmfCwREIGlTqgmlNoGTqgzL90563963 = -60947419;    double fmfCwREIGlTqgmlNoGTqgzL15687352 = -705057113;    double fmfCwREIGlTqgmlNoGTqgzL71973917 = -606399864;    double fmfCwREIGlTqgmlNoGTqgzL5917177 = -868469991;    double fmfCwREIGlTqgmlNoGTqgzL84225066 = -621821424;    double fmfCwREIGlTqgmlNoGTqgzL97472752 = -894588409;    double fmfCwREIGlTqgmlNoGTqgzL61586050 = -941468528;    double fmfCwREIGlTqgmlNoGTqgzL67042344 = -433194739;    double fmfCwREIGlTqgmlNoGTqgzL27574932 = -631358305;    double fmfCwREIGlTqgmlNoGTqgzL47310889 = -261935841;    double fmfCwREIGlTqgmlNoGTqgzL74255879 = -51683339;    double fmfCwREIGlTqgmlNoGTqgzL46961643 = -611428912;    double fmfCwREIGlTqgmlNoGTqgzL76322422 = 98223810;    double fmfCwREIGlTqgmlNoGTqgzL98825291 = -128592598;    double fmfCwREIGlTqgmlNoGTqgzL49911239 = -302491124;    double fmfCwREIGlTqgmlNoGTqgzL19298887 = -843308777;    double fmfCwREIGlTqgmlNoGTqgzL5657801 = -345339767;    double fmfCwREIGlTqgmlNoGTqgzL81503026 = -369245730;    double fmfCwREIGlTqgmlNoGTqgzL40783354 = -281873249;    double fmfCwREIGlTqgmlNoGTqgzL64060697 = -760759194;    double fmfCwREIGlTqgmlNoGTqgzL42166728 = -532803866;    double fmfCwREIGlTqgmlNoGTqgzL50468538 = -840343862;    double fmfCwREIGlTqgmlNoGTqgzL76614204 = -969541322;    double fmfCwREIGlTqgmlNoGTqgzL96173027 = -982725567;    double fmfCwREIGlTqgmlNoGTqgzL24384768 = -914666252;    double fmfCwREIGlTqgmlNoGTqgzL99555615 = -786427299;    double fmfCwREIGlTqgmlNoGTqgzL74580030 = -433054196;    double fmfCwREIGlTqgmlNoGTqgzL61663304 = -280164911;    double fmfCwREIGlTqgmlNoGTqgzL12768883 = -845868928;    double fmfCwREIGlTqgmlNoGTqgzL79853256 = -505255497;    double fmfCwREIGlTqgmlNoGTqgzL31009666 = -668198779;    double fmfCwREIGlTqgmlNoGTqgzL49631264 = -944749469;    double fmfCwREIGlTqgmlNoGTqgzL10258847 = -480975721;    double fmfCwREIGlTqgmlNoGTqgzL24839350 = -115860585;    double fmfCwREIGlTqgmlNoGTqgzL68675157 = -813603484;    double fmfCwREIGlTqgmlNoGTqgzL40613387 = 38912201;    double fmfCwREIGlTqgmlNoGTqgzL37842038 = -198471423;    double fmfCwREIGlTqgmlNoGTqgzL17824841 = -687040047;    double fmfCwREIGlTqgmlNoGTqgzL50366462 = -311924775;    double fmfCwREIGlTqgmlNoGTqgzL24832080 = -198599916;    double fmfCwREIGlTqgmlNoGTqgzL9503335 = -18458825;    double fmfCwREIGlTqgmlNoGTqgzL33138221 = -663130628;    double fmfCwREIGlTqgmlNoGTqgzL96583718 = -24567072;    double fmfCwREIGlTqgmlNoGTqgzL72865701 = -102209046;    double fmfCwREIGlTqgmlNoGTqgzL20210666 = -533939611;    double fmfCwREIGlTqgmlNoGTqgzL22071213 = -271628461;    double fmfCwREIGlTqgmlNoGTqgzL78296062 = -315771180;    double fmfCwREIGlTqgmlNoGTqgzL65931965 = -372190653;    double fmfCwREIGlTqgmlNoGTqgzL21255126 = -974841762;    double fmfCwREIGlTqgmlNoGTqgzL15836719 = -35358285;    double fmfCwREIGlTqgmlNoGTqgzL97415831 = -449273748;    double fmfCwREIGlTqgmlNoGTqgzL36457607 = -763241915;    double fmfCwREIGlTqgmlNoGTqgzL67479025 = -80644271;    double fmfCwREIGlTqgmlNoGTqgzL88070244 = -617031552;    double fmfCwREIGlTqgmlNoGTqgzL76125249 = -415749575;    double fmfCwREIGlTqgmlNoGTqgzL40095425 = -220603557;    double fmfCwREIGlTqgmlNoGTqgzL39073148 = -166190215;     fmfCwREIGlTqgmlNoGTqgzL19453749 = fmfCwREIGlTqgmlNoGTqgzL58064412;     fmfCwREIGlTqgmlNoGTqgzL58064412 = fmfCwREIGlTqgmlNoGTqgzL6211978;     fmfCwREIGlTqgmlNoGTqgzL6211978 = fmfCwREIGlTqgmlNoGTqgzL29114842;     fmfCwREIGlTqgmlNoGTqgzL29114842 = fmfCwREIGlTqgmlNoGTqgzL29399818;     fmfCwREIGlTqgmlNoGTqgzL29399818 = fmfCwREIGlTqgmlNoGTqgzL16094555;     fmfCwREIGlTqgmlNoGTqgzL16094555 = fmfCwREIGlTqgmlNoGTqgzL63088185;     fmfCwREIGlTqgmlNoGTqgzL63088185 = fmfCwREIGlTqgmlNoGTqgzL35150082;     fmfCwREIGlTqgmlNoGTqgzL35150082 = fmfCwREIGlTqgmlNoGTqgzL42347293;     fmfCwREIGlTqgmlNoGTqgzL42347293 = fmfCwREIGlTqgmlNoGTqgzL74627189;     fmfCwREIGlTqgmlNoGTqgzL74627189 = fmfCwREIGlTqgmlNoGTqgzL96790794;     fmfCwREIGlTqgmlNoGTqgzL96790794 = fmfCwREIGlTqgmlNoGTqgzL90639573;     fmfCwREIGlTqgmlNoGTqgzL90639573 = fmfCwREIGlTqgmlNoGTqgzL93417623;     fmfCwREIGlTqgmlNoGTqgzL93417623 = fmfCwREIGlTqgmlNoGTqgzL54174099;     fmfCwREIGlTqgmlNoGTqgzL54174099 = fmfCwREIGlTqgmlNoGTqgzL39337288;     fmfCwREIGlTqgmlNoGTqgzL39337288 = fmfCwREIGlTqgmlNoGTqgzL32829670;     fmfCwREIGlTqgmlNoGTqgzL32829670 = fmfCwREIGlTqgmlNoGTqgzL47904488;     fmfCwREIGlTqgmlNoGTqgzL47904488 = fmfCwREIGlTqgmlNoGTqgzL82926888;     fmfCwREIGlTqgmlNoGTqgzL82926888 = fmfCwREIGlTqgmlNoGTqgzL48970299;     fmfCwREIGlTqgmlNoGTqgzL48970299 = fmfCwREIGlTqgmlNoGTqgzL57893642;     fmfCwREIGlTqgmlNoGTqgzL57893642 = fmfCwREIGlTqgmlNoGTqgzL62389788;     fmfCwREIGlTqgmlNoGTqgzL62389788 = fmfCwREIGlTqgmlNoGTqgzL43131327;     fmfCwREIGlTqgmlNoGTqgzL43131327 = fmfCwREIGlTqgmlNoGTqgzL59239120;     fmfCwREIGlTqgmlNoGTqgzL59239120 = fmfCwREIGlTqgmlNoGTqgzL56300738;     fmfCwREIGlTqgmlNoGTqgzL56300738 = fmfCwREIGlTqgmlNoGTqgzL9815956;     fmfCwREIGlTqgmlNoGTqgzL9815956 = fmfCwREIGlTqgmlNoGTqgzL23742018;     fmfCwREIGlTqgmlNoGTqgzL23742018 = fmfCwREIGlTqgmlNoGTqgzL34591529;     fmfCwREIGlTqgmlNoGTqgzL34591529 = fmfCwREIGlTqgmlNoGTqgzL22304832;     fmfCwREIGlTqgmlNoGTqgzL22304832 = fmfCwREIGlTqgmlNoGTqgzL71089384;     fmfCwREIGlTqgmlNoGTqgzL71089384 = fmfCwREIGlTqgmlNoGTqgzL180565;     fmfCwREIGlTqgmlNoGTqgzL180565 = fmfCwREIGlTqgmlNoGTqgzL24158651;     fmfCwREIGlTqgmlNoGTqgzL24158651 = fmfCwREIGlTqgmlNoGTqgzL20176590;     fmfCwREIGlTqgmlNoGTqgzL20176590 = fmfCwREIGlTqgmlNoGTqgzL94466545;     fmfCwREIGlTqgmlNoGTqgzL94466545 = fmfCwREIGlTqgmlNoGTqgzL69032856;     fmfCwREIGlTqgmlNoGTqgzL69032856 = fmfCwREIGlTqgmlNoGTqgzL54618484;     fmfCwREIGlTqgmlNoGTqgzL54618484 = fmfCwREIGlTqgmlNoGTqgzL64757257;     fmfCwREIGlTqgmlNoGTqgzL64757257 = fmfCwREIGlTqgmlNoGTqgzL71166365;     fmfCwREIGlTqgmlNoGTqgzL71166365 = fmfCwREIGlTqgmlNoGTqgzL35135606;     fmfCwREIGlTqgmlNoGTqgzL35135606 = fmfCwREIGlTqgmlNoGTqgzL3073632;     fmfCwREIGlTqgmlNoGTqgzL3073632 = fmfCwREIGlTqgmlNoGTqgzL17960633;     fmfCwREIGlTqgmlNoGTqgzL17960633 = fmfCwREIGlTqgmlNoGTqgzL8262379;     fmfCwREIGlTqgmlNoGTqgzL8262379 = fmfCwREIGlTqgmlNoGTqgzL52130942;     fmfCwREIGlTqgmlNoGTqgzL52130942 = fmfCwREIGlTqgmlNoGTqgzL18291977;     fmfCwREIGlTqgmlNoGTqgzL18291977 = fmfCwREIGlTqgmlNoGTqgzL90563963;     fmfCwREIGlTqgmlNoGTqgzL90563963 = fmfCwREIGlTqgmlNoGTqgzL15687352;     fmfCwREIGlTqgmlNoGTqgzL15687352 = fmfCwREIGlTqgmlNoGTqgzL71973917;     fmfCwREIGlTqgmlNoGTqgzL71973917 = fmfCwREIGlTqgmlNoGTqgzL5917177;     fmfCwREIGlTqgmlNoGTqgzL5917177 = fmfCwREIGlTqgmlNoGTqgzL84225066;     fmfCwREIGlTqgmlNoGTqgzL84225066 = fmfCwREIGlTqgmlNoGTqgzL97472752;     fmfCwREIGlTqgmlNoGTqgzL97472752 = fmfCwREIGlTqgmlNoGTqgzL61586050;     fmfCwREIGlTqgmlNoGTqgzL61586050 = fmfCwREIGlTqgmlNoGTqgzL67042344;     fmfCwREIGlTqgmlNoGTqgzL67042344 = fmfCwREIGlTqgmlNoGTqgzL27574932;     fmfCwREIGlTqgmlNoGTqgzL27574932 = fmfCwREIGlTqgmlNoGTqgzL47310889;     fmfCwREIGlTqgmlNoGTqgzL47310889 = fmfCwREIGlTqgmlNoGTqgzL74255879;     fmfCwREIGlTqgmlNoGTqgzL74255879 = fmfCwREIGlTqgmlNoGTqgzL46961643;     fmfCwREIGlTqgmlNoGTqgzL46961643 = fmfCwREIGlTqgmlNoGTqgzL76322422;     fmfCwREIGlTqgmlNoGTqgzL76322422 = fmfCwREIGlTqgmlNoGTqgzL98825291;     fmfCwREIGlTqgmlNoGTqgzL98825291 = fmfCwREIGlTqgmlNoGTqgzL49911239;     fmfCwREIGlTqgmlNoGTqgzL49911239 = fmfCwREIGlTqgmlNoGTqgzL19298887;     fmfCwREIGlTqgmlNoGTqgzL19298887 = fmfCwREIGlTqgmlNoGTqgzL5657801;     fmfCwREIGlTqgmlNoGTqgzL5657801 = fmfCwREIGlTqgmlNoGTqgzL81503026;     fmfCwREIGlTqgmlNoGTqgzL81503026 = fmfCwREIGlTqgmlNoGTqgzL40783354;     fmfCwREIGlTqgmlNoGTqgzL40783354 = fmfCwREIGlTqgmlNoGTqgzL64060697;     fmfCwREIGlTqgmlNoGTqgzL64060697 = fmfCwREIGlTqgmlNoGTqgzL42166728;     fmfCwREIGlTqgmlNoGTqgzL42166728 = fmfCwREIGlTqgmlNoGTqgzL50468538;     fmfCwREIGlTqgmlNoGTqgzL50468538 = fmfCwREIGlTqgmlNoGTqgzL76614204;     fmfCwREIGlTqgmlNoGTqgzL76614204 = fmfCwREIGlTqgmlNoGTqgzL96173027;     fmfCwREIGlTqgmlNoGTqgzL96173027 = fmfCwREIGlTqgmlNoGTqgzL24384768;     fmfCwREIGlTqgmlNoGTqgzL24384768 = fmfCwREIGlTqgmlNoGTqgzL99555615;     fmfCwREIGlTqgmlNoGTqgzL99555615 = fmfCwREIGlTqgmlNoGTqgzL74580030;     fmfCwREIGlTqgmlNoGTqgzL74580030 = fmfCwREIGlTqgmlNoGTqgzL61663304;     fmfCwREIGlTqgmlNoGTqgzL61663304 = fmfCwREIGlTqgmlNoGTqgzL12768883;     fmfCwREIGlTqgmlNoGTqgzL12768883 = fmfCwREIGlTqgmlNoGTqgzL79853256;     fmfCwREIGlTqgmlNoGTqgzL79853256 = fmfCwREIGlTqgmlNoGTqgzL31009666;     fmfCwREIGlTqgmlNoGTqgzL31009666 = fmfCwREIGlTqgmlNoGTqgzL49631264;     fmfCwREIGlTqgmlNoGTqgzL49631264 = fmfCwREIGlTqgmlNoGTqgzL10258847;     fmfCwREIGlTqgmlNoGTqgzL10258847 = fmfCwREIGlTqgmlNoGTqgzL24839350;     fmfCwREIGlTqgmlNoGTqgzL24839350 = fmfCwREIGlTqgmlNoGTqgzL68675157;     fmfCwREIGlTqgmlNoGTqgzL68675157 = fmfCwREIGlTqgmlNoGTqgzL40613387;     fmfCwREIGlTqgmlNoGTqgzL40613387 = fmfCwREIGlTqgmlNoGTqgzL37842038;     fmfCwREIGlTqgmlNoGTqgzL37842038 = fmfCwREIGlTqgmlNoGTqgzL17824841;     fmfCwREIGlTqgmlNoGTqgzL17824841 = fmfCwREIGlTqgmlNoGTqgzL50366462;     fmfCwREIGlTqgmlNoGTqgzL50366462 = fmfCwREIGlTqgmlNoGTqgzL24832080;     fmfCwREIGlTqgmlNoGTqgzL24832080 = fmfCwREIGlTqgmlNoGTqgzL9503335;     fmfCwREIGlTqgmlNoGTqgzL9503335 = fmfCwREIGlTqgmlNoGTqgzL33138221;     fmfCwREIGlTqgmlNoGTqgzL33138221 = fmfCwREIGlTqgmlNoGTqgzL96583718;     fmfCwREIGlTqgmlNoGTqgzL96583718 = fmfCwREIGlTqgmlNoGTqgzL72865701;     fmfCwREIGlTqgmlNoGTqgzL72865701 = fmfCwREIGlTqgmlNoGTqgzL20210666;     fmfCwREIGlTqgmlNoGTqgzL20210666 = fmfCwREIGlTqgmlNoGTqgzL22071213;     fmfCwREIGlTqgmlNoGTqgzL22071213 = fmfCwREIGlTqgmlNoGTqgzL78296062;     fmfCwREIGlTqgmlNoGTqgzL78296062 = fmfCwREIGlTqgmlNoGTqgzL65931965;     fmfCwREIGlTqgmlNoGTqgzL65931965 = fmfCwREIGlTqgmlNoGTqgzL21255126;     fmfCwREIGlTqgmlNoGTqgzL21255126 = fmfCwREIGlTqgmlNoGTqgzL15836719;     fmfCwREIGlTqgmlNoGTqgzL15836719 = fmfCwREIGlTqgmlNoGTqgzL97415831;     fmfCwREIGlTqgmlNoGTqgzL97415831 = fmfCwREIGlTqgmlNoGTqgzL36457607;     fmfCwREIGlTqgmlNoGTqgzL36457607 = fmfCwREIGlTqgmlNoGTqgzL67479025;     fmfCwREIGlTqgmlNoGTqgzL67479025 = fmfCwREIGlTqgmlNoGTqgzL88070244;     fmfCwREIGlTqgmlNoGTqgzL88070244 = fmfCwREIGlTqgmlNoGTqgzL76125249;     fmfCwREIGlTqgmlNoGTqgzL76125249 = fmfCwREIGlTqgmlNoGTqgzL40095425;     fmfCwREIGlTqgmlNoGTqgzL40095425 = fmfCwREIGlTqgmlNoGTqgzL39073148;     fmfCwREIGlTqgmlNoGTqgzL39073148 = fmfCwREIGlTqgmlNoGTqgzL19453749; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void oNoFRoIeVWieTqztYlHjhOVmoUvJZ58562771() { float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU40694895 = -799527271;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43083310 = 73714030;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU22180270 = -366238789;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU39558097 = -152692955;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU73594481 = -235182958;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU36261650 = -914051896;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU50341955 = -591051682;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU6276608 = -726894906;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45081851 = -650639205;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU97028839 = -235981878;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU51882985 = -547257393;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU2829584 = 37337649;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU72019613 = -859220067;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU30102833 = 60697866;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU6047875 = -166825719;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU20926578 = -342552462;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU24611154 = -867942031;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU9502411 = -234247214;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU18033791 = -256811278;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU21781052 = -467183775;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU74988811 = -807602501;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU53860729 = -526129749;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU67084597 = -507919626;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU13481466 = -724651092;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43243766 = -521840752;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU6025924 = -536501686;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU59129270 = 18232923;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU76061845 = -456047813;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU91686766 = -14737841;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU22882215 = -117036323;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU65730073 = -253797806;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU62227694 = -343405594;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU77697862 = -696347757;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU55429496 = -252274108;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45952640 = -101712960;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU69288782 = 36365819;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43744586 = -342752573;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU64179427 = -62283805;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU63810384 = -170081572;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU52798442 = -484940833;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU57895868 = -497478116;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU60871045 = -479294394;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU8184983 = -446782121;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU85389307 = -973607217;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU17580838 = -802439397;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43356514 = 88853113;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU14200483 = -755276334;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU41764848 = -214181800;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU13787006 = -774514184;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU46710814 = -825120860;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU68245190 = -212284631;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU82869146 = -156659551;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU90063084 = -797349913;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU95645255 = -937820277;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU83573959 = -885216484;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU86834165 = -173397522;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU75998713 = -418366345;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU8698804 = -641587698;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU96314330 = -630852204;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU67568557 = -698681272;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU77132380 = -832284819;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU74280110 = -35003869;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU14589842 = -612157065;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU22199636 = -433602883;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU31298766 = -982184073;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU89655291 = -103851800;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU25131721 = -266314595;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU16590117 = -506945960;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU84150192 = -837589175;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU36759092 = -103191538;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU77181992 = -999799889;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU60431726 = -705658226;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45692026 = 35834358;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU65235349 = -771870446;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU63885183 = -969705659;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU14117767 = -228308108;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45675747 = 20652371;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU81695290 = -534312410;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU95900628 = -922211695;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU99887251 = -510693865;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU91825440 = -781225353;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU17364422 = -767585278;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU62274840 = -681533630;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU44975952 = -189616982;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU54637025 = -904751692;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU82860927 = 2861745;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU72164609 = -546055681;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU82052607 = -758527480;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU71855537 = -367057625;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU59118474 = -928315438;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU93290069 = -545267836;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU35045782 = -701164875;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU67865097 = -431431602;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU96241827 = -471400301;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU75666062 = -652656014;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU83615758 = -362474247;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU46281203 = -867137329;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU85985346 = 86820761;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU54090541 = -991423144;    float BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU27925547 = -799527271;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU40694895 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43083310;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43083310 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU22180270;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU22180270 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU39558097;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU39558097 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU73594481;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU73594481 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU36261650;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU36261650 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU50341955;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU50341955 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU6276608;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU6276608 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45081851;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45081851 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU97028839;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU97028839 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU51882985;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU51882985 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU2829584;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU2829584 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU72019613;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU72019613 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU30102833;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU30102833 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU6047875;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU6047875 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU20926578;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU20926578 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU24611154;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU24611154 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU9502411;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU9502411 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU18033791;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU18033791 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU21781052;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU21781052 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU74988811;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU74988811 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU53860729;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU53860729 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU67084597;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU67084597 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU13481466;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU13481466 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43243766;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43243766 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU6025924;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU6025924 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU59129270;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU59129270 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU76061845;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU76061845 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU91686766;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU91686766 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU22882215;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU22882215 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU65730073;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU65730073 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU62227694;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU62227694 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU77697862;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU77697862 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU55429496;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU55429496 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45952640;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45952640 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU69288782;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU69288782 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43744586;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43744586 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU64179427;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU64179427 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU63810384;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU63810384 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU52798442;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU52798442 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU57895868;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU57895868 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU60871045;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU60871045 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU8184983;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU8184983 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU85389307;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU85389307 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU17580838;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU17580838 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43356514;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU43356514 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU14200483;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU14200483 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU41764848;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU41764848 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU13787006;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU13787006 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU46710814;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU46710814 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU68245190;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU68245190 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU82869146;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU82869146 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU90063084;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU90063084 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU95645255;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU95645255 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU83573959;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU83573959 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU86834165;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU86834165 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU75998713;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU75998713 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU8698804;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU8698804 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU96314330;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU96314330 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU67568557;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU67568557 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU77132380;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU77132380 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU74280110;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU74280110 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU14589842;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU14589842 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU22199636;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU22199636 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU31298766;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU31298766 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU89655291;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU89655291 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU25131721;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU25131721 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU16590117;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU16590117 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU84150192;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU84150192 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU36759092;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU36759092 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU77181992;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU77181992 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU60431726;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU60431726 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45692026;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45692026 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU65235349;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU65235349 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU63885183;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU63885183 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU14117767;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU14117767 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45675747;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU45675747 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU81695290;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU81695290 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU95900628;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU95900628 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU99887251;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU99887251 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU91825440;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU91825440 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU17364422;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU17364422 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU62274840;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU62274840 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU44975952;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU44975952 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU54637025;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU54637025 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU82860927;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU82860927 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU72164609;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU72164609 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU82052607;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU82052607 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU71855537;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU71855537 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU59118474;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU59118474 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU93290069;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU93290069 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU35045782;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU35045782 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU67865097;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU67865097 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU96241827;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU96241827 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU75666062;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU75666062 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU83615758;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU83615758 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU46281203;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU46281203 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU85985346;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU85985346 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU54090541;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU54090541 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU27925547;     BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU27925547 = BQoFpilrWmweVXYcIegIqqqiChasCQxtyFU40694895; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LuWrHzWdUTgdxeiqMYL43646785() { int CsARbiGaQoxATtGs3804272 = -635351217;    int CsARbiGaQoxATtGs88328940 = -158659235;    int CsARbiGaQoxATtGs81128408 = -640866775;    int CsARbiGaQoxATtGs13495468 = -839105985;    int CsARbiGaQoxATtGs98729710 = -850134878;    int CsARbiGaQoxATtGs27293072 = -677339386;    int CsARbiGaQoxATtGs37488671 = -463262393;    int CsARbiGaQoxATtGs79859124 = -689153227;    int CsARbiGaQoxATtGs40186010 = -657244455;    int CsARbiGaQoxATtGs95843770 = -350408154;    int CsARbiGaQoxATtGs58099088 = -71499135;    int CsARbiGaQoxATtGs45833801 = -36067450;    int CsARbiGaQoxATtGs44295614 = -720474159;    int CsARbiGaQoxATtGs49373917 = -154317981;    int CsARbiGaQoxATtGs79675981 = -299949774;    int CsARbiGaQoxATtGs70365952 = -206875303;    int CsARbiGaQoxATtGs45397766 = -621173433;    int CsARbiGaQoxATtGs20116035 = -576979578;    int CsARbiGaQoxATtGs87157252 = 10721515;    int CsARbiGaQoxATtGs648520 = -273222568;    int CsARbiGaQoxATtGs41588166 = -733205419;    int CsARbiGaQoxATtGs86794901 = -884431233;    int CsARbiGaQoxATtGs72639610 = -697027875;    int CsARbiGaQoxATtGs27583433 = -21853076;    int CsARbiGaQoxATtGs66529934 = -301281733;    int CsARbiGaQoxATtGs48232131 = -830770924;    int CsARbiGaQoxATtGs74241399 = 49074046;    int CsARbiGaQoxATtGs15714934 = -653161052;    int CsARbiGaQoxATtGs35564772 = -235575980;    int CsARbiGaQoxATtGs87268159 = -798619092;    int CsARbiGaQoxATtGs14571029 = -24126010;    int CsARbiGaQoxATtGs31951585 = -655443154;    int CsARbiGaQoxATtGs82539107 = -287735397;    int CsARbiGaQoxATtGs59405657 = -396857435;    int CsARbiGaQoxATtGs88730419 = -593709933;    int CsARbiGaQoxATtGs65922734 = -885917622;    int CsARbiGaQoxATtGs54263233 = -468689530;    int CsARbiGaQoxATtGs48496466 = -200209476;    int CsARbiGaQoxATtGs69541400 = 14352333;    int CsARbiGaQoxATtGs85357915 = -651285774;    int CsARbiGaQoxATtGs2057451 = -269514877;    int CsARbiGaQoxATtGs98583350 = -190018816;    int CsARbiGaQoxATtGs54194701 = -762082675;    int CsARbiGaQoxATtGs44580513 = 81225563;    int CsARbiGaQoxATtGs99449896 = -39569319;    int CsARbiGaQoxATtGs96444583 = -311365104;    int CsARbiGaQoxATtGs61375482 = -171625008;    int CsARbiGaQoxATtGs93291972 = -728841769;    int CsARbiGaQoxATtGs28379976 = -390568198;    int CsARbiGaQoxATtGs29726821 = -337756018;    int CsARbiGaQoxATtGs26234083 = -495705717;    int CsARbiGaQoxATtGs99368962 = -825841446;    int CsARbiGaQoxATtGs30058134 = -896676157;    int CsARbiGaQoxATtGs29750664 = 69608042;    int CsARbiGaQoxATtGs2058620 = -460468256;    int CsARbiGaQoxATtGs17009370 = -750919984;    int CsARbiGaQoxATtGs15689330 = -461631360;    int CsARbiGaQoxATtGs53544976 = -519013700;    int CsARbiGaQoxATtGs46965534 = -437824253;    int CsARbiGaQoxATtGs50497579 = 80636046;    int CsARbiGaQoxATtGs53051672 = -626413433;    int CsARbiGaQoxATtGs21773737 = -810101341;    int CsARbiGaQoxATtGs44294353 = -353577247;    int CsARbiGaQoxATtGs52917850 = -858625363;    int CsARbiGaQoxATtGs81272742 = -226282144;    int CsARbiGaQoxATtGs26147504 = -416055981;    int CsARbiGaQoxATtGs63294694 = -748332053;    int CsARbiGaQoxATtGs84889957 = -223616725;    int CsARbiGaQoxATtGs60643497 = -560608049;    int CsARbiGaQoxATtGs13753247 = -414032152;    int CsARbiGaQoxATtGs16102719 = -738185773;    int CsARbiGaQoxATtGs96901299 = -320963958;    int CsARbiGaQoxATtGs50574634 = -491331912;    int CsARbiGaQoxATtGs1799337 = -337992711;    int CsARbiGaQoxATtGs98591069 = 96292308;    int CsARbiGaQoxATtGs43004815 = -443186604;    int CsARbiGaQoxATtGs32600201 = -22348558;    int CsARbiGaQoxATtGs28059098 = -678253438;    int CsARbiGaQoxATtGs28133536 = -982283757;    int CsARbiGaQoxATtGs70085350 = -989916629;    int CsARbiGaQoxATtGs86856649 = -559145917;    int CsARbiGaQoxATtGs80949427 = -222084185;    int CsARbiGaQoxATtGs87334958 = -162592854;    int CsARbiGaQoxATtGs5837951 = -897819963;    int CsARbiGaQoxATtGs61034076 = -202913376;    int CsARbiGaQoxATtGs15202067 = -198284565;    int CsARbiGaQoxATtGs1893451 = -758766997;    int CsARbiGaQoxATtGs52788443 = -257343440;    int CsARbiGaQoxATtGs57347037 = -936389180;    int CsARbiGaQoxATtGs71721050 = -842789949;    int CsARbiGaQoxATtGs50233404 = -324286263;    int CsARbiGaQoxATtGs718258 = -949675831;    int CsARbiGaQoxATtGs1530932 = -762385224;    int CsARbiGaQoxATtGs19043821 = 33716286;    int CsARbiGaQoxATtGs32306244 = 75127658;    int CsARbiGaQoxATtGs80283713 = -459413536;    int CsARbiGaQoxATtGs54288998 = -836441569;    int CsARbiGaQoxATtGs1276851 = -903457313;    int CsARbiGaQoxATtGs63307770 = -692492294;    int CsARbiGaQoxATtGs73302393 = -635351217;     CsARbiGaQoxATtGs3804272 = CsARbiGaQoxATtGs88328940;     CsARbiGaQoxATtGs88328940 = CsARbiGaQoxATtGs81128408;     CsARbiGaQoxATtGs81128408 = CsARbiGaQoxATtGs13495468;     CsARbiGaQoxATtGs13495468 = CsARbiGaQoxATtGs98729710;     CsARbiGaQoxATtGs98729710 = CsARbiGaQoxATtGs27293072;     CsARbiGaQoxATtGs27293072 = CsARbiGaQoxATtGs37488671;     CsARbiGaQoxATtGs37488671 = CsARbiGaQoxATtGs79859124;     CsARbiGaQoxATtGs79859124 = CsARbiGaQoxATtGs40186010;     CsARbiGaQoxATtGs40186010 = CsARbiGaQoxATtGs95843770;     CsARbiGaQoxATtGs95843770 = CsARbiGaQoxATtGs58099088;     CsARbiGaQoxATtGs58099088 = CsARbiGaQoxATtGs45833801;     CsARbiGaQoxATtGs45833801 = CsARbiGaQoxATtGs44295614;     CsARbiGaQoxATtGs44295614 = CsARbiGaQoxATtGs49373917;     CsARbiGaQoxATtGs49373917 = CsARbiGaQoxATtGs79675981;     CsARbiGaQoxATtGs79675981 = CsARbiGaQoxATtGs70365952;     CsARbiGaQoxATtGs70365952 = CsARbiGaQoxATtGs45397766;     CsARbiGaQoxATtGs45397766 = CsARbiGaQoxATtGs20116035;     CsARbiGaQoxATtGs20116035 = CsARbiGaQoxATtGs87157252;     CsARbiGaQoxATtGs87157252 = CsARbiGaQoxATtGs648520;     CsARbiGaQoxATtGs648520 = CsARbiGaQoxATtGs41588166;     CsARbiGaQoxATtGs41588166 = CsARbiGaQoxATtGs86794901;     CsARbiGaQoxATtGs86794901 = CsARbiGaQoxATtGs72639610;     CsARbiGaQoxATtGs72639610 = CsARbiGaQoxATtGs27583433;     CsARbiGaQoxATtGs27583433 = CsARbiGaQoxATtGs66529934;     CsARbiGaQoxATtGs66529934 = CsARbiGaQoxATtGs48232131;     CsARbiGaQoxATtGs48232131 = CsARbiGaQoxATtGs74241399;     CsARbiGaQoxATtGs74241399 = CsARbiGaQoxATtGs15714934;     CsARbiGaQoxATtGs15714934 = CsARbiGaQoxATtGs35564772;     CsARbiGaQoxATtGs35564772 = CsARbiGaQoxATtGs87268159;     CsARbiGaQoxATtGs87268159 = CsARbiGaQoxATtGs14571029;     CsARbiGaQoxATtGs14571029 = CsARbiGaQoxATtGs31951585;     CsARbiGaQoxATtGs31951585 = CsARbiGaQoxATtGs82539107;     CsARbiGaQoxATtGs82539107 = CsARbiGaQoxATtGs59405657;     CsARbiGaQoxATtGs59405657 = CsARbiGaQoxATtGs88730419;     CsARbiGaQoxATtGs88730419 = CsARbiGaQoxATtGs65922734;     CsARbiGaQoxATtGs65922734 = CsARbiGaQoxATtGs54263233;     CsARbiGaQoxATtGs54263233 = CsARbiGaQoxATtGs48496466;     CsARbiGaQoxATtGs48496466 = CsARbiGaQoxATtGs69541400;     CsARbiGaQoxATtGs69541400 = CsARbiGaQoxATtGs85357915;     CsARbiGaQoxATtGs85357915 = CsARbiGaQoxATtGs2057451;     CsARbiGaQoxATtGs2057451 = CsARbiGaQoxATtGs98583350;     CsARbiGaQoxATtGs98583350 = CsARbiGaQoxATtGs54194701;     CsARbiGaQoxATtGs54194701 = CsARbiGaQoxATtGs44580513;     CsARbiGaQoxATtGs44580513 = CsARbiGaQoxATtGs99449896;     CsARbiGaQoxATtGs99449896 = CsARbiGaQoxATtGs96444583;     CsARbiGaQoxATtGs96444583 = CsARbiGaQoxATtGs61375482;     CsARbiGaQoxATtGs61375482 = CsARbiGaQoxATtGs93291972;     CsARbiGaQoxATtGs93291972 = CsARbiGaQoxATtGs28379976;     CsARbiGaQoxATtGs28379976 = CsARbiGaQoxATtGs29726821;     CsARbiGaQoxATtGs29726821 = CsARbiGaQoxATtGs26234083;     CsARbiGaQoxATtGs26234083 = CsARbiGaQoxATtGs99368962;     CsARbiGaQoxATtGs99368962 = CsARbiGaQoxATtGs30058134;     CsARbiGaQoxATtGs30058134 = CsARbiGaQoxATtGs29750664;     CsARbiGaQoxATtGs29750664 = CsARbiGaQoxATtGs2058620;     CsARbiGaQoxATtGs2058620 = CsARbiGaQoxATtGs17009370;     CsARbiGaQoxATtGs17009370 = CsARbiGaQoxATtGs15689330;     CsARbiGaQoxATtGs15689330 = CsARbiGaQoxATtGs53544976;     CsARbiGaQoxATtGs53544976 = CsARbiGaQoxATtGs46965534;     CsARbiGaQoxATtGs46965534 = CsARbiGaQoxATtGs50497579;     CsARbiGaQoxATtGs50497579 = CsARbiGaQoxATtGs53051672;     CsARbiGaQoxATtGs53051672 = CsARbiGaQoxATtGs21773737;     CsARbiGaQoxATtGs21773737 = CsARbiGaQoxATtGs44294353;     CsARbiGaQoxATtGs44294353 = CsARbiGaQoxATtGs52917850;     CsARbiGaQoxATtGs52917850 = CsARbiGaQoxATtGs81272742;     CsARbiGaQoxATtGs81272742 = CsARbiGaQoxATtGs26147504;     CsARbiGaQoxATtGs26147504 = CsARbiGaQoxATtGs63294694;     CsARbiGaQoxATtGs63294694 = CsARbiGaQoxATtGs84889957;     CsARbiGaQoxATtGs84889957 = CsARbiGaQoxATtGs60643497;     CsARbiGaQoxATtGs60643497 = CsARbiGaQoxATtGs13753247;     CsARbiGaQoxATtGs13753247 = CsARbiGaQoxATtGs16102719;     CsARbiGaQoxATtGs16102719 = CsARbiGaQoxATtGs96901299;     CsARbiGaQoxATtGs96901299 = CsARbiGaQoxATtGs50574634;     CsARbiGaQoxATtGs50574634 = CsARbiGaQoxATtGs1799337;     CsARbiGaQoxATtGs1799337 = CsARbiGaQoxATtGs98591069;     CsARbiGaQoxATtGs98591069 = CsARbiGaQoxATtGs43004815;     CsARbiGaQoxATtGs43004815 = CsARbiGaQoxATtGs32600201;     CsARbiGaQoxATtGs32600201 = CsARbiGaQoxATtGs28059098;     CsARbiGaQoxATtGs28059098 = CsARbiGaQoxATtGs28133536;     CsARbiGaQoxATtGs28133536 = CsARbiGaQoxATtGs70085350;     CsARbiGaQoxATtGs70085350 = CsARbiGaQoxATtGs86856649;     CsARbiGaQoxATtGs86856649 = CsARbiGaQoxATtGs80949427;     CsARbiGaQoxATtGs80949427 = CsARbiGaQoxATtGs87334958;     CsARbiGaQoxATtGs87334958 = CsARbiGaQoxATtGs5837951;     CsARbiGaQoxATtGs5837951 = CsARbiGaQoxATtGs61034076;     CsARbiGaQoxATtGs61034076 = CsARbiGaQoxATtGs15202067;     CsARbiGaQoxATtGs15202067 = CsARbiGaQoxATtGs1893451;     CsARbiGaQoxATtGs1893451 = CsARbiGaQoxATtGs52788443;     CsARbiGaQoxATtGs52788443 = CsARbiGaQoxATtGs57347037;     CsARbiGaQoxATtGs57347037 = CsARbiGaQoxATtGs71721050;     CsARbiGaQoxATtGs71721050 = CsARbiGaQoxATtGs50233404;     CsARbiGaQoxATtGs50233404 = CsARbiGaQoxATtGs718258;     CsARbiGaQoxATtGs718258 = CsARbiGaQoxATtGs1530932;     CsARbiGaQoxATtGs1530932 = CsARbiGaQoxATtGs19043821;     CsARbiGaQoxATtGs19043821 = CsARbiGaQoxATtGs32306244;     CsARbiGaQoxATtGs32306244 = CsARbiGaQoxATtGs80283713;     CsARbiGaQoxATtGs80283713 = CsARbiGaQoxATtGs54288998;     CsARbiGaQoxATtGs54288998 = CsARbiGaQoxATtGs1276851;     CsARbiGaQoxATtGs1276851 = CsARbiGaQoxATtGs63307770;     CsARbiGaQoxATtGs63307770 = CsARbiGaQoxATtGs73302393;     CsARbiGaQoxATtGs73302393 = CsARbiGaQoxATtGs3804272; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void rRpXiuqXjLYqLmbmnYazxkNTYmcF78453795() { long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54403458 = -291752928;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy96815847 = -46265711;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy65157963 = -10482149;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy6387646 = -872669246;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy8213900 = -698528694;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy5252256 = -837668985;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy53542541 = -473672091;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy90322193 = -692278506;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy72945268 = -364937993;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy319691 = -783815136;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy40461211 = -210960282;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy91039724 = -511399938;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy95290846 = -189945834;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy10689287 = -770887660;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy95908384 = -339118747;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy64087399 = -372752906;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy44472465 = 74027510;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy41897263 = -376584148;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy93870557 = 88758001;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy97638575 = 27263627;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79547298 = -7662749;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy68684141 = -699873826;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy23223684 = -624116691;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy85439460 = -566577566;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy43169322 = -152305718;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy25057341 = -851169240;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy35185983 = -252569488;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy8584435 = -697446285;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy5001996 = -676922795;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy6662909 = -747787917;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy98951436 = -292924902;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy52405442 = -718674059;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54646428 = -598582275;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy80013066 = -837521888;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy78503282 = -141957860;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy46218716 = -599893404;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy15137917 = -746035943;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy19871280 = -606819091;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy98774468 = -986364976;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy67283087 = -421983071;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy9834156 = -338540747;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy293972 = -231445293;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54695707 = -434861923;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy94049708 = -324012599;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79362246 = -768416107;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy779394 = -689707985;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy1980897 = -320440618;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy22142173 = -605929816;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy61265192 = -389084237;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy76355965 = -557862136;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy39358550 = -260390978;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy90970785 = -358704352;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54562151 = -16211172;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy45537330 = 92232377;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy98906496 = -987966002;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy85719317 = -591879102;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy73592163 = -422149021;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79718502 = -443904583;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy63218323 = -620363528;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy83156558 = -847359454;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy70066273 = -485099497;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy44958107 = -776225807;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy85320198 = 84644288;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy66282359 = -617150076;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy1368254 = -390890235;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy88055768 = -492286223;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy36393296 = -912817664;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy15277780 = -352423946;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy32186004 = -528929800;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy49689669 = -739225344;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy48949483 = -626716964;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy24601185 = -319153399;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy43122794 = -390219173;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy26587470 = -489258928;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy87804419 = -634195627;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79253327 = -776217457;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy13988434 = -165011904;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy29173976 = -200104093;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy6077215 = -798161460;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy42389929 = -462597734;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy23076444 = -430728622;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy13043810 = -646639672;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy47319242 = -208362048;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy28646030 = -19060659;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy67304359 = -387396940;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy7980652 = -934220550;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy97843291 = -602462887;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy9109099 = -590814653;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy81106569 = -849555886;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy92783964 = -550078758;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy72626552 = -77744383;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy35419414 = -202131360;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy56652957 = -986455563;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy15617911 = -39005522;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy97216814 = -936883574;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy64876048 = -562314940;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy14973773 = -216089581;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy88413348 = -817711848;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy92681455 = -933122364;    long McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy91306477 = -291752928;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54403458 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy96815847;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy96815847 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy65157963;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy65157963 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy6387646;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy6387646 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy8213900;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy8213900 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy5252256;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy5252256 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy53542541;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy53542541 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy90322193;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy90322193 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy72945268;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy72945268 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy319691;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy319691 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy40461211;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy40461211 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy91039724;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy91039724 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy95290846;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy95290846 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy10689287;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy10689287 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy95908384;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy95908384 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy64087399;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy64087399 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy44472465;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy44472465 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy41897263;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy41897263 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy93870557;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy93870557 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy97638575;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy97638575 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79547298;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79547298 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy68684141;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy68684141 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy23223684;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy23223684 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy85439460;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy85439460 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy43169322;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy43169322 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy25057341;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy25057341 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy35185983;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy35185983 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy8584435;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy8584435 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy5001996;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy5001996 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy6662909;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy6662909 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy98951436;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy98951436 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy52405442;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy52405442 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54646428;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54646428 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy80013066;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy80013066 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy78503282;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy78503282 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy46218716;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy46218716 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy15137917;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy15137917 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy19871280;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy19871280 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy98774468;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy98774468 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy67283087;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy67283087 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy9834156;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy9834156 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy293972;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy293972 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54695707;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54695707 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy94049708;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy94049708 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79362246;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79362246 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy779394;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy779394 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy1980897;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy1980897 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy22142173;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy22142173 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy61265192;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy61265192 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy76355965;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy76355965 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy39358550;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy39358550 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy90970785;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy90970785 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54562151;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54562151 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy45537330;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy45537330 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy98906496;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy98906496 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy85719317;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy85719317 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy73592163;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy73592163 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79718502;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79718502 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy63218323;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy63218323 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy83156558;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy83156558 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy70066273;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy70066273 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy44958107;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy44958107 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy85320198;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy85320198 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy66282359;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy66282359 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy1368254;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy1368254 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy88055768;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy88055768 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy36393296;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy36393296 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy15277780;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy15277780 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy32186004;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy32186004 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy49689669;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy49689669 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy48949483;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy48949483 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy24601185;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy24601185 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy43122794;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy43122794 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy26587470;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy26587470 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy87804419;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy87804419 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79253327;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy79253327 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy13988434;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy13988434 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy29173976;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy29173976 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy6077215;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy6077215 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy42389929;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy42389929 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy23076444;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy23076444 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy13043810;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy13043810 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy47319242;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy47319242 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy28646030;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy28646030 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy67304359;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy67304359 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy7980652;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy7980652 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy97843291;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy97843291 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy9109099;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy9109099 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy81106569;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy81106569 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy92783964;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy92783964 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy72626552;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy72626552 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy35419414;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy35419414 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy56652957;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy56652957 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy15617911;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy15617911 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy97216814;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy97216814 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy64876048;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy64876048 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy14973773;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy14973773 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy88413348;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy88413348 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy92681455;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy92681455 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy91306477;     McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy91306477 = McPnSRGgPjSgAMcmlRWPjBfmEsHHDVy54403458; }
// Junk Finished
