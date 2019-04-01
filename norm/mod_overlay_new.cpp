#include "stdafx.h"
#include "mod_overlay_new.h"
#include "hook_session.h"
#include "hook_renderer.h"
#include "norm.h"

#define D3DCOLOR_ARGB(a,r,g,b) \
((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))


overlay_new::overlay_new(norm_dll::norm* c_state) : mod(c_state)
{
}


overlay_new::~overlay_new()
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
	if (!(pddpf->dwFlags&DDPF_ALPHAPIXELS))
		return DDENUMRET_OK;
	// get 16 bit with alphapixel format
	memcpy((DDPIXELFORMAT*)param, pddpf, sizeof(DDPIXELFORMAT));
	return DDENUMRET_CANCEL;
}

void overlay_new::init(IDirect3DDevice7* d3ddevice)
{
	if (initialized != 0)
		return;
	D3DDEVICEDESC7 ddDesc;
	d3ddevice->GetCaps(&ddDesc);

	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH |
		DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
	ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	ddsd.dwWidth = 512;
	ddsd.dwHeight = 512;

	if (ddDesc.deviceGUID == IID_IDirect3DHALDevice) {
		ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	}
	else if (ddDesc.deviceGUID == IID_IDirect3DTnLHalDevice) {
		ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	}
	else {
		ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
	}

	d3ddevice->EnumTextureFormats(TextureSearchCallback, &ddsd.ddpfPixelFormat);
	if (ddsd.ddpfPixelFormat.dwRGBBitCount) {
		LPDIRECTDRAWSURFACE7 pddsRender = NULL;
		LPDIRECTDRAW7        pDD = NULL;

		d3ddevice->GetRenderTarget(&pddsRender);
		if (pddsRender) {
			pddsRender->GetDDInterface((VOID**)&pDD);
			pddsRender->Release();
		}
		if (pDD) {
			if (SUCCEEDED(pDD->CreateSurface(&ddsd, &m_pddsFontTexture, NULL))) {
				c_state->dbg_sock->do_send("created font texture");
			}
			else {
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

HRESULT overlay_new::begin_scene(IDirect3DDevice7** instance)
{
	//c_state->dbg_sock->do_send("overlay_new: begin_scene");
	this->init(*instance);
	return S_OK;
}

HRESULT overlay_new::end_scene(IDirect3DDevice7** d3ddevice)
{
	c_state->dbg_sock->do_send("overlay_new: end_scene");
	if (!m_pSFastFont || !m_pddsFontTexture)
		return E_ABORT;

	if (display_ping) {
		char ping_str[32];
		sprintf_s(ping_str, "Ping: %ld ms", session_get_averagePingTime());
		/*char test_str[] = "Test text!";
		for (int x = 0; x < 10; x++) {
			for (int y = 0; y < 10; y++) {
				m_pSFastFont->DrawText(test_str, x * 100 + 100, y * 100 + 100, D3DCOLOR_ARGB(255, 255, 255, 0), 0, NULL);
			}
		}*/
		c_state->dbg_sock->do_send(ping_str);

		m_pSFastFont->DrawText(ping_str, this->x, this->y, D3DCOLOR_ARGB(255, 255, 255, 0), 0, NULL);
	}

	if (display_fps) {
		char fps_str[32];
		sprintf_s(fps_str, "FPS: %d", renderer_get_fps());
		m_pSFastFont->DrawText(fps_str, this->x, this->y + 20, D3DCOLOR_ARGB(255, 255, 255, 0), 0, NULL);
	}

	if (display_ping || display_fps)
		m_pSFastFont->Flush();
	return S_OK;
}

#if (CLIENT_VER == 20180620 || CLIENT_VER == 20180621) 
int overlay_new::get_talk_type(void **this_obj, void **src, int *a1, int *a2, int* retval)
#elif CLIENT_VER == 20150000
int overlay_new::get_talk_type(void**this_obj, char** src, int* a1, char** a2, int* retval)
#endif
{
	if (strcmp((char*)*src, "/pingo2") == 0) {
		this->display_ping ^= 1;
		char buf[64];
		if (this->display_ping)
			sprintf_s(buf, "Ping2 is now displayed.");
		else
			sprintf_s(buf, "Ping2 is no longer displayed.");
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
		int screen_width = (int)renderer_get_width();
		int screen_height = (int)renderer_get_height();
		this->x = (int)(screen_width - 115);
		this->y = (int)(screen_height - (screen_height - 160));
		char buf[32];
		sprintf_s(buf, "Width: %d | Height: %d", screen_width, screen_height);
		c_state->dbg_sock->do_send(buf);
	}
}

void overlay_new::ddraw_release()
{
	if (m_pSFastFont)
		delete m_pSFastFont;
	if (m_pddsFontTexture) {
		m_pddsFontTexture->Release();
		m_pddsFontTexture = NULL;
	}
}
