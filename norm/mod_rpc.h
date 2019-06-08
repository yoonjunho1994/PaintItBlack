#pragma once
#include "mod.h"

class rpc : public norm_dll::mod
{
    public:
    rpc(norm_dll::norm* c_state);
    ~rpc();

	void updateDiscordPresence();
	void init();

    int get_talk_type(char*, int*);
    void draw_scene(void* this_obj);

	const char* APPLICATION_ID = "570835350193176579";
    int64_t StartTime;
    int SendPresence = 1;
};
