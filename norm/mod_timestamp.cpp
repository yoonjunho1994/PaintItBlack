#include "stdafx.h"

#include "mod_timestamp.h"

#include <time.h>

timestamp::timestamp(norm_dll::norm* c_state) : mod(c_state)
{
}

timestamp::~timestamp()
{
}

#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
void timestamp::send_msg(void** this_obj, int* a1, void** a2, void** a3, int* a4, int* a5)
#elif CLIENT_VER == 20150000
void timestamp::send_msg(void** this_obj, int* a1, int*   a2, int*   a3, int* a4, int* a5)
#endif
{
	if (*a1 != 1)
		return;

	if (this->enabled) {
		const time_t t = time(nullptr);
		struct tm buf_tm;
		char ts[10] = "";
		const int err = localtime_s(&buf_tm, &t);
		strftime(ts, 10, "%H:%M:%S", &buf_tm);
		sprintf_s(this->msg_buf, "[%s] %s", ts, reinterpret_cast<char*>(*a2));
#if CLIENT_VER == 20150000
		*a2 = reinterpret_cast<int>(&this->msg_buf);
#elif ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
		*a2 = this->msg_buf;
#endif	
	}
}

#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
int timestamp::get_talk_type(void **this_obj, void **src, int *a1, int *a2, int* retval)
#elif CLIENT_VER == 20150000
[[gsl::suppress(26473)]] // suppressing cast from char* to char*
int timestamp::get_talk_type(void** this_obj, char** src, int* a1, char** a2, int* retval)
#endif
{
	// debug: c_state->dbg_sock->do_send((char*)*src);
	if (strcmp(static_cast<char*>(*src), "/timestamp") == 0) {
		char buf[64] = { 0 };
		this->enabled ^= 1;
		if (this->enabled)
			sprintf_s(buf, "Timestamp is now enabled.");
		else
			sprintf_s(buf, "Timestamp is now disabled.");
		this->print_to_chat(buf);
		*retval = -1;
		return 1;
	}
	return 0;
}
