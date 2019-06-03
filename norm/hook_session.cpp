#include "stdafx.h"

#include "hook.h"
#include "hook_session.h"
#include "hook_gamemode.h"

#include "detours.h"
#include "norm.h"

#pragma warning(disable: 26440) // Suppress "noexcept" warning

namespace norm_dll {
static std::shared_ptr<norm_dll::norm> c_state;
int init_ping_calls = 2;

/*
 * Search for:
 * /goldpc
 */
#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
int __fastcall ProxySession::pGetTalkType(void* this_obj, DWORD EDX, void* a2, int a3, int a4)
{
#elif CLIENT_VER == 20150000
signed int __fastcall ProxySession::pGetTalkType(void *this_obj, DWORD EDX, char *a2, int a3, char *a4)
{
#endif
    auto& instance = ProxySession::instance();

	int cret = 0;
	int retval = 0;

	for (auto callback : instance.c_state->mods)
		cret += callback->get_talk_type(reinterpret_cast<char*>(a2), &retval);

	if (cret == 1)
		return retval;

	if (cret > 1)
		instance.c_state->dbg_sock->do_send("Error: Multiple GetTalkType hooks want to return a value.");

	return (instance.GetTalkType)(this_obj, a2, a3, a4);
}

/*
 * Search for:
 * %10.2f\t%10d\n
 * Found function will call RecalcAveragePingTime at the end.
 */
void __fastcall ProxySession::pRecalcAveragePingTime(void* this_obj, DWORD EDX, unsigned long a1)
{
    auto& instance = ProxySession::instance();
	instance.c_state->dbg_sock->do_send("CSession__RecalcAveragePingTime called!");

	char buf[64];
	sprintf_s(buf, "Arg: %lu", a1);
	instance.c_state->dbg_sock->do_send(buf);

	print_time(instance.c_state.get());

	if (init_ping_calls > 0) {
		init_ping_calls--;
		return;
	}

	if (!initialize_called())
		(instance.RecalcAveragePingTime)(this_obj, a1);
}

void ProxySession::hook(std::shared_ptr<norm_dll::norm> state_) 
{
    if (hooked)
        return;

	LONG err = 0;
	int hook_count = 0;
	char info_buf[256];
	this->c_state = state_;

	err = DetourAttach(&(LPVOID&)GetTalkType, &pGetTalkType);
	CHECK(info_buf, err);
	if (err == NO_ERROR) {
		hook_count++;
	} else 
		this->c_state->dbg_sock->do_send(info_buf);

	err = DetourAttach(&(LPVOID&)RecalcAveragePingTime, &pRecalcAveragePingTime);
	CHECK(info_buf, err);
	if (err == NO_ERROR) {
		hook_count++;
	}
	else
     this->c_state->dbg_sock->do_send(info_buf);

	sprintf_s(info_buf, "Session hooks available: %d", hook_count);
    this->c_state->dbg_sock->do_send(info_buf);

	this->hooked = true;
}

#include <stdexcept>
ULONG ProxySession::get_average_ping_time()
{
    return this->c_session->average_ping_time;
}

ULONG ProxySession::get_aid()
{
    return this->c_session->aid;
}

ULONG ProxySession::get_gid()
{
    return this->c_session->gid;
}

int ProxySession::get_job()
{
    return this->c_session->job;
}

int ProxySession::get_exp()
{
    return this->c_session->exp;
}

int ProxySession::get_level()
{
    return this->c_session->level;
}

int ProxySession::get_next_exp()
{
    return this->c_session->next_exp;
}

int ProxySession::get_joblevel()
{
    return this->c_session->joblevel;
}

int ProxySession::get_jobnextexp()
{
    return this->c_session->jobnextexp;
}

int ProxySession::get_jobexp()
{
    return this->c_session->jobexp;
}
}