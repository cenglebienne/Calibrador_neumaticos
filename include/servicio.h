#pragma once
#include <Arduino.h>
#include "app_state.h"
#include "mensajes.h"

#ifndef TIEMPO_SERV_GRATIS
  #define TIEMPO_SERV_GRATIS   5     // minutos de servicio gratuito por defecto
#endif
#ifndef TIEMPO_REFRESCO_7SEG
  #define TIEMPO_REFRESCO_7SEG 200   // ms entre refrescos del display 7-seg
#endif

extern bool           g_Timeout;
extern unsigned long  g_Tiempo_Ahora;
extern unsigned long  g_Tiempo_Ultimo_Refresco;
extern bool           g_Servicio_Cortesia;

void Servicio(bool ServicioPago);
void Seteo_Presion();
