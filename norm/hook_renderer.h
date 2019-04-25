#pragma once
#include "client_ver.h"
#include "mod.h"
#include "norm.h"

ULONG renderer_get_width();
ULONG renderer_get_height();
int renderer_get_fps();


class ProxyRenderer {
private:
    std::shared_ptr<norm_dll::norm> c_state;

	void init() { c_renderer = RENDERER_PTR; }

    class CRenderer {
    public:
        BYTE OFFSET_0;
        ULONG width;
        ULONG height;	
        BYTE OFFSET_1;
        int fps;
    };

    CRenderer* c_renderer = nullptr;
    lpDrawScene DrawScene = DRAWSCENE_FN;

	bool hooked = false;

    static bool __fastcall pDrawScene(void* this_obj);

public:
    ProxyRenderer(std::shared_ptr<norm_dll::norm> c_state)
        : c_state(c_state)
    {
        instance = this;
    }

    void hook();
    
	/* getter */
	ULONG get_width();
	ULONG get_height();
    int get_fps();

	static ProxyRenderer* instance;
};
