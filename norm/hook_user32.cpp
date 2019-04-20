#include "stdafx.h"

#include "hook_user32.h"
#include "detours.h"


#pragma warning(disable : 26440) // Suppress "noexcept" warning

/* debug */
static std::shared_ptr<norm_dll::norm> c_state;
char title[128];

HWND(WINAPI* pCreateWindowExA)
(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, LPVOID)
    = CreateWindowExA;

HWND WINAPI CreateWindowExA_hook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, LPVOID lpParam)
{
    sprintf_s(title, "%s | [Mods] Paint it Black", lpWindowName);
    c_state->dbg_sock->do_send(title);
    return pCreateWindowExA(dwExStyle, lpClassName, title, dwStyle, X, Y, nWidth, nHeight, hWndParent,
        hMenu, hInstance, lpParam);
}

int user32_detour(std::shared_ptr<norm_dll::norm> state_)
{
    int err = 0;
    int hook_count = 0;
    char info_buf[256];
    c_state = state_;

    err = DetourAttach(&(LPVOID&)pCreateWindowExA, &CreateWindowExA_hook);
    CHECK(info_buf, err);
    //sprintf_s(info_buf, "%x", pCreateWindowExA);
    //c_state->dbg_sock->do_send(info_buf);
    if (err == NO_ERROR) {
        hook_count++;
    } else
        c_state->dbg_sock->do_send(info_buf);

    sprintf_s(info_buf, "USER32 hooks available: %d", hook_count);
    c_state->dbg_sock->do_send(info_buf);

    return hook_count;
}
