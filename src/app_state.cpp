#include "app_state.h"
#include "app_state_common.h"

bool     g_levantarAP         = false;
bool     g_pagoQrDisponible   = true;
bool     g_pagoAceptado       = false;
bool     g_pagoelectronico    = false;
bool     g_enviar_log_Por_FTP = false;
bool     g_reinicio_pendiente = false;
bool     g_enServicio         = false;
bool     pingPromedioCalculado= false;
uint16_t VERSION_FIRMWARE2    = 100;
