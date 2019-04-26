#pragma once
#include "norm.h"
class rpc : public norm_dll::mod {
public:
    rpc(norm_dll::norm* c_state);
    ~rpc();

    void updateDiscordPresence();

	void init();

    int get_talk_type(void**, void**, int*, int*, int*);
    int get_talk_type(void**, char**, int*, char**, int*);
    void draw_scene(void* this_obj);

    const char* APPLICATION_ID = "570835350193176579";
    int FrustrationLevel = 0;
    int64_t StartTime;
    int SendPresence = 1;
};