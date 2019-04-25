#pragma once

//
// CRenderer defines
//
typedef bool(__thiscall* lpDrawScene)(void*);

#define RENDERER_PTR (CRenderer*)*(DWORD*)0x00DA6D00;
#define OFFSET_0 offset0[0x24]
#define OFFSET_1 offset1[0x18];
#define DRAWSCENE_FN (lpDrawScene)0x0043FB70;