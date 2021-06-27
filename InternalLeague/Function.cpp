#include "Function.h"
#include "Render.h"

//bool Function::WorldToScreen(Vector3 pos, Vector2& screen)
//{
//    auto v1 = Global::BaseAddress + Offsets::;
//    auto v2 = G->ProjViewMatrix;
//
//    auto v3 = R->GetWidth();
//    auto v4 = R->GetHeight();
//
//    D3DXMATRIX matrix;
//    D3DXMatrixMultiply(&matrix, &v1, &v2);
//
//    Vector4 clipCoords;
//
//    clipCoords.x = pos.x * matrix._11 + pos.y * matrix._21 + pos.z * matrix._31 + matrix._41;
//    clipCoords.y = pos.x * matrix._12 + pos.y * matrix._22 + pos.z * matrix._32 + matrix._42;
//    clipCoords.z = pos.x * matrix._13 + pos.y * matrix._23 + pos.z * matrix._33 + matrix._43;
//    clipCoords.w = pos.x * matrix._14 + pos.y * matrix._24 + pos.z * matrix._34 + matrix._44;
//
//    if (clipCoords.z < 0.1f)
//        return false;
//
//    Vector3 M;
//    M.x = clipCoords.x / clipCoords.w;
//    M.y = clipCoords.y / clipCoords.w;
//    M.z = clipCoords.z / clipCoords.w;
//
//    screen.x = (v3 / 2 * M.x) + (M.x + v3 / 2);
//    screen.y = -(v4 / 2 * M.y) + (M.y + v4 / 2);
//
//    if (screen.x >= 1 && screen.y >= 1)
//        return true;
//}

