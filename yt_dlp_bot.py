#!/usr/bin/env python3
"""
YT-DLP Telegram Bot - Downloads videos from YouTube and 1000+ sites
Run: python3 yt_dlp_bot.py
"""

import os
import asyncio
import yt_dlp
import logging
import glob
import math
from telegram import Update, InlineKeyboardButton, InlineKeyboardMarkup
from telegram.ext import Application, CommandHandler, MessageHandler, CallbackQueryHandler, filters, ContextTypes

# ============== CONFIGURATION ==============
BOT_TOKEN = ""  # <-- PASTE YOUR TOKEN HERE
DOWNLOAD_DIR = "/tmp/ytdlp_downloads"
CHUNK_SIZE = 49 * 1024 * 1024  # 49MB chunks
# ===========================================

logging.basicConfig(format='%(asctime)s - %(levelname)s - %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)
os.makedirs(DOWNLOAD_DIR, exist_ok=True)

pending_urls = {}


def split_file(file_path: str, chunk_size: int) -> list:
    """Split a file into chunks."""
    file_size = os.path.getsize(file_path)
    if file_size <= chunk_size:
        return [file_path]
    
    chunks = []
    base_name = os.path.basename(file_path)
    num_chunks = math.ceil(file_size / chunk_size)
    
    with open(file_path, 'rb') as f:
        for i in range(num_chunks):
            chunk_path = f"{DOWNLOAD_DIR}/part{i+1}of{num_chunks}_{base_name}"
            with open(chunk_path, 'wb') as chunk_file:
                chunk_file.write(f.read(chunk_size))
            chunks.append(chunk_path)
    
    return chunks


async def start(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text(
        "🎬 *YT-DLP Bot*\n\n"
        "Send any video URL to download!\n"
        "✅ No size limit - large files split into parts\n\n"
        "Supports: YouTube, Twitter, Instagram, TikTok +1000 more!",
        parse_mode='Markdown'
    )


def get_video_info(url: str) -> dict:
    ydl_opts = {'quiet': True, 'no_warnings': True}
    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        return ydl.extract_info(url, download=False)


def download_video(url: str, audio_only: bool = False) -> str:
    # Clean up download directory
    for f in glob.glob(f"{DOWNLOAD_DIR}/*"):
        try: os.remove(f)
        except: pass
    
    if audio_only:
        ydl_opts = {
            'format': 'bestaudio[ext=m4a]/bestaudio/best',
            'outtmpl': f'{DOWNLOAD_DIR}/%(title).50s.%(ext)s',
            'postprocessors': [{'key': 'FFmpegExtractAudio', 'preferredcodec': 'mp3', 'preferredquality': '192'}],
            'extractor_args': {'youtube': {'player_client': ['android', 'web']}},
        }
    else:
        ydl_opts = {
            'format': 'bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best',
            'outtmpl': f'{DOWNLOAD_DIR}/%(title).50s.%(ext)s',
            'extractor_args': {'youtube': {'player_client': ['android', 'web']}},
            'merge_output_format': 'mp4',
        }
    
    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        ydl.extract_info(url, download=True)
    
    files = glob.glob(f"{DOWNLOAD_DIR}/*")
    return max(files, key=os.path.getctime) if files else None


async def handle_url(update: Update, context: ContextTypes.DEFAULT_TYPE):
    url = update.message.text.strip()
    user_id = update.effective_user.id
    
    if not url.startswith(('http://', 'https://')):
        await update.message.reply_text("❌ Send a valid URL")
        return
    
    status_msg = await update.message.reply_text("🔍 Fetching info...")
    
    try:
        info = get_video_info(url)
        title = info.get('title', 'Unknown')[:80]
        duration = info.get('duration', 0)
        dur_str = f"{duration // 60}:{duration % 60:02d}" if duration else "?"
        
        pending_urls[user_id] = url
        
        keyboard = [[
            InlineKeyboardButton("🎬 Video (Best)", callback_data="video"),
            InlineKeyboardButton("🎵 MP3", callback_data="audio"),
        ], [InlineKeyboardButton("❌ Cancel", callback_data="cancel")]]
        
        await status_msg.edit_text(
            f"📹 *{title}*\n⏱ {dur_str}\n\nChoose format:",
            reply_markup=InlineKeyboardMarkup(keyboard),
            parse_mode='Markdown'
        )
    except Exception as e:
        logger.error(f"Error: {e}")
        await status_msg.edit_text(f"❌ {str(e)[:150]}")


async def button_callback(update: Update, context: ContextTypes.DEFAULT_TYPE):
    query = update.callback_query
    await query.answer()
    
    user_id = update.effective_user.id
    choice = query.data
    
    if choice == "cancel":
        pending_urls.pop(user_id, None)
        await query.edit_message_text("❌ Cancelled")
        return
    
    url = pending_urls.get(user_id)
    if not url:
        await query.edit_message_text("❌ Session expired. Send URL again.")
        return
    
    audio_only = choice == "audio"
    await query.edit_message_text(f"⬇️ Downloading {'audio' if audio_only else 'video'}...")
    
    try:
        file_path = download_video(url, audio_only)
        
        if not file_path or not os.path.exists(file_path) or os.path.getsize(file_path) == 0:
            await query.edit_message_text("❌ Download failed")
            return
        
        file_size = os.path.getsize(file_path)
        size_mb = file_size / (1024*1024)
        
        chunks = split_file(file_path, CHUNK_SIZE)
        total_parts = len(chunks)
        
        if total_parts > 1:
            await query.edit_message_text(f"📤 Uploading {size_mb:.1f}MB in {total_parts} parts...")
        else:
            await query.edit_message_text(f"📤 Uploading {size_mb:.1f}MB...")
        
        for i, chunk_path in enumerate(chunks):
            caption = f"Part {i+1}/{total_parts}" if total_parts > 1 else "🎬 Downloaded via YT-DLP Bot"
            
            with open(chunk_path, 'rb') as f:
                if audio_only and total_parts == 1:
                    await context.bot.send_audio(
                        chat_id=update.effective_chat.id,
                        audio=f,
                        caption="🎵 Downloaded via YT-DLP Bot"
                    )
                else:
                    await context.bot.send_document(
                        chat_id=update.effective_chat.id,
                        document=f,
                        caption=caption
                    )
        
        await query.edit_message_text(f"✅ Done! {size_mb:.1f}MB" + (f" in {total_parts} parts" if total_parts > 1 else ""))
        
        # Cleanup
        for f in glob.glob(f"{DOWNLOAD_DIR}/*"):
            try: os.remove(f)
            except: pass
        pending_urls.pop(user_id, None)
        
    except Exception as e:
        logger.error(f"Error: {e}")
        await query.edit_message_text(f"❌ {str(e)[:150]}")


def main():
    print("🤖 Starting YT-DLP Telegram Bot...")
    
    if BOT_TOKEN == "YOUR_BOT_TOKEN_HERE":
        print("❌ ERROR: Please set your BOT_TOKEN in the script!")
        return
    
    app = Application.builder().token(BOT_TOKEN).build()
    
    app.add_handler(CommandHandler("start", start))
    app.add_handler(MessageHandler(filters.TEXT & ~filters.COMMAND, handle_url))
    app.add_handler(CallbackQueryHandler(button_callback))
    
    print("✅ Bot is running! Press Ctrl+C to stop.")
    app.run_polling(drop_pending_updates=True)


if __name__ == "__main__":
    main()
