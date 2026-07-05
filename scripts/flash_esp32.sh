#!/usr/bin/env bash
# Flash Loki firmware to ESP32 via PlatformIO
# Usage: bash flash_esp32.sh [PORT]
#   PORT: optional serial port (e.g. /dev/ttyUSB0 or COM3)
#         If omitted, PlatformIO auto-detects.

set -euo pipefail

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; NC='\033[0m'
info()  { echo -e "${GREEN}[INFO]${NC} $*"; }
warn()  { echo -e "${YELLOW}[WARN]${NC} $*"; }
error() { echo -e "${RED}[ERROR]${NC} $*"; exit 1; }

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
FIRMWARE_DIR="$REPO_ROOT/firmware"
PORT="${1:-}"

# ─── Checks ───────────────────────────────────────────────────────────────────
[ -d "$FIRMWARE_DIR" ] || error "firmware/ directory not found at $FIRMWARE_DIR"
[ -f "$FIRMWARE_DIR/platformio.ini" ] || error "platformio.ini not found"

if ! command -v pio &>/dev/null; then
  warn "PlatformIO CLI not found. Installing..."
  pip install platformio || error "pip install failed. Install Python3 + pip first."
fi

# ─── WiFi config check ────────────────────────────────────────────────────────
WIFI_CONFIG="$FIRMWARE_DIR/src/wifi_config.h"
if [ -f "$WIFI_CONFIG" ]; then
  if grep -q "YOUR_SSID" "$WIFI_CONFIG"; then
    warn "wifi_config.h contains placeholder SSID."
    warn "Edit $WIFI_CONFIG before flashing:"
    warn "  #define WIFI_SSID   \"your_network_name\""
    warn "  #define WIFI_PASS   \"your_password\""
    warn "  #define AGENT_IP    \"192.168.1.100\"  // RPi IP"
    read -p "Continue anyway? (y/N): " -n 1 -r
    echo
    [[ $REPLY =~ ^[Yy]$ ]] || exit 0
  fi
fi

# ─── Build ────────────────────────────────────────────────────────────────────
info "Building firmware..."
cd "$FIRMWARE_DIR"
pio run --environment esp32dev

# ─── Flash ────────────────────────────────────────────────────────────────────
info "Flashing to ESP32..."
if [ -n "$PORT" ]; then
  info "Using port: $PORT"
  pio run --environment esp32dev --target upload --upload-port "$PORT"
else
  info "Auto-detecting serial port..."
  pio run --environment esp32dev --target upload
fi

# ─── Monitor ─────────────────────────────────────────────────────────────────
echo
info "Flash complete! Starting serial monitor (Ctrl+C to exit)..."
echo "Expected output:"
echo "  [WiFi] Connecting to SSID..."
echo "  [WiFi] Connected! IP: 192.168.1.XXX"
echo "  [microROS] Agent IP: 192.168.1.100:8888"
echo "  [LiDAR] YDLidar X4 init OK"
echo "  [IMU] BNO055 sys=3 gyr=3 acc=3 mag=3"
echo

if [ -n "$PORT" ]; then
  pio device monitor --port "$PORT" --baud 115200
else
  pio device monitor --baud 115200
fi
