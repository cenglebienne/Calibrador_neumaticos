
#include <Arduino.h>
#include <Preferences.h>
#include "memory_api.h"
#include "config_common.h"
#include "constants.h"

// NOTA: No usamos un Preferences estático compartido porque en ESP32
// los callbacks de AsyncMqttClient corren en una tarea FreeRTOS separada
// (prioridad 3 > loop Arduino prioridad 1) y pueden interrumpir el loop
// principal durante Flash.begin()...Flash.end(), corrompiendo el handle NVS
// interno del objeto compartido. Cada función declara su propio Preferences
// local; NVS es thread-safe a nivel de handles independientes.

void Get_Flash_All() {
  Preferences Flash;
  Flash.begin("My_Flash", true);

  // Cliente WiFi
  strcpy(Mientorno.CLIENTE_WIFI_ssid,  Flash.getString("CWIFI_ssid",  "").c_str());
  strcpy(Mientorno.CLIENTE_WIFI_clave, Flash.getString("CWIFI_clave", "").c_str());
  Mientorno.CLIENTE_WIFI_Habilitado = Flash.getBool("CWIFI_Hab", true);
  Mientorno.CLIENTE_WIFI_segundosEsperarConexion = Flash.getUInt("CWIFI_WSEC", 0);

  // MQTT
  strcpy(Mientorno.MQTT_servidor, Flash.getString("MQTT_servidor", "").c_str());
  strcpy(Mientorno.MQTT_topic,    Flash.getString("MQTT_topic",    "").c_str());
  strcpy(Mientorno.MQTT_usuario,  Flash.getString("MQTT_usuario",  "").c_str());
  strcpy(Mientorno.MQTT_clave,    Flash.getString("MQTT_clave",    "").c_str());

  // Equipo
  strcpy(Mientorno.EQUIPO_serie, Flash.getString("EQUIPO_serie", "").c_str());
  Mientorno.EQUIPO_inicializado = Flash.getBool("EQ_inicializado", false);

  // Levantar AP
  g_levantarAP = Flash.getBool("Levantar_AP", false);

  // Contadores
  Miscontadores.Cont_Parcial_OnLine   = Flash.getUInt("C_P_OnLine",   Miscontadores.Cont_Parcial_OnLine);
  Miscontadores.Cont_Parcial_Cortesia = Flash.getUInt("C_P_Cortesia", Miscontadores.Cont_Parcial_Cortesia);
  Miscontadores.Cont_Parcial_Cospeles = Flash.getUInt("C_P_Cospeles", Miscontadores.Cont_Parcial_Cospeles);
  Miscontadores.Cont_Total_Cospeles   = Flash.getUInt("C_T_Cospeles", Miscontadores.Cont_Total_Cospeles);
  Miscontadores.Cont_Total_Online     = Flash.getUInt("C_T_Online",   Miscontadores.Cont_Total_Online);
  Miscontadores.Cont_Total_Cortesia   = Flash.getUInt("C_T_Cortesia", Miscontadores.Cont_Total_Cortesia);

  // Valor Servicio
  Misvalores.Servicio_Gratis         = Flash.getBool("Servicio_Gratis", false);
  Misvalores.Audio_Encendido = Flash.getBool("Audio_Encendido",  true);
  // Migración: si existe la nueva clave la usa; si no, reconstruye desde los bools viejos
  if (Flash.isKey("Version_Carr")) {
    Misvalores.Version_Carrousel = Flash.getUChar("Version_Carr", 0);
  } else {
    bool ypf    = Flash.getBool("Version_YPF",    false);
    bool soloQR = Flash.getBool("Version_SoloQR", false);
    Misvalores.Version_Carrousel = soloQR ? 2 : (ypf ? 1 : 0);
  }
  Misvalores.Opcion_Extra_1          = Flash.getBool("Opcion_Extra_1",  false);
  Misvalores.Opcion_Extra_2          = Flash.getBool("Opcion_Extra_2",  false);
  Misvalores.Opcion_Extra_3          = Flash.getBool("Opcion_Extra_3",  false);
  Misvalores.Display_I2C             = Flash.getBool("Display_I2C",     true);
  Misvalores.Tiempo_Programado       = Flash.getInt ("Tiempo_Prog",     30);
  Misvalores.Validador               = Flash.getInt ("Validador",       3);
  strncpy(Misvalores.Empresa,  Flash.getString("MSG_Empresa",  EMPRESA).c_str(),  16); Misvalores.Empresa[16]  = '\0';
  strncpy(Misvalores.Telefono, Flash.getString("MSG_Telefono", TELEFONO).c_str(), 16); Misvalores.Telefono[16] = '\0';

  // Actualizaciones
  Miupdate.updateFirmwarePendiente   = Flash.getBool("updateFWEspera", false);
  Miupdate.updateFirmwareAlIniciar   = Flash.getBool("updateFWAlIni",  false);
  strcpy(Miupdate.urlFirmware, Flash.getString("urlFirmware", "").c_str());
  Miupdate.updateFilesystemPendiente = Flash.getBool("updateFSEspera", false);
  Miupdate.updateFilesystemAlIniciar = Flash.getBool("updateFSAlIni",  false);
  strcpy(Miupdate.urlFilesystem, Flash.getString("urlFilesystem", "").c_str());

  Flash.end();
}

void Get_Flash_Contadores() {
  Preferences Flash;
  Flash.begin("My_Flash", true);
  Miscontadores.Cont_Parcial_Cortesia = Flash.getUInt("C_P_Cortesia", Miscontadores.Cont_Parcial_Cortesia);
  Miscontadores.Cont_Parcial_Cospeles = Flash.getUInt("C_P_Cospeles", Miscontadores.Cont_Parcial_Cospeles);
  Miscontadores.Cont_Total_Cospeles   = Flash.getUInt("C_T_Cospeles", Miscontadores.Cont_Total_Cospeles);
  Miscontadores.Cont_Parcial_OnLine   = Flash.getUInt("C_P_OnLine",   Miscontadores.Cont_Parcial_OnLine);
  Miscontadores.Cont_Total_Online     = Flash.getUInt("C_T_Online",   Miscontadores.Cont_Total_Online);
  Miscontadores.Cont_Total_Cortesia   = Flash.getUInt("C_T_Cortesia", Miscontadores.Cont_Total_Cortesia);
  Flash.end();
}

void Save_Flash_Entorno() {
  Preferences Flash;
  Flash.begin("My_Flash", false);
  Flash.putString("CWIFI_ssid",  Mientorno.CLIENTE_WIFI_ssid);
  Flash.putString("CWIFI_clave", Mientorno.CLIENTE_WIFI_clave);
  Flash.putBool  ("CWIFI_Hab",   Mientorno.CLIENTE_WIFI_Habilitado);
  Flash.putUInt  ("CWIFI_WSEC",  Mientorno.CLIENTE_WIFI_segundosEsperarConexion);

  Flash.putString("MQTT_servidor", Mientorno.MQTT_servidor);
  Flash.putString("MQTT_topic",    Mientorno.MQTT_topic);
  Flash.putString("MQTT_usuario",  Mientorno.MQTT_usuario);
  Flash.putString("MQTT_clave",    Mientorno.MQTT_clave);

  Flash.putString("EQUIPO_serie",   Mientorno.EQUIPO_serie);
  Flash.putBool  ("EQ_inicializado",Mientorno.EQUIPO_inicializado);
  Flash.end();
}

void Save_Flash_Contadores() {
  Preferences Flash;
  Flash.begin("My_Flash", false);
  Flash.putUInt("C_P_Cospeles", Miscontadores.Cont_Parcial_Cospeles);
  Flash.putUInt("C_P_OnLine",   Miscontadores.Cont_Parcial_OnLine);
  Flash.putUInt("C_P_Cortesia", Miscontadores.Cont_Parcial_Cortesia);
  Flash.putUInt("C_T_Cospeles", Miscontadores.Cont_Total_Cospeles);
  Flash.putUInt("C_T_Online",   Miscontadores.Cont_Total_Online);
  Flash.putUInt("C_T_Cortesia", Miscontadores.Cont_Total_Cortesia);
  Flash.end();
}

void Save_Flash_ValorServicio() {
  Serial.println("Grabando Valores en Flash");

  // Sincronizar rangos de pulso con el Validador que se va a guardar
  {
    const int idx = constrain(Misvalores.Validador, 1, 3) - 1;
    minimoPulso = PULSE_MIN[idx];
    maximoPulso = PULSE_MAX[idx];
  }

  Preferences Flash;
  Flash.begin("My_Flash", false);
  Flash.putBool ("Servicio_Gratis", Misvalores.Servicio_Gratis);
  Flash.putUChar("Version_Carr",   Misvalores.Version_Carrousel);
  Flash.putBool ("Audio_Encendido", Misvalores.Audio_Encendido);
  Flash.putBool("Opcion_Extra_1",  Misvalores.Opcion_Extra_1);
  Flash.putBool("Opcion_Extra_2",  Misvalores.Opcion_Extra_2);
  Flash.putBool("Opcion_Extra_3",  Misvalores.Opcion_Extra_3);
  Flash.putBool("Display_I2C",     Misvalores.Display_I2C);
  Flash.putInt   ("Tiempo_Prog",  Misvalores.Tiempo_Programado);
  Flash.putInt   ("Validador",    Misvalores.Validador);
  Flash.putString("MSG_Empresa",  Misvalores.Empresa);
  Flash.putString("MSG_Telefono", Misvalores.Telefono);
  Flash.end();
}

void Save_Flash_Actualizar() {
  Preferences Flash;
  Flash.begin("My_Flash", false);
  Flash.putBool  ("updateFWEspera", Miupdate.updateFirmwarePendiente);
  Flash.putBool  ("updateFWAlIni",  Miupdate.updateFirmwareAlIniciar);
  Flash.putString("urlFirmware",    Miupdate.urlFirmware);
  Flash.putBool  ("updateFSEspera", Miupdate.updateFilesystemPendiente);
  Flash.putBool  ("updateFSAlIni",  Miupdate.updateFilesystemAlIniciar);
  Flash.putString("urlFilesystem",  Miupdate.urlFilesystem);
  Flash.end();
}

void Inicializar_Flash() {
  Serial.println("Inicializando Flash");

  // Cliente WiFi (tus defaults)
  strncpy(Mientorno.CLIENTE_WIFI_ssid,  "Farina",    CAMPOS_CONFIG_LENGTH);
  strncpy(Mientorno.CLIENTE_WIFI_clave, "Lacar2025", CAMPOS_CONFIG_LENGTH);
 // strncpy(Mientorno.CLIENTE_WIFI_ssid,  "Berna",    CAMPOS_CONFIG_LENGTH);
  //strncpy(Mientorno.CLIENTE_WIFI_clave, "21431568", CAMPOS_CONFIG_LENGTH);
 Mientorno.CLIENTE_WIFI_Habilitado = true;
  Mientorno.CLIENTE_WIFI_segundosEsperarConexion = 0;

  // MQTT
  strcpy(Mientorno.MQTT_servidor, "66.97.47.118");
  strcpy(Mientorno.MQTT_topic,    "FarinaVending");
  strcpy(Mientorno.MQTT_usuario,  "farinamqtt");
  strcpy(Mientorno.MQTT_clave,    "fv#297esp8266");
  Mientorno.MQTT_GPRS_segundosEntreRafagas = 30;
  Mientorno.MQTT_GPRS_intentosPorRafagas   = 3;

  // Equipo
  strcpy(Mientorno.EQUIPO_serie, "123456789");
  Mientorno.EQUIPO_inicializado = true;

  // Contadores
  Miscontadores.Cont_Parcial_Cortesia = 0;
  Miscontadores.Cont_Parcial_Dinero   = 0;
  Miscontadores.Cont_Total_Dinero     = 0;
  Miscontadores.Cont_Parcial_Cospeles = 0;
  Miscontadores.Cont_Total_Cospeles   = 0;
  Miscontadores.Cont_Parcial_OnLine   = 0;
  Miscontadores.Cont_Total_Online     = 0;

  // Update
  Miupdate.updateFirmwarePendiente   = false;
  Miupdate.updateFirmwareAlIniciar   = false;
  strcpy(Miupdate.urlFirmware, "");
  Miupdate.updateFilesystemPendiente = false;
  Miupdate.updateFilesystemAlIniciar = false;
  strcpy(Miupdate.urlFilesystem, "");

  // Valores
  Misvalores.Tiempo_Programado       = 5;
  Misvalores.Servicio_Gratis         = false;
  Misvalores.Version_Carrousel       = 0;
  Misvalores.Audio_Encendido = true;
  Misvalores.Opcion_Extra_1          = false;
  Misvalores.Opcion_Extra_2          = false;
  Misvalores.Opcion_Extra_3          = false;
  Misvalores.Display_I2C             = false;
  Misvalores.Validador               = 3;
  strncpy(Misvalores.Empresa,  EMPRESA,  16); Misvalores.Empresa[16]  = '\0';
  strncpy(Misvalores.Telefono, TELEFONO, 16); Misvalores.Telefono[16] = '\0';

  // Persistir todo
  Save_Flash_Entorno();
  Save_Flash_Actualizar();
  Save_Flash_Contadores();
  Save_Flash_ValorServicio();
  Set_Flash_Bool("Levantar_AP", false);
}

void Set_Flash_Bool(const char *key, bool valor) {
  Preferences Flash;
  Flash.begin("My_Flash", false);
  Flash.putBool(key, valor);
  Flash.end();
  if (key && (strcmp(key,"Levantar_AP")==0 || strcmp(key,"levantar_AP")==0 || strcmp(key,"Modo_AP")==0)) {
    g_levantarAP = valor;
  }
}

bool Get_Flash_Bool(const char *key) {
  Preferences Flash;
  Flash.begin("My_Flash", true);
  bool v = Flash.getBool(key, false);
  Flash.end();
  return v;
}

static bool ensure_nvs_namespace_myflash() {
  Preferences Flash;
  // Intento RO: si abre, ya existe
  if (Flash.begin("My_Flash", true)) {
    Flash.end();
    return true;
  }
  // No existe: abrir RW para crearlo
  if (!Flash.begin("My_Flash", false)) {
    Serial.println("NVS: no se pudo crear el namespace 'My_Flash'");
    return false;
  }
  // Escribo una key dummy para asegurar la creación física
  Flash.putBool("__ns_created", true);
  Flash.end();
  return true;
}

void Storage_Bootstrap() {
  // 1) Asegurar NVS listo
  ensure_nvs_namespace_myflash();

  // 2) Cargar todo a RAM
  Get_Flash_All();

  // 3) Migración legacy: si existe "WiFi_Activado" (firmware viejo), activar WiFi por defecto y borrar clave
  {
    Preferences Flash;
    Flash.begin("My_Flash", true);
    bool hasLegacy = Flash.isKey("WiFi_Activado");
    Flash.end();
    if (hasLegacy) {
      Mientorno.CLIENTE_WIFI_Habilitado = true;
      Flash.begin("My_Flash", false);
      Flash.putBool("CWIFI_Hab", true);
      Flash.remove("WiFi_Activado");
      Flash.end();
      Serial.println("[MIG] Legacy WiFi_Activado eliminado. WiFi activado por defecto.");
    }
  }

  // 3) Primera vez: defaults y persistencia
  if (!Mientorno.EQUIPO_inicializado) {
    Inicializar_Flash();
    Mientorno.EQUIPO_inicializado = true;
    Save_Flash_Entorno();              // persistir flag dentro del bloque Entorno

    // (opcional) compatibilidad antigua si en algún lado mirabas esa key:
    // Flash.begin("My_Flash", false);
    // Flash.putBool("EQ_inicializado", true);
    // Flash.end();
  }
}
