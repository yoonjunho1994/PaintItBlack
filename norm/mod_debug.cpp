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
			p_session.get_jobnextexp());
        int height = p_renderer.get_height();
        int pos_height = height - (height - 200);
        g->print_screen(this->debug_output, p_renderer.get_width() - 170, pos_height);
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
    return 0;
}


