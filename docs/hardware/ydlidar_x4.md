# YDLidar X4 — Подробные характеристики

## Технические параметры

| Параметр | Значение |
|----------|----------|
| Принцип измерения | Триангуляция |
| Длина волны лазера | 775–795 нм (класс безопасности I) |
| Диапазон измерений | 0.12 – 10 м |
| Частота измерений | 5000 раз/с |
| Частота сканирования | 6–12 Гц (8 Гц номинальная) |
| Угловое разрешение | 0.3°–0.5° при 8 Гц |
| Угол обзора | 360° |
| Интерфейс | UART TTL, 115200 бод |
| Напряжение питания | 5В ±5% |
| Средняя мощность | < 2 Вт |
| Ток потребления | ~400 мА |
| Рабочая температура | −10°C до +60°C |
| Масса | ~180 г |
| Габариты | 98.5 × 70.5 × 39.5 мм |

## Подключение к ESP32

```
YDLidar X4      ESP32
-----------     -----
TX         -->  GPIO16 (RX2)
RX         <--  GPIO17 (TX2)
VCC (5V)   -->  5V
GND        -->  GND
PWM/EN     -->  GPIO через транзистор (для управления мотором)
```

> **Внимание**: YDLidar X4 требует отдельного управления мотором через сигнал M_EN.
> На плате Loki предусмотрен транзисторный ключ.

## Инициализация в firmware

```cpp
#include <LDS.h>
#include <LDS_YDLIDAR_X4.h>  // из kaiaai/LDS

LDS_YDLIDAR_X4 lidar;
HardwareSerial LidarSerial(2);  // UART2

void setup() {
    LidarSerial.begin(115200, SERIAL_8N1, 16, 17);  // RX=16, TX=17
    lidar.init();
    lidar.begin(LidarSerial, [](uint16_t angle, uint16_t dist, uint8_t quality, bool scan_start) {
        // callback: angle=0..35999 (в сотых долях градуса), dist в мм
    });
}

void loop() {
    lidar.loopOnce();
}
```

## Альтернативные LiDAR (drop-in замена)

| Модель | Дальность | Цена |
|--------|-----------|------|
| YDLidar X4 | 10 м | ~$60 |
| RPLIDAR A1 | 12 м | ~$99 |
| Xiaomi LDS02RR | 6 м | ~$15 (б/у) |
| Neato XV11 | 6 м | ~$20 (б/у) |

Все поддерживаются библиотекой kaiaai/LDS без изменения кода ROS2.
Для смены модели LiDAR достаточно заменить `LDS_YDLIDAR_X4` на нужный класс.

## Ссылки
- [kaiaai/LDS библиотека](https://github.com/kaiaai/LDS) — 20+ поддерживаемых моделей
- [Awesome 2D LiDARs](https://github.com/kaiaai/awesome-2d-lidars) — сравнение всех доступных 2D LiDAR
- [Туториал по подключению LD14P к ESP32](https://kaia.ai/blog/tutorial-connect-ld14p-lidar/)
