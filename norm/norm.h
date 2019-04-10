#pragma once
#include <memory>
#include <vector>
#include "debug_socket.h"
#include "mod.h"
namespace norm_dll {
constexpr auto RO_offset = 3; /* RO_2018-.... removes RO_*/

class norm : public std::enable_shared_from_this<norm> {
	public:
		std::shared_ptr<debug_socket>dbg_sock;
		std::vector<std::shared_ptr<mod>> mods;

		norm();
		virtual ~norm();
		void start();

	private:
		void install_mods();
		
	};
}