# Troubleshooting Guide

## ESP32 / Firmware

### ESP32 не подключается к WiFi
**Симптомы:** Serial monitor показывает "Connecting to WiFi..." бесконечно.
```
Причины и решения:
1. Проверить SSID/пароль в firmware/src/wifi_config.h
2. Убедиться, что роутер работает в диапазоне 2.4GHz (ESP32 не поддерживает 5GHz)
3. Проверить, что MAC-адрес ESP32 не заблокирован на роутере
4. Попробовать mDNS: ping loki.local
```

### microROS агент не видит ESP32
**Симптомы:** `ros2 topic list` не показывает `/odom`, `/scan`.
```bash
# На RPi: убедиться что агент запущен
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888 -v6

# Проверить UDP порт открыт
sudo ufw allow 8888/udp

# Проверить IP агента в firmware/src/wifi_config.h совпадает с IP RPi
hostname -I  # на RPi
```

### LiDAR не даёт данные
**Симптомы:** `/scan` приходит, но все значения `inf`.
```
1. Проверить питание LiDAR (5V, достаточный ток)
2. Проверить UART подключение: GPIO16 (RX), GPIO17 (TX)
3. YDLidar X4: проверить M_EN пин — мотор должен вращаться
4. Уменьшить min_range до 0.05м в Cartographer config
5. Проверить Serial2.begin(115200, SERIAL_8N1, 16, 17)
```

### Энкодеры не считают тики
**Симптомы:** Одометрия не обновляется.
```
1. GPIO34/35 на ESP32 — только вход, не подключать к 5V напрямую
2. Добавить делитель напряжения 2:1 если энкодер 5В
3. Проверить ISR: attachInterrupt(digitalPinToInterrupt(ENC_L_A), isrA, CHANGE)
4. Проверить, что пины не пересекаются с другими функциями
```

### Перегрев L298N
```
1. L298N нормально греется при нагрузке (до 60°C — норма)
2. Установить радиатор на чип
3. Убедиться, что EN pin не 5В постоянно (использовать PWM)
4. Если ток > 2А → рассмотреть TB6612FNG или DRV8871
```

### OTA обновление не работает
```bash
# Убедиться что ESP32 и ПК в одной сети
pio run --target upload --upload-port loki.local

# Если не находит: узнать IP через Serial monitor
# и использовать: --upload-port 192.168.1.XXX
```

---

## Raspberry Pi 4 / ROS2

### `colcon build` падает с ошибкой памяти
```bash
# Увеличить swap
sudo dphys-swapfile swapoff
sudo sed -i 's/CONF_SWAPSIZE=.*/CONF_SWAPSIZE=2048/' /etc/dphys-swapfile
sudo dphys-swapfile setup && sudo dphys-swapfile swapon

# Собирать с ограниченным параллелизмом
colcon build --executor sequential --parallel-workers 1
```

### Cartographer не строит карту
**Симптомы:** `/map` пустой или не обновляется.
```bash
# Проверить что /scan приходит
ros2 topic hz /scan  # должно быть ~8Hz

# Проверить TF: должен быть base_link → laser
ros2 run tf2_tools view_frames && evince frames.pdf

# Уменьшить min_score в cartographer_lds_2d.lua до 0.55
# Увеличить angular_search_window до math.rad(30.)
```

### Nav2 не строит путь
**Симптомы:** "Failed to get plan" в логах.
```bash
# Проверить что карта загружена
ros2 topic echo /map --once | head -5

# Сбросить costmap
ros2 service call /global_costmap/clear_entirely_global_costmap \
  nav2_msgs/srv/ClearEntireCostmap {}

# Убедиться в корректной начальной позиции (2D Pose Estimate в RViz)
```

### Robot застрял в одном месте
**Симптомы:** `explore_lite` не находит новых фронтиров.
```bash
# Уменьшить min_frontier_size в explore_lite.yaml (с 0.75 до 0.5)
# Увеличить inflation_radius в costmap (с 0.1 до 0.15) — робот будет осторожнее
# Перезапустить explore_lite: ros2 lifecycle_node list → restart
```

### RViz не показывает робота (URDF не загружен)
```bash
# Убедиться что robot_state_publisher запущен
ros2 node list | grep robot_state

# Проверить topic
ros2 topic echo /robot_description --once | head -3
```

---

## Питание и электроника

### Робот перезагружается при движении
**Причина:** просадка питания — 5V buck конвертер не справляется.
```
Решение:
1. Использовать buck конвертер с током > 3А для RPi (рекомендуется 5А)
2. Добавить конденсатор 1000мкФ 10В на выходе buck конвертера
3. Проверить пайку разъёмов XT60
4. Измерить напряжение под нагрузкой: должно быть 4.9–5.1В
```

### INA219 показывает отрицательный ток
```
Подключить shunt-резистор в правильном направлении:
+ сторона → к мотору (нагрузке)
- сторона → к питанию
```

### BNO055 не калибруется (sys = 0)
```
1. Убедиться, что робот стоит на ровной поверхности (accel cal)
2. Крутить робот в 6 позициях (mag cal)
3. Двигаться медленно восьмёркой (gyro cal)
4. Если магнетометр мешает — отключить в config: OPERATION_MODE_IMUPLUS
```

---

## Сборка и механика

### Мотор вращается, но колесо не двигается
```
→ Проверить стопорный винт (set screw) на муфте вала 6мм
→ Использовать Loctite (синий, 243) на резьбе
```

### Кастер-колесо соскальзывает
```
→ Смазать шариковый кастер силиконовой смазкой
→ Проверить что кастер касается пола (регулировочные шайбы)
```

### Вибрация при движении
```
→ Проверить балансировку колёс (пластиковые заглушки)
→ Убедиться что роликовые оси omni-wheels не шатаются
→ Затянуть крепления моторных кронштейнов
```
