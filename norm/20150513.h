#pragma once

//
// CRenderer defines
//
typedef bool(__thiscall* lpDrawScene)(void*);

#define RENDERER_PTR *reinterpret_cast<DWORD*>(0x00ba9208)
#define OFFSET_0 offset0[0x24]
#define OFFSET_1 offset1[0x18];
#define DRAWSCENE_FN 0x00423040

//
// CSession defines
//

