#include "PlayerHurt.hpp"

#include "../Structs.hpp"
#include "Visuals.hpp"
#include "Resolver.hpp"
#include "AngryPew.hpp"
#include "LagCompensation.hpp"
#include "..//Handlers.h"
#include "../Options.hpp"

#include <chrono>

std::vector<std::string> tt_tap =	//damm you, linker error
{
	"You just got tapped by an garbage self pasta meme, go get an refund",
	"self pasta meme > all",
	"Your p2c sucks ass",
	"Really? Getting tapped by paste meme? Damn that's horrable cheat",
	"learncpp.com owns me and all",
	"1",
	"1 tap by learncpp.com",
	"1 tap by unknowncheats.me",
	"1 tap by mpgh.net",
	"That's 1 tap on my book.",
	"I bet even ayyware can tap you",
	"I bet even texashook can tap you",
	"I bet even ezfrags can tap you",
	"forgot to toggle aa buddy?",
	"[nanosense] Hit gay fag in the head for 100 dmg (used No Fake resolver)"
};

std::vector<std::string> tt_tap_legit =
{
	"1 tap by learncpp.com",
	"1 tap by unknowncheats.me",
	"1 tap by mpgh.net",
	"You can't cheat on VAC secured server!",
	"You can't cheat on SMAC secured server!",
	"Don't worry, i'll get banned for sure!",
	"CSGO is a dead game why tf do people still play this shit",
	"admin? are you here? if you are, type ''!ban @all 0''",
	"Valve Anti Cheat? More like Valve Allow Cheat",
	"SourceMod Anti Cheat? More like SourceMod Allow Cheat",
	"VACnet is a joke. it doesn't do anything"
};

std::vector<std::string> tt_kill_legit =
{
	"Don't be noob! learncpp.com!",
	"www.AYYWARE.net | Premium CSGO MEME Cheat",
	"Stop being noob! Get unknowncheats.me!",
	"You can't cheat on VAC secured server!",
	"You can't cheat on SMAC secured server!",
	"Don't worry, i'll get banned for sure!",
	"CSGO is a dead game why tf do people still play this shit",
	"admin? are you here? if you are, type ''!ban @all 0''",
	"Valve Anti Cheat? More like Valve Allow Cheat",
	"SourceMod Anti Cheat? More like SourceMod Allow Cheat",
	"VACnet is a joke. it doesn't do anything"
};

std::vector<std::string> tt_baim =
{
	"baim > all",
	"baim is gay, except when i'm the one doing it",
	"hiding head against paste? you afraid you're gonna get tapped? by paste?",
	"wanna learn what makes p100 resolver? ''Global::Should_Baim[playerindex] = true;''",
};

std::vector<std::string> tt_tapped =
{
	"Fucking lag",
	"Nice 1 way",
	"ESP Broke",
	"LBY Backtrack is just the best",
	"I'm getting like 50% loss wtf is wrong with this server",
	"Nice serverside",
	"I was typing stop",
	"pure luck"
};

std::vector<std::string> tt_hittedhead =
{
	"I just FUCKING HATE LBY backtrack",
	"ahh my lby breaker broke from high ping",
	"I bet that was bruteforce and you were just lucky.",
	"B1G BACKTRACK",
	"nice, my esp is doing it again."
};

std::vector<std::string> tt_baimed =
{
	"you remember when i said ''baim is gay, except when i'm the one doing it''? yep. you're gay.",
	"baim on pasted antiaim? you can't resolve this?",
	"Hey, look! p2c is baiming paste cheat! it can't resolve pasted antiaim!",
	"ahh these p2c with thier p100 resolver.. you can't miss when you baim!",
	"why are you baiming? can't resolve pasted antiaim? go get an refund"
};

auto HitgroupToString = [](int hitgroup) -> std::string
{
	switch (hitgroup)
	{
	case HITGROUP_GENERIC:
		return "generic";
	case HITGROUP_HEAD:
		return "head";
	case HITGROUP_CHEST:
		return "chest";
	case HITGROUP_STOMACH:
		return "stomach";
	case HITGROUP_LEFTARM:
		return "left arm";
	case HITGROUP_RIGHTARM:
		return "right arm";
	case HITGROUP_LEFTLEG:
		return "left leg";
	case HITGROUP_RIGHTLEG:
		return "right leg";
	case 8:
		return "unknown";
	case HITGROUP_GEAR:
		return "gear";
	}
};

auto HitboxToHitGroup = [](int hitbox) -> int
{
	switch (hitbox)
	{
	case HITBOX_HEAD:
		return HITGROUP_HEAD;
	case HITBOX_CHEST:
	case HITBOX_LOWER_CHEST:
	case HITBOX_UPPER_CHEST:
		return HITGROUP_CHEST;
	case HITBOX_STOMACH:
	case HITBOX_PELVIS:
		return HITGROUP_STOMACH;
	case HITBOX_LEFT_FOOT:
	case HITBOX_LEFT_THIGH:
	case HITBOX_LEFT_CALF:
		return HITGROUP_LEFTLEG;
	case HITBOX_RIGHT_FOOT:
	case HITBOX_RIGHT_THIGH:
	case HITBOX_RIGHT_CALF:
		return HITGROUP_RIGHTLEG;
	case HITBOX_LEFT_HAND:
	case HITBOX_LEFT_FOREARM:
	case HITBOX_LEFT_UPPER_ARM:
		return HITGROUP_LEFTARM;
	case HITBOX_RIGHT_HAND:
	case HITBOX_RIGHT_FOREARM:
	case HITBOX_RIGHT_UPPER_ARM:
		return HITGROUP_RIGHTARM;
	default:
		return HITGROUP_GENERIC;
	}
};

void PlayerHurtEvent::PushEvent(EventInfo event)
{
	eventInfo.emplace_back(event);
}

void PlayerHurtEvent::FireGameEvent(IGameEvent* event)
{
	static LagRecord last_LR[65];

	if (!g_LocalPlayer || !event)
		return;

	if (!strcmp(event->GetName(), "player_hurt"))
	{
		if (XSystemCFG.visuals_others_hitmarker)
		{
			if (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() &&
				g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer() &&
				g_LocalPlayer->IsAlive())
			{
				bool done = false;
				for (auto i = hitMarkerInfo.rbegin(); i != hitMarkerInfo.rend(); i++)
				{
					if (i->m_iIndex == g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) && i->m_iDmg == 0)
					{
						i->m_iDmg = event->GetInt("dmg_health");
						i->m_iHitbox = event->GetInt("hitgroup");
						done = true;
						break;
					}
				}
				if (!done)
				{
					std::string mode = ((last_LR[g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))] == CMBacktracking::Get().current_record[g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))]) ? (Global::resolverModes[g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))]) : (CMBacktracking::Get().current_record[g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))].m_strResolveMode));
					last_LR[g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))] = CMBacktracking::Get().current_record[g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))];
					hitMarkerInfo.push_back({ g_GlobalVars->curtime, g_LocalPlayer->m_hActiveWeapon().Get()->m_flNextPrimaryAttack(), g_GlobalVars->curtime + 5.0f, event->GetInt("dmg_health"), g_EngineClient->GetPlayerForUserID(event->GetInt("userid")), event->GetInt("hitgroup"), mode });
				}

				if (event->GetInt("hitgroup") == 1)
				{
					g_EngineClient->ExecuteClientCmd((event->GetInt("health")) ? ("play buttons\\arena_switch_press_02.wav") : ("play player\\headshot1.wav"));
				}
				else
				{
					g_EngineClient->ExecuteClientCmd((event->GetInt("health")) ? ("play buttons\\arena_switch_press_02.wav") : ("play ui\\deathnotice.wav"));
				}
			}
		}

		if (XSystemCFG.esp_lagcompensated_hitboxes)
		{
			int32_t attacker = g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));
			int32_t userid = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));

			if (attacker == g_EngineClient->GetLocalPlayer() && userid != g_EngineClient->GetLocalPlayer())
				Visuals::DrawCapsuleOverlay(userid, 0.8f);
		}

		if (XSystemCFG.misc_logevents || XSystemCFG.hvh_resolver || XSystemCFG.misc_toxic_chatspamer)
		{
			EventInfo info;
			std::string msg;
			std::stringstream msg2;

			auto enemy = event->GetInt("userid");
			auto attacker = event->GetInt("attacker");
			auto remaining_health = event->GetString("health");
			auto dmg_to_health = event->GetString("dmg_health");
			auto hitgroup = event->GetInt("hitgroup");

			auto enemy_index = g_EngineClient->GetPlayerForUserID(enemy);
			auto attacker_index = g_EngineClient->GetPlayerForUserID(attacker);
			auto pEnemy = C_BasePlayer::GetPlayerByIndex(enemy_index);
			auto pAttacker = C_BasePlayer::GetPlayerByIndex(attacker_index);

			player_info_t attacker_info;
			player_info_t enemy_info;

			std::string str;

			if (pEnemy && pAttacker && g_EngineClient->GetPlayerInfo(attacker_index, &attacker_info) && g_EngineClient->GetPlayerInfo(enemy_index, &enemy_info))
			{
				static long timestamp = 0;
				long curTime;
				if (Global::use_ud_vmt)
					curTime = g_GlobalVars->realtime * 1000;	//idk why we were using chrono in the first place but i'm sticking with that
				else
					curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

				if (attacker_index == g_EngineClient->GetLocalPlayer())
				{
					std::string mode = ((last_LR[enemy_index] == CMBacktracking::Get().current_record[enemy_index]) ? (Global::resolverModes[enemy_index]) : (CMBacktracking::Get().current_record[enemy_index].m_strResolveMode));
					if (XSystemCFG.misc_logevents)
					{
						info.m_flExpTime = g_GlobalVars->curtime + 4.f;
						std::string szHitgroup = HitgroupToString(hitgroup);
						msg2 << "[nanosense] Shot at " << enemy_info.szName << " in the " << szHitgroup << " for " << dmg_to_health << " health " << "(" << remaining_health << " health remaining)";

					//	if (XSystemCFG.hvh_resolver)
					//	{
					//		msg2 << ", using " << mode << ".)";
					//	}
					//	else
					//	{
					///		msg2 << ")";
					//	}
						info.m_szMessage = msg2.str();
						eventInfo.emplace_back(info);

						g_CVar->ConsoleColorPrintf(Color(50, 122, 239), "[nanosense]");
						g_CVar->ConsoleDPrintf(" ""Shot at"" ");
						g_CVar->ConsoleColorPrintf(Color(255, 255, 255), "%s", enemy_info.szName);
						g_CVar->ConsoleColorPrintf(Color(255, 255, 255), " in the %s", szHitgroup.c_str());
						g_CVar->ConsoleDPrintf(" ""for"" ");
						g_CVar->ConsoleColorPrintf(Color(255, 255, 255), "%s health", dmg_to_health);
						g_CVar->ConsoleColorPrintf(Color(255, 255, 255), " (%s hp remaining", remaining_health);
						//if (XSystemCFG.hvh_resolver)
						//	g_CVar->ConsoleColorPrintf(Color(255, 255, 255), ", using %s.)\n", mode.c_str());
						//else
						g_CVar->ConsoleColorPrintf(Color(255, 255, 255), ")\n");
					}

					if (XSystemCFG.angrpwenabled && XSystemCFG.hvh_resolver)
					{
						//if (event->GetInt("health") < 1)
						//	Resolver::Get().missed_shots[enemy_index] = 0;
						//else
						//	Resolver::Get().missed_shots[enemy_index]--;

						if (hitgroup == 1 && event->GetInt("health") == 0 && !(mode == "LBY Update" || mode == "No Fakes" || mode == "Shot" || Global::resolverModes[enemy_index].find("Moving") != std::string::npos) && !Resolver::Get().records[enemy_index].empty())
						{
							auto record = Resolver::Get().records[enemy_index].front(); bool should_record = true;
							for (auto i = Resolver::Get().angle_records.begin(); i != Resolver::Get().angle_records.end(); i++)
							{
								if (i->handle == &pEnemy->GetRefEHandle() && i->position.DistTo((last_LR[enemy_index] == CMBacktracking::Get().current_record[enemy_index]) ? record.origin : CMBacktracking::Get().current_record[enemy_index].m_vecOrigin) < 128.f)
								{
									should_record = false;
									break;
								}
							}

							if (should_record)
							{
								angle_recording temp; temp.SaveRecord(pEnemy, (last_LR[enemy_index] == CMBacktracking::Get().current_record[enemy_index]) ? record.origin : CMBacktracking::Get().current_record[enemy_index].m_vecOrigin, record.resolvedang);
								Resolver::Get().angle_records.push_front(temp);
							}
						}
					}

					if (XSystemCFG.hvh_resolver && hitgroup == 1)
					{
						Global::hit_while_brute[enemy_index] = true;
					}

					if (XSystemCFG.misc_toxic_chatspamer == 1)
					{
						if ((curTime - timestamp) > 850)
						{
							if (hitgroup == 1 && (event->GetInt("dmg_health") >= 100 || event->GetInt("health") == 0))
							{
								msg = tt_tap[Utils::RandomInt(0, tt_tap.size() - 1)];
							}
							else if (hitgroup == 1)
							{
								msg.append(dmg_to_health);
								if (event->GetInt("dmg_health") < 75)
									msg.append(" damage on head? that's bullshit");
								else
									msg.append(" damage on head.. that was close");
							}
							else if (hitgroup > 3 && hitgroup < 8)
							{
								if (hitgroup < 6)
									msg = "kys";
								else
									msg = "kys";
							}
							else if (hitgroup == 0)
							{
								if (AngryPew::Get().local_weapon->IsWeaponNonAim())
									msg = "kys";
								else
									msg = "kys";
							}
							else
							{
								msg = tt_baim[Utils::RandomInt(0, tt_baim.size() - 1)];
							}
							str.append("say ");
							str.append(msg);
							g_EngineClient->ExecuteClientCmd(str.c_str());

							timestamp = curTime;
						}
					}
					last_LR[enemy_index] = CMBacktracking::Get().current_record[enemy_index];
				}
				else if (enemy_index == g_EngineClient->GetLocalPlayer())
				{
					if (XSystemCFG.misc_toxic_chatspamer == 1)
					{
						if ((curTime - timestamp) > 850)
						{
							auto awall1 = AngryPew::Get().GetDamageVec2(pEnemy->GetEyePos(), g_LocalPlayer->GetEyePos(), g_LocalPlayer, pEnemy, HITBOX_HEAD);
							auto awall2 = AngryPew::Get().GetDamageVec2(g_LocalPlayer->GetEyePos(), pEnemy->GetEyePos(), g_LocalPlayer, pEnemy, HITBOX_HEAD);
							if (awall1.damage - awall2.damage > 25 || (awall1.damage > 10 && awall2.damage < 5))
							{
								msg = "[nanosense] ";
								msg += pEnemy->GetName();
								msg += " is hiding in 1 way spot like a fucking coward. (";
								msg += awall1.damage;
								msg += "maximum damage possable on he(she)'s spot, ";
								msg += awall2.damage;
								msg += "on my side.)";
							}
							else if (pEnemy->IsDormant())
							{
								msg = "[nanosense] ";
								msg += pEnemy->GetName();
								msg += " is hiding in 1 way spot like a fucking coward.";
							}
							else if (event->GetInt("dmg_health") >= 100)
							{
								msg = tt_tapped[Utils::RandomInt(0, tt_tapped.size() - 1)];
							}
							else if (hitgroup == 1)
							{
								msg = tt_hittedhead[Utils::RandomInt(0, tt_hittedhead.size() - 1)];
							}
							else if (hitgroup == 0)
							{
								msg = "ohh, nade? you're too scared to peek at a pasted cheat?";
							}
							else
							{
								msg = tt_baimed[Utils::RandomInt(0, tt_baimed.size() - 1)];
							}
							str.append("say ");
							str.append(msg);
							g_EngineClient->ExecuteClientCmd(str.c_str());

							timestamp = curTime;
						}
					}
				}
			}
		}
		else
			if (eventInfo.size() > 0)
				eventInfo.clear();

		if (XSystemCFG.hvh_resolver)
		{
			int32_t userid = event->GetInt("userid");
			auto player = C_BasePlayer::GetPlayerByUserId(userid);
			if (!player)
				return;

			int32_t idx = player->EntIndex();
			auto& player_recs = Resolver::Get().arr_infos[idx];

			if (!player->IsDormant())
			{
				int32_t local_id = g_EngineClient->GetLocalPlayer();
				int32_t attacker = g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));

				if (attacker == local_id)
				{
					bool airJump = !(player->m_fFlags() & FL_ONGROUND) && player->m_vecVelocity().Length2D() > 1.0f;
					int32_t tickcount = g_GlobalVars->tickcount;

					if (tickHitWall == tickcount)
					{
						player_recs.m_nShotsMissed = originalShotsMissed;
						player_recs.m_nCorrectedFakewalkIdx = originalCorrectedFakewalkIdx;
					}
					if (tickcount != tickHitPlayer)
					{
						tickHitPlayer = tickcount;
						player_recs.m_nShotsMissed = 0;

						if (!airJump)
						{
							if (++player_recs.m_nCorrectedFakewalkIdx > 7)
								player_recs.m_nCorrectedFakewalkIdx = 0;
						}
					}
				}
			}
		}
	}

	if (!strcmp(event->GetName(), "weapon_fire"))
	{
		if (g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) == g_EngineClient->GetLocalPlayer())
		{
			//if (XSystemCFG.angrpwenabled && XSystemCFG.hvh_resolver && !Aimhnly::Get().local_weapon->IsMiscellaneousWeapon() && Aimhnly::Get().CheckTarget(Global::aimbot_target)) Resolver::Get().missed_shots[Global::aimbot_target]++;
			if (XSystemCFG.visuals_others_hitmarker && g_LocalPlayer->IsAlive() && AngryPew::Get().CheckTarget(Global::aimbot_target))
			{
				std::string mode = ((last_LR[Global::aimbot_target] == CMBacktracking::Get().current_record[Global::aimbot_target]) ? (Global::resolverModes[Global::aimbot_target]) : (CMBacktracking::Get().current_record[Global::aimbot_target].m_strResolveMode));
				last_LR[Global::aimbot_target] = CMBacktracking::Get().current_record[Global::aimbot_target];
				hitMarkerInfo.push_back({ g_GlobalVars->curtime, g_LocalPlayer->m_hActiveWeapon().Get()->m_flNextPrimaryAttack(), g_GlobalVars->curtime + 5.0f, 0, Global::aimbot_target, HitboxToHitGroup(Global::aim_hitbox), mode });
			}
		}
		else if (XSystemCFG.misc_toxic_chatspamer == 1)
		{
			std::string weapon = event->GetString("weapon");
			if (weapon.find("awp") != std::string::npos)
			{
				auto pEnemy = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetPlayerForUserID(event->GetInt("userid")));
				std::string str = "say ";
				str += pEnemy->GetName();
				str += " is gay enough to use ";
				str += weapon;
				str += " on hvh server.";
				g_EngineClient->ExecuteClientCmd(str.c_str());
			}
		}
	}

	if (!strcmp(event->GetName(), "player_death") && XSystemCFG.misc_toxic_chatspamer == 2)
	{
		static float timestamp = 0;
		int local = g_EngineClient->GetLocalPlayer();
		int target = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));
		int killer = g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));
		bool hs = event->GetBool("headshot");
		bool pen = event->GetInt("penetrated") > 2;
		if (local == target || local == killer)
		{
			if (g_GlobalVars->curtime - timestamp > 0.85)
			{
				timestamp = g_GlobalVars->curtime;
				std::string str = "say ";
				if (local == killer)
				{
					str += (hs) ? (tt_tap_legit[Utils::RandomInt(0, tt_tap_legit.size() - 1)]) : (tt_kill_legit[Utils::RandomInt(0, tt_kill_legit.size() - 1)]);
				}
				else
				{
					auto pEnemy = C_BasePlayer::GetPlayerByIndex(killer);

					if (hs)
					{
						str += "Nice resolver, ";
						str += pEnemy->GetName();
						str += ". wanna hvh?";
					}
					else if (pen)
					{
						str += "Nice autowall, ";
						str += pEnemy->GetName();
						str += ". wanna hvh?";
					}
					else
					{
						str += "You just got lucky, ";
						str += pEnemy->GetName();
						str += ".";
					}
				}
				g_EngineClient->ExecuteClientCmd(str.c_str());
			}
		}
	}
	if (!strcmp(event->GetName(), "grenade_thrown") && g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer() && XSystemCFG.misc_toxic_chatspamer == 1)
	{
		std::string weapon = event->GetString("weapon");
		if (weapon.find("smoke") == std::string::npos && weapon.find("flash") == std::string::npos && weapon.find("decoy") == std::string::npos)
		{
			auto pEnemy = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetPlayerForUserID(event->GetInt("userid")));
			std::string str = "say ";
			str += pEnemy->GetName();
			str += " is gay enough to use ";
			str += weapon;
			str += " on hvh server.";
			g_EngineClient->ExecuteClientCmd(str.c_str());
		}
	}
}

int PlayerHurtEvent::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void PlayerHurtEvent::RegisterSelf()
{
	g_GameEvents->AddListener(this, "player_hurt", false);
	g_GameEvents->AddListener(this, "player_death", false);
	g_GameEvents->AddListener(this, "grenade_thrown", false);
	g_GameEvents->AddListener(this, "weapon_fire", false);
	g_GameEvents->AddListener(this, "bullet_impact", false);
}

void PlayerHurtEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}

void PlayerHurtEvent::Paint(void)
{
	static int width = 0, height = 0;
	if (width == 0 || height == 0)
		g_EngineClient->GetScreenSize(width, height);

	RECT scrn = Visuals::GetViewport();

	if (eventInfo.size() > 15)
		eventInfo.erase(eventInfo.begin() + 1);

	float alpha = 0.f;

	if (XSystemCFG.visuals_others_hitmarker)
	{
		for (size_t i = 0; i < hitMarkerInfo.size(); i++)
		{
			float diff = hitMarkerInfo.at(i).m_flExpTime - g_GlobalVars->curtime;

			if (diff < 0.f || diff > 6.0f)
			{
				hitMarkerInfo.erase(hitMarkerInfo.begin() + i);
				continue;
			}
		}

		const float h = 14;
		Vector2D Pos = { (float)width / 2 + 18, (float)height / 2 + 18 - (h * hitMarkerInfo.size()) };

		Vector2D curpos = Pos;
		for (size_t i = 0; i < hitMarkerInfo.size(); i++)
		{
			float diff = hitMarkerInfo.at(i).m_flExpTime - g_GlobalVars->curtime;
			float height_offset = (diff < 1.0f) ? (pow(((1.0f - diff) * 100), 1.2) * -1) : (0);
			alpha = min((hitMarkerInfo.at(i).m_flExpTime - g_GlobalVars->curtime) / 1.0f, 0.3f);

			int w1, w2, w3, h1, h2, h3; Visuals::GetTextSize(Visuals::ui_font, std::to_string(hitMarkerInfo.at(i).m_iDmg).c_str(), w1, h1); Visuals::GetTextSize(Visuals::ui_font, hitMarkerInfo.at(i).m_szMethod.c_str(), w2, h2); Visuals::GetTextSize(Visuals::ui_font, HitgroupToString(hitMarkerInfo.at(i).m_iHitbox).c_str(), w3, h3);
			if (XSystemCFG.hvh_resolver) w1 += 3;
			else w2 = h2 = 0;

			g_VGuiSurface->DrawSetColor(Color(0, 0, 0, (int)(alpha * 255.f))); g_VGuiSurface->DrawFilledRect(curpos.x, curpos.y - (h1 / 2) + height_offset, curpos.x + 10 + w1 + w2 + w3, curpos.y + 4 + (h1 / 2) + height_offset);
			curpos.y += h;
		}
		curpos = Pos;
		for (size_t i = 0; i < hitMarkerInfo.size(); i++)
		{
			float diff = hitMarkerInfo.at(i).m_flExpTime - g_GlobalVars->curtime;
			float height_offset = (diff < 1.0f) ? (pow(((1.0f - diff) * 100), 1.2) * -1) : (0);
			alpha = min((hitMarkerInfo.at(i).m_flExpTime - g_GlobalVars->curtime) / 1.0f, 1.0f);
			int w1, w2, w3, h1, h2, h3; Visuals::GetTextSize(Visuals::ui_font, std::to_string(hitMarkerInfo.at(i).m_iDmg).c_str(), w1, h1); Visuals::GetTextSize(Visuals::ui_font, hitMarkerInfo.at(i).m_szMethod.c_str(), w2, h2); Visuals::GetTextSize(Visuals::ui_font, HitgroupToString(hitMarkerInfo.at(i).m_iHitbox).c_str(), w3, h3);
			if (XSystemCFG.hvh_resolver) w1 += 3;
			else w2 = h2 = 0;

			Visuals::DrawString(Visuals::ui_font, curpos.x + 2, curpos.y + 2 + height_offset, Color(255, 255, 255, (int)(alpha * 255.f)), FONT_LEFT, HitgroupToString(hitMarkerInfo.at(i).m_iHitbox).c_str());
			Visuals::DrawString(Visuals::ui_font, curpos.x + 5 + w3, curpos.y + 2 + height_offset, Color(255, 255, 255, (int)(alpha * 255.f)), FONT_LEFT, std::to_string(hitMarkerInfo.at(i).m_iDmg).c_str());
			if (XSystemCFG.hvh_resolver) Visuals::DrawString(Visuals::ui_font, curpos.x + 5 + w1 + w3, curpos.y + 2 + height_offset, Color(255, 255, 255, (int)(alpha * 255.f)), FONT_LEFT, hitMarkerInfo.at(i).m_szMethod.c_str());
			curpos.y += h;
		}

		if (hitMarkerInfo.size() > 0)
		{
			int lineSize = 12;
			alpha = 0;
			for (auto i = hitMarkerInfo.rbegin(); i != hitMarkerInfo.rend(); i++)
			{
				if (i->m_iDmg > 0)
					alpha = min(((i->m_flCurTime + 0.8f) - g_GlobalVars->curtime) / 0.8f, 0);
			}
			g_VGuiSurface->DrawSetColor(Color(255, 255, 255, (int)(alpha * 255.f)));
			g_VGuiSurface->DrawLine(width / 2 - lineSize / 2, height / 2 - lineSize / 2, width / 2 + lineSize / 2, height / 2 + lineSize / 2);
			g_VGuiSurface->DrawLine(width / 2 + lineSize / 2, height / 2 - lineSize / 2, width / 2 - lineSize / 2, height / 2 + lineSize / 2);
		}
	}

	if (XSystemCFG.misc_logevents)
	{
		for (size_t i = 0; i < eventInfo.size(); i++)
		{
			float diff = eventInfo[i].m_flExpTime - g_GlobalVars->curtime;
			if (eventInfo[i].m_flExpTime < g_GlobalVars->curtime)
			{
				eventInfo.erase(eventInfo.begin() + i);
				continue;
			}

			alpha = 0.8f - diff / 0.8f;
			//																											was 9.5
			Visuals::DrawString(Visuals::eventlog_font, 5, (scrn.top + (XSystemCFG.indicators_enabled ? 39 : 26)) + (13.5 * i), Color(255, 255, 255), FONT_LEFT, eventInfo[i].m_szMessage.c_str());
		}
	}
}
















// Junk Code By Troll Face & Thaisen's Gen
void vJMCcgtRjhSqsqZgqxZMTJuUXLdElhebTwUofcxS60048440() {     int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd6085205 = -635735415;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd59728400 = -783630439;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29030044 = -556063258;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd49829764 = -774951018;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd57201047 = -425713387;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd69410465 = -301149448;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd15652796 = -336239455;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd89715318 = -885345435;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd12616155 = -803494146;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd38448166 = -791704696;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd23556972 = -208655013;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd73121488 = -17755921;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd48190218 = -274411015;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd45979336 = -746610828;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd13752037 = -948859340;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd51926752 = -695335340;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd60405145 = -525191354;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd82754183 = -584116568;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd55076127 = -739416700;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd39860013 = -265887903;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd3969974 = -258176069;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd80382266 = -397653308;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd35575284 = -508229562;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd63982498 = -274538454;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd54158659 = -705164640;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd45315467 = -702912944;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd12592644 = -775599220;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd574220 = -284171631;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd30529584 = -42811663;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd22512844 = -982192131;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd25012263 = -675490973;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29589644 = -810293851;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd24713517 = -476652789;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29781141 = -690298867;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd2865613 = -565271604;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd5550969 = -36768837;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd94983191 = -23605365;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd67838089 = -433993294;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd64782229 = -239126969;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd62438413 = -968297436;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd26659846 = -50077706;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd66827444 = -822837895;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd26619782 = -166364864;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd717084 = -397565894;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd64897589 = -942555572;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd81522933 = 13132302;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd38079038 = -296296876;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd83471609 = -459552343;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd28660066 = -469277529;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd9151435 = -142838151;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56098924 = -247414680;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd78313822 = -231824631;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd3060856 = -858902120;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd31064336 = -774019113;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56078467 = -961591057;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd25702938 = -138082107;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd24153116 = -175400877;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd65047545 = -181524804;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd95671105 = 30213622;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd11885581 = -722800444;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56817822 = -525550228;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd15078576 = 47932175;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd59185734 = -742533772;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd90103310 = -821302015;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd13435904 = -16213723;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd93967327 = -398361162;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd48407971 = -541103132;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd18409078 = -584112148;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd43113724 = -81339225;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd8201068 = -812090503;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56943561 = -571729975;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd92567055 = 8801939;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd17971955 = -244989599;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd92637713 = -771119265;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd13200167 = -115810197;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd37142530 = -435338175;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd53762485 = -131288445;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd34858200 = -10663669;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd99084909 = -331982883;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd72635725 = -618296942;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd7236430 = -306616068;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29121035 = -216046878;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd71914153 = -814894102;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd21378150 = -899973513;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd66413920 = -634777451;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd46698440 = -343666343;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd26528789 = -951391731;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd93649181 = -702633677;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd73702674 = -728707811;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd77162674 = -327189497;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd81397853 = -861367960;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29935647 = -842080562;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd72166984 = -364206916;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd52896648 = -516326526;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd5620592 = -342747208;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd11581270 = 1990119;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd7641710 = 19695877;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd36516471 = -345062849;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd87281180 = -281352171;    int sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd70930261 = -635735415;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd6085205 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd59728400;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd59728400 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29030044;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29030044 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd49829764;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd49829764 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd57201047;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd57201047 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd69410465;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd69410465 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd15652796;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd15652796 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd89715318;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd89715318 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd12616155;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd12616155 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd38448166;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd38448166 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd23556972;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd23556972 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd73121488;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd73121488 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd48190218;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd48190218 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd45979336;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd45979336 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd13752037;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd13752037 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd51926752;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd51926752 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd60405145;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd60405145 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd82754183;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd82754183 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd55076127;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd55076127 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd39860013;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd39860013 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd3969974;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd3969974 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd80382266;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd80382266 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd35575284;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd35575284 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd63982498;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd63982498 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd54158659;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd54158659 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd45315467;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd45315467 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd12592644;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd12592644 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd574220;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd574220 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd30529584;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd30529584 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd22512844;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd22512844 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd25012263;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd25012263 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29589644;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29589644 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd24713517;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd24713517 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29781141;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29781141 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd2865613;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd2865613 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd5550969;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd5550969 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd94983191;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd94983191 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd67838089;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd67838089 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd64782229;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd64782229 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd62438413;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd62438413 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd26659846;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd26659846 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd66827444;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd66827444 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd26619782;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd26619782 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd717084;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd717084 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd64897589;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd64897589 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd81522933;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd81522933 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd38079038;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd38079038 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd83471609;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd83471609 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd28660066;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd28660066 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd9151435;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd9151435 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56098924;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56098924 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd78313822;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd78313822 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd3060856;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd3060856 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd31064336;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd31064336 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56078467;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56078467 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd25702938;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd25702938 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd24153116;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd24153116 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd65047545;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd65047545 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd95671105;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd95671105 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd11885581;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd11885581 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56817822;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56817822 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd15078576;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd15078576 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd59185734;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd59185734 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd90103310;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd90103310 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd13435904;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd13435904 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd93967327;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd93967327 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd48407971;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd48407971 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd18409078;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd18409078 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd43113724;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd43113724 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd8201068;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd8201068 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56943561;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd56943561 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd92567055;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd92567055 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd17971955;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd17971955 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd92637713;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd92637713 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd13200167;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd13200167 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd37142530;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd37142530 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd53762485;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd53762485 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd34858200;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd34858200 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd99084909;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd99084909 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd72635725;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd72635725 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd7236430;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd7236430 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29121035;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29121035 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd71914153;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd71914153 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd21378150;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd21378150 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd66413920;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd66413920 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd46698440;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd46698440 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd26528789;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd26528789 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd93649181;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd93649181 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd73702674;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd73702674 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd77162674;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd77162674 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd81397853;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd81397853 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29935647;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd29935647 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd72166984;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd72166984 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd52896648;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd52896648 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd5620592;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd5620592 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd11581270;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd11581270 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd7641710;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd7641710 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd36516471;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd36516471 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd87281180;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd87281180 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd70930261;     sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd70930261 = sUZLFRYUpacAbbbmOreRDpgCDqWvWkAOzHTykiOGnIOLEdAsViZoxgODoXHOucyFZOBTHd6085205;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void UTYHByNmqplkcCTfXNSRGfgpqPrhrHTBsEsCPvsZUMHtooMAYjj40993246() {     float ptKtomiQlGgvjVjsYKRQbVtwtYnB11053105 = -32832465;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB73059236 = -389127191;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB82927846 = -193409396;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB64881042 = -824075427;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB76536996 = -923817063;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB83514362 = 34186322;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB55513461 = -71475469;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB69574902 = -459919707;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB24200160 = 84336221;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB8635648 = -286054915;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB11377896 = -182775419;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB68377430 = -473469836;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB61919602 = -617910466;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB39359104 = -759044631;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB62055827 = 73811526;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB24555415 = -688119832;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB9050842 = -817669973;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB61906708 = -380810530;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB4901964 = -675199660;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB41818185 = -736085380;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB44073795 = -926245434;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB2965608 = -967528377;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB94157427 = -391514101;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB480867 = -931817026;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB59967218 = -737117928;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB22305092 = -72768660;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB73611534 = -17095665;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB79228670 = -128989108;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB16705884 = -848782910;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB92717886 = -737793774;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB31241769 = -198914806;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB4072108 = 57159097;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB79343323 = -501619583;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB51761077 = -975271384;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB20624074 = 45929157;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB58529632 = -988133390;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB11354318 = -909539660;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB12304863 = -519121912;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB68477902 = -13813212;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB50528892 = -912681660;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB47133023 = -241106479;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB12058444 = -433471193;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB69171254 = -37432673;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB63121998 = -10687202;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB52769299 = -579322234;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB76958428 = -650624096;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB65692417 = 35891185;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB92061447 = -59653939;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB19519338 = -817105550;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB20126818 = -864993470;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB90762917 = -402999471;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB77840127 = -968105793;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB3471281 = -910221551;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB93261182 = -320905313;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB96919450 = -573655940;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB8087497 = -65304089;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB78901808 = -997613091;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB82446979 = -261592370;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB4913824 = 13042501;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB54231904 = -751048404;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB9902829 = -948718014;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB76284790 = -942486361;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB52869018 = -611136798;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB31482273 = -177870005;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB77393879 = 12859890;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB7305788 = -139934517;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB89034106 = -971850253;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB10158526 = -642639082;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB18735030 = -704973789;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB3526195 = 61944916;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB13201097 = -778580172;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB96745978 = -198548062;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB93428806 = -266997318;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB54373072 = -762518001;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB94685162 = -394978902;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB32015352 = -392774242;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB33794353 = -830095704;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB31035430 = -280826899;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB47711568 = -252494793;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB83008790 = 13506168;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB56612674 = -8659846;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB81550086 = -957441726;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB59709333 = -311883559;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB96579065 = -983789441;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB1954970 = -234794304;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB53401641 = -230809013;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB600828 = -32619352;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB86082141 = -80714271;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB54841626 = -301615445;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB12536577 = -888766754;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB79627824 = -990520300;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB28907338 = -547947291;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB7391040 = -432164413;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB14245998 = -262764809;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB40626064 = -963963647;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB70848232 = -298620118;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB59189425 = -822334395;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB37688981 = -859562669;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB85728119 = 76452907;    float ptKtomiQlGgvjVjsYKRQbVtwtYnB45463512 = -32832465;     ptKtomiQlGgvjVjsYKRQbVtwtYnB11053105 = ptKtomiQlGgvjVjsYKRQbVtwtYnB73059236;     ptKtomiQlGgvjVjsYKRQbVtwtYnB73059236 = ptKtomiQlGgvjVjsYKRQbVtwtYnB82927846;     ptKtomiQlGgvjVjsYKRQbVtwtYnB82927846 = ptKtomiQlGgvjVjsYKRQbVtwtYnB64881042;     ptKtomiQlGgvjVjsYKRQbVtwtYnB64881042 = ptKtomiQlGgvjVjsYKRQbVtwtYnB76536996;     ptKtomiQlGgvjVjsYKRQbVtwtYnB76536996 = ptKtomiQlGgvjVjsYKRQbVtwtYnB83514362;     ptKtomiQlGgvjVjsYKRQbVtwtYnB83514362 = ptKtomiQlGgvjVjsYKRQbVtwtYnB55513461;     ptKtomiQlGgvjVjsYKRQbVtwtYnB55513461 = ptKtomiQlGgvjVjsYKRQbVtwtYnB69574902;     ptKtomiQlGgvjVjsYKRQbVtwtYnB69574902 = ptKtomiQlGgvjVjsYKRQbVtwtYnB24200160;     ptKtomiQlGgvjVjsYKRQbVtwtYnB24200160 = ptKtomiQlGgvjVjsYKRQbVtwtYnB8635648;     ptKtomiQlGgvjVjsYKRQbVtwtYnB8635648 = ptKtomiQlGgvjVjsYKRQbVtwtYnB11377896;     ptKtomiQlGgvjVjsYKRQbVtwtYnB11377896 = ptKtomiQlGgvjVjsYKRQbVtwtYnB68377430;     ptKtomiQlGgvjVjsYKRQbVtwtYnB68377430 = ptKtomiQlGgvjVjsYKRQbVtwtYnB61919602;     ptKtomiQlGgvjVjsYKRQbVtwtYnB61919602 = ptKtomiQlGgvjVjsYKRQbVtwtYnB39359104;     ptKtomiQlGgvjVjsYKRQbVtwtYnB39359104 = ptKtomiQlGgvjVjsYKRQbVtwtYnB62055827;     ptKtomiQlGgvjVjsYKRQbVtwtYnB62055827 = ptKtomiQlGgvjVjsYKRQbVtwtYnB24555415;     ptKtomiQlGgvjVjsYKRQbVtwtYnB24555415 = ptKtomiQlGgvjVjsYKRQbVtwtYnB9050842;     ptKtomiQlGgvjVjsYKRQbVtwtYnB9050842 = ptKtomiQlGgvjVjsYKRQbVtwtYnB61906708;     ptKtomiQlGgvjVjsYKRQbVtwtYnB61906708 = ptKtomiQlGgvjVjsYKRQbVtwtYnB4901964;     ptKtomiQlGgvjVjsYKRQbVtwtYnB4901964 = ptKtomiQlGgvjVjsYKRQbVtwtYnB41818185;     ptKtomiQlGgvjVjsYKRQbVtwtYnB41818185 = ptKtomiQlGgvjVjsYKRQbVtwtYnB44073795;     ptKtomiQlGgvjVjsYKRQbVtwtYnB44073795 = ptKtomiQlGgvjVjsYKRQbVtwtYnB2965608;     ptKtomiQlGgvjVjsYKRQbVtwtYnB2965608 = ptKtomiQlGgvjVjsYKRQbVtwtYnB94157427;     ptKtomiQlGgvjVjsYKRQbVtwtYnB94157427 = ptKtomiQlGgvjVjsYKRQbVtwtYnB480867;     ptKtomiQlGgvjVjsYKRQbVtwtYnB480867 = ptKtomiQlGgvjVjsYKRQbVtwtYnB59967218;     ptKtomiQlGgvjVjsYKRQbVtwtYnB59967218 = ptKtomiQlGgvjVjsYKRQbVtwtYnB22305092;     ptKtomiQlGgvjVjsYKRQbVtwtYnB22305092 = ptKtomiQlGgvjVjsYKRQbVtwtYnB73611534;     ptKtomiQlGgvjVjsYKRQbVtwtYnB73611534 = ptKtomiQlGgvjVjsYKRQbVtwtYnB79228670;     ptKtomiQlGgvjVjsYKRQbVtwtYnB79228670 = ptKtomiQlGgvjVjsYKRQbVtwtYnB16705884;     ptKtomiQlGgvjVjsYKRQbVtwtYnB16705884 = ptKtomiQlGgvjVjsYKRQbVtwtYnB92717886;     ptKtomiQlGgvjVjsYKRQbVtwtYnB92717886 = ptKtomiQlGgvjVjsYKRQbVtwtYnB31241769;     ptKtomiQlGgvjVjsYKRQbVtwtYnB31241769 = ptKtomiQlGgvjVjsYKRQbVtwtYnB4072108;     ptKtomiQlGgvjVjsYKRQbVtwtYnB4072108 = ptKtomiQlGgvjVjsYKRQbVtwtYnB79343323;     ptKtomiQlGgvjVjsYKRQbVtwtYnB79343323 = ptKtomiQlGgvjVjsYKRQbVtwtYnB51761077;     ptKtomiQlGgvjVjsYKRQbVtwtYnB51761077 = ptKtomiQlGgvjVjsYKRQbVtwtYnB20624074;     ptKtomiQlGgvjVjsYKRQbVtwtYnB20624074 = ptKtomiQlGgvjVjsYKRQbVtwtYnB58529632;     ptKtomiQlGgvjVjsYKRQbVtwtYnB58529632 = ptKtomiQlGgvjVjsYKRQbVtwtYnB11354318;     ptKtomiQlGgvjVjsYKRQbVtwtYnB11354318 = ptKtomiQlGgvjVjsYKRQbVtwtYnB12304863;     ptKtomiQlGgvjVjsYKRQbVtwtYnB12304863 = ptKtomiQlGgvjVjsYKRQbVtwtYnB68477902;     ptKtomiQlGgvjVjsYKRQbVtwtYnB68477902 = ptKtomiQlGgvjVjsYKRQbVtwtYnB50528892;     ptKtomiQlGgvjVjsYKRQbVtwtYnB50528892 = ptKtomiQlGgvjVjsYKRQbVtwtYnB47133023;     ptKtomiQlGgvjVjsYKRQbVtwtYnB47133023 = ptKtomiQlGgvjVjsYKRQbVtwtYnB12058444;     ptKtomiQlGgvjVjsYKRQbVtwtYnB12058444 = ptKtomiQlGgvjVjsYKRQbVtwtYnB69171254;     ptKtomiQlGgvjVjsYKRQbVtwtYnB69171254 = ptKtomiQlGgvjVjsYKRQbVtwtYnB63121998;     ptKtomiQlGgvjVjsYKRQbVtwtYnB63121998 = ptKtomiQlGgvjVjsYKRQbVtwtYnB52769299;     ptKtomiQlGgvjVjsYKRQbVtwtYnB52769299 = ptKtomiQlGgvjVjsYKRQbVtwtYnB76958428;     ptKtomiQlGgvjVjsYKRQbVtwtYnB76958428 = ptKtomiQlGgvjVjsYKRQbVtwtYnB65692417;     ptKtomiQlGgvjVjsYKRQbVtwtYnB65692417 = ptKtomiQlGgvjVjsYKRQbVtwtYnB92061447;     ptKtomiQlGgvjVjsYKRQbVtwtYnB92061447 = ptKtomiQlGgvjVjsYKRQbVtwtYnB19519338;     ptKtomiQlGgvjVjsYKRQbVtwtYnB19519338 = ptKtomiQlGgvjVjsYKRQbVtwtYnB20126818;     ptKtomiQlGgvjVjsYKRQbVtwtYnB20126818 = ptKtomiQlGgvjVjsYKRQbVtwtYnB90762917;     ptKtomiQlGgvjVjsYKRQbVtwtYnB90762917 = ptKtomiQlGgvjVjsYKRQbVtwtYnB77840127;     ptKtomiQlGgvjVjsYKRQbVtwtYnB77840127 = ptKtomiQlGgvjVjsYKRQbVtwtYnB3471281;     ptKtomiQlGgvjVjsYKRQbVtwtYnB3471281 = ptKtomiQlGgvjVjsYKRQbVtwtYnB93261182;     ptKtomiQlGgvjVjsYKRQbVtwtYnB93261182 = ptKtomiQlGgvjVjsYKRQbVtwtYnB96919450;     ptKtomiQlGgvjVjsYKRQbVtwtYnB96919450 = ptKtomiQlGgvjVjsYKRQbVtwtYnB8087497;     ptKtomiQlGgvjVjsYKRQbVtwtYnB8087497 = ptKtomiQlGgvjVjsYKRQbVtwtYnB78901808;     ptKtomiQlGgvjVjsYKRQbVtwtYnB78901808 = ptKtomiQlGgvjVjsYKRQbVtwtYnB82446979;     ptKtomiQlGgvjVjsYKRQbVtwtYnB82446979 = ptKtomiQlGgvjVjsYKRQbVtwtYnB4913824;     ptKtomiQlGgvjVjsYKRQbVtwtYnB4913824 = ptKtomiQlGgvjVjsYKRQbVtwtYnB54231904;     ptKtomiQlGgvjVjsYKRQbVtwtYnB54231904 = ptKtomiQlGgvjVjsYKRQbVtwtYnB9902829;     ptKtomiQlGgvjVjsYKRQbVtwtYnB9902829 = ptKtomiQlGgvjVjsYKRQbVtwtYnB76284790;     ptKtomiQlGgvjVjsYKRQbVtwtYnB76284790 = ptKtomiQlGgvjVjsYKRQbVtwtYnB52869018;     ptKtomiQlGgvjVjsYKRQbVtwtYnB52869018 = ptKtomiQlGgvjVjsYKRQbVtwtYnB31482273;     ptKtomiQlGgvjVjsYKRQbVtwtYnB31482273 = ptKtomiQlGgvjVjsYKRQbVtwtYnB77393879;     ptKtomiQlGgvjVjsYKRQbVtwtYnB77393879 = ptKtomiQlGgvjVjsYKRQbVtwtYnB7305788;     ptKtomiQlGgvjVjsYKRQbVtwtYnB7305788 = ptKtomiQlGgvjVjsYKRQbVtwtYnB89034106;     ptKtomiQlGgvjVjsYKRQbVtwtYnB89034106 = ptKtomiQlGgvjVjsYKRQbVtwtYnB10158526;     ptKtomiQlGgvjVjsYKRQbVtwtYnB10158526 = ptKtomiQlGgvjVjsYKRQbVtwtYnB18735030;     ptKtomiQlGgvjVjsYKRQbVtwtYnB18735030 = ptKtomiQlGgvjVjsYKRQbVtwtYnB3526195;     ptKtomiQlGgvjVjsYKRQbVtwtYnB3526195 = ptKtomiQlGgvjVjsYKRQbVtwtYnB13201097;     ptKtomiQlGgvjVjsYKRQbVtwtYnB13201097 = ptKtomiQlGgvjVjsYKRQbVtwtYnB96745978;     ptKtomiQlGgvjVjsYKRQbVtwtYnB96745978 = ptKtomiQlGgvjVjsYKRQbVtwtYnB93428806;     ptKtomiQlGgvjVjsYKRQbVtwtYnB93428806 = ptKtomiQlGgvjVjsYKRQbVtwtYnB54373072;     ptKtomiQlGgvjVjsYKRQbVtwtYnB54373072 = ptKtomiQlGgvjVjsYKRQbVtwtYnB94685162;     ptKtomiQlGgvjVjsYKRQbVtwtYnB94685162 = ptKtomiQlGgvjVjsYKRQbVtwtYnB32015352;     ptKtomiQlGgvjVjsYKRQbVtwtYnB32015352 = ptKtomiQlGgvjVjsYKRQbVtwtYnB33794353;     ptKtomiQlGgvjVjsYKRQbVtwtYnB33794353 = ptKtomiQlGgvjVjsYKRQbVtwtYnB31035430;     ptKtomiQlGgvjVjsYKRQbVtwtYnB31035430 = ptKtomiQlGgvjVjsYKRQbVtwtYnB47711568;     ptKtomiQlGgvjVjsYKRQbVtwtYnB47711568 = ptKtomiQlGgvjVjsYKRQbVtwtYnB83008790;     ptKtomiQlGgvjVjsYKRQbVtwtYnB83008790 = ptKtomiQlGgvjVjsYKRQbVtwtYnB56612674;     ptKtomiQlGgvjVjsYKRQbVtwtYnB56612674 = ptKtomiQlGgvjVjsYKRQbVtwtYnB81550086;     ptKtomiQlGgvjVjsYKRQbVtwtYnB81550086 = ptKtomiQlGgvjVjsYKRQbVtwtYnB59709333;     ptKtomiQlGgvjVjsYKRQbVtwtYnB59709333 = ptKtomiQlGgvjVjsYKRQbVtwtYnB96579065;     ptKtomiQlGgvjVjsYKRQbVtwtYnB96579065 = ptKtomiQlGgvjVjsYKRQbVtwtYnB1954970;     ptKtomiQlGgvjVjsYKRQbVtwtYnB1954970 = ptKtomiQlGgvjVjsYKRQbVtwtYnB53401641;     ptKtomiQlGgvjVjsYKRQbVtwtYnB53401641 = ptKtomiQlGgvjVjsYKRQbVtwtYnB600828;     ptKtomiQlGgvjVjsYKRQbVtwtYnB600828 = ptKtomiQlGgvjVjsYKRQbVtwtYnB86082141;     ptKtomiQlGgvjVjsYKRQbVtwtYnB86082141 = ptKtomiQlGgvjVjsYKRQbVtwtYnB54841626;     ptKtomiQlGgvjVjsYKRQbVtwtYnB54841626 = ptKtomiQlGgvjVjsYKRQbVtwtYnB12536577;     ptKtomiQlGgvjVjsYKRQbVtwtYnB12536577 = ptKtomiQlGgvjVjsYKRQbVtwtYnB79627824;     ptKtomiQlGgvjVjsYKRQbVtwtYnB79627824 = ptKtomiQlGgvjVjsYKRQbVtwtYnB28907338;     ptKtomiQlGgvjVjsYKRQbVtwtYnB28907338 = ptKtomiQlGgvjVjsYKRQbVtwtYnB7391040;     ptKtomiQlGgvjVjsYKRQbVtwtYnB7391040 = ptKtomiQlGgvjVjsYKRQbVtwtYnB14245998;     ptKtomiQlGgvjVjsYKRQbVtwtYnB14245998 = ptKtomiQlGgvjVjsYKRQbVtwtYnB40626064;     ptKtomiQlGgvjVjsYKRQbVtwtYnB40626064 = ptKtomiQlGgvjVjsYKRQbVtwtYnB70848232;     ptKtomiQlGgvjVjsYKRQbVtwtYnB70848232 = ptKtomiQlGgvjVjsYKRQbVtwtYnB59189425;     ptKtomiQlGgvjVjsYKRQbVtwtYnB59189425 = ptKtomiQlGgvjVjsYKRQbVtwtYnB37688981;     ptKtomiQlGgvjVjsYKRQbVtwtYnB37688981 = ptKtomiQlGgvjVjsYKRQbVtwtYnB85728119;     ptKtomiQlGgvjVjsYKRQbVtwtYnB85728119 = ptKtomiQlGgvjVjsYKRQbVtwtYnB45463512;     ptKtomiQlGgvjVjsYKRQbVtwtYnB45463512 = ptKtomiQlGgvjVjsYKRQbVtwtYnB11053105;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void WasEwTwVusVOtwMXHZUFnOHxz42993298() {     long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru59054935 = -411875715;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru43340327 = -946756642;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru71548756 = -755550298;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru23680191 = -660151379;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru22127597 = -497650580;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru28605213 = -67242047;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru32319262 = -780876848;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru83821262 = -80040702;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru90867472 = -715611332;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru91032810 = -926846971;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru94230294 = -708026210;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru75801650 = -747301610;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru96374239 = -680908150;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru24583822 = -809080190;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru13693996 = -852779963;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru54892040 = -607152191;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru43262287 = -611324469;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru9579544 = -91217095;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru18038416 = -266759388;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru85134671 = -810301955;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru99948546 = -727331028;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru84482130 = -830948976;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru85635501 = -537940758;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru80066140 = -6179754;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru11772430 = -111555419;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru5625850 = -465957492;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru67646816 = -736956163;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98129440 = -174110047;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru1426068 = -852922229;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru93319606 = -728659886;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru13893586 = -392408401;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru69150116 = -899622794;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru12046479 = -329349643;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98706802 = -230411736;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru4404666 = -548058727;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru45315566 = 81055823;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98912668 = -966755147;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru65628485 = -252436119;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru75500193 = -404785186;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru32813053 = -492972041;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru47328605 = -560289720;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru63554071 = -816746313;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru36452398 = -752132808;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru50903256 = -460870464;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru17591875 = -963823319;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru17221016 = -288980248;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru2507743 = -137361272;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru42401857 = -599991839;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru99273758 = -498237134;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru1647501 = -602446591;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru62429978 = -144148915;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru53999637 = 83964729;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru68858026 = -205290761;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru63659582 = -378293707;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru43044751 = -255040311;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru74572804 = -580926739;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru57704826 = -308815884;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru91482616 = -649370544;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru11907761 = -448595961;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru16501748 = 68306912;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru60958397 = -330285885;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru34189821 = -506766802;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru82395194 = -227118473;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru97547865 = -986951447;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru77139225 = -434438571;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru25080178 = -808403416;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru63755171 = -317951967;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru97667437 = -350496415;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru20179157 = -161021463;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru68378429 = -833835786;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru55979372 = -640397044;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru77633801 = -258888351;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru34079351 = -686431909;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru85225362 = -773787347;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru37806067 = -150012235;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru36394476 = -910584716;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru48029733 = 21183831;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru34732245 = 22929706;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru62474266 = -42356435;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru94551414 = -822575171;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru3118108 = -228596220;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru25244960 = -36964324;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98855682 = -675872914;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru99778566 = -150475638;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru30889629 = -484510972;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru59893948 = -376373131;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru292090 = -594332034;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru48386896 = -951055936;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru55662051 = -975371425;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru29831861 = -967131989;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru87610740 = -610128294;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru7430053 = -217384603;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru53720725 = -803840158;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru58998446 = -373092098;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru71854656 = -62686157;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru13138784 = 46477081;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru81158876 = -489627840;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru38904532 = -765181362;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru73764031 = 73568106;    long jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru92511696 = -411875715;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru59054935 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru43340327;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru43340327 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru71548756;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru71548756 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru23680191;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru23680191 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru22127597;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru22127597 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru28605213;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru28605213 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru32319262;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru32319262 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru83821262;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru83821262 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru90867472;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru90867472 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru91032810;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru91032810 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru94230294;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru94230294 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru75801650;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru75801650 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru96374239;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru96374239 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru24583822;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru24583822 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru13693996;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru13693996 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru54892040;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru54892040 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru43262287;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru43262287 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru9579544;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru9579544 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru18038416;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru18038416 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru85134671;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru85134671 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru99948546;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru99948546 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru84482130;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru84482130 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru85635501;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru85635501 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru80066140;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru80066140 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru11772430;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru11772430 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru5625850;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru5625850 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru67646816;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru67646816 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98129440;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98129440 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru1426068;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru1426068 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru93319606;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru93319606 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru13893586;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru13893586 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru69150116;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru69150116 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru12046479;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru12046479 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98706802;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98706802 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru4404666;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru4404666 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru45315566;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru45315566 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98912668;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98912668 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru65628485;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru65628485 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru75500193;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru75500193 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru32813053;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru32813053 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru47328605;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru47328605 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru63554071;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru63554071 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru36452398;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru36452398 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru50903256;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru50903256 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru17591875;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru17591875 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru17221016;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru17221016 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru2507743;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru2507743 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru42401857;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru42401857 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru99273758;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru99273758 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru1647501;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru1647501 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru62429978;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru62429978 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru53999637;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru53999637 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru68858026;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru68858026 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru63659582;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru63659582 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru43044751;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru43044751 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru74572804;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru74572804 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru57704826;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru57704826 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru91482616;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru91482616 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru11907761;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru11907761 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru16501748;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru16501748 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru60958397;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru60958397 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru34189821;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru34189821 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru82395194;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru82395194 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru97547865;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru97547865 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru77139225;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru77139225 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru25080178;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru25080178 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru63755171;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru63755171 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru97667437;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru97667437 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru20179157;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru20179157 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru68378429;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru68378429 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru55979372;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru55979372 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru77633801;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru77633801 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru34079351;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru34079351 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru85225362;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru85225362 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru37806067;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru37806067 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru36394476;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru36394476 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru48029733;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru48029733 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru34732245;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru34732245 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru62474266;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru62474266 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru94551414;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru94551414 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru3118108;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru3118108 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru25244960;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru25244960 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98855682;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru98855682 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru99778566;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru99778566 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru30889629;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru30889629 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru59893948;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru59893948 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru292090;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru292090 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru48386896;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru48386896 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru55662051;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru55662051 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru29831861;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru29831861 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru87610740;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru87610740 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru7430053;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru7430053 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru53720725;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru53720725 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru58998446;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru58998446 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru71854656;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru71854656 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru13138784;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru13138784 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru81158876;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru81158876 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru38904532;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru38904532 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru73764031;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru73764031 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru92511696;     jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru92511696 = jdNMzcBbMERPdKvIMRNtfOVdiaBVMTHTvEMqjJvPTmhru59054935;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kDdoJXSPwdVbQqxwYaFoYQaEgycnmzgejvnuR34315131() {     double hBEAJkoCJNRxBtOWqAUdY66363943 = -809105168;    double hBEAJkoCJNRxBtOWqAUdY42259737 = -474164501;    double hBEAJkoCJNRxBtOWqAUdY56963262 = -702742580;    double hBEAJkoCJNRxBtOWqAUdY21619188 = -738567362;    double hBEAJkoCJNRxBtOWqAUdY18831565 = -83443405;    double hBEAJkoCJNRxBtOWqAUdY10746216 = -351830291;    double hBEAJkoCJNRxBtOWqAUdY93463305 = -645197690;    double hBEAJkoCJNRxBtOWqAUdY23721343 = -77342490;    double hBEAJkoCJNRxBtOWqAUdY3768649 = -712677144;    double hBEAJkoCJNRxBtOWqAUdY37853823 = -659443284;    double hBEAJkoCJNRxBtOWqAUdY39385434 = -823858163;    double hBEAJkoCJNRxBtOWqAUdY12328216 = -777851152;    double hBEAJkoCJNRxBtOWqAUdY16426736 = -81401062;    double hBEAJkoCJNRxBtOWqAUdY84202456 = -719611168;    double hBEAJkoCJNRxBtOWqAUdY87073338 = -484292928;    double hBEAJkoCJNRxBtOWqAUdY58404874 = -144702592;    double hBEAJkoCJNRxBtOWqAUdY91100448 = -757082264;    double hBEAJkoCJNRxBtOWqAUdY73195686 = -853020500;    double hBEAJkoCJNRxBtOWqAUdY13723137 = -34437778;    double hBEAJkoCJNRxBtOWqAUdY31738710 = -78256935;    double hBEAJkoCJNRxBtOWqAUdY44089430 = 57800483;    double hBEAJkoCJNRxBtOWqAUdY13077899 = -879755762;    double hBEAJkoCJNRxBtOWqAUdY59272837 = -597593718;    double hBEAJkoCJNRxBtOWqAUdY23420679 = -98854244;    double hBEAJkoCJNRxBtOWqAUdY37193546 = -613493276;    double hBEAJkoCJNRxBtOWqAUdY572022 = -293615374;    double hBEAJkoCJNRxBtOWqAUdY58217342 = -441705146;    double hBEAJkoCJNRxBtOWqAUdY32379093 = -937576455;    double hBEAJkoCJNRxBtOWqAUdY19111217 = -404068877;    double hBEAJkoCJNRxBtOWqAUdY76814613 = -119899777;    double hBEAJkoCJNRxBtOWqAUdY8309630 = -130420357;    double hBEAJkoCJNRxBtOWqAUdY92392309 = -27353182;    double hBEAJkoCJNRxBtOWqAUdY51424493 = 54399015;    double hBEAJkoCJNRxBtOWqAUdY35035024 = -739964139;    double hBEAJkoCJNRxBtOWqAUdY27783080 = -542601612;    double hBEAJkoCJNRxBtOWqAUdY17461632 = -740687594;    double hBEAJkoCJNRxBtOWqAUdY13865337 = -474737220;    double hBEAJkoCJNRxBtOWqAUdY12386006 = -832424037;    double hBEAJkoCJNRxBtOWqAUdY42890181 = -912824717;    double hBEAJkoCJNRxBtOWqAUdY56038227 = -117237542;    double hBEAJkoCJNRxBtOWqAUdY56406905 = -851559253;    double hBEAJkoCJNRxBtOWqAUdY84823430 = -263533627;    double hBEAJkoCJNRxBtOWqAUdY75804748 = -737626142;    double hBEAJkoCJNRxBtOWqAUdY96481287 = -847654169;    double hBEAJkoCJNRxBtOWqAUdY73387089 = -356674451;    double hBEAJkoCJNRxBtOWqAUdY78257798 = -842926797;    double hBEAJkoCJNRxBtOWqAUdY29194939 = 64951347;    double hBEAJkoCJNRxBtOWqAUdY83442778 = -972824822;    double hBEAJkoCJNRxBtOWqAUdY93378310 = -144770061;    double hBEAJkoCJNRxBtOWqAUdY27862865 = -716694522;    double hBEAJkoCJNRxBtOWqAUdY37638962 = -194368114;    double hBEAJkoCJNRxBtOWqAUdY62560261 = -264633151;    double hBEAJkoCJNRxBtOWqAUdY41562866 = -328204387;    double hBEAJkoCJNRxBtOWqAUdY1452064 = -85435034;    double hBEAJkoCJNRxBtOWqAUdY90225701 = -667466864;    double hBEAJkoCJNRxBtOWqAUdY53286045 = -929349406;    double hBEAJkoCJNRxBtOWqAUdY82986899 = -876570784;    double hBEAJkoCJNRxBtOWqAUdY33542584 = -503888336;    double hBEAJkoCJNRxBtOWqAUdY84425641 = -25074086;    double hBEAJkoCJNRxBtOWqAUdY18259544 = -789828031;    double hBEAJkoCJNRxBtOWqAUdY52528874 = -910125145;    double hBEAJkoCJNRxBtOWqAUdY61084212 = -707621235;    double hBEAJkoCJNRxBtOWqAUdY4610126 = -673273614;    double hBEAJkoCJNRxBtOWqAUdY26954036 = -492777367;    double hBEAJkoCJNRxBtOWqAUdY29544193 = -429022928;    double hBEAJkoCJNRxBtOWqAUdY46993124 = -696504981;    double hBEAJkoCJNRxBtOWqAUdY60903723 = -732250168;    double hBEAJkoCJNRxBtOWqAUdY81391712 = -341436924;    double hBEAJkoCJNRxBtOWqAUdY56419377 = -77009556;    double hBEAJkoCJNRxBtOWqAUdY69611707 = -743605334;    double hBEAJkoCJNRxBtOWqAUdY44539537 = -669965372;    double hBEAJkoCJNRxBtOWqAUdY78714442 = -924658228;    double hBEAJkoCJNRxBtOWqAUdY30305505 = -940195783;    double hBEAJkoCJNRxBtOWqAUdY57684910 = -917200236;    double hBEAJkoCJNRxBtOWqAUdY75331804 = -226697683;    double hBEAJkoCJNRxBtOWqAUdY59265999 = -678665891;    double hBEAJkoCJNRxBtOWqAUdY37273151 = -42129621;    double hBEAJkoCJNRxBtOWqAUdY62791550 = -749939550;    double hBEAJkoCJNRxBtOWqAUdY50033589 = -742179794;    double hBEAJkoCJNRxBtOWqAUdY58935747 = -770566480;    double hBEAJkoCJNRxBtOWqAUdY71377082 = -258566722;    double hBEAJkoCJNRxBtOWqAUdY74774563 = -468880325;    double hBEAJkoCJNRxBtOWqAUdY39000782 = -692806395;    double hBEAJkoCJNRxBtOWqAUdY91248352 = -687374355;    double hBEAJkoCJNRxBtOWqAUdY39175652 = -925531663;    double hBEAJkoCJNRxBtOWqAUdY45749369 = -865787207;    double hBEAJkoCJNRxBtOWqAUdY50829444 = -699148795;    double hBEAJkoCJNRxBtOWqAUdY49972429 = -860165952;    double hBEAJkoCJNRxBtOWqAUdY44809323 = 27502725;    double hBEAJkoCJNRxBtOWqAUdY74497035 = -613252207;    double hBEAJkoCJNRxBtOWqAUdY34474732 = -864116811;    double hBEAJkoCJNRxBtOWqAUdY80322753 = -970848885;    double hBEAJkoCJNRxBtOWqAUdY27960365 = -707349951;    double hBEAJkoCJNRxBtOWqAUdY24630638 = -22996686;    double hBEAJkoCJNRxBtOWqAUdY3509354 = -207112398;    double hBEAJkoCJNRxBtOWqAUdY95322692 = -43938018;    double hBEAJkoCJNRxBtOWqAUdY80213304 = -590260014;    double hBEAJkoCJNRxBtOWqAUdY48850712 = -144848776;    double hBEAJkoCJNRxBtOWqAUdY66937094 = -318631241;    double hBEAJkoCJNRxBtOWqAUdY26393966 = -809105168;     hBEAJkoCJNRxBtOWqAUdY66363943 = hBEAJkoCJNRxBtOWqAUdY42259737;     hBEAJkoCJNRxBtOWqAUdY42259737 = hBEAJkoCJNRxBtOWqAUdY56963262;     hBEAJkoCJNRxBtOWqAUdY56963262 = hBEAJkoCJNRxBtOWqAUdY21619188;     hBEAJkoCJNRxBtOWqAUdY21619188 = hBEAJkoCJNRxBtOWqAUdY18831565;     hBEAJkoCJNRxBtOWqAUdY18831565 = hBEAJkoCJNRxBtOWqAUdY10746216;     hBEAJkoCJNRxBtOWqAUdY10746216 = hBEAJkoCJNRxBtOWqAUdY93463305;     hBEAJkoCJNRxBtOWqAUdY93463305 = hBEAJkoCJNRxBtOWqAUdY23721343;     hBEAJkoCJNRxBtOWqAUdY23721343 = hBEAJkoCJNRxBtOWqAUdY3768649;     hBEAJkoCJNRxBtOWqAUdY3768649 = hBEAJkoCJNRxBtOWqAUdY37853823;     hBEAJkoCJNRxBtOWqAUdY37853823 = hBEAJkoCJNRxBtOWqAUdY39385434;     hBEAJkoCJNRxBtOWqAUdY39385434 = hBEAJkoCJNRxBtOWqAUdY12328216;     hBEAJkoCJNRxBtOWqAUdY12328216 = hBEAJkoCJNRxBtOWqAUdY16426736;     hBEAJkoCJNRxBtOWqAUdY16426736 = hBEAJkoCJNRxBtOWqAUdY84202456;     hBEAJkoCJNRxBtOWqAUdY84202456 = hBEAJkoCJNRxBtOWqAUdY87073338;     hBEAJkoCJNRxBtOWqAUdY87073338 = hBEAJkoCJNRxBtOWqAUdY58404874;     hBEAJkoCJNRxBtOWqAUdY58404874 = hBEAJkoCJNRxBtOWqAUdY91100448;     hBEAJkoCJNRxBtOWqAUdY91100448 = hBEAJkoCJNRxBtOWqAUdY73195686;     hBEAJkoCJNRxBtOWqAUdY73195686 = hBEAJkoCJNRxBtOWqAUdY13723137;     hBEAJkoCJNRxBtOWqAUdY13723137 = hBEAJkoCJNRxBtOWqAUdY31738710;     hBEAJkoCJNRxBtOWqAUdY31738710 = hBEAJkoCJNRxBtOWqAUdY44089430;     hBEAJkoCJNRxBtOWqAUdY44089430 = hBEAJkoCJNRxBtOWqAUdY13077899;     hBEAJkoCJNRxBtOWqAUdY13077899 = hBEAJkoCJNRxBtOWqAUdY59272837;     hBEAJkoCJNRxBtOWqAUdY59272837 = hBEAJkoCJNRxBtOWqAUdY23420679;     hBEAJkoCJNRxBtOWqAUdY23420679 = hBEAJkoCJNRxBtOWqAUdY37193546;     hBEAJkoCJNRxBtOWqAUdY37193546 = hBEAJkoCJNRxBtOWqAUdY572022;     hBEAJkoCJNRxBtOWqAUdY572022 = hBEAJkoCJNRxBtOWqAUdY58217342;     hBEAJkoCJNRxBtOWqAUdY58217342 = hBEAJkoCJNRxBtOWqAUdY32379093;     hBEAJkoCJNRxBtOWqAUdY32379093 = hBEAJkoCJNRxBtOWqAUdY19111217;     hBEAJkoCJNRxBtOWqAUdY19111217 = hBEAJkoCJNRxBtOWqAUdY76814613;     hBEAJkoCJNRxBtOWqAUdY76814613 = hBEAJkoCJNRxBtOWqAUdY8309630;     hBEAJkoCJNRxBtOWqAUdY8309630 = hBEAJkoCJNRxBtOWqAUdY92392309;     hBEAJkoCJNRxBtOWqAUdY92392309 = hBEAJkoCJNRxBtOWqAUdY51424493;     hBEAJkoCJNRxBtOWqAUdY51424493 = hBEAJkoCJNRxBtOWqAUdY35035024;     hBEAJkoCJNRxBtOWqAUdY35035024 = hBEAJkoCJNRxBtOWqAUdY27783080;     hBEAJkoCJNRxBtOWqAUdY27783080 = hBEAJkoCJNRxBtOWqAUdY17461632;     hBEAJkoCJNRxBtOWqAUdY17461632 = hBEAJkoCJNRxBtOWqAUdY13865337;     hBEAJkoCJNRxBtOWqAUdY13865337 = hBEAJkoCJNRxBtOWqAUdY12386006;     hBEAJkoCJNRxBtOWqAUdY12386006 = hBEAJkoCJNRxBtOWqAUdY42890181;     hBEAJkoCJNRxBtOWqAUdY42890181 = hBEAJkoCJNRxBtOWqAUdY56038227;     hBEAJkoCJNRxBtOWqAUdY56038227 = hBEAJkoCJNRxBtOWqAUdY56406905;     hBEAJkoCJNRxBtOWqAUdY56406905 = hBEAJkoCJNRxBtOWqAUdY84823430;     hBEAJkoCJNRxBtOWqAUdY84823430 = hBEAJkoCJNRxBtOWqAUdY75804748;     hBEAJkoCJNRxBtOWqAUdY75804748 = hBEAJkoCJNRxBtOWqAUdY96481287;     hBEAJkoCJNRxBtOWqAUdY96481287 = hBEAJkoCJNRxBtOWqAUdY73387089;     hBEAJkoCJNRxBtOWqAUdY73387089 = hBEAJkoCJNRxBtOWqAUdY78257798;     hBEAJkoCJNRxBtOWqAUdY78257798 = hBEAJkoCJNRxBtOWqAUdY29194939;     hBEAJkoCJNRxBtOWqAUdY29194939 = hBEAJkoCJNRxBtOWqAUdY83442778;     hBEAJkoCJNRxBtOWqAUdY83442778 = hBEAJkoCJNRxBtOWqAUdY93378310;     hBEAJkoCJNRxBtOWqAUdY93378310 = hBEAJkoCJNRxBtOWqAUdY27862865;     hBEAJkoCJNRxBtOWqAUdY27862865 = hBEAJkoCJNRxBtOWqAUdY37638962;     hBEAJkoCJNRxBtOWqAUdY37638962 = hBEAJkoCJNRxBtOWqAUdY62560261;     hBEAJkoCJNRxBtOWqAUdY62560261 = hBEAJkoCJNRxBtOWqAUdY41562866;     hBEAJkoCJNRxBtOWqAUdY41562866 = hBEAJkoCJNRxBtOWqAUdY1452064;     hBEAJkoCJNRxBtOWqAUdY1452064 = hBEAJkoCJNRxBtOWqAUdY90225701;     hBEAJkoCJNRxBtOWqAUdY90225701 = hBEAJkoCJNRxBtOWqAUdY53286045;     hBEAJkoCJNRxBtOWqAUdY53286045 = hBEAJkoCJNRxBtOWqAUdY82986899;     hBEAJkoCJNRxBtOWqAUdY82986899 = hBEAJkoCJNRxBtOWqAUdY33542584;     hBEAJkoCJNRxBtOWqAUdY33542584 = hBEAJkoCJNRxBtOWqAUdY84425641;     hBEAJkoCJNRxBtOWqAUdY84425641 = hBEAJkoCJNRxBtOWqAUdY18259544;     hBEAJkoCJNRxBtOWqAUdY18259544 = hBEAJkoCJNRxBtOWqAUdY52528874;     hBEAJkoCJNRxBtOWqAUdY52528874 = hBEAJkoCJNRxBtOWqAUdY61084212;     hBEAJkoCJNRxBtOWqAUdY61084212 = hBEAJkoCJNRxBtOWqAUdY4610126;     hBEAJkoCJNRxBtOWqAUdY4610126 = hBEAJkoCJNRxBtOWqAUdY26954036;     hBEAJkoCJNRxBtOWqAUdY26954036 = hBEAJkoCJNRxBtOWqAUdY29544193;     hBEAJkoCJNRxBtOWqAUdY29544193 = hBEAJkoCJNRxBtOWqAUdY46993124;     hBEAJkoCJNRxBtOWqAUdY46993124 = hBEAJkoCJNRxBtOWqAUdY60903723;     hBEAJkoCJNRxBtOWqAUdY60903723 = hBEAJkoCJNRxBtOWqAUdY81391712;     hBEAJkoCJNRxBtOWqAUdY81391712 = hBEAJkoCJNRxBtOWqAUdY56419377;     hBEAJkoCJNRxBtOWqAUdY56419377 = hBEAJkoCJNRxBtOWqAUdY69611707;     hBEAJkoCJNRxBtOWqAUdY69611707 = hBEAJkoCJNRxBtOWqAUdY44539537;     hBEAJkoCJNRxBtOWqAUdY44539537 = hBEAJkoCJNRxBtOWqAUdY78714442;     hBEAJkoCJNRxBtOWqAUdY78714442 = hBEAJkoCJNRxBtOWqAUdY30305505;     hBEAJkoCJNRxBtOWqAUdY30305505 = hBEAJkoCJNRxBtOWqAUdY57684910;     hBEAJkoCJNRxBtOWqAUdY57684910 = hBEAJkoCJNRxBtOWqAUdY75331804;     hBEAJkoCJNRxBtOWqAUdY75331804 = hBEAJkoCJNRxBtOWqAUdY59265999;     hBEAJkoCJNRxBtOWqAUdY59265999 = hBEAJkoCJNRxBtOWqAUdY37273151;     hBEAJkoCJNRxBtOWqAUdY37273151 = hBEAJkoCJNRxBtOWqAUdY62791550;     hBEAJkoCJNRxBtOWqAUdY62791550 = hBEAJkoCJNRxBtOWqAUdY50033589;     hBEAJkoCJNRxBtOWqAUdY50033589 = hBEAJkoCJNRxBtOWqAUdY58935747;     hBEAJkoCJNRxBtOWqAUdY58935747 = hBEAJkoCJNRxBtOWqAUdY71377082;     hBEAJkoCJNRxBtOWqAUdY71377082 = hBEAJkoCJNRxBtOWqAUdY74774563;     hBEAJkoCJNRxBtOWqAUdY74774563 = hBEAJkoCJNRxBtOWqAUdY39000782;     hBEAJkoCJNRxBtOWqAUdY39000782 = hBEAJkoCJNRxBtOWqAUdY91248352;     hBEAJkoCJNRxBtOWqAUdY91248352 = hBEAJkoCJNRxBtOWqAUdY39175652;     hBEAJkoCJNRxBtOWqAUdY39175652 = hBEAJkoCJNRxBtOWqAUdY45749369;     hBEAJkoCJNRxBtOWqAUdY45749369 = hBEAJkoCJNRxBtOWqAUdY50829444;     hBEAJkoCJNRxBtOWqAUdY50829444 = hBEAJkoCJNRxBtOWqAUdY49972429;     hBEAJkoCJNRxBtOWqAUdY49972429 = hBEAJkoCJNRxBtOWqAUdY44809323;     hBEAJkoCJNRxBtOWqAUdY44809323 = hBEAJkoCJNRxBtOWqAUdY74497035;     hBEAJkoCJNRxBtOWqAUdY74497035 = hBEAJkoCJNRxBtOWqAUdY34474732;     hBEAJkoCJNRxBtOWqAUdY34474732 = hBEAJkoCJNRxBtOWqAUdY80322753;     hBEAJkoCJNRxBtOWqAUdY80322753 = hBEAJkoCJNRxBtOWqAUdY27960365;     hBEAJkoCJNRxBtOWqAUdY27960365 = hBEAJkoCJNRxBtOWqAUdY24630638;     hBEAJkoCJNRxBtOWqAUdY24630638 = hBEAJkoCJNRxBtOWqAUdY3509354;     hBEAJkoCJNRxBtOWqAUdY3509354 = hBEAJkoCJNRxBtOWqAUdY95322692;     hBEAJkoCJNRxBtOWqAUdY95322692 = hBEAJkoCJNRxBtOWqAUdY80213304;     hBEAJkoCJNRxBtOWqAUdY80213304 = hBEAJkoCJNRxBtOWqAUdY48850712;     hBEAJkoCJNRxBtOWqAUdY48850712 = hBEAJkoCJNRxBtOWqAUdY66937094;     hBEAJkoCJNRxBtOWqAUdY66937094 = hBEAJkoCJNRxBtOWqAUdY26393966;     hBEAJkoCJNRxBtOWqAUdY26393966 = hBEAJkoCJNRxBtOWqAUdY66363943;}
// Junk Finished
