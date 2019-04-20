#pragma once
#include "client_ver.h"
#include "debug_socket.h"
#define INITGUID
#include <d3d.h>

#pragma warning(disable : 26440) // Suppress "noexcept" warning

namespace norm_dll {
class norm;
class mod : public std::enable_shared_from_this<mod> {
public:
    mod(norm* c_state)
        : c_state(c_state)
    {
    }
    virtual ~mod() {}
    virtual void draw_scene(void*) {}
#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
    virtual void send_msg(void**, int*, void**, void**, int*, int*)
    {
    }
    virtual int get_talk_type(void**, void**, int*, int*, int*) { return 0; }
#elif CLIENT_VER == 20150000
    virtual void send_msg(void**, int*, int*, int*, int*, int*)
    {
    }
    virtual int get_talk_type(void**, char**, int*, char**, int*) { return 0; }
#endif
    virtual HRESULT begin_scene(IDirect3DDevice7**)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT end_scene(IDirect3DDevice7**) { return E_NOTIMPL; }
    virtual HRESULT WaitForVerticalBlank(DWORD*, HANDLE*) { return E_NOTIMPL; };
    virtual void ddraw_release() {}

protected:
    norm* c_state;
    void print_to_chat(char* msg);

private:
    int active = -1;
};
}
