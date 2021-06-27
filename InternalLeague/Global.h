#pragma once
#include "pch.h"
#include "Vectors.h"

class Global
{
public:
	static HINSTANCE Inst;
	static DWORD BaseAddress;
	static HWND GameWnd;
	static DWORD ModuleSize;
	static D3DXMATRIX ViewMatrix;
	static D3DXMATRIX ProjViewMatrix;
};		   

extern Global* G;