#pragma once

#include "../interfaces/IGameEventmanager.hpp"
#include "../Singleton.hpp"

#include <vector>

#include <iostream>

struct HitMarkerInfo
{
	float m_flCurTime;
	float m_flNextShot;
	float m_flExpTime;
	int m_iDmg;
	int m_iIndex;
	int m_iHitbox;
	std::string m_szMethod;
};

struct EventInfo
{
	std::string m_szMessage;
	float m_flExpTime;
};

class PlayerHurtEvent : public IGameEventListener2, public Singleton<PlayerHurtEvent>
{
public:

	void FireGameEvent(IGameEvent* event);
	int  GetEventDebugID(void);

	void RegisterSelf();
	void UnregisterSelf();

	void Paint(void);

	void PushEvent(EventInfo event);

private:

	std::vector<HitMarkerInfo> hitMarkerInfo;
	std::vector<EventInfo> eventInfo;
};