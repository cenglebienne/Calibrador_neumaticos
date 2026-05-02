#pragma once
#include <Arduino.h>
#include "config.h"
#include "app_state_common.h"


#define CAMPO_URL_LENGTH 120
#define CAMPOS_CONFIG_LENGTH 41



struct Entorno {
  char CLIENTE_WIFI_ssid[CAMPOS_CONFIG_LENGTH];
  char CLIENTE_WIFI_clave[CAMPOS_CONFIG_LENGTH];
  bool CLIENTE_WIFI_Habilitado;
  uint32_t CLIENTE_WIFI_segundosEsperarConexion;

  char MQTT_servidor[64];
  char MQTT_topic[64];
  char MQTT_usuario[32];
  char MQTT_clave[32];

  char EQUIPO_serie[32];
  bool EQUIPO_inicializado;

  uint16_t MQTT_GPRS_segundosEntreRafagas;
  uint8_t  MQTT_GPRS_intentosPorRafagas;
};

struct Contadores {
  uint32_t Cont_Parcial_Cortesia;
  uint32_t Cont_Parcial_Dinero;
  uint32_t Cont_Total_Dinero;
  uint32_t Cont_Parcial_Cospeles;
  uint32_t Cont_Total_Cospeles;
  uint32_t Cont_Parcial_OnLine;
  uint32_t Cont_Total_Online;
  uint32_t Cont_Total_Cortesia;
};

struct Valores {
  bool    Servicio_Gratis;
  uint8_t Version_Carrousel; // 0-3, textos en constants.h
  bool    Audio_Encendido;
  bool    Opcion_Extra_1;
  bool    Opcion_Extra_2;
  bool    Opcion_Extra_3;
  bool    Display_I2C;
  int32_t Tiempo_Programado;
  int32_t Validador;
  bool    Sensor_Temp_Activado;
  char    Empresa[17];   // hasta 16 chars + '\0', modificable por comando MQTT 'M'
  char    Telefono[17];  // hasta 16 chars + '\0', modificable por comando MQTT 'M'
};

struct UpdateCfg {
  bool  updateFirmwarePendiente;
  bool  updateFirmwareAlIniciar;
  char  urlFirmware[CAMPO_URL_LENGTH];
  bool  updateFilesystemPendiente;
  bool  updateFilesystemAlIniciar;
  char  urlFilesystem[CAMPO_URL_LENGTH];
};


extern Config g_cfg;
extern Entorno     Mientorno;
extern Contadores  Miscontadores;
extern Valores     Misvalores;
extern UpdateCfg   Miupdate;


