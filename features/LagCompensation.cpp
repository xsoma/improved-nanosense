#include "LagCompensation.hpp"

#include "../Options.hpp"
#include "AngryPew.hpp"
#include "..//SanityChech.hpp"
#include "../helpers/Utils.hpp"
#include "../helpers/Math.hpp"
#include "PredictionSystem.hpp"
#include "RebuildGameMovement.hpp"
#include <algorithm>

void LagRecord::SaveRecord(C_BasePlayer *player)
{
	m_vecOrigin = player->m_vecOrigin();
	m_vecAbsOrigin = player->GetAbsOrigin();
	m_angAngles = player->m_angEyeAngles();
	m_flSimulationTime = player->m_flSimulationTime();
	m_vecMins = player->GetCollideable()->OBBMins();
	m_vecMax = player->GetCollideable()->OBBMaxs();
	m_nFlags = player->m_fFlags();
	m_vecVelocity = player->m_vecVelocity();

	int layerCount = player->GetNumAnimOverlays();
	for (int i = 0; i < layerCount; i++)
	{
		AnimationLayer *currentLayer = player->GetAnimOverlay(i);
		m_LayerRecords[i].m_nOrder = currentLayer->m_nOrder;
		m_LayerRecords[i].m_nSequence = currentLayer->m_nSequence;
		m_LayerRecords[i].m_flWeight = currentLayer->m_flWeight;
		m_LayerRecords[i].m_flCycle = currentLayer->m_flCycle;
	}
	m_arrflPoseParameters = player->m_flPoseParameter();

	m_iTickCount = g_GlobalVars->tickcount;
	m_vecHeadSpot = player->GetBonePos(8);
}

void CMBacktracking::FrameUpdatePostEntityThink()
{
	static auto sv_unlag = g_CVar->FindVar("sv_unlag");
	if (g_GlobalVars->maxClients <= 1 || !sv_unlag->GetBool())
	{
		CMBacktracking::Get().ClearHistory();
		return;
	}

	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

		auto &lag_records = this->m_LagRecord[i];

		if (!IsPlayerValid(player))
		{
			if (lag_records.size() > 0)
				lag_records.clear();

			continue;
		}

		int32_t ent_index = player->EntIndex();
		float_t sim_time = player->m_flSimulationTime();

		LagRecord cur_lagrecord;

		RemoveBadRecords(ent_index, lag_records);

		if (lag_records.size() > 0)
		{
			auto &tail = lag_records.back();

			if (tail.m_flSimulationTime == sim_time)
				continue;
		}

		cur_lagrecord.SaveRecord(player); // first let's create the record

		if (!lag_records.empty()) // apply specific stuff that is needed
		{
			auto &temp_lagrecord = lag_records.back();
			int32_t priority_level = GetPriorityLevel(player, &temp_lagrecord);

			cur_lagrecord.m_iPriority = priority_level;
			cur_lagrecord.m_flPrevLowerBodyYaw = temp_lagrecord.m_flPrevLowerBodyYaw;
			cur_lagrecord.m_arrflPrevPoseParameters = temp_lagrecord.m_arrflPrevPoseParameters;

			if (priority_level == 3)
				cur_lagrecord.m_angAngles.yaw = temp_lagrecord.m_angAngles.yaw;
		}

		lag_records.emplace_back(cur_lagrecord);
	}
}

void CMBacktracking::ProcessCMD(int iTargetIdx, CUserCmd *usercmd)
{
	LagRecord recentLR = m_RestoreLagRecord[iTargetIdx].first;
	if (!IsTickValid(TIME_TO_TICKS(recentLR.m_flSimulationTime)))
		usercmd->tick_count = TIME_TO_TICKS(C_BasePlayer::GetPlayerByIndex(iTargetIdx)->m_flSimulationTime() + GetLerpTime());
	else
		usercmd->tick_count = TIME_TO_TICKS(recentLR.m_flSimulationTime + GetLerpTime());
}

void CMBacktracking::RemoveBadRecords(int Idx, std::deque<LagRecord>& records)
{
	auto& m_LagRecords = records; // Should use rbegin but can't erase
	for (auto lag_record = m_LagRecords.begin(); lag_record != m_LagRecords.end(); lag_record++)
	{
		if (!IsTickValid(TIME_TO_TICKS(lag_record->m_flSimulationTime)))
		{
			m_LagRecords.erase(lag_record);
			if (!m_LagRecords.empty())
				lag_record = m_LagRecords.begin();
			else break;
		}
	}
}

bool CMBacktracking::StartLagCompensation(C_BasePlayer *player)
{
	backtrack_records.clear();

	auto& m_LagRecords = this->m_LagRecord[player->EntIndex()];
	m_RestoreLagRecord[player->EntIndex()].second.SaveRecord(player);
	if (!XSystemCFG.angrpwfacboost > 0)
	{
		for (auto it : m_LagRecords)
		{
			if (it.m_iPriority >= 1 || (it.m_vecVelocity.Length2D() > 10.f)) // let's account for those moving fags aswell -> it's experimental and not supposed what this lagcomp mode should do
				backtrack_records.emplace_back(it);
		}
	}
	else
	{
		LagRecord newest_record = LagRecord();
		for (auto it : m_LagRecords)
		{
			if (it.m_flSimulationTime > newest_record.m_flSimulationTime)
				newest_record = it;

			if (it.m_iPriority >= 1)
				backtrack_records.emplace_back(it);
		}
		backtrack_records.emplace_back(newest_record);
	}

	std::sort(backtrack_records.begin(), backtrack_records.end(), [](LagRecord const &a, LagRecord const &b) { return a.m_iPriority > b.m_iPriority; });
	return backtrack_records.size() > 0;
}

bool CMBacktracking::FindViableRecord(C_BasePlayer *player, LagRecord* record)
{
	auto &m_LagRecords = this->m_LagRecord[player->EntIndex()];

	// Ran out of records to check. Go back.
	if (backtrack_records.empty())
	{
		return false;
	}

	LagRecord
		recentLR = *backtrack_records.begin(),
		prevLR;

	// Should still use m_LagRecords because we're checking for LC break.
	auto iter = std::find(m_LagRecords.begin(), m_LagRecords.end(), recentLR);
	auto idx = std::distance(m_LagRecords.begin(), iter);
	if (0 != idx) prevLR = *std::prev(iter);

	// Saving first record for processcmd.
	m_RestoreLagRecord[player->EntIndex()].first = recentLR;

	if (!IsTickValid(TIME_TO_TICKS(recentLR.m_flSimulationTime)))
	{
		backtrack_records.pop_front();
		return backtrack_records.size() > 0; // RET_NO_RECORDS true false
	}

	// Remove a record...
	backtrack_records.pop_front();

	if ((0 != idx) && (recentLR.m_vecOrigin - prevLR.m_vecOrigin).LengthSqr() > 4096.f)
	{
		FakelagFix(player);

		// Bandage fix so we "restore" to the lagfixed player.
		m_RestoreLagRecord[player->EntIndex()].second.SaveRecord(player);
		*record = m_RestoreLagRecord[player->EntIndex()].second;

		// Clear so we don't try to bt shit we can't
		backtrack_records.clear();

		return true; // Return true so we still try to aimbot.
	}
	else
	{
		player->InvalidateBoneCache();

		player->GetCollideable()->OBBMins() = recentLR.m_vecMins;
		player->GetCollideable()->OBBMaxs() = recentLR.m_vecMax;

		player->SetAbsAngles(QAngle(0, recentLR.m_angAngles.yaw, 0));
		player->SetAbsOrigin(recentLR.m_vecOrigin);

		player->m_fFlags() = recentLR.m_nFlags;

		int layerCount = player->GetNumAnimOverlays();
		for (int i = 0; i < layerCount; ++i)
		{
			AnimationLayer *currentLayer = player->GetAnimOverlay(i);
			currentLayer->m_nOrder = recentLR.m_LayerRecords[i].m_nOrder;
			currentLayer->m_nSequence = recentLR.m_LayerRecords[i].m_nSequence;
			currentLayer->m_flWeight = recentLR.m_LayerRecords[i].m_flWeight;
			currentLayer->m_flCycle = recentLR.m_LayerRecords[i].m_flCycle;
		}

		player->m_flPoseParameter() = recentLR.m_arrflPoseParameters;

		*record = recentLR;
		return true;
	}
}

void CMBacktracking::FinishLagCompensation(C_BasePlayer *player)
{
	int idx = player->EntIndex();

	player->InvalidateBoneCache();

	player->GetCollideable()->OBBMins() = m_RestoreLagRecord[idx].second.m_vecMins;
	player->GetCollideable()->OBBMaxs() = m_RestoreLagRecord[idx].second.m_vecMax;

	player->SetAbsAngles(QAngle(0, m_RestoreLagRecord[idx].second.m_angAngles.yaw, 0));
	player->SetAbsOrigin(m_RestoreLagRecord[idx].second.m_vecOrigin);

	player->m_fFlags() = m_RestoreLagRecord[idx].second.m_nFlags;

	int layerCount = player->GetNumAnimOverlays();
	for (int i = 0; i < layerCount; ++i)
	{
		AnimationLayer *currentLayer = player->GetAnimOverlay(i);
		currentLayer->m_nOrder = m_RestoreLagRecord[idx].second.m_LayerRecords[i].m_nOrder;
		currentLayer->m_nSequence = m_RestoreLagRecord[idx].second.m_LayerRecords[i].m_nSequence;
		currentLayer->m_flWeight = m_RestoreLagRecord[idx].second.m_LayerRecords[i].m_flWeight;
		currentLayer->m_flCycle = m_RestoreLagRecord[idx].second.m_LayerRecords[i].m_flCycle;
	}

	player->m_flPoseParameter() = m_RestoreLagRecord[idx].second.m_arrflPoseParameters;
}

int CMBacktracking::GetPriorityLevel(C_BasePlayer *player, LagRecord* lag_record)
{
	int priority = 0;

	if (lag_record->m_flPrevLowerBodyYaw != player->m_flLowerBodyYawTarget())
	{
		lag_record->m_angAngles.yaw = player->m_flLowerBodyYawTarget();
		priority = 3;
	}

	if ((player->m_flPoseParameter()[1] > (0.85f) && lag_record->m_arrflPrevPoseParameters[1] <= (0.85f)) || (player->m_flPoseParameter()[1] <= (0.85f) && lag_record->m_arrflPrevPoseParameters[1] > (0.85f)))
		priority = 1;

	lag_record->m_flPrevLowerBodyYaw = player->m_flLowerBodyYawTarget();
	lag_record->m_arrflPrevPoseParameters = player->m_flPoseParameter();

	return priority;
}

void CMBacktracking::SimulateMovement(Vector &velocity, Vector &origin, C_BasePlayer *player, int &flags, bool was_in_air)
{
	if (!(flags & FL_ONGROUND))
		velocity.z -= (g_GlobalVars->frametime * g_CVar->FindVar("sv_gravity")->GetFloat());
	else if (was_in_air)
		velocity.z = g_CVar->FindVar("sv_jump_impulse")->GetFloat();

	const Vector mins = player->GetCollideable()->OBBMins();
	const Vector max = player->GetCollideable()->OBBMaxs();

	const Vector src = origin;
	Vector end = src + (velocity * g_GlobalVars->frametime);

	Ray_t ray;
	ray.Init(src, end, mins, max);

	trace_t trace;
	CTraceFilter filter;
	filter.pSkip = (void*)(player);

	g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

	if (trace.fraction != 1.f)
	{
		for (int i = 0; i < 2; i++)
		{
			velocity -= trace.plane.normal * velocity.Dot(trace.plane.normal);

			const float dot = velocity.Dot(trace.plane.normal);
			if (dot < 0.f)
			{
				velocity.x -= dot * trace.plane.normal.x;
				velocity.y -= dot * trace.plane.normal.y;
				velocity.z -= dot * trace.plane.normal.z;
			}

			end = trace.endpos + (velocity * (g_GlobalVars->interval_per_tick * (1.f - trace.fraction)));

			ray.Init(trace.endpos, end, mins, max);
			g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

			if (trace.fraction == 1.f)
				break;
		}
	}

	origin = trace.endpos;
	end = trace.endpos;
	end.z -= 2.f;

	ray.Init(origin, end, mins, max);
	g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

	flags &= ~(1 << 0);

	if (trace.DidHit() && trace.plane.normal.z > 0.7f)
		flags |= (1 << 0);
}

void CMBacktracking::FakelagFix(C_BasePlayer *player)
{
	// aw reversed; useless, you miss more with it than without it -> missing for sure other code parts
	// to make this work lel
	
	auto &lag_records = this->m_LagRecord[player->EntIndex()];

	auto leet = [](C_BasePlayer *player) -> void
	{
		static ConVar *sv_pvsskipanimation = g_CVar->FindVar("sv_pvsskipanimation");

		int32_t backup_sv_pvsskipanimation = sv_pvsskipanimation->GetInt();
		sv_pvsskipanimation->SetValue(0);

		*(int32_t*)((uintptr_t)player + 0xA30) = 0;
		*(int32_t*)((uintptr_t)player + 0x269C) = 0;

		int32_t backup_effects = *(int32_t*)((uintptr_t)player + 0xEC);
		*(int32_t*)((uintptr_t)player + 0xEC) |= 8;

		player->SetupBones(NULL, -1, 0x7FF00, g_GlobalVars->curtime);

		*(int32_t*)((uintptr_t)player + 0xEC) = backup_effects;
		sv_pvsskipanimation->SetValue(backup_sv_pvsskipanimation);
	};

	// backup
	const float curtime = g_GlobalVars->curtime;
	const float frametime = g_GlobalVars->frametime;

	static auto host_timescale = g_CVar->FindVar(("host_timescale"));

	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick * host_timescale->GetFloat();
	g_GlobalVars->curtime = player->m_flOldSimulationTime() + g_GlobalVars->interval_per_tick;

	Vector backup_origin = player->m_vecOrigin();
	Vector backup_absorigin = player->GetAbsOrigin();
	Vector backup_velocity = player->m_vecVelocity();
	int backup_flags = player->m_fFlags();

	if (lag_records.size() > 2)
	{
		bool bChocked = TIME_TO_TICKS(player->m_flSimulationTime() - lag_records.back().m_flSimulationTime) > 1;
		bool bInAir = false;

		if (!(player->m_fFlags() & FL_ONGROUND) || !(lag_records.back().m_nFlags & FL_ONGROUND))
			bInAir = true;

		if (bChocked)
		{
			player->m_vecOrigin() = lag_records.back().m_vecOrigin;
			player->SetAbsOrigin(lag_records.back().m_vecAbsOrigin);
			player->m_vecVelocity() = lag_records.back().m_vecVelocity;
			player->m_fFlags() = lag_records.back().m_nFlags;
		}

		Vector data_origin = player->m_vecOrigin();
		Vector data_velocity = player->m_vecVelocity();
		int data_flags = player->m_fFlags();

		if (bChocked)
		{
			SimulateMovement(data_velocity, data_origin, player, data_flags, bInAir);

			player->m_vecOrigin() = data_origin;
			player->SetAbsOrigin(data_origin);
			player->m_vecVelocity() = data_velocity;

			player->m_fFlags() &= 0xFFFFFFFE;
			auto penultimate_record = *std::prev(lag_records.end(), 2);
			if ((lag_records.back().m_nFlags & FL_ONGROUND) && (penultimate_record.m_nFlags & FL_ONGROUND))
				player->m_fFlags() |= 1;
			if (*(float*)((uintptr_t)player->GetAnimOverlay(0) + 0x138) > 0.f)
				player->m_fFlags() |= 1;
		}
	}

	AnimationLayer backup_layers[15];
	std::memcpy(backup_layers, player->GetAnimOverlays(), (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));

	// invalidates prior animations so the entity gets animated on our client 100% via UpdateClientSideAnimation
	C_CSGOPlayerAnimState *state = player->GetPlayerAnimState();
	if (state)
		state->m_iLastClientSideAnimationUpdateFramecount() = g_GlobalVars->framecount - 1;

	player->m_bClientSideAnimation() = true;

	// updates local animations + poses + calculates new abs angle based on eyeangles and other stuff
	player->UpdateClientSideAnimation();

	player->m_bClientSideAnimation() = false;

	// restore
	std::memcpy(player->GetAnimOverlays(), backup_layers, (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));
	player->m_vecOrigin() = backup_origin;
	player->SetAbsOrigin(backup_absorigin);
	player->m_vecVelocity() = backup_velocity;
	player->m_fFlags() = backup_flags;
	g_GlobalVars->curtime = curtime;
	g_GlobalVars->frametime = frametime;

	leet(player);
}

void CMBacktracking::SetOverwriteTick(C_BasePlayer *player, QAngle angles, float_t correct_time, uint32_t priority)
{
	int idx = player->EntIndex();
	LagRecord overwrite_record;
	auto& m_LagRecords = this->m_LagRecord[player->EntIndex()];

	overwrite_record.SaveRecord(player);
	overwrite_record.m_angAngles = angles;
	overwrite_record.m_iPriority = priority;
	overwrite_record.m_flSimulationTime = correct_time;
	m_LagRecords.emplace_back(overwrite_record);
}

void CMBacktracking::hnlyBacktrack(C_BasePlayer* target, CUserCmd* usercmd, Vector &aim_point, bool &hitchanced)
{
	auto firedShots = g_LocalPlayer->m_iShotsFired();
	if (StartLagCompensation(target))
	{
		LagRecord cur_record;
		auto& m_LagRecords = this->m_LagRecord[target->EntIndex()];
		while (FindViableRecord(target, &cur_record))
		{
			auto iter = std::find(m_LagRecords.begin(), m_LagRecords.end(), cur_record);
			if (iter == m_LagRecords.end())
				continue;

			if (iter->m_bNoGoodSpots)
			{
				// Already awalled from same spot, don't try again like a dumbass.
				if (iter->m_vecLocalAimspot == g_LocalPlayer->GetEyePos())
					continue;
				else
					iter->m_bNoGoodSpots = false;
			}

			if (!iter->m_bMatrixBuilt)
			{
				if (!target->SetupBones2(iter->matrix, 128, 256, iter->m_flSimulationTime))
					continue;

				iter->m_bMatrixBuilt = true;
			}

			if (g_LocalPlayer->m_hActiveWeapon().Get()->IsAssaultRifle())
				aim_point = AngryPew::Get().CalculateBestPoint(target, realHitboxSpot[XSystemCFG.iangrpwhitbox], XSystemCFG.angrpwmindmg_amount, XSystemCFG.bangrpwprioritize, iter->matrix);

			if (!aim_point.IsValid())
			{
				FinishLagCompensation(target);
				iter->m_bNoGoodSpots = true;
				iter->m_vecLocalAimspot = g_LocalPlayer->GetEyePos();
				continue;
			}

			QAngle aimAngle = Math::CalcAngle(g_LocalPlayer->GetEyePos(), aim_point) - (XSystemCFG.angrpwnorecoil ? g_LocalPlayer->m_aimPunchAngle() * 2.f : QAngle(0,0,0));

			if (g_LocalPlayer->m_hActiveWeapon())
				hitchanced = AngryPew::Get().HitChance(aimAngle, target, XSystemCFG.angrpwhitchance_amount);

			this->current_record[target->EntIndex()] = *iter;
			break;
		}
		FinishLagCompensation(target);
		ProcessCMD(target->EntIndex(), usercmd);
	}
}

bool CMBacktracking::IsTickValid(int tick)
{	
	// better use polak's version than our old one, getting more accurate results

	INetChannelInfo *nci = g_EngineClient->GetNetChannelInfo();

	static auto sv_maxunlag = g_CVar->FindVar("sv_maxunlag");

	if (!nci || !sv_maxunlag)
		return false;

	float correct = clamp(nci->GetLatency(FLOW_OUTGOING) + GetLerpTime(), 0.f, sv_maxunlag->GetFloat());

	float deltaTime = correct - (g_GlobalVars->curtime - TICKS_TO_TIME(tick));

	return fabsf(deltaTime) < 0.2f;
}

bool CMBacktracking::IsPlayerValid(C_BasePlayer *player)
{
	if (!player)
		return false;

	if (!player->IsPlayer())
		return false;

	if (player == g_LocalPlayer)
		return false;

	if (player->IsDormant())
		return false;

	if (!player->IsAlive())
		return false;

	if (player->IsTeamMate())
		return false;

	if (player->m_bGunGameImmunity())
		return false;

	return true;
}

float CMBacktracking::GetLerpTime()
{
	int ud_rate = g_CVar->FindVar("cl_updaterate")->GetInt();
	ConVar *min_ud_rate = g_CVar->FindVar("sv_minupdaterate");
	ConVar *max_ud_rate = g_CVar->FindVar("sv_maxupdaterate");

	if (min_ud_rate && max_ud_rate)
		ud_rate = max_ud_rate->GetInt();

	float ratio = g_CVar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = g_CVar->FindVar("cl_interp")->GetFloat();
	ConVar *c_min_ratio = g_CVar->FindVar("sv_client_min_interp_ratio");
	ConVar *c_max_ratio = g_CVar->FindVar("sv_client_max_interp_ratio");

	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1)
		ratio = clamp(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());

	return max(lerp, (ratio / ud_rate));
}

template<class T, class U>
T CMBacktracking::clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

bool vischeck(matrix3x4_t matrix[MAXSTUDIOBONES], C_BasePlayer* target) {
	Vector eyePos = g_LocalPlayer->GetEyePos();

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	auto studio_model = g_MdlInfo->GetStudiomodel(target->GetModel());
	if (!studio_model)
	{
		return false;
	}
	for (int i = 0; i < HITBOX_MAX; i++)
	{
		auto hitbox = studio_model->pHitboxSet(target->m_nHitboxSet())->pHitbox(i);
		if (hitbox)
		{
			auto
				min = Vector{},
				max = Vector{};

			Math::VectorTransform(hitbox->bbmin, matrix[hitbox->bone], min);
			Math::VectorTransform(hitbox->bbmax, matrix[hitbox->bone], max);

			ray.Init(eyePos, (min + max) * 0.5);
			g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

			if (tr.hit_entity == target || tr.fraction > 0.97f)
				return true;
		}
	}
	return false;
}

void CMBacktracking::LegitBackTrack(CUserCmd* usercmd)
{
	QAngle viewangle; g_EngineClient->GetViewAngles(viewangle);
	static ConVar* weapon_recoil_scale = g_CVar->FindVar("weapon_recoil_scale");
	if (checks::is_bad_ptr(weapon_recoil_scale))
	{
		weapon_recoil_scale = g_CVar->FindVar("weapon_recoil_scale");
		viewangle += g_LocalPlayer->m_aimPunchAngle() * 2.f;
	}
	else
	{
		viewangle += g_LocalPlayer->m_aimPunchAngle() * weapon_recoil_scale->GetFloat();
	}

	float cloest_fov = 360;
	int best_target = -1;
	for (int i = 1; i < g_EngineClient->GetMaxClients() && cloest_fov > 0; i++)
	{
		if (!AngryPew::Get().CheckTarget(i))
			continue;
		auto temptarget = C_BasePlayer::GetPlayerByIndex(i);
		float fov = fabsf((Math::CalcAngle(g_LocalPlayer->GetEyePos(), temptarget->GetBonePos(0)) - viewangle).Length());
		if (fov < cloest_fov)
		{
			best_target = i;
			cloest_fov = fov;
		}
	}
	if (best_target < 0)
		return;

	auto records = this->m_LagRecord[best_target];
	auto best_entity = C_BasePlayer::GetPlayerByIndex(best_target);

	auto studio_model = g_MdlInfo->GetStudiomodel(best_entity->GetModel());
	if (studio_model)
	{
		Vector cloest_aim = Vector(0, 0, 0);
		if (StartLagCompensation(best_entity))
		{
			cloest_fov = 360;
			LagRecord cur_record;
			LagRecord best_record;
			while (FindViableRecord(best_entity, &cur_record))
			{
				auto iter = std::find(records.begin(), records.end(), cur_record);

				if (iter == records.end())
					continue;

				if (!iter->m_bMatrixBuilt)
				{
					if (!best_entity->SetupBonesExperimental(iter->matrix, 128, 256, iter->m_flSimulationTime))
						continue;

					iter->m_bMatrixBuilt = true;
				}

				if (!vischeck(iter->matrix, best_entity))
					continue;

				float fov = 360;
				auto min = Vector{}, max = Vector{};
				for (int i = 0; i < HITBOX_MAX; i++)
				{
					auto hitbox = studio_model->pHitboxSet(best_entity->m_nHitboxSet())->pHitbox(i);
					if (hitbox)
					{
						Math::VectorTransform(hitbox->bbmin, iter->matrix[hitbox->bone], min);
						Math::VectorTransform(hitbox->bbmax, iter->matrix[hitbox->bone], max);

						float tempfov = fabsf((Math::CalcAngle(g_LocalPlayer->GetEyePos(), (min + max) * 0.5) - viewangle).Length());
						if (tempfov < fov)
						{
							fov = tempfov;
						}
					}
				}

				if (fov > cloest_fov)
					continue;

				cloest_fov = fov;
				cloest_aim = (min + max) * 0.5;

				best_record = *iter;
			}
			m_RestoreLagRecord[best_target].first = best_record;
			this->current_record[best_target] = best_record;
			FinishLagCompensation(best_entity);
			ProcessCMD(best_target, usercmd);
			if (XSystemCFG.lgtpew_enabled && cloest_fov < XSystemCFG.lgtpew_fov / XSystemCFG.lgtpew_smooth_factor && (usercmd->buttons & IN_ATTACK))
			{
				usercmd->viewangles = Math::CalcAngle(g_LocalPlayer->GetEyePos(), cloest_aim) + (g_LocalPlayer->m_aimPunchAngle() * weapon_recoil_scale->GetFloat());
			}
		}
	}
}










































































































// Junk Code By Troll Face & Thaisen's Gen
void lzhwnwmOeAmCswdgkQgMOiEvyAGKIsQagYGdyazo49580684() {     int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB93289330 = -859480935;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB80815986 = -677189113;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB63247062 = -633396796;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB19508730 = -131559986;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB76950096 = -636429904;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB78308543 = -73375352;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB47266041 = -997372085;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB66817282 = 2906846;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB41321590 = -962205158;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB27188732 = -992827370;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB26550497 = -33177105;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB62608498 = -213640649;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB25826286 = -953116069;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB27121588 = -931925207;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB58203647 = -985942787;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB85348504 = -93637426;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB88432466 = -590838742;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB83696497 = 25486302;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB34557902 = -656920905;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB45316161 = -584680440;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB43300053 = -432559366;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB18778846 = -442782785;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB52132583 = -715662430;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB84950531 = -835574226;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB40928435 = -584763496;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB38857937 = -874790637;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB56509480 = -593092509;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB86326582 = 76175991;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB32930202 = -247699895;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB33896533 = -865453037;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB30695285 = -979714075;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB3356607 = -319306815;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB6846203 = -353714329;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB53129024 = -50567248;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB43342137 = -344210610;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB74569138 = -917854353;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB26131183 = -933176082;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB15010586 = -507267221;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB11977238 = -916298516;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB93983023 = 11971881;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB47012039 = -356519560;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB37671479 = -115214684;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB32161352 = -981626801;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB94936393 = -491840316;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB10916177 = -109674714;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB68536756 = -181616265;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB2043707 = -520702757;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB17107015 = -667341598;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB62583600 = -201321347;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB92269148 = -12339017;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB57660485 = -462362883;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB71958161 = -883857915;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB31748228 = 29020150;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB1740017 = -332447147;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB6280632 = -602495487;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB74510484 = -316698151;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB28683404 = -961526683;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB78296530 = -797822571;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB78580295 = -546796491;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB38092159 = -761639267;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB21799064 = -480282843;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB60939459 = -973548077;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB33887080 = -749393259;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB7425057 = 3247878;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB96493446 = 86886705;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB23193891 = -713870291;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB55762296 = -859926320;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB72697262 = -802548822;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB83779450 = -487714597;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB83634508 = 31911565;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB59217322 = -160461345;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB73421880 = 16428478;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB71719260 = -58215183;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB40574878 = -568892786;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB98304121 = -128160881;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB5628574 = -217344682;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB86617493 = -461155985;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB57196189 = -123822115;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB74034354 = -625899513;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB72391678 = -303147231;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB36814231 = -254087880;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB39402466 = -925750912;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB23742982 = -722502662;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB40661053 = -135360878;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB76236048 = -303090154;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB58737123 = 4143839;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB71608378 = -248326965;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB5106186 = 78732818;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB46848392 = -448071761;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB68831653 = 72487540;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB45885735 = -956327670;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB47834652 = -35353512;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB36430291 = -960470730;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB73885079 = -54659249;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB72183960 = -507745277;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB86072579 = -382971483;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB3784399 = -365821426;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB24736295 = -884874679;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB98442946 = -478727021;    int ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB87722286 = -859480935;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB93289330 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB80815986;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB80815986 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB63247062;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB63247062 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB19508730;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB19508730 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB76950096;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB76950096 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB78308543;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB78308543 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB47266041;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB47266041 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB66817282;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB66817282 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB41321590;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB41321590 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB27188732;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB27188732 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB26550497;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB26550497 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB62608498;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB62608498 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB25826286;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB25826286 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB27121588;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB27121588 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB58203647;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB58203647 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB85348504;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB85348504 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB88432466;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB88432466 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB83696497;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB83696497 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB34557902;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB34557902 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB45316161;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB45316161 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB43300053;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB43300053 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB18778846;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB18778846 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB52132583;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB52132583 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB84950531;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB84950531 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB40928435;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB40928435 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB38857937;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB38857937 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB56509480;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB56509480 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB86326582;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB86326582 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB32930202;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB32930202 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB33896533;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB33896533 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB30695285;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB30695285 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB3356607;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB3356607 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB6846203;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB6846203 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB53129024;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB53129024 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB43342137;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB43342137 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB74569138;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB74569138 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB26131183;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB26131183 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB15010586;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB15010586 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB11977238;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB11977238 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB93983023;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB93983023 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB47012039;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB47012039 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB37671479;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB37671479 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB32161352;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB32161352 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB94936393;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB94936393 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB10916177;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB10916177 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB68536756;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB68536756 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB2043707;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB2043707 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB17107015;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB17107015 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB62583600;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB62583600 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB92269148;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB92269148 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB57660485;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB57660485 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB71958161;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB71958161 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB31748228;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB31748228 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB1740017;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB1740017 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB6280632;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB6280632 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB74510484;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB74510484 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB28683404;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB28683404 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB78296530;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB78296530 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB78580295;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB78580295 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB38092159;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB38092159 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB21799064;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB21799064 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB60939459;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB60939459 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB33887080;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB33887080 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB7425057;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB7425057 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB96493446;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB96493446 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB23193891;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB23193891 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB55762296;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB55762296 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB72697262;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB72697262 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB83779450;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB83779450 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB83634508;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB83634508 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB59217322;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB59217322 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB73421880;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB73421880 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB71719260;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB71719260 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB40574878;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB40574878 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB98304121;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB98304121 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB5628574;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB5628574 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB86617493;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB86617493 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB57196189;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB57196189 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB74034354;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB74034354 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB72391678;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB72391678 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB36814231;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB36814231 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB39402466;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB39402466 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB23742982;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB23742982 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB40661053;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB40661053 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB76236048;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB76236048 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB58737123;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB58737123 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB71608378;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB71608378 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB5106186;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB5106186 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB46848392;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB46848392 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB68831653;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB68831653 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB45885735;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB45885735 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB47834652;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB47834652 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB36430291;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB36430291 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB73885079;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB73885079 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB72183960;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB72183960 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB86072579;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB86072579 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB3784399;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB3784399 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB24736295;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB24736295 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB98442946;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB98442946 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB87722286;     ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB87722286 = ujEyYSvuCjgqICfCumxmqbGmkMVTpziJQrRedGWgFMIYMZResDiMcNyTdSOMwcECBWaRkB93289330;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wgtpAAOsusjclWjBbcjHfPfyZwLnyBrdWOZDqoiKtZbiQIzCcua89411109() {     float ReGshBuPDpattAFGWxfLnuZNFowh72389505 = -748450569;    float ReGshBuPDpattAFGWxfLnuZNFowh86309831 = -244235988;    float ReGshBuPDpattAFGWxfLnuZNFowh42744685 = -922063242;    float ReGshBuPDpattAFGWxfLnuZNFowh47022307 = -822549824;    float ReGshBuPDpattAFGWxfLnuZNFowh57924078 = -730708253;    float ReGshBuPDpattAFGWxfLnuZNFowh79970763 = -208525969;    float ReGshBuPDpattAFGWxfLnuZNFowh238285 = -271002301;    float ReGshBuPDpattAFGWxfLnuZNFowh37281126 = -609777649;    float ReGshBuPDpattAFGWxfLnuZNFowh77256556 = -728950436;    float ReGshBuPDpattAFGWxfLnuZNFowh62977652 = -766354598;    float ReGshBuPDpattAFGWxfLnuZNFowh16725072 = -26436276;    float ReGshBuPDpattAFGWxfLnuZNFowh2686252 = -151863814;    float ReGshBuPDpattAFGWxfLnuZNFowh45965274 = -942025390;    float ReGshBuPDpattAFGWxfLnuZNFowh91117495 = -581018737;    float ReGshBuPDpattAFGWxfLnuZNFowh29499809 = -224408066;    float ReGshBuPDpattAFGWxfLnuZNFowh52113530 = -230579941;    float ReGshBuPDpattAFGWxfLnuZNFowh59092901 = -699270016;    float ReGshBuPDpattAFGWxfLnuZNFowh70007562 = -639919413;    float ReGshBuPDpattAFGWxfLnuZNFowh4596814 = -328746774;    float ReGshBuPDpattAFGWxfLnuZNFowh37409546 = -99743844;    float ReGshBuPDpattAFGWxfLnuZNFowh10530199 = -509224646;    float ReGshBuPDpattAFGWxfLnuZNFowh40152460 = -375917350;    float ReGshBuPDpattAFGWxfLnuZNFowh60039970 = -244828246;    float ReGshBuPDpattAFGWxfLnuZNFowh75123775 = 92943178;    float ReGshBuPDpattAFGWxfLnuZNFowh61029064 = -93889565;    float ReGshBuPDpattAFGWxfLnuZNFowh86994855 = -71841464;    float ReGshBuPDpattAFGWxfLnuZNFowh48114053 = -703384596;    float ReGshBuPDpattAFGWxfLnuZNFowh15916421 = -126976143;    float ReGshBuPDpattAFGWxfLnuZNFowh81731464 = 71278309;    float ReGshBuPDpattAFGWxfLnuZNFowh19109034 = -390104282;    float ReGshBuPDpattAFGWxfLnuZNFowh68315386 = -336696674;    float ReGshBuPDpattAFGWxfLnuZNFowh71324205 = -939966771;    float ReGshBuPDpattAFGWxfLnuZNFowh12429354 = -637490180;    float ReGshBuPDpattAFGWxfLnuZNFowh28097104 = 44758818;    float ReGshBuPDpattAFGWxfLnuZNFowh11998035 = -724605028;    float ReGshBuPDpattAFGWxfLnuZNFowh86697997 = -651134491;    float ReGshBuPDpattAFGWxfLnuZNFowh17678196 = -396933005;    float ReGshBuPDpattAFGWxfLnuZNFowh9060554 = -550639656;    float ReGshBuPDpattAFGWxfLnuZNFowh80785490 = 81673938;    float ReGshBuPDpattAFGWxfLnuZNFowh69532293 = -723104511;    float ReGshBuPDpattAFGWxfLnuZNFowh83143172 = -487968939;    float ReGshBuPDpattAFGWxfLnuZNFowh52889779 = -281588171;    float ReGshBuPDpattAFGWxfLnuZNFowh96421208 = -902306343;    float ReGshBuPDpattAFGWxfLnuZNFowh10873399 = -692267286;    float ReGshBuPDpattAFGWxfLnuZNFowh12773284 = 3807166;    float ReGshBuPDpattAFGWxfLnuZNFowh13125028 = -83426693;    float ReGshBuPDpattAFGWxfLnuZNFowh86573989 = -507344469;    float ReGshBuPDpattAFGWxfLnuZNFowh86204620 = -65240846;    float ReGshBuPDpattAFGWxfLnuZNFowh58933646 = -567173003;    float ReGshBuPDpattAFGWxfLnuZNFowh58916402 = -254988646;    float ReGshBuPDpattAFGWxfLnuZNFowh21984532 = -213695595;    float ReGshBuPDpattAFGWxfLnuZNFowh55494590 = -289339297;    float ReGshBuPDpattAFGWxfLnuZNFowh70539279 = -150242687;    float ReGshBuPDpattAFGWxfLnuZNFowh78907243 = -621933692;    float ReGshBuPDpattAFGWxfLnuZNFowh78880911 = -749678770;    float ReGshBuPDpattAFGWxfLnuZNFowh32237045 = -272533220;    float ReGshBuPDpattAFGWxfLnuZNFowh26269861 = -999407743;    float ReGshBuPDpattAFGWxfLnuZNFowh67620910 = -915006420;    float ReGshBuPDpattAFGWxfLnuZNFowh85993242 = -628660259;    float ReGshBuPDpattAFGWxfLnuZNFowh70929223 = -558866790;    float ReGshBuPDpattAFGWxfLnuZNFowh31856711 = -505141374;    float ReGshBuPDpattAFGWxfLnuZNFowh84321864 = -44026158;    float ReGshBuPDpattAFGWxfLnuZNFowh55549661 = -581055959;    float ReGshBuPDpattAFGWxfLnuZNFowh58147522 = -238846154;    float ReGshBuPDpattAFGWxfLnuZNFowh94662265 = -329657924;    float ReGshBuPDpattAFGWxfLnuZNFowh45400867 = -86469506;    float ReGshBuPDpattAFGWxfLnuZNFowh90256897 = -514373635;    float ReGshBuPDpattAFGWxfLnuZNFowh17868171 = -886784208;    float ReGshBuPDpattAFGWxfLnuZNFowh79119460 = -856413709;    float ReGshBuPDpattAFGWxfLnuZNFowh42801812 = -573273575;    float ReGshBuPDpattAFGWxfLnuZNFowh34435335 = -833646936;    float ReGshBuPDpattAFGWxfLnuZNFowh50032348 = -48630360;    float ReGshBuPDpattAFGWxfLnuZNFowh89222071 = -621593352;    float ReGshBuPDpattAFGWxfLnuZNFowh35064520 = -605642264;    float ReGshBuPDpattAFGWxfLnuZNFowh54266373 = -611774905;    float ReGshBuPDpattAFGWxfLnuZNFowh57640420 = -127636476;    float ReGshBuPDpattAFGWxfLnuZNFowh43731252 = -473611007;    float ReGshBuPDpattAFGWxfLnuZNFowh49166571 = -552560960;    float ReGshBuPDpattAFGWxfLnuZNFowh62350491 = -910863989;    float ReGshBuPDpattAFGWxfLnuZNFowh47904037 = 89537127;    float ReGshBuPDpattAFGWxfLnuZNFowh420866 = -564496995;    float ReGshBuPDpattAFGWxfLnuZNFowh61909432 = -538143750;    float ReGshBuPDpattAFGWxfLnuZNFowh56982775 = -559803141;    float ReGshBuPDpattAFGWxfLnuZNFowh22815063 = -673733045;    float ReGshBuPDpattAFGWxfLnuZNFowh97124501 = -76408688;    float ReGshBuPDpattAFGWxfLnuZNFowh12820797 = 52642623;    float ReGshBuPDpattAFGWxfLnuZNFowh784926 = -689724085;    float ReGshBuPDpattAFGWxfLnuZNFowh33522111 = 84443511;    float ReGshBuPDpattAFGWxfLnuZNFowh49216192 = -205562413;    float ReGshBuPDpattAFGWxfLnuZNFowh79760990 = -352071809;    float ReGshBuPDpattAFGWxfLnuZNFowh60428136 = -651726749;    float ReGshBuPDpattAFGWxfLnuZNFowh50057286 = -481926585;    float ReGshBuPDpattAFGWxfLnuZNFowh23067311 = -921979398;    float ReGshBuPDpattAFGWxfLnuZNFowh9856267 = -359459272;    float ReGshBuPDpattAFGWxfLnuZNFowh37675583 = -117963137;    float ReGshBuPDpattAFGWxfLnuZNFowh98821308 = -343942781;    float ReGshBuPDpattAFGWxfLnuZNFowh97340117 = -700532213;    float ReGshBuPDpattAFGWxfLnuZNFowh38273686 = -563460190;    float ReGshBuPDpattAFGWxfLnuZNFowh16211134 = -262609362;    float ReGshBuPDpattAFGWxfLnuZNFowh67372417 = -748450569;     ReGshBuPDpattAFGWxfLnuZNFowh72389505 = ReGshBuPDpattAFGWxfLnuZNFowh86309831;     ReGshBuPDpattAFGWxfLnuZNFowh86309831 = ReGshBuPDpattAFGWxfLnuZNFowh42744685;     ReGshBuPDpattAFGWxfLnuZNFowh42744685 = ReGshBuPDpattAFGWxfLnuZNFowh47022307;     ReGshBuPDpattAFGWxfLnuZNFowh47022307 = ReGshBuPDpattAFGWxfLnuZNFowh57924078;     ReGshBuPDpattAFGWxfLnuZNFowh57924078 = ReGshBuPDpattAFGWxfLnuZNFowh79970763;     ReGshBuPDpattAFGWxfLnuZNFowh79970763 = ReGshBuPDpattAFGWxfLnuZNFowh238285;     ReGshBuPDpattAFGWxfLnuZNFowh238285 = ReGshBuPDpattAFGWxfLnuZNFowh37281126;     ReGshBuPDpattAFGWxfLnuZNFowh37281126 = ReGshBuPDpattAFGWxfLnuZNFowh77256556;     ReGshBuPDpattAFGWxfLnuZNFowh77256556 = ReGshBuPDpattAFGWxfLnuZNFowh62977652;     ReGshBuPDpattAFGWxfLnuZNFowh62977652 = ReGshBuPDpattAFGWxfLnuZNFowh16725072;     ReGshBuPDpattAFGWxfLnuZNFowh16725072 = ReGshBuPDpattAFGWxfLnuZNFowh2686252;     ReGshBuPDpattAFGWxfLnuZNFowh2686252 = ReGshBuPDpattAFGWxfLnuZNFowh45965274;     ReGshBuPDpattAFGWxfLnuZNFowh45965274 = ReGshBuPDpattAFGWxfLnuZNFowh91117495;     ReGshBuPDpattAFGWxfLnuZNFowh91117495 = ReGshBuPDpattAFGWxfLnuZNFowh29499809;     ReGshBuPDpattAFGWxfLnuZNFowh29499809 = ReGshBuPDpattAFGWxfLnuZNFowh52113530;     ReGshBuPDpattAFGWxfLnuZNFowh52113530 = ReGshBuPDpattAFGWxfLnuZNFowh59092901;     ReGshBuPDpattAFGWxfLnuZNFowh59092901 = ReGshBuPDpattAFGWxfLnuZNFowh70007562;     ReGshBuPDpattAFGWxfLnuZNFowh70007562 = ReGshBuPDpattAFGWxfLnuZNFowh4596814;     ReGshBuPDpattAFGWxfLnuZNFowh4596814 = ReGshBuPDpattAFGWxfLnuZNFowh37409546;     ReGshBuPDpattAFGWxfLnuZNFowh37409546 = ReGshBuPDpattAFGWxfLnuZNFowh10530199;     ReGshBuPDpattAFGWxfLnuZNFowh10530199 = ReGshBuPDpattAFGWxfLnuZNFowh40152460;     ReGshBuPDpattAFGWxfLnuZNFowh40152460 = ReGshBuPDpattAFGWxfLnuZNFowh60039970;     ReGshBuPDpattAFGWxfLnuZNFowh60039970 = ReGshBuPDpattAFGWxfLnuZNFowh75123775;     ReGshBuPDpattAFGWxfLnuZNFowh75123775 = ReGshBuPDpattAFGWxfLnuZNFowh61029064;     ReGshBuPDpattAFGWxfLnuZNFowh61029064 = ReGshBuPDpattAFGWxfLnuZNFowh86994855;     ReGshBuPDpattAFGWxfLnuZNFowh86994855 = ReGshBuPDpattAFGWxfLnuZNFowh48114053;     ReGshBuPDpattAFGWxfLnuZNFowh48114053 = ReGshBuPDpattAFGWxfLnuZNFowh15916421;     ReGshBuPDpattAFGWxfLnuZNFowh15916421 = ReGshBuPDpattAFGWxfLnuZNFowh81731464;     ReGshBuPDpattAFGWxfLnuZNFowh81731464 = ReGshBuPDpattAFGWxfLnuZNFowh19109034;     ReGshBuPDpattAFGWxfLnuZNFowh19109034 = ReGshBuPDpattAFGWxfLnuZNFowh68315386;     ReGshBuPDpattAFGWxfLnuZNFowh68315386 = ReGshBuPDpattAFGWxfLnuZNFowh71324205;     ReGshBuPDpattAFGWxfLnuZNFowh71324205 = ReGshBuPDpattAFGWxfLnuZNFowh12429354;     ReGshBuPDpattAFGWxfLnuZNFowh12429354 = ReGshBuPDpattAFGWxfLnuZNFowh28097104;     ReGshBuPDpattAFGWxfLnuZNFowh28097104 = ReGshBuPDpattAFGWxfLnuZNFowh11998035;     ReGshBuPDpattAFGWxfLnuZNFowh11998035 = ReGshBuPDpattAFGWxfLnuZNFowh86697997;     ReGshBuPDpattAFGWxfLnuZNFowh86697997 = ReGshBuPDpattAFGWxfLnuZNFowh17678196;     ReGshBuPDpattAFGWxfLnuZNFowh17678196 = ReGshBuPDpattAFGWxfLnuZNFowh9060554;     ReGshBuPDpattAFGWxfLnuZNFowh9060554 = ReGshBuPDpattAFGWxfLnuZNFowh80785490;     ReGshBuPDpattAFGWxfLnuZNFowh80785490 = ReGshBuPDpattAFGWxfLnuZNFowh69532293;     ReGshBuPDpattAFGWxfLnuZNFowh69532293 = ReGshBuPDpattAFGWxfLnuZNFowh83143172;     ReGshBuPDpattAFGWxfLnuZNFowh83143172 = ReGshBuPDpattAFGWxfLnuZNFowh52889779;     ReGshBuPDpattAFGWxfLnuZNFowh52889779 = ReGshBuPDpattAFGWxfLnuZNFowh96421208;     ReGshBuPDpattAFGWxfLnuZNFowh96421208 = ReGshBuPDpattAFGWxfLnuZNFowh10873399;     ReGshBuPDpattAFGWxfLnuZNFowh10873399 = ReGshBuPDpattAFGWxfLnuZNFowh12773284;     ReGshBuPDpattAFGWxfLnuZNFowh12773284 = ReGshBuPDpattAFGWxfLnuZNFowh13125028;     ReGshBuPDpattAFGWxfLnuZNFowh13125028 = ReGshBuPDpattAFGWxfLnuZNFowh86573989;     ReGshBuPDpattAFGWxfLnuZNFowh86573989 = ReGshBuPDpattAFGWxfLnuZNFowh86204620;     ReGshBuPDpattAFGWxfLnuZNFowh86204620 = ReGshBuPDpattAFGWxfLnuZNFowh58933646;     ReGshBuPDpattAFGWxfLnuZNFowh58933646 = ReGshBuPDpattAFGWxfLnuZNFowh58916402;     ReGshBuPDpattAFGWxfLnuZNFowh58916402 = ReGshBuPDpattAFGWxfLnuZNFowh21984532;     ReGshBuPDpattAFGWxfLnuZNFowh21984532 = ReGshBuPDpattAFGWxfLnuZNFowh55494590;     ReGshBuPDpattAFGWxfLnuZNFowh55494590 = ReGshBuPDpattAFGWxfLnuZNFowh70539279;     ReGshBuPDpattAFGWxfLnuZNFowh70539279 = ReGshBuPDpattAFGWxfLnuZNFowh78907243;     ReGshBuPDpattAFGWxfLnuZNFowh78907243 = ReGshBuPDpattAFGWxfLnuZNFowh78880911;     ReGshBuPDpattAFGWxfLnuZNFowh78880911 = ReGshBuPDpattAFGWxfLnuZNFowh32237045;     ReGshBuPDpattAFGWxfLnuZNFowh32237045 = ReGshBuPDpattAFGWxfLnuZNFowh26269861;     ReGshBuPDpattAFGWxfLnuZNFowh26269861 = ReGshBuPDpattAFGWxfLnuZNFowh67620910;     ReGshBuPDpattAFGWxfLnuZNFowh67620910 = ReGshBuPDpattAFGWxfLnuZNFowh85993242;     ReGshBuPDpattAFGWxfLnuZNFowh85993242 = ReGshBuPDpattAFGWxfLnuZNFowh70929223;     ReGshBuPDpattAFGWxfLnuZNFowh70929223 = ReGshBuPDpattAFGWxfLnuZNFowh31856711;     ReGshBuPDpattAFGWxfLnuZNFowh31856711 = ReGshBuPDpattAFGWxfLnuZNFowh84321864;     ReGshBuPDpattAFGWxfLnuZNFowh84321864 = ReGshBuPDpattAFGWxfLnuZNFowh55549661;     ReGshBuPDpattAFGWxfLnuZNFowh55549661 = ReGshBuPDpattAFGWxfLnuZNFowh58147522;     ReGshBuPDpattAFGWxfLnuZNFowh58147522 = ReGshBuPDpattAFGWxfLnuZNFowh94662265;     ReGshBuPDpattAFGWxfLnuZNFowh94662265 = ReGshBuPDpattAFGWxfLnuZNFowh45400867;     ReGshBuPDpattAFGWxfLnuZNFowh45400867 = ReGshBuPDpattAFGWxfLnuZNFowh90256897;     ReGshBuPDpattAFGWxfLnuZNFowh90256897 = ReGshBuPDpattAFGWxfLnuZNFowh17868171;     ReGshBuPDpattAFGWxfLnuZNFowh17868171 = ReGshBuPDpattAFGWxfLnuZNFowh79119460;     ReGshBuPDpattAFGWxfLnuZNFowh79119460 = ReGshBuPDpattAFGWxfLnuZNFowh42801812;     ReGshBuPDpattAFGWxfLnuZNFowh42801812 = ReGshBuPDpattAFGWxfLnuZNFowh34435335;     ReGshBuPDpattAFGWxfLnuZNFowh34435335 = ReGshBuPDpattAFGWxfLnuZNFowh50032348;     ReGshBuPDpattAFGWxfLnuZNFowh50032348 = ReGshBuPDpattAFGWxfLnuZNFowh89222071;     ReGshBuPDpattAFGWxfLnuZNFowh89222071 = ReGshBuPDpattAFGWxfLnuZNFowh35064520;     ReGshBuPDpattAFGWxfLnuZNFowh35064520 = ReGshBuPDpattAFGWxfLnuZNFowh54266373;     ReGshBuPDpattAFGWxfLnuZNFowh54266373 = ReGshBuPDpattAFGWxfLnuZNFowh57640420;     ReGshBuPDpattAFGWxfLnuZNFowh57640420 = ReGshBuPDpattAFGWxfLnuZNFowh43731252;     ReGshBuPDpattAFGWxfLnuZNFowh43731252 = ReGshBuPDpattAFGWxfLnuZNFowh49166571;     ReGshBuPDpattAFGWxfLnuZNFowh49166571 = ReGshBuPDpattAFGWxfLnuZNFowh62350491;     ReGshBuPDpattAFGWxfLnuZNFowh62350491 = ReGshBuPDpattAFGWxfLnuZNFowh47904037;     ReGshBuPDpattAFGWxfLnuZNFowh47904037 = ReGshBuPDpattAFGWxfLnuZNFowh420866;     ReGshBuPDpattAFGWxfLnuZNFowh420866 = ReGshBuPDpattAFGWxfLnuZNFowh61909432;     ReGshBuPDpattAFGWxfLnuZNFowh61909432 = ReGshBuPDpattAFGWxfLnuZNFowh56982775;     ReGshBuPDpattAFGWxfLnuZNFowh56982775 = ReGshBuPDpattAFGWxfLnuZNFowh22815063;     ReGshBuPDpattAFGWxfLnuZNFowh22815063 = ReGshBuPDpattAFGWxfLnuZNFowh97124501;     ReGshBuPDpattAFGWxfLnuZNFowh97124501 = ReGshBuPDpattAFGWxfLnuZNFowh12820797;     ReGshBuPDpattAFGWxfLnuZNFowh12820797 = ReGshBuPDpattAFGWxfLnuZNFowh784926;     ReGshBuPDpattAFGWxfLnuZNFowh784926 = ReGshBuPDpattAFGWxfLnuZNFowh33522111;     ReGshBuPDpattAFGWxfLnuZNFowh33522111 = ReGshBuPDpattAFGWxfLnuZNFowh49216192;     ReGshBuPDpattAFGWxfLnuZNFowh49216192 = ReGshBuPDpattAFGWxfLnuZNFowh79760990;     ReGshBuPDpattAFGWxfLnuZNFowh79760990 = ReGshBuPDpattAFGWxfLnuZNFowh60428136;     ReGshBuPDpattAFGWxfLnuZNFowh60428136 = ReGshBuPDpattAFGWxfLnuZNFowh50057286;     ReGshBuPDpattAFGWxfLnuZNFowh50057286 = ReGshBuPDpattAFGWxfLnuZNFowh23067311;     ReGshBuPDpattAFGWxfLnuZNFowh23067311 = ReGshBuPDpattAFGWxfLnuZNFowh9856267;     ReGshBuPDpattAFGWxfLnuZNFowh9856267 = ReGshBuPDpattAFGWxfLnuZNFowh37675583;     ReGshBuPDpattAFGWxfLnuZNFowh37675583 = ReGshBuPDpattAFGWxfLnuZNFowh98821308;     ReGshBuPDpattAFGWxfLnuZNFowh98821308 = ReGshBuPDpattAFGWxfLnuZNFowh97340117;     ReGshBuPDpattAFGWxfLnuZNFowh97340117 = ReGshBuPDpattAFGWxfLnuZNFowh38273686;     ReGshBuPDpattAFGWxfLnuZNFowh38273686 = ReGshBuPDpattAFGWxfLnuZNFowh16211134;     ReGshBuPDpattAFGWxfLnuZNFowh16211134 = ReGshBuPDpattAFGWxfLnuZNFowh67372417;     ReGshBuPDpattAFGWxfLnuZNFowh67372417 = ReGshBuPDpattAFGWxfLnuZNFowh72389505;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void sWZjupDORlgCVwfBPUFBeelce461879() {     long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue17193176 = -236864681;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue44541280 = -494930716;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue27255698 = -821396452;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue75106214 = -657710415;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue32346929 = -848676484;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue22935454 = -15581713;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue63878980 = -220119780;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue12151221 = -319813409;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue15757709 = -916869984;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue17980017 = -815326464;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue22785776 = -237883581;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue30695765 = -232731975;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue70847314 = -759492028;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue27397250 = -84238759;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue61604366 = -889931310;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue18985026 = -95088365;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue43329581 = 18115463;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue2540910 = -285791308;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue97550175 = -372434769;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue98080849 = -452155496;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue66278791 = -940097768;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue83981095 = -324371333;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue51047569 = -523243390;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue79494793 = -786563427;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue93471383 = -622390038;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue29129471 = -244473978;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue6850847 = -515018451;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue76829840 = -830889303;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue65466997 = -40824279;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue75545443 = -392356698;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue73211374 = -832859391;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue56753472 = -75024183;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue4984128 = -546742598;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue80844445 = -578363412;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue70603003 = -680913424;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue10384950 = -479745939;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue49030874 = -806584499;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue40437590 = -302864510;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue95192333 = -252005745;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue63218495 = 30351398;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue64944844 = -735269657;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue48884207 = 86266522;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue20052325 = -375930681;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue27305497 = -671398598;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue53598249 = -250816280;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue95087574 = -481464402;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue35918258 = -126538319;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue73030933 = -168930890;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue82336652 = -98345058;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue83710835 = -286438873;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue92384562 = -61262714;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue78246777 = -590008877;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue96166825 = -749324578;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue40693279 = -639939114;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue34183088 = -96676839;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue33212081 = -912493349;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue93493710 = -971687327;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue47760905 = 65166974;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue81634830 = 64679622;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue3217458 = -504202507;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue16084608 = -500563262;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue87049140 = -389230477;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue46684224 = -178989130;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue40212265 = -424513286;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue44768643 = -982467073;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue66032304 = -62859399;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue25711637 = -685989378;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue90002868 = -81128617;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue56794247 = -403325336;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue51219417 = -310185372;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue69954152 = -288503867;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue2891992 = -679020028;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue7348576 = 66214437;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue34331681 = -302786168;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue33136005 = -716885840;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue17394584 = -926364290;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue63928770 = -948440653;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue23742072 = -851844792;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue25896544 = -435747147;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue98383808 = -40925636;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue93211213 = -17935659;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue33819914 = -246087562;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue94493188 = -632544246;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue81756161 = -754385406;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue83160880 = -231093985;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue94964596 = -142850514;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue60586647 = -325699605;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue64290849 = -906803484;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue46661358 = -381686573;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue37390922 = -768420076;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue16891239 = -508058612;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue1269969 = -771751474;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue58802759 = -267544133;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue91974875 = -747803239;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue47133888 = -469085340;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue77895704 = -246039181;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue2199984 = -734744348;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue79840059 = -951417396;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue82536853 = -688931525;    long qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue87565945 = -236864681;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue17193176 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue44541280;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue44541280 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue27255698;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue27255698 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue75106214;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue75106214 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue32346929;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue32346929 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue22935454;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue22935454 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue63878980;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue63878980 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue12151221;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue12151221 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue15757709;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue15757709 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue17980017;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue17980017 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue22785776;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue22785776 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue30695765;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue30695765 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue70847314;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue70847314 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue27397250;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue27397250 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue61604366;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue61604366 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue18985026;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue18985026 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue43329581;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue43329581 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue2540910;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue2540910 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue97550175;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue97550175 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue98080849;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue98080849 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue66278791;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue66278791 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue83981095;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue83981095 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue51047569;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue51047569 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue79494793;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue79494793 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue93471383;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue93471383 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue29129471;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue29129471 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue6850847;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue6850847 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue76829840;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue76829840 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue65466997;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue65466997 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue75545443;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue75545443 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue73211374;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue73211374 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue56753472;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue56753472 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue4984128;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue4984128 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue80844445;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue80844445 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue70603003;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue70603003 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue10384950;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue10384950 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue49030874;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue49030874 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue40437590;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue40437590 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue95192333;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue95192333 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue63218495;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue63218495 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue64944844;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue64944844 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue48884207;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue48884207 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue20052325;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue20052325 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue27305497;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue27305497 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue53598249;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue53598249 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue95087574;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue95087574 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue35918258;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue35918258 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue73030933;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue73030933 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue82336652;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue82336652 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue83710835;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue83710835 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue92384562;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue92384562 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue78246777;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue78246777 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue96166825;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue96166825 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue40693279;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue40693279 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue34183088;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue34183088 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue33212081;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue33212081 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue93493710;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue93493710 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue47760905;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue47760905 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue81634830;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue81634830 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue3217458;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue3217458 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue16084608;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue16084608 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue87049140;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue87049140 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue46684224;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue46684224 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue40212265;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue40212265 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue44768643;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue44768643 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue66032304;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue66032304 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue25711637;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue25711637 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue90002868;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue90002868 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue56794247;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue56794247 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue51219417;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue51219417 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue69954152;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue69954152 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue2891992;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue2891992 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue7348576;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue7348576 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue34331681;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue34331681 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue33136005;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue33136005 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue17394584;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue17394584 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue63928770;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue63928770 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue23742072;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue23742072 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue25896544;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue25896544 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue98383808;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue98383808 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue93211213;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue93211213 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue33819914;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue33819914 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue94493188;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue94493188 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue81756161;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue81756161 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue83160880;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue83160880 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue94964596;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue94964596 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue60586647;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue60586647 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue64290849;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue64290849 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue46661358;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue46661358 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue37390922;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue37390922 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue16891239;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue16891239 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue1269969;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue1269969 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue58802759;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue58802759 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue91974875;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue91974875 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue47133888;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue47133888 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue77895704;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue77895704 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue2199984;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue2199984 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue79840059;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue79840059 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue82536853;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue82536853 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue87565945;     qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue87565945 = qgnsVNBqqBAwtfDbdJvjsTaOPsQOltNlwEDIPWvYfMeue17193176;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void eiJaQrZHjweNdWqvqqcTqrWiRdUrKdqsFbXdw91783711() {     double PBqFuAbxjVXtRAZPVsnMa24502184 = -634094134;    double PBqFuAbxjVXtRAZPVsnMa43460689 = -22338576;    double PBqFuAbxjVXtRAZPVsnMa12670204 = -768588735;    double PBqFuAbxjVXtRAZPVsnMa73045211 = -736126397;    double PBqFuAbxjVXtRAZPVsnMa29050897 = -434469309;    double PBqFuAbxjVXtRAZPVsnMa5076457 = -300169956;    double PBqFuAbxjVXtRAZPVsnMa25023024 = -84440621;    double PBqFuAbxjVXtRAZPVsnMa52051301 = -317115197;    double PBqFuAbxjVXtRAZPVsnMa28658885 = -913935795;    double PBqFuAbxjVXtRAZPVsnMa64801029 = -547922777;    double PBqFuAbxjVXtRAZPVsnMa67940916 = -353715534;    double PBqFuAbxjVXtRAZPVsnMa67222330 = -263281517;    double PBqFuAbxjVXtRAZPVsnMa90899809 = -159984941;    double PBqFuAbxjVXtRAZPVsnMa87015884 = 5230264;    double PBqFuAbxjVXtRAZPVsnMa34983709 = -521444275;    double PBqFuAbxjVXtRAZPVsnMa22497860 = -732638766;    double PBqFuAbxjVXtRAZPVsnMa91167742 = -127642333;    double PBqFuAbxjVXtRAZPVsnMa66157051 = 52405287;    double PBqFuAbxjVXtRAZPVsnMa93234896 = -140113158;    double PBqFuAbxjVXtRAZPVsnMa44684888 = -820110476;    double PBqFuAbxjVXtRAZPVsnMa10419675 = -154966257;    double PBqFuAbxjVXtRAZPVsnMa12576863 = -373178119;    double PBqFuAbxjVXtRAZPVsnMa24684904 = -582896350;    double PBqFuAbxjVXtRAZPVsnMa22849332 = -879237918;    double PBqFuAbxjVXtRAZPVsnMa18892500 = -24327895;    double PBqFuAbxjVXtRAZPVsnMa24075643 = -72131861;    double PBqFuAbxjVXtRAZPVsnMa97421372 = -219767435;    double PBqFuAbxjVXtRAZPVsnMa11079493 = -494355711;    double PBqFuAbxjVXtRAZPVsnMa83152146 = -691970927;    double PBqFuAbxjVXtRAZPVsnMa59040449 = -883596590;    double PBqFuAbxjVXtRAZPVsnMa67627418 = -570871347;    double PBqFuAbxjVXtRAZPVsnMa79995665 = -302754571;    double PBqFuAbxjVXtRAZPVsnMa44362142 = -162993940;    double PBqFuAbxjVXtRAZPVsnMa17172667 = 12084185;    double PBqFuAbxjVXtRAZPVsnMa93981417 = -675456308;    double PBqFuAbxjVXtRAZPVsnMa82531014 = -201489355;    double PBqFuAbxjVXtRAZPVsnMa63983542 = -314566572;    double PBqFuAbxjVXtRAZPVsnMa87195110 = -882852429;    double PBqFuAbxjVXtRAZPVsnMa62582321 = -760045276;    double PBqFuAbxjVXtRAZPVsnMa86443669 = -693914103;    double PBqFuAbxjVXtRAZPVsnMa74023145 = 73460811;    double PBqFuAbxjVXtRAZPVsnMa70153567 = -460520792;    double PBqFuAbxjVXtRAZPVsnMa59404675 = -361424015;    double PBqFuAbxjVXtRAZPVsnMa72883527 = 41817698;    double PBqFuAbxjVXtRAZPVsnMa9393465 = -743667412;    double PBqFuAbxjVXtRAZPVsnMa56124358 = 64589049;    double PBqFuAbxjVXtRAZPVsnMa62605454 = 75774301;    double PBqFuAbxjVXtRAZPVsnMa14071855 = -541763873;    double PBqFuAbxjVXtRAZPVsnMa76441204 = -844877985;    double PBqFuAbxjVXtRAZPVsnMa9926200 = -400686805;    double PBqFuAbxjVXtRAZPVsnMa67593545 = -111481913;    double PBqFuAbxjVXtRAZPVsnMa86807401 = -938606758;    double PBqFuAbxjVXtRAZPVsnMa68871664 = -872238204;    double PBqFuAbxjVXtRAZPVsnMa78485761 = -347080440;    double PBqFuAbxjVXtRAZPVsnMa81364037 = -509103392;    double PBqFuAbxjVXtRAZPVsnMa11925322 = -160916015;    double PBqFuAbxjVXtRAZPVsnMa18775785 = -439442226;    double PBqFuAbxjVXtRAZPVsnMa89820872 = -889350817;    double PBqFuAbxjVXtRAZPVsnMa54152711 = -611798503;    double PBqFuAbxjVXtRAZPVsnMa4975255 = -262337449;    double PBqFuAbxjVXtRAZPVsnMa7655085 = 19597478;    double PBqFuAbxjVXtRAZPVsnMa13943531 = -590084910;    double PBqFuAbxjVXtRAZPVsnMa68899155 = -625144271;    double PBqFuAbxjVXtRAZPVsnMa69618435 = 69660794;    double PBqFuAbxjVXtRAZPVsnMa97173610 = -977051431;    double PBqFuAbxjVXtRAZPVsnMa87945250 = 49039036;    double PBqFuAbxjVXtRAZPVsnMa22860189 = -287578;    double PBqFuAbxjVXtRAZPVsnMa73727143 = -72069126;    double PBqFuAbxjVXtRAZPVsnMa93034467 = -319313429;    double PBqFuAbxjVXtRAZPVsnMa52452694 = -219954920;    double PBqFuAbxjVXtRAZPVsnMa58514318 = -318072194;    double PBqFuAbxjVXtRAZPVsnMa3972633 = -244789905;    double PBqFuAbxjVXtRAZPVsnMa3574731 = -187549437;    double PBqFuAbxjVXtRAZPVsnMa6791228 = -446199056;    double PBqFuAbxjVXtRAZPVsnMa70661742 = -793571287;    double PBqFuAbxjVXtRAZPVsnMa40266108 = -694445465;    double PBqFuAbxjVXtRAZPVsnMa53172188 = 88245895;    double PBqFuAbxjVXtRAZPVsnMa51801377 = -524714048;    double PBqFuAbxjVXtRAZPVsnMa13455867 = -35570506;    double PBqFuAbxjVXtRAZPVsnMa62768142 = 11083055;    double PBqFuAbxjVXtRAZPVsnMa61470188 = -47906162;    double PBqFuAbxjVXtRAZPVsnMa83349517 = -678003562;    double PBqFuAbxjVXtRAZPVsnMa34638289 = -649477726;    double PBqFuAbxjVXtRAZPVsnMa73225947 = -191284123;    double PBqFuAbxjVXtRAZPVsnMa91446903 = -672114677;    double PBqFuAbxjVXtRAZPVsnMa80820017 = -632264589;    double PBqFuAbxjVXtRAZPVsnMa11124001 = -430516367;    double PBqFuAbxjVXtRAZPVsnMa65876381 = -815913500;    double PBqFuAbxjVXtRAZPVsnMa35808629 = -478812424;    double PBqFuAbxjVXtRAZPVsnMa82056096 = -414540294;    double PBqFuAbxjVXtRAZPVsnMa63755230 = -762047129;    double PBqFuAbxjVXtRAZPVsnMa74162669 = -425215755;    double PBqFuAbxjVXtRAZPVsnMa33042399 = -171053927;    double PBqFuAbxjVXtRAZPVsnMa57607067 = -397707828;    double PBqFuAbxjVXtRAZPVsnMa78788584 = -613511581;    double PBqFuAbxjVXtRAZPVsnMa60079614 = -336454279;    double PBqFuAbxjVXtRAZPVsnMa1254412 = -835376522;    double PBqFuAbxjVXtRAZPVsnMa89786239 = -331084809;    double PBqFuAbxjVXtRAZPVsnMa75709917 = 18869128;    double PBqFuAbxjVXtRAZPVsnMa21448214 = -634094134;     PBqFuAbxjVXtRAZPVsnMa24502184 = PBqFuAbxjVXtRAZPVsnMa43460689;     PBqFuAbxjVXtRAZPVsnMa43460689 = PBqFuAbxjVXtRAZPVsnMa12670204;     PBqFuAbxjVXtRAZPVsnMa12670204 = PBqFuAbxjVXtRAZPVsnMa73045211;     PBqFuAbxjVXtRAZPVsnMa73045211 = PBqFuAbxjVXtRAZPVsnMa29050897;     PBqFuAbxjVXtRAZPVsnMa29050897 = PBqFuAbxjVXtRAZPVsnMa5076457;     PBqFuAbxjVXtRAZPVsnMa5076457 = PBqFuAbxjVXtRAZPVsnMa25023024;     PBqFuAbxjVXtRAZPVsnMa25023024 = PBqFuAbxjVXtRAZPVsnMa52051301;     PBqFuAbxjVXtRAZPVsnMa52051301 = PBqFuAbxjVXtRAZPVsnMa28658885;     PBqFuAbxjVXtRAZPVsnMa28658885 = PBqFuAbxjVXtRAZPVsnMa64801029;     PBqFuAbxjVXtRAZPVsnMa64801029 = PBqFuAbxjVXtRAZPVsnMa67940916;     PBqFuAbxjVXtRAZPVsnMa67940916 = PBqFuAbxjVXtRAZPVsnMa67222330;     PBqFuAbxjVXtRAZPVsnMa67222330 = PBqFuAbxjVXtRAZPVsnMa90899809;     PBqFuAbxjVXtRAZPVsnMa90899809 = PBqFuAbxjVXtRAZPVsnMa87015884;     PBqFuAbxjVXtRAZPVsnMa87015884 = PBqFuAbxjVXtRAZPVsnMa34983709;     PBqFuAbxjVXtRAZPVsnMa34983709 = PBqFuAbxjVXtRAZPVsnMa22497860;     PBqFuAbxjVXtRAZPVsnMa22497860 = PBqFuAbxjVXtRAZPVsnMa91167742;     PBqFuAbxjVXtRAZPVsnMa91167742 = PBqFuAbxjVXtRAZPVsnMa66157051;     PBqFuAbxjVXtRAZPVsnMa66157051 = PBqFuAbxjVXtRAZPVsnMa93234896;     PBqFuAbxjVXtRAZPVsnMa93234896 = PBqFuAbxjVXtRAZPVsnMa44684888;     PBqFuAbxjVXtRAZPVsnMa44684888 = PBqFuAbxjVXtRAZPVsnMa10419675;     PBqFuAbxjVXtRAZPVsnMa10419675 = PBqFuAbxjVXtRAZPVsnMa12576863;     PBqFuAbxjVXtRAZPVsnMa12576863 = PBqFuAbxjVXtRAZPVsnMa24684904;     PBqFuAbxjVXtRAZPVsnMa24684904 = PBqFuAbxjVXtRAZPVsnMa22849332;     PBqFuAbxjVXtRAZPVsnMa22849332 = PBqFuAbxjVXtRAZPVsnMa18892500;     PBqFuAbxjVXtRAZPVsnMa18892500 = PBqFuAbxjVXtRAZPVsnMa24075643;     PBqFuAbxjVXtRAZPVsnMa24075643 = PBqFuAbxjVXtRAZPVsnMa97421372;     PBqFuAbxjVXtRAZPVsnMa97421372 = PBqFuAbxjVXtRAZPVsnMa11079493;     PBqFuAbxjVXtRAZPVsnMa11079493 = PBqFuAbxjVXtRAZPVsnMa83152146;     PBqFuAbxjVXtRAZPVsnMa83152146 = PBqFuAbxjVXtRAZPVsnMa59040449;     PBqFuAbxjVXtRAZPVsnMa59040449 = PBqFuAbxjVXtRAZPVsnMa67627418;     PBqFuAbxjVXtRAZPVsnMa67627418 = PBqFuAbxjVXtRAZPVsnMa79995665;     PBqFuAbxjVXtRAZPVsnMa79995665 = PBqFuAbxjVXtRAZPVsnMa44362142;     PBqFuAbxjVXtRAZPVsnMa44362142 = PBqFuAbxjVXtRAZPVsnMa17172667;     PBqFuAbxjVXtRAZPVsnMa17172667 = PBqFuAbxjVXtRAZPVsnMa93981417;     PBqFuAbxjVXtRAZPVsnMa93981417 = PBqFuAbxjVXtRAZPVsnMa82531014;     PBqFuAbxjVXtRAZPVsnMa82531014 = PBqFuAbxjVXtRAZPVsnMa63983542;     PBqFuAbxjVXtRAZPVsnMa63983542 = PBqFuAbxjVXtRAZPVsnMa87195110;     PBqFuAbxjVXtRAZPVsnMa87195110 = PBqFuAbxjVXtRAZPVsnMa62582321;     PBqFuAbxjVXtRAZPVsnMa62582321 = PBqFuAbxjVXtRAZPVsnMa86443669;     PBqFuAbxjVXtRAZPVsnMa86443669 = PBqFuAbxjVXtRAZPVsnMa74023145;     PBqFuAbxjVXtRAZPVsnMa74023145 = PBqFuAbxjVXtRAZPVsnMa70153567;     PBqFuAbxjVXtRAZPVsnMa70153567 = PBqFuAbxjVXtRAZPVsnMa59404675;     PBqFuAbxjVXtRAZPVsnMa59404675 = PBqFuAbxjVXtRAZPVsnMa72883527;     PBqFuAbxjVXtRAZPVsnMa72883527 = PBqFuAbxjVXtRAZPVsnMa9393465;     PBqFuAbxjVXtRAZPVsnMa9393465 = PBqFuAbxjVXtRAZPVsnMa56124358;     PBqFuAbxjVXtRAZPVsnMa56124358 = PBqFuAbxjVXtRAZPVsnMa62605454;     PBqFuAbxjVXtRAZPVsnMa62605454 = PBqFuAbxjVXtRAZPVsnMa14071855;     PBqFuAbxjVXtRAZPVsnMa14071855 = PBqFuAbxjVXtRAZPVsnMa76441204;     PBqFuAbxjVXtRAZPVsnMa76441204 = PBqFuAbxjVXtRAZPVsnMa9926200;     PBqFuAbxjVXtRAZPVsnMa9926200 = PBqFuAbxjVXtRAZPVsnMa67593545;     PBqFuAbxjVXtRAZPVsnMa67593545 = PBqFuAbxjVXtRAZPVsnMa86807401;     PBqFuAbxjVXtRAZPVsnMa86807401 = PBqFuAbxjVXtRAZPVsnMa68871664;     PBqFuAbxjVXtRAZPVsnMa68871664 = PBqFuAbxjVXtRAZPVsnMa78485761;     PBqFuAbxjVXtRAZPVsnMa78485761 = PBqFuAbxjVXtRAZPVsnMa81364037;     PBqFuAbxjVXtRAZPVsnMa81364037 = PBqFuAbxjVXtRAZPVsnMa11925322;     PBqFuAbxjVXtRAZPVsnMa11925322 = PBqFuAbxjVXtRAZPVsnMa18775785;     PBqFuAbxjVXtRAZPVsnMa18775785 = PBqFuAbxjVXtRAZPVsnMa89820872;     PBqFuAbxjVXtRAZPVsnMa89820872 = PBqFuAbxjVXtRAZPVsnMa54152711;     PBqFuAbxjVXtRAZPVsnMa54152711 = PBqFuAbxjVXtRAZPVsnMa4975255;     PBqFuAbxjVXtRAZPVsnMa4975255 = PBqFuAbxjVXtRAZPVsnMa7655085;     PBqFuAbxjVXtRAZPVsnMa7655085 = PBqFuAbxjVXtRAZPVsnMa13943531;     PBqFuAbxjVXtRAZPVsnMa13943531 = PBqFuAbxjVXtRAZPVsnMa68899155;     PBqFuAbxjVXtRAZPVsnMa68899155 = PBqFuAbxjVXtRAZPVsnMa69618435;     PBqFuAbxjVXtRAZPVsnMa69618435 = PBqFuAbxjVXtRAZPVsnMa97173610;     PBqFuAbxjVXtRAZPVsnMa97173610 = PBqFuAbxjVXtRAZPVsnMa87945250;     PBqFuAbxjVXtRAZPVsnMa87945250 = PBqFuAbxjVXtRAZPVsnMa22860189;     PBqFuAbxjVXtRAZPVsnMa22860189 = PBqFuAbxjVXtRAZPVsnMa73727143;     PBqFuAbxjVXtRAZPVsnMa73727143 = PBqFuAbxjVXtRAZPVsnMa93034467;     PBqFuAbxjVXtRAZPVsnMa93034467 = PBqFuAbxjVXtRAZPVsnMa52452694;     PBqFuAbxjVXtRAZPVsnMa52452694 = PBqFuAbxjVXtRAZPVsnMa58514318;     PBqFuAbxjVXtRAZPVsnMa58514318 = PBqFuAbxjVXtRAZPVsnMa3972633;     PBqFuAbxjVXtRAZPVsnMa3972633 = PBqFuAbxjVXtRAZPVsnMa3574731;     PBqFuAbxjVXtRAZPVsnMa3574731 = PBqFuAbxjVXtRAZPVsnMa6791228;     PBqFuAbxjVXtRAZPVsnMa6791228 = PBqFuAbxjVXtRAZPVsnMa70661742;     PBqFuAbxjVXtRAZPVsnMa70661742 = PBqFuAbxjVXtRAZPVsnMa40266108;     PBqFuAbxjVXtRAZPVsnMa40266108 = PBqFuAbxjVXtRAZPVsnMa53172188;     PBqFuAbxjVXtRAZPVsnMa53172188 = PBqFuAbxjVXtRAZPVsnMa51801377;     PBqFuAbxjVXtRAZPVsnMa51801377 = PBqFuAbxjVXtRAZPVsnMa13455867;     PBqFuAbxjVXtRAZPVsnMa13455867 = PBqFuAbxjVXtRAZPVsnMa62768142;     PBqFuAbxjVXtRAZPVsnMa62768142 = PBqFuAbxjVXtRAZPVsnMa61470188;     PBqFuAbxjVXtRAZPVsnMa61470188 = PBqFuAbxjVXtRAZPVsnMa83349517;     PBqFuAbxjVXtRAZPVsnMa83349517 = PBqFuAbxjVXtRAZPVsnMa34638289;     PBqFuAbxjVXtRAZPVsnMa34638289 = PBqFuAbxjVXtRAZPVsnMa73225947;     PBqFuAbxjVXtRAZPVsnMa73225947 = PBqFuAbxjVXtRAZPVsnMa91446903;     PBqFuAbxjVXtRAZPVsnMa91446903 = PBqFuAbxjVXtRAZPVsnMa80820017;     PBqFuAbxjVXtRAZPVsnMa80820017 = PBqFuAbxjVXtRAZPVsnMa11124001;     PBqFuAbxjVXtRAZPVsnMa11124001 = PBqFuAbxjVXtRAZPVsnMa65876381;     PBqFuAbxjVXtRAZPVsnMa65876381 = PBqFuAbxjVXtRAZPVsnMa35808629;     PBqFuAbxjVXtRAZPVsnMa35808629 = PBqFuAbxjVXtRAZPVsnMa82056096;     PBqFuAbxjVXtRAZPVsnMa82056096 = PBqFuAbxjVXtRAZPVsnMa63755230;     PBqFuAbxjVXtRAZPVsnMa63755230 = PBqFuAbxjVXtRAZPVsnMa74162669;     PBqFuAbxjVXtRAZPVsnMa74162669 = PBqFuAbxjVXtRAZPVsnMa33042399;     PBqFuAbxjVXtRAZPVsnMa33042399 = PBqFuAbxjVXtRAZPVsnMa57607067;     PBqFuAbxjVXtRAZPVsnMa57607067 = PBqFuAbxjVXtRAZPVsnMa78788584;     PBqFuAbxjVXtRAZPVsnMa78788584 = PBqFuAbxjVXtRAZPVsnMa60079614;     PBqFuAbxjVXtRAZPVsnMa60079614 = PBqFuAbxjVXtRAZPVsnMa1254412;     PBqFuAbxjVXtRAZPVsnMa1254412 = PBqFuAbxjVXtRAZPVsnMa89786239;     PBqFuAbxjVXtRAZPVsnMa89786239 = PBqFuAbxjVXtRAZPVsnMa75709917;     PBqFuAbxjVXtRAZPVsnMa75709917 = PBqFuAbxjVXtRAZPVsnMa21448214;     PBqFuAbxjVXtRAZPVsnMa21448214 = PBqFuAbxjVXtRAZPVsnMa24502184;}
// Junk Finished
