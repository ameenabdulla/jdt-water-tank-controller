#ifndef WEB_ASSETS_H
#define WEB_ASSETS_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta name="description" content="NEXUS HOME — Premium IoT Smart Home Control Center. Monitor and control lights, gate, water tank, pump, and safety sensors from one dashboard.">
  <title>NEXUS HOME — Smart Home Control Center</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;500;700&family=Orbitron:wght@400;500;600;700;800;900&family=Outfit:wght@300;400;500;600;700;800&display=swap" rel="stylesheet">
  <link rel="stylesheet" href="style.css">
</head>
<body>

  <!-- ═══════════════════════════════════════════════ -->
  <!-- BOOT / INTRO SCREEN                            -->
  <!-- ═══════════════════════════════════════════════ -->
  <div id="boot-screen" class="boot-screen">
    <div class="boot-content">
      <div class="boot-logo">
        <svg viewBox="0 0 100 100" width="80" height="80" class="boot-hex">
          <polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5"
                   fill="none" stroke-width="2" class="hex-outer"/>
          <polygon points="50,20 80,37.5 80,62.5 50,80 20,62.5 20,37.5"
                   fill="none" stroke-width="1.5" class="hex-inner"/>
          <circle cx="50" cy="50" r="8" class="hex-core"/>
        </svg>
      </div>
      <h1 class="boot-title" id="boot-title">SMART HOME</h1>
      <p class="boot-subtitle">Control Center v3.0</p>
      <div class="boot-progress">
        <div id="boot-progress-bar" class="boot-progress-bar"></div>
      </div>
      <div id="boot-status" class="boot-status"></div>
    </div>
  </div>

  <!-- ═══════════════════════════════════════════════ -->
  <!-- MAIN DASHBOARD                                  -->
  <!-- ═══════════════════════════════════════════════ -->
  <div id="dashboard" class="dashboard" style="display:none; opacity:0;">

    <!-- ─── STICKY NAVIGATION ─── -->
    <nav id="main-nav" class="main-nav">
      <div class="nav-inner">
        <a href="#" class="nav-brand">
          <svg viewBox="0 0 100 100" width="28" height="28">
            <polygon points="50,10 90,32.5 90,67.5 50,90 10,67.5 10,32.5"
                     fill="none" stroke="var(--accent-cyan)" stroke-width="4"/>
            <circle cx="50" cy="50" r="7" fill="var(--accent-cyan)"/>
          </svg>
          <span id="nav-brand-title">SMART HOME</span>
        </a>

        <div class="nav-center">
          <div class="nav-status">
            <span id="system-status-dot" class="status-dot online"></span>
            <span id="system-status-text" class="status-text">ONLINE</span>
          </div>
          <span id="nav-clock" class="nav-clock">--:--:--</span>
        </div>

        <div class="nav-links" id="nav-links">
          <a href="#lights" class="nav-link" data-section="lights">
            <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 18h6M10 22h4M12 2a7 7 0 0 1 7 7c0 2.38-1.19 4.47-3 5.74V17a1 1 0 0 1-1 1h-6a1 1 0 0 1-1-1v-2.26C6.19 13.47 5 11.38 5 9a7 7 0 0 1 7-7z"/></svg>
            <span class="nav-link-text">Lights</span>
            <span class="nav-badge" id="nav-light-indicator">0/5</span>
          </a>
          <a href="#gate" class="nav-link" data-section="gate">
            <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2"/><path d="M12 3v18M3 12h18"/></svg>
            <span class="nav-link-text">Gate</span>
            <span class="nav-badge" id="nav-gate-indicator">CLOSED</span>
          </a>
          <a href="#servo" class="nav-link" data-section="servo">
            <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="9"/><line x1="12" y1="12" x2="16.5" y2="7.5"/></svg>
            <span class="nav-link-text">Servo</span>
            <span class="nav-badge" id="nav-servo-indicator">0°</span>
          </a>
          <a href="#water" class="nav-link" data-section="water">
            <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 22a7 7 0 0 0 7-7c0-2-1-3.9-3-5.5s-3.5-4-4-6.5c-.5 2.5-2 4.9-4 6.5C6 11.1 5 13 5 15a7 7 0 0 0 7 7z"/></svg>
            <span class="nav-link-text">Water</span>
            <span class="nav-badge" id="nav-water-indicator">--%</span>
          </a>
          <a href="#safety" class="nav-link" data-section="safety">
            <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg>
            <span class="nav-link-text">Safety</span>
            <span class="nav-badge safe" id="nav-safety-indicator">OK</span>
          </a>
          <a href="#settings" class="nav-link" data-section="settings">
            <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-4 0v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83-2.83l.06-.06A1.65 1.65 0 0 0 4.6 15a1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1 0-4h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 2.83-2.83l.06.06A1.65 1.65 0 0 0 9 4.6a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 4 0v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 2.83l-.06.06A1.65 1.65 0 0 0 19.4 9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z"/></svg>
            <span class="nav-link-text">Settings</span>
          </a>
        </div>

        <button id="nav-hamburger" class="nav-hamburger" aria-label="Toggle navigation menu">
          <span></span><span></span><span></span>
        </button>
      </div>
    </nav>

    <!-- Mobile dropdown -->
    <div id="nav-mobile-menu" class="nav-mobile-menu"></div>

    <!-- ─── MAIN CONTENT ─── -->
    <main class="dashboard-main">

      <!-- ═══ 1. LIGHT CONTROL ═══ -->
      <section id="lights" class="dash-section">
        <div class="section-header scroll-reveal">
          <div class="section-icon light-accent">
            <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 18h6M10 22h4M12 2a7 7 0 0 1 7 7c0 2.38-1.19 4.47-3 5.74V17a1 1 0 0 1-1 1h-6a1 1 0 0 1-1-1v-2.26C6.19 13.47 5 11.38 5 9a7 7 0 0 1 7-7z"/></svg>
          </div>
          <div>
            <h2>Light Control</h2>
            <p class="section-subtitle">Manage your home lighting</p>
          </div>
        </div>
        <div id="lights-grid" class="lights-grid">
          <!-- JS generates 5 light cards -->
        </div>
      </section>

      <!-- ═══ 2. SMART GATE ═══ -->
      <section id="gate" class="dash-section">
        <div class="section-header scroll-reveal">
          <div class="section-icon gate-accent">
            <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2"/><path d="M12 3v18M3 12h18"/></svg>
          </div>
          <div>
            <h2>Smart Gate</h2>
            <p class="section-subtitle">Gate access control &amp; monitoring</p>
          </div>
        </div>
        <div class="gate-wrapper scroll-reveal">
          <div class="gate-status-bar">
            <div class="gate-stat">
              <span class="gate-stat-label">STATUS</span>
              <span id="gate-status-text" class="gate-stat-value status-closed">CLOSED</span>
            </div>
            <div class="gate-stat">
              <span class="gate-stat-label">POSITION</span>
              <span id="gate-position-text" class="gate-stat-value">0%</span>
            </div>
          </div>

          <div id="gate-visual" class="gate-visual">
            <div class="gate-pillar left-pillar">
              <div class="pillar-light"></div>
            </div>
            <div class="gate-track">
              <div id="gate-door-left" class="gate-door left-door"></div>
              <div id="gate-door-right" class="gate-door right-door"></div>
            </div>
            <div class="gate-pillar right-pillar">
              <div class="pillar-light"></div>
            </div>
            <!-- Safety blocked overlay -->
            <div id="gate-blocked-overlay" class="gate-blocked-overlay">
              <span class="gate-blocked-label">⚠ IR BLOCKED — GATE LOCKED</span>
            </div>
          </div>

          <div class="gate-controls">
            <div class="gate-slider-group">
              <label for="gate-slider" class="control-label">Manual Position</label>
              <div class="slider-row">
                <span class="slider-label">0%</span>
                <input type="range" id="gate-slider" class="styled-range" min="0" max="100" value="0">
                <span class="slider-label">100%</span>
              </div>
            </div>
            <div class="gate-btn-row">
              <button id="gate-open-btn" class="action-btn open-btn">
                <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="7 11 12 6 17 11"/></svg>
                OPEN
              </button>
              <button id="gate-close-btn" class="action-btn close-btn">
                <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="7 13 12 18 17 13"/></svg>
                CLOSE
              </button>
            </div>
            <div class="gate-hold-row">
              <button id="gate-hold-open" class="hold-btn open-hold">
                <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="7 11 12 6 17 11"/></svg>
                HOLD TO OPEN
              </button>
              <button id="gate-hold-close" class="hold-btn close-hold">
                <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="7 13 12 18 17 13"/></svg>
                HOLD TO CLOSE
              </button>
            </div>
          </div>
        </div>
      </section>

      <!-- ═══ 2B. SERVO CONTROL ═══ -->
      <section id="servo" class="dash-section">
        <div class="section-header scroll-reveal">
          <div class="section-icon servo-accent">
            <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><path d="M12 6v6l4 2"/></svg>
          </div>
          <div>
            <h2>Servo 2 (Auxiliary Motor)</h2>
            <p class="section-subtitle">Optional secondary motor control (GPIO 17)</p>
          </div>
        </div>
        <div class="servo-wrapper scroll-reveal">
          <div class="servo-layout">
            <!-- Visual -->
            <div class="servo-visual-area">
              <div class="servo-gauge">
                <svg viewBox="0 0 200 120" class="servo-arc-svg">
                  <!-- Track arc -->
                  <path d="M 20 110 A 80 80 0 0 1 180 110" fill="none" stroke="var(--bg-tertiary)" stroke-width="8" stroke-linecap="round"/>
                  <!-- Active arc (clipped by JS) -->
                  <path id="servo-arc-active" d="M 20 110 A 80 80 0 0 1 180 110" fill="none" stroke="var(--accent-purple)" stroke-width="8" stroke-linecap="round" stroke-dasharray="251.2" stroke-dashoffset="251.2"/>
                  <!-- Needle -->
                  <line id="servo-needle" x1="100" y1="110" x2="100" y2="35" stroke="var(--accent-cyan)" stroke-width="3" stroke-linecap="round" transform="rotate(-90, 100, 110)"/>
                  <!-- Center dot -->
                  <circle cx="100" cy="110" r="6" fill="var(--accent-cyan)"/>
                  <!-- Tick marks at 0, 45, 90, 135, 180 -->
                  <text x="12" y="108" fill="var(--text-dim)" font-size="10" font-family="JetBrains Mono">0°</text>
                  <text x="38" y="48" fill="var(--text-dim)" font-size="10" font-family="JetBrains Mono">45°</text>
                  <text x="90" y="22" fill="var(--text-dim)" font-size="10" font-family="JetBrains Mono">90°</text>
                  <text x="145" y="48" fill="var(--text-dim)" font-size="10" font-family="JetBrains Mono">135°</text>
                  <text x="164" y="108" fill="var(--text-dim)" font-size="10" font-family="JetBrains Mono">180°</text>
                </svg>
                <div class="servo-angle-display">
                  <span id="servo-angle-value" class="servo-angle-num">0</span>
                  <span class="servo-angle-unit">°</span>
                </div>
              </div>
            </div>
            <!-- Controls -->
            <div class="servo-controls-area">
              <div class="servo-power-row">
                <span class="servo-label">Power</span>
                <div class="servo-power-group">
                  <span class="servo-power-status" id="servo-power-status">OFF</span>
                  <label class="toggle-wrap" id="servo-toggle-wrap">
                    <input type="checkbox" class="toggle-input" id="servo-power-toggle">
                    <span class="toggle-slider"></span>
                  </label>
                </div>
              </div>
              <div class="servo-slider-group">
                <label for="servo-slider" class="control-label">Angle Position</label>
                <div class="slider-row">
                  <span class="slider-label">0°</span>
                  <input type="range" id="servo-slider" class="styled-range servo-range" min="0" max="180" value="0" disabled>
                  <span class="slider-label">180°</span>
                </div>
              </div>
              <div class="servo-preset-row">
                <button class="servo-preset-btn" data-angle="0">0°</button>
                <button class="servo-preset-btn" data-angle="45">45°</button>
                <button class="servo-preset-btn" data-angle="90">90°</button>
                <button class="servo-preset-btn" data-angle="135">135°</button>
                <button class="servo-preset-btn" data-angle="180">180°</button>
              </div>
            </div>
          </div>
        </div>
      </section>

      <!-- ═══ 3. WATER MANAGEMENT ═══ -->
      <section id="water" class="dash-section">
        <div class="section-header scroll-reveal">
          <div class="section-icon water-accent">
            <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 22a7 7 0 0 0 7-7c0-2-1-3.9-3-5.5s-3.5-4-4-6.5c-.5 2.5-2 4.9-4 6.5C6 11.1 5 13 5 15a7 7 0 0 0 7 7z"/></svg>
          </div>
          <div>
            <h2>Water Management</h2>
            <p class="section-subtitle">Tank monitoring &amp; pump control</p>
          </div>
        </div>

        <div class="wm-layout">
          <!-- LEFT COLUMN: Tank + Data -->
          <div class="wm-col-tank">
            <!-- Tank Visual -->
            <div class="wm-tank-card glass-card scroll-reveal">
              <div class="wm-tank-container">
                <!-- Vertical Ruler Scale (Left side) -->
                <div class="wm-ruler-scale">
                  <div class="wm-ruler-mark" style="bottom:100%"><span>100%</span></div>
                  <div class="wm-ruler-mark" style="bottom:90%"><span>90%</span></div>
                  <div class="wm-ruler-mark" style="bottom:80%"><span>80%</span></div>
                  <div class="wm-ruler-mark" style="bottom:70%"><span>70%</span></div>
                  <div class="wm-ruler-mark" style="bottom:60%"><span>60%</span></div>
                  <div class="wm-ruler-mark" style="bottom:50%"><span>50%</span></div>
                  <div class="wm-ruler-mark" style="bottom:40%"><span>40%</span></div>
                  <div class="wm-ruler-mark" style="bottom:30%"><span>30%</span></div>
                  <div class="wm-ruler-mark" style="bottom:20%"><span>20%</span></div>
                  <div class="wm-ruler-mark" style="bottom:10%"><span>10%</span></div>
                  <div class="wm-ruler-mark" style="bottom:0%"><span>0%</span></div>
                </div>

                <div class="wm-tank-wrapper">
                  <!-- Sensor icon & Inlet Pipe at top -->
                  <div class="wm-sensor-mount">
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--accent-cyan)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/></svg>
                    <span class="sensor-mount-label">HC-SR04</span>
                  </div>
                  <!-- Top Water Inlet Pipe -->
                  <div class="wm-inlet-pipe">
                    <div id="wm-water-stream" class="wm-water-stream" style="display:none;"></div>
                  </div>
                  <div class="wm-tank-body">
                    <!-- Glass specular reflection -->
                    <div class="wm-tank-glass-sheen"></div>
                    <div class="wm-tank-glass-highlight"></div>
                    <!-- Tick marks inside tank -->
                    <div class="wm-tick-marks">
                      <div class="wm-tick" style="bottom:75%"><span>75%</span></div>
                      <div class="wm-tick" style="bottom:50%"><span>50%</span></div>
                      <div class="wm-tick" style="bottom:25%"><span>25%</span></div>
                    </div>
                    <!-- Water fill -->
                    <div id="tank-water" class="wm-water-fill" style="height:0%">
                      <div class="wm-wave w1"></div>
                      <div class="wm-wave w2"></div>
                      <div class="wm-wave w3"></div>
                      <!-- Animated rising bubbles -->
                      <div class="wm-bubble b1"></div>
                      <div class="wm-bubble b2"></div>
                      <div class="wm-bubble b3"></div>
                      <div class="wm-bubble b4"></div>
                      <div class="wm-bubble b5"></div>
                    </div>
                  </div>
                  <!-- Tank base & Bottom Drain Valve -->
                  <div class="wm-tank-base">
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--accent-cyan)" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round" opacity="0.7"><path d="M12 22a7 7 0 0 0 7-7c0-2-1-3.9-3-5.5s-3.5-4-4-6.5c-.5 2.5-2 4.9-4 6.5C6 11.1 5 13 5 15a7 7 0 0 0 7 7z"/></svg>
                    <span class="wm-base-text">JDT INDUSTRIAL TANK</span>
                  </div>
                  <div class="wm-drain-valve"></div>
                </div>
              </div>
            </div>

            <!-- Data readouts row -->
            <div class="wm-data-row scroll-reveal">
              <!-- Water Level big display -->
              <div class="wm-data-main glass-card">
                <span class="wm-data-label">WATER LEVEL</span>
                <div class="wm-lcd-display">
                  <span id="water-percent" class="wm-lcd-value">0.0</span>
                  <span class="wm-lcd-unit">%</span>
                </div>
              </div>
            </div>
            <div class="wm-data-grid scroll-reveal">
              <div class="wm-data-cell glass-card">
                <span class="wm-data-label">DISTANCE</span>
                <div class="wm-lcd-display small">
                  <span id="sensor-distance" class="wm-lcd-value">0.0</span>
                  <span class="wm-lcd-unit">cm</span>
                </div>
              </div>
              <div class="wm-data-cell glass-card">
                <span class="wm-data-label">TANK DEPTH</span>
                <div class="wm-lcd-display small">
                  <span id="tank-depth-display" class="wm-lcd-value">100</span>
                  <span class="wm-lcd-unit">cm</span>
                </div>
              </div>
            </div>
            <div class="wm-data-row scroll-reveal">
              <div class="wm-data-cell glass-card" style="width:100%">
                <span class="wm-data-label">WATER HEIGHT</span>
                <div class="wm-lcd-display small">
                  <span id="water-height" class="wm-lcd-value">0.0</span>
                  <span class="wm-lcd-unit">cm</span>
                </div>
              </div>
            </div>
          </div>

          <!-- RIGHT COLUMN: Pump Control + Graph -->
          <div class="wm-col-controls">
            <!-- Pump Control -->
            <div class="wm-pump-card glass-card scroll-reveal">
              <div class="wm-pump-header">
                <div class="wm-pump-title-row">
                  <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="var(--accent-cyan)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"/></svg>
                  <div>
                    <h3>Pump Control</h3>
                    <span class="wm-pump-mode-label" id="pump-mode-label">Mode: AUTO</span>
                  </div>
                </div>
              </div>
              <div class="wm-pump-mode-tabs">
                <button id="pump-auto-btn" class="wm-pump-tab active" data-mode="auto">AUTO</button>
                <button id="pump-manual-btn" class="wm-pump-tab" data-mode="manual">MANUAL</button>
              </div>
              <div class="wm-pump-status-row">
                <div class="wm-pump-motor-info">
                  <span class="wm-pump-motor-label">Motor Pump</span>
                  <span class="wm-pump-motor-state" id="pump-status-text">OFF</span>
                </div>
                <div class="wm-pump-toggle-area">
                  <span class="pump-dot" id="pump-dot"></span>
                  <div class="pump-status-pill" id="pump-status-pill" style="display:none"></div>
                  <span id="pump-mini-status" style="display:none">OFF</span>
                </div>
              </div>
              <div id="pump-manual-controls" class="wm-pump-manual" style="display:none;">
                <button id="pump-power-btn" class="wm-pump-power-btn off">
                  <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M18.36 6.64a9 9 0 1 1-12.73 0"/><line x1="12" y1="2" x2="12" y2="12"/></svg>
                  <span>TURN ON</span>
                </button>
              </div>
              <p class="wm-pump-hint">Pump is controlled automatically based on thresholds</p>
            </div>

            <!-- Level History Graph -->
            <div class="wm-graph-card glass-card scroll-reveal">
              <div class="wm-graph-header">
                <div class="wm-graph-title-row">
                  <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--accent-cyan)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/></svg>
                  <div>
                    <h3>Level History</h3>
                    <span class="wm-graph-subtitle">Real-time water level graph</span>
                  </div>
                </div>
                <span class="card-badge">● LIVE</span>
              </div>
              <div class="wm-chart-wrap">
                <canvas id="water-chart"></canvas>
              </div>
            </div>
          </div>
        </div>
      </section>

      <!-- ═══ 4. SAFETY MONITORING ═══ -->
      <section id="safety" class="dash-section">
        <div class="section-header scroll-reveal">
          <div class="section-icon safety-accent">
            <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg>
          </div>
          <div>
            <h2>Safety Monitoring</h2>
            <p class="section-subtitle">Environmental sensor status</p>
          </div>
        </div>
        <div class="safety-grid">
          <!-- IR Sensor -->
          <div id="ir-sensor-card" class="sensor-card glass-card scroll-reveal" data-sensor="ir">
            <div class="sensor-icon-wrap ir-accent">
              <svg width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/></svg>
            </div>
            <h3 class="sensor-name">IR Motion Sensor</h3>
            <div class="sensor-reading">
              <span class="sensor-dot safe" id="ir-sensor-dot"></span>
              <span class="sensor-value" id="ir-sensor-status">No Motion</span>
            </div>
            <div class="sensor-bar-wrap">
              <div class="sensor-bar" id="ir-sensor-bar" style="width:10%"></div>
            </div>
            <span class="sensor-state-label" id="ir-state-label">SAFE</span>
          </div>
          <!-- Gas Sensor -->
          <div id="gas-sensor-card" class="sensor-card glass-card scroll-reveal" data-sensor="gas">
            <div class="sensor-icon-wrap gas-accent">
              <svg width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M8 2h8l4 10H4L8 2z"/><path d="M12 12v6"/><path d="M8 22h8"/><path d="M7 12a5 5 0 0 0 10 0"/></svg>
            </div>
            <h3 class="sensor-name">Gas Sensor</h3>
            <div class="sensor-reading">
              <span class="sensor-dot safe" id="gas-sensor-dot"></span>
              <span class="sensor-value" id="gas-sensor-status">Normal</span>
            </div>
            <div class="sensor-bar-wrap">
              <div class="sensor-bar" id="gas-sensor-bar" style="width:15%"></div>
            </div>
            <span class="sensor-state-label" id="gas-state-label">SAFE</span>
          </div>
          <!-- Flame Sensor -->
          <div id="flame-sensor-card" class="sensor-card glass-card scroll-reveal" data-sensor="flame">
            <div class="sensor-icon-wrap flame-accent">
              <svg width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 22c-4.97 0-9-2.69-9-6 0-2.16 1.34-4.12 3.42-5.37A4.001 4.001 0 0 1 12 2a4.001 4.001 0 0 1 5.58 8.63C19.66 11.88 21 13.84 21 16c0 3.31-4.03 6-9 6z"/></svg>
            </div>
            <h3 class="sensor-name">Flame Sensor</h3>
            <div class="sensor-reading">
              <span class="sensor-dot safe" id="flame-sensor-dot"></span>
              <span class="sensor-value" id="flame-sensor-status">No Flame</span>
            </div>
            <div class="sensor-bar-wrap">
              <div class="sensor-bar" id="flame-sensor-bar" style="width:5%"></div>
            </div>
            <span class="sensor-state-label" id="flame-state-label">SAFE</span>
          </div>
        </div>
        <!-- Demo trigger buttons for testing -->
        <div class="safety-demo-row">
          <button class="safety-demo-btn ir" onclick="window.SafetyMonitor && SafetyMonitor.demo('ir','safe')">IR: Safe</button>
          <button class="safety-demo-btn ir" onclick="window.SafetyMonitor && SafetyMonitor.demo('ir','warning')">IR: Motion</button>
          <button class="safety-demo-btn ir" onclick="window.SafetyMonitor && SafetyMonitor.demo('ir','danger')">IR: Intruder</button>
          <button class="safety-demo-btn gas" onclick="window.SafetyMonitor && SafetyMonitor.demo('gas','safe')">Gas: Safe</button>
          <button class="safety-demo-btn gas" onclick="window.SafetyMonitor && SafetyMonitor.demo('gas','danger')">Gas: Danger</button>
          <button class="safety-demo-btn flame" onclick="window.SafetyMonitor && SafetyMonitor.demo('flame','safe')">Flame: Safe</button>
          <button class="safety-demo-btn flame" onclick="window.SafetyMonitor && SafetyMonitor.demo('flame','danger')">Flame: FIRE</button>
        </div>
      </section>

      <!-- ═══ 5. SETTINGS ═══ -->
      <section id="settings" class="dash-section">
        <div class="section-header scroll-reveal">
          <div class="section-icon settings-accent">
            <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-4 0v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83-2.83l.06-.06A1.65 1.65 0 0 0 4.6 15a1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1 0-4h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 2.83-2.83l.06.06A1.65 1.65 0 0 0 9 4.6a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 4 0v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 2.83l-.06.06A1.65 1.65 0 0 0 19.4 9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z"/></svg>
          </div>
          <div>
            <h2>System Settings</h2>
            <p class="section-subtitle">Configure tank &amp; sensor parameters</p>
          </div>
        </div>
        <div class="settings-grid scroll-reveal">
          <div class="setting-card glass-card">
            <label for="setting-dash-name" class="setting-label">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="var(--accent-cyan)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"/><circle cx="12" cy="7" r="4"/></svg>
              Dashboard Name
            </label>
            <input type="text" id="setting-dash-name" class="setting-input" value="Smart Home" maxlength="40">
          </div>
          <div class="setting-card glass-card">
            <label for="setting-tank-depth" class="setting-label">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="var(--accent-cyan)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="4" y="2" width="16" height="20" rx="2"/><path d="M4 10h16"/></svg>
              Tank Depth (cm)
            </label>
            <input type="number" id="setting-tank-depth" class="setting-input" value="150" min="10" max="1000">
          </div>
          <div class="setting-card glass-card">
            <label for="setting-sensor-offset" class="setting-label">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="var(--accent-blue)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 20V10M18 20V4M6 20v-4"/></svg>
              Sensor Offset (cm)
            </label>
            <input type="number" id="setting-sensor-offset" class="setting-input" value="5" min="0" max="100">
          </div>
          <div class="setting-card glass-card">
            <label for="setting-low-threshold" class="setting-label">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="var(--warning)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg>
              Low Water Threshold (%)
            </label>
            <input type="number" id="setting-low-threshold" class="setting-input" value="20" min="0" max="95">
          </div>
          <div class="setting-card glass-card">
            <label for="setting-high-threshold" class="setting-label">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="var(--success)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="20 6 9 17 4 12"/></svg>
              High Water Threshold (%)
            </label>
            <input type="number" id="setting-high-threshold" class="setting-input" value="90" min="5" max="100">
          </div>
          <!-- Sound Alert Mute Setting -->
          <div class="setting-card glass-card">
            <label for="setting-sound-toggle" class="setting-label">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="var(--accent-purple)" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="11 5 6 9 2 9 2 15 6 15 11 19 11 5"/><path d="M19.07 4.93a10 10 0 0 1 0 14.14M15.54 8.46a5 5 0 0 1 0 7.07"/></svg>
              Alert Sounds
            </label>
            <div class="servo-power-group" style="justify-content: space-between; margin-top: 6px;">
              <span id="sound-status-text" style="font-family:'JetBrains Mono',monospace; font-size:0.8rem; color:var(--success); font-weight:700;">ENABLED</span>
              <label class="toggle-wrap">
                <input type="checkbox" class="toggle-input" id="setting-sound-toggle" checked>
                <span class="toggle-slider"></span>
              </label>
            </div>
          </div>
          <div class="setting-actions">
            <button id="settings-save-btn" class="action-btn save-btn">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/><polyline points="17 21 17 13 7 13 7 21"/><polyline points="7 3 7 8 15 8"/></svg>
              Save Settings
            </button>
          </div>
        </div>
      </section>

      <!-- ═══ 6. SYSTEM LOGS & ALERT HISTORY ═══ -->
      <section id="timeline" class="dash-section">
        <div class="timeline-grid">
          <!-- LEFT: Activity Timeline -->
          <div class="timeline-col">
            <div class="section-header scroll-reveal">
              <div class="section-icon timeline-accent">
                <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><polyline points="12 6 12 12 16 14"/></svg>
              </div>
              <div>
                <h2>Activity Timeline</h2>
                <p class="section-subtitle">Recent system events</p>
              </div>
            </div>
            <div class="timeline-card glass-card scroll-reveal">
              <div id="activity-timeline" class="activity-timeline">
                <!-- JS populates timeline items -->
              </div>
            </div>
          </div>

          <!-- RIGHT: Alert History Panel -->
          <div class="timeline-col">
            <div class="section-header scroll-reveal">
              <div class="section-icon safety-accent">
                <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg>
              </div>
              <div>
                <h2>Alert History</h2>
                <p class="section-subtitle">Stored log (Last 100 alerts)</p>
              </div>
              </div>
            </div>
            <div class="alert-history-card glass-card scroll-reveal">
              <div class="alert-history-filters">
                <button class="alert-filter-btn active" data-filter="all">ALL</button>
                <button class="alert-filter-btn" data-filter="info">INFO</button>
                <button class="alert-filter-btn" data-filter="warning">WARN</button>
                <button class="alert-filter-btn" data-filter="danger">DANGER</button>
                <button class="alert-filter-btn" data-filter="emergency">EMERGENCY</button>
              </div>
              <div id="alert-history-list" class="alert-history-list"></div>
            </div>
          </div>
        </div>
      </section>

    </main>

    <!-- ─── FOOTER ─── -->
    <footer class="dashboard-footer">
      <div class="footer-inner">
        <span id="footer-brand-title">SMART HOME v3.0</span>
        <span class="footer-sep">·</span>
        <span>Powered by <a href="https://youtube.com/@easyelectrotech7743" target="_blank" rel="noopener">Easy Electro Tech</a></span>
      </div>
    </footer>
  </div>

  <!-- ─── TOAST NOTIFICATION STACK (TOP-RIGHT) ─── -->
  <div id="toast-container" class="toast-container"></div>
  <div id="toast" class="toast"></div>

  <!-- ─── IR GATE SAFETY ALERT MODAL ─── -->
  <div id="safety-modal" class="safety-modal" role="dialog" aria-modal="true" aria-labelledby="safety-modal-title">
    <div class="safety-modal-backdrop"></div>
    <div class="safety-modal-panel">
      <!-- Animated warning header -->
      <div class="safety-modal-header">
        <div class="safety-modal-icon-wrap">
          <div class="safety-modal-pulse-ring"></div>
          <div class="safety-modal-pulse-ring delay1"></div>
          <svg class="safety-modal-icon" width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/>
          </svg>
        </div>
        <div>
          <h2 id="safety-modal-title" class="safety-modal-title">IR MOTION DETECTED</h2>
          <p class="safety-modal-subtitle">Gate Safety Validation Required</p>
        </div>
      </div>

      <!-- Alert details -->
      <div class="safety-modal-body">
        <div class="safety-modal-reason">
          <span class="safety-modal-reason-label">ALERT REASON</span>
          <span class="safety-modal-reason-text" id="safety-modal-reason">Object or person detected near gate</span>
        </div>
        <div class="safety-modal-sensor-row">
          <div class="safety-modal-sensor-item">
            <div class="sms-dot ir-dot" id="sms-ir-dot"></div>
            <span>IR Sensor</span>
            <span class="sms-val" id="sms-ir-val">TRIGGERED</span>
          </div>
          <div class="safety-modal-sensor-item">
            <div class="sms-dot gas-dot" id="sms-gas-dot"></div>
            <span>Gas</span>
            <span class="sms-val" id="sms-gas-val">OK</span>
          </div>
          <div class="safety-modal-sensor-item">
            <div class="sms-dot flame-dot" id="sms-flame-dot"></div>
            <span>Flame</span>
            <span class="sms-val" id="sms-flame-val">OK</span>
          </div>
        </div>
        <div class="safety-modal-warning-note">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg>
          <span>Final hardware safety authority rests with the ESP32. This is a frontend validation layer only.</span>
        </div>
      </div>

      <!-- Action buttons -->
      <div class="safety-modal-actions">
        <button id="safety-modal-reject" class="safety-btn reject-btn">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round"><line x1="18" y1="6" x2="6" y2="18"/><line x1="6" y1="6" x2="18" y2="18"/></svg>
          REJECT — KEEP CLOSED
        </button>
        <button id="safety-modal-accept" class="safety-btn accept-btn">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round"><polyline points="20 6 9 17 4 12"/></svg>
          ACCEPT — OPEN GATE
        </button>
      </div>
    </div>
  </div>

  <!-- ─── SYSTEM DANGER BANNER ─── -->
  <div id="danger-banner" class="danger-banner" style="display:none">
    <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg>
    <span id="danger-banner-text">SYSTEM ALERT — Dangerous conditions detected</span>
    <button id="danger-banner-close" class="danger-banner-close" aria-label="Dismiss">✕</button>
  </div>

  <!-- ─── SCRIPTS ─── -->
  <script src="app.js"></script>
  <script src="esp32-ws.js"></script>
</body>
</html>

)rawliteral";

const char STYLE_CSS[] PROGMEM = R"rawliteral(
/* ==========================================================================
   NEXUS HOME — COMMERCIAL SMART HOME IoT CONTROL CENTER DESIGN SYSTEM
   ========================================================================== */

:root {
  --bg-primary: #030712;
  --bg-secondary: #0b1329;
  --bg-tertiary: #111e38;
  --accent-cyan: #00e5ff;
  --accent-blue: #2979ff;
  --accent-purple: #b388ff;
  --accent-purple-dim: rgba(179, 136, 255, 0.15);
  --danger: #ff5252;
  --danger-dim: rgba(255, 82, 82, 0.15);
  --warning: #ffab40;
  --warning-dim: rgba(255, 171, 64, 0.15);
  --success: #00e676;
  --success-dim: rgba(0, 230, 118, 0.15);
  --text-primary: #f1f5f9;
  --text-secondary: #94a3b8;
  --text-dim: #475569;
  --glass-bg: rgba(11, 19, 41, 0.75);
  --glass-border: rgba(0, 229, 255, 0.14);
  --glass-border-hover: rgba(0, 229, 255, 0.35);
  --glass-shadow: 0 12px 40px rgba(0, 0, 0, 0.6), inset 0 1px 0 rgba(255, 255, 255, 0.06);
  --glow-cyan: 0 0 25px rgba(0, 229, 255, 0.35);
  --glow-blue: 0 0 25px rgba(41, 121, 255, 0.35);
  --glow-purple: 0 0 25px rgba(179, 136, 255, 0.35);
  --water-color: rgba(0, 229, 255, 0.75);
  --radius-sm: 10px;
  --radius-md: 18px;
  --radius-lg: 28px;
}

/* ─── 1. RESET & BASE ─── */
*, *::before, *::after {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

body {
  background-color: var(--bg-primary);
  color: var(--text-primary);
  font-family: 'Outfit', -apple-system, BlinkMacSystemFont, sans-serif;
  overflow-x: hidden;
  scroll-behavior: smooth;
  line-height: 1.5;
}

/* Custom Scrollbar */
::-webkit-scrollbar {
  width: 8px;
}
::-webkit-scrollbar-track {
  background: var(--bg-primary);
}
::-webkit-scrollbar-thumb {
  background: rgba(0, 229, 255, 0.2);
  border-radius: 4px;
}
::-webkit-scrollbar-thumb:hover {
  background: var(--accent-cyan);
}

/* ─── 2. BOOT SCREEN ─── */
#boot-screen {
  position: fixed;
  inset: 0;
  background-color: var(--bg-primary);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 9999;
  transition: opacity 0.5s ease;
}

.boot-content {
  text-align: center;
}

.boot-logo {
  display: flex;
  justify-content: center;
  margin-bottom: 24px;
}

.boot-hex .hex-outer {
  stroke: var(--accent-cyan);
  stroke-dasharray: 200;
  stroke-dashoffset: 200;
  animation: draw-hex 2s forwards;
}

.boot-hex .hex-inner {
  stroke: var(--accent-blue);
  stroke-dasharray: 150;
  stroke-dashoffset: 150;
  animation: draw-hex 2s 0.5s forwards;
}

.boot-hex .hex-core {
  fill: var(--accent-cyan);
  animation: pulse-core 2s infinite alternate;
}

@keyframes draw-hex {
  to { stroke-dashoffset: 0; }
}

@keyframes pulse-core {
  from { transform: scale(0.8); opacity: 0.6; }
  to   { transform: scale(1.2); opacity: 1; }
}

.boot-title {
  font-size: 2.5rem;
  font-weight: 800;
  letter-spacing: 0.3em;
  text-transform: uppercase;
  color: var(--accent-cyan);
  animation: fade-in 1s ease forwards;
  opacity: 0;
}

.boot-subtitle {
  color: var(--text-secondary);
  font-size: 1rem;
  animation: fade-in 1s 0.5s ease forwards;
  opacity: 0;
  margin-bottom: 32px;
}

@keyframes fade-in {
  to { opacity: 1; }
}

.boot-progress {
  width: 320px;
  height: 4px;
  background-color: var(--bg-tertiary);
  border-radius: var(--radius-sm);
  margin: 0 auto;
  overflow: hidden;
}

.boot-progress-bar {
  background: linear-gradient(to right, var(--accent-cyan), var(--accent-blue));
  height: 100%;
  border-radius: var(--radius-sm);
  width: 0%;
  transition: width 0.3s ease;
}

.boot-status {
  color: var(--text-secondary);
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.85rem;
  margin-top: 16px;
}

/* ─── 3. NAVIGATION ─── */
.main-nav {
  position: sticky;
  top: 0;
  z-index: 100;
  background-color: rgba(3, 7, 18, 0.85);
  backdrop-filter: blur(24px);
  border-bottom: 1px solid var(--glass-border);
  box-shadow: 0 4px 30px rgba(0, 0, 0, 0.4);
}

.nav-inner {
  max-width: 1440px;
  margin: 0 auto;
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 10px 24px;
}

.nav-brand {
  display: flex;
  align-items: center;
  gap: 10px;
  color: var(--accent-cyan);
  font-family: 'Outfit', sans-serif;
  font-weight: 800;
  font-size: 1.15rem;
  letter-spacing: 0.1em;
  text-decoration: none;
}

.nav-center {
  display: flex;
  align-items: center;
  gap: 18px;
}

.nav-status {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 4px 12px;
  background: rgba(255, 255, 255, 0.04);
  border: 1px solid rgba(255, 255, 255, 0.08);
  border-radius: 20px;
  transition: all 0.2s;
  cursor: pointer;
}

.nav-status:hover {
  background: rgba(255, 255, 255, 0.08);
}

.status-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  display: inline-block;
}

.status-dot.online {
  background-color: var(--success);
  box-shadow: 0 0 10px var(--success);
  animation: glow-pulse-success 2s infinite;
}

.status-dot.offline {
  background-color: var(--danger);
  box-shadow: 0 0 10px var(--danger);
}

@keyframes glow-pulse-success {
  0%, 100% { box-shadow: 0 0 5px var(--success); }
  50%       { box-shadow: 0 0 15px var(--success); }
}

.status-text {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.72rem;
  font-weight: 700;
  color: var(--text-secondary);
}

.nav-clock {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.85rem;
  color: var(--accent-cyan);
  font-weight: 600;
}

.nav-links {
  display: flex;
  gap: 4px;
}

.nav-link {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 7px 12px;
  border-radius: var(--radius-sm);
  color: var(--text-secondary);
  text-decoration: none;
  transition: all 0.25s ease;
  font-size: 0.8rem;
  font-weight: 600;
}

.nav-link:hover, .nav-link.active {
  color: var(--accent-cyan);
  background: rgba(0, 229, 255, 0.08);
  box-shadow: inset 0 0 15px rgba(0, 229, 255, 0.05);
}

.nav-badge {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.65rem;
  padding: 2px 6px;
  border-radius: 4px;
  background: rgba(0, 229, 255, 0.1);
  color: var(--accent-cyan);
  border: 1px solid rgba(0, 229, 255, 0.2);
}

.nav-badge.active {
  background: rgba(0, 229, 255, 0.25);
  color: var(--accent-cyan);
  border-color: var(--accent-cyan);
  box-shadow: 0 0 10px rgba(0, 229, 255, 0.4);
}

.nav-badge.safe, .nav-badge.status-open {
  background: var(--success-dim);
  color: var(--success);
  border-color: rgba(0, 230, 118, 0.3);
}

.nav-badge.danger {
  background: var(--danger-dim);
  color: var(--danger);
  border-color: rgba(255, 82, 82, 0.3);
}

.nav-hamburger {
  display: none;
  background: none;
  border: none;
  cursor: pointer;
  width: 32px;
  height: 32px;
  position: relative;
}

.nav-hamburger span {
  display: block;
  width: 22px;
  height: 2px;
  background: var(--text-primary);
  margin: 5px auto;
  transition: all 0.3s;
  border-radius: 2px;
}

.nav-hamburger.open span:nth-child(1) { transform: translateY(7px) rotate(45deg); }
.nav-hamburger.open span:nth-child(2) { opacity: 0; }
.nav-hamburger.open span:nth-child(3) { transform: translateY(-7px) rotate(-45deg); }

.nav-mobile-menu {
  display: none;
  flex-direction: column;
  gap: 8px;
  padding: 16px 24px;
  background: var(--bg-secondary);
  border-bottom: 1px solid var(--glass-border);
}

.nav-mobile-menu.open {
  display: flex;
}

/* ─── 4. LAYOUT & DASHBOARD MAIN ─── */
.dashboard {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
}

.dashboard-main {
  max-width: 1440px;
  width: 100%;
  margin: 0 auto;
  padding: 16px 24px;
  flex: 1;
}

.dash-section {
  margin-bottom: 32px;
  padding-top: 68px;
}

.section-header {
  display: flex;
  align-items: center;
  gap: 16px;
  margin-bottom: 20px;
}

.section-icon {
  width: 48px;
  height: 48px;
  border-radius: var(--radius-md);
  display: flex;
  align-items: center;
  justify-content: center;
  border: 1px solid var(--glass-border);
}

.light-accent { background: rgba(0, 229, 255, 0.1); color: var(--accent-cyan); border-color: rgba(0, 229, 255, 0.2); }
.gate-accent  { background: rgba(41, 121, 255, 0.1); color: var(--accent-blue); border-color: rgba(41, 121, 255, 0.2); }
.servo-accent { background: rgba(179, 136, 255, 0.1); color: var(--accent-purple); border-color: rgba(179, 136, 255, 0.2); }
.water-accent { background: rgba(0, 229, 255, 0.1); color: var(--accent-cyan); border-color: rgba(0, 229, 255, 0.2); }
.safety-accent{ background: rgba(0, 230, 118, 0.1); color: var(--success); border-color: rgba(0, 230, 118, 0.2); }
.settings-accent{ background: var(--accent-purple-dim); color: var(--accent-purple); border-color: rgba(179, 136, 255, 0.2); }
.timeline-accent{ background: rgba(41, 121, 255, 0.1); color: var(--accent-blue); border-color: rgba(41, 121, 255, 0.2); }

.section-header h2 {
  font-family: 'Outfit', sans-serif;
  font-weight: 800;
  font-size: 1.45rem;
  letter-spacing: 0.02em;
  color: var(--text-primary);
}

.section-subtitle {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.76rem;
  color: var(--text-secondary);
}

.glass-card {
  background: var(--glass-bg);
  backdrop-filter: blur(24px);
  border: 1px solid var(--glass-border);
  border-radius: var(--radius-md);
  box-shadow: var(--glass-shadow);
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
}

/* ─── 5. LIGHT CONTROL ─── */
.lights-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(220px, 1fr));
  gap: 16px;
}

.light-card {
  padding: 22px 20px;
  display: flex;
  flex-direction: column;
  align-items: center;
  text-align: center;
  position: relative;
  overflow: hidden;
  box-shadow: var(--glass-shadow);
  cursor: pointer;
  user-select: none;
}

.light-card:hover {
  border-color: var(--glass-border-hover);
  transform: translateY(-4px);
  box-shadow: 0 16px 40px rgba(0, 0, 0, 0.5), 0 0 20px rgba(0, 229, 255, 0.15);
}

.light-card.on {
  border-color: rgba(0, 229, 255, 0.45);
  box-shadow: 0 16px 44px rgba(0, 0, 0, 0.65), 0 0 35px rgba(0, 229, 255, 0.25);
  background: linear-gradient(180deg, rgba(0, 229, 255, 0.1) 0%, rgba(11, 19, 41, 0.85) 100%);
}

.light-card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  width: 100%;
  margin-bottom: 16px;
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.68rem;
}

.light-conn-badge {
  color: var(--success);
  font-weight: 700;
  letter-spacing: 0.05em;
}

.light-power-val {
  color: var(--text-secondary);
  background: rgba(255, 255, 255, 0.04);
  padding: 2px 8px;
  border-radius: 6px;
  border: 1px solid rgba(255, 255, 255, 0.08);
  transition: all 0.3s;
}

.light-card.on .light-power-val {
  color: var(--accent-cyan);
  border-color: rgba(0, 229, 255, 0.3);
  background: rgba(0, 229, 255, 0.15);
  box-shadow: 0 0 10px rgba(0, 229, 255, 0.2);
}

.light-icon {
  width: 68px;
  height: 68px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  background: var(--bg-tertiary);
  color: var(--text-dim);
  border: 1px solid var(--glass-border);
  margin-bottom: 16px;
  transition: all 0.35s ease;
}

.light-icon svg {
  transition: all 0.35s ease;
}

.light-card.on .light-icon {
  color: var(--accent-cyan);
  background: rgba(0, 229, 255, 0.18);
  border-color: rgba(0, 229, 255, 0.6);
  box-shadow: 0 0 30px rgba(0, 229, 255, 0.45);
}

.light-card.on .light-icon svg {
  color: var(--accent-cyan);
  fill: rgba(0, 229, 255, 0.35);
  filter: drop-shadow(0 0 10px rgba(0, 229, 255, 0.8));
  transform: scale(1.1);
}

.light-name-row {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 4px;
}

.edit-light-name-btn {
  background: transparent;
  border: none;
  color: var(--text-dim);
  font-size: 0.85rem;
  cursor: pointer;
  padding: 2px 5px;
  border-radius: 4px;
  transition: all 0.2s ease;
  line-height: 1;
}

.edit-light-name-btn:hover {
  color: var(--accent-cyan);
  background: rgba(0, 229, 255, 0.15);
  transform: scale(1.15);
}

.light-name {
  font-size: 0.98rem;
  font-weight: 700;
  color: var(--text-primary);
  display: inline-block;
}

.light-status {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.75rem;
  color: var(--text-secondary);
  font-weight: 700;
  letter-spacing: 0.05em;
  transition: all 0.3s;
}

.light-card.on .light-status {
  color: var(--accent-cyan);
  text-shadow: 0 0 8px rgba(0, 229, 255, 0.5);
}

/* Custom Animated Switch Toggle */
.toggle-wrap {
  position: relative;
  display: inline-block;
  width: 56px;
  height: 30px;
  cursor: pointer;
  flex-shrink: 0;
}

.toggle-input {
  opacity: 0;
  width: 0;
  height: 0;
  position: absolute;
  pointer-events: none;
}

.toggle-slider {
  position: absolute;
  inset: 0;
  background: var(--bg-tertiary);
  border: 1px solid rgba(255, 255, 255, 0.15);
  border-radius: 20px;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  box-shadow: inset 0 2px 6px rgba(0, 0, 0, 0.5);
}

.toggle-slider::before {
  content: '';
  position: absolute;
  top: 3px;
  left: 3px;
  width: 22px;
  height: 22px;
  border-radius: 50%;
  background: var(--text-secondary);
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  box-shadow: 0 2px 6px rgba(0, 0, 0, 0.5);
}

.toggle-input:checked + .toggle-slider,
.light-card.on .toggle-slider {
  background: rgba(0, 229, 255, 0.25);
  border-color: var(--accent-cyan);
  box-shadow: 0 0 18px rgba(0, 229, 255, 0.35), inset 0 0 8px rgba(0, 229, 255, 0.2);
}

.toggle-input:checked + .toggle-slider::before,
.light-card.on .toggle-slider::before {
  transform: translateX(26px);
  background: var(--accent-cyan);
  box-shadow: 0 0 12px var(--accent-cyan);
}

/* ─── 6. SMART GATE ─── */
.gate-wrapper {
  padding: 24px;
}

.gate-status-bar {
  display: flex;
  gap: 32px;
  padding-bottom: 16px;
  border-bottom: 1px solid var(--glass-border);
  margin-bottom: 20px;
}

.gate-stat {
  display: flex;
  flex-direction: column;
}

.gate-stat-label {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.65rem;
  color: var(--text-dim);
  letter-spacing: 0.1em;
}

.gate-stat-value {
  font-family: 'JetBrains Mono', monospace;
  font-size: 1.15rem;
  font-weight: 700;
}

.status-closed { color: var(--accent-blue); }
.status-open   { color: var(--success); }
.status-opening, .status-closing { color: var(--warning); }

.gate-visual {
  height: 200px;
  background: radial-gradient(circle at 50% 50%, rgba(15, 29, 53, 0.8), rgba(3, 7, 18, 0.95));
  border: 1px solid var(--glass-border);
  border-radius: var(--radius-md);
  position: relative;
  overflow: hidden;
  margin-bottom: 20px;
  display: flex;
  align-items: flex-end;
  box-shadow: inset 0 0 30px rgba(0, 0, 0, 0.8);
}

.gate-pillar {
  width: 24px;
  height: 100%;
  background: linear-gradient(180deg, var(--bg-tertiary), rgba(5, 10, 24, 0.9));
  border: 1px solid var(--glass-border);
  position: relative;
  z-index: 2;
  flex-shrink: 0;
}

.pillar-light {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  position: absolute;
  top: 16px;
  left: 50%;
  transform: translateX(-50%);
  background: var(--accent-cyan);
  box-shadow: 0 0 10px var(--accent-cyan);
  animation: pulse 2s infinite;
}

.gate-track {
  flex: 1;
  height: 75%;
  display: flex;
  position: relative;
  background: repeating-linear-gradient(90deg, transparent, transparent 15px, rgba(0, 229, 255, 0.04) 15px, rgba(0, 229, 255, 0.04) 16px);
}

.gate-door {
  height: 100%;
  background: linear-gradient(180deg, rgba(20, 35, 60, 0.95), rgba(10, 20, 38, 0.95));
  border: 1px solid var(--glass-border);
  position: relative;
  background-image: repeating-linear-gradient(90deg, rgba(0, 229, 255, 0.05), rgba(0, 229, 255, 0.05) 10px, transparent 10px, transparent 20px);
}

.left-door  { border-right: 2px solid var(--accent-blue); }
.right-door { border-left: 2px solid var(--accent-blue); }

.gate-controls {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.gate-slider-group {
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.control-label {
  font-size: 0.8rem;
  color: var(--text-secondary);
  font-weight: 500;
}

.slider-row {
  display: flex;
  align-items: center;
  gap: 12px;
}

.slider-label {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.75rem;
  color: var(--text-dim);
  min-width: 36px;
}

.styled-range {
  -webkit-appearance: none;
  width: 100%;
  height: 6px;
  background: var(--bg-tertiary);
  border-radius: 3px;
  outline: none;
}

.styled-range::-webkit-slider-thumb {
  -webkit-appearance: none;
  width: 20px;
  height: 20px;
  border-radius: 50%;
  background: var(--accent-cyan);
  cursor: pointer;
  box-shadow: 0 0 15px var(--accent-cyan);
  transition: transform 0.2s;
}

.styled-range::-webkit-slider-thumb:hover {
  transform: scale(1.25);
}

.gate-btn-row, .gate-hold-row {
  display: flex;
  gap: 12px;
}

.action-btn, .hold-btn {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  padding: 12px 20px;
  border-radius: var(--radius-sm);
  border: 1px solid var(--glass-border);
  background: var(--glass-bg);
  color: var(--text-primary);
  font-weight: 700;
  font-size: 0.82rem;
  cursor: pointer;
  transition: all 0.25s;
}

.action-btn:hover {
  border-color: var(--accent-cyan);
  color: var(--accent-cyan);
  box-shadow: 0 0 15px rgba(0, 229, 255, 0.2);
}

.open-btn:hover, .open-hold:hover {
  border-color: var(--success);
  color: var(--success);
  box-shadow: 0 0 15px rgba(0, 230, 118, 0.2);
}

.close-btn:hover, .close-hold:hover {
  border-color: var(--danger);
  color: var(--danger);
  box-shadow: 0 0 15px rgba(255, 82, 82, 0.2);
}

.hold-btn {
  border-style: dashed;
  font-size: 0.75rem;
}

.save-btn {
  background: linear-gradient(135deg, var(--accent-cyan), var(--accent-blue));
  color: #030712;
  border: none;
  font-weight: 800;
}

.save-btn:hover {
  box-shadow: var(--glow-cyan);
  transform: translateY(-2px);
}

/* ─── 7. SERVO CONTROL ─── */
.servo-wrapper {
  padding: 24px;
}

.servo-layout {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 28px;
}

.servo-visual-area {
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 16px;
}

.servo-gauge {
  position: relative;
  width: 220px;
  text-align: center;
}

.servo-arc-svg {
  width: 100%;
  height: auto;
}

#servo-needle {
  transition: transform 0.4s cubic-bezier(0.4, 0, 0.2, 1);
}

#servo-arc-active {
  transition: stroke-dashoffset 0.4s cubic-bezier(0.4, 0, 0.2, 1);
}

.servo-angle-display {
  position: absolute;
  bottom: 8px;
  left: 50%;
  transform: translateX(-50%);
  display: flex;
  align-items: baseline;
}

.servo-angle-num {
  font-family: 'Orbitron', sans-serif;
  font-size: 2.4rem;
  font-weight: 800;
  color: var(--accent-purple);
}

.servo-angle-unit {
  font-size: 1.1rem;
  color: var(--text-secondary);
}

.servo-controls-area {
  display: flex;
  flex-direction: column;
  gap: 16px;
  justify-content: center;
}

.servo-power-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding-bottom: 12px;
  border-bottom: 1px solid var(--glass-border);
}

.servo-label {
  font-weight: 600;
  font-size: 0.9rem;
}

.servo-power-group {
  display: flex;
  align-items: center;
  gap: 12px;
}

.servo-power-status {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.8rem;
  color: var(--text-secondary);
}

.on .servo-power-status {
  color: var(--accent-purple);
  font-weight: 700;
}

.servo-range:disabled {
  opacity: 0.35;
  cursor: not-allowed;
}

.servo-range:not(:disabled)::-webkit-slider-thumb {
  background: var(--accent-purple);
  box-shadow: var(--glow-purple);
}

.servo-preset-row {
  display: flex;
  gap: 8px;
  flex-wrap: wrap;
}

.servo-preset-btn {
  flex: 1;
  padding: 8px 12px;
  border-radius: var(--radius-sm);
  border: 1px solid var(--glass-border);
  background: transparent;
  color: var(--text-secondary);
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.75rem;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.25s;
}

.servo-preset-btn:hover:not(:disabled) {
  border-color: var(--accent-purple);
  color: var(--accent-purple);
  box-shadow: var(--glow-purple);
}

.servo-preset-btn.active {
  background: var(--accent-purple-dim);
  border-color: var(--accent-purple);
  color: var(--accent-purple);
}

.servo-preset-btn:disabled {
  opacity: 0.35;
  cursor: not-allowed;
}

/* ─── 8. WATER MANAGEMENT (INDUSTRIAL DIGITAL TWIN) ─── */
.wm-layout {
  display: grid;
  grid-template-columns: 1.1fr 0.9fr;
  gap: 24px;
  align-items: flex-start;
}

.wm-col-tank {
  display: flex;
  flex-direction: column;
  gap: 16px;
  align-items: center;
  width: 100%;
}

.wm-col-controls {
  display: flex;
  flex-direction: column;
  gap: 18px;
  width: 100%;
}

.wm-tank-card {
  padding: 28px 20px;
  display: flex;
  justify-content: center;
  align-items: center;
  width: 100%;
  position: relative;
  overflow: hidden;
}

.wm-tank-container {
  display: flex;
  align-items: flex-end;
  gap: 18px;
  position: relative;
  margin: 10px 0;
}

/* Vertical Ruler Scale */
.wm-ruler-scale {
  position: relative;
  height: 400px;
  width: 48px;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.65rem;
  color: var(--text-secondary);
  user-select: none;
}

.wm-ruler-mark {
  position: absolute;
  right: 0;
  display: flex;
  align-items: center;
  gap: 6px;
  transform: translateY(50%);
}

.wm-ruler-mark::after {
  content: '';
  width: 10px;
  height: 2px;
  background: rgba(0, 229, 255, 0.4);
  box-shadow: 0 0 6px rgba(0, 229, 255, 0.3);
}

.wm-ruler-mark span {
  font-weight: 700;
  color: var(--text-secondary);
}

.wm-tank-wrapper {
  display: flex;
  flex-direction: column;
  align-items: center;
  position: relative;
}

/* Top Sensor Mount & Label */
.wm-sensor-mount {
  width: 80px;
  height: 26px;
  background: linear-gradient(180deg, var(--bg-tertiary), rgba(15, 29, 53, 0.95));
  border: 1px solid rgba(0, 229, 255, 0.35);
  border-bottom: none;
  border-radius: 8px 8px 0 0;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 6px;
  margin-bottom: -1px;
  z-index: 5;
  box-shadow: 0 -4px 15px rgba(0, 229, 255, 0.2);
}

.sensor-mount-label {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.58rem;
  font-weight: 700;
  color: var(--accent-cyan);
  letter-spacing: 0.05em;
}

/* Top Water Inlet Pipe & Active Water Stream */
.wm-inlet-pipe {
  position: absolute;
  top: -12px;
  right: 25px;
  width: 18px;
  height: 38px;
  border: 2px solid rgba(0, 229, 255, 0.4);
  border-bottom: none;
  border-radius: 4px 4px 0 0;
  background: linear-gradient(90deg, rgba(15, 29, 53, 0.9), rgba(0, 229, 255, 0.2));
  z-index: 4;
}

.wm-water-stream {
  position: absolute;
  top: 36px;
  left: 3px;
  width: 10px;
  height: 390px;
  background: linear-gradient(180deg, rgba(0, 229, 255, 0.8), rgba(0, 229, 255, 0.2));
  border-radius: 4px;
  box-shadow: 0 0 12px rgba(0, 229, 255, 0.6);
  animation: stream-flow 0.5s linear infinite;
  z-index: 3;
}

@keyframes stream-flow {
  0% { opacity: 0.6; transform: scaleX(0.9); }
  50% { opacity: 1; transform: scaleX(1.1); }
  100% { opacity: 0.6; transform: scaleX(0.9); }
}

/* Glass Tank Body */
.wm-tank-body {
  width: 220px;
  height: 400px;
  border: 2px solid rgba(0, 229, 255, 0.35);
  border-radius: 8px 8px 20px 20px;
  position: relative;
  overflow: hidden;
  background: radial-gradient(circle at 30% 20%, rgba(0, 229, 255, 0.06), rgba(3, 7, 18, 0.85));
  box-shadow: 
    inset 0 0 35px rgba(0, 0, 0, 0.85),
    0 16px 40px rgba(0, 0, 0, 0.6),
    0 0 25px rgba(0, 229, 255, 0.18);
}

.wm-tank-glass-sheen {
  position: absolute;
  top: 0;
  left: 0;
  width: 35%;
  height: 100%;
  background: linear-gradient(90deg, rgba(255, 255, 255, 0.12) 0%, rgba(255, 255, 255, 0) 100%);
  pointer-events: none;
  z-index: 10;
}

.wm-tank-glass-highlight {
  position: absolute;
  top: 0;
  right: 12px;
  width: 3px;
  height: 100%;
  background: linear-gradient(180deg, rgba(255, 255, 255, 0.25), rgba(255, 255, 255, 0));
  pointer-events: none;
  z-index: 10;
}

.wm-tick-marks {
  position: absolute;
  inset: 0;
  z-index: 5;
  pointer-events: none;
}

.wm-tick {
  position: absolute;
  left: 0;
  right: 0;
  height: 1px;
  background: rgba(255, 255, 255, 0.12);
  border-bottom: 1px dashed rgba(0, 229, 255, 0.2);
}

.wm-tick span {
  position: absolute;
  right: 8px;
  top: -8px;
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.62rem;
  color: rgba(255, 255, 255, 0.35);
}

.wm-water-fill {
  position: absolute;
  bottom: 0;
  left: 0;
  right: 0;
  background: linear-gradient(180deg, rgba(0, 229, 255, 0.88) 0%, rgba(41, 121, 255, 0.78) 100%);
  box-shadow: 0 -6px 30px rgba(0, 229, 255, 0.55);
  transition: height 0.8s cubic-bezier(0.4, 0, 0.2, 1);
  z-index: 1;
}

/* Water Wave Animations */
.wm-wave {
  position: absolute;
  top: -12px;
  left: -10%;
  width: 120%;
  height: 24px;
  background: url('data:image/svg+xml;utf8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 24"><path d="M0,12 C25,0 25,24 50,12 C75,0 75,24 100,12 L100,24 L0,24 Z" fill="rgba(0, 229, 255, 0.85)"/></svg>') repeat-x;
  background-size: 100px 24px;
  animation: wm-wave-slide infinite linear;
}

.wm-wave.w1 { opacity: 0.9; animation-duration: 2.8s; }
.wm-wave.w2 { opacity: 0.5; animation-duration: 2.2s; top: -8px; animation-direction: reverse; }
.wm-wave.w3 { opacity: 0.3; animation-duration: 3.5s; top: -14px; }

@keyframes wm-wave-slide {
  from { transform: translateX(0); }
  to   { transform: translateX(-50%); }
}

/* Rising Bubbles */
.wm-bubble {
  position: absolute;
  bottom: 0;
  background: rgba(255, 255, 255, 0.6);
  border-radius: 50%;
  pointer-events: none;
  animation: float-bubble 4s infinite ease-in;
}

.wm-bubble.b1 { left: 15%; width: 6px; height: 6px; animation-delay: 0s; animation-duration: 3.5s; }
.wm-bubble.b2 { left: 40%; width: 8px; height: 8px; animation-delay: 0.8s; animation-duration: 4.2s; }
.wm-bubble.b3 { left: 65%; width: 5px; height: 5px; animation-delay: 1.5s; animation-duration: 3.8s; }
.wm-bubble.b4 { left: 80%; width: 7px; height: 7px; animation-delay: 2.2s; animation-duration: 4.5s; }
.wm-bubble.b5 { left: 30%; width: 9px; height: 9px; animation-delay: 2.9s; animation-duration: 3.2s; }

@keyframes float-bubble {
  0%   { transform: translateY(0) scale(0.6); opacity: 0; }
  20%  { opacity: 0.7; }
  80%  { opacity: 0.7; }
  100% { transform: translateY(-380px) scale(1.2); opacity: 0; }
}

/* Base & Drain Valve */
.wm-tank-base {
  width: 250px;
  height: 34px;
  background: linear-gradient(180deg, var(--bg-tertiary), rgba(3, 7, 18, 0.95));
  border: 1px solid rgba(0, 229, 255, 0.25);
  border-top: 2px solid rgba(0, 229, 255, 0.4);
  border-radius: 0 0 10px 10px;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  margin-top: -2px;
  box-shadow: 0 10px 25px rgba(0, 0, 0, 0.5);
  z-index: 2;
}

.wm-base-text {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.6rem;
  letter-spacing: 0.12em;
  color: var(--accent-cyan);
  font-weight: 700;
}

.wm-drain-valve {
  position: absolute;
  bottom: 4px;
  left: 25px;
  width: 14px;
  height: 20px;
  border: 2px solid rgba(0, 229, 255, 0.4);
  background: var(--bg-tertiary);
  border-radius: 0 0 4px 4px;
  z-index: 1;
}

/* LCD Data Readouts */
.wm-data-row {
  display: flex;
  gap: 12px;
  width: 100%;
  max-width: 360px;
}

.wm-data-main {
  flex: 1;
  padding: 18px;
  text-align: center;
}

.wm-data-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 12px;
  width: 100%;
  max-width: 360px;
}

.wm-data-cell {
  padding: 14px;
  text-align: center;
}

.wm-data-label {
  display: block;
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.65rem;
  letter-spacing: 0.1em;
  color: var(--text-dim);
  margin-bottom: 6px;
  text-transform: uppercase;
}

.wm-lcd-display {
  display: flex;
  align-items: baseline;
  justify-content: center;
  gap: 4px;
}

.wm-lcd-value {
  font-family: 'Orbitron', sans-serif;
  font-size: 2.2rem;
  font-weight: 700;
  color: var(--accent-cyan);
  letter-spacing: 0.05em;
  line-height: 1;
}

.wm-lcd-display.small .wm-lcd-value {
  font-size: 1.35rem;
}

.wm-lcd-unit {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.85rem;
  color: var(--text-secondary);
}

.wm-lcd-display.small .wm-lcd-unit {
  font-size: 0.7rem;
}

/* Pump Card & Graph Card */
.wm-pump-card, .wm-graph-card {
  padding: 20px;
}

.wm-pump-header, .wm-graph-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 14px;
}

.wm-pump-title-row, .wm-graph-title-row {
  display: flex;
  align-items: center;
  gap: 10px;
}

.wm-pump-title-row h3, .wm-graph-title-row h3 {
  font-size: 0.95rem;
  font-weight: 700;
  margin: 0;
}

.wm-pump-mode-label, .wm-graph-subtitle {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.68rem;
  color: var(--text-dim);
}

.wm-pump-mode-tabs {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 0;
  border: 1px solid var(--glass-border);
  border-radius: var(--radius-sm);
  overflow: hidden;
  margin-bottom: 16px;
}

.wm-pump-tab {
  padding: 10px;
  border: none;
  background: transparent;
  color: var(--text-secondary);
  font-family: 'Outfit', sans-serif;
  font-weight: 700;
  font-size: 0.8rem;
  cursor: pointer;
  transition: all 0.25s;
}

.wm-pump-tab.active {
  background: rgba(0, 229, 255, 0.12);
  color: var(--accent-cyan);
  box-shadow: inset 0 0 15px rgba(0, 229, 255, 0.08);
}

.wm-pump-status-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 0;
  border-top: 1px solid var(--glass-border);
}

.wm-pump-motor-label {
  font-size: 0.85rem;
  color: var(--text-primary);
  font-weight: 600;
}

.wm-pump-motor-state {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.8rem;
  font-weight: 700;
  margin-left: 8px;
  color: var(--danger);
}

.wm-pump-toggle-area {
  display: flex;
  align-items: center;
  gap: 8px;
}

.pump-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--text-dim);
  display: inline-block;
  transition: all 0.3s;
}

.pump-dot.on {
  background: var(--success);
  box-shadow: 0 0 10px var(--success);
}

.wm-pump-manual {
  padding-top: 14px;
  border-top: 1px solid var(--glass-border);
  display: flex;
  justify-content: center;
}

.wm-pump-power-btn {
  width: 72px;
  height: 72px;
  border-radius: 50%;
  border: 2px solid var(--glass-border);
  background: transparent;
  color: var(--text-secondary);
  cursor: pointer;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 4px;
  font-size: 0.62rem;
  font-weight: 700;
  transition: all 0.25s;
}

.wm-pump-power-btn:hover {
  border-color: var(--accent-cyan);
  color: var(--accent-cyan);
}

.wm-pump-power-btn.on {
  border-color: var(--success);
  color: var(--success);
  background: var(--success-dim);
  box-shadow: 0 0 20px rgba(0, 230, 118, 0.35);
}

.wm-pump-hint {
  font-size: 0.72rem;
  color: var(--text-dim);
  text-align: center;
  margin-top: 10px;
  font-style: italic;
}

.wm-chart-wrap {
  position: relative;
  height: 210px;
}

.wm-chart-wrap canvas {
  width: 100%;
  height: 100%;
  display: block;
}

.card-badge {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.65rem;
  padding: 3px 10px;
  border-radius: 4px;
  background: rgba(0, 229, 255, 0.12);
  color: var(--accent-cyan);
  border: 1px solid rgba(0, 229, 255, 0.25);
}

/* ─── 9. SAFETY MONITORING ─── */
.safety-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
  gap: 16px;
}

.sensor-card {
  padding: 22px 20px;
  text-align: center;
  position: relative;
  overflow: hidden;
  transition: all 0.3s;
}

.sensor-card::before {
  content: '';
  position: absolute;
  top: 0; left: 0; right: 0;
  height: 3px;
  background: var(--success);
  transition: all 0.3s;
}

.sensor-card.warning::before { background: var(--warning); }
.sensor-card.danger::before  { background: var(--danger); }

.sensor-card.danger {
  border-color: rgba(255, 82, 82, 0.4);
  box-shadow: 0 0 30px rgba(255, 82, 82, 0.2);
}

.sensor-icon-wrap {
  width: 60px;
  height: 60px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  margin: 0 auto 12px;
  transition: all 0.3s;
}

.ir-accent    { background: rgba(0, 229, 255, 0.1); color: var(--accent-cyan); }
.gas-accent   { background: rgba(179, 136, 255, 0.1); color: var(--accent-purple); }
.flame-accent { background: rgba(255, 171, 64, 0.1); color: var(--warning); }

.sensor-name {
  font-size: 0.95rem;
  font-weight: 700;
  margin-bottom: 8px;
}

.sensor-reading {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  margin-bottom: 12px;
}

.sensor-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  display: inline-block;
}

.sensor-dot.safe    { background: var(--success); box-shadow: 0 0 8px var(--success); }
.sensor-dot.warning { background: var(--warning); box-shadow: 0 0 8px var(--warning); }
.sensor-dot.danger  { background: var(--danger);  box-shadow: 0 0 10px var(--danger); animation: pulse 1s infinite; }

.sensor-value {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.8rem;
  font-weight: 600;
}

.sensor-bar-wrap {
  height: 4px;
  background: var(--bg-tertiary);
  border-radius: 2px;
  overflow: hidden;
}

.sensor-bar {
  height: 100%;
  background: var(--success);
  transition: width 0.5s ease;
}

.sensor-card.warning .sensor-bar { background: var(--warning); }
.sensor-card.danger  .sensor-bar { background: var(--danger); }

/* Safety Modal */
.safety-modal {
  display: none;
  position: fixed;
  inset: 0;
  z-index: 9000;
  align-items: center;
  justify-content: center;
}

.safety-modal.active { display: flex; }

.safety-modal-backdrop {
  position: absolute;
  inset: 0;
  background: rgba(0, 0, 0, 0.85);
  backdrop-filter: blur(8px);
}

.safety-modal-panel {
  position: relative;
  z-index: 1;
  width: 100%;
  max-width: 480px;
  margin: 16px;
  background: var(--bg-secondary);
  border: 1px solid rgba(255, 82, 82, 0.4);
  border-radius: var(--radius-lg);
  box-shadow: 0 0 60px rgba(255, 82, 82, 0.2), 0 32px 64px rgba(0, 0, 0, 0.6);
  overflow: hidden;
}

.safety-modal-header {
  display: flex;
  align-items: center;
  gap: 20px;
  padding: 24px;
  border-bottom: 1px solid rgba(255, 82, 82, 0.15);
}

.safety-modal-icon-wrap {
  position: relative;
  width: 60px;
  height: 60px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.safety-modal-icon { color: var(--danger); }

.safety-modal-pulse-ring {
  position: absolute;
  inset: 0;
  border-radius: 50%;
  border: 2px solid var(--danger);
  animation: sm-ring-pulse 2s ease-out infinite;
}

.safety-modal-title {
  font-family: 'Outfit', sans-serif;
  font-size: 1.2rem;
  font-weight: 800;
  color: var(--danger);
  margin: 0 0 4px;
}

.safety-modal-subtitle {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.7rem;
  color: var(--text-secondary);
  margin: 0;
}

.safety-modal-body {
  padding: 20px 24px;
  display: flex;
  flex-direction: column;
  gap: 14px;
}

.safety-modal-reason {
  background: rgba(255, 82, 82, 0.07);
  border: 1px solid rgba(255, 82, 82, 0.2);
  border-radius: var(--radius-sm);
  padding: 12px 14px;
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.safety-modal-reason-label {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.6rem;
  color: var(--danger);
  text-transform: uppercase;
}

.safety-modal-reason-text {
  font-size: 0.9rem;
  font-weight: 600;
  color: var(--text-primary);
}

.safety-modal-sensor-row {
  display: flex;
  gap: 8px;
}

.safety-modal-sensor-item {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 4px;
  padding: 10px 6px;
  background: var(--bg-tertiary);
  border-radius: var(--radius-sm);
  border: 1px solid var(--glass-border);
  font-size: 0.7rem;
  color: var(--text-secondary);
  font-family: 'JetBrains Mono', monospace;
}

.sms-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--success);
}

.sms-dot.triggered { background: var(--danger); box-shadow: 0 0 8px var(--danger); }
.sms-dot.warning   { background: var(--warning); box-shadow: 0 0 8px var(--warning); }

.sms-val { font-weight: 700; font-size: 0.62rem; color: var(--success); }
.sms-val.triggered { color: var(--danger); }
.sms-val.warning   { color: var(--warning); }

.safety-modal-warning-note {
  display: flex;
  align-items: flex-start;
  gap: 8px;
  padding: 10px 12px;
  background: rgba(255, 171, 64, 0.07);
  border: 1px solid rgba(255, 171, 64, 0.2);
  border-radius: var(--radius-sm);
  font-size: 0.7rem;
  color: var(--warning);
}

.safety-modal-actions {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 10px;
  padding: 0 24px 24px;
}

.safety-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  padding: 12px 14px;
  border-radius: var(--radius-sm);
  font-family: 'Outfit', sans-serif;
  font-weight: 700;
  font-size: 0.78rem;
  cursor: pointer;
  border: none;
  transition: all 0.2s;
}

.reject-btn {
  background: rgba(255, 82, 82, 0.12);
  border: 1px solid rgba(255, 82, 82, 0.4);
  color: var(--danger);
}

.accept-btn {
  background: linear-gradient(135deg, rgba(105, 240, 174, 0.2), rgba(0, 229, 255, 0.15));
  border: 1px solid rgba(105, 240, 174, 0.4);
  color: var(--success);
}

/* Danger Banner */
.danger-banner {
  position: fixed;
  top: 0; left: 0; right: 0;
  z-index: 8500;
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 10px 20px;
  background: linear-gradient(90deg, rgba(255, 82, 82, 0.18), rgba(255, 82, 82, 0.12));
  border-bottom: 1px solid rgba(255, 82, 82, 0.4);
  backdrop-filter: blur(12px);
  color: var(--danger);
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.78rem;
  font-weight: 600;
}

.danger-banner-close {
  margin-left: auto;
  background: none;
  border: none;
  color: var(--danger);
  font-size: 1rem;
  cursor: pointer;
}

/* Gate Blocked Overlay */
.gate-blocked-overlay {
  position: absolute;
  inset: 0;
  background: rgba(255, 82, 82, 0.12);
  border-radius: inherit;
  display: none;
  align-items: center;
  justify-content: center;
  z-index: 10;
  backdrop-filter: blur(2px);
}

.gate-blocked-overlay.visible { display: flex; }

.gate-blocked-label {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.75rem;
  font-weight: 700;
  color: var(--danger);
  letter-spacing: 0.15em;
  text-transform: uppercase;
  border: 1px solid rgba(255, 82, 82, 0.5);
  padding: 6px 14px;
  border-radius: 4px;
  background: rgba(255, 82, 82, 0.1);
}

.sensor-state-label {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.65rem;
  font-weight: 700;
  padding: 3px 10px;
  border-radius: 4px;
  margin-top: 8px;
  display: inline-block;
  background: var(--success-dim);
  color: var(--success);
}

.sensor-card.warning .sensor-state-label { background: var(--warning-dim); color: var(--warning); }
.sensor-card.danger .sensor-state-label  { background: var(--danger-dim);  color: var(--danger); }

.safety-demo-row {
  display: flex;
  gap: 8px;
  margin-top: 24px;
  flex-wrap: wrap;
  justify-content: center;
}

.safety-demo-btn {
  padding: 6px 12px;
  border-radius: 6px;
  border: 1px solid var(--glass-border);
  background: var(--glass-bg);
  color: var(--text-secondary);
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.68rem;
  cursor: pointer;
  transition: all 0.25s;
}

.safety-demo-btn:hover {
  border-color: var(--accent-cyan);
  color: var(--accent-cyan);
}

.safety-demo-btn.ir    { border-color: rgba(0,229,255,0.3); }
.safety-demo-btn.gas   { border-color: rgba(179,136,255,0.3); color: var(--accent-purple); }
.safety-demo-btn.flame { border-color: rgba(255,82,82,0.3);   color: var(--danger); }

/* ─── 10. SETTINGS & TIMELINE & ALERT HISTORY ─── */
.settings-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(260px, 1fr));
  gap: 16px;
}

.setting-card {
  padding: 18px;
}

.setting-label {
  display: flex;
  align-items: center;
  gap: 8px;
  font-size: 0.8rem;
  color: var(--text-secondary);
  margin-bottom: 10px;
  font-weight: 600;
}

.setting-input {
  width: 100%;
  padding: 10px 14px;
  background: var(--bg-secondary);
  border: 1px solid var(--glass-border);
  border-radius: var(--radius-sm);
  color: var(--text-primary);
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.9rem;
  outline: none;
  transition: all 0.25s;
}

.setting-input:focus {
  border-color: var(--accent-cyan);
  box-shadow: 0 0 0 3px rgba(0, 229, 255, 0.15);
}

.setting-actions {
  grid-column: 1 / -1;
  display: flex;
  justify-content: flex-end;
}

/* Timeline & Alert History Grid */
.timeline-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 24px;
}

.timeline-card, .alert-history-card {
  padding: 20px;
  height: 400px;
  overflow-y: auto;
}

.activity-timeline {
  position: relative;
  padding-left: 20px;
}

.activity-timeline::before {
  content: '';
  position: absolute;
  left: 7px;
  top: 0;
  bottom: 0;
  width: 2px;
  background: var(--glass-border);
}

.timeline-item {
  position: relative;
  padding: 10px 0 10px 16px;
  border-bottom: 1px solid rgba(255, 255, 255, 0.04);
}

.timeline-item:last-child {
  border-bottom: none;
}

.timeline-dot {
  position: absolute;
  left: -18px;
  top: 14px;
  width: 10px;
  height: 10px;
  border-radius: 50%;
  background: var(--accent-blue);
  border: 2px solid var(--bg-primary);
  z-index: 1;
}

.timeline-dot.light { background: var(--accent-cyan); }
.timeline-dot.gate  { background: var(--accent-blue); }
.timeline-dot.servo { background: var(--accent-purple); }
.timeline-dot.water { background: var(--accent-cyan); }
.timeline-dot.pump  { background: var(--warning); }
.timeline-dot.sensor{ background: var(--success); }
.timeline-dot.danger-dot{ background: var(--danger); }

.timeline-time {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.68rem;
  color: var(--text-dim);
  display: block;
  margin-bottom: 2px;
}

.timeline-text {
  font-size: 0.82rem;
  color: var(--text-primary);
}

/* Alert History Panel Styles */
.alert-history-card {
  display: flex;
  flex-direction: column;
}

.alert-history-filters {
  display: flex;
  gap: 6px;
  margin-bottom: 14px;
  overflow-x: auto;
  padding-bottom: 4px;
}

.alert-filter-btn {
  padding: 4px 10px;
  border-radius: 4px;
  border: 1px solid var(--glass-border);
  background: transparent;
  color: var(--text-secondary);
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.68rem;
  font-weight: 700;
  cursor: pointer;
  transition: all 0.2s;
  white-space: nowrap;
}

.alert-filter-btn.active, .alert-filter-btn:hover {
  border-color: var(--accent-cyan);
  color: var(--accent-cyan);
  background: rgba(0, 229, 255, 0.08);
}

.alert-history-list {
  overflow-y: auto;
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.alert-history-empty {
  text-align: center;
  color: var(--text-dim);
  font-size: 0.8rem;
  padding: 40px 0;
  font-family: 'JetBrains Mono', monospace;
}

.alert-history-item {
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 10px 12px;
  background: rgba(255, 255, 255, 0.03);
  border: 1px solid var(--glass-border);
  border-radius: var(--radius-sm);
  transition: all 0.2s;
}

.alert-history-item:hover {
  border-color: var(--glass-border-hover);
  background: rgba(255, 255, 255, 0.05);
}

.ahi-icon {
  font-size: 1.2rem;
}

.ahi-content {
  flex: 1;
}

.ahi-top {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.ahi-title {
  font-family: 'Outfit', sans-serif;
  font-size: 0.8rem;
  font-weight: 700;
  color: var(--text-primary);
}

.ahi-time {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.65rem;
  color: var(--text-dim);
}

.ahi-msg {
  font-size: 0.72rem;
  color: var(--text-secondary);
  display: block;
}

.ahi-badge {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.62rem;
  font-weight: 700;
  padding: 2px 6px;
  border-radius: 4px;
}

.ahi-badge.info      { background: rgba(0, 229, 255, 0.12); color: var(--accent-cyan); }
.ahi-badge.warning   { background: var(--warning-dim); color: var(--warning); }
.ahi-badge.danger    { background: var(--danger-dim);  color: var(--danger); }
.ahi-badge.emergency { background: rgba(255, 82, 82, 0.25); color: var(--danger); border: 1px solid var(--danger); }

/* ─── 11. TOAST NOTIFICATION STACK (TOP-RIGHT) ─── */
.toast-container {
  position: fixed;
  top: 20px;
  right: 20px;
  z-index: 10000;
  display: flex;
  flex-direction: column;
  gap: 12px;
  max-width: 380px;
  width: calc(100% - 40px);
  pointer-events: none;
}

.toast-card {
  pointer-events: auto;
  display: flex;
  align-items: flex-start;
  gap: 12px;
  padding: 14px 16px;
  background: rgba(11, 19, 41, 0.95);
  backdrop-filter: blur(20px);
  border: 1px solid var(--glass-border);
  border-radius: var(--radius-md);
  box-shadow: 0 12px 40px rgba(0, 0, 0, 0.6), inset 0 1px 0 rgba(255, 255, 255, 0.08);
  animation: slide-in-right 0.35s cubic-bezier(0.34, 1.56, 0.64, 1);
  transition: all 0.3s ease;
  position: relative;
  overflow: hidden;
}

.toast-card.hide {
  opacity: 0;
  transform: translateX(120%);
}

.toast-card.toast-info {
  border-color: rgba(0, 229, 255, 0.4);
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5), 0 0 15px rgba(0, 229, 255, 0.15);
}

.toast-card.toast-warning {
  border-color: rgba(255, 171, 64, 0.5);
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5), 0 0 20px rgba(255, 171, 64, 0.2);
}

.toast-card.toast-danger {
  border-color: rgba(255, 82, 82, 0.6);
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5), 0 0 25px rgba(255, 82, 82, 0.25);
  background: linear-gradient(135deg, rgba(255, 82, 82, 0.12), rgba(11, 19, 41, 0.95));
}

.toast-card.toast-emergency {
  border-color: var(--danger);
  box-shadow: 0 12px 40px rgba(255, 82, 82, 0.4);
  background: linear-gradient(135deg, rgba(255, 82, 82, 0.22), rgba(11, 19, 41, 0.95));
  animation: slide-in-right 0.35s cubic-bezier(0.34, 1.56, 0.64, 1), toast-pulse-border 1.5s infinite alternate;
}

@keyframes slide-in-right {
  from { opacity: 0; transform: translateX(120%); }
  to   { opacity: 1; transform: translateX(0); }
}

@keyframes toast-pulse-border {
  from { border-color: rgba(255, 82, 82, 0.4); }
  to   { border-color: rgba(255, 82, 82, 1); box-shadow: 0 0 35px rgba(255, 82, 82, 0.5); }
}

.toast-card-icon {
  font-size: 1.3rem;
  line-height: 1;
  flex-shrink: 0;
  margin-top: 2px;
}

.toast-card-content {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.toast-card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 8px;
}

.toast-card-title {
  font-family: 'Outfit', sans-serif;
  font-weight: 800;
  font-size: 0.85rem;
  color: var(--text-primary);
}

.toast-card-time {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.65rem;
  color: var(--text-dim);
}

.toast-card-msg {
  font-size: 0.78rem;
  color: var(--text-secondary);
  line-height: 1.35;
}

.toast-card-actions {
  display: flex;
  gap: 8px;
  margin-top: 8px;
}

.toast-action-btn {
  padding: 4px 10px;
  border-radius: 4px;
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.68rem;
  font-weight: 700;
  cursor: pointer;
  border: 1px solid var(--glass-border);
  background: transparent;
  transition: all 0.2s;
}

.toast-action-btn.accept { color: var(--success); border-color: rgba(0, 230, 118, 0.4); }
.toast-action-btn.accept:hover { background: var(--success-dim); }
.toast-action-btn.reject { color: var(--danger); border-color: rgba(255, 82, 82, 0.4); }
.toast-action-btn.reject:hover { background: var(--danger-dim); }

.toast-card-close {
  background: none;
  border: none;
  color: var(--text-dim);
  font-size: 0.9rem;
  cursor: pointer;
  padding: 0 4px;
  line-height: 1;
  transition: color 0.2s;
}

.toast-card-close:hover {
  color: var(--text-primary);
}

/* Flashing Alert Cards */
.flash-red {
  animation: card-flash-red 0.6s infinite alternate !important;
}

@keyframes card-flash-red {
  from { border-color: rgba(255, 82, 82, 0.3); box-shadow: 0 0 10px rgba(255, 82, 82, 0.2); }
  to   { border-color: rgba(255, 82, 82, 1); box-shadow: 0 0 35px rgba(255, 82, 82, 0.6); }
}

/* ─── 12. FOOTER & LEGACY TOAST ─── */
.dashboard-footer {
  padding: 24px;
  text-align: center;
  border-top: 1px solid var(--glass-border);
  margin-top: 24px;
}

.footer-inner {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 8px;
  font-size: 0.78rem;
  color: var(--text-secondary);
}

.footer-inner a {
  color: var(--accent-cyan);
  text-decoration: none;
}

.toast {
  position: fixed;
  bottom: 28px;
  left: 50%;
  transform: translateX(-50%) translateY(100px);
  background: var(--bg-secondary);
  border: 1px solid var(--glass-border);
  box-shadow: 0 10px 40px rgba(0, 0, 0, 0.6);
  border-radius: var(--radius-md);
  padding: 12px 24px;
  color: var(--text-primary);
  font-size: 0.85rem;
  font-weight: 600;
  z-index: 10000;
  opacity: 0;
  transition: all 0.35s cubic-bezier(0.34, 1.56, 0.64, 1);
  pointer-events: none;
}

.toast.show {
  transform: translateX(-50%) translateY(0);
  opacity: 1;
}

.scroll-reveal {
  opacity: 0;
  transform: translateY(20px);
  transition: opacity 0.6s ease, transform 0.6s ease;
}

.scroll-reveal.revealed {
  opacity: 1;
  transform: translateY(0);
}

/* ─── 13. RESPONSIVE BREAKPOINTS ─── */
@media (max-width: 1024px) {
  .wm-layout, .servo-layout, .timeline-grid {
    grid-template-columns: 1fr;
  }
  .wm-ruler-scale {
    display: none;
  }
}

@media (max-width: 768px) {
  .nav-links { display: none; }
  .nav-hamburger { display: block; }
  .lights-grid { grid-template-columns: repeat(auto-fill, minmax(160px, 1fr)); }
  .gate-btn-row, .gate-hold-row { flex-wrap: wrap; }
  .wm-tank-body { height: 350px; }
  .section-header h2 { font-size: 1.25rem; }
  .dash-section { padding-top: 56px; margin-bottom: 24px; }
  .toast-container { right: 10px; left: 10px; max-width: none; width: auto; }
}

@media (max-width: 480px) {
  .lights-grid { grid-template-columns: 1fr; }
  .wm-data-grid { grid-template-columns: 1fr; }
  .nav-center { display: none; }
}

)rawliteral";

const char APP_JS[] PROGMEM = R"rawliteral(
'use strict';

const ActivityTimeline = (function() {
    let events = [];
    const maxEvents = 50;
    const timelineEl = document.getElementById('activity-timeline');

    function formatTime(date) {
        return date.toLocaleTimeString('en-US', { hour12: false });
    }

    function addEvent(text, type) {
        const time = new Date();
        events.unshift({ time, text, type });
        if (events.length > maxEvents) events.pop();
        
        if (timelineEl) {
            const item = document.createElement('div');
            item.className = 'timeline-item';
            item.innerHTML = `
                <div class="timeline-dot ${type}"></div>
                <span class="timeline-time">${formatTime(time)}</span>
                <span class="timeline-text">${text}</span>
            `;
            timelineEl.insertBefore(item, timelineEl.firstChild);
            while (timelineEl.children.length > maxEvents) {
                timelineEl.removeChild(timelineEl.lastChild);
            }
        }
    }

    function render() {
        if (!timelineEl) return;
        timelineEl.innerHTML = '';
        events.forEach(ev => {
            const item = document.createElement('div');
            item.className = 'timeline-item';
            item.innerHTML = `
                <div class="timeline-dot ${ev.type}"></div>
                <span class="timeline-time">${formatTime(ev.time)}</span>
                <span class="timeline-text">${ev.text}</span>
            `;
            timelineEl.appendChild(item);
        });
    }

    function init() {
        const now = Date.now();
        const demoEvents = [
            { text: 'System initialized (Demo Mode)', type: 'sensor', offset: 30 * 60000 },
            { text: 'Water pump turned ON (Auto)', type: 'pump', offset: 25 * 60000 },
            { text: 'Water level reached 85%', type: 'water', offset: 20 * 60000 },
            { text: 'Water pump turned OFF (Auto)', type: 'pump', offset: 19 * 60000 },
            { text: 'Gate opened', type: 'gate', offset: 15 * 60000 },
            { text: 'Living Room light turned ON', type: 'light', offset: 10 * 60000 },
            { text: 'Gate closed', type: 'gate', offset: 8 * 60000 },
            { text: 'IR Sensor: No Motion', type: 'sensor', offset: 5 * 60000 }
        ];

        demoEvents.forEach(ev => {
            events.push({
                time: new Date(now - ev.offset),
                text: ev.text,
                type: ev.type
            });
        });
        
        events.sort((a, b) => b.time - a.time);
        render();
    }

    return { init, addEvent };
})();

const Toast = (function() {
    const toastEl = document.getElementById('toast');
    let timeout;
    function showToast(message, duration = 3000) {
        if (!toastEl) return;
        toastEl.textContent = message;
        toastEl.classList.add('show');
        clearTimeout(timeout);
        timeout = setTimeout(() => {
            toastEl.classList.remove('show');
        }, duration);
    }
    return { showToast };
})();

const Settings = (function() {
    let state = {
        dashName: 'Smart Home',
        tankDepth: 150,
        sensorOffset: 5,
        lowThreshold: 20,
        highThreshold: 90,
        soundMuted: false
    };

    function updateBrandUI(name) {
        if (!name) return;
        state.dashName = name;
        const navTitle = document.getElementById('nav-brand-title');
        const bootTitle = document.getElementById('boot-title');
        const footerTitle = document.getElementById('footer-brand-title');
        if (navTitle) navTitle.textContent = name.toUpperCase();
        if (bootTitle) bootTitle.textContent = name.toUpperCase();
        if (footerTitle) footerTitle.textContent = `${name.toUpperCase()} v3.0`;
        document.title = `${name} — Control Center`;
    }

    function load() {
        const saved = localStorage.getItem('nexus-settings');
        if (saved) {
            try {
                Object.assign(state, JSON.parse(saved));
            } catch (e) {}
        }
        const nameEl = document.getElementById('setting-dash-name');
        const depthEl = document.getElementById('setting-tank-depth');
        const offsetEl = document.getElementById('setting-sensor-offset');
        const lowEl = document.getElementById('setting-low-threshold');
        const highEl = document.getElementById('setting-high-threshold');
        const soundEl = document.getElementById('setting-sound-toggle');
        const soundText = document.getElementById('sound-status-text');

        if (nameEl) nameEl.value = state.dashName || 'Smart Home';
        if (depthEl) depthEl.value = state.tankDepth;
        if (offsetEl) offsetEl.value = state.sensorOffset;
        if (lowEl) lowEl.value = state.lowThreshold;
        if (highEl) highEl.value = state.highThreshold;
        if (soundEl) {
            soundEl.checked = !state.soundMuted;
            if (soundText) {
                soundText.textContent = state.soundMuted ? 'MUTED' : 'ENABLED';
                soundText.style.color = state.soundMuted ? 'var(--danger)' : 'var(--success)';
            }
        }
        updateBrandUI(state.dashName);
    }

    function save() {
        const nameEl = document.getElementById('setting-dash-name');
        const depth = parseInt(document.getElementById('setting-tank-depth').value);
        const offset = parseInt(document.getElementById('setting-sensor-offset').value);
        const low = parseInt(document.getElementById('setting-low-threshold').value);
        const high = parseInt(document.getElementById('setting-high-threshold').value);
        const soundEl = document.getElementById('setting-sound-toggle');

        if (isNaN(depth) || isNaN(offset) || isNaN(low) || isNaN(high)) {
            Toast.showToast('Invalid settings: values must be valid numbers');
            return;
        }
        if (depth <= 0 || depth > 1000) {
            Toast.showToast('Invalid depth: must be between 1 and 1000 cm');
            return;
        }
        if (low < 0 || high > 100 || low >= high) {
            Toast.showToast('Invalid thresholds: Low must be less than High (0-100%)');
            return;
        }

        if (nameEl && nameEl.value.trim()) {
            state.dashName = nameEl.value.trim();
            updateBrandUI(state.dashName);
        }
        state.tankDepth = depth;
        state.sensorOffset = offset;
        state.lowThreshold = low;
        state.highThreshold = high;
        if (soundEl) state.soundMuted = !soundEl.checked;

        localStorage.setItem('nexus-settings', JSON.stringify(state));
        Toast.showToast('Settings saved successfully');
        
        const soundText = document.getElementById('sound-status-text');
        if (soundText) {
            soundText.textContent = state.soundMuted ? 'MUTED' : 'ENABLED';
            soundText.style.color = state.soundMuted ? 'var(--danger)' : 'var(--success)';
        }
        
        if (window.WaterTank) {
            window.WaterTank.updateFromSettings();
        }

        if (window.ESP32WS) {
            window.ESP32WS.send({
                type: 'settings',
                dashName: state.dashName,
                tankDepth: state.tankDepth,
                sensorOffset: state.sensorOffset,
                lowThreshold: state.lowThreshold,
                highThreshold: state.highThreshold
            });
        }
    }

    function applyServerSettings(srv) {
        if (!srv) return;
        if (srv.dashName) {
            state.dashName = srv.dashName;
            const nameEl = document.getElementById('setting-dash-name');
            if (nameEl) nameEl.value = srv.dashName;
            updateBrandUI(srv.dashName);
        }
        if (typeof srv.tankDepth === 'number') {
            state.tankDepth = srv.tankDepth;
            const depthEl = document.getElementById('setting-tank-depth');
            if (depthEl) depthEl.value = srv.tankDepth;
        }
        if (typeof srv.sensorOffset === 'number') {
            state.sensorOffset = srv.sensorOffset;
            const offsetEl = document.getElementById('setting-sensor-offset');
            if (offsetEl) offsetEl.value = srv.sensorOffset;
        }
        if (typeof srv.lowThreshold === 'number') {
            state.lowThreshold = srv.lowThreshold;
            const lowEl = document.getElementById('setting-low-threshold');
            if (lowEl) lowEl.value = srv.lowThreshold;
        }
        if (typeof srv.highThreshold === 'number') {
            state.highThreshold = srv.highThreshold;
            const highEl = document.getElementById('setting-high-threshold');
            if (highEl) highEl.value = srv.highThreshold;
        }
        if (window.WaterTank) window.WaterTank.updateFromSettings();
    }

    function init() {
        load();
        const saveBtn = document.getElementById('settings-save-btn');
        if (saveBtn) {
            saveBtn.addEventListener('click', save);
        }

        const soundEl = document.getElementById('setting-sound-toggle');
        if (soundEl) {
            soundEl.addEventListener('change', () => {
                state.soundMuted = !soundEl.checked;
                const soundText = document.getElementById('sound-status-text');
                if (soundText) {
                    soundText.textContent = state.soundMuted ? 'MUTED' : 'ENABLED';
                    soundText.style.color = state.soundMuted ? 'var(--danger)' : 'var(--success)';
                }
            });
        }
    }

    return { init, getState: () => state, applyServerSettings };
})();

const Navigation = (function() {
    function init() {
        const clockEl = document.getElementById('nav-clock');
        if (clockEl) {
            setInterval(() => {
                clockEl.textContent = new Date().toLocaleTimeString('en-US', { hour12: false });
            }, 1000);
        }

        const navLinks = document.querySelectorAll('.nav-link');
        const sections = document.querySelectorAll('.dash-section');
        
        navLinks.forEach(link => {
            link.addEventListener('click', e => {
                e.preventDefault();
                const targetId = link.getAttribute('href').substring(1);
                const targetSec = document.getElementById(targetId);
                if (targetSec) {
                    const offset = 80;
                    const top = targetSec.getBoundingClientRect().top + window.pageYOffset - offset;
                    window.scrollTo({ top, behavior: 'smooth' });
                }
                const mobileMenu = document.getElementById('nav-mobile-menu');
                const hamburger = document.getElementById('nav-hamburger');
                if (mobileMenu && hamburger) {
                    mobileMenu.classList.remove('open');
                    hamburger.classList.remove('open');
                }
            });
        });

        const navStatus = document.querySelector('.nav-status');
        const statusDot = document.getElementById('system-status-dot');
        const statusText = document.getElementById('system-status-text');
        
        let isOnline = true;
        if (navStatus && statusDot && statusText) {
            navStatus.style.cursor = 'pointer';
            navStatus.setAttribute('title', 'Click to toggle ESP32 status mode');
            navStatus.addEventListener('click', () => {
                isOnline = !isOnline;
                if (isOnline) {
                    statusDot.className = 'status-dot online';
                    statusText.textContent = 'ONLINE';
                    Toast.showToast('ESP32 System Status: ONLINE (Demo Mode)');
                    ActivityTimeline.addEvent('ESP32 Status changed: ONLINE', 'sensor');
                } else {
                    statusDot.className = 'status-dot offline';
                    statusText.textContent = 'ESP32 OFFLINE';
                    Toast.showToast('ESP32 Disconnected — Hardware offline');
                    ActivityTimeline.addEvent('ESP32 Status changed: DISCONNECTED', 'danger-dot');
                }
            });
        }

        const hamburger = document.getElementById('nav-hamburger');
        const mobileMenu = document.getElementById('nav-mobile-menu');
        const navLinksContainer = document.getElementById('nav-links');
        
        if (hamburger && mobileMenu && navLinksContainer) {
            hamburger.addEventListener('click', () => {
                hamburger.classList.toggle('open');
                mobileMenu.classList.toggle('open');
                if (mobileMenu.classList.contains('open')) {
                    mobileMenu.innerHTML = navLinksContainer.innerHTML;
                    mobileMenu.querySelectorAll('.nav-link').forEach(link => {
                        link.addEventListener('click', e => {
                            e.preventDefault();
                            const targetId = link.getAttribute('href').substring(1);
                            const targetSec = document.getElementById(targetId);
                            if (targetSec) {
                                const offset = 80;
                                const top = targetSec.getBoundingClientRect().top + window.pageYOffset - offset;
                                window.scrollTo({ top, behavior: 'smooth' });
                            }
                            hamburger.classList.remove('open');
                            mobileMenu.classList.remove('open');
                        });
                    });
                }
            });
        }

        const observer = new IntersectionObserver((entries) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    const id = entry.target.getAttribute('id');
                    document.querySelectorAll('.nav-link').forEach(l => l.classList.remove('active'));
                    document.querySelectorAll(`.nav-link[data-section="${id}"]`).forEach(l => l.classList.add('active'));
                }
            });
        }, { threshold: 0.2, rootMargin: '-80px 0px -20% 0px' });

        sections.forEach(sec => observer.observe(sec));
    }
    return { init };
})();

const ScrollAnimations = (function() {
    function init() {
        const observer = new IntersectionObserver((entries, obs) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    entry.target.classList.add('revealed');
                    obs.unobserve(entry.target);
                }
            });
        }, { threshold: 0.1, rootMargin: '-50px' });

        const reveals = document.querySelectorAll('.scroll-reveal');
        
        const groups = {};
        reveals.forEach(el => {
            const parent = el.parentElement;
            if (!parent) return;
            if (!groups[parent]) groups[parent] = [];
            groups[parent].push(el);
        });
        
        Object.values(groups).forEach(group => {
            if (group.length > 1) {
                group.forEach((el, index) => {
                    el.style.transitionDelay = `${index * 0.1}s`;
                });
            }
        });

        reveals.forEach(el => observer.observe(el));
    }
    return { init };
})();

const LightControl = (function() {
    const lights = [
        { id: 1, name: 'Living Room', watts: 14, on: false },
        { id: 2, name: 'Kitchen', watts: 18, on: false },
        { id: 3, name: 'Bedroom', watts: 10, on: false },
        { id: 4, name: 'Bathroom', watts: 8, on: false },
        { id: 5, name: 'Garden', watts: 24, on: false }
    ];

    function loadSavedNames() {
        try {
            const saved = localStorage.getItem('nexus_light_names');
            if (saved) {
                const arr = JSON.parse(saved);
                if (Array.isArray(arr)) {
                    arr.forEach((n, idx) => {
                        if (lights[idx] && n) lights[idx].name = n;
                    });
                }
            }
        } catch (e) {}
    }

    function saveNames() {
        try {
            const arr = lights.map(l => l.name);
            localStorage.setItem('nexus_light_names', JSON.stringify(arr));
        } catch (e) {}
    }

    function updateNavIndicator() {
        const onCount = lights.filter(l => l.on).length;
        const ind = document.getElementById('nav-light-indicator');
        if (ind) {
            ind.textContent = `${onCount}/${lights.length}`;
            if (onCount > 0) ind.classList.add('active');
            else ind.classList.remove('active');
        }
    }

    function init() {
        loadSavedNames();
        const grid = document.getElementById('lights-grid');
        if (!grid) return;
        grid.innerHTML = '';
        lights.forEach((light, i) => {
            const card = document.createElement('div');
            card.className = 'light-card glass-card scroll-reveal';
            card.setAttribute('data-light-id', light.id);
            card.style.transitionDelay = `${i * 0.08}s`;
            
            card.innerHTML = `
                <div class="light-card-header">
                    <span class="light-conn-badge">● RELAY ${light.id}</span>
                    <span class="light-power-val">${light.on ? light.watts + ' W' : '0 W'}</span>
                </div>
                <div class="light-icon">
                    <svg width="36" height="36" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M9 18h6M10 22h4M12 2a7 7 0 0 1 7 7c0 2.38-1.19 4.47-3 5.74V17a1 1 0 0 1-1 1h-6a1 1 0 0 1-1-1v-2.26C6.19 13.47 5 11.38 5 9a7 7 0 0 1 7-7z"/>
                    </svg>
                </div>
                <div class="light-info">
                    <div class="light-name-row">
                        <span class="light-name">${light.name}</span>
                        <button class="edit-light-name-btn" title="Rename Switch ✏️">✏️</button>
                    </div>
                    <span class="light-status">${light.on ? 'ON' : 'OFF'}</span>
                </div>
                <label class="toggle-wrap">
                    <input type="checkbox" class="toggle-input" ${light.on ? 'checked' : ''}>
                    <span class="toggle-slider"></span>
                </label>
            `;

            const input = card.querySelector('.toggle-input');
            const powerVal = card.querySelector('.light-power-val');
            const statusSpan = card.querySelector('.light-status');
            const editBtn = card.querySelector('.edit-light-name-btn');
            const nameSpan = card.querySelector('.light-name');

            if (editBtn) {
                editBtn.addEventListener('click', (e) => {
                    e.stopPropagation();
                    const newName = prompt(`Enter new name for Switch #${light.id}:`, light.name);
                    if (newName !== null && newName.trim().length > 0) {
                        light.name = newName.trim();
                        nameSpan.textContent = light.name;
                        saveNames();
                        ActivityTimeline.addEvent(`Switch #${light.id} renamed to "${light.name}"`, 'light');
                    }
                });
            }

            function applyState(isOn, sendWs = true) {
                light.on = isOn;
                input.checked = isOn;
                if (isOn) {
                    card.classList.add('on');
                    statusSpan.textContent = 'ON';
                    if (powerVal) powerVal.textContent = `${light.watts} W`;
                } else {
                    card.classList.remove('on');
                    statusSpan.textContent = 'OFF';
                    if (powerVal) powerVal.textContent = '0 W';
                }
                updateNavIndicator();
                if (sendWs && window.ESP32WS) {
                    window.ESP32WS.send({ type: 'light', action: 'light', id: light.id - 1, index: light.id - 1, state: isOn, on: isOn });
                }
            }

            input.addEventListener('change', (e) => {
                applyState(e.target.checked);
                ActivityTimeline.addEvent(`${light.name} (Relay ${light.id}) turned ${light.on ? 'ON' : 'OFF'}`, 'light');
            });

            card.addEventListener('click', (e) => {
                if (e.target.closest('.toggle-wrap') || e.target.closest('.edit-light-name-btn')) return;
                applyState(!light.on);
                ActivityTimeline.addEvent(`${light.name} (Relay ${light.id}) turned ${light.on ? 'ON' : 'OFF'}`, 'light');
            });

            grid.appendChild(card);
            setTimeout(() => card.classList.add('revealed'), 50 + i * 80);
        });
        updateNavIndicator();
    }

    function applyServerState(serverLights) {
        if (!Array.isArray(serverLights)) return;
        serverLights.forEach((isOn, idx) => {
            if (lights[idx] && lights[idx].on !== isOn) {
                lights[idx].on = isOn;
                const card = document.querySelector(`.light-card[data-light-id="${lights[idx].id}"]`);
                if (card) {
                    const input = card.querySelector('.toggle-input');
                    const powerVal = card.querySelector('.light-power-val');
                    const statusSpan = card.querySelector('.light-status');
                    if (input) input.checked = isOn;
                    if (isOn) {
                        card.classList.add('on');
                        if (statusSpan) statusSpan.textContent = 'ON';
                        if (powerVal) powerVal.textContent = `${lights[idx].watts} W`;
                    } else {
                        card.classList.remove('on');
                        if (statusSpan) statusSpan.textContent = 'OFF';
                        if (powerVal) powerVal.textContent = '0 W';
                    }
                }
            }
        });
        updateNavIndicator();
    }

    return { init, applyServerState };
})();

const GateControl = (function() {
    let state = {
        position: 0,
        targetPosition: 0,
        isHolding: false,
        holdDirection: 0,
        rafId: null,
        lastTime: 0,
        status: 'CLOSED'
    };

    function updateVisuals() {
        const leftDoor = document.getElementById('gate-door-left');
        const rightDoor = document.getElementById('gate-door-right');
        const posText = document.getElementById('gate-position-text');
        const statusText = document.getElementById('gate-status-text');
        const slider = document.getElementById('gate-slider');
        const navInd = document.getElementById('nav-gate-indicator');

        if (leftDoor) leftDoor.style.width = `${50 - (state.position / 2)}%`;
        if (rightDoor) rightDoor.style.width = `${50 - (state.position / 2)}%`;
        
        if (posText) posText.textContent = `${Math.round(state.position)}%`;
        if (slider && document.activeElement !== slider) slider.value = Math.round(state.position);

        let isIrBlocked = window.SafetyMonitor && window.SafetyMonitor.isIrBlocked();
        let newStatus = '';
        if (isIrBlocked && state.position === 0) newStatus = 'IR BLOCKED — GATE LOCKED';
        else if (state.position === 0) newStatus = 'CLOSED';
        else if (state.position === 100) newStatus = 'OPEN';
        else if (state.targetPosition > state.position || (state.isHolding && state.holdDirection > 0)) newStatus = 'OPENING';
        else if (state.targetPosition < state.position || (state.isHolding && state.holdDirection < 0)) newStatus = 'CLOSING';
        else newStatus = 'PARTIAL';

        if (state.status !== newStatus) {
            state.status = newStatus;
            if (statusText) {
                statusText.textContent = state.status;
                statusText.className = 'gate-stat-value';
                if (state.status.indexOf('IR BLOCKED') >= 0) statusText.classList.add('status-closing');
                else if (state.status === 'OPEN') statusText.classList.add('status-open');
                else if (state.status === 'CLOSED') statusText.classList.add('status-closed');
                else if (state.status === 'OPENING') statusText.classList.add('status-opening');
                else if (state.status === 'CLOSING') statusText.classList.add('status-closing');
            }
            if (navInd) {
                navInd.textContent = (state.status.indexOf('IR BLOCKED') >= 0) ? 'BLOCKED' : state.status;
                if (state.status === 'OPEN') navInd.className = 'nav-badge status-open';
                else if (state.status.indexOf('IR BLOCKED') >= 0) navInd.className = 'nav-badge danger';
                else if (state.status === 'CLOSED') navInd.className = 'nav-badge';
                else navInd.className = 'nav-badge active';
            }
        }
        
        const pillars = document.querySelectorAll('.pillar-light');
        if (state.status === 'OPENING' || state.status === 'CLOSING') {
            pillars.forEach(p => p.style.animationDuration = '0.5s');
        } else {
            pillars.forEach(p => p.style.animationDuration = '2s');
        }
    }

    function loop(timestamp) {
        if (!state.lastTime) state.lastTime = timestamp;
        const dt = timestamp - state.lastTime;
        state.lastTime = timestamp;
        
        let oldPos = state.position;
        let changed = false;

        if (state.isHolding && state.holdDirection !== 0) {
            const move = state.holdDirection * 60 * (dt / 1000);
            state.position = Math.max(0, Math.min(100, state.position + move));
            state.targetPosition = state.position;
        } else if (state.position !== state.targetPosition) {
            const diff = state.targetPosition - state.position;
            if (Math.abs(diff) < 0.5) {
                state.position = state.targetPosition;
            } else {
                const move = Math.sign(diff) * Math.min(Math.abs(diff), 60 * (dt / 1000));
                state.position += move;
            }
        }

        if (state.position !== oldPos) {
            changed = true;
            if (state.position === 100 && oldPos < 100) ActivityTimeline.addEvent('Gate fully opened', 'gate');
            if (state.position === 0 && oldPos > 0) ActivityTimeline.addEvent('Gate fully closed', 'gate');
        }

        if (changed) {
            updateVisuals();
        }
        
        state.rafId = requestAnimationFrame(loop);
    }

    function setTarget(target, sendWs = true) {
        if (window.SafetyMonitor) window.SafetyMonitor.authorizeWebControl();
        state.targetPosition = Math.max(0, Math.min(100, target));
        if (sendWs && window.ESP32WS) {
            if (target === 100) window.ESP32WS.send({ type: 'gate', cmd: 'open', pos: 100 });
            else if (target === 0) window.ESP32WS.send({ type: 'gate', cmd: 'close', pos: 0 });
            else window.ESP32WS.send({ type: 'gate', pos: target });
        }
    }

    function setHolding(direction) {
        if (window.SafetyMonitor) window.SafetyMonitor.authorizeWebControl();
        if (direction !== 0) {
            if (direction > 0 && state.targetPosition < state.position) state.targetPosition = state.position;
            if (direction < 0 && state.targetPosition > state.position) state.targetPosition = state.position;
        }
        state.isHolding = direction !== 0;
        state.holdDirection = direction;
        
        const leftDoor = document.getElementById('gate-door-left');
        const rightDoor = document.getElementById('gate-door-right');
        if (state.isHolding) {
            if (leftDoor) leftDoor.style.transition = 'none';
            if (rightDoor) rightDoor.style.transition = 'none';
        } else {
            if (leftDoor) leftDoor.style.transition = '';
            if (rightDoor) rightDoor.style.transition = '';
        }

        if (window.ESP32WS) {
            if (direction > 0) window.ESP32WS.send({ type: 'gate', cmd: 'hold_open' });
            else if (direction < 0) window.ESP32WS.send({ type: 'gate', cmd: 'hold_close' });
            else window.ESP32WS.send({ type: 'gate', cmd: 'hold_stop' });
        }
    }

    function applyServerState(pos, status) {
        if (typeof pos === 'number' && !state.isHolding) {
            state.position = pos;
            state.targetPosition = pos;
            updateVisuals();
        }
    }

    function init() {
        const slider = document.getElementById('gate-slider');
        const openBtn = document.getElementById('gate-open-btn');
        const closeBtn = document.getElementById('gate-close-btn');
        const holdOpen = document.getElementById('gate-hold-open');
        const holdClose = document.getElementById('gate-hold-close');

        if (slider) {
            slider.addEventListener('input', (e) => {
                if (window.SafetyMonitor) window.SafetyMonitor.authorizeWebControl();
                const val = parseInt(e.target.value);
                let oldPos = state.position;
                state.position = val;
                state.targetPosition = val;
                updateVisuals();
                if (val === 100 && oldPos < 100) ActivityTimeline.addEvent('Gate fully opened', 'gate');
                if (val === 0 && oldPos > 0) ActivityTimeline.addEvent('Gate fully closed', 'gate');
                if (window.ESP32WS) window.ESP32WS.send({ type: 'gate', pos: val });
            });
        }
        if (openBtn) openBtn.addEventListener('click', () => {
            if (window.SafetyMonitor) window.SafetyMonitor.authorizeWebControl();
            setTarget(100);
        });
        if (closeBtn) closeBtn.addEventListener('click', () => {
            if (window.SafetyMonitor) window.SafetyMonitor.authorizeWebControl();
            setTarget(0);
        });

        const startHoldOpen = (e) => {
            e.preventDefault();
            if (window.SafetyMonitor) window.SafetyMonitor.authorizeWebControl();
            setHolding(1);
        };
        const startHoldClose = (e) => {
            e.preventDefault();
            if (window.SafetyMonitor) window.SafetyMonitor.authorizeWebControl();
            setHolding(-1);
        };
        const stopHold = (e) => { setHolding(0); };

        if (holdOpen) {
            holdOpen.addEventListener('mousedown', startHoldOpen);
            holdOpen.addEventListener('touchstart', startHoldOpen, {passive: false});
            holdOpen.addEventListener('mouseup', stopHold);
            holdOpen.addEventListener('mouseleave', stopHold);
            holdOpen.addEventListener('touchend', stopHold, {passive: false});
        }
        if (holdClose) {
            holdClose.addEventListener('mousedown', startHoldClose);
            holdClose.addEventListener('touchstart', startHoldClose, {passive: false});
            holdClose.addEventListener('mouseup', stopHold);
            holdClose.addEventListener('mouseleave', stopHold);
            holdClose.addEventListener('touchend', stopHold, {passive: false});
        }
        
        updateVisuals();
        state.rafId = requestAnimationFrame(loop);
    }
    return {
        init,
        forceOpen: () => setTarget(100),
        applyServerState
    };
})();

window.GateControl = GateControl;

window.PumpControl = (function() {
    let state = { mode: 'auto', isOn: false };
    
    function setOn(isOn, source = 'manual') {
        if (state.isOn === isOn) return;
        state.isOn = isOn;
        
        const powerBtn = document.getElementById('pump-power-btn');
        const statusText = document.getElementById('pump-status-text');
        const dot = document.getElementById('pump-dot');
        const miniStatus = document.getElementById('pump-mini-status');
        const pill = document.getElementById('pump-status-pill');

        if (isOn) {
            if (powerBtn) {
                powerBtn.classList.remove('off');
                powerBtn.classList.add('on');
                powerBtn.querySelector('span').textContent = 'TURN OFF';
            }
            if (statusText) statusText.textContent = 'RUNNING';
            if (dot) dot.classList.add('on');
            if (miniStatus) miniStatus.textContent = 'RUNNING';
            if (pill) pill.style.backgroundColor = 'rgba(0, 230, 118, 0.2)';
            ActivityTimeline.addEvent(`Pump turned ON (${source})`, 'pump');
        } else {
            if (powerBtn) {
                powerBtn.classList.remove('on');
                powerBtn.classList.add('off');
                powerBtn.querySelector('span').textContent = 'TURN ON';
            }
            if (statusText) statusText.textContent = 'OFF';
            if (dot) dot.classList.remove('on');
            if (miniStatus) miniStatus.textContent = 'OFF';
            if (pill) pill.style.backgroundColor = '';
            ActivityTimeline.addEvent(`Pump turned OFF (${source})`, 'pump');
        }
    }

    // Hysteresis-based auto control:
    // Pump ON when level drops to or below lowThreshold
    // Pump OFF only when level reaches or exceeds highThreshold
    // This prevents rapid cycling near threshold boundaries
    function checkAuto(level, low, high) {
        if (state.mode !== 'auto') return;
        if (!state.isOn && level <= low) {
            setOn(true, 'auto — level ≤ ' + low + '%');
            if (window.AlertSystem) window.AlertSystem.triggerWaterAlert('low');
        } else if (state.isOn && level >= high) {
            setOn(false, 'auto — level ≥ ' + high + '%');
            if (window.AlertSystem) window.AlertSystem.triggerWaterAlert('full');
        }
        // Between low and high: maintain current state (hysteresis)
    }

    function init() {
        const autoBtn = document.getElementById('pump-auto-btn');
        const manualBtn = document.getElementById('pump-manual-btn');
        const manualControls = document.getElementById('pump-manual-controls');
        const powerBtn = document.getElementById('pump-power-btn');

        if (autoBtn && manualBtn && manualControls) {
            autoBtn.addEventListener('click', () => {
                state.mode = 'auto';
                autoBtn.classList.add('active');
                manualBtn.classList.remove('active');
                manualControls.style.display = 'none';
                const modeLabel = document.getElementById('pump-mode-label');
                if (modeLabel) modeLabel.textContent = 'Mode: AUTO (Low -> ON, High -> OFF)';
                const hint = document.querySelector('.wm-pump-hint');
                if (hint) hint.style.display = '';
                ActivityTimeline.addEvent('Pump set to AUTO mode (Low -> ON, High -> OFF)', 'pump');
                if (window.ESP32WS) window.ESP32WS.send({ type: 'pump_mode', action: 'pump_mode', mode: 'AUTO', state: 'AUTO' });
            });

            manualBtn.addEventListener('click', () => {
                state.mode = 'manual';
                manualBtn.classList.add('active');
                autoBtn.classList.remove('active');
                manualControls.style.display = 'flex';
                const modeLabel = document.getElementById('pump-mode-label');
                if (modeLabel) modeLabel.textContent = 'Mode: MANUAL (User Switch Control)';
                const hint = document.querySelector('.wm-pump-hint');
                if (hint) hint.style.display = 'none';
                ActivityTimeline.addEvent('Pump set to MANUAL mode', 'pump');
                if (window.ESP32WS) window.ESP32WS.send({ type: 'pump_mode', action: 'pump_mode', mode: 'MANUAL', state: 'MANUAL' });
            });
        }

        if (powerBtn) {
            powerBtn.addEventListener('click', () => {
                const nextOn = !state.isOn;
                state.mode = 'manual';
                if (manualBtn) manualBtn.classList.add('active');
                if (autoBtn) autoBtn.classList.remove('active');
                if (manualControls) manualControls.style.display = 'flex';
                const modeLabel = document.getElementById('pump-mode-label');
                if (modeLabel) modeLabel.textContent = 'Mode: MANUAL';
                setOn(nextOn, 'manual');
                if (window.ESP32WS) window.ESP32WS.send({ type: 'pump', action: 'pump', mode: 'MANUAL', on: nextOn, state: nextOn });
            });
        }
    }

    function applyServerState(isOn, mode) {
        if (mode) {
            const normMode = String(mode).toLowerCase();
            state.mode = (normMode === 'auto' || normMode === 'automatic') ? 'auto' : 'manual';
            const autoBtn = document.getElementById('pump-auto-btn');
            const manualBtn = document.getElementById('pump-manual-btn');
            const manualControls = document.getElementById('pump-manual-controls');
            const modeLabel = document.getElementById('pump-mode-label');
            const hint = document.querySelector('.wm-pump-hint');

            if (state.mode === 'auto') {
                if (autoBtn) autoBtn.classList.add('active');
                if (manualBtn) manualBtn.classList.remove('active');
                if (manualControls) manualControls.style.display = 'none';
                if (modeLabel) modeLabel.textContent = 'Mode: AUTO (Low ≤ 20% → ON, High ≥ 90% → OFF)';
                if (hint) hint.style.display = '';
            } else {
                if (manualBtn) manualBtn.classList.add('active');
                if (autoBtn) autoBtn.classList.remove('active');
                if (manualControls) manualControls.style.display = 'flex';
                if (modeLabel) modeLabel.textContent = 'Mode: MANUAL (User Switch Control)';
                if (hint) hint.style.display = 'none';
            }
        }
        if (typeof isOn === 'boolean') {
            setOn(isOn, 'ESP32 telemetry');
        }
    }

    return { init, checkAuto, isOn: () => state.isOn, applyServerState };
})();

window.WaterGraph = (function() {
    let canvas, ctx;
    const MAX_POINTS = 60;
    let chartData = [];       // { time: Date, value: number }
    let displayData = [];     // smoothly animated current display values
    let animFrame = null;

    function generateDemoData() {
        chartData = [];
        displayData = [];
        const now = Date.now();
        let val = 40;
        for (let i = MAX_POINTS - 1; i >= 0; i--) {
            val += (Math.random() * 3 - 1.2);
            val = Math.max(5, Math.min(95, val));
            chartData.push({ time: new Date(now - i * 5000), value: val });
            displayData.push(val);
        }
    }

    function addPoint(val) {
        chartData.push({ time: new Date(), value: val });
        if (chartData.length > MAX_POINTS) chartData.shift();
        if (displayData.length > MAX_POINTS) displayData.shift();
        displayData.push(displayData.length > 0 ? displayData[displayData.length - 1] : val);
        // actual value will animate in the draw loop
    }

    function draw() {
        if (!canvas || !ctx) return;
        const dpr = window.devicePixelRatio || 1;
        const w = canvas.width / dpr;
        const h = canvas.height / dpr;
        ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
        ctx.clearRect(0, 0, w, h);

        const padL = 36, padR = 10, padT = 10, padB = 24;
        const cw = w - padL - padR;
        const ch = h - padT - padB;

        // Smoothly animate display data toward actual data
        for (let i = 0; i < chartData.length && i < displayData.length; i++) {
            const diff = chartData[i].value - displayData[i];
            displayData[i] += diff * 0.15;
        }

        // Y-axis labels and grid
        ctx.font = '10px JetBrains Mono, monospace';
        ctx.textAlign = 'right';
        for (let pct = 0; pct <= 100; pct += 25) {
            const y = padT + ch - (pct / 100 * ch);
            ctx.strokeStyle = 'rgba(255, 255, 255, 0.06)';
            ctx.lineWidth = 1;
            ctx.beginPath();
            ctx.moveTo(padL, y);
            ctx.lineTo(padL + cw, y);
            ctx.stroke();
            ctx.fillStyle = 'rgba(121, 134, 203, 0.6)';
            ctx.fillText(pct + '%', padL - 6, y + 3);
        }

        if (displayData.length < 2) return;
        const dx = cw / (displayData.length - 1);

        // Line path
        ctx.beginPath();
        let pts = [];
        for (let i = 0; i < displayData.length; i++) {
            const x = padL + i * dx;
            const y = padT + ch - (displayData[i] / 100 * ch);
            pts.push({ x, y });
            if (i === 0) ctx.moveTo(x, y);
            else {
                const prev = pts[i - 1];
                const cpx = (prev.x + x) / 2;
                ctx.bezierCurveTo(cpx, prev.y, cpx, y, x, y);
            }
        }

        // Gradient fill under curve
        const grad = ctx.createLinearGradient(0, padT, 0, padT + ch);
        grad.addColorStop(0, 'rgba(0, 229, 255, 0.25)');
        grad.addColorStop(0.7, 'rgba(0, 229, 255, 0.05)');
        grad.addColorStop(1, 'rgba(0, 229, 255, 0)');
        ctx.strokeStyle = '#00e5ff';
        ctx.lineWidth = 2;
        ctx.stroke();
        ctx.lineTo(padL + cw, padT + ch);
        ctx.lineTo(padL, padT + ch);
        ctx.closePath();
        ctx.fillStyle = grad;
        ctx.fill();

        // Data point dots (last 5 for emphasis)
        const dotStart = Math.max(0, pts.length - 5);
        for (let i = dotStart; i < pts.length; i++) {
            const alpha = 0.3 + 0.7 * ((i - dotStart) / (pts.length - dotStart));
            ctx.beginPath();
            ctx.arc(pts[i].x, pts[i].y, 3, 0, Math.PI * 2);
            ctx.fillStyle = `rgba(0, 229, 255, ${alpha})`;
            ctx.fill();
        }

        // Glow on last point
        if (pts.length > 0) {
            const last = pts[pts.length - 1];
            ctx.beginPath();
            ctx.arc(last.x, last.y, 5, 0, Math.PI * 2);
            ctx.fillStyle = 'rgba(0, 229, 255, 0.8)';
            ctx.fill();
            ctx.beginPath();
            ctx.arc(last.x, last.y, 10, 0, Math.PI * 2);
            ctx.fillStyle = 'rgba(0, 229, 255, 0.15)';
            ctx.fill();
        }

        // Time labels on x-axis (show every ~10th)
        ctx.textAlign = 'center';
        ctx.fillStyle = 'rgba(121, 134, 203, 0.5)';
        ctx.font = '9px JetBrains Mono, monospace';
        const step = Math.max(1, Math.floor(chartData.length / 6));
        for (let i = 0; i < chartData.length; i += step) {
            const x = padL + i * dx;
            const t = chartData[i].time;
            const label = t.toLocaleTimeString('en-US', { hour12: false, hour: '2-digit', minute: '2-digit' });
            ctx.fillText(label, x, padT + ch + 16);
        }

        animFrame = requestAnimationFrame(draw);
    }

    function resize() {
        if (!canvas) return;
        const dpr = window.devicePixelRatio || 1;
        const parent = canvas.parentElement;
        canvas.width = parent.clientWidth * dpr;
        canvas.height = parent.clientHeight * dpr;
        canvas.style.width = parent.clientWidth + 'px';
        canvas.style.height = parent.clientHeight + 'px';
    }

    function init() {
        canvas = document.getElementById('water-chart');
        if (canvas) {
            ctx = canvas.getContext('2d');
            generateDemoData();
            window.addEventListener('resize', resize);
            resize();
            animFrame = requestAnimationFrame(draw);
        }
    }
    return { init, addPoint };
})();

window.WaterTank = (function() {
    // State models the real sensor: sensorDistance is what the ultrasonic reads
    // waterHeight = tankDepth - sensorDistance
    // waterPercentage = (waterHeight / tankDepth) * 100, clamped 0-100
    let state = {
        sensorDistance: 0, // cm from sensor to water surface (simulated)
        level: 0,          // computed percentage (0-100)
        displayLevel: 0,   // smoothly animated display value
        rafId: null,
        lastTime: 0
    };

    let demoInterval = null;
    let lastLoggedLevel = -1; // for timeline threshold events

    function computeFromDistance() {
        const settings = Settings.getState();
        const waterHeight = Math.max(0, settings.tankDepth - state.sensorDistance);
        state.level = Math.max(0, Math.min(100, (waterHeight / settings.tankDepth) * 100));
    }

    function updateVisuals() {
        const settings = Settings.getState();
        const tankWater = document.getElementById('tank-water');
        const waterPercent = document.getElementById('water-percent');
        const waterHeight = document.getElementById('water-height');
        const sensorDist = document.getElementById('sensor-distance');
        const navInd = document.getElementById('nav-water-indicator');
        const depthDisplay = document.getElementById('tank-depth-display');
        const modeLabel = document.getElementById('pump-mode-label');

        // Animate display level smoothly
        const diff = state.level - state.displayLevel;
        state.displayLevel += diff * 0.12;
        if (Math.abs(diff) < 0.05) state.displayLevel = state.level;

        const displayPct = state.displayLevel;
        const heightCm = (displayPct / 100) * settings.tankDepth;
        const distCm = settings.tankDepth - heightCm + settings.sensorOffset;

        if (tankWater) tankWater.style.height = `${displayPct}%`;
        // LCD values: show one decimal, no unit suffix (units in separate HTML elements)
        if (waterPercent) waterPercent.textContent = displayPct.toFixed(1);
        if (navInd) navInd.textContent = `${Math.round(displayPct)}%`;
        if (waterHeight) waterHeight.textContent = heightCm.toFixed(1);
        if (sensorDist) sensorDist.textContent = distCm.toFixed(1);
        if (depthDisplay) depthDisplay.textContent = settings.tankDepth;

        // Stream animation when pump is ON
        const stream = document.getElementById('wm-water-stream');
        const pumpOn = window.PumpControl && window.PumpControl.isOn();
        if (stream) {
            stream.style.display = pumpOn ? 'block' : 'none';
        }

        // Water color based on level
        let color = 'rgba(0, 229, 255, 0.75)';
        if (displayPct < 20) color = 'rgba(255, 82, 82, 0.75)';
        else if (displayPct < 40) color = 'rgba(255, 171, 64, 0.75)';
        document.documentElement.style.setProperty('--water-color', color);

        // LCD value color matches water state
        const lcdEls = document.querySelectorAll('.wm-lcd-value');
        lcdEls.forEach(el => {
            if (displayPct < 20) el.style.color = 'var(--danger)';
            else if (displayPct < 40) el.style.color = 'var(--warning)';
            else el.style.color = '';
        });
    }

    function animationLoop(timestamp) {
        if (!state.lastTime) state.lastTime = timestamp;
        state.lastTime = timestamp;
        updateVisuals();
        state.rafId = requestAnimationFrame(animationLoop);
    }

    function setSensorDistance(dist) {
        const settings = Settings.getState();
        state.sensorDistance = Math.max(0, Math.min(settings.tankDepth + settings.sensorOffset, dist));
        computeFromDistance();

        // Pump auto-check
        if (window.PumpControl) {
            window.PumpControl.checkAuto(state.level, settings.lowThreshold, settings.highThreshold);
        }

        // Log threshold crossings to timeline
        const roundedLevel = Math.round(state.level);
        if (lastLoggedLevel >= 0) {
            if (lastLoggedLevel > settings.lowThreshold && roundedLevel <= settings.lowThreshold) {
                ActivityTimeline.addEvent(`Water level dropped to ${roundedLevel}% (low threshold)`, 'water');
            }
            if (lastLoggedLevel < settings.highThreshold && roundedLevel >= settings.highThreshold) {
                ActivityTimeline.addEvent(`Water level reached ${roundedLevel}% (high threshold)`, 'water');
            }
        }
        lastLoggedLevel = roundedLevel;
    }

    function updateFromSettings() {
        computeFromDistance();
        updateVisuals();
    }

    function startDemoSimulation() {
        if (demoInterval) clearInterval(demoInterval);
        const settings = Settings.getState();

        // Initial sensor distance = 48cm from top (about 68% full for 150cm tank)
        setSensorDistance(settings.tankDepth * 0.32);
        state.displayLevel = state.level; // Immediate fill on load

        // Every 3 seconds, simulate the water system:
        // - If pump is ON: water rises (sensor distance decreases)
        // - Natural evaporation/drain: water drops slightly (sensor distance increases)
        demoInterval = setInterval(() => {
            const settings = Settings.getState();
            const pumpOn = window.PumpControl && window.PumpControl.isOn();
            let newDist = state.sensorDistance;

            if (pumpOn) {
                // Pump fills at ~2-3 cm per tick
                newDist -= (2 + Math.random() * 1.5);
            } else {
                // Natural drain/evaporation: ~0.5-1.5 cm per tick
                newDist += (0.5 + Math.random());
            }

            // Small random noise to feel alive
            newDist += (Math.random() - 0.5) * 0.4;

            // Clamp: sensor can't read less than sensorOffset or more than tankDepth
            newDist = Math.max(settings.sensorOffset, Math.min(settings.tankDepth, newDist));

            setSensorDistance(newDist);

            // Feed the graph
            if (window.WaterGraph) {
                window.WaterGraph.addPoint(state.level);
            }
        }, 3000);
    }

    function stopDemo() {
        if (demoInterval) {
            clearInterval(demoInterval);
            demoInterval = null;
        }
    }

    function applyServerState(dist, level, height) {
        stopDemo();
        if (typeof dist === 'number') {
            state.sensorDistance = dist;
            computeFromDistance();
        } else if (typeof level === 'number') {
            state.level = Math.max(0, Math.min(100, level));
        }
        if (window.WaterGraph) {
            window.WaterGraph.addPoint(state.level);
        }
    }

    function init() {
        state.rafId = requestAnimationFrame(animationLoop);
        startDemoSimulation();
    }

    return {
        init,
        updateFromSettings,
        getState: () => ({ level: state.level, displayLevel: state.displayLevel }),
        setSensorDistance,
        stopDemo,
        applyServerState
    };
})();

const AlertSystem = (function() {
    let alertHistory = [];
    const maxHistory = 100;
    let activeFilter = 'all';
    let audioCtx = null;

    function getAudioContext() {
        if (!audioCtx) {
            const AudioContextClass = window.AudioContext || window.webkitAudioContext;
            if (AudioContextClass) audioCtx = new AudioContextClass();
        }
        if (audioCtx && audioCtx.state === 'suspended') {
            audioCtx.resume();
        }
        return audioCtx;
    }

    function playSound(type) {
        if (window.Settings && window.Settings.getState().soundMuted) return;
        try {
            const ctx = getAudioContext();
            if (!ctx) return;
            const now = ctx.currentTime;
            const osc = ctx.createOscillator();
            const gain = ctx.createGain();

            osc.connect(gain);
            gain.connect(ctx.destination);

            if (type === 'warning') {
                osc.type = 'sine';
                osc.frequency.setValueAtTime(880, now);
                osc.frequency.exponentialRampToValueAtTime(660, now + 0.25);
                gain.gain.setValueAtTime(0.3, now);
                gain.gain.exponentialRampToValueAtTime(0.01, now + 0.3);
                osc.start(now);
                osc.stop(now + 0.3);
            } else if (type === 'danger') {
                osc.type = 'triangle';
                osc.frequency.setValueAtTime(1050, now);
                osc.frequency.setValueAtTime(800, now + 0.15);
                osc.frequency.setValueAtTime(1050, now + 0.3);
                gain.gain.setValueAtTime(0.4, now);
                gain.gain.exponentialRampToValueAtTime(0.01, now + 0.45);
                osc.start(now);
                osc.stop(now + 0.45);
            } else if (type === 'emergency') {
                osc.type = 'sawtooth';
                osc.frequency.setValueAtTime(500, now);
                osc.frequency.linearRampToValueAtTime(1400, now + 0.2);
                osc.frequency.linearRampToValueAtTime(500, now + 0.4);
                gain.gain.setValueAtTime(0.5, now);
                gain.gain.exponentialRampToValueAtTime(0.01, now + 0.6);
                osc.start(now);
                osc.stop(now + 0.6);
            }
        } catch (e) {}
    }

    function vibrate(pattern) {
        if ('vibrate' in navigator) {
            try { navigator.vibrate(pattern); } catch (e) {}
        }
    }

    // ── Toast Stack Manager ──────────────────────────────────────────────
    function showToastNotification(options) {
        const container = document.getElementById('toast-container');
        if (!container) return;

        const { id = Date.now() + Math.random(), title, message, type = 'info', icon, duration = 6000, actions } = options;
        const toastCard = document.createElement('div');
        toastCard.className = `toast-card toast-${type}`;
        toastCard.setAttribute('data-toast-id', id);

        const timeStr = new Date().toLocaleTimeString('en-US', { hour12: false });

        toastCard.innerHTML = `
            <div class="toast-card-icon">${icon || '🔔'}</div>
            <div class="toast-card-content">
                <div class="toast-card-header">
                    <span class="toast-card-title">${title}</span>
                    <span class="toast-card-time">${timeStr}</span>
                </div>
                <p class="toast-card-msg">${message}</p>
                ${actions ? `
                <div class="toast-card-actions">
                    <button class="toast-action-btn accept" id="toast-accept-${id}">ACCEPT</button>
                    <button class="toast-action-btn reject" id="toast-reject-${id}">REJECT</button>
                </div>
                ` : ''}
            </div>
            <button class="toast-card-close" aria-label="Close">✕</button>
        `;

        const closeBtn = toastCard.querySelector('.toast-card-close');
        if (closeBtn) closeBtn.addEventListener('click', () => removeToast(toastCard));

        if (actions) {
            const acceptBtn = toastCard.querySelector(`.toast-action-btn.accept`);
            const rejectBtn = toastCard.querySelector(`.toast-action-btn.reject`);
            if (acceptBtn) {
                acceptBtn.addEventListener('click', () => {
                    if (actions.onAccept) actions.onAccept();
                    removeToast(toastCard);
                });
            }
            if (rejectBtn) {
                rejectBtn.addEventListener('click', () => {
                    if (actions.onReject) actions.onReject();
                    removeToast(toastCard);
                });
            }
        }

        container.appendChild(toastCard);

        if (duration > 0) {
            setTimeout(() => {
                removeToast(toastCard);
            }, duration);
        }
    }

    function removeToast(el) {
        if (!el || !el.parentElement) return;
        el.classList.add('hide');
        setTimeout(() => {
            if (el.parentElement) el.parentElement.removeChild(el);
        }, 300);
    }

    // ── Alert History Logger ──────────────────────────────────────────────
    function logAlert(alertObj) {
        const item = {
            id: Date.now() + Math.random(),
            time: new Date(),
            title: alertObj.title,
            message: alertObj.message,
            severity: alertObj.severity || 'info',
            icon: alertObj.icon || '🔔'
        };

        alertHistory.unshift(item);
        if (alertHistory.length > maxHistory) alertHistory.pop();

        renderHistory();
    }

    function renderHistory() {
        const list = document.getElementById('alert-history-list');
        if (!list) return;

        const filtered = alertHistory.filter(item => {
            if (activeFilter === 'all') return true;
            return item.severity === activeFilter;
        });

        if (filtered.length === 0) {
            list.innerHTML = `<div class="alert-history-empty">No ${activeFilter.toUpperCase()} alerts recorded</div>`;
            return;
        }

        list.innerHTML = filtered.map(item => `
            <div class="alert-history-item">
                <span class="ahi-icon">${item.icon}</span>
                <div class="ahi-content">
                    <div class="ahi-top">
                        <span class="ahi-title">${item.title}</span>
                        <span class="ahi-time">${item.time.toLocaleTimeString('en-US', { hour12: false })}</span>
                    </div>
                    <span class="ahi-msg">${item.message}</span>
                </div>
                <span class="ahi-badge ${item.severity}">${item.severity.toUpperCase()}</span>
            </div>
        `).join('');
    }

    function initHistoryFilters() {
        const btns = document.querySelectorAll('.alert-filter-btn');
        btns.forEach(btn => {
            btn.addEventListener('click', () => {
                btns.forEach(b => b.classList.remove('active'));
                btn.classList.add('active');
                activeFilter = btn.getAttribute('data-filter');
                renderHistory();
            });
        });
    }

    function triggerGasAlert(status) {
        if (status === 'warning') {
            const title = '⚠ GAS WARNING';
            const msg = 'Gas concentration is increasing. Please inspect the environment.';
            showToastNotification({ title, message: msg, type: 'warning', icon: '☣' });
            logAlert({ title, message: msg, severity: 'warning', icon: '☣' });
            playSound('warning');
            vibrate([200]);
        } else if (status === 'danger') {
            const title = '🚨 GAS LEAK DETECTED';
            const msg = 'High gas concentration detected. Immediate action is recommended.';
            showToastNotification({ title, message: msg, type: 'danger', icon: '🚨', duration: 8000 });
            logAlert({ title, message: msg, severity: 'danger', icon: '🚨' });
            playSound('danger');
            vibrate([300, 100, 300, 100, 300]);
        }
    }

    function triggerFlameAlert(status) {
        if (status === 'warning') {
            const title = '⚠ FLAME WARNING';
            const msg = 'Possible flame detected. Monitoring...';
            showToastNotification({ title, message: msg, type: 'warning', icon: '🔥' });
            logAlert({ title, message: msg, severity: 'warning', icon: '🔥' });
            playSound('warning');
            vibrate([200]);
        } else if (status === 'danger') {
            const title = '🔥 FIRE DETECTED';
            const msg = 'Critical danger. Emergency response required.';
            showToastNotification({ title, message: msg, type: 'emergency', icon: '🔥', duration: 10000 });
            logAlert({ title, message: msg, severity: 'emergency', icon: '🔥' });
            playSound('emergency');
            vibrate([500, 200, 500, 200, 500]);

            const card = document.getElementById('flame-sensor-card');
            if (card) {
                card.classList.add('flash-red');
                setTimeout(() => card.classList.remove('flash-red'), 4000);
            }
        }
    }

    function triggerIRAlert(status) {
        if (status === 'danger' || status === 'warning') {
            const title = '🚶 PERSON DETECTED';
            const msg = 'Object detected in front of gate. Gate opening paused.';
            showToastNotification({
                title,
                message: msg,
                type: 'warning',
                icon: '🚶',
                duration: 8000,
                actions: {
                    onAccept: () => {
                        ActivityTimeline.addEvent('⚠ Gate open ACCEPTED despite IR alert', 'gate');
                        if (window.GateControl) window.GateControl.forceOpen();
                    },
                    onReject: () => {
                        ActivityTimeline.addEvent('🔒 Gate open REJECTED — IR safety block', 'gate');
                    }
                }
            });
            logAlert({ title, message: msg, severity: 'warning', icon: '🚶' });
            playSound('warning');
            vibrate([200]);
        }
    }

    function triggerWaterAlert(type) {
        if (type === 'full') {
            const title = '💧 WATER TANK FULL';
            const msg = 'Pump stopped automatically.';
            showToastNotification({ title, message: msg, type: 'info', icon: '💧' });
            logAlert({ title, message: msg, severity: 'info', icon: '💧' });
            playSound('warning');
        } else if (type === 'low') {
            const title = '💧 LOW WATER LEVEL';
            const msg = 'Pump started automatically.';
            showToastNotification({ title, message: msg, type: 'warning', icon: '💧' });
            logAlert({ title, message: msg, severity: 'warning', icon: '💧' });
            playSound('warning');
        } else if (type === 'overflow') {
            const title = '⚠ OVERFLOW WARNING';
            const msg = 'Tank is overflowing. Turn pump OFF.';
            showToastNotification({ title, message: msg, type: 'danger', icon: '⚠', duration: 8000 });
            logAlert({ title, message: msg, severity: 'emergency', icon: '⚠' });
            playSound('danger');
            vibrate([300, 100, 300]);

            const tankCard = document.querySelector('.wm-tank-card');
            if (tankCard) {
                tankCard.classList.add('flash-red');
                setTimeout(() => tankCard.classList.remove('flash-red'), 4000);
            }
        }
    }

    function init() {
        initHistoryFilters();
        logAlert({ title: 'System Initialized', message: 'Nexus Home Alert System operational', severity: 'info', icon: 'ℹ' });
    }

    return { init, triggerGasAlert, triggerFlameAlert, triggerIRAlert, triggerWaterAlert, showToastNotification, logAlert };
})();

window.AlertSystem = AlertSystem;

const SafetyMonitor = (function() {
    // ── State ─────────────────────────────────────────────────────────────
    const sensors = {
        ir:    { status: 'safe', id: 'ir' },
        gas:   { status: 'safe', id: 'gas' },
        flame: { status: 'safe', id: 'flame' }
    };

    let pendingGateAction = null;
    let userWebControlActive = false;
    let webControlTimeout = null;

    function authorizeWebControl() {
        userWebControlActive = true;
        if (webControlTimeout) clearTimeout(webControlTimeout);
        webControlTimeout = setTimeout(() => {
            userWebControlActive = false;
        }, 10000);
    }

    function isIrBlocked() {
        return sensors.ir.status !== 'safe';
    }

    const sensorTexts = {
        ir:    { safe: 'No Motion',      warning: 'Motion Detected', danger: 'Intruder Alert!' },
        gas:   { safe: 'Normal',         warning: 'Gas Detected',    danger: 'Dangerous Level!' },
        flame: { safe: 'No Flame',       warning: 'Heat Detected',   danger: 'Fire Detected!' }
    };
    const sensorBars = {
        ir:    { safe: '10%', warning: '60%', danger: '95%' },
        gas:   { safe: '15%', warning: '55%', danger: '90%' },
        flame: { safe:  '5%', warning: '65%', danger: '100%' }
    };

    function updateNavIndicator() {
        let isDanger = false, isWarn = false;
        Object.values(sensors).forEach(s => {
            if (s.status === 'danger')  isDanger = true;
            if (s.status === 'warning') isWarn   = true;
        });
        const navInd = document.getElementById('nav-safety-indicator');
        if (navInd) {
            navInd.className = 'nav-badge';
            if (isDanger)     { navInd.textContent = 'ALERT'; navInd.classList.add('danger'); }
            else if (isWarn)  { navInd.textContent = 'WARN';  navInd.classList.add('warning'); }
            else              { navInd.textContent = 'OK';    navInd.classList.add('safe'); }
        }
    }

    function updateDangerBanner() {
        const dangerSensors = Object.values(sensors).filter(s => s.status === 'danger');
        const banner     = document.getElementById('danger-banner');
        const bannerText = document.getElementById('danger-banner-text');
        if (!banner) return;
        if (dangerSensors.length === 0) {
            banner.style.display = 'none';
        } else {
            const labels = { ir: '⚠ IR INTRUDER', gas: '☣ GAS DANGER', flame: '🔥 FIRE ALERT' };
            const msgs = dangerSensors.map(s => labels[s.id]).join('  •  ');
            if (bannerText) bannerText.textContent = msgs + '  —  Check safety section immediately';
            banner.style.display = 'flex';
        }
    }

    function updateGateBlockedOverlay() {
        const overlay = document.getElementById('gate-blocked-overlay');
        if (!overlay) return;
        const irTriggered = sensors.ir.status !== 'safe';
        overlay.classList.toggle('visible', irTriggered);
    }

    function setSensorState(sensorId, status, skipModal) {
        const prev = sensors[sensorId].status;
        if (prev === status) return;
        sensors[sensorId].status = status;

        const card      = document.getElementById(`${sensorId}-sensor-card`);
        const dot       = document.getElementById(`${sensorId}-sensor-dot`);
        const statusEl  = document.getElementById(`${sensorId}-sensor-status`);
        const bar       = document.getElementById(`${sensorId}-sensor-bar`);
        const stateLabel= document.getElementById(`${sensorId}-state-label`);

        if (card) {
            card.classList.remove('warning', 'danger');
            if (status !== 'safe') card.classList.add(status);
        }
        if (dot)        dot.className = `sensor-dot ${status}`;
        if (statusEl)   statusEl.textContent = sensorTexts[sensorId][status];
        if (bar)        bar.style.width = sensorBars[sensorId][status];
        if (stateLabel) {
            stateLabel.textContent =
                status === 'safe' ? 'SAFE' : status === 'warning' ? 'CAUTION' : 'DANGER';
        }

        updateNavIndicator();
        updateDangerBanner();
        updateGateBlockedOverlay();

        if (status === 'warning' || status === 'danger') {
            const text = sensorTexts[sensorId][status];
            ActivityTimeline.addEvent(`⚠ Sensor Alert [${sensorId.toUpperCase()}]: ${text}`, 'sensor');
        } else if (prev !== 'safe') {
            ActivityTimeline.addEvent(`✓ ${sensorId.toUpperCase()} sensor cleared — All safe`, 'sensor');
        }

        // Trigger AlertSystem notifications
        if (window.AlertSystem) {
            if (sensorId === 'gas') window.AlertSystem.triggerGasAlert(status);
            if (sensorId === 'flame') window.AlertSystem.triggerFlameAlert(status);
            if (sensorId === 'ir') window.AlertSystem.triggerIRAlert(status);
        }

        if (sensorId === 'ir' && status === 'danger' && !skipModal) {
            if (!userWebControlActive) {
                showGateModal('Visitor / Object detected near gate — Authorize entry?');
            }
        }
    }

    // ── IR Safety Modal ───────────────────────────────────────────────────
    function showGateModal(reason) {
        const modal  = document.getElementById('safety-modal');
        const reasonEl = document.getElementById('safety-modal-reason');
        if (!modal) return;
        if (reasonEl) reasonEl.textContent = reason;

        // Update mini sensor dots inside modal
        ['ir', 'gas', 'flame'].forEach(id => {
            const dot = document.getElementById(`sms-${id}-dot`);
            const val = document.getElementById(`sms-${id}-val`);
            const s = sensors[id].status;
            if (dot) {
                dot.className = 'sms-dot';
                if (s === 'danger')  dot.classList.add('triggered');
                else if (s === 'warning') dot.classList.add('warning');
            }
            if (val) {
                val.className = 'sms-val';
                if (s === 'danger')  { val.textContent = 'DANGER';   val.classList.add('triggered'); }
                else if (s === 'warning') { val.textContent = 'CAUTION'; val.classList.add('warning'); }
                else                { val.textContent = 'OK'; }
            }
        });

        modal.classList.add('active');
        document.body.style.overflow = 'hidden';
    }

    function closeGateModal() {
        const modal = document.getElementById('safety-modal');
        if (modal) modal.classList.remove('active');
        document.body.style.overflow = '';
        pendingGateAction = null;
    }

    // ── Gate interception: called by GateControl before opening ──────────
    function requestGateOpen(actionCallback) {
        if (userWebControlActive) return true;
        if (sensors.ir.status === 'safe' && sensors.gas.status !== 'danger' && sensors.flame.status !== 'danger') {
            return true;
        }
        pendingGateAction = actionCallback;
        let reason = '';
        if (sensors.ir.status !== 'safe')    reason = 'IR sensor detected object or person near gate';
        else if (sensors.gas.status === 'danger')   reason = 'Dangerous gas level detected in area';
        else if (sensors.flame.status === 'danger') reason = 'Flame/fire detected — cannot open gate';
        showGateModal(reason);
        return false;
    }

    // ── Modal button handlers ─────────────────────────────────────────────
    function initModalButtons() {
        const acceptBtn = document.getElementById('safety-modal-accept');
        const rejectBtn = document.getElementById('safety-modal-reject');
        const bannerClose = document.getElementById('danger-banner-close');

        if (acceptBtn) {
            acceptBtn.addEventListener('click', () => {
                authorizeWebControl();
                closeGateModal();
                ActivityTimeline.addEvent('⚠ Visitor Accepted — Opening Gate', 'gate');
                if (pendingGateAction === null) {
                    if (window.GateControl) window.GateControl.forceOpen();
                } else if (typeof pendingGateAction === 'function') {
                    pendingGateAction();
                }
            });
        }

        if (rejectBtn) {
            rejectBtn.addEventListener('click', () => {
                authorizeWebControl();
                ActivityTimeline.addEvent('🔒 Visitor Rejected — Gate stays locked', 'gate');
                closeGateModal();
            });
        }

        if (bannerClose) {
            bannerClose.addEventListener('click', () => {
                document.getElementById('danger-banner').style.display = 'none';
            });
        }

        // Close modal on backdrop click
        const backdrop = document.querySelector('.safety-modal-backdrop');
        if (backdrop) {
            backdrop.addEventListener('click', () => {
                ActivityTimeline.addEvent('🔒 Gate modal dismissed — gate stays closed', 'gate');
                closeGateModal();
            });
        }
    }

    let autoDemoInterval = null;

    // ── Auto demo simulation ──────────────────────────────────────────────
    function startAutoDemo() {
        if (autoDemoInterval) clearInterval(autoDemoInterval);
        // Gentle random simulation — mostly safe, occasional events
        autoDemoInterval = setInterval(() => {
            const keys = ['ir', 'gas', 'flame'];
            const id   = keys[Math.floor(Math.random() * keys.length)];
            const r    = Math.random();
            let newStatus = 'safe';
            if      (r > 0.93) newStatus = 'danger';
            else if (r > 0.80) newStatus = 'warning';
            setSensorState(id, newStatus);
        }, 12000);
    }

    function stopAutoDemo() {
        if (autoDemoInterval) {
            clearInterval(autoDemoInterval);
            autoDemoInterval = null;
        }
    }

    function applyServerState(gas, flame, ir) {
        stopAutoDemo();
        if (gas && gas.status) setSensorState('gas', gas.status);
        if (flame && flame.status) setSensorState('flame', flame.status);
        if (ir && ir.status) setSensorState('ir', ir.status);
    }

    // ── Public demo() for manual HTML buttons ────────────────────────────
    function demo(sensorId, status) {
        setSensorState(sensorId, status);
    }

    function init() {
        initModalButtons();
        stopAutoDemo(); // Real hardware pin readings only — no fake demo timers!
    }

    return { init, setSensorState, requestGateOpen, authorizeWebControl, isIrBlocked, demo, stopAutoDemo, applyServerState };
})();

// Expose globally for HTML onclick
window.SafetyMonitor = SafetyMonitor;

const ServoControl = (function() {
    let state = {
        isOn: false,
        angle: 0,
        targetAngle: 0,
        rafId: null,
        lastTime: 0
    };

    function updateVisuals() {
        const angleVal = document.getElementById('servo-angle-value');
        const needle = document.getElementById('servo-needle');
        const arcActive = document.getElementById('servo-arc-active');
        const slider = document.getElementById('servo-slider');
        const navInd = document.getElementById('nav-servo-indicator');

        if (angleVal) angleVal.textContent = Math.round(state.angle);
        if (navInd) navInd.textContent = `${Math.round(state.angle)}°`;
        if (needle) needle.setAttribute('transform', `rotate(${state.angle - 90}, 100, 110)`);
        
        if (arcActive) {
            const arcLen = 251.2;
            const offset = arcLen - ((state.angle / 180) * arcLen);
            arcActive.style.strokeDashoffset = offset;
        }

        if (slider && document.activeElement !== slider) {
            slider.value = Math.round(state.angle);
        }
        
        const presets = document.querySelectorAll('.servo-preset-btn');
        presets.forEach(btn => {
            if (parseInt(btn.getAttribute('data-angle')) === Math.round(state.angle)) {
                btn.classList.add('active');
            } else {
                btn.classList.remove('active');
            }
        });
    }

    function loop(timestamp) {
        if (!state.lastTime) state.lastTime = timestamp;
        const dt = timestamp - state.lastTime;
        state.lastTime = timestamp;

        if (state.angle !== state.targetAngle) {
            const diff = state.targetAngle - state.angle;
            if (Math.abs(diff) < 0.5) {
                state.angle = state.targetAngle;
            } else {
                const move = Math.sign(diff) * Math.min(Math.abs(diff), 180 * (dt / 1000));
                state.angle += move;
            }
            updateVisuals();
        }

        state.rafId = requestAnimationFrame(loop);
    }

    function init() {
        const toggle = document.getElementById('servo-power-toggle');
        const status = document.getElementById('servo-power-status');
        const toggleWrap = document.getElementById('servo-toggle-wrap');
        const slider = document.getElementById('servo-slider');
        const presets = document.querySelectorAll('.servo-preset-btn');

        if (toggle) {
            toggle.addEventListener('change', (e) => {
                state.isOn = e.target.checked;
                if (state.isOn) {
                    if (status) status.textContent = 'ON';
                    if (toggleWrap) toggleWrap.classList.add('on');
                    if (slider) slider.disabled = false;
                    ActivityTimeline.addEvent('Servo turned ON', 'servo');
                } else {
                    if (status) status.textContent = 'OFF';
                    if (toggleWrap) toggleWrap.classList.remove('on');
                    if (slider) slider.disabled = true;
                    state.targetAngle = 0;
                    ActivityTimeline.addEvent('Servo turned OFF', 'servo');
                }
                if (window.ESP32WS) window.ESP32WS.send({ type: 'servo', on: state.isOn, angle: state.angle });
            });
        }

        if (slider) {
            slider.addEventListener('input', (e) => {
                if (!state.isOn) return;
                state.targetAngle = parseInt(e.target.value);
                if (window.ESP32WS) window.ESP32WS.send({ type: 'servo', on: state.isOn, angle: state.targetAngle });
            });
        }

        presets.forEach(btn => {
            btn.addEventListener('click', () => {
                if (!state.isOn) return;
                const angle = parseInt(btn.getAttribute('data-angle'));
                state.targetAngle = angle;
                ActivityTimeline.addEvent(`Servo moved to ${angle}°`, 'servo');
                if (window.ESP32WS) window.ESP32WS.send({ type: 'servo', on: state.isOn, angle });
            });
        });

        updateVisuals();
        state.rafId = requestAnimationFrame(loop);
    }

    function applyServerState(angle, isOn) {
        const toggle = document.getElementById('servo-power-toggle');
        const status = document.getElementById('servo-power-status');
        const toggleWrap = document.getElementById('servo-toggle-wrap');
        const slider = document.getElementById('servo-slider');

        if (typeof isOn === 'boolean' && state.isOn !== isOn) {
            state.isOn = isOn;
            if (toggle) toggle.checked = isOn;
            if (status) status.textContent = isOn ? 'ON' : 'OFF';
            if (toggleWrap) {
                if (isOn) toggleWrap.classList.add('on');
                else toggleWrap.classList.remove('on');
            }
            if (slider) slider.disabled = !isOn;
        }

        if (typeof angle === 'number' && state.targetAngle !== angle) {
            state.targetAngle = angle;
        }
    }

    return { init, applyServerState };
})();

const BootSequence = (function() {
    function init() {
        const bootScreen = document.getElementById('boot-screen');
        const progressBar = document.getElementById('boot-progress-bar');
        const bootStatus = document.getElementById('boot-status');
        const dashboard = document.getElementById('dashboard');
        
        if (!bootScreen || !dashboard) return;

        const messages = [
            { text: 'Initializing core systems...', time: 0 },
            { text: 'Loading sensor modules...', time: 625 },
            { text: 'Connecting peripherals...', time: 1250 },
            { text: 'Calibrating sensors...', time: 1875 },
            { text: 'System ready.', time: 2500 }
        ];

        if (progressBar) {
            progressBar.style.transition = 'width 2.5s ease-in-out';
            setTimeout(() => {
                progressBar.style.width = '100%';
            }, 50);
        }

        messages.forEach(msg => {
            setTimeout(() => {
                if (bootStatus) bootStatus.textContent = msg.text;
            }, msg.time);
        });

        setTimeout(() => {
            bootScreen.style.transition = 'opacity 0.5s ease';
            bootScreen.style.opacity = '0';
            setTimeout(() => {
                bootScreen.style.display = 'none';
                dashboard.style.display = 'flex';
                void dashboard.offsetWidth;
                dashboard.style.transition = 'opacity 0.8s ease';
                dashboard.style.opacity = '1';
                initDashboard();
            }, 500);
        }, 2500);
    }
    return { init };
})();

function initDashboard() {
    Navigation.init();
    ScrollAnimations.init();
    Settings.init();
    LightControl.init();
    GateControl.init();
    ServoControl.init();
    if (window.WaterTank) window.WaterTank.init();
    if (window.WaterGraph) window.WaterGraph.init();
    if (window.PumpControl) window.PumpControl.init();
    if (window.AlertSystem) window.AlertSystem.init();
    SafetyMonitor.init();
    ActivityTimeline.init();
}

document.addEventListener('DOMContentLoaded', () => {
    BootSequence.init();
});

)rawliteral";

const char ESP32_WS_JS[] PROGMEM = R"rawliteral(
/**
 * ESP32 WebSocket & REST API Integration Bridge
 * Connects frontend dashboard to ESP32 WebSocket server (ws://${location.host}/ws).
 * Handles real-time telemetry streaming & remote device control.
 */
(function() {
    'use strict';

    let ws = null;
    let reconnectTimer = null;
    let isConnected = false;
    const host = location.hostname || '192.168.4.1';
    const wsPort = (location.port && location.port !== '80' && location.port !== '') ? location.port : '81';
    const WS_URL = (location.protocol === 'https:' ? 'wss://' : 'ws://') + host + ':' + wsPort + '/ws';

    function init() {
        connect();
    }

    function connect() {
        if (ws && (ws.readyState === WebSocket.CONNECTING || ws.readyState === WebSocket.OPEN)) return;

        try {
            ws = new WebSocket(WS_URL);
        } catch (e) {
            console.warn('[ESP32 WS] Connection error:', e);
            scheduleReconnect();
            return;
        }

        ws.onopen = function() {
            console.log('[ESP32 WS] Connected to ' + WS_URL);
            isConnected = true;
            updateStatusUI(true);
            if (window.WaterTank && window.WaterTank.stopDemo) {
                window.WaterTank.stopDemo();
            }
            if (window.SafetyMonitor && window.SafetyMonitor.stopAutoDemo) {
                window.SafetyMonitor.stopAutoDemo();
            }
            if (window.Toast) {
                window.Toast.showToast('ESP32 WebSocket Connected');
            }
        };

        ws.onmessage = function(event) {
            try {
                const data = JSON.parse(event.data);
                handleStateMessage(data);
            } catch (err) {
                console.error('[ESP32 WS] Error parsing packet:', err);
            }
        };

        ws.onclose = function() {
            if (isConnected) {
                console.warn('[ESP32 WS] Disconnected from hardware');
                if (window.Toast) {
                    window.Toast.showToast('ESP32 Disconnected — Operating in Demo Mode');
                }
            }
            isConnected = false;
            updateStatusUI(false);
            scheduleReconnect();
        };

        ws.onerror = function(err) {
            console.warn('[ESP32 WS] Error:', err);
            ws.close();
        };
    }

    function scheduleReconnect() {
        if (reconnectTimer) clearTimeout(reconnectTimer);
        reconnectTimer = setTimeout(connect, 3000);
    }

    function updateStatusUI(online) {
        const dot = document.getElementById('system-status-dot');
        const text = document.getElementById('system-status-text');
        if (dot && text) {
            if (online) {
                dot.className = 'status-dot online';
                text.textContent = 'ESP32 ONLINE';
            } else {
                dot.className = 'status-dot offline';
                text.textContent = 'OFFLINE (DEMO)';
            }
        }
    }

    function sendCommand(data) {
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify(data));
        } else {
            // Fallback REST POST if WebSocket is closed
            fetch('/api/' + data.type, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            }).catch(err => console.warn('[ESP32 REST Fallback Error]', err));
        }
    }

    function handleStateMessage(data) {
        if (data.type !== 'state') return;

        // 1. Lights
        if (Array.isArray(data.lights) && window.LightControl && window.LightControl.applyServerState) {
            window.LightControl.applyServerState(data.lights);
        }

        // 2. Gate
        if (data.gate && window.GateControl && window.GateControl.applyServerState) {
            window.GateControl.applyServerState(data.gate.pos, data.gate.status);
        }

        // 3. Servo
        if (data.servo && window.ServoControl && window.ServoControl.applyServerState) {
            window.ServoControl.applyServerState(data.servo.angle, data.servo.on);
        }

        // 4. Pump
        if (data.pump && window.PumpControl && window.PumpControl.applyServerState) {
            window.PumpControl.applyServerState(data.pump.on, data.pump.mode);
        }

        // 5. Water level
        if (data.water && window.WaterTank && window.WaterTank.applyServerState) {
            window.WaterTank.applyServerState(data.water.distance, data.water.level, data.water.height);
        }

        // 6. Safety Sensors (Gas, Flame, IR)
        if (window.SafetyMonitor && window.SafetyMonitor.applyServerState) {
            window.SafetyMonitor.applyServerState(data.gas, data.flame, data.ir);
        }

        // 7. Dashboard Settings & System Name
        if (data.settings && window.Settings && window.Settings.applyServerSettings) {
            window.Settings.applyServerSettings(data.settings);
        }
    }

    // Export WebSocket Controller to global scope
    window.ESP32WS = {
        init,
        send: sendCommand,
        isConnected: () => isConnected
    };

    document.addEventListener('DOMContentLoaded', init);
})();

)rawliteral";

#endif // WEB_ASSETS_H
