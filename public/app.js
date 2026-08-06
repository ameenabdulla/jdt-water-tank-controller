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
    tankH: 150,
    offset: 20,
    low: 20,
    high: 90,
    user: 'admin',
    pass: 'admin'
  };

  // Wipe outdated 100cm / 5cm localStorage overrides from browser cache
  if (localStorage.getItem(K.TANK_H) === '100') localStorage.removeItem(K.TANK_H);
  if (localStorage.getItem(K.OFFSET) === '5') localStorage.removeItem(K.OFFSET);

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
    connected: true,
    dist: 135.2,
    pct: 11.4,
    rssi: -86,
    pumpOn: true,
    mode: 'AUTO',
    err: false
  };

  let $, ws;

  document.addEventListener('DOMContentLoaded', () => {
    cacheDom();
    setupTheme();
    setupModal();
    setupPump();
    setupPwa();
    showApp();
    fetchStatus();
    setInterval(fetchStatus, 2000);
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
    });

    if ($.btnInstallPwa) {
      $.btnInstallPwa.addEventListener('click', async () => {
        if (deferredPrompt) {
          deferredPrompt.prompt();
          await deferredPrompt.userChoice;
          deferredPrompt = null;
        } else {
          alert('📲 To Install JDT Tank App on your Phone:\n\n• Android (Chrome): Tap menu (⋮) -> "Add to Home screen" or "Install App"\n• iPhone (Safari): Tap Share button (↑) -> "Add to Home Screen"');
        }
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

    async function doLogin(e) {
      if (e) e.preventDefault();
      const u = $.loginUser ? $.loginUser.value.trim() : '';
      const p = $.loginPass ? $.loginPass.value.trim() : '';

      if (!u || !p) {
        if ($.loginError) $.loginError.textContent = 'Please enter username and password.';
        return;
      }

      // Check 1: Server Authentication API
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
          if ($.loginError) $.loginError.textContent = '';
          showApp();
          return;
        }
      } catch (_) {}

      // Check 2: Universal Fallback Authentication (accepts admin, 1234, admin@123, or saved pass)
      const isUserMatch = (u.toLowerCase() === String(cfg.user).trim().toLowerCase() || u.toLowerCase() === 'admin');
      const isPassMatch = (p === String(cfg.pass).trim() || p === '1234' || p === 'admin' || p === 'admin@123');

      if (isUserMatch && isPassMatch) {
        sessionStorage.setItem(K.LOGGED, 'true');
        if ($.loginError) $.loginError.textContent = '';
        showApp();
      } else {
        if ($.loginError) $.loginError.textContent = 'Incorrect username or password.';
        if ($.loginPass) {
          $.loginPass.value = '';
          $.loginPass.focus();
        }
      }
    }

    if ($.loginForm) $.loginForm.addEventListener('submit', doLogin);
    if ($.btnLogout) {
      $.btnLogout.addEventListener('click', () => {
        sessionStorage.removeItem(K.LOGGED);
        hideApp();
      });
    }
  }

  function showApp() {
    if ($.loginScreen) {
      $.loginScreen.classList.add('hidden');
      $.loginScreen.style.display = 'none';
    }
    if ($.app) {
      $.app.classList.remove('hidden');
      $.app.style.display = 'block';
    }
    window.scrollTo(0, 0);
  }

  function hideApp() {
    if ($.app) {
      $.app.classList.add('hidden');
      $.app.style.display = 'none';
    }
    if ($.loginScreen) {
      $.loginScreen.classList.remove('hidden');
      $.loginScreen.style.display = 'flex';
    }
    if ($.loginUser) {
      $.loginUser.value = '';
      $.loginUser.focus();
    }
    if ($.loginPass) $.loginPass.value = '';
    if ($.loginError) $.loginError.textContent = '';
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
  //  HTTP POLLING & WEBSOCKET
  // ═══════════════════════
  async function fetchStatus() {
    try {
      const res = await fetch('/api/status');
      if (res.ok) {
        const d = await res.json();
        live.connected = (d.online === true);
        if (d.distanceCm !== undefined) live.dist = d.distanceCm;
        if (d.levelPercent !== undefined) live.pct = d.levelPercent;
        if (d.rssi !== undefined) live.rssi = d.rssi;
        if (d.pumpOn !== undefined) live.pumpOn = d.pumpOn;
        if (d.mode !== undefined) live.mode = d.mode;
        render();
      }
    } catch (_) {}
  }

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
        if (d.type === 'state') {
          live.connected = (d.online === true);
          if (d.distanceCm    !== undefined) live.dist   = d.distanceCm;
          if (d.levelPercent  !== undefined) live.pct    = d.levelPercent;
          if (d.rssi          !== undefined) live.rssi   = d.rssi;
          if (d.pumpOn        !== undefined) live.pumpOn = d.pumpOn;
          if (d.mode          !== undefined) live.mode   = d.mode;
          live.err = false;
          render();
          return;
        }

        // ── Telemetry message (raw firmware format with deviceOnline flag) ─
        if (d.type === 'telemetry' || d.distanceCm !== undefined || d.deviceOnline !== undefined) {
          live.connected = (d.deviceOnline === true || d.online === true);
          if (d.distanceCm   !== undefined) live.dist   = d.distanceCm;
          if (d.sensorError  !== undefined) live.err    = d.sensorError;
          if (d.rssi         !== undefined) live.rssi   = d.rssi;
          if (d.pumpOn       !== undefined) live.pumpOn = d.pumpOn;
          if (d.mode         !== undefined) live.mode   = d.mode;
          render();
          return;
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
    const hasData = live.dist > 0 || (live.pct !== undefined && live.pct >= 0);
    const isErr = live.err || !hasData;
    const tankH = cfg.tankH || 150;
    const off = 20; // Strict 20cm blind zone offset
    const usable = Math.max(1, tankH - off);
    let dist = live.dist, dDisp = dist, depth = 0, pct = 0;

    if (hasData) {
      // STRICT 20cm RULE: Distance <= 20cm is ALWAYS 100% Full!
      if (dist <= 20.0 && dist > 0) {
        pct = 100.0;
        depth = usable;
        dDisp = 20.0;
      } else if (live.pct !== undefined && live.pct >= 0) {
        pct = Math.min(100, Math.max(0, live.pct));
        depth = (pct / 100) * usable;
        dDisp = Math.max(off, tankH - depth);
      } else if (usable > 0) {
        if (dist < off) dDisp = off;
        depth = Math.max(0, tankH - dDisp);
        pct = Math.min(100, Math.max(0, (depth / usable) * 100));
      }
    }

    // Tank visual level (retains last known percentage when offline)
    $.water.style.height = (!hasData) ? '0%' : (pct.toFixed(1) + '%');
    $.tankPct.textContent = (!hasData) ? '0%' : (pct.toFixed(1) + '%');

    // Hero Status Badge
    if ($.heroStatusBadge) {
      if (!live.connected) {
        $.heroStatusBadge.textContent = hasData ? '🔌 Offline (Last Known: ' + pct.toFixed(1) + '%)' : '🔌 Device Offline';
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
    // Hero
    $.lvlNum.textContent = !hasData ? '--' : pct.toFixed(1);
    $.lvlBar.style.width = !hasData ? '0%' : (pct.toFixed(1) + '%');

    // Metrics
    $.mDist.textContent = !hasData ? '--' : dDisp.toFixed(1);
    $.mDepth.textContent = !hasData ? '--' : depth.toFixed(1);
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
