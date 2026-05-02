#pragma once
#include <Arduino.h>

// Named indices into the segment lookup table
enum SegChar : uint8_t {
    SEG_0 = 0, SEG_1, SEG_2, SEG_3, SEG_4,
    SEG_5, SEG_6, SEG_7, SEG_8, SEG_9,
    SEG_GUION = 10, SEG_PUNTO = 11, SEG_SLASH = 12,
    SEG_A = 13, SEG_F = 14, SEG_D = 15, SEG_P = 16,
    SEG_I = 17, SEG_S = 18, SEG_OFF = 19,
    SEG_E = 20, SEG_R = 21, SEG_U = 22, SEG_N = 23
};

// Primitive: send a raw segment index to both digits
void Mostrar_7seg(uint8_t decena, uint8_t unidad);

// Numeric display
void Mostrar_Presion(int x);

// Named display states
void Mostrar_UP();
void Mostrar_DN();
void Mostrar_Rayita();
void Mostrar_Off();
void Mostrar_Error();
void Mostrar_Error_Pico();
void Mostrar_Programa(int prog);

// Animated states (call repeatedly from loop)
void Blink();
void Mostrar_Presion_Blink(int presion);
