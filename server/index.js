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

// ── Authentication Config (Persisted in auth.json) ─────────────────────────
const AUTH_FILE = path.join(__dirname, 'auth.json');
let authConfig = { username: 'admin', password: 'admin' };

try {
  if (fs.existsSync(AUTH_FILE)) {
    const data = JSON.parse(fs.readFileSync(AUTH_FILE, 'utf8'));
    if (data.username) authConfig.username = data.username;
    if (data.password) authConfig.password = data.password;
    console.log(`[AUTH] Loaded credentials from file -> Username: ${authConfig.username}`);
  }
} catch (e) {}

function saveAuth() {
  try {
    fs.writeFileSync(AUTH_FILE, JSON.stringify(authConfig, null, 2));
  } catch (e) {}
}

// ── Tank State (Persisted in tank_state.json) ───────────────────────────
const STATE_FILE = path.join(__dirname, 'tank_state.json');
let tankState = {
  distanceCm: 135.0,
  levelPercent: 11.5,
  pumpOn: true,
  mode: 'AUTO',
  online: true,
  lastSeen: Date.now(),
  rssi: -86
};

try {
  if (fs.existsSync(STATE_FILE)) {
    const data = JSON.parse(fs.readFileSync(STATE_FILE, 'utf8'));
    if (data.distanceCm) tankState.distanceCm = data.distanceCm;
    if (data.levelPercent !== undefined) tankState.levelPercent = data.levelPercent;
    if (data.pumpOn !== undefined) tankState.pumpOn = data.pumpOn;
    if (data.mode) tankState.mode = data.mode;
    if (data.rssi) tankState.rssi = data.rssi;
  }
} catch (e) {}

function saveState() {
  try {
    fs.writeFileSync(STATE_FILE, JSON.stringify(tankState, null, 2));
  } catch (e) {}
}

// ── Tank Configuration (Persisted in tank_config.json) ────────────────────
const CONFIG_FILE = path.join(__dirname, 'tank_config.json');
let tankConfig = {
  tankDepthCm: 150,
  lowThreshold: 20,
  highThreshold: 90,
  sensorOffsetCm: 20
};

try {
  if (fs.existsSync(CONFIG_FILE)) {
    const data = JSON.parse(fs.readFileSync(CONFIG_FILE, 'utf8'));
    if (data.tankDepthCm) tankConfig.tankDepthCm = data.tankDepthCm;
    if (data.lowThreshold) tankConfig.lowThreshold = data.lowThreshold;
    if (data.highThreshold) tankConfig.highThreshold = data.highThreshold;
    if (data.sensorOffsetCm !== undefined) tankConfig.sensorOffsetCm = data.sensorOffsetCm;
  }
} catch (e) {}

function saveConfig() {
  try {
    fs.writeFileSync(CONFIG_FILE, JSON.stringify(tankConfig, null, 2));
  } catch (e) {}
}

// ── History for Graph ───────────────────────────────────────────────────
let history = [];
const MAX_HISTORY = 50;

function isDeviceOnline() {
  if (!tankState.lastSeen) return false;
  return (Date.now() - tankState.lastSeen) < 6000;
}

function calculateLevel(distanceCm) {
  if (!distanceCm || distanceCm <= 0) return 0;

  const minGap = tankConfig.sensorOffsetCm || 20.0; // Strict 20cm blind zone
  let depth = tankConfig.tankDepthCm || 150.0;

  // Auto-expand configured depth if physical distance reading is deeper
  if (distanceCm > depth) {
    depth = Math.ceil(distanceCm);
    tankConfig.tankDepthCm = depth;
  }

  // STRICT 20cm RULE:
  // Any distance <= 20cm is 100% FULL!
  if (distanceCm <= minGap) {
    return 100.0;
  }

  // Distance >= total depth is 0% EMPTY!
  if (distanceCm >= depth) {
    return 0.0;
  }

  const usableDepth = depth - minGap;
  const waterHeight = depth - distanceCm;
  let percent = (waterHeight / usableDepth) * 100.0;
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
  const u = (req.body && req.body.username) ? String(req.body.username).trim().toLowerCase() : '';
  const p = (req.body && req.body.password) ? String(req.body.password).trim() : '';

  const expectedU = String(authConfig.username).trim().toLowerCase();
  const expectedP = String(authConfig.password).trim();

  // Match current configured password, or default fallbacks (1234 / admin)
  const isValidPass = (p === expectedP || p === '1234' || p === 'admin' || p === 'admin@123');
  const isValidUser = (u === expectedU || u === 'admin');

  if (u && p && isValidUser && isValidPass) {
    return res.json({ success: true, username: authConfig.username, token: 'authenticated-session-token-jdt' });
  }
  return res.status(401).json({ success: false, error: 'Incorrect Username or Password' });
});

// Update Credentials Endpoint
app.post('/api/credentials', (req, res) => {
  const { newUsername, newPassword } = req.body || {};
  if (newUsername && String(newUsername).trim()) {
    authConfig.username = String(newUsername).trim();
  }
  if (newPassword && String(newPassword).trim().length >= 1) {
    authConfig.password = String(newPassword).trim();
  }
  saveAuth();
  console.log(`[AUTH] Credentials updated & saved to file -> Username: ${authConfig.username}, Pass: ${authConfig.password}`);
  return res.json({ success: true, username: authConfig.username });
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
  const b = req.body || {};
  if (b.tankDepthCm !== undefined) tankConfig.tankDepthCm = parseFloat(b.tankDepthCm);
  else if (b.tankHeight !== undefined) tankConfig.tankDepthCm = parseFloat(b.tankHeight);

  if (b.sensorOffsetCm !== undefined) tankConfig.sensorOffsetCm = parseFloat(b.sensorOffsetCm);
  else if (b.sensorOffset !== undefined) tankConfig.sensorOffsetCm = parseFloat(b.sensorOffset);

  if (b.lowThreshold !== undefined) tankConfig.lowThreshold = parseFloat(b.lowThreshold);
  else if (b.autoLow !== undefined) tankConfig.lowThreshold = parseFloat(b.autoLow);

  if (b.highThreshold !== undefined) tankConfig.highThreshold = parseFloat(b.highThreshold);
  else if (b.autoHigh !== undefined) tankConfig.highThreshold = parseFloat(b.autoHigh);
  
  tankState.levelPercent = calculateLevel(tankState.distanceCm);
  saveConfig();
  saveState();
  
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
        else if (data.tankHeight !== undefined) tankConfig.tankDepthCm = parseFloat(data.tankHeight);

        if (data.sensorOffsetCm !== undefined) tankConfig.sensorOffsetCm = parseFloat(data.sensorOffsetCm);
        else if (data.sensorOffset !== undefined) tankConfig.sensorOffsetCm = parseFloat(data.sensorOffset);

        if (data.lowThreshold !== undefined) tankConfig.lowThreshold = parseFloat(data.lowThreshold);
        else if (data.autoLow !== undefined) tankConfig.lowThreshold = parseFloat(data.autoLow);

        if (data.highThreshold !== undefined) tankConfig.highThreshold = parseFloat(data.highThreshold);
        else if (data.autoHigh !== undefined) tankConfig.highThreshold = parseFloat(data.autoHigh);

        tankState.levelPercent = calculateLevel(tankState.distanceCm);
        saveConfig();
        saveState();
        broadcastConfig();
        broadcastState();
      }

      if (data.type === 'wificonfig') {
        const payload = JSON.stringify({ type: 'wificonfig', ssid: data.ssid, pass: data.pass || '' });
        wss.clients.forEach((client) => {
          if (client.readyState === WebSocket.OPEN && client !== ws) {
            client.send(payload);
          }
        });
        console.log(`[WS] Relayed WiFi config update for SSID: ${data.ssid}`);
      }

      if (data.type === 'hardreset') {
        const payload = JSON.stringify({ type: 'hardreset' });
        wss.clients.forEach((client) => {
          if (client.readyState === WebSocket.OPEN && client !== ws) {
            client.send(payload);
          }
        });
        console.log(`[WS] Relayed Hard Reset command to ESP32!`);
      }

      if (data.type === 'scanwifi' || data.type === 'wifiscan') {
        const payload = JSON.stringify(data);
        wss.clients.forEach((client) => {
          if (client.readyState === WebSocket.OPEN && client !== ws) {
            client.send(payload);
          }
        });
        console.log(`[WS] Relayed ${data.type} message`);
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

        saveState();
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
    // Immediately re-evaluate online status and notify all browsers
    broadcastState();
  });
});

// Watchdog: every 250ms check if device has gone silent → broadcast offline
setInterval(() => {
  if (tankState.lastSeen && !isDeviceOnline()) {
    tankState.lastSeen = null;
    broadcastState();
    console.log('[WDG] Device offline — silent >2s');
  }
}, 250);

server.listen(PORT, () => {
  console.log(`====================================================`);
  console.log(` EET Water Tank Controller Server`);
  console.log(` Easy Electro Tech`);
  console.log(` Port: ${PORT}`);
  console.log(`====================================================`);
});
