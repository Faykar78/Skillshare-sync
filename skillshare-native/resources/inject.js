(function () {
    const url = '__M3U8_URL__';

    // Create container if not exists
    let container = document.getElementById('skillshare-downloader-container');
    if (!container) {
        container = document.createElement('div');
        container.id = 'skillshare-downloader-container';
        container.style.cssText = 'position:fixed;bottom:20px;right:20px;z-index:2147483647;';
        document.body.appendChild(container);
    }

    // Check if button exists
    let btn = document.getElementById('yt-dlp-download-btn');
    if (btn) {
        btn.dataset.url = url;
        btn.innerText = 'Download Video (Updated)';
        setTimeout(() => btn.innerText = 'Download Video', 2000);
        return;
    }

    // Create button
    btn = document.createElement('button');
    btn.id = 'yt-dlp-download-btn';
    btn.innerText = 'Download Video';
    btn.dataset.url = url;
    btn.style.cssText = `
        background: linear-gradient(135deg, #00FF84 0%, #00D970 100%);
        color: #002333;
        padding: 14px 24px;
        border: none;
        border-radius: 12px;
        font-weight: bold;
        cursor: pointer;
        box-shadow: 0 8px 20px rgba(0,255,132,0.3);
        font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
        font-size: 14px;
        transition: all 0.2s ease;
    `;

    btn.onmouseenter = () => {
        btn.style.transform = 'translateY(-2px)';
        btn.style.boxShadow = '0 12px 24px rgba(0,255,132,0.4)';
    };

    btn.onmouseleave = () => {
        btn.style.transform = 'translateY(0)';
        btn.style.boxShadow = '0 8px 20px rgba(0,255,132,0.3)';
    };

    btn.onclick = () => {
        // Get lesson title
        let title = null;
        const activeTitleElement = document.querySelector('.session-item.active .session-item-title h3');
        if (activeTitleElement) {
            title = activeTitleElement.innerText.trim();
        }

        // Fallback to page title
        if (!title) {
            title = document.title.replace(' | Skillshare', '').trim();
        }

        // Send message to native app
        window.chrome.webview.postMessage('DOWNLOAD|' + btn.dataset.url + '|' + title);

        // Visual feedback
        btn.innerText = 'Starting download...';
        btn.style.background = '#888';
        setTimeout(() => {
            btn.innerText = 'Download Video';
            btn.style.background = 'linear-gradient(135deg, #00FF84 0%, #00D970 100%)';
        }, 3000);
    };

    container.appendChild(btn);
    console.log('[Skillshare Sync] Download button injected');
})();
