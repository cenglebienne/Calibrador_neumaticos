#include "display_7seg.h"
#include "audio.h"
#include "pins.h"
#include "constants.h"
#include <Arduino.h>

// Segment patterns — order must match SegChar enum in display_7seg.h.
// const keeps these in flash instead of RAM.
static const byte SEG_DATA[24][8] = {
    {1,0,1,1,1,1,0,1}, // SEG_0
    {1,0,1,0,0,0,0,0}, // SEG_1
    {1,0,0,1,1,1,1,0}, // SEG_2
    {1,0,1,1,1,0,1,0}, // SEG_3
    {1,0,1,0,0,0,1,1}, // SEG_4
    {0,0,1,1,1,0,1,1}, // SEG_5
    {0,0,1,1,1,1,1,1}, // SEG_6
    {1,0,1,1,0,0,0,0}, // SEG_7
    {1,0,1,1,1,1,1,1}, // SEG_8
    {1,0,1,1,1,0,1,1}, // SEG_9
    {0,0,0,0,0,0,1,0}, // SEG_GUION  (-)
    {0,1,0,0,0,0,0,0}, // SEG_PUNTO  (.)
    {0,1,0,0,0,0,1,1}, // SEG_SLASH  (/)
    {1,1,0,1,0,1,1,1}, // SEG_A
    {1,0,0,0,1,0,1,1}, // SEG_F
    {1,0,1,0,1,1,1,0}, // SEG_D
    {1,0,0,1,0,1,1,1}, // SEG_P
    {0,1,0,1,0,0,0,0}, // SEG_I
    {1,0,0,1,1,1,0,1}, // SEG_S
    {0,0,0,0,0,0,0,0}, // SEG_OFF    (blank)
    {0,0,0,1,1,1,1,1}, // SEG_E
    {0,0,0,0,0,1,1,0}, // SEG_R
    {1,0,1,0,1,1,0,1}, // SEG_U
    {0,0,1,0,0,1,1,0}, // SEG_N
};

static void updateDisplay(const byte* data, int clockPin, int dataPin)
{
    for (int i = 7; i >= 0; i--)
    {
        digitalWrite(clockPin, LOW);
        digitalWrite(dataPin, data[i]);
        digitalWrite(clockPin, HIGH);
    }
}

void Mostrar_7seg(uint8_t valorDecena, uint8_t valorUnidad)
{
    updateDisplay(SEG_DATA[valorDecena % 24], Clock_Pin_1, Data_Pin_1);
    updateDisplay(SEG_DATA[valorUnidad % 24], Clock_Pin_2, Data_Pin_2);
}

void Mostrar_Presion(int x)
{
    Mostrar_7seg(x / 10, x % 10);
}

void Mostrar_UP()     { Mostrar_7seg(SEG_U,     SEG_P);     }
void Mostrar_DN()     { Mostrar_7seg(SEG_D,     SEG_N);     }
void Mostrar_Rayita() { Mostrar_7seg(SEG_GUION, SEG_GUION); }
void Mostrar_Off()    { Mostrar_7seg(SEG_OFF,   SEG_OFF);   }
void Mostrar_Error()  { Mostrar_7seg(SEG_E,     SEG_R);     }

void Mostrar_Error_Pico()
{
    Mostrar_7seg(SEG_E, SEG_R);
    reproducirMensaje(56, 0);
}

void Mostrar_Programa(int prog)
{
    Mostrar_7seg(SEG_P, (uint8_t)prog);
}

void Blink()
{
    static bool intermitente = false;
    static unsigned long t_ultimo = 0;
    if (millis() - t_ultimo > TIEMPO_ENTRE_BLINK)
    {
        intermitente = !intermitente;
        Mostrar_7seg(intermitente ? SEG_GUION : SEG_OFF,
                     intermitente ? SEG_GUION : SEG_OFF);
        t_ultimo = millis();
    }
}

void Mostrar_Presion_Blink(int presion)
{
    static bool intermitente = false;
    static unsigned long t_ultimo = 0;
    if (presion < 5) return;
    if (millis() - t_ultimo > TIEMPO_ENTRE_BLINK)
    {
        intermitente = !intermitente;
        if (intermitente) Mostrar_Presion(presion);
        else              Mostrar_7seg(SEG_OFF, SEG_OFF);
        t_ultimo = millis();
    }
}
