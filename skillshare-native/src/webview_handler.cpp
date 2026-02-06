#include "webview_handler.h"
#include "downloader.h"
#include <fstream>
#include <sstream>
#include <regex>

using namespace Microsoft::WRL;

extern std::unique_ptr<Downloader> g_downloader;

WebViewHandler::WebViewHandler(HWND parentWindow)
    : m_parentWindow(parentWindow) {
}

WebViewHandler::~WebViewHandler() {
    if (m_controller) {
        m_controller->Close();
    }
}

bool WebViewHandler::Initialize() {
    // Create WebView2 environment
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                if (FAILED(result) || !env) {
                    return E_FAIL;
                }

                m_environment = env;

                // Create WebView2 controller
                env->CreateCoreWebView2Controller(
                    m_parentWindow,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                            if (FAILED(result) || !controller) {
                                return E_FAIL;
                            }

                            m_controller = controller;
                            m_controller->get_CoreWebView2(&m_webview);

                            // Setup WebView
                            SetupWebResourceFilter();

                            // Resize to fill window
                            RECT bounds;
                            GetClientRect(m_parentWindow, &bounds);
                            m_controller->put_Bounds(bounds);

                            return S_OK;
                        }).Get());

                return S_OK;
            }).Get());

    if (FAILED(hr)) {
        return false;
    }

    // Wait for initialization (simplified - in production use events)
    MSG msg;
    while (!m_webview && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return m_webview != nullptr;
}

void WebViewHandler::SetupWebResourceFilter() {
    if (!m_webview) return;

    // Add filter for m3u8 URLs
    m_webview->AddWebResourceRequestedFilter(L"*://*/*.m3u8*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);

    // Handle web resource requests
    m_webview->add_WebResourceRequested(
        Callback<ICoreWebView2WebResourceRequestedEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args) -> HRESULT {
                wil::com_ptr<ICoreWebView2WebResourceRequest> request;
                args->get_Request(&request);

                wil::unique_cotaskmem_string uri;
                request->get_Uri(&uri);

                std::wstring urlStr(uri.get());
                
                // Check if it's an m3u8 URL
                if (urlStr.find(L".m3u8") != std::wstring::npos) {
                    m_currentM3U8Url = urlStr;
                    
                    // Inject download button
                    InjectDownloadButton(urlStr);
                    
                    if (m_m3u8Callback) {
                        m_m3u8Callback(urlStr);
                    }
                }

                return S_OK;
            }).Get(),
        nullptr);

    // Handle messages from injected script
    m_webview->add_WebMessageReceived(
        Callback<ICoreWebView2WebMessageReceivedEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                wil::unique_cotaskmem_string message;
                args->TryGetWebMessageAsString(&message);

                std::wstring msgStr(message.get());

                // Parse download request: "DOWNLOAD|url|title"
                if (msgStr.find(L"DOWNLOAD|") == 0) {
                    size_t firstPipe = msgStr.find(L'|');
                    size_t secondPipe = msgStr.find(L'|', firstPipe + 1);
                    
                    if (secondPipe != std::wstring::npos) {
                        std::wstring url = msgStr.substr(firstPipe + 1, secondPipe - firstPipe - 1);
                        std::wstring title = msgStr.substr(secondPipe + 1);

                        // Show folder picker
                        std::wstring outputDir = Downloader::SelectFolder(m_parentWindow);
                        if (!outputDir.empty() && g_downloader) {
                            g_downloader->Download(url, title, outputDir);
                        }
                    }
                }

                return S_OK;
            }).Get(),
        nullptr);
}

void WebViewHandler::InjectDownloadButton(const std::wstring& m3u8Url) {
    std::wstring script = LoadInjectScript();
    
    // Replace placeholder with actual URL
    size_t pos = script.find(L"__M3U8_URL__");
    if (pos != std::wstring::npos) {
        script.replace(pos, 12, m3u8Url);
    }

    m_webview->ExecuteScript(script.c_str(), nullptr);
}

std::wstring WebViewHandler::LoadInjectScript() {
    // Get executable directory
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring exePath(path);
    size_t lastSlash = exePath.find_last_of(L'\\');
    std::wstring dir = exePath.substr(0, lastSlash + 1);

    // Read inject.js
    std::wstring scriptPath = dir + L"inject.js";
    std::wifstream file(scriptPath);
    if (!file.is_open()) {
        // Fallback embedded script
        return LR"(
            (function() {
                const url = '__M3U8_URL__';
                let container = document.getElementById('skillshare-downloader-container');
                if (!container) {
                    container = document.createElement('div');
                    container.id = 'skillshare-downloader-container';
                    container.style.cssText = 'position:fixed;bottom:20px;right:20px;z-index:2147483647;';
                    document.body.appendChild(container);
                }
                
                let btn = document.getElementById('yt-dlp-download-btn');
                if (btn) {
                    btn.dataset.url = url;
                    return;
                }
                
                btn = document.createElement('button');
                btn.id = 'yt-dlp-download-btn';
                btn.innerText = 'Download Video';
                btn.dataset.url = url;
                btn.style.cssText = 'background:#00FF84;color:#002333;padding:12px 18px;border:none;border-radius:8px;font-weight:bold;cursor:pointer;box-shadow:0 6px 12px rgba(0,0,0,0.3);font-family:sans-serif;font-size:14px;';
                
                btn.onclick = () => {
                    const title = document.querySelector('.session-item.active .session-item-title h3')?.innerText?.trim() 
                        || document.title.replace(' | Skillshare', '').trim();
                    window.chrome.webview.postMessage('DOWNLOAD|' + btn.dataset.url + '|' + title);
                };
                
                container.appendChild(btn);
            })();
        )";
    }

    std::wstringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void WebViewHandler::Resize(RECT bounds) {
    if (m_controller) {
        m_controller->put_Bounds(bounds);
    }
}

void WebViewHandler::Navigate(const std::wstring& url) {
    if (m_webview) {
        m_webview->Navigate(url.c_str());
    }
}

void WebViewHandler::InjectScript(const std::wstring& script) {
    if (m_webview) {
        m_webview->ExecuteScript(script.c_str(), nullptr);
    }
}

void WebViewHandler::SetM3U8Callback(std::function<void(const std::wstring&)> callback) {
    m_m3u8Callback = callback;
}
