/**
 * JDT Water Tank Controller — Render.com WebSocket Relay Server
 * Bridges ESP32 hardware and web browsers seamlessly.
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
let lastTelemetry = null;

function broadcast(data, originWs = null) {
  clients.forEach(ws => {
    if (ws !== originWs && ws.readyState === WebSocket.OPEN) {
      ws.send(data);
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

  // Send last cached telemetry if available
  if (lastTelemetry) {
    ws.send(lastTelemetry);
  }

  ws.on('message', (raw) => {
    const str = raw.toString();
    try {
      const msg = JSON.parse(str);
      if (msg.type === 'telemetry' || msg.distanceCm !== undefined) {
        lastTelemetry = str;
      }
    } catch (_) {}
    
    // Broadcast message to all other connected clients (ESP32 <-> Browser)
    broadcast(str, ws);
  });

  ws.on('close', () => {
    clients.delete(ws);
    console.log(`[WS] Client Disconnected (${clients.size} remaining)`);
  });

  ws.on('error', (e) => console.error('[WS] Error:', e.message));
});

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
