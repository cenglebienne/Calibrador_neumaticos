#pragma once
#include <Arduino.h>
#include "config.h"
#include "constants.h"

// Variables expuestas al resto del sistema
extern bool g_calentando;
extern float g_temperatura;
extern uint32_t g_timer_sensado;
extern uint32_t g_timer_mensajes; // <-- Agregada para que no de error en main o sensado

// Variables de la interrupción del validador de monedas
extern volatile uint32_t pulseDuration;
extern volatile bool g_entroPulso;

// Funciones
float Lectura_ADC();
bool Determina_Estado(float value, bool estado_actual);
bool PushProg();
bool PushServ();

// ISR del validador de monedas (necesaria para attachInterrupt en ambas plataformas)
#define ISR_PREFIX IRAM_ATTR
void ISR_PREFIX buttonPinInterrupt();