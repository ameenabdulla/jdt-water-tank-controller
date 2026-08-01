/*
  JDT Water Tank Controller — App Script
  WebSocket telemetry, pump control, settings modal
*/

document.addEventListener('DOMContentLoaded', () => {
  // DOM refs
  const statusPill = document.getElementById('status-pill');
  const statusText = document.getElementById('status-text');
  const waterFill = document.getElementById('water-fill');
  const levelPercent = document.getElementById('level-percent');
  const distanceCm = document.getElementById('distance-cm');
  const waterDepthCm = document.getElementById('water-depth-cm');
  const rssiVal = document.getElementById('rssi-val');
  const calibrateDistance = document.getElementById('calibrate-distance');

  const pumpModeSub = document.getElementById('pump-mode-sub');
  const btnModeAuto = document.getElementById('btn-mode-auto');
  const btnModeManual = document.getElementById('btn-mode-manual');
  const pumpStateText = document.getElementById('pump-state-text');
  const pumpToggleInput = document.getElementById('pump-toggle-input');

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
    distanceCm: 29.2,
    tankDepthCm: 100,
    sensorOffsetCm: 20,
    rssi: -35,
    pumpState: false,
    pumpMode: 'AUTO',
    autoMinPct: 20,
    autoMaxPct: 90,
    sensorError: false
  };

  // Settings Modal
  btnOpenSettings.addEventListener('click', () => {
    inputDepth.value = state.tankDepthCm;
    inputOffset.value = state.sensorOffsetCm;
    inputLow.value = state.autoMinPct;
    lowVal.textContent = state.autoMinPct + '%';
    inputHigh.value = state.autoMaxPct;
    highVal.textContent = state.autoMaxPct + '%';
    modalOverlay.classList.add('show');
  });
  modalClose.addEventListener('click', () => modalOverlay.classList.remove('show'));
  modalOverlay.addEventListener('click', (e) => { if (e.target === modalOverlay) modalOverlay.classList.remove('show'); });

  inputLow.addEventListener('input', (e) => lowVal.textContent = e.target.value + '%');
  inputHigh.addEventListener('input', (e) => highVal.textContent = e.target.value + '%');

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

  // Mode & Pump
  btnModeAuto.addEventListener('click', () => setMode('AUTO'));
  btnModeManual.addEventListener('click', () => setMode('MANUAL'));
  pumpToggleInput.addEventListener('change', (e) => {
    if (state.pumpMode !== 'MANUAL') return;
    sendPump(e.target.checked, 'MANUAL');
  });

  function setMode(mode) { sendPump(state.pumpState, mode); }
  function sendPump(on, mode) {
    state.pumpState = on;
    state.pumpMode = mode;
    updateUI();
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({ type: 'control', pumpOn: on, mode: mode }));
    }
  }

  // WebSocket
  let ws;
  function connectWS() {
    const proto = location.protocol === 'https:' ? 'wss:' : 'ws:';
    ws = new WebSocket(proto + '//' + location.host + '/ws');
    ws.onopen = () => { state.connected = true; updateUI(); };
    ws.onmessage = (e) => {
      try {
        const d = JSON.parse(e.data);
        if (d.type === 'telemetry' || d.distanceCm !== undefined) {
          state.connected = true;
          state.distanceCm = d.distanceCm;
          state.sensorError = d.sensorError || (d.distanceCm === -1);
          if (d.rssi !== undefined) state.rssi = d.rssi;
          updateUI();
        }
      } catch (err) {}
    };
    ws.onclose = () => { state.connected = false; updateUI(); setTimeout(connectWS, 3000); };
    ws.onerror = () => { state.connected = false; updateUI(); };
  }

  // UI Update
  function updateUI() {
    // Connection status
    if (state.connected) {
      statusPill.className = 'status-pill online';
      statusText.textContent = 'ESP32 ONLINE';
    } else {
      statusPill.className = 'status-pill';
      statusText.textContent = 'OFFLINE';
    }

    const dist = state.distanceCm;
    const isErr = state.sensorError || dist === -1;

    if (isErr) {
      distanceCm.textContent = '--';
      waterDepthCm.textContent = '--';
      calibrateDistance.textContent = '-- cm';
      levelPercent.textContent = '0.0';
      waterFill.style.height = '0%';
    } else {
      const displayDist = dist < 20 ? 20 : dist;
      distanceCm.textContent = displayDist.toFixed(1);
      calibrateDistance.textContent = displayDist.toFixed(1) + ' cm';

      const usable = state.tankDepthCm - state.sensorOffsetCm;
      const depth = Math.max(0, usable - (displayDist - state.sensorOffsetCm));
      waterDepthCm.textContent = depth.toFixed(0);

      let pct = (depth / usable) * 100;
      if (displayDist <= 20) pct = 100;
      pct = Math.min(100, Math.max(0, pct));

      levelPercent.textContent = pct.toFixed(1);
      waterFill.style.height = pct + '%';
    }

    rssiVal.textContent = state.rssi;

    // Mode
    if (state.pumpMode === 'AUTO') {
      btnModeAuto.classList.add('active');
      btnModeManual.classList.remove('active');
      pumpModeSub.textContent = 'Mode: AUTO';
      pumpToggleInput.disabled = true;
    } else {
      btnModeAuto.classList.remove('active');
      btnModeManual.classList.add('active');
      pumpModeSub.textContent = 'Mode: MANUAL';
      pumpToggleInput.disabled = false;
    }

    // Pump state
    pumpToggleInput.checked = state.pumpState;
    if (state.pumpState) {
      pumpStateText.textContent = 'ON';
      pumpStateText.className = 'motor-state on';
    } else {
      pumpStateText.textContent = 'OFF';
      pumpStateText.className = 'motor-state';
    }
  }

  connectWS();
  updateUI();
});
