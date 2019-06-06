#include "stdafx.h"

#include "mod_debug.h"
#include "mod_graphics.h"
#include "norm.h"

debug::debug(norm_dll::norm* c_state, std::shared_ptr<graphics> g)
    : mod(c_state)
    , g(g)
{
}

debug::~debug()
{
}

HRESULT debug::end_scene(IDirect3DDevice7** d3ddevice)
{
    //c_state->dbg_sock->do_send("overlay_new: end_scene");
    if (display_debug) {
        sprintf_s(this->debug_output, this->debug_fmt.c_str(), 
			p_session.get_aid(),
			p_session.get_gid(),
			p_session.get_job(),
			p_session.get_level(),
			p_session.get_exp(),
			p_session.get_next_exp(),
			p_session.get_joblevel(),
			p_session.get_jobexp(),
			p_session.get_jobnextexp(),
			p_session.get_skillpoints());
        int height = p_renderer.get_height();
        int pos_height = height - (height - 200);
        g->print_screen(this->debug_output, p_renderer.get_width() - 170, pos_height);
    }

	if (display_memory) {
		sprintf_s(this->mem_output, "Check at address 0x%x:\n", this->mem_address);
		for (int i = 0; i < 20; i++) {
			char buf[64];
			sprintf_s(buf, "+0x%x : %d\n", i * 4, *(DWORD*)(this->mem_address + i * 4));
			strcat_s(this->mem_output, buf);
		}
		int height = p_renderer.get_height();
        int pos_height = height - (height - 200);
		int width = p_renderer.get_width();
		int pos_width = width - (width - 200);
		g->print_screen(this->mem_output, pos_width, pos_height);
	}

    return S_OK;
}

int debug::get_talk_type(char* src, int* retval)
{
    if (strcmp(src, "/debug") == 0) {
        this->display_debug ^= 1;
        char buf[64];
        if (this->display_debug)
            sprintf_s(buf, "Debug is now displayed.");
        else
            sprintf_s(buf, "Debug is no longer displayed.");
        this->print_to_chat(buf);
        *retval = -1;
        return 1;
    }
	
	char tmp_src[125]; // max chat input length	
	strcpy_s(tmp_src, src);
	const char delimiter[] = " ";
	char* next_token;
	char* token = strtok_s(tmp_src, delimiter, &next_token);
	if (token && strcmp(token, "/showmem") == 0) {
		token = strtok_s(nullptr, delimiter, &next_token);

        char buf[64];
		if (!token) {
			this->display_memory = 0;
    		sprintf_s(buf, "Memory is no longer displayed.");
        	this->print_to_chat(buf);
		} else {
        	this->display_memory = 1;
        	sprintf_s(buf, "Memory is now displayed.");
        	this->print_to_chat(buf);
			this->mem_address = (DWORD)strtol(token, NULL, 16);
		}
        *retval = -1;
        return 1;
    }

    return 0;
}


