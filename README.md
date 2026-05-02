# Calibrador de Neumaticos - Farina Vending

## Estructura
```
calibrador_neumaticos/
├── include/
│   ├── config.h          <- configuracion del producto
│   ├── pins.h            <- pinout del hardware
│   ├── calibrador.h      <- logica principal
│   ├── mqtt_handlers.h   <- topics MQTT propios
│   └── fs_compat.h       <- filesystem propio
├── src/
│   ├── main.cpp
│   └── common/
│       ├── calibrador.cpp
│       ├── mqtt_handlers.cpp
│       └── fs_compat.cpp
├── data/                 <- LittleFS (web UI, config JSON)
└── platformio.ini
```

## Libreria compartida
Los modulos comunes viven en `../lib_shared/` y se incluyen via:
```ini
lib_extra_dirs = ../lib_shared
```

Modulos disponibles: logger, display, connectivity, platform, compat

## Compilar
```bash
pio run -e esp8266_calibrador
pio run -t uploadfs -e esp8266_calibrador
```
