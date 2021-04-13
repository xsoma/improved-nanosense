#include "AngryPew.hpp"
#include "..//SanityChech.hpp"
#include "../Options.hpp"
#include "../helpers/Math.hpp"
#include "../helpers/Utils.hpp"
#include "LagCompensation.hpp"

#define NETVAROFFS(type, name, table, netvar)                           \
    int name##() const {                                          \
        static int _##name = NetMngr::Get().getOffs(table, netvar);     \
        return _##name;                 \
	}
float override_hitchance;
float override_mindmg;
void AngryPew::Work(CUserCmd* usercmd)
{
	if (!g_LocalPlayer->IsAlive())
		return;

	if (!XSystemCFG.angrpwenabled || XSystemCFG.angrpwaimtype < 0)
		return;

	this->local_weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	this->usercmd = usercmd;
	this->cur_time = this->GetTickbase() * g_GlobalVars->interval_per_tick;

	Global::bAimbotting = false;
	Global::bVisualAimbotting = false;

	if (XSystemCFG.angrpwautocockrevolver)
	{
		if (!this->CockRevolver())
			return;
	}

	if (!local_weapon)
		return;

	if (g_LocalPlayer->m_flNextAttack() > this->cur_time)
		return;

	// Also add checks for grenade throw time if we dont have that yet.
	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsWeaponNonAim() || g_LocalPlayer->m_hActiveWeapon().Get()->m_iClip1() < 1)
		return;

	TargetEntities();

	if (g_LocalPlayer->m_hActiveWeapon())
	{
		override_hitchance = XSystemCFG.angrpwhitchance_amount;
		override_mindmg = XSystemCFG.angrpwmindmg_amount;
	}
}

bool AngryPew::TargetSpecificEnt(C_BasePlayer* pEnt)
{
	int i = pEnt->EntIndex();
	auto firedShots = g_LocalPlayer->m_iShotsFired();

	Vector vecTarget;

	// Disgusting ass codes, can't think of a cleaner way now though. FIX ME.
	bool LagComp_Hitchanced = false;
	if (XSystemCFG.angrpwaccboost > 0)
	{
		if (pEnt->m_vecVelocity().Length2D() > 0.1)
		{
			CMBacktracking::Get().hnlyBacktrack(pEnt, usercmd, vecTarget, LagComp_Hitchanced);
		}
		else
		{
			matrix3x4_t matrix[128];
			if (!pEnt->SetupBones2(matrix, 128, 256, pEnt->m_flSimulationTime()))
				return false;

			if (XSystemCFG.angrpwautobaim && firedShots > XSystemCFG.angrpwbaim_after_x_shots)
				vecTarget = CalculateBestPoint(pEnt, HITBOX_PELVIS, override_mindmg, true, matrix);
			else
			{
				if (g_LocalPlayer->m_hActiveWeapon())
					vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[XSystemCFG.iangrpwhitbox], override_mindmg, XSystemCFG.bangrpwprioritize, matrix);
			}
		}
	}
	else
	{
		matrix3x4_t matrix[128];
		if (!pEnt->SetupBones2(matrix, 128, 256, pEnt->m_flSimulationTime()))
			return false;

		if (XSystemCFG.angrpwautobaim && firedShots > XSystemCFG.angrpwbaim_after_x_shots)
			vecTarget = CalculateBestPoint(pEnt, HITBOX_PELVIS, override_mindmg, true, matrix);
		else
		{
			if (g_LocalPlayer->m_hActiveWeapon())
				vecTarget = CalculateBestPoint(pEnt, realHitboxSpot[XSystemCFG.iangrpwhitbox], override_mindmg, XSystemCFG.bangrpwprioritize, matrix);

		}
	}

	// Invalid target/no hitable points at all.
	if (!vecTarget.IsValid())
		return false;

	AutoStop();
	AutoCrouch();

	QAngle new_aim_angles = Math::CalcAngle(g_LocalPlayer->GetEyePos(), vecTarget) - (XSystemCFG.angrpwnorecoil ? g_LocalPlayer->m_aimPunchAngle() * g_CVar->FindVar("weapon_recoil_scale")->GetFloat() : QAngle(0, 0, 0));
	this->usercmd->viewangles = Global::vecVisualAimbotAngs = new_aim_angles;
	Global::vecVisualAimbotAngs += (XSystemCFG.removals_novisualrecoil ? g_LocalPlayer->m_aimPunchAngle() * 2.f : QAngle(0, 0, 0));
	Global::bVisualAimbotting = true;

	if (this->can_fire_weapon)
	{
		// Save more fps by remembering to try the same entity again next time.
		prev_aimtarget = pEnt->EntIndex();

		if (XSystemCFG.angrpwautoscope && g_LocalPlayer->m_hActiveWeapon().Get()->IsAutoScopeable() && g_LocalPlayer->m_hActiveWeapon().Get()->m_zoomLevel() == 0)
		{
			usercmd->buttons |= IN_ATTACK2;
		}
		else if ((XSystemCFG.angrpwaccboost > 0 && LagComp_Hitchanced) || (!LagComp_Hitchanced && HitChance(usercmd->viewangles, pEnt, override_hitchance)))
		{
			Global::bAimbotting = true;

			if (XSystemCFG.angrpwautoshoot)
			{
				usercmd->buttons |= IN_ATTACK;
			}
		}
	}

	return true;
}

void AngryPew::TargetEntities()
{
	static C_BaseCombatWeapon* oldWeapon; // what is this for?
	if (local_weapon != oldWeapon)
	{
		oldWeapon = local_weapon;
		usercmd->buttons &= ~IN_ATTACK;
		return;
	}

	if (local_weapon->IsPistolNotDeagR8() && usercmd->tick_count % 2)
	{
		static int lastshot;
		if (usercmd->buttons & IN_ATTACK)
			lastshot++;

		if (!usercmd->buttons & IN_ATTACK || lastshot > 1)
		{
			usercmd->buttons &= ~IN_ATTACK;
			lastshot = 0;
		}
		return;
	}

	this->can_fire_weapon = local_weapon->CanFire();

	if (prev_aimtarget && CheckTarget(prev_aimtarget))
	{
		if (TargetSpecificEnt(C_BasePlayer::GetPlayerByIndex(prev_aimtarget)))
			return;
	}

	float bestFOV = XSystemCFG.angrpwfov;
	C_BasePlayer* bestTarget = nullptr;

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		// Failed to shoot at him again, reset him and try others.
		if (prev_aimtarget && prev_aimtarget == i)
		{
			prev_aimtarget = NULL;
			continue;
		}

		if (!CheckTarget(i))
			continue;

		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

		float fov = Math::GetFov(usercmd->viewangles, Math::CalcAngle(g_LocalPlayer->GetEyePos(), player->m_vecOrigin()));

		if (fov < bestFOV) {
			bestFOV = fov;
			bestTarget = player;
		}
	}
	if (bestTarget)
	{
	if (TargetSpecificEnt(bestTarget))
		return;
	}
}

float AngryPew::BestHitPoint(C_BasePlayer* player, int prioritized, float minDmg, mstudiohitboxset_t* hitset, matrix3x4_t matrix[], Vector& vecOut)
{
	mstudiobbox_t* hitbox = hitset->pHitbox(prioritized);
	if (!hitbox)
		return 0.f;

	std::vector<Vector> vecArray;
	float flHigherDamage = 0.f;

	float mod = hitbox->m_flRadius != -1.f ? hitbox->m_flRadius : 0.f;

	Vector max;
	Vector min;

	Math::VectorTransform(hitbox->bbmax + mod, matrix[hitbox->bone], max);
	Math::VectorTransform(hitbox->bbmin - mod, matrix[hitbox->bone], min);

	auto center = (min + max) * 0.5f;

	QAngle curAngles = Math::CalcAngle(center, g_LocalPlayer->GetEyePos());

	Vector forward;
	Math::AngleVectors(curAngles, forward);

	Vector right = forward.Cross(Vector(0, 0, 1));
	Vector left = Vector(-right.x, -right.y, right.z);

	Vector top = Vector(0, 0, 1);
	Vector bot = Vector(0, 0, -1);

	float POINT_SCALE;
	if (g_LocalPlayer->m_hActiveWeapon())
		POINT_SCALE = XSystemCFG.angrpwpointscale_amount;

	if (g_LocalPlayer->m_hActiveWeapon() && XSystemCFG.bangrpwmultipoint)
	{
		switch (prioritized)
		{
		case HITBOX_HEAD:
			for (auto i = 0; i < 4; ++i)
			{
				vecArray.emplace_back(center);
			}
			vecArray[1] += top * (hitbox->m_flRadius * POINT_SCALE);
			vecArray[2] += right * (hitbox->m_flRadius * POINT_SCALE);
			vecArray[3] += left * (hitbox->m_flRadius * POINT_SCALE);
			break;

		default:

			for (auto i = 0; i < 2; ++i)
			{
				vecArray.emplace_back(center);
			}
			vecArray[0] += right * (hitbox->m_flRadius * POINT_SCALE);
			vecArray[1] += left * (hitbox->m_flRadius * POINT_SCALE);
			break;
		}
	}
	else
		vecArray.emplace_back(center);

	for (Vector cur : vecArray)
	{
		float flCurDamage = GetDamageVec(cur, player, prioritized);

		if (!flCurDamage)
			continue;

		if ((flCurDamage > flHigherDamage) && (flCurDamage > minDmg))
		{
			flHigherDamage = flCurDamage;
			vecOut = cur;
		}
	}
	return flHigherDamage;
}

Vector AngryPew::CalculateBestPoint(C_BasePlayer* player, int prioritized, float minDmg, bool onlyPrioritized, matrix3x4_t matrix[])
{
	studiohdr_t* studioHdr = g_MdlInfo->GetStudiomodel(player->GetModel());
	mstudiohitboxset_t* set = studioHdr->pHitboxSet(player->m_nHitboxSet());
	Vector vecOutput;

	if (BestHitPoint(player, prioritized, minDmg, set, matrix, vecOutput) > minDmg && onlyPrioritized)
	{
		return vecOutput;
	}
	else
	{
		float flHigherDamage = 0.f;

		Vector vecCurVec;

		// why not use all the hitboxes then
		static int hitboxesLoop[] =
		{
			HITBOX_HEAD,
			HITBOX_PELVIS,
			HITBOX_UPPER_CHEST,
			HITBOX_CHEST,
			HITBOX_NECK,
			HITBOX_LEFT_FOREARM,
			HITBOX_RIGHT_FOREARM,
			HITBOX_RIGHT_HAND,
			HITBOX_LEFT_THIGH,
			HITBOX_RIGHT_THIGH,
			HITBOX_LEFT_CALF,
			HITBOX_RIGHT_CALF,
			HITBOX_LEFT_FOOT,
			HITBOX_RIGHT_FOOT
		};

		int loopSize = ARRAYSIZE(hitboxesLoop);
		for (int i = 0; i < loopSize; ++i)
		{
			if (g_LocalPlayer->m_hActiveWeapon()->IsPistolNotDeagR8())
				if (!XSystemCFG.angrpwmultiHitboxesAll[i])
					continue;

			float flCurDamage = BestHitPoint(player, hitboxesLoop[i], minDmg, set, matrix, vecCurVec);

			if (!flCurDamage)
				continue;

			if (flCurDamage > flHigherDamage)
			{
				flHigherDamage = flCurDamage;
				vecOutput = vecCurVec;
				if (static_cast<int32_t>(flHigherDamage) >= player->m_iHealth())
					break;
			}
		}
		return vecOutput;
	}
}

bool AngryPew::CheckTarget(int i)
{
	C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

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

	return true;
}

bool AngryPew::HitChance(QAngle angles, C_BasePlayer* ent, float chance)
{
	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return false;

	Vector forward, right, up;
	Vector src = g_LocalPlayer->GetEyePos();
	Math::AngleVectors(angles, forward, right, up);

	int cHits = 0;
	int cNeededHits = static_cast<int>(150.f * (chance / 100.f));

	weapon->UpdateAccuracyPenalty();
	float weap_spread = weapon->GetSpread();
	float weap_inaccuracy = weapon->GetInaccuracy();

	for (int i = 0; i < 150; i++)
	{
		float a = Utils::RandomFloat(0.f, 1.f);
		float b = Utils::RandomFloat(0.f, 2.f * PI_F);
		float c = Utils::RandomFloat(0.f, 1.f);
		float d = Utils::RandomFloat(0.f, 2.f * PI_F);

		float inaccuracy = a * weap_inaccuracy;
		float spread = c * weap_spread;

		if (weapon->m_iItemDefinitionIndex() == 64)
		{
			a = 1.f - a * a;
			a = 1.f - c * c;
		}

		Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

		direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
		direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
		direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
		direction.Normalized();

		QAngle viewAnglesSpread;
		Math::VectorAngles(direction, up, viewAnglesSpread);
		Math::NormalizeAngles(viewAnglesSpread);

		Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, viewForward);
		viewForward.NormalizeInPlace();

		viewForward = src + (viewForward * weapon->GetWeapInfo()->m_fRange());

		trace_t tr;
		Ray_t ray;

		ray.Init(src, viewForward);
		g_EngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, ent, &tr);

		if (tr.hit_entity == ent)
			++cHits;

		if (static_cast<int>((static_cast<float>(cHits) / 150.f) * 100.f) >= chance)
			return true;

		if ((150 - i + cHits) < cNeededHits)
			return false;
	}
	return false;
}

void AngryPew::AutoStop()
{
	if (!XSystemCFG.angrpwqstypes > 0)
		return;

	if (g_InputSystem->IsButtonDown(XSystemCFG.misc_fakewalk_bind))
		return;

	usercmd->forwardmove = 0;
	usercmd->sidemove = 0;
}

bool AngryPew::CockRevolver()
{
	// 0.234375f to cock and shoot, 15 ticks in 64 servers, 30(31?) in 128

	// THIS DOESNT WORK, WILL WORK ON LATER AGAIN WHEN I FEEL LIKE KILLING MYSELF

	// DONT USE TIME_TO_TICKS as these values aren't good for it. it's supposed to be 0.2f but that's also wrong
	constexpr float REVOLVER_COCK_TIME = 0.234375f;// 0.2421875f;
	const int count_needed = floor(REVOLVER_COCK_TIME / g_GlobalVars->interval_per_tick);
	static int cocks_done = 0;

	if (!local_weapon ||
		local_weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER ||
		g_LocalPlayer->m_flNextAttack() > g_GlobalVars->curtime ||
		local_weapon->IsReloading())
	{
		if (local_weapon && local_weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
			usercmd->buttons &= ~IN_ATTACK;
		cocks_done = 0;
		return true;
	}

	if (cocks_done < count_needed)
	{
		usercmd->buttons |= IN_ATTACK;
		++cocks_done;
		return false;
	}
	else
	{
		usercmd->buttons &= ~IN_ATTACK;
		cocks_done = 0;
		return true;
	}

	// 0.0078125 - 128ticks - 31 - 0.2421875
	// 0.015625  - 64 ticks - 16 - 0.234375f

	usercmd->buttons |= IN_ATTACK;

	/*
		3 steps:

		1. Come, not time for update, cock and return false;

		2. Come, completely outdated, cock and set time, return false;

		3. Come, time is up, cock and return true;

		Notes:
			Will I not have to account for high ping when I shouldn't send another update?
			Lower framerate than ticks = riperino? gotta check if lower then account by sending earlier | frametime memes
	*/

	float curtime = TICKS_TO_TIME(g_LocalPlayer->m_nTickBase());
	static float next_shoot_time = 0.f;

	bool ret = false;

	if (next_shoot_time - curtime < -0.5)
		next_shoot_time = curtime + 0.2f - g_GlobalVars->interval_per_tick; // -1 because we already cocked THIS tick ???

	if (next_shoot_time - curtime - g_GlobalVars->interval_per_tick <= 0.f) {
		next_shoot_time = curtime + 0.2f;
		ret = true;

		// should still go for one more tick but if we do, we're gonna shoot sooo idk how2do rn, its late
		// the aimbot should decide whether to shoot or not yeh
	}

	return ret;
}

void AngryPew::AutoCrouch()
{
	if (!XSystemCFG.angrpwautocrouch)
		return;

	usercmd->buttons |= IN_DUCK;
}

float AngryPew::GetDamageVec(const Vector& vecPoint, C_BasePlayer* player, int hitbox)
{
	float damage = 0.f;

	Vector rem = vecPoint;

	FireBulletData data;

	data.src = g_LocalPlayer->GetEyePos();
	data.filter.pSkip = g_LocalPlayer;

	QAngle angle = Math::CalcAngle(data.src, rem);
	Math::AngleVectors(angle, data.direction);

	data.direction.Normalized();

	auto weap = g_LocalPlayer->m_hActiveWeapon().Get();
	if (SimulateFireBullet(weap, data, player, hitbox))
		damage = data.current_damage;

	return damage;
}

bool AngryPew::SimulateFireBullet(C_BaseCombatWeapon* weap, FireBulletData& data, C_BasePlayer* player, int hitbox)
{
	if (!weap)
		return false;

	auto GetHitgroup = [](int hitbox) -> int
	{
		switch (hitbox)
		{
		case HITBOX_HEAD:
		case HITBOX_NECK:
			return HITGROUP_HEAD;
		case HITBOX_LOWER_CHEST:
		case HITBOX_CHEST:
		case HITBOX_UPPER_CHEST:
			return HITGROUP_CHEST;
		case HITBOX_STOMACH:
		case HITBOX_PELVIS:
			return HITGROUP_STOMACH;
		case HITBOX_LEFT_HAND:
		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_LEFT_FOREARM:
			return HITGROUP_LEFTARM;
		case HITBOX_RIGHT_HAND:
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_RIGHT_FOREARM:
			return HITGROUP_RIGHTARM;
		case HITBOX_LEFT_THIGH:
		case HITBOX_LEFT_CALF:
		case HITBOX_LEFT_FOOT:
			return HITGROUP_LEFTLEG;
		case HITBOX_RIGHT_THIGH:
		case HITBOX_RIGHT_CALF:
		case HITBOX_RIGHT_FOOT:
			return HITGROUP_RIGHTLEG;
		default:
			return -1;
		}
	};

	data.penetrate_count = 4;
	data.trace_length = 0.0f;
	WeapInfo_t* weaponData = g_LocalPlayer->m_hActiveWeapon().Get()->GetWeapInfo();

	if (weaponData == NULL)
		return false;

	data.current_damage = (float)weaponData->m_iDamage();

	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = weaponData->m_fRange() - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		traceIt(data.src, end, MASK_SHOT | CONTENTS_GRATE, g_LocalPlayer, &data.enter_trace);
		ClipTraceToPlayers(data.src, end + data.direction * 40.f, MASK_SHOT | CONTENTS_GRATE, &data.filter, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
		{
			if (player && !(player->m_fFlags() & FL_ONGROUND))
			{
				data.enter_trace.hitgroup = GetHitgroup(hitbox);
				data.enter_trace.hit_entity = player;
			}
			else
				break;
		}

		surfacedata_t* enter_surface_data = g_PhysSurface->GetSurfaceData(data.enter_trace.surface.surfaceProps);
		unsigned short enter_material = enter_surface_data->game.material;
		float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;

		data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
		data.current_damage *= pow(weaponData->m_fRangeModifier(), data.trace_length * 0.002);

		if (data.trace_length > 3000.f && weaponData->m_fPenetration() > 0.f || enter_surf_penetration_mod < 0.1f)
			break;

		if ((data.enter_trace.hitgroup <= 7) && (data.enter_trace.hitgroup > 0))
		{
			C_BasePlayer* pPlayer = reinterpret_cast<C_BasePlayer*>(data.enter_trace.hit_entity);
			if (pPlayer->IsPlayer() && pPlayer->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
				return false;

			ScaleDamage(data.enter_trace.hitgroup, pPlayer, weaponData->m_fArmorRatio(), data.current_damage);

			return true;
		}

		if (!HandleBulletPenetration(weaponData, data))
			break;
	}

	return false;
}

bool AngryPew::HandleBulletPenetration(WeapInfo_t* wpn_data, FireBulletData& data)
{
	bool bSolidSurf = ((data.enter_trace.contents >> 3) & CONTENTS_SOLID);
	bool bLightSurf = (data.enter_trace.surface.flags >> 7) & SURF_LIGHT;

	surfacedata_t* enter_surface_data = g_PhysSurface->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	unsigned short enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;

	if (!data.penetrate_count && !bLightSurf && !bSolidSurf && enter_material != 89)
	{
		if (enter_material != 71)
			return false;
	}

	if (data.penetrate_count <= 0 || wpn_data->m_fPenetration() <= 0.f)
		return false;

	Vector dummy;
	trace_t trace_exit;

	if (!TraceToExit(dummy, &data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
	{
		if (!(g_EngineTrace->GetPointContents(dummy, MASK_SHOT_HULL) & MASK_SHOT_HULL))
			return false;
	}

	surfacedata_t* exit_surface_data = g_PhysSurface->GetSurfaceData(trace_exit.surface.surfaceProps);
	unsigned short exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float exit_surf_damage_mod = exit_surface_data->game.flDamageModifier;

	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (enter_material == 89 || enter_material == 71)
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else if (bSolidSurf || bLightSurf)
	{
		combined_penetration_modifier = 1.0f;
		final_damage_modifier = 0.16f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float modifier = fmaxf(0.0f, 1.0f / combined_penetration_modifier);
	float thickness = (trace_exit.endpos - data.enter_trace.endpos).LengthSqr();
	float taken_damage = ((modifier * 3.0f) * fmaxf(0.0f, (3.0f / wpn_data->m_fPenetration()) * 1.25f) + (data.current_damage * final_damage_modifier)) + ((thickness * modifier) / 24.0f);

	float lost_damage = fmaxf(0.0f, taken_damage);

	if (lost_damage > data.current_damage)
		return false;

	if (lost_damage > 0.0f)
		data.current_damage -= lost_damage;

	if (data.current_damage < 1.0f)
		return false;

	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool AngryPew::TraceToExit(Vector& end, CGameTrace* enter_trace, Vector start, Vector dir, CGameTrace* exit_trace)
{
	auto distance = 0.0f;
	int first_contents = 0;

	while (distance < 90.0f)
	{
		distance += 4.0f;
		end = start + (dir * distance);

		if (!first_contents)
			first_contents = g_EngineTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX);

		auto point_contents = g_EngineTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX);

		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX) || first_contents == point_contents))
			continue;

		auto new_end = end - (dir * 4.0f);

		traceIt(end, new_end, MASK_SHOT | CONTENTS_GRATE, nullptr, exit_trace);

		if (exit_trace->startsolid && (exit_trace->surface.flags & SURF_HITBOX) < 0)
		{
			traceIt(end, start, MASK_SHOT_HULL, reinterpret_cast<C_BasePlayer*>(exit_trace->hit_entity), exit_trace);

			if (exit_trace->DidHit() && !exit_trace->startsolid)
			{
				end = exit_trace->endpos;
				return true;
			}
			continue;
		}

		if (!exit_trace->DidHit() || exit_trace->startsolid)
		{
			if (enter_trace->hit_entity)
			{
				if (enter_trace->DidHitNonWorldEntity() && IsBreakableEntity(reinterpret_cast<C_BasePlayer*>(enter_trace->hit_entity)))
				{
					*exit_trace = *enter_trace;
					exit_trace->endpos = start + dir;
					return true;
				}
			}
			continue;
		}

		if ((exit_trace->surface.flags >> 7) & SURF_LIGHT)
		{
			if (IsBreakableEntity(reinterpret_cast<C_BasePlayer*>(exit_trace->hit_entity)) && IsBreakableEntity(reinterpret_cast<C_BasePlayer*>(enter_trace->hit_entity)))
			{
				end = exit_trace->endpos;
				return true;
			}

			if (!((enter_trace->surface.flags >> 7) & SURF_LIGHT))
				continue;
		}

		if (exit_trace->plane.normal.Dot(dir) <= 1.0f)
		{
			float fraction = exit_trace->fraction * 4.0f;
			end = end - (dir * fraction);

			return true;
		}
	}
	return false;
}

bool AngryPew::IsBreakableEntity(C_BasePlayer* ent)
{
	typedef bool(__thiscall * isBreakbaleEntityFn)(C_BasePlayer*);
	static isBreakbaleEntityFn IsBreakableEntityFn = (isBreakbaleEntityFn)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 51 56 8B F1 85 F6 74 68");

	if (IsBreakableEntityFn)
	{
		// 0x280 = m_takedamage

		auto backupval = *reinterpret_cast<int*>((uint32_t)ent + 0x280);
		auto className = ent->GetClientClass()->m_pNetworkName;

		if (ent != g_EntityList->GetClientEntity(0))
		{

			if ((className[1] == 'B' && className[9] == 'e' && className[10] == 'S' && className[16] == 'e') // CBreakableSurface
				|| (className[1] != 'B' || className[5] != 'D')) // CBaseDoor because fuck doors
			{
				*reinterpret_cast<int*>((uint32_t)ent + 0x280) = 2;
			}
		}

		bool retn = IsBreakableEntityFn(ent);

		*reinterpret_cast<int*>((uint32_t)ent + 0x280) = backupval;

		return retn;
	}
	else
		return false;
}

void AngryPew::ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr)
{
	trace_t playerTrace;
	Ray_t ray;
	float smallestFraction = tr->fraction;
	const float maxRange = 60.0f;

	ray.Init(vecAbsStart, vecAbsEnd);

	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player || !player->IsAlive() || player->IsDormant())
			continue;

		if (filter && filter->ShouldHitEntity(player, mask) == false)
			continue;

		float range = Math::DistanceToRay(player->WorldSpaceCenter(), vecAbsStart, vecAbsEnd);
		if (range < 0.0f || range > maxRange)
			continue;

		g_EngineTrace->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, player, &playerTrace);
		if (playerTrace.fraction < smallestFraction)
		{
			*tr = playerTrace;
			smallestFraction = playerTrace.fraction;
		}
	}
}

void AngryPew::ScaleDamage(int hitgroup, C_BasePlayer* player, float weapon_armor_ratio, float& current_damage)
{
	bool heavArmor = player->m_bHasHeavyArmor();
	int armor = player->m_ArmorValue();

	switch (hitgroup)
	{
	case HITGROUP_HEAD:

		if (heavArmor)
			current_damage *= (current_damage * 4.f) * 0.5f;
		else
			current_damage *= 4.f;

		break;

	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:

		current_damage *= 1.f;
		break;

	case HITGROUP_STOMACH:

		current_damage *= 1.25f;
		break;

	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:

		current_damage *= 0.75f;
		break;
	}

	if (IsArmored(player, armor, hitgroup))
	{
		float v47 = 1.f, armor_bonus_ratio = 0.5f, armor_ratio = weapon_armor_ratio * 0.5f;

		if (heavArmor)
		{
			armor_bonus_ratio = 0.33f;
			armor_ratio = (weapon_armor_ratio * 0.5f) * 0.5f;
			v47 = 0.33f;
		}

		float new_damage = current_damage * armor_ratio;

		if (heavArmor)
			new_damage *= 0.85f;

		if (((current_damage - (current_damage * armor_ratio)) * (v47 * armor_bonus_ratio)) > armor)
			new_damage = current_damage - (armor / armor_bonus_ratio);

		current_damage = new_damage;
	}
}

bool AngryPew::IsArmored(C_BasePlayer* player, int armorVal, int hitgroup)
{
	bool res = false;

	if (armorVal > 0)
	{
		switch (hitgroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:

			res = true;
			break;

		case HITGROUP_HEAD:

			res = player->m_bHasHelmet();
			break;

		}
	}

	return res;
}

void AngryPew::traceIt(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, C_BasePlayer* ign, CGameTrace* tr)
{
	Ray_t ray;

	CTraceFilter filter;
	filter.pSkip = ign;

	ray.Init(vecAbsStart, vecAbsEnd);

	g_EngineTrace->TraceRay(ray, mask, &filter, tr);
}

int AngryPew::GetTickbase(CUserCmd* ucmd) {

	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;

	if (!ucmd)
		return g_tick;

	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = g_LocalPlayer->m_nTickBase();
	}
	else {
		// Required because prediction only runs on frames, not ticks
		// So if your framerate goes below tickrate, m_nTickBase won't update every tick
		++g_tick;
	}

	g_pLastCmd = ucmd;
	return g_tick;
}
Autowall_Return_Info AngryPew::GetDamageVec2(Vector start, Vector end, C_BasePlayer* from_entity, C_BasePlayer* to_entity, int specific_hitgroup)
{
	// default values for return info, in case we need to return abruptly
	Autowall_Return_Info return_info;
	return_info.damage = -1;
	return_info.hitgroup = -1;
	return_info.hit_entity = nullptr;
	return_info.penetration_count = 4;
	return_info.thickness = 0.f;
	return_info.did_penetrate_wall = false;

	Autowall_Info autowall_info;
	autowall_info.penetration_count = 4;
	autowall_info.start = start;
	autowall_info.end = end;
	autowall_info.current_position = start;
	autowall_info.thickness = 0.f;

	// direction 
	Math::AngleVectors(Math::CalcAngle(start, end), autowall_info.direction);

	// attacking entity
	if (!from_entity)
		from_entity = g_LocalPlayer;
	if (checks::is_bad_ptr(from_entity))
		return return_info;

	auto filter_player = CTraceFilterOneEntity();
	filter_player.pEntity = to_entity;
	auto filter_local = CTraceFilter();
	filter_local.pSkip = from_entity;

	// setup filters
	if (to_entity)
		autowall_info.filter = &filter_player;
	else
		autowall_info.filter = &filter_player;

	// weapon
	auto weapon = (C_BaseCombatWeapon*)(from_entity->m_hActiveWeapon());
	if (!weapon)
		return return_info;

	// weapon data
	auto weapon_info = weapon->GetWeapInfo();
	if (!weapon_info)
		return return_info;

	// client class
	auto weapon_client_class = reinterpret_cast<C_BaseEntity*>(weapon)->GetClientClass();
	if (!weapon_client_class)
		return return_info;

	// weapon range
	float range = min(weapon_info->m_fRange(), (start - end).Length());
	end = start + (autowall_info.direction * range);
	autowall_info.current_damage = weapon_info->m_iDamage();

	while (autowall_info.current_damage > 0 && autowall_info.penetration_count > 0)
	{
		return_info.penetration_count = autowall_info.penetration_count;

		traceIt(autowall_info.current_position, end, MASK_SHOT | CONTENTS_GRATE, g_LocalPlayer, &autowall_info.enter_trace);
		ClipTraceToPlayers(autowall_info.current_position, autowall_info.current_position + autowall_info.direction * 40.f, MASK_SHOT | CONTENTS_GRATE, autowall_info.filter, &autowall_info.enter_trace);

		const float distance_traced = (autowall_info.enter_trace.endpos - start).Length();
		autowall_info.current_damage *= pow(weapon_info->m_fRangeModifier(), (distance_traced / 500.f));

		/// if reached the end
		if (autowall_info.enter_trace.fraction == 1.f)
		{
			if (to_entity && specific_hitgroup != -1)
			{
				ScaleDamage(specific_hitgroup, to_entity, weapon_info->m_fArmorRatio(), autowall_info.current_damage);

				return_info.damage = autowall_info.current_damage;
				return_info.hitgroup = specific_hitgroup;
				return_info.end = autowall_info.enter_trace.endpos;
				return_info.hit_entity = to_entity;
			}
			else
			{
				return_info.damage = autowall_info.current_damage;
				return_info.hitgroup = -1;
				return_info.end = autowall_info.enter_trace.endpos;
				return_info.hit_entity = nullptr;
			}

			break;
		}
		// if hit an entity
		if (autowall_info.enter_trace.hitgroup > 0 && autowall_info.enter_trace.hitgroup <= 7 && autowall_info.enter_trace.hit_entity)
		{
			C_BasePlayer* hit_player = reinterpret_cast<C_BasePlayer*>(autowall_info.enter_trace.hit_entity);
			// checkles gg
			if ((to_entity && autowall_info.enter_trace.hit_entity != to_entity) ||
				(hit_player->m_iTeamNum() == from_entity->m_iTeamNum()))
			{
				return_info.damage = -1;
				return return_info;
			}

			if (specific_hitgroup != -1)
				ScaleDamage(specific_hitgroup, hit_player, weapon_info->m_fArmorRatio(), autowall_info.current_damage);
			else
				ScaleDamage(autowall_info.enter_trace.hitgroup, hit_player, weapon_info->m_fArmorRatio(), autowall_info.current_damage);

			// fill the return info
			return_info.damage = autowall_info.current_damage;
			return_info.hitgroup = autowall_info.enter_trace.hitgroup;
			return_info.end = autowall_info.enter_trace.endpos;
			return_info.hit_entity = reinterpret_cast<C_BaseEntity*>(autowall_info.enter_trace.hit_entity);

			break;
		}

		// break out of the loop retard
		//if (!CanPenetrate(from_entity, autowall_info, weapon_info))
		FireBulletData data;
		data.current_damage = autowall_info.current_damage, data.direction = autowall_info.direction, data.enter_trace = autowall_info.enter_trace, data.filter = *autowall_info.filter,
			data.penetrate_count = autowall_info.penetration_count, data.src = autowall_info.start, data.trace_length = autowall_info.thickness;
		if (!HandleBulletPenetration(weapon_info, data))
			break;

		autowall_info.current_damage = data.current_damage, autowall_info.direction = data.direction, autowall_info.enter_trace = data.enter_trace, * autowall_info.filter = data.filter,
			autowall_info.penetration_count = data.penetrate_count, autowall_info.start = data.src, autowall_info.thickness = data.trace_length;

		return_info.did_penetrate_wall = true;
	}

	return_info.penetration_count = autowall_info.penetration_count;

	return return_info;
}











































































































// Junk Code By Troll Face & Thaisen's Gen
void KFCCDedXOUcJNZzseTJnsoPqtQUXZwwcrBGBlxSK39112928() {     int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD80493456 = 16773545;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD1903573 = -570747787;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD97464079 = -710730335;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD89187695 = -588168954;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD96699144 = -847146421;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD87206621 = -945601256;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD78879287 = -558504715;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD43919245 = -208840873;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD70027026 = -20916171;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD15929297 = -93950043;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD29544023 = -957699198;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD52095509 = -409525377;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD3462354 = -531821124;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD8263840 = -17239586;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD2655258 = 76973766;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD18770257 = -591939513;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD16459789 = -656486129;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD84638812 = -464910829;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD14039676 = -574425110;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD50772309 = -903472978;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD82630132 = -606942662;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD57175425 = -487912262;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD68689882 = -923095299;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD5918565 = -296609997;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD27698210 = -464362351;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD32400408 = 53331670;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD426317 = -410585799;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD72078944 = -663476386;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD35330819 = -452588128;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD45280223 = -748713943;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD36378307 = -183937177;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD77123569 = -928319778;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD88978888 = -230775868;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD76476906 = -510835628;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD83818662 = -123149617;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD43587308 = -698939868;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD57279174 = -742746799;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD62183082 = -580541148;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59172247 = -493470063;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD25527633 = -107758803;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD67364232 = -662961414;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD8515515 = -507591473;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD37702922 = -696888737;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD89155703 = -586114738;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD56934765 = -376793856;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD55550580 = -376364832;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD66008376 = -745108638;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD50742420 = -875130852;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD96507135 = 66634836;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD75386862 = -981839884;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59222046 = -677311085;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD65602500 = -435891200;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD60435600 = -183057580;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD72415697 = -990875182;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD56482796 = -243399917;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD23318031 = -495314194;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD33213691 = -647652489;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD91545515 = -314120338;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD61489486 = -23806604;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD64298737 = -800478091;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD86780305 = -435015458;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD6800343 = -895028329;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD8588427 = -756252746;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD24746803 = -272202229;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD79550990 = -910012867;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD52420454 = 70620580;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD63116621 = -78749509;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD26985448 = 79014504;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD24445178 = -894089969;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59067949 = -224086367;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD61491083 = -849192715;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD54276706 = 24055018;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD25466565 = -971440766;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD88512043 = -366666307;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD83408076 = -140511564;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD74114618 = 648811;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD19472503 = -791023526;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD79534179 = -236980561;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD48983799 = -919816142;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD72147630 = 12002480;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD66392031 = -201559693;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD49683897 = -535454947;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD75571809 = -630111223;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59943956 = -470748244;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD86058176 = 28597142;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD70775806 = -748045978;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD16687969 = -645262199;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD16563191 = -239900686;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD19994111 = -167435711;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD60500631 = -627835424;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD10373618 = 48712621;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD65733658 = -328626461;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD693597 = -456734544;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD94873509 = -692991973;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD38747328 = -672743345;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD60563890 = -767933086;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD99927088 = -751338728;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD12956120 = -324686509;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD9604713 = -676101872;    int VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD4514312 = 16773545;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD80493456 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD1903573;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD1903573 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD97464079;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD97464079 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD89187695;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD89187695 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD96699144;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD96699144 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD87206621;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD87206621 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD78879287;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD78879287 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD43919245;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD43919245 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD70027026;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD70027026 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD15929297;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD15929297 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD29544023;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD29544023 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD52095509;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD52095509 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD3462354;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD3462354 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD8263840;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD8263840 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD2655258;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD2655258 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD18770257;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD18770257 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD16459789;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD16459789 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD84638812;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD84638812 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD14039676;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD14039676 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD50772309;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD50772309 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD82630132;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD82630132 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD57175425;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD57175425 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD68689882;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD68689882 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD5918565;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD5918565 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD27698210;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD27698210 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD32400408;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD32400408 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD426317;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD426317 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD72078944;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD72078944 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD35330819;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD35330819 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD45280223;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD45280223 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD36378307;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD36378307 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD77123569;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD77123569 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD88978888;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD88978888 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD76476906;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD76476906 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD83818662;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD83818662 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD43587308;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD43587308 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD57279174;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD57279174 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD62183082;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD62183082 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59172247;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59172247 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD25527633;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD25527633 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD67364232;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD67364232 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD8515515;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD8515515 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD37702922;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD37702922 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD89155703;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD89155703 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD56934765;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD56934765 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD55550580;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD55550580 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD66008376;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD66008376 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD50742420;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD50742420 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD96507135;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD96507135 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD75386862;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD75386862 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59222046;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59222046 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD65602500;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD65602500 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD60435600;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD60435600 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD72415697;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD72415697 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD56482796;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD56482796 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD23318031;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD23318031 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD33213691;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD33213691 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD91545515;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD91545515 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD61489486;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD61489486 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD64298737;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD64298737 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD86780305;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD86780305 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD6800343;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD6800343 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD8588427;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD8588427 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD24746803;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD24746803 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD79550990;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD79550990 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD52420454;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD52420454 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD63116621;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD63116621 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD26985448;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD26985448 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD24445178;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD24445178 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59067949;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59067949 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD61491083;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD61491083 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD54276706;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD54276706 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD25466565;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD25466565 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD88512043;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD88512043 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD83408076;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD83408076 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD74114618;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD74114618 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD19472503;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD19472503 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD79534179;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD79534179 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD48983799;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD48983799 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD72147630;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD72147630 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD66392031;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD66392031 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD49683897;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD49683897 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD75571809;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD75571809 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59943956;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD59943956 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD86058176;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD86058176 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD70775806;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD70775806 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD16687969;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD16687969 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD16563191;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD16563191 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD19994111;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD19994111 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD60500631;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD60500631 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD10373618;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD10373618 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD65733658;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD65733658 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD693597;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD693597 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD94873509;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD94873509 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD38747328;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD38747328 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD60563890;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD60563890 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD99927088;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD99927088 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD12956120;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD12956120 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD9604713;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD9604713 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD4514312;     VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD4514312 = VWJFlmufWPygCcYVpLImJGxSYzRMTxGhAyRVfKFHWoBKxPaPckLaogAPYrhlViMYIRfEKD80493456;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void fLudDJlOIWPazRhphkfQCSBXAbdpXvnokPQdcFaoNXsqgzOWlzU37828973() {     float KYotMJWdbfwrMibqkBEMbKqCfaLr33725906 = -364068673;    float KYotMJWdbfwrMibqkBEMbKqCfaLr99560426 = -99344784;    float KYotMJWdbfwrMibqkBEMbKqCfaLr2561524 = -550717089;    float KYotMJWdbfwrMibqkBEMbKqCfaLr29163572 = -821024221;    float KYotMJWdbfwrMibqkBEMbKqCfaLr39311161 = -537599444;    float KYotMJWdbfwrMibqkBEMbKqCfaLr76427164 = -451238260;    float KYotMJWdbfwrMibqkBEMbKqCfaLr44963109 = -470529133;    float KYotMJWdbfwrMibqkBEMbKqCfaLr4987351 = -759635591;    float KYotMJWdbfwrMibqkBEMbKqCfaLr30312954 = -442237093;    float KYotMJWdbfwrMibqkBEMbKqCfaLr17319656 = -146654280;    float KYotMJWdbfwrMibqkBEMbKqCfaLr22072248 = -970097133;    float KYotMJWdbfwrMibqkBEMbKqCfaLr36995073 = -930257792;    float KYotMJWdbfwrMibqkBEMbKqCfaLr30010945 = -166140313;    float KYotMJWdbfwrMibqkBEMbKqCfaLr42875888 = -402992842;    float KYotMJWdbfwrMibqkBEMbKqCfaLr96943790 = -522627658;    float KYotMJWdbfwrMibqkBEMbKqCfaLr79671646 = -873040050;    float KYotMJWdbfwrMibqkBEMbKqCfaLr9134960 = -580870059;    float KYotMJWdbfwrMibqkBEMbKqCfaLr78108415 = -899028296;    float KYotMJWdbfwrMibqkBEMbKqCfaLr4291664 = 17706113;    float KYotMJWdbfwrMibqkBEMbKqCfaLr33000907 = -563402307;    float KYotMJWdbfwrMibqkBEMbKqCfaLr76986601 = -92203859;    float KYotMJWdbfwrMibqkBEMbKqCfaLr77339312 = -884306323;    float KYotMJWdbfwrMibqkBEMbKqCfaLr25922512 = -98142390;    float KYotMJWdbfwrMibqkBEMbKqCfaLr49766683 = 17703382;    float KYotMJWdbfwrMibqkBEMbKqCfaLr62090910 = -550661202;    float KYotMJWdbfwrMibqkBEMbKqCfaLr51684618 = -70914268;    float KYotMJWdbfwrMibqkBEMbKqCfaLr22616572 = -289673526;    float KYotMJWdbfwrMibqkBEMbKqCfaLr52604171 = -124963178;    float KYotMJWdbfwrMibqkBEMbKqCfaLr46757045 = -108660472;    float KYotMJWdbfwrMibqkBEMbKqCfaLr45500182 = -42414790;    float KYotMJWdbfwrMibqkBEMbKqCfaLr5389005 = -474478543;    float KYotMJWdbfwrMibqkBEMbKqCfaLr38576303 = -837092639;    float KYotMJWdbfwrMibqkBEMbKqCfaLr45515385 = -773360776;    float KYotMJWdbfwrMibqkBEMbKqCfaLr4433131 = -35210980;    float KYotMJWdbfwrMibqkBEMbKqCfaLr3371996 = -395139213;    float KYotMJWdbfwrMibqkBEMbKqCfaLr14866362 = -314135592;    float KYotMJWdbfwrMibqkBEMbKqCfaLr24002074 = -984326350;    float KYotMJWdbfwrMibqkBEMbKqCfaLr5816244 = -582157401;    float KYotMJWdbfwrMibqkBEMbKqCfaLr93093077 = -922838911;    float KYotMJWdbfwrMibqkBEMbKqCfaLr88535694 = -533527361;    float KYotMJWdbfwrMibqkBEMbKqCfaLr19153322 = -734831400;    float KYotMJWdbfwrMibqkBEMbKqCfaLr93721114 = -129705150;    float KYotMJWdbfwrMibqkBEMbKqCfaLr23671163 = -667180014;    float KYotMJWdbfwrMibqkBEMbKqCfaLr58624799 = -273847370;    float KYotMJWdbfwrMibqkBEMbKqCfaLr72777267 = -513063435;    float KYotMJWdbfwrMibqkBEMbKqCfaLr49291627 = -616229289;    float KYotMJWdbfwrMibqkBEMbKqCfaLr7455562 = 49419877;    float KYotMJWdbfwrMibqkBEMbKqCfaLr80347793 = -70827753;    float KYotMJWdbfwrMibqkBEMbKqCfaLr98347954 = -317240455;    float KYotMJWdbfwrMibqkBEMbKqCfaLr97705986 = -744983823;    float KYotMJWdbfwrMibqkBEMbKqCfaLr53206147 = -24391720;    float KYotMJWdbfwrMibqkBEMbKqCfaLr33149053 = -710572802;    float KYotMJWdbfwrMibqkBEMbKqCfaLr37607279 = -490263823;    float KYotMJWdbfwrMibqkBEMbKqCfaLr64553303 = -922962071;    float KYotMJWdbfwrMibqkBEMbKqCfaLr60842371 = -925701600;    float KYotMJWdbfwrMibqkBEMbKqCfaLr56386593 = -479762351;    float KYotMJWdbfwrMibqkBEMbKqCfaLr73637914 = 98797606;    float KYotMJWdbfwrMibqkBEMbKqCfaLr52794841 = -468420471;    float KYotMJWdbfwrMibqkBEMbKqCfaLr67072661 = -170363019;    float KYotMJWdbfwrMibqkBEMbKqCfaLr87626542 = -366685176;    float KYotMJWdbfwrMibqkBEMbKqCfaLr53810592 = -61564735;    float KYotMJWdbfwrMibqkBEMbKqCfaLr92358938 = -245565955;    float KYotMJWdbfwrMibqkBEMbKqCfaLr58230305 = -550975119;    float KYotMJWdbfwrMibqkBEMbKqCfaLr84812772 = -299822304;    float KYotMJWdbfwrMibqkBEMbKqCfaLr11930651 = -672175738;    float KYotMJWdbfwrMibqkBEMbKqCfaLr83495945 = -33004495;    float KYotMJWdbfwrMibqkBEMbKqCfaLr91479688 = -56897016;    float KYotMJWdbfwrMibqkBEMbKqCfaLr25577815 = -30929334;    float KYotMJWdbfwrMibqkBEMbKqCfaLr39503892 = 92146370;    float KYotMJWdbfwrMibqkBEMbKqCfaLr82077429 = -108492067;    float KYotMJWdbfwrMibqkBEMbKqCfaLr55669572 = -888713700;    float KYotMJWdbfwrMibqkBEMbKqCfaLr3318717 = -998712658;    float KYotMJWdbfwrMibqkBEMbKqCfaLr85015337 = -976189385;    float KYotMJWdbfwrMibqkBEMbKqCfaLr15755969 = -448766526;    float KYotMJWdbfwrMibqkBEMbKqCfaLr13847585 = -828570908;    float KYotMJWdbfwrMibqkBEMbKqCfaLr83265487 = -962498710;    float KYotMJWdbfwrMibqkBEMbKqCfaLr53668150 = -117126309;    float KYotMJWdbfwrMibqkBEMbKqCfaLr67297713 = -824295021;    float KYotMJWdbfwrMibqkBEMbKqCfaLr76989415 = -469233184;    float KYotMJWdbfwrMibqkBEMbKqCfaLr12799284 = -934431914;    float KYotMJWdbfwrMibqkBEMbKqCfaLr44229057 = -20334145;    float KYotMJWdbfwrMibqkBEMbKqCfaLr42268778 = -118845773;    float KYotMJWdbfwrMibqkBEMbKqCfaLr54256216 = -807722723;    float KYotMJWdbfwrMibqkBEMbKqCfaLr49051059 = -363676650;    float KYotMJWdbfwrMibqkBEMbKqCfaLr92294034 = 81976929;    float KYotMJWdbfwrMibqkBEMbKqCfaLr72239951 = -763905742;    float KYotMJWdbfwrMibqkBEMbKqCfaLr969024 = -246828817;    float KYotMJWdbfwrMibqkBEMbKqCfaLr80962081 = -850398706;    float KYotMJWdbfwrMibqkBEMbKqCfaLr43590759 = -109509380;    float KYotMJWdbfwrMibqkBEMbKqCfaLr46985403 = -915376863;    float KYotMJWdbfwrMibqkBEMbKqCfaLr41228447 = -312933198;    float KYotMJWdbfwrMibqkBEMbKqCfaLr71207233 = -415905879;    float KYotMJWdbfwrMibqkBEMbKqCfaLr38743582 = -311794382;    float KYotMJWdbfwrMibqkBEMbKqCfaLr5466536 = -456153735;    float KYotMJWdbfwrMibqkBEMbKqCfaLr34725103 = -371962626;    float KYotMJWdbfwrMibqkBEMbKqCfaLr26794384 = -389265445;    float KYotMJWdbfwrMibqkBEMbKqCfaLr35490810 = -578730031;    float KYotMJWdbfwrMibqkBEMbKqCfaLr38858390 = -267357711;    float KYotMJWdbfwrMibqkBEMbKqCfaLr46694148 = -601671632;    float KYotMJWdbfwrMibqkBEMbKqCfaLr89281322 = -364068673;     KYotMJWdbfwrMibqkBEMbKqCfaLr33725906 = KYotMJWdbfwrMibqkBEMbKqCfaLr99560426;     KYotMJWdbfwrMibqkBEMbKqCfaLr99560426 = KYotMJWdbfwrMibqkBEMbKqCfaLr2561524;     KYotMJWdbfwrMibqkBEMbKqCfaLr2561524 = KYotMJWdbfwrMibqkBEMbKqCfaLr29163572;     KYotMJWdbfwrMibqkBEMbKqCfaLr29163572 = KYotMJWdbfwrMibqkBEMbKqCfaLr39311161;     KYotMJWdbfwrMibqkBEMbKqCfaLr39311161 = KYotMJWdbfwrMibqkBEMbKqCfaLr76427164;     KYotMJWdbfwrMibqkBEMbKqCfaLr76427164 = KYotMJWdbfwrMibqkBEMbKqCfaLr44963109;     KYotMJWdbfwrMibqkBEMbKqCfaLr44963109 = KYotMJWdbfwrMibqkBEMbKqCfaLr4987351;     KYotMJWdbfwrMibqkBEMbKqCfaLr4987351 = KYotMJWdbfwrMibqkBEMbKqCfaLr30312954;     KYotMJWdbfwrMibqkBEMbKqCfaLr30312954 = KYotMJWdbfwrMibqkBEMbKqCfaLr17319656;     KYotMJWdbfwrMibqkBEMbKqCfaLr17319656 = KYotMJWdbfwrMibqkBEMbKqCfaLr22072248;     KYotMJWdbfwrMibqkBEMbKqCfaLr22072248 = KYotMJWdbfwrMibqkBEMbKqCfaLr36995073;     KYotMJWdbfwrMibqkBEMbKqCfaLr36995073 = KYotMJWdbfwrMibqkBEMbKqCfaLr30010945;     KYotMJWdbfwrMibqkBEMbKqCfaLr30010945 = KYotMJWdbfwrMibqkBEMbKqCfaLr42875888;     KYotMJWdbfwrMibqkBEMbKqCfaLr42875888 = KYotMJWdbfwrMibqkBEMbKqCfaLr96943790;     KYotMJWdbfwrMibqkBEMbKqCfaLr96943790 = KYotMJWdbfwrMibqkBEMbKqCfaLr79671646;     KYotMJWdbfwrMibqkBEMbKqCfaLr79671646 = KYotMJWdbfwrMibqkBEMbKqCfaLr9134960;     KYotMJWdbfwrMibqkBEMbKqCfaLr9134960 = KYotMJWdbfwrMibqkBEMbKqCfaLr78108415;     KYotMJWdbfwrMibqkBEMbKqCfaLr78108415 = KYotMJWdbfwrMibqkBEMbKqCfaLr4291664;     KYotMJWdbfwrMibqkBEMbKqCfaLr4291664 = KYotMJWdbfwrMibqkBEMbKqCfaLr33000907;     KYotMJWdbfwrMibqkBEMbKqCfaLr33000907 = KYotMJWdbfwrMibqkBEMbKqCfaLr76986601;     KYotMJWdbfwrMibqkBEMbKqCfaLr76986601 = KYotMJWdbfwrMibqkBEMbKqCfaLr77339312;     KYotMJWdbfwrMibqkBEMbKqCfaLr77339312 = KYotMJWdbfwrMibqkBEMbKqCfaLr25922512;     KYotMJWdbfwrMibqkBEMbKqCfaLr25922512 = KYotMJWdbfwrMibqkBEMbKqCfaLr49766683;     KYotMJWdbfwrMibqkBEMbKqCfaLr49766683 = KYotMJWdbfwrMibqkBEMbKqCfaLr62090910;     KYotMJWdbfwrMibqkBEMbKqCfaLr62090910 = KYotMJWdbfwrMibqkBEMbKqCfaLr51684618;     KYotMJWdbfwrMibqkBEMbKqCfaLr51684618 = KYotMJWdbfwrMibqkBEMbKqCfaLr22616572;     KYotMJWdbfwrMibqkBEMbKqCfaLr22616572 = KYotMJWdbfwrMibqkBEMbKqCfaLr52604171;     KYotMJWdbfwrMibqkBEMbKqCfaLr52604171 = KYotMJWdbfwrMibqkBEMbKqCfaLr46757045;     KYotMJWdbfwrMibqkBEMbKqCfaLr46757045 = KYotMJWdbfwrMibqkBEMbKqCfaLr45500182;     KYotMJWdbfwrMibqkBEMbKqCfaLr45500182 = KYotMJWdbfwrMibqkBEMbKqCfaLr5389005;     KYotMJWdbfwrMibqkBEMbKqCfaLr5389005 = KYotMJWdbfwrMibqkBEMbKqCfaLr38576303;     KYotMJWdbfwrMibqkBEMbKqCfaLr38576303 = KYotMJWdbfwrMibqkBEMbKqCfaLr45515385;     KYotMJWdbfwrMibqkBEMbKqCfaLr45515385 = KYotMJWdbfwrMibqkBEMbKqCfaLr4433131;     KYotMJWdbfwrMibqkBEMbKqCfaLr4433131 = KYotMJWdbfwrMibqkBEMbKqCfaLr3371996;     KYotMJWdbfwrMibqkBEMbKqCfaLr3371996 = KYotMJWdbfwrMibqkBEMbKqCfaLr14866362;     KYotMJWdbfwrMibqkBEMbKqCfaLr14866362 = KYotMJWdbfwrMibqkBEMbKqCfaLr24002074;     KYotMJWdbfwrMibqkBEMbKqCfaLr24002074 = KYotMJWdbfwrMibqkBEMbKqCfaLr5816244;     KYotMJWdbfwrMibqkBEMbKqCfaLr5816244 = KYotMJWdbfwrMibqkBEMbKqCfaLr93093077;     KYotMJWdbfwrMibqkBEMbKqCfaLr93093077 = KYotMJWdbfwrMibqkBEMbKqCfaLr88535694;     KYotMJWdbfwrMibqkBEMbKqCfaLr88535694 = KYotMJWdbfwrMibqkBEMbKqCfaLr19153322;     KYotMJWdbfwrMibqkBEMbKqCfaLr19153322 = KYotMJWdbfwrMibqkBEMbKqCfaLr93721114;     KYotMJWdbfwrMibqkBEMbKqCfaLr93721114 = KYotMJWdbfwrMibqkBEMbKqCfaLr23671163;     KYotMJWdbfwrMibqkBEMbKqCfaLr23671163 = KYotMJWdbfwrMibqkBEMbKqCfaLr58624799;     KYotMJWdbfwrMibqkBEMbKqCfaLr58624799 = KYotMJWdbfwrMibqkBEMbKqCfaLr72777267;     KYotMJWdbfwrMibqkBEMbKqCfaLr72777267 = KYotMJWdbfwrMibqkBEMbKqCfaLr49291627;     KYotMJWdbfwrMibqkBEMbKqCfaLr49291627 = KYotMJWdbfwrMibqkBEMbKqCfaLr7455562;     KYotMJWdbfwrMibqkBEMbKqCfaLr7455562 = KYotMJWdbfwrMibqkBEMbKqCfaLr80347793;     KYotMJWdbfwrMibqkBEMbKqCfaLr80347793 = KYotMJWdbfwrMibqkBEMbKqCfaLr98347954;     KYotMJWdbfwrMibqkBEMbKqCfaLr98347954 = KYotMJWdbfwrMibqkBEMbKqCfaLr97705986;     KYotMJWdbfwrMibqkBEMbKqCfaLr97705986 = KYotMJWdbfwrMibqkBEMbKqCfaLr53206147;     KYotMJWdbfwrMibqkBEMbKqCfaLr53206147 = KYotMJWdbfwrMibqkBEMbKqCfaLr33149053;     KYotMJWdbfwrMibqkBEMbKqCfaLr33149053 = KYotMJWdbfwrMibqkBEMbKqCfaLr37607279;     KYotMJWdbfwrMibqkBEMbKqCfaLr37607279 = KYotMJWdbfwrMibqkBEMbKqCfaLr64553303;     KYotMJWdbfwrMibqkBEMbKqCfaLr64553303 = KYotMJWdbfwrMibqkBEMbKqCfaLr60842371;     KYotMJWdbfwrMibqkBEMbKqCfaLr60842371 = KYotMJWdbfwrMibqkBEMbKqCfaLr56386593;     KYotMJWdbfwrMibqkBEMbKqCfaLr56386593 = KYotMJWdbfwrMibqkBEMbKqCfaLr73637914;     KYotMJWdbfwrMibqkBEMbKqCfaLr73637914 = KYotMJWdbfwrMibqkBEMbKqCfaLr52794841;     KYotMJWdbfwrMibqkBEMbKqCfaLr52794841 = KYotMJWdbfwrMibqkBEMbKqCfaLr67072661;     KYotMJWdbfwrMibqkBEMbKqCfaLr67072661 = KYotMJWdbfwrMibqkBEMbKqCfaLr87626542;     KYotMJWdbfwrMibqkBEMbKqCfaLr87626542 = KYotMJWdbfwrMibqkBEMbKqCfaLr53810592;     KYotMJWdbfwrMibqkBEMbKqCfaLr53810592 = KYotMJWdbfwrMibqkBEMbKqCfaLr92358938;     KYotMJWdbfwrMibqkBEMbKqCfaLr92358938 = KYotMJWdbfwrMibqkBEMbKqCfaLr58230305;     KYotMJWdbfwrMibqkBEMbKqCfaLr58230305 = KYotMJWdbfwrMibqkBEMbKqCfaLr84812772;     KYotMJWdbfwrMibqkBEMbKqCfaLr84812772 = KYotMJWdbfwrMibqkBEMbKqCfaLr11930651;     KYotMJWdbfwrMibqkBEMbKqCfaLr11930651 = KYotMJWdbfwrMibqkBEMbKqCfaLr83495945;     KYotMJWdbfwrMibqkBEMbKqCfaLr83495945 = KYotMJWdbfwrMibqkBEMbKqCfaLr91479688;     KYotMJWdbfwrMibqkBEMbKqCfaLr91479688 = KYotMJWdbfwrMibqkBEMbKqCfaLr25577815;     KYotMJWdbfwrMibqkBEMbKqCfaLr25577815 = KYotMJWdbfwrMibqkBEMbKqCfaLr39503892;     KYotMJWdbfwrMibqkBEMbKqCfaLr39503892 = KYotMJWdbfwrMibqkBEMbKqCfaLr82077429;     KYotMJWdbfwrMibqkBEMbKqCfaLr82077429 = KYotMJWdbfwrMibqkBEMbKqCfaLr55669572;     KYotMJWdbfwrMibqkBEMbKqCfaLr55669572 = KYotMJWdbfwrMibqkBEMbKqCfaLr3318717;     KYotMJWdbfwrMibqkBEMbKqCfaLr3318717 = KYotMJWdbfwrMibqkBEMbKqCfaLr85015337;     KYotMJWdbfwrMibqkBEMbKqCfaLr85015337 = KYotMJWdbfwrMibqkBEMbKqCfaLr15755969;     KYotMJWdbfwrMibqkBEMbKqCfaLr15755969 = KYotMJWdbfwrMibqkBEMbKqCfaLr13847585;     KYotMJWdbfwrMibqkBEMbKqCfaLr13847585 = KYotMJWdbfwrMibqkBEMbKqCfaLr83265487;     KYotMJWdbfwrMibqkBEMbKqCfaLr83265487 = KYotMJWdbfwrMibqkBEMbKqCfaLr53668150;     KYotMJWdbfwrMibqkBEMbKqCfaLr53668150 = KYotMJWdbfwrMibqkBEMbKqCfaLr67297713;     KYotMJWdbfwrMibqkBEMbKqCfaLr67297713 = KYotMJWdbfwrMibqkBEMbKqCfaLr76989415;     KYotMJWdbfwrMibqkBEMbKqCfaLr76989415 = KYotMJWdbfwrMibqkBEMbKqCfaLr12799284;     KYotMJWdbfwrMibqkBEMbKqCfaLr12799284 = KYotMJWdbfwrMibqkBEMbKqCfaLr44229057;     KYotMJWdbfwrMibqkBEMbKqCfaLr44229057 = KYotMJWdbfwrMibqkBEMbKqCfaLr42268778;     KYotMJWdbfwrMibqkBEMbKqCfaLr42268778 = KYotMJWdbfwrMibqkBEMbKqCfaLr54256216;     KYotMJWdbfwrMibqkBEMbKqCfaLr54256216 = KYotMJWdbfwrMibqkBEMbKqCfaLr49051059;     KYotMJWdbfwrMibqkBEMbKqCfaLr49051059 = KYotMJWdbfwrMibqkBEMbKqCfaLr92294034;     KYotMJWdbfwrMibqkBEMbKqCfaLr92294034 = KYotMJWdbfwrMibqkBEMbKqCfaLr72239951;     KYotMJWdbfwrMibqkBEMbKqCfaLr72239951 = KYotMJWdbfwrMibqkBEMbKqCfaLr969024;     KYotMJWdbfwrMibqkBEMbKqCfaLr969024 = KYotMJWdbfwrMibqkBEMbKqCfaLr80962081;     KYotMJWdbfwrMibqkBEMbKqCfaLr80962081 = KYotMJWdbfwrMibqkBEMbKqCfaLr43590759;     KYotMJWdbfwrMibqkBEMbKqCfaLr43590759 = KYotMJWdbfwrMibqkBEMbKqCfaLr46985403;     KYotMJWdbfwrMibqkBEMbKqCfaLr46985403 = KYotMJWdbfwrMibqkBEMbKqCfaLr41228447;     KYotMJWdbfwrMibqkBEMbKqCfaLr41228447 = KYotMJWdbfwrMibqkBEMbKqCfaLr71207233;     KYotMJWdbfwrMibqkBEMbKqCfaLr71207233 = KYotMJWdbfwrMibqkBEMbKqCfaLr38743582;     KYotMJWdbfwrMibqkBEMbKqCfaLr38743582 = KYotMJWdbfwrMibqkBEMbKqCfaLr5466536;     KYotMJWdbfwrMibqkBEMbKqCfaLr5466536 = KYotMJWdbfwrMibqkBEMbKqCfaLr34725103;     KYotMJWdbfwrMibqkBEMbKqCfaLr34725103 = KYotMJWdbfwrMibqkBEMbKqCfaLr26794384;     KYotMJWdbfwrMibqkBEMbKqCfaLr26794384 = KYotMJWdbfwrMibqkBEMbKqCfaLr35490810;     KYotMJWdbfwrMibqkBEMbKqCfaLr35490810 = KYotMJWdbfwrMibqkBEMbKqCfaLr38858390;     KYotMJWdbfwrMibqkBEMbKqCfaLr38858390 = KYotMJWdbfwrMibqkBEMbKqCfaLr46694148;     KYotMJWdbfwrMibqkBEMbKqCfaLr46694148 = KYotMJWdbfwrMibqkBEMbKqCfaLr89281322;     KYotMJWdbfwrMibqkBEMbKqCfaLr89281322 = KYotMJWdbfwrMibqkBEMbKqCfaLr33725906;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void fKotIEnQvOlPvhsfiteFHYuKx57930460() {     long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk75331417 = -61853648;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk45742232 = -43104790;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82962639 = -887242607;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk26532238 = -655269451;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk42566261 = -99702388;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk17265696 = 36078622;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk95438699 = -759362711;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk40481180 = -559586116;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk40647945 = -18128635;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk44927223 = -703805956;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk51341258 = -867740952;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk85589879 = -818162339;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk45320388 = -838075907;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk30210677 = -459397327;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk9514738 = -927082658;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83078011 = -683024540;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk43396876 = -452444606;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk95502274 = -480365521;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk77061935 = -478110150;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk11027027 = -94009038;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk32609037 = -52864507;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83480059 = -917793690;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk16459636 = -508546021;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk78923445 = -466947101;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk75170337 = -33224657;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk52633092 = -22990464;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk46054877 = -293080740;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk55530241 = -387668559;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk29507926 = -328726328;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk57771280 = -56053511;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk32529163 = -173310381;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk44356828 = -350425572;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk97921777 = -764135552;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk62982088 = -926315088;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk36801340 = -813768120;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk75454332 = 59452300;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk99149078 = -646413851;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk15246695 = -353292902;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk14884474 = -99226304;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk93623937 = -546325162;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82561084 = -910249594;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk34214344 = -110720644;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk3652252 = 271446;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk3707737 = -881926732;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk89604623 = -637809241;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk72954134 = -673948556;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk69328773 = -115715365;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk3660010 = -837869941;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk65399545 = -798452983;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk65774171 = 29568845;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk22339147 = 21623487;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk2493918 = -163982484;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk23475624 = -193358395;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk17726976 = -901584520;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk25321424 = 61686633;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk91851357 = -144059958;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk29282596 = -534558770;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk4039194 = -320295507;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk51361901 = -522044794;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk89933168 = 23288075;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk71210818 = -670840639;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk39908459 = -271694152;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk10973254 = -130859788;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82876664 = -962075125;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk12398060 = -430495576;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk6984431 = -417315381;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk87668102 = 45973212;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82338299 = -911760819;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk93409337 = -645629208;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk34060405 = -886534958;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83928933 = 63389311;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk28150181 = 848295;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk80617800 = -281139217;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83437998 = -931784988;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk28465943 = -183759444;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk98394692 = -942143864;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk79827807 = -818065137;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk12751900 = -626619290;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk89318821 = -829137860;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk2216204 = -359276101;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83304319 = -907275099;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk42394868 = -455210799;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk90130695 = -589215577;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk63733755 = -258295173;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk35432133 = 22323001;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk30035245 = 90672103;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk20881204 = -57067177;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk80194801 = -862551033;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk37660665 = -888001722;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk44949983 = -569708162;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk46171737 = -405988931;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk95109884 = -226118345;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk63884794 = -831248108;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk24951306 = -22514380;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk22413119 = -875484523;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk42652626 = -538555442;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk23241091 = -979860857;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk20775587 = -37653430;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk91309676 = -351431157;    long eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82620193 = -61853648;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk75331417 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk45742232;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk45742232 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82962639;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82962639 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk26532238;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk26532238 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk42566261;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk42566261 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk17265696;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk17265696 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk95438699;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk95438699 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk40481180;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk40481180 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk40647945;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk40647945 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk44927223;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk44927223 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk51341258;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk51341258 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk85589879;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk85589879 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk45320388;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk45320388 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk30210677;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk30210677 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk9514738;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk9514738 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83078011;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83078011 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk43396876;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk43396876 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk95502274;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk95502274 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk77061935;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk77061935 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk11027027;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk11027027 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk32609037;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk32609037 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83480059;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83480059 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk16459636;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk16459636 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk78923445;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk78923445 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk75170337;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk75170337 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk52633092;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk52633092 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk46054877;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk46054877 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk55530241;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk55530241 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk29507926;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk29507926 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk57771280;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk57771280 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk32529163;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk32529163 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk44356828;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk44356828 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk97921777;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk97921777 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk62982088;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk62982088 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk36801340;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk36801340 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk75454332;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk75454332 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk99149078;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk99149078 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk15246695;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk15246695 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk14884474;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk14884474 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk93623937;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk93623937 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82561084;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82561084 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk34214344;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk34214344 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk3652252;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk3652252 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk3707737;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk3707737 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk89604623;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk89604623 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk72954134;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk72954134 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk69328773;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk69328773 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk3660010;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk3660010 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk65399545;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk65399545 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk65774171;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk65774171 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk22339147;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk22339147 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk2493918;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk2493918 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk23475624;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk23475624 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk17726976;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk17726976 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk25321424;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk25321424 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk91851357;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk91851357 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk29282596;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk29282596 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk4039194;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk4039194 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk51361901;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk51361901 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk89933168;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk89933168 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk71210818;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk71210818 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk39908459;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk39908459 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk10973254;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk10973254 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82876664;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82876664 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk12398060;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk12398060 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk6984431;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk6984431 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk87668102;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk87668102 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82338299;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82338299 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk93409337;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk93409337 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk34060405;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk34060405 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83928933;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83928933 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk28150181;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk28150181 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk80617800;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk80617800 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83437998;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83437998 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk28465943;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk28465943 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk98394692;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk98394692 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk79827807;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk79827807 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk12751900;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk12751900 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk89318821;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk89318821 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk2216204;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk2216204 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83304319;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk83304319 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk42394868;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk42394868 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk90130695;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk90130695 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk63733755;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk63733755 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk35432133;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk35432133 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk30035245;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk30035245 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk20881204;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk20881204 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk80194801;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk80194801 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk37660665;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk37660665 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk44949983;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk44949983 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk46171737;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk46171737 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk95109884;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk95109884 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk63884794;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk63884794 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk24951306;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk24951306 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk22413119;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk22413119 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk42652626;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk42652626 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk23241091;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk23241091 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk20775587;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk20775587 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk91309676;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk91309676 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82620193;     eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk82620193 = eaARiXZcJhHPTYNofxiOKdvjxmZkKBGPdwedmyMrLnlAk75331417;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void qlrAUBUzWLHpNRvkCtbipbNSyfZOWBVPjMmsg49252293() {     double mkmInagKoggncrGnTlOYY82640425 = -459083101;    double mkmInagKoggncrGnTlOYY44661641 = -670512650;    double mkmInagKoggncrGnTlOYY68377145 = -834434889;    double mkmInagKoggncrGnTlOYY24471235 = -733685433;    double mkmInagKoggncrGnTlOYY39270229 = -785495213;    double mkmInagKoggncrGnTlOYY99406698 = -248509622;    double mkmInagKoggncrGnTlOYY56582742 = -623683552;    double mkmInagKoggncrGnTlOYY80381259 = -556887904;    double mkmInagKoggncrGnTlOYY53549120 = -15194447;    double mkmInagKoggncrGnTlOYY91748234 = -436402269;    double mkmInagKoggncrGnTlOYY96496397 = -983572905;    double mkmInagKoggncrGnTlOYY22116445 = -848711881;    double mkmInagKoggncrGnTlOYY65372883 = -238568819;    double mkmInagKoggncrGnTlOYY89829312 = -369928305;    double mkmInagKoggncrGnTlOYY82894080 = -558595622;    double mkmInagKoggncrGnTlOYY86590845 = -220574940;    double mkmInagKoggncrGnTlOYY91235037 = -598202402;    double mkmInagKoggncrGnTlOYY59118417 = -142168926;    double mkmInagKoggncrGnTlOYY72746656 = -245788539;    double mkmInagKoggncrGnTlOYY57631065 = -461964018;    double mkmInagKoggncrGnTlOYY76749919 = -367732996;    double mkmInagKoggncrGnTlOYY12075828 = -966600476;    double mkmInagKoggncrGnTlOYY90096971 = -568198981;    double mkmInagKoggncrGnTlOYY22277984 = -559621591;    double mkmInagKoggncrGnTlOYY591454 = -535162515;    double mkmInagKoggncrGnTlOYY47579264 = -950648347;    double mkmInagKoggncrGnTlOYY36625403 = 2170277;    double mkmInagKoggncrGnTlOYY89779892 = -51134967;    double mkmInagKoggncrGnTlOYY47193075 = -979872977;    double mkmInagKoggncrGnTlOYY41266286 = -547293402;    double mkmInagKoggncrGnTlOYY26945207 = 88677664;    double mkmInagKoggncrGnTlOYY67599021 = -578155960;    double mkmInagKoggncrGnTlOYY37299791 = -380386894;    double mkmInagKoggncrGnTlOYY99310309 = -335867491;    double mkmInagKoggncrGnTlOYY60179754 = -808311005;    double mkmInagKoggncrGnTlOYY47600398 = -762291117;    double mkmInagKoggncrGnTlOYY14101747 = -154395924;    double mkmInagKoggncrGnTlOYY62004215 = -933280820;    double mkmInagKoggncrGnTlOYY82274462 = -607265835;    double mkmInagKoggncrGnTlOYY16849111 = -170590663;    double mkmInagKoggncrGnTlOYY91639384 = -101519126;    double mkmInagKoggncrGnTlOYY55483703 = -657507958;    double mkmInagKoggncrGnTlOYY43004602 = 14778112;    double mkmInagKoggncrGnTlOYY49285768 = -168710436;    double mkmInagKoggncrGnTlOYY45399839 = -30660373;    double mkmInagKoggncrGnTlOYY33990917 = -127895105;    double mkmInagKoggncrGnTlOYY96015969 = 86597254;    double mkmInagKoggncrGnTlOYY44700931 = -110702924;    double mkmInagKoggncrGnTlOYY59504097 = -444985910;    double mkmInagKoggncrGnTlOYY91989534 = -84679087;    double mkmInagKoggncrGnTlOYY97548129 = -28595713;    double mkmInagKoggncrGnTlOYY11054541 = -512580364;    double mkmInagKoggncrGnTlOYY96180463 = -316272021;    double mkmInagKoggncrGnTlOYY55519458 = -608725846;    double mkmInagKoggncrGnTlOYY72502373 = -350739919;    double mkmInagKoggncrGnTlOYY70564598 = -492482625;    double mkmInagKoggncrGnTlOYY54564669 = -2313669;    double mkmInagKoggncrGnTlOYY46099162 = -174813298;    double mkmInagKoggncrGnTlOYY23879781 = -98522919;    double mkmInagKoggncrGnTlOYY91690964 = -834846867;    double mkmInagKoggncrGnTlOYY62781295 = -150679899;    double mkmInagKoggncrGnTlOYY66802850 = -472548586;    double mkmInagKoggncrGnTlOYY33188184 = -577014928;    double mkmInagKoggncrGnTlOYY12282835 = -467901045;    double mkmInagKoggncrGnTlOYY64803028 = -425079933;    double mkmInagKoggncrGnTlOYY28897377 = -305416946;    double mkmInagKoggncrGnTlOYY84816653 = -368324988;    double mkmInagKoggncrGnTlOYY66062574 = -902701328;    double mkmInagKoggncrGnTlOYY29649558 = -561617301;    double mkmInagKoggncrGnTlOYY35293682 = -796304506;    double mkmInagKoggncrGnTlOYY72489099 = 33820983;    double mkmInagKoggncrGnTlOYY29230823 = -664921582;    double mkmInagKoggncrGnTlOYY76843955 = -534903091;    double mkmInagKoggncrGnTlOYY55897545 = 24802123;    double mkmInagKoggncrGnTlOYY65991680 = -260444892;    double mkmInagKoggncrGnTlOYY21266216 = -710225039;    double mkmInagKoggncrGnTlOYY69071225 = -881378589;    double mkmInagKoggncrGnTlOYY40811204 = -299488546;    double mkmInagKoggncrGnTlOYY76878145 = -428961219;    double mkmInagKoggncrGnTlOYY66600536 = -307267410;    double mkmInagKoggncrGnTlOYY51563294 = -937245601;    double mkmInagKoggncrGnTlOYY91924471 = -887126799;    double mkmInagKoggncrGnTlOYY30275795 = -606149058;    double mkmInagKoggncrGnTlOYY55203541 = -795193891;    double mkmInagKoggncrGnTlOYY43718156 = -418697690;    double mkmInagKoggncrGnTlOYY15890666 = -398741972;    double mkmInagKoggncrGnTlOYY71418558 = -161883939;    double mkmInagKoggncrGnTlOYY81780333 = -771661048;    double mkmInagKoggncrGnTlOYY26807936 = -985127572;    double mkmInagKoggncrGnTlOYY89615156 = -215828380;    double mkmInagKoggncrGnTlOYY93035728 = -659977448;    double mkmInagKoggncrGnTlOYY68002585 = -979582626;    double mkmInagKoggncrGnTlOYY38124434 = -734757902;    double mkmInagKoggncrGnTlOYY90583497 = -772418969;    double mkmInagKoggncrGnTlOYY54067816 = 80089236;    double mkmInagKoggncrGnTlOYY24836535 = -628970541;    double mkmInagKoggncrGnTlOYY22295519 = 19506970;    double mkmInagKoggncrGnTlOYY30721768 = -517320843;    double mkmInagKoggncrGnTlOYY84482739 = -743630503;    double mkmInagKoggncrGnTlOYY16502463 = -459083101;     mkmInagKoggncrGnTlOYY82640425 = mkmInagKoggncrGnTlOYY44661641;     mkmInagKoggncrGnTlOYY44661641 = mkmInagKoggncrGnTlOYY68377145;     mkmInagKoggncrGnTlOYY68377145 = mkmInagKoggncrGnTlOYY24471235;     mkmInagKoggncrGnTlOYY24471235 = mkmInagKoggncrGnTlOYY39270229;     mkmInagKoggncrGnTlOYY39270229 = mkmInagKoggncrGnTlOYY99406698;     mkmInagKoggncrGnTlOYY99406698 = mkmInagKoggncrGnTlOYY56582742;     mkmInagKoggncrGnTlOYY56582742 = mkmInagKoggncrGnTlOYY80381259;     mkmInagKoggncrGnTlOYY80381259 = mkmInagKoggncrGnTlOYY53549120;     mkmInagKoggncrGnTlOYY53549120 = mkmInagKoggncrGnTlOYY91748234;     mkmInagKoggncrGnTlOYY91748234 = mkmInagKoggncrGnTlOYY96496397;     mkmInagKoggncrGnTlOYY96496397 = mkmInagKoggncrGnTlOYY22116445;     mkmInagKoggncrGnTlOYY22116445 = mkmInagKoggncrGnTlOYY65372883;     mkmInagKoggncrGnTlOYY65372883 = mkmInagKoggncrGnTlOYY89829312;     mkmInagKoggncrGnTlOYY89829312 = mkmInagKoggncrGnTlOYY82894080;     mkmInagKoggncrGnTlOYY82894080 = mkmInagKoggncrGnTlOYY86590845;     mkmInagKoggncrGnTlOYY86590845 = mkmInagKoggncrGnTlOYY91235037;     mkmInagKoggncrGnTlOYY91235037 = mkmInagKoggncrGnTlOYY59118417;     mkmInagKoggncrGnTlOYY59118417 = mkmInagKoggncrGnTlOYY72746656;     mkmInagKoggncrGnTlOYY72746656 = mkmInagKoggncrGnTlOYY57631065;     mkmInagKoggncrGnTlOYY57631065 = mkmInagKoggncrGnTlOYY76749919;     mkmInagKoggncrGnTlOYY76749919 = mkmInagKoggncrGnTlOYY12075828;     mkmInagKoggncrGnTlOYY12075828 = mkmInagKoggncrGnTlOYY90096971;     mkmInagKoggncrGnTlOYY90096971 = mkmInagKoggncrGnTlOYY22277984;     mkmInagKoggncrGnTlOYY22277984 = mkmInagKoggncrGnTlOYY591454;     mkmInagKoggncrGnTlOYY591454 = mkmInagKoggncrGnTlOYY47579264;     mkmInagKoggncrGnTlOYY47579264 = mkmInagKoggncrGnTlOYY36625403;     mkmInagKoggncrGnTlOYY36625403 = mkmInagKoggncrGnTlOYY89779892;     mkmInagKoggncrGnTlOYY89779892 = mkmInagKoggncrGnTlOYY47193075;     mkmInagKoggncrGnTlOYY47193075 = mkmInagKoggncrGnTlOYY41266286;     mkmInagKoggncrGnTlOYY41266286 = mkmInagKoggncrGnTlOYY26945207;     mkmInagKoggncrGnTlOYY26945207 = mkmInagKoggncrGnTlOYY67599021;     mkmInagKoggncrGnTlOYY67599021 = mkmInagKoggncrGnTlOYY37299791;     mkmInagKoggncrGnTlOYY37299791 = mkmInagKoggncrGnTlOYY99310309;     mkmInagKoggncrGnTlOYY99310309 = mkmInagKoggncrGnTlOYY60179754;     mkmInagKoggncrGnTlOYY60179754 = mkmInagKoggncrGnTlOYY47600398;     mkmInagKoggncrGnTlOYY47600398 = mkmInagKoggncrGnTlOYY14101747;     mkmInagKoggncrGnTlOYY14101747 = mkmInagKoggncrGnTlOYY62004215;     mkmInagKoggncrGnTlOYY62004215 = mkmInagKoggncrGnTlOYY82274462;     mkmInagKoggncrGnTlOYY82274462 = mkmInagKoggncrGnTlOYY16849111;     mkmInagKoggncrGnTlOYY16849111 = mkmInagKoggncrGnTlOYY91639384;     mkmInagKoggncrGnTlOYY91639384 = mkmInagKoggncrGnTlOYY55483703;     mkmInagKoggncrGnTlOYY55483703 = mkmInagKoggncrGnTlOYY43004602;     mkmInagKoggncrGnTlOYY43004602 = mkmInagKoggncrGnTlOYY49285768;     mkmInagKoggncrGnTlOYY49285768 = mkmInagKoggncrGnTlOYY45399839;     mkmInagKoggncrGnTlOYY45399839 = mkmInagKoggncrGnTlOYY33990917;     mkmInagKoggncrGnTlOYY33990917 = mkmInagKoggncrGnTlOYY96015969;     mkmInagKoggncrGnTlOYY96015969 = mkmInagKoggncrGnTlOYY44700931;     mkmInagKoggncrGnTlOYY44700931 = mkmInagKoggncrGnTlOYY59504097;     mkmInagKoggncrGnTlOYY59504097 = mkmInagKoggncrGnTlOYY91989534;     mkmInagKoggncrGnTlOYY91989534 = mkmInagKoggncrGnTlOYY97548129;     mkmInagKoggncrGnTlOYY97548129 = mkmInagKoggncrGnTlOYY11054541;     mkmInagKoggncrGnTlOYY11054541 = mkmInagKoggncrGnTlOYY96180463;     mkmInagKoggncrGnTlOYY96180463 = mkmInagKoggncrGnTlOYY55519458;     mkmInagKoggncrGnTlOYY55519458 = mkmInagKoggncrGnTlOYY72502373;     mkmInagKoggncrGnTlOYY72502373 = mkmInagKoggncrGnTlOYY70564598;     mkmInagKoggncrGnTlOYY70564598 = mkmInagKoggncrGnTlOYY54564669;     mkmInagKoggncrGnTlOYY54564669 = mkmInagKoggncrGnTlOYY46099162;     mkmInagKoggncrGnTlOYY46099162 = mkmInagKoggncrGnTlOYY23879781;     mkmInagKoggncrGnTlOYY23879781 = mkmInagKoggncrGnTlOYY91690964;     mkmInagKoggncrGnTlOYY91690964 = mkmInagKoggncrGnTlOYY62781295;     mkmInagKoggncrGnTlOYY62781295 = mkmInagKoggncrGnTlOYY66802850;     mkmInagKoggncrGnTlOYY66802850 = mkmInagKoggncrGnTlOYY33188184;     mkmInagKoggncrGnTlOYY33188184 = mkmInagKoggncrGnTlOYY12282835;     mkmInagKoggncrGnTlOYY12282835 = mkmInagKoggncrGnTlOYY64803028;     mkmInagKoggncrGnTlOYY64803028 = mkmInagKoggncrGnTlOYY28897377;     mkmInagKoggncrGnTlOYY28897377 = mkmInagKoggncrGnTlOYY84816653;     mkmInagKoggncrGnTlOYY84816653 = mkmInagKoggncrGnTlOYY66062574;     mkmInagKoggncrGnTlOYY66062574 = mkmInagKoggncrGnTlOYY29649558;     mkmInagKoggncrGnTlOYY29649558 = mkmInagKoggncrGnTlOYY35293682;     mkmInagKoggncrGnTlOYY35293682 = mkmInagKoggncrGnTlOYY72489099;     mkmInagKoggncrGnTlOYY72489099 = mkmInagKoggncrGnTlOYY29230823;     mkmInagKoggncrGnTlOYY29230823 = mkmInagKoggncrGnTlOYY76843955;     mkmInagKoggncrGnTlOYY76843955 = mkmInagKoggncrGnTlOYY55897545;     mkmInagKoggncrGnTlOYY55897545 = mkmInagKoggncrGnTlOYY65991680;     mkmInagKoggncrGnTlOYY65991680 = mkmInagKoggncrGnTlOYY21266216;     mkmInagKoggncrGnTlOYY21266216 = mkmInagKoggncrGnTlOYY69071225;     mkmInagKoggncrGnTlOYY69071225 = mkmInagKoggncrGnTlOYY40811204;     mkmInagKoggncrGnTlOYY40811204 = mkmInagKoggncrGnTlOYY76878145;     mkmInagKoggncrGnTlOYY76878145 = mkmInagKoggncrGnTlOYY66600536;     mkmInagKoggncrGnTlOYY66600536 = mkmInagKoggncrGnTlOYY51563294;     mkmInagKoggncrGnTlOYY51563294 = mkmInagKoggncrGnTlOYY91924471;     mkmInagKoggncrGnTlOYY91924471 = mkmInagKoggncrGnTlOYY30275795;     mkmInagKoggncrGnTlOYY30275795 = mkmInagKoggncrGnTlOYY55203541;     mkmInagKoggncrGnTlOYY55203541 = mkmInagKoggncrGnTlOYY43718156;     mkmInagKoggncrGnTlOYY43718156 = mkmInagKoggncrGnTlOYY15890666;     mkmInagKoggncrGnTlOYY15890666 = mkmInagKoggncrGnTlOYY71418558;     mkmInagKoggncrGnTlOYY71418558 = mkmInagKoggncrGnTlOYY81780333;     mkmInagKoggncrGnTlOYY81780333 = mkmInagKoggncrGnTlOYY26807936;     mkmInagKoggncrGnTlOYY26807936 = mkmInagKoggncrGnTlOYY89615156;     mkmInagKoggncrGnTlOYY89615156 = mkmInagKoggncrGnTlOYY93035728;     mkmInagKoggncrGnTlOYY93035728 = mkmInagKoggncrGnTlOYY68002585;     mkmInagKoggncrGnTlOYY68002585 = mkmInagKoggncrGnTlOYY38124434;     mkmInagKoggncrGnTlOYY38124434 = mkmInagKoggncrGnTlOYY90583497;     mkmInagKoggncrGnTlOYY90583497 = mkmInagKoggncrGnTlOYY54067816;     mkmInagKoggncrGnTlOYY54067816 = mkmInagKoggncrGnTlOYY24836535;     mkmInagKoggncrGnTlOYY24836535 = mkmInagKoggncrGnTlOYY22295519;     mkmInagKoggncrGnTlOYY22295519 = mkmInagKoggncrGnTlOYY30721768;     mkmInagKoggncrGnTlOYY30721768 = mkmInagKoggncrGnTlOYY84482739;     mkmInagKoggncrGnTlOYY84482739 = mkmInagKoggncrGnTlOYY16502463;     mkmInagKoggncrGnTlOYY16502463 = mkmInagKoggncrGnTlOYY82640425;}
// Junk Finished
