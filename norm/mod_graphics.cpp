#include "stdafx.h"

#include "mod_graphics.h"
#include "norm.h"


graphics::graphics(norm_dll::norm* c_state) : mod(c_state)
{
}

graphics::~graphics()
{
}

static HRESULT CALLBACK TextureSearchCallback(DDPIXELFORMAT* pddpf,
    VOID* param)
{
    if (pddpf->dwFlags & (DDPF_LUMINANCE | DDPF_BUMPLUMINANCE | DDPF_BUMPDUDV))
        return DDENUMRET_OK;
    if (pddpf->dwFourCC != 0)
        return DDENUMRET_OK;
    if (pddpf->dwRGBBitCount != 16)
        return DDENUMRET_OK;
    if (!(pddpf->dwFlags & DDPF_ALPHAPIXELS))
        return DDENUMRET_OK;
    // get 16 bit with alphapixel format
    memcpy((DDPIXELFORMAT*)param, pddpf, sizeof(DDPIXELFORMAT));
    return DDENUMRET_CANCEL;
}

void graphics::init(IDirect3DDevice7* d3ddevice)
{
    if (initialized != 0)
        return;
    D3DDEVICEDESC7 ddDesc;
    d3ddevice->GetCaps(&ddDesc);

    DDSURFACEDESC2 ddsd;
    ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    ddsd.dwWidth = 512;
    ddsd.dwHeight = 512;

    if (ddDesc.deviceGUID == IID_IDirect3DHALDevice) {
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    } else if (ddDesc.deviceGUID == IID_IDirect3DTnLHalDevice) {
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    } else {
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    }

    d3ddevice->EnumTextureFormats(TextureSearchCallback, &ddsd.ddpfPixelFormat);
    if (ddsd.ddpfPixelFormat.dwRGBBitCount) {
        LPDIRECTDRAWSURFACE7 pddsRender = NULL;
        LPDIRECTDRAW7 pDD = NULL;

        d3ddevice->GetRenderTarget(&pddsRender);
        if (pddsRender) {
            pddsRender->GetDDInterface((VOID**)&pDD);
            pddsRender->Release();
        }
        if (pDD) {
            if (SUCCEEDED(pDD->CreateSurface(&ddsd, &m_pddsFontTexture, NULL))) {
                c_state->dbg_sock->do_send("created font texture");
            } else {
                c_state->dbg_sock->do_send("failed create a font texture");
            }
            pDD->Release();
        }
        if (m_pddsFontTexture) {
            LOGFONT logfont;
            logfont.lfHeight = -12;
            logfont.lfWidth = 0;
            logfont.lfEscapement = 0;
            logfont.lfOrientation = 0;
            logfont.lfWeight = FW_REGULAR;
            //
            logfont.lfItalic = FALSE;
            logfont.lfUnderline = FALSE;
            logfont.lfStrikeOut = FALSE;
            logfont.lfCharSet = DEFAULT_CHARSET;
            logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
            logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
            logfont.lfQuality = NONANTIALIASED_QUALITY;
            logfont.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
            wcscpy_s(logfont.lfFaceName, L"‚l‚r ‚oƒSƒVƒbƒN");

            m_pSFastFont = new CSFastFont;
            m_pSFastFont->CreateFastFont(&logfont, d3ddevice, m_pddsFontTexture, 0);
            initialized++;
        }
    }
}

#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
int graphics::get_talk_type(void** this_obj, void** src, int* a1, int* a2, int* retval)
#elif CLIENT_VER == 20150000
int graphics::get_talk_type(void** this_obj, char** src, int* a1, char** a2, int* retval)
#endif
{
    // debug: c_state->dbg_sock->do_send((char*)*src);
    if (strcmp((char*)*src, "/vsync") == 0) {
        char buf[64];
        this->vsync_active ^= 1;
        if (this->vsync_active)
            sprintf_s(buf, "VSync is now enabled.");
        else
            sprintf_s(buf, "VSync is now disabled.");
        this->print_to_chat(buf);
        *retval = -1;
        return 1;
    }
    return 0;
}

HRESULT graphics::begin_scene(IDirect3DDevice7** instance)
{
    //c_state->dbg_sock->do_send("overlay_new: begin_scene");
    this->init(*instance);
    return S_OK;
}

HRESULT graphics::WaitForVerticalBlank(DWORD*, HANDLE*)
{
    if (!this->vsync_active)
        return DD_OK;
    return DD_FALSE;
}

void graphics::ddraw_release()
{
    if (m_pSFastFont)
        delete m_pSFastFont;
    if (m_pddsFontTexture) {
        m_pddsFontTexture->Release();
        m_pddsFontTexture = NULL;
    }
}

#define D3DCOLOR_ARGB(a, r, g, b) \
    ((D3DCOLOR)((((a)&0xff) << 24) | (((r)&0xff) << 16) | (((g)&0xff) << 8) | ((b)&0xff)))

void graphics::print_screen(char* str, int x, int y, int a, int r, int g, int b)
{
    m_pSFastFont->DrawText(str, x, y, D3DCOLOR_ARGB(a, r, g, b), 0, NULL);
}

HRESULT graphics::end_scene(IDirect3DDevice7** d3ddevice)
{
    m_pSFastFont->Flush();
    return S_OK;
}
