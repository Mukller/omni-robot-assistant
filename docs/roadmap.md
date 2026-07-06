# Roadmap

## v1.0 — Базовая платформа ✅ (Апрель 2026)

- [x] Дифференциальный привод JGA25-370 + L298N
- [x] Квадратурные энкодеры (1920 тиков/об)
- [x] YDLidar X4 через UART
- [x] microROS UDP (ESP32 ↔ RPi4)
- [x] Cartographer 2D SLAM
- [x] Nav2 навигация (DWB controller)
- [x] explore_lite автономное исследование
- [x] robot_localization EKF (odom + IMU)
- [x] BNO055 IMU (NDOF режим)
- [x] INA219 мониторинг тока (2 канала)
- [x] LiPo 3S мониторинг напряжения
- [x] OTA обновления прошивки
- [x] Gazebo симуляция (SDF модель)
- [x] CI/CD (PlatformIO GitHub Actions)

---

## v1.1 — Стабилизация и баги ✅ (Июль 2026)

- [x] **Исправлена постоянная e-stop** — cmd_vel timeout теперь auto-clear; батарея/перегрузка лэтчатся
- [x] **Startup e-stop** — `_lastCmdMs` инициализируется `millis()`, нет ложного таймаута при старте
- [x] **PID anti-windup** — интеграл не накапливается при Ki=0
- [x] **Midpoint odometry** — используется угол θ+dθ/2 для корректной интеграции положения
- [x] **IMU в Cartographer** — `use_imu_data=true`, `tracking_frame="imu_link"`
- [x] **EKF** — 50Гц, полная матрица шума процесса, `odom0_differential: false`
- [x] **Gazebo launch** — xacro обрабатывается через `xacro.process_file()` (не raw read)
- [x] **SLAM launch** — добавлен `robot_state_publisher` (TF был сломан)
- [x] **Nav2 тюнинг** — max_vel_x=0.22 м/с, inflation_radius=0.18 м, robot_radius=0.12 м
- [x] **CI** — PlatformIO + ROS2 Build GitHub Actions
- [ ] **Velocity smoother** — плавные профили скорости для снижения wear на моторах
- [ ] **Waypoint following** — маршрутные точки через RViz (Nav2 waypoint follower уже включён)
- [ ] **Multi-floor mapping** — сохранение/загрузка нескольких карт для разных помещений
- [ ] **Auto-docking** — возврат к зарядной станции при низком заряде батареи

---

## v1.2 — Перцепция (Q4 2025)

- [ ] **Depth camera** — Intel RealSense D435i или OAK-D Lite
  - 3D obstacle detection
  - Object recognition (YOLO)
  - Person following mode
- [ ] **3D costmap** — интеграция depth camera с Nav2 voxel layer
- [ ] **Semantic SLAM** — маркировка объектов на карте
- [ ] **QR-code localization** — коррекция позиции через QR-коды на стенах
- [ ] **LED ring** — визуальная индикация состояния (WS2812B, GPIO)

---

## v1.3 — Голосовое управление (Q1 2026)

- [ ] **Wake word** — офлайн распознавание ("Hey Loki")
- [ ] **STT** — Whisper на RPi4 (офлайн)
- [ ] **TTS** — espeak / Piper
- [ ] **LLM интеграция** — Ollama (llama3.2) для команд на естественном языке
- [ ] **Навигация по словам** — "поедь на кухню", "вернись назад"

---

## v2.0 — Аппаратная ревизия (2026)

- [ ] **Собственная PCB** — объединить ESP32 + L298N + INA219 + power management на одной плате
- [ ] **Беспроводная зарядка** — Qi-совместимая зарядная станция
- [ ] **BLDC моторы** — JGA25-BL2418 для меньшего шума и большей эффективности
- [ ] **Расширенный корпус** — крепления для манипулятора (6-DOF arm)
- [ ] **ROS2 Jazzy** — переход на LTS версию

---

## Известные ограничения (текущая версия)

| Ограничение | Причина | Workaround |
|-------------|---------|------------|
| Нет работы без WiFi | microROS требует UDP-соединения | Запустить агент локально на ESP32 (ESP-IDF) |
| Картограф работает только 2D | Нет 3D сенсора | Добавить depth camera в v1.2 |
| Max скорость 0.26 м/с при SLAM | Ограничение Cartographer при высоком угловом движении | Увеличить в nav params для open areas |
| L298N греется при >1А | КПД ~60% | Заменить на TB6612FNG (~95% КПД) |
| RPi4 boot 30-45 сек | Systemd + ROS2 | Использовать delayed launch или preempt-rt kernel |

---

## Contribution

Приветствуются PR по следующим направлениям:
- Поддержка новых LiDAR моделей (через kaiaai/LDS)
- Портирование на другие платформы (Raspberry Pi Zero 2W, Jetson Nano)
- Тесты для firmware (Unity / GoogleTest)
- Документация на других языках

Открывайте Issue перед началом крупных изменений.
