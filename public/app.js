/* 
  JDT Water Tank Controller - Mobile Application Script
  Includes Theme Switcher, Real-Time WebSockets Telemetry, & Settings
*/

document.addEventListener('DOMContentLoaded', () => {
  // DOM Elements
  const htmlDoc = document.documentElement;
  const btnThemeToggle = document.getElementById('btn-theme-toggle');
  const iconTheme = document.getElementById('icon-theme');
  const labelTheme = document.getElementById('label-theme');

  const connectionStatus = document.getElementById('connection-status');
  const connectionLabel = document.getElementById('connection-label');

  const valLevelPct = document.getElementById('val-level-pct');
  const valDistanceCm = document.getElementById('val-distance-cm');
  const valWaterCm = document.getElementById('val-water-cm');
  const valWaterStatus = document.getElementById('val-water-status');

  const waterFill = document.getElementById('water-fill');
  const sensorBeam = document.getElementById('sensor-beam');

  const valWifiRssi = document.getElementById('val-wifi-rssi');
  const valVolumeLiters = document.getElementById('val-volume-liters');

  const btnModeAuto = document.getElementById('btn-mode-auto');
  const btnModeManual = document.getElementById('btn-mode-manual');
  const pumpModeDesc = document.getElementById('pump-mode-desc');

  const pumpIndicator = document.getElementById('pump-indicator');
  const lblPumpState = document.getElementById('lbl-pump-state');
  const lblPumpSub = document.getElementById('lbl-pump-sub');
  const btnTogglePump = document.getElementById('btn-toggle-pump');
  const lblBtnPump = document.getElementById('lbl-btn-pump');

  // Modal Elements
  const settingsModal = document.getElementById('settings-modal');
  const btnOpenSettings = document.getElementById('btn-open-settings');
  const btnCloseSettings = document.getElementById('btn-close-settings');
  const btnSaveSettings = document.getElementById('btn-save-settings');

  const inputTankDepth = document.getElementById('input-tank-depth');
  const inputSensorOffset = document.getElementById('input-sensor-offset');
  const inputAutoMin = document.getElementById('input-auto-min');
  const valAutoMin = document.getElementById('val-auto-min');
  const inputAutoMax = document.getElementById('input-auto-max');
  const valAutoMax = document.getElementById('val-auto-max');
  const inputWifiSsid = document.getElementById('input-wifi-ssid');
  const inputWifiPass = document.getElementById('input-wifi-pass');

  // State
  let state = {
    connected: false,
    apMode: false,
    waterLevelPct: 0,
    distanceCm: 0,
    waterDepthCm: 0,
    tankDepthCm: 100,
    sensorOffsetCm: 20,
    volumeLiters: 0,
    pumpState: false,
    pumpMode: 'AUTO',
    rssi: -60,
    autoMinPct: 20,
    autoMaxPct: 95,
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

  // ── 2. Settings Modal Event Handlers ──
  inputAutoMin.addEventListener('input', (e) => {
    valAutoMin.textContent = `${e.target.value}%`;
  });

  inputAutoMax.addEventListener('input', (e) => {
    valAutoMax.textContent = `${e.target.value}%`;
  });

  btnOpenSettings.addEventListener('click', () => {
    inputTankDepth.value = state.tankDepthCm;
    inputSensorOffset.value = state.sensorOffsetCm;
    inputAutoMin.value = state.autoMinPct;
    valAutoMin.textContent = `${state.autoMinPct}%`;
    inputAutoMax.value = state.autoMaxPct;
    valAutoMax.textContent = `${state.autoMaxPct}%`;
    
    settingsModal.classList.add('open');
  });

  btnCloseSettings.addEventListener('click', () => {
    settingsModal.classList.remove('open');
  });

  // ── 3. Mode Switcher & Pump Action ──
  btnModeAuto.addEventListener('click', () => setPumpMode('AUTO'));
  btnModeManual.addEventListener('click', () => setPumpMode('MANUAL'));

  btnTogglePump.addEventListener('click', () => {
    if (state.pumpMode !== 'MANUAL') return;
    const nextState = !state.pumpState;
    sendPumpCommand(nextState, 'MANUAL');
  });

  btnSaveSettings.addEventListener('click', async () => {
    const payload = {
      tankDepthCm: parseFloat(inputTankDepth.value) || 100,
      sensorOffsetCm: parseFloat(inputSensorOffset.value) || 20,
      autoMinPct: parseInt(inputAutoMin.value) || 20,
      autoMaxPct: parseInt(inputAutoMax.value) || 95,
      ssid: inputWifiSsid.value.trim(),
      pass: inputWifiPass.value.trim()
    };

    try {
      const res = await fetch('/api/settings', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });
      if (res.ok) {
        alert('Settings saved successfully!');
        settingsModal.classList.remove('open');
      }
    } catch (err) {
      state.tankDepthCm = payload.tankDepthCm;
      state.sensorOffsetCm = payload.sensorOffsetCm;
      state.autoMinPct = payload.autoMinPct;
      state.autoMaxPct = payload.autoMaxPct;
      updateUI();
      settingsModal.classList.remove('open');
    }
  });

  function setPumpMode(mode) {
    state.pumpMode = mode;
    sendPumpCommand(state.pumpState, mode);
  }

  function sendPumpCommand(pumpStateVal, modeVal) {
    state.pumpState = pumpStateVal;
    state.pumpMode = modeVal;
    updateUI();

    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({
        type: 'control',
        pumpOn: pumpStateVal,
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
    // Connection Status Badge
    if (state.connected) {
      connectionStatus.className = 'status-badge online';
      connectionLabel.textContent = 'ONLINE';
    } else {
      connectionStatus.className = 'status-badge offline';
      connectionLabel.textContent = 'OFFLINE';
    }

    // Distance calculation
    const rawDist = state.distanceCm;
    const isError = state.sensorError || rawDist === -1;

    if (isError) {
      valDistanceCm.textContent = '--';
      valWaterCm.textContent = '--';
      valLevelPct.textContent = '0';
      waterFill.style.height = '0%';
      sensorBeam.style.height = '100%';
      valWaterStatus.className = 'status-tag warning';
      valWaterStatus.textContent = '⚠️ SENSOR DISCONNECTED';
      valVolumeLiters.textContent = '-- L';
    } else {
      // Valid distance
      const displayDist = rawDist < 20 ? 20 : rawDist;
      valDistanceCm.textContent = displayDist.toFixed(1);

      // Usable depth calculation
      const usableDepth = state.tankDepthCm - state.sensorOffsetCm;
      const waterDepth = Math.max(0, usableDepth - (displayDist - state.sensorOffsetCm));
      valWaterCm.textContent = waterDepth.toFixed(1);

      // Percentage calculation
      let pct = Math.round((waterDepth / usableDepth) * 100);
      pct = Math.min(100, Math.max(0, pct));
      if (displayDist <= 20) pct = 100;

      valLevelPct.textContent = `${pct}`;
      waterFill.style.height = `${pct}%`;
      sensorBeam.style.height = `${100 - pct}%`;

      // Est Volume (assuming 1000L tank capacity)
      const estLiters = Math.round((pct / 100) * 1000);
      valVolumeLiters.textContent = `${estLiters} L`;

      // Status Tag
      if (pct < 20) {
        valWaterStatus.className = 'status-tag warning';
        valWaterStatus.textContent = '⚠️ Water Level Low';
      } else if (pct >= 90) {
        valWaterStatus.className = 'status-tag success';
        valWaterStatus.textContent = '✅ Tank Full';
      } else {
        valWaterStatus.className = 'status-tag normal';
        valWaterStatus.textContent = '💧 Level Normal';
      }
    }

    // WiFi RSSI
    valWifiRssi.textContent = `${state.rssi} dBm`;

    // Pump Mode Buttons
    if (state.pumpMode === 'AUTO') {
      btnModeAuto.classList.add('active');
      btnModeManual.classList.remove('active');
      pumpModeDesc.textContent = `AUTOMATIC: Turns ON at ≤${state.autoMinPct}% and OFF at ≥${state.autoMaxPct}%.`;
      btnTogglePump.disabled = true;
    } else {
      btnModeAuto.classList.remove('active');
      btnModeManual.classList.add('active');
      pumpModeDesc.textContent = 'MANUAL OVERRIDE: Directly control motor power below.';
      btnTogglePump.disabled = false;
    }

    // Pump State Action
    if (state.pumpState) {
      pumpIndicator.classList.add('active');
      lblPumpState.textContent = 'PUMP RUNNING';
      lblPumpState.style.color = 'var(--accent-green)';
      lblPumpSub.textContent = 'Relay Status: ACTIVE (HIGH)';
      btnTogglePump.classList.add('on');
      lblBtnPump.textContent = 'STOP PUMP';
    } else {
      pumpIndicator.classList.remove('active');
      lblPumpState.textContent = 'PUMP OFF';
      lblPumpState.style.color = 'var(--text-main)';
      lblPumpSub.textContent = 'Relay Status: IDLE (LOW)';
      btnTogglePump.classList.remove('on');
      lblBtnPump.textContent = 'START PUMP';
    }
  }

  // Start WebSockets
  connectWebSocket();
  updateUI();
});
