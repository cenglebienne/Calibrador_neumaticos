#pragma once
#include "config.h" 

void app_core_setup(const Config& cfg);
void app_core_loop();
void ejecutar_OTA_Remoto();
void temperatura_set_intervalo(uint32_t ms);
void temperatura_forzar_lectura();
void temperatura_pausar_sensado();
void temperatura_reanudar_sensado();