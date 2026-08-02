/**
 * JDT Water Tank Controller — Render.com WebSocket Relay Server
 * Bridges hardware device and web browsers with instant disconnect detection via ping/pong.
 */

'use strict';
const http  = require('http');
const WebSocket = require('ws');
const fs    = require('fs');
const path  = require('path');

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
  const mime = MIME[ext] || 'text/plain';

  fs.readFile(filePath, (err, data) => {
    if (err) {
      fs.readFile(path.join(__dirname, 'public', 'index.html'), (err2, data2) => {
        if (err2) { res.writeHead(404); res.end('Not Found'); return; }
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8', 'Cache-Control': 'no-cache' });
        res.end(data2);
      });
      return;
    }
    res.writeHead(200, { 'Content-Type': mime, 'Cache-Control': 'no-cache' });
    res.end(data);
  });
});

// ─── WebSocket relay ───
const wss = new WebSocket.Server({ noServer: true });

const clients    = new Set();
let deviceSocket = null;
let lastTelemetry = null;
let lastDeviceTime = 0;

function markDeviceOffline(reason) {
  console.log('[WS] Device OFFLINE —', reason);
  deviceSocket  = null;
  lastDeviceTime = 0;
  const payload = JSON.stringify({ type: 'telemetry', deviceOnline: false, distanceCm: -1, sensorError: true });
  lastTelemetry = payload;
  broadcast(payload);
}

function broadcast(data, skip = null) {
  clients.forEach(c => {
    if (c !== skip && c.readyState === WebSocket.OPEN) c.send(data);
  });
}

httpServer.on('upgrade', (req, socket, head) => {
  const url = req.url.split('?')[0];
  if (url === '/ws' || url === '/device') {
    wss.handleUpgrade(req, socket, head, ws => wss.emit('connection', ws, req));
  } else {
    socket.destroy();
  }
});

wss.on('connection', (ws, req) => {
  clients.add(ws);
  ws.isAlive  = true;
  ws.isDevice = false;
  ws.on('pong', () => { ws.isAlive = true; });

  console.log(`[WS] Connected  (total ${clients.size})`);

  // Tell the new browser whether the hardware device is currently live
  const deviceActive = (deviceSocket !== null &&
                        deviceSocket.readyState === WebSocket.OPEN &&
                        (Date.now() - lastDeviceTime < 3000));

  if (lastTelemetry) {
    try {
      const obj = JSON.parse(lastTelemetry);
      obj.deviceOnline = deviceActive;
      if (!deviceActive) obj.distanceCm = -1;
      ws.send(JSON.stringify(obj));
    } catch (_) { ws.send(lastTelemetry); }
  } else {
    ws.send(JSON.stringify({ type: 'telemetry', deviceOnline: deviceActive, distanceCm: -1 }));
  }

  ws.on('message', raw => {
    const str = raw.toString();
    try {
      const msg = JSON.parse(str);
      // Only the hardware device sends telemetry with distanceCm
      if (msg.type === 'telemetry' || msg.distanceCm !== undefined) {
        ws.isDevice    = true;
        deviceSocket   = ws;
        lastDeviceTime = Date.now();
        msg.deviceOnline = true;
        lastTelemetry = JSON.stringify(msg);
        broadcast(JSON.stringify(msg), ws);
        return;
      }
    } catch (_) {}
    // Relay everything else (control commands, config) to all sockets
    broadcast(str, ws);
  });

  ws.on('close', () => {
    clients.delete(ws);
    console.log(`[WS] Disconnected (total ${clients.size})`);
    if (ws === deviceSocket || ws.isDevice) {
      markDeviceOffline('socket closed');
    }
  });

  ws.on('error', e => console.error('[WS] Error:', e.message));
});

// ─── Ping every socket every 1.5 s ───────────────────────────────────────────
// If a socket doesn't reply with pong within the next cycle → kill it.
// This catches abrupt power-off (no clean TCP close).
setInterval(() => {
  wss.clients.forEach(ws => {
    if (!ws.isAlive) {
      // Dead socket — terminate it; 'close' event will fire and handle device state
      ws.terminate();
      return;
    }
    ws.isAlive = false;
    try { ws.ping(); } catch (_) {}
  });
}, 1500);

// ─── Safety-net: if device telemetry goes silent for 3 s → mark offline ──────
setInterval(() => {
  if (deviceSocket && (Date.now() - lastDeviceTime > 3000)) {
    markDeviceOffline('telemetry silent 3 s');
    if (deviceSocket) { try { deviceSocket.terminate(); } catch (_) {} }
  }
}, 500);

// ─── Keep Render free-tier alive ─────────────────────────────────────────────
const SELF_URL = process.env.RENDER_EXTERNAL_URL;
if (SELF_URL) {
  setInterval(() => {
    require('https').get(SELF_URL, () => {}).on('error', () => {});
  }, 14 * 60 * 1000);
}

httpServer.listen(PORT, () => console.log(`JDT Tank Server running on port ${PORT}`));
