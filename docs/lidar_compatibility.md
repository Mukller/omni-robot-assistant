# LiDAR Compatibility Reference

Supported by the [kaiaai/LDS](https://github.com/kaiaai/LDS) Arduino library used in this project.

## Fully Supported (tested)

| Model | Type | Range | RPM | Interface | Price (approx.) | Notes |
|-------|------|--------|-----|-----------|---------|-------|
| **YDLidar X4** | triangulation | 0.12–10m | 6–12 | UART 115200 | ~$60 | Recommended for this build |
| YDLidar X4-PRO | triangulation | 0.12–10m | 6–12 | UART 128000 | ~$80 | Faster spin |
| YDLidar X3 | triangulation | 0.1–8m | 5–10 | UART 115200 | ~$40 | Budget option |
| YDLidar X3-PRO | triangulation | 0.1–8m | 5–10 | UART 115200 | ~$50 | |
| YDLidar X2 / X2L | triangulation | 0.1–8m | 5–8 | UART 115200 | ~$35 | Smallest, weakest |
| YDLidar SCL | triangulation | 0.1–12m | 5–10 | UART | ~$70 | |
| SLAMTEC RPLIDAR A1 | triangulation | 0.15–12m | 1–10 | UART 115200 | ~$99 | Great quality |
| SLAMTEC RPLIDAR C1 | ToF | 0.05–12m | 5–10 | UART | ~$79 | |
| Neato XV11 / Botvac | triangulation | 0.2–6m | 5 | UART 115200 | ~$20 (used) | Needs adapter board |
| Xiaomi LDS02RR | triangulation | 0.15–6m | 5 | UART 115200 | ~$15 (used) | Needs adapter board |
| LDS08RR | triangulation | 0.15–8m | 5 | UART | ~$20 (used) | |
| 3irobotix Delta-2A | triangulation | 0.15–8m | 6 | UART | ~$25 | Needs adapter board |
| 3irobotix Delta-2B | triangulation | 0.15–8m | 6 | UART | ~$25 | |
| 3irobotix Delta-2D | triangulation | 0.15–8m | 6 | UART | ~$25 | |
| 3irobotix Delta-2G | triangulation | 0.15–8m | 6 | UART | ~$25 | |
| LDROBOT LD14P | triangulation | 0.02–8m | 5–10 | UART 115200 | ~$40 | |
| LDROBOT LD19 | triangulation | 0.02–12m | 5–13 | UART | ~$50 | |
| CAMSENSE X1 | triangulation | 0.1–8m | 5–10 | UART | ~$35 | |

## Experimental (needs testing)

| Model | Notes |
|-------|-------|
| Xiaomi LDS01RR (2nd gen ToF) | Same protocol as Neato XV11 |
| LDROBOT LD06 | Same protocol as LD19 |
| LDROBOT STL-19P / D500 | Same protocol as LD19 |
| Hitachi-LG HLS-LFCD2 / LDS-01 | TurtleBot3 LiDAR |
| YDLidar T-mini Plus / T-mini Pro | ToF |

## YDLidar X4 — Technical Specifications

| Parameter | Value |
|-----------|-------|
| Ranging principle | Triangulation |
| Laser wavelength | 775~795nm (Class I) |
| Detection range | 0.12 – 10m |
| Ranging frequency | 5000 times/s |
| Scan frequency | 6–12Hz (8Hz nominal) |
| Angular resolution | 0.3°–0.5° (at 8Hz) |
| Communication interface | UART, TTL, 115200 baud |
| Power supply | 5V±5% |
| Average power | < 2W |
| Operating temperature | -10°C to 60°C |
| Weight | ~180g |
| Dimensions | ~98.5 × 70.5 × 39.5mm |

## Adapter Boards Required

Some LiDARs need extra hardware for motor control:

- **Xiaomi LDS02RR** → [MakersPet adapter board](https://makerspet.com/store#!/Adapter-for-LDS02RR-LiDAR/p/697091363)
- **Neato XV11** → [GitHub PCB design](https://github.com/makerspet/pcb/tree/main/neato_delta_adapter)
- **3irobotix Delta-2\*** → Same adapter as Neato XV11

## STL Mount Plates

The Loki 200mm frame includes mount plates for all supported LiDARs:

| STL File | LiDAR |
|----------|-------|
| `YDLidar X4 plate.stl` | YDLidar X4 (recommended) |
| `YDLIDAR X3 PRO plate.stl` | YDLidar X3-PRO |
| `YDLIDAR X2 Plate.stl` | YDLidar X2 / X2L |
| `RPLIDAR A1 Plate.stl` | SLAMTEC RPLIDAR A1 |
| `LDS02RR Plate.stl` | Xiaomi LDS02RR |
| `Neato XV11 Plate.stl` | Neato XV11 |

All plates available in the [MakersPet 3D models repo](https://github.com/makerspet/3d_models/tree/main/loki_200mm/stl).
