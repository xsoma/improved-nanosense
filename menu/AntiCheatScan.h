#pragma once

#include <Windows.h>


namespace checks
{
	inline auto get_min_sys_addr() -> void*
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		return si.lpMinimumApplicationAddress;
	}

	inline auto is_bad_ptr(void* ptr) -> bool
	{
		static auto min_sys_addr = get_min_sys_addr();
		return (ptr <= min_sys_addr);
	}

	inline auto is_bad_const_ptr(const void* ptr) -> bool
	{
		static auto min_sys_addr = get_min_sys_addr();
		return (ptr <= min_sys_addr);
	}
}