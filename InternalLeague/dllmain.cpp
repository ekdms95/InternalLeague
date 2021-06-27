#include "Offsets.h"
#include "VEHHook.h"
#include "Engine.h"
#include "Bypass.h"
#include "imgui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"
#include <mutex>
#include "Render.h"

#pragma warning(disable : 4740)

bool Initializion = 0;
bool MenuOpen = 0;
HWND Hwnd = nullptr;
WNDPROC Wndproc = nullptr;

void __fastcall hk_OnProcessSpell(void* SpellBook, void* edx) {
    Engine::PrintChat(es("A spell has been created"));
}

typedef void(__thiscall* tOnProcessSpell)(DWORD* SpellBook, int a2);

DWORD IdontKnow;
DWORD SendChat;
DWORD OnProcessSpell;


/*
//events
#define oOnprocessSpell 0x4F1B90 // 		E8 ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 80 BE ?? ?? ?? ?? ?? D8 // sub_[offset]
#define oOnCreateObject 0x280EA0 // 		E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 74 ?? 32 C9 EB // E8 ? ? ? ? 0F B6 47 35 // sub_[offset]
#define oOnDeleteObject 0x276B00 // 		E8 ? ? ? ? 8B 4E 20 85 C9 74 0F // E8 ? ? ? ? 83 C7 04 3B FB 75 E2 // sub_[offset]
#define oOnNewPath 0x281180 // 		83 EC 18 56 8B 74 24 20 8B CE 57 // fn // E8 ? ? ? ? 8B 74 24 4C 83 C4 30 // sub_[offset]
#define oOnFinishCast 0x4E3F30 // 83 EC 18 53 55 56 57 8B F9 8B // fn
#define oOnStopCast 0x4F1E50 // 8B 44 24 10 56 8B F1 3B // fn
*/

IDirect3DDevice9* LeagueDevice;
typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
HRESULT WINAPI HookedPresent(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
LRESULT WINAPI WndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

bool HookInit = false;

#define DO_ONCE(todo) do { \
   static std::once_flag _flag ;\
   std::call_once(_flag, todo); \
} while (false)

LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto& io = ImGui::GetIO();

	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		io.MouseDown[0] = true;
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		return true;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONDBLCLK:
		if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON1) == MK_XBUTTON1)
			io.MouseDown[3] = true;
		else if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON2) == MK_XBUTTON2)
			io.MouseDown[4] = true;
		return true;
	case WM_XBUTTONUP:
		if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON1) == MK_XBUTTON1)
			io.MouseDown[3] = false;
		else if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON2) == MK_XBUTTON2)
			io.MouseDown[4] = false;
		return true;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;
	}

	return 0;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
	switch (u_msg)
	{
	case WM_KEYDOWN: {
		switch (w_param) {
		case VK_INSERT: {
			MenuOpen = !MenuOpen;
			break;
		}
		default:
			break;
		};
		break;
	}
	}

	if (MenuOpen && ImGui_ImplDX9_WndProcHandler(hwnd, u_msg, w_param, l_param))
	{
		return true;
	}

	return CallWindowProcA(Wndproc, hwnd, u_msg, w_param, l_param);
}

HRESULT WINAPI HookedPresent(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion)
{
	MessageBoxA(NULL, es("1"), es("1"), NULL);


	if (HookInit == false) {
		Global::BaseAddress = (DWORD)GetModuleHandle(NULL);

		Hwnd = FindWindowA(nullptr, es("League of Legends (TM) Client"));
		Wndproc = WNDPROC(SetWindowLongA(Hwnd, GWL_WNDPROC, LONG_PTR(WndProc)));

		//ObjManager = (CObjectManager*)(*(DWORD*)(g_BaseAddr + offsets::global::oObjManager));
		//
		//Functions.PrintChat = (Typedefs::fnPrintChat)(g_BaseAddr + offsets::functions::oPrintChat);
		//Functions.IsTargetable = (Typedefs::fnIsTargetable)(g_BaseAddr + offsets::functions::oIsTargetable);
		//Functions.IsAlive = (Typedefs::fnIsAlive)(g_BaseAddr + offsets::functions::oIsAlive);
		//
		//Functions.IsMinion = (Typedefs::fnIsMinion)(g_BaseAddr + offsets::functions::oIsMinion);
		//Functions.IsTurret = (Typedefs::fnIsTurret)(g_BaseAddr + offsets::functions::oIsTurret);
		//Functions.IsHero = (Typedefs::fnIsHero)(g_BaseAddr + offsets::functions::oIsHero);
		//Functions.IsMissile = (Typedefs::fnIsMissile)(g_BaseAddr + offsets::functions::oIsMissile);
		//Functions.IsNexus = (Typedefs::fnIsNexus)(g_BaseAddr + offsets::functions::oIsNexus);
		//Functions.IsInhibitor = (Typedefs::fnIsInhibitor)(g_BaseAddr + offsets::functions::oIsInhib);
		//Functions.IsTroyEnt = (Typedefs::fnIsTroyEnt)(g_BaseAddr + offsets::functions::oIsTroy);
		//
		//Functions.CastSpell = (Typedefs::fnCastSpell)(g_BaseAddr + offsets::functions::oCastSpell);
		//Functions.IssueOrder = (Typedefs::fnIssueOrder)(g_BaseAddr + offsets::functions::oIssueOrder);
		//Functions.DrawCircle = (Typedefs::fnDrawCircle)spoofedDrawCircle;
		//Functions.WorldToScreen = (Typedefs::fnWorldToScreen)(g_BaseAddr + (DWORD)offsets::global::oWorldToScreen);
		//
		//Functions.GetAttackCastDelay = (Typedefs::fnGetAttackCastDelay)(g_BaseAddr + offsets::functions::oGetAttackCastDelay);
		//Functions.GetAttackDelay = (Typedefs::fnGetAttackDelay)(g_BaseAddr + offsets::functions::oGetAttackDelay);
		//
		//Functions.GetPing = (Typedefs::fnGetPing)(g_BaseAddr + offsets::functions::oGetPing);
		//
		//Original_Reset = (Prototype_Reset)DetourFunc((PBYTE)GetDeviceAddress(16), (PBYTE)Hooked_Reset, 5);

		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX9_Shutdown();

		ImGui::DestroyContext(ImGui::GetCurrentContext());

		//championScript = ScriptUtils::GetScriptByChampionName(Engine::GetLocalObject()->GetChampionName());

		//Engine::PrintChat("[ SpaghettiHack ]");
		//Engine::PrintChat("[ Credits: Kmsmym ]");
		//Engine::PrintChat("[ Unknowncheats.me ]");

		HookInit = true;
	}

	LeagueDevice = Device;
	DO_ONCE([&]()
		{
			R->Init(Device);
		});

	ImGui::CreateContext();
	R->BeginDraw();

	if (ImGui_ImplWin32_Init(Hwnd))
	{
		if (ImGui_ImplDX9_Init(Device))
		{
			ImGui::Begin(es("Nameless"), &MenuOpen, ImGuiWindowFlags_NoSavedSettings);
			{
				ImGui::BeginChild(es("##child"), ImVec2(450.0f, 450.0f), false, ImGuiWindowFlags_NoSavedSettings);
				{
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}
	}

	//auto localObj = Engine::GetLocalObject();
	//
	////evader
	//if (g_auto_evade == true) {
	//	if (localObj && localObj->IsAlive() && Engine::IsLeagueInForeground()) {
	//		evader.drawEvent();
	//	}
	//}
	//
	////orbwalker
	//if (g_orbwalker == true) {
	//	if (localObj && localObj->IsAlive()) {
	//		if (IsKeyDown(VK_SPACE) && Engine::IsLeagueInForeground()) {
	//			orbWalker.drawEvent();
	//		}
	//	}
	//}

	//me Range
	//if (g_range == true) {
	//	if (localObj && localObj->IsAlive()) { 
	//		auto color = createRGB(0, 255, 0);
	//		auto localPos = Engine::GetLocalObject()->GetPos();
	//		Functions.DrawCircle(&localPos, Engine::GetLocalObject()->GetTrueAttackRange(), &color, 0, 0.0f, 0, 0.5f);
	//	}
	//}

	/*for (auto pObject : CycleManager::GetObjects())
	{
		//draw range all hero
		//if (g_2range_objmanager == true) {
		//	if (pObject->IsHero() && pObject->IsAlive() && pObject->IsTargetable() && pObject->IsVisible())
		//	{
		//		auto color = createRGB(255, 0, 0);
		//		auto objPos = pObject->GetPos();
		//		Functions.DrawCircle(&objPos, pObject->GetAttackRange(), &color, 0, 0.0f, 0, 0.5f);
		//	}
		//}

#ifndef NO_IMGUI
		//champion info demonstration
		if (g_champ_info == true) {
			if (pObject->IsHero() && pObject->IsAlive() && pObject->IsTargetable() && pObject->IsVisible())
			{
				Vector obj_pos = pObject->GetPos();
				Vector objpos_w2s;
				Functions.WorldToScreen(&obj_pos, &objpos_w2s);

				auto objSpellBook = pObject->GetSpellBook();

				auto spellQ = objSpellBook->GetSpellByID(static_cast<int>(ESpellSlot::Q));
				auto spellW = objSpellBook->GetSpellByID(static_cast<int>(ESpellSlot::W));
				auto spellE = objSpellBook->GetSpellByID(static_cast<int>(ESpellSlot::E));
				auto spellR = objSpellBook->GetSpellByID(static_cast<int>(ESpellSlot::R));

				//render.draw_text(objpos_w2s.X, objpos_w2s.Y + 15, pObject->GetChampionName(), true, ImColor(255, 0, 0, 255));
				render.draw_text(objpos_w2s.X, objpos_w2s.Y + 30, "[Q]", true, spellQ->IsSpellReady() ? ImColor(0, 255, 0, 255) : ImColor(255, 255, 255, 255));
				render.draw_text(objpos_w2s.X + 20, objpos_w2s.Y + 30, "[W]", true, spellW->IsSpellReady() ? ImColor(0, 255, 0, 255) : ImColor(255, 255, 255, 255));
				render.draw_text(objpos_w2s.X + 40, objpos_w2s.Y + 30, "[E]", true, spellE->IsSpellReady() ? ImColor(0, 255, 0, 255) : ImColor(255, 255, 255, 255));
				render.draw_text(objpos_w2s.X + 60, objpos_w2s.Y + 30, "[R]", true, spellR->IsSpellReady() ? ImColor(0, 255, 0, 255) : ImColor(255, 255, 255, 255));
			}
		}
#endif

		//turret range
		//if (g_turret_range == true) {
		//	if (pObject->IsTurret() && pObject->IsEnemyTo(localObj)) {
		//		auto turretRange = pObject->GetStatOwner()->GetStatTotal(EStatType::AttackRange, EStatOutputType::BASE)
		//			+ pObject->GetStatOwner()->GetStatTotal(EStatType::AttackRange, EStatOutputType::BONUS)
		//			+ pObject->GetBoundingRadius();
		//		if (pObject->GetDistanceToMe() < (turretRange + 300.f)) {
		//			auto color = createRGB(255, 255, 255);
		//			auto turretPos = pObject->GetPos();
		//			Functions.DrawCircle(&turretPos, turretRange, &color, 0, 0.0f, 0, 0.5f);
		//		}
		//	}
		//}
	}*/

	//return ((Prototype_Present)(presentHvpp.GetOriginalAddress()))(Device, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

void __declspec(naked) HookSpell()
{
    ////Engine::PrintChat(es("[Nameless.cc] : HookSpell"));
    //__asm
    //{
    //    pushad
    //    mov eax, [esp + 0x24]
    //    mov eax, [eax]
    //    mov eax, [eax + 0x44]
    //    mov eax, [eax + 0x6C]
    //    mov ecx, [IdontKnow]
    //    mov ecx, [ecx]
    //    push 0
    //    push eax
    //    call SendChat  //sendchat
    //    popad
    //    push esi
    //    mov esi, [esp + 8]
    //    jmp OnProcessSpell  //OnProcessSpell + 5
    //    //push esi
    //    //mov esi, [esp + 8]
    //    //jmp OnProcessSpell
    //}
    __asm
    {
        pushad
        mov esi, [esp + 0x24]
        mov eax, [esi + 0xE1]
        test eax, eax
        jne label1
        mov eax, [esi + 0xE2]
        test eax, eax
        jne label1
        mov eax, [esp + 0x24]
        mov eax, [eax]
        mov eax, [eax + 0x44]
        mov eax, [eax + 0x6C]
        mov ecx, [IdontKnow]
        mov ecx, [ecx]
        push 0
        push eax
        //call WriteSHARE
        call SendChat  //sendchat
        label1:
        popad
        push esi
        mov esi, [esp + 8]
        jmp OnProcessSpell  //OnProcessSpell + 5
    }
}

DWORD FindD3D9VTable(DWORD Len)
{
    DWORD dwObjBase = 0;
    dwObjBase = (DWORD)LoadLibrary(TEXT("d3d9.dll"));
    while (dwObjBase++ < dwObjBase + Len)
    {
        if ((*(WORD*)(dwObjBase + 0x00)) == 0x06C7
            && (*(WORD*)(dwObjBase + 0x06)) == 0x8689
            && (*(WORD*)(dwObjBase + 0x0C)) == 0x8689
            ) {
            dwObjBase += 2; break;
        }
    }
    return(dwObjBase);
}

DWORD GetD3D9VTableFunction(int VTableIndex)
{
    PDWORD VTable;
    *(DWORD*)&VTable = *(DWORD*)FindD3D9VTable(0x128000);
    return VTable[VTableIndex];
}

void WhatTheFuckRiot()
{
    Global::BaseAddress = (DWORD)GetModuleHandle(NULL);

    IdontKnow = Global::BaseAddress + 0x2FB5810;
    SendChat = Global::BaseAddress + Offsets::SendChat; // 0x5E4B40;
    OnProcessSpell = Global::BaseAddress + Offsets::OnProcessSpell + 5; // 0x4F8CE5; 
    //Functions.PrintChat = (Typedefs::fnPrintChat)(Global::BaseAddress + Offset->PrintChat);

    //Engine::SendChat(es("/all Successfully Loaded"));
    //Engine::SendChat(es("/all Successfully Loaded"));
	//
    //Engine::PrintChat(es("<font color='#FFFFFF'>[Nameless.cc] : </font><font color='#FF0000'>Successfully Loaded</font>"));
    //Engine::PrintChat(es("<font color='#FFFFFF'>[Nameless.cc] : </font><font color='#FF5E00'>Initalization</font>"));
    //Engine::PrintChat(es("<font color='#FFFFFF'>[Nameless.cc] : </font><font color='#FFE400'>Internal for 11.10</font>"));
    //Engine::PrintChat(es("<font color='#FFFFFF'>[Nameless.cc] : </font><font color='#1DDB16'>Fully Support Bypass In KR</font>"));
    //Engine::PrintChat(es("<font color='#FFFFFF'>[Nameless.cc] : </font><font color='#0100FF'>This is BETA now. Have Fun :)</font>"));
    //Engine::PrintChat(es("<font color='#FFFFFF'>[Nameless.cc] : </font><font color='#5F00FF'>Good Luck to your Rank</font>"));
	//MessageBoxA(NULL, es("1"), es("1"), NULL);

	if (!NamelessHook::Hook((uintptr_t)GetD3D9VTableFunction(17), (uintptr_t)HookedPresent))
	{
		MessageBoxA(NULL, es("1"), es("1"), NULL);
	}

    //if (!NamelessHook::Hook((uintptr_t)(Global::BaseAddress + Offsets::OnProcessSpell), (uintptr_t)HookSpell)) // 0x4F8CE0
    //{
    //    Engine::PrintChat("[Nameless.cc] : Failed To Hook");
    //}
    //NamelessHook::Hook((uintptr_t)(0x5FD214), (uintptr_t)TestHook);
    //NamelessHook::Unhook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        Global::Inst = hModule;
        Global::BaseAddress = (DWORD)GetModuleHandleA(NULL);
        WhatTheFuckRiot();
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {}

    return TRUE;
}

