#pragma once
#include "mod.h"
#include "SFastFont.h"
#include <vector>
class overlay_new :
	public norm_dll::mod
{
public:
	overlay_new(norm_dll::norm* c_state);
	virtual ~overlay_new();
	HRESULT begin_scene(IDirect3DDevice7**);
	HRESULT end_scene(IDirect3DDevice7**);

private:
	int initialized = 0;
	int display_ping = false;
	int display_fps = false;
	int old_display_ping = 0;
	int old_display_fps = 0;
	/* postion of the overlay */
	int x = -1;
	int y = -1;

	void init(IDirect3DDevice7* d3ddevice);
	LPDIRECTDRAWSURFACE7 m_pddsFontTexture;
	CSFastFont *m_pSFastFont;
	int get_talk_type(void**, void**, int*, int*, int*);
	int get_talk_type(void**, char**, int*, char**, int*);
	void draw_scene(void * this_obj);
	void ddraw_release();
	void Npcack_Mapmove(void**, const char**);
	void Initialize(void**);
};

