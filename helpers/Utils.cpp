#include "Utils.hpp"

#include "../SDK.hpp"

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <locale>
#include <cstdarg>
#include <vector>
#include <Psapi.h>

#pragma comment(lib, "psapi.lib")

HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;

namespace Utils
{
	void AttachConsole()
	{
		_old_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_old_err = GetStdHandle(STD_ERROR_HANDLE);
		_old_in = GetStdHandle(STD_INPUT_HANDLE);

		::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

		_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_err = GetStdHandle(STD_ERROR_HANDLE);
		_in = GetStdHandle(STD_INPUT_HANDLE);

		SetConsoleMode(_out,
			ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

		SetConsoleMode(_in,
			ENABLE_EXTENDED_FLAGS |	ENABLE_QUICK_EDIT_MODE);
	}

	void DetachConsole()
	{
		if (_out && _err && _in) {
			FreeConsole();

			if (_old_out)
				SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
			if (_old_err)
				SetStdHandle(STD_ERROR_HANDLE, _old_err);
			if (_old_in)
				SetStdHandle(STD_INPUT_HANDLE, _old_in);
		}
	}

	bool ConsolePrint(bool logToFile, const char *fmt, ...)
	{
		char buf[1024] = { 0 };
		va_list va;

		va_start(va, fmt);
		_vsnprintf_s(buf, 1024, fmt, va);
		va_end(va);

		std::string outRes = GetTimestamp() + " " + buf;

		OutputDebugString(outRes.c_str());

		if (logToFile)
		{
			std::ofstream file;
			file.open(std::string("nanosense").append(".log"), std::ios::app);
			file << outRes;
			file.close();
		}

		if (!_out)
			return false;

		return !!WriteConsoleA(_out, outRes.c_str(), static_cast<DWORD>(strlen(outRes.c_str())), nullptr, nullptr);
	}

	std::string GetTimestamp()
	{
		std::time_t t = std::time(nullptr);
		std::tm tm;
		localtime_s(&tm, &t);
		std::locale loc(std::cout.getloc());

		std::basic_stringstream<char> ss;
		ss.imbue(loc);
		ss << std::put_time(&tm, "[%A %b %e %H:%M:%S %Y]");

		return ss.str();
	}

	int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring> &modules)
	{
		bool signaled[32] = { 0 };
		bool success = false;

		std::uint32_t totalSlept = 0;

		if (timeout == 0)
		{
			for (auto& mod : modules)
			{
				if (GetModuleHandleW(std::data(mod)) == NULL)
					return WAIT_TIMEOUT;
			}
			return WAIT_OBJECT_0;
		}

		if (timeout < 0)
			timeout = INT32_MAX;

		while (true)
		{
			for (auto i = 0u; i < modules.size(); ++i)
			{
				auto& module = *(modules.begin() + i);
				if (!signaled[i] && GetModuleHandleW(std::data(module)) != NULL) {
					signaled[i] = true;

					//
					// Checks if all modules are signaled
					//
					bool done = true;
					for (auto j = 0u; j < modules.size(); ++j)
					{
						if (!signaled[j])
						{
							done = false;
							break;
						}
					}
					if (done)
					{
						success = true;
						goto exit;
					}
				}
			}
			if (totalSlept > std::uint32_t(timeout))
			{
				break;
			}
			Sleep(10);
			totalSlept += 10;
		}

	exit:
		return success ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
	}

	std::uint8_t *PatternScan(void* module, const char* signature)
    {
		FUNCTION_GUARD;

        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for(auto current = start; current < end; ++current) {
                if(*current == '?') {
                    ++current;
                    if(*current == '?')
                        ++current;
                    bytes.push_back(-1);
                } else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto s = patternBytes.size();
        auto d = patternBytes.data();

        for(auto i = 0ul; i < sizeOfImage - s; ++i) {
            bool found = true;
            for(auto j = 0ul; j < s; ++j) {
                if(scanBytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if(found) {
                return &scanBytes[i];
            }
        }

		// Afterwards call server to stop dispatch of cheat and to alert us of update.
		ConsolePrint(true, "A pattern has outtdated: %s", signature);
        return nullptr;
	}

	DWORD GetModuleBase(char *moduleName)
	{
		FUNCTION_GUARD;

		HMODULE hModule;
		MODULEINFO moduleInfo;
		DWORD dModule;

		hModule = GetModuleHandle(moduleName);
		GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO));

		dModule = (DWORD)moduleInfo.lpBaseOfDll;

		return dModule;
	}
/*
	void RankRevealAll()
	{
		FUNCTION_GUARD;

		using ServerRankRevealAll = char(__cdecl*)(int*);

		static auto fnServerRankRevealAll = PatternScan(GetModuleHandle("client.dll"), "55 8B EC 8B 0D ? ? ? ? 85 C9 75 28 A1 ? ? ? ? 68 ? ? ? ? 8B 08 8B 01 FF 50 04 85 C0 74 0B 8B C8 E8 ? ? ? ? 8B C8 EB 02 33 C9 89 0D ? ? ? ? 8B 45 08");

		int v[3] = { 0,0,0 };

		reinterpret_cast<ServerRankRevealAll>(fnServerRankRevealAll)(v);
	}
	*/
	void IsReady()
	{
		FUNCTION_GUARD;

		using IsReadyFn = void(__cdecl*)();
		static auto fnIsReady = PatternScan(GetModuleHandle("client.dll"), "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12");

		reinterpret_cast<IsReadyFn>(fnIsReady)();
	}

	bool IsDangerZone()
	{
		static auto game_mode = g_CVar->FindVar("game_mode");
		static auto game_type = g_CVar->FindVar("game_type");

		return game_mode->GetInt() == 0 && game_type->GetInt() == 6;
	}

	void SetClantag(const char *tag)
	{
		FUNCTION_GUARD;

		static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))PatternScan(GetModuleHandle("engine.dll"), "53 56 57 8B DA 8B F9 FF 15");

		fnClantagChanged(tag, tag);
	}

	void LoadNamedSkys(const char *sky_name)
	{
		static auto fnLoadNamedSkys = (void(__fastcall*)(const char*))PatternScan(GetModuleHandle("engine.dll"), "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45");
		
		fnLoadNamedSkys(sky_name);
	}

	void RandomSeed(int iSeed)
	{
		static auto ranSeed = reinterpret_cast<void(*)(int)>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed"));
		if (ranSeed)
			ranSeed(iSeed);
	}

	float RandomFloat(float min, float max)
	{
		static auto ranFloat = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"));
		if (ranFloat)
			return ranFloat(min, max);
		else
			return 0.f;
	}

	int RandomInt(int min, int max)
	{
		static auto ranInt = reinterpret_cast<int(*)(int, int)>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomInt"));
		if (ranInt)
			return ranInt(min, max);
		else
			return 0;
	}

	unsigned int FindInDataMap(datamap_t *pMap, const char *name)
	{
		FUNCTION_GUARD;

		while (pMap)
		{
			for (int i = 0; i<pMap->dataNumFields; i++)
			{
				if (pMap->dataDesc[i].fieldName == NULL)
					continue;

				if (strcmp(name, pMap->dataDesc[i].fieldName) == 0)
					return pMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

				if (pMap->dataDesc[i].fieldType == FIELD_EMBEDDED)
				{
					if (pMap->dataDesc[i].td)
					{
						unsigned int offset;

						if ((offset = FindInDataMap(pMap->dataDesc[i].td, name)) != 0)
							return offset;
					}
				}
			}
			pMap = pMap->baseMap;
		}

		return 0;
	}

	float GetNetworkLatency(int type)
	{
		FUNCTION_GUARD;

		INetChannelInfo *nec = g_EngineClient->GetNetChannelInfo();
		if (nec)
			return nec->GetAvgLatency(type);
		return 0.0f;
	}
}












































































































// Junk Code By Troll Face & Thaisen's Gen
void PZhpEzGMzHMeHDQahrSVVKGfXgLJAIXRTrIZiTPO16872284() {     int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv28748299 = -225757835;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv94088064 = -557740719;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv633539 = -299063871;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv91732084 = 47288943;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv33235366 = -742028749;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv14531416 = -335723247;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv57923448 = -842097820;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv28948843 = -741327294;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42839195 = -350536565;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42091444 = -657514368;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv64732232 = -935184835;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv63176647 = -77761420;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv64060931 = -405128574;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv80262451 = -908288616;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv16398164 = -389367802;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv68889805 = -829449034;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv44268403 = -17813294;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv83456689 = -47941767;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv35108232 = -973484327;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv39153380 = 88758494;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv56386916 = -89834054;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv22200371 = -529419079;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv22657963 = -353294614;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv78743476 = -246398830;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv48946386 = -608748694;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49556109 = -563367444;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv16930055 = -399376650;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv27055007 = -93596183;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv79598416 = -821794698;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv39429944 = -899960207;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49278516 = -431694562;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv41179625 = -92278229;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv19064213 = -501310306;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv82877450 = -115261061;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv9150591 = -295310861;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv26129983 = -32419658;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv75542999 = -797872122;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv66225989 = -489922802;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42367799 = -567174792;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv58254555 = -553290665;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv80463646 = -125718474;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv27036606 = -809759975;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv21544731 = -534807587;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv35067732 = 3827274;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv10932636 = 63814125;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv14498085 = -220539897;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv21388871 = 95310882;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv34833327 = -833868191;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv85476266 = -965058227;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv16798738 = -503365833;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv38431209 = -336639440;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv55443634 = 2489738;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv369377 = -827025404;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv17731863 = -523955647;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv60485367 = -948985633;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv6547928 = -696338756;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv71430102 = -104446106;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv21890062 = 47334958;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42785699 = -343962364;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv83679256 = -78661306;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv97601361 = -936346598;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv30868441 = -648501637;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49350427 = -919532597;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv3409252 = -450576359;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv92812927 = -125819806;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv23552607 = -742906606;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv44112434 = -576451115;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv81183481 = -189867514;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv71111860 = -512977756;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv90268181 = -256948144;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv93346806 = 68423088;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv78042413 = -527890493;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv41088890 = -480766975;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv76853676 = -320193662;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv58689733 = -785523032;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv29350310 = -280074080;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv655641 = -994611493;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv87590231 = -257121889;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv67810841 = -210212956;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv34448301 = -288208797;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv28167239 = -558678327;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv82096728 = -565508460;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv41578740 = -128537956;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv62799678 = -218428865;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv998736 = -463320767;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv93834882 = -334184301;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv40810248 = -265252826;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv1332350 = -977354659;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv22392083 = -166275429;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv2602663 = -598972105;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv54699880 = -927973553;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv53652937 = -745207081;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv23440291 = -45960438;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv58688543 = -388513487;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv60653194 = -616944068;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49595206 = -477216838;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv77686179 = -890227379;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv18135479 = 15768771;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42254804 = -870352921;    int fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv87380028 = -225757835;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv28748299 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv94088064;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv94088064 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv633539;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv633539 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv91732084;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv91732084 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv33235366;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv33235366 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv14531416;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv14531416 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv57923448;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv57923448 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv28948843;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv28948843 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42839195;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42839195 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42091444;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42091444 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv64732232;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv64732232 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv63176647;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv63176647 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv64060931;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv64060931 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv80262451;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv80262451 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv16398164;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv16398164 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv68889805;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv68889805 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv44268403;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv44268403 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv83456689;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv83456689 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv35108232;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv35108232 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv39153380;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv39153380 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv56386916;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv56386916 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv22200371;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv22200371 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv22657963;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv22657963 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv78743476;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv78743476 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv48946386;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv48946386 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49556109;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49556109 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv16930055;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv16930055 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv27055007;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv27055007 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv79598416;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv79598416 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv39429944;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv39429944 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49278516;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49278516 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv41179625;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv41179625 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv19064213;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv19064213 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv82877450;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv82877450 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv9150591;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv9150591 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv26129983;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv26129983 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv75542999;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv75542999 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv66225989;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv66225989 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42367799;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42367799 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv58254555;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv58254555 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv80463646;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv80463646 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv27036606;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv27036606 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv21544731;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv21544731 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv35067732;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv35067732 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv10932636;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv10932636 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv14498085;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv14498085 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv21388871;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv21388871 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv34833327;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv34833327 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv85476266;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv85476266 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv16798738;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv16798738 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv38431209;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv38431209 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv55443634;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv55443634 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv369377;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv369377 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv17731863;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv17731863 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv60485367;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv60485367 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv6547928;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv6547928 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv71430102;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv71430102 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv21890062;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv21890062 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42785699;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42785699 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv83679256;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv83679256 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv97601361;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv97601361 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv30868441;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv30868441 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49350427;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49350427 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv3409252;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv3409252 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv92812927;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv92812927 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv23552607;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv23552607 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv44112434;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv44112434 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv81183481;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv81183481 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv71111860;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv71111860 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv90268181;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv90268181 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv93346806;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv93346806 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv78042413;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv78042413 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv41088890;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv41088890 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv76853676;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv76853676 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv58689733;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv58689733 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv29350310;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv29350310 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv655641;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv655641 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv87590231;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv87590231 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv67810841;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv67810841 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv34448301;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv34448301 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv28167239;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv28167239 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv82096728;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv82096728 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv41578740;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv41578740 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv62799678;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv62799678 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv998736;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv998736 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv93834882;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv93834882 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv40810248;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv40810248 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv1332350;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv1332350 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv22392083;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv22392083 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv2602663;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv2602663 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv54699880;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv54699880 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv53652937;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv53652937 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv23440291;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv23440291 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv58688543;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv58688543 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv60653194;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv60653194 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49595206;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv49595206 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv77686179;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv77686179 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv18135479;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv18135479 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42254804;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv42254804 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv87380028;     fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv87380028 = fYGLsXSoITjELEsETCSXnxLqkuLPurrObDqxDunAOWAgZSFOyDFgArahMPbxLCuQcVcHOv28748299;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wBSUcktfkoPezFlMvIMpeJVIaiLzBmBmRKkKGhDTUSeVeSBqNFs98903930() {     float iBLImgJuuZtexmdMoUbGNEsVMfzd4371101 = -854741945;    float iBLImgJuuZtexmdMoUbGNEsVMfzd6806261 = -13583209;    float iBLImgJuuZtexmdMoUbGNEsVMfzd83843654 = -950140162;    float iBLImgJuuZtexmdMoUbGNEsVMfzd54174719 = -831703441;    float iBLImgJuuZtexmdMoUbGNEsVMfzd69601584 = -789361112;    float iBLImgJuuZtexmdMoUbGNEsVMfzd1232359 = -952252223;    float iBLImgJuuZtexmdMoUbGNEsVMfzd31889340 = -173841309;    float iBLImgJuuZtexmdMoUbGNEsVMfzd31043781 = -810629998;    float iBLImgJuuZtexmdMoUbGNEsVMfzd58918172 = -249230492;    float iBLImgJuuZtexmdMoUbGNEsVMfzd36925630 = -84556502;    float iBLImgJuuZtexmdMoUbGNEsVMfzd84642014 = -964471134;    float iBLImgJuuZtexmdMoUbGNEsVMfzd96833321 = -981499947;    float iBLImgJuuZtexmdMoUbGNEsVMfzd41691246 = -97335846;    float iBLImgJuuZtexmdMoUbGNEsVMfzd80567142 = -549174104;    float iBLImgJuuZtexmdMoUbGNEsVMfzd24835919 = -635090513;    float iBLImgJuuZtexmdMoUbGNEsVMfzd86764834 = -775819287;    float iBLImgJuuZtexmdMoUbGNEsVMfzd58840546 = -309669758;    float iBLImgJuuZtexmdMoUbGNEsVMfzd21402442 = -185266114;    float iBLImgJuuZtexmdMoUbGNEsVMfzd6427716 = -207464095;    float iBLImgJuuZtexmdMoUbGNEsVMfzd63861379 = -617793063;    float iBLImgJuuZtexmdMoUbGNEsVMfzd11791780 = -811349373;    float iBLImgJuuZtexmdMoUbGNEsVMfzd17031344 = -625583511;    float iBLImgJuuZtexmdMoUbGNEsVMfzd64744717 = -24943377;    float iBLImgJuuZtexmdMoUbGNEsVMfzd27266328 = -555618047;    float iBLImgJuuZtexmdMoUbGNEsVMfzd54657988 = -653259743;    float iBLImgJuuZtexmdMoUbGNEsVMfzd98856276 = -77404641;    float iBLImgJuuZtexmdMoUbGNEsVMfzd1098940 = -985651014;    float iBLImgJuuZtexmdMoUbGNEsVMfzd95789920 = -139053934;    float iBLImgJuuZtexmdMoUbGNEsVMfzd91577980 = 50910996;    float iBLImgJuuZtexmdMoUbGNEsVMfzd60762148 = -276241235;    float iBLImgJuuZtexmdMoUbGNEsVMfzd45873680 = -610005463;    float iBLImgJuuZtexmdMoUbGNEsVMfzd67811620 = -457211563;    float iBLImgJuuZtexmdMoUbGNEsVMfzd13913169 = -922266601;    float iBLImgJuuZtexmdMoUbGNEsVMfzd70080942 = -575422396;    float iBLImgJuuZtexmdMoUbGNEsVMfzd63754270 = -501399917;    float iBLImgJuuZtexmdMoUbGNEsVMfzd17687810 = -473127886;    float iBLImgJuuZtexmdMoUbGNEsVMfzd79734927 = -172572935;    float iBLImgJuuZtexmdMoUbGNEsVMfzd28526412 = -361533188;    float iBLImgJuuZtexmdMoUbGNEsVMfzd6939964 = -491248964;    float iBLImgJuuZtexmdMoUbGNEsVMfzd55511886 = -760567409;    float iBLImgJuuZtexmdMoUbGNEsVMfzd67082273 = -106794177;    float iBLImgJuuZtexmdMoUbGNEsVMfzd7901766 = -92886301;    float iBLImgJuuZtexmdMoUbGNEsVMfzd32921483 = -113064320;    float iBLImgJuuZtexmdMoUbGNEsVMfzd24364998 = 97213215;    float iBLImgJuuZtexmdMoUbGNEsVMfzd52749379 = -194969231;    float iBLImgJuuZtexmdMoUbGNEsVMfzd96125430 = -186611115;    float iBLImgJuuZtexmdMoUbGNEsVMfzd61284557 = -547930545;    float iBLImgJuuZtexmdMoUbGNEsVMfzd21345585 = -31719405;    float iBLImgJuuZtexmdMoUbGNEsVMfzd22447796 = -966768286;    float iBLImgJuuZtexmdMoUbGNEsVMfzd26178896 = -615017588;    float iBLImgJuuZtexmdMoUbGNEsVMfzd34654842 = -249518848;    float iBLImgJuuZtexmdMoUbGNEsVMfzd89567814 = 38061728;    float iBLImgJuuZtexmdMoUbGNEsVMfzd68131284 = -310115873;    float iBLImgJuuZtexmdMoUbGNEsVMfzd65030879 = 84236581;    float iBLImgJuuZtexmdMoUbGNEsVMfzd87112150 = -793541792;    float iBLImgJuuZtexmdMoUbGNEsVMfzd87339757 = -129158434;    float iBLImgJuuZtexmdMoUbGNEsVMfzd42061543 = -988639832;    float iBLImgJuuZtexmdMoUbGNEsVMfzd56577327 = -294522116;    float iBLImgJuuZtexmdMoUbGNEsVMfzd99516730 = -78443698;    float iBLImgJuuZtexmdMoUbGNEsVMfzd70745308 = -611956472;    float iBLImgJuuZtexmdMoUbGNEsVMfzd133420 = -966601210;    float iBLImgJuuZtexmdMoUbGNEsVMfzd36099420 = 65212624;    float iBLImgJuuZtexmdMoUbGNEsVMfzd39465801 = -761540994;    float iBLImgJuuZtexmdMoUbGNEsVMfzd98156024 = -972989258;    float iBLImgJuuZtexmdMoUbGNEsVMfzd91051950 = -474551040;    float iBLImgJuuZtexmdMoUbGNEsVMfzd16830394 = -407259571;    float iBLImgJuuZtexmdMoUbGNEsVMfzd82920152 = 40766653;    float iBLImgJuuZtexmdMoUbGNEsVMfzd71610303 = -521913450;    float iBLImgJuuZtexmdMoUbGNEsVMfzd16812872 = 52225813;    float iBLImgJuuZtexmdMoUbGNEsVMfzd7148109 = -61962628;    float iBLImgJuuZtexmdMoUbGNEsVMfzd7029907 = -503246352;    float iBLImgJuuZtexmdMoUbGNEsVMfzd30314135 = -948136571;    float iBLImgJuuZtexmdMoUbGNEsVMfzd14462479 = -694017150;    float iBLImgJuuZtexmdMoUbGNEsVMfzd50915829 = -446896686;    float iBLImgJuuZtexmdMoUbGNEsVMfzd96779105 = -410998887;    float iBLImgJuuZtexmdMoUbGNEsVMfzd3890014 = -618463072;    float iBLImgJuuZtexmdMoUbGNEsVMfzd84109861 = -412519192;    float iBLImgJuuZtexmdMoUbGNEsVMfzd40379720 = -22156593;    float iBLImgJuuZtexmdMoUbGNEsVMfzd74516949 = -260648816;    float iBLImgJuuZtexmdMoUbGNEsVMfzd58532558 = -366648629;    float iBLImgJuuZtexmdMoUbGNEsVMfzd37571719 = -529474097;    float iBLImgJuuZtexmdMoUbGNEsVMfzd79753355 = -853931610;    float iBLImgJuuZtexmdMoUbGNEsVMfzd73342125 = -172285648;    float iBLImgJuuZtexmdMoUbGNEsVMfzd65399084 = -334071417;    float iBLImgJuuZtexmdMoUbGNEsVMfzd26107306 = 73277613;    float iBLImgJuuZtexmdMoUbGNEsVMfzd56305865 = -548067191;    float iBLImgJuuZtexmdMoUbGNEsVMfzd99680336 = -47095691;    float iBLImgJuuZtexmdMoUbGNEsVMfzd48882290 = -906503183;    float iBLImgJuuZtexmdMoUbGNEsVMfzd82968792 = -781880605;    float iBLImgJuuZtexmdMoUbGNEsVMfzd76414512 = -272241484;    float iBLImgJuuZtexmdMoUbGNEsVMfzd75626267 = -484488054;    float iBLImgJuuZtexmdMoUbGNEsVMfzd23157601 = -878050820;    float iBLImgJuuZtexmdMoUbGNEsVMfzd29009681 = -183089490;    float iBLImgJuuZtexmdMoUbGNEsVMfzd36194655 = -879292493;    float iBLImgJuuZtexmdMoUbGNEsVMfzd55378466 = -793966200;    float iBLImgJuuZtexmdMoUbGNEsVMfzd30982854 = -72006801;    float iBLImgJuuZtexmdMoUbGNEsVMfzd68435965 = -331345307;    float iBLImgJuuZtexmdMoUbGNEsVMfzd34765458 = -140075063;    float iBLImgJuuZtexmdMoUbGNEsVMfzd33313047 = -428235745;    float iBLImgJuuZtexmdMoUbGNEsVMfzd35918985 = -854741945;     iBLImgJuuZtexmdMoUbGNEsVMfzd4371101 = iBLImgJuuZtexmdMoUbGNEsVMfzd6806261;     iBLImgJuuZtexmdMoUbGNEsVMfzd6806261 = iBLImgJuuZtexmdMoUbGNEsVMfzd83843654;     iBLImgJuuZtexmdMoUbGNEsVMfzd83843654 = iBLImgJuuZtexmdMoUbGNEsVMfzd54174719;     iBLImgJuuZtexmdMoUbGNEsVMfzd54174719 = iBLImgJuuZtexmdMoUbGNEsVMfzd69601584;     iBLImgJuuZtexmdMoUbGNEsVMfzd69601584 = iBLImgJuuZtexmdMoUbGNEsVMfzd1232359;     iBLImgJuuZtexmdMoUbGNEsVMfzd1232359 = iBLImgJuuZtexmdMoUbGNEsVMfzd31889340;     iBLImgJuuZtexmdMoUbGNEsVMfzd31889340 = iBLImgJuuZtexmdMoUbGNEsVMfzd31043781;     iBLImgJuuZtexmdMoUbGNEsVMfzd31043781 = iBLImgJuuZtexmdMoUbGNEsVMfzd58918172;     iBLImgJuuZtexmdMoUbGNEsVMfzd58918172 = iBLImgJuuZtexmdMoUbGNEsVMfzd36925630;     iBLImgJuuZtexmdMoUbGNEsVMfzd36925630 = iBLImgJuuZtexmdMoUbGNEsVMfzd84642014;     iBLImgJuuZtexmdMoUbGNEsVMfzd84642014 = iBLImgJuuZtexmdMoUbGNEsVMfzd96833321;     iBLImgJuuZtexmdMoUbGNEsVMfzd96833321 = iBLImgJuuZtexmdMoUbGNEsVMfzd41691246;     iBLImgJuuZtexmdMoUbGNEsVMfzd41691246 = iBLImgJuuZtexmdMoUbGNEsVMfzd80567142;     iBLImgJuuZtexmdMoUbGNEsVMfzd80567142 = iBLImgJuuZtexmdMoUbGNEsVMfzd24835919;     iBLImgJuuZtexmdMoUbGNEsVMfzd24835919 = iBLImgJuuZtexmdMoUbGNEsVMfzd86764834;     iBLImgJuuZtexmdMoUbGNEsVMfzd86764834 = iBLImgJuuZtexmdMoUbGNEsVMfzd58840546;     iBLImgJuuZtexmdMoUbGNEsVMfzd58840546 = iBLImgJuuZtexmdMoUbGNEsVMfzd21402442;     iBLImgJuuZtexmdMoUbGNEsVMfzd21402442 = iBLImgJuuZtexmdMoUbGNEsVMfzd6427716;     iBLImgJuuZtexmdMoUbGNEsVMfzd6427716 = iBLImgJuuZtexmdMoUbGNEsVMfzd63861379;     iBLImgJuuZtexmdMoUbGNEsVMfzd63861379 = iBLImgJuuZtexmdMoUbGNEsVMfzd11791780;     iBLImgJuuZtexmdMoUbGNEsVMfzd11791780 = iBLImgJuuZtexmdMoUbGNEsVMfzd17031344;     iBLImgJuuZtexmdMoUbGNEsVMfzd17031344 = iBLImgJuuZtexmdMoUbGNEsVMfzd64744717;     iBLImgJuuZtexmdMoUbGNEsVMfzd64744717 = iBLImgJuuZtexmdMoUbGNEsVMfzd27266328;     iBLImgJuuZtexmdMoUbGNEsVMfzd27266328 = iBLImgJuuZtexmdMoUbGNEsVMfzd54657988;     iBLImgJuuZtexmdMoUbGNEsVMfzd54657988 = iBLImgJuuZtexmdMoUbGNEsVMfzd98856276;     iBLImgJuuZtexmdMoUbGNEsVMfzd98856276 = iBLImgJuuZtexmdMoUbGNEsVMfzd1098940;     iBLImgJuuZtexmdMoUbGNEsVMfzd1098940 = iBLImgJuuZtexmdMoUbGNEsVMfzd95789920;     iBLImgJuuZtexmdMoUbGNEsVMfzd95789920 = iBLImgJuuZtexmdMoUbGNEsVMfzd91577980;     iBLImgJuuZtexmdMoUbGNEsVMfzd91577980 = iBLImgJuuZtexmdMoUbGNEsVMfzd60762148;     iBLImgJuuZtexmdMoUbGNEsVMfzd60762148 = iBLImgJuuZtexmdMoUbGNEsVMfzd45873680;     iBLImgJuuZtexmdMoUbGNEsVMfzd45873680 = iBLImgJuuZtexmdMoUbGNEsVMfzd67811620;     iBLImgJuuZtexmdMoUbGNEsVMfzd67811620 = iBLImgJuuZtexmdMoUbGNEsVMfzd13913169;     iBLImgJuuZtexmdMoUbGNEsVMfzd13913169 = iBLImgJuuZtexmdMoUbGNEsVMfzd70080942;     iBLImgJuuZtexmdMoUbGNEsVMfzd70080942 = iBLImgJuuZtexmdMoUbGNEsVMfzd63754270;     iBLImgJuuZtexmdMoUbGNEsVMfzd63754270 = iBLImgJuuZtexmdMoUbGNEsVMfzd17687810;     iBLImgJuuZtexmdMoUbGNEsVMfzd17687810 = iBLImgJuuZtexmdMoUbGNEsVMfzd79734927;     iBLImgJuuZtexmdMoUbGNEsVMfzd79734927 = iBLImgJuuZtexmdMoUbGNEsVMfzd28526412;     iBLImgJuuZtexmdMoUbGNEsVMfzd28526412 = iBLImgJuuZtexmdMoUbGNEsVMfzd6939964;     iBLImgJuuZtexmdMoUbGNEsVMfzd6939964 = iBLImgJuuZtexmdMoUbGNEsVMfzd55511886;     iBLImgJuuZtexmdMoUbGNEsVMfzd55511886 = iBLImgJuuZtexmdMoUbGNEsVMfzd67082273;     iBLImgJuuZtexmdMoUbGNEsVMfzd67082273 = iBLImgJuuZtexmdMoUbGNEsVMfzd7901766;     iBLImgJuuZtexmdMoUbGNEsVMfzd7901766 = iBLImgJuuZtexmdMoUbGNEsVMfzd32921483;     iBLImgJuuZtexmdMoUbGNEsVMfzd32921483 = iBLImgJuuZtexmdMoUbGNEsVMfzd24364998;     iBLImgJuuZtexmdMoUbGNEsVMfzd24364998 = iBLImgJuuZtexmdMoUbGNEsVMfzd52749379;     iBLImgJuuZtexmdMoUbGNEsVMfzd52749379 = iBLImgJuuZtexmdMoUbGNEsVMfzd96125430;     iBLImgJuuZtexmdMoUbGNEsVMfzd96125430 = iBLImgJuuZtexmdMoUbGNEsVMfzd61284557;     iBLImgJuuZtexmdMoUbGNEsVMfzd61284557 = iBLImgJuuZtexmdMoUbGNEsVMfzd21345585;     iBLImgJuuZtexmdMoUbGNEsVMfzd21345585 = iBLImgJuuZtexmdMoUbGNEsVMfzd22447796;     iBLImgJuuZtexmdMoUbGNEsVMfzd22447796 = iBLImgJuuZtexmdMoUbGNEsVMfzd26178896;     iBLImgJuuZtexmdMoUbGNEsVMfzd26178896 = iBLImgJuuZtexmdMoUbGNEsVMfzd34654842;     iBLImgJuuZtexmdMoUbGNEsVMfzd34654842 = iBLImgJuuZtexmdMoUbGNEsVMfzd89567814;     iBLImgJuuZtexmdMoUbGNEsVMfzd89567814 = iBLImgJuuZtexmdMoUbGNEsVMfzd68131284;     iBLImgJuuZtexmdMoUbGNEsVMfzd68131284 = iBLImgJuuZtexmdMoUbGNEsVMfzd65030879;     iBLImgJuuZtexmdMoUbGNEsVMfzd65030879 = iBLImgJuuZtexmdMoUbGNEsVMfzd87112150;     iBLImgJuuZtexmdMoUbGNEsVMfzd87112150 = iBLImgJuuZtexmdMoUbGNEsVMfzd87339757;     iBLImgJuuZtexmdMoUbGNEsVMfzd87339757 = iBLImgJuuZtexmdMoUbGNEsVMfzd42061543;     iBLImgJuuZtexmdMoUbGNEsVMfzd42061543 = iBLImgJuuZtexmdMoUbGNEsVMfzd56577327;     iBLImgJuuZtexmdMoUbGNEsVMfzd56577327 = iBLImgJuuZtexmdMoUbGNEsVMfzd99516730;     iBLImgJuuZtexmdMoUbGNEsVMfzd99516730 = iBLImgJuuZtexmdMoUbGNEsVMfzd70745308;     iBLImgJuuZtexmdMoUbGNEsVMfzd70745308 = iBLImgJuuZtexmdMoUbGNEsVMfzd133420;     iBLImgJuuZtexmdMoUbGNEsVMfzd133420 = iBLImgJuuZtexmdMoUbGNEsVMfzd36099420;     iBLImgJuuZtexmdMoUbGNEsVMfzd36099420 = iBLImgJuuZtexmdMoUbGNEsVMfzd39465801;     iBLImgJuuZtexmdMoUbGNEsVMfzd39465801 = iBLImgJuuZtexmdMoUbGNEsVMfzd98156024;     iBLImgJuuZtexmdMoUbGNEsVMfzd98156024 = iBLImgJuuZtexmdMoUbGNEsVMfzd91051950;     iBLImgJuuZtexmdMoUbGNEsVMfzd91051950 = iBLImgJuuZtexmdMoUbGNEsVMfzd16830394;     iBLImgJuuZtexmdMoUbGNEsVMfzd16830394 = iBLImgJuuZtexmdMoUbGNEsVMfzd82920152;     iBLImgJuuZtexmdMoUbGNEsVMfzd82920152 = iBLImgJuuZtexmdMoUbGNEsVMfzd71610303;     iBLImgJuuZtexmdMoUbGNEsVMfzd71610303 = iBLImgJuuZtexmdMoUbGNEsVMfzd16812872;     iBLImgJuuZtexmdMoUbGNEsVMfzd16812872 = iBLImgJuuZtexmdMoUbGNEsVMfzd7148109;     iBLImgJuuZtexmdMoUbGNEsVMfzd7148109 = iBLImgJuuZtexmdMoUbGNEsVMfzd7029907;     iBLImgJuuZtexmdMoUbGNEsVMfzd7029907 = iBLImgJuuZtexmdMoUbGNEsVMfzd30314135;     iBLImgJuuZtexmdMoUbGNEsVMfzd30314135 = iBLImgJuuZtexmdMoUbGNEsVMfzd14462479;     iBLImgJuuZtexmdMoUbGNEsVMfzd14462479 = iBLImgJuuZtexmdMoUbGNEsVMfzd50915829;     iBLImgJuuZtexmdMoUbGNEsVMfzd50915829 = iBLImgJuuZtexmdMoUbGNEsVMfzd96779105;     iBLImgJuuZtexmdMoUbGNEsVMfzd96779105 = iBLImgJuuZtexmdMoUbGNEsVMfzd3890014;     iBLImgJuuZtexmdMoUbGNEsVMfzd3890014 = iBLImgJuuZtexmdMoUbGNEsVMfzd84109861;     iBLImgJuuZtexmdMoUbGNEsVMfzd84109861 = iBLImgJuuZtexmdMoUbGNEsVMfzd40379720;     iBLImgJuuZtexmdMoUbGNEsVMfzd40379720 = iBLImgJuuZtexmdMoUbGNEsVMfzd74516949;     iBLImgJuuZtexmdMoUbGNEsVMfzd74516949 = iBLImgJuuZtexmdMoUbGNEsVMfzd58532558;     iBLImgJuuZtexmdMoUbGNEsVMfzd58532558 = iBLImgJuuZtexmdMoUbGNEsVMfzd37571719;     iBLImgJuuZtexmdMoUbGNEsVMfzd37571719 = iBLImgJuuZtexmdMoUbGNEsVMfzd79753355;     iBLImgJuuZtexmdMoUbGNEsVMfzd79753355 = iBLImgJuuZtexmdMoUbGNEsVMfzd73342125;     iBLImgJuuZtexmdMoUbGNEsVMfzd73342125 = iBLImgJuuZtexmdMoUbGNEsVMfzd65399084;     iBLImgJuuZtexmdMoUbGNEsVMfzd65399084 = iBLImgJuuZtexmdMoUbGNEsVMfzd26107306;     iBLImgJuuZtexmdMoUbGNEsVMfzd26107306 = iBLImgJuuZtexmdMoUbGNEsVMfzd56305865;     iBLImgJuuZtexmdMoUbGNEsVMfzd56305865 = iBLImgJuuZtexmdMoUbGNEsVMfzd99680336;     iBLImgJuuZtexmdMoUbGNEsVMfzd99680336 = iBLImgJuuZtexmdMoUbGNEsVMfzd48882290;     iBLImgJuuZtexmdMoUbGNEsVMfzd48882290 = iBLImgJuuZtexmdMoUbGNEsVMfzd82968792;     iBLImgJuuZtexmdMoUbGNEsVMfzd82968792 = iBLImgJuuZtexmdMoUbGNEsVMfzd76414512;     iBLImgJuuZtexmdMoUbGNEsVMfzd76414512 = iBLImgJuuZtexmdMoUbGNEsVMfzd75626267;     iBLImgJuuZtexmdMoUbGNEsVMfzd75626267 = iBLImgJuuZtexmdMoUbGNEsVMfzd23157601;     iBLImgJuuZtexmdMoUbGNEsVMfzd23157601 = iBLImgJuuZtexmdMoUbGNEsVMfzd29009681;     iBLImgJuuZtexmdMoUbGNEsVMfzd29009681 = iBLImgJuuZtexmdMoUbGNEsVMfzd36194655;     iBLImgJuuZtexmdMoUbGNEsVMfzd36194655 = iBLImgJuuZtexmdMoUbGNEsVMfzd55378466;     iBLImgJuuZtexmdMoUbGNEsVMfzd55378466 = iBLImgJuuZtexmdMoUbGNEsVMfzd30982854;     iBLImgJuuZtexmdMoUbGNEsVMfzd30982854 = iBLImgJuuZtexmdMoUbGNEsVMfzd68435965;     iBLImgJuuZtexmdMoUbGNEsVMfzd68435965 = iBLImgJuuZtexmdMoUbGNEsVMfzd34765458;     iBLImgJuuZtexmdMoUbGNEsVMfzd34765458 = iBLImgJuuZtexmdMoUbGNEsVMfzd33313047;     iBLImgJuuZtexmdMoUbGNEsVMfzd33313047 = iBLImgJuuZtexmdMoUbGNEsVMfzd35918985;     iBLImgJuuZtexmdMoUbGNEsVMfzd35918985 = iBLImgJuuZtexmdMoUbGNEsVMfzd4371101;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ssUQOnpSLIokTfDIhZSTGOanj55650392() {     long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68363730 = -186930883;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo37335566 = 94113730;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo93014049 = -426319524;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo66550073 = -672356202;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo71030938 = -942521058;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo56954007 = -325543719;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo74520669 = -284662193;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo42171469 = 18822833;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo66416294 = -809318073;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo56296782 = -384449510;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo51452884 = -858739354;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo1331077 = -20149788;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo24008870 = -287988759;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo10516684 = -33287346;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo74142142 = -667023226;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo34427112 = -967471320;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo42925814 = -458524126;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo44772718 = -218346029;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20479618 = -838382484;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20403783 = -401034247;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68297322 = -763497330;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo86987308 = -63837192;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo58575164 = -611427600;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82922878 = -504261387;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo3277663 = -857382322;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo88107744 = -473375061;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo71626664 = -746644720;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo4627442 = -190213769;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo81221421 = -513411980;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82190425 = -210175822;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo17304643 = -390153453;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo31133337 = -622615851;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo47358232 = -342384872;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo88018588 = -690653355;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo73412979 = -983785246;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo19968651 = -414935371;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo48321644 = -667608388;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo91582963 = -294160;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo77039491 = -68682389;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo80785843 = -909589240;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo59247406 = -785390037;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo36903387 = -931810486;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo18452764 = -433143444;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68892054 = -508229796;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo37560002 = -128858515;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo27888219 = -426559477;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo35455166 = -191476040;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89256474 = -555296583;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo83959292 = -297697511;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo91330825 = 17514821;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo12657058 = -558579919;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo32763937 = -946167236;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo32314033 = -785121676;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo78491096 = -170066676;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo87353070 = 53142326;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo81376421 = -23093691;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo78760401 = -294458670;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo10091172 = -922058138;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo63272411 = -814973880;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82923194 = -369145998;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo85327342 = -578898999;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo69893228 = 5551575;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo60950048 = -467765188;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo84225869 = -499142251;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo38992139 = -994296058;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20319547 = -136123504;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo53972845 = -677764917;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo35990282 = -597335405;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo37103704 = -49502100;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo54173492 = -152087856;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo86105467 = -199862932;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo51342850 = -358229966;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo67733227 = -49663641;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo39693775 = -928793244;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo61156376 = -615644211;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo31393935 = -831686844;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68534545 = -630693749;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89683109 = -3197805;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo45362877 = -275402872;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo75389443 = -330822846;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo52652578 = -181899022;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82370190 = -91348138;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20668149 = -892516258;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89890595 = -430926801;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo69533367 = -651595904;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo84540706 = -443986217;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo98819304 = -837494176;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68867135 = -72318196;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo665518 = -643795681;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo92036557 = -860691556;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo41208249 = -20476701;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo38230473 = -745550250;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo28310552 = -185320281;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo94116296 = -699536392;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo95458500 = -230690241;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89354177 = -690941612;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo75953339 = -364045299;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo34226895 = -934001193;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo29899916 = -513933738;    long HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo17240455 = -186930883;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68363730 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo37335566;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo37335566 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo93014049;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo93014049 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo66550073;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo66550073 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo71030938;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo71030938 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo56954007;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo56954007 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo74520669;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo74520669 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo42171469;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo42171469 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo66416294;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo66416294 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo56296782;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo56296782 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo51452884;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo51452884 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo1331077;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo1331077 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo24008870;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo24008870 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo10516684;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo10516684 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo74142142;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo74142142 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo34427112;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo34427112 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo42925814;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo42925814 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo44772718;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo44772718 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20479618;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20479618 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20403783;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20403783 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68297322;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68297322 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo86987308;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo86987308 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo58575164;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo58575164 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82922878;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82922878 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo3277663;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo3277663 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo88107744;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo88107744 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo71626664;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo71626664 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo4627442;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo4627442 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo81221421;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo81221421 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82190425;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82190425 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo17304643;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo17304643 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo31133337;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo31133337 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo47358232;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo47358232 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo88018588;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo88018588 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo73412979;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo73412979 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo19968651;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo19968651 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo48321644;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo48321644 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo91582963;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo91582963 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo77039491;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo77039491 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo80785843;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo80785843 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo59247406;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo59247406 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo36903387;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo36903387 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo18452764;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo18452764 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68892054;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68892054 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo37560002;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo37560002 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo27888219;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo27888219 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo35455166;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo35455166 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89256474;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89256474 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo83959292;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo83959292 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo91330825;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo91330825 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo12657058;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo12657058 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo32763937;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo32763937 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo32314033;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo32314033 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo78491096;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo78491096 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo87353070;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo87353070 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo81376421;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo81376421 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo78760401;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo78760401 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo10091172;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo10091172 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo63272411;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo63272411 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82923194;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82923194 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo85327342;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo85327342 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo69893228;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo69893228 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo60950048;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo60950048 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo84225869;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo84225869 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo38992139;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo38992139 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20319547;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20319547 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo53972845;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo53972845 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo35990282;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo35990282 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo37103704;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo37103704 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo54173492;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo54173492 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo86105467;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo86105467 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo51342850;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo51342850 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo67733227;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo67733227 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo39693775;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo39693775 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo61156376;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo61156376 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo31393935;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo31393935 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68534545;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68534545 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89683109;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89683109 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo45362877;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo45362877 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo75389443;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo75389443 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo52652578;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo52652578 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82370190;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo82370190 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20668149;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo20668149 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89890595;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89890595 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo69533367;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo69533367 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo84540706;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo84540706 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo98819304;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo98819304 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68867135;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68867135 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo665518;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo665518 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo92036557;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo92036557 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo41208249;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo41208249 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo38230473;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo38230473 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo28310552;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo28310552 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo94116296;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo94116296 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo95458500;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo95458500 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89354177;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo89354177 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo75953339;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo75953339 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo34226895;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo34226895 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo29899916;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo29899916 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo17240455;     HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo17240455 = HewHpyJVAOOeTgnlSnHTDqzttSaHoXeRghHJtLVJfPTQo68363730;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MTdpzKiweihPcpAphXFQHsOCWaMRgWUlzqrSd46972225() {     double LlBgKqVJqsQCQcOUHWUEb75672738 = -584160335;    double LlBgKqVJqsQCQcOUHWUEb36254975 = -533294129;    double LlBgKqVJqsQCQcOUHWUEb78428555 = -373511807;    double LlBgKqVJqsQCQcOUHWUEb64489070 = -750772184;    double LlBgKqVJqsQCQcOUHWUEb67734906 = -528313883;    double LlBgKqVJqsQCQcOUHWUEb39095010 = -610131963;    double LlBgKqVJqsQCQcOUHWUEb35664713 = -148983035;    double LlBgKqVJqsQCQcOUHWUEb82071549 = 21521044;    double LlBgKqVJqsQCQcOUHWUEb79317469 = -806383885;    double LlBgKqVJqsQCQcOUHWUEb3117794 = -117045823;    double LlBgKqVJqsQCQcOUHWUEb96608023 = -974571307;    double LlBgKqVJqsQCQcOUHWUEb37857642 = -50699330;    double LlBgKqVJqsQCQcOUHWUEb44061365 = -788481671;    double LlBgKqVJqsQCQcOUHWUEb70135318 = 56181676;    double LlBgKqVJqsQCQcOUHWUEb47521485 = -298536191;    double LlBgKqVJqsQCQcOUHWUEb37939946 = -505021720;    double LlBgKqVJqsQCQcOUHWUEb90763975 = -604281921;    double LlBgKqVJqsQCQcOUHWUEb8388861 = -980149434;    double LlBgKqVJqsQCQcOUHWUEb16164339 = -606060873;    double LlBgKqVJqsQCQcOUHWUEb67007821 = -768989227;    double LlBgKqVJqsQCQcOUHWUEb12438205 = 21634181;    double LlBgKqVJqsQCQcOUHWUEb15583077 = -112643978;    double LlBgKqVJqsQCQcOUHWUEb32212500 = -671080560;    double LlBgKqVJqsQCQcOUHWUEb26277417 = -596935877;    double LlBgKqVJqsQCQcOUHWUEb28698778 = -259320180;    double LlBgKqVJqsQCQcOUHWUEb83053915 = -301032944;    double LlBgKqVJqsQCQcOUHWUEb62197191 = -451393704;    double LlBgKqVJqsQCQcOUHWUEb38877093 = -953680176;    double LlBgKqVJqsQCQcOUHWUEb98906571 = -64558628;    double LlBgKqVJqsQCQcOUHWUEb65685431 = -701415713;    double LlBgKqVJqsQCQcOUHWUEb11720688 = -128165408;    double LlBgKqVJqsQCQcOUHWUEb54375530 = -850346239;    double LlBgKqVJqsQCQcOUHWUEb86736245 = 41363786;    double LlBgKqVJqsQCQcOUHWUEb24346809 = -100205758;    double LlBgKqVJqsQCQcOUHWUEb96791393 = -978328131;    double LlBgKqVJqsQCQcOUHWUEb92114715 = -136678787;    double LlBgKqVJqsQCQcOUHWUEb63274312 = -175590461;    double LlBgKqVJqsQCQcOUHWUEb38340484 = -580282079;    double LlBgKqVJqsQCQcOUHWUEb44429479 = -576721920;    double LlBgKqVJqsQCQcOUHWUEb4011017 = -533854741;    double LlBgKqVJqsQCQcOUHWUEb68325707 = 23340431;    double LlBgKqVJqsQCQcOUHWUEb58172747 = -378597800;    double LlBgKqVJqsQCQcOUHWUEb57805114 = -418636778;    double LlBgKqVJqsQCQcOUHWUEb14470086 = -895013500;    double LlBgKqVJqsQCQcOUHWUEb93355216 = -621709647;    double LlBgKqVJqsQCQcOUHWUEb88925002 = -980506026;    double LlBgKqVJqsQCQcOUHWUEb62142363 = 10836580;    double LlBgKqVJqsQCQcOUHWUEb30297397 = -928129566;    double LlBgKqVJqsQCQcOUHWUEb78063844 = 55769562;    double LlBgKqVJqsQCQcOUHWUEb17546190 = -96733111;    double LlBgKqVJqsQCQcOUHWUEb87866040 = -608799118;    double LlBgKqVJqsQCQcOUHWUEb41324560 = -194765117;    double LlBgKqVJqsQCQcOUHWUEb5018872 = -908035302;    double LlBgKqVJqsQCQcOUHWUEb16283579 = -977208003;    double LlBgKqVJqsQCQcOUHWUEb34534020 = -359284227;    double LlBgKqVJqsQCQcOUHWUEb60089662 = -371516358;    double LlBgKqVJqsQCQcOUHWUEb4042476 = -862213570;    double LlBgKqVJqsQCQcOUHWUEb52151139 = -776575930;    double LlBgKqVJqsQCQcOUHWUEb35790292 = -391452004;    double LlBgKqVJqsQCQcOUHWUEb84680990 = -127280940;    double LlBgKqVJqsQCQcOUHWUEb76897819 = -58738260;    double LlBgKqVJqsQCQcOUHWUEb96787619 = -195302859;    double LlBgKqVJqsQCQcOUHWUEb83164978 = -913920328;    double LlBgKqVJqsQCQcOUHWUEb13632039 = -4968172;    double LlBgKqVJqsQCQcOUHWUEb91397106 = -988880416;    double LlBgKqVJqsQCQcOUHWUEb42232493 = -24225069;    double LlBgKqVJqsQCQcOUHWUEb51121396 = 7936883;    double LlBgKqVJqsQCQcOUHWUEb19714556 = -588275914;    double LlBgKqVJqsQCQcOUHWUEb73343924 = 34509807;    double LlBgKqVJqsQCQcOUHWUEb55406770 = -61857404;    double LlBgKqVJqsQCQcOUHWUEb74665633 = -229431259;    double LlBgKqVJqsQCQcOUHWUEb52423492 = 76000157;    double LlBgKqVJqsQCQcOUHWUEb63959381 = -303427515;    double LlBgKqVJqsQCQcOUHWUEb12153322 = 27793867;    double LlBgKqVJqsQCQcOUHWUEb98682113 = -692329659;    double LlBgKqVJqsQCQcOUHWUEb54265458 = -599768019;    double LlBgKqVJqsQCQcOUHWUEb57777962 = -694007201;    double LlBgKqVJqsQCQcOUHWUEb17742415 = -776067061;    double LlBgKqVJqsQCQcOUHWUEb32922200 = -975226231;    double LlBgKqVJqsQCQcOUHWUEb39773776 = -278814155;    double LlBgKqVJqsQCQcOUHWUEb20911553 = -211869524;    double LlBgKqVJqsQCQcOUHWUEb31899794 = -523264138;    double LlBgKqVJqsQCQcOUHWUEb60813249 = -909449738;    double LlBgKqVJqsQCQcOUHWUEb81360381 = -967825518;    double LlBgKqVJqsQCQcOUHWUEb77819390 = 7383404;    double LlBgKqVJqsQCQcOUHWUEb70396127 = -933400292;    double LlBgKqVJqsQCQcOUHWUEb49356659 = -942310937;    double LlBgKqVJqsQCQcOUHWUEb70452667 = 18571789;    double LlBgKqVJqsQCQcOUHWUEb89812788 = -740921532;    double LlBgKqVJqsQCQcOUHWUEb36701732 = -506811774;    double LlBgKqVJqsQCQcOUHWUEb88072240 = -274465218;    double LlBgKqVJqsQCQcOUHWUEb11123174 = -399014532;    double LlBgKqVJqsQCQcOUHWUEb2550192 = -88830075;    double LlBgKqVJqsQCQcOUHWUEb59748488 = -349440980;    double LlBgKqVJqsQCQcOUHWUEb27113198 = -375116482;    double LlBgKqVJqsQCQcOUHWUEb71538087 = -781356710;    double LlBgKqVJqsQCQcOUHWUEb75007768 = -464677473;    double LlBgKqVJqsQCQcOUHWUEb44173075 = -313668607;    double LlBgKqVJqsQCQcOUHWUEb23072979 = -906133085;    double LlBgKqVJqsQCQcOUHWUEb51122723 = -584160335;     LlBgKqVJqsQCQcOUHWUEb75672738 = LlBgKqVJqsQCQcOUHWUEb36254975;     LlBgKqVJqsQCQcOUHWUEb36254975 = LlBgKqVJqsQCQcOUHWUEb78428555;     LlBgKqVJqsQCQcOUHWUEb78428555 = LlBgKqVJqsQCQcOUHWUEb64489070;     LlBgKqVJqsQCQcOUHWUEb64489070 = LlBgKqVJqsQCQcOUHWUEb67734906;     LlBgKqVJqsQCQcOUHWUEb67734906 = LlBgKqVJqsQCQcOUHWUEb39095010;     LlBgKqVJqsQCQcOUHWUEb39095010 = LlBgKqVJqsQCQcOUHWUEb35664713;     LlBgKqVJqsQCQcOUHWUEb35664713 = LlBgKqVJqsQCQcOUHWUEb82071549;     LlBgKqVJqsQCQcOUHWUEb82071549 = LlBgKqVJqsQCQcOUHWUEb79317469;     LlBgKqVJqsQCQcOUHWUEb79317469 = LlBgKqVJqsQCQcOUHWUEb3117794;     LlBgKqVJqsQCQcOUHWUEb3117794 = LlBgKqVJqsQCQcOUHWUEb96608023;     LlBgKqVJqsQCQcOUHWUEb96608023 = LlBgKqVJqsQCQcOUHWUEb37857642;     LlBgKqVJqsQCQcOUHWUEb37857642 = LlBgKqVJqsQCQcOUHWUEb44061365;     LlBgKqVJqsQCQcOUHWUEb44061365 = LlBgKqVJqsQCQcOUHWUEb70135318;     LlBgKqVJqsQCQcOUHWUEb70135318 = LlBgKqVJqsQCQcOUHWUEb47521485;     LlBgKqVJqsQCQcOUHWUEb47521485 = LlBgKqVJqsQCQcOUHWUEb37939946;     LlBgKqVJqsQCQcOUHWUEb37939946 = LlBgKqVJqsQCQcOUHWUEb90763975;     LlBgKqVJqsQCQcOUHWUEb90763975 = LlBgKqVJqsQCQcOUHWUEb8388861;     LlBgKqVJqsQCQcOUHWUEb8388861 = LlBgKqVJqsQCQcOUHWUEb16164339;     LlBgKqVJqsQCQcOUHWUEb16164339 = LlBgKqVJqsQCQcOUHWUEb67007821;     LlBgKqVJqsQCQcOUHWUEb67007821 = LlBgKqVJqsQCQcOUHWUEb12438205;     LlBgKqVJqsQCQcOUHWUEb12438205 = LlBgKqVJqsQCQcOUHWUEb15583077;     LlBgKqVJqsQCQcOUHWUEb15583077 = LlBgKqVJqsQCQcOUHWUEb32212500;     LlBgKqVJqsQCQcOUHWUEb32212500 = LlBgKqVJqsQCQcOUHWUEb26277417;     LlBgKqVJqsQCQcOUHWUEb26277417 = LlBgKqVJqsQCQcOUHWUEb28698778;     LlBgKqVJqsQCQcOUHWUEb28698778 = LlBgKqVJqsQCQcOUHWUEb83053915;     LlBgKqVJqsQCQcOUHWUEb83053915 = LlBgKqVJqsQCQcOUHWUEb62197191;     LlBgKqVJqsQCQcOUHWUEb62197191 = LlBgKqVJqsQCQcOUHWUEb38877093;     LlBgKqVJqsQCQcOUHWUEb38877093 = LlBgKqVJqsQCQcOUHWUEb98906571;     LlBgKqVJqsQCQcOUHWUEb98906571 = LlBgKqVJqsQCQcOUHWUEb65685431;     LlBgKqVJqsQCQcOUHWUEb65685431 = LlBgKqVJqsQCQcOUHWUEb11720688;     LlBgKqVJqsQCQcOUHWUEb11720688 = LlBgKqVJqsQCQcOUHWUEb54375530;     LlBgKqVJqsQCQcOUHWUEb54375530 = LlBgKqVJqsQCQcOUHWUEb86736245;     LlBgKqVJqsQCQcOUHWUEb86736245 = LlBgKqVJqsQCQcOUHWUEb24346809;     LlBgKqVJqsQCQcOUHWUEb24346809 = LlBgKqVJqsQCQcOUHWUEb96791393;     LlBgKqVJqsQCQcOUHWUEb96791393 = LlBgKqVJqsQCQcOUHWUEb92114715;     LlBgKqVJqsQCQcOUHWUEb92114715 = LlBgKqVJqsQCQcOUHWUEb63274312;     LlBgKqVJqsQCQcOUHWUEb63274312 = LlBgKqVJqsQCQcOUHWUEb38340484;     LlBgKqVJqsQCQcOUHWUEb38340484 = LlBgKqVJqsQCQcOUHWUEb44429479;     LlBgKqVJqsQCQcOUHWUEb44429479 = LlBgKqVJqsQCQcOUHWUEb4011017;     LlBgKqVJqsQCQcOUHWUEb4011017 = LlBgKqVJqsQCQcOUHWUEb68325707;     LlBgKqVJqsQCQcOUHWUEb68325707 = LlBgKqVJqsQCQcOUHWUEb58172747;     LlBgKqVJqsQCQcOUHWUEb58172747 = LlBgKqVJqsQCQcOUHWUEb57805114;     LlBgKqVJqsQCQcOUHWUEb57805114 = LlBgKqVJqsQCQcOUHWUEb14470086;     LlBgKqVJqsQCQcOUHWUEb14470086 = LlBgKqVJqsQCQcOUHWUEb93355216;     LlBgKqVJqsQCQcOUHWUEb93355216 = LlBgKqVJqsQCQcOUHWUEb88925002;     LlBgKqVJqsQCQcOUHWUEb88925002 = LlBgKqVJqsQCQcOUHWUEb62142363;     LlBgKqVJqsQCQcOUHWUEb62142363 = LlBgKqVJqsQCQcOUHWUEb30297397;     LlBgKqVJqsQCQcOUHWUEb30297397 = LlBgKqVJqsQCQcOUHWUEb78063844;     LlBgKqVJqsQCQcOUHWUEb78063844 = LlBgKqVJqsQCQcOUHWUEb17546190;     LlBgKqVJqsQCQcOUHWUEb17546190 = LlBgKqVJqsQCQcOUHWUEb87866040;     LlBgKqVJqsQCQcOUHWUEb87866040 = LlBgKqVJqsQCQcOUHWUEb41324560;     LlBgKqVJqsQCQcOUHWUEb41324560 = LlBgKqVJqsQCQcOUHWUEb5018872;     LlBgKqVJqsQCQcOUHWUEb5018872 = LlBgKqVJqsQCQcOUHWUEb16283579;     LlBgKqVJqsQCQcOUHWUEb16283579 = LlBgKqVJqsQCQcOUHWUEb34534020;     LlBgKqVJqsQCQcOUHWUEb34534020 = LlBgKqVJqsQCQcOUHWUEb60089662;     LlBgKqVJqsQCQcOUHWUEb60089662 = LlBgKqVJqsQCQcOUHWUEb4042476;     LlBgKqVJqsQCQcOUHWUEb4042476 = LlBgKqVJqsQCQcOUHWUEb52151139;     LlBgKqVJqsQCQcOUHWUEb52151139 = LlBgKqVJqsQCQcOUHWUEb35790292;     LlBgKqVJqsQCQcOUHWUEb35790292 = LlBgKqVJqsQCQcOUHWUEb84680990;     LlBgKqVJqsQCQcOUHWUEb84680990 = LlBgKqVJqsQCQcOUHWUEb76897819;     LlBgKqVJqsQCQcOUHWUEb76897819 = LlBgKqVJqsQCQcOUHWUEb96787619;     LlBgKqVJqsQCQcOUHWUEb96787619 = LlBgKqVJqsQCQcOUHWUEb83164978;     LlBgKqVJqsQCQcOUHWUEb83164978 = LlBgKqVJqsQCQcOUHWUEb13632039;     LlBgKqVJqsQCQcOUHWUEb13632039 = LlBgKqVJqsQCQcOUHWUEb91397106;     LlBgKqVJqsQCQcOUHWUEb91397106 = LlBgKqVJqsQCQcOUHWUEb42232493;     LlBgKqVJqsQCQcOUHWUEb42232493 = LlBgKqVJqsQCQcOUHWUEb51121396;     LlBgKqVJqsQCQcOUHWUEb51121396 = LlBgKqVJqsQCQcOUHWUEb19714556;     LlBgKqVJqsQCQcOUHWUEb19714556 = LlBgKqVJqsQCQcOUHWUEb73343924;     LlBgKqVJqsQCQcOUHWUEb73343924 = LlBgKqVJqsQCQcOUHWUEb55406770;     LlBgKqVJqsQCQcOUHWUEb55406770 = LlBgKqVJqsQCQcOUHWUEb74665633;     LlBgKqVJqsQCQcOUHWUEb74665633 = LlBgKqVJqsQCQcOUHWUEb52423492;     LlBgKqVJqsQCQcOUHWUEb52423492 = LlBgKqVJqsQCQcOUHWUEb63959381;     LlBgKqVJqsQCQcOUHWUEb63959381 = LlBgKqVJqsQCQcOUHWUEb12153322;     LlBgKqVJqsQCQcOUHWUEb12153322 = LlBgKqVJqsQCQcOUHWUEb98682113;     LlBgKqVJqsQCQcOUHWUEb98682113 = LlBgKqVJqsQCQcOUHWUEb54265458;     LlBgKqVJqsQCQcOUHWUEb54265458 = LlBgKqVJqsQCQcOUHWUEb57777962;     LlBgKqVJqsQCQcOUHWUEb57777962 = LlBgKqVJqsQCQcOUHWUEb17742415;     LlBgKqVJqsQCQcOUHWUEb17742415 = LlBgKqVJqsQCQcOUHWUEb32922200;     LlBgKqVJqsQCQcOUHWUEb32922200 = LlBgKqVJqsQCQcOUHWUEb39773776;     LlBgKqVJqsQCQcOUHWUEb39773776 = LlBgKqVJqsQCQcOUHWUEb20911553;     LlBgKqVJqsQCQcOUHWUEb20911553 = LlBgKqVJqsQCQcOUHWUEb31899794;     LlBgKqVJqsQCQcOUHWUEb31899794 = LlBgKqVJqsQCQcOUHWUEb60813249;     LlBgKqVJqsQCQcOUHWUEb60813249 = LlBgKqVJqsQCQcOUHWUEb81360381;     LlBgKqVJqsQCQcOUHWUEb81360381 = LlBgKqVJqsQCQcOUHWUEb77819390;     LlBgKqVJqsQCQcOUHWUEb77819390 = LlBgKqVJqsQCQcOUHWUEb70396127;     LlBgKqVJqsQCQcOUHWUEb70396127 = LlBgKqVJqsQCQcOUHWUEb49356659;     LlBgKqVJqsQCQcOUHWUEb49356659 = LlBgKqVJqsQCQcOUHWUEb70452667;     LlBgKqVJqsQCQcOUHWUEb70452667 = LlBgKqVJqsQCQcOUHWUEb89812788;     LlBgKqVJqsQCQcOUHWUEb89812788 = LlBgKqVJqsQCQcOUHWUEb36701732;     LlBgKqVJqsQCQcOUHWUEb36701732 = LlBgKqVJqsQCQcOUHWUEb88072240;     LlBgKqVJqsQCQcOUHWUEb88072240 = LlBgKqVJqsQCQcOUHWUEb11123174;     LlBgKqVJqsQCQcOUHWUEb11123174 = LlBgKqVJqsQCQcOUHWUEb2550192;     LlBgKqVJqsQCQcOUHWUEb2550192 = LlBgKqVJqsQCQcOUHWUEb59748488;     LlBgKqVJqsQCQcOUHWUEb59748488 = LlBgKqVJqsQCQcOUHWUEb27113198;     LlBgKqVJqsQCQcOUHWUEb27113198 = LlBgKqVJqsQCQcOUHWUEb71538087;     LlBgKqVJqsQCQcOUHWUEb71538087 = LlBgKqVJqsQCQcOUHWUEb75007768;     LlBgKqVJqsQCQcOUHWUEb75007768 = LlBgKqVJqsQCQcOUHWUEb44173075;     LlBgKqVJqsQCQcOUHWUEb44173075 = LlBgKqVJqsQCQcOUHWUEb23072979;     LlBgKqVJqsQCQcOUHWUEb23072979 = LlBgKqVJqsQCQcOUHWUEb51122723;     LlBgKqVJqsQCQcOUHWUEb51122723 = LlBgKqVJqsQCQcOUHWUEb75672738;}
// Junk Finished
