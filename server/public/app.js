/*
  JDT Water Tank Controller - Application Script for Image 2 Design
  Supports WebSockets, Light/Dark Theme Switcher, & Real-time Telemetry
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
  const tankDepthDisplay = document.getElementById('tank-depth-display');
  const rssiVal = document.getElementById('rssi-val');
  const calibrateDistance = document.getElementById('calibrate-distance');

  const pumpModeSub = document.getElementById('pump-mode-sub');
  const btnModeAuto = document.getElementById('btn-mode-auto');
  const btnModeManual = document.getElementById('btn-mode-manual');
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
    distanceCm: 33.5,
    tankDepthCm: 100,
    sensorOffsetCm: 20,
    rssi: -34,
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
    // Status Pill
    if (state.connected) {
      statusPill.className = 'status-pill online';
      statusText.textContent = 'ESP32 ONLINE';
    } else {
      statusPill.className = 'status-pill';
      statusText.textContent = 'DISCONNECTED';
    }

    const dist = state.distanceCm;
    const isError = state.sensorError || dist === -1;

    if (isError) {
      distanceCm.innerHTML = `-- <small>cm</small>`;
      calibrateDistance.textContent = `-- cm`;
      levelPercent.innerHTML = `0.0<small>%</small>`;
      waterFill.style.height = `0%`;
    } else {
      const displayDist = dist < 20 ? 20 : dist;
      distanceCm.innerHTML = `${displayDist.toFixed(1)} <small>cm</small>`;
      calibrateDistance.textContent = `${displayDist.toFixed(1)} cm`;

      // Level calculation
      const usableDepth = state.tankDepthCm - state.sensorOffsetCm;
      const waterDepth  = Math.max(0, usableDepth - (displayDist - state.sensorOffsetCm));
      let pct = ((waterDepth / usableDepth) * 100).toFixed(1);
      if (displayDist <= 20) pct = '100.0';

      levelPercent.innerHTML = `${pct}<small>%</small>`;
      waterFill.style.height = `${pct}%`;
    }

    tankDepthDisplay.innerHTML = `${state.tankDepthCm} <small>cm</small>`;
    rssiVal.innerHTML = `${state.rssi} <small>dBm</small>`;

    // Mode Selector
    if (state.pumpMode === 'AUTO') {
      btnModeAuto.classList.add('active');
      btnModeManual.classList.remove('active');
      pumpModeSub.textContent = 'Mode: AUTO (System Controls Motor)';
      pumpToggleInput.disabled = true;
    } else {
      btnModeAuto.classList.remove('active');
      btnModeManual.classList.add('active');
      pumpModeSub.textContent = 'Mode: MANUAL (User Direct Override)';
      pumpToggleInput.disabled = false;
    }

    // Pump State Toggle
    pumpToggleInput.checked = state.pumpState;
    if (state.pumpState) {
      pumpStateBadge.className = 'pump-state-badge on';
      pumpStateBadge.textContent = 'ON';
    } else {
      pumpStateBadge.className = 'pump-state-badge off';
      pumpStateBadge.textContent = 'OFF';
    }
  }

  // Start WebSockets & UI
  connectWebSocket();
  updateUI();
});
