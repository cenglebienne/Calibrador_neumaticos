#include <Arduino.h>
#include "config.h"
#include "constants.h"       
#include "app_state.h"
#include "http_server_app.h" 
#include "vending_machine.h" 
#include "platform_hooks.h"
#include "app_core.h"
#include "display_hal.h"
#include "pins.h"
#include "programas.h"
#include "memory_api.h"
#include "system_init.h"
#include "fs_compat.h"

 uint32_t minimoPulso = 0;
 uint32_t maximoPulso = 0;
 uint32_t Ultimo_sensado = 0;

// Banderas de estado
bool _POSBrindandoServicio = false;
bool go_prog = false;

volatile bool g_cospelIn = false;

// Temporizador para el modo Access Point
uint32_t t_inicio_ap = 0;

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n=== Boot Farina Vending ===");

  boot_load_config(g_cfg);
  
  // 1. Configurar Hardware de Bajo Nivel (inicializa pines, incluido Puls_prog)
  plat_setup(g_cfg);

    Serial.println("Starting...");
    delay(500);
    digitalWrite(Pin_Buzzer, HIGH);
    delay(100);
    digitalWrite(Pin_Buzzer, LOW);
    delay(20);
    digitalWrite(Pin_Buzzer, HIGH);
    delay(100);
    digitalWrite(Pin_Buzzer, LOW);
    delay(20);

    // Inicialización del sistema
    system_init_filesystem();
    system_init_display();
    // system_init_audio();
    system_init_connectivity();

}

void loop() {
    // ==========================================================
    // MODO ACCESS POINT (Exclusivo y Bloqueante)
    // ==========================================================
    if (g_levantarAP) {
        // Solo atendemos peticiones a la página web
        webserver_loop(); 
        
        // Timeout de seguridad de 5 Minutos (300.000 ms)
        if (millis() - t_inicio_ap > TIME_OUT_PROGRAMACION) {
            Serial.println("[MAIN] Timeout de AP alcanzado. Reiniciando equipo...");
            Set_Flash_Bool("Levantar_AP", false);
            delay(1000);
            ESP.restart();
        }
        return; // El 'return' impide que se lea el validador, botones o FSM
    }

    // ==========================================================
    // MODO NORMAL (Wi-Fi Station + Vending)
    // ==========================================================
    app_core_loop();  
    plat_poll();
    
    // Ejecutar FSM de Menú o FSM de Vending
    if (menu_activo()) {
        menu_loop(); 
    } else {
        maquina_vending_loop(); 
    }
}
