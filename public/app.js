/*
  JDT Water Tank Controller — Production App Script
  All settings persist in localStorage.
  Tank Height comes from settings, not telemetry.
  Theme toggle, WebSocket telemetry, pump control.
*/

(function () {
  'use strict';

  // ─── LocalStorage Keys ───
  const KEYS = {
    THEME: 'jdt_theme',
    TANK_HEIGHT: 'jdt_tankHeight',
    SENSOR_OFFSET: 'jdt_sensorOffset',
    AUTO_LOW: 'jdt_autoLow',
    AUTO_HIGH: 'jdt_autoHigh'
  };

  // ─── Default Config ───
  const DEFAULTS = {
    theme: 'dark',
    tankHeight: 100,
    sensorOffset: 5,
    autoLow: 20,
    autoHigh: 90
  };

  // ─── Load persisted settings ───
  function loadNum(key, fallback) {
    const v = localStorage.getItem(key);
    if (v === null) return fallback;
    const n = parseFloat(v);
    return isNaN(n) ? fallback : n;
  }

  const config = {
    tankHeight: loadNum(KEYS.TANK_HEIGHT, DEFAULTS.tankHeight),
    sensorOffset: loadNum(KEYS.SENSOR_OFFSET, DEFAULTS.sensorOffset),
    autoLow: loadNum(KEYS.AUTO_LOW, DEFAULTS.autoLow),
    autoHigh: loadNum(KEYS.AUTO_HIGH, DEFAULTS.autoHigh)
  };

  // ─── Live state (from WebSocket) ───
  const live = {
    connected: false,
    distanceCm: -1,
    rssi: 0,
    pumpOn: false,
    mode: 'AUTO',
    sensorError: false
  };

  // ─── DOM Ready ───
  document.addEventListener('DOMContentLoaded', init);

  function init() {
    cacheDom();
    setupTheme();
    setupModal();
    setupPump();
    render();
    connectWS();
  }

  // ─── DOM Cache ───
  let $;
  function cacheDom() {
    $ = {
      html: document.documentElement,
      // Header
      statusPill: document.getElementById('status-pill'),
      statusText: document.getElementById('status-text'),
      btnTheme: document.getElementById('btn-theme'),
      themeIcon: document.getElementById('theme-icon'),
      themeLabel: document.getElementById('theme-label'),
      btnSettings: document.getElementById('btn-settings'),
      // Tank
      water: document.getElementById('water'),
      tankLevelText: document.getElementById('tank-level-text'),
      // Hero
      lvlNum: document.getElementById('lvl-num'),
      lvlBar: document.getElementById('lvl-bar'),
      // Metrics
      mDist: document.getElementById('m-dist'),
      mDepth: document.getElementById('m-depth'),
      mTank: document.getElementById('m-tank'),
      mRssi: document.getElementById('m-rssi'),
      rssiQuality: document.getElementById('rssi-quality'),
      // Pump
      pumpSub: document.getElementById('pump-sub'),
      tabAuto: document.getElementById('tab-auto'),
      tabManual: document.getElementById('tab-manual'),
      motorDetail: document.getElementById('motor-detail'),
      motorBadge: document.getElementById('motor-badge'),
      pumpSw: document.getElementById('pump-sw'),
      thLowVal: document.getElementById('th-low-val'),
      thHighVal: document.getElementById('th-high-val'),
      // Modal
      overlay: document.getElementById('overlay'),
      modalX: document.getElementById('modal-x'),
      btnCancel: document.getElementById('btn-cancel'),
      btnApply: document.getElementById('btn-apply'),
      sHeight: document.getElementById('s-height'),
      sOffset: document.getElementById('s-offset'),
      sLiveDist: document.getElementById('s-live-dist'),
      sLow: document.getElementById('s-low'),
      sLowLabel: document.getElementById('s-low-label'),
      sHigh: document.getElementById('s-high'),
      sHighLabel: document.getElementById('s-high-label'),
      sSsid: document.getElementById('s-ssid'),
      sPass: document.getElementById('s-pass')
    };
  }

  // ═══════════════════════════════════
  // THEME
  // ═══════════════════════════════════
  function setupTheme() {
    const saved = localStorage.getItem(KEYS.THEME) || DEFAULTS.theme;
    applyTheme(saved);

    $.btnTheme.addEventListener('click', () => {
      const current = $.html.getAttribute('data-theme');
      const next = current === 'dark' ? 'light' : 'dark';
      applyTheme(next);
      localStorage.setItem(KEYS.THEME, next);
    });
  }

  function applyTheme(theme) {
    $.html.setAttribute('data-theme', theme);
    const iconName = theme === 'dark' ? 'moon' : 'sun';
    if ($.themeIcon) $.themeIcon.setAttribute('data-lucide', iconName);
    if ($.themeLabel) $.themeLabel.textContent = theme === 'dark' ? 'Dark' : 'Light';
    if (window.lucide) lucide.createIcons();
  }

  // ═══════════════════════════════════
  // SETTINGS MODAL
  // ═══════════════════════════════════
  function setupModal() {
    $.btnSettings.addEventListener('click', openModal);
    $.modalX.addEventListener('click', closeModal);
    $.btnCancel.addEventListener('click', closeModal);
    $.overlay.addEventListener('click', (e) => {
      if (e.target === $.overlay) closeModal();
    });

    $.sLow.addEventListener('input', () => {
      $.sLowLabel.textContent = $.sLow.value + '%';
    });
    $.sHigh.addEventListener('input', () => {
      $.sHighLabel.textContent = $.sHigh.value + '%';
    });

    $.btnApply.addEventListener('click', saveSettings);
  }

  function openModal() {
    // Populate with current config
    $.sHeight.value = config.tankHeight;
    $.sOffset.value = config.sensorOffset;
    $.sLow.value = config.autoLow;
    $.sLowLabel.textContent = config.autoLow + '%';
    $.sHigh.value = config.autoHigh;
    $.sHighLabel.textContent = config.autoHigh + '%';

    // Show live distance
    if (live.distanceCm > 0 && !live.sensorError) {
      $.sLiveDist.textContent = live.distanceCm.toFixed(1) + ' cm';
    } else {
      $.sLiveDist.textContent = '-- cm';
    }

    $.overlay.classList.add('open');
  }

  function closeModal() {
    $.overlay.classList.remove('open');
  }

  function saveSettings() {
    const newHeight = parseFloat($.sHeight.value);
    const newOffset = parseFloat($.sOffset.value);
    const newLow = parseInt($.sLow.value);
    const newHigh = parseInt($.sHigh.value);

    // Validation
    if (!newHeight || newHeight < 10 || newHeight > 500) {
      alert('Tank height must be between 10 and 500 cm.');
      return;
    }
    if (isNaN(newOffset) || newOffset < 0 || newOffset >= newHeight) {
      alert('Sensor offset must be between 0 and tank height.');
      return;
    }
    if (newLow >= newHigh) {
      alert('Auto-ON threshold must be lower than Auto-OFF threshold.');
      return;
    }

    // Save to config + localStorage
    config.tankHeight = newHeight;
    config.sensorOffset = newOffset;
    config.autoLow = newLow;
    config.autoHigh = newHigh;

    localStorage.setItem(KEYS.TANK_HEIGHT, newHeight);
    localStorage.setItem(KEYS.SENSOR_OFFSET, newOffset);
    localStorage.setItem(KEYS.AUTO_LOW, newLow);
    localStorage.setItem(KEYS.AUTO_HIGH, newHigh);

    // Send to ESP32 via WebSocket
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({
        type: 'config',
        tankHeight: newHeight,
        sensorOffset: newOffset,
        autoLow: newLow,
        autoHigh: newHigh,
        ssid: $.sSsid.value.trim() || undefined,
        pass: $.sPass.value.trim() || undefined
      }));
    }

    render();
    closeModal();
  }

  // ═══════════════════════════════════
  // PUMP CONTROL
  // ═══════════════════════════════════
  function setupPump() {
    $.tabAuto.addEventListener('click', () => setMode('AUTO'));
    $.tabManual.addEventListener('click', () => setMode('MANUAL'));
    $.pumpSw.addEventListener('change', (e) => {
      if (live.mode !== 'MANUAL') return;
      sendControl(e.target.checked, 'MANUAL');
    });
  }

  function setMode(mode) {
    sendControl(live.pumpOn, mode);
  }

  function sendControl(pumpOn, mode) {
    live.pumpOn = pumpOn;
    live.mode = mode;
    render();
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({
        type: 'control',
        pumpOn: pumpOn,
        mode: mode
      }));
    }
  }

  // ═══════════════════════════════════
  // WEBSOCKET
  // ═══════════════════════════════════
  let ws;
  let reconnectTimer;

  function connectWS() {
    const proto = location.protocol === 'https:' ? 'wss:' : 'ws:';
    const url = proto + '//' + location.host + '/ws';

    ws = new WebSocket(url);

    ws.onopen = () => {
      live.connected = true;
      render();
    };

    ws.onmessage = (evt) => {
      try {
        const d = JSON.parse(evt.data);
        if (d.type === 'telemetry' || d.distanceCm !== undefined) {
          live.connected = true;
          live.distanceCm = d.distanceCm;
          live.sensorError = d.sensorError || d.distanceCm === -1;
          if (d.rssi !== undefined) live.rssi = d.rssi;
          if (d.pumpOn !== undefined) live.pumpOn = d.pumpOn;
          if (d.mode !== undefined) live.mode = d.mode;
          render();
        }
      } catch (e) { /* ignore malformed */ }
    };

    ws.onclose = () => {
      live.connected = false;
      render();
      clearTimeout(reconnectTimer);
      reconnectTimer = setTimeout(connectWS, 3000);
    };

    ws.onerror = () => {
      live.connected = false;
      render();
    };
  }

  // ═══════════════════════════════════
  // RENDER / UI UPDATE
  // ═══════════════════════════════════
  function render() {
    // ── Connection ──
    if (live.connected) {
      $.statusPill.className = 'status-pill online';
      $.statusText.textContent = 'ESP32 ONLINE';
    } else {
      $.statusPill.className = 'status-pill offline';
      $.statusText.textContent = 'OFFLINE';
    }

    // ── Calculations ──
    const isErr = live.sensorError || live.distanceCm <= 0;
    const tankH = config.tankHeight;
    const offset = config.sensorOffset;
    const usable = tankH - offset; // usable water column height

    let dist = live.distanceCm;
    let displayDist = dist;
    let waterDepth = 0;
    let levelPct = 0;

    if (!isErr && usable > 0) {
      // Clamp readings below offset to offset (100% full)
      if (dist < offset) displayDist = offset;

      // Water depth = tank height - measured distance
      waterDepth = Math.max(0, tankH - displayDist);

      // Level %
      levelPct = (waterDepth / usable) * 100;
      levelPct = Math.min(100, Math.max(0, levelPct));
    }

    // ── Tank Visualizer ──
    $.water.style.height = levelPct.toFixed(1) + '%';
    $.tankLevelText.textContent = levelPct.toFixed(1) + '%';

    // ── Hero Level ──
    $.lvlNum.textContent = isErr ? '--' : levelPct.toFixed(1);
    $.lvlBar.style.width = levelPct.toFixed(1) + '%';

    // ── Metrics ──
    $.mDist.textContent = isErr ? '--' : displayDist.toFixed(1);
    $.mDepth.textContent = isErr ? '--' : waterDepth.toFixed(1);
    $.mTank.textContent = tankH;  // Always shows configured tank height
    $.mRssi.textContent = live.connected ? live.rssi : '--';

    // WiFi quality label
    if (live.connected) {
      const r = live.rssi;
      if (r >= -50) $.rssiQuality.textContent = 'Excellent';
      else if (r >= -60) $.rssiQuality.textContent = 'Good';
      else if (r >= -70) $.rssiQuality.textContent = 'Fair';
      else $.rssiQuality.textContent = 'Weak';
    } else {
      $.rssiQuality.textContent = '--';
    }

    // Live distance in settings modal (if open)
    if ($.overlay.classList.contains('open')) {
      $.sLiveDist.textContent = isErr ? '-- cm' : displayDist.toFixed(1) + ' cm';
    }

    // ── Threshold display ──
    $.thLowVal.textContent = config.autoLow + '%';
    $.thHighVal.textContent = config.autoHigh + '%';

    // ── Pump Mode ──
    if (live.mode === 'AUTO') {
      $.tabAuto.classList.add('active');
      $.tabManual.classList.remove('active');
      $.pumpSub.textContent = 'Mode: AUTO';
      $.pumpSw.disabled = true;
    } else {
      $.tabAuto.classList.remove('active');
      $.tabManual.classList.add('active');
      $.pumpSub.textContent = 'Mode: MANUAL';
      $.pumpSw.disabled = false;
    }

    // ── Pump State ──
    $.pumpSw.checked = live.pumpOn;
    if (live.pumpOn) {
      $.motorBadge.className = 'motor-badge on';
      $.motorBadge.textContent = 'ON';
      $.motorDetail.textContent = 'Relay: ACTIVE';
    } else {
      $.motorBadge.className = 'motor-badge off';
      $.motorBadge.textContent = 'OFF';
      $.motorDetail.textContent = 'Relay: IDLE';
    }
  }

})();
