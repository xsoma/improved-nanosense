#pragma once

#include "../Structs.hpp"
#include "../Singleton.hpp"

struct FireBulletData
{
	Vector src;
	CGameTrace enter_trace;
	Vector direction;
	CTraceFilter filter;
	float trace_length;
	float trace_length_remaining;
	float current_damage;
	int penetrate_count;
};

struct Autowall_Return_Info
{
	int damage;
	int hitgroup;
	int penetration_count;
	bool did_penetrate_wall;
	float thickness;
	Vector end;
	C_BaseEntity* hit_entity;
};

struct Autowall_Info
{
	Vector start;
	Vector end;
	Vector current_position;
	Vector direction;

	CTraceFilter* filter;
	CGameTrace enter_trace;

	float thickness;
	float current_damage;
	int penetration_count;
};


class AngryPew : public Singleton<AngryPew>
{

public:

	void Work(CUserCmd *usercmd);
	float GetDamageVec(const Vector &vecPoint, C_BasePlayer *player, int hitbox);

	int GetTickbase(CUserCmd* ucmd = nullptr);
	bool CockRevolver();
	void ScaleDamage(int hitgroup, C_BasePlayer *player, float weapon_armor_ratio, float &current_damage);
	float BestHitPoint(C_BasePlayer *player, int prioritized, float minDmg, mstudiohitboxset_t *hitset, matrix3x4_t matrix[], Vector &vecOut);
	Vector CalculateBestPoint(C_BasePlayer *player, int prioritized, float minDmg, bool onlyPrioritized, matrix3x4_t matrix[]);
	Autowall_Return_Info GetDamageVec2(Vector start, Vector end, C_BasePlayer* from_entity = nullptr, C_BasePlayer* to_entity = nullptr, int specific_hitgroup = -1);
	bool CheckTarget(int i);

	void TargetEntities();
	bool TargetSpecificEnt(C_BasePlayer* pEnt);
	bool HitChance(QAngle angles, C_BasePlayer *ent, float chance);

	void AutoStop();
	void AutoCrouch();

	bool SimulateFireBullet(C_BaseCombatWeapon *weap, FireBulletData &data, C_BasePlayer *player, int hitbox);
	bool HandleBulletPenetration(WeapInfo_t *wpn_data, FireBulletData &data);
	bool TraceToExit(Vector &end, CGameTrace *enter_trace, Vector start, Vector dir, CGameTrace *exit_trace);
	bool IsBreakableEntity(C_BasePlayer *ent);
	void ClipTraceToPlayers(const Vector &vecAbsStart, const Vector &vecAbsEnd, unsigned int mask, ITraceFilter *filter, CGameTrace *tr);
	bool IsArmored(C_BasePlayer *player, int armorVal, int hitgroup);

	void traceIt(Vector &vecAbsStart, Vector &vecAbsEnd, unsigned int mask, C_BasePlayer *ign, CGameTrace *tr);

	CUserCmd *usercmd = nullptr;
	C_BaseCombatWeapon* local_weapon = nullptr;
	int prev_aimtarget = NULL;
	bool can_fire_weapon = false;
	float cur_time = 0.f;
};