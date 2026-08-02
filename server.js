/**
 * JDT Water Tank Controller — Render.com WebSocket Relay Server
 * Bridges ESP32 hardware and web browsers seamlessly with instant disconnect detection.
 */

'use strict';
const http  = require('http');
const WebSocket = require('ws');
const fs    = require('fs');
const path  = require('path');

const PORT = process.env.PORT || 10000;

// MIME types
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
        res.writeHead(200, {
          'Content-Type': 'text/html; charset=utf-8',
          'Cache-Control': 'no-cache, no-store, must-revalidate',
          'Pragma': 'no-cache',
          'Expires': '0'
        });
        res.end(data2);
      });
      return;
    }
    res.writeHead(200, {
      'Content-Type': mime,
      'Cache-Control': 'no-cache, no-store, must-revalidate',
      'Pragma': 'no-cache',
      'Expires': '0'
    });
    res.end(data);
  });
});

// WebSocket relay
const wss = new WebSocket.Server({ noServer: true });

const clients = new Set();
let deviceSocket = null;
let lastTelemetry = null;
let lastDeviceTime = 0;

function broadcast(data, originWs = null) {
  clients.forEach(client => {
    if (client !== originWs && client.readyState === WebSocket.OPEN) {
      client.send(data);
    }
  });
}

httpServer.on('upgrade', (req, socket, head) => {
  const url = req.url.split('?')[0];
  if (url === '/ws' || url === '/device') {
    wss.handleUpgrade(req, socket, head, (ws) => {
      wss.emit('connection', ws, req);
    });
  } else {
    socket.destroy();
  }
});

wss.on('connection', (ws, req) => {
  clients.add(ws);
  console.log(`[WS] Client Connected (${clients.size} total)`);

  // Check if hardware ESP32 is currently connected and active (telemetry within last 4s)
  const isDeviceActive = (deviceSocket !== null && deviceSocket.readyState === WebSocket.OPEN && (Date.now() - lastDeviceTime < 4000));

  if (lastTelemetry) {
    try {
      const obj = JSON.parse(lastTelemetry);
      obj.deviceOnline = isDeviceActive;
      if (!isDeviceActive) obj.distanceCm = -1;
      ws.send(JSON.stringify(obj));
    } catch (_) {
      ws.send(lastTelemetry);
    }
  } else {
    // Send initial status to new browser if no telemetry yet
    ws.send(JSON.stringify({ type: 'telemetry', deviceOnline: isDeviceActive, distanceCm: -1 }));
  }

  ws.on('message', (raw) => {
    const str = raw.toString();
    try {
      const msg = JSON.parse(str);
      if (msg.type === 'telemetry' || msg.distanceCm !== undefined) {
        // Tag this WebSocket as the hardware ESP32 device
        ws.isDevice = true;
        deviceSocket = ws;
        lastDeviceTime = Date.now();
        msg.deviceOnline = true;

        lastTelemetry = JSON.stringify(msg);
        broadcast(JSON.stringify(msg), ws);
        return;
      }
    } catch (_) {}
    
    // Relay control or config messages to all connected sockets
    broadcast(str, ws);
  });

  ws.on('close', () => {
    clients.delete(ws);
    console.log(`[WS] Client Disconnected (${clients.size} remaining)`);

    // If the disconnected client was the ESP32 hardware device
    if (ws === deviceSocket || ws.isDevice) {
      console.log('[WS] ⚠️ HARDWARE ESP32 DEVICE DISCONNECTED');
      deviceSocket = null;
      lastDeviceTime = 0;

      const offlinePayload = JSON.stringify({
        type: 'telemetry',
        deviceOnline: false,
        distanceCm: -1,
        sensorError: true
      });

      lastTelemetry = offlinePayload;
      // Immediately notify all connected web browsers
      broadcast(offlinePayload);
    }
  });

  ws.on('error', (e) => {
    console.error('[WS] Error:', e.message);
  });
});

// Periodic sanity check: if ESP32 hasn't sent telemetry in 4 seconds, mark offline
setInterval(() => {
  if (deviceSocket && (Date.now() - lastDeviceTime > 4000)) {
    console.log('[WS] ⚠️ Hardware ESP32 heartbeat timeout (4s silent)');
    deviceSocket = null;
    lastDeviceTime = 0;
    const offlinePayload = JSON.stringify({
      type: 'telemetry',
      deviceOnline: false,
      distanceCm: -1,
      sensorError: true
    });
    lastTelemetry = offlinePayload;
    broadcast(offlinePayload);
  }
}, 1000);

// Keep Render.com free tier alive
const SELF_URL = process.env.RENDER_EXTERNAL_URL;
if (SELF_URL) {
  setInterval(() => {
    require('https').get(SELF_URL, () => {}).on('error', () => {});
  }, 14 * 60 * 1000);
}

httpServer.listen(PORT, () => {
  console.log(`JDT Water Tank Server running on port ${PORT}`);
});
