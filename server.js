/**
 * JDT Water Tank Controller — Render.com WebSocket Relay Server
 * Cleanly separates hardware device socket from browser sockets.
 * Device identified by first telemetry packet it sends.
 */

'use strict';
const http      = require('http');
const WebSocket = require('ws');
const fs        = require('fs');
const path      = require('path');

const PORT = process.env.PORT || 10000;

const MIME = {
  '.html': 'text/html; charset=utf-8',
  '.css' : 'text/css',
  '.js'  : 'application/javascript',
  '.json': 'application/json',
  '.png' : 'image/png',
  '.ico' : 'image/x-icon'
};

const httpServer = http.createServer((req, res) => {
  let urlPath = req.url.split('?')[0];
  if (urlPath === '/') urlPath = '/index.html';
  if (urlPath === '/ws' || urlPath === '/device') {
    res.writeHead(426, { 'Content-Type': 'text/plain' });
    res.end('WebSocket only');
    return;
  }
  const filePath = path.join(__dirname, 'public', urlPath);
  const ext = path.extname(filePath);
  fs.readFile(filePath, (err, data) => {
    if (err) {
      fs.readFile(path.join(__dirname, 'public', 'index.html'), (e2, d2) => {
        if (e2) { res.writeHead(404); res.end('Not Found'); return; }
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8', 'Cache-Control': 'no-cache' });
        res.end(d2);
      });
      return;
    }
    res.writeHead(200, { 'Content-Type': MIME[ext] || 'text/plain', 'Cache-Control': 'no-cache' });
    res.end(data);
  });
});

// ─── State ───────────────────────────────────────────────────────────────────
const browsers      = new Set();  // All connected browser WebSockets
let   deviceWs      = null;       // The hardware device socket (ESP32)
let   lastDevMs     = 0;          // Timestamp of last telemetry from device
let   lastRealData  = null;       // Last full telemetry JSON string from device (with deviceOnline:true)
const OFFLINE_MSG   = JSON.stringify({ type: 'telemetry', deviceOnline: false, distanceCm: -1, sensorError: true });

function broadcastBrowsers(msg) {
  browsers.forEach(b => {
    if (b.readyState === WebSocket.OPEN) b.send(msg);
  });
}

// ─── WebSocket ────────────────────────────────────────────────────────────────
const wss = new WebSocket.Server({ noServer: true });

httpServer.on('upgrade', (req, socket, head) => {
  const url = req.url.split('?')[0];
  if (url === '/ws' || url === '/device') {
    wss.handleUpgrade(req, socket, head, ws => wss.emit('connection', ws, req));
  } else {
    socket.destroy();
  }
});

wss.on('connection', (ws) => {
  // All new connections start as browsers until they send telemetry
  browsers.add(ws);
  console.log(`[WS] Connected  — total ${wss.clients.size}`);

  // Immediately tell the new browser the current device status
  const isActive = deviceWs !== null && deviceWs.readyState === WebSocket.OPEN && (Date.now() - lastDevMs < 8000);
  if (isActive && lastRealData) {
    ws.send(lastRealData);   // Send the most recent real sensor data
  } else {
    ws.send(OFFLINE_MSG);    // Device not connected — show offline
  }

  ws.on('message', raw => {
    const str = raw.toString();
    let msg;
    try { msg = JSON.parse(str); } catch (_) { return; }

    // ── Is this telemetry from the hardware device? ───────────────────────
    if (msg.type === 'telemetry' || msg.distanceCm !== undefined) {
      // First telemetry packet: promote this socket to device
      if (ws !== deviceWs) {
        console.log('[WS] Hardware device identified');
        if (deviceWs) {
          // Old device socket orphaned — close it cleanly
          try { deviceWs.close(); } catch (_) {}
        }
        deviceWs = ws;
        browsers.delete(ws);  // Device is not a browser
      }

      lastDevMs = Date.now();

      // Attach deviceOnline flag and cache it
      msg.deviceOnline = true;
      const out = JSON.stringify(msg);
      lastRealData = out;

      // Forward to all connected browser tabs
      broadcastBrowsers(out);
      return;
    }

    // ── Control / config message from browser → relay to device ──────────
    if (deviceWs && deviceWs.readyState === WebSocket.OPEN) {
      deviceWs.send(str);
    }
  });

  ws.on('close', () => {
    browsers.delete(ws);
    console.log(`[WS] Disconnected — total ${wss.clients.size}`);

    if (ws === deviceWs) {
      console.log('[WS] Hardware device DISCONNECTED');
      deviceWs   = null;
      lastDevMs  = 0;
      lastRealData = null;
      broadcastBrowsers(OFFLINE_MSG);
    }
  });

  ws.on('error', e => console.error('[WS] Error:', e.message));
});

// ─── Telemetry silence watchdog ───────────────────────────────────────────────
// If device stops sending for >8s (accounts for Render free-tier SSL jitter),
// mark it offline and notify all browsers.
setInterval(() => {
  if (deviceWs !== null && (Date.now() - lastDevMs) > 8000) {
    console.log('[WS] Device silent >8s → marking OFFLINE');
    try { deviceWs.terminate(); } catch (_) {}
    deviceWs   = null;
    lastDevMs  = 0;
    lastRealData = null;
    broadcastBrowsers(OFFLINE_MSG);
  }
}, 500);

// ─── Keep Render free-tier alive ─────────────────────────────────────────────
const SELF_URL = process.env.RENDER_EXTERNAL_URL;
if (SELF_URL) {
  setInterval(() => require('https').get(SELF_URL, () => {}).on('error', () => {}), 14 * 60 * 1000);
}

httpServer.listen(PORT, () => console.log(`JDT Tank Server on port ${PORT}`));
