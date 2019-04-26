#pragma once
#include "client_ver.h"
#include "singleton.h"
#include "mod.h"
#include "norm.h"

ULONG renderer_get_width();
ULONG renderer_get_height();
int renderer_get_fps();


class ProxyRenderer final : public Singleton<ProxyRenderer> {
private:
    std::shared_ptr<norm_dll::norm> c_state;

	void init() { c_renderer = reinterpret_cast<CRenderer*>(RENDERER_PTR); }

    struct CRenderer {
        BYTE OFFSET_0;
        ULONG width;
        ULONG height;	
        BYTE OFFSET_1;
        int fps;
    };

    CRenderer* c_renderer = nullptr;
    lpDrawScene DrawScene = reinterpret_cast<lpDrawScene>(DRAWSCENE_FN);

	bool hooked = false;

    static bool __fastcall pDrawScene(void* this_obj);

	ProxyRenderer() {}

public:
    ProxyRenderer(token) {}
    ~ProxyRenderer() {}

    void hook(std::shared_ptr<norm_dll::norm> c_state);
	ULONG get_width();
	ULONG get_height();
    int get_fps();
};

