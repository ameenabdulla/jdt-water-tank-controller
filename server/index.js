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

// ── Authentication Config (Stored in memory, can be changed via API) ───
let authConfig = {
  username: 'admin',
  password: 'admin@123'
};

// ── Tank State ──────────────────────────────────────────────────────────
let tankState = {
  distanceCm: 0,
  levelPercent: 0,
  pumpOn: false,
  mode: 'AUTO',
  online: false,
  lastSeen: null,
  rssi: 0
};

// ── Tank Configuration ──────────────────────────────────────────────────
let tankConfig = {
  tankDepthCm: 100,
  lowThreshold: 20,
  highThreshold: 90,
  sensorOffsetCm: 5
};

// ── History for Graph ───────────────────────────────────────────────────
let history = [];
const MAX_HISTORY = 50;

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

// ── REST API ────────────────────────────────────────────────────────────

// Login Endpoint
app.post('/api/login', (req, res) => {
  const { username, password } = req.body;
  if (username === authConfig.username && password === authConfig.password) {
    return res.json({ success: true, token: 'authenticated-session-token-jdt' });
  }
  return res.status(401).json({ success: false, error: 'Incorrect Username or Password' });
});

// Change Password Endpoint
app.post('/api/change-password', (req, res) => {
  const { currentPassword, newPassword } = req.body;
  if (currentPassword === authConfig.password) {
    authConfig.password = newPassword;
    console.log('[AUTH] Password changed successfully');
    return res.json({ success: true });
  }
  return res.status(400).json({ success: false, error: 'Incorrect current password' });
});

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

  // AUTO mode logic
  if (tankState.mode === 'AUTO') {
    if (tankState.levelPercent <= tankConfig.lowThreshold && !tankState.pumpOn) {
      tankState.pumpOn = true;
      console.log('[AUTO] Pump turned ON');
    }
    if (tankState.levelPercent >= tankConfig.highThreshold && tankState.pumpOn) {
      tankState.pumpOn = false;
      console.log('[AUTO] Pump turned OFF');
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
  
  tankState.levelPercent = calculateLevel(tankState.distanceCm);
  
  broadcastConfig();
  broadcastState();
  res.json({ success: true, config: tankConfig });
});

// ── WebSocket Logic ─────────────────────────────────────────────────────

wss.on('connection', (ws) => {
  console.log('[WS] New client connected');

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

  ws.on('message', (message) => {
    try {
      const data = JSON.parse(message);

      if (data.type === 'control') {
        if (data.mode !== undefined) {
          tankState.mode = data.mode;
        }
        if (data.pumpOn !== undefined && tankState.mode === 'MANUAL') {
          tankState.pumpOn = data.pumpOn;
        }
        broadcastState(ws);
      }

      if (data.type === 'config') {
        if (data.tankDepthCm !== undefined) tankConfig.tankDepthCm = parseFloat(data.tankDepthCm);
        if (data.lowThreshold !== undefined) tankConfig.lowThreshold = parseFloat(data.lowThreshold);
        if (data.highThreshold !== undefined) tankConfig.highThreshold = parseFloat(data.highThreshold);
        if (data.sensorOffsetCm !== undefined) tankConfig.sensorOffsetCm = parseFloat(data.sensorOffsetCm);
        tankState.levelPercent = calculateLevel(tankState.distanceCm);
        broadcastConfig();
        broadcastState();
      }

      // ESP32 telemetry via WebSocket
      if (data.type === 'telemetry') {
        tankState.lastSeen = Date.now();
        if (data.distanceCm !== undefined) {
          tankState.distanceCm = parseFloat(data.distanceCm);
          tankState.levelPercent = calculateLevel(tankState.distanceCm);
        }
        if (data.rssi !== undefined) tankState.rssi = data.rssi;

        if (tankState.mode === 'AUTO') {
          if (tankState.levelPercent <= tankConfig.lowThreshold && !tankState.pumpOn) {
            tankState.pumpOn = true;
          }
          if (tankState.levelPercent >= tankConfig.highThreshold && tankState.pumpOn) {
            tankState.pumpOn = false;
          }
        }

        broadcastState(ws);
        ws.send(JSON.stringify({ type: 'command', pumpOn: tankState.pumpOn }));
      }

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
    broadcastState();
  });
});

setInterval(() => { broadcastState(); }, 5000);

server.listen(PORT, () => {
  console.log(`====================================================`);
  console.log(` EET Water Tank Controller Server`);
  console.log(` Easy Electro Tech`);
  console.log(` Port: ${PORT}`);
  console.log(`====================================================`);
});
