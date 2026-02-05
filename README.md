<![CDATA[<div align="center">

# 📚 Skillshare Sync

**Your Personal Offline Learning Companion**

[![Electron](https://img.shields.io/badge/Electron-33.0-47848F?style=flat-square&logo=electron&logoColor=white)](https://www.electronjs.org/)
[![Python](https://img.shields.io/badge/Python-3.8+-3776AB?style=flat-square&logo=python&logoColor=white)](https://python.org)
[![yt-dlp](https://img.shields.io/badge/Powered%20by-yt--dlp-red?style=flat-square)](https://github.com/yt-dlp/yt-dlp)
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](LICENSE)

---

A desktop application to help you save your **purchased** Skillshare lessons for personal offline viewing.

[Features](#-features) • [Installation](#-installation) • [Usage](#-usage) • [Disclaimer](#%EF%B8%8F-disclaimer)

</div>

---

## ✨ Features

### 🖥️ Desktop Application (Electron)

- **Native Desktop Experience** — Browse Skillshare in a dedicated window
- **One-Click Downloads** — Floating download button appears when watching lessons  
- **Automatic Subtitles** — Downloads and embeds subtitles when available
- **Smart File Naming** — Saves files with proper lesson titles

### 🤖 Telegram Bot

- **Multi-Platform Support** — Works with YouTube, Twitter, Instagram, TikTok & 1000+ sites
- **No File Size Limits** — Large files are automatically split into parts
- **Format Selection** — Choose between video (best quality) or MP3 audio
- **Self-Hosted** — Run your own private instance

---

## 📦 Installation

### Prerequisites

- [Node.js](https://nodejs.org/) 18 or higher
- [yt-dlp](https://github.com/yt-dlp/yt-dlp) installed and available in PATH
- Python 3.8+ (for Telegram bot)

### Desktop App

```bash
cd skillshare-app
npm install
npm start
```

### Telegram Bot

```bash
pip install python-telegram-bot yt-dlp
```

Edit `yt_dlp_bot.py` and add your bot token:

```python
BOT_TOKEN = "your_bot_token_here"
```

Then run:

```bash
python3 yt_dlp_bot.py
```

---

## 📁 Usage

### Desktop App

1. Launch the app with `npm start`
2. Log in to your Skillshare account
3. Navigate to any lesson
4. Click the green **"Download Video"** button that appears
5. Select your download location

### Telegram Bot

1. Start your bot with `python3 yt_dlp_bot.py`
2. Send `/start` to your bot on Telegram
3. Paste any video URL
4. Choose your preferred format (Video or MP3)
5. Receive the file in your chat

---

## 📂 Project Structure

```
skillshare-sync/
├── skillshare-app/
│   ├── main.js           # Electron main process
│   ├── preload.js        # Renderer bridge & UI injection
│   ├── package.json      # Dependencies
│   └── icon.ico          # App icon
├── yt_dlp_bot.py         # Telegram bot
├── .gitignore
└── README.md
```

---

## ⚠️ Disclaimer

> **FOR PERSONAL USE ONLY**
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
]]>
