#ifndef PULSADORES_H
#define PULSADORES_H
#include <config.h>
#include <constants.h>



bool PushProg();

// Fuego al soltar — para navegación de menú (un paso por pulsación)
bool Push_P1();
bool Push_P2();

// Fuego inmediato + repeat acelerado — para ajuste de valores (seteo de presión)
bool Push_P1_accel();
bool Push_P2_accel();

#endif