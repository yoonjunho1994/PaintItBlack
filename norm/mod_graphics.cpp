#include "stdafx.h"
#include "mod_graphics.h"

graphics::graphics(norm_dll::norm * c_state) : mod(c_state)
{
}

graphics::~graphics()
{
}

#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
int graphics::get_talk_type(void **this_obj, void **src, int *a1, int *a2, int* retval)
#elif CLIENT_VER == 20150000
int graphics::get_talk_type(void**this_obj, char** src, int* a1, char** a2, int* retval)
#endif
{
	// debug: c_state->dbg_sock->do_send((char*)*src);
	if (strcmp((char*)*src, "/vsync") == 0) {
		char buf[64];
		this->vsync_active ^= 1;
		if (this->vsync_active)
			sprintf_s(buf, "VSync is now enabled.");
		else
			sprintf_s(buf, "VSync is now disabled.");
		this->print_to_chat(buf);
		*retval = -1;
		return 1;
	}
	return 0;
}

HRESULT graphics::WaitForVerticalBlank(DWORD *, HANDLE *)
{
	if (!this->vsync_active)
		return DD_OK;
	return DD_FALSE;
}
