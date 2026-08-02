/**
 * JDT Water Tank Controller — Render.com WebSocket Relay Server
 * Device detection: based on telemetry silence (2.5s timeout).
 * Browser connections are never terminated by the server.
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
  const ext  = path.extname(filePath);
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
const browsers    = new Set();   // All connected browser clients
let   deviceWs    = null;        // The one hardware device socket
let   lastDevMs   = 0;           // Last time hardware sent telemetry (ms)
let   deviceOnline = false;      // Current known device state
const OFFLINE_PAYLOAD = JSON.stringify({ type: 'telemetry', deviceOnline: false, distanceCm: -1, sensorError: true });

// ─── Helpers ─────────────────────────────────────────────────────────────────
function broadcastBrowsers(data) {
  browsers.forEach(b => { if (b.readyState === WebSocket.OPEN) b.send(data); });
}

function setDeviceOnline(online, reason) {
  if (deviceOnline === online) return;          // No state change → no broadcast
  deviceOnline = online;
  console.log('[WS] Device', online ? 'ONLINE' : 'OFFLINE', '—', reason);
  if (!online) {
    broadcastBrowsers(OFFLINE_PAYLOAD);
  }
}

// ─── WebSocket setup ──────────────────────────────────────────────────────────
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
  console.log(`[WS] Connected (total ${wss.clients.size})`);

  // Send current device status immediately to new browser
  browsers.add(ws);
  ws.send(deviceOnline
    ? JSON.stringify({ type: 'telemetry', deviceOnline: true, distanceCm: -1 })
    : OFFLINE_PAYLOAD
  );

  ws.on('message', raw => {
    const str = raw.toString();
    try {
      const msg = JSON.parse(str);

      // ── Hardware device telemetry ─────────────────────────────────────────
      if (msg.type === 'telemetry' || msg.distanceCm !== undefined) {
        // First packet from this socket → mark it as the device socket
        if (deviceWs !== ws) {
          if (deviceWs) {
            // Remove old device from browser pool too
            browsers.delete(deviceWs);
          }
          deviceWs = ws;
          browsers.delete(ws);  // Device socket is NOT a browser
        }
        lastDevMs = Date.now();
        setDeviceOnline(true, 'telemetry received');
        msg.deviceOnline = true;
        broadcastBrowsers(JSON.stringify(msg));
        return;
      }

      // ── Control / config from browser → relay to device ──────────────────
      if (deviceWs && deviceWs.readyState === WebSocket.OPEN) {
        deviceWs.send(str);
      }
    } catch (_) {}
  });

  ws.on('close', () => {
    browsers.delete(ws);
    console.log(`[WS] Disconnected (total ${wss.clients.size})`);

    if (ws === deviceWs) {
      deviceWs = null;
      lastDevMs = 0;
      setDeviceOnline(false, 'device socket closed');
    }
  });

  ws.on('error', e => console.error('[WS] Error:', e.message));
});

// ─── Silence watchdog: if device hasn't sent in 2.5 s → offline ──────────────
setInterval(() => {
  if (deviceOnline && deviceWs && (Date.now() - lastDevMs > 2500)) {
    setDeviceOnline(false, 'telemetry silent > 2.5s');
  }
}, 500);

// ─── Keep Render free-tier alive ─────────────────────────────────────────────
const SELF_URL = process.env.RENDER_EXTERNAL_URL;
if (SELF_URL) {
  setInterval(() => require('https').get(SELF_URL, () => {}).on('error', () => {}), 14 * 60 * 1000);
}

httpServer.listen(PORT, () => console.log(`JDT Tank Server on port ${PORT}`));
