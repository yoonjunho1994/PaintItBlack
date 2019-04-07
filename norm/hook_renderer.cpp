#include "stdafx.h"
#include "hook_renderer.h"
#include "detours.h"
#include <vector>

static std::shared_ptr<norm_dll::norm> c_state;
//static std::vector<std::shared_ptr<norm_dll::mod>> DrawScene_callbacks;

bool DrawScene_detoured = false;

#if (CLIENT_VER == 20180620 || CLIENT_VER == 20180621 || CLIENT_VER_RE == 20180621 )
#define DRAWSCENE
DWORD DrawScene_Addr = 0x0043FA20;
#elif CLIENT_VER == 20150000
#define DRAWSCENE
DWORD DrawScene_Addr = 0x00423040;
#endif
#ifdef DRAWSCENE
//typedef void*(__thiscall *makewindow)(void*, int);
//typedef int(__thiscall *UIFrameWnd__DrawItemText)(void*, int, int, int, int, int, int);
bool __fastcall DrawScene_hook(void* this_obj)
{
	//DWORD target = DrawScene_Addr;
	bool res = ((CRenderer__DrawScene)DrawScene_Addr)(this_obj);
	//for (std::shared_ptr<norm_dll::mod> callback : DrawScene_callbacks) {
	for (auto callback : c_state->mods)
		callback->draw_scene(this_obj);
	//}
	return res;
}
#endif

/*DWORD get_DrawScene_Addr()
{
	return DrawScene_Addr;
}*/

/*DWORD get_TextOutScreen_Addr()
{
	return 0x00443D00;
}*/

/*int register_DrawScene_hook(std::shared_ptr<norm_dll::mod> mod_ptr) {
#ifdef DRAWSCENE
	if (!DrawScene_detoured)
		return 0;
	//DrawScene_callbacks.push_back(mod_ptr);
	return 1;
#else
	return 0;
#endif
}*/

#if (CLIENT_VER == 20180621 || CLIENT_VER == 20180620)
DWORD verify_at = 0x00422B98;
BYTE renderer_verify[] = {  0x8B, 0x0D, 0x68, 0x20, 0xDA, 0x00, 0x8D, 0x45,
							0xCC, 0x50, 0x57, 0x53, 0xE8, 0xF7, 0xE5, 0x01,
							0x00, 0x8B, 0xF8, 0x85, 0xFF, 0x75, 0x22, 0x8B,
							0x0D, 0x68, 0x20, 0xDA, 0x00, 0x8D, 0x45, 0xCC,
							0x50, 0xFF, 0x75, 0x2C, 0x53, 0xE8, 0x6E, 0xB4 };
#endif
#if CLIENT_VER == 20150000
DWORD verify_at = 0x004064EF;
BYTE renderer_verify[] = {  0x8B, 0x0D, 0x08, 0x92, 0xBA, 0x00, 0x8D, 0x45,
							0xC8, 0x50, 0x57, 0x53, 0xE8, 0x30, 0xE1, 0x01,
							0x00, 0x8B, 0xF8, 0x85, 0xFF, 0x75, 0x22, 0x8B,
							0x0D, 0x08, 0x92, 0xBA, 0x00, 0x8D, 0x45, 0xC8,
							0x50, 0xFF, 0x75, 0xFC, 0x53, 0xE8, 0x67, 0xB1 };
#endif
#if CLIENT_VER_RE == 20180621
DWORD verify_at = 0x00422B98;
BYTE renderer_verify[] = {	0x8B, 0x0D, 0x08, 0x6F, 0xE6, 0x00, 0x8D, 0x45, 
							0xCC, 0x50, 0x57, 0x53, 0xE8, 0xF7, 0xE5, 0x01, 
							0x00, 0x8B, 0xF8, 0x85, 0xFF, 0x75, 0x22, 0x8B, 
							0x0D, 0x08, 0x6F, 0xE6, 0x00, 0x8D, 0x45, 0xCC, 
							0x50, 0xFF, 0x75, 0x2C, 0x53, 0xE8, 0x6E, 0xB4 };
#endif

int renderer_detour(std::shared_ptr<norm_dll::norm> c_state_) {
	LONG err = 0;
	int hook_count = 0;
	char info_buf[256];
	c_state = c_state_;


#ifdef DRAWSCENE
	c_state->dbg_sock->do_send("Trying to verify renderer hook for given clientdate ...");
	sprintf_s(info_buf, "Size %d", sizeof(renderer_verify));
	MessageBoxA(0, info_buf, "norm.dll error!", MB_OK);
	for (int i = 0; i < sizeof(renderer_verify); i++) {
		if (renderer_verify[i] != ((BYTE*)verify_at)[i]) {
			c_state->dbg_sock->do_send("Failed to verify for client date! Aborting!");
			MessageBoxA(0, "Verify Error: 1001", "norm.dll error!", MB_OK);
			return 0;
		}
	}
	err = DetourAttach(&(LPVOID&)DrawScene_Addr, &DrawScene_hook);
	CHECK(info_buf, err);
	if (err == NO_ERROR) {
		DrawScene_detoured = true;
		hook_count++;
	} else
		c_state->dbg_sock->do_send(info_buf);
#endif

	sprintf_s(info_buf, "Renderer hooks available: %d", hook_count);
	c_state->dbg_sock->do_send(info_buf);
	return hook_count;
}

DWORD renderer_get_addr()
{
// 20180621RE 00E66F08
#if CLIENT_VER_RE == 20180621
	return 0x00E66F08;
#elif (CLIENT_VER == 20180620 || CLIENT_VER == 20180621)
	return 0x00DA2068;
#elif CLIENT_VER == 20150000
	return 0x00ba9208;
#endif
	return -1;
}

ULONG renderer_get_width()
{
#if (CLIENT_VER == 20180620 || CLIENT_VER == 20180621 || CLIENT_VER == 20150000 || CLIENT_VER_RE == 20180621)
	return *(ULONG*)(*(DWORD*)(renderer_get_addr()) + 0x24);
#endif
	return -1;
}

ULONG renderer_get_height()
{
#if (CLIENT_VER == 20180620 || CLIENT_VER == 20180621 || CLIENT_VER == 20150000 || CLIENT_VER_RE == 20180621)
	return  *(ULONG*)(*(DWORD*)(renderer_get_addr()) + 0x28);
#endif
	return -1;
}

int renderer_get_fps()
{
#if (CLIENT_VER == 20180620 || CLIENT_VER == 20180621 || CLIENT_VER == 20150000 || CLIENT_VER_RE == 20180621)
	return *(int*)(*(DWORD*)(renderer_get_addr()) + 0x44);
#endif
	return -1;
}
