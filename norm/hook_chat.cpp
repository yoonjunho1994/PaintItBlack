#include "stdafx.h"

#include "hook_chat.h"
#include "detours.h"

static std::shared_ptr<norm_dll::norm> c_state;
//static std::vector<std::shared_ptr<norm_dll::mod>> SendMsg_callbacks;

bool SendMsg_detoured = false;

#if (CLIENT_VER == 20180620 || CLIENT_VER == 20180621 || CLIENT_VER_RE == 20180621)
#define SENDMSG
#if CLIENT_VER_RE == 20180621
DWORD UIWindowMgr_SendMsg_func = 0x00720AC0;
#else
DWORD UIWindowMgr_SendMsg_func = 0x0071ED80;
#endif
int __fastcall UIWindowMgr_SendMsg_hook(void* this_obj, DWORD EDX, int a1, void* a2, void* a3, int a4, int a5)

#elif CLIENT_VER == 20150000
#define SENDMSG
DWORD UIWindowMgr_SendMsg_func = 0x005F4CA0;
int __fastcall UIWindowMgr_SendMsg_hook(void* this_obj, DWORD EDX, int a1, int a2, int a3, int a4, int a5)
#endif
// fallback to have valid function
//int __fastcall UIWindowMgr_SendMsg_hook(void* this_obj, DWORD EDX, int a1, int a2, int a3, int a4, int a5)
//#endif
{
	UIWindowMgr_SendMsg original_sendmsg = (UIWindowMgr_SendMsg)UIWindowMgr_SendMsg_func;
	if (c_state->g_window_mgr == 0 && a1 == 1)
		c_state->g_window_mgr = (DWORD)this_obj;

	for (auto callback : c_state->mods)
		callback->send_msg(&this_obj, &a1, &a2, &a3, &a4, &a5);

	return original_sendmsg(this_obj, a1, a2, a3, a4, a5);
}

DWORD get_SendMsg_addr() {
#if  (CLIENT_VER == 20180620 || CLIENT_VER == 20180621)
	return 0x0071ED80;
#elif CLIENT_VER == 20150000
	return 0x005F4CA0;
#elif CLIENT_VER_RE == 20180621
	return 0x00720AC0;
#endif
	return 0;
}

int chat_detour(std::shared_ptr<norm_dll::norm> state_) {
	LONG err = 0;
	int hook_count = 0;
	char info_buf[256];
	c_state = state_;

#ifdef SENDMSG
	err = DetourAttach(&(LPVOID&)UIWindowMgr_SendMsg_func, &UIWindowMgr_SendMsg_hook);
	CHECK(info_buf, err);
	if (err == NO_ERROR) {
		SendMsg_detoured = true;
		hook_count++;
	} else 
		c_state->dbg_sock->do_send(info_buf);
#endif

	sprintf_s(info_buf, "Chat hooks available: %d", hook_count);
	c_state->dbg_sock->do_send(info_buf);

	return hook_count;
}