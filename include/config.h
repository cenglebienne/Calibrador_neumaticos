#pragma once
#include <stdint.h>
#include <config/config_common.h>   // hereda configuracion comun

// =============================================================================
// Configuracion especifica - Calibrador de neumaticos
// =============================================================================

#ifndef PRODUCTO
  #define PRODUCTO  "CALIBRADOR NEUMATICOS"
#endif

// Topics MQTT propios del calibrador
#define MQTT_TOPIC_PRESION    "calibrador/presion"
#define MQTT_TOPIC_ESTADO     "calibrador/estado"
#define MQTT_TOPIC_CONFIG     "calibrador/config"
#define MQTT_TOPIC_CMD        "calibrador/cmd"

// Filesystem - archivos de configuracion
#define FS_CONFIG_FILE        "/config.json"
#define FS_LOG_FILE           "/log_actual.txt"
#define FS_VERSION_FILE       "/versionfilesystem.txt"

// Timeouts
#define TIMEOUT_INFLADO_MS    30000   // 30 seg maximo por ciclo
#define PRESION_MAX_PSI       50
#define PRESION_MIN_PSI       10


#define HTTP_PORT          80
#define VERSION_FS_PATH    "/versionfilesystem.txt"

// ── Wi-Fi (timeouts por defecto) ────────────────────────────────
#define WIFI_CONNECT_TIMEOUT_MS  10000UL
#define WIFI_RETRY_MS            5000UL
// config.h


#ifndef PIN_FTP_ENABLE
  #define PIN_FTP_ENABLE -1    // <0 => ignorar digitalWrite
#endif

#ifndef LOG_ACTUAL
  #define LOG_ACTUAL "/log.txt"
#endif

#ifndef MAX_LOG_SIZE
  #define MAX_LOG_SIZE (100UL * 1024UL)   // ajustá tu límite
#endif


  #define VERSION_FIRMWARE "1.0.0"



struct Config {
  bool display_use_i2c = true;
  bool wifi_mode_ap    = false;
  int      validador       = 3;      // 1..3
  uint16_t min_pulse       = 0;      // ← elegido según validador
  uint16_t max_pulse       = 0;      // ← elegido según validador
  
};

bool boot_load_config(Config& out);  // <-- corre primero, lee solo lo necesario
