#pragma once

#include <windows.h>
#include <string>
#include <functional>

class AppWindow {
public:
    AppWindow(HINSTANCE hInstance, int nCmdShow);
    ~AppWindow();

    bool Create();
    void Run();
    HWND GetHandle() const { return m_hwnd; }
    
    void SetWebViewReadyCallback(std::function<void()> callback);

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    HINSTANCE m_hInstance;
    HWND m_hwnd;
    int m_nCmdShow;
    std::function<void()> m_webViewReadyCallback;

    static constexpr wchar_t CLASS_NAME[] = L"SkillshareSyncWindowClass";
    static constexpr wchar_t WINDOW_TITLE[] = L"Skillshare Sync";
    static constexpr int WINDOW_WIDTH = 1280;
    static constexpr int WINDOW_HEIGHT = 800;
};
