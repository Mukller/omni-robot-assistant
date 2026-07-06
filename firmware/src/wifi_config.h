#pragma once
// ──────────────────────────────────────────────────────────────────────────────
// WiFi / microROS configuration
//
// HOW TO SET UP:
//   1. Copy this file to wifi_config.local.h (already in .gitignore)
//   2. Fill in your credentials below
//   3. The build picks up wifi_config.h — keep this template in git
//
// PRODUCTION: store credentials in NVS, not in firmware:
//   Preferences prefs; prefs.begin("wifi"); prefs.getString("ssid", "");
// ──────────────────────────────────────────────────────────────────────────────

// WiFi network your Raspberry Pi is on
#define WIFI_SSID    "YOUR_SSID"
#define WIFI_PASS    "YOUR_PASS"

// Raspberry Pi 4 IP (static preferred; set in dhcpcd.conf or router reservation)
#define AGENT_IP     "192.168.1.100"
#define AGENT_PORT   8888

// OTA update password (change before deployment)
#define OTA_PASSWORD "loki_ota_2026"

// microROS reconnect interval on agent loss (ms)
#define AGENT_RECONNECT_MS 5000
