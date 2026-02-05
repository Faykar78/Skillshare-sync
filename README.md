<![CDATA[<div align="center">

# 📚 Skillshare Sync

### *Your Personal Offline Learning Companion*

[![Electron](https://img.shields.io/badge/Electron-33.0-47848F?style=for-the-badge&logo=electron&logoColor=white)](https://www.electronjs.org/)
[![Python](https://img.shields.io/badge/Python-3.8+-3776AB?style=for-the-badge&logo=python&logoColor=white)](https://python.org)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

<br/>

*A desktop application designed to help you save your purchased Skillshare lessons for personal offline viewing.*

---

</div>

## ✨ Features

<table>
<tr>
<td width="50%">

### 🖥️ Desktop App
- **Native Experience** — Dedicated Skillshare browser
- **Offline Access** — Save lessons for personal use
- **Auto Subtitles** — Downloads & embeds captions
- **Smart Naming** — Proper lesson titles preserved

</td>
<td width="50%">

### 🤖 Telegram Bot
- **Multi-Platform** — YouTube, Twitter, Instagram & more
- **No Size Limits** — Auto-splits large files
- **Format Choice** — Video or MP3 audio
- **Self-Hosted** — Your own private instance

</td>
</tr>
</table>

---

## 🚀 Quick Start

### Desktop App

```bash
cd skillshare-app
npm install
npm start
```

> **Requirements:** Node.js 18+ and `yt-dlp` in PATH

### Telegram Bot

```bash
pip install python-telegram-bot yt-dlp
# Add your bot token to yt_dlp_bot.py
python3 yt_dlp_bot.py
```

---

## 📁 Structure

```
skillshare-sync/
├── skillshare-app/       # Electron desktop app
│   ├── main.js           # Main process & stream handler
│   ├── preload.js        # UI bridge & IPC
│   └── package.json
├── yt_dlp_bot.py         # Telegram download bot
└── README.md
```

---

<div align="center">

## ⚠️ Important Disclaimer

</div>

> ### 🔒 FOR PERSONAL USE ONLY
>
> This software is intended **exclusively for personal, educational backup purposes** of content you have legitimately purchased or have authorized access to.
>
> **This project does NOT promote, encourage, or support piracy in any form.**
>
> By using this software, you agree to:
> - ✅ Use it only for content you have **paid for** or have **legitimate access** to
> - ✅ Comply with Skillshare's Terms of Service
> - ✅ Follow all applicable copyright laws in your jurisdiction
> - ❌ **NOT** redistribute, share, or commercially exploit any downloaded content
> - ❌ **NOT** use this tool to access content you haven't purchased
>
> The developers assume **no liability** for any misuse. Users are solely responsible for ensuring their usage is legal and ethical.

---

<div align="center">

## 📜 License

MIT License — See [LICENSE](LICENSE) for details.

---

<sub>Built for learners who value offline access to their **purchased** content.</sub>

<br/>

**⭐ Star this repo if you find it useful!**

</div>
]]>
