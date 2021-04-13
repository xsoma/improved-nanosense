#include "Structs.hpp"
#include "helpers/Math.hpp"
#include "helpers/Utils.hpp"

#include "features/AngryPew.hpp" // for tickbase correction
#include "features/Animation.hpp"

bool C_BaseEntity::IsPlayer()
{
	return VT::vfunc<bool(__thiscall*)(C_BaseEntity*)>(this, 157)(this); // 156
}

bool C_BaseEntity::IsWeapon()
{
	return VT::vfunc<bool(__thiscall*)(C_BaseEntity*)>(this, 166)(this); // 163
}

bool C_BaseEntity::IsPlantedC4()
{
	return GetClientClass()->m_ClassID == ClassId_CPlantedC4;
}

bool C_BaseEntity::IsDefuseKit()
{
	return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
}

unsigned int C_BaseEntity::PhysicsSolidMaskForEntity()
{
	return VT::vfunc<unsigned int(__thiscall*)(C_BaseEntity*)>(this, 149)(this);
}

WeapInfo_t* C_BaseCombatWeapon::GetWeapInfo()
{
	if (!this || this == NULL)
		return NULL;

	typedef WeapInfo_t* (__thiscall* o_getWeapInfo)(void*);
	return VT::vfunc<o_getWeapInfo>(this, 460)(this); // 454 originally // 455 offset update 1
}


bool C_BasePlayer::SetupBonesExperimental(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime, bool fakelag_fix)
{
	if (boneMask == BONE_USED_BY_HITBOX)					//fixing values
		boneMask |= 0x200;
	if (currentTime == g_EngineClient->GetLastTimeStamp())
		currentTime += 3.5f;

	int checks_bak, occflag_bak;

	checks_bak = *(int*)((uintptr_t)this + 0xA30);
	occflag_bak = *(int*)((uintptr_t)this + 0xA28);

	*(int*)((uintptr_t)this + 0xA30) = g_GlobalVars->framecount; //we'll skip occlusion checks now
	*(int*)((uintptr_t)this + 0xA28) = 0;

	// must be set to retrive right matrix
	this->m_fFlags() |= 0xF0;

	//this->CorrectCSEntityBoneLayout(this->m_fFlags());	can't find these fucking index

	//Animation::Get().HandleAnimFix(this, false, !fakelag_fix, !fakelag_fix);

	bool returnval = this->SetupBones(pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	//Animation::Get().HandleAnimFix(this, true, !fakelag_fix, !fakelag_fix);

	// strip flags
	this->m_fFlags() &= ~(0xF0);

	// set new flags
	this->m_fFlags() |= 0x98;

	*(int*)((uintptr_t)this + 0xA30) = checks_bak;
	*(int*)((uintptr_t)this + 0xA28) = occflag_bak;

	// clear our bone cache so we dont look retarded when dead, should be easy to find. index is getabsangles + 0x2
	//this->ClearCSRagdollBoneCache();

	/* "new" animfix should do this by it self
	pBoneToWorldOut[3].m_flMatVal[0][2] *= 0.6431f;	//fix leg position
	pBoneToWorldOut[1].m_flMatVal[0][1] /= 0.2117f;
	*/

	return returnval;
}
//

bool C_BaseCombatWeapon::HasBullets()
{
	return !IsReloading() && m_iClip1() > 0;
}

bool C_BaseCombatWeapon::CanFire()
{
	if (IsReloading() || m_iClip1() <= 0)
		return false;

	if (!g_LocalPlayer)
		return false;

	float flServerTime = AngryPew::Get().GetTickbase() * g_GlobalVars->interval_per_tick;

	return m_flNextPrimaryAttack() < flServerTime;
}

bool C_BaseCombatWeapon::IsReloading()
{
	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandle("client.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

bool C_BaseCombatWeapon::IsNotPistol()
{
	switch (GetWeapInfo()->weapon_type())
	{
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsRifle()
{
	switch (GetWeapInfo()->weapon_type())
	{
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsShotgun()
{
	switch (GetWeapInfo()->weapon_type())
	{
	case WEAPONTYPE_SHOTGUN:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsSMG()
{
	switch (GetWeapInfo()->weapon_type())
	{
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsMG()
{
	switch (GetWeapInfo()->weapon_type())
	{
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsAssaultRifle()
{
	switch (GetWeapInfo()->weapon_type())
	{
	case WEAPONTYPE_RIFLE:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsPistol()
{
	switch (GetWeapInfo()->weapon_type())
	{
	case WEAPONTYPE_PISTOL:
		return true;
	default:
		return false;
	}
}



bool C_BaseCombatWeapon::IsPistol2()
{
	switch (m_iItemDefinitionIndex())
	{
	case WEAPON_P2000:
		return true;
	case WEAPON_GLOCK:
		return true;
	case WEAPON_USPS:
		return true;
	case WEAPON_TEC9:
		return true;
	case WEAPON_CZ75:
		return true;
	case WEAPON_FIVESEVEN:
		return true;
	case WEAPON_P250:
		return true;
	case WEAPON_ELITE:
		return true;
	case WEAPON_DEAGLE:
		return true;
	case WEAPON_REVOLVER:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsPistolNotDeagR8()
{
	switch (m_iItemDefinitionIndex())
	{
	case WEAPON_P2000:
		return true;
	case WEAPON_GLOCK:
		return true;
	case WEAPON_USPS:
		return true;
	case WEAPON_TEC9:
		return true;
	case WEAPON_CZ75:
		return true;
	case WEAPON_FIVESEVEN:
		return true;
	case WEAPON_P250:
		return true;
	case WEAPON_ELITE:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsSniper()
{
	switch (GetWeapInfo()->weapon_type())
	{
	case WEAPONTYPE_SNIPER_RIFLE:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsAutoSniper()
{
	switch (m_iItemDefinitionIndex())
	{
	case WEAPON_SCAR20:
		return true;
	case WEAPON_G3SG1:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsDeagR8()
{
	switch (m_iItemDefinitionIndex())
	{
	case WEAPON_REVOLVER:
		return true;
	case WEAPON_DEAGLE:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsScout()
{
	switch (m_iItemDefinitionIndex())
	{
	case WEAPON_SSG08:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsAutoScopeable()
{
	switch (m_iItemDefinitionIndex())
	{
	case WEAPON_SSG08:
		return true;
	case WEAPON_AWP:
		return true;

	case WEAPON_SG553:
		return true;
	case WEAPON_AUG:
		return true;

	case WEAPON_SCAR20:
		return true;
	case WEAPON_G3SG1:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsAWP()
{
	switch (m_iItemDefinitionIndex())
	{
	case WEAPON_AWP:
		return true;
	default:
		return false;
	}
}
bool C_BaseCombatWeapon::IsGrenade()
{
	switch (GetWeapInfo()->weapon_type())
	{
	case WEAPONTYPE_GRENADE:
		return true;
	default:
		return false;
	}
}

float C_BaseCombatWeapon::GetInaccuracy()
{
	return VT::vfunc<float(__thiscall*)(void*)>(this, 482)(this); // 477 Pre-Offset 20/09/2019
}

float C_BaseCombatWeapon::GetSpread()
{
	return VT::vfunc<float(__thiscall*)(void*)>(this, 452)(this); //
}

void C_BaseCombatWeapon::UpdateAccuracyPenalty()
{
	VT::vfunc<void(__thiscall*)(void*)>(this, 483)(this); //
}

bool C_BaseCombatWeapon::IsWeaponNonAim()
{
	int idx = m_iItemDefinitionIndex();

	return (idx == WEAPON_C4 || idx == WEAPON_KNIFE || idx == WEAPON_KNIFE_BAYONET || idx == WEAPON_KNIFE_BUTTERFLY || idx == WEAPON_KNIFE_FALCHION
		|| idx == WEAPON_KNIFE_FLIP || idx == WEAPON_KNIFE_GUT || idx == WEAPON_KNIFE_KARAMBIT || idx == WEAPON_KNIFE_M9_BAYONET || idx == WEAPON_KNIFE_PUSH
		|| idx == WEAPON_KNIFE_SURVIVAL_BOWIE || idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE_TACTICAL || idx == WEAPON_KNIFE_CSS || idx == WEAPON_FLASHBANG || idx == WEAPON_HEGRENADE
		|| idx == WEAPON_SMOKEGRENADE || idx == WEAPON_MOLOTOV || idx == WEAPON_DECOY || idx == WEAPON_INC);
}

bool C_BaseCombatWeapon::CanFirePostPone()
{
	float rdyTime = m_flPostponeFireReadyTime();

	if (rdyTime > 0 && rdyTime < g_GlobalVars->curtime)
		return true;

	return false;
}

char* C_BaseCombatWeapon::GetWeaponIcon()
{
	int id = this->m_iItemDefinitionIndex();
	switch (id)
	{
	case WEAPON_DEAGLE:
		return "F";
	case WEAPON_ELITE:
		return "S";
	case WEAPON_FIVESEVEN:
		return "U";
	case WEAPON_GLOCK:
		return "C";
	case WEAPON_AK47:
		return "B";
	case WEAPON_AUG:
		return "E";
	case WEAPON_AWP:
		return "R";
	case WEAPON_FAMAS:
		return "T";
	case WEAPON_G3SG1:
		return "I";
	case WEAPON_GALIL:
		return "V";
	case WEAPON_M249:
		return "Z";
	case WEAPON_M4A4:
	case WEAPON_M4A1S:
		return "W";
	case WEAPON_MAC10:
		return "L";
	case WEAPON_P90:
		return "M";
	case WEAPON_UMP45:
		return "Q";
	case WEAPON_XM1014:
		return "]";
	case WEAPON_BIZON:
		return "D";
	case WEAPON_MAG7:
		return "K";
	case WEAPON_NEGEV:
		return "Z";
	case WEAPON_SAWEDOFF:
		return "K";
	case WEAPON_TEC9:
		return "C";
	case WEAPON_MP7:
		return "X";
	case WEAPON_MP9:
		return "D";
	case WEAPON_NOVA:
		return "K";
	case WEAPON_P250:
	case WEAPON_USPS:
		//case WEAPON_TASER: //smh, add hints OR just show text, why not
	case WEAPON_P2000:
	case WEAPON_CZ75:
		return "Y";
	case WEAPON_SCAR20:
		return "I";
	case WEAPON_SG553:
		return "[";
	case WEAPON_SSG08:
		return "N";
	case WEAPON_KNIFE:
	case WEAPON_KNIFE_T:
	case WEAPON_KNIFE_BAYONET:
	case WEAPON_KNIFE_BUTTERFLY:
	case WEAPON_KNIFE_FALCHION:
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
	case WEAPON_KNIFE_FLIP:
	case WEAPON_KNIFE_GUT:
	case WEAPON_KNIFE_KARAMBIT:
	case WEAPON_KNIFE_M9_BAYONET:
	case WEAPON_KNIFE_PUSH:
	case WEAPON_KNIFE_CSS:
	case WEAPON_KNIFE_TACTICAL:
		return "J";
	case WEAPON_FLASHBANG:
	case WEAPON_DECOY:
		return "G";
	case WEAPON_HEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_INC:
		return "H";
	case WEAPON_SMOKEGRENADE:
		return "P";
	case WEAPON_C4:
		return "\\";
	case WEAPON_REVOLVER:
		return "F";
	default:
		return "";
	}
}

bool C_BaseCombatWeapon::IsInThrow()
{
	if (!m_bPinPulled() || (Global::userCMD->buttons & IN_ATTACK) || (Global::userCMD->buttons & IN_ATTACK2))
	{
		float throwTime = m_fThrowTime();

		if (throwTime > 0)
			return true;
	}
	return false;
}

float_t C_BasePlayer::m_flSpawnTime()
{
	return *(float_t*)((uintptr_t)this + 0xA350);
}

std::array<float, 24>& C_BasePlayer::m_flPoseParameter()
{
	static int _m_flPoseParameter = NetMngr::Get().getOffs("CBaseAnimating", "m_flPoseParameter");
	return *(std::array<float, 24>*)((uintptr_t)this + _m_flPoseParameter);
}

QAngle& C_BasePlayer::visuals_Angles()
{
	return *(QAngle*)((uintptr_t)this + NetMngr::Get().getOffs("CCSPlayer", "deadflag") + 4);
}

int32_t C_BasePlayer::GetMoveType()
{
	static unsigned int _m_MoveType = Utils::FindInDataMap(GetPredDescMap(), "m_MoveType");
	return *(int32_t*)((uintptr_t)this + _m_MoveType);
}

void C_BasePlayer::SetPoseAngles(float_t yaw, float_t pitch)
{
	auto& poses = m_flPoseParameter();
	poses[11] = (pitch + 90) / 180;
	poses[2] = (yaw + 180) / 360;
}

void C_BasePlayer::InvalidateBoneCache()
{
	unsigned long g_iModelBoneCounter = **(unsigned long**)(Offsets::invalidateBoneCache + 10);
	*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF;
	*(unsigned int*)((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1);
}

int C_BasePlayer::GetNumAnimOverlays()
{
	return *(int*)((DWORD)this + 0x298C);
}

AnimationLayer* C_BasePlayer::GetAnimOverlays()
{
	// to find offset: use 9/12/17 dll
	// sig: 55 8B EC 51 53 8B 5D 08 33 C0
	return *(AnimationLayer**)((DWORD)this + 0x2980);
}

AnimationLayer* C_BasePlayer::GetAnimOverlay(int i)
{
	if (i < 15)
		return &GetAnimOverlays()[i];
	return nullptr;
}

int C_BasePlayer::GetSequenceActivity(int sequence)
{
	auto hdr = g_MdlInfo->GetStudiomodel(this->GetModel());

	if (!hdr)
		return -1;

	// sig for stuidohdr_t version: 53 56 8B F1 8B DA 85 F6 74 55
	// sig for C_BaseAnimating version: 55 8B EC 83 7D 08 FF 56 8B F1 74 3D
	// c_csplayer vfunc 242, follow calls to find the function.

	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(Offsets::getSequenceActivity);

	return get_sequence_activity(this, hdr, sequence);
}

C_CSGOPlayerAnimState* C_BasePlayer::GetPlayerAnimState()
{
	return (C_CSGOPlayerAnimState*)((uintptr_t)this + 0x3914);
}

void C_BasePlayer::UpdateAnimationState(C_CSGOPlayerAnimState* state, QAngle angle)
{
	if (!state)
		return;

	static auto UpdateAnimState = Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");
	if (!UpdateAnimState)
		return;

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void C_BasePlayer::ResetAnimationState(C_CSGOPlayerAnimState* state)
{
	if (!state)
		return;

	using ResetAnimState_t = void(__thiscall*)(C_CSGOPlayerAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)Utils::PatternScan(GetModuleHandle("client.dll"), "56 6A 01 68 ? ? ? ? 8B F1");
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void C_BasePlayer::CreateAnimationState(C_CSGOPlayerAnimState* state)
{
	using CreateAnimState_t = void(__thiscall*)(C_CSGOPlayerAnimState*, C_BasePlayer*);
	static auto CreateAnimState = (CreateAnimState_t)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	if (!CreateAnimState)
		return;

	CreateAnimState(state, this);
}

CBoneAccessor* C_BasePlayer::GetBoneAccessor()
{
	return (CBoneAccessor*)((uintptr_t)this + 0x26A8);
}

CStudioHdr* C_BasePlayer::GetModelPtr()
{
	return *(CStudioHdr**)((uintptr_t)this + 0x294C);
}

void C_BasePlayer::StandardBlendingRules(CStudioHdr* hdr, Vector* pos, Quaternion* q, float_t curtime, int32_t boneMask)
{
	typedef void(__thiscall* o_StandardBlendingRules)(void*, CStudioHdr*, Vector*, Quaternion*, float_t, int32_t);
	VT::vfunc<o_StandardBlendingRules>(this, 201)(this, hdr, pos, q, curtime, boneMask);
}

void C_BasePlayer::BuildTransformations(CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& cameraTransform, int32_t boneMask, byte* computed)
{
	typedef void(__thiscall* o_BuildTransformations)(void*, CStudioHdr*, Vector*, Quaternion*, const matrix3x4_t&, int32_t, byte*);
	VT::vfunc<o_BuildTransformations>(this, 188)(this, hdr, pos, q, cameraTransform, boneMask, computed); // 185
}

bool C_BasePlayer::HandleBoneSetup(int32_t boneMask, matrix3x4_t* boneOut, float_t curtime)
{
	CStudioHdr* hdr = this->GetModelPtr();
	if (!hdr)
		return false;

	CBoneAccessor* accessor = this->GetBoneAccessor();
	if (!accessor)
		return false;

	matrix3x4_t* backup_matrix = accessor->GetBoneArrayForWrite();
	if (!backup_matrix)
		return false;

	Vector origin = this->m_vecOrigin();
	QAngle angles = this->m_angEyeAngles();

	Vector backup_origin = this->GetAbsOrigin();
	QAngle backup_angles = this->GetAbsAngles();

	std::array<float_t, 24> backup_poses;
	backup_poses = this->m_flPoseParameter();

	AnimationLayer backup_layers[15];
	std::memcpy(backup_layers, this->GetAnimOverlays(), (sizeof(AnimationLayer) * this->GetNumAnimOverlays()));

	alignas(16) matrix3x4_t parentTransform;
	Math::AngleMatrix(angles, origin, parentTransform);

	auto& anim_data = Animation::Get().GetPlayerAnimationInfo(this->EntIndex());

	this->SetAbsOrigin(origin);
	this->SetAbsAngles(angles);
	this->m_flPoseParameter() = anim_data.m_flPoseParameters;
	std::memcpy(this->GetAnimOverlays(), anim_data.m_AnimationLayer, (sizeof(AnimationLayer) * this->GetNumAnimOverlays()));

	Vector* pos = (Vector*)(g_pMemAlloc->Alloc(sizeof(Vector[128])));
	Quaternion* q = (Quaternion*)(g_pMemAlloc->Alloc(sizeof(Quaternion[128])));
	std::memset(pos, 0xFF, sizeof(pos));
	std::memset(q, 0xFF, sizeof(q));

	this->StandardBlendingRules(hdr, pos, q, curtime, boneMask);

	accessor->SetBoneArrayForWrite(boneOut);

	byte* computed = (byte*)(g_pMemAlloc->Alloc(sizeof(byte[0x20])));
	std::memset(computed, 0, sizeof(byte[0x20]));

	this->BuildTransformations(hdr, pos, q, parentTransform, boneMask, &computed[0]);

	accessor->SetBoneArrayForWrite(backup_matrix);

	this->SetAbsOrigin(backup_origin);
	this->SetAbsAngles(backup_angles);
	this->m_flPoseParameter() = backup_poses;
	std::memcpy(this->GetAnimOverlays(), backup_layers, (sizeof(AnimationLayer) * this->GetNumAnimOverlays()));

	return true;
}

bool C_BasePlayer::SetupBones2(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	auto backupval = *reinterpret_cast<uint8_t*>((uintptr_t)this + 0x274);

	*reinterpret_cast<uint8_t*>((uintptr_t)this + 0x274) = 0;
	bool setuped_bones = this->SetupBones(pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	*reinterpret_cast<uint8_t*>((uintptr_t)this + 0x274) = backupval;

	return setuped_bones;
}

const Vector C_BasePlayer::WorldSpaceCenter()
{
	Vector vecOrigin = m_vecOrigin();

	Vector min = this->GetCollideable()->OBBMins() + vecOrigin;
	Vector max = this->GetCollideable()->OBBMaxs() + vecOrigin;

	Vector size = max - min;
	size /= 2.f;
	size += min;

	return size;
}

Vector C_BasePlayer::GetEyePos()
{
	if (this != g_LocalPlayer)
	{
		Vector vecOrigin = m_vecOrigin();

		Vector vecMinDuckHull = g_GameMovement->GetPlayerMins(true);
		Vector vecMaxStandHull = g_GameMovement->GetPlayerMaxs(false);

		float_t flMore = vecMinDuckHull.z - vecMaxStandHull.z;

		Vector vecDuckViewOffset = g_GameMovement->GetPlayerViewOffset(true);
		Vector vecStandViewOffset = g_GameMovement->GetPlayerViewOffset(false);

		float_t flDuckAmount = m_flDuckAmount();

		float_t flTempZ = ((vecDuckViewOffset.z - flMore) * flDuckAmount) + (vecStandViewOffset.z * (1 - flDuckAmount));

		vecOrigin.z += flTempZ;

		return vecOrigin;
	}
	else
		return m_vecOrigin() + m_vecViewOffset();
}

player_info_t C_BasePlayer::GetPlayerInfo()
{
	player_info_t info;
	g_EngineClient->GetPlayerInfo(EntIndex(), &info);
	return info;
}

std::string C_BasePlayer::GetName(bool console_safe)
{
	// Cleans player's name so we don't get new line memes. Use this everywhere you get the players name.
	// Also, if you're going to use the console for its command and use the players name, set console_safe.
	player_info_t pinfo = this->GetPlayerInfo();

	char* pl_name = pinfo.szName;
	char buf[128];
	int c = 0;

	for (int i = 0; pl_name[i]; ++i)
	{
		if (c >= sizeof(buf) - 1)
			break;

		switch (pl_name[i])
		{
		case '"': if (console_safe) break;
		case '\\':
		case ';': if (console_safe) break;
		case '\n':
			break;
		default:
			buf[c++] = pl_name[i];
		}
	}

	buf[c] = '\0';
	return std::string(buf);
}

bool C_BasePlayer::IsAlive()
{
	return m_lifeState() == LIFE_ALIVE;
}

bool C_BasePlayer::HasC4()
{
	static auto fnHasC4
		= reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandle("client.dll"), "56 8B F1 85 F6 74 31")
			);

	return fnHasC4(this);
}

Vector C_BasePlayer::GetBonePos(int bone)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones2(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_GlobalVars->curtime)) {
		return Vector(boneMatrix[bone][0][3], boneMatrix[bone][1][3], boneMatrix[bone][2][3]);
	}
	return Vector(0, 0, 0);
}

int C_BasePlayer::GetBoneByName(const char* boneName)
{
	studiohdr_t* studioHdr = g_MdlInfo->GetStudiomodel(this->GetModel());
	if (!studioHdr)
		return -1;

	matrix3x4_t boneToWorldOut[MAXSTUDIOBONES];
	if (!this->SetupBones2(boneToWorldOut, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, this->m_flSimulationTime()))
		return -1;

	for (int i = 0; i < studioHdr->numbones; i++)
	{
		mstudiobone_t* studioKost = studioHdr->pBone(i);

		if (!studioKost)
			continue;

		if (studioKost->pszName() && strcmp(studioKost->pszName(), boneName) == 0)
			return i;
	}

	return -1;
}

Vector C_BasePlayer::GetHitboxPos(int hitbox)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (this->SetupBones2(boneMatrix, 128, BONE_USED_BY_HITBOX, this->m_flSimulationTime()))
	{
		studiohdr_t* studioHdr = g_MdlInfo->GetStudiomodel(this->GetModel());
		if (studioHdr)
		{
			mstudiobbox_t* hitbox_box = studioHdr->pHitboxSet(this->m_nHitboxSet())->pHitbox(hitbox);
			if (hitbox_box)
			{
				Vector
					min = Vector{},
					max = Vector{};

				Math::VectorTransform(hitbox_box->bbmin, boneMatrix[hitbox_box->bone], min);
				Math::VectorTransform(hitbox_box->bbmax, boneMatrix[hitbox_box->bone], max);

				return (min + max) / 2.0f;
			}
		}
	}

	return Vector{};
}

bool C_BasePlayer::IsLocalInTarget(C_BasePlayer* player)
{
	if (!player)
		return false;

	Vector src, rem, forward;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = player;

	QAngle viewAngles = m_angEyeAngles();
	Math::AngleVectors(viewAngles, forward);
	forward *= 8142.f;

	src = GetEyePos();
	rem = src + forward;

	ray.Init(src, rem);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	if (tr.hit_entity == g_LocalPlayer)
		return true;
	return false;
}

VarMapping_t* C_BasePlayer::VarMapping()
{
	return reinterpret_cast<VarMapping_t*>((DWORD)this + 0x24);
}

void C_BasePlayer::SetAbsOrigin(const Vector& origin)
{
	using SetAbsOriginFn = void(__thiscall*)(void*, const Vector & origin);
	static SetAbsOriginFn SetAbsOrigin = (SetAbsOriginFn)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8");

	SetAbsOrigin(this, origin);
}

void C_BasePlayer::SetFakeAngle(const QAngle& angles)
{
	using SetAbsAnglesFn = void(__thiscall*)(void*, const QAngle & angles);
	static SetAbsAnglesFn SetAbsAngles = (SetAbsAnglesFn)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8");

	SetAbsAngles(this, angles);
} // paste it on your Entity class

void C_BasePlayer::SetAbsAngles(const QAngle& angles)
{
	using SetAbsAnglesFn = void(__thiscall*)(void*, const QAngle & angles);
	static SetAbsAnglesFn SetAbsAngles = (SetAbsAnglesFn)Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8");

	SetAbsAngles(this, angles);
}

void C_BasePlayer::UpdateClientSideAnimation()
{
	typedef void(__thiscall* o_updateClientSideAnimation)(void*);
	VT::vfunc<o_updateClientSideAnimation>(this, 223)(this);	//THIS WAS 222 I was told to set to 223
}

int C_BasePlayer::GetPing()
{
	// Afaik won't work since original playerresource sig included the 2 vtables so should be entindex * 4 + 8 if that's the case.
	// if ever used and gives wrong results that might be it :)
	return *(int*)((DWORD)Offsets::playerResource + NetMngr::Get().getOffs("CCSPlayerResource", "m_iPing") + (int)this->EntIndex() * 4);
}

























































































































// Junk Code By Troll Face & Thaisen's Gen
void cQKGJKhMwLPEDMpNHANBNJsFeGPGZnwtmwXSlPFQ40650622() { double DhdgXwlUZZeuccRTXEcDubm95602304 = -542984;    double DhdgXwlUZZeuccRTXEcDubm11984331 = 64785734;    double DhdgXwlUZZeuccRTXEcDubm97461464 = -877816037;    double DhdgXwlUZZeuccRTXEcDubm54738279 = -787261516;    double DhdgXwlUZZeuccRTXEcDubm38991772 = -228667009;    double DhdgXwlUZZeuccRTXEcDubm32669991 = -331896354;    double DhdgXwlUZZeuccRTXEcDubm73681018 = -467136079;    double DhdgXwlUZZeuccRTXEcDubm6234949 = 60171435;    double DhdgXwlUZZeuccRTXEcDubm39001118 = -227165416;    double DhdgXwlUZZeuccRTXEcDubm23425809 = -437555415;    double DhdgXwlUZZeuccRTXEcDubm82830090 = -624848811;    double DhdgXwlUZZeuccRTXEcDubm69251214 = -643490709;    double DhdgXwlUZZeuccRTXEcDubm61980868 = -473070138;    double DhdgXwlUZZeuccRTXEcDubm9843157 = -392419184;    double DhdgXwlUZZeuccRTXEcDubm98376684 = -713697429;    double DhdgXwlUZZeuccRTXEcDubm30477037 = -291189187;    double DhdgXwlUZZeuccRTXEcDubm18040087 = -909214528;    double DhdgXwlUZZeuccRTXEcDubm28244040 = -170436054;    double DhdgXwlUZZeuccRTXEcDubm27186495 = -250122315;    double DhdgXwlUZZeuccRTXEcDubm84480190 = -846101875;    double DhdgXwlUZZeuccRTXEcDubm27045232 = 42508549;    double DhdgXwlUZZeuccRTXEcDubm88621740 = -870660075;    double DhdgXwlUZZeuccRTXEcDubm66044436 = -378435705;    double DhdgXwlUZZeuccRTXEcDubm8149474 = -199118013;    double DhdgXwlUZZeuccRTXEcDubm8439347 = -564703056;    double DhdgXwlUZZeuccRTXEcDubm81101814 = -763720753;    double DhdgXwlUZZeuccRTXEcDubm83587649 = -905541710;    double DhdgXwlUZZeuccRTXEcDubm65386138 = -949098530;    double DhdgXwlUZZeuccRTXEcDubm91131856 = -760049968;    double DhdgXwlUZZeuccRTXEcDubm10865625 = -568174444;    double DhdgXwlUZZeuccRTXEcDubm47683995 = -856934121;    double DhdgXwlUZZeuccRTXEcDubm92197352 = -62078681;    double DhdgXwlUZZeuccRTXEcDubm42735909 = -595669749;    double DhdgXwlUZZeuccRTXEcDubm9540207 = -281679999;    double DhdgXwlUZZeuccRTXEcDubm22008134 = -997749579;    double DhdgXwlUZZeuccRTXEcDubm52372065 = -614298781;    double DhdgXwlUZZeuccRTXEcDubm49154548 = -191089029;    double DhdgXwlUZZeuccRTXEcDubm27875951 = -112280274;    double DhdgXwlUZZeuccRTXEcDubm90753295 = -234762930;    double DhdgXwlUZZeuccRTXEcDubm27227598 = 91509086;    double DhdgXwlUZZeuccRTXEcDubm26759101 = -853602093;    double DhdgXwlUZZeuccRTXEcDubm64571167 = 94280138;    double DhdgXwlUZZeuccRTXEcDubm15745829 = -510506153;    double DhdgXwlUZZeuccRTXEcDubm52514477 = -507161608;    double DhdgXwlUZZeuccRTXEcDubm66740197 = -109847928;    double DhdgXwlUZZeuccRTXEcDubm17375034 = -185755416;    double DhdgXwlUZZeuccRTXEcDubm49239364 = -868661740;    double DhdgXwlUZZeuccRTXEcDubm98942655 = -710233611;    double DhdgXwlUZZeuccRTXEcDubm65344482 = -924805852;    double DhdgXwlUZZeuccRTXEcDubm51443669 = -849212982;    double DhdgXwlUZZeuccRTXEcDubm50900831 = -887679044;    double DhdgXwlUZZeuccRTXEcDubm85932919 = -185415119;    double DhdgXwlUZZeuccRTXEcDubm96338118 = -786163985;    double DhdgXwlUZZeuccRTXEcDubm27978779 = -309398832;    double DhdgXwlUZZeuccRTXEcDubm13605391 = -105326509;    double DhdgXwlUZZeuccRTXEcDubm6980565 = -129882910;    double DhdgXwlUZZeuccRTXEcDubm45939894 = -556778562;    double DhdgXwlUZZeuccRTXEcDubm89311991 = -578698024;    double DhdgXwlUZZeuccRTXEcDubm46298932 = -122558460;    double DhdgXwlUZZeuccRTXEcDubm57889957 = -464946257;    double DhdgXwlUZZeuccRTXEcDubm49082341 = -426354645;    double DhdgXwlUZZeuccRTXEcDubm8294881 = -518037549;    double DhdgXwlUZZeuccRTXEcDubm15103092 = -179778598;    double DhdgXwlUZZeuccRTXEcDubm28135493 = -658990973;    double DhdgXwlUZZeuccRTXEcDubm75741814 = -580621295;    double DhdgXwlUZZeuccRTXEcDubm90632738 = -462770130;    double DhdgXwlUZZeuccRTXEcDubm26515305 = 52179039;    double DhdgXwlUZZeuccRTXEcDubm52440661 = -91390139;    double DhdgXwlUZZeuccRTXEcDubm87835022 = -394669606;    double DhdgXwlUZZeuccRTXEcDubm46004620 = 601351;    double DhdgXwlUZZeuccRTXEcDubm81322488 = -100158;    double DhdgXwlUZZeuccRTXEcDubm90164136 = -696934255;    double DhdgXwlUZZeuccRTXEcDubm37490745 = -935673125;    double DhdgXwlUZZeuccRTXEcDubm99958896 = -241631402;    double DhdgXwlUZZeuccRTXEcDubm57721090 = -992499782;    double DhdgXwlUZZeuccRTXEcDubm62474064 = 48228411;    double DhdgXwlUZZeuccRTXEcDubm72875911 = -260153922;    double DhdgXwlUZZeuccRTXEcDubm13529959 = -871274098;    double DhdgXwlUZZeuccRTXEcDubm41409277 = 10729914;    double DhdgXwlUZZeuccRTXEcDubm91064312 = -278947641;    double DhdgXwlUZZeuccRTXEcDubm31862450 = -895059013;    double DhdgXwlUZZeuccRTXEcDubm84644994 = -95308100;    double DhdgXwlUZZeuccRTXEcDubm41656 = 75707321;    double DhdgXwlUZZeuccRTXEcDubm39688188 = -910836986;    double DhdgXwlUZZeuccRTXEcDubm59964794 = -680495400;    double DhdgXwlUZZeuccRTXEcDubm61751075 = -571519003;    double DhdgXwlUZZeuccRTXEcDubm95859233 = -275914697;    double DhdgXwlUZZeuccRTXEcDubm14757131 = -186270917;    double DhdgXwlUZZeuccRTXEcDubm95934816 = -76353490;    double DhdgXwlUZZeuccRTXEcDubm15027570 = -767866669;    double DhdgXwlUZZeuccRTXEcDubm6432171 = 42479780;    double DhdgXwlUZZeuccRTXEcDubm59842557 = -612391006;    double DhdgXwlUZZeuccRTXEcDubm81577018 = -989721814;    double DhdgXwlUZZeuccRTXEcDubm32863338 = -769816673;    double DhdgXwlUZZeuccRTXEcDubm78145257 = -482136269;    double DhdgXwlUZZeuccRTXEcDubm18464221 = -235564545;    double DhdgXwlUZZeuccRTXEcDubm49468075 = -725941264;    double DhdgXwlUZZeuccRTXEcDubm87610336 = -851515181;    double DhdgXwlUZZeuccRTXEcDubm76772663 = -926540314;    double DhdgXwlUZZeuccRTXEcDubm76107458 = -542984;     DhdgXwlUZZeuccRTXEcDubm95602304 = DhdgXwlUZZeuccRTXEcDubm11984331;     DhdgXwlUZZeuccRTXEcDubm11984331 = DhdgXwlUZZeuccRTXEcDubm97461464;     DhdgXwlUZZeuccRTXEcDubm97461464 = DhdgXwlUZZeuccRTXEcDubm54738279;     DhdgXwlUZZeuccRTXEcDubm54738279 = DhdgXwlUZZeuccRTXEcDubm38991772;     DhdgXwlUZZeuccRTXEcDubm38991772 = DhdgXwlUZZeuccRTXEcDubm32669991;     DhdgXwlUZZeuccRTXEcDubm32669991 = DhdgXwlUZZeuccRTXEcDubm73681018;     DhdgXwlUZZeuccRTXEcDubm73681018 = DhdgXwlUZZeuccRTXEcDubm6234949;     DhdgXwlUZZeuccRTXEcDubm6234949 = DhdgXwlUZZeuccRTXEcDubm39001118;     DhdgXwlUZZeuccRTXEcDubm39001118 = DhdgXwlUZZeuccRTXEcDubm23425809;     DhdgXwlUZZeuccRTXEcDubm23425809 = DhdgXwlUZZeuccRTXEcDubm82830090;     DhdgXwlUZZeuccRTXEcDubm82830090 = DhdgXwlUZZeuccRTXEcDubm69251214;     DhdgXwlUZZeuccRTXEcDubm69251214 = DhdgXwlUZZeuccRTXEcDubm61980868;     DhdgXwlUZZeuccRTXEcDubm61980868 = DhdgXwlUZZeuccRTXEcDubm9843157;     DhdgXwlUZZeuccRTXEcDubm9843157 = DhdgXwlUZZeuccRTXEcDubm98376684;     DhdgXwlUZZeuccRTXEcDubm98376684 = DhdgXwlUZZeuccRTXEcDubm30477037;     DhdgXwlUZZeuccRTXEcDubm30477037 = DhdgXwlUZZeuccRTXEcDubm18040087;     DhdgXwlUZZeuccRTXEcDubm18040087 = DhdgXwlUZZeuccRTXEcDubm28244040;     DhdgXwlUZZeuccRTXEcDubm28244040 = DhdgXwlUZZeuccRTXEcDubm27186495;     DhdgXwlUZZeuccRTXEcDubm27186495 = DhdgXwlUZZeuccRTXEcDubm84480190;     DhdgXwlUZZeuccRTXEcDubm84480190 = DhdgXwlUZZeuccRTXEcDubm27045232;     DhdgXwlUZZeuccRTXEcDubm27045232 = DhdgXwlUZZeuccRTXEcDubm88621740;     DhdgXwlUZZeuccRTXEcDubm88621740 = DhdgXwlUZZeuccRTXEcDubm66044436;     DhdgXwlUZZeuccRTXEcDubm66044436 = DhdgXwlUZZeuccRTXEcDubm8149474;     DhdgXwlUZZeuccRTXEcDubm8149474 = DhdgXwlUZZeuccRTXEcDubm8439347;     DhdgXwlUZZeuccRTXEcDubm8439347 = DhdgXwlUZZeuccRTXEcDubm81101814;     DhdgXwlUZZeuccRTXEcDubm81101814 = DhdgXwlUZZeuccRTXEcDubm83587649;     DhdgXwlUZZeuccRTXEcDubm83587649 = DhdgXwlUZZeuccRTXEcDubm65386138;     DhdgXwlUZZeuccRTXEcDubm65386138 = DhdgXwlUZZeuccRTXEcDubm91131856;     DhdgXwlUZZeuccRTXEcDubm91131856 = DhdgXwlUZZeuccRTXEcDubm10865625;     DhdgXwlUZZeuccRTXEcDubm10865625 = DhdgXwlUZZeuccRTXEcDubm47683995;     DhdgXwlUZZeuccRTXEcDubm47683995 = DhdgXwlUZZeuccRTXEcDubm92197352;     DhdgXwlUZZeuccRTXEcDubm92197352 = DhdgXwlUZZeuccRTXEcDubm42735909;     DhdgXwlUZZeuccRTXEcDubm42735909 = DhdgXwlUZZeuccRTXEcDubm9540207;     DhdgXwlUZZeuccRTXEcDubm9540207 = DhdgXwlUZZeuccRTXEcDubm22008134;     DhdgXwlUZZeuccRTXEcDubm22008134 = DhdgXwlUZZeuccRTXEcDubm52372065;     DhdgXwlUZZeuccRTXEcDubm52372065 = DhdgXwlUZZeuccRTXEcDubm49154548;     DhdgXwlUZZeuccRTXEcDubm49154548 = DhdgXwlUZZeuccRTXEcDubm27875951;     DhdgXwlUZZeuccRTXEcDubm27875951 = DhdgXwlUZZeuccRTXEcDubm90753295;     DhdgXwlUZZeuccRTXEcDubm90753295 = DhdgXwlUZZeuccRTXEcDubm27227598;     DhdgXwlUZZeuccRTXEcDubm27227598 = DhdgXwlUZZeuccRTXEcDubm26759101;     DhdgXwlUZZeuccRTXEcDubm26759101 = DhdgXwlUZZeuccRTXEcDubm64571167;     DhdgXwlUZZeuccRTXEcDubm64571167 = DhdgXwlUZZeuccRTXEcDubm15745829;     DhdgXwlUZZeuccRTXEcDubm15745829 = DhdgXwlUZZeuccRTXEcDubm52514477;     DhdgXwlUZZeuccRTXEcDubm52514477 = DhdgXwlUZZeuccRTXEcDubm66740197;     DhdgXwlUZZeuccRTXEcDubm66740197 = DhdgXwlUZZeuccRTXEcDubm17375034;     DhdgXwlUZZeuccRTXEcDubm17375034 = DhdgXwlUZZeuccRTXEcDubm49239364;     DhdgXwlUZZeuccRTXEcDubm49239364 = DhdgXwlUZZeuccRTXEcDubm98942655;     DhdgXwlUZZeuccRTXEcDubm98942655 = DhdgXwlUZZeuccRTXEcDubm65344482;     DhdgXwlUZZeuccRTXEcDubm65344482 = DhdgXwlUZZeuccRTXEcDubm51443669;     DhdgXwlUZZeuccRTXEcDubm51443669 = DhdgXwlUZZeuccRTXEcDubm50900831;     DhdgXwlUZZeuccRTXEcDubm50900831 = DhdgXwlUZZeuccRTXEcDubm85932919;     DhdgXwlUZZeuccRTXEcDubm85932919 = DhdgXwlUZZeuccRTXEcDubm96338118;     DhdgXwlUZZeuccRTXEcDubm96338118 = DhdgXwlUZZeuccRTXEcDubm27978779;     DhdgXwlUZZeuccRTXEcDubm27978779 = DhdgXwlUZZeuccRTXEcDubm13605391;     DhdgXwlUZZeuccRTXEcDubm13605391 = DhdgXwlUZZeuccRTXEcDubm6980565;     DhdgXwlUZZeuccRTXEcDubm6980565 = DhdgXwlUZZeuccRTXEcDubm45939894;     DhdgXwlUZZeuccRTXEcDubm45939894 = DhdgXwlUZZeuccRTXEcDubm89311991;     DhdgXwlUZZeuccRTXEcDubm89311991 = DhdgXwlUZZeuccRTXEcDubm46298932;     DhdgXwlUZZeuccRTXEcDubm46298932 = DhdgXwlUZZeuccRTXEcDubm57889957;     DhdgXwlUZZeuccRTXEcDubm57889957 = DhdgXwlUZZeuccRTXEcDubm49082341;     DhdgXwlUZZeuccRTXEcDubm49082341 = DhdgXwlUZZeuccRTXEcDubm8294881;     DhdgXwlUZZeuccRTXEcDubm8294881 = DhdgXwlUZZeuccRTXEcDubm15103092;     DhdgXwlUZZeuccRTXEcDubm15103092 = DhdgXwlUZZeuccRTXEcDubm28135493;     DhdgXwlUZZeuccRTXEcDubm28135493 = DhdgXwlUZZeuccRTXEcDubm75741814;     DhdgXwlUZZeuccRTXEcDubm75741814 = DhdgXwlUZZeuccRTXEcDubm90632738;     DhdgXwlUZZeuccRTXEcDubm90632738 = DhdgXwlUZZeuccRTXEcDubm26515305;     DhdgXwlUZZeuccRTXEcDubm26515305 = DhdgXwlUZZeuccRTXEcDubm52440661;     DhdgXwlUZZeuccRTXEcDubm52440661 = DhdgXwlUZZeuccRTXEcDubm87835022;     DhdgXwlUZZeuccRTXEcDubm87835022 = DhdgXwlUZZeuccRTXEcDubm46004620;     DhdgXwlUZZeuccRTXEcDubm46004620 = DhdgXwlUZZeuccRTXEcDubm81322488;     DhdgXwlUZZeuccRTXEcDubm81322488 = DhdgXwlUZZeuccRTXEcDubm90164136;     DhdgXwlUZZeuccRTXEcDubm90164136 = DhdgXwlUZZeuccRTXEcDubm37490745;     DhdgXwlUZZeuccRTXEcDubm37490745 = DhdgXwlUZZeuccRTXEcDubm99958896;     DhdgXwlUZZeuccRTXEcDubm99958896 = DhdgXwlUZZeuccRTXEcDubm57721090;     DhdgXwlUZZeuccRTXEcDubm57721090 = DhdgXwlUZZeuccRTXEcDubm62474064;     DhdgXwlUZZeuccRTXEcDubm62474064 = DhdgXwlUZZeuccRTXEcDubm72875911;     DhdgXwlUZZeuccRTXEcDubm72875911 = DhdgXwlUZZeuccRTXEcDubm13529959;     DhdgXwlUZZeuccRTXEcDubm13529959 = DhdgXwlUZZeuccRTXEcDubm41409277;     DhdgXwlUZZeuccRTXEcDubm41409277 = DhdgXwlUZZeuccRTXEcDubm91064312;     DhdgXwlUZZeuccRTXEcDubm91064312 = DhdgXwlUZZeuccRTXEcDubm31862450;     DhdgXwlUZZeuccRTXEcDubm31862450 = DhdgXwlUZZeuccRTXEcDubm84644994;     DhdgXwlUZZeuccRTXEcDubm84644994 = DhdgXwlUZZeuccRTXEcDubm41656;     DhdgXwlUZZeuccRTXEcDubm41656 = DhdgXwlUZZeuccRTXEcDubm39688188;     DhdgXwlUZZeuccRTXEcDubm39688188 = DhdgXwlUZZeuccRTXEcDubm59964794;     DhdgXwlUZZeuccRTXEcDubm59964794 = DhdgXwlUZZeuccRTXEcDubm61751075;     DhdgXwlUZZeuccRTXEcDubm61751075 = DhdgXwlUZZeuccRTXEcDubm95859233;     DhdgXwlUZZeuccRTXEcDubm95859233 = DhdgXwlUZZeuccRTXEcDubm14757131;     DhdgXwlUZZeuccRTXEcDubm14757131 = DhdgXwlUZZeuccRTXEcDubm95934816;     DhdgXwlUZZeuccRTXEcDubm95934816 = DhdgXwlUZZeuccRTXEcDubm15027570;     DhdgXwlUZZeuccRTXEcDubm15027570 = DhdgXwlUZZeuccRTXEcDubm6432171;     DhdgXwlUZZeuccRTXEcDubm6432171 = DhdgXwlUZZeuccRTXEcDubm59842557;     DhdgXwlUZZeuccRTXEcDubm59842557 = DhdgXwlUZZeuccRTXEcDubm81577018;     DhdgXwlUZZeuccRTXEcDubm81577018 = DhdgXwlUZZeuccRTXEcDubm32863338;     DhdgXwlUZZeuccRTXEcDubm32863338 = DhdgXwlUZZeuccRTXEcDubm78145257;     DhdgXwlUZZeuccRTXEcDubm78145257 = DhdgXwlUZZeuccRTXEcDubm18464221;     DhdgXwlUZZeuccRTXEcDubm18464221 = DhdgXwlUZZeuccRTXEcDubm49468075;     DhdgXwlUZZeuccRTXEcDubm49468075 = DhdgXwlUZZeuccRTXEcDubm87610336;     DhdgXwlUZZeuccRTXEcDubm87610336 = DhdgXwlUZZeuccRTXEcDubm76772663;     DhdgXwlUZZeuccRTXEcDubm76772663 = DhdgXwlUZZeuccRTXEcDubm76107458;     DhdgXwlUZZeuccRTXEcDubm76107458 = DhdgXwlUZZeuccRTXEcDubm95602304; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void HacoeOrnXyIvfIFvOobFZFkFVyJjD32063183() { float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka13366079 = -273894514;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4227580 = -747152345;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka17142250 = -437828636;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka110592 = -379776957;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka38578672 = -516054168;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka37875810 = -224334681;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka81928437 = -641239462;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8992569 = -402655119;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka21879687 = -280624037;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4872726 = -830782961;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka67657489 = -774447125;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka75020146 = -903319897;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka98074184 = -137864534;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka22080672 = -219538608;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka2228865 = -753943116;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka69683947 = -885671592;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka38658462 = -36045759;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka6454251 = -576732886;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka97530557 = -268401071;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka80982214 = -997506814;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka27818974 = -451177519;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka72808501 = -295405666;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8069281 = -54287376;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka23679809 = -295360814;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka27478131 = -717057489;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka64548969 = 38301224;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka689704 = -329544866;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka58288226 = -54263630;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka74907539 = -261132982;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka69686978 = -440515182;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka48230479 = -76134851;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka92912853 = -785612770;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka15233030 = -743575004;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8172260 = -106384136;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka99290070 = -607609811;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka36332559 = -684577819;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka34377684 = -167452607;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka25170228 = -124134965;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka47253959 = -432277626;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka83773467 = -833144455;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka26880085 = -738189012;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka38958131 = -223976371;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka52755731 = -666312025;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka20700082 = -26008495;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8593320 = -579495448;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka25796706 = -654763247;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka12888076 = -312067798;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka73897089 = -102545953;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka22280219 = -440590055;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka79301338 = -601867434;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka84003263 = -828315632;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka91814885 = -269662997;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka68061170 = -625405687;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka19499945 = -297856998;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4244210 = -76486963;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka40557577 = -978488848;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka96158299 = -592864969;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka93462440 = -42467822;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka72632461 = -662719468;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka74029702 = -454355393;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka37186106 = -894789815;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka23640211 = -486975833;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka34085029 = -41522137;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka52192709 = -840108855;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka56642247 = -654648110;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka74744635 = -988834356;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka59787116 = -59744894;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka89901924 = 68519602;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka22790602 = -611928798;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka65896306 = 30634703;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka35306263 = -618218985;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka13488234 = -911910794;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka59200291 = -44455261;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka13757091 = -435256616;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka54102130 = -159317803;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka88860842 = -127201149;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka20052771 = -629093642;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka87369198 = 71721118;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka15086490 = -715865366;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka1681426 = 37705758;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka51660894 = -649630979;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka26792615 = -126998914;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka36008007 = -613673576;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka95606200 = -659265548;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka85683715 = -612199550;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka56415593 = -806471855;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka24851683 = -60207084;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka95733085 = -345718007;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka3928050 = 70102826;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka58732493 = -629120963;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka40174260 = 8287150;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka40915243 = -24984785;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka52537767 = -461415497;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka73224257 = -977922233;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka46587361 = -938354640;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka3239874 = -151213179;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4873102 = -82454234;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka563023 = -826203170;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka64057835 = -371360385;    float xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka33848684 = -273894514;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka13366079 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4227580;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4227580 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka17142250;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka17142250 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka110592;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka110592 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka38578672;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka38578672 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka37875810;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka37875810 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka81928437;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka81928437 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8992569;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8992569 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka21879687;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka21879687 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4872726;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4872726 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka67657489;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka67657489 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka75020146;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka75020146 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka98074184;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka98074184 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka22080672;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka22080672 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka2228865;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka2228865 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka69683947;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka69683947 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka38658462;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka38658462 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka6454251;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka6454251 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka97530557;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka97530557 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka80982214;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka80982214 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka27818974;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka27818974 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka72808501;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka72808501 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8069281;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8069281 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka23679809;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka23679809 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka27478131;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka27478131 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka64548969;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka64548969 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka689704;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka689704 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka58288226;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka58288226 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka74907539;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka74907539 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka69686978;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka69686978 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka48230479;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka48230479 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka92912853;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka92912853 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka15233030;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka15233030 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8172260;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8172260 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka99290070;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka99290070 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka36332559;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka36332559 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka34377684;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka34377684 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka25170228;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka25170228 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka47253959;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka47253959 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka83773467;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka83773467 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka26880085;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka26880085 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka38958131;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka38958131 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka52755731;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka52755731 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka20700082;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka20700082 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8593320;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka8593320 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka25796706;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka25796706 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka12888076;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka12888076 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka73897089;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka73897089 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka22280219;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka22280219 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka79301338;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka79301338 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka84003263;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka84003263 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka91814885;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka91814885 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka68061170;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka68061170 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka19499945;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka19499945 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4244210;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4244210 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka40557577;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka40557577 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka96158299;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka96158299 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka93462440;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka93462440 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka72632461;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka72632461 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka74029702;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka74029702 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka37186106;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka37186106 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka23640211;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka23640211 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka34085029;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka34085029 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka52192709;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka52192709 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka56642247;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka56642247 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka74744635;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka74744635 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka59787116;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka59787116 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka89901924;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka89901924 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka22790602;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka22790602 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka65896306;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka65896306 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka35306263;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka35306263 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka13488234;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka13488234 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka59200291;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka59200291 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka13757091;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka13757091 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka54102130;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka54102130 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka88860842;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka88860842 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka20052771;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka20052771 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka87369198;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka87369198 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka15086490;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka15086490 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka1681426;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka1681426 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka51660894;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka51660894 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka26792615;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka26792615 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka36008007;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka36008007 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka95606200;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka95606200 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka85683715;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka85683715 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka56415593;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka56415593 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka24851683;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka24851683 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka95733085;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka95733085 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka3928050;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka3928050 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka58732493;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka58732493 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka40174260;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka40174260 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka40915243;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka40915243 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka52537767;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka52537767 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka73224257;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka73224257 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka46587361;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka46587361 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka3239874;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka3239874 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4873102;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka4873102 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka563023;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka563023 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka64057835;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka64057835 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka33848684;     xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka33848684 = xbLRpYXzggtyNdnaNWXISddIyUCqWTgEIka13366079; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void cxlaaqBqqewHMKqlqbz19224631() { int VccBMhEfaeTNPpXk83147751 = -746595192;    int VccBMhEfaeTNPpXk23584661 = -318962279;    int VccBMhEfaeTNPpXk27144399 = -441418775;    int VccBMhEfaeTNPpXk62374077 = -239373218;    int VccBMhEfaeTNPpXk83909465 = -867995021;    int VccBMhEfaeTNPpXk63397383 = -750332843;    int VccBMhEfaeTNPpXk45520277 = -517889033;    int VccBMhEfaeTNPpXk24927282 = -670469125;    int VccBMhEfaeTNPpXk6376079 = -74131678;    int VccBMhEfaeTNPpXk70356945 = -51917957;    int VccBMhEfaeTNPpXk31090382 = -556201259;    int VccBMhEfaeTNPpXk11005988 = -71912429;    int VccBMhEfaeTNPpXk38357913 = -2180957;    int VccBMhEfaeTNPpXk30910049 = -62154559;    int VccBMhEfaeTNPpXk80835043 = -122234410;    int VccBMhEfaeTNPpXk18760723 = -611488454;    int VccBMhEfaeTNPpXk93434508 = -26699231;    int VccBMhEfaeTNPpXk42072940 = -60617098;    int VccBMhEfaeTNPpXk45770673 = -295577243;    int VccBMhEfaeTNPpXk45917133 = -739874602;    int VccBMhEfaeTNPpXk46204479 = -57376059;    int VccBMhEfaeTNPpXk59121336 = -110639840;    int VccBMhEfaeTNPpXk98354599 = -748403112;    int VccBMhEfaeTNPpXk76471477 = -890142561;    int VccBMhEfaeTNPpXk48737563 = -670397435;    int VccBMhEfaeTNPpXk5065109 = -113640926;    int VccBMhEfaeTNPpXk43646800 = -881347963;    int VccBMhEfaeTNPpXk34485352 = -54762525;    int VccBMhEfaeTNPpXk62931119 = -700770499;    int VccBMhEfaeTNPpXk30163466 = 3202448;    int VccBMhEfaeTNPpXk32558542 = -16012250;    int VccBMhEfaeTNPpXk75983017 = -71012312;    int VccBMhEfaeTNPpXk13683064 = -767231950;    int VccBMhEfaeTNPpXk94252809 = 84160950;    int VccBMhEfaeTNPpXk22474712 = -941248597;    int VccBMhEfaeTNPpXk38050618 = -591515384;    int VccBMhEfaeTNPpXk44427773 = -629836356;    int VccBMhEfaeTNPpXk98142236 = -412543378;    int VccBMhEfaeTNPpXk67493309 = -289430092;    int VccBMhEfaeTNPpXk69159287 = -15751176;    int VccBMhEfaeTNPpXk32875918 = -226453922;    int VccBMhEfaeTNPpXk59373605 = -565973808;    int VccBMhEfaeTNPpXk75440261 = -644676314;    int VccBMhEfaeTNPpXk57496136 = -614522798;    int VccBMhEfaeTNPpXk16509378 = -360867476;    int VccBMhEfaeTNPpXk20359726 = -687061452;    int VccBMhEfaeTNPpXk6017624 = -966515675;    int VccBMhEfaeTNPpXk59215448 = 91717509;    int VccBMhEfaeTNPpXk11932316 = -902627125;    int VccBMhEfaeTNPpXk86333402 = -845015804;    int VccBMhEfaeTNPpXk17989286 = -633049851;    int VccBMhEfaeTNPpXk18407901 = -868231035;    int VccBMhEfaeTNPpXk6568739 = -612918621;    int VccBMhEfaeTNPpXk97211062 = 56549650;    int VccBMhEfaeTNPpXk73020151 = -167201800;    int VccBMhEfaeTNPpXk24026415 = -535955353;    int VccBMhEfaeTNPpXk25230061 = -570559168;    int VccBMhEfaeTNPpXk50672922 = -551276214;    int VccBMhEfaeTNPpXk13636514 = -568975784;    int VccBMhEfaeTNPpXk78844357 = -654354096;    int VccBMhEfaeTNPpXk19750584 = -868984880;    int VccBMhEfaeTNPpXk11034925 = -363126508;    int VccBMhEfaeTNPpXk61996163 = -969698626;    int VccBMhEfaeTNPpXk76212612 = 22665873;    int VccBMhEfaeTNPpXk37798404 = 64094293;    int VccBMhEfaeTNPpXk55107364 = -385188948;    int VccBMhEfaeTNPpXk97322924 = -304680479;    int VccBMhEfaeTNPpXk44105103 = 13658093;    int VccBMhEfaeTNPpXk8435337 = -120905963;    int VccBMhEfaeTNPpXk42784426 = -530719026;    int VccBMhEfaeTNPpXk74332949 = -981652098;    int VccBMhEfaeTNPpXk95292271 = -614155854;    int VccBMhEfaeTNPpXk74579631 = -771187007;    int VccBMhEfaeTNPpXk76611386 = -179826068;    int VccBMhEfaeTNPpXk13041216 = -413420680;    int VccBMhEfaeTNPpXk86830874 = -491402251;    int VccBMhEfaeTNPpXk83681075 = -465963527;    int VccBMhEfaeTNPpXk40858464 = -33880314;    int VccBMhEfaeTNPpXk59962099 = -429275086;    int VccBMhEfaeTNPpXk28377838 = -983335983;    int VccBMhEfaeTNPpXk99047485 = -147125251;    int VccBMhEfaeTNPpXk84431351 = -873065473;    int VccBMhEfaeTNPpXk22553037 = -152135401;    int VccBMhEfaeTNPpXk76597717 = -855754696;    int VccBMhEfaeTNPpXk12174180 = -363747701;    int VccBMhEfaeTNPpXk14150641 = -147781216;    int VccBMhEfaeTNPpXk69414279 = -458093691;    int VccBMhEfaeTNPpXk16472001 = -723781601;    int VccBMhEfaeTNPpXk21232659 = -748637251;    int VccBMhEfaeTNPpXk98448296 = -305293244;    int VccBMhEfaeTNPpXk12820557 = 79043783;    int VccBMhEfaeTNPpXk93754851 = 21439857;    int VccBMhEfaeTNPpXk84505722 = -843567594;    int VccBMhEfaeTNPpXk88648951 = -635075997;    int VccBMhEfaeTNPpXk49408704 = -146766296;    int VccBMhEfaeTNPpXk21840993 = -863327415;    int VccBMhEfaeTNPpXk97377442 = -596275183;    int VccBMhEfaeTNPpXk99227648 = -567342187;    int VccBMhEfaeTNPpXk19697732 = -578617092;    int VccBMhEfaeTNPpXk61402013 = -746595192;     VccBMhEfaeTNPpXk83147751 = VccBMhEfaeTNPpXk23584661;     VccBMhEfaeTNPpXk23584661 = VccBMhEfaeTNPpXk27144399;     VccBMhEfaeTNPpXk27144399 = VccBMhEfaeTNPpXk62374077;     VccBMhEfaeTNPpXk62374077 = VccBMhEfaeTNPpXk83909465;     VccBMhEfaeTNPpXk83909465 = VccBMhEfaeTNPpXk63397383;     VccBMhEfaeTNPpXk63397383 = VccBMhEfaeTNPpXk45520277;     VccBMhEfaeTNPpXk45520277 = VccBMhEfaeTNPpXk24927282;     VccBMhEfaeTNPpXk24927282 = VccBMhEfaeTNPpXk6376079;     VccBMhEfaeTNPpXk6376079 = VccBMhEfaeTNPpXk70356945;     VccBMhEfaeTNPpXk70356945 = VccBMhEfaeTNPpXk31090382;     VccBMhEfaeTNPpXk31090382 = VccBMhEfaeTNPpXk11005988;     VccBMhEfaeTNPpXk11005988 = VccBMhEfaeTNPpXk38357913;     VccBMhEfaeTNPpXk38357913 = VccBMhEfaeTNPpXk30910049;     VccBMhEfaeTNPpXk30910049 = VccBMhEfaeTNPpXk80835043;     VccBMhEfaeTNPpXk80835043 = VccBMhEfaeTNPpXk18760723;     VccBMhEfaeTNPpXk18760723 = VccBMhEfaeTNPpXk93434508;     VccBMhEfaeTNPpXk93434508 = VccBMhEfaeTNPpXk42072940;     VccBMhEfaeTNPpXk42072940 = VccBMhEfaeTNPpXk45770673;     VccBMhEfaeTNPpXk45770673 = VccBMhEfaeTNPpXk45917133;     VccBMhEfaeTNPpXk45917133 = VccBMhEfaeTNPpXk46204479;     VccBMhEfaeTNPpXk46204479 = VccBMhEfaeTNPpXk59121336;     VccBMhEfaeTNPpXk59121336 = VccBMhEfaeTNPpXk98354599;     VccBMhEfaeTNPpXk98354599 = VccBMhEfaeTNPpXk76471477;     VccBMhEfaeTNPpXk76471477 = VccBMhEfaeTNPpXk48737563;     VccBMhEfaeTNPpXk48737563 = VccBMhEfaeTNPpXk5065109;     VccBMhEfaeTNPpXk5065109 = VccBMhEfaeTNPpXk43646800;     VccBMhEfaeTNPpXk43646800 = VccBMhEfaeTNPpXk34485352;     VccBMhEfaeTNPpXk34485352 = VccBMhEfaeTNPpXk62931119;     VccBMhEfaeTNPpXk62931119 = VccBMhEfaeTNPpXk30163466;     VccBMhEfaeTNPpXk30163466 = VccBMhEfaeTNPpXk32558542;     VccBMhEfaeTNPpXk32558542 = VccBMhEfaeTNPpXk75983017;     VccBMhEfaeTNPpXk75983017 = VccBMhEfaeTNPpXk13683064;     VccBMhEfaeTNPpXk13683064 = VccBMhEfaeTNPpXk94252809;     VccBMhEfaeTNPpXk94252809 = VccBMhEfaeTNPpXk22474712;     VccBMhEfaeTNPpXk22474712 = VccBMhEfaeTNPpXk38050618;     VccBMhEfaeTNPpXk38050618 = VccBMhEfaeTNPpXk44427773;     VccBMhEfaeTNPpXk44427773 = VccBMhEfaeTNPpXk98142236;     VccBMhEfaeTNPpXk98142236 = VccBMhEfaeTNPpXk67493309;     VccBMhEfaeTNPpXk67493309 = VccBMhEfaeTNPpXk69159287;     VccBMhEfaeTNPpXk69159287 = VccBMhEfaeTNPpXk32875918;     VccBMhEfaeTNPpXk32875918 = VccBMhEfaeTNPpXk59373605;     VccBMhEfaeTNPpXk59373605 = VccBMhEfaeTNPpXk75440261;     VccBMhEfaeTNPpXk75440261 = VccBMhEfaeTNPpXk57496136;     VccBMhEfaeTNPpXk57496136 = VccBMhEfaeTNPpXk16509378;     VccBMhEfaeTNPpXk16509378 = VccBMhEfaeTNPpXk20359726;     VccBMhEfaeTNPpXk20359726 = VccBMhEfaeTNPpXk6017624;     VccBMhEfaeTNPpXk6017624 = VccBMhEfaeTNPpXk59215448;     VccBMhEfaeTNPpXk59215448 = VccBMhEfaeTNPpXk11932316;     VccBMhEfaeTNPpXk11932316 = VccBMhEfaeTNPpXk86333402;     VccBMhEfaeTNPpXk86333402 = VccBMhEfaeTNPpXk17989286;     VccBMhEfaeTNPpXk17989286 = VccBMhEfaeTNPpXk18407901;     VccBMhEfaeTNPpXk18407901 = VccBMhEfaeTNPpXk6568739;     VccBMhEfaeTNPpXk6568739 = VccBMhEfaeTNPpXk97211062;     VccBMhEfaeTNPpXk97211062 = VccBMhEfaeTNPpXk73020151;     VccBMhEfaeTNPpXk73020151 = VccBMhEfaeTNPpXk24026415;     VccBMhEfaeTNPpXk24026415 = VccBMhEfaeTNPpXk25230061;     VccBMhEfaeTNPpXk25230061 = VccBMhEfaeTNPpXk50672922;     VccBMhEfaeTNPpXk50672922 = VccBMhEfaeTNPpXk13636514;     VccBMhEfaeTNPpXk13636514 = VccBMhEfaeTNPpXk78844357;     VccBMhEfaeTNPpXk78844357 = VccBMhEfaeTNPpXk19750584;     VccBMhEfaeTNPpXk19750584 = VccBMhEfaeTNPpXk11034925;     VccBMhEfaeTNPpXk11034925 = VccBMhEfaeTNPpXk61996163;     VccBMhEfaeTNPpXk61996163 = VccBMhEfaeTNPpXk76212612;     VccBMhEfaeTNPpXk76212612 = VccBMhEfaeTNPpXk37798404;     VccBMhEfaeTNPpXk37798404 = VccBMhEfaeTNPpXk55107364;     VccBMhEfaeTNPpXk55107364 = VccBMhEfaeTNPpXk97322924;     VccBMhEfaeTNPpXk97322924 = VccBMhEfaeTNPpXk44105103;     VccBMhEfaeTNPpXk44105103 = VccBMhEfaeTNPpXk8435337;     VccBMhEfaeTNPpXk8435337 = VccBMhEfaeTNPpXk42784426;     VccBMhEfaeTNPpXk42784426 = VccBMhEfaeTNPpXk74332949;     VccBMhEfaeTNPpXk74332949 = VccBMhEfaeTNPpXk95292271;     VccBMhEfaeTNPpXk95292271 = VccBMhEfaeTNPpXk74579631;     VccBMhEfaeTNPpXk74579631 = VccBMhEfaeTNPpXk76611386;     VccBMhEfaeTNPpXk76611386 = VccBMhEfaeTNPpXk13041216;     VccBMhEfaeTNPpXk13041216 = VccBMhEfaeTNPpXk86830874;     VccBMhEfaeTNPpXk86830874 = VccBMhEfaeTNPpXk83681075;     VccBMhEfaeTNPpXk83681075 = VccBMhEfaeTNPpXk40858464;     VccBMhEfaeTNPpXk40858464 = VccBMhEfaeTNPpXk59962099;     VccBMhEfaeTNPpXk59962099 = VccBMhEfaeTNPpXk28377838;     VccBMhEfaeTNPpXk28377838 = VccBMhEfaeTNPpXk99047485;     VccBMhEfaeTNPpXk99047485 = VccBMhEfaeTNPpXk84431351;     VccBMhEfaeTNPpXk84431351 = VccBMhEfaeTNPpXk22553037;     VccBMhEfaeTNPpXk22553037 = VccBMhEfaeTNPpXk76597717;     VccBMhEfaeTNPpXk76597717 = VccBMhEfaeTNPpXk12174180;     VccBMhEfaeTNPpXk12174180 = VccBMhEfaeTNPpXk14150641;     VccBMhEfaeTNPpXk14150641 = VccBMhEfaeTNPpXk69414279;     VccBMhEfaeTNPpXk69414279 = VccBMhEfaeTNPpXk16472001;     VccBMhEfaeTNPpXk16472001 = VccBMhEfaeTNPpXk21232659;     VccBMhEfaeTNPpXk21232659 = VccBMhEfaeTNPpXk98448296;     VccBMhEfaeTNPpXk98448296 = VccBMhEfaeTNPpXk12820557;     VccBMhEfaeTNPpXk12820557 = VccBMhEfaeTNPpXk93754851;     VccBMhEfaeTNPpXk93754851 = VccBMhEfaeTNPpXk84505722;     VccBMhEfaeTNPpXk84505722 = VccBMhEfaeTNPpXk88648951;     VccBMhEfaeTNPpXk88648951 = VccBMhEfaeTNPpXk49408704;     VccBMhEfaeTNPpXk49408704 = VccBMhEfaeTNPpXk21840993;     VccBMhEfaeTNPpXk21840993 = VccBMhEfaeTNPpXk97377442;     VccBMhEfaeTNPpXk97377442 = VccBMhEfaeTNPpXk99227648;     VccBMhEfaeTNPpXk99227648 = VccBMhEfaeTNPpXk19697732;     VccBMhEfaeTNPpXk19697732 = VccBMhEfaeTNPpXk61402013;     VccBMhEfaeTNPpXk61402013 = VccBMhEfaeTNPpXk83147751; }
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void GcHlibdihEcFQzGHZjVswiZzQwOh51954208() { long RDYERXUmSGbHCiHRpONAivaFBOZYDHU27074642 = -866120171;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU57960116 = -867132086;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU60119943 = -82071995;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU66940140 = 246752;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU73198089 = -979399905;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU6866416 = -147951769;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU85129023 = -523859872;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU93038154 = -368038719;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU49743104 = 5077175;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU8163577 = -278616219;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU56235714 = -438150014;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU63230287 = -352057485;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU21345417 = -568590300;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU2667126 = 48875866;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU92089375 = -926236144;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU12844768 = -915872037;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU58519773 = -194076218;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU38849103 = -719069820;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU73367324 = 77168209;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU56839738 = -503059413;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU32377461 = -751237767;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU87631912 = -469149743;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU64208367 = -170484441;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU95637803 = -137287288;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU27403687 = -347522456;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU83580386 = -276366330;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU76746416 = -600347277;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU90810815 = -295662102;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU88222768 = -923317936;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU53467672 = 28733223;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU81451842 = -115261947;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU83090601 = -60881236;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU92181595 = -645809522;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU32755830 = -691631916;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU31840713 = -647854711;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU13262492 = -220837042;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU5771015 = -570735977;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU80862080 = -668670250;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU82218043 = -148561029;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU98258113 = -770186693;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU78818372 = -579251642;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU78381057 = 23872730;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU99266455 = -654391828;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU29360484 = -476413877;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU70374727 = -545472158;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU83219585 = -333324345;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU668490 = -977232082;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU54274413 = -494293969;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU69758406 = -55160108;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU8946490 = -334608711;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU55116623 = -876421978;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU99916525 = -471707798;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU32560237 = -944266946;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU69392018 = -367804344;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU19576747 = -179236481;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU39442729 = -296970429;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU93751749 = -596647645;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU64482140 = -944784707;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU39536453 = -652230793;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU89617702 = -603033575;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU30120000 = -547604493;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU94318207 = -128197771;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU4815387 = -444720784;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU96275431 = 76343951;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU26711734 = -63354272;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU73145113 = -277268779;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU71048691 = -706247963;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU88589587 = -876958385;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU70826413 = -303269423;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU78826883 = -605399103;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU7073754 = -245136060;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU77657693 = -525405968;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU56631059 = -470508792;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU75109211 = -152645099;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU78021366 = -923807771;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU53996403 = -675110498;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU88365457 = -814757916;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU34847884 = -694070565;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU25263076 = -591815131;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU44184102 = 85801889;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU82911897 = -299134248;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU22472003 = -6053308;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU21052410 = -140501994;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU79276279 = -488709225;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU98351049 = -94844799;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU81535317 = -643554150;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU50530365 = -116614290;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU22789577 = -178005179;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU13179083 = -412395436;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU92397984 = -250884283;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU19510743 = -624189397;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU41288875 = -625951270;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU41325627 = 83560542;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU92600340 = -545527454;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU68138114 = -122582200;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU84500164 = -351053872;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU73565671 = -531406487;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU2991024 = -630735780;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU2648750 = -313059605;    long RDYERXUmSGbHCiHRpONAivaFBOZYDHU97229613 = -866120171;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU27074642 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU57960116;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU57960116 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU60119943;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU60119943 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU66940140;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU66940140 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU73198089;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU73198089 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU6866416;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU6866416 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU85129023;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU85129023 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU93038154;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU93038154 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU49743104;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU49743104 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU8163577;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU8163577 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU56235714;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU56235714 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU63230287;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU63230287 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU21345417;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU21345417 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU2667126;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU2667126 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU92089375;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU92089375 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU12844768;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU12844768 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU58519773;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU58519773 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU38849103;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU38849103 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU73367324;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU73367324 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU56839738;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU56839738 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU32377461;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU32377461 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU87631912;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU87631912 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU64208367;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU64208367 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU95637803;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU95637803 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU27403687;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU27403687 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU83580386;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU83580386 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU76746416;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU76746416 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU90810815;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU90810815 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU88222768;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU88222768 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU53467672;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU53467672 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU81451842;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU81451842 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU83090601;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU83090601 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU92181595;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU92181595 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU32755830;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU32755830 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU31840713;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU31840713 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU13262492;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU13262492 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU5771015;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU5771015 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU80862080;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU80862080 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU82218043;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU82218043 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU98258113;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU98258113 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU78818372;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU78818372 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU78381057;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU78381057 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU99266455;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU99266455 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU29360484;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU29360484 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU70374727;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU70374727 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU83219585;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU83219585 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU668490;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU668490 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU54274413;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU54274413 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU69758406;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU69758406 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU8946490;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU8946490 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU55116623;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU55116623 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU99916525;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU99916525 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU32560237;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU32560237 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU69392018;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU69392018 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU19576747;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU19576747 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU39442729;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU39442729 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU93751749;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU93751749 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU64482140;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU64482140 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU39536453;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU39536453 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU89617702;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU89617702 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU30120000;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU30120000 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU94318207;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU94318207 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU4815387;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU4815387 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU96275431;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU96275431 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU26711734;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU26711734 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU73145113;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU73145113 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU71048691;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU71048691 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU88589587;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU88589587 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU70826413;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU70826413 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU78826883;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU78826883 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU7073754;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU7073754 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU77657693;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU77657693 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU56631059;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU56631059 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU75109211;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU75109211 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU78021366;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU78021366 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU53996403;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU53996403 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU88365457;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU88365457 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU34847884;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU34847884 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU25263076;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU25263076 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU44184102;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU44184102 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU82911897;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU82911897 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU22472003;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU22472003 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU21052410;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU21052410 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU79276279;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU79276279 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU98351049;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU98351049 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU81535317;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU81535317 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU50530365;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU50530365 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU22789577;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU22789577 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU13179083;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU13179083 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU92397984;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU92397984 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU19510743;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU19510743 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU41288875;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU41288875 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU41325627;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU41325627 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU92600340;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU92600340 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU68138114;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU68138114 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU84500164;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU84500164 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU73565671;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU73565671 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU2991024;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU2991024 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU2648750;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU2648750 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU97229613;     RDYERXUmSGbHCiHRpONAivaFBOZYDHU97229613 = RDYERXUmSGbHCiHRpONAivaFBOZYDHU27074642; }
// Junk Finished
