#pragma once
#include "stdafx.h"
#include <stdio.h>

#define CLIENT_VER 20150000 /* needs to fit the client date of the exe. */
//#define CLIENT_VER_RE 20180621
#define DEBUG /* requires a debug-server listening on localhost:1337 */

/* simple macro for some debug output (used with the debug server) */
#define CHECK(buf, errno_) (errno_ == 0) ? sprintf_s(buf, "Success at Line %s:%d", __FILE__, __LINE__) : sprintf_s(buf, "Error %d at %s:%d", errno_, __FILE__, __LINE__)

/* disable mods by undefining OUTDATED*/
//#define COMENC	/* requires src edit to enable "encryption" on server side. */

#if defined(CLIENT_VER) && defined(CLIENT_VER_RE)
#undef CLIENT_VER_RE
#endif

#if CLIENT_VER == 20180919
#define DLL_VER "RO_2018-09-19Ragexe"
#include "20180921.h"

#elif CLIENT_VER == 20180621
#define DLL_VER "RO_2018-06-21Ragexe"

#elif CLIENT_VER == 20150000
#define DLL_VER "RO_2015-05-13Ragexe"
#include "20150513.h"

#elif CLIENT_VER == 20180620
#define DLL_VER "RO_2018-06-20#2Ragexe"

#elif CLIENT_VER_RE == 20180621
#define DLL_VER "RO_2018-06-21RagexeRE"

#endif