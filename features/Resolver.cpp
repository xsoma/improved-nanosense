#include "Resolver.hpp"
#include "AngryPew.hpp"
#include "..//SDK.hpp"
#include "PlayerHurt.hpp"
#include "..//Handlers.h"
#include "../Options.hpp"
#include "..//SanityChech.hpp"
#include "LagCompensation.hpp"
#include "RebuildGameMovement.hpp"
#include <algorithm>

#define M_PI 3.14159265358979323846

void Resolver::Log()
{
	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		//if (XSystemCFG.hvh_resolver_experimental)
		{
			auto player = C_BasePlayer::GetPlayerByIndex(i);

			if (!player)
			{
				records[i].clear();
				continue;
			}
			else if (AngryPew::Get().CheckTarget(i))
			{
				if (records[i].size() > 15 / g_GlobalVars->interval_per_tick)
					records[i].pop_back();

				resolvrecord record_to_store;
				record_to_store.SaveRecord(player);

				if (!(records[i].empty()))
				{
					record_to_store.shot = (record_to_store.shot && !records[i].front().was_dormant);
					record_to_store.suppresing_animation = records[i].front().suppresing_animation;

					record_to_store.moving = (record_to_store.moving && !PFakewalkDetection(player, record_to_store));
					record_to_store.was_moving = (records[i].front().was_moving && !PFakewalkDetection(player, record_to_store));

					record_to_store.update_origin = records[i].front().update_origin;
					record_to_store.last_standing_simtime = records[i].front().moving ? record_to_store.simtime : records[i].front().last_standing_simtime;
					record_to_store.moving_lby = records[i].front().moving_lby;
					record_to_store.lastlby_lby_delta = records[i].front().lastlby_lby_delta;
					record_to_store.last_moving_simtime = records[i].front().last_moving_simtime;
					record_to_store.last_update_simtime = records[i].front().last_update_simtime;
					record_to_store.last_update_angle = records[i].front().last_update_angle;
					record_to_store.saw_update = records[i].front().saw_update;
					if (abs(Math::ClampYaw(records[i].front().lby - record_to_store.lby)) > 10)
					{
						record_to_store.update = true;
						record_to_store.saw_update = true;
						record_to_store.lastlby_lby_delta = record_to_store.lby - records[i].front().lby;
					}
				}

				if (record_to_store.moving)
				{
					record_to_store.moving ? record_to_store.last_moving_simtime = record_to_store.simtime, record_to_store.was_moving = true : record_to_store.was_moving = false;
					record_to_store.moving = true;
					record_to_store.moving_lby = record_to_store.lby;
					record_to_store.update = false;
					record_to_store.last_update_angle = record_to_store.resolvedang;
					record_to_store.last_update_angle.yaw = record_to_store.lby;
				}
				if (record_to_store.was_moving)
					record_to_store.moving_lby = record_to_store.lby;
				if (!records[i].empty() && (record_to_store.lby == records[i].front().lby || records[i].front().moving_lby < -1000))
				{
					record_to_store.moving_lby_delta = records[i].front().moving_lby_delta;
					record_to_store.raw_lby_delta = records[i].front().raw_lby_delta;
				}
				else
				{
					record_to_store.moving_lby_delta = record_to_store.lby - record_to_store.moving_lby;
					record_to_store.raw_lby_delta = record_to_store.rawang.yaw - record_to_store.moving_lby;
				}

				if (player->m_fFlags() & FL_ONGROUND && !record_to_store.moving && !records[i].empty() && record_to_store.simtime != records[i].front().simtime)	//last simtime == curr simtime => choking packet
				{
					if (record_to_store.simtime - record_to_store.last_standing_simtime > 0.22f && record_to_store.was_moving)
					{
						record_to_store.update = true;
						record_to_store.saw_update = true;
						record_to_store.was_moving = false;
					}
					else if (record_to_store.simtime - record_to_store.last_update_simtime > 1.1f)
						record_to_store.update = true;
					else
						record_to_store.update = false;
				}

				if (record_to_store.update)
				{
					record_to_store.update_origin = record_to_store.origin;
					record_to_store.last_update_simtime = record_to_store.simtime;
					record_to_store.last_update_angle.yaw = record_to_store.lby;
					record_to_store.last_update_angle.pitch = SWResolver_pitch(player);
				}

				records[i].push_front(record_to_store);
			}
			else if (!records[i].empty())
			{
				records[i].front().moving_lby = -100000;
				records[i].front().saw_update = false;
				records[i].front().was_dormant = true;
			}
		}
		
		{
			auto& record = arr_infos[i];

			C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);
			if (!player || !player->IsAlive() || player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			{
				record.m_bActive = false;
				continue;
			}

			if (player->IsDormant())
			{
				bFirstUpdate[player->EntIndex()] = false;
				bSawUpdate[player->EntIndex()] = false;
				continue;
			}

			if (record.m_flSimulationTime == player->m_flSimulationTime())
				continue;

			record.SaveRecord(player);
			record.m_bActive = true;
		}
	}
}

void Resolver::Resolve()
{
	EventHandler.process();		//run shots missed counter before we continue
	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		if (1)
		{
			REALSelfWrittenResolver(i);
		}
		else
		{
			auto& record = arr_infos[i];
			if (!record.m_bActive)
				continue;

			C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);
			if (!player || !player->IsAlive() || player->IsDormant() || player == g_LocalPlayer)
				continue;

			if (record.m_flVelocity == 0.f && player->m_vecVelocity().Length2D() != 0.f)
			{
				Math::VectorAngles(player->m_vecVelocity(), record.m_angDirectionFirstMoving);
				record.m_nCorrectedFakewalkIdx = 0;
			}

			auto firedShots = g_LocalPlayer->m_iShotsFired();

			if (XSystemCFG.debug_fliponkey)
			{
				float_t new_yaw = player->m_flLowerBodyYawTarget();
				if (g_InputSystem->IsButtonDown(XSystemCFG.debug_flipkey))
					new_yaw += 180.f;
				new_yaw = Math::ClampYaw(new_yaw);
				player->m_angEyeAngles().yaw = new_yaw;
				return;
			}

			if (XSystemCFG.hvh_resolver_override && g_InputSystem->IsButtonDown(XSystemCFG.hvh_resolver_override_key))
			{
				Override(); //needs an improvement sometimes fucked up xD

				Global::resolverModes[player->EntIndex()] = "Overriding";

				return;
			}

			AnimationLayer curBalanceLayer, prevBalanceLayer;

			ResolveInfo curtickrecord;
			curtickrecord.SaveRecord(player);

			if (((player->m_fFlags() & FL_ONGROUND) && (IsFakewalking(player, curtickrecord) || (player->m_vecVelocity().Length2D() > 0.1f && player->m_vecVelocity().Length2D() < 45.f && !(player->m_fFlags() & FL_DUCKING))))) //Fakewalk, shiftwalk check // We have to rework the fakewalk resolving, it sucks :D
			{
				float_t new_yaw = ResolveFakewalk(player, curtickrecord);
				new_yaw = Math::ClampYaw(new_yaw);

				player->m_angEyeAngles().yaw = new_yaw;

				Global::resolverModes[player->EntIndex()] = "Fakewalking";

				continue;
			}
			if (IsEntityMoving(player) && !(/*XSystemCFG.hvh_resolver_experimental &&*/ (player->m_fFlags() & FL_ONGROUND && (IsFakewalking(player, curtickrecord) || (player->m_vecVelocity().Length2D() > 0.1f && player->m_vecVelocity().Length2D() < 45.f && !(player->m_fFlags() & FL_DUCKING))))))
			{
				float_t new_yaw = player->m_flLowerBodyYawTarget();
				new_yaw = Math::ClampYaw(new_yaw);

				player->m_angEyeAngles().yaw = new_yaw;

				record.m_flStandingTime = player->m_flSimulationTime();
				record.m_flMovingLBY = player->m_flLowerBodyYawTarget();
				record.m_bIsMoving = true;

				Global::resolverModes[player->EntIndex()] = "Moving";

				continue;
			}
			ConVar* nospread = g_CVar->FindVar("weapon_accuracy_nospread");
			if (!player->m_fFlags() & FL_ONGROUND && nospread->GetBool())
			{
				float_t new_yaw = player->m_flLowerBodyYawTarget();
				new_yaw = ResolveBruteforce(player, new_yaw);
				new_yaw = Math::ClampYaw(new_yaw);

				player->m_angEyeAngles().yaw = new_yaw;

				continue;
			}
			if (IsAdjustingBalance(player, curtickrecord, &curBalanceLayer))
			{
				if (fabsf(LBYDelta(curtickrecord)) > 35.f)
				{
					float
						flAnimTime = curBalanceLayer.m_flCycle,	// no matter how accurate fakehead resolvers are, backtrack are always more accurate
						flSimTime = player->m_flSimulationTime();

					if (flAnimTime < 0.01f && prevBalanceLayer.m_flCycle > 0.01f && XSystemCFG.angrpwlagcompensation && CMBacktracking::Get().IsTickValid(TIME_TO_TICKS(flSimTime - flAnimTime)))
					{
						CMBacktracking::Get().SetOverwriteTick(player, QAngle(player->m_angEyeAngles().pitch, player->m_flLowerBodyYawTarget(), 0), (flSimTime - flAnimTime), 2);
					}

					float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
					new_yaw = Math::ClampYaw(new_yaw);

					player->m_angEyeAngles().yaw = new_yaw;

					Global::resolverModes[player->EntIndex()] = "Fakehead (delta > 35)";
				}
				if (IsAdjustingBalance(player, record, &prevBalanceLayer))
				{
					if (!(XSystemCFG.resolver_eyelby_running))
					{
						if ((prevBalanceLayer.m_flCycle != curBalanceLayer.m_flCycle) || curBalanceLayer.m_flWeight == 1.f)
						{
							float
								flAnimTime = curBalanceLayer.m_flCycle,
								flSimTime = player->m_flSimulationTime();

							if (flAnimTime < 0.01f && prevBalanceLayer.m_flCycle > 0.01f && XSystemCFG.angrpwlagcompensation && CMBacktracking::Get().IsTickValid(TIME_TO_TICKS(flSimTime - flAnimTime)))
							{
								CMBacktracking::Get().SetOverwriteTick(player, QAngle(player->m_angEyeAngles().pitch, player->m_flLowerBodyYawTarget(), 0), (flSimTime - flAnimTime), 2);
							}

							float_t new_yaw = player->m_flLowerBodyYawTarget();
							new_yaw = Math::ClampYaw(new_yaw);

							player->m_angEyeAngles().yaw = new_yaw;

							Global::resolverModes[player->EntIndex()] = "Breaking LBY";

							continue;
						}
						else if (curBalanceLayer.m_flWeight == 0.f && (prevBalanceLayer.m_flCycle > 0.92f && curBalanceLayer.m_flCycle > 0.92f)) // breaking lby with delta < 120
						{
							if (player->m_flSimulationTime() >= record.m_flStandingTime + 0.22f && record.m_bIsMoving)
							{
								record.m_flLbyDelta = record.m_flLowerBodyYawTarget - player->m_flLowerBodyYawTarget();

								float
									flAnimTime = curBalanceLayer.m_flCycle,
									flSimTime = player->m_flSimulationTime();

								if (flAnimTime < 0.01f && prevBalanceLayer.m_flCycle > 0.01f && XSystemCFG.angrpwlagcompensation && CMBacktracking::Get().IsTickValid(TIME_TO_TICKS(flSimTime - flAnimTime)))
								{
									CMBacktracking::Get().SetOverwriteTick(player, QAngle(player->m_angEyeAngles().pitch, player->m_flLowerBodyYawTarget(), 0), (flSimTime - flAnimTime), 2);
								}

								float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
								new_yaw = Math::ClampYaw(new_yaw);

								player->m_angEyeAngles().yaw = new_yaw;

								record.m_bIsMoving = false;

								Global::resolverModes[player->EntIndex()] = "Breaking LBY (delta < 120)";

								continue;
							}

							if (player->m_flSimulationTime() >= record.m_flStandingTime + 1.32f && std::fabsf(record.m_flLbyDelta) < 35.f)
							{
								record.m_flLbyDelta = record.m_flMovingLBY - player->m_flLowerBodyYawTarget();
								float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
								new_yaw = Math::ClampYaw(new_yaw);

								player->m_angEyeAngles().yaw = new_yaw;

								record.m_bIsMoving = false;

								Global::resolverModes[player->EntIndex()] = "LBY delta < 35";

								continue;
							}
						}
					}
				}
				else
				{
					float_t new_yaw = player->m_flLowerBodyYawTarget();
					new_yaw = Math::ClampYaw(new_yaw);

					player->m_angEyeAngles().yaw = new_yaw;

					Global::resolverModes[player->EntIndex()] = "Other";

					continue;
				}
			}
			if (player->m_flSimulationTime() >= record.m_flStandingTime + 0.22f && record.m_bIsMoving)
			{
				record.m_flLbyDelta = record.m_flLowerBodyYawTarget - player->m_flLowerBodyYawTarget();

				float
					flAnimTime = curBalanceLayer.m_flCycle,
					flSimTime = player->m_flSimulationTime();

				if (flAnimTime < 0.01f && prevBalanceLayer.m_flCycle > 0.01f && XSystemCFG.angrpwlagcompensation && CMBacktracking::Get().IsTickValid(TIME_TO_TICKS(flSimTime - flAnimTime)))
				{
					CMBacktracking::Get().SetOverwriteTick(player, QAngle(player->m_angEyeAngles().pitch, player->m_flLowerBodyYawTarget(), 0), (flSimTime - flAnimTime), 2);
				}

				float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
				new_yaw = Math::ClampYaw(new_yaw);

				player->m_angEyeAngles().yaw = new_yaw;

				record.m_bIsMoving = false;

				Global::resolverModes[player->EntIndex()] = "Breaking LBY (delta < 120)";

				continue;
			}
			if (player->m_flSimulationTime() >= record.m_flStandingTime + 1.32f && std::fabsf(record.m_flLbyDelta) < 35.f)
			{
				record.m_flLbyDelta = record.m_flMovingLBY - player->m_flLowerBodyYawTarget();
				float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
				new_yaw = Math::ClampYaw(new_yaw);

				player->m_angEyeAngles().yaw = new_yaw;

				record.m_bIsMoving = false;

				Global::resolverModes[player->EntIndex()] = "LBY delta < 35";

				continue;
			}

			float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
			new_yaw = Math::ClampYaw(new_yaw);

			player->m_angEyeAngles().yaw = new_yaw;
		}
	}
}

void Resolver::FakelagFix()
{
	static bool m_bWasInAir[65] = { false };
	for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto player = C_BasePlayer::GetPlayerByIndex(i);
		if (checks::is_bad_ptr(player) || !player->IsPlayer() ||
			!player->IsAlive() || player->IsDormant() || player == g_LocalPlayer)
			continue;

		Global::FakelagFixed[i] = false;

		Global::PlayersChockedPackets[i] = TIME_TO_TICKS((player->m_flOldSimulationTime() + g_GlobalVars->interval_per_tick) - player->m_flSimulationTime());

		static Vector origin[65] = { Vector(0, 0, 0) }, velocity[65] = { Vector(0, 0, 0) };
		static Vector origin_backup[65] = { Vector(0, 0, 0) }, velocity_backup[65] = { Vector(0, 0, 0) };
		static int flag[65] = { 0 };
		static int flag_backup[65] = { 0 };

		if (Global::PlayersChockedPackets[i] > 1)
		{
			if (Global::PlayersChockedPackets[i] <= 14)
			{
				//RebuildGameMovement::Get().VelocityExtrapolate(player, origin[i], velocity[i], flag[i], m_bWasInAir[i]);
				RebuildGameMovement::Get().FullWalkMove(player);
				origin[i] = player->GetAbsOrigin();	//rebuild movement changes abs origin

				m_bWasInAir[i] = (flag[i] & FL_ONGROUND) && !(player->m_fFlags() & FL_ONGROUND);

				if (player->m_vecOrigin().DistTo(origin_backup[i]) > 64)
				{
					player->SetAbsOrigin(origin_backup[i]);
					player->m_vecOrigin() = origin_backup[i];
					player->m_vecVelocity() = velocity_backup[i];
					player->m_fFlags() = flag_backup[i];
				}
				else
				{
					player->m_vecOrigin() = origin[i];
					player->m_vecVelocity() = velocity[i];
					player->m_fFlags() = flag[i];
					Global::FakelagFixed[i] = true;
				}
			}
		}
		else
		{
			origin[i] = player->m_vecOrigin();
			velocity[i] = player->m_vecVelocity();
			flag[i] = player->m_fFlags();

			origin_backup[i] = origin[i];
			velocity_backup[i] = velocity[i];
			flag_backup[i] = flag[i];

			Global::FakelagUnfixedPos[i] = origin[i];
		}
	}
}

void Resolver::Override()
{
	if (!XSystemCFG.hvh_resolver_override)
		return;

	if (!g_InputSystem->IsButtonDown(XSystemCFG.hvh_resolver_override_key))
		return;

	int w, h, cx, cy;

	g_EngineClient->GetScreenSize(w, h);

	cx = w / 2;
	cy = h / 2;

	Vector crosshair = Vector(cx, cy, 0);

	C_BasePlayer* nearest_player = nullptr;
	float bestFoV = 0;
	Vector bestHead2D;

	for (int i = 1; i <= g_GlobalVars->maxClients; i++) //0 is always the world entity
	{
		C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!CMBacktracking::Get().IsPlayerValid(player)) //ghetto
			continue;

		Vector headPos3D = player->GetBonePos(HITBOX_HEAD), headPos2D;

		if (!Math::WorldToScreen(headPos3D, headPos2D))
			continue;

		float FoV2D = crosshair.DistTo(headPos2D);

		if (!nearest_player || FoV2D < bestFoV)
		{
			nearest_player = player;
			bestFoV = FoV2D;
			bestHead2D = headPos2D;
		}
	}

	if (nearest_player) //use pointers and avoid double calling of GetClientEntity
	{
		int minX = cx - (w / 10), maxX = cx + (w / 10);

		if (bestHead2D.x < minX || bestHead2D.x > maxX)
			return;

		int totalWidth = maxX - minX;

		int playerX = bestHead2D.x - minX;

		int yawCorrection = -(((playerX * 360) / totalWidth) - 180);

		float_t new_yaw = yawCorrection;

		Math::ClampYaw(new_yaw);

		nearest_player->m_angEyeAngles().yaw += new_yaw;
	}
}

float_t Resolver::ResolveFakewalk(C_BasePlayer* player, ResolveInfo& curtickrecord)	//high chance of missing, but still try to tap atleast 2 shot
{
	auto& record = arr_infos[player->EntIndex()];

	float_t yaw;
	int32_t correctedFakewalkIdx = record.m_nCorrectedFakewalkIdx;

	if (correctedFakewalkIdx < 2)
	{
		yaw = record.m_angDirectionFirstMoving.yaw + 180.f;	//from mutiny, no idea why but it works?
		Global::resolverModes[player->EntIndex()] = "Fakewalking stage 1";
	}
	else if (correctedFakewalkIdx < 4)
	{
		yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
		Global::resolverModes[player->EntIndex()] = "Fakewalking stage 2";
	}
	else if (correctedFakewalkIdx < 6)
	{
		yaw = record.m_angDirectionFirstMoving.yaw;
		Global::resolverModes[player->EntIndex()] = "Fakewalking stage 3";
	}
	else
	{
		QAngle dir;
		Math::VectorAngles(curtickrecord.m_vecVelocity, dir);

		//yaw = dir.yaw;
		yaw = ResolveBruteforce(player, dir.yaw);	//goes full on retarded and brute everything
	}

	return yaw;
}

float_t Resolver::ResolveBruteforce(C_BasePlayer* player, float baseangle)
{
	static float hitang[65] = { -999 };
	static float lastang[65] = { -999 };

	int index = player->EntIndex();

	if (Global::hit_while_brute[index] && missed_shots[index] < 5)
	{
		if (hitang[index] < -200)
			hitang[index] = lastang[index];

		Global::resolverModes[index] = "Fake: Bruteforce";
		return hitang[index];
	}
	else
		hitang[index] = -999;

	lastang[index] = baseangle;
	if (missed_shots[index] < 5 && abs(Math::ClampYaw(player->m_flLowerBodyYawTarget() - baseangle)) < 10)
	{
		switch (missed_shots[index] % 8)
		{
		case 0:
			lastang[index] += 60;
			break;
		case 1:
			lastang[index] += 58;
			break;
		case 2:
			lastang[index] += 49;
			break;
		case 3:
			lastang[index] += 35;
			break;
		case 4:
			lastang[index] -= 60;
			break;
		case 5:
			lastang[index] -= 58;
			break;
		case 6:
			lastang[index] -= 49;
			break;
		case 7:
			lastang[index] -= 35;
			break;
		}
	}
	else
	{
		switch (missed_shots[index] % 10)
		{
		case 0:
			lastang[index] += 60;
			break;
		case 1:
			lastang[index] += 58;
			break;
		case 2:
			lastang[index] += 49;
			break;
		case 3:
			lastang[index] += 35;
			break;
		case 4:
			lastang[index] -= 60;
			break;
		case 5:
			lastang[index] -= 58;
			break;
		case 6:
			lastang[index] -= 49;
			break;
		case 7:
			lastang[index] -= 35;
			break;
		case 8:
			lastang[index] += (missed_shots[index] % 8) * 45;
			break;
		case 9:
			lastang[index] += (missed_shots[index] % 8) * 45 + 90;
			break;
		}
	}

	lastang[index] = Math::ClampYaw(lastang[index]);

	Global::resolverModes[index] = "Fake: Bruteforce";

	return lastang[index];
}

bool Resolver::IsEntityMoving(C_BasePlayer* player)
{
	return (player->m_vecVelocity().Length2D() > 0.1f && player->m_fFlags() & FL_ONGROUND);
}

bool Resolver::IsAdjustingBalance(C_BasePlayer* player, ResolveInfo& record, AnimationLayer* layer)
{
	for (int i = 0; i < record.m_iLayerCount; i++)
	{
		const int activity = player->GetSequenceActivity(record.animationLayer[i].m_nSequence);
		if (activity == 979)
		{
			*layer = record.animationLayer[i];
			return true;
		}
	}
	return false;
}

bool Resolver::IsAdjustingStopMoving(C_BasePlayer* player, ResolveInfo& record, AnimationLayer* layer)
{
	for (int i = 0; i < record.m_iLayerCount; i++)
	{
		const int activity = player->GetSequenceActivity(record.animationLayer[i].m_nSequence);
		if (activity == 980)
		{
			*layer = record.animationLayer[i];
			return true;
		}
	}
	return false;
}

bool Resolver::IsFakewalking(C_BasePlayer* player, ResolveInfo& record)
{
	bool
		bFakewalking = false,
		stage1 = false,			// stages needed cause we are iterating all layers, eitherwise won't work :)
		stage2 = false,
		stage3 = false;

	for (int i = 0; i < record.m_iLayerCount; i++)
	{
		if (record.animationLayer[i].m_nSequence == 26 && record.animationLayer[i].m_flWeight < 0.4f)
			stage1 = true;
		if (record.animationLayer[i].m_nSequence == 7 && record.animationLayer[i].m_flWeight > 0.001f)
			stage2 = true;
		if (record.animationLayer[i].m_nSequence == 2 && record.animationLayer[i].m_flWeight == 0)
			stage3 = true;
	}

	if (stage1 && stage2)
		if (stage3 || (player->m_fFlags() & FL_DUCKING)) // since weight from stage3 can be 0 aswell when crouching, we need this kind of check, cause you can fakewalk while crouching, thats why it's nested under stage1 and stage2
			bFakewalking = true;
		else
			bFakewalking = false;
	else
		bFakewalking = false;

	return bFakewalking;
}

bool Resolver::PFakewalkDetection(C_BasePlayer* player, resolvrecord& record)
{
	bool weight981, weightseq2, weightlay12, pbratelay6 = false;
	for (int i = 0; i < record.layercount; i++)
	{
		const int activity = player->GetSequenceActivity(record.animationLayer[i].m_nSequence);
		if (activity == 981 && record.animationLayer[i].m_flWeight == 1)
			weight981 = true;
		if (record.animationLayer[i].m_nSequence == 2 && record.animationLayer[i].m_flWeight == 0)
			weightseq2 = true;
		if (i == 12 && record.animationLayer[i].m_flWeight > 0)
			weightlay12 = true;
		if (i == 6 && record.animationLayer[i].m_flPlaybackRate < 0.001)
			pbratelay6 = true;
	}
	if (player->m_vecVelocity().Length2D() > 100.f || player->m_vecVelocity().Length2D() < 0.1f || !(player->m_fFlags() & FL_ONGROUND)) return false;
	else if (weight981 && weightseq2 && weightlay12 && pbratelay6)
	{
		if (!records[player->EntIndex()].empty()) records[player->EntIndex()].front().suppresing_animation = false;
		return true;
	}
	else //animation-less method
	{
		static int choked[65] = { 0 };
		static int last_choke[65] = { 0 };
		static Vector last_origin[65] = { Vector(0, 0, 0) };

		int choke = TIME_TO_TICKS((player->m_flOldSimulationTime() + g_GlobalVars->interval_per_tick) - player->m_flSimulationTime());
		static bool returnval[65] = { false };

		if (choke < last_choke[player->EntIndex()])
		{
			choked[player->EntIndex()] = last_choke[player->EntIndex()];
			if (!last_origin[player->EntIndex()].IsZero())
			{
				Vector delta = player->m_vecOrigin() - last_origin[player->EntIndex()];
				float calced_vel = delta.Length2D() / choked[player->EntIndex()];
				returnval[player->EntIndex()] = calced_vel != player->m_vecVelocity().Length2D() * g_GlobalVars->interval_per_tick;
			}
			last_origin[player->EntIndex()] = player->m_vecOrigin();
		}
		last_choke[player->EntIndex()] = choke;

		if (choked[player->EntIndex()] < 3) return false;
		if (returnval[player->EntIndex()] && !records[player->EntIndex()].empty()) records[player->EntIndex()].front().suppresing_animation = true;

		return returnval[player->EntIndex()];
	}
}
float Resolver::GetLBYByCompairingTicks(int playerindex)
{
	int modulo = 1;
	//int diffrence = exp_res_infos[playerindex].Get_Diffrent_LBYs(10.f);
	int diffrence = Get_Diffrent_LBYs(10.f, playerindex);
	int step = modulo * diffrence;
	for (auto var : records[playerindex])
	{
		for (int last_tick = var.tickcount; last_tick <= g_GlobalVars->tickcount; last_tick += step)
		{
			if (last_tick == g_GlobalVars->tickcount)
				return var.lby;
		}
	}
	return -1000;
}
float Resolver::GetDeltaByCompairingTicks(int playerindex)
{
	int modulo = 1;
	//int diffrence = exp_res_infos[playerindex].Get_Diffrent_Deltas(10.f);
	int diffrence = Get_Diffrent_Deltas(10.f, playerindex);
	int step = modulo * diffrence;
	for (auto var : records[playerindex])
	{
		for (int last_tick = var.tickcount; last_tick <= g_GlobalVars->tickcount; last_tick += step)
		{
			if (last_tick == g_GlobalVars->tickcount)
				return var.moving_lby_delta;
		}
	}
	return -1000;
}
bool Resolver::Choking_Packets(int i)
{
	float delta = records[i].front().simtime - records[i].at(1).simtime;
	return delta != max(g_GlobalVars->interval_per_tick, g_GlobalVars->frametime);
}
bool Resolver::Has_Static_Real(float tolerance, int i)
{
	auto minmax = std::minmax_element(std::begin(records[i]), std::end(records[i]), [](const resolvrecord& t1, const resolvrecord& t2)
		{ return t1.lby < t2.lby; });
	return (fabs(minmax.first->lby - minmax.second->lby) <= tolerance);
}
bool Resolver::Has_Static_Yaw_Difference(float tolerance, int i)
{
	if (Fake_Unusuable(tolerance, i))
		return false;

	return GetDelta(records[i].at(0).rawang.yaw, records[i].at(1).rawang.yaw) < tolerance;
}
bool Resolver::Has_Steady_Difference(float tolerance, int i)
{
	size_t misses = 0;
	for (size_t x = 0; x < records[i].size() - 1; x++) {
		float tickdif = static_cast<float>(records[i].at(x).simtime - records[i].at(x + 1).tickcount);
		float lbydif = GetDelta(records[i].at(x).lby, records[i].at(x + 1).lby);
		float ntickdif = static_cast<float>(g_GlobalVars->tickcount - records[i].at(x).tickcount);
		if (((lbydif / tickdif) * ntickdif) > tolerance) misses++;
	}
	return (misses <= (records[i].size() / 3));
}
int Resolver::Get_Diffrent_Deltas(float tolerance, int i)
{
	std::vector<float>var;
	float avg = 0;
	float total = 0;
	for (int x = 0; x < records[i].size(); x++)
	{
		total += records[i].at(x).moving_lby_delta;
		avg = total / (x + 1);
		if (std::fabsf(records[i].at(x).moving_lby_delta - avg) > tolerance)
			var.push_back(records[i].at(x).moving_lby_delta);
	}
	return var.size();
}
int Resolver::Get_Diffrent_Fakes(float tolerance, int i)
{
	std::vector<float>var;
	float avg = 0;
	float total = 0;
	for (int x = 0; x < records[i].size(); x++)
	{
		total += records[i].at(x).rawang.yaw;
		avg = total / (x + 1);
		if (std::fabsf(records[i].at(x).rawang.yaw - avg) > tolerance)
			var.push_back(records[i].at(x).rawang.yaw);
	}
	return var.size();
}
int Resolver::Get_Diffrent_LBYs(float tolerance, int i)
{
	std::vector<float>var;
	float avg = 0;
	float total = 0;
	for (int x = 0; x < records[i].size(); x++)
	{
		total += records[i].at(x).lby;
		avg = total / (x + 1);
		if (std::fabsf(records[i].at(x).lby - avg) > tolerance)
			var.push_back(records[i].at(x).lby);
	}
	return var.size();
}

bool Resolver::Delta_Keeps_Changing(float tolerance, int i)
{
	return (Get_Diffrent_Deltas(tolerance, i) > (int)records[i].size() / 2);
}
bool Resolver::Fake_Unusuable(float tolerance, int i)
{
	return (Get_Diffrent_Fakes(tolerance, i) > (int)records[i].size() / 2);
}
bool Resolver::LBY_Keeps_Changing(float tolerance, int i)
{
	return (Get_Diffrent_LBYs(tolerance, i) > (int)records[i].size() / 2);
}

bool Resolver::IsFreestanding_thickness(C_BasePlayer* player, float& angle)
{
	float bestrotation, highestthickness, radius = 0.f;
	Vector besthead, headpos, eyepos, origin;
	float step = M_PI * 2.0 / 15;

	auto checkWallThickness = [&](Vector newhead) -> float
	{
		Ray_t ray;
		trace_t trace1, trace2;
		Vector endpos1, endpos2;
		Vector eyepos = g_LocalPlayer->GetEyePos();
		CTraceFilterSkipTwoEntities filter(g_LocalPlayer, player);

		ray.Init(newhead, eyepos);
		g_EngineTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace1);

		if (trace1.DidHit())
			endpos1 = trace1.endpos;
		else
			return 0.f;

		ray.Init(eyepos, newhead);
		g_EngineTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace2);

		if (trace2.DidHit())
			endpos2 = trace2.endpos;

		float add = newhead.DistTo(eyepos) - (player->GetAbsOrigin() + player->m_vecViewOffset()).DistTo(eyepos) + 3.f;
		return endpos1.DistTo(endpos2) + add / 3;
	};

	origin = player->GetAbsOrigin();
	eyepos = origin + player->m_vecViewOffset();
	headpos = player->GetHitboxPos(0);

	radius = Vector(headpos - origin).Length2D();

	for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
	{
		float thickness = 0.f;
		Vector newhead(radius * cos(rotation) + eyepos.x, radius * sin(rotation) + eyepos.y, eyepos.z);

		thickness = checkWallThickness(newhead);

		if (thickness > highestthickness)
		{
			highestthickness = thickness;
			bestrotation = rotation;
			besthead = newhead;
		}
	}
	angle = RAD2DEG(bestrotation);
	return (highestthickness != 0 && besthead.IsValid());
}

int Resolver::IsFreestanding_awall(C_BasePlayer* player)
{
	Vector direction_1, direction_2, direction_3;
	Math::AngleVectors(QAngle(0, Math::CalcAngle(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin()).yaw - 90, 0), direction_1);
	Math::AngleVectors(QAngle(0, Math::CalcAngle(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin()).yaw + 90, 0), direction_2);
	Math::AngleVectors(QAngle(0, Math::CalcAngle(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin()).yaw + 180, 0), direction_3);

	trace_t trace;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = player;

	auto eyepos_1 = player->GetEyePos() + (direction_1 * 128);
	auto eyepos_2 = player->GetEyePos() + (direction_2 * 128);
	auto eyepos_3 = player->GetEyePos() + (direction_3 * 128);

	ray.Init(player->GetEyePos(), eyepos_1);
	g_EngineTrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);
	eyepos_1 = trace.endpos;

	ray.Init(player->GetEyePos(), eyepos_2);
	g_EngineTrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);
	eyepos_2 = trace.endpos;

	ray.Init(player->GetEyePos(), eyepos_3);
	g_EngineTrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);
	eyepos_3 = trace.endpos;


	//float damage1 = AngryPew::Get().GetDamageVec(eyepos_1, g_LocalPlayer, HITBOX_HEAD, false, true, nullptr, player);
	//float damage2 = AngryPew::Get().GetDamageVec(eyepos_2, g_LocalPlayer, HITBOX_HEAD, false, true, nullptr, player);
	float damage1 = AngryPew::Get().GetDamageVec2(g_LocalPlayer->GetEyePos(), eyepos_1, g_LocalPlayer, player, HITBOX_HEAD).damage;
	float damage2 = AngryPew::Get().GetDamageVec2(g_LocalPlayer->GetEyePos(), eyepos_2, g_LocalPlayer, player, HITBOX_HEAD).damage;
	float damage3 = AngryPew::Get().GetDamageVec2(g_LocalPlayer->GetEyePos(), eyepos_3, g_LocalPlayer, player, HITBOX_HEAD).damage;

	if (std::fabsf(damage1 - damage2) < 20)
	{
		if (std::fabsf(((damage1 + damage2) / 2) - damage3) > 30)
			return 3;
		else
			return 0;
	}
	else if (damage1 < damage2)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

bool Resolver::IsFreestanding_walldt(C_BasePlayer* player, float& angle)
{
	trace_t trace;
	Ray_t ray;
	CTraceFilterWorldOnly filter;

	static constexpr float trace_distance = 25.f;
	const auto head_position = player->GetAbsOrigin() + player->m_vecViewOffset();

	float last_fraction = 1.f;
	std::deque<float> angles;
	for (int i = 0; i < 360; i += 2)
	{
		Vector direction;
		Math::AngleVectors(QAngle(0, i, 0), direction);

		ray.Init(head_position, head_position + (direction * trace_distance));
		g_EngineTrace->TraceRay(ray, MASK_ALL, &filter, &trace);

		if (trace.fraction > last_fraction)
		{
			angles.push_front(i - 2);
		}
		else
			last_fraction = trace.fraction;
	}
	for (int i = 0; i < angles.size(); i++)
	{
		if (std::fabsf((player->m_flLowerBodyYawTarget() + 180) - angles.at(i)) < 35)
		{
			angle = angles.at(i);
			return true;
		}
	}
	return false;
}

bool Resolver::IsBreakingLBY(C_BasePlayer* player)
{
	auto poses = player->m_flPoseParameter();
	return ((abs(poses[11] - 0.5f) > 0.302f));
}

float Resolver::SWResolver_pitch(C_BasePlayer* player)
{
	static auto weapon_accuracy_nospread = g_CVar->FindVar("weapon_accuracy_nospread");
	float returnval = networkedPlayerPitch[player->EntIndex()];

	if (weapon_accuracy_nospread->GetBool())
	{
		static bool cant_find_pitch[65] = { false };
		if (missed_shots[player->EntIndex()] > 5)
		{
			if (missed_shots[player->EntIndex()] > 30 || cant_find_pitch[player->EntIndex()])
			{
				switch (missed_shots[player->EntIndex()] % 9)
				{
				case 0: break;
				case 1: returnval *= -1; break;
				case 2: returnval *= 0.5; break;
				case 3: returnval *= -0.5; break;
				case 4: returnval *= 1.5; break;
				case 5: returnval *= -1.5; break;
				case 6: returnval = 89.f; break;
				case 7: returnval = -89.f; break;
				case 8: returnval = 0; break;
				}
			}
			else
			{
				float temp_var = Math::FindSmallestFake(returnval, missed_shots[player->EntIndex()] % 3);
				if (temp_var < -2000) cant_find_pitch[player->EntIndex()] = true;
				return Math::ComputeBodyPitch(temp_var);
			}
		}
		else
		{
			cant_find_pitch[player->EntIndex()] = false;
		}
	}

	return Math::ClampPitch(returnval);
}

bool Resolver::Is979MEME(C_BasePlayer* player)
{
	int animlayers = player->GetNumAnimOverlays();

	for (int i = 0; i < animlayers; i++)
	{
		if (player->GetSequenceActivity(player->GetAnimOverlay(i)->m_nSequence) == 979)
		{
			return true;
		}
	}

	return false;
}

float Resolver::AnimationResolve(C_BasePlayer* player)
{
	if (records[player->EntIndex()].front().suppresing_animation)
		return ResolveBruteforce(player, player->m_flLowerBodyYawTarget());

	if (!IsBreakingLBY(player))
	{
		Global::resolverModes[player->EntIndex()] = "LBY Not Broken (Anim)";
		return player->m_flLowerBodyYawTarget();
	}
	else
	{
		Global::resolverModes[player->EntIndex()] = "Animation Brute";
		auto poses = Animation::Get().GetPlayerAnimationInfo(player->EntIndex()).m_flPoseParameters;
		if (abs(poses[11] - 0.5) > 0.499f) //60+ delta
		{
			switch (missed_shots[player->EntIndex()] % 8)
			{
			case 0: return Math::ClampYaw(player->m_flLowerBodyYawTarget() + 60); break;
			case 1: return Math::ClampYaw(player->m_flLowerBodyYawTarget() - 60); break;
			case 2: return Math::ClampYaw(player->m_flLowerBodyYawTarget() - 58); break;
			case 3: return Math::ClampYaw(player->m_flLowerBodyYawTarget() + 58); break;
			case 4: return Math::ClampYaw(player->m_flLowerBodyYawTarget() - 49); break;
			case 5: return Math::ClampYaw(player->m_flLowerBodyYawTarget() + 40); break;
			case 6: return Math::ClampYaw(player->m_flLowerBodyYawTarget() - 35); break;
			case 7: return Math::ClampYaw(player->m_flLowerBodyYawTarget() + 35); break;
			}
		}
		else
		{								   //35-60 delta
			switch (missed_shots[player->EntIndex()] % 4)
			{
			case 0: return Math::ClampYaw(player->m_flLowerBodyYawTarget() + 47.5); break;
			case 1: return Math::ClampYaw(player->m_flLowerBodyYawTarget() + 55); break;
			case 2: return Math::ClampYaw(player->m_flLowerBodyYawTarget() - 47.5); break;
			case 3: return Math::ClampYaw(player->m_flLowerBodyYawTarget() - 55); break;
			}
		}
	}
}

float Resolver::OnAirBrute(C_BasePlayer* player)
{
	return Math::ClampYaw(player->m_flLowerBodyYawTarget() + ((missed_shots[player->EntIndex()] % 14) * 25.7f));
}

void Resolver::REALSelfWrittenResolver(int playerindex)
{
	if (!AngryPew::Get().CheckTarget(playerindex))
		return;

	auto player = C_BasePlayer::GetPlayerByIndex(playerindex);
	float freestanding_yaw;
	int freestanding_awall = IsFreestanding_awall(player);
	bool freestanding_thickness = false;
	//if (!freestanding_awall)
		//freestanding_thickness = IsFreestanding_thickness(player, freestanding_yaw);
	int last_clip[65] = { 0 };

	if (records[playerindex].size() < 2)
	{
		Global::resolverModes[player->EntIndex()] = "Resolver: Automatic Decision";
		player->m_angEyeAngles().pitch = SWResolver_pitch(player);
		return;
	}

	bool hs_only = false;

	auto record = records[playerindex].front();

	AnimationLayer curBalanceLayer, prevBalanceLayer;

	ResolveInfo curtickrecord;
	curtickrecord.SaveRecord(player);

	QAngle viewangles;
	g_EngineClient->GetViewAngles(viewangles);

	float at_target_yaw = Math::CalcAngle(g_LocalPlayer->GetAbsOrigin(), player->GetAbsOrigin()).yaw;

	if (XSystemCFG.hvh_resolver_override && g_InputSystem->IsButtonDown(XSystemCFG.hvh_resolver_override_key) && std::fabsf(Math::ClampYaw(viewangles.yaw - at_target_yaw)) < 50)
	{
		auto rotate = [](float lby, float yaw)	//better override
		{
			float delta = Math::ClampYaw(lby - yaw);
			if (fabs(delta) < 25.f)
				return lby;

			if (delta > 0.f)
				return yaw + 25.f;

			return yaw;
		};

		player->m_angEyeAngles().yaw = Math::ClampYaw(rotate(player->m_flLowerBodyYawTarget(), Math::ClampYaw(viewangles.yaw - at_target_yaw) > 0) ? at_target_yaw + 90.f : at_target_yaw - 90.f);

		Global::resolverModes[player->EntIndex()] = "Overriding";

		return;
	}

	static ConVar* weapon_accuracy_nospread = g_CVar->FindVar("weapon_accuracy_nospread"), * mp_damage_headshot_only = g_CVar->FindVar("mp_damage_headshot_only");
	if (checks::is_bad_ptr(weapon_accuracy_nospread))
		weapon_accuracy_nospread = g_CVar->FindVar("weapon_accuracy_nospread");
	if (checks::is_bad_ptr(mp_damage_headshot_only))
		mp_damage_headshot_only = g_CVar->FindVar("mp_damage_headshot_only");
	if (checks::is_bad_ptr(weapon_accuracy_nospread) || checks::is_bad_ptr(mp_damage_headshot_only))
		hs_only = false;
	else
		hs_only = (weapon_accuracy_nospread->GetBool() || mp_damage_headshot_only->GetBool());

	Global::bBaim[playerindex] = missed_shots[playerindex] > 10 && !hs_only;

	
	if (Choking_Packets(playerindex))
	{
		
		if (!record.suppresing_animation)
		{
			if (IsBreakingLBY(player))
			{
				for (int i = 1; abs(record.moving_lby_delta) < 5 && i < records[playerindex].size(); i++)
				{
					records[playerindex].front().moving_lby_delta = records[playerindex].at(i).moving_lby_delta;
					record = records[playerindex].front();
				}
			}
			else
			{
				for (int i = 1; abs(record.moving_lby_delta) > 35 && i < records[playerindex].size(); i++)
				{
					records[playerindex].front().moving_lby_delta = records[playerindex].at(i).moving_lby_delta;
					record = records[playerindex].front();
				}
			}
		}
		float cloest_dest = 9999.f; bool using_recorded_angle = false;
		if (missed_shots[playerindex] < 2 || Has_Static_Real(15, playerindex))
		{
			for (auto i = angle_records.begin(); i != angle_records.end(); i++)
			{
				if (i->handle != &player->GetRefEHandle() || i->position.DistTo(record.origin) > 64.f || i->position.DistTo(record.origin) > cloest_dest)
					continue;

				using_recorded_angle = true;
				Global::resolverModes[playerindex] = "Angle Recording";
				player->m_angEyeAngles() = i->angle;
				cloest_dest = i->position.DistTo(record.origin);
			}
		}

		if (record.shot)
		{
			Global::resolverModes[playerindex] = "Shot";
			if (!EventHandler.records2.empty()) player->m_angEyeAngles() = EventHandler.records2.back().direction;
			CMBacktracking::Get().SetOverwriteTick(player, player->m_angEyeAngles(), player->m_hActiveWeapon().Get()->m_fLastShotTime(), 2);
		}
		else if (!(player->m_fFlags() & FL_ONGROUND) && missed_shots[playerindex] > 1)
		{
			player->m_angEyeAngles().yaw = OnAirBrute(player);
			Global::resolverModes[playerindex] = "On-Air";
			Global::bBaim[playerindex] = (!hs_only && missed_shots[playerindex] > 5);
		}
		else if (record.moving && player->m_fFlags() & FL_ONGROUND && (!record.suppresing_animation || missed_shots[playerindex] < 2))
		{
			Global::resolverModes[playerindex] = "Moving";
			player->m_angEyeAngles().yaw = player->m_flLowerBodyYawTarget();
		}
		else if (record.was_moving && player->m_fFlags() & FL_ONGROUND && (!record.suppresing_animation || missed_shots[playerindex] < 2))
		{
			Global::resolverModes[playerindex] = "Was Moving";
			player->m_angEyeAngles().yaw = player->m_flLowerBodyYawTarget();
		}
		else if (record.update && record.saw_update)
		{
			Global::resolverModes[playerindex] = "LBY Update";
			player->m_angEyeAngles().yaw = player->m_flLowerBodyYawTarget();
			CMBacktracking::Get().SetOverwriteTick(player, record.last_update_angle, record.last_update_simtime, 2);
		}
		
		else if (abs(floor(record.lastlby_lby_delta) - record.lastlby_lby_delta) < 0.001 && abs(ceil(record.lastlby_lby_delta) - record.lastlby_lby_delta) < 0.001 && abs(record.lastlby_lby_delta) > 35 && missed_shots[playerindex] < 10)
		{
			Global::resolverModes[playerindex] = "Faulty LBY Breaker";
			player->m_angEyeAngles().yaw = Math::ClampYaw(player->m_flLowerBodyYawTarget() + record.lastlby_lby_delta);
		}
		else if (!record.update && !using_recorded_angle && freestanding_awall)
		{
			Global::resolverModes[playerindex] = "Fake: Reverse Freestanding";
			switch (freestanding_awall)
			{
			case 0:
				player->m_angEyeAngles().yaw = freestanding_yaw;
				break;
			case 1:
				player->m_angEyeAngles().yaw = Math::ClampYaw(Math::CalcAngle(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin()).yaw - 90);
				break;
			case 2:
				player->m_angEyeAngles().yaw = Math::ClampYaw(Math::CalcAngle(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin()).yaw + 90);
				break;
			case 3:
				player->m_angEyeAngles().yaw = Math::ClampYaw(Math::CalcAngle(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin()).yaw + 180);
				break;
			}
		}
		else if (missed_shots[playerindex] < 5 && !using_recorded_angle)
		{
			if (record.moving_lby > -1000)
			{
				float tickdiff, lbydiff, tickdiff2, freestandingyaw;

				if (Has_Static_Real(15, playerindex))
				{
					Global::resolverModes[playerindex] = "Fake: Static LBY Delta";
					player->m_angEyeAngles().yaw = Math::ClampYaw(player->m_flLowerBodyYawTarget() + record.moving_lby_delta);
				}
				else if (Has_Steady_Difference(10, playerindex))
				{
					Global::resolverModes[playerindex] = "Fake: Steady Delta";
					tickdiff = record.tickcount - records[playerindex].at(1).tickcount;
					lbydiff = record.lby - records[playerindex].at(1).lby;
					tickdiff2 = g_GlobalVars->tickcount - record.tickcount;
					player->m_angEyeAngles().yaw = Math::ClampYaw((lbydiff / tickdiff) * tickdiff2);
				}
				else if (Delta_Keeps_Changing(15, playerindex))
				{
					Global::resolverModes[playerindex] = "Fake: Dynamic Delta";
					if (GetDeltaByCompairingTicks(playerindex) > -1000)
						player->m_angEyeAngles().yaw = Math::ClampYaw(player->m_flLowerBodyYawTarget() + GetDeltaByCompairingTicks(playerindex));
					else
					{
						Global::bBaim[playerindex] = !hs_only;
						player->m_angEyeAngles().yaw = ResolveBruteforce(player, player->m_flLowerBodyYawTarget());
					}
				}
				else if (LBY_Keeps_Changing(15, playerindex))
				{
					Global::resolverModes[playerindex] = "Fake: Dynamic LBY";
					if (GetLBYByCompairingTicks(playerindex) > -1000)
						player->m_angEyeAngles().yaw = Math::ClampYaw(GetLBYByCompairingTicks(playerindex));
					else
					{
						Global::bBaim[playerindex] = !hs_only;
						player->m_angEyeAngles().yaw = ResolveBruteforce(player, player->m_flLowerBodyYawTarget());
					}
				}
			}
			else								// we dont have delta!
			{
				Global::bBaim[playerindex] = !hs_only;
				player->m_angEyeAngles().yaw = AnimationResolve(player);
			}
		}
		else if (!using_recorded_angle)
		{
			player->m_angEyeAngles().yaw = AnimationResolve(player);
		}
		if (!using_recorded_angle)
		{
			player->m_angEyeAngles().pitch = SWResolver_pitch(player);
		}

		records[playerindex].front().resolvedang = player->m_angEyeAngles();
	}
	else
	{
		player->m_angEyeAngles().pitch = SWResolver_pitch(player);
		player->m_angEyeAngles().yaw = Math::ClampYaw(networkedPlayerYaw[playerindex]);
		CMBacktracking::Get().SetOverwriteTick(player, player->m_angEyeAngles(), player->m_flSimulationTime(), 2);
		Global::resolverModes[playerindex] = "No Fake";
		return;
	}
	player->m_angEyeAngles().pitch = SWResolver_pitch(player);
	if (missed_shots[playerindex] < 3 || player->m_angEyeAngles().pitch < -50.f)
	{
		player->m_angEyeAngles().yaw = Math::ClampYaw(networkedPlayerYaw[playerindex]);
		Global::resolverModes[playerindex] = "No Fake";
		return;
	}
	else
	{
		player->m_angEyeAngles().yaw = Math::ClampYaw(networkedPlayerYaw[playerindex] + ((missed_shots[playerindex] % 4) * 19)) * ((missed_shots[playerindex] % 2) * 2 - 1);
		Global::resolverModes[playerindex] = "Bruteforce";
		return;
	}
}

void PredictResolve::log(C_BasePlayer* player)
{

}

float PredictResolve::predict(C_BasePlayer* player)
{
	int index = player->EntIndex();
	auto currecord = record[index];

	switch (currecord.CurrentPredictMode)
	{
	case PredictResolveModes::Static:
		return (player->m_flLowerBodyYawTarget() + currecord.DeltaFromPrediction);
	case PredictResolveModes::Spin:
		return predictSpin(player);
	case PredictResolveModes::Flips:
		return predictFlips(player);
	case PredictResolveModes::Freestand:
		return predictFreestand(player) + currecord.DeltaFromPrediction;

	}
}

float PredictResolve::predictSpin(C_BasePlayer* player)
{
	int index = player->EntIndex();
	auto currecord = record[index];
	float TimeDelta = currecord.LbyPredictedUpdate - currecord.LbyLastUpdate;
	Global::resolverModes[index] = "Predicted Spin";
	return (player->m_flLowerBodyYawTarget() + (currecord.SpinSpeed * TIME_TO_TICKS(TimeDelta)));
}

float PredictResolve::predictFlips(C_BasePlayer* player)
{
	int index = player->EntIndex();
	auto currecord = record[index];

	return 0.f;
}

float PredictResolve::predictFreestand(C_BasePlayer* player)
{
	int index = player->EntIndex();
	auto currecord = record[index];

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) return 0.f;
}

void PredictResolve::predictFuckIt(C_BasePlayer* player)
{
	int index = player->EntIndex();
	auto currecord = record[index];
	int MissedShot = Resolver::Get().missed_shots[index];

	float returnval;

	switch (MissedShot % 5)
	{
	case 0: currecord.DeltaFromPrediction = 0; currecord.SpinSpeed = 10; break;
	case 1: currecord.DeltaFromPrediction = 180.f; currecord.SpinSpeed = 30; break;
	case 2: currecord.DeltaFromPrediction = 90.f; currecord.SpinSpeed = -30; break;
	case 3: currecord.DeltaFromPrediction = -90.f; currecord.SpinSpeed = -10; break;
	case 4: currecord.DeltaFromPrediction = Utils::RandomFloat(-180.f, 180.f); currecord.SpinSpeed = Utils::RandomFloat(-270.f, 270.f);  break;
	}

	record[index] = currecord;

	switch (MissedShot % 4)
	{
	case 0: returnval = (player->m_flLowerBodyYawTarget() + currecord.DeltaFromPrediction); break;
	case 1: returnval = predictSpin(player); break;
	case 2: returnval = predictFlips(player); break;
	case 3: returnval = predictFreestand(player) + currecord.DeltaFromPrediction; break;
	}

	Global::resolverModes[index] = "Fake: Bruteforce";

}

void AimbotBulletImpactEvent::FireGameEvent(IGameEvent* event)
{
	if (!g_LocalPlayer || !event || records.empty())
		return;

	if (!strcmp(event->GetName(), "player_hurt"))
	{
		if (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() &&
			g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) == records.front().target)
		{
			records.front().processed = true;
			records.front().hit = true;
			if (event->GetInt("health") < 1) Resolver::Get().missed_shots[g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))] = 0;
		}
	}

	if (!strcmp(event->GetName(), "bullet_impact"))
	{
		if (g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) == g_EngineClient->GetLocalPlayer())
		{
			records.front().processed = true;
			records.front().impacts.push_back(Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z")));
		}
		else
		{
			if (records2.empty() || abs(records2.back().time - g_GlobalVars->curtime) > 0.1f)
				records2.push_back(g_EngineClient->GetPlayerForUserID(event->GetInt("userid")));

			records2.back().impacts.push_back(Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z")));
			records2.back().direction = Math::CalcAngle(records2.back().src, records2.back().impacts.front());
			for (auto i = records2.back().impacts.begin(); i != records2.back().impacts.end(); i++)
			{
				records2.back().direction += Math::CalcAngle(records2.back().src, *i);
				records2.back().direction /= 2;
			}
		}
	}
}

int AimbotBulletImpactEvent::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void AimbotBulletImpactEvent::RegisterSelf()
{
	g_GameEvents->AddListener(this, "player_hurt", false);
	g_GameEvents->AddListener(this, "bullet_impact", false);
}

void AimbotBulletImpactEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}

void SetupCapsule(const Vector& vecMin, const Vector& vecMax, float flRadius, int hitgroup, std::vector<CSphere>& m_cSpheres)
{
	auto vecDelta = (vecMax - vecMin);
	Math::NormalizeVector(vecDelta);
	auto vecCenter = vecMin;

	CSphere Sphere = CSphere{ vecMin, flRadius, hitgroup };
	m_cSpheres.push_back(Sphere);

	for (size_t i = 1; i < std::floor(vecMin.DistTo(vecMax)); ++i)
	{
		CSphere Sphere = CSphere{ vecMin + vecDelta * static_cast<float>(i), flRadius, hitgroup };
		m_cSpheres.push_back(CSphere{ Sphere });
	}

	CSphere UsedSphere = CSphere{ vecMin, flRadius, hitgroup };
	m_cSpheres.push_back(UsedSphere);
}

bool IntersectInfiniteRayWithSphere(const Vector& vecRayOrigin, const Vector& vecRayDelta, const Vector& vecSphereCenter, float flRadius, float* pT1, float* pT2)
{
	Vector vecSphereToRay;
	VectorSubtract(vecRayOrigin, vecSphereCenter, vecSphereToRay);

	float a = vecRayDelta.Dot(vecRayDelta);

	// This would occur in the case of a zero-length ray
	if (a == 0.0f) {
		*pT1 = *pT2 = 0.0f;
		return vecSphereToRay.LengthSqr() <= flRadius * flRadius;
	}

	float b = 2 * vecSphereToRay.Dot(vecRayDelta);
	float c = vecSphereToRay.Dot(vecSphereToRay) - flRadius * flRadius;
	float flDiscrim = b * b - 4 * a * c;
	if (flDiscrim < 0.0f)
		return false;

	flDiscrim = sqrt(flDiscrim);
	float oo2a = 0.5f / a;
	*pT1 = (-b - flDiscrim) * oo2a;
	*pT2 = (-b + flDiscrim) * oo2a;
	return true;
}

bool CSphere::intersectsRay(const Ray_t& ray, Vector& vecIntersection)
{
	float T1, T2;
	if (!IntersectInfiniteRayWithSphere(ray.m_Start, ray.m_Delta, m_vecCenter, m_flRadius, &T1, &T2))
		return false;

	if (T1 > 1.0f || T2 < 0.0f)
		return false;

	// Clamp it!
	if (T1 < 0.0f)
		T1 = 0.0f;
	if (T2 > 1.0f)
		T2 = 1.0f;

	vecIntersection = ray.m_Start + ray.m_Delta * T1;

	return true;
}

void AimbotBulletImpactEvent::process()
{
	static LagRecord last_LR[65];

	if (records.empty())
		return;

	for (auto i = records.begin(); i != records.end();)
	{
		auto player = C_BasePlayer::GetPlayerByIndex(i->target);
		if (!i->processed || abs(i->time - g_GlobalVars->curtime) < max(g_GlobalVars->interval_per_tick, g_GlobalVars->frametime) * 2)
		{
			i++;
			continue;
		}

		if (i->hit)
		{
			records.clear();
			break;
		}

		std::string mode = ((last_LR[i->target] == CMBacktracking::Get().current_record[i->target]) ? (Global::resolverModes[i->target]) : (CMBacktracking::Get().current_record[i->target].m_strResolveMode));

		bool trace_hit = false;

		Vector impact_to_use;
		float biggest_lenght = 0.f;

		auto matrix = CMBacktracking::Get().current_record[i->target].matrix;
		if (last_LR[i->target] == CMBacktracking::Get().current_record[i->target])
		{
			player->SetupBonesExperimental(matrix, 128, BONE_USED_BY_HITBOX, g_EngineClient->GetLastTimeStamp());
		}

		std::vector<CSphere> m_cSpheres;

		Ray_t Ray;
		trace_t Trace;

		studiohdr_t* studioHdr = g_MdlInfo->GetStudiomodel(player->GetModel());
		mstudiohitboxset_t* set = studioHdr->pHitboxSet(player->m_nHitboxSet());

		while (!i->impacts.empty())
		{
			if ((i->src - i->impacts.front()).Length() > biggest_lenght)
			{
				impact_to_use = i->impacts.front();
				biggest_lenght = (i->src - i->impacts.front()).Length();
			}
			i->impacts.erase(i->impacts.begin());
		}

		for (int h = 0; h < set->numhitboxes; h++)
		{
			auto hitbox = set->pHitbox(h);
			if (hitbox->m_flRadius != -1.f)
			{
				Vector min, max;

				Math::VectorTransform(hitbox->bbmin, matrix[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, matrix[hitbox->bone], max);

				SetupCapsule(min, max, hitbox->m_flRadius, hitbox->group, m_cSpheres);
			}
		}

		Ray.Init(i->src, impact_to_use);

		Vector intersectpos;
		for (auto& i : m_cSpheres)
		{
			if (i.intersectsRay(Ray, intersectpos))
			{
				trace_hit = true;
				break;
			}
		}

		EventInfo info;
		info.m_flExpTime = g_GlobalVars->curtime + 4.f;

		if (trace_hit)
		{
			Resolver::Get().missed_shots[i->target]++;

			g_CVar->ConsoleColorPrintf(Color(50, 122, 239), "[vsonyp0wer]");
			g_CVar->ConsoleColorPrintf(Color(255, 0, 0), " Missed shot due to bad resolver. ");
			g_CVar->ConsoleColorPrintf(Color(50, 122, 239), "(%f Damage expacted, used %s)\n", Global::lastdmg[i->target], mode.c_str());

			info.m_szMessage = "Missed shot due to bad resolver. (" + std::to_string(Global::lastdmg[i->target]) + " Damage expacted, used " + mode + ")";
			PlayerHurtEvent::Get().PushEvent(info);
		}
		else
		{
			g_CVar->ConsoleColorPrintf(Color(50, 122, 239), "[vsonyp0wer]");
			g_CVar->ConsoleColorPrintf(Color(255, 255, 255), " Missed shot due to spread.\n");

			info.m_szMessage = "Missed shot due to spread.";
			PlayerHurtEvent::Get().PushEvent(info);
		}

		last_LR[i->target] = CMBacktracking::Get().current_record[i->target];
		//i = records.erase(i);
		records.clear();
		break;
	}
}
















// Junk Code By Troll Face & Thaisen's Gen
void QXmjjmbbVdHuBPYtUBtRwpXttuolMHWgCSsDFaXR27992606() {     int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK4620878 = -104492145;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK97995818 = -564244253;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK49048809 = -504897103;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK90459890 = -820440006;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK64967255 = -244587585;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK869019 = -90662252;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK18401368 = -150301267;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK36434044 = -475084084;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK6433111 = -185726368;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK79010370 = -375732206;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK47138128 = -396442016;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK7636078 = -243643398;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK83761642 = -468474849;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK44263145 = -462764101;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK9526711 = -156197018;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK93830030 = -710694273;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK80364095 = -887149712;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK34047751 = -806426298;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK74573953 = -223954718;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK44962844 = -957357242;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK69508524 = -348388358;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK89687897 = -508665671;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK45673923 = -88194956;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK42331021 = -271504414;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK88322297 = 13444478;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK90978258 = -255017887;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK8678186 = -404981224;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK49566976 = -928536285;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK57464617 = -87191413;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK42355084 = -274337075;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK92828411 = -857815870;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK59151597 = 39700996;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK54021550 = -916043087;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK29677179 = -313048345;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK46484627 = -209230239;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK34858645 = -365679763;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK66411086 = -770309461;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK14204536 = -535231975;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK770024 = 19677572;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK91891094 = -880524734;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK73913939 = -944339944;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK67776060 = -108675724;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK29623826 = -615848162;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK62111717 = -841143732;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK83933700 = -156489865;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK35024333 = -848452364;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK93698623 = -874898878;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK42787873 = -304499522;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK90991701 = -449211696;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK96092800 = -192602859;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK48826628 = 43024737;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK10523067 = -216700731;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK30402489 = 44958508;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK45073780 = -207415415;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK58484082 = -596192775;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK14932980 = -595826475;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK52321896 = -376049297;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK6717789 = -133392690;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK2137593 = -733884484;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK73988997 = -989569699;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK92190833 = -685681028;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK68834392 = -221764983;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK78969426 = -287892671;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK64078027 = -911389294;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK86181958 = -517916336;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK87986530 = -336143013;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK53614527 = -327600312;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK54084464 = -55426505;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK97778518 = -153533862;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK74668065 = -790517256;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK27418945 = -940384813;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK66159559 = -251917738;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK33277728 = -726103870;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK82682859 = -343429984;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK71048904 = 86982702;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK1732464 = -139712634;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK60064072 = -892817509;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK83562205 = -247051225;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK58397320 = -15014549;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK53297965 = -138103159;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK97279635 = -380119010;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK65890312 = -481703;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK58575275 = -379324590;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK61371817 = -894588555;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK93528455 = -217361812;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK82305344 = -541115140;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK28749109 = 94742488;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK8947771 = -608627673;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK71193096 = -716855570;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK31551647 = -613403764;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK82536749 = -989630466;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK59693298 = -536916771;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK12066944 = -801347491;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK26781026 = 9247270;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK99700260 = -94843707;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK5079548 = -622574962;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK88806633 = -820783053;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK65545799 = -704458869;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK75929758 = -223227396;    int WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK95947169 = -104492145;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK4620878 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK97995818;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK97995818 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK49048809;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK49048809 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK90459890;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK90459890 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK64967255;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK64967255 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK869019;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK869019 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK18401368;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK18401368 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK36434044;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK36434044 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK6433111;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK6433111 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK79010370;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK79010370 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK47138128;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK47138128 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK7636078;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK7636078 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK83761642;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK83761642 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK44263145;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK44263145 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK9526711;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK9526711 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK93830030;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK93830030 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK80364095;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK80364095 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK34047751;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK34047751 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK74573953;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK74573953 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK44962844;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK44962844 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK69508524;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK69508524 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK89687897;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK89687897 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK45673923;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK45673923 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK42331021;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK42331021 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK88322297;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK88322297 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK90978258;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK90978258 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK8678186;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK8678186 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK49566976;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK49566976 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK57464617;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK57464617 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK42355084;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK42355084 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK92828411;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK92828411 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK59151597;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK59151597 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK54021550;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK54021550 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK29677179;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK29677179 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK46484627;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK46484627 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK34858645;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK34858645 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK66411086;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK66411086 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK14204536;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK14204536 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK770024;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK770024 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK91891094;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK91891094 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK73913939;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK73913939 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK67776060;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK67776060 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK29623826;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK29623826 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK62111717;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK62111717 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK83933700;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK83933700 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK35024333;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK35024333 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK93698623;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK93698623 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK42787873;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK42787873 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK90991701;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK90991701 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK96092800;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK96092800 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK48826628;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK48826628 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK10523067;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK10523067 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK30402489;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK30402489 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK45073780;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK45073780 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK58484082;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK58484082 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK14932980;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK14932980 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK52321896;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK52321896 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK6717789;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK6717789 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK2137593;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK2137593 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK73988997;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK73988997 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK92190833;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK92190833 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK68834392;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK68834392 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK78969426;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK78969426 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK64078027;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK64078027 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK86181958;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK86181958 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK87986530;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK87986530 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK53614527;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK53614527 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK54084464;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK54084464 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK97778518;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK97778518 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK74668065;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK74668065 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK27418945;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK27418945 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK66159559;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK66159559 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK33277728;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK33277728 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK82682859;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK82682859 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK71048904;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK71048904 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK1732464;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK1732464 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK60064072;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK60064072 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK83562205;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK83562205 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK58397320;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK58397320 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK53297965;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK53297965 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK97279635;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK97279635 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK65890312;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK65890312 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK58575275;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK58575275 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK61371817;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK61371817 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK93528455;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK93528455 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK82305344;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK82305344 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK28749109;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK28749109 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK8947771;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK8947771 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK71193096;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK71193096 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK31551647;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK31551647 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK82536749;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK82536749 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK59693298;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK59693298 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK12066944;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK12066944 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK26781026;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK26781026 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK99700260;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK99700260 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK5079548;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK5079548 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK88806633;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK88806633 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK65545799;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK65545799 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK75929758;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK75929758 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK95947169;     WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK95947169 = WDFdCSnBcOeYUvDbzsqoxpPNGarjWUnudcusktrbHmJydxMHOQENjjTgorWbaEeHbjurBK4620878;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ilqSWlLaSyaJXnhxMvhuaencoaTVUFWTXMiOaHGOHKJAwhTrFdE44487717() {     float AUetDxNdwSpGNcGmKESXFpTnPUnf33719434 = -236592814;    float AUetDxNdwSpGNcGmKESXFpTnPUnf38132743 = -141939909;    float AUetDxNdwSpGNcGmKESXFpTnPUnf3874736 = -507845703;    float AUetDxNdwSpGNcGmKESXFpTnPUnf14243433 = -641565051;    float AUetDxNdwSpGNcGmKESXFpTnPUnf48151263 = 94045235;    float AUetDxNdwSpGNcGmKESXFpTnPUnf11232597 = -150671072;    float AUetDxNdwSpGNcGmKESXFpTnPUnf13510445 = -175065600;    float AUetDxNdwSpGNcGmKESXFpTnPUnf74201400 = -463812908;    float AUetDxNdwSpGNcGmKESXFpTnPUnf13053119 = -155883163;    float AUetDxNdwSpGNcGmKESXFpTnPUnf13959169 = -883461152;    float AUetDxNdwSpGNcGmKESXFpTnPUnf42392854 = -929569205;    float AUetDxNdwSpGNcGmKESXFpTnPUnf51286750 = -665736154;    float AUetDxNdwSpGNcGmKESXFpTnPUnf61863859 = -187798788;    float AUetDxNdwSpGNcGmKESXFpTnPUnf96720108 = -748145894;    float AUetDxNdwSpGNcGmKESXFpTnPUnf42115899 = -485340510;    float AUetDxNdwSpGNcGmKESXFpTnPUnf54236194 = -105835700;    float AUetDxNdwSpGNcGmKESXFpTnPUnf64987719 = -400484628;    float AUetDxNdwSpGNcGmKESXFpTnPUnf1108948 = -196099918;    float AUetDxNdwSpGNcGmKESXFpTnPUnf24719934 = -600276153;    float AUetDxNdwSpGNcGmKESXFpTnPUnf5074670 = -75426758;    float AUetDxNdwSpGNcGmKESXFpTnPUnf56656025 = -902670232;    float AUetDxNdwSpGNcGmKESXFpTnPUnf55635578 = -294298206;    float AUetDxNdwSpGNcGmKESXFpTnPUnf62800909 = -557100232;    float AUetDxNdwSpGNcGmKESXFpTnPUnf93416790 = -963217141;    float AUetDxNdwSpGNcGmKESXFpTnPUnf90533599 = -977355055;    float AUetDxNdwSpGNcGmKESXFpTnPUnf68258695 = 20879804;    float AUetDxNdwSpGNcGmKESXFpTnPUnf92838153 = -590473239;    float AUetDxNdwSpGNcGmKESXFpTnPUnf88007695 = -733930166;    float AUetDxNdwSpGNcGmKESXFpTnPUnf59435492 = -754730787;    float AUetDxNdwSpGNcGmKESXFpTnPUnf21590052 = -817846164;    float AUetDxNdwSpGNcGmKESXFpTnPUnf38801683 = -861571777;    float AUetDxNdwSpGNcGmKESXFpTnPUnf23300160 = -128914401;    float AUetDxNdwSpGNcGmKESXFpTnPUnf30967808 = -975304992;    float AUetDxNdwSpGNcGmKESXFpTnPUnf4715968 = 88107288;    float AUetDxNdwSpGNcGmKESXFpTnPUnf85730033 = -654453138;    float AUetDxNdwSpGNcGmKESXFpTnPUnf57694551 = -393901338;    float AUetDxNdwSpGNcGmKESXFpTnPUnf35943442 = 16020272;    float AUetDxNdwSpGNcGmKESXFpTnPUnf75733923 = -577420080;    float AUetDxNdwSpGNcGmKESXFpTnPUnf28406675 = -535520327;    float AUetDxNdwSpGNcGmKESXFpTnPUnf60408587 = -142959047;    float AUetDxNdwSpGNcGmKESXFpTnPUnf7008990 = -822775298;    float AUetDxNdwSpGNcGmKESXFpTnPUnf79418775 = -323497232;    float AUetDxNdwSpGNcGmKESXFpTnPUnf83954522 = -126310268;    float AUetDxNdwSpGNcGmKESXFpTnPUnf17856697 = -930811813;    float AUetDxNdwSpGNcGmKESXFpTnPUnf56795320 = -986322836;    float AUetDxNdwSpGNcGmKESXFpTnPUnf89719595 = -428273960;    float AUetDxNdwSpGNcGmKESXFpTnPUnf35153354 = -431860745;    float AUetDxNdwSpGNcGmKESXFpTnPUnf55731374 = -679589109;    float AUetDxNdwSpGNcGmKESXFpTnPUnf17006344 = -981004317;    float AUetDxNdwSpGNcGmKESXFpTnPUnf17751410 = -332008709;    float AUetDxNdwSpGNcGmKESXFpTnPUnf66465517 = -702946727;    float AUetDxNdwSpGNcGmKESXFpTnPUnf47696310 = 7281450;    float AUetDxNdwSpGNcGmKESXFpTnPUnf13055627 = -382317831;    float AUetDxNdwSpGNcGmKESXFpTnPUnf74803574 = -354881922;    float AUetDxNdwSpGNcGmKESXFpTnPUnf87831691 = -682600877;    float AUetDxNdwSpGNcGmKESXFpTnPUnf78083855 = -942294609;    float AUetDxNdwSpGNcGmKESXFpTnPUnf75331834 = -584839678;    float AUetDxNdwSpGNcGmKESXFpTnPUnf10457945 = -544628562;    float AUetDxNdwSpGNcGmKESXFpTnPUnf23709833 = -664209996;    float AUetDxNdwSpGNcGmKESXFpTnPUnf79892567 = -926834569;    float AUetDxNdwSpGNcGmKESXFpTnPUnf18394443 = -560197834;    float AUetDxNdwSpGNcGmKESXFpTnPUnf25502749 = -441135434;    float AUetDxNdwSpGNcGmKESXFpTnPUnf14765909 = -709082122;    float AUetDxNdwSpGNcGmKESXFpTnPUnf91463066 = -338037000;    float AUetDxNdwSpGNcGmKESXFpTnPUnf75157486 = 78110625;    float AUetDxNdwSpGNcGmKESXFpTnPUnf19092694 = -700654805;    float AUetDxNdwSpGNcGmKESXFpTnPUnf20318942 = -690431163;    float AUetDxNdwSpGNcGmKESXFpTnPUnf57147891 = -175906076;    float AUetDxNdwSpGNcGmKESXFpTnPUnf10990075 = 6307244;    float AUetDxNdwSpGNcGmKESXFpTnPUnf84421348 = 8560828;    float AUetDxNdwSpGNcGmKESXFpTnPUnf18292752 = -21855973;    float AUetDxNdwSpGNcGmKESXFpTnPUnf89253796 = -823064549;    float AUetDxNdwSpGNcGmKESXFpTnPUnf72702273 = -660579591;    float AUetDxNdwSpGNcGmKESXFpTnPUnf64311346 = -357317107;    float AUetDxNdwSpGNcGmKESXFpTnPUnf98065680 = -252651461;    float AUetDxNdwSpGNcGmKESXFpTnPUnf77237250 = -479173001;    float AUetDxNdwSpGNcGmKESXFpTnPUnf71681056 = -67987938;    float AUetDxNdwSpGNcGmKESXFpTnPUnf44944212 = -626288420;    float AUetDxNdwSpGNcGmKESXFpTnPUnf36621471 = -976894305;    float AUetDxNdwSpGNcGmKESXFpTnPUnf814005 = -449081096;    float AUetDxNdwSpGNcGmKESXFpTnPUnf33105341 = -547259451;    float AUetDxNdwSpGNcGmKESXFpTnPUnf37106779 = -910884130;    float AUetDxNdwSpGNcGmKESXFpTnPUnf71001352 = -752925850;    float AUetDxNdwSpGNcGmKESXFpTnPUnf41684083 = -322722079;    float AUetDxNdwSpGNcGmKESXFpTnPUnf55124535 = -14899437;    float AUetDxNdwSpGNcGmKESXFpTnPUnf91105373 = -768853228;    float AUetDxNdwSpGNcGmKESXFpTnPUnf10244533 = -746596570;    float AUetDxNdwSpGNcGmKESXFpTnPUnf56164234 = -520423071;    float AUetDxNdwSpGNcGmKESXFpTnPUnf16884276 = -229291835;    float AUetDxNdwSpGNcGmKESXFpTnPUnf7646179 = -712158529;    float AUetDxNdwSpGNcGmKESXFpTnPUnf82362717 = -809061661;    float AUetDxNdwSpGNcGmKESXFpTnPUnf25485497 = -439351166;    float AUetDxNdwSpGNcGmKESXFpTnPUnf52024091 = -913210084;    float AUetDxNdwSpGNcGmKESXFpTnPUnf48514107 = -608685759;    float AUetDxNdwSpGNcGmKESXFpTnPUnf42014145 = -582761213;    float AUetDxNdwSpGNcGmKESXFpTnPUnf81506240 = -281639865;    float AUetDxNdwSpGNcGmKESXFpTnPUnf64652867 = -614415111;    float AUetDxNdwSpGNcGmKESXFpTnPUnf92491456 = -788273269;    float AUetDxNdwSpGNcGmKESXFpTnPUnf42699211 = -908922439;    float AUetDxNdwSpGNcGmKESXFpTnPUnf37702627 = -236592814;     AUetDxNdwSpGNcGmKESXFpTnPUnf33719434 = AUetDxNdwSpGNcGmKESXFpTnPUnf38132743;     AUetDxNdwSpGNcGmKESXFpTnPUnf38132743 = AUetDxNdwSpGNcGmKESXFpTnPUnf3874736;     AUetDxNdwSpGNcGmKESXFpTnPUnf3874736 = AUetDxNdwSpGNcGmKESXFpTnPUnf14243433;     AUetDxNdwSpGNcGmKESXFpTnPUnf14243433 = AUetDxNdwSpGNcGmKESXFpTnPUnf48151263;     AUetDxNdwSpGNcGmKESXFpTnPUnf48151263 = AUetDxNdwSpGNcGmKESXFpTnPUnf11232597;     AUetDxNdwSpGNcGmKESXFpTnPUnf11232597 = AUetDxNdwSpGNcGmKESXFpTnPUnf13510445;     AUetDxNdwSpGNcGmKESXFpTnPUnf13510445 = AUetDxNdwSpGNcGmKESXFpTnPUnf74201400;     AUetDxNdwSpGNcGmKESXFpTnPUnf74201400 = AUetDxNdwSpGNcGmKESXFpTnPUnf13053119;     AUetDxNdwSpGNcGmKESXFpTnPUnf13053119 = AUetDxNdwSpGNcGmKESXFpTnPUnf13959169;     AUetDxNdwSpGNcGmKESXFpTnPUnf13959169 = AUetDxNdwSpGNcGmKESXFpTnPUnf42392854;     AUetDxNdwSpGNcGmKESXFpTnPUnf42392854 = AUetDxNdwSpGNcGmKESXFpTnPUnf51286750;     AUetDxNdwSpGNcGmKESXFpTnPUnf51286750 = AUetDxNdwSpGNcGmKESXFpTnPUnf61863859;     AUetDxNdwSpGNcGmKESXFpTnPUnf61863859 = AUetDxNdwSpGNcGmKESXFpTnPUnf96720108;     AUetDxNdwSpGNcGmKESXFpTnPUnf96720108 = AUetDxNdwSpGNcGmKESXFpTnPUnf42115899;     AUetDxNdwSpGNcGmKESXFpTnPUnf42115899 = AUetDxNdwSpGNcGmKESXFpTnPUnf54236194;     AUetDxNdwSpGNcGmKESXFpTnPUnf54236194 = AUetDxNdwSpGNcGmKESXFpTnPUnf64987719;     AUetDxNdwSpGNcGmKESXFpTnPUnf64987719 = AUetDxNdwSpGNcGmKESXFpTnPUnf1108948;     AUetDxNdwSpGNcGmKESXFpTnPUnf1108948 = AUetDxNdwSpGNcGmKESXFpTnPUnf24719934;     AUetDxNdwSpGNcGmKESXFpTnPUnf24719934 = AUetDxNdwSpGNcGmKESXFpTnPUnf5074670;     AUetDxNdwSpGNcGmKESXFpTnPUnf5074670 = AUetDxNdwSpGNcGmKESXFpTnPUnf56656025;     AUetDxNdwSpGNcGmKESXFpTnPUnf56656025 = AUetDxNdwSpGNcGmKESXFpTnPUnf55635578;     AUetDxNdwSpGNcGmKESXFpTnPUnf55635578 = AUetDxNdwSpGNcGmKESXFpTnPUnf62800909;     AUetDxNdwSpGNcGmKESXFpTnPUnf62800909 = AUetDxNdwSpGNcGmKESXFpTnPUnf93416790;     AUetDxNdwSpGNcGmKESXFpTnPUnf93416790 = AUetDxNdwSpGNcGmKESXFpTnPUnf90533599;     AUetDxNdwSpGNcGmKESXFpTnPUnf90533599 = AUetDxNdwSpGNcGmKESXFpTnPUnf68258695;     AUetDxNdwSpGNcGmKESXFpTnPUnf68258695 = AUetDxNdwSpGNcGmKESXFpTnPUnf92838153;     AUetDxNdwSpGNcGmKESXFpTnPUnf92838153 = AUetDxNdwSpGNcGmKESXFpTnPUnf88007695;     AUetDxNdwSpGNcGmKESXFpTnPUnf88007695 = AUetDxNdwSpGNcGmKESXFpTnPUnf59435492;     AUetDxNdwSpGNcGmKESXFpTnPUnf59435492 = AUetDxNdwSpGNcGmKESXFpTnPUnf21590052;     AUetDxNdwSpGNcGmKESXFpTnPUnf21590052 = AUetDxNdwSpGNcGmKESXFpTnPUnf38801683;     AUetDxNdwSpGNcGmKESXFpTnPUnf38801683 = AUetDxNdwSpGNcGmKESXFpTnPUnf23300160;     AUetDxNdwSpGNcGmKESXFpTnPUnf23300160 = AUetDxNdwSpGNcGmKESXFpTnPUnf30967808;     AUetDxNdwSpGNcGmKESXFpTnPUnf30967808 = AUetDxNdwSpGNcGmKESXFpTnPUnf4715968;     AUetDxNdwSpGNcGmKESXFpTnPUnf4715968 = AUetDxNdwSpGNcGmKESXFpTnPUnf85730033;     AUetDxNdwSpGNcGmKESXFpTnPUnf85730033 = AUetDxNdwSpGNcGmKESXFpTnPUnf57694551;     AUetDxNdwSpGNcGmKESXFpTnPUnf57694551 = AUetDxNdwSpGNcGmKESXFpTnPUnf35943442;     AUetDxNdwSpGNcGmKESXFpTnPUnf35943442 = AUetDxNdwSpGNcGmKESXFpTnPUnf75733923;     AUetDxNdwSpGNcGmKESXFpTnPUnf75733923 = AUetDxNdwSpGNcGmKESXFpTnPUnf28406675;     AUetDxNdwSpGNcGmKESXFpTnPUnf28406675 = AUetDxNdwSpGNcGmKESXFpTnPUnf60408587;     AUetDxNdwSpGNcGmKESXFpTnPUnf60408587 = AUetDxNdwSpGNcGmKESXFpTnPUnf7008990;     AUetDxNdwSpGNcGmKESXFpTnPUnf7008990 = AUetDxNdwSpGNcGmKESXFpTnPUnf79418775;     AUetDxNdwSpGNcGmKESXFpTnPUnf79418775 = AUetDxNdwSpGNcGmKESXFpTnPUnf83954522;     AUetDxNdwSpGNcGmKESXFpTnPUnf83954522 = AUetDxNdwSpGNcGmKESXFpTnPUnf17856697;     AUetDxNdwSpGNcGmKESXFpTnPUnf17856697 = AUetDxNdwSpGNcGmKESXFpTnPUnf56795320;     AUetDxNdwSpGNcGmKESXFpTnPUnf56795320 = AUetDxNdwSpGNcGmKESXFpTnPUnf89719595;     AUetDxNdwSpGNcGmKESXFpTnPUnf89719595 = AUetDxNdwSpGNcGmKESXFpTnPUnf35153354;     AUetDxNdwSpGNcGmKESXFpTnPUnf35153354 = AUetDxNdwSpGNcGmKESXFpTnPUnf55731374;     AUetDxNdwSpGNcGmKESXFpTnPUnf55731374 = AUetDxNdwSpGNcGmKESXFpTnPUnf17006344;     AUetDxNdwSpGNcGmKESXFpTnPUnf17006344 = AUetDxNdwSpGNcGmKESXFpTnPUnf17751410;     AUetDxNdwSpGNcGmKESXFpTnPUnf17751410 = AUetDxNdwSpGNcGmKESXFpTnPUnf66465517;     AUetDxNdwSpGNcGmKESXFpTnPUnf66465517 = AUetDxNdwSpGNcGmKESXFpTnPUnf47696310;     AUetDxNdwSpGNcGmKESXFpTnPUnf47696310 = AUetDxNdwSpGNcGmKESXFpTnPUnf13055627;     AUetDxNdwSpGNcGmKESXFpTnPUnf13055627 = AUetDxNdwSpGNcGmKESXFpTnPUnf74803574;     AUetDxNdwSpGNcGmKESXFpTnPUnf74803574 = AUetDxNdwSpGNcGmKESXFpTnPUnf87831691;     AUetDxNdwSpGNcGmKESXFpTnPUnf87831691 = AUetDxNdwSpGNcGmKESXFpTnPUnf78083855;     AUetDxNdwSpGNcGmKESXFpTnPUnf78083855 = AUetDxNdwSpGNcGmKESXFpTnPUnf75331834;     AUetDxNdwSpGNcGmKESXFpTnPUnf75331834 = AUetDxNdwSpGNcGmKESXFpTnPUnf10457945;     AUetDxNdwSpGNcGmKESXFpTnPUnf10457945 = AUetDxNdwSpGNcGmKESXFpTnPUnf23709833;     AUetDxNdwSpGNcGmKESXFpTnPUnf23709833 = AUetDxNdwSpGNcGmKESXFpTnPUnf79892567;     AUetDxNdwSpGNcGmKESXFpTnPUnf79892567 = AUetDxNdwSpGNcGmKESXFpTnPUnf18394443;     AUetDxNdwSpGNcGmKESXFpTnPUnf18394443 = AUetDxNdwSpGNcGmKESXFpTnPUnf25502749;     AUetDxNdwSpGNcGmKESXFpTnPUnf25502749 = AUetDxNdwSpGNcGmKESXFpTnPUnf14765909;     AUetDxNdwSpGNcGmKESXFpTnPUnf14765909 = AUetDxNdwSpGNcGmKESXFpTnPUnf91463066;     AUetDxNdwSpGNcGmKESXFpTnPUnf91463066 = AUetDxNdwSpGNcGmKESXFpTnPUnf75157486;     AUetDxNdwSpGNcGmKESXFpTnPUnf75157486 = AUetDxNdwSpGNcGmKESXFpTnPUnf19092694;     AUetDxNdwSpGNcGmKESXFpTnPUnf19092694 = AUetDxNdwSpGNcGmKESXFpTnPUnf20318942;     AUetDxNdwSpGNcGmKESXFpTnPUnf20318942 = AUetDxNdwSpGNcGmKESXFpTnPUnf57147891;     AUetDxNdwSpGNcGmKESXFpTnPUnf57147891 = AUetDxNdwSpGNcGmKESXFpTnPUnf10990075;     AUetDxNdwSpGNcGmKESXFpTnPUnf10990075 = AUetDxNdwSpGNcGmKESXFpTnPUnf84421348;     AUetDxNdwSpGNcGmKESXFpTnPUnf84421348 = AUetDxNdwSpGNcGmKESXFpTnPUnf18292752;     AUetDxNdwSpGNcGmKESXFpTnPUnf18292752 = AUetDxNdwSpGNcGmKESXFpTnPUnf89253796;     AUetDxNdwSpGNcGmKESXFpTnPUnf89253796 = AUetDxNdwSpGNcGmKESXFpTnPUnf72702273;     AUetDxNdwSpGNcGmKESXFpTnPUnf72702273 = AUetDxNdwSpGNcGmKESXFpTnPUnf64311346;     AUetDxNdwSpGNcGmKESXFpTnPUnf64311346 = AUetDxNdwSpGNcGmKESXFpTnPUnf98065680;     AUetDxNdwSpGNcGmKESXFpTnPUnf98065680 = AUetDxNdwSpGNcGmKESXFpTnPUnf77237250;     AUetDxNdwSpGNcGmKESXFpTnPUnf77237250 = AUetDxNdwSpGNcGmKESXFpTnPUnf71681056;     AUetDxNdwSpGNcGmKESXFpTnPUnf71681056 = AUetDxNdwSpGNcGmKESXFpTnPUnf44944212;     AUetDxNdwSpGNcGmKESXFpTnPUnf44944212 = AUetDxNdwSpGNcGmKESXFpTnPUnf36621471;     AUetDxNdwSpGNcGmKESXFpTnPUnf36621471 = AUetDxNdwSpGNcGmKESXFpTnPUnf814005;     AUetDxNdwSpGNcGmKESXFpTnPUnf814005 = AUetDxNdwSpGNcGmKESXFpTnPUnf33105341;     AUetDxNdwSpGNcGmKESXFpTnPUnf33105341 = AUetDxNdwSpGNcGmKESXFpTnPUnf37106779;     AUetDxNdwSpGNcGmKESXFpTnPUnf37106779 = AUetDxNdwSpGNcGmKESXFpTnPUnf71001352;     AUetDxNdwSpGNcGmKESXFpTnPUnf71001352 = AUetDxNdwSpGNcGmKESXFpTnPUnf41684083;     AUetDxNdwSpGNcGmKESXFpTnPUnf41684083 = AUetDxNdwSpGNcGmKESXFpTnPUnf55124535;     AUetDxNdwSpGNcGmKESXFpTnPUnf55124535 = AUetDxNdwSpGNcGmKESXFpTnPUnf91105373;     AUetDxNdwSpGNcGmKESXFpTnPUnf91105373 = AUetDxNdwSpGNcGmKESXFpTnPUnf10244533;     AUetDxNdwSpGNcGmKESXFpTnPUnf10244533 = AUetDxNdwSpGNcGmKESXFpTnPUnf56164234;     AUetDxNdwSpGNcGmKESXFpTnPUnf56164234 = AUetDxNdwSpGNcGmKESXFpTnPUnf16884276;     AUetDxNdwSpGNcGmKESXFpTnPUnf16884276 = AUetDxNdwSpGNcGmKESXFpTnPUnf7646179;     AUetDxNdwSpGNcGmKESXFpTnPUnf7646179 = AUetDxNdwSpGNcGmKESXFpTnPUnf82362717;     AUetDxNdwSpGNcGmKESXFpTnPUnf82362717 = AUetDxNdwSpGNcGmKESXFpTnPUnf25485497;     AUetDxNdwSpGNcGmKESXFpTnPUnf25485497 = AUetDxNdwSpGNcGmKESXFpTnPUnf52024091;     AUetDxNdwSpGNcGmKESXFpTnPUnf52024091 = AUetDxNdwSpGNcGmKESXFpTnPUnf48514107;     AUetDxNdwSpGNcGmKESXFpTnPUnf48514107 = AUetDxNdwSpGNcGmKESXFpTnPUnf42014145;     AUetDxNdwSpGNcGmKESXFpTnPUnf42014145 = AUetDxNdwSpGNcGmKESXFpTnPUnf81506240;     AUetDxNdwSpGNcGmKESXFpTnPUnf81506240 = AUetDxNdwSpGNcGmKESXFpTnPUnf64652867;     AUetDxNdwSpGNcGmKESXFpTnPUnf64652867 = AUetDxNdwSpGNcGmKESXFpTnPUnf92491456;     AUetDxNdwSpGNcGmKESXFpTnPUnf92491456 = AUetDxNdwSpGNcGmKESXFpTnPUnf42699211;     AUetDxNdwSpGNcGmKESXFpTnPUnf42699211 = AUetDxNdwSpGNcGmKESXFpTnPUnf37702627;     AUetDxNdwSpGNcGmKESXFpTnPUnf37702627 = AUetDxNdwSpGNcGmKESXFpTnPUnf33719434;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wHbQebJhtVHsSAMJhPGezQFDb6790427() {     long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc71847573 = -674392265;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc91538898 = 25504470;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87988344 = -106781066;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc46541156 = -663812826;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc6798600 = -521111723;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc37109851 = -144732549;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc84979684 = -522012452;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc41326325 = -820381641;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc3532120 = -963723354;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc612003 = 5872267;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc1397072 = -863240153;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc93460477 = -419156063;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc34664629 = -563032333;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc20363680 = -796342337;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc91828439 = -797052942;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc8752562 = -825247930;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc43161345 = -455484366;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc70137496 = -349355775;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc98770776 = -658246317;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc15715405 = -797521643;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc453180 = -408180919;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc35233684 = -490815441;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87517400 = -9986811;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc80923162 = -485604244;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc89223999 = -995303490;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc70370418 = -248182762;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc8840771 = 30137270;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc30078841 = -838941164;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc5364674 = -421069154;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc69980852 = -683114667;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24916903 = -281731917;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87745082 = -486520711;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc72640004 = -3260212;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc75500338 = -258484221;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc5107160 = -898776683;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc97711491 = -727741535;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc23735362 = -657011120;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc53414829 = -176793531;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc45961982 = -633954347;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87204890 = -727957201;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc20904246 = -847819815;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc85558865 = -521265565;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc11052508 = -216435999;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc36299896 = -145078264;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc13582313 = -383333878;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc50421177 = -550254017;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc52391969 = -153595703;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc96458241 = -146583262;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24679419 = 1924753;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc28552498 = -526458167;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc67498102 = -818478216;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc67628927 = -5074860;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc27894828 = -489240035;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc98109036 = -535825598;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc6337247 = -492585520;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc36613890 = -83576825;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc4021499 = -964508720;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc7065183 = -621176823;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc57317156 = -668509337;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc36428181 = -172928961;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc28269081 = -74869819;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc54900843 = -683071289;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc35961651 = -299312488;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc33551267 = -180608688;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc75695099 = -712395817;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc13651989 = -276719442;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc20820474 = -315895852;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc59164290 = -204548112;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc15256521 = -897565654;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc94116948 = 30688593;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc85017200 = -68236811;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc89746515 = -178690835;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24175514 = -715401429;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc11565887 = -930289116;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc94811159 = -949701828;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc14894314 = -886915354;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24181176 = -174379443;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc51217504 = -864908548;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc67340849 = -2270366;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc38802823 = -345049473;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc17978449 = 5412940;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc12382529 = -823279468;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc5399423 = -740865917;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc76812175 = -894610987;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc2482751 = -864636451;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc57287976 = -176657057;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc59850254 = -997280676;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc74530968 = -467434614;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc69163091 = -765898702;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc68493270 = -715199859;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc93689992 = -763232816;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc16670179 = 64165703;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc96097672 = -508284195;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc9533802 = -361025386;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc58935809 = -553087382;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc66003402 = -64748527;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc49597215 = -121953078;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc77501241 = 64172689;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc60604796 = -432682447;    long siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc99930323 = -674392265;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc71847573 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc91538898;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc91538898 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87988344;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87988344 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc46541156;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc46541156 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc6798600;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc6798600 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc37109851;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc37109851 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc84979684;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc84979684 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc41326325;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc41326325 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc3532120;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc3532120 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc612003;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc612003 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc1397072;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc1397072 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc93460477;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc93460477 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc34664629;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc34664629 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc20363680;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc20363680 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc91828439;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc91828439 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc8752562;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc8752562 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc43161345;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc43161345 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc70137496;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc70137496 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc98770776;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc98770776 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc15715405;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc15715405 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc453180;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc453180 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc35233684;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc35233684 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87517400;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87517400 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc80923162;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc80923162 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc89223999;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc89223999 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc70370418;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc70370418 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc8840771;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc8840771 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc30078841;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc30078841 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc5364674;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc5364674 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc69980852;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc69980852 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24916903;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24916903 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87745082;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87745082 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc72640004;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc72640004 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc75500338;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc75500338 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc5107160;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc5107160 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc97711491;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc97711491 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc23735362;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc23735362 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc53414829;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc53414829 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc45961982;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc45961982 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87204890;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc87204890 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc20904246;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc20904246 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc85558865;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc85558865 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc11052508;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc11052508 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc36299896;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc36299896 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc13582313;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc13582313 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc50421177;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc50421177 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc52391969;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc52391969 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc96458241;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc96458241 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24679419;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24679419 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc28552498;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc28552498 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc67498102;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc67498102 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc67628927;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc67628927 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc27894828;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc27894828 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc98109036;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc98109036 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc6337247;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc6337247 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc36613890;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc36613890 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc4021499;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc4021499 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc7065183;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc7065183 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc57317156;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc57317156 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc36428181;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc36428181 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc28269081;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc28269081 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc54900843;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc54900843 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc35961651;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc35961651 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc33551267;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc33551267 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc75695099;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc75695099 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc13651989;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc13651989 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc20820474;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc20820474 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc59164290;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc59164290 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc15256521;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc15256521 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc94116948;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc94116948 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc85017200;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc85017200 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc89746515;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc89746515 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24175514;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24175514 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc11565887;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc11565887 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc94811159;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc94811159 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc14894314;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc14894314 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24181176;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc24181176 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc51217504;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc51217504 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc67340849;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc67340849 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc38802823;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc38802823 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc17978449;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc17978449 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc12382529;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc12382529 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc5399423;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc5399423 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc76812175;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc76812175 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc2482751;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc2482751 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc57287976;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc57287976 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc59850254;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc59850254 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc74530968;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc74530968 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc69163091;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc69163091 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc68493270;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc68493270 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc93689992;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc93689992 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc16670179;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc16670179 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc96097672;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc96097672 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc9533802;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc9533802 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc58935809;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc58935809 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc66003402;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc66003402 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc49597215;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc49597215 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc77501241;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc77501241 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc60604796;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc60604796 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc99930323;     siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc99930323 = siLeUFUbMKkTykLKYPKwCzgnIveAfhgCYomsyCYPnQGpc71847573;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void CxJTFOPcVOkrxjOodUoeSZGEDjaqLhrnMHIeH98112258() {     double wYsUIiRANxORpEQFyVdqF79156581 = 28378282;    double wYsUIiRANxORpEQFyVdqF90458308 = -601903390;    double wYsUIiRANxORpEQFyVdqF73402850 = -53973348;    double wYsUIiRANxORpEQFyVdqF44480152 = -742228808;    double wYsUIiRANxORpEQFyVdqF3502568 = -106904548;    double wYsUIiRANxORpEQFyVdqF19250854 = -429320792;    double wYsUIiRANxORpEQFyVdqF46123727 = -386333293;    double wYsUIiRANxORpEQFyVdqF81226404 = -817683430;    double wYsUIiRANxORpEQFyVdqF16433295 = -960789166;    double wYsUIiRANxORpEQFyVdqF47433014 = -826724046;    double wYsUIiRANxORpEQFyVdqF46552211 = -979072106;    double wYsUIiRANxORpEQFyVdqF29987044 = -449705606;    double wYsUIiRANxORpEQFyVdqF54717124 = 36474755;    double wYsUIiRANxORpEQFyVdqF79982315 = -706873314;    double wYsUIiRANxORpEQFyVdqF65207782 = -428565907;    double wYsUIiRANxORpEQFyVdqF12265396 = -362798330;    double wYsUIiRANxORpEQFyVdqF90999506 = -601242161;    double wYsUIiRANxORpEQFyVdqF33753639 = -11159180;    double wYsUIiRANxORpEQFyVdqF94455497 = -425924706;    double wYsUIiRANxORpEQFyVdqF62319443 = -65476622;    double wYsUIiRANxORpEQFyVdqF44594062 = -723049408;    double wYsUIiRANxORpEQFyVdqF63829452 = -539622227;    double wYsUIiRANxORpEQFyVdqF61154736 = -69639771;    double wYsUIiRANxORpEQFyVdqF24277700 = -578278734;    double wYsUIiRANxORpEQFyVdqF14645116 = -397241347;    double wYsUIiRANxORpEQFyVdqF65316590 = -75840645;    double wYsUIiRANxORpEQFyVdqF99411296 = -774611714;    double wYsUIiRANxORpEQFyVdqF64328492 = -502407572;    double wYsUIiRANxORpEQFyVdqF23049823 = 27784198;    double wYsUIiRANxORpEQFyVdqF53475858 = -74354558;    double wYsUIiRANxORpEQFyVdqF19332947 = -19743872;    double wYsUIiRANxORpEQFyVdqF10987276 = -714251099;    double wYsUIiRANxORpEQFyVdqF12018019 = -719511554;    double wYsUIiRANxORpEQFyVdqF11828559 = -768036625;    double wYsUIiRANxORpEQFyVdqF28485574 = -893319568;    double wYsUIiRANxORpEQFyVdqF69857557 = -449484952;    double wYsUIiRANxORpEQFyVdqF38688030 = -164993192;    double wYsUIiRANxORpEQFyVdqF172350 = -756781450;    double wYsUIiRANxORpEQFyVdqF13351971 = -41993878;    double wYsUIiRANxORpEQFyVdqF10430064 = -352222702;    double wYsUIiRANxORpEQFyVdqF29982546 = -39089347;    double wYsUIiRANxORpEQFyVdqF6828226 = 31947121;    double wYsUIiRANxORpEQFyVdqF50404858 = -201929333;    double wYsUIiRANxORpEQFyVdqF81877926 = -531861968;    double wYsUIiRANxORpEQFyVdqF69377528 = -876185010;    double wYsUIiRANxORpEQFyVdqF11457960 = -4200565;    double wYsUIiRANxORpEQFyVdqF79079166 = 48716917;    double wYsUIiRANxORpEQFyVdqF37499164 = -519416245;    double wYsUIiRANxORpEQFyVdqF18783971 = -744608174;    double wYsUIiRANxORpEQFyVdqF54767862 = -640706099;    double wYsUIiRANxORpEQFyVdqF42707085 = -868697415;    double wYsUIiRANxORpEQFyVdqF76189550 = -353672741;    double wYsUIiRANxORpEQFyVdqF599668 = -612153662;    double wYsUIiRANxORpEQFyVdqF35901518 = -242966924;    double wYsUIiRANxORpEQFyVdqF53518197 = -905012073;    double wYsUIiRANxORpEQFyVdqF15327130 = -431999491;    double wYsUIiRANxORpEQFyVdqF29303573 = -432263619;    double wYsUIiRANxORpEQFyVdqF49125150 = -475694614;    double wYsUIiRANxORpEQFyVdqF29835037 = -244987462;    double wYsUIiRANxORpEQFyVdqF38185978 = 68936096;    double wYsUIiRANxORpEQFyVdqF19839558 = -654709079;    double wYsUIiRANxORpEQFyVdqF81795234 = -883925722;    double wYsUIiRANxORpEQFyVdqF58176581 = -745467628;    double wYsUIiRANxORpEQFyVdqF62957436 = -786434609;    double wYsUIiRANxORpEQFyVdqF28100067 = -706980174;    double wYsUIiRANxORpEQFyVdqF35564935 = -164821008;    double wYsUIiRANxORpEQFyVdqF17969025 = -730194052;    double wYsUIiRANxORpEQFyVdqF42888565 = -195488621;    double wYsUIiRANxORpEQFyVdqF51496741 = -813553747;    double wYsUIiRANxORpEQFyVdqF95350225 = -979080955;    double wYsUIiRANxORpEQFyVdqF73577366 = -97805138;    double wYsUIiRANxORpEQFyVdqF90827157 = -844460712;    double wYsUIiRANxORpEQFyVdqF20401668 = -969165303;    double wYsUIiRANxORpEQFyVdqF84025433 = 26297995;    double wYsUIiRANxORpEQFyVdqF32336897 = 73612725;    double wYsUIiRANxORpEQFyVdqF37765837 = -654996529;    double wYsUIiRANxORpEQFyVdqF13424594 = -237692895;    double wYsUIiRANxORpEQFyVdqF79276809 = -537777803;    double wYsUIiRANxORpEQFyVdqF54900172 = -702093725;    double wYsUIiRANxORpEQFyVdqF3187156 = -293040782;    double wYsUIiRANxORpEQFyVdqF86237423 = -24557563;    double wYsUIiRANxORpEQFyVdqF61912133 = -155195469;    double wYsUIiRANxORpEQFyVdqF45544522 = -757799398;    double wYsUIiRANxORpEQFyVdqF68281961 = -331509704;    double wYsUIiRANxORpEQFyVdqF10768774 = -205657143;    double wYsUIiRANxORpEQFyVdqF43143396 = -666071132;    double wYsUIiRANxORpEQFyVdqF10387609 = -2097438;    double wYsUIiRANxORpEQFyVdqF76116500 = -376544630;    double wYsUIiRANxORpEQFyVdqF58310362 = -863024552;    double wYsUIiRANxORpEQFyVdqF13158444 = -361320077;    double wYsUIiRANxORpEQFyVdqF40553985 = 82778667;    double wYsUIiRANxORpEQFyVdqF89562879 = -689298579;    double wYsUIiRANxORpEQFyVdqF70337313 = -411793988;    double wYsUIiRANxORpEQFyVdqF75165993 = -10929975;    double wYsUIiRANxORpEQFyVdqF90590506 = -697513623;    double wYsUIiRANxORpEQFyVdqF48187311 = -155163626;    double wYsUIiRANxORpEQFyVdqF48651644 = -222585251;    double wYsUIiRANxORpEQFyVdqF87447421 = -415494725;    double wYsUIiRANxORpEQFyVdqF53777859 = -824881794;    double wYsUIiRANxORpEQFyVdqF33812593 = 28378282;     wYsUIiRANxORpEQFyVdqF79156581 = wYsUIiRANxORpEQFyVdqF90458308;     wYsUIiRANxORpEQFyVdqF90458308 = wYsUIiRANxORpEQFyVdqF73402850;     wYsUIiRANxORpEQFyVdqF73402850 = wYsUIiRANxORpEQFyVdqF44480152;     wYsUIiRANxORpEQFyVdqF44480152 = wYsUIiRANxORpEQFyVdqF3502568;     wYsUIiRANxORpEQFyVdqF3502568 = wYsUIiRANxORpEQFyVdqF19250854;     wYsUIiRANxORpEQFyVdqF19250854 = wYsUIiRANxORpEQFyVdqF46123727;     wYsUIiRANxORpEQFyVdqF46123727 = wYsUIiRANxORpEQFyVdqF81226404;     wYsUIiRANxORpEQFyVdqF81226404 = wYsUIiRANxORpEQFyVdqF16433295;     wYsUIiRANxORpEQFyVdqF16433295 = wYsUIiRANxORpEQFyVdqF47433014;     wYsUIiRANxORpEQFyVdqF47433014 = wYsUIiRANxORpEQFyVdqF46552211;     wYsUIiRANxORpEQFyVdqF46552211 = wYsUIiRANxORpEQFyVdqF29987044;     wYsUIiRANxORpEQFyVdqF29987044 = wYsUIiRANxORpEQFyVdqF54717124;     wYsUIiRANxORpEQFyVdqF54717124 = wYsUIiRANxORpEQFyVdqF79982315;     wYsUIiRANxORpEQFyVdqF79982315 = wYsUIiRANxORpEQFyVdqF65207782;     wYsUIiRANxORpEQFyVdqF65207782 = wYsUIiRANxORpEQFyVdqF12265396;     wYsUIiRANxORpEQFyVdqF12265396 = wYsUIiRANxORpEQFyVdqF90999506;     wYsUIiRANxORpEQFyVdqF90999506 = wYsUIiRANxORpEQFyVdqF33753639;     wYsUIiRANxORpEQFyVdqF33753639 = wYsUIiRANxORpEQFyVdqF94455497;     wYsUIiRANxORpEQFyVdqF94455497 = wYsUIiRANxORpEQFyVdqF62319443;     wYsUIiRANxORpEQFyVdqF62319443 = wYsUIiRANxORpEQFyVdqF44594062;     wYsUIiRANxORpEQFyVdqF44594062 = wYsUIiRANxORpEQFyVdqF63829452;     wYsUIiRANxORpEQFyVdqF63829452 = wYsUIiRANxORpEQFyVdqF61154736;     wYsUIiRANxORpEQFyVdqF61154736 = wYsUIiRANxORpEQFyVdqF24277700;     wYsUIiRANxORpEQFyVdqF24277700 = wYsUIiRANxORpEQFyVdqF14645116;     wYsUIiRANxORpEQFyVdqF14645116 = wYsUIiRANxORpEQFyVdqF65316590;     wYsUIiRANxORpEQFyVdqF65316590 = wYsUIiRANxORpEQFyVdqF99411296;     wYsUIiRANxORpEQFyVdqF99411296 = wYsUIiRANxORpEQFyVdqF64328492;     wYsUIiRANxORpEQFyVdqF64328492 = wYsUIiRANxORpEQFyVdqF23049823;     wYsUIiRANxORpEQFyVdqF23049823 = wYsUIiRANxORpEQFyVdqF53475858;     wYsUIiRANxORpEQFyVdqF53475858 = wYsUIiRANxORpEQFyVdqF19332947;     wYsUIiRANxORpEQFyVdqF19332947 = wYsUIiRANxORpEQFyVdqF10987276;     wYsUIiRANxORpEQFyVdqF10987276 = wYsUIiRANxORpEQFyVdqF12018019;     wYsUIiRANxORpEQFyVdqF12018019 = wYsUIiRANxORpEQFyVdqF11828559;     wYsUIiRANxORpEQFyVdqF11828559 = wYsUIiRANxORpEQFyVdqF28485574;     wYsUIiRANxORpEQFyVdqF28485574 = wYsUIiRANxORpEQFyVdqF69857557;     wYsUIiRANxORpEQFyVdqF69857557 = wYsUIiRANxORpEQFyVdqF38688030;     wYsUIiRANxORpEQFyVdqF38688030 = wYsUIiRANxORpEQFyVdqF172350;     wYsUIiRANxORpEQFyVdqF172350 = wYsUIiRANxORpEQFyVdqF13351971;     wYsUIiRANxORpEQFyVdqF13351971 = wYsUIiRANxORpEQFyVdqF10430064;     wYsUIiRANxORpEQFyVdqF10430064 = wYsUIiRANxORpEQFyVdqF29982546;     wYsUIiRANxORpEQFyVdqF29982546 = wYsUIiRANxORpEQFyVdqF6828226;     wYsUIiRANxORpEQFyVdqF6828226 = wYsUIiRANxORpEQFyVdqF50404858;     wYsUIiRANxORpEQFyVdqF50404858 = wYsUIiRANxORpEQFyVdqF81877926;     wYsUIiRANxORpEQFyVdqF81877926 = wYsUIiRANxORpEQFyVdqF69377528;     wYsUIiRANxORpEQFyVdqF69377528 = wYsUIiRANxORpEQFyVdqF11457960;     wYsUIiRANxORpEQFyVdqF11457960 = wYsUIiRANxORpEQFyVdqF79079166;     wYsUIiRANxORpEQFyVdqF79079166 = wYsUIiRANxORpEQFyVdqF37499164;     wYsUIiRANxORpEQFyVdqF37499164 = wYsUIiRANxORpEQFyVdqF18783971;     wYsUIiRANxORpEQFyVdqF18783971 = wYsUIiRANxORpEQFyVdqF54767862;     wYsUIiRANxORpEQFyVdqF54767862 = wYsUIiRANxORpEQFyVdqF42707085;     wYsUIiRANxORpEQFyVdqF42707085 = wYsUIiRANxORpEQFyVdqF76189550;     wYsUIiRANxORpEQFyVdqF76189550 = wYsUIiRANxORpEQFyVdqF599668;     wYsUIiRANxORpEQFyVdqF599668 = wYsUIiRANxORpEQFyVdqF35901518;     wYsUIiRANxORpEQFyVdqF35901518 = wYsUIiRANxORpEQFyVdqF53518197;     wYsUIiRANxORpEQFyVdqF53518197 = wYsUIiRANxORpEQFyVdqF15327130;     wYsUIiRANxORpEQFyVdqF15327130 = wYsUIiRANxORpEQFyVdqF29303573;     wYsUIiRANxORpEQFyVdqF29303573 = wYsUIiRANxORpEQFyVdqF49125150;     wYsUIiRANxORpEQFyVdqF49125150 = wYsUIiRANxORpEQFyVdqF29835037;     wYsUIiRANxORpEQFyVdqF29835037 = wYsUIiRANxORpEQFyVdqF38185978;     wYsUIiRANxORpEQFyVdqF38185978 = wYsUIiRANxORpEQFyVdqF19839558;     wYsUIiRANxORpEQFyVdqF19839558 = wYsUIiRANxORpEQFyVdqF81795234;     wYsUIiRANxORpEQFyVdqF81795234 = wYsUIiRANxORpEQFyVdqF58176581;     wYsUIiRANxORpEQFyVdqF58176581 = wYsUIiRANxORpEQFyVdqF62957436;     wYsUIiRANxORpEQFyVdqF62957436 = wYsUIiRANxORpEQFyVdqF28100067;     wYsUIiRANxORpEQFyVdqF28100067 = wYsUIiRANxORpEQFyVdqF35564935;     wYsUIiRANxORpEQFyVdqF35564935 = wYsUIiRANxORpEQFyVdqF17969025;     wYsUIiRANxORpEQFyVdqF17969025 = wYsUIiRANxORpEQFyVdqF42888565;     wYsUIiRANxORpEQFyVdqF42888565 = wYsUIiRANxORpEQFyVdqF51496741;     wYsUIiRANxORpEQFyVdqF51496741 = wYsUIiRANxORpEQFyVdqF95350225;     wYsUIiRANxORpEQFyVdqF95350225 = wYsUIiRANxORpEQFyVdqF73577366;     wYsUIiRANxORpEQFyVdqF73577366 = wYsUIiRANxORpEQFyVdqF90827157;     wYsUIiRANxORpEQFyVdqF90827157 = wYsUIiRANxORpEQFyVdqF20401668;     wYsUIiRANxORpEQFyVdqF20401668 = wYsUIiRANxORpEQFyVdqF84025433;     wYsUIiRANxORpEQFyVdqF84025433 = wYsUIiRANxORpEQFyVdqF32336897;     wYsUIiRANxORpEQFyVdqF32336897 = wYsUIiRANxORpEQFyVdqF37765837;     wYsUIiRANxORpEQFyVdqF37765837 = wYsUIiRANxORpEQFyVdqF13424594;     wYsUIiRANxORpEQFyVdqF13424594 = wYsUIiRANxORpEQFyVdqF79276809;     wYsUIiRANxORpEQFyVdqF79276809 = wYsUIiRANxORpEQFyVdqF54900172;     wYsUIiRANxORpEQFyVdqF54900172 = wYsUIiRANxORpEQFyVdqF3187156;     wYsUIiRANxORpEQFyVdqF3187156 = wYsUIiRANxORpEQFyVdqF86237423;     wYsUIiRANxORpEQFyVdqF86237423 = wYsUIiRANxORpEQFyVdqF61912133;     wYsUIiRANxORpEQFyVdqF61912133 = wYsUIiRANxORpEQFyVdqF45544522;     wYsUIiRANxORpEQFyVdqF45544522 = wYsUIiRANxORpEQFyVdqF68281961;     wYsUIiRANxORpEQFyVdqF68281961 = wYsUIiRANxORpEQFyVdqF10768774;     wYsUIiRANxORpEQFyVdqF10768774 = wYsUIiRANxORpEQFyVdqF43143396;     wYsUIiRANxORpEQFyVdqF43143396 = wYsUIiRANxORpEQFyVdqF10387609;     wYsUIiRANxORpEQFyVdqF10387609 = wYsUIiRANxORpEQFyVdqF76116500;     wYsUIiRANxORpEQFyVdqF76116500 = wYsUIiRANxORpEQFyVdqF58310362;     wYsUIiRANxORpEQFyVdqF58310362 = wYsUIiRANxORpEQFyVdqF13158444;     wYsUIiRANxORpEQFyVdqF13158444 = wYsUIiRANxORpEQFyVdqF40553985;     wYsUIiRANxORpEQFyVdqF40553985 = wYsUIiRANxORpEQFyVdqF89562879;     wYsUIiRANxORpEQFyVdqF89562879 = wYsUIiRANxORpEQFyVdqF70337313;     wYsUIiRANxORpEQFyVdqF70337313 = wYsUIiRANxORpEQFyVdqF75165993;     wYsUIiRANxORpEQFyVdqF75165993 = wYsUIiRANxORpEQFyVdqF90590506;     wYsUIiRANxORpEQFyVdqF90590506 = wYsUIiRANxORpEQFyVdqF48187311;     wYsUIiRANxORpEQFyVdqF48187311 = wYsUIiRANxORpEQFyVdqF48651644;     wYsUIiRANxORpEQFyVdqF48651644 = wYsUIiRANxORpEQFyVdqF87447421;     wYsUIiRANxORpEQFyVdqF87447421 = wYsUIiRANxORpEQFyVdqF53777859;     wYsUIiRANxORpEQFyVdqF53777859 = wYsUIiRANxORpEQFyVdqF33812593;     wYsUIiRANxORpEQFyVdqF33812593 = wYsUIiRANxORpEQFyVdqF79156581;}
// Junk Finished
