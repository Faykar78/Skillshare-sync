const { contextBridge, ipcRenderer } = require('electron');

console.log('Preload script loaded!');

// We still expose this for debugging via DevTools console if needed
contextBridge.exposeInMainWorld('electronAPI', {
    downloadVideo: (url, title) => ipcRenderer.send('download-video', { url, title })
});

// Logic to inject button - runs in Preload context (Isolated World)
// We use ipcRenderer directly here because we are in the preload scope.
ipcRenderer.on('show-download-button', (_event, url) => {
    console.log('Preload received show-download-button for:', url);

    // Create container if not exists
    let container = document.getElementById('skillshare-downloader-container');
    if (!container) {
        container = document.createElement('div');
        container.id = 'skillshare-downloader-container';
        container.style.position = 'fixed';
        container.style.bottom = '20px';
        container.style.right = '20px';
        container.style.zIndex = '2147483647'; // Max z-index
        document.body.appendChild(container);
    }

    // Check if button exists
    let btn = document.getElementById('yt-dlp-download-btn');
    if (btn) {
        btn.dataset.url = url;
        // visual feedback it updated
        btn.innerText = 'Download Video (Updated)';
        setTimeout(() => btn.innerText = 'Download Video', 1000);
        return;
    }

    btn = document.createElement('button');
    btn.id = 'yt-dlp-download-btn';
    btn.innerText = 'Download Video';
    btn.dataset.url = url;

    // Styling
    btn.style.backgroundColor = '#00FF84';
    btn.style.color = '#002333';
    btn.style.padding = '12px 18px';
    btn.style.border = 'none';
    btn.style.borderRadius = '8px';
    btn.style.fontWeight = 'bold';
    btn.style.cursor = 'pointer';
    btn.style.boxShadow = '0 6px 12px rgba(0,0,0,0.3)';
    btn.style.fontFamily = 'sans-serif';
    btn.style.fontSize = '14px';

    btn.onclick = () => {
        const currentUrl = btn.dataset.url;
        // Scrape title from document
        let pageTitle = document.title.replace(' | Skillshare', '').trim();
        const h1 = document.querySelector('h1.title');
        if (h1) pageTitle = h1.innerText.trim();

        console.log('Button clicked, downloading:', currentUrl, 'Title:', pageTitle);
        ipcRenderer.send('download-video', { url: currentUrl, title: pageTitle });
    };

    container.appendChild(btn);
});
