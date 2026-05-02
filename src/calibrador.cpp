#include "calibrador.h"
#include "config.h"
#include "pins.h"
#include "system_init.h"
// #include <constants.h>
#include "logger.h"

EstadoCalibrador estado_calibrador = REPOSO;

void calibrador_init() {
    estado_calibrador = REPOSO;
    system_init_pins();  // Inicializar pines del calibrador
    // TODO: inicializar sensor, etc.
}

void calibrador_loop() {
    // TODO: maquina de estados del calibrador

    // Ejemplo: verificar botones para iniciar servicio
    // if (digitalRead(Pin_Pulsador_1) == LOW || digitalRead(Pin_Pulsador_2) == LOW) {
    //     // TODO: lógica de servicio
    //     Serial.println("Botón presionado");
    // }
}

void calibrador_iniciar_ciclo(float presion_objetivo_psi) {
    // TODO: logica de inflado
}

void calibrador_detener() {
    estado_calibrador = REPOSO;
    // TODO: cerrar valvulas
}

float calibrador_leer_presion() {
    // TODO: leer sensor analogico
    return 0.0f;
}
