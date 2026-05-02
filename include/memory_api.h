#pragma once
#include <Arduino.h>
#include "app_state.h"

// Constantes que usás
#ifndef CAMPO_URL_LENGTH
#define CAMPO_URL_LENGTH 120
#endif
#ifndef CAMPOS_CONFIG_LENGTH
#define CAMPOS_CONFIG_LENGTH 41
#endif

// Mismo “contrato” para ambos MCUs
void Get_Flash_All();
void Get_Flash_Contadores();

void Save_Flash_Entorno();
void Save_Flash_Contadores();
void Save_Flash_ValorServicio();
void Save_Flash_Actualizar();

void Inicializar_Flash();
void Storage_Bootstrap();
void Set_Flash_Bool(const char *key, bool valor);
bool Get_Flash_Bool(const char *key);


