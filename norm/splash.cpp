#include "stdafx.h"

#include "splash.h"


splash::splash()
{
}

splash::~splash()
{
    DestroyWindow(hSplashWnd);
}

void splash::Init(HWND hWnd, HINSTANCE hInst, HBITMAP bmp)
{
    hParentWindow = hWnd;
    hSplashWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST, L"STATIC", L"",
        WS_POPUP | SS_BITMAP, 0, 0, 0, 0, hWnd, NULL, hInst, NULL);
    //HBITMAP bmp = (HBITMAP)::LoadImage(NULL, _T("logo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //auto bmp = LoadBitmap(hInst, MAKEINTRESOURCE(resid));
    SetLayeredWindowAttributes(hSplashWnd, RGB(0xff, 0x0, 0xff), 0, LWA_COLORKEY);
    SendMessage(hSplashWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp);
    this->SHOWING = FALSE;
}

void splash::Show()
{
    if (!hSplashWnd)
        return;
    //get size of hSplashWnd (width and height)
    int x, y;
    int pwidth, pheight;
    int tx, ty;
    HDWP windefer;
    RECT rect, prect;
    GetClientRect(hSplashWnd, &rect);
    x = rect.right;
    y = rect.bottom;
    //get parent screen coordinates
    GetWindowRect(this->hParentWindow, &prect);

    //center splash window on parent window
    pwidth = prect.right - prect.left;
    pheight = prect.bottom - prect.top;

    tx = (pwidth / 2) - (x / 2);
    ty = (pheight / 2) - (y / 2);

    tx += prect.left;
    ty += prect.top;

    windefer = BeginDeferWindowPos(1);
    DeferWindowPos(windefer, hSplashWnd, HWND_NOTOPMOST, tx, ty, 50, 50, SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOZORDER);
    EndDeferWindowPos(windefer);

    ShowWindow(hSplashWnd, SW_SHOWNORMAL);
    UpdateWindow(hSplashWnd);
    this->SHOWING = TRUE;
}

void splash::Hide()
{
    if (!hSplashWnd)
        return;
    ShowWindow(hSplashWnd, SW_HIDE);
    this->SHOWING = FALSE;
}
