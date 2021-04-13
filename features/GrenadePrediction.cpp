#include "GrenadePrediction.h"

void CCSGrenadeHint::Tick(int buttons)
{
	if (!XSystemCFG.visuals_others_grenade_pred)
		return;

	bool in_attack = (buttons & IN_ATTACK);
	bool in_attack2 = (buttons & IN_ATTACK2);

	act = (in_attack && in_attack2) ? ACT_DROP :
		(in_attack2) ? ACT_THROW :
		(in_attack) ? ACT_LOB :
		ACT_NONE;
}

void CCSGrenadeHint::View()
{
	if (!XSystemCFG.visuals_others_grenade_pred)
		return;

	if (!g_LocalPlayer)
		return;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!weapon)
		return;

	if (weapon->IsGrenade())
	{
		QAngle Angles;
		g_EngineClient->GetViewAngles(Angles);

		ClientClass* pWeaponClass = weapon->GetClientClass();
		if (!pWeaponClass)
		{
			type = -1;
			Simulate(Angles, g_LocalPlayer);
		}
		else
		{
			type = pWeaponClass->m_ClassID;
			Simulate(Angles, g_LocalPlayer);
		}
	}
	else
	{
		type = -1;
	}
}

inline float CSGO_Armor(float flDamage, int ArmorValue) {
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast< float >(ArmorValue)) {
			flArmor = static_cast< float >(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

void CCSGrenadeHint::Paint()
{
	if (!XSystemCFG.visuals_others_grenade_pred)
		return;

	if (!g_LocalPlayer)
		return;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!weapon)
		return;

	if ((type) && path.size()>1 && weapon->IsGrenade())
	{
		Vector ab, cd;
		Vector prev = path[0];
		for (auto it = path.begin(), end = path.end(); it != end; ++it)
		{
			if (Math::WorldToScreen(prev, ab) && Math::WorldToScreen(*it, cd))
			{
				g_VGuiSurface->DrawSetColor(TracerColor);
				g_VGuiSurface->DrawLine(ab[0], ab[1], cd[0], cd[1]);
			}
			prev = *it;
		}

		for (auto it = OtherCollisions.begin(), end = OtherCollisions.end(); it != end; ++it)
		{
			Visuals::Draw3DCube(2.f, it->second, it->first, Color(0, 255, 0, 200));
		}

		Visuals::Draw3DCube(2.f, OtherCollisions.rbegin()->second, OtherCollisions.rbegin()->first, Color(255, 0, 0, 200));

		std::string EntName;
		auto bestdmg = 0;
		static Color redcol = { 255, 0, 0, 255 };
		static Color greencol = { 25, 255, 25, 255 };
		static Color yellowgreencol = { 177, 253, 2, 255 };
		static Color yellowcol = { 255, 255, 0, 255 };
		static Color orangecol = { 255, 128, 0, 255 };
		Color *BestColor = &redcol;

		Vector endpos = path[path.size() - 1];
		Vector absendpos = endpos;

		float totaladded = 0.0f;

		while (totaladded < 30.0f) {
			if (g_EngineTrace->GetPointContents(endpos) == CONTENTS_EMPTY)
				break;

			totaladded += 2.0f;
			endpos.z += 2.0f;
		}

		C_BaseCombatWeapon* pWeapon = g_LocalPlayer->m_hActiveWeapon().Get();
		int weap_id = pWeapon->m_iItemDefinitionIndex();

		if (pWeapon &&
			weap_id == WEAPON_HEGRENADE ||
			weap_id == WEAPON_MOLOTOV ||
			weap_id == WEAPON_INC) {
			for (int i = 1; i < 64; i++) {
				C_BasePlayer *pEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

				if (!pEntity || pEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
					continue;

				float dist = (pEntity->m_vecOrigin() - endpos).Length();

				if (dist < 350.0f) {
					CTraceFilter filter;
					filter.pSkip = g_LocalPlayer;
					Ray_t ray;
					Vector NadeScreen;
					Math::WorldToScreen(endpos, NadeScreen);

					Vector vPelvis = pEntity->GetBonePos(HITBOX_PELVIS);
					ray.Init(endpos, vPelvis);
					trace_t ptr;
					g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &ptr);

					if (ptr.hit_entity == pEntity) {
						Vector PelvisScreen;

						Math::WorldToScreen(vPelvis, PelvisScreen);

						static float a = 105.0f;
						static float b = 25.0f;
						static float c = 140.0f;

						float d = ((((pEntity->m_vecOrigin()) - prev).Length() - b) / c);
						float flDamage = a*exp(-d * d);
						auto dmg = max(static_cast<int>(ceilf(CSGO_Armor(flDamage, pEntity->m_ArmorValue()))), 0);

						Color *destcolor = dmg >= 65 ? &redcol : dmg >= 40.0f ? &orangecol : dmg >= 20.0f ? &yellowgreencol : &greencol;

						if (dmg > bestdmg) {
							EntName = pEntity->GetName();
							BestColor = destcolor;
							bestdmg = dmg;
						}
					}
				}
			}
		}

		if (bestdmg > 0.f) {
			if (weap_id != WEAPON_HEGRENADE)
			{
				if (Math::WorldToScreen(prev, cd))
					Visuals::DrawString(Visuals::ui_font, cd[0], cd[1] - 10, *BestColor, FONT_CENTER, firegrenade_didnt_hit ? "No collisions" : (EntName + " will be burnt.").c_str());
			}
			else
			{
				if (Math::WorldToScreen(*path.begin(), cd))
					Visuals::DrawString(Visuals::ui_font, cd[0], cd[1] - 10, *BestColor, FONT_CENTER, ("Most damage dealt to: " + EntName + " -" + std::to_string(bestdmg)).c_str());
			}
		}
	}
}

void CCSGrenadeHint::Setup(C_BasePlayer* pl, Vector& vecSrc, Vector& vecThrow, const QAngle& angEyeAngles)
{
	QAngle angThrow = angEyeAngles;
	float pitch = angThrow.pitch;

	if (pitch <= 90.0f)
	{
		if (pitch<-90.0f)
		{
			pitch += 360.0f;
		}
	}
	else
	{
		pitch -= 360.0f;
	}
	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	angThrow.pitch = a;

	// Gets ThrowVelocity from weapon files
	// Clamped to [15,750]
	float flVel = 750.0f * 0.9f;

	// Do magic on member of grenade object [esi+9E4h]
	// m1=1  m1+m2=0.5  m2=0
	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	// Clamped to [0,1]
	b = b * 0.7f;
	b = b + 0.3f;
	flVel *= b;

	Vector vForward, vRight, vUp;
	Math::AngleVectors(angThrow, vForward, vRight, vUp);

	vecSrc = pl->m_vecOrigin();
	vecSrc += pl->m_vecViewOffset();
	float off = (power[act] * 12.0f) - 12.0f;
	vecSrc.z += off;

	// Game calls UTIL_TraceHull here with hull and assigns vecSrc tr.endpos
	trace_t tr;
	Vector vecDest = vecSrc;
	vecDest.MulAdd(vecDest, vForward, 22.0f);
	TraceHull(vecSrc, vecDest, tr);

	// After the hull trace it moves 6 units back along vForward
	Vector vecBack = vForward; vecBack *= 6.0f;
	vecSrc = tr.endpos;
	vecSrc -= vecBack;

	// Finally calculate velocity
	vecThrow = pl->m_vecVelocity(); vecThrow *= 1.25f;
	vecThrow.MulAdd(vecThrow, vForward, flVel);
}

void CCSGrenadeHint::Simulate(QAngle& Angles, C_BasePlayer* pLocal)
{
	Vector vecSrc, vecThrow;
	Setup(pLocal, vecSrc, vecThrow, Angles);

	float interval = g_GlobalVars->interval_per_tick;

	// Log positions 20 times per sec
	int logstep = static_cast<int>(0.05f / interval);
	int logtimer = 0;

	path.clear();
	OtherCollisions.clear();

	static float rainbow; // del this if dont work
	rainbow += 0.005; // del this if dont work

	if (rainbow > 1.f) // del this if dont work
		rainbow = 0.f; // del this if dont work

	Color rgbnade = Color::FromHSB(rainbow, 1.f, 1.f);

	TracerColor = Color(rgbnade);
	for (unsigned int i = 0; i<path.max_size() - 1; ++i)
	{
		if (!logtimer)
			path.push_back(vecSrc);

		int s = Step(vecSrc, vecThrow, i, interval);
		if ((s & 1) || vecThrow == Vector(0, 0, 0))
			break;

		// Reset the log timer every logstep OR we bounced
		if ((s & 2) || logtimer >= logstep) logtimer = 0;
		else ++logtimer;
	}
	path.push_back(vecSrc);
}

int CCSGrenadeHint::Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval)
{
	// Apply gravity
	Vector move;
	AddGravityMove(move, vecThrow, interval, false);

	// Push entity
	trace_t tr;
	PushEntity(vecSrc, move, tr);

	int result = 0;
	// Check ending conditions
	if (CheckDetonate(vecThrow, tr, tick, interval))
	{
		result |= 1;
	}

	// Resolve collisions
	if (tr.fraction != 1.0f)
	{
		result |= 2; // Collision!
		ResolveFlyCollisionCustom(tr, vecThrow, interval);
	}

	if (tr.hit_entity && ((C_BasePlayer*)tr.hit_entity)->IsPlayer())
	{
		TracerColor = Color(255, 0, 0, 255);
	}

	if ((result & 1) || vecThrow == Vector(0, 0, 0) || tr.fraction != 1.0f)
	{
		QAngle angles;
		Math::VectorAngles((tr.endpos - tr.startpos).Normalized(), angles);
		OtherCollisions.push_back(std::make_pair(tr.endpos, angles));
	}

	// Set new position
	vecSrc = tr.endpos;

	return result;
}

bool CCSGrenadeHint::CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval)
{
	firegrenade_didnt_hit = false;
	switch (type)
	{
	case ClassId_CSmokeGrenade:
	case ClassId_CDecoyGrenade:
		// Velocity must be <0.1, this is only checked every 0.2s
		if (vecThrow.Length()<0.1f)
		{
			int det_tick_mod = static_cast<int>(0.2f / interval);
			return !(tick%det_tick_mod);
		}
		return false;

	/* TIMES AREN'T COMPLETELY RIGHT FROM WHAT I'VE SEEN ! ! ! */
	case ClassId_CMolotovGrenade:
	case ClassId_CIncendiaryGrenade:
		// Detonate when hitting the floor
		if (tr.fraction != 1.0f && tr.plane.normal.z>0.7f)
			return true;
		// OR we've been flying for too long

	case ClassId_CFlashbang:
	case ClassId_CHEGrenade: {
		// Pure timer based, detonate at 1.5s, checked every 0.2s
		firegrenade_didnt_hit = static_cast<float>(tick)*interval > 1.5f && !(tick%static_cast<int>(0.2f / interval));
		return firegrenade_didnt_hit;
	}
	default:
		Assert(false);
		return false;
	}
}

void CCSGrenadeHint::TraceHull(Vector& src, Vector& end, trace_t& tr)
{
	// Setup grenade hull
	static const Vector hull[2] = { Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f) };

	CTraceFilter filter;
	filter.SetIgnoreClass("BaseCSGrenadeProjectile");
	filter.pSkip = g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

	Ray_t ray;
	ray.Init(src, end, hull[0], hull[1]);

	const unsigned int mask = 0x200400B;
	g_EngineTrace->TraceRay(ray, mask, &filter, &tr);
}

void CCSGrenadeHint::AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground)
{
	Vector basevel(0.0f, 0.0f, 0.0f);

	move.x = (vel.x + basevel.x) * frametime;
	move.y = (vel.y + basevel.y) * frametime;

	if (onground)
	{
		move.z = (vel.z + basevel.z) * frametime;
	}
	else
	{
		// Game calls GetActualGravity( this );
		float gravity = 800.0f * 0.4f;

		float newZ = vel.z - (gravity * frametime);
		move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;

		vel.z = newZ;
	}
}

void CCSGrenadeHint::PushEntity(Vector& src, const Vector& move, trace_t& tr)
{
	Vector vecAbsEnd = src;
	vecAbsEnd += move;

	// Trace through world
	TraceHull(src, vecAbsEnd, tr);
}

void CCSGrenadeHint::ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval)
{
	// Calculate elasticity
	float flSurfaceElasticity = 1.0;  // Assume all surfaces have the same elasticity
	float flGrenadeElasticity = 0.45f; // GetGrenadeElasticity()
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity>0.9f) flTotalElasticity = 0.9f;
	if (flTotalElasticity<0.0f) flTotalElasticity = 0.0f;

	// Calculate bounce
	Vector vecAbsVelocity;
	PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;

	// Stop completely once we move too slow
	float flSpeedSqr = vecAbsVelocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
	if (flSpeedSqr<flMinSpeedSqr)
	{
		vecAbsVelocity.Zero();
	}

	// Stop if on ground
	if (tr.plane.normal.z>0.7f)
	{
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity.Mul((1.0f - tr.fraction) * interval);
		PushEntity(tr.endpos, vecAbsVelocity, tr);
	}
	else
	{
		vecVelocity = vecAbsVelocity;
	}
}

int CCSGrenadeHint::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	float    backoff;
	float    change;
	float    angle;
	int        i, blocked;

	blocked = 0;

	angle = normal[2];

	if (angle > 0)
	{
		blocked |= 1;        // floor
	}
	if (!angle)
	{
		blocked |= 2;        // step
	}

	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i<3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		{
			out[i] = 0;
		}
	}

	return blocked;
}












































































































// Junk Code By Troll Face & Thaisen's Gen
void ULmekyiBTwVUJujvaAwjeTBbrwekxpYJNAjsqWam28645172() {     int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm67697582 = -206971976;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm22991160 = -464306461;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm31681098 = -788063874;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm58866661 = 55222077;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm16448194 = 42137062;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm96104699 = -717827161;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm10492533 = -119637345;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm21021208 = -420588592;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm98732461 = -179627183;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm4669863 = -295072717;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm32537549 = -782221291;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm41582520 = -605410105;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm81098422 = -110526179;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm89406091 = -202553965;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm47106868 = 39890319;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52192008 = 9758400;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm44487110 = -722133517;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm85581126 = -955307959;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm93521450 = -491929314;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm56228457 = -122265516;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm21960212 = -781325958;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm95572004 = -533041739;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm85247181 = -30528167;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm26886598 = -857645769;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm14467985 = -343961206;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm25942878 = -118546024;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm44343153 = -228079088;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm57831307 = -303128764;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm37731436 = -657476360;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm56663912 = -631974849;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42061329 = -488160279;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm50890531 = -437332742;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm71111573 = -107837408;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm99824789 = -971104009;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm24295187 = 97911376;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm12605478 = -480025383;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm88427164 = -552317516;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm9355579 = -653815075;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm6367256 = -70641610;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm57072242 = -227489486;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm87716425 = -969403269;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm79359550 = -899968262;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm43244493 = -412150673;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm83375012 = -680389160;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm2953354 = -643912998;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42564403 = -571113398;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm29973045 = -969514519;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm84377824 = 17079893;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm30430670 = -765408981;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm58504577 = -851340751;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm60783607 = -892259288;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm59246839 = 12075516;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm89122972 = -395135309;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm43091379 = -549303217;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm6684961 = -984304347;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm72125577 = -673930238;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm37743978 = -333778295;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm4794501 = -930418106;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm44398676 = -600816717;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm90505316 = -839316915;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm51761547 = -389748073;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52661226 = -816508582;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm83289772 = -763112233;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42068549 = -547652335;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm62608534 = -806912439;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm81647017 = -244888549;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm70470946 = -397572697;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm81273632 = -139422170;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm65110904 = -200465342;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm34501390 = -480084299;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm63764844 = -437924085;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm35131531 = 31681557;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm79213870 = -784666349;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm36449208 = -164439829;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm68512031 = -152862248;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42600662 = -881357696;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52327512 = -20891066;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm1872170 = -350139007;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm23933245 = -113732771;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm71903582 = -772847809;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm95969832 = -149031505;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm59965328 = -145158981;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm27400637 = -537719783;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm79226859 = -806135610;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm95880304 = -739715561;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm82814489 = -400235795;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm61767559 = 57802567;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm28020195 = -558534191;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm93139829 = -986799662;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52169610 = -228158387;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm74861500 = -46247088;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm83632664 = -621899411;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm64956903 = 47001642;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm15861940 = -231324696;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm5310696 = -837741413;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm35055200 = -52894688;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm96069778 = -36856030;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm1175944 = -864498338;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm20766479 = -873476722;    int LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm21306336 = -206971976;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm67697582 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm22991160;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm22991160 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm31681098;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm31681098 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm58866661;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm58866661 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm16448194;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm16448194 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm96104699;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm96104699 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm10492533;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm10492533 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm21021208;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm21021208 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm98732461;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm98732461 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm4669863;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm4669863 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm32537549;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm32537549 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm41582520;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm41582520 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm81098422;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm81098422 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm89406091;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm89406091 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm47106868;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm47106868 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52192008;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52192008 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm44487110;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm44487110 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm85581126;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm85581126 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm93521450;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm93521450 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm56228457;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm56228457 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm21960212;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm21960212 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm95572004;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm95572004 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm85247181;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm85247181 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm26886598;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm26886598 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm14467985;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm14467985 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm25942878;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm25942878 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm44343153;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm44343153 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm57831307;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm57831307 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm37731436;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm37731436 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm56663912;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm56663912 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42061329;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42061329 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm50890531;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm50890531 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm71111573;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm71111573 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm99824789;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm99824789 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm24295187;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm24295187 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm12605478;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm12605478 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm88427164;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm88427164 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm9355579;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm9355579 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm6367256;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm6367256 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm57072242;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm57072242 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm87716425;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm87716425 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm79359550;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm79359550 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm43244493;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm43244493 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm83375012;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm83375012 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm2953354;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm2953354 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42564403;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42564403 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm29973045;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm29973045 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm84377824;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm84377824 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm30430670;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm30430670 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm58504577;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm58504577 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm60783607;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm60783607 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm59246839;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm59246839 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm89122972;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm89122972 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm43091379;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm43091379 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm6684961;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm6684961 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm72125577;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm72125577 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm37743978;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm37743978 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm4794501;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm4794501 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm44398676;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm44398676 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm90505316;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm90505316 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm51761547;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm51761547 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52661226;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52661226 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm83289772;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm83289772 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42068549;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42068549 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm62608534;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm62608534 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm81647017;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm81647017 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm70470946;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm70470946 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm81273632;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm81273632 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm65110904;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm65110904 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm34501390;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm34501390 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm63764844;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm63764844 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm35131531;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm35131531 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm79213870;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm79213870 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm36449208;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm36449208 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm68512031;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm68512031 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42600662;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm42600662 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52327512;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52327512 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm1872170;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm1872170 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm23933245;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm23933245 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm71903582;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm71903582 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm95969832;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm95969832 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm59965328;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm59965328 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm27400637;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm27400637 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm79226859;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm79226859 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm95880304;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm95880304 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm82814489;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm82814489 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm61767559;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm61767559 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm28020195;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm28020195 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm93139829;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm93139829 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52169610;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm52169610 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm74861500;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm74861500 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm83632664;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm83632664 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm64956903;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm64956903 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm15861940;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm15861940 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm5310696;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm5310696 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm35055200;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm35055200 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm96069778;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm96069778 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm1175944;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm1175944 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm20766479;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm20766479 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm21306336;     LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm21306336 = LZMVDxwkdRfFKmsBKKmnTxDeXMGILrEDaJXFGfYtzuGgJSybIihfjmSNVixrrnYlMbeAOm67697582;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void HKiCKlfDbKNYrCEwSeteVRnkQlfVTbGdUjBePsOfLYCWtnckaGp86246835() {     float THurpWxmYGJjzDkEIAagaQOTxPQr95062306 = 20313223;    float THurpWxmYGJjzDkEIAagaQOTxPQr12811022 = 45546419;    float THurpWxmYGJjzDkEIAagaQOTxPQr62378362 = -179370936;    float THurpWxmYGJjzDkEIAagaQOTxPQr11304837 = -819498618;    float THurpWxmYGJjzDkEIAagaQOTxPQr20698243 = -344490634;    float THurpWxmYGJjzDkEIAagaQOTxPQr72883564 = -693950551;    float THurpWxmYGJjzDkEIAagaQOTxPQr89687932 = -670055964;    float THurpWxmYGJjzDkEIAagaQOTxPQr72693574 = -909493533;    float THurpWxmYGJjzDkEIAagaQOTxPQr83369351 = -155523751;    float THurpWxmYGJjzDkEIAagaQOTxPQr71661659 = -626953963;    float THurpWxmYGJjzDkEIAagaQOTxPQr27419425 = -813757990;    float THurpWxmYGJjzDkEIAagaQOTxPQr71303895 = -608651770;    float THurpWxmYGJjzDkEIAagaQOTxPQr14056617 = -490255237;    float THurpWxmYGJjzDkEIAagaQOTxPQr94634280 = -224966948;    float THurpWxmYGJjzDkEIAagaQOTxPQr64387772 = -820847250;    float THurpWxmYGJjzDkEIAagaQOTxPQr7229763 = -415500159;    float THurpWxmYGJjzDkEIAagaQOTxPQr59177019 = -462470101;    float THurpWxmYGJjzDkEIAagaQOTxPQr86209268 = -58137179;    float THurpWxmYGJjzDkEIAagaQOTxPQr3986514 = -735841000;    float THurpWxmYGJjzDkEIAagaQOTxPQr28592268 = 72939229;    float THurpWxmYGJjzDkEIAagaQOTxPQr43443005 = -775183071;    float THurpWxmYGJjzDkEIAagaQOTxPQr14526166 = -292695296;    float THurpWxmYGJjzDkEIAagaQOTxPQr91805054 = 48543465;    float THurpWxmYGJjzDkEIAagaQOTxPQr24409591 = -57536414;    float THurpWxmYGJjzDkEIAagaQOTxPQr63152756 = 92567161;    float THurpWxmYGJjzDkEIAagaQOTxPQr16374382 = -69987072;    float THurpWxmYGJjzDkEIAagaQOTxPQr97119090 = -975962456;    float THurpWxmYGJjzDkEIAagaQOTxPQr89291920 = -122950213;    float THurpWxmYGJjzDkEIAagaQOTxPQr11782627 = -288599253;    float THurpWxmYGJjzDkEIAagaQOTxPQr71891329 = -794725298;    float THurpWxmYGJjzDkEIAagaQOTxPQr42462622 = -612260411;    float THurpWxmYGJjzDkEIAagaQOTxPQr5828400 = -734218507;    float THurpWxmYGJjzDkEIAagaQOTxPQr78601415 = -909231373;    float THurpWxmYGJjzDkEIAagaQOTxPQr80769157 = -115180777;    float THurpWxmYGJjzDkEIAagaQOTxPQr94745956 = -65673398;    float THurpWxmYGJjzDkEIAagaQOTxPQr43034726 = 22863307;    float THurpWxmYGJjzDkEIAagaQOTxPQr30325952 = -471719695;    float THurpWxmYGJjzDkEIAagaQOTxPQr2571934 = -613675146;    float THurpWxmYGJjzDkEIAagaQOTxPQr5400666 = -827351761;    float THurpWxmYGJjzDkEIAagaQOTxPQr7539096 = -343950211;    float THurpWxmYGJjzDkEIAagaQOTxPQr55163472 = -981693860;    float THurpWxmYGJjzDkEIAagaQOTxPQr34552450 = 22177872;    float THurpWxmYGJjzDkEIAagaQOTxPQr50921117 = -432053684;    float THurpWxmYGJjzDkEIAagaQOTxPQr6376199 = -955427453;    float THurpWxmYGJjzDkEIAagaQOTxPQr32781252 = 70065965;    float THurpWxmYGJjzDkEIAagaQOTxPQr85458226 = -49031885;    float THurpWxmYGJjzDkEIAagaQOTxPQr28337134 = -493815777;    float THurpWxmYGJjzDkEIAagaQOTxPQr74490966 = -76414660;    float THurpWxmYGJjzDkEIAagaQOTxPQr37762263 = -67307908;    float THurpWxmYGJjzDkEIAagaQOTxPQr36495571 = -134978999;    float THurpWxmYGJjzDkEIAagaQOTxPQr84427762 = -935087844;    float THurpWxmYGJjzDkEIAagaQOTxPQr10803516 = -31806306;    float THurpWxmYGJjzDkEIAagaQOTxPQr4675278 = -830284958;    float THurpWxmYGJjzDkEIAagaQOTxPQr50199364 = -123990450;    float THurpWxmYGJjzDkEIAagaQOTxPQr42803831 = -1724429;    float THurpWxmYGJjzDkEIAagaQOTxPQr80536140 = -686991482;    float THurpWxmYGJjzDkEIAagaQOTxPQr21005968 = 97002954;    float THurpWxmYGJjzDkEIAagaQOTxPQr37968772 = -21834522;    float THurpWxmYGJjzDkEIAagaQOTxPQr48152080 = -812065780;    float THurpWxmYGJjzDkEIAagaQOTxPQr4323862 = -174503563;    float THurpWxmYGJjzDkEIAagaQOTxPQr75764474 = -717988096;    float THurpWxmYGJjzDkEIAagaQOTxPQr396013 = -447105752;    float THurpWxmYGJjzDkEIAagaQOTxPQr60910948 = -520894280;    float THurpWxmYGJjzDkEIAagaQOTxPQr11478022 = -360798453;    float THurpWxmYGJjzDkEIAagaQOTxPQr29199037 = 85306448;    float THurpWxmYGJjzDkEIAagaQOTxPQr21591025 = 20460516;    float THurpWxmYGJjzDkEIAagaQOTxPQr92702479 = -699420397;    float THurpWxmYGJjzDkEIAagaQOTxPQr33287459 = -275074461;    float THurpWxmYGJjzDkEIAagaQOTxPQr99888323 = -59293550;    float THurpWxmYGJjzDkEIAagaQOTxPQr21353046 = -743710558;    float THurpWxmYGJjzDkEIAagaQOTxPQr76903810 = -943780465;    float THurpWxmYGJjzDkEIAagaQOTxPQr56605085 = -848794956;    float THurpWxmYGJjzDkEIAagaQOTxPQr80808602 = -230785419;    float THurpWxmYGJjzDkEIAagaQOTxPQr96447417 = -291890789;    float THurpWxmYGJjzDkEIAagaQOTxPQr73428796 = 54633089;    float THurpWxmYGJjzDkEIAagaQOTxPQr8890555 = -697360944;    float THurpWxmYGJjzDkEIAagaQOTxPQr63605049 = -860641612;    float THurpWxmYGJjzDkEIAagaQOTxPQr85428855 = 3970918;    float THurpWxmYGJjzDkEIAagaQOTxPQr91628338 = -27602379;    float THurpWxmYGJjzDkEIAagaQOTxPQr77694529 = -858400954;    float THurpWxmYGJjzDkEIAagaQOTxPQr88037248 = -576171295;    float THurpWxmYGJjzDkEIAagaQOTxPQr22628125 = -799547796;    float THurpWxmYGJjzDkEIAagaQOTxPQr51529658 = 44357695;    float THurpWxmYGJjzDkEIAagaQOTxPQr75287055 = -53620255;    float THurpWxmYGJjzDkEIAagaQOTxPQr87463567 = -859637454;    float THurpWxmYGJjzDkEIAagaQOTxPQr31659107 = -480454106;    float THurpWxmYGJjzDkEIAagaQOTxPQr1153122 = -903933549;    float THurpWxmYGJjzDkEIAagaQOTxPQr28402052 = -685240924;    float THurpWxmYGJjzDkEIAagaQOTxPQr37965326 = -13456348;    float THurpWxmYGJjzDkEIAagaQOTxPQr14209816 = -378681917;    float THurpWxmYGJjzDkEIAagaQOTxPQr22028759 = 25860353;    float THurpWxmYGJjzDkEIAagaQOTxPQr92357180 = -349885173;    float THurpWxmYGJjzDkEIAagaQOTxPQr54419854 = -801609367;    float THurpWxmYGJjzDkEIAagaQOTxPQr1076804 = -552848199;    float THurpWxmYGJjzDkEIAagaQOTxPQr31774622 = -625962116;    float THurpWxmYGJjzDkEIAagaQOTxPQr54767460 = -434588108;    float THurpWxmYGJjzDkEIAagaQOTxPQr73641501 = -456927848;    float THurpWxmYGJjzDkEIAagaQOTxPQr39443095 = 28744768;    float THurpWxmYGJjzDkEIAagaQOTxPQr77177162 = -940733901;    float THurpWxmYGJjzDkEIAagaQOTxPQr11190228 = 20313223;     THurpWxmYGJjzDkEIAagaQOTxPQr95062306 = THurpWxmYGJjzDkEIAagaQOTxPQr12811022;     THurpWxmYGJjzDkEIAagaQOTxPQr12811022 = THurpWxmYGJjzDkEIAagaQOTxPQr62378362;     THurpWxmYGJjzDkEIAagaQOTxPQr62378362 = THurpWxmYGJjzDkEIAagaQOTxPQr11304837;     THurpWxmYGJjzDkEIAagaQOTxPQr11304837 = THurpWxmYGJjzDkEIAagaQOTxPQr20698243;     THurpWxmYGJjzDkEIAagaQOTxPQr20698243 = THurpWxmYGJjzDkEIAagaQOTxPQr72883564;     THurpWxmYGJjzDkEIAagaQOTxPQr72883564 = THurpWxmYGJjzDkEIAagaQOTxPQr89687932;     THurpWxmYGJjzDkEIAagaQOTxPQr89687932 = THurpWxmYGJjzDkEIAagaQOTxPQr72693574;     THurpWxmYGJjzDkEIAagaQOTxPQr72693574 = THurpWxmYGJjzDkEIAagaQOTxPQr83369351;     THurpWxmYGJjzDkEIAagaQOTxPQr83369351 = THurpWxmYGJjzDkEIAagaQOTxPQr71661659;     THurpWxmYGJjzDkEIAagaQOTxPQr71661659 = THurpWxmYGJjzDkEIAagaQOTxPQr27419425;     THurpWxmYGJjzDkEIAagaQOTxPQr27419425 = THurpWxmYGJjzDkEIAagaQOTxPQr71303895;     THurpWxmYGJjzDkEIAagaQOTxPQr71303895 = THurpWxmYGJjzDkEIAagaQOTxPQr14056617;     THurpWxmYGJjzDkEIAagaQOTxPQr14056617 = THurpWxmYGJjzDkEIAagaQOTxPQr94634280;     THurpWxmYGJjzDkEIAagaQOTxPQr94634280 = THurpWxmYGJjzDkEIAagaQOTxPQr64387772;     THurpWxmYGJjzDkEIAagaQOTxPQr64387772 = THurpWxmYGJjzDkEIAagaQOTxPQr7229763;     THurpWxmYGJjzDkEIAagaQOTxPQr7229763 = THurpWxmYGJjzDkEIAagaQOTxPQr59177019;     THurpWxmYGJjzDkEIAagaQOTxPQr59177019 = THurpWxmYGJjzDkEIAagaQOTxPQr86209268;     THurpWxmYGJjzDkEIAagaQOTxPQr86209268 = THurpWxmYGJjzDkEIAagaQOTxPQr3986514;     THurpWxmYGJjzDkEIAagaQOTxPQr3986514 = THurpWxmYGJjzDkEIAagaQOTxPQr28592268;     THurpWxmYGJjzDkEIAagaQOTxPQr28592268 = THurpWxmYGJjzDkEIAagaQOTxPQr43443005;     THurpWxmYGJjzDkEIAagaQOTxPQr43443005 = THurpWxmYGJjzDkEIAagaQOTxPQr14526166;     THurpWxmYGJjzDkEIAagaQOTxPQr14526166 = THurpWxmYGJjzDkEIAagaQOTxPQr91805054;     THurpWxmYGJjzDkEIAagaQOTxPQr91805054 = THurpWxmYGJjzDkEIAagaQOTxPQr24409591;     THurpWxmYGJjzDkEIAagaQOTxPQr24409591 = THurpWxmYGJjzDkEIAagaQOTxPQr63152756;     THurpWxmYGJjzDkEIAagaQOTxPQr63152756 = THurpWxmYGJjzDkEIAagaQOTxPQr16374382;     THurpWxmYGJjzDkEIAagaQOTxPQr16374382 = THurpWxmYGJjzDkEIAagaQOTxPQr97119090;     THurpWxmYGJjzDkEIAagaQOTxPQr97119090 = THurpWxmYGJjzDkEIAagaQOTxPQr89291920;     THurpWxmYGJjzDkEIAagaQOTxPQr89291920 = THurpWxmYGJjzDkEIAagaQOTxPQr11782627;     THurpWxmYGJjzDkEIAagaQOTxPQr11782627 = THurpWxmYGJjzDkEIAagaQOTxPQr71891329;     THurpWxmYGJjzDkEIAagaQOTxPQr71891329 = THurpWxmYGJjzDkEIAagaQOTxPQr42462622;     THurpWxmYGJjzDkEIAagaQOTxPQr42462622 = THurpWxmYGJjzDkEIAagaQOTxPQr5828400;     THurpWxmYGJjzDkEIAagaQOTxPQr5828400 = THurpWxmYGJjzDkEIAagaQOTxPQr78601415;     THurpWxmYGJjzDkEIAagaQOTxPQr78601415 = THurpWxmYGJjzDkEIAagaQOTxPQr80769157;     THurpWxmYGJjzDkEIAagaQOTxPQr80769157 = THurpWxmYGJjzDkEIAagaQOTxPQr94745956;     THurpWxmYGJjzDkEIAagaQOTxPQr94745956 = THurpWxmYGJjzDkEIAagaQOTxPQr43034726;     THurpWxmYGJjzDkEIAagaQOTxPQr43034726 = THurpWxmYGJjzDkEIAagaQOTxPQr30325952;     THurpWxmYGJjzDkEIAagaQOTxPQr30325952 = THurpWxmYGJjzDkEIAagaQOTxPQr2571934;     THurpWxmYGJjzDkEIAagaQOTxPQr2571934 = THurpWxmYGJjzDkEIAagaQOTxPQr5400666;     THurpWxmYGJjzDkEIAagaQOTxPQr5400666 = THurpWxmYGJjzDkEIAagaQOTxPQr7539096;     THurpWxmYGJjzDkEIAagaQOTxPQr7539096 = THurpWxmYGJjzDkEIAagaQOTxPQr55163472;     THurpWxmYGJjzDkEIAagaQOTxPQr55163472 = THurpWxmYGJjzDkEIAagaQOTxPQr34552450;     THurpWxmYGJjzDkEIAagaQOTxPQr34552450 = THurpWxmYGJjzDkEIAagaQOTxPQr50921117;     THurpWxmYGJjzDkEIAagaQOTxPQr50921117 = THurpWxmYGJjzDkEIAagaQOTxPQr6376199;     THurpWxmYGJjzDkEIAagaQOTxPQr6376199 = THurpWxmYGJjzDkEIAagaQOTxPQr32781252;     THurpWxmYGJjzDkEIAagaQOTxPQr32781252 = THurpWxmYGJjzDkEIAagaQOTxPQr85458226;     THurpWxmYGJjzDkEIAagaQOTxPQr85458226 = THurpWxmYGJjzDkEIAagaQOTxPQr28337134;     THurpWxmYGJjzDkEIAagaQOTxPQr28337134 = THurpWxmYGJjzDkEIAagaQOTxPQr74490966;     THurpWxmYGJjzDkEIAagaQOTxPQr74490966 = THurpWxmYGJjzDkEIAagaQOTxPQr37762263;     THurpWxmYGJjzDkEIAagaQOTxPQr37762263 = THurpWxmYGJjzDkEIAagaQOTxPQr36495571;     THurpWxmYGJjzDkEIAagaQOTxPQr36495571 = THurpWxmYGJjzDkEIAagaQOTxPQr84427762;     THurpWxmYGJjzDkEIAagaQOTxPQr84427762 = THurpWxmYGJjzDkEIAagaQOTxPQr10803516;     THurpWxmYGJjzDkEIAagaQOTxPQr10803516 = THurpWxmYGJjzDkEIAagaQOTxPQr4675278;     THurpWxmYGJjzDkEIAagaQOTxPQr4675278 = THurpWxmYGJjzDkEIAagaQOTxPQr50199364;     THurpWxmYGJjzDkEIAagaQOTxPQr50199364 = THurpWxmYGJjzDkEIAagaQOTxPQr42803831;     THurpWxmYGJjzDkEIAagaQOTxPQr42803831 = THurpWxmYGJjzDkEIAagaQOTxPQr80536140;     THurpWxmYGJjzDkEIAagaQOTxPQr80536140 = THurpWxmYGJjzDkEIAagaQOTxPQr21005968;     THurpWxmYGJjzDkEIAagaQOTxPQr21005968 = THurpWxmYGJjzDkEIAagaQOTxPQr37968772;     THurpWxmYGJjzDkEIAagaQOTxPQr37968772 = THurpWxmYGJjzDkEIAagaQOTxPQr48152080;     THurpWxmYGJjzDkEIAagaQOTxPQr48152080 = THurpWxmYGJjzDkEIAagaQOTxPQr4323862;     THurpWxmYGJjzDkEIAagaQOTxPQr4323862 = THurpWxmYGJjzDkEIAagaQOTxPQr75764474;     THurpWxmYGJjzDkEIAagaQOTxPQr75764474 = THurpWxmYGJjzDkEIAagaQOTxPQr396013;     THurpWxmYGJjzDkEIAagaQOTxPQr396013 = THurpWxmYGJjzDkEIAagaQOTxPQr60910948;     THurpWxmYGJjzDkEIAagaQOTxPQr60910948 = THurpWxmYGJjzDkEIAagaQOTxPQr11478022;     THurpWxmYGJjzDkEIAagaQOTxPQr11478022 = THurpWxmYGJjzDkEIAagaQOTxPQr29199037;     THurpWxmYGJjzDkEIAagaQOTxPQr29199037 = THurpWxmYGJjzDkEIAagaQOTxPQr21591025;     THurpWxmYGJjzDkEIAagaQOTxPQr21591025 = THurpWxmYGJjzDkEIAagaQOTxPQr92702479;     THurpWxmYGJjzDkEIAagaQOTxPQr92702479 = THurpWxmYGJjzDkEIAagaQOTxPQr33287459;     THurpWxmYGJjzDkEIAagaQOTxPQr33287459 = THurpWxmYGJjzDkEIAagaQOTxPQr99888323;     THurpWxmYGJjzDkEIAagaQOTxPQr99888323 = THurpWxmYGJjzDkEIAagaQOTxPQr21353046;     THurpWxmYGJjzDkEIAagaQOTxPQr21353046 = THurpWxmYGJjzDkEIAagaQOTxPQr76903810;     THurpWxmYGJjzDkEIAagaQOTxPQr76903810 = THurpWxmYGJjzDkEIAagaQOTxPQr56605085;     THurpWxmYGJjzDkEIAagaQOTxPQr56605085 = THurpWxmYGJjzDkEIAagaQOTxPQr80808602;     THurpWxmYGJjzDkEIAagaQOTxPQr80808602 = THurpWxmYGJjzDkEIAagaQOTxPQr96447417;     THurpWxmYGJjzDkEIAagaQOTxPQr96447417 = THurpWxmYGJjzDkEIAagaQOTxPQr73428796;     THurpWxmYGJjzDkEIAagaQOTxPQr73428796 = THurpWxmYGJjzDkEIAagaQOTxPQr8890555;     THurpWxmYGJjzDkEIAagaQOTxPQr8890555 = THurpWxmYGJjzDkEIAagaQOTxPQr63605049;     THurpWxmYGJjzDkEIAagaQOTxPQr63605049 = THurpWxmYGJjzDkEIAagaQOTxPQr85428855;     THurpWxmYGJjzDkEIAagaQOTxPQr85428855 = THurpWxmYGJjzDkEIAagaQOTxPQr91628338;     THurpWxmYGJjzDkEIAagaQOTxPQr91628338 = THurpWxmYGJjzDkEIAagaQOTxPQr77694529;     THurpWxmYGJjzDkEIAagaQOTxPQr77694529 = THurpWxmYGJjzDkEIAagaQOTxPQr88037248;     THurpWxmYGJjzDkEIAagaQOTxPQr88037248 = THurpWxmYGJjzDkEIAagaQOTxPQr22628125;     THurpWxmYGJjzDkEIAagaQOTxPQr22628125 = THurpWxmYGJjzDkEIAagaQOTxPQr51529658;     THurpWxmYGJjzDkEIAagaQOTxPQr51529658 = THurpWxmYGJjzDkEIAagaQOTxPQr75287055;     THurpWxmYGJjzDkEIAagaQOTxPQr75287055 = THurpWxmYGJjzDkEIAagaQOTxPQr87463567;     THurpWxmYGJjzDkEIAagaQOTxPQr87463567 = THurpWxmYGJjzDkEIAagaQOTxPQr31659107;     THurpWxmYGJjzDkEIAagaQOTxPQr31659107 = THurpWxmYGJjzDkEIAagaQOTxPQr1153122;     THurpWxmYGJjzDkEIAagaQOTxPQr1153122 = THurpWxmYGJjzDkEIAagaQOTxPQr28402052;     THurpWxmYGJjzDkEIAagaQOTxPQr28402052 = THurpWxmYGJjzDkEIAagaQOTxPQr37965326;     THurpWxmYGJjzDkEIAagaQOTxPQr37965326 = THurpWxmYGJjzDkEIAagaQOTxPQr14209816;     THurpWxmYGJjzDkEIAagaQOTxPQr14209816 = THurpWxmYGJjzDkEIAagaQOTxPQr22028759;     THurpWxmYGJjzDkEIAagaQOTxPQr22028759 = THurpWxmYGJjzDkEIAagaQOTxPQr92357180;     THurpWxmYGJjzDkEIAagaQOTxPQr92357180 = THurpWxmYGJjzDkEIAagaQOTxPQr54419854;     THurpWxmYGJjzDkEIAagaQOTxPQr54419854 = THurpWxmYGJjzDkEIAagaQOTxPQr1076804;     THurpWxmYGJjzDkEIAagaQOTxPQr1076804 = THurpWxmYGJjzDkEIAagaQOTxPQr31774622;     THurpWxmYGJjzDkEIAagaQOTxPQr31774622 = THurpWxmYGJjzDkEIAagaQOTxPQr54767460;     THurpWxmYGJjzDkEIAagaQOTxPQr54767460 = THurpWxmYGJjzDkEIAagaQOTxPQr73641501;     THurpWxmYGJjzDkEIAagaQOTxPQr73641501 = THurpWxmYGJjzDkEIAagaQOTxPQr39443095;     THurpWxmYGJjzDkEIAagaQOTxPQr39443095 = THurpWxmYGJjzDkEIAagaQOTxPQr77177162;     THurpWxmYGJjzDkEIAagaQOTxPQr77177162 = THurpWxmYGJjzDkEIAagaQOTxPQr11190228;     THurpWxmYGJjzDkEIAagaQOTxPQr11190228 = THurpWxmYGJjzDkEIAagaQOTxPQr95062306;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void UFcALDiIyQQmzxKSVMtDdecPv15399042() {     long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ33469658 = -986842614;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ46943184 = -691278865;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ38669581 = -953088762;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ77958261 = -652828486;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52785592 = -450728292;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ11595937 = 87738956;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ26998418 = -198605642;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ68811138 = -799358822;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ65538180 = -219387287;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ71874428 = -592285448;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ79896740 = -397598323;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ40483994 = -303592703;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ19793462 = -916659785;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ33024105 = -834555896;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ57425108 = -964234005;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ47170997 = -170960714;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ43464170 = -923004675;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ88463639 = -674939734;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ56573695 = -583785530;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ23973205 = -835862579;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ98939281 = -265631247;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ82979023 = -411216047;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ81871703 = -493848653;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ78352098 = -147330774;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ56869291 = -544059276;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ76136713 = -901506950;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ85258907 = -71143028;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ34230641 = 55552185;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ93548855 = -616628378;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ39997116 = -819750324;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ91846951 = -613761370;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ31960184 = -625826960;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ90859426 = -981528506;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ45119732 = -174266764;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ2999678 = -946622816;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ40523716 = -501349461;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ49267283 = -486243203;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ90055798 = -403721294;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ34576615 = 53553136;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ24029380 = -23001722;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ177324 = 14770469;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ19544481 = -307707809;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ87252178 = -723526427;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ80109977 = 7545134;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ25610999 = 75197798;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50820693 = -866432710;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ2739289 = -104892412;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ34289086 = -406808992;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ48462439 = -398560907;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ47837506 = -754423438;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52293731 = -995490312;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ26741058 = -837956091;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50784423 = -737392212;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ94760673 = -63229926;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ16459760 = -879949894;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50490634 = -475626568;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ65071481 = -97430212;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ60317483 = -705757988;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ21088971 = -8769210;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ76648879 = -549221343;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ26337030 = -841118016;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ92767777 = -154157827;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ75262283 = -82730445;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ25541064 = -399636964;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ80027477 = -978524078;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ47936557 = -771771363;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ49624567 = -322064198;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ74673730 = -642393021;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ30024428 = -887933081;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ16901392 = -362884544;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ97903714 = -684717512;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ53408371 = -419283382;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ53887025 = -628492870;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ32544316 = -460783809;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ23795881 = -750633049;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ79394800 = -957923438;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ95726845 = -687689621;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ1761727 = -401393788;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52741100 = -122528573;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ6048598 = -677626566;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ73397425 = -696614539;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50969822 = -664334036;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ85768202 = -545886908;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ45711350 = -862204941;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ87703385 = -824260012;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ65105893 = -775805280;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ81175761 = -888434749;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ96098753 = -818298581;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ28659972 = -294316870;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52509043 = -370996249;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ75452235 = -303919249;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ88949800 = -780485215;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ68966828 = -294952084;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ57927735 = -397225521;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ97692349 = -181883707;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ7409547 = -831071704;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ44282198 = -124977365;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ61711114 = -223889463;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ82500 = -13930788;    long qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ77674442 = -986842614;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ33469658 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ46943184;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ46943184 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ38669581;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ38669581 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ77958261;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ77958261 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52785592;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52785592 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ11595937;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ11595937 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ26998418;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ26998418 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ68811138;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ68811138 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ65538180;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ65538180 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ71874428;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ71874428 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ79896740;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ79896740 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ40483994;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ40483994 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ19793462;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ19793462 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ33024105;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ33024105 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ57425108;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ57425108 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ47170997;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ47170997 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ43464170;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ43464170 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ88463639;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ88463639 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ56573695;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ56573695 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ23973205;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ23973205 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ98939281;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ98939281 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ82979023;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ82979023 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ81871703;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ81871703 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ78352098;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ78352098 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ56869291;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ56869291 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ76136713;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ76136713 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ85258907;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ85258907 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ34230641;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ34230641 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ93548855;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ93548855 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ39997116;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ39997116 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ91846951;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ91846951 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ31960184;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ31960184 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ90859426;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ90859426 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ45119732;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ45119732 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ2999678;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ2999678 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ40523716;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ40523716 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ49267283;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ49267283 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ90055798;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ90055798 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ34576615;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ34576615 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ24029380;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ24029380 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ177324;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ177324 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ19544481;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ19544481 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ87252178;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ87252178 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ80109977;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ80109977 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ25610999;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ25610999 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50820693;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50820693 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ2739289;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ2739289 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ34289086;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ34289086 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ48462439;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ48462439 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ47837506;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ47837506 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52293731;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52293731 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ26741058;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ26741058 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50784423;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50784423 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ94760673;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ94760673 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ16459760;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ16459760 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50490634;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50490634 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ65071481;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ65071481 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ60317483;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ60317483 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ21088971;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ21088971 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ76648879;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ76648879 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ26337030;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ26337030 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ92767777;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ92767777 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ75262283;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ75262283 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ25541064;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ25541064 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ80027477;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ80027477 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ47936557;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ47936557 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ49624567;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ49624567 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ74673730;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ74673730 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ30024428;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ30024428 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ16901392;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ16901392 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ97903714;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ97903714 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ53408371;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ53408371 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ53887025;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ53887025 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ32544316;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ32544316 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ23795881;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ23795881 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ79394800;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ79394800 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ95726845;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ95726845 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ1761727;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ1761727 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52741100;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52741100 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ6048598;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ6048598 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ73397425;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ73397425 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50969822;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ50969822 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ85768202;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ85768202 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ45711350;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ45711350 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ87703385;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ87703385 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ65105893;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ65105893 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ81175761;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ81175761 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ96098753;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ96098753 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ28659972;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ28659972 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52509043;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ52509043 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ75452235;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ75452235 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ88949800;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ88949800 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ68966828;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ68966828 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ57927735;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ57927735 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ97692349;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ97692349 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ7409547;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ7409547 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ44282198;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ44282198 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ61711114;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ61711114 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ82500;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ82500 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ77674442;     qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ77674442 = qRhTUHwkVDkcPUrcwrEekZWVXJJulEqcNEVmBodJUCeuZ33469658;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void aotFRBCUQHQqXkxztYtotHEwbExndofWRFCfS6720874() {     double HyrtUzPzMtVOYEEwtITjx40778666 = -284072067;    double HyrtUzPzMtVOYEEwtITjx45862593 = -218686724;    double HyrtUzPzMtVOYEEwtITjx24084087 = -900281044;    double HyrtUzPzMtVOYEEwtITjx75897257 = -731244469;    double HyrtUzPzMtVOYEEwtITjx49489560 = -36521118;    double HyrtUzPzMtVOYEEwtITjx93736939 = -196849287;    double HyrtUzPzMtVOYEEwtITjx88142460 = -62926483;    double HyrtUzPzMtVOYEEwtITjx8711219 = -796660611;    double HyrtUzPzMtVOYEEwtITjx78439356 = -216453099;    double HyrtUzPzMtVOYEEwtITjx18695441 = -324881761;    double HyrtUzPzMtVOYEEwtITjx25051880 = -513430276;    double HyrtUzPzMtVOYEEwtITjx77010559 = -334142246;    double HyrtUzPzMtVOYEEwtITjx39845958 = -317152697;    double HyrtUzPzMtVOYEEwtITjx92642739 = -745086874;    double HyrtUzPzMtVOYEEwtITjx30804451 = -595746970;    double HyrtUzPzMtVOYEEwtITjx50683831 = -808511114;    double HyrtUzPzMtVOYEEwtITjx91302332 = 31237530;    double HyrtUzPzMtVOYEEwtITjx52079782 = -336743139;    double HyrtUzPzMtVOYEEwtITjx52258416 = -351463920;    double HyrtUzPzMtVOYEEwtITjx70577243 = -103817559;    double HyrtUzPzMtVOYEEwtITjx43080164 = -580499736;    double HyrtUzPzMtVOYEEwtITjx11574792 = -460022833;    double HyrtUzPzMtVOYEEwtITjx55509039 = -553501613;    double HyrtUzPzMtVOYEEwtITjx21706637 = -240005265;    double HyrtUzPzMtVOYEEwtITjx82290407 = 54002866;    double HyrtUzPzMtVOYEEwtITjx71082885 = -729164833;    double HyrtUzPzMtVOYEEwtITjx75829433 = -875892012;    double HyrtUzPzMtVOYEEwtITjx68480292 = -707914223;    double HyrtUzPzMtVOYEEwtITjx11234005 = -167775027;    double HyrtUzPzMtVOYEEwtITjx23492122 = -210990215;    double HyrtUzPzMtVOYEEwtITjx86262995 = -351773326;    double HyrtUzPzMtVOYEEwtITjx55202377 = -853557348;    double HyrtUzPzMtVOYEEwtITjx30237441 = -597779848;    double HyrtUzPzMtVOYEEwtITjx81447952 = -683819167;    double HyrtUzPzMtVOYEEwtITjx26378092 = -941165701;    double HyrtUzPzMtVOYEEwtITjx12669782 = -223092878;    double HyrtUzPzMtVOYEEwtITjx64219951 = 5774724;    double HyrtUzPzMtVOYEEwtITjx36813319 = -983709212;    double HyrtUzPzMtVOYEEwtITjx1966603 = -454486395;    double HyrtUzPzMtVOYEEwtITjx47254553 = -747267223;    double HyrtUzPzMtVOYEEwtITjx9255625 = -276499063;    double HyrtUzPzMtVOYEEwtITjx40813840 = -854495123;    double HyrtUzPzMtVOYEEwtITjx26604529 = -709019761;    double HyrtUzPzMtVOYEEwtITjx25688008 = -379238570;    double HyrtUzPzMtVOYEEwtITjx81406213 = -417653334;    double HyrtUzPzMtVOYEEwtITjx11857477 = -320379259;    double HyrtUzPzMtVOYEEwtITjx29426485 = 97420208;    double HyrtUzPzMtVOYEEwtITjx75330007 = -779641975;    double HyrtUzPzMtVOYEEwtITjx42566991 = -45093834;    double HyrtUzPzMtVOYEEwtITjx74052869 = -868671369;    double HyrtUzPzMtVOYEEwtITjx27502714 = 54290488;    double HyrtUzPzMtVOYEEwtITjx35301681 = -86553971;    double HyrtUzPzMtVOYEEwtITjx23489262 = -860305838;    double HyrtUzPzMtVOYEEwtITjx32553155 = -870371252;    double HyrtUzPzMtVOYEEwtITjx63640709 = -192376447;    double HyrtUzPzMtVOYEEwtITjx29203875 = -824049235;    double HyrtUzPzMtVOYEEwtITjx90353554 = -665185112;    double HyrtUzPzMtVOYEEwtITjx2377451 = -560275780;    double HyrtUzPzMtVOYEEwtITjx93606850 = -685247335;    double HyrtUzPzMtVOYEEwtITjx78406675 = -307356285;    double HyrtUzPzMtVOYEEwtITjx17907506 = -320957276;    double HyrtUzPzMtVOYEEwtITjx19662169 = -355012261;    double HyrtUzPzMtVOYEEwtITjx97477213 = -528885585;    double HyrtUzPzMtVOYEEwtITjx54947234 = 94537116;    double HyrtUzPzMtVOYEEwtITjx32432445 = -973108436;    double HyrtUzPzMtVOYEEwtITjx69849503 = -659872929;    double HyrtUzPzMtVOYEEwtITjx46773119 = -736362398;    double HyrtUzPzMtVOYEEwtITjx58398005 = -633333530;    double HyrtUzPzMtVOYEEwtITjx66264648 = -803921174;    double HyrtUzPzMtVOYEEwtITjx18134670 = -272654092;    double HyrtUzPzMtVOYEEwtITjx86463880 = -714285839;    double HyrtUzPzMtVOYEEwtITjx54489013 = 14946741;    double HyrtUzPzMtVOYEEwtITjx50113180 = -882256745;    double HyrtUzPzMtVOYEEwtITjx5003863 = -604196697;    double HyrtUzPzMtVOYEEwtITjx61321618 = -827318497;    double HyrtUzPzMtVOYEEwtITjx2266325 = -726004614;    double HyrtUzPzMtVOYEEwtITjx84970263 = -751003073;    double HyrtUzPzMtVOYEEwtITjx29821031 = -74263044;    double HyrtUzPzMtVOYEEwtITjx40300423 = -822351932;    double HyrtUzPzMtVOYEEwtITjx70432930 = -625617875;    double HyrtUzPzMtVOYEEwtITjx41656400 = -726585041;    double HyrtUzPzMtVOYEEwtITjx499426 = 3749963;    double HyrtUzPzMtVOYEEwtITjx25913302 = -562820389;    double HyrtUzPzMtVOYEEwtITjx37181135 = -299103658;    double HyrtUzPzMtVOYEEwtITjx95989408 = -165280704;    double HyrtUzPzMtVOYEEwtITjx50961314 = -165219355;    double HyrtUzPzMtVOYEEwtITjx31713115 = -993251510;    double HyrtUzPzMtVOYEEwtITjx97684285 = -727408597;    double HyrtUzPzMtVOYEEwtITjx17807243 = -391442721;    double HyrtUzPzMtVOYEEwtITjx97174217 = -17116467;    double HyrtUzPzMtVOYEEwtITjx22316227 = -557907767;    double HyrtUzPzMtVOYEEwtITjx61842501 = -433949497;    double HyrtUzPzMtVOYEEwtITjx43206468 = -198461877;    double HyrtUzPzMtVOYEEwtITjx23559927 = -47130110;    double HyrtUzPzMtVOYEEwtITjx29347047 = -326309947;    double HyrtUzPzMtVOYEEwtITjx89593456 = -921486802;    double HyrtUzPzMtVOYEEwtITjx43336627 = -225609538;    double HyrtUzPzMtVOYEEwtITjx71657295 = -703556877;    double HyrtUzPzMtVOYEEwtITjx93255562 = -406130135;    double HyrtUzPzMtVOYEEwtITjx11556711 = -284072067;     HyrtUzPzMtVOYEEwtITjx40778666 = HyrtUzPzMtVOYEEwtITjx45862593;     HyrtUzPzMtVOYEEwtITjx45862593 = HyrtUzPzMtVOYEEwtITjx24084087;     HyrtUzPzMtVOYEEwtITjx24084087 = HyrtUzPzMtVOYEEwtITjx75897257;     HyrtUzPzMtVOYEEwtITjx75897257 = HyrtUzPzMtVOYEEwtITjx49489560;     HyrtUzPzMtVOYEEwtITjx49489560 = HyrtUzPzMtVOYEEwtITjx93736939;     HyrtUzPzMtVOYEEwtITjx93736939 = HyrtUzPzMtVOYEEwtITjx88142460;     HyrtUzPzMtVOYEEwtITjx88142460 = HyrtUzPzMtVOYEEwtITjx8711219;     HyrtUzPzMtVOYEEwtITjx8711219 = HyrtUzPzMtVOYEEwtITjx78439356;     HyrtUzPzMtVOYEEwtITjx78439356 = HyrtUzPzMtVOYEEwtITjx18695441;     HyrtUzPzMtVOYEEwtITjx18695441 = HyrtUzPzMtVOYEEwtITjx25051880;     HyrtUzPzMtVOYEEwtITjx25051880 = HyrtUzPzMtVOYEEwtITjx77010559;     HyrtUzPzMtVOYEEwtITjx77010559 = HyrtUzPzMtVOYEEwtITjx39845958;     HyrtUzPzMtVOYEEwtITjx39845958 = HyrtUzPzMtVOYEEwtITjx92642739;     HyrtUzPzMtVOYEEwtITjx92642739 = HyrtUzPzMtVOYEEwtITjx30804451;     HyrtUzPzMtVOYEEwtITjx30804451 = HyrtUzPzMtVOYEEwtITjx50683831;     HyrtUzPzMtVOYEEwtITjx50683831 = HyrtUzPzMtVOYEEwtITjx91302332;     HyrtUzPzMtVOYEEwtITjx91302332 = HyrtUzPzMtVOYEEwtITjx52079782;     HyrtUzPzMtVOYEEwtITjx52079782 = HyrtUzPzMtVOYEEwtITjx52258416;     HyrtUzPzMtVOYEEwtITjx52258416 = HyrtUzPzMtVOYEEwtITjx70577243;     HyrtUzPzMtVOYEEwtITjx70577243 = HyrtUzPzMtVOYEEwtITjx43080164;     HyrtUzPzMtVOYEEwtITjx43080164 = HyrtUzPzMtVOYEEwtITjx11574792;     HyrtUzPzMtVOYEEwtITjx11574792 = HyrtUzPzMtVOYEEwtITjx55509039;     HyrtUzPzMtVOYEEwtITjx55509039 = HyrtUzPzMtVOYEEwtITjx21706637;     HyrtUzPzMtVOYEEwtITjx21706637 = HyrtUzPzMtVOYEEwtITjx82290407;     HyrtUzPzMtVOYEEwtITjx82290407 = HyrtUzPzMtVOYEEwtITjx71082885;     HyrtUzPzMtVOYEEwtITjx71082885 = HyrtUzPzMtVOYEEwtITjx75829433;     HyrtUzPzMtVOYEEwtITjx75829433 = HyrtUzPzMtVOYEEwtITjx68480292;     HyrtUzPzMtVOYEEwtITjx68480292 = HyrtUzPzMtVOYEEwtITjx11234005;     HyrtUzPzMtVOYEEwtITjx11234005 = HyrtUzPzMtVOYEEwtITjx23492122;     HyrtUzPzMtVOYEEwtITjx23492122 = HyrtUzPzMtVOYEEwtITjx86262995;     HyrtUzPzMtVOYEEwtITjx86262995 = HyrtUzPzMtVOYEEwtITjx55202377;     HyrtUzPzMtVOYEEwtITjx55202377 = HyrtUzPzMtVOYEEwtITjx30237441;     HyrtUzPzMtVOYEEwtITjx30237441 = HyrtUzPzMtVOYEEwtITjx81447952;     HyrtUzPzMtVOYEEwtITjx81447952 = HyrtUzPzMtVOYEEwtITjx26378092;     HyrtUzPzMtVOYEEwtITjx26378092 = HyrtUzPzMtVOYEEwtITjx12669782;     HyrtUzPzMtVOYEEwtITjx12669782 = HyrtUzPzMtVOYEEwtITjx64219951;     HyrtUzPzMtVOYEEwtITjx64219951 = HyrtUzPzMtVOYEEwtITjx36813319;     HyrtUzPzMtVOYEEwtITjx36813319 = HyrtUzPzMtVOYEEwtITjx1966603;     HyrtUzPzMtVOYEEwtITjx1966603 = HyrtUzPzMtVOYEEwtITjx47254553;     HyrtUzPzMtVOYEEwtITjx47254553 = HyrtUzPzMtVOYEEwtITjx9255625;     HyrtUzPzMtVOYEEwtITjx9255625 = HyrtUzPzMtVOYEEwtITjx40813840;     HyrtUzPzMtVOYEEwtITjx40813840 = HyrtUzPzMtVOYEEwtITjx26604529;     HyrtUzPzMtVOYEEwtITjx26604529 = HyrtUzPzMtVOYEEwtITjx25688008;     HyrtUzPzMtVOYEEwtITjx25688008 = HyrtUzPzMtVOYEEwtITjx81406213;     HyrtUzPzMtVOYEEwtITjx81406213 = HyrtUzPzMtVOYEEwtITjx11857477;     HyrtUzPzMtVOYEEwtITjx11857477 = HyrtUzPzMtVOYEEwtITjx29426485;     HyrtUzPzMtVOYEEwtITjx29426485 = HyrtUzPzMtVOYEEwtITjx75330007;     HyrtUzPzMtVOYEEwtITjx75330007 = HyrtUzPzMtVOYEEwtITjx42566991;     HyrtUzPzMtVOYEEwtITjx42566991 = HyrtUzPzMtVOYEEwtITjx74052869;     HyrtUzPzMtVOYEEwtITjx74052869 = HyrtUzPzMtVOYEEwtITjx27502714;     HyrtUzPzMtVOYEEwtITjx27502714 = HyrtUzPzMtVOYEEwtITjx35301681;     HyrtUzPzMtVOYEEwtITjx35301681 = HyrtUzPzMtVOYEEwtITjx23489262;     HyrtUzPzMtVOYEEwtITjx23489262 = HyrtUzPzMtVOYEEwtITjx32553155;     HyrtUzPzMtVOYEEwtITjx32553155 = HyrtUzPzMtVOYEEwtITjx63640709;     HyrtUzPzMtVOYEEwtITjx63640709 = HyrtUzPzMtVOYEEwtITjx29203875;     HyrtUzPzMtVOYEEwtITjx29203875 = HyrtUzPzMtVOYEEwtITjx90353554;     HyrtUzPzMtVOYEEwtITjx90353554 = HyrtUzPzMtVOYEEwtITjx2377451;     HyrtUzPzMtVOYEEwtITjx2377451 = HyrtUzPzMtVOYEEwtITjx93606850;     HyrtUzPzMtVOYEEwtITjx93606850 = HyrtUzPzMtVOYEEwtITjx78406675;     HyrtUzPzMtVOYEEwtITjx78406675 = HyrtUzPzMtVOYEEwtITjx17907506;     HyrtUzPzMtVOYEEwtITjx17907506 = HyrtUzPzMtVOYEEwtITjx19662169;     HyrtUzPzMtVOYEEwtITjx19662169 = HyrtUzPzMtVOYEEwtITjx97477213;     HyrtUzPzMtVOYEEwtITjx97477213 = HyrtUzPzMtVOYEEwtITjx54947234;     HyrtUzPzMtVOYEEwtITjx54947234 = HyrtUzPzMtVOYEEwtITjx32432445;     HyrtUzPzMtVOYEEwtITjx32432445 = HyrtUzPzMtVOYEEwtITjx69849503;     HyrtUzPzMtVOYEEwtITjx69849503 = HyrtUzPzMtVOYEEwtITjx46773119;     HyrtUzPzMtVOYEEwtITjx46773119 = HyrtUzPzMtVOYEEwtITjx58398005;     HyrtUzPzMtVOYEEwtITjx58398005 = HyrtUzPzMtVOYEEwtITjx66264648;     HyrtUzPzMtVOYEEwtITjx66264648 = HyrtUzPzMtVOYEEwtITjx18134670;     HyrtUzPzMtVOYEEwtITjx18134670 = HyrtUzPzMtVOYEEwtITjx86463880;     HyrtUzPzMtVOYEEwtITjx86463880 = HyrtUzPzMtVOYEEwtITjx54489013;     HyrtUzPzMtVOYEEwtITjx54489013 = HyrtUzPzMtVOYEEwtITjx50113180;     HyrtUzPzMtVOYEEwtITjx50113180 = HyrtUzPzMtVOYEEwtITjx5003863;     HyrtUzPzMtVOYEEwtITjx5003863 = HyrtUzPzMtVOYEEwtITjx61321618;     HyrtUzPzMtVOYEEwtITjx61321618 = HyrtUzPzMtVOYEEwtITjx2266325;     HyrtUzPzMtVOYEEwtITjx2266325 = HyrtUzPzMtVOYEEwtITjx84970263;     HyrtUzPzMtVOYEEwtITjx84970263 = HyrtUzPzMtVOYEEwtITjx29821031;     HyrtUzPzMtVOYEEwtITjx29821031 = HyrtUzPzMtVOYEEwtITjx40300423;     HyrtUzPzMtVOYEEwtITjx40300423 = HyrtUzPzMtVOYEEwtITjx70432930;     HyrtUzPzMtVOYEEwtITjx70432930 = HyrtUzPzMtVOYEEwtITjx41656400;     HyrtUzPzMtVOYEEwtITjx41656400 = HyrtUzPzMtVOYEEwtITjx499426;     HyrtUzPzMtVOYEEwtITjx499426 = HyrtUzPzMtVOYEEwtITjx25913302;     HyrtUzPzMtVOYEEwtITjx25913302 = HyrtUzPzMtVOYEEwtITjx37181135;     HyrtUzPzMtVOYEEwtITjx37181135 = HyrtUzPzMtVOYEEwtITjx95989408;     HyrtUzPzMtVOYEEwtITjx95989408 = HyrtUzPzMtVOYEEwtITjx50961314;     HyrtUzPzMtVOYEEwtITjx50961314 = HyrtUzPzMtVOYEEwtITjx31713115;     HyrtUzPzMtVOYEEwtITjx31713115 = HyrtUzPzMtVOYEEwtITjx97684285;     HyrtUzPzMtVOYEEwtITjx97684285 = HyrtUzPzMtVOYEEwtITjx17807243;     HyrtUzPzMtVOYEEwtITjx17807243 = HyrtUzPzMtVOYEEwtITjx97174217;     HyrtUzPzMtVOYEEwtITjx97174217 = HyrtUzPzMtVOYEEwtITjx22316227;     HyrtUzPzMtVOYEEwtITjx22316227 = HyrtUzPzMtVOYEEwtITjx61842501;     HyrtUzPzMtVOYEEwtITjx61842501 = HyrtUzPzMtVOYEEwtITjx43206468;     HyrtUzPzMtVOYEEwtITjx43206468 = HyrtUzPzMtVOYEEwtITjx23559927;     HyrtUzPzMtVOYEEwtITjx23559927 = HyrtUzPzMtVOYEEwtITjx29347047;     HyrtUzPzMtVOYEEwtITjx29347047 = HyrtUzPzMtVOYEEwtITjx89593456;     HyrtUzPzMtVOYEEwtITjx89593456 = HyrtUzPzMtVOYEEwtITjx43336627;     HyrtUzPzMtVOYEEwtITjx43336627 = HyrtUzPzMtVOYEEwtITjx71657295;     HyrtUzPzMtVOYEEwtITjx71657295 = HyrtUzPzMtVOYEEwtITjx93255562;     HyrtUzPzMtVOYEEwtITjx93255562 = HyrtUzPzMtVOYEEwtITjx11556711;     HyrtUzPzMtVOYEEwtITjx11556711 = HyrtUzPzMtVOYEEwtITjx40778666;}
// Junk Finished
