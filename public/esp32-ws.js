/**
 * AquaSense Pro — WebSocket Bridge (Render.com)
 * Connects browser dashboard to Render.com relay → ESP32
 * Handles telemetry messages from ESP32 water tank controller
 */
(function () {
    'use strict';

    const RENDER_HOST = location.hostname;
    const WS_PROTOCOL = location.protocol === 'https:' ? 'wss://' : 'ws://';
    const WS_URL = WS_PROTOCOL + RENDER_HOST + '/ws';

    let ws = null;
    let reconnectTimer = null;
    let isConnected = false;
    let deviceOnline = false;

    function init() {
        connect();
    }

    function connect() {
        if (ws && (ws.readyState === WebSocket.CONNECTING || ws.readyState === WebSocket.OPEN)) return;

        try {
            ws = new WebSocket(WS_URL);
        } catch (e) {
            console.warn('[WS] Cannot connect:', e);
            scheduleReconnect();
            return;
        }

        ws.onopen = function () {
            console.log('[WS] Connected to server:', WS_URL);
            isConnected = true;
            updateConnectionUI('server');
        };

        ws.onmessage = function (event) {
            try {
                const data = JSON.parse(event.data);

                // Handle device online/offline notifications from server
                if (data.type === 'device_status') {
                    deviceOnline = data.online;
                    updateConnectionUI(data.online ? 'online' : 'offline');
                    return;
                }

                // Handle telemetry from ESP32
                if (data.type === 'telemetry') {
                    deviceOnline = true;
                    updateConnectionUI('online');
                    handleTelemetry(data);
                    return;
                }

            } catch (err) {
                console.error('[WS] Parse error:', err);
            }
        };

        ws.onclose = function () {
            if (isConnected) console.warn('[WS] Disconnected');
            isConnected = false;
            deviceOnline = false;
            updateConnectionUI('offline');
            scheduleReconnect();
        };

        ws.onerror = function () {
            ws.close();
        };
    }

    function scheduleReconnect() {
        if (reconnectTimer) clearTimeout(reconnectTimer);
        reconnectTimer = setTimeout(connect, 3000);
    }

    function updateConnectionUI(status) {
        const chip  = document.getElementById('conn-chip');
        const label = document.getElementById('conn-label');
        if (!chip || !label) return;

        if (status === 'online') {
            chip.className = 'conn-chip online';
            label.textContent = 'ESP32 ONLINE';
        } else if (status === 'server') {
            chip.className = 'conn-chip demo';
            label.textContent = 'SERVER OK';
        } else {
            chip.className = 'conn-chip offline';
            label.textContent = 'OFFLINE';
        }
    }

    function handleTelemetry(data) {
        // Dispatch telemetry to the app.js updateFromWS function
        if (window.AquaSense && window.AquaSense.updateFromWS) {
            window.AquaSense.updateFromWS(data);
        }
    }

    function sendCommand(data) {
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify(data));
        } else {
            console.warn('[WS] Not connected — command dropped');
        }
    }

    window.ESP32WS = {
        init,
        send: sendCommand,
        isConnected: () => isConnected && deviceOnline
    };

    document.addEventListener('DOMContentLoaded', init);
})();
