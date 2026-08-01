#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart Water Tank Monitor & Controller</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;700&family=Outfit:wght@400;500;600;700;800&display=swap" rel="stylesheet">
  <script src="https://unpkg.com/lucide@latest"></script>
  <style>
    :root {
      --bg-primary: #090d16;
      --bg-card: rgba(17, 24, 39, 0.75);
      --bg-card-hover: rgba(31, 41, 55, 0.85);
      --bg-input: rgba(15, 23, 42, 0.8);
      --border-color: rgba(255, 255, 255, 0.08);
      --border-glow: rgba(6, 182, 212, 0.3);
      --text-primary: #f3f4f6;
      --text-secondary: #9ca3af;
      --text-muted: #6b7280;
      --accent-cyan: #06b6d4;
      --accent-blue: #3b82f6;
      --accent-teal: #14b8a6;
      --accent-green: #10b981;
      --accent-amber: #f59e0b;
      --accent-red: #ef4444;
      --gradient-tank: linear-gradient(180deg, rgba(6, 182, 212, 0.85) 0%, rgba(59, 130, 246, 0.9) 100%);
      --radius-lg: 18px;
      --radius-md: 12px;
      --radius-sm: 8px;
      --font-main: 'Outfit', sans-serif;
      --font-mono: 'JetBrains Mono', monospace;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: var(--font-main);
      background-color: var(--bg-primary);
      background-image: radial-gradient(circle at 20% 10%, rgba(6, 182, 212, 0.08) 0%, transparent 40%), radial-gradient(circle at 80% 90%, rgba(59, 130, 246, 0.08) 0%, transparent 40%);
      color: var(--text-primary);
      min-height: 100vh;
      padding: 20px;
      display: flex;
      justify-content: center;
      align-items: flex-start;
    }
    .app-wrapper { width: 100%; max-width: 1100px; margin: 0 auto; }
    header {
      display: flex; justify-content: space-between; align-items: center;
      background: var(--bg-card); backdrop-filter: blur(16px);
      border: 1px solid var(--border-color); padding: 16px 24px;
      border-radius: var(--radius-lg); margin-bottom: 24px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4);
    }
    .brand { display: flex; align-items: center; gap: 14px; }
    .brand-icon {
      width: 44px; height: 44px; border-radius: 12px;
      background: linear-gradient(135deg, rgba(6, 182, 212, 0.2), rgba(59, 130, 246, 0.2));
      border: 1px solid var(--border-glow); display: flex; align-items: center; justify-content: center;
      color: var(--accent-cyan); box-shadow: 0 0 15px rgba(6, 182, 212, 0.3);
    }
    .brand h1 {
      font-size: 1.35rem; font-weight: 700; letter-spacing: -0.02em;
      background: linear-gradient(90deg, #ffffff, #a5f3fc);
      -webkit-background-clip: text; -webkit-text-fill-color: transparent;
    }
    .brand p { font-size: 0.8rem; color: var(--text-muted); }
    .header-actions { display: flex; align-items: center; gap: 12px; }
    .status-pill {
      display: inline-flex; align-items: center; gap: 8px; padding: 8px 14px;
      border-radius: 20px; font-size: 0.82rem; font-weight: 600;
      border: 1px solid var(--border-color); background: rgba(15, 23, 42, 0.6);
    }
    .status-dot { width: 8px; height: 8px; border-radius: 50%; background: var(--text-muted); }
    .status-pill.online .status-dot { background: var(--accent-green); box-shadow: 0 0 10px var(--accent-green); }
    .status-pill.ap-mode .status-dot { background: var(--accent-amber); box-shadow: 0 0 10px var(--accent-amber); }
    .status-pill.offline .status-dot { background: var(--accent-red); }
    .btn-icon {
      width: 42px; height: 42px; border-radius: var(--radius-md);
      border: 1px solid var(--border-color); background: rgba(30, 41, 59, 0.5);
      color: var(--text-primary); display: flex; align-items: center; justify-content: center;
      cursor: pointer; transition: all 0.2s ease;
    }
    .btn-icon:hover { background: rgba(6, 182, 212, 0.15); border-color: var(--accent-cyan); color: var(--accent-cyan); }
    .dashboard-grid { display: grid; grid-template-columns: 360px 1fr; gap: 24px; }
    @media (max-width: 900px) { .dashboard-grid { grid-template-columns: 1fr; } }
    .tank-card {
      background: var(--bg-card); backdrop-filter: blur(16px);
      border: 1px solid var(--border-color); border-radius: var(--radius-lg);
      padding: 24px; display: flex; flex-direction: column; align-items: center;
      position: relative; box-shadow: 0 12px 40px rgba(0, 0, 0, 0.5);
    }
    .tank-container-wrapper { position: relative; width: 200px; height: 320px; margin: 20px 0; }
    .sensor-housing {
      position: absolute; top: -22px; left: 50%; transform: translateX(-50%);
      width: 110px; height: 28px; background: #1e293b; border: 2px solid #334155;
      border-radius: 6px; display: flex; justify-content: space-around; align-items: center; z-index: 10;
    }
    .sensor-eye { width: 18px; height: 18px; border-radius: 50%; background: radial-gradient(circle, #475569 40%, #0f172a 100%); border: 2px solid #64748b; }
    .sensor-beam {
      position: absolute; top: 6px; left: 50%; transform: translateX(-50%);
      width: 80px; height: 0; background: linear-gradient(180deg, rgba(6, 182, 212, 0.3) 0%, rgba(6, 182, 212, 0) 100%);
      border-left: 1px dashed rgba(6, 182, 212, 0.4); border-right: 1px dashed rgba(6, 182, 212, 0.4);
      z-index: 2; transition: height 0.4s ease;
    }
    .tank-glass {
      position: absolute; bottom: 0; width: 100%; height: 290px;
      border-radius: 24px; border: 3px solid rgba(255, 255, 255, 0.15);
      background: rgba(15, 23, 42, 0.85); overflow: hidden; position: relative;
    }
    .water-fill {
      position: absolute; bottom: 0; left: 0; width: 100%; height: 0%;
      background: var(--gradient-tank); transition: height 0.8s cubic-bezier(0.4, 0, 0.2, 1);
    }
    .wave {
      position: absolute; top: -18px; left: 0; width: 200%; height: 24px;
      background: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 1200 120' preserveAspectRatio='none'%3E%3Cpath d='M0,0 C150,90 350,-40 500,40 C650,120 900,10 1200,40 L1200,120 L0,120 Z' fill='%2306b6d4' opacity='0.8'%3E%3C/path%3E%3C/svg%3E") repeat-x;
      animation: wave-anim 6s linear infinite;
    }
    @keyframes wave-anim { 0% { transform: translateX(0); } 100% { transform: translateX(-50%); } }
    .level-ticks {
      position: absolute; right: 8px; top: 15px; bottom: 15px;
      display: flex; flex-direction: column; justify-content: space-between;
      font-family: var(--font-mono); font-size: 0.68rem; color: var(--text-muted); z-index: 6;
    }
    .level-display { text-align: center; margin-top: 10px; }
    .level-percentage {
      font-size: 3.2rem; font-weight: 800; letter-spacing: -0.03em;
      background: linear-gradient(180deg, #ffffff 30%, var(--accent-cyan) 100%);
      -webkit-background-clip: text; -webkit-text-fill-color: transparent; line-height: 1;
    }
    .level-sub { font-size: 0.85rem; color: var(--text-secondary); margin-top: 6px; }
    .controls-column { display: flex; flex-direction: column; gap: 20px; }
    .metrics-row { display: grid; grid-template-columns: repeat(auto-fit, minmax(160px, 1fr)); gap: 16px; }
    .metric-card {
      background: var(--bg-card); backdrop-filter: blur(16px);
      border: 1px solid var(--border-color); border-radius: var(--radius-md);
      padding: 18px; display: flex; flex-direction: column; gap: 8px;
    }
    .metric-header { display: flex; align-items: center; gap: 8px; font-size: 0.8rem; color: var(--text-secondary); }
    .metric-icon { color: var(--accent-cyan); }
    .metric-value { font-size: 1.5rem; font-weight: 700; font-family: var(--font-mono); color: #ffffff; }
    .metric-value small { font-size: 0.85rem; color: var(--text-muted); font-weight: 400; margin-left: 2px; }
    .pump-card {
      background: var(--bg-card); backdrop-filter: blur(16px);
      border: 1px solid var(--border-color); border-radius: var(--radius-lg);
      padding: 24px; display: flex; flex-direction: column; gap: 20px;
    }
    .pump-card-header { display: flex; justify-space-between; align-items: center; }
    .pump-title-wrap h2 { font-size: 1.15rem; font-weight: 700; }
    .pump-title-wrap p { font-size: 0.8rem; color: var(--text-muted); }
    .mode-toggle { display: flex; background: rgba(15, 23, 42, 0.8); border: 1px solid var(--border-color); border-radius: 30px; padding: 4px; }
    .mode-btn { border: none; background: transparent; color: var(--text-muted); padding: 6px 16px; border-radius: 20px; font-size: 0.8rem; font-weight: 600; cursor: pointer; }
    .mode-btn.active { background: var(--accent-cyan); color: #000000; }
    .pump-state-box { display: flex; align-items: center; justify-content: space-between; background: rgba(15, 23, 42, 0.6); border: 1px solid var(--border-color); border-radius: var(--radius-md); padding: 16px 20px; }
    .pump-info-status { display: flex; align-items: center; gap: 12px; }
    .pump-indicator-light { width: 14px; height: 14px; border-radius: 50%; background: #334155; }
    .pump-indicator-light.active { background: var(--accent-green); box-shadow: 0 0 16px var(--accent-green); }
    .btn-pump-power { padding: 12px 24px; border-radius: 30px; border: 1px solid var(--border-color); background: linear-gradient(135deg, #1e293b, #0f172a); color: var(--text-primary); font-size: 0.9rem; font-weight: 700; display: flex; align-items: center; gap: 8px; cursor: pointer; }
    .btn-pump-power.on { background: linear-gradient(135deg, #059669, #10b981); color: #ffffff; border-color: #34d399; }
    .safety-bar { display: flex; gap: 12px; background: rgba(15, 23, 42, 0.4); border-radius: var(--radius-sm); padding: 12px 16px; border: 1px solid var(--border-color); font-size: 0.8rem; color: var(--text-secondary); align-items: center; }
    .modal-backdrop { position: fixed; top: 0; left: 0; width: 100vw; height: 100vh; background: rgba(0, 0, 0, 0.75); backdrop-filter: blur(12px); display: flex; align-items: center; justify-content: center; z-index: 100; opacity: 0; pointer-events: none; transition: opacity 0.3s ease; }
    .modal-backdrop.open { opacity: 1; pointer-events: auto; }
    .modal-card { background: #0f172a; border: 1px solid var(--border-color); border-radius: var(--radius-lg); width: 90%; max-width: 540px; padding: 24px; }
    .modal-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 20px; }
    .btn-close { background: transparent; border: none; color: var(--text-muted); font-size: 1.4rem; cursor: pointer; }
    .form-group { display: flex; flex-direction: column; gap: 8px; margin-bottom: 16px; }
    .form-group label { font-size: 0.85rem; font-weight: 600; }
    .form-group p { font-size: 0.76rem; color: var(--text-muted); }
    .input-with-unit { display: flex; align-items: center; background: var(--bg-input); border: 1px solid var(--border-color); border-radius: var(--radius-sm); padding: 0 14px; }
    .input-with-unit input { flex: 1; background: transparent; border: none; outline: none; color: #ffffff; padding: 10px 0; font-family: var(--font-mono); }
    .slider-group { display: flex; align-items: center; gap: 16px; }
    .slider-group input[type="range"] { flex: 1; accent-color: var(--accent-cyan); }
    .slider-val { font-family: var(--font-mono); font-weight: 700; color: var(--accent-cyan); }
    .btn-primary { background: linear-gradient(135deg, var(--accent-cyan), var(--accent-blue)); border: none; color: #000000; font-weight: 700; padding: 10px 20px; border-radius: var(--radius-sm); cursor: pointer; width: 100%; margin-top: 10px; }
  </style>
</head>
<body>
  <div class="app-wrapper">
    <header>
      <div class="brand">
        <div class="brand-icon"><i data-lucide="waves"></i></div>
        <div>
          <h1>AquaSense Pro</h1>
          <p>ESP32 Smart Water Tank System</p>
        </div>
      </div>
      <div class="header-actions">
        <div id="connection-status" class="status-pill offline">
          <span class="status-dot"></span>
          <span id="connection-label">DISCONNECTED</span>
        </div>
        <button class="btn-icon" id="btn-open-settings" title="Settings"><i data-lucide="settings"></i></button>
      </div>
    </header>

    <main class="dashboard-grid">
      <section class="tank-card">
        <div class="sensor-housing"><div class="sensor-eye"></div><div class="sensor-eye"></div></div>
        <div class="tank-container-wrapper">
          <div class="sensor-beam" id="sensor-beam"></div>
          <div class="tank-glass">
            <div class="water-fill" id="water-fill"><div class="wave"></div></div>
            <div class="level-ticks"><div>100%</div><div>75%</div><div>50%</div><div>25%</div><div>0%</div></div>
          </div>
        </div>
        <div class="level-display">
          <div class="level-percentage" id="val-level-pct">0%</div>
          <div class="level-sub" id="val-water-status">Calculating Water Level...</div>
        </div>
      </section>

      <section class="controls-column">
        <div class="metrics-row">
          <div class="metric-card"><div class="metric-header"><i data-lucide="ruler" class="metric-icon"></i><span>Water Depth</span></div><div class="metric-value" id="val-water-cm">0 <small>cm</small></div></div>
          <div class="metric-card"><div class="metric-header"><i data-lucide="arrow-up-down" class="metric-icon"></i><span>Air Gap</span></div><div class="metric-value" id="val-distance-cm">0 <small>cm</small></div></div>
          <div class="metric-card"><div class="metric-header"><i data-lucide="droplet" class="metric-icon"></i><span>Volume</span></div><div class="metric-value" id="val-volume-liters">0 <small>L</small></div></div>
          <div class="metric-card"><div class="metric-header"><i data-lucide="wifi" class="metric-icon"></i><span>WiFi Signal</span></div><div class="metric-value" id="val-wifi-rssi">-- <small>dBm</small></div></div>
        </div>

        <div class="pump-card">
          <div class="pump-card-header">
            <div class="pump-title-wrap">
              <h2>Motor Pump Controller</h2>
              <p id="pump-mode-desc">AUTOMATIC Mode</p>
            </div>
            <div class="mode-toggle">
              <button class="mode-btn active" id="btn-mode-auto">AUTO</button>
              <button class="mode-btn" id="btn-mode-manual">MANUAL</button>
            </div>
          </div>

          <div class="pump-state-box">
            <div class="pump-info-status">
              <div class="pump-indicator-light" id="pump-indicator"></div>
              <div class="pump-status-label">
                <h3 id="lbl-pump-state">PUMP OFF</h3>
                <p id="lbl-pump-sub">Relay Pin: LOW</p>
              </div>
            </div>
            <button class="btn-pump-power" id="btn-toggle-pump" disabled>
              <i data-lucide="power"></i><span id="lbl-btn-pump">START PUMP</span>
            </button>
          </div>

          <div class="safety-bar">
            <i data-lucide="shield-check"></i><span>Dry-Run Protection Active</span>
          </div>
        </div>
      </section>
    </main>

    <div class="modal-backdrop" id="settings-modal">
      <div class="modal-card">
        <div class="modal-header">
          <h2><i data-lucide="sliders"></i> Calibration & Settings</h2>
          <button class="btn-close" id="btn-close-settings">&times;</button>
        </div>
        <div class="modal-body">
          <div class="form-group">
            <label>Total Tank Height (cm)</label>
            <div class="input-with-unit"><input type="number" id="input-tank-depth" value="100"><span class="input-unit">cm</span></div>
          </div>
          <div class="form-group">
            <label>Sensor Offset (cm)</label>
            <div class="input-with-unit"><input type="number" id="input-sensor-offset" value="5"><span class="input-unit">cm</span></div>
          </div>
          <div class="form-group">
            <label>Auto-ON Below (%)</label>
            <div class="slider-group"><input type="range" id="input-auto-min" min="5" max="50" value="20"><span class="slider-val" id="val-auto-min">20%</span></div>
          </div>
          <div class="form-group">
            <label>Auto-OFF Above (%)</label>
            <div class="slider-group"><input type="range" id="input-auto-max" min="50" max="100" value="95"><span class="slider-val" id="val-auto-max">95%</span></div>
          </div>
          <div class="form-group">
            <label>Router WiFi Setup (SSID / Password)</label>
            <div class="input-with-unit" style="margin-bottom: 8px;"><input type="text" id="input-wifi-ssid" placeholder="SSID"></div>
            <div class="input-with-unit"><input type="password" id="input-wifi-pass" placeholder="Password"></div>
          </div>
          <button class="btn-primary" id="btn-save-settings">Save & Restart</button>
        </div>
      </div>
    </div>
  </div>

  <script>
    document.addEventListener('DOMContentLoaded', () => {
      let state = { connected: false, apMode: true, waterLevelPct: 0, distanceCm: 0, waterDepthCm: 0, tankDepthCm: 100, sensorOffsetCm: 5, volumeLiters: 0, pumpState: false, pumpMode: 'AUTO', rssi: -55, autoMinPct: 20, autoMaxPct: 95 };
      const el = id => document.getElementById(id);

      el('input-auto-min').oninput = e => el('val-auto-min').textContent = e.target.value + '%';
      el('input-auto-max').oninput = e => el('val-auto-max').textContent = e.target.value + '%';

      el('btn-open-settings').onclick = () => {
        el('input-tank-depth').value = state.tankDepthCm;
        el('input-sensor-offset').value = state.sensorOffsetCm;
        el('input-auto-min').value = state.autoMinPct;
        el('val-auto-min').textContent = state.autoMinPct + '%';
        el('input-auto-max').value = state.autoMaxPct;
        el('val-auto-max').textContent = state.autoMaxPct + '%';
        el('settings-modal').classList.add('open');
      };
      el('btn-close-settings').onclick = () => el('settings-modal').classList.remove('open');

      el('btn-mode-auto').onclick = () => sendPump(state.pumpState, 'AUTO');
      el('btn-mode-manual').onclick = () => sendPump(state.pumpState, 'MANUAL');
      el('btn-toggle-pump').onclick = () => sendPump(!state.pumpState, 'MANUAL');

      el('btn-save-settings').onclick = async () => {
        const payload = {
          tankDepthCm: parseFloat(el('input-tank-depth').value)||100,
          sensorOffsetCm: parseFloat(el('input-sensor-offset').value)||5,
          autoMinPct: parseInt(el('input-auto-min').value)||20,
          autoMaxPct: parseInt(el('input-auto-max').value)||95,
          ssid: el('input-wifi-ssid').value.trim(),
          pass: el('input-wifi-pass').value.trim()
        };
        try {
          await fetch('/api/settings', { method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify(payload) });
          alert('Settings saved!');
          el('settings-modal').classList.remove('open');
        } catch(e) { alert('Saved locally'); }
      };

      async function sendPump(pumpStateVal, modeVal) {
        state.pumpState = pumpStateVal;
        state.pumpMode = modeVal;
        updateUI();
        try {
          await fetch('/api/pump', { method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify({state:pumpStateVal, mode:modeVal}) });
        } catch(e){}
      }

      async function fetchTelemetry() {
        try {
          const res = await fetch('/api/telemetry');
          if (res.ok) {
            const d = await res.json();
            Object.assign(state, d, { connected: true });
          }
        } catch(e) { state.connected = false; }
        updateUI();
      }

      function updateUI() {
        el('connection-status').className = 'status-pill ' + (state.connected ? (state.apMode ? 'ap-mode':'online') : 'ap-mode');
        el('connection-label').textContent = state.connected ? (state.apMode ? 'ESP32 AP MODE (192.168.4.1)' : 'ESP32 ONLINE (STA)') : 'LOCAL AP MODE';
        
        const pct = Math.min(100, Math.max(0, Math.round(state.waterLevelPct)));
        el('val-level-pct').textContent = pct + '%';
        el('water-fill').style.height = pct + '%';
        el('sensor-beam').style.height = (100 - pct) + '%';
        
        el('val-water-status').textContent = pct < 20 ? '⚠️ Water Level Low' : (pct > 90 ? '✅ Tank Full' : '💧 Water Normal');
        el('val-water-cm').innerHTML = (state.waterDepthCm || Math.round((pct/100)*state.tankDepthCm)) + ' <small>cm</small>';
        el('val-distance-cm').innerHTML = (state.distanceCm || (state.tankDepthCm - Math.round((pct/100)*state.tankDepthCm))) + ' <small>cm</small>';
        el('val-volume-liters').innerHTML = (state.volumeLiters || Math.round(pct * 10)) + ' <small>L</small>';
        el('val-wifi-rssi').innerHTML = state.rssi + ' <small>dBm</small>';

        if (state.pumpMode === 'AUTO') {
          el('btn-mode-auto').classList.add('active');
          el('btn-mode-manual').classList.remove('active');
          el('pump-mode-desc').textContent = `AUTOMATIC Mode (ON ≤${state.autoMinPct}%, OFF ≥${state.autoMaxPct}%)`;
          el('btn-toggle-pump').disabled = true;
        } else {
          el('btn-mode-auto').classList.remove('active');
          el('btn-mode-manual').classList.add('active');
          el('pump-mode-desc').textContent = 'MANUAL OVERRIDE Mode';
          el('btn-toggle-pump').disabled = false;
        }

        if (state.pumpState) {
          el('pump-indicator').classList.add('active');
          el('lbl-pump-state').textContent = 'PUMP RUNNING';
          el('lbl-pump-sub').textContent = 'Relay Status: ACTIVE (HIGH)';
          el('btn-toggle-pump').classList.add('on');
          el('lbl-btn-pump').textContent = 'STOP PUMP';
        } else {
          el('pump-indicator').classList.remove('active');
          el('lbl-pump-state').textContent = 'PUMP OFF';
          el('lbl-pump-sub').textContent = 'Relay Status: IDLE (LOW)';
          el('btn-toggle-pump').classList.remove('on');
          el('lbl-btn-pump').textContent = 'START PUMP';
        }
      }

      fetchTelemetry();
      setInterval(fetchTelemetry, 1000);
      if (window.lucide) lucide.createIcons();
    });
  </script>
</body>
</html>
)rawliteral";

#endif
