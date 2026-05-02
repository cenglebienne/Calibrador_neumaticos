#ifndef AUDIO_H
#define AUDIO_H

#include <config.h>
#include <constants.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>


extern bool isDFPlayerAvailable;

void testDeSonido();
void printDetail(uint8_t type, int value);
void reproducirMensaje(uint8_t trackNumber, unsigned long Espera) ;
void Audio_Error_Pico();
void buzzerCorto();
void buzzerLargo();
void doblePitidoCorto() ;
void triplePitidoLargo();
void triplePitidoCorto();
void handleDFPlayerError();
void mensajeIndicacion();

#endif
