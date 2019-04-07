#include "stdafx.h"
#include "detours.h"
#include "norm.h"

#include "hook_chat.h"
#include "hook_renderer.h"
#include "hook_socket.h"
#include "hook_session.h"
#include "hook_dx.h"
#include "hook_gamemode.h"

#include "mod_overlay.h"
#include "mod_statistics.h"
#include "mod_timestamp.h"
#include "mod_overlay_new.h"
#include "mod_graphics.h"

#include "verifier.h"

#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

// Installs a mod based on the classname.
#define INSTALL_MOD(modname)	mods.push_back(std::make_shared<modname>(this));
								

/* dll class */
namespace norm_dll {

norm::norm()
{
}

void norm::install_mods()
{
	// Disable a mod by commenting out the specific line.
	//INSTALL_MOD(overlay);
	//INSTALL_MOD(statistics);
	INSTALL_MOD(timestamp);
	INSTALL_MOD(overlay_new);
	INSTALL_MOD(graphics);
}

void norm::start()
{
	char info_buf[256];
	/* Connect to the debug socket */
	dbg_sock = std::make_shared<debug_socket>();

	int err = dbg_sock->do_connect();
	if (err != 0) {
		MessageBoxA(0, "Unable to connect to the debug socket!", "norm.dll error!", MB_OK);
		dbg_sock->disabled = 1;
	}
	dbg_sock->do_send("Hello Debugger!");
#ifdef CLIENT_VER
	sprintf_s(info_buf, "DLL compiled for: %d", CLIENT_VER);
#endif
#ifdef CLIENT_VER_RE
	sprintf_s(info_buf, "DLL compiled for: %dRE", CLIENT_VER_RE);
#endif
	dbg_sock->do_send(info_buf);

	dbg_sock->do_send("Verifying PE compatibility ...");
	if (strcmp((char*)VERIFY_ADDR, VERIFY_STR) != 0) {
		dbg_sock->do_send("DLL and PE are not compatibile!");
#ifdef CLIENT_VER_RE
		sprintf_s(info_buf, "Client is not compatible with %dRE-norm.dll", CLIENT_VER_RE);
#else
		sprintf_s(info_buf, "Client is not compatible with %d-norm.dll", CLIENT_VER_RE);
#endif
		MessageBoxA(0, info_buf, "norm.dll error!", MB_OK);
		return;
	}
	dbg_sock->do_send("Success!");

	/* Hook functions. */
	err = DetourTransactionBegin();
	CHECK(info_buf, err);
	dbg_sock->do_send(info_buf);

	err = DetourUpdateThread(GetCurrentThread());
	CHECK(info_buf, err);
	dbg_sock->do_send(info_buf);

	// attach hooks and install mods
	int total_hooks = 0;
	auto sptr = shared_from_this();
	total_hooks += chat_detour(sptr);
	total_hooks += socket_detour(sptr);
	total_hooks += renderer_detour(sptr);
	total_hooks += session_detour(sptr);
	total_hooks += dx_detour(sptr);
	total_hooks += gamemode_detour(sptr);

	this->install_mods();

	err = DetourTransactionCommit();
	CHECK(info_buf, err);
	dbg_sock->do_send(info_buf);

	sprintf_s(info_buf, "Total hooks available: %d", total_hooks);
	dbg_sock->do_send(info_buf);
	
	//
	// patching .text section
	//
	// ping interval change from 12000 to 2000
	// Search for: 12000
#if (CLIENT_VER == 20180621 || CLIENT_VER == 20180620)
	LPVOID hex_code = (LPVOID)0x0094AB1E;
#elif CLIENT_VER == 20150000
	LPVOID hex_code = (LPVOID)0x0087344E;
#elif CLIENT_VER_RE == 20180621
	LPVOID hex_code = (LPVOID)0x00A054CE;
#endif
	DWORD old_protect;

	int ret = VirtualProtect(hex_code, 6, 0x04, &old_protect);
	sprintf_s(info_buf, "VirtualProtect ret val: %d", ret);
	dbg_sock->do_send(info_buf);
	
	char hex_code_str[128] = "Changing ASM instructions for ping interval:\n ";
	char tmp[4];
	for (int i = 0; i < 6; i++) {
		sprintf_s(tmp, "%x ", ((BYTE*)hex_code)[i]);
		strcat_s(hex_code_str, tmp);
	}
	strcat_s(hex_code_str, " -> ");

	for (int i = 0; i < 6; i++) {
		if (i == 2)
			((BYTE*)hex_code)[i] = 0xD0;
		if (i == 3)
			((BYTE*)hex_code)[i] = 0x07;
		sprintf_s(tmp, "%x ", ((BYTE*)hex_code)[i]);
		strcat_s(hex_code_str, tmp);
	}

	dbg_sock->do_send(hex_code_str);
	DWORD old_reset_protect;
	ret = VirtualProtect(hex_code, 6, old_protect, &old_reset_protect);
	sprintf_s(info_buf, "VirtualProtect reset ret val: %d", ret);
	dbg_sock->do_send(info_buf);
}

norm::~norm()
{
}
}