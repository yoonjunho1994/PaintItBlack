#include "stdafx.h"
#include "CProxyIDirect3DDevice7.h"

void CProxyIDirect3DDevice7::Proxy_Release(void)
{
}

HRESULT CProxyIDirect3DDevice7::Proxy_BeginScene(void)
{
	HRESULT result;
	//this->c_state->dbg_sock->do_send("CProxyIDirect3D7::Proxy_BeginScene()");
	for (auto it : this->c_state->mods)
		it->begin_scene(&m_Instance);

	result = m_Instance->BeginScene();

	return result;
}

HRESULT CProxyIDirect3DDevice7::Proxy_EndScene(void)
{
	HRESULT result;
	//this->c_state->dbg_sock->do_send("CProxyIDirect3D7::Proxy_EndScene()");
	for (auto it : this->c_state->mods)
		it->end_scene(&m_Instance);

	result = m_Instance->EndScene();

	return result;
}

void CProxyIDirect3DDevice7::add_beginSecene_callback(std::shared_ptr<norm_dll::mod> cb)
{
	//this->beginScene_callbacks.push_back(cb);
}

void CProxyIDirect3DDevice7::add_endSecene_callback(std::shared_ptr<norm_dll::mod> cb)
{
	//this->endScene_callbacks.push_back(cb);
}
