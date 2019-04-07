#pragma once
#include "mod.h"
class overlay :
	public norm_dll::mod
{
public:
	overlay(norm_dll::norm* c_state);
	virtual ~overlay();
	void draw_scene(void*);
	int get_talk_type(void**, void**, int*, int*, int*);

private:
	int display_ping = 0;
	int display_grid = 0;
};

