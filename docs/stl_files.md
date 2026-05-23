# 3D Print Files — Список деталей для печати

Все файлы доступны в репозитории [MakersPet 3D models](https://github.com/makerspet/3d_models/tree/main/loki_200mm).

## Основные детали корпуса

| Файл | Описание | Кол-во | Заполнение | Материал | Время печати |
|------|----------|---------|------------|----------|--------------|
| `Bottom plate.stl` | Нижняя плита (моторы, аккумулятор) | 1 | 20% | PETG | ~3ч |
| `Middle plate.stl` | Средняя плита (электроника) | 1 | 20% | PETG | ~2.5ч |
| `Upper plate.stl` | Верхняя плита (LiDAR, RPi) | 1 | 20% | PETG | ~2ч |
| `Motor bracket x2.stl` | Кронштейн мотора (печатает 2 сразу) | 1 файл | 40% | PETG | ~2ч |
| `Caster wheel with break-off pedestal.stl` | Кастер-колесо с подставкой | 1 | 30% | PETG | ~1ч |
| `Head.stl` | Корпус головы (декор) | 1 | 15% | PLA | ~4ч |

## Бамперы

| Файл | Описание | Кол-во | Заполнение |
|------|----------|---------|------------|
| `Bumper front.stl` | Передний бампер | 1 | 25% |
| `Bumper back.stl` | Задний бампер | 1 | 25% |

## Крепления LiDAR

| Файл | LiDAR модель | Примечание |
|------|-------------|------------|
| `YDLidar X4 plate.stl` | **YDLidar X4** (рекомендован) | |
| `YDLIDAR X3 PRO plate.stl` | YDLidar X3-PRO | |
| `YDLIDAR X2 Plate.stl` | YDLidar X2 / X2L | |
| `RPLIDAR A1 Plate.stl` | SLAMTEC RPLIDAR A1 | |
| `LDS02RR Plate.stl` | Xiaomi LDS02RR | Нужен адаптер |
| `Neato XV11 Plate.stl` | Neato XV11 / Botvac | Нужен адаптер |

## Аккумуляторный отсек

| Файл | Описание |
|------|----------|
| `Battery Pack 3x9V Upright.stl` | Отсек для аккумулятора (вертикальный) |
| `Battery Pack 3x9V Upright Lid.stl` | Крышка отсека |
| `Battery 3x9V Upright Pedestal.stl` | Подставка |

## Аксессуары

| Файл | Описание |
|------|----------|
| `Handset Clamp.stl` | Зажим для дистанционного управления |
| `Handset Footrest.stl` | Подставка для пульта |

## Рекомендуемые настройки печати

### PETG (основные детали)
- Температура экструдера: 235–245°C
- Температура стола: 70–80°C
- Скорость: 40–50 мм/с
- Поддержки: только для Bumpers и Caster bracket
- Адгезия: клей-карандаш или Magigoo

### PLA (декор — Head)
- Температура экструдера: 200–215°C
- Температура стола: 60°C
- Скорость: 50–60 мм/с
- Поддержки: да (для внутренних поверхностей)

## Болты и крепёж

| Тип | Кол-во | Применение |
|-----|--------|------------|
| M3 × 10 | 16 | Моторы → кронштейны |
| M3 × 6 | 20 | Платы → стойки |
| M3 × 30 | 8 | Стойки между платами |
| M3 гайки | 30 | Везде |
| M2.5 × 6 | 8 | Raspberry Pi |
| M2.5 стойки 10мм | 4 | RPi стойки |

## Ссылки на файлы

- **STL**: [github.com/makerspet/3d_models/tree/main/loki_200mm/stl](https://github.com/makerspet/3d_models/tree/main/loki_200mm/stl)
- **3MF** (Prusaslicer): [github.com/makerspet/3d_models/tree/main/loki_200mm/3mf](https://github.com/makerspet/3d_models/tree/main/loki_200mm/3mf)
- **Fusion 360**: [github.com/makerspet/3d_models/tree/main/loki_200mm/fusion360](https://github.com/makerspet/3d_models/tree/main/loki_200mm/fusion360)
