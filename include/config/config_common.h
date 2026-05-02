#pragma once

// =============================================================================
// Configuracion comun - compartida entre todos los modulos
// =============================================================================

// WiFi Configuration
#ifndef WIFI_SSID
  #define WIFI_SSID           "YOUR_SSID"
#endif

#ifndef WIFI_PASSWORD
  #define WIFI_PASSWORD       "YOUR_PASSWORD"
#endif

// MQTT Configuration
#ifndef MQTT_BROKER
  #define MQTT_BROKER         "192.168.1.100"
#endif

#ifndef MQTT_PORT
  #define MQTT_PORT           1883
#endif

#ifndef MQTT_USER
  #define MQTT_USER           ""
#endif

#ifndef MQTT_PASSWORD
  #define MQTT_PASSWORD       ""
#endif

#ifndef MQTT_CLIENT_ID
  #define MQTT_CLIENT_ID      "calibrador_esp32"
#endif

// Logging
#ifndef LOG_LEVEL
  #define LOG_LEVEL           3  // 0=ERROR, 1=WARN, 2=INFO, 3=DEBUG
#endif

// Device Configuration
#ifndef DEVICE_ID
  #define DEVICE_ID           "DEVICE_001"
#endif

#ifndef FIRMWARE_VERSION
  #define FIRMWARE_VERSION    "1.0.0"
#endif
