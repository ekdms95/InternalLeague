#include "Render.h"
#include "Vectors.h"

extern ImFont* g_font;

Render* R = new Render();

void Render::Init(LPDIRECT3DDEVICE9 m_Device)
{
	if (!m_Device)
		return;

	if (!Device)
		Device = m_Device;

	ImGui::CreateContext();

	ImGui_ImplWin32_Init(Hwnd);
	ImGui_ImplDX9_Init(m_Device);

	if (!m_default)
		m_default = ImGui::GetIO().Fonts->AddFontDefault();
}

void Render::Free()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

void Render::BeginDraw()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs;

	ImGui::GetStyle().AntiAliasedFill = true;
	ImGui::GetStyle().AntiAliasedLines = true;

	ImGui::Begin(es("##overlay"), nullptr, flags);

	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

	Width = ImGui::GetIO().DisplaySize.x;
	Height = ImGui::GetIO().DisplaySize.y;
}

void Render::EndDraw()
{
	ImGui::GetOverlayDrawList()->PushClipRectFullScreen();

	ImGui::End();

	ImGui::EndFrame();

	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Render::ImGuiDrawText(ImVec2 Pos, const char* Text, float size, ImVec4 Color, bool Outlined)
{
	ImU32 ImColor = ImGui::GetColorU32(Color);
	va_list argptr;
	va_start(argptr, Text);

	char buffer[2048];
	vsprintf_s(buffer, Text, argptr);

	va_end(argptr);

	if (Outlined)
	{
		ImGui::GetForegroundDrawList()->AddText(m_default, size, ImVec2(Pos.x - 1, Pos.y - 1), D3DCOLOR_ARGB(255, 0, 0, 0), buffer);
		ImGui::GetForegroundDrawList()->AddText(m_default, size, ImVec2(Pos.x + 1, Pos.y - 1), D3DCOLOR_ARGB(255, 0, 0, 0), buffer);
		ImGui::GetForegroundDrawList()->AddText(m_default, size, ImVec2(Pos.x - 1, Pos.y + 1), D3DCOLOR_ARGB(255, 0, 0, 0), buffer);
		ImGui::GetForegroundDrawList()->AddText(m_default, size, ImVec2(Pos.x + 1, Pos.y + 1), D3DCOLOR_ARGB(255, 0, 0, 0), buffer);
	}

	ImGui::GetForegroundDrawList()->AddText(m_default, size, Pos, ImColor, buffer);
}

void Render::ImGuiDrawLine(ImVec2 Start, ImVec2 End, ImVec4 Color, float Thickness)
{
	ImU32 ImColor = ImGui::GetColorU32(Color);
	ImGui::GetWindowDrawList()->AddLine(Start, End, ImColor, Thickness);
}

void Render::ImGuiDrawRect(float x, float y, float w, float h, ImVec4 Color, float Thickness)
{
	ImGuiDrawLine(ImVec2(x, y), ImVec2(x + w, y), Color, Thickness);
	ImGuiDrawLine(ImVec2(x, y), ImVec2(x, y + h), Color, Thickness);
	ImGuiDrawLine(ImVec2(x, y + h), ImVec2(x + w, y + h), Color, Thickness);
	ImGuiDrawLine(ImVec2(x + w, y), ImVec2(x + w, y + h), Color, Thickness);
}

float Render::fastcos(float x)
{
	constexpr float tp = 1. / (2. * M_PI);
	x *= tp;
	x -= float(.25) + std::floor(x + float(.25));
	x *= float(16.) * (std::abs(x) - float(.5));
	x += float(.225) * x * (std::abs(x) - float(1.));

	return x;
}

float Render::fastsin(float x)
{
	x = fmod(x + M_PI, M_PI * 2) - M_PI;
	const float B = 4.0f / M_PI;
	const float C = -4.0f / (M_PI * M_PI);

	float y = B * x + C * x * std::abs(x);

	const float P = 0.225f;

	return P * (y * std::abs(y) - y) + y;
}

//void Render::DrawCircle3D(Vector3 vPos, float flPoints, float flRadius, ImVec4 clrColor1, ImVec4 clrColor2)
//{
//	float flPoint = D3DX_PI * 2.0f / flPoints;
//
//	for (float flAngle = 0; flAngle < (D3DX_PI * 2.0f); flAngle += flPoint)
//	{
//		Vector3 vStart(flRadius * fastcos(flAngle) + vPos.x, flRadius * fastsin(flAngle) + vPos.z, vPos.y);
//		Vector3 vEnd(flRadius * fastcos(flAngle + flPoint) + vPos.x, flRadius * fastsin(flAngle + flPoint) + vPos.z, vPos.y);
//
//		float z_temp = vStart.z;
//		vStart.z = vStart.y;
//		vStart.y = z_temp;
//
//		z_temp = vEnd.z;
//		vEnd.z = vEnd.y;
//		vEnd.y = z_temp;
//
//		Vector2 vStartScreen = M->WorldToScreen(vStart);
//		Vector2 vEndScreen = M->WorldToScreen(vEnd);
//
//		if (!vStartScreen.x || !vStartScreen.y || !vEndScreen.x || !vEndScreen.y)
//			break;
//
//		//const int vert_start_idx = ImGui::GetOverlayLineList()->VtxBuffer.Size;
//		ImGuiDrawLine(ImVec2(vStartScreen.x, vStartScreen.y), ImVec2(vEndScreen.x, vEndScreen.y), clrColor1, 1);
//		//const int vert_end_idx = ImGui::GetOverlayLineList()->VtxBuffer.Size;
//		//ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetBackgroundDrawList(), vert_start_idx, vert_end_idx, ImVec2(vStartScreen.x, vStartScreen.y), ImVec2(vEndScreen.x, vEndScreen.y), ImGui::GetColorU32({ clrColor1.x, clrColor1.y, clrColor1.z, clrColor1.w }), ImGui::GetColorU32({ clrColor2.x, clrColor2.y, clrColor2.z, clrColor2.w })); // YOUR_COLOR use  IM_COL32(R,G,B,A)
//	}
//}