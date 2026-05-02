#pragma once

// =============================================================================
// Filesystem - especifico del calibrador
// Estructura de archivos propia
// =============================================================================

bool fs_init();
bool fs_cargar_config();
bool fs_guardar_config();
void fs_log(const char* mensaje);
