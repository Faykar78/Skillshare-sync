const { app, BrowserWindow, shell, session, ipcMain, dialog, Notification } = require('electron');
const path = require('path');
const { exec } = require('child_process');

function createWindow() {
    const mainWindow = new BrowserWindow({
        width: 1280,
        height: 800,
        backgroundColor: '#FFFFFF',
        autoHideMenuBar: true,
        webPreferences: {
            nodeIntegration: false,
            contextIsolation: true,
            preload: path.join(__dirname, 'preload.js')
        }
    });

    mainWindow.loadURL('https://www.skillshare.com/');

    mainWindow.webContents.setWindowOpenHandler(({ url }) => {
        if (!url.includes('skillshare.com') && !url.includes('GOOGLE') && !url.includes('facebook')) {
            if (url.startsWith('http')) {
                shell.openExternal(url);
                return { action: 'deny' };
            }
        }
        return { action: 'allow' };
    });

    mainWindow.webContents.on('before-input-event', (event, input) => {
        if (input.key === 'F12') {
            mainWindow.webContents.toggleDevTools();
            event.preventDefault();
        }
    });

    mainWindow.webContents.on('console-message', (event, level, message, line, sourceId) => {
        console.log(`[Renderer]: ${message}`);
    });

    mainWindow.webContents.openDevTools();

    session.defaultSession.webRequest.onHeadersReceived((details, callback) => {
        callback({
            responseHeaders: {
                ...details.responseHeaders,
                'Content-Security-Policy': ['default-src * \'unsafe-inline\' \'unsafe-eval\' data: blob:']
            }
        });
    });

    const filter = { urls: ['*://*/*.m3u8*'] };
    session.defaultSession.webRequest.onBeforeRequest(filter, (details, callback) => {
        console.log('Detected m3u8:', details.url);

        const safeUrl = JSON.stringify(details.url);
        const injectionCode = `
            (function() {
                console.log("Injected code running for URL:", ${safeUrl});
                const url = ${safeUrl};
                
                let container = document.getElementById('skillshare-downloader-container');
                if (!container) {
                    container = document.createElement('div');
                    container.id = 'skillshare-downloader-container';
                    container.style.position = 'fixed';
                    container.style.bottom = '20px';
                    container.style.right = '20px';
                    container.style.zIndex = '2147483647';
                    document.body.appendChild(container);
                }

                let btn = document.getElementById('yt-dlp-download-btn');
                if (btn) {
                    btn.dataset.url = url;
                    btn.innerText = 'Download Video (Updated)';
                    setTimeout(() => btn.innerText = 'Download Video', 2000);
                    return;
                }

                btn = document.createElement('button');
                btn.id = 'yt-dlp-download-btn';
                btn.innerText = 'Download Video';
                btn.dataset.url = url;
                
                btn.style.backgroundColor = '#00FF84'; 
                btn.style.color = '#002333';
                btn.style.padding = '12px 18px';
                btn.style.border = 'none';
                btn.style.borderRadius = '8px';
                btn.style.fontWeight = 'bold';
                btn.style.cursor = 'pointer';
                btn.style.boxShadow = '0 6px 12px rgba(0,0,0,0.3)';
                btn.style.zIndex = '2147483647';

                btn.onclick = () => {
                     if (window.electronAPI && window.electronAPI.downloadVideo) {
                        // CORRECT SELECTOR: .session-item.active .session-item-title h3
                        // This is the currently playing lesson in Skillshare's lesson list
                        let lessonTitle = null;
                        
                        const activeTitleElement = document.querySelector('.session-item.active .session-item-title h3');
                        if (activeTitleElement) {
                            lessonTitle = activeTitleElement.innerText.trim();
                            console.log('Found lesson title from .session-item.active:', lessonTitle);
                        }
                        
                        // Fallback: use page title
                        if (!lessonTitle) {
                            lessonTitle = document.title.replace(' | Skillshare', '').trim();
                            console.log('Fallback to page title:', lessonTitle);
                        }

                        console.log('Final lesson title:', lessonTitle);
                        window.electronAPI.downloadVideo(btn.dataset.url, lessonTitle);
                     } else {
                        alert("Error: Electron API not found!");
                     }
                };

                container.appendChild(btn);
                console.log("Button injected successfully");
            })();
        `;

        mainWindow.webContents.executeJavaScript(injectionCode).catch(err => console.log('Injection failed:', err));

        callback({ cancel: false });
    });
}

// IPC Handler for Download
ipcMain.on('download-video', async (event, args) => {
    const url = (typeof args === 'string') ? args : args.url;
    const title = (typeof args === 'string') ? 'Skillshare Video' : (args.title || 'Skillshare Video');

    console.log(`Received download request. URL: ${url}, Title: ${title}`);

    const { canceled, filePaths } = await dialog.showOpenDialog({
        properties: ['openDirectory', 'createDirectory'],
        title: 'Select Download Location'
    });

    if (!canceled && filePaths.length > 0) {
        const downloadPath = filePaths[0];

        // Sanitize title for filename
        const safeTitle = title.replace(/[^a-zA-Z0-9 \-_]/g, '_').trim();

        new Notification({ title: 'Download Started', body: `Downloading: ${title}` }).show();

        const command = `yt-dlp "${url}" --write-subs --write-auto-subs --sub-langs all --embed-subs -o "${path.join(downloadPath, safeTitle + '.%(ext)s')}"`;

        console.log('Executing command:', command);

        exec(command, (error, stdout, stderr) => {
            if (error) {
                console.error(`exec error: ${error}`);
                new Notification({ title: 'Download Failed', body: 'Check terminal/logs for error.' }).show();
                return;
            }
            console.log(`stdout: ${stdout}`);
            new Notification({ title: 'Download Complete', body: 'Video saved successfully.' }).show();
        });
    }
});

app.whenReady().then(() => {
    createWindow();

    app.on('activate', function () {
        if (BrowserWindow.getAllWindows().length === 0) createWindow();
    });
});

app.on('window-all-closed', function () {
    if (process.platform !== 'darwin') app.quit();
});
