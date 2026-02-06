<div align="center">

# 📚 Skillshare Sync

**Your Personal Offline Learning Companion**

[![Native C++](https://img.shields.io/badge/Native-C++-00599C?style=flat-square&logo=cplusplus&logoColor=white)](skillshare-native/)
[![Electron](https://img.shields.io/badge/Electron-33.0-47848F?style=flat-square&logo=electron&logoColor=white)](https://www.electronjs.org/)
[![yt-dlp](https://img.shields.io/badge/Powered%20by-yt--dlp-red?style=flat-square)](https://github.com/yt-dlp/yt-dlp)
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](LICENSE)

---

A desktop application to help you save your **purchased** Skillshare lessons for personal offline viewing.

[Features](#-features) • [Installation](#-installation) • [yt-dlp Setup](#-yt-dlp-setup) • [Disclaimer](#%EF%B8%8F-disclaimer)

</div>

---

## ✨ Features

- **Native Desktop Experience** — Browse Skillshare in a dedicated window
- **One-Click Downloads** — Floating download button appears when watching lessons  
- **Automatic Subtitles** — Downloads and embeds subtitles when available
- **Smart File Naming** — Saves files with proper lesson titles

---

## 📦 Installation

### Windows (Native C++ App) — Recommended

Lightweight native Windows app (~2MB) using WebView2.

1. Download `SkillshareSync-Windows-x64.zip` from [Releases](https://github.com/Faykar78/Skillshare-sync/releases)
2. Extract and run `SkillshareSync.exe`
3. Ensure [WebView2 Runtime](https://developer.microsoft.com/en-us/microsoft-edge/webview2/) is installed (pre-installed on Windows 11)

### Linux (Electron AppImage)

1. Download the latest `.AppImage` from [Releases](https://github.com/Faykar78/Skillshare-sync/releases)
2. Make it executable:
   ```bash
   chmod +x Skillshare_Sync-*.AppImage
   ```
3. Run it!

### Build from Source

**Windows Native (requires Visual Studio):**
```powershell
cd skillshare-native
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

**Electron (cross-platform):**
```bash
cd skillshare-app
npm install
npm start
```

---

## 🔧 yt-dlp Setup

This app requires **yt-dlp** to be installed on your system. yt-dlp is a powerful video downloader that handles the actual downloading process.

### Linux

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install yt-dlp
```

**Or via pip (recommended for latest version):**
```bash
pip install -U yt-dlp
```

**Or download binary:**
```bash
sudo curl -L https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp -o /usr/local/bin/yt-dlp
sudo chmod a+rx /usr/local/bin/yt-dlp
```

### macOS

**Using Homebrew:**
```bash
brew install yt-dlp
```

**Or via pip:**
```bash
pip install -U yt-dlp
```

### Windows

**Using winget:**
```bash
winget install yt-dlp
```

**Using pip:**
```bash
pip install -U yt-dlp
```

**Or download the exe** from [yt-dlp releases](https://github.com/yt-dlp/yt-dlp/releases) and add it to your PATH.

### Verify Installation

After installation, verify yt-dlp is accessible:
```bash
yt-dlp --version
```

You should see a version number like `2024.12.23`. If you get "command not found", make sure yt-dlp is in your system PATH.

---

## 📁 Usage

1. Launch Skillshare Sync
2. Log in to your Skillshare account
3. Navigate to any lesson you have access to
4. Click the green **"Download Video"** button that appears
5. Select your download location
6. Done! Video with embedded subtitles will be saved.

---

## 📁 Project Structure

```
skillshare-sync/
├── skillshare-native/    # Native C++ Windows app (WebView2)
├── skillshare-app/       # Electron cross-platform app
└── yt_dlp_bot.py        # Telegram bot for multi-site downloads
```

---

## ⚠️ Disclaimer

> ### 🔒 FOR PERSONAL USE ONLY
>
> This software is provided **exclusively** for personal, educational backup purposes of content you have **legitimately purchased** or have authorized access to.
>
> **This project does NOT promote, encourage, or support piracy.**
>
> ### By using this software, you agree to:
>
> - ✅ Only download content you have **paid for** or have **legitimate access** to
> - ✅ Comply with Skillshare's Terms of Service  
> - ✅ Follow all applicable copyright laws in your jurisdiction
> - ❌ **NOT** redistribute, share, or sell any downloaded content
> - ❌ **NOT** use this tool to access content you haven't purchased
>
> The developers assume **no liability** for any misuse of this software. Users are solely responsible for ensuring their usage is legal and ethical.

---

## 📜 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

<div align="center">

Made with ❤️ for offline learners

**If you find this useful, consider ⭐ starring the repo!**

</div>
