#pragma once
#include "mod.h"
class statistics :
	public norm_dll::mod
{
public:
	statistics(norm_dll::norm* c_state);
	virtual ~statistics();
	void draw_scene(void*);

private:
	void notify_stat_server();
	bool already_notified = false;
};

