#include "lua.hpp"

void LUA::CreateState()
{
	luastate output = { luaL_newstate(), true, false };
	if (output.L == NULL)
	{
		Utils::ConsolePrint(true, "LUA: not enough memory\n");
		g_CVar->ConsoleColorPrintf(Color::Red, "LUA: not enough memory\n");
		output.usuable = false;
	}
	else luaL_openlibs(output.L);
	state = output;
}

void LUA::KillState()
{
	if (state.L == nullptr) return;

	lua_close(state.L);
	state.usuable = state.can_execute = false;
}

void LUA::LoadFile(const char *path)
{
	int load_status = luaL_loadfile(state.L, path);

	if (load_status != 0)
	{
		Utils::ConsolePrint(true, "LUA: Error Loading File %s\n", path);
		g_CVar->ConsoleColorPrintf(Color::Red, "LUA: Error Loading File %s\n", path);
		PrintError(load_status);
		return;
	}

	lua_pcall(state.L, 0, 0, 0); // load the script with no args for function setup.
	state.can_execute = true;
	state.loaded_is_File = true;
	state.loaded = path;
}

void LUA::LoadCode(std::string code)
{
	int load_status = luaL_loadbuffer(state.L, code.c_str(), code.length(), code.c_str());

	if (load_status != 0)
	{
		Utils::ConsolePrint(true, "LUA: Error Loading Buffer\n");
		g_CVar->ConsoleColorPrintf(Color::Red, "LUA: Error Loading Buffer\n");
		PrintError(load_status);
		return;
	}

	lua_pcall(state.L, 0, 0, 0); // load the script with no args for function setup.
	state.can_execute = true;
	state.loaded_is_File = false;
	state.loaded = code;
}