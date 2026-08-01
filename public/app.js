/* AquaSense Pro — Dashboard Application Script
   Mobile-First · Dark/Light Mode · Live WebSocket Telemetry */

document.addEventListener('DOMContentLoaded', () => {

  // ── DOM Elements ──
  const connChip   = document.getElementById('conn-chip');
  const connDot    = document.getElementById('conn-dot');
  const connLabel  = document.getElementById('conn-label');
  const errorBanner = document.getElementById('error-banner');

  const distanceNum  = document.getElementById('distance-num');
  const distanceSub  = document.getElementById('distance-sub');
  const water        = document.getElementById('water');
  const beam         = document.getElementById('beam');
  const valLevelPct  = document.getElementById('val-level-pct');
  const valWaterStatus = document.getElementById('val-water-status');

  const valWaterCm = document.getElementById('val-water-cm');
  const valAirGap  = document.getElementById('val-air-gap');
  const valVolume  = document.getElementById('val-volume');
  const valRssi    = document.getElementById('val-rssi');

  const btnAuto    = document.getElementById('btn-auto');
  const btnManual  = document.getElementById('btn-manual');
  const pumpDesc   = document.getElementById('pump-desc');
  const pumpDot    = document.getElementById('pump-dot');
  const pumpStateText  = document.getElementById('pump-state-text');
  const pumpRelayText  = document.getElementById('pump-relay-text');
  const btnPump        = document.getElementById('btn-pump');
  const btnPumpLabel   = document.getElementById('btn-pump-label');

  const btnTheme       = document.getElementById('btn-theme');
  const btnSettings    = document.getElementById('btn-settings');
  const modalOverlay   = document.getElementById('modal-overlay');
  const btnCloseModal  = document.getElementById('btn-close-modal');
  const btnSaveCfg     = document.getElementById('btn-save-cfg');

  const cfgTankDepth     = document.getElementById('cfg-tank-depth');
  const cfgSensorOffset  = document.getElementById('cfg-sensor-offset');
  const cfgAutoOn        = document.getElementById('cfg-auto-on');
  const cfgAutoOnVal     = document.getElementById('cfg-auto-on-val');
  const cfgAutoOff       = document.getElementById('cfg-auto-off');
  const cfgAutoOffVal    = document.getElementById('cfg-auto-off-val');
  const cfgWifiSsid      = document.getElementById('cfg-wifi-ssid');
  const cfgWifiPass      = document.getElementById('cfg-wifi-pass');

  // ── Application State ──
  let state = {
    connected: false,
    sensorError: false,
    distanceCm: 0,
    waterLevelPct: 0,
    waterDepthCm: 0,
    tankDepthCm: 100,
    sensorOffsetCm: 20,
    volumeLiters: 0,
    pumpState: false,
    pumpMode: 'AUTO',
    rssi: -60,
    autoMinPct: 20,
    autoMaxPct: 95
  };

  // ── Theme Toggle ──
  const savedTheme = localStorage.getItem('aq-theme') || 'dark';
  document.documentElement.setAttribute('data-theme', savedTheme);

  btnTheme.addEventListener('click', () => {
    const current = document.documentElement.getAttribute('data-theme');
    const next = current === 'dark' ? 'light' : 'dark';
    document.documentElement.setAttribute('data-theme', next);
    localStorage.setItem('aq-theme', next);
    // Re-render Lucide icons for theme switch
    if (window.lucide) lucide.createIcons();
  });

  // ── Settings Modal ──
  btnSettings.addEventListener('click', () => {
    cfgTankDepth.value = state.tankDepthCm;
    cfgSensorOffset.value = state.sensorOffsetCm;
    cfgAutoOn.value = state.autoMinPct;
    cfgAutoOnVal.textContent = state.autoMinPct + '%';
    cfgAutoOff.value = state.autoMaxPct;
    cfgAutoOffVal.textContent = state.autoMaxPct + '%';
    modalOverlay.classList.add('open');
  });

  btnCloseModal.addEventListener('click', () => {
    modalOverlay.classList.remove('open');
  });

  modalOverlay.addEventListener('click', (e) => {
    if (e.target === modalOverlay) modalOverlay.classList.remove('open');
  });

  cfgAutoOn.addEventListener('input', (e) => {
    cfgAutoOnVal.textContent = e.target.value + '%';
  });
  cfgAutoOff.addEventListener('input', (e) => {
    cfgAutoOffVal.textContent = e.target.value + '%';
  });

  // ── Save Settings ──
  btnSaveCfg.addEventListener('click', async () => {
    const payload = {
      tankDepthCm: parseFloat(cfgTankDepth.value) || 100,
      sensorOffsetCm: parseFloat(cfgSensorOffset.value) || 20,
      autoMinPct: parseInt(cfgAutoOn.value) || 20,
      autoMaxPct: parseInt(cfgAutoOff.value) || 95,
      ssid: cfgWifiSsid.value.trim(),
      pass: cfgWifiPass.value.trim()
    };

    try {
      const res = await fetch('/api/settings', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });
      if (res.ok) {
        alert('✅ Settings saved to ESP32!');
        modalOverlay.classList.remove('open');
        fetchTelemetry();
      }
    } catch {
      state.tankDepthCm = payload.tankDepthCm;
      state.sensorOffsetCm = payload.sensorOffsetCm;
      state.autoMinPct = payload.autoMinPct;
      state.autoMaxPct = payload.autoMaxPct;
      updateUI();
      modalOverlay.classList.remove('open');
    }
  });

  // ── Mode Switchers ──
  btnAuto.addEventListener('click', () => setPumpMode('AUTO'));
  btnManual.addEventListener('click', () => setPumpMode('MANUAL'));

  // ── Pump Toggle ──
  btnPump.addEventListener('click', () => {
    if (state.pumpMode !== 'MANUAL') return;
    sendPumpCommand(!state.pumpState, 'MANUAL');
  });

  function setPumpMode(mode) {
    state.pumpMode = mode;
    sendPumpCommand(state.pumpState, mode);
  }

  async function sendPumpCommand(pumpVal, modeVal) {
    state.pumpState = pumpVal;
    state.pumpMode = modeVal;
    updateUI();
    try {
      await fetch('/api/pump', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ state: pumpVal, mode: modeVal })
      });
    } catch { /* offline mode */ }
  }

  // ── Telemetry Fetch ──
  async function fetchTelemetry() {
    try {
      const res = await fetch('/api/telemetry');
      if (res.ok) {
        const d = await res.json();
        state.connected = true;
        state.sensorError = d.sensorError || false;
        state.distanceCm = d.distanceCm;
        state.waterLevelPct = d.waterLevelPct;
        state.waterDepthCm = d.waterDepthCm;
        state.tankDepthCm = d.tankDepthCm || 100;
        state.sensorOffsetCm = d.sensorOffsetCm || 20;
        state.volumeLiters = d.volumeLiters;
        state.pumpState = d.pumpState;
        state.pumpMode = d.pumpMode || 'AUTO';
        state.rssi = d.rssi;
        state.autoMinPct = d.autoMinPct || 20;
        state.autoMaxPct = d.autoMaxPct || 95;
      }
    } catch {
      if (!state.connected) runDemo();
    }
    updateUI();
  }

  // ── Update UI ──
  function updateUI() {
    // Connection
    if (state.connected) {
      connChip.className = 'conn-chip online';
      connLabel.textContent = 'ONLINE';
    } else {
      connChip.className = 'conn-chip demo';
      connLabel.textContent = 'DEMO';
    }

    // Sensor error
    const hasError = state.sensorError || state.distanceCm === -1;
    errorBanner.style.display = hasError ? 'flex' : 'none';

    // Distance hero
    if (hasError) {
      distanceNum.textContent = '--';
      distanceSub.textContent = 'Sensor Error';
      distanceSub.style.color = 'var(--red)';
    } else {
      distanceNum.textContent = (typeof state.distanceCm === 'number') 
        ? state.distanceCm.toFixed(1) 
        : parseFloat(state.distanceCm || 0).toFixed(1);
      
      if (state.distanceCm <= 20) {
        distanceSub.textContent = '🔝 Very Close (Full)';
        distanceSub.style.color = 'var(--green)';
      } else if (state.distanceCm > 80) {
        distanceSub.textContent = '⚠️ Low Water Level';
        distanceSub.style.color = 'var(--amber)';
      } else {
        distanceSub.textContent = '📏 Measuring Live';
        distanceSub.style.color = 'var(--text-3)';
      }
    }

    // Water level %
    const pct = Math.min(100, Math.max(0, Math.round(state.waterLevelPct)));
    valLevelPct.textContent = pct + '%';
    water.style.height = pct + '%';
    beam.style.height = (100 - pct) + '%';

    // Status label
    if (hasError) {
      valWaterStatus.textContent = '⚠️ ERROR';
      valWaterStatus.style.color = 'var(--red)';
    } else if (pct >= 90) {
      valWaterStatus.textContent = '✅ Full';
      valWaterStatus.style.color = 'var(--green)';
    } else if (pct < 20) {
      valWaterStatus.textContent = '⚠️ Low';
      valWaterStatus.style.color = 'var(--red)';
    } else {
      valWaterStatus.textContent = '💧 Normal';
      valWaterStatus.style.color = 'var(--cyan)';
    }

    // Metric cards
    const waterCm = hasError ? '--' : (state.waterDepthCm || Math.round((pct / 100) * state.tankDepthCm));
    const airGap  = hasError ? '--' : (state.distanceCm || (state.tankDepthCm - Math.round((pct / 100) * state.tankDepthCm)));
    const volume  = hasError ? '--' : (state.volumeLiters || Math.round(pct * 10));

    valWaterCm.innerHTML = waterCm + ' <small>cm</small>';
    valAirGap.innerHTML  = (typeof airGap === 'number' ? airGap.toFixed(1) : airGap) + ' <small>cm</small>';
    valVolume.innerHTML   = volume + ' <small>L</small>';
    valRssi.innerHTML     = state.rssi + ' <small>dBm</small>';

    // Pump mode
    if (state.pumpMode === 'AUTO') {
      btnAuto.classList.add('active');
      btnManual.classList.remove('active');
      pumpDesc.textContent = `AUTO: ON at ≤${state.autoMinPct}%, OFF at ≥${state.autoMaxPct}%`;
      btnPump.disabled = true;
    } else {
      btnAuto.classList.remove('active');
      btnManual.classList.add('active');
      pumpDesc.textContent = 'MANUAL: Direct motor control below.';
      btnPump.disabled = false;
    }

    // Pump state
    if (state.pumpState) {
      pumpDot.classList.add('on');
      pumpStateText.textContent = 'PUMP RUNNING';
      pumpStateText.style.color = 'var(--green)';
      pumpRelayText.textContent = 'Relay: ACTIVE (HIGH)';
      btnPump.classList.add('on');
      btnPumpLabel.textContent = 'STOP';
    } else {
      pumpDot.classList.remove('on');
      pumpStateText.textContent = 'PUMP OFF';
      pumpStateText.style.color = 'var(--text-1)';
      pumpRelayText.textContent = 'Relay: IDLE (LOW)';
      btnPump.classList.remove('on');
      btnPumpLabel.textContent = 'START';
    }
  }

  // ── Demo Simulation ──
  let simPct = 45;
  let simDir = 1;
  function runDemo() {
    simPct += simDir * 0.5;
    if (simPct >= 95) simDir = -1;
    if (simPct <= 15) simDir = 1;
    state.waterLevelPct = simPct;
    state.tankDepthCm = 100;
    state.waterDepthCm = Math.round(simPct);
    state.distanceCm = 100 - state.waterDepthCm;
    state.volumeLiters = Math.round(simPct * 10);
    state.rssi = -58;
    if (state.pumpMode === 'AUTO') {
      if (simPct <= state.autoMinPct) state.pumpState = true;
      if (simPct >= state.autoMaxPct) state.pumpState = false;
    }
  }

  // ── Start ──
  fetchTelemetry();
  setInterval(fetchTelemetry, 1000);
});
