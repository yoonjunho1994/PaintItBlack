#include "stdafx.h"

#include "hook_renderer.h"

#include "detours.h"
#include "norm.h"

#pragma warning(disable : 26440) // Suppress "noexcept" warning

ULONG renderer_get_width()
{
    return norm_dll::ProxyRenderer::instance().get_width();
}

ULONG renderer_get_height()
{
    return norm_dll::ProxyRenderer::instance().get_height();
}

int renderer_get_fps()
{
    return norm_dll::ProxyRenderer::instance().get_fps();
}

namespace norm_dll {
void ProxyRenderer::hook(std::shared_ptr<norm_dll::norm> c_state)
{
    if (hooked)
        return;

	this->c_state = c_state;

    LONG err = 0;
    int hook_count = 0;
    char info_buf[256];

    err = DetourAttach(&(LPVOID&)DrawScene, pDrawScene);
    CHECK(info_buf, err);
    if (err == NO_ERROR) {
        hook_count++;
    } else
        this->c_state->dbg_sock->do_send(info_buf);

    sprintf_s(info_buf, "Renderer hooks available: %d", hook_count);
    this->c_state->dbg_sock->do_send(info_buf);

	this->hooked = true;
}

bool __fastcall ProxyRenderer::pDrawScene(void* this_obj)
{
    auto& instance = ProxyRenderer::instance();
    if (!instance.c_renderer)
        instance.init();

    bool res = (instance.DrawScene)(this_obj);

    for (auto callback : instance.c_state->mods)
        callback->draw_scene(this_obj);

    return res;
}

#define AV_ERR MessageBoxA(0, "CRenderer was accesed before initated", "norm.dll warning!", MB_OK)

ULONG ProxyRenderer::get_width()
{
	if (c_renderer)
		return c_renderer->width;
    AV_ERR;
    return 0;
}

ULONG ProxyRenderer::get_height()
{
    if (c_renderer)
        return c_renderer->height;
    AV_ERR;
    return 0;
}

int ProxyRenderer::get_fps()
{
    if (c_renderer)
        return c_renderer->fps;
    AV_ERR;
    return 0;
}

#undef AV_ERR
}