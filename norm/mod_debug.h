#pragma once
#include "mod.h"
#include "mod_graphics.h"
#include <string>

class debug : public norm_dll::mod {
public:
    debug(norm_dll::norm* c_state, std::shared_ptr<graphics> g);
    virtual ~debug();

    std::shared_ptr<graphics> g;

    int get_talk_type(char*, int*);
    HRESULT end_scene(IDirect3DDevice7**);
    int display_debug = 0;
	int display_memory = 0;
    std::string debug_fmt = "AID:         %ld\nGID:         %ld\nJob:         %d\nLevel:       %d\nExp:         %d\nNextExp:     %d\nJobLevel:    %d\nJobExp:      %d\nNextJobExp:  %d\nSkillPoints: %d";
    char debug_output[512];
	char mem_output[512];
	DWORD mem_address;
};
