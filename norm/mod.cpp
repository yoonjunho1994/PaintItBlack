#include "stdafx.h"
#include "norm.h"
#include "mod.h"

#include "hook_chat.h"

void norm_dll::mod::print_to_chat(char * msg)
{
	DWORD smsg_addr = get_SendMsg_addr();
	DWORD color = 0x00FFFF;
	if (smsg_addr) 
#if (CLIENT_VER == 20180620 || CLIENT_VER == 20180621 || CLIENT_VER_RE == 20180621)
		((UIWindowMgr_SendMsg)smsg_addr)((void*)c_state->g_window_mgr, UIM_PUSHINTOCHATHISTORY, msg, &color, 0, 0);
#elif CLIENT_VER == 20150000
		((UIWindowMgr_SendMsg)smsg_addr)((void*)c_state->g_window_mgr, UIM_PUSHINTOCHATHISTORY, (int)msg, (int)&color, 0, 0);
#endif
	else
		c_state->dbg_sock->do_send("print_to_chat failed.");
}
