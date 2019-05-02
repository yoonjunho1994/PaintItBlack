#pragma once
#include "mod.h"
#include "mod_graphics.h"

class overlay_new :
	public norm_dll::mod
{
public:
    overlay_new(norm_dll::norm* c_state, std::shared_ptr<graphics> g);
	virtual ~overlay_new();

private:
	int display_ping = false;
	int display_fps = false;

	/* postion of the overlay */
	int x = -1;
	int y = -1;

	std::shared_ptr<graphics> g;

	HRESULT end_scene(IDirect3DDevice7**);
	int get_talk_type(void**, void**, int*, int*, int*);
	int get_talk_type(void**, char**, int*, char**, int*);
	void draw_scene(void * this_obj);
};

