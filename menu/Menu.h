#pragma once
#include "../Singleton.hpp"
#include <vector>

class Menu : public Singleton<Menu> {

public:
	void Render();
	void Shutdown();
	void ColorPicker(const char* name, float* color, bool alpha);

	void lgtpew();
	void Aimbot();
	void Antiaim();
	void Visuals();
	void Misc();
	void Skins();
	void Players();
	void Config();
	void LuaTab();
//	void MessageSend();
	bool isOpen = false;
	bool d3d9 = false;
};
