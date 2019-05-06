#pragma once
#include <memory>
#include <vector>

#include "debug_socket.h"
#include "mod.h"
#include "splash.h"

namespace norm_dll {
constexpr auto RO_offset = 3; /* RO_2018-.... removes RO_*/
constexpr auto splash_offset = 152 + 5; /* Logo length / 2 and + 10 for some gap*/

class norm : public std::enable_shared_from_this<norm> {
	public:
		std::shared_ptr<debug_socket>dbg_sock;
		std::vector<std::shared_ptr<mod>> mods;

		norm(HINSTANCE hInst);
		virtual ~norm();
		void start();
        void hide_splash();

	private:
        bool check_cheat_defender();
        bool verify_client();
        void show_splash();
        HINSTANCE hInst;
		void install_mods();
        splash logo;
        splash pib;
	};
}