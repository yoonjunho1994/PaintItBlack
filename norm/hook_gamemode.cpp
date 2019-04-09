#include "stdafx.h"

#include "hook_gamemode.h"
#include "detours.h"

#include <chrono>

using namespace std::chrono;

static std::shared_ptr<norm_dll::norm> c_state;
bool init_called = false;
long long init_time = 0;

#if CLIENT_VER == 20150000
#define INITIALIZE
DWORD CGameMode__Initialize_func = 0x00860160;
#elif (CLIENT_VER == 20180621 || CLIENT_VER == 20180620)
#define INITIALIZE
DWORD CGameMode__Initialize_func = 0x009361A0;
#elif CLIENT_VER_RE == 20180621
#define INITIALIZE
DWORD CGameMode__Initialize_func = 0x009F0990;
#endif
void __fastcall CGameMode__Initialize_hook(void* this_obj, DWORD EDX)
{
	CGameMode__Initialize original_init = (CGameMode__Initialize)CGameMode__Initialize_func;
	c_state->dbg_sock->do_send("CGameMode__Initialize called!");
	print_time(c_state.get());
	init_called = true;

	original_init(this_obj);

	for (auto it : c_state->mods)
		it->Initialize(&this_obj);
	
}

#if CLIENT_VER == 20150000
#define ONINIT
DWORD CGameMode__OnInit_func = 0x00866880;
#elif (CLIENT_VER == 20180621 || CLIENT_VER == 20180620)
#define ONINIT
DWORD CGameMode__OnInit_func = 0x0093D100;
#elif CLIENT_VER_RE == 20180621
#define ONINIT
DWORD CGameMode__OnInit_func = 0x009F7940;
#endif
void __fastcall CGameMode__OnInit_hook(void* this_obj, DWORD EDX, const char * a1)
{
	CGameMode__OnInit original_oninit = (CGameMode__OnInit)CGameMode__OnInit_func;
	c_state->dbg_sock->do_send("CGameMode__OnInit called!");
	print_time(c_state.get());
	original_oninit(this_obj, a1);
}

#if CLIENT_VER == 20150000
#define MAPMOVE	// Does not work when used with debugger. 
					// Currently not needed by the dll.
DWORD CGameMode__Zc_Npcack_Mapmove_func = 0x008B67F0;
#endif
void __fastcall CGameMode__Zc_Npcack_Mapmove_hook(void* this_obj, DWORD EDX, const char * a1)
{
	CGameMode__Zc_Npcack_Mapmove original_mapmove = (CGameMode__Zc_Npcack_Mapmove)CGameMode__Zc_Npcack_Mapmove_func;
	c_state->dbg_sock->do_send("CGameMode__Zc_Npcack_Mapmove called!");
	print_time(c_state.get());
	for (auto it : c_state->mods)
		it->Npcack_Mapmove(&this_obj, &a1);

	original_mapmove(this_obj, a1);
}

bool initialize_called()
{
	bool ret = init_called;
	init_called = false;
	return ret;
}

int gamemode_detour(std::shared_ptr<norm_dll::norm> state_) {
	LONG err = 0;
	int hook_count = 0;
	char info_buf[256];
	c_state = state_;

#ifdef INITIALIZE
	err = DetourAttach(&(LPVOID&)CGameMode__Initialize_func, &CGameMode__Initialize_hook);
	CHECK(info_buf, err);
	if (err == NO_ERROR)
		hook_count++;
	else
		c_state->dbg_sock->do_send(info_buf);
#endif

#ifdef ONINIT
	err = DetourAttach(&(LPVOID&)CGameMode__OnInit_func, &CGameMode__OnInit_hook);
	CHECK(info_buf, err);
	if (err == NO_ERROR)
		hook_count++;
	else
		c_state->dbg_sock->do_send(info_buf);
#endif

#ifdef MAPMOVE
	err = DetourAttach(&(LPVOID&)CGameMode__Zc_Npcack_Mapmove_func, &CGameMode__Zc_Npcack_Mapmove_hook);
	CHECK(info_buf, err);
	if (err == NO_ERROR)
		hook_count++;
	else
		c_state->dbg_sock->do_send(info_buf);
#endif

	sprintf_s(info_buf, "Gamemode hooks available: %d", hook_count);
	c_state->dbg_sock->do_send(info_buf);

	return hook_count;
}