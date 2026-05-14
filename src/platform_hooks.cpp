#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "platform_hooks.h"
#include "memory_api.h"
#include "display_hal.h"
#include "logger.h"
#include "pins.h"
#include "sensado.h"

void plat_pins_init() {
    pinMode(Data_Pin_1, OUTPUT);
    pinMode(Clock_Pin_1, OUTPUT);
    pinMode(Data_Pin_2, OUTPUT);
    pinMode(Clock_Pin_2, OUTPUT);
    pinMode(Valvula_Inflado, OUTPUT);
    pinMode(Valvula_Desinflado, OUTPUT);
    pinMode(Pin_Buzzer, OUTPUT);
    pinMode(PIN_HABILITACION, OUTPUT);
    pinMode(BUSY_PIN, INPUT_PULLUP); // Configurar el pin BUSY como entrada
    pinMode(Puls_prog, INPUT);
    pinMode(Pin_Sensor, INPUT);
    pinMode(pin_moneda, INPUT); // Le saqué la resistencia de pull up porque le pongo una externa

    pinMode(Pin_Pulsador_1, INPUT);
    pinMode(Pin_Pulsador_2, INPUT);
}

void plat_adc_init() {
  adcAttachPin(Pin_Sensor);
  analogReadResolution(12);
  analogSetAttenuation(ADC_0db);
}

void plat_setup(const Config& cfg) {
  Serial.println("\n=== Inicializando Hardware ESP32 ===");

  plat_adc_init();
  plat_pins_init();
  attachInterrupt(digitalPinToInterrupt(pin_moneda), buttonPinInterrupt, CHANGE);

  // Inicializar LittleFS una sola vez de forma segura ('true' para formatear en caso de fallo crítico)
  if (!LittleFS.begin(true)) {
    Serial.println("[FS] ERROR: LittleFS no montó");
  } else {
    Serial.println("[FS] LittleFS OK");
  }

  Storage_Bootstrap();

  display_begin(true);
  display_clear();
  display_setCursor(0, 0);
  display_print("Farina Vending");
  display_setCursor(0, 1);
  display_print("Iniciando...");

}

void plat_poll() {
  // webserver_loop() se removió de aquí porque ya lo atiende main.cpp y app_core.cpp
  delay(1);
}