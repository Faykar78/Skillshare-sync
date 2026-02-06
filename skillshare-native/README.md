# Native C++ Skillshare Sync

A lightweight native Windows application using WebView2 for browser embedding.

## Requirements

- Visual Studio 2019/2022 with C++ workload
- CMake 3.20+
- Windows 10 SDK

## Building

### On Windows (Recommended)

```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

The executable will be in `build/Release/SkillshareSync.exe`

### Cross-compile from Linux (Advanced)

Requires MinGW-w64 toolchain and Wine for testing.

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-w64.cmake
make
```

## Dependencies

The build automatically downloads:
- WebView2 SDK from NuGet

## Runtime Requirements

- Windows 10/11
- WebView2 Runtime (pre-installed on Win11, [download for Win10](https://developer.microsoft.com/en-us/microsoft-edge/webview2/))
- yt-dlp installed and in PATH

## Project Structure

```
skillshare-native/
├── CMakeLists.txt          # Build configuration
├── include/
│   ├── app_window.h        # Win32 window wrapper
│   ├── webview_handler.h   # WebView2 controller
│   └── downloader.h        # yt-dlp wrapper
├── src/
│   ├── main.cpp            # Entry point
│   ├── app_window.cpp      # Window implementation
│   ├── webview_handler.cpp # WebView2 + m3u8 detection
│   └── downloader.cpp      # Download logic
└── resources/
    ├── inject.js           # Download button injection
    └── app.rc              # Windows resource file
```
