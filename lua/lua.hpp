// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <vector>
#include <string>

#include "../helpers/Utils.hpp"
#include "../SDK.hpp"

struct luastate
{
	lua_State *L;

	bool usuable;
	bool can_execute;

	bool loaded_is_File;
	std::string loaded;
};

enum failure_code
{
	CANT_EXECUTE = -9999,
	FAILED_EXECUTE = -10000,
	UNKNOWN_TYPE = -10001
};

class LUA
{
public:

	void CreateState();

	void KillState();

	void LoadFile(const char *path);
	void LoadCode(std::string code);

	inline void PrintError(int run_status)
	{
		switch (run_status)
		{
		case LUA_ERRRUN:
		{
			Utils::ConsolePrint(true, "LUA: Runtime Error: %s\n", lua_tostring(state.L, -1));
			g_CVar->ConsoleColorPrintf(Color::Red, "LUA: Runtime Error: %s\n", lua_tostring(state.L, -1));
			break;
		}
		case LUA_ERRSYNTAX:
		{
			Utils::ConsolePrint(true, "LUA: Syntax Error: %s\n", lua_tostring(state.L, -1));
			g_CVar->ConsoleColorPrintf(Color::Red, "LUA: Syntax Error: %s\n", lua_tostring(state.L, -1));
			break;
		}
		case LUA_ERRMEM:
		{
			Utils::ConsolePrint(true, "LUA: Memory Alloc Error: %s\n", lua_tostring(state.L, -1));
			g_CVar->ConsoleColorPrintf(Color::Red, "LUA: Memory Alloc Error: %s\n", lua_tostring(state.L, -1));
			break;
		}
		case LUA_ERRERR:
		{
			Utils::ConsolePrint(true, "LUA: Error returning Error: %s\n", lua_tostring(state.L, -1));
			g_CVar->ConsoleColorPrintf(Color::Red, "LUA: Error returning Error: %s\n", lua_tostring(state.L, -1));
			break;
		}
		default:
		{
			Utils::ConsolePrint(true, "LUA: Unknown Error: %s\n", lua_tostring(state.L, -1));
			g_CVar->ConsoleColorPrintf(Color::Red, "LUA: Unknown Error: %s\n", lua_tostring(state.L, -1));
			break;
		}
		}
	}

	float ExecuteFunction(const char *func, std::vector<float> arguments)
	{
		if (!state.usuable || !state.can_execute)
			return failure_code::CANT_EXECUTE;

		lua_getglobal(state.L, func);

		for (auto i = arguments.begin(); i != arguments.end(); i++)
		{
			lua_pushnumber(state.L, *i);
		}

		int run_status = lua_pcall(state.L, arguments.size(), 1, 0);
		if (run_status != 0)
		{
			Utils::ConsolePrint(true, "LUA: Error Calling Function %s\n", func);
			g_CVar->ConsoleColorPrintf(Color::Red, "LUA: Error Calling Function %s\n", func);
			PrintError(run_status);
			return failure_code::FAILED_EXECUTE;
		}

		float output;

		if (lua_isnumber(state.L, -1))
			output = lua_tonumber(state.L, -1);
		else
		{
			lua_pop(state.L, 1);
			return failure_code::UNKNOWN_TYPE;
		}

		lua_pop(state.L, 1);
		return output;
	}

	const char *ExecuteFunction(const char *func, std::vector<int> arguments)
	{
		if (!state.usuable || !state.can_execute)
			return nullptr;

		lua_getglobal(state.L, func);

		for (auto i = arguments.begin(); i != arguments.end(); i++)
		{
			lua_pushnumber(state.L, *i);
		}

		int run_status = lua_pcall(state.L, arguments.size(), 1, 0);
		if (run_status != 0)
		{
			Utils::ConsolePrint(true, "LUA: Error Calling Function %s\n", func);
			g_CVar->ConsoleColorPrintf(Color::Red, "LUA: Error Calling Function %s\n", func);
			PrintError(run_status);
			return nullptr;
		}

		const char *output;

		if (lua_isstring(state.L, -1))
			output = lua_tostring(state.L, -1);
		else
		{
			lua_pop(state.L, 1);
			return nullptr;
		}

		lua_pop(state.L, 1);
		return output;
	}
public:
	luastate state;
};