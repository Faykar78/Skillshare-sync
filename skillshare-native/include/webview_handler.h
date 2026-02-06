#pragma once

#include <windows.h>
#include <wrl.h>
#include <wrl/client.h>
#include <wil/com.h>
#include <WebView2.h>
#include <string>
#include <functional>

class WebViewHandler {
public:
    WebViewHandler(HWND parentWindow);
    ~WebViewHandler();

    bool Initialize();
    void Resize(RECT bounds);
    void Navigate(const std::wstring& url);
    void InjectScript(const std::wstring& script);
    
    void SetM3U8Callback(std::function<void(const std::wstring&)> callback);

private:
    void SetupWebResourceFilter();
    void InjectDownloadButton(const std::wstring& m3u8Url);
    std::wstring LoadInjectScript();

    HWND m_parentWindow;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
    Microsoft::WRL::ComPtr<ICoreWebView2> m_webview;
    Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_environment;
    
    std::function<void(const std::wstring&)> m_m3u8Callback;
    std::wstring m_currentM3U8Url;
};
