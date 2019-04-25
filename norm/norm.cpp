#include "stdafx.h"

#include "detours.h"
#include "norm.h"
#include "resource.h"
#include "verifier.h"

#include "hook_chat.h"
#include "hook_dx.h"
#include "hook_gamemode.h"
#include "hook_session.h"
#include "hook_socket.h"
#include "hook_user32.h"
#include "hook_renderer.h"

#include "mod_graphics.h"
#include "mod_overlay.h"
#include "mod_overlay_new.h"
#include "mod_statistics.h"
#include "mod_timestamp.h"
#include "mod_rpc.h"

#include <tchar.h>
#include <winhttp.h>
#include <fstream>

#pragma comment(lib, "winhttp.lib")

// Installs a mod based on the classname.
#define INSTALL_MOD(modname) mods.push_back(std::make_shared<modname>(this));

/* dll class */
namespace norm_dll {

norm::norm(HINSTANCE hInst)
    : hInst(hInst)
{
}

void norm::install_mods()
{
    // Disable a mod by commenting out the specific line.
    //INSTALL_MOD(overlay);
    //INSTALL_MOD(statistics);
    INSTALL_MOD(timestamp);
    INSTALL_MOD(overlay_new);
    INSTALL_MOD(graphics);
    INSTALL_MOD(rpc);
}

void norm::start()
{
    char info_buf[256];
    /* Connect to the debug socket */
    dbg_sock = std::make_shared<debug_socket>();

    int err = dbg_sock->do_connect();
    if (err != 0) {
        MessageBoxA(0, "Unable to connect to the debug socket!", "norm.dll error!", MB_OK);
        dbg_sock->disabled = 1;
    }
    dbg_sock->do_send("Hello Debugger!");
#ifdef CLIENT_VER
    sprintf_s(info_buf, "DLL compiled for: %d", CLIENT_VER);
#endif
#ifdef CLIENT_VER_RE
    sprintf_s(info_buf, "DLL compiled for: %dRE", CLIENT_VER_RE);
#endif
    dbg_sock->do_send(info_buf);

    /* Verify client date. */
    if (!this->verify_client())
        return;

	/* Check for drop-in cheat defender dll */
    if (!this->check_cheat_defender())
        return;

    /* Display splash screen. */
    this->show_splash();

    /* Hook functions. */
    err = DetourTransactionBegin();
    CHECK(info_buf, err);
    dbg_sock->do_send(info_buf);

    err = DetourUpdateThread(GetCurrentThread());
    CHECK(info_buf, err);
    dbg_sock->do_send(info_buf);

    //
    // Attach hooks and install mods
    //
    int total_hooks = 0;
    auto sptr = shared_from_this();
    total_hooks += chat_detour(sptr);
    total_hooks += socket_detour(sptr);
    //total_hooks += renderer_detour(sptr);
    total_hooks += session_detour(sptr);
    total_hooks += dx_detour(sptr);
    total_hooks += gamemode_detour(sptr);
    total_hooks += user32_detour(sptr);

	ProxyRenderer* p_renderer = new ProxyRenderer(sptr);
    p_renderer->hook();

    this->install_mods();

    err = DetourTransactionCommit();
    CHECK(info_buf, err);
    dbg_sock->do_send(info_buf);

    sprintf_s(info_buf, "Total hooks available: %d", total_hooks);
    dbg_sock->do_send(info_buf);

    //
    // patching .text section
    //
    // ping interval change from 12000 to 2000
    // Search for: 12000
#if CLIENT_VER == 20180919
    LPVOID hex_code = (LPVOID)0x0094BADE;
#elif (CLIENT_VER == 20180621 || CLIENT_VER == 20180620)
    LPVOID hex_code = (LPVOID)0x0094AB1E;
#elif CLIENT_VER == 20150000
    LPVOID hex_code = (LPVOID)0x0087344E;
#elif CLIENT_VER_RE == 20180621
    LPVOID hex_code = (LPVOID)0x00A054CE;
#endif
    DWORD old_protect;

    int ret = VirtualProtect(hex_code, 6, 0x04, &old_protect);
    sprintf_s(info_buf, "VirtualProtect ret val: %d", ret);
    dbg_sock->do_send(info_buf);

    char hex_code_str[128] = "Changing ASM instructions for ping interval:\n ";
    char tmp[4];
    for (int i = 0; i < 6; i++) {
        sprintf_s(tmp, "%x ", ((BYTE*)hex_code)[i]);
        strcat_s(hex_code_str, tmp);
    }
    strcat_s(hex_code_str, " -> ");

    for (int i = 0; i < 6; i++) {
        if (i == 2)
            ((BYTE*)hex_code)[i] = 0xD0;
        if (i == 3)
            ((BYTE*)hex_code)[i] = 0x07;
        sprintf_s(tmp, "%x ", ((BYTE*)hex_code)[i]);
        strcat_s(hex_code_str, tmp);
    }

    dbg_sock->do_send(hex_code_str);
    DWORD old_reset_protect;
    ret = VirtualProtect(hex_code, 6, old_protect, &old_reset_protect);
    sprintf_s(info_buf, "VirtualProtect reset ret val: %d", ret);
    dbg_sock->do_send(info_buf);
}

bool norm::verify_client()
{
    //
    // Checking for compatible client and produce debug message.
    //
    this->dbg_sock->do_send("Verifying PE compatibility ...");
    if (strcmp((char*)VERIFY_ADDR, VERIFY_STR) != 0) {
        char date_buf[512];
        this->dbg_sock->do_send("DLL and PE are not compatibile!");
#ifdef CLIENT_VER_RE
        sprintf_s(date_buf, "Client is not compatible with %dRE-norm.dll!", CLIENT_VER_RE);
#else
        sprintf_s(date_buf, "Client is not compatible with %d-norm.dll!", CLIENT_VER);
#endif
        strcat_s(date_buf, "\n\nSeaching for more debug information ...");
        strcat_s(date_buf, "\nIf the client crashes before any additional output is provided,");
        strcat_s(date_buf, "\nplease report it!");
        strcat_s(date_buf, "\nNOTE: Not compatible with NEMO Hide build info in client!");
        strcat_s(date_buf, "\n\n Your report should include your client 'exe'.");
        //strcat_s(date_buf, "\n Note: nemo patch 'Hide build info' is currently not supported!");
        MessageBoxA(0, date_buf, "norm.dll error!", MB_OK);

        // Search for the real clientdate.
        // Greedy/dumb search.
        // TODO: crashes ACCESS VIOLATION if string is not in binary.
        DWORD search_addr = 0x00401000;
        for (;;) {
            if (strncmp("\\RagnarokClient", (char*)search_addr, 15) == 0) {
                char error_buf[512];

                // Search the date.
                DWORD found_addr = search_addr;
                search_addr--;
                while ('\\' != ((char*)search_addr)[0])
                    search_addr--;

                int length = found_addr - search_addr - 1 - RO_offset;
                DWORD string_start = search_addr + 1 + RO_offset;

                // Search for RE or nonRE
                search_addr = found_addr;
                while ('.' != ((char*)search_addr)[0])
                    search_addr++;

                DWORD end = search_addr;
                while ('\\' != ((char*)search_addr)[0])
                    search_addr--;

                DWORD begin = search_addr + 1;
                int ragexe_strlen = end - begin + 1;

                char ragexe_str[16];
                snprintf(ragexe_str, ragexe_strlen, "%s", (char*)begin);

                // Search for the begin of the path.
                search_addr = found_addr;
                while (strncmp("D:\\", (char*)search_addr, 3) != 0)
                    search_addr--;

                char* pdb_path = (char*)(search_addr + 3);

                // Create output.
                char tmp_str[] = "The following information has been found: \n\nClientdate: ";
                snprintf(error_buf, strlen(tmp_str) + length + 1, "%s%s", tmp_str, (char*)string_start);
                strncat_s(error_buf, ragexe_str, ragexe_strlen);
                strcat_s(error_buf, "\nPDB: ");
                strncat_s(error_buf, pdb_path, strlen(pdb_path) - 4);
                strcat_s(error_buf, "\nDLL: ");
                strcat_s(error_buf, DLL_VER);
                strcat_s(error_buf, "\n\nIf this debug message does not help you solve your problem then please ");
                strcat_s(error_buf, "report this message!");
                MessageBoxA(0, error_buf, "norm.dll error!", MB_OK);
                break;
            }
            search_addr++;
        }
        return false;
    }
    this->dbg_sock->do_send("Success!");
    return true;
}

void norm::hide_splash()
{
    logo.Hide();
    pib.Hide();
}

void norm::show_splash()
{
    int offset = 0;
    HBITMAP bmp_logo = (HBITMAP)::LoadImage(NULL, _T("logo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    auto bmp_pib = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
    if (bmp_logo) {
        BITMAP binfo_logo;
        BITMAP binfo_pib;
        GetObject(bmp_logo, sizeof(BITMAP), &binfo_logo);
        GetObject(bmp_pib, sizeof(BITMAP), &binfo_pib);
        if (binfo_logo.bmWidth <= binfo_pib.bmWidth) { /* logo width is not allowed be larger then the pib logo*/
            HWND hWnd_logo = ::CreateWindowA("STATIC", "splash", WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) / 2) + splash_offset, (GetSystemMetrics(SM_CYSCREEN) / 2), 0, 0, NULL, NULL, NULL, NULL);
            this->logo.Init(hWnd_logo, this->hInst, bmp_logo);
            offset = -splash_offset - 1;
        }
    }

    HWND hWnd_pib = ::CreateWindowA("STATIC", "splash", WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) / 2) + offset, (GetSystemMetrics(SM_CYSCREEN) / 2), 0, 0, NULL, NULL, NULL, NULL);
    this->pib.Init(hWnd_pib, this->hInst, bmp_pib);

    this->pib.Show();
    this->logo.Show();
    Sleep(1000); // Display the logo for atleast 1 second.
}

bool norm::check_cheat_defender()
{
    std::ifstream fp("CDClient.dll");

    /* Client does not have cheat defender. Skipping. */
    if (!fp.is_open())
        return true;

	std::string str((std::istreambuf_iterator<char>(fp)),
        std::istreambuf_iterator<char>());

	std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(str);
	
	char info_buf[64];
    sprintf_s(info_buf, "Cheat Defender hash: %zx", str_hash);
    this->dbg_sock->do_send(info_buf);

	if (str_hash != 0x2a4bd5ce) {
        char err_buf[512];
        sprintf_s(err_buf, "Detected Cheat-Defender!\n\n");
        strcat_s(err_buf, "The found file is not compatible.\n");
        strcat_s(err_buf, "Please use a dropin replacement! It can be found here:\n\n");
        strcat_s(err_buf, "https://rathena.org/board/topic/118729-client-mods-paint-it-black/\nor\n");
        strcat_s(err_buf, "https://gitlab.com/4144/Nemo/blob/master/Input/CDClient.dll\n\n");
        strcat_s(err_buf, "SHA-1: 32158C097CD656FF62BB094CF9F4F2BF1A6B1CAD");
        MessageBoxA(0, err_buf, "norm.dll error!", MB_OK);
        return false;
    }
    return true;
}

norm::~norm()
{
}
}