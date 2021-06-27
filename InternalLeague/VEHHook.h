#pragma once
#include "pch.h"

#ifdef _WIN64
#define XIP Rip
#else
#define XIP Eip
#endif

class NamelessHook {
public:
	static bool Hook(uintptr_t og_fun, uintptr_t hk_fun);
	static bool Unhook();

private:
	static uintptr_t og_fun;
	static uintptr_t hk_fun;
	static PVOID VEH_Handle;
	static DWORD oldProtection;

	static bool AreInSamePage(const uint8_t* Addr1, const uint8_t* Addr2);
	static LONG WINAPI LeoHandler(EXCEPTION_POINTERS* pExceptionInfo);
};