#include "VEHHook.h"
#include "Engine.h"
#include "Bypass.h"

void* NtdllAddress = GetProcAddress(GetModuleHandleA(es("ntdll.dll")), es("RtlInterlockedCompareExchange64"));
void* KernelAddress = GetProcAddress(GetModuleHandleA(es("kernel.dll")), es("GetSystemLeapSecondInformation"));
const DWORD ntdll170 = (DWORD)NtdllAddress + 0x170;
const DWORD KernelEC85 = (DWORD)KernelAddress + 0xEC85;

/*
static void SendChat(const char* Message) {
	typedef void(__thiscall* tSendChat)(DWORD MenuGUI, const char* Message, int Color);
	typedef void(__thiscall* tSendChat)(DWORD ChatClient, BYTE* Message, char var1);
	tSendChat fnSendChat = (tSendChat)(baseAddr + 0x5FA8D0);
	fnSendChat(*(DWORD*)(baseAddr + 0x34FF610), Message, 1);
}
*/

void __declspec(naked) CopyZwVirtualProtect()
{
	__asm
	{
		mov eax, 0x50
		mov edx, ntdll170 // ntdll.RtlInterlockedCompareExchange64 + 170
		call edx
		ret 0x14
	}
}

BOOL __declspec(naked) __cdecl CopyVirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
	__asm
	{
		mov edi, edi
		push ebp
		mov ebp, esp
		push ecx
		push ecx
		mov eax, [ebp + 0xC]
		push esi
		push[ebp + 0x14]
		mov[ebp - 4], eax
		push[ebp + 0x10]
		mov eax, [ebp + 8]
		mov[ebp - 8], eax
		lea eax, [ebp - 4]
		push eax
		lea eax, [ebp - 8]
		push eax
		push - 1
		call CopyZwVirtualProtect
		mov esi, eax
		test esi, esi
		js KK //KERNELBASE.GetSystemLeapSecondInformation + EC85 
		xor eax, eax
		inc eax
		pop esi
		leave
		ret 0x10

		KK:
		jmp KernelEC85
	}
}

uintptr_t NamelessHook::og_fun = 0;
uintptr_t NamelessHook::hk_fun = 0;
PVOID NamelessHook::VEH_Handle = nullptr;
DWORD NamelessHook::oldProtection = 0;

bool NamelessHook::Hook(uintptr_t original_fun, uintptr_t hooked_fun)
{
	//Engine::PrintChat(es("Hi"));
	NamelessHook::og_fun = original_fun;
	NamelessHook::hk_fun = hooked_fun;

	//We cannot hook two functions in the same page, because we will cause an infinite callback
	if (AreInSamePage((const uint8_t*)og_fun, (const uint8_t*)hk_fun))
	{
		//Engine::PrintChat(es("This is SamePage false"));
		return false;
	}

	//Engine::PrintChat(es("Start to AddVectoredExceptionHandler"));
	//Register the Custom Exception Handler
	VEH_Handle = AddVectoredExceptionHandler(true, (PVECTORED_EXCEPTION_HANDLER)LeoHandler);
	//Engine::PrintChat(es("End to AddVectoredExceptionHandler"));

	//char LeoAddress[64];
	//sprintf_s(LeoAddress, es("LeoHandler : %x"), LeoHandler);
	//MessageBox(NULL, LeoAddress, es("Test"), NULL);

	if (!VEH_Handle)
	{
		//Engine::PrintChat(es("Can't Get VEH_Handle"));
		return true;
	}

	//KernelVirtualProtect((UINT64)(PVOID*)og_fun, 1, (UINT64)(PAGE_EXECUTE_READ | PAGE_GUARD), (UINT64)&oldProtection))
	//Toggle PAGE_GUARD flag on the page

	// 커널 : Controller->VirtualProtectEx((PVOID*)og_fun, 1, DWORD(PAGE_EXECUTE_READ | PAGE_GUARD)
	// 유저모드 : VirtualProtect((PVOID*)og_fun, 1, DWORD(PAGE_EXECUTE_READ | PAGE_GUARD), &oldProtection)
	// 함수복사 : ZwCopyVirtualProtect(og_fun, 1, (PAGE_EXECUTE_READ | PAGE_GUARD), &oldProtection)
	if (VEH_Handle && CopyVirtualProtect((PVOID*)og_fun, 1, (PAGE_EXECUTE_READ | PAGE_GUARD), &oldProtection)) //ZwCopyVirtualProtect(GetModuleHandle, (PVOID*)og_fun, (PSIZE_T)1, PAGE_EXECUTE_READ | PAGE_GUARD, &oldProtection))
	{
		//Engine::PrintChat(es("Can't do VirtualProtect"));
		return true;
	}

	return false;
}

bool NamelessHook::Unhook()
{
	DWORD old = 0;
	if (VEH_Handle && //Make sure we have a valid Handle to the registered VEH
		//Controller->VirtualProtectEx((PVOID*)og_fun, 1, oldProtection, &old);
		//VirtualProtect((LPVOID)og_fun, 1, oldProtection, &old) && //Restore old Flags
		CopyVirtualProtect((LPVOID)og_fun, 1, oldProtection, &old) &&
		RemoveVectoredExceptionHandler(VEH_Handle)) //Remove the VEH
		return true;

	return false;
}

LONG WINAPI NamelessHook::LeoHandler(EXCEPTION_POINTERS* pExceptionInfo)
{
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) //We will catch PAGE_GUARD Violation
	{
		//MessageBox(NULL, "STATUS_GUARD_PAGE_VIOLATION EXCEPTION_CONTINUE_EXECUTION", "hI", NULL);
		//Sleep(10000);
		if (pExceptionInfo->ContextRecord->XIP == (uintptr_t)og_fun) //Make sure we are at the address we want within the page
		{
			pExceptionInfo->ContextRecord->XIP = (uintptr_t)hk_fun; //Modify EIP/RIP to where we want to jump to instead of the original function
		}

		pExceptionInfo->ContextRecord->EFlags |= 0x100; //Will trigger an STATUS_SINGLE_STEP exception right after the next instruction get executed. In short, we come right back into this exception handler 1 instruction later
		
		//Engine::PrintChat(es("STATUS_GUARD_PAGE_VIOLATION EXCEPTION_CONTINUE_EXECUTION"));
		return EXCEPTION_CONTINUE_EXECUTION; //Continue to next instruction
	}

	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) //We will also catch STATUS_SINGLE_STEP, meaning we just had a PAGE_GUARD violation
	{
		DWORD dwOld = 0;
		CopyVirtualProtect((PVOID*)og_fun, 1, (PAGE_EXECUTE_READ | PAGE_GUARD), &dwOld);
		//ZwCopyVirtualProtect(GetModuleHandle, (PVOID*)og_fun, (PSIZE_T)1, (PAGE_EXECUTE_READ | PAGE_GUARD), &dwOld);
		//Controller->VirtualProtectEx((PVOID*)og_fun, 1, (DWORD)(PAGE_EXECUTE_READ | PAGE_GUARD), &dwOld);
		//VirtualProtect((LPVOID)og_fun, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &dwOld); //Reapply the PAGE_GUARD flag because everytime it is triggered, it get removes

		//Engine::PrintChat(es("STATUS_SINGLE_STEP EXCEPTION_CONTINUE_EXECUTION"));
		return EXCEPTION_CONTINUE_EXECUTION; //Continue the next instruction
	}

	//char procID[50];
	//sprintf_s(procID, "%x", pExceptionInfo->ExceptionRecord->ExceptionCode);

	//Engine::PrintChat(procID);
	//Engine::PrintChat(es("EXCEPTION_CONTINUE_SEARCH"));
	return EXCEPTION_CONTINUE_SEARCH; //Keep going down the exception handling list to find the right handler IF it is not PAGE_GUARD nor SINGLE_STEP
}

bool NamelessHook::AreInSamePage(const uint8_t* Addr1, const uint8_t* Addr2)
{
	MEMORY_BASIC_INFORMATION mbi1;
	if (!VirtualQuery(Addr1, &mbi1, sizeof(mbi1))) //Get Page information for Addr1
	{
		//Engine::PrintChat(es("Can't GetPageInformation For Address 1"));
		return true;
	}


	MEMORY_BASIC_INFORMATION mbi2;
	if (!VirtualQuery(Addr2, &mbi2, sizeof(mbi2))) //Get Page information for Addr1
	{
		//Engine::PrintChat(es("Can't GetPageInformation For Address 2"));
		return true;
	}


	if (mbi1.BaseAddress == mbi2.BaseAddress) //See if the two pages start at the same Base Address
	{
		//Engine::PrintChat(es("Check Is SamePage."));
		return true;  //Both addresses are in the same page, abort hooking!
	}


	return false;
}