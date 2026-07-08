// ── Connection Setup ────────────────────────────────────────────────────
const host = window.location.host;
const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
const WS_URL = `${protocol}//${host}`;
const API_URL = `${window.location.protocol}//${host}`;

// ── UI Elements ─────────────────────────────────────────────────────────
const connectionBadge = document.getElementById('connection-badge');
const badgeText = document.getElementById('badge-text');
const waterFill = document.getElementById('water-fill');
const levelPercent = document.getElementById('level-percent');
const distanceCm = document.getElementById('distance-cm');
const tankDepthDisplay = document.getElementById('tank-depth-display');
const rssiVal = document.getElementById('rssi-val');
const pumpToggle = document.getElementById('pump-toggle');
const pumpStateIndicator = document.getElementById('pump-state-indicator');
const pumpStateText = document.getElementById('pump-state-text');
const pumpStatusText = document.getElementById('pump-status-text');
const pumpHint = document.getElementById('pump-hint');
const autoBtn = document.getElementById('auto-btn');
const manualBtn = document.getElementById('manual-btn');
const settingsBtn = document.getElementById('settings-btn');
const modalOverlay = document.getElementById('modal-overlay');
const modalClose = document.getElementById('modal-close');
const inputDepth = document.getElementById('input-depth');
const inputOffset = document.getElementById('input-offset');
const inputLow = document.getElementById('input-low');
const inputHigh = document.getElementById('input-high');
const lowVal = document.getElementById('low-val');
const highVal = document.getElementById('high-val');
const calibrateDistance = document.getElementById('calibrate-distance');
const btnSave = document.getElementById('btn-save-settings');

// ── State ───────────────────────────────────────────────────────────────
let socket = null;
let currentState = { levelPercent: 0, distanceCm: 0, pumpOn: false, mode: 'AUTO', online: false, rssi: 0 };
let currentConfig = { tankDepthCm: 100, sensorOffsetCm: 5, lowThreshold: 20, highThreshold: 90 };
let levelChart = null;

// ── Chart.js Setup ──────────────────────────────────────────────────────
function initChart() {
  const ctx = document.getElementById('levelChart').getContext('2d');

  const gradient = ctx.createLinearGradient(0, 0, 0, 220);
  gradient.addColorStop(0, 'rgba(14, 165, 233, 0.25)');
  gradient.addColorStop(1, 'rgba(14, 165, 233, 0.0)');

  levelChart = new Chart(ctx, {
    type: 'line',
    data: {
      labels: [],
      datasets: [{
        label: 'Water Level %',
        data: [],
        borderColor: '#0ea5e9',
        backgroundColor: gradient,
        borderWidth: 2,
        fill: true,
        tension: 0.4,
        pointRadius: 0,
        pointHoverRadius: 4,
        pointHoverBackgroundColor: '#22d3ee',
        pointHoverBorderColor: '#fff'
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      interaction: { mode: 'index', intersect: false },
      plugins: {
        legend: { display: false },
        tooltip: {
          backgroundColor: '#111',
          borderColor: '#222',
          borderWidth: 1,
          titleColor: '#888',
          bodyColor: '#0ea5e9',
          bodyFont: { family: "'JetBrains Mono', monospace", weight: 'bold' },
          padding: 10,
          cornerRadius: 8,
          callbacks: {
            label: (ctx) => ` ${ctx.parsed.y.toFixed(1)}%`
          }
        }
      },
      scales: {
        x: {
          display: true,
          grid: { color: 'rgba(255,255,255,0.03)', drawBorder: false },
          ticks: { color: '#333', font: { size: 9, family: "'JetBrains Mono'" }, maxTicksLimit: 8 }
        },
        y: {
          display: true,
          min: 0, max: 100,
          grid: { color: 'rgba(255,255,255,0.03)', drawBorder: false },
          ticks: {
            color: '#333',
            font: { size: 9, family: "'JetBrains Mono'" },
            callback: (v) => v + '%',
            stepSize: 25
          }
        }
      }
    }
  });
}

function addChartPoint(level, timestamp) {
  const date = new Date(timestamp);
  const timeLabel = date.toLocaleTimeString('en-US', { hour12: false, hour: '2-digit', minute: '2-digit', second: '2-digit' });

  levelChart.data.labels.push(timeLabel);
  levelChart.data.datasets[0].data.push(level);

  // Keep only last 50 points visible
  if (levelChart.data.labels.length > 50) {
    levelChart.data.labels.shift();
    levelChart.data.datasets[0].data.shift();
  }

  levelChart.update('none'); // 'none' for no animation (smooth performance)
}

function loadHistory(historyData) {
  if (!historyData || historyData.length === 0) return;

  const labels = [];
  const values = [];

  historyData.forEach(point => {
    const date = new Date(point.time);
    labels.push(date.toLocaleTimeString('en-US', { hour12: false, hour: '2-digit', minute: '2-digit', second: '2-digit' }));
    values.push(point.level);
  });

  // Keep last 50
  const sliceStart = Math.max(0, labels.length - 50);
  levelChart.data.labels = labels.slice(sliceStart);
  levelChart.data.datasets[0].data = values.slice(sliceStart);
  levelChart.update('none');
}

// ── UI Update Functions ─────────────────────────────────────────────────
function updateTankUI(state) {
  // Water fill animation
  waterFill.style.height = state.levelPercent + '%';

  // Change water color based on level
  if (state.levelPercent < 20) {
    waterFill.style.background = 'linear-gradient(180deg, rgba(239,68,68,0.6) 0%, rgba(180,40,40,0.4) 100%)';
  } else if (state.levelPercent < 50) {
    waterFill.style.background = 'linear-gradient(180deg, rgba(245,158,11,0.6) 0%, rgba(180,100,10,0.4) 100%)';
  } else {
    waterFill.style.background = 'linear-gradient(180deg, rgba(14,165,233,0.6) 0%, rgba(14,165,233,0.35) 40%, rgba(6,95,140,0.5) 100%)';
  }

  // Live numbers
  levelPercent.innerHTML = state.levelPercent.toFixed(1) + '<small>%</small>';
  distanceCm.innerHTML = state.distanceCm.toFixed(1) + ' <small>cm</small>';
  rssiVal.innerHTML = state.rssi + ' <small>dBm</small>';

  // Add chart point
  addChartPoint(state.levelPercent, Date.now());

  // Update calibration distance in settings modal
  calibrateDistance.textContent = state.distanceCm.toFixed(1) + ' cm';
}

function updatePumpUI(state) {
  pumpToggle.checked = state.pumpOn;
  pumpStateText.textContent = state.pumpOn ? 'ON' : 'OFF';
  pumpStateIndicator.className = 'pump-state ' + (state.pumpOn ? 'on' : 'off');

  // Mode UI
  if (state.mode === 'AUTO') {
    autoBtn.classList.add('active');
    manualBtn.classList.remove('active');
    pumpToggle.disabled = true;
    pumpStatusText.textContent = 'Mode: AUTO';
    pumpHint.textContent = 'Pump is controlled automatically based on thresholds.';
  } else {
    autoBtn.classList.remove('active');
    manualBtn.classList.add('active');
    pumpToggle.disabled = false;
    pumpStatusText.textContent = 'Mode: MANUAL';
    pumpHint.textContent = 'You have full manual control over the pump.';
  }
}

function updateDeviceStatus(online) {
  if (online) {
    connectionBadge.className = 'badge online';
    badgeText.textContent = 'ESP32 ONLINE';
  } else {
    connectionBadge.className = 'badge offline';
    badgeText.textContent = 'ESP32 OFFLINE';
  }
}

function updateConfigUI(config) {
  tankDepthDisplay.innerHTML = config.tankDepthCm + ' <small>cm</small>';
  inputDepth.value = config.tankDepthCm;
  inputOffset.value = config.sensorOffsetCm;
  inputLow.value = config.lowThreshold;
  inputHigh.value = config.highThreshold;
  lowVal.textContent = config.lowThreshold + '%';
  highVal.textContent = config.highThreshold + '%';
}

// ── WebSocket ───────────────────────────────────────────────────────────
function connectWebSocket() {
  socket = new WebSocket(WS_URL);

  socket.onopen = () => {
    console.log('Connected to server');
  };

  socket.onmessage = (event) => {
    try {
      const data = JSON.parse(event.data);

      if (data.type === 'state') {
        currentState = data;
        updateTankUI(data);
        updatePumpUI(data);
        updateDeviceStatus(data.online);
      }

      if (data.type === 'config') {
        currentConfig = data;
        updateConfigUI(data);
      }

      if (data.type === 'history') {
        loadHistory(data.history);
      }
    } catch (err) {
      console.error('WS parse error:', err);
    }
  };

  socket.onclose = () => {
    console.log('Disconnected. Reconnecting in 3s...');
    updateDeviceStatus(false);
    setTimeout(connectWebSocket, 3000);
  };

  socket.onerror = () => {};
}

// ── Controls Events ─────────────────────────────────────────────────────

// Mode buttons
autoBtn.addEventListener('click', () => {
  if (!socket || socket.readyState !== WebSocket.OPEN) return;
  socket.send(JSON.stringify({ type: 'control', mode: 'AUTO' }));
  currentState.mode = 'AUTO';
  updatePumpUI(currentState);
});

manualBtn.addEventListener('click', () => {
  if (!socket || socket.readyState !== WebSocket.OPEN) return;
  socket.send(JSON.stringify({ type: 'control', mode: 'MANUAL' }));
  currentState.mode = 'MANUAL';
  updatePumpUI(currentState);
});

// Pump toggle (manual mode only)
pumpToggle.addEventListener('change', () => {
  if (!socket || socket.readyState !== WebSocket.OPEN) return;
  socket.send(JSON.stringify({ type: 'control', pumpOn: pumpToggle.checked }));
  currentState.pumpOn = pumpToggle.checked;
  updatePumpUI(currentState);
});

// ── Settings Modal ──────────────────────────────────────────────────────
settingsBtn.addEventListener('click', () => {
  modalOverlay.classList.add('show');
});

modalClose.addEventListener('click', () => {
  modalOverlay.classList.remove('show');
});

modalOverlay.addEventListener('click', (e) => {
  if (e.target === modalOverlay) modalOverlay.classList.remove('show');
});

// Sliders live update
inputLow.addEventListener('input', () => { lowVal.textContent = inputLow.value + '%'; });
inputHigh.addEventListener('input', () => { highVal.textContent = inputHigh.value + '%'; });

// Save settings
btnSave.addEventListener('click', () => {
  if (!socket || socket.readyState !== WebSocket.OPEN) return;

  const newConfig = {
    type: 'config',
    tankDepthCm: parseFloat(inputDepth.value),
    sensorOffsetCm: parseFloat(inputOffset.value),
    lowThreshold: parseFloat(inputLow.value),
    highThreshold: parseFloat(inputHigh.value)
  };

  socket.send(JSON.stringify(newConfig));

  // Update local display
  currentConfig = { ...currentConfig, ...newConfig };
  updateConfigUI(currentConfig);

  // Close modal
  modalOverlay.classList.remove('show');

  // Button feedback animation
  btnSave.textContent = '✓ Saved!';
  setTimeout(() => {
    btnSave.innerHTML = '<i data-lucide="save"></i> Save Settings';
    lucide.createIcons();
  }, 1500);
});

// ── Init ────────────────────────────────────────────────────────────────
initChart();
connectWebSocket();
