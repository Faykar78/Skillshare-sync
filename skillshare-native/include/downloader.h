#pragma once

#include <windows.h>
#include <string>
#include <functional>

class Downloader {
public:
    Downloader();
    ~Downloader();

    bool Download(const std::wstring& url, const std::wstring& title, const std::wstring& outputDir);
    void SetProgressCallback(std::function<void(int)> callback);
    void SetCompleteCallback(std::function<void(bool, const std::wstring&)> callback);

    static bool IsYtDlpAvailable();
    static std::wstring SanitizeFilename(const std::wstring& filename);
    static std::wstring SelectFolder(HWND parent);

private:
    std::function<void(int)> m_progressCallback;
    std::function<void(bool, const std::wstring&)> m_completeCallback;
};
