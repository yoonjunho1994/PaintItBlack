#pragma once
#include "stdafx.h"
#include "client_ver.h"
#include "debug_socket.h"
#include "norm.h"

int user32_detour(std::shared_ptr<norm_dll::norm> c_state);
