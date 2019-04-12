#include "stdafx.h"
#include "CProxyIDirectDraw7.h"

CProxyIDirectDraw7* CProxyIDirectDraw7::lpthis;

HRESULT CProxyIDirectDraw7::Proxy_WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent)
{
	for (auto it : this->c_state->mods)
		if (it->WaitForVerticalBlank(&dwFlags, &hEvent) == DD_OK)
			return DD_OK;
	return m_Instance->WaitForVerticalBlank(dwFlags, hEvent);
}

HRESULT CProxyIDirectDraw7::Proxy_QueryInterface(THIS_ REFIID riid, LPVOID FAR * ppvObj)
{
	
	if (IsEqualGUID(riid, IID_IDirect3D7)) {
		this->c_state->dbg_sock->do_send("Proxy_QueryInterface 'IsEqualGUID'");
		HRESULT temp_ret = m_Instance->QueryInterface(riid, ppvObj);
		if (temp_ret == S_OK) {
			void *ret_cProxy;
			//LPVOID FAR * ppvObj_proxy;
			ret_cProxy = new CProxyIDirect3D7((IDirect3D7*)*ppvObj, c_state);
			*ppvObj = ret_cProxy;
			this->pd3d = (CProxyIDirect3D7*)ret_cProxy;

			return S_OK;
		}
		else {
			return temp_ret;
		}
	}

	return m_Instance->QueryInterface(riid, ppvObj);
}

HRESULT CProxyIDirectDraw7::Proxy_CreateSurface(LPDDSURFACEDESC2 SurfaceDesc,
	LPDIRECTDRAWSURFACE7 FAR * CreatedSurface, IUnknown FAR * pUnkOuter)
{

	HRESULT Result = m_Instance->CreateSurface(SurfaceDesc, CreatedSurface, pUnkOuter);
	if (FAILED(Result))
		return Result;

	if (SurfaceDesc->dwFlags & DDSD_CAPS) {
		DDSCAPS2* Caps = &SurfaceDesc->ddsCaps;
		if (Caps->dwCaps & DDSCAPS_PRIMARYSURFACE) {
			this->c_state->dbg_sock->do_send("Primary Surface");
			PrimarySurfaceFlag = 1;
		}
		else
			if (Caps->dwCaps & DDSCAPS_3DDEVICE) {
				if (CooperativeLevel & DDSCL_FULLSCREEN && !PrimarySurfaceFlag) {
					;
				}
				else {
					TargetSurface = *CreatedSurface;
					this->c_state->dbg_sock->do_send("Target Surface");
					//SetScreenSize( (int)SurfaceDesc->dwWidth , (int)SurfaceDesc->dwHeight );
				}
			}
			else {
				;
			}
	}
}

CProxyIDirectDraw7::~CProxyIDirectDraw7()
{
}

ULONG  CProxyIDirectDraw7::Proxy_Release()
{
	//c_state->dbg_sock->do_send("Releasing ddraw");
	for (auto it : c_state->mods)
		it->ddraw_release();
	ULONG count = m_Instance->Release();
	delete this;

	return count;
}