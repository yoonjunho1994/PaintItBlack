#pragma once

class splash {
public:
    void Hide();
    void Show();
    void Init(HWND hWnd, HINSTANCE hInst, HBITMAP bmp);
    BOOL SHOWING;
    splash();
    virtual ~splash();

private:
    UINT TimerID;
    HWND hParentWindow;
    HWND hSplashWnd;
};
