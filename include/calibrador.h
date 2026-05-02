#pragma once

// =============================================================================
// Logica principal del calibrador de neumaticos
// =============================================================================

void calibrador_init();
void calibrador_loop();
void calibrador_iniciar_ciclo(float presion_objetivo_psi);
void calibrador_detener();
float calibrador_leer_presion();

enum EstadoCalibrador {
  REPOSO,
  REPOSO_GRATIS,
  SELECCIONANDO_PRESION,
  ERROR_PICO,
  CALIBRANDO,
  MOSTRANDO_GRACIAS
};
