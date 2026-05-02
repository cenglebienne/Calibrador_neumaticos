#include "system_init.h"
#include "pins.h"
#include "config.h"
#include <Arduino.h>
#include <LittleFS.h>
// #include <constants.h>
// #include <display.h>
// #include <audio.h>
// #include <connections.h>

// =============================================================================
// Inicialización del sistema
// =============================================================================

void system_init_pins() {
    // Configurar pines de salida
    pinMode(Data_Pin_1, OUTPUT);
    pinMode(Clock_Pin_1, OUTPUT);
    pinMode(Data_Pin_2, OUTPUT);
    pinMode(Clock_Pin_2, OUTPUT);
    pinMode(Valvula_Inflado, OUTPUT);
    pinMode(Valvula_Desinflado, OUTPUT);
    pinMode(Pin_Buzzer, OUTPUT);
    pinMode(pin_habilitacion, OUTPUT);

    // Configurar pines de entrada
    pinMode(BUSY_PIN, INPUT_PULLUP);
    pinMode(Puls_prog, INPUT);
    pinMode(Pin_Sensor, INPUT);
    pinMode(pin_moneda, INPUT); // Con resistencia externa
    // pinMode(Pin_Pulsador_1, INPUT);
    // pinMode(Pin_Pulsador_2, INPUT);

    // Estados iniciales
    digitalWrite(pin_habilitacion, LOW);
}

void system_init_filesystem() {
    if (!LittleFS.begin()) {
        Serial.println("Error al montar LittleFS");
    } else {
        Serial.println("LittleFS montado correctamente");
        // TODO: listar archivos si necesario
    }
}

void system_init_display() {
    // setupLCD();
    // TODO: inicializar display
}

void system_init_audio() {
    // TODO: inicializar DFPlayer
}

void system_init_connectivity() {
    // TODO: inicializar WiFi, MQTT
}