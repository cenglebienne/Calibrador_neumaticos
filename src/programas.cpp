#include <Arduino.h>
#include "config.h"
#include "constants.h"
#include "app_state.h"
#include "sensado.h"
#include "lcd_manager.h"
#include "programas.h"
#include "memory_api.h"
#include "vending_machine.h"
#include "pins.h"
#include "connections.h"  // Para g_conectadoMQTT
#include "task_compat.h"  // Para task_delay()
#include "app_core.h"     // Para temperatura_set_intervalo()
#include "pulsadores.h"

enum EstadoMenu {
  MENU_INACTIVO = 0,
  MENU_1_ESTADISTICAS,
  MENU_2_PRUEBA,
  MENU_3_PROGRAMACION,
  MENU_4_MODO_OP,
  MENU_5_AP,
  MENU_6_SIMULACION, 
  MENU_7_SALIDA
};

static EstadoMenu estado_menu = MENU_INACTIVO;
static int sub_estado = 0;
static bool accion_activa = false;
static uint32_t t_ultimo_boton = 0;
static uint32_t t_aux = 0;
static uint32_t t_inicio_proceso = 0;
static int valor_temp = 0;
static uint32_t t_aux_temp = 0;      // timer refresco temperatura en P2
static bool mostrando_pulso = false; // flag display pulso en P2
bool g_inTest = false;

void menu_cerrar() {
  estado_menu = MENU_INACTIVO;
  g_pagoQrDisponible = true; // Volvemos a habilitar ventas
  digitalWrite(pin_habilitacion, HIGH);
  digitalWrite(Valvula_Inflado, LOW); // Seguridad: Cortar agua si quedó abierta
  digitalWrite(Valvula_Desinflado, LOW); // Seguridad: Cortar agua si quedó abierta
  clearLCD();
  vending_forzar_refresco(); // Fuerza que el display se actualice al volver a la FSM
}

void menu_iniciar() {
  estado_menu = MENU_1_ESTADISTICAS;
  sub_estado = 0;
  accion_activa = false;
  t_ultimo_boton = millis();
  g_pagoQrDisponible = false; // Bloquea pagos mientras se programa
  
  clearLCD();
  displayCenteredMessage("PROGRAMA 1", 0);
  displayCenteredMessage("ESTADISTICAS", 1);
}

bool menu_activo() {
  return estado_menu != MENU_INACTIVO;
}

void menu_loop() {
  if (estado_menu == MENU_INACTIVO) return;

  uint32_t now = millis();
  bool pp_pressed = PushProg();
  bool P1_pressed = Push_P1();
  bool P2_pressed = Push_P2();
  bool cambio_contador = false;
  // Registrar actividad para evitar el timeout
  if (pp_pressed || P1_pressed || P2_pressed ) {
    t_ultimo_boton = now;
  }

  // Timeout global (1 minuto sin tocar nada). No aplica en Autollenado (MENU 4)
  if ((estado_menu != MENU_2_PRUEBA) && (now - t_ultimo_boton > 60000)) {
    Serial.println("Salida por timeout de menu");
    menu_cerrar();
    return;
  }

  // --- NAVEGACIÓN PRINCIPAL ---
  // Si presiona Prog y no está dentro de una configuración, salta al siguiente menú
  if (pp_pressed && !accion_activa) {
    sub_estado = 0;
    estado_menu = (EstadoMenu)((int)estado_menu + 1);
    if (estado_menu > MENU_7_SALIDA) estado_menu = MENU_1_ESTADISTICAS;

    clearLCD();
    switch (estado_menu) {
      case MENU_1_ESTADISTICAS:  displayCenteredMessage("PROGRAMA 1", 0); displayCenteredMessage("ESTADISTICAS", 1); break;
      case MENU_2_PRUEBA:        displayCenteredMessage("PROGRAMA 2", 0); displayCenteredMessage("PRUEBA", 1); break;
      case MENU_3_PROGRAMACION:  displayCenteredMessage("PROGRAMA 3", 0); displayCenteredMessage("PROGRAMACION", 1); break;
      case MENU_4_MODO_OP:       displayCenteredMessage("PROGRAMA 4", 0); displayCenteredMessage("CONFIGURACION", 1); break;
      case MENU_5_AP:            displayCenteredMessage("PROGRAMA 5", 0); displayCenteredMessage("ACCESS POINT", 1); break;
      case MENU_6_SIMULACION:   displayCenteredMessage("PROGRAMA 6", 0); displayCenteredMessage("SIMULACION", 1); break;
      case MENU_7_SALIDA:        displayCenteredMessage("SALIR ?", 0);    displayCenteredMessage("PROG:NO  SERV:SI", 1); break;
      default: break;
    }
    return;
  }

  // --- LÓGICA INTERNA DE CADA MENÚ ---
  switch (estado_menu) {
    
    // ================== P1: ESTADISTICAS ==================
    case MENU_1_ESTADISTICAS:

      if (P1_pressed) {
        sub_estado++;
        if (sub_estado > 7) sub_estado = 1;
        clearLCD();
        cambio_contador = true;
      }
      if (P2_pressed) {
        sub_estado--;
        if (sub_estado < 1) sub_estado = 7;
        clearLCD();  
        cambio_contador = true;
      }  
      if (digitalRead(Pin_Pulsador_1) == LOW && digitalRead(Pin_Pulsador_2) == LOW) {
        sub_estado = 8; // Ir directo a opción de reset si presiona ambos botones
        clearLCD();
        cambio_contador = true;
      }
      if (cambio_contador) {
        switch (sub_estado) {
          case 1: displayCenteredMessage("PARCIAL FICHAS", 0); displayValor(Miscontadores.Cont_Parcial_Cospeles, 6, 1); cambio_contador=false; break;
          case 2: displayCenteredMessage("TOTAL FICHAS", 0); displayValor(Miscontadores.Cont_Total_Cospeles, 6, 1); cambio_contador=false; break;
          case 3: displayCenteredMessage("PARCIAL ONLINE", 0); displayValor(Miscontadores.Cont_Parcial_OnLine, 6, 1); cambio_contador=false; break;
          case 4: displayCenteredMessage("TOTAL ONLINE", 0); displayValor(Miscontadores.Cont_Total_Online, 6, 1); cambio_contador=false; break;
          case 5: displayCenteredMessage("PARCIAL CORTESIA", 0); displayValor(Miscontadores.Cont_Parcial_Cortesia, 6, 1); cambio_contador=false; break;
          case 6: displayCenteredMessage("TOTAL CORTESIA", 0); displayValor(Miscontadores.Cont_Total_Cortesia, 6, 1); cambio_contador=false; break;
          case 7: displayCenteredMessage("RESET CONTADORES?", 0); displayCenteredMessage("PRESIONE + Y -", 1); cambio_contador=false; break;
          case 8:
            displayCenteredMessage("BORRANDO", 0); displayCenteredMessage("CONTADORES", 1);
            Miscontadores.Cont_Parcial_Cortesia = 0;
            Miscontadores.Cont_Parcial_Cospeles = 0;
            Miscontadores.Cont_Parcial_OnLine   = 0;
            Miscontadores.Cont_Parcial_Dinero   = 0;
            Save_Flash_Contadores();
            t_aux = now;
            break;
        }
      }
      if (sub_estado == 8 && (now - t_aux > 2000)) menu_cerrar();
      break;

    // ================== P2: PRUEBA ==================
    case MENU_2_PRUEBA:
      if (!accion_activa) {
        // Igual que MENU_3: espera el flanco de soltar (sp_pressed) para entrar.
        // Así la misma pulsación no puede entrar al modo Y disparar el servicio.
        if (P1_pressed || P2_pressed) {
          accion_activa = true;
          mostrando_pulso = false;
          clearLCD();
          displayCenteredMessage("MODO PRUEBA", 0);
          digitalWrite(pin_habilitacion, LOW);
        //  temperatura_set_intervalo(TIEMPO_ENTRE_LECTURAS_PRUEBA);
        }
      } else {

        // 1. Detección de Monedas y Ancho de Pulso
        if (g_entroPulso || g_cospelIn) {
            clearLCD();
            if (g_cospelIn) {
                displayCenteredMessage("INGRESO FICHA", 0);
            } else {
                char bufPulso[16];
                snprintf(bufPulso, sizeof(bufPulso), "PULSO: %lu ms", (unsigned long)pulseDuration);
                displayCenteredMessage("RECHAZADO", 0);
                displayCenteredMessage(bufPulso, 1);
            }
            g_cospelIn = false;
            g_entroPulso = false;
            mostrando_pulso = true;
            t_aux = now;
        }

        // 2. Refresco de Temperatura cada 5 segundos
 /*        if (!mostrando_pulso && (now - t_aux_temp > 5000)) {
            t_aux_temp = now;
            if (!g_conectado_wifi) displayCenteredMessage("SIN CONEXION", 0);
            else if (g_conectadoMQTT) displayCenteredMessage("CONECTADO SERV", 0);
            else displayCenteredMessage("CONECTADO WIFI", 0);
            char bufTemp[16];
            snprintf(bufTemp, sizeof(bufTemp), "  TEMP: %.1f", g_temperatura);
            displayCenteredMessage(bufTemp, 1);
        }
 */
        // 3. Borrar el mensaje del monedero después de 3 segundos
        if (mostrando_pulso && (now - t_aux > 3000)) {
            mostrando_pulso = false;
            clearLCD();
            t_aux_temp = now - 5000;
        }

        
        if (digitalRead(Pin_Pulsador_1) == LOW) {
             digitalWrite(Valvula_Desinflado, LOW);
             digitalWrite(Valvula_Inflado, HIGH);
        }
        if (digitalRead(Pin_Pulsador_2) == LOW) {
            digitalWrite(Valvula_Inflado, LOW);
            digitalWrite(Valvula_Desinflado, HIGH);
        }
        // 5. Salir al siguiente menú (abortar prueba sin entregar agua)
        if (pp_pressed) {
            accion_activa = false;
            estado_menu = MENU_3_PROGRAMACION;
            clearLCD(); displayCenteredMessage("PROGRAMA 3", 0); displayCenteredMessage("PROGRAMACION", 1);
        }
      }
      break;

    // ================== P3: PROGRAMACION ==================
    case MENU_3_PROGRAMACION: {
    uint8_t Tiempo = 0;
      if (!accion_activa) {
        if (P1_pressed || P2_pressed) {
          accion_activa = true;
          sub_estado = 1;
          clearLCD();
          displayMessage("PRECIO:", 0, 0);
          displayMessage("TIEMPO:", 0, 1);
          digitalWrite(pin_habilitacion, LOW);
        }
      } else {
        if (sub_estado == 1) {
          if (g_cospelIn) {
            clearLCD();
            displayCenteredMessage("PRECIO: 1 FICHA", 0);
            Misvalores.Servicio_Gratis = false;
            g_cospelIn = false;
          }
          // Al mantener presionado el pulsador de servicio
          if (P1_pressed) {
            Tiempo++;
             if (Tiempo > 10) Tiempo = 1;
             {
               String msg = String("TIEMPO : ") + String(Tiempo);
               displayMessage(msg.c_str(), 0, 1);
             }
            t_aux = now;
          }
          if (P2_pressed) {
            Tiempo--;
             if (Tiempo == 0) Tiempo = 10;
             {
               String msg = String("TIEMPO : ") + String(Tiempo);
               displayMessage(msg.c_str(), 0, 1);
             }
            t_aux = now;
          }
         if (pp_pressed) { // Salida anticipada
             accion_activa = false;
             estado_menu = MENU_4_MODO_OP;
             clearLCD(); displayCenteredMessage("PROGRAMA 4", 0); displayCenteredMessage("CONFIGURACION", 1);
          } 
        
          if((digitalRead(Pin_Pulsador_1)==LOW)|| (digitalRead(Pin_Pulsador_2)==LOW))
          {
    
            Misvalores.Tiempo_Programado = Tiempo;
            clearLCD();
            displayCenteredMessage("GRABANDO", 0);
            displayValor(valor_temp, 7, 1); // muestra el valor que realmente se guarda
            Save_Flash_ValorServicio();
            t_aux = now;
            sub_estado = 3;
            digitalWrite(pin_habilitacion, HIGH);
          }
        
        if (sub_estado == 3) {
          if (now - t_aux > 1500) menu_cerrar();
        }
          
      }
      } // else
      break;
    } // case MENU_3_PROGRAMACION
   // ================== P4: CONFIGURACIONES ==================
   case MENU_4_MODO_OP: {
      // sub_estado: 0=version carrousel, 1=servicio gratis, 2=sensor temp, 3=wifi, 4=validador, 5=confirmar
      static const char* nombreVersion[] = {
        CARROUSEL_VERSION_0, CARROUSEL_VERSION_1,
        CARROUSEL_VERSION_2, CARROUSEL_VERSION_3
      };
      if (!accion_activa) {
        if (P1_pressed) {
          accion_activa = true;
          sub_estado = 0;
          clearLCD();
          displayCenteredMessage("VERSION", 0);
          displayCenteredMessage(nombreVersion[Misvalores.Version_Carrousel], 1);
        }
      } else {
        if (sub_estado == 0) {
          if (P1_pressed) {
            Misvalores.Version_Carrousel = (Misvalores.Version_Carrousel + 1) & 0x03;
            displayCenteredMessage(nombreVersion[Misvalores.Version_Carrousel], 1);
          }
          if (pp_pressed) {
            sub_estado = 1;
            clearLCD();
            displayCenteredMessage("SERV.GRATIS", 0);
            displayCenteredMessage(Misvalores.Servicio_Gratis ? "SI" : "NO", 1);
          }
        } else if (sub_estado == 1) {
          if ((P1_pressed) || (P2_pressed)) {
            Misvalores.Servicio_Gratis = !Misvalores.Servicio_Gratis;
            displayCenteredMessage(Misvalores.Servicio_Gratis ? "SI" : "NO", 1);
          }
          if (pp_pressed) {
            sub_estado = 2;
            clearLCD();
            displayCenteredMessage("AUDIO", 0);
            displayCenteredMessage(Misvalores.Audio_Encendido ? "ACTIVADO" : "DESACTIVADO", 1);
          }
        } else if (sub_estado == 2) {
          if ((P1_pressed) || (P2_pressed)) {
             Misvalores.Sensor_Temp_Activado = !Misvalores.Sensor_Temp_Activado;
             displayCenteredMessage(Misvalores.Sensor_Temp_Activado ? "ACTIVADO" : "DESACTIVADO", 1);
            Misvalores.Audio_Encendido = !Misvalores.Audio_Encendido;
            displayCenteredMessage(Misvalores.Audio_Encendido ? "ACTIVADO" : "DESACTIVADO", 1);
          }
          if (pp_pressed) {
            sub_estado = 3;
            clearLCD();
            displayCenteredMessage("WIFI", 0);
            displayCenteredMessage(Mientorno.CLIENTE_WIFI_Habilitado ? "ACTIVADO" : "DESACTIVADO", 1);
          }
        } else if (sub_estado == 3) {
          if ((P1_pressed) || (P2_pressed))  {
            Mientorno.CLIENTE_WIFI_Habilitado = !Mientorno.CLIENTE_WIFI_Habilitado;
            displayCenteredMessage(Mientorno.CLIENTE_WIFI_Habilitado ? "ACTIVADO" : "DESACTIVADO", 1);
          }
          if (pp_pressed) {
            sub_estado = 4;
            clearLCD();
            displayCenteredMessage("VALIDADOR", 0);
            char buf[4]; snprintf(buf, sizeof(buf), "%d", Misvalores.Validador);
            displayCenteredMessage(buf, 1);
          }
        } else if (sub_estado == 4) {
          if (P1_pressed) {
            Misvalores.Validador++;
            if (Misvalores.Validador > 3) Misvalores.Validador = 1;
            char buf[4]; snprintf(buf, sizeof(buf), "%d", Misvalores.Validador);
            displayCenteredMessage(buf, 1);
          }
          if (P2_pressed) {
            Misvalores.Validador--;
            if (Misvalores.Validador < 1) Misvalores.Validador = 3;
            char buf[4]; snprintf(buf, sizeof(buf), "%d", Misvalores.Validador);
            displayCenteredMessage(buf, 1);
          }
          if (pp_pressed) {
            sub_estado = 5;
            clearLCD();
            displayCenteredMessage("GUARDAR?", 0);
            displayCenteredMessage("PRESIONE + ", 1);
          }
        } else if (sub_estado == 5) {
          if (P1_pressed) {
            clearLCD();
            displayCenteredMessage("GUARDANDO...", 0);
            Save_Flash_ValorServicio();
            Save_Flash_Entorno();
            task_delay(1000);
            ESP.restart();
          }
          if (pp_pressed) {
            // Cancelar — recargar desde flash para descartar cambios en RAM
            Get_Flash_All();
                         estado_menu = MENU_5_AP;
             clearLCD(); displayCenteredMessage("PROGRAMA 5", 0); displayCenteredMessage("ACCESS POINT", 1);
             break;
            
          }
        }
      }
      break;
    }
     // ================== P5: ACCESS POINT ==================
    case MENU_5_AP:
      if (!accion_activa) {
        if (P1_pressed) {
          accion_activa = true;
          clearLCD();
          displayCenteredMessage("PRES. PULSADOR", 0);
          displayCenteredMessage("PARA A.POINT", 1);
        }
      } else {
        if (P1_pressed) {
          clearLCD();
          displayCenteredMessage("REINICIANDO...", 0);
          Set_Flash_Bool("Levantar_AP", true);
          task_delay(1000);
          ESP.restart(); 
        }
        if (pp_pressed) { 
          accion_activa = false;
          estado_menu = MENU_6_SIMULACION;
          clearLCD(); displayCenteredMessage("PROGRAMA 6", 0); displayCenteredMessage("SIMULACION", 1);
        }
      }
      break;

  
    // ================== P6: AUTOLLENADO ==================
    case MENU_6_SIMULACION:
      if (!accion_activa) {
        if (P1_pressed) {
          accion_activa = true;
          sub_estado = 1;
          clearLCD();
          displayCenteredMessage("PRESIONE +", 0);
          displayCenteredMessage("PARA COMENZAR", 1);
        }
      } else {
        if (pp_pressed) {   
          accion_activa = false;
          estado_menu = MENU_7_SALIDA;
          clearLCD(); displayCenteredMessage("PROGRAMA 7", 0); displayCenteredMessage("SALIDA", 1);
        }
        
        if (P1_pressed) {
          t_aux = now;
          t_inicio_proceso = now;
          clearLCD();
          displayCenteredMessage("SERVICIO DE", 0);
          displayCenteredMessage("SIMULACION", 1);
          g_inTest = true; //  activa el servicio de prueba
          menu_cerrar();
          break;
        } 
       
        }
      break;

    // ================== P7: SALIDA ==================
    case MENU_7_SALIDA:
      if (P1_pressed) menu_cerrar();
      break;

    default:
      break;
  }
}