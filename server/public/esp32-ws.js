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
    const WS_URL = (location.protocol === 'https:' ? 'wss://' : 'ws://') + (location.host || '192.168.4.1') + '/ws';

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
