#ifndef CALIBRACION_H
#define CALIBRACION_H
#include <config.h>

#include <Arduino.h>
#include <display.h>
#include <audio.h>
#include <mediciones.h>


void Calibracion(bool ServicioPago, bool &calcularRatio);
void InfladoLargo();
//float calculoRatio(bool inflado);
#endif