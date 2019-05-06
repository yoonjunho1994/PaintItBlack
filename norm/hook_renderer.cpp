#include "stdafx.h"

#include "hook_renderer.h"
#include "detours.h"

#include <vector>

#pragma warning(disable : 26440) // Suppress "noexcept" warning

static std::shared_ptr<norm_dll::norm> c_state;

// Search for: 3dlib\\Renderer.cpp
#if CLIENT_VER == 20180919
DWORD DrawScene_Addr = 0x0043FB70;

#elif (CLIENT_VER == 20180621 || CLIENT_VER == 20180620 || CLIENT_VER_RE == 20180621)
DWORD DrawScene_Addr = 0x0043FA20;

#elif CLIENT_VER == 20150000
DWORD DrawScene_Addr = 0x00423040;

#endif
//typedef void*(__thiscall *makewindow)(void*, int);
//typedef int(__thiscall *UIFrameWnd__DrawItemText)(void*, int, int, int, int, int, int);
bool __fastcall DrawScene_hook(void* this_obj)
{
    bool res = ((CRenderer__DrawScene)DrawScene_Addr)(this_obj);

    for (auto callback : c_state->mods)
        callback->draw_scene(this_obj);

    return res;
}

int renderer_detour(std::shared_ptr<norm_dll::norm> c_state_)
{
    LONG err = 0;
    int hook_count = 0;
    char info_buf[256];
    c_state = c_state_;

    err = DetourAttach(&(LPVOID&)DrawScene_Addr, &DrawScene_hook);
    CHECK(info_buf, err);
    if (err == NO_ERROR) {
		hook_count++;
    } else
        c_state->dbg_sock->do_send(info_buf);

    sprintf_s(info_buf, "Renderer hooks available: %d", hook_count);
    c_state->dbg_sock->do_send(info_buf);
    return hook_count;
}

// Search for: http://admtools.gnjoy.com/gamestarter/devro.asp?GameCode=6011
DWORD renderer_get_addr()
{
#if CLIENT_VER_RE == 20180621
    return 0x00E66F08;
#elif CLIENT_VER == 20180919
    return 0x00DA6D00;
#elif (CLIENT_VER == 20180620 || CLIENT_VER == 20180621)
    return 0x00DA2068;
#elif CLIENT_VER == 20150000
    return 0x00ba9208;
#endif
}

ULONG renderer_get_width()
{
#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20150000) || CLIENT_VER_RE == 20180621)
    return *(ULONG*)(*(DWORD*)(renderer_get_addr()) + 0x24);
#endif
    return -1;
}

ULONG renderer_get_height()
{
#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20150000) || CLIENT_VER_RE == 20180621)
    return *(ULONG*)(*(DWORD*)(renderer_get_addr()) + 0x28);
#endif
    return -1;
}

int renderer_get_fps()
{
#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20150000) || CLIENT_VER_RE == 20180621)
    return *(int*)(*(DWORD*)(renderer_get_addr()) + 0x44);
#endif
    return -1;
}
