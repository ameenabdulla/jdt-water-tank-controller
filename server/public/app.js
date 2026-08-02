/*
  JDT Water Tank Controller — Production App
  Login system, settings with credentials, theme, WebSocket, pump control
  All config persisted in localStorage
*/
(function () {
  'use strict';

  const K = {
    THEME: 'jdt_theme',
    TANK_H: 'jdt_tankH',
    OFFSET: 'jdt_offset',
    LOW: 'jdt_low',
    HIGH: 'jdt_high',
    USER: 'jdt_user',
    PASS: 'jdt_pass',
    LOGGED: 'jdt_logged'
  };

  const DEF = {
    theme: 'dark',
    tankH: 100,
    offset: 5,
    low: 20,
    high: 90,
    user: 'admin',
    pass: 'admin'
  };

  function ld(k, d) { const v = localStorage.getItem(k); return v === null ? d : v; }
  function ldn(k, d) { const n = parseFloat(localStorage.getItem(k)); return isNaN(n) ? d : n; }

  const cfg = {
    tankH: ldn(K.TANK_H, DEF.tankH),
    offset: ldn(K.OFFSET, DEF.offset),
    low: ldn(K.LOW, DEF.low),
    high: ldn(K.HIGH, DEF.high),
    user: ld(K.USER, DEF.user),
    pass: ld(K.PASS, DEF.pass)
  };

  const live = {
    connected: false,
    dist: -1,
    rssi: 0,
    pumpOn: false,
    mode: 'AUTO',
    err: false
  };

  let $, ws;

  document.addEventListener('DOMContentLoaded', () => {
    cacheDom();
    setupLogin();
    setupTheme();
    setupModal();
    setupPump();
    setupPwa();
    render();
    connectWS();
  });

  function cacheDom() {
    $ = {
      html: document.documentElement,
      loginScreen: document.getElementById('login-screen'),
      loginForm: document.getElementById('login-form'),
      loginUser: document.getElementById('login-user'),
      loginPass: document.getElementById('login-pass'),
      loginError: document.getElementById('login-error'),
      app: document.getElementById('app'),

      statusPill: document.getElementById('status-pill'),
      statusText: document.getElementById('status-text'),
      btnTheme: document.getElementById('btn-theme'),
      themeIcon: document.getElementById('theme-icon'),
      themeLabel: document.getElementById('theme-label'),
      btnSettings: document.getElementById('btn-settings'),
      btnLogout: document.getElementById('btn-logout'),

      water: document.getElementById('water'),
      tankPct: document.getElementById('tank-pct-text'),
      heroStatusBadge: document.getElementById('hero-status-badge'),
      lvlNum: document.getElementById('lvl-num'),
      lvlBar: document.getElementById('lvl-bar'),
      mDist: document.getElementById('m-dist'),
      mDepth: document.getElementById('m-depth'),
      mTank: document.getElementById('m-tank'),
      mRssi: document.getElementById('m-rssi'),
      rssiQ: document.getElementById('rssi-q'),

      pumpSub: document.getElementById('pump-sub'),
      tabAuto: document.getElementById('tab-auto'),
      tabManual: document.getElementById('tab-manual'),
      motorDetail: document.getElementById('motor-detail'),
      motorBadge: document.getElementById('motor-badge'),
      pumpSw: document.getElementById('pump-sw'),
      thLo: document.getElementById('th-lo'),
      thHi: document.getElementById('th-hi'),

      overlay: document.getElementById('overlay'),
      modalX: document.getElementById('modal-x'),
      btnCancel: document.getElementById('btn-cancel'),
      btnApply: document.getElementById('btn-apply'),
      sH: document.getElementById('s-height'),
      sO: document.getElementById('s-offset'),
      sLive: document.getElementById('s-live'),
      sLow: document.getElementById('s-low'),
      sLowV: document.getElementById('s-low-v'),
      sHigh: document.getElementById('s-high'),
      sHighV: document.getElementById('s-high-v'),
      sSsid: document.getElementById('s-ssid'),
      sPass: document.getElementById('s-pass'),
      sUser: document.getElementById('s-user'),
      sCredPass: document.getElementById('s-cred-pass'),
      sCredPass2: document.getElementById('s-cred-pass2'),
      btnHardReset: document.getElementById('btn-hard-reset'),
      wifiDatalist: document.getElementById('wifi-list-options'),
      wifiScanStatus: document.getElementById('wifi-scan-status'),
      btnInstallPwa: document.getElementById('btn-install-pwa')
    };
  }

  // ═══════════════════════
  //  PWA MOBILE APP INSTALL
  // ═══════════════════════
  let deferredPrompt;

  function setupPwa() {
    if ('serviceWorker' in navigator) {
      navigator.serviceWorker.register('/sw.js').catch(() => {});
    }

    window.addEventListener('beforeinstallprompt', (e) => {
      e.preventDefault();
      deferredPrompt = e;
      if ($.btnInstallPwa) {
        $.btnInstallPwa.classList.remove('hidden');
      }
    });

    if ($.btnInstallPwa) {
      $.btnInstallPwa.addEventListener('click', async () => {
        if (!deferredPrompt) return;
        deferredPrompt.prompt();
        const { outcome } = await deferredPrompt.userChoice;
        if (outcome === 'accepted') {
          $.btnInstallPwa.classList.add('hidden');
        }
        deferredPrompt = null;
      });
    }
  }

  // ═══════════════════════
  //  LOGIN / AUTH
  // ═══════════════════════
  function setupLogin() {
    // Check if already logged in this session
    const logged = sessionStorage.getItem(K.LOGGED);
    if (logged === 'true') {
      showApp();
    }

    $.loginForm.addEventListener('submit', async (e) => {
      e.preventDefault();
      const u = $.loginUser.value.trim();
      const p = $.loginPass.value.trim();

      try {
        const res = await fetch('/api/login', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ username: u, password: p })
        });
        const data = await res.json();
        if (res.ok && data.success) {
          sessionStorage.setItem(K.LOGGED, 'true');
          cfg.user = u;
          cfg.pass = p;
          localStorage.setItem(K.USER, u);
          localStorage.setItem(K.PASS, p);
          $.loginError.textContent = '';
          showApp();
          return;
        }
      } catch (_) {}

      // Fallback local authentication
      if (u && p && u.toLowerCase() === String(cfg.user).trim().toLowerCase() && p === String(cfg.pass).trim()) {
        sessionStorage.setItem(K.LOGGED, 'true');
        $.loginError.textContent = '';
        showApp();
      } else {
        $.loginError.textContent = 'Incorrect username or password.';
        $.loginPass.value = '';
        $.loginPass.focus();
      }
    });

    $.btnLogout.addEventListener('click', () => {
      sessionStorage.removeItem(K.LOGGED);
      hideApp();
    });
  }

  function showApp() {
    $.loginScreen.classList.add('hidden');
    $.app.classList.remove('hidden');
  }

  function hideApp() {
    $.app.classList.add('hidden');
    $.loginScreen.classList.remove('hidden');
    $.loginUser.value = '';
    $.loginPass.value = '';
    $.loginError.textContent = '';
    $.loginUser.focus();
  }

  // ═══════════════════════
  //  THEME
  // ═══════════════════════
  function setupTheme() {
    applyTheme(ld(K.THEME, DEF.theme));
    $.btnTheme.addEventListener('click', () => {
      const next = $.html.getAttribute('data-theme') === 'dark' ? 'light' : 'dark';
      applyTheme(next);
      localStorage.setItem(K.THEME, next);
    });
  }

  function applyTheme(t) {
    $.html.setAttribute('data-theme', t);
    if ($.themeIcon) $.themeIcon.setAttribute('data-lucide', t === 'dark' ? 'moon' : 'sun');
    if ($.themeLabel) $.themeLabel.textContent = t === 'dark' ? 'Dark' : 'Light';
    if (window.lucide) lucide.createIcons();
  }

  // ═══════════════════════
  //  SETTINGS MODAL
  // ═══════════════════════
  function setupModal() {
    $.btnSettings.addEventListener('click', openModal);
    $.modalX.addEventListener('click', closeModal);
    $.btnCancel.addEventListener('click', closeModal);
    $.overlay.addEventListener('click', (e) => { if (e.target === $.overlay) closeModal(); });
    $.sLow.addEventListener('input', () => { $.sLowV.textContent = $.sLow.value + '%'; });
    $.sHigh.addEventListener('input', () => { $.sHighV.textContent = $.sHigh.value + '%'; });
    $.btnApply.addEventListener('click', saveSettings);
    if ($.btnHardReset) $.btnHardReset.addEventListener('click', handleHardReset);
  }

  function handleHardReset() {
    if (!confirm('⚠️ Are you sure you want to HARD RESET the device?\n\nThis will erase saved WiFi credentials on the ESP32 and reboot it into AP Hotspot setup mode (JDT-Tank-Setup).')) {
      return;
    }
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({ type: 'hardreset' }));
      alert('🔄 Hard Reset command sent! The ESP32 is erasing WiFi settings and starting hotspot "JDT-Tank-Setup".');
      closeModal();
    } else {
      alert('❌ Device is currently offline or disconnected.');
    }
  }

  function openModal() {
    $.sH.value = cfg.tankH;
    $.sO.value = cfg.offset;
    $.sLow.value = cfg.low;
    $.sLowV.textContent = cfg.low + '%';
    $.sHigh.value = cfg.high;
    $.sHighV.textContent = cfg.high + '%';
    $.sUser.value = cfg.user;
    $.sCredPass.value = '';
    $.sCredPass2.value = '';
    $.sLive.textContent = (live.dist > 0 && !live.err) ? live.dist.toFixed(1) + ' cm' : '-- cm';

    // Request nearby WiFi scan from ESP32
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({ type: 'scanwifi' }));
      if ($.wifiScanStatus) $.wifiScanStatus.textContent = '⚡ Scanning nearby WiFi networks...';
    }

    $.overlay.classList.add('open');
  }

  function closeModal() { $.overlay.classList.remove('open'); }

  function saveSettings() {
    const h = parseFloat($.sH.value);
    const o = parseFloat($.sO.value);
    const lo = parseInt($.sLow.value);
    const hi = parseInt($.sHigh.value);

    if (!h || h < 10 || h > 500) return alert('Tank height: 10–500 cm.');
    if (isNaN(o) || o < 0 || o >= h) return alert('Offset must be 0 to less than tank height.');
    if (lo >= hi) return alert('Auto ON must be lower than Auto OFF.');

    cfg.tankH = h; cfg.offset = o; cfg.low = lo; cfg.high = hi;
    localStorage.setItem(K.TANK_H, h);
    localStorage.setItem(K.OFFSET, o);
    localStorage.setItem(K.LOW, lo);
    localStorage.setItem(K.HIGH, hi);

    // Credentials update
    const newUser = $.sUser ? $.sUser.value.trim() : '';
    const newPass = $.sCredPass ? $.sCredPass.value : '';
    const newPass2 = $.sCredPass2 ? $.sCredPass2.value : '';

    if (newPass) {
      if (newPass !== newPass2) return alert('Passwords do not match.');
    }

    if (newUser || newPass) {
      const payload = {};
      if (newUser) payload.newUsername = newUser;
      if (newPass) payload.newPassword = newPass;

      fetch('/api/credentials', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      })
      .then(r => r.json())
      .then(res => {
        if (res.success) {
          if (newUser) { cfg.user = newUser; localStorage.setItem(K.USER, newUser); }
          if (newPass) { cfg.pass = newPass; localStorage.setItem(K.PASS, newPass); }
          alert('🔐 Login credentials updated on server! You can now log in with the new credentials on all phones.');
        }
      })
      .catch(() => {});
    }

    // Send config to ESP32
    const newSsid = $.sSsid ? $.sSsid.value.trim() : '';
    const newWifiPass = $.sPass ? $.sPass.value.trim() : '';

    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({
        type: 'config',
        tankHeight: h, sensorOffset: o,
        autoLow: lo, autoHigh: hi
      }));

      if (newSsid) {
        ws.send(JSON.stringify({
          type: 'wificonfig',
          ssid: newSsid,
          pass: newWifiPass
        }));
        alert('WiFi credentials sent to device! The ESP32 is restarting to connect to "' + newSsid + '".');
        if ($.sSsid) $.sSsid.value = '';
        if ($.sPass) $.sPass.value = '';
      }
    } else if (newSsid) {
      alert('Device is currently offline. Connect the ESP32 to send new WiFi settings.');
    }

    render();
    closeModal();
  }

  // ═══════════════════════
  //  PUMP
  // ═══════════════════════
  function setupPump() {
    $.tabAuto.addEventListener('click', () => sendCtrl(live.pumpOn, 'AUTO'));
    $.tabManual.addEventListener('click', () => sendCtrl(live.pumpOn, 'MANUAL'));
    $.pumpSw.addEventListener('change', (e) => {
      if (live.mode !== 'MANUAL') return;
      sendCtrl(e.target.checked, 'MANUAL');
    });
  }

  function sendCtrl(on, mode) {
    live.pumpOn = on; live.mode = mode;
    render();
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({ type: 'control', pumpOn: on, mode: mode }));
    }
  }

  // ═══════════════════════
  //  WEBSOCKET
  // ═══════════════════════
  let reconTimer;

  function connectWS() {
    const proto = location.protocol === 'https:' ? 'wss:' : 'ws:';
    ws = new WebSocket(proto + '//' + location.host + '/ws');

    ws.onopen = () => {
      // Connected to cloud relay — server will immediately send device status
    };

    ws.onmessage = (ev) => {
      try {
        const d = JSON.parse(ev.data);

        // ── WiFi scan results from ESP32 ──────────────────────────────────
        if (d.type === 'wifiscan' && Array.isArray(d.networks)) {
          if ($.wifiDatalist) {
            $.wifiDatalist.innerHTML = '';
            d.networks.forEach(net => {
              if (!net.ssid) return;
              const opt = document.createElement('option');
              opt.value = net.ssid;
              opt.label = net.ssid + (net.rssi ? ' (' + net.rssi + ' dBm)' : '');
              $.wifiDatalist.appendChild(opt);
            });
          }
          if ($.wifiScanStatus) {
            $.wifiScanStatus.textContent = '✅ Found ' + d.networks.length + ' networks. Select from list or type manually.';
          }
          return;
        }

        // ── State message from server (deployed format) ──────────────────
        // Server sends: {type:"state", online:true, distanceCm, levelPercent, pumpOn, mode, rssi, lastSeen}
        if (d.type === 'state') {
          if (d.online === true) {
            live.connected = true;
            if (d.distanceCm    !== undefined) live.dist   = d.distanceCm;
            if (d.levelPercent  !== undefined) live.pct    = d.levelPercent;
            if (d.rssi          !== undefined) live.rssi   = d.rssi;
            if (d.pumpOn        !== undefined) live.pumpOn = d.pumpOn;
            if (d.mode          !== undefined) live.mode   = d.mode;
            live.err = false;
          } else {
            live.connected = false;
            live.err       = true;
          }
          render();
          return;
        }

        // ── Telemetry message (raw firmware format with deviceOnline flag) ─
        // Server sends: {type:"telemetry", deviceOnline:true, distanceCm, rssi, sensorError}
        if (d.type === 'telemetry' || d.distanceCm !== undefined || d.deviceOnline !== undefined) {
          const isOnline = d.deviceOnline === true || d.online === true;
          if (isOnline) {
            live.connected = true;
            if (d.distanceCm   !== undefined) live.dist   = d.distanceCm;
            if (d.sensorError  !== undefined) live.err    = d.sensorError;
            if (d.rssi         !== undefined) live.rssi   = d.rssi;
            if (d.pumpOn       !== undefined) live.pumpOn = d.pumpOn;
            if (d.mode         !== undefined) live.mode   = d.mode;
          } else {
            live.connected = false;
            live.err       = true;
          }
          render();
          return;
        }

      } catch (e) {}
    };

    ws.onclose = () => {
      live.connected = false;
      render();
      clearTimeout(reconTimer);
      reconTimer = setTimeout(connectWS, 2000);
    };

    ws.onerror = () => {
      live.connected = false;
      render();
    };
  }


  // ═══════════════════════
  //  RENDER
  // ═══════════════════════
  function render() {
    // Status
    if (live.connected) {
      $.statusPill.className = 'status-pill online';
      $.statusText.textContent = 'DEVICE ONLINE';
    } else {
      $.statusPill.className = 'status-pill offline';
      $.statusText.textContent = 'DEVICE OFFLINE';
    }

    // Calc
    const isErr = !live.connected || live.err || live.dist <= 0;
    const tankH = cfg.tankH;
    const off = cfg.offset;
    const usable = tankH - off;
    let dist = live.dist, dDisp = dist, depth = 0, pct = 0;

    if (!isErr) {
      // If server sends pre-calculated levelPercent, use it directly
      if (live.pct !== undefined && live.pct >= 0) {
        pct = Math.min(100, Math.max(0, live.pct));
        depth = (pct / 100) * usable;
        dDisp = Math.max(off, tankH - depth);
      } else if (usable > 0) {
        // Fallback: calculate from distanceCm
        if (dist < off) dDisp = off;
        depth = Math.max(0, tankH - dDisp);
        pct = Math.min(100, Math.max(0, (depth / usable) * 100));
      }
    }


    // Tank
    $.water.style.height = isErr ? '0%' : (pct.toFixed(1) + '%');
    $.tankPct.textContent = isErr ? '0%' : (pct.toFixed(1) + '%');

    // Hero Status Badge
    if ($.heroStatusBadge) {
      if (!live.connected) {
        $.heroStatusBadge.textContent = '🔌 Device Offline';
        $.heroStatusBadge.className = 'hero-status-badge warn';
      } else if (live.err) {
        $.heroStatusBadge.textContent = '⚠️ Checking Sensor...';
        $.heroStatusBadge.className = 'hero-status-badge warn';
      } else if (pct >= 90) {
        $.heroStatusBadge.textContent = '✅ Tank Full';
        $.heroStatusBadge.className = 'hero-status-badge ok';
      } else if (pct <= cfg.low) {
        $.heroStatusBadge.textContent = '⚠️ Level Low';
        $.heroStatusBadge.className = 'hero-status-badge warn';
      } else {
        $.heroStatusBadge.textContent = '💧 Level Normal';
        $.heroStatusBadge.className = 'hero-status-badge ok';
      }
    }

    // Hero
    $.lvlNum.textContent = isErr ? '--' : pct.toFixed(1);
    $.lvlBar.style.width = isErr ? '0%' : (pct.toFixed(1) + '%');

    // Metrics
    $.mDist.textContent = isErr ? '--' : dDisp.toFixed(1);
    $.mDepth.textContent = isErr ? '--' : depth.toFixed(1);
    $.mTank.textContent = tankH;
    $.mRssi.textContent = live.connected ? live.rssi : '--';

    if (live.connected) {
      const r = live.rssi;
      $.rssiQ.textContent = r >= -55 ? 'Strong Signal' : r >= -70 ? 'Good Signal' : 'Weak Signal';
    } else {
      $.rssiQ.textContent = 'Device Offline';
    }

    // Live dist in modal
    if ($.overlay.classList.contains('open')) {
      $.sLive.textContent = isErr ? '-- cm' : dDisp.toFixed(1) + ' cm';
    }

    // Thresholds
    $.thLo.textContent = cfg.low + '%';
    $.thHi.textContent = cfg.high + '%';

    // Mode
    if (live.mode === 'AUTO') {
      $.tabAuto.classList.add('active'); $.tabManual.classList.remove('active');
      $.pumpSub.textContent = 'Automatic Mode (Smart Control)'; $.pumpSw.disabled = true;
    } else {
      $.tabAuto.classList.remove('active'); $.tabManual.classList.add('active');
      $.pumpSub.textContent = 'Manual Mode (User Switch Override)'; $.pumpSw.disabled = false;
    }

    // Pump
    $.pumpSw.checked = live.pumpOn;
    if (live.pumpOn) {
      $.motorBadge.className = 'motor-badge on'; $.motorBadge.textContent = 'ON';
      $.motorDetail.textContent = 'Water pump is RUNNING';
    } else {
      $.motorBadge.className = 'motor-badge off'; $.motorBadge.textContent = 'OFF';
      $.motorDetail.textContent = 'Water pump is turned OFF';
    }
  }

})();
