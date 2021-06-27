#pragma once
#include "pch.h"

class Render
{
public:

	HWND Hwnd = nullptr;

	void Init(LPDIRECT3DDEVICE9 m_Device);
	void Free();

	void BeginDraw();
	void EndDraw();

	ImFont* m_default = nullptr;

	void ImGuiDrawText(ImVec2 Pos, const char* Text, float size, ImVec4 Color, bool Outlined = true);
	void ImGuiDrawLine(ImVec2 Start, ImVec2 End, ImVec4 Color, float Thickness = 1.0f);
	void ImGuiDrawRect(float x, float y, float w, float h, ImVec4 Color, float Thickness = 1.0f);
	void DrawCircle3D(Vector3 vPos, float flPoints, float flRadius, ImVec4 clrColor1, ImVec4 clrColor2);

	float GetWidth() { return Width; }
	float GetHeight() { return Height; }

	LPDIRECT3DDEVICE9 GetDevice() { return Device; }

private:
	LPDIRECT3DDEVICE9 Device = nullptr;
	float Width = 0.0f;
	float Height = 0.0f;

	float fastsin(float x);
	float fastcos(float x);

};

extern Render* R;