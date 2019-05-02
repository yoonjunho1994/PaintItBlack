#include "stdafx.h"
#include "mod_overlay_new.h"

#include "hook_renderer.h"
#include "norm.h"


overlay_new::overlay_new(norm_dll::norm* c_state, std::shared_ptr<graphics> g)
    : mod(c_state)
    , g(g)
{
}

overlay_new::~overlay_new()
{
}

HRESULT overlay_new::end_scene(IDirect3DDevice7** d3ddevice)
{
	//c_state->dbg_sock->do_send("overlay_new: end_scene");
	if (display_ping) {
		char ping_str[32];
        ULONG ping = p_session.get_average_ping_time();
		if (ping == 0)
			sprintf_s(ping_str, "Ping: wait...");
		else
			sprintf_s(ping_str, "Ping: %ld ms", ping);
        g->print_screen(ping_str, this->x, this->y);
	}

	if (display_fps) {
		char fps_str[32];
		sprintf_s(fps_str, "FPS: %d", renderer_get_fps());
		g->print_screen(fps_str, this->x, this->y + 20);
    }

	return S_OK;
}

#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
int overlay_new::get_talk_type(void **this_obj, void **src, int *a1, int *a2, int* retval)
#elif CLIENT_VER == 20150000
int overlay_new::get_talk_type(void**this_obj, char** src, int* a1, char** a2, int* retval)
#endif
{
	if (strcmp((char*)*src, "/ping") == 0) {
		this->display_ping ^= 1;
		char buf[64];
		if (this->display_ping)
			sprintf_s(buf, "Ping is now displayed.");
		else
			sprintf_s(buf, "Ping is no longer displayed.");
		this->print_to_chat(buf);
		*retval = -1;
		return 1;
	}

	if (strcmp((char*)*src, "/fps") == 0) {
		this->display_fps ^= 1;
		char buf[64];
		if (this->display_fps)
			sprintf_s(buf, "FPS is now displayed.");
		else
			sprintf_s(buf, "FPS is no longer displayed.");
		this->print_to_chat(buf);
		*retval = -1;
		return 1;
	}
	return 0;
}

// we get the screen dimension when we are ingame.
void overlay_new::draw_scene(void* this_obj)
{
	if (this->x == -1 && this->y == -1) {
		c_state->dbg_sock->do_send("Trying to get screen size!");
        //ProxyRenderer::instance->get_height();
		int screen_width = (int)renderer_get_width();
		int screen_height = (int)renderer_get_height();
		this->x = (int)(screen_width - 145);
		this->y = (int)(screen_height - (screen_height - 160));
		char buf[32];
		sprintf_s(buf, "Width: %d | Height: %d", screen_width, screen_height);
		c_state->dbg_sock->do_send(buf);
	}
}
