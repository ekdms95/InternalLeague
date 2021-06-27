#include "Engine.h"
#include "Offsets.h"

CFunctions Functions;
Engine* Eng = new Engine();

void Engine::PrintChat(const char* Message)
{
	typedef void(__thiscall* tPrintChat)(DWORD ChatClient, const char* Message, int Color);
	tPrintChat fnPrintChat = (tPrintChat)(Global::BaseAddress + Offsets::PrintChat);
	fnPrintChat(*(DWORD*)(Global::BaseAddress + Offsets::ChatClientPtr), Message, 1);
}

void Engine::SendChat(const char* Message)
{
	typedef void(__thiscall* tSendChat)(DWORD ChatClient, const char* Message, int Color);
	tSendChat fnSendChat = (tSendChat)(Global::BaseAddress + Offsets::SendChat);
	fnSendChat(*(DWORD*)(Global::BaseAddress + Offsets::ChatClientPtr), Message, 1);
}