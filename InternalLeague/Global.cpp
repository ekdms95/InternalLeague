#include "Global.h"

Global* G = new Global();

HINSTANCE Global::Inst = NULL;
DWORD Global::BaseAddress = NULL;
HWND Global::GameWnd = NULL;
DWORD Global::ModuleSize = NULL;
D3DXMATRIX Global::ViewMatrix;
D3DXMATRIX Global::ProjViewMatrix;