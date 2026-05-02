#pragma once
#include <Arduino.h>

void maquina_vending_setup();
void maquina_vending_loop();
void Incremento_contadores();
void vending_forzar_refresco(); // Fuerza que el display se actualice en el próximo ciclo