// include/constants.h
#pragma once

// --- Config común del display ---
#define LCD_COLS 16
#define LCD_ROWS 2
#define TIEMPO_ENTRE_BLINK 500


constexpr uint16_t PULSE_MIN[3] = { /*minimoPulso1*/ 85, /*2*/ 185, /*3*/ 480 };
constexpr uint16_t PULSE_MAX[3] = { /*maximoPulso1*/ 110, /*2*/ 200, /*3*/ 545 };

#define LIMITE_SUPERIOR_TEMP 80
#define LIMITE_INFERIOR_TEMP 77
#define TIME_OUT_DELIVER_COSPEL 30000
#define TIME_OUT_DELIVER_ONLINE 60000
#define TIEMPO_ENTRE_LECTURAS_DEFAULT 30000
#define TIEMPO_ENTRE_LECTURAS_PRUEBA 5000
#define TIME_OUT_PROGRAMACION 300000

// Nombres de las 4 versiones de carrousel (modificar según necesidad)
#define CARROUSEL_VERSION_0  "OPCION 0"
#define CARROUSEL_VERSION_1  "OPCION 1"
#define CARROUSEL_VERSION_2  "OPCION 2"
#define CARROUSEL_VERSION_3  "OPCION 3"
