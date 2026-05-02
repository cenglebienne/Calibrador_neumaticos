#ifndef MEDICIONES_H
#define MEDICIONES_H

#include <config.h>
#include <constants.h>

enum class ResultadoMedicion : uint8_t {
    EN_CURSO  = 0,
    ESTABLE   = 1,
    INESTABLE = 2
};

extern float   PresionMedidaPromedio;
extern uint8_t PresionRedondeada;
extern float   g_Presion_PSI;

void              Lectura();
ResultadoMedicion EstadoPicoV3();

#endif
