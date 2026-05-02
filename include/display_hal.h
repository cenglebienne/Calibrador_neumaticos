#pragma once
#include <Arduino.h>

// API única para todo el código
void display_begin(bool useI2C);             // ESP8266: siempre true; ESP32: según flag
void display_clear();
void display_clearRow(uint8_t row);          // borra una fila (16 espacios)
void display_setCursor(uint8_t col, uint8_t row);
void display_print(const char* s);
void display_print_int(int v);
void display_center(const char* s, uint8_t row);  // centra texto en 16 columnas

// ESP32 only: inicializa el tipo de display OPUESTO al configurado y le escribe un aviso.
// No altera los punteros HAL activos. Si el hardware incorrecto está instalado, el mensaje queda fijo.
void display_warn_other(bool main_is_i2c, const char* line1, const char* line2);
