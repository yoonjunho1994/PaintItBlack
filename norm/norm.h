#pragma once
#include <memory>
#include <vector>
#include "debug_socket.h"
#include "mod.h"
namespace norm_dll {

class norm : public std::enable_shared_from_this<norm> {
	public:
		/* Constatns from the client*/
		DWORD g_renderer;
		DWORD g_window_mgr = 0;

		/* Screen height and width*/
		ULONG m_width = -1;
		ULONG m_height = -1;

		std::shared_ptr<debug_socket>dbg_sock;
		std::vector<std::shared_ptr<mod>> mods;

		norm();
		virtual ~norm();
		void start();

	private:
		void install_mods();
		
	};
}