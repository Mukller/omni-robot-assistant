# Assembly Guide

## Step 1 — Print STL Parts
Print from `stl/` folder:
- `Bottom_Plate.stl` — 20% infill, PETG
- `Middle_Plate.stl` — 20% infill, PETG
- `Upper_Plate.stl` — 20% infill, PETG
- `Motor_Bracket_L.stl` / `Motor_Bracket_R.stl` — 40% infill
- `Caster_Bracket.stl` — 30% infill
- `Head.stl` — 15% infill, decorative

## Step 2 — Mount Motors
1. Press JGA25-370 motors into Motor Brackets
2. Attach brackets to Bottom Plate with M3×10 screws
3. Install encoders on motor shafts (press-fit)

## Step 3 — Wheel Assembly
1. Press 48mm omni wheels onto motor shafts (6mm keyed)
2. Mount caster ball in front bracket

## Step 4 — Electronics
1. Mount L298N on Middle Plate with M3 standoffs
2. Mount ESP32 DevKit beside L298N
3. Route wiring through cable holes

## Step 5 — LiDAR
1. Mount YDLidar X4 on Upper Plate centre post
2. Connect USB to Raspberry Pi (USB→Serial adapter)
   OR direct UART2 to ESP32 (pins 16/17)

## Step 6 — Raspberry Pi
1. Mount RPi 4 on Upper Plate with brass standoffs
2. Connect 5V/3A USB-C power from buck converter
3. Connect to network via WiFi (configure in `/etc/netplan`)
