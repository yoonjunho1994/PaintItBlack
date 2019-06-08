#include "stdafx.h"

#include "detours.h"
#include "hook_gamemode.h"

#include <chrono>

#pragma warning(disable : 26440) // Suppress "noexcept" warning

using namespace std::chrono;

static std::shared_ptr<norm_dll::norm> c_state;
bool init_called = false;
long long init_time = 0;

// Search for: xmas_fild01
#if CLIENT_VER == 20150000
DWORD CGameMode__Initialize_func = 0x00860160;

#elif CLIENT_VER == 20180919
DWORD CGameMode__Initialize_func = 0x00937130;

#elif (CLIENT_VER == 20180621 || CLIENT_VER == 20180620)
DWORD CGameMode__Initialize_func = 0x009361A0;

#elif CLIENT_VER_RE == 20180621
DWORD CGameMode__Initialize_func = 0x009F0990;

#endif
void __fastcall CGameMode__Initialize_hook(void* this_obj, DWORD EDX)
{
    CGameMode__Initialize original_init = (CGameMode__Initialize)CGameMode__Initialize_func;
    c_state->dbg_sock->do_send("CGameMode__Initialize called!");
    print_time(c_state.get());
    init_called = true;
    original_init(this_obj);
}

// Search for: valkyrie.rsw
#if CLIENT_VER == 20150000
DWORD CGameMode__OnInit_func = 0x00866880;

#elif CLIENT_VER == 20180919
DWORD CGameMode__OnInit_func = 0x0093E0D0;

#elif (CLIENT_VER == 20180621 || CLIENT_VER == 20180620)
DWORD CGameMode__OnInit_func = 0x0093D100;

#elif CLIENT_VER_RE == 20180621
DWORD CGameMode__OnInit_func = 0x009F7940;

#endif
void __fastcall CGameMode__OnInit_hook(void* this_obj, DWORD EDX, const char* a1)
{
    CGameMode__OnInit original_oninit = (CGameMode__OnInit)CGameMode__OnInit_func;
    c_state->dbg_sock->do_send("CGameMode__OnInit called!");
    print_time(c_state.get());
    original_oninit(this_obj, a1);
}

bool initialize_called()
{
    bool ret = init_called;
    init_called = false;
    return ret;
}

int gamemode_detour(std::shared_ptr<norm_dll::norm> state_)
{
    LONG err = 0;
    int hook_count = 0;
    char info_buf[256];
    c_state = state_;

    err = DetourAttach(&(LPVOID&)CGameMode__Initialize_func, &CGameMode__Initialize_hook);
    CHECK(info_buf, err);
    if (err == NO_ERROR)
        hook_count++;
    else
        c_state->dbg_sock->do_send(info_buf);

    err = DetourAttach(&(LPVOID&)CGameMode__OnInit_func, &CGameMode__OnInit_hook);
    CHECK(info_buf, err);
    if (err == NO_ERROR)
        hook_count++;
    else
        c_state->dbg_sock->do_send(info_buf);

    /* Unused
	err = DetourAttach(&(LPVOID&)CGameMode__Zc_Npcack_Mapmove_func, &CGameMode__Zc_Npcack_Mapmove_hook);
    CHECK(info_buf, err);
    if (err == NO_ERROR)
        hook_count++;
    else
        c_state->dbg_sock->do_send(info_buf);
	*/

    sprintf_s(info_buf, "Gamemode hooks available: %d", hook_count);
    c_state->dbg_sock->do_send(info_buf);

    return hook_count;
}
