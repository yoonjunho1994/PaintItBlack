#pragma once

//
// CRenderer defines
//
typedef bool(__thiscall* lpDrawScene)(void*);

//struct CRenderer {
#define RENDERER_DATA                  \
    /* 0x0	*/ BYTE offset0[0x24];	   \
    /* 0x24 */ ULONG width;            \
    /* 0x28 */ ULONG height;           \
    /* 0x2C */ BYTE offset2[0x18];     \
    /* 0x44 */ int fps;
//};

#define DRAWSCENE_FN 0x0043FB70
#define RENDERER_PTR *reinterpret_cast<DWORD*>(0x00DA6D00)