const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const cors = require('cors');
const path = require('path');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

// ── Tank State ──────────────────────────────────────────────────────────
let tankState = {
  distanceCm: 0,         // raw ultrasonic distance in cm
  levelPercent: 0,        // calculated water level percentage
  pumpOn: false,          // is the pump relay active?
  mode: 'AUTO',           // 'AUTO' or 'MANUAL'
  online: false,
  lastSeen: null,
  rssi: 0                 // WiFi signal strength
};

// ── Tank Configuration ──────────────────────────────────────────────────
let tankConfig = {
  tankDepthCm: 100,       // total tank depth in cm (calibrated)
  lowThreshold: 20,       // % below which pump turns ON in AUTO mode
  highThreshold: 90,      // % above which pump turns OFF in AUTO mode
  sensorOffsetCm: 5       // distance from sensor to max water level
};

// ── History for Graph (last 100 readings) ───────────────────────────────
let history = [];
const MAX_HISTORY = 200;

function isDeviceOnline() {
  if (!tankState.lastSeen) return false;
  return (Date.now() - tankState.lastSeen) < 15000;
}

function calculateLevel(distanceCm) {
  const effectiveDepth = tankConfig.tankDepthCm - tankConfig.sensorOffsetCm;
  const waterHeight = effectiveDepth - (distanceCm - tankConfig.sensorOffsetCm);
  let percent = (waterHeight / effectiveDepth) * 100;
  percent = Math.max(0, Math.min(100, percent));
  return Math.round(percent * 10) / 10;
}

function broadcastState(sender = null) {
  const data = JSON.stringify({
    type: 'state',
    distanceCm: tankState.distanceCm,
    levelPercent: tankState.levelPercent,
    pumpOn: tankState.pumpOn,
    mode: tankState.mode,
    online: isDeviceOnline(),
    lastSeen: tankState.lastSeen,
    rssi: tankState.rssi
  });
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN && client !== sender) {
      client.send(data);
    }
  });
}

function broadcastConfig() {
  const data = JSON.stringify({
    type: 'config',
    ...tankConfig
  });
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(data);
    }
  });
}

function broadcastHistory() {
  const data = JSON.stringify({
    type: 'history',
    history: history
  });
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(data);
    }
  });
}

// ── REST API ────────────────────────────────────────────────────────────

app.get('/api/status', (req, res) => {
  res.json({
    ...tankState,
    online: isDeviceOnline(),
    config: tankConfig
  });
});

// ESP32 posts sensor data here
app.post('/api/device', (req, res) => {
  tankState.lastSeen = Date.now();
  
  if (req.body.distanceCm !== undefined) {
    tankState.distanceCm = parseFloat(req.body.distanceCm);
    tankState.levelPercent = calculateLevel(tankState.distanceCm);
  }
  if (req.body.rssi !== undefined) {
    tankState.rssi = req.body.rssi;
  }

  // Store history point
  history.push({
    time: Date.now(),
    level: tankState.levelPercent,
    distance: tankState.distanceCm
  });
  if (history.length > MAX_HISTORY) history.shift();

  // AUTO mode logic
  if (tankState.mode === 'AUTO') {
    if (tankState.levelPercent <= tankConfig.lowThreshold && !tankState.pumpOn) {
      tankState.pumpOn = true;
      console.log('[AUTO] Pump turned ON (level below low threshold)');
    }
    if (tankState.levelPercent >= tankConfig.highThreshold && tankState.pumpOn) {
      tankState.pumpOn = false;
      console.log('[AUTO] Pump turned OFF (level above high threshold)');
    }
  }

  broadcastState();

  res.json({
    pumpOn: tankState.pumpOn,
    mode: tankState.mode,
    config: tankConfig
  });
});

// Update config
app.post('/api/config', (req, res) => {
  if (req.body.tankDepthCm !== undefined) tankConfig.tankDepthCm = parseFloat(req.body.tankDepthCm);
  if (req.body.lowThreshold !== undefined) tankConfig.lowThreshold = parseFloat(req.body.lowThreshold);
  if (req.body.highThreshold !== undefined) tankConfig.highThreshold = parseFloat(req.body.highThreshold);
  if (req.body.sensorOffsetCm !== undefined) tankConfig.sensorOffsetCm = parseFloat(req.body.sensorOffsetCm);
  
  // Recalculate level with new config
  tankState.levelPercent = calculateLevel(tankState.distanceCm);
  
  broadcastConfig();
  broadcastState();
  console.log('[CONFIG] Updated:', tankConfig);
  res.json({ success: true, config: tankConfig });
});

// ── WebSocket Logic ─────────────────────────────────────────────────────

wss.on('connection', (ws) => {
  console.log('[WS] New client connected');

  // Send current state, config, and history
  ws.send(JSON.stringify({
    type: 'state',
    distanceCm: tankState.distanceCm,
    levelPercent: tankState.levelPercent,
    pumpOn: tankState.pumpOn,
    mode: tankState.mode,
    online: isDeviceOnline(),
    lastSeen: tankState.lastSeen,
    rssi: tankState.rssi
  }));

  ws.send(JSON.stringify({
    type: 'config',
    ...tankConfig
  }));

  ws.send(JSON.stringify({
    type: 'history',
    history: history
  }));

  ws.on('message', (message) => {
    try {
      const data = JSON.parse(message);

      // Dashboard control commands
      if (data.type === 'control') {
        if (data.mode !== undefined) {
          tankState.mode = data.mode;
          console.log(`[WS] Mode changed to: ${tankState.mode}`);
        }
        if (data.pumpOn !== undefined && tankState.mode === 'MANUAL') {
          tankState.pumpOn = data.pumpOn;
          console.log(`[WS] Manual pump control: ${tankState.pumpOn}`);
        }
        broadcastState(ws);
      }

      // Config updates from dashboard
      if (data.type === 'config') {
        if (data.tankDepthCm !== undefined) tankConfig.tankDepthCm = parseFloat(data.tankDepthCm);
        if (data.lowThreshold !== undefined) tankConfig.lowThreshold = parseFloat(data.lowThreshold);
        if (data.highThreshold !== undefined) tankConfig.highThreshold = parseFloat(data.highThreshold);
        if (data.sensorOffsetCm !== undefined) tankConfig.sensorOffsetCm = parseFloat(data.sensorOffsetCm);
        tankState.levelPercent = calculateLevel(tankState.distanceCm);
        broadcastConfig();
        broadcastState();
        console.log('[WS] Config updated:', tankConfig);
      }

      // ESP32 telemetry via WebSocket
      if (data.type === 'telemetry') {
        tankState.lastSeen = Date.now();
        if (data.distanceCm !== undefined) {
          tankState.distanceCm = parseFloat(data.distanceCm);
          tankState.levelPercent = calculateLevel(tankState.distanceCm);
        }
        if (data.rssi !== undefined) tankState.rssi = data.rssi;

        history.push({
          time: Date.now(),
          level: tankState.levelPercent,
          distance: tankState.distanceCm
        });
        if (history.length > MAX_HISTORY) history.shift();

        // AUTO mode logic
        if (tankState.mode === 'AUTO') {
          if (tankState.levelPercent <= tankConfig.lowThreshold && !tankState.pumpOn) {
            tankState.pumpOn = true;
          }
          if (tankState.levelPercent >= tankConfig.highThreshold && tankState.pumpOn) {
            tankState.pumpOn = false;
          }
        }

        broadcastState(ws);
        // Reply with pump command
        ws.send(JSON.stringify({ type: 'command', pumpOn: tankState.pumpOn }));
      }

      // Ping/heartbeat from ESP32
      if (data.type === 'ping') {
        tankState.lastSeen = Date.now();
        ws.send(JSON.stringify({ type: 'pong', pumpOn: tankState.pumpOn }));
        broadcastState();
      }

    } catch (err) {
      console.error('[WS] Error:', err.message);
    }
  });

  ws.on('close', () => {
    console.log('[WS] Client disconnected');
    broadcastState();
  });
});

// Periodic broadcast
setInterval(() => { broadcastState(); }, 5000);

server.listen(PORT, () => {
  console.log(`====================================================`);
  console.log(` JDT Water Tank Controller Server Running!`);
  console.log(` Port: ${PORT}`);
  console.log(` Dashboard: http://localhost:${PORT}`);
  console.log(`====================================================`);
});
