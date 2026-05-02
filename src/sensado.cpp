#include "sensado.h"
#include "app_state.h" 
#include "pins.h" // <--- CORRECCIÓN VITAL: Ahora carga el de la placa correcta
#include "config.h" // Asegurate de incluir esto arriba de todo en sensado.cpp


// Variables globales para la interrupción (deben ser 'volatile' para evitar bugs de memoria)
volatile uint32_t pulseInTimeBegin = 0;
volatile uint32_t pulseInTimeEnd = 0;
volatile uint32_t pulseDuration = 0;
volatile bool g_entroPulso = false;

// Variables globales de sensado
bool g_calentando = false;
float g_temperatura = 0.0;
uint32_t g_timer_sensado = 0;
uint32_t g_timer_mensajes = 0; // Temporizador global para la pantalla



void ISR_PREFIX buttonPinInterrupt()
{
    if (digitalRead(pin_moneda) == LOW)
    {
        // start measuring
        pulseInTimeBegin = millis();
    }
    else
    {
        // stop measuring
        pulseInTimeEnd = millis();
        pulseDuration = pulseInTimeEnd - pulseInTimeBegin;
        if ((pulseDuration > minimoPulso) && (pulseDuration < maximoPulso))
        {
            g_cospelIn = true;
            g_entroPulso = false;
        }
        else
            g_entroPulso = true;
    }
}