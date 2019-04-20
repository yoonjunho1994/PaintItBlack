#include "stdafx.h"
#include "hook_session.h"
#include "hook_gamemode.h"
#include "detours.h"

#pragma warning(disable: 26440) // Suppress "noexcept" warning

static std::shared_ptr<norm_dll::norm> c_state;

bool GetTalkType_detoured = true;
int init_ping_calls = 2;

/*
 * Search for:
 * /goldpc
 */
#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
#define GETTALKTYPE
#if CLIENT_VER_RE == 20180621
DWORD GetTalkType_func = 0x00AC7D90;
#elif (CLIENT_VER == 20180620 || CLIENT_VER == 20180621)
DWORD GetTalkType_func = 0x00A0CF40;
#elif CLIENT_VER == 20180919
DWORD GetTalkType_func = 0x00A0EC10;
#endif
typedef int(__thiscall *GetTalkType)(void*, void*, int, int);

// __thiscall to __fastcall workaround with EDX.
int __fastcall GetTalkType_hook(void *this_obj, DWORD EDX, void* a2, int a3, int a4)
{
#elif CLIENT_VER == 20150000
#define GETTALKTYPE
DWORD GetTalkType_func = 0x00925100;
typedef  signed int(__thiscall *GetTalkType)(void*, char*, int, char*);

// __thiscall to __fastcall workaround with EDX.
signed int __fastcall GetTalkType_hook(void *this_obj, DWORD EDX, char *a2, int a3, char *a4)
{
#endif
	GetTalkType original_GetTalkType = (GetTalkType)GetTalkType_func;

	int cret = 0;
	int retval = 0;

	for (auto callback : c_state->mods)
		cret += callback->get_talk_type(&this_obj, &a2, &a3, &a4, &retval);

	if (cret == 1)
		return retval;

	if (cret > 1)
		c_state->dbg_sock->do_send("Error: Multiple GetTalkType hooks want to return a value.");

	return original_GetTalkType(this_obj, a2, a3, a4);
}

/*
 * Search for:
 * %10.2f\t%10d\n
 * Found function will call RecalcAveragePingTime at the end.
 */
#if CLIENT_VER == 20150000
#define RECALCPING
DWORD CSession__RecalcAveragePingTime_func = 0x00935560;
typedef  void(__thiscall *CSession__RecalcAveragePingTime)(void*, unsigned long);
#elif ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
#define RECALCPING
#if CLIENT_VER_RE == 20180621
DWORD CSession__RecalcAveragePingTime_func = 0x00ADA470;
#elif (CLIENT_VER == 20180620 || CLIENT_VER == 20180621)
DWORD CSession__RecalcAveragePingTime_func = 0x00A1F510;
#elif CLIENT_VER == 20180919
DWORD CSession__RecalcAveragePingTime_func = 0x00A212D0;
#endif
typedef  void(__thiscall *CSession__RecalcAveragePingTime)(void*, unsigned long);
#endif

void __fastcall CSession__RecalcAveragePingTime_hook(void* this_obj, DWORD EDX, unsigned long a1)
{
	CSession__RecalcAveragePingTime original_recalc = (CSession__RecalcAveragePingTime)CSession__RecalcAveragePingTime_func;
	c_state->dbg_sock->do_send("CSession__RecalcAveragePingTime called!");

	char buf[64];
	sprintf_s(buf, "Arg: %lu", a1);
	c_state->dbg_sock->do_send(buf);

	print_time(c_state.get());

	if (init_ping_calls > 0) {
		init_ping_calls--;
		return;
	}

	if (!initialize_called())
		original_recalc(this_obj, a1);
}

int session_detour(std::shared_ptr<norm_dll::norm> state_) {
	LONG err = 0;
	int hook_count = 0;
	char info_buf[256];
	c_state = state_;

#ifdef GETTALKTYPE
	err = DetourAttach(&(LPVOID&)GetTalkType_func, &GetTalkType_hook);
	CHECK(info_buf, err);
	if (err == NO_ERROR) {
		GetTalkType_detoured = true;
		hook_count++;
	} else 
		c_state->dbg_sock->do_send(info_buf);
#endif

#ifdef RECALCPING
	err = DetourAttach(&(LPVOID&)CSession__RecalcAveragePingTime_func, &CSession__RecalcAveragePingTime_hook);
	CHECK(info_buf, err);
	if (err == NO_ERROR) {
		hook_count++;
	}
	else
		c_state->dbg_sock->do_send(info_buf);
#endif

	sprintf_s(info_buf, "Session hooks available: %d", hook_count);
	c_state->dbg_sock->do_send(info_buf);

	return hook_count;
}

DWORD session_get_addr()
{
#if CLIENT_VER == 20180919
    return 0x010178D0;
#elif (CLIENT_VER == 20180620 || CLIENT_VER == 20180621)
	return 0x010130C8;
#elif CLIENT_VER_RE == 20180621
	return 0x010D7F58;
#elif CLIENT_VER == 20150000
	return 0x00E0EE28;
#endif
}

ULONG session_get_averagePingTime()
{
#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
	return *(ULONG*)(session_get_addr() + 0x630);
#endif
#if CLIENT_VER == 20150000
	return *(ULONG*)(session_get_addr() + 0x634);
#endif
}
