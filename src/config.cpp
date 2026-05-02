#include <config.h>
#include "app_state.h"

int g_num_mensaje = 1;
bool g_gratis = false;
bool g_conectado_wifi = false;
bool g_conectadoMQTT  = false;
bool g_pago_qr_disponible = false;

Config g_cfg{};

bool g_cospel_in = false;

const char *mqttHost     = "66.97.47.118";
const int   mqttPort     = 1883;
const char *mqttUser     = "farinamqtt";
const char *mqttPassword = "fv#297esp8266";

const char* COMPANY  = "FARINA VENDING";
const char* TELEFONO = "0223-155218985";

// Valores por defecto en RAM — Storage_Bootstrap() los sobreescribe con Flash
Entorno    Mientorno     = {};
Contadores Miscontadores = {};
Valores    Misvalores    = {};
UpdateCfg  Miupdate      = {};
