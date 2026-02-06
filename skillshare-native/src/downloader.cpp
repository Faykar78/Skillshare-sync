#include "downloader.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <thread>
#include <regex>

Downloader::Downloader() {
}

Downloader::~Downloader() {
}

bool Downloader::IsYtDlpAvailable() {
    // Try to run yt-dlp --version
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    wchar_t cmd[] = L"yt-dlp --version";
    
    if (CreateProcessW(nullptr, cmd, nullptr, nullptr, FALSE, 
                       CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, 5000);
        
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        return exitCode == 0;
    }
    
    return false;
}

std::wstring Downloader::SanitizeFilename(const std::wstring& filename) {
    std::wstring result = filename;
    
    // Replace invalid characters
    const std::wstring invalidChars = L"<>:\"/\\|?*";
    for (wchar_t& c : result) {
        if (invalidChars.find(c) != std::wstring::npos) {
            c = L'_';
        }
    }
    
    // Trim whitespace
    size_t start = result.find_first_not_of(L" \t");
    size_t end = result.find_last_not_of(L" \t");
    
    if (start != std::wstring::npos && end != std::wstring::npos) {
        result = result.substr(start, end - start + 1);
    }
    
    return result;
}

std::wstring Downloader::SelectFolder(HWND parent) {
    wchar_t path[MAX_PATH] = { 0 };
    
    BROWSEINFOW bi = {};
    bi.hwndOwner = parent;
    bi.lpszTitle = L"Select Download Location";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    
    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
    if (pidl) {
        SHGetPathFromIDListW(pidl, path);
        CoTaskMemFree(pidl);
        return std::wstring(path);
    }
    
    return L"";
}

bool Downloader::Download(const std::wstring& url, const std::wstring& title, const std::wstring& outputDir) {
    // Run download in background thread
    std::thread([this, url, title, outputDir]() {
        std::wstring safeTitle = SanitizeFilename(title);
        
        // Build output path
        std::wstring outputPath = outputDir + L"\\" + safeTitle + L".%(ext)s";
        
        // Build command
        std::wstring command = L"yt-dlp \"" + url + L"\" "
            L"--write-subs --write-auto-subs --sub-langs all --embed-subs "
            L"-o \"" + outputPath + L"\"";
        
        // Create process
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        
        std::vector<wchar_t> cmdBuffer(command.begin(), command.end());
        cmdBuffer.push_back(L'\0');
        
        BOOL success = CreateProcessW(
            nullptr,
            cmdBuffer.data(),
            nullptr,
            nullptr,
            FALSE,
            CREATE_NO_WINDOW,
            nullptr,
            outputDir.c_str(),
            &si,
            &pi
        );
        
        if (!success) {
            if (m_completeCallback) {
                m_completeCallback(false, L"Failed to start yt-dlp. Is it installed?");
            }
            return;
        }
        
        // Wait for completion
        WaitForSingleObject(pi.hProcess, INFINITE);
        
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        if (m_completeCallback) {
            if (exitCode == 0) {
                m_completeCallback(true, L"Download complete!");
            } else {
                m_completeCallback(false, L"Download failed. Check yt-dlp output.");
            }
        }
    }).detach();
    
    return true;
}

void Downloader::SetProgressCallback(std::function<void(int)> callback) {
    m_progressCallback = callback;
}

void Downloader::SetCompleteCallback(std::function<void(bool, const std::wstring&)> callback) {
    m_completeCallback = callback;
}
