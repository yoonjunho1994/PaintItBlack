#include "stdafx.h"
#include "hook.h"
#include <chrono>

void print_time(norm_dll::norm* c_state)
{
	using namespace std::chrono;
	milliseconds ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		);
	char buf[64];
	long long time = ms.count();
	sprintf_s(buf, "%lld", time);
	c_state->dbg_sock->do_send(buf);
}