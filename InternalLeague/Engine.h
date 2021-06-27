#pragma once
#include "pch.h"

class Engine
{
public:

	static void PrintChat(const char* Message);
	static void SendChat(const char* Message);
};

class Typedefs {
public:
	typedef void(__thiscall* tPrintChat)(DWORD ChatClient, const char* Message, int Color);
	typedef void(__thiscall* fnPrintChat)(DWORD ChatClient, const char* Message, int Color);
	typedef void(__thiscall* fnSendChat)(DWORD ChatClient, const char* Message, int Color);

};

class CFunctions {
public:
	Typedefs::fnPrintChat PrintChat;
	Typedefs::fnSendChat SendChat;
};

extern Engine* Eng;

extern CFunctions Functions;