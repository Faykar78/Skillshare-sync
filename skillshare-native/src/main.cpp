#include "app_window.h"
#include "webview_handler.h"
#include "downloader.h"
#include <memory>

std::unique_ptr<WebViewHandler> g_webviewHandler;
std::unique_ptr<Downloader> g_downloader;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        MessageBoxW(nullptr, L"Failed to initialize COM", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Check if yt-dlp is available
    if (!Downloader::IsYtDlpAvailable()) {
        MessageBoxW(nullptr, 
            L"yt-dlp not found!\n\n"
            L"Please install yt-dlp and ensure it's in your PATH.\n\n"
            L"Install via pip: pip install yt-dlp\n"
            L"Or download from: https://github.com/yt-dlp/yt-dlp/releases",
            L"Skillshare Sync - Missing Dependency", 
            MB_OK | MB_ICONWARNING);
    }

    // Create main window
    AppWindow appWindow(hInstance, nCmdShow);
    if (!appWindow.Create()) {
        MessageBoxW(nullptr, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        CoUninitialize();
        return 1;
    }

    // Create downloader
    g_downloader = std::make_unique<Downloader>();
    g_downloader->SetCompleteCallback([](bool success, const std::wstring& message) {
        if (success) {
            MessageBoxW(nullptr, L"Download complete!", L"Skillshare Sync", MB_OK | MB_ICONINFORMATION);
        } else {
            MessageBoxW(nullptr, message.c_str(), L"Download Failed", MB_OK | MB_ICONERROR);
        }
    });

    // Create WebView handler
    g_webviewHandler = std::make_unique<WebViewHandler>(appWindow.GetHandle());
    g_webviewHandler->SetM3U8Callback([&appWindow](const std::wstring& url) {
        // M3U8 URL detected - download button will be injected by WebViewHandler
    });

    if (!g_webviewHandler->Initialize()) {
        MessageBoxW(nullptr, 
            L"Failed to initialize WebView2.\n\n"
            L"Please ensure WebView2 Runtime is installed.\n"
            L"Download from: https://developer.microsoft.com/en-us/microsoft-edge/webview2/",
            L"Error", MB_OK | MB_ICONERROR);
        CoUninitialize();
        return 1;
    }

    // Navigate to Skillshare
    g_webviewHandler->Navigate(L"https://www.skillshare.com/");

    // Run message loop
    appWindow.Run();

    // Cleanup
    g_webviewHandler.reset();
    g_downloader.reset();
    CoUninitialize();

    return 0;
}
