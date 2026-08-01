/*
  JDT Water Tank Controller - Ultimate Application Script
  Includes Theme Switcher, WebSockets Telemetry, & Large Metrics
*/

document.addEventListener('DOMContentLoaded', () => {
  // DOM Elements
  const htmlDoc = document.documentElement;
  const btnThemeToggle = document.getElementById('btn-theme-toggle');
  const iconTheme = document.getElementById('icon-theme');
  const labelTheme = document.getElementById('label-theme');

  const statusPill = document.getElementById('status-pill');
  const statusText = document.getElementById('status-text');

  const waterFill = document.getElementById('water-fill');
  const levelPercent = document.getElementById('level-percent');
  const distanceCm = document.getElementById('distance-cm');
  const waterDepthCm = document.getElementById('water-depth-cm');
  const rssiVal = document.getElementById('rssi-val');
  const waterStatusBadge = document.getElementById('water-status-badge');
  const calibrateDistance = document.getElementById('calibrate-distance');

  const pumpModeSub = document.getElementById('pump-mode-sub');
  const btnModeAuto = document.getElementById('btn-mode-auto');
  const btnModeManual = document.getElementById('btn-mode-manual');
  const pumpRelayText = document.getElementById('pump-relay-text');
  const pumpStateBadge = document.getElementById('pump-state-badge');
  const pumpToggleInput = document.getElementById('pump-toggle-input');

  // Modal Elements
  const modalOverlay = document.getElementById('modal-overlay');
  const btnOpenSettings = document.getElementById('btn-open-settings');
  const modalClose = document.getElementById('modal-close');
  const btnSaveSettings = document.getElementById('btn-save-settings');

  const inputDepth = document.getElementById('input-depth');
  const inputOffset = document.getElementById('input-offset');
  const inputLow = document.getElementById('input-low');
  const lowVal = document.getElementById('low-val');
  const inputHigh = document.getElementById('input-high');
  const highVal = document.getElementById('high-val');
  const inputWifiSsid = document.getElementById('input-wifi-ssid');
  const inputWifiPass = document.getElementById('input-wifi-pass');

  // State
  let state = {
    connected: false,
    distanceCm: 28.8,
    tankDepthCm: 100,
    sensorOffsetCm: 20,
    rssi: -37,
    pumpState: false,
    pumpMode: 'AUTO',
    autoMinPct: 20,
    autoMaxPct: 90,
    sensorError: false
  };

  // ── 1. Theme Toggle (Light / Dark Mode) ──
  const savedTheme = localStorage.getItem('jdt_theme') || 'dark';
  applyTheme(savedTheme);

  btnThemeToggle.addEventListener('click', () => {
    const currentTheme = htmlDoc.getAttribute('data-theme');
    const newTheme = currentTheme === 'dark' ? 'light' : 'dark';
    applyTheme(newTheme);
    localStorage.setItem('jdt_theme', newTheme);
  });

  function applyTheme(theme) {
    htmlDoc.setAttribute('data-theme', theme);
    if (theme === 'light') {
      labelTheme.textContent = 'Light';
      if (iconTheme) iconTheme.setAttribute('data-lucide', 'sun');
    } else {
      labelTheme.textContent = 'Dark';
      if (iconTheme) iconTheme.setAttribute('data-lucide', 'moon');
    }
    if (window.lucide) lucide.createIcons();
  }

  // ── 2. Settings Modal Handlers ──
  btnOpenSettings.addEventListener('click', () => {
    inputDepth.value = state.tankDepthCm;
    inputOffset.value = state.sensorOffsetCm;
    inputLow.value = state.autoMinPct;
    lowVal.textContent = `${state.autoMinPct}%`;
    inputHigh.value = state.autoMaxPct;
    highVal.textContent = `${state.autoMaxPct}%`;
    
    modalOverlay.classList.add('show');
  });

  modalClose.addEventListener('click', () => {
    modalOverlay.classList.remove('show');
  });

  inputLow.addEventListener('input', (e) => lowVal.textContent = `${e.target.value}%`);
  inputHigh.addEventListener('input', (e) => highVal.textContent = `${e.target.value}%`);

  btnSaveSettings.addEventListener('click', () => {
    state.tankDepthCm = parseFloat(inputDepth.value) || 100;
    state.sensorOffsetCm = parseFloat(inputOffset.value) || 20;
    state.autoMinPct = parseInt(inputLow.value) || 20;
    state.autoMaxPct = parseInt(inputHigh.value) || 90;

    updateUI();
    modalOverlay.classList.remove('show');

    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({
        type: 'config',
        tankDepthCm: state.tankDepthCm,
        sensorOffsetCm: state.sensorOffsetCm,
        ssid: inputWifiSsid.value.trim(),
        pass: inputWifiPass.value.trim()
      }));
    }
  });

  // ── 3. Mode Switcher & Pump Action ──
  btnModeAuto.addEventListener('click', () => setMode('AUTO'));
  btnModeManual.addEventListener('click', () => setMode('MANUAL'));

  pumpToggleInput.addEventListener('change', (e) => {
    if (state.pumpMode !== 'MANUAL') return;
    sendPumpCommand(e.target.checked, 'MANUAL');
  });

  function setMode(mode) {
    state.pumpMode = mode;
    sendPumpCommand(state.pumpState, mode);
  }

  function sendPumpCommand(pumpVal, modeVal) {
    state.pumpState = pumpVal;
    state.pumpMode = modeVal;
    updateUI();

    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({
        type: 'control',
        pumpOn: pumpVal,
        mode: modeVal
      }));
    }
  }

  // ── 4. WebSockets Telemetry Client ──
  let ws;
  function connectWebSocket() {
    const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${location.host}/ws`;

    ws = new WebSocket(wsUrl);

    ws.onopen = () => {
      state.connected = true;
      updateUI();
    };

    ws.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data);
        if (data.type === 'telemetry' || data.distanceCm !== undefined) {
          state.connected = true;
          state.distanceCm = data.distanceCm;
          state.sensorError = data.sensorError || (data.distanceCm === -1);
          if (data.rssi !== undefined) state.rssi = data.rssi;
          updateUI();
        }
      } catch (e) {}
    };

    ws.onclose = () => {
      state.connected = false;
      updateUI();
      setTimeout(connectWebSocket, 3000);
    };

    ws.onerror = () => {
      state.connected = false;
      updateUI();
    };
  }

  // ── 5. UI Update Engine ──
  function updateUI() {
    // Status Badge
    if (state.connected) {
      statusPill.className = 'status-pill online';
      statusText.textContent = 'ONLINE';
    } else {
      statusPill.className = 'status-pill';
      statusText.textContent = 'OFFLINE';
    }

    const dist = state.distanceCm;
    const isError = state.sensorError || dist === -1;

    if (isError) {
      distanceCm.textContent = `--`;
      waterDepthCm.textContent = `--`;
      calibrateDistance.textContent = `-- cm`;
      levelPercent.textContent = `0.0`;
      waterFill.style.height = `0%`;
      waterStatusBadge.className = 'badge warning';
      waterStatusBadge.textContent = '⚠️ SENSOR ERROR';
    } else {
      const displayDist = dist < 20 ? 20 : dist;
      distanceCm.textContent = displayDist.toFixed(1);
      calibrateDistance.textContent = `${displayDist.toFixed(1)} cm`;

      // Level & depth calculation
      const usableDepth = state.tankDepthCm - state.sensorOffsetCm;
      const depth = Math.max(0, usableDepth - (displayDist - state.sensorOffsetCm));
      waterDepthCm.textContent = depth.toFixed(1);

      let pct = ((depth / usableDepth) * 100).toFixed(1);
      if (displayDist <= 20) pct = '100.0';

      levelPercent.textContent = `${pct}`;
      waterFill.style.height = `${pct}%`;

      if (parseFloat(pct) >= 90) {
        waterStatusBadge.className = 'badge success';
        waterStatusBadge.textContent = '✅ Tank Full';
      } else if (parseFloat(pct) <= 20) {
        waterStatusBadge.className = 'badge warning';
        waterStatusBadge.textContent = '⚠️ Level Low';
      } else {
        waterStatusBadge.className = 'badge success';
        waterStatusBadge.textContent = '💧 Level Normal';
      }
    }

    rssiVal.textContent = `${state.rssi}`;

    // Mode Switcher
    if (state.pumpMode === 'AUTO') {
      btnModeAuto.classList.add('active');
      btnModeManual.classList.remove('active');
      pumpModeSub.textContent = 'Mode: AUTOMATIC (System Controls Motor)';
      pumpToggleInput.disabled = true;
    } else {
      btnModeAuto.classList.remove('active');
      btnModeManual.classList.add('active');
      pumpModeSub.textContent = 'Mode: MANUAL OVERRIDE (User Direct Control)';
      pumpToggleInput.disabled = false;
    }

    // Pump Toggle
    pumpToggleInput.checked = state.pumpState;
    if (state.pumpState) {
      pumpStateBadge.className = 'pump-state-badge on';
      pumpStateBadge.textContent = 'ON';
      pumpRelayText.textContent = 'Relay: ACTIVE (ON)';
    } else {
      pumpStateBadge.className = 'pump-state-badge off';
      pumpStateBadge.textContent = 'OFF';
      pumpRelayText.textContent = 'Relay: IDLE (OFF)';
    }
  }

  // Start WebSockets & UI
  connectWebSocket();
  updateUI();
});
