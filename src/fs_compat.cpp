#include "fs_compat.h"
#include "config.h"
#include "logger.h"

bool fs_init() {
    // TODO: montar LittleFS
    return true;
}

bool fs_cargar_config() {
    // TODO: leer FS_CONFIG_FILE y parsear JSON
    return true;
}

bool fs_guardar_config() {
    // TODO: serializar config a JSON y guardar
    return true;
}

void fs_log(const char* mensaje) {
    // TODO: append a FS_LOG_FILE
}
