#pragma once

#include "../Singleton.hpp"

#include "../Structs.hpp"

#include "../helpers/Math.hpp"

#include "Animation.hpp"
#include "..//SanityChech.hpp"
#include <deque>

class QAngle;
class C_BasePlayer;

struct ResolveInfo
{
	ResolveInfo()
	{
		m_bActive = false;

		m_flVelocity = 0.f;
		m_vecVelocity = Vector(0, 0, 0);
		m_angEyeAngles = QAngle(0, 0, 0);
		m_flSimulationTime = 0.f;
		m_flLowerBodyYawTarget = 0.f;

		m_flStandingTime = 0.f;
		m_flMovingLBY = 0.f;
		m_flLbyDelta = 180.f;
		m_bIsMoving = false;

		m_angDirectionFirstMoving = QAngle(0, 0, 0);
		m_nCorrectedFakewalkIdx = 0;
	}

	void SaveRecord(C_BasePlayer* player)
	{
		if (!player || player == nullptr)
			return;
		m_flLowerBodyYawTarget = player->m_flLowerBodyYawTarget();
		m_flSimulationTime = player->m_flSimulationTime();
		m_flVelocity = player->m_vecVelocity().Length2D();
		m_vecVelocity = player->m_vecVelocity();
		m_angEyeAngles = player->m_angEyeAngles();

		m_iLayerCount = player->GetNumAnimOverlays();
		for (int i = 0; i < m_iLayerCount; i++)
			animationLayer[i] = Animation::Get().GetPlayerAnimationInfo(player->EntIndex()).m_AnimationLayer[i];
	}

	bool operator==(const ResolveInfo& other)
	{
		return other.m_flSimulationTime == m_flSimulationTime;
	}

	bool m_bActive;

	float_t m_flVelocity;
	Vector m_vecVelocity;
	QAngle m_angEyeAngles;
	float_t m_flSimulationTime;
	float_t m_flLowerBodyYawTarget;

	int32_t m_iLayerCount = 0;
	AnimationLayer animationLayer[15];

	float_t m_flStandingTime;
	float_t m_flMovingLBY;
	float_t m_flLbyDelta;
	bool m_bIsMoving;

	QAngle m_angDirectionFirstMoving;
	int32_t m_nCorrectedFakewalkIdx;

	int32_t m_nShotsMissed = 0;
};

/*
struct lbyresinfo
{
	float_t m_flSimulationTime;
	float_t m_flLowerBodyYawTarget;
	QAngle m_angEyeAngles;

};

struct expresout
{
	float_t m_flSimulationTime;
	QAngle m_angEyeAngles;
	std::string modename;
};
*/


struct resolvrecord
{
	bool moving;
	bool moving2;
	bool shot;
	bool was_moving;
	bool update;
	bool saw_update;
	bool suppresing_animation;
	bool was_dormant;

	Vector origin;
	Vector update_origin;

	QAngle rawang;
	QAngle resolvedang;
	QAngle last_update_angle;
	float lby;
	float moving_lby;

	float raw_lby_delta;
	float moving_lby_delta;
	float lastlby_lby_delta;

	float_t simtime;
	float_t animtime;
	float_t last_moving_simtime;
	float_t last_standing_simtime;
	float_t last_update_simtime;

	int tickcount;

	CBaseHandle weapon_handle;

	int32_t layercount = 0;
	AnimationLayer animationLayer[15];

	void SaveRecord(C_BasePlayer* player)
	{
		was_dormant = false;
		suppresing_animation = false;
		shot = false;
		moving = player->m_vecVelocity().Length2D() >= 0.1;
		moving2 = moving;
		rawang = player->m_angEyeAngles();
		lby = player->m_flLowerBodyYawTarget();
		moving_lby = -10000;		//recording function should solve this

		origin = player->m_vecOrigin();

		update = false;
		saw_update = false;

		raw_lby_delta = rawang.yaw - lby;

		simtime = player->m_flSimulationTime();
		animtime = player->m_flOldSimulationTime() + g_GlobalVars->interval_per_tick;

		weapon_handle = player->m_hActiveWeapon();
		if (checks::is_bad_ptr(player->m_hActiveWeapon().Get()))
			shot = false;
		else
			shot = (animtime == player->m_hActiveWeapon().Get()->m_fLastShotTime());

		tickcount = g_GlobalVars->tickcount;

		layercount = player->GetNumAnimOverlays();
		was_moving = moving;
	}
};

struct angle_recording
{
	CBaseHandle* handle;
	Vector position;
	QAngle angle;

	void SaveRecord(resolvrecord input, C_BasePlayer* player)
	{
		handle = const_cast<CBaseHandle*>(&player->GetRefEHandle());
		position = input.origin;
		angle = QAngle(Math::ClampPitch(input.resolvedang.pitch), input.lby, 0);
	}
	void SaveRecord(C_BasePlayer* player, Vector pos, QAngle ang)
	{
		handle = const_cast<CBaseHandle*>(&player->GetRefEHandle());
		position = pos;
		angle = ang;
	}
};

class CSphere
{
public:
	Vector m_vecCenter;
	float   m_flRadius = 0.f;
	//float   m_flRadius2 = 0.f; // r^2

	CSphere(void) {};
	CSphere(const Vector& vecCenter, float flRadius, int hitgroup) { m_vecCenter = vecCenter; m_flRadius = flRadius; Hitgroup = hitgroup; };

	int Hitgroup;
	bool intersectsRay(const Ray_t& ray);
	bool intersectsRay(const Ray_t& ray, Vector& vecIntersection);
};

struct shot_record_local
{
	shot_record_local(int player)
	{
		src = g_LocalPlayer->GetEyePos();
		end = Global::vecAimpos;
		target = player;
		time = g_GlobalVars->curtime;
		hp = 0;
		processed = false;
		hit = false;
	}

	Vector src;
	Vector end;
	std::vector<Vector> impacts;
	float time;
	int target;
	int hp;
	bool processed;
	bool hit;
};

struct shot_record_enemy
{
	shot_record_enemy(int player)
	{
		src = C_BasePlayer::GetPlayerByIndex(player)->GetEyePos();
		time = g_GlobalVars->curtime;
	}
	Vector src;
	float time;

	std::vector<Vector> impacts;
	QAngle direction;
};

enum PredictResolveModes : unsigned int
{
	Static = 0,	//static real
	Spin,		//using spin aa in 2018? okay
	Flips,		//kinda retarded naming but idk how would i name it anything else, basically switching between x angles
	Freestand,	//prolly most common aa in 2018
	FuckIt		//b r u t e f o r c e
};

struct PredictResolveRecord
{
	PredictResolveRecord()
	{
		SawLbyUpdate = false;
		CurrentPredictMode = PredictResolveModes::FuckIt;
		LbyLastUpdate = 0.f;
		LbyPredictedUpdate = 0.f;

		DeltaFromPrediction = 0.f;
	}
	bool SawLbyUpdate;

	unsigned int CurrentPredictMode;

	std::deque<float> lby;
	float LbyLastUpdate;
	float LbyPredictedUpdate;

	float SpinSpeed;			//for spin prediction only lol
	float DeltaFromPrediction;	//for something like Freestand + 180 aa meme
};

class AimbotBulletImpactEvent : public IGameEventListener2
{
public:
	void FireGameEvent(IGameEvent* event);
	int  GetEventDebugID(void);

	void RegisterSelf();
	void UnregisterSelf();

	void process();

	std::vector<shot_record_local> records;
	std::vector<shot_record_enemy> records2;
};

class PredictResolve
{
public:
	void log(C_BasePlayer* player);			//logs informations into record
	float predict(C_BasePlayer* player);	//return predicted yaw value
private:
	float predictSpin(C_BasePlayer* player);
	float predictFlips(C_BasePlayer* player);
	float predictFreestand(C_BasePlayer* player);
	void predictFuckIt(C_BasePlayer* player);
private:
	PredictResolveRecord record[65];
};

class Resolver : public Singleton<Resolver>
{
public:

	void Log();
	void Resolve();
	void FakelagFix();

	void Override();

	ResolveInfo arr_infos[65];
	//std::deque<resolvrecord> self_res_records[65];
	std::deque<resolvrecord> records[65];
	std::deque<angle_recording> angle_records;

	int missed_shots[65];
	float networkedPlayerYaw[65], networkedPlayerPitch[65] = { 0 };

	AimbotBulletImpactEvent EventHandler;
private:

	float_t ResolveFakewalk(C_BasePlayer* player, ResolveInfo& curtickrecord);
	float_t ResolveBruteforce(C_BasePlayer* player, float baseangle = 0);
	bool IsEntityMoving(C_BasePlayer* player);
	bool IsAdjustingBalance(C_BasePlayer* player, ResolveInfo& record, AnimationLayer* layer);
	void ExperimentalResolve(C_BasePlayer* player);
	bool IsAdjustingStopMoving(C_BasePlayer* player, ResolveInfo& record, AnimationLayer* layer);
	bool IsFakewalking(C_BasePlayer* player, ResolveInfo& record);
	bool PFakewalkDetection(C_BasePlayer* player, resolvrecord& record);
	void REALSelfWrittenResolver(int playerindex);
	float SWResolver_yaw(C_BasePlayer* player);
	float SWResolver_pitch(C_BasePlayer* player);

	float GetLBYByCompairingTicks(int playerindex);
	float GetDeltaByCompairingTicks(int playerindex);

	bool Choking_Packets(int i);
	bool Has_Static_Real(float tolerance, int i);
	bool Has_Static_Yaw_Difference(float tolerance, int i);
	bool Has_Steady_Difference(float tolerance, int i);
	int Get_Diffrent_Deltas(float tolerance, int i);
	int Get_Diffrent_Fakes(float tolerance, int i);
	int Get_Diffrent_LBYs(float tolerance, int i);
	bool Delta_Keeps_Changing(float tolerance, int i);
	bool Fake_Unusuable(float tolerance, int i);
	bool LBY_Keeps_Changing(float tolerance, int i);

	bool IsFreestanding_thickness(C_BasePlayer* player, float& angle);
	int IsFreestanding_awall(C_BasePlayer* player);
	bool IsFreestanding_walldt(C_BasePlayer* player, float& angle);
	bool IsBreakingLBY(C_BasePlayer* player);
	bool Is979MEME(C_BasePlayer* player);
	float AnimationResolve(C_BasePlayer* player);
	float OnAirBrute(C_BasePlayer* player);

	bool bFirstUpdate[65], bSawUpdate[65] = { false };

	const inline float_t GetDelta(float_t a, float_t b)
	{
		return fabsf(Math::ClampYaw(a - b));
	}

	const inline bool IsDifferent(float a, float b, float tolerance = 10.f) {
		return (GetDelta(a, b) > tolerance);
	}

	const inline float_t LBYDelta(const ResolveInfo& v)
	{
		return v.m_angEyeAngles.yaw - v.m_flLowerBodyYawTarget;
	}
};