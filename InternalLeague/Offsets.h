#pragma once
#include "pch.h"

class Offsets
{
public:
	//374B128
	const static uint32_t ChatClientPtr				= 0x2FA89F0; // 8B 0D ? ? ? ? FF 74 24 38 E8 ? ? ? ? 8B 0D ? ? ? ? 
	const static uint32_t PrintChat					= 0x563E90; // 5F 8B 4D FC 5B };
	//D0EBD0
	const static uint32_t SendChat					= 0x5E4B40; // 83 3D ? ? ? ? ? 57 8B F9 74 65 ÈÄ ¶ß´Â ÇÔ¼ö

	const static uint32_t OnProcessSpell			= 0x4EC980;//0x4F8CE0; // E8 ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 80 BE ?? ?? ?? ?? ?? D8

	
};